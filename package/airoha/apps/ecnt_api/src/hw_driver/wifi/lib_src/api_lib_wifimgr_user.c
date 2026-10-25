#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/socket.h>    /* for connect and socket*/
#include <linux/wireless.h>
#include "wifi_comm.h"
#include "wifi_file_comm.h"
#include "libapi_lib_wifimgr.h"
#include "libapi_lib_wifi_internal.h"
#include "wifi_dbg.h"
#include <libapi_lib_utility.h>
#include <flash_layout/tc_partition.h>
#include "rt_os.h"
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif
#include <netinet/in.h>
#include <common/ecnt_chip_id.h>

#if !defined(TCSUPPORT_OPENWRT)
#if ((defined(MT7615_11N) || defined(MT7615E) || defined(MT7613E) || defined(TCSUPPORT_WLAN_MULTI_CHIP)) && ((TCSUPPORT_RESERVEAREA_BLOCK==6)||(TCSUPPORT_RESERVEAREA_BLOCK==7)||(TCSUPPORT_RESERVEAREA_BLOCK==9))) 
#if !(defined(TCSUPPORT_CT_PON) && (TCSUPPORT_RESERVEAREA_BLOCK==9))
#define IMG_BOOT_FLAG_SIZE 	1
#if defined(TCSUPPORT_CT)
#define IMG_BOOT_FLAG_OFFSET_COVER  	(CERM1_RA_OFFSET - IMG_BOOT_FLAG_SIZE)
#else
#ifdef TCSUPPORT_NAND_FLASH
#define IMG_BOOT_FLAG_OFFSET_COVER  	(MRD_RA_OFFSET - IMG_BOOT_FLAG_SIZE)
#else
#if (TCSUPPORT_RESERVEAREA_BLOCK==6)
#define IMG_BOOT_FLAG_OFFSET_COVER  	(DEFAULTROMFILE_RA_OFFSET)
#else
#define IMG_BOOT_FLAG_OFFSET_COVER  	(MRD_RA_OFFSET - IMG_BOOT_FLAG_SIZE)
#endif
#endif
#endif
#endif
#else
#if !(defined(TCSUPPORT_CT_PON) && (TCSUPPORT_RESERVEAREA_BLOCK==7))
#define IMG_BOOT_FLAG_SIZE 	1
#if defined(TCSUPPORT_CT)
#define IMG_BOOT_FLAG_OFFSET_COVER  	(CERM1_RA_OFFSET - IMG_BOOT_FLAG_SIZE)
#else
#ifdef TCSUPPORT_NAND_FLASH
#ifdef TCSUPPORT_NOR_FLASH_USED
#define IMG_BOOT_FLAG_OFFSET_COVER  	(SYSLOG_RA_OFFSET)
#else
#define IMG_BOOT_FLAG_OFFSET_COVER  	(MRD_RA_OFFSET - IMG_BOOT_FLAG_SIZE)
#endif
#else
#if (TCSUPPORT_RESERVEAREA_BLOCK==4)
#define IMG_BOOT_FLAG_OFFSET_COVER  	(DEFAULTROMFILE_RA_OFFSET)
#else
#define IMG_BOOT_FLAG_OFFSET_COVER  	(MRD_RA_OFFSET - IMG_BOOT_FLAG_SIZE)
#endif
#endif
#endif
#endif
#endif
#endif
#ifndef IMG_BOOT_FLAG_OFFSET_COVER
#define IMG_BOOT_FLAG_OFFSET_COVER IMG_BOOT_FLAG_OFFSET
#endif
int wifimgr_lib_get_version(int type, char *version, int len)
{
	int ret = -1;
	
	memset(version, 0, len);
	if (WIFI_2_4G == type)
	{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
		if(WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID])
		{
			snprintf(version, len, "7592");
			ret = 0;
		}
		if(WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID])
		{
			snprintf(version, len, "7615");	
			ret = 0;
		}

		if(WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID])
		{
			snprintf(version, len, "7915");	
			ret = 0;
		}
#else
#if defined(MT7592)
		snprintf(version, len, "7592");
		ret = 0;
#endif
#if defined(MT7615_11N)
		snprintf(version, len, "7615");	
		ret = 0;
#endif
#if defined(MT7915N)
		snprintf(version, len, "7915");	
		ret = 0;
#endif
#if defined(MT7916N)
		snprintf(version, len, "7916"); 
		ret = 0;
#endif
#endif
	}
	else if (WIFI_5G == type)
	{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
		if(WLAN_5G_CHIPYPTE[WLAN_5G_7613E_ID])
		{
			snprintf(version, len, "7663");
			ret = 0;
		}
		if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID])
		{
			snprintf(version, len, "7615");	
			ret = 0;
		}

		if(WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID])
		{
			snprintf(version, len, "7915");	
			ret = 0;
		}
#else
#if defined(MT7612E)
		snprintf(version, len, "7612");
		ret = 0;	
#endif
#if defined(MT7613E)
		snprintf(version, len, "7663");
		ret = 0;	
#endif
#if defined(MT7615E)
		snprintf(version, len, "7615");	
		ret = 0;
#endif
#if defined(MT7915E)
		snprintf(version, len, "7915");	
		ret = 0;
#endif
#endif
	}
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID])
	{
		snprintf(version, len, "7915D");	
		ret = 0;
	}
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
	{
		snprintf(version, len, "7916D");	
		ret = 0;
	}
#else
#if defined(MT7915D)
		snprintf(version, len, "7915D"); 
		ret = 0;
#endif
#if defined(MT7916D)
		snprintf(version, len, "7916D"); 
		ret = 0;
#endif
#endif

	return ret;
}

int wifimgr_lib_set_TxBurst_to_file(char *buf, int len, int index, char *value, int flag)
{
	int ret = -1;
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	snprintf(buf, len, WLAN_AC_SCRIPT_PREFIX, index, "TxBurst", value);
	ret = 0;
	#else
#ifndef MT7612E /*don't need to set TxBurst=0 when test WMM for 7612 3.0.4.0*/
	snprintf(buf, len, WLAN_AC_SCRIPT_PREFIX, index, "TxBurst", value);
	ret = 0;
#else
	if (1 == flag)
	{
		snprintf(buf, len, WLAN_AC_SCRIPT_PREFIX, index, "TxBurst", "1");
		ret = 0;
	}
#endif
#endif
	return ret;
}

int wifimgr_lib_set_MaxStaNum(wifi_type type, int index, char *input)
{
	if (WIFI_2_4G == type)
	{
	#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID])
	{
		return wifimgr_lib_set_mbssMaxStaNum(type, index, input);
	}
	else
	{
		return wifimgr_lib_set_maxStaNum(type, index, input);
	}
	#else
#if defined(MT7615_11N)||defined(MT7915N)||defined(MT7916N)
	return wifimgr_lib_set_mbssMaxStaNum(type, index, input);
#else
	return wifimgr_lib_set_maxStaNum(type, index, input);
#endif	
#endif
	}
	else if (WIFI_5G == type)
	{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(chipflag5g)
	{
		return wifimgr_lib_set_mbssMaxStaNum(type, index, input);
	}
	else
	{
		return wifimgr_lib_set_maxStaNum(type, index, input);
	}
#else
#if defined(MT7615E) || defined(MT7613E) || defined(MT7915E) || defined(MT7916E)
	return wifimgr_lib_set_mbssMaxStaNum(type, index, input);
#else
	return wifimgr_lib_set_maxStaNum(type, index, input);
#endif
#endif
	}
	return 0;
}

int ixia5gflag = -1;

int wifimgr_lib_get_ixia5gflag()
{
#if defined(TCSUPPORT_TEST_WLAN_SHORTCUT) && defined(MT7613E)
	if (-1 == ixia5gflag)
		return 0;
#endif
	return ixia5gflag;
}

int wifimgr_lib_set_ixia5gflag(int val)
{
#if defined(TCSUPPORT_TEST_WLAN_SHORTCUT) && defined(MT7613E)
	ixia5gflag = val;
#endif
	return 0;
}

int wifimgr_lib_check_wifi_ixia_test()
{
#if defined(TCSUPPORT_TEST_WLAN_SHORTCUT) && defined(MT7613E)
	char string[8];
	int fd=0, ret=0,reset=0, onoff_5g=0;
	
	fd = open("/proc/tc3162/wifi5g_ixia",O_RDONLY|O_NONBLOCK);
	if(fd!=-1){
		ret = read(fd, string, 8);
		close(fd);
		sscanf(string, "%d", &onoff_5g);
		if(onoff_5g==1){
			if(0 == wifimgr_lib_get_ixia5gflag())
			{
#if defined(TCSUPPORT_TEST_VWTEST)
				printf("execute wlan_5g_ixia_V2.sh\r\n");
				system("/usr/script/wlan_5g_ixia_V2.sh &");
#else
				printf("execute wlan_5g_ixia.sh\r\n");
				system("/usr/script/wlan_5g_ixia.sh &");
#endif
				ret = wifimgr_lib_set_ixia5gflag(1);
			}
		}
		else
		{
			ret = wifimgr_lib_set_ixia5gflag(0);
		}
	}
#endif

	return 0;
}
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
int wifimgr_lib_reboot(int type, char *oldNum, int newNum, int wdsNum, char *bssidNum, char *changeName)
{
	return 0;
}
#else
int wifimgr_lib_reboot(int type, char *oldNum, int newNum, int wdsNum, char *bssidNum, char *changeName)
{
	char tmp[64] = {0};
	int i = 0;

	if (WIFI_2_4G == type)
	{
		if (!strcmp(changeName, "BssidNum"))
		{
#if ((defined(TCSUPPORT_WLAN_MT7615_BUILD_IN) && defined(MT7615_11N))|| defined(MT7615D) || (defined(TCSUPPORT_WLAN_MT7915_BUILD_IN) && defined(MT7915N))|| defined(MT7915D) || (defined(TCSUPPORT_WLAN_MT7916_BUILD_IN) && defined(MT7916N))|| defined(MT7916D))
#else
			return 0;
#endif
		}

		if (!strcmp(changeName, "WHNAT"))
		{
#if (defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD) && defined(TCSUPPORT_WLAN_MT7615_BUILD_IN) && defined(MT7615_11N)) || (defined(TCSUPPORT_WLAN_MT7915_TXOFFLOAD) && defined(TCSUPPORT_WLAN_MT7915_BUILD_IN) && defined(MT7915N)) || (defined(TCSUPPORT_WLAN_MT7916_TXOFFLOAD) && defined(TCSUPPORT_WLAN_MT7916_BUILD_IN) && defined(MT7916N))
#else
			return 0;
#endif
		}

		if(*oldNum != newNum){
#ifdef TCSUPPORT_WLAN_WDS
			for(i = 0; i < wdsNum; ++i ) {				
				snprintf(tmp, sizeof(tmp),"/sbin/ifconfig wds%d down", i);
				system(tmp);
			}
#if defined(MT7615E) || defined(MT7915E) || defined(MT7916E)
			for(i = 0; i < wdsNum; ++i ) {				
				snprintf(tmp, sizeof(tmp),"/sbin/ifconfig wdsi%d down", i);
				system(tmp);
			}
#endif
#endif
			for(i = 0; i < *oldNum; i++){
				snprintf(tmp, sizeof(tmp), "/sbin/ifconfig ra%d down", i);
				system(tmp);
			}
#if defined(MT7615E) || defined(MT7915E) || defined(MT7916E)
			for(i = 0; i < atoi(bssidNum); ++i ) { 			
				snprintf(tmp, sizeof(tmp),"/sbin/ifconfig rai%d down", i);
				system(tmp);
			}
#endif
		*oldNum = newNum;
		system("reboot");
		}
	}
	else if (WIFI_5G == type)
	{

		if (!strcmp(changeName, "BssidNum"))
		{
#if defined(TCSUPPORT_WLAN_MT7615_BUILD_IN) || defined(TCSUPPORT_WLAN_MT7613_BUILD_IN) ||defined(MT7615D) || defined(TCSUPPORT_WLAN_MT7915_BUILD_IN) ||defined(MT7615D) ||defined(MT7915D) || defined(TCSUPPORT_WLAN_MT7916_BUILD_IN) || defined(MT7916D)
#else
			return 0;
#endif
		}

		if (!strcmp(changeName, "WHNAT"))
		{
#if (defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD) && defined(TCSUPPORT_WLAN_MT7615_BUILD_IN) && defined(MT7615E)) || (defined(TCSUPPORT_WLAN_MT7915_TXOFFLOAD) && defined(TCSUPPORT_WLAN_MT7915_BUILD_IN) && defined(MT7915E)) || (defined(TCSUPPORT_WLAN_MT7916_TXOFFLOAD) && defined(TCSUPPORT_WLAN_MT7916_BUILD_IN) && defined(MT7916E))
#else
			return 0;
#endif
		}

		if(*oldNum != newNum){
#ifdef TCSUPPORT_WLAN_WDS
			for(i = 0; i < wdsNum; ++i ) {	
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp),"/sbin/ifconfig wdsi%d down", i);
				system(tmp);
			}
#if defined(MT7615_11N) ||defined(MT7915N) || defined(MT7916N)
			for(i = 0; i < wdsNum; ++i ) {
				memset(tmp, 0, sizeof(tmp));				
				snprintf(tmp, sizeof(tmp),"/sbin/ifconfig wds%d down", i);
				system(tmp);
			}
#endif
#endif
			for(i = 0; i < *oldNum; i++){
				memset(tmp, 0, sizeof(tmp));
				snprintf(tmp, sizeof(tmp), "/sbin/ifconfig rai%d down", i);
				system(tmp);
			}
#if defined(MT7615_11N) ||defined(MT7915N) || defined(MT7916N)
			for(i = 0; i < atoi(bssidNum); ++i ) {	
				memset(tmp, 0, sizeof(tmp));			
				snprintf(tmp, sizeof(tmp),"/sbin/ifconfig ra%d down", i);
				system(tmp);
			}
#endif
			*oldNum = newNum;
			system("reboot");
		}
	
	}

	return 0;
}
#endif
char wlan_script_com_attr[][20]=
{
	{"CountryRegion"},
	{"Channel"},
	{"BeaconPeriod"},
	{"RTSThreshold"},
	{"FragThreshold"},
	{"DtimPeriod"},
#if defined(WLAN_MULTIDRIVER) || defined(MT7592) || defined(MT7615_11N) || defined(MT7915N) || defined(MT7916N)
	{"IgmpSnEnable"},
#endif
	{""}
};

int wifimgr_lib_get_wlan_script_attr(char output[][20])
{
	int i = 0;

	while(wlan_script_com_attr[i][0] != '\0')
	{
		snprintf(output[i], 20, "%s", wlan_script_com_attr[i]);
		i++;
	}

	return i;
}

wifi_attr_fun wlan_common_default_parm[]=
{
#if/*TCSUPPORT_COMPILE*/ !defined(TCSUPPORT_CT_WLAN_NODE)
	{"TxPower",				wifimgr_lib_set_txPower},
#endif/*TCSUPPORT_COMPILE*/
#if defined(MT7615_11N)|| defined(MT7915N) || defined(MT7916N)
	{"PERCENTAGEenable",	wifimgr_lib_set_PERCENTAGEenable},
#endif	
	{"BGProtection",		wifimgr_lib_set_bgProtection},
	{"TxPreamble",			wifimgr_lib_set_txPreamble},
	{"TxBurst",				wifimgr_lib_set_txBurst},
	{"PktAggregate",		wifimgr_lib_set_pktAggregate},
	{"TurboRate",			wifimgr_lib_set_turboRate},
	{"NoForwardingBTNBSSID",wifimgr_lib_set_noForwardingBTNBSSID},
	{"ShortSlot",			wifimgr_lib_set_shortSlot},
	{"RekeyInterval",		wifimgr_lib_set_rekeyInterval},
	{"RekeyMethod",			wifimgr_lib_set_rekeyMethod},
	{"PMKCachePeriod",		wifimgr_lib_set_pmkCachePeriod},
	{"APAifsn",				wifimgr_lib_set_apAifsn},
	{"APCwmin",				wifimgr_lib_set_apCwmin},
	{"APCwmax",				wifimgr_lib_set_apCwmax},
	{"APTxop",				wifimgr_lib_set_apTxop},
	{"APACM",				wifimgr_lib_set_apACM},
	{"BSSAifsn",			wifimgr_lib_set_bssAifsn},
	{"BSSCwmin",			wifimgr_lib_set_bssCwmin},
	{"BSSCwmax",			wifimgr_lib_set_bssCwmax},
	{"BSSTxop",				wifimgr_lib_set_bssTxop},
	{"BSSACM",				wifimgr_lib_set_bssACM},
	{"AckPolicy",			wifimgr_lib_set_ackPolicy},  
	{"DLSCapable",			wifimgr_lib_set_dlsCapable},
	{"WirelessEvent",		wifimgr_lib_set_wirelessEvent},
	{"HT_HTC",				wifimgr_lib_set_HT_HTC},
	{"HT_LinkAdapt",		wifimgr_lib_set_HT_LinkAdapt},				
	{"HT_AMSDU",			wifimgr_lib_set_HT_AMSDU},		
	{"HT_STBC",				wifimgr_lib_set_HT_STBC},	
	{"DisableOLBC",			wifimgr_lib_set_disableOLBC},
	{"HT_AutoBA",			wifimgr_lib_set_HT_AutoBA},
	{"HT_RDG",				wifimgr_lib_set_HT_RDG},
	{"HT_MpduDensity",		wifimgr_lib_set_HT_MpduDensity},
	{"HT_BAWinSize",		wifimgr_lib_set_HT_BAWinSize},
	{"HT_BSSCoexistence",	wifimgr_lib_set_HT_BSSCoexistence},
	#if !defined(MT7915N) && !defined(MT7615_11N) && !defined(MT7916N)
	{"PMFMFPC",				wifimgr_lib_set_PMFMFPC},
	{"PMFMFPR",				wifimgr_lib_set_PMFMFPR},
	{"PMFSHA256",			wifimgr_lib_set_PMFSHA256},
	{"APSDCapable",			wifimgr_lib_set_apsdCapable},
	#endif

	{"HT_TxStream",			wifimgr_lib_set_HT_TxStream},
	{"HT_RxStream",			wifimgr_lib_set_HT_RxStream},

	{"IEEE80211H",			wifimgr_lib_set_IEEE80211H},
	{"G_BAND_256QAM",		wifimgr_lib_set_G_BAND_256QAM},
	{"BandSteering",		NULL},
#if (defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD) && defined(MT7615_11N)) || (defined(TCSUPPORT_WLAN_MT7915_TXOFFLOAD) && defined(MT7915N))||(defined(TCSUPPORT_WLAN_MT7916_TXOFFLOAD) && defined(MT7916N)) 
	{"WHNAT",			wifimgr_lib_set_WHNAT}, 
	{"WOEOFF",			wifimgr_lib_set_WOEOFF},	
#endif		
#if defined TCSUPPORT_WLAN_TXBF && defined(MT7615_11N) 
#ifndef MT7615D
	{"ITxBfEn", 			wifimgr_lib_set_ITxBfEn}, 
	{"ETxBfEnCond", 		wifimgr_lib_set_ETxBfEnCond},
	{"ETxBfIncapable",		wifimgr_lib_set_ETxBfIncapable},
	{"MUTxRxEnable",		wifimgr_lib_set_MUTxRxEnable},
	{"ITxBfTimeout",		NULL},
	{"ETxBfTimeout",		NULL},
	{"ETxBfNoncompress",	NULL},
#endif
#endif
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
	{"ProbeHideSSID",			wifimgr_lib_set_ProbeHideSSID}, 
#endif
#if defined(TCSUPPORT_WLAN_AX)	
		{"SREnable",	wifimgr_lib_set_SREnable},
		{"SRMode",	wifimgr_lib_set_SRMode},
		{"SRSDEnable",	wifimgr_lib_set_SRSDEnable},
		{"PPEnable",	wifimgr_lib_set_PPEnable},
		{"TWTSupport",	wifimgr_lib_set_TWTSupport},
		{"HeraStbcPriority",	wifimgr_lib_set_HeraStbcPriority},
#if defined(TCSUPPORT_WLAN_MT7915_V73)|| defined(TCSUPPORT_WLAN_MT7915_V74) || defined(MT7916N)
		{"MaxRuOfdma",	wifimgr_lib_set_MaxRuOfdma},
		{"MaxDLMuMimo",	wifimgr_lib_set_MaxDLMuMimo},
		{"MaxULMuMimo",	wifimgr_lib_set_MaxULMuMimo},
		{"Disable160RuMu",	wifimgr_lib_set_Disable160RuMu},
		{"Vht1024QamSupport",	wifimgr_lib_set_Vht1024QamSupport},
		{"DscpPriMapEnable",	wifimgr_lib_set_DscpPriMapEnable},		
#endif
#endif
#ifdef TCSUPPORT_TEST_WLAN_SHORTCUT
	{"SHORTGIFLAG",				wifimgr_lib_set_SHORTGIFLAG},
	{"StaNumDetect",			wifimgr_lib_set_StaNumDetect},	
#endif
	{"",					NULL},
};

wifi_attr_fun wlan_ac_common_default_parm[]=
{
	{"BGProtection",		wifimgr_lib_set_bgProtection},
#if defined(MT7615E) || defined(MT7915E) || defined(MT7916E)
	{"PERCENTAGEenable",	wifimgr_lib_set_PERCENTAGEenable},
#endif	
	{"TxBurst",				wifimgr_lib_set_txBurst},
	{"PktAggregate",		wifimgr_lib_set_pktAggregate},
	{"TurboRate",			wifimgr_lib_set_turboRate},
	{"NoForwardingBTNBSSID",wifimgr_lib_set_noForwardingBTNBSSID},
	{"ShortSlot",			wifimgr_lib_set_shortSlot},
	{"RekeyInterval",		wifimgr_lib_set_rekeyInterval},
	{"RekeyMethod",			wifimgr_lib_set_rekeyMethod},
	{"PMKCachePeriod",		wifimgr_lib_set_pmkCachePeriod},
	{"APAifsn",				wifimgr_lib_set_apAifsn},
	{"APCwmin",				wifimgr_lib_set_apCwmin},
	{"APCwmax",				wifimgr_lib_set_apCwmax},
	{"APTxop",				wifimgr_lib_set_apTxop},
	{"APACM",				wifimgr_lib_set_apACM},
	{"BSSAifsn",			wifimgr_lib_set_bssAifsn},
	{"BSSCwmin",			wifimgr_lib_set_bssCwmin},
	{"BSSCwmax",			wifimgr_lib_set_bssCwmax},
	{"BSSTxop",				wifimgr_lib_set_bssTxop},
	{"BSSACM",				wifimgr_lib_set_bssACM},
	{"AckPolicy",			wifimgr_lib_set_ackPolicy},
	#if !defined(MT7915E) && !defined(MT7615E) && !defined(MT7916E)
	{"APSDCapable",			wifimgr_lib_set_apsdCapable},
	#endif
	{"DLSCapable",			wifimgr_lib_set_dlsCapable},
	{"WirelessEvent",		wifimgr_lib_set_wirelessEvent},
	{"HT_HTC",				wifimgr_lib_set_HT_HTC},
	{"HT_LinkAdapt",		wifimgr_lib_set_HT_LinkAdapt},		
	{"HT_AMSDU",			wifimgr_lib_set_HT_AMSDU},		
	{"HT_STBC",				wifimgr_lib_set_HT_STBC},	
	{"DisableOLBC",			wifimgr_lib_set_disableOLBC},
	{"HT_AutoBA",			wifimgr_lib_set_HT_AutoBA},
	{"HT_RDG",				wifimgr_lib_set_HT_RDG},
	{"HT_MpduDensity",		wifimgr_lib_set_HT_MpduDensity},
	{"HT_BAWinSize",		wifimgr_lib_set_HT_BAWinSize},
	{"HT_BSSCoexistence",	wifimgr_lib_set_HT_BSSCoexistence},


	{"HT_TxStream",			wifimgr_lib_set_HT_TxStream},
	{"HT_RxStream",			wifimgr_lib_set_HT_RxStream},

	{"IEEE80211H",			wifimgr_lib_set_IEEE80211H},
	{"BandSteering",		NULL}, 
#ifdef TCSUPPORT_WLAN_TXBF
	{"ITxBfEn",				wifimgr_lib_set_ITxBfEn}, 
	{"ETxBfEnCond",			wifimgr_lib_set_ETxBfEnCond},
	{"ETxBfIncapable",		wifimgr_lib_set_ETxBfIncapable},
	{"MUTxRxEnable",		wifimgr_lib_set_MUTxRxEnable},
	{"ITxBfTimeout",		NULL},
	{"ETxBfTimeout",		NULL},
	{"ETxBfNoncompress",	NULL},
#endif		
	{"G_BAND_256QAM",		wifimgr_lib_set_G_BAND_256QAM},
	{"VOW_Airtime_Fairness_En",NULL},	
	{"RED_Enable",			NULL}, 		
#if (defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD) && defined(MT7615E)) || (defined(TCSUPPORT_WLAN_MT7915_TXOFFLOAD) && defined(MT7915E)) || (defined(TCSUPPORT_WLAN_MT7916_TXOFFLOAD) && defined(MT7916E))
	{"WHNAT",			wifimgr_lib_set_WHNAT}, 
	{"WOEOFF",			wifimgr_lib_set_WOEOFF},		
#endif		
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
	{"ProbeHideSSID",			wifimgr_lib_set_ProbeHideSSID}, 
#endif
            {"SHORTGIFLAG",				wifimgr_lib_set_SHORTGIFLAG},
#ifdef TCSUPPORT_TEST_WLAN_SHORTCUT
	{"StaNumDetect",			wifimgr_lib_set_StaNumDetect},	
#endif
	{"WMMFLAG",				NULL},
#if defined(TCSUPPORT_WLAN_AX)	
		{"SREnable",	wifimgr_lib_set_SREnable},
		{"SRMode",	wifimgr_lib_set_SRMode},
		{"SRSDEnable",	wifimgr_lib_set_SRSDEnable},
		{"PPEnable",	wifimgr_lib_set_PPEnable},
		{"TWTSupport",	wifimgr_lib_set_TWTSupport},
		{"HeraStbcPriority",	wifimgr_lib_set_HeraStbcPriority},	
#if defined(TCSUPPORT_WLAN_MT7915_V73)|| defined(TCSUPPORT_WLAN_MT7915_V74) || defined(MT7916E)
		{"MaxRuOfdma",	wifimgr_lib_set_MaxRuOfdma},
		{"MaxDLMuMimo",	wifimgr_lib_set_MaxDLMuMimo},
		{"MaxULMuMimo",	wifimgr_lib_set_MaxULMuMimo},
		{"Disable160RuMu",	wifimgr_lib_set_Disable160RuMu},
		{"Vht1024QamSupport",	wifimgr_lib_set_Vht1024QamSupport},
		{"DscpPriMapEnable",	wifimgr_lib_set_DscpPriMapEnable},				
#endif
#endif
#if defined(TCSUPPORT_MAP_R2)
		{"DfsDedicatedZeroWait",	wifimgr_lib_set_DfsDedicatedZeroWait},
		{"DfsZeroWaitDefault",	wifimgr_lib_set_DfsZeroWaitDefault},
#endif
	{"",					NULL},
};

wifi_attr_fun wlan_global_script_com_attr[] = 
{
#if defined(MT7615D)
	{"CountryCode",				wifimgr_lib_set_countryCode},
	{"NoForwardingBTNBSSID",	wifimgr_lib_set_noForwardingBTNBSSID},
	{"ACSCheckTime",			NULL},	
	{"GreenAP",					NULL},	
	{"DBDC_MODE",				NULL},		
	{"IcapMode",				NULL},		
	{"CarrierDetect",			NULL},		
	{"DebugFlags",				NULL},	 
	//{"E2pAccessMode"},	
	{"EfuseBufferMode",			wifimgr_lib_set_EfuseBufferMode},	
	{"WCNTest",					NULL},		
	#if defined(MT7915D) || defined(MT7916D)
	#else
	{"AutoChannelSelect",		wifimgr_lib_set_autoChannelSelect},	
	#endif
	{"HT_RDG",					wifimgr_lib_set_HT_RDG}, 
	{"HT_OpMode",				wifimgr_lib_set_HT_OpMode},	
	{"HT_MpduDensity",			wifimgr_lib_set_HT_MpduDensity}, 
	{"HT_BADecline",			NULL},	
	{"HT_AutoBA",				wifimgr_lib_set_HT_AutoBA},	
	{"HT_BAWinSize",			wifimgr_lib_set_HT_BAWinSize},	
	{"HT_AMSDU",				wifimgr_lib_set_HT_AMSDU},	
	{"BeaconPeriod",			wifimgr_lib_set_beaconPeriod},
	{"DtimPeriod",				wifimgr_lib_set_dtimPeriod},
	{"TxBurst",					wifimgr_lib_set_txBurst},	
	{"PktAggregate",			wifimgr_lib_set_pktAggregate},	
	{"VOW_WATF_Enable",			NULL},	
	{"VOW_WATF_MAC_LV1",		NULL},	
	{"VOW_WATF_MAC_LV2",		NULL},	
	{"VOW_WATF_MAC_LV3",		NULL},	
	{"VOW_WATF_MAC_LV4",		NULL},		
	{"VOW_Airtime_Fairness_En",	NULL},	
	{"VOW_BW_Ctrl",				NULL},		
	{"VOW_RX_En",				NULL},		
	{"RED_Enable",				NULL}, 
#endif
	{"",						NULL}					
};

int wifimgr_lib_get_wlan_attr(int type, wifi_attr_fun *output)
{
	int i = 0;

	if (WIFI_2_4G == type)
	{
		while(wlan_common_default_parm[i].attr_name[0] != '\0')
		{
			snprintf(output[i].attr_name, ATTR_NAME_LENGTH, "%s", wlan_common_default_parm[i].attr_name);
			output[i].wifi_api = wlan_common_default_parm[i].wifi_api;
			i++;
		}
	}
	else if (WIFI_5G == type)
	{
		while(wlan_ac_common_default_parm[i].attr_name[0] != '\0')
		{
			snprintf(output[i].attr_name, ATTR_NAME_LENGTH, "%s", wlan_ac_common_default_parm[i].attr_name);
			output[i].wifi_api = wlan_ac_common_default_parm[i].wifi_api;
			i++;
		}
	}
	else if (2 == type)/*7615D*/
	{
		while(wlan_global_script_com_attr[i].attr_name[0] != '\0')
		{
			snprintf(output[i].attr_name, ATTR_NAME_LENGTH, "%s", wlan_global_script_com_attr[i].attr_name);
			output[i].wifi_api = wlan_global_script_com_attr[i].wifi_api;
			i++;
		}
	}

	return i;
}

int _11AC_CMDflag	= -1;
int _11N_CMDflag	= -1;

int wifimgr_lib_get_11AC_CMDflag()
{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7613E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID]|| WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
	{
		if (-1 == _11AC_CMDflag)
			return 0;
	}
#else
#if defined(MT7615E) || defined(MT7613E) || defined(MT7915E) || defined(MT7916E)
	if (-1 == _11AC_CMDflag)
		return 0;
#endif
#endif
	return _11AC_CMDflag;
}

int wifimgr_lib_set_11AC_CMDflag(int val)
{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7613E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID]|| WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
	{
		_11AC_CMDflag = val;
	}
#else
#if defined(MT7615E) || defined(MT7613E) || defined(MT7915E) || defined(MT7916E)
	_11AC_CMDflag = val;
#endif
#endif
	return 0;
}

int wifimgr_lib_get_11N_CMDflag()
{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID])
	{
		if (-1 == _11N_CMDflag)
			return 0;
	}
#else
#if defined(MT7615_11N) || defined(MT7915N) || defined(MT7916N)
	if (-1 == _11N_CMDflag)
		return 0;
#endif
#endif
	return _11N_CMDflag;
}

int wifimgr_lib_set_11N_CMDflag(int val)
{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(chipflag2g)
	{
		_11N_CMDflag = val;
	}
#else
#if defined(MT7615_11N) || defined(MT7915N) || defined(MT7592) || defined(MT7916N)
	_11N_CMDflag = val;
#endif
#endif
	return 0;
}

int wifimgr_lib_execute_wlan_script(int type, char *filename)
{
	int ret = 0;
	if (WIFI_2_4G == type)
	{
	#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID])
	{
		if(wifimgr_lib_get_11N_CMDflag() == 1)
		{
			system(filename);
			ret = wifimgr_lib_set_11N_CMDflag(0);
		}
	}
	else
	{
		system(filename);
	}
#else
#if defined(MT7615_11N)
		if(wifimgr_lib_get_11N_CMDflag() == 1)
		{
			system(filename);
			ret = wifimgr_lib_set_11N_CMDflag(0);
		}
#else
		system(filename);
#endif
#endif
	}
	else if (WIFI_5G == type)
	{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(chipflag5g)
	{
		if(wifimgr_lib_get_11AC_CMDflag() == 1) {
		system(filename);
		ret = wifimgr_lib_set_11AC_CMDflag(0);
		}
	}
#else	
#if defined(MT7612E) || defined(MT7613E) || defined(MT7615E) || defined(MT7915E) || defined(MT7916E)
#if defined(MT7615E) || defined(MT7613E) || defined(MT7915E) || defined(MT7916E)
	if(wifimgr_lib_get_11AC_CMDflag() == 1) {
		system(filename);
		ret = wifimgr_lib_set_11AC_CMDflag(0);
	}
#else
	system(filename);
#endif
#endif
#endif
	}
	else if (2 == type)
	{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID])
		system(filename);
#else
#if	defined(MT7592)
		system(filename);
#endif
#endif
	}

	return ret;
}
#if defined(MT7615D) || defined(MT7915D) || defined(MT7916D) || defined(MT7615E) || defined(MT7615_11N) || defined(TCSUPPORT_WLAN_MULTI_CHIP) || defined(MT7613E)
static int doWlanIfDownUp(WLan_info wlan_info, int downup)
{
	char tmp[32]		= {0};
	int up = 0;
	int i = 0;
#ifdef TCSUPPORT_WLAN_PERSSID_SWITCH
	int ismasterra = 0;
#endif	
	char tmp_svc_wlan_name[64]				= {0};
	char tmp_svc_wlan_ra_name[64]			= {0};
	char tmp_svc_wlan_wds_name[64]			= {0};
#if defined(TCSUPPORT_ECNT_MAP)
	char tmp_svc_wlan_apcli_name[64]		= {0};
#endif	

	if (wlan_info.type == WIFI_2_4G)
	{
		sprintf(tmp_svc_wlan_name, 			"WLan");	
		sprintf(tmp_svc_wlan_ra_name, 		"ra");
		sprintf(tmp_svc_wlan_wds_name, 		"wds");
#if defined(TCSUPPORT_ECNT_MAP)
		sprintf(tmp_svc_wlan_apcli_name, 	"apcli");
#endif
	}	
	else if(wlan_info.type == WIFI_5G)
	{	
		sprintf(tmp_svc_wlan_name,			"WLan11ac");
		sprintf(tmp_svc_wlan_ra_name, 		"rai");
		sprintf(tmp_svc_wlan_wds_name, 		"wdsi");
#if defined(TCSUPPORT_ECNT_MAP)
		sprintf(tmp_svc_wlan_apcli_name, 	"apclii");
#endif
	}	
	
	if (downup == 0){
		for(i = 0; i < MAX_SSID_NUM ; i++){
				snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_ra_name, i);
				system(tmp);
				snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_ra_name, i);
				system(tmp);			
			}
#ifdef TCSUPPORT_WLAN_WDS
		for(i = 0;i < MAX_WDS_ENTRY; i++){
			snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_wds_name, i);
			system(tmp);
			snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_wds_name, i);
			system(tmp);		
		}
#endif				
#if defined(TCSUPPORT_ECNT_MAP)
		snprintf(tmp, sizeof(tmp), "ifconfig %s0 down", tmp_svc_wlan_apcli_name);
		system(tmp);
#endif
	}
	else if (downup == 1) {
		for(i = 0; i < wlan_info.newNum; i++){				
			up = 1;
#if defined(TCSUPPORT_FON)
			if (i==1)
				continue;
#endif

#ifdef TCSUPPORT_WLAN_PERSSID_SWITCH
			ismasterra = 0;			
			if(((wlan_info.index >> i) & 1) == 0){
				up = 0;

				if((0 == i)
					#if defined(MT7615D)
					&&(!strcmp(tmp_svc_wlan_ra_name,"rai"))
					#else//7915 use ra0 as master
					&&(!strcmp(tmp_svc_wlan_ra_name,"ra"))
					#endif
					)
					ismasterra = 1;
			}
#endif
			if(up){
	            snprintf(tmp, sizeof(tmp), "ifconfig %s%d up", tmp_svc_wlan_ra_name, i);
				system(tmp);					
				snprintf(tmp, sizeof(tmp), "brctl addif br0 %s%d", tmp_svc_wlan_ra_name, i);
				system(tmp);
				printf("add %s%d to br0\n", tmp_svc_wlan_ra_name, i);
			}
#ifdef TCSUPPORT_WLAN_PERSSID_SWITCH
			else{
				#if ((defined (TCSUPPORT_WLAN_MT7915_BUILD_IN)&& defined(MT7915D)) || (defined (TCSUPPORT_WLAN_MT7916_BUILD_IN)&& defined(MT7916D)))
				/*wlan_load.sh do ifconfig ra0 up/down,so not do anymore in buildin mode*/
				snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", tmp_svc_wlan_ra_name, i);
				system(tmp);
				snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", tmp_svc_wlan_ra_name, i);
				system(tmp);
				#else
				if(ismasterra){					
					/*need do up and down operation when disable rai0,enable other raix*/	
					snprintf(tmp, sizeof(tmp), "ifconfig %s0 up", tmp_svc_wlan_ra_name);
					system(tmp);
					snprintf(tmp, sizeof(tmp), "ifconfig %s0 down", tmp_svc_wlan_ra_name);
					system(tmp);
				}
			#endif
			}
#endif
		}
	

#ifdef TCSUPPORT_WLAN_WDS
			if(wlan_info.wdsActive != 0){
				for(i = 0; i < MAX_WDS_ENTRY; i++){
					snprintf(tmp, sizeof(tmp), "ifconfig %s%d up", tmp_svc_wlan_wds_name, i);
					system(tmp);
					snprintf(tmp, sizeof(tmp), "brctl addif br0 %s%d", tmp_svc_wlan_wds_name, i);
					system(tmp);
					printf("addif %s%d to br0\n", tmp_svc_wlan_wds_name, i);						
				}
			}
#endif			
	}

	return 0;		
}
#endif
static int downAllInterface(WLan_info wlan_info)
{
	int i = 0;	
	char tmp[32] = {0};
	char interName[16] = {0};

	if (WIFI_2_4G == wlan_info.type)
		snprintf(interName, sizeof(interName), "ra");
	else if (WIFI_5G == wlan_info.type)
		snprintf(interName, sizeof(interName), "rai");
	
	for(i = 0; i < wlan_info.newNum; i++){
		snprintf(tmp, sizeof(tmp), "ifconfig %s%d down", interName, i);
		system(tmp);
		snprintf(tmp, sizeof(tmp), "brctl delif br0 %s%d", interName, i);
		system(tmp);
	}
#ifdef TCSUPPORT_WLAN_WDS
	if (WIFI_5G == wlan_info.type)
	{
		for(i = 0; i < MAX_WDS_ENTRY; i++){
				snprintf(tmp, sizeof(tmp), "/sbin/ifconfig wdsi%d down", i);
				system(tmp);
				snprintf(tmp, sizeof(tmp), "brctl delif br0 wdsi%d", i);
				system(tmp);
		}
	}
#endif


	return 0;
}
/**
type:1		bssidnum is change
type:2		wifi down
type:3		wifi up
type:4		init
*/
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
int wifimgr_lib_restart_wlan_Interface(WLan_info wlan_info, WLan_info wlan_info_other, char *flag, int type, int nodeid, int resetFlag)
{
	int ret = 0;
	#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY) 
	FILE *fp = NULL;	
	#endif
	switch(type)
	{
		case 1:
#ifdef MT7615D	
			doWlanIfDownUp(wlan_info, 0);/*close all interface*/
			doWlanIfDownUp(wlan_info_other, 0);/*close all interface*/	
			system("rmmod mt7615_ap");
			system("insmod /lib/modules/mt7615_ap.ko"); 
			if (WIFI_2_4G == wlan_info.type)
			{
				doWlanIfDownUp(wlan_info_other, 1);/*Enable all interface*/	
				if(!strcmp(flag, "0"))		 /*IF 5G is disabled,then close all interface*/		
					doWlanIfDownUp(wlan_info_other, 0);
			}
#elif defined(MT7915D) || defined(MT7916D)
		/*1.close all interface include 5g and 2.4g and do module insmod*/
			doWlanIfDownUp(wlan_info, 0);  /*close all 2.4G interface*/	
			doWlanIfDownUp(wlan_info_other, 0);  /*close all 5G interface*/					
			if (WIFI_2_4G == wlan_info.type)
			{
				doWlanIfDownUp(wlan_info_other, 1);/*Enable all interface*/	
				if(!strcmp(flag, "0"))		 /*IF 5G is disabled,then close all interface*/		
					doWlanIfDownUp(wlan_info_other, 0);
			}
#else		
			wifimgr_lib_restartInterface(wlan_info, nodeid, resetFlag);
#if defined(MT7615E) && defined(MT7615_11N)
			if (WIFI_5G == wlan_info.type)
			{
				if( 0 == strcmp(flag, "1") )
					doWlanIfDownUp(wlan_info, 1);
				else if(0 == strcmp(flag, "0"))	
					doWlanIfDownUp(wlan_info, 0);
			}	
#endif
#endif
			break;
		case 2:			
#if defined(MT7615D) || defined(MT7915D) || defined(MT7916D)
			doWlanIfDownUp(wlan_info, 0);  /*close all interface*/
			if(!strcmp(flag, "1"))
				doWlanIfDownUp(wlan_info_other, 1);
#else
			downAllInterface(wlan_info); /*close all interface*/
#endif
			break;
		case 3:
			if (WIFI_2_4G == wlan_info.type)
			{
#if defined(MT7615D)
				doWlanIfDownUp(wlan_info, 0);
				doWlanIfDownUp(wlan_info_other, 0);
				doWlanIfDownUp(wlan_info, 1);
				if(!strcmp(flag, "1"))
					doWlanIfDownUp(wlan_info_other, 1);
#elif defined(MT7915D)||defined(MT7916D)
				if (1 == resetFlag)
				{
					doWlanIfDownUp(wlan_info, 0);
					doWlanIfDownUp(wlan_info_other, 0);
					doWlanIfDownUp(wlan_info, 1);
					if(!strcmp(flag, "1"))
						doWlanIfDownUp(wlan_info_other, 1);
				}
				else 
					doWlanIfDownUp(wlan_info, 1);

#else
				if(!strcmp(flag, "1")){
					/*when select AutoChannel, we must reload the RT2860AP.dat*/
#if !defined(MT7615_11N) && !defined(MT7915N) && !defined(MT7916N)
					wifimgr_lib_restartInterface(wlan_info, nodeid, resetFlag);
#endif
				}else{
					/*start all interface */
					ret = 1;
				}
#endif
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY) 
				fp = fopen(SVC_WLAN_ANT_PATH, "r");
				if(fp != NULL){
					fclose(fp);
					system(SVC_WLAN_ANT_PATH);
				}
#endif
			}
			else if (WIFI_5G == wlan_info.type)
			{
#if defined(MT7615D)
				doWlanIfDownUp(wlan_info, 0);
				doWlanIfDownUp(wlan_info_other, 0);
				doWlanIfDownUp(wlan_info, 1);
				if(!strcmp(flag, "1"))
					doWlanIfDownUp(wlan_info_other, 1);
#elif defined(MT7915D)||defined(MT7916D)
				if (1 == resetFlag)
				{
					doWlanIfDownUp(wlan_info, 0);
					doWlanIfDownUp(wlan_info_other, 0);
					doWlanIfDownUp(wlan_info, 1);
					if(!strcmp(flag, "1"))
						doWlanIfDownUp(wlan_info_other, 1);
				}
				else 
					doWlanIfDownUp(wlan_info, 1);

#else
				if (1 == resetFlag)
					downAllInterface(wlan_info); /*close all interface*/
				ret = 1;
#endif
				
			}
			break;
		case 4:
#if defined(MT7615D) || defined(MT7915D) || defined(MT7916D)
			doWlanIfDownUp(wlan_info, 1);
#else
			if(!strcmp(flag, "1")){
				downAllInterface(wlan_info);
			}
#endif
			break;
	}

	return ret;
}
#else
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
int wifimgr_lib_restart_wlan_Interface(WLan_info wlan_info, WLan_info wlan_info_other, char *flag, int type)
{
	int ret = 0;
	
	switch(type)
	{
		case 1:
		if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
		{
			doWlanIfDownUp(wlan_info, 0);  /*close all 2.4G interface*/	
			doWlanIfDownUp(wlan_info_other, 0);  /*close all 5G interface*/	
			if (WIFI_2_4G == wlan_info.type)
			{
				doWlanIfDownUp(wlan_info_other, 1);/*Enable all interface*/	
				if(!strcmp(flag, "0"))		 /*IF 5G is disabled,then close all interface*/		
					doWlanIfDownUp(wlan_info_other, 0);
			}
		}
		else
		{
			wifimgr_lib_restartInterface(wlan_info);
			if(WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID] && WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID])
			{
				if (WIFI_5G == wlan_info.type)
				{
					if( 0 == strcmp(flag, "1") )
						doWlanIfDownUp(wlan_info, 1);
					else if(0 == strcmp(flag, "0"))	
						doWlanIfDownUp(wlan_info, 0);
				}
			}
		}

		break;
		case 2:	
		if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
		{
			doWlanIfDownUp(wlan_info, 0);  /*close all interface*/
			if(!strcmp(flag, "1"))
				doWlanIfDownUp(wlan_info_other, 1);
		}
		else
		{
			downAllInterface(wlan_info); /*close all interface*/
		}
			
		break;
		case 3:
			if (WIFI_2_4G == wlan_info.type)
			{
				if(WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID])
				{
					if (1 == wifimgr_lib_get_11N_CMDflag())
						doWlanIfDownUp(wlan_info, 1);	/*Enable all interface*/
					else
					{
						doWlanIfDownUp(wlan_info, 0);/*close all interface*/
						doWlanIfDownUp(wlan_info_other, 0);/*close all interface*/	
						
						doWlanIfDownUp(wlan_info_other, 1);/*Enable all interface*/	
						if(!strcmp(flag, "0"))		 /*IF 5G is disabled,then close all interface*/		
							doWlanIfDownUp(wlan_info_other, 0);

						doWlanIfDownUp(wlan_info, 1);/*Enable all interface*/
					}
				}
				else
				{
					if(!strcmp(flag, "1")){
					/*when select AutoChannel, we must reload the RT2860AP.dat*/
						if(!WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID])
							wifimgr_lib_restartInterface(wlan_info);
					}else{
						/*start all interface */
						ret = 1;
					}
				}
			}		
			else if (WIFI_5G == wlan_info.type)
			{
				if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7613E_ID])
				{
					if (1 == wifimgr_lib_get_11AC_CMDflag())
						doWlanIfDownUp(wlan_info, 1);	/*Enable all interface*/
					else
					{
						doWlanIfDownUp(wlan_info, 0);/*close all interface*/
				
					if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
					{
						doWlanIfDownUp(wlan_info_other, 0);/*close all interface*/	
						doWlanIfDownUp(wlan_info_other, 1);/*Enable all interface*/	
					}

					if(!strcmp(flag, "0"))		 /*IF 5G is disabled,then close all interface*/		
						doWlanIfDownUp(wlan_info_other, 0);

					doWlanIfDownUp(wlan_info, 1);/*Enable all interface*/
					}	
				}
				else
				{
					ret = 1;
					downAllInterface(wlan_info); /*close all interface*/
				}
			}

			break;
		case 4:
			if(!strcmp(flag, "1")){
				downAllInterface(wlan_info);
			}
			break;
	}

	return ret;
}
#else
int wifimgr_lib_restart_wlan_Interface(WLan_info wlan_info, WLan_info wlan_info_other, char *flag, int type)
{
	int ret = 0;
	
	switch(type)
	{
		case 1:
#ifdef MT7615D	
			doWlanIfDownUp(wlan_info, 0);/*close all interface*/
			doWlanIfDownUp(wlan_info_other, 0);/*close all interface*/	
			system("rmmod mt7615_ap");
			system("insmod /lib/modules/mt7615_ap.ko"); 
			if (WIFI_2_4G == wlan_info.type)
			{
				doWlanIfDownUp(wlan_info_other, 1);/*Enable all interface*/	
				if(!strcmp(flag, "0"))		 /*IF 5G is disabled,then close all interface*/		
					doWlanIfDownUp(wlan_info_other, 0);
			}
#elif defined(MT7915D) || defined(MT7916D)
		/*1.close all interface include 5g and 2.4g and do module insmod*/
			doWlanIfDownUp(wlan_info, 0);  /*close all 2.4G interface*/	
			doWlanIfDownUp(wlan_info_other, 0);  /*close all 5G interface*/	
			if (WIFI_2_4G == wlan_info.type)
			{
				doWlanIfDownUp(wlan_info_other, 1);/*Enable all interface*/	
				if(!strcmp(flag, "0"))		 /*IF 5G is disabled,then close all interface*/		
					doWlanIfDownUp(wlan_info_other, 0);
			}
#else		
			wifimgr_lib_restartInterface(wlan_info);
#if defined(MT7615E) && defined(MT7615_11N)
			if (WIFI_5G == wlan_info.type)
			{
				if( 0 == strcmp(flag, "1") )
					doWlanIfDownUp(wlan_info, 1);
				else if(0 == strcmp(flag, "0"))	
					doWlanIfDownUp(wlan_info, 0);
			}	
#endif
#endif
			break;
		case 2:			
#if defined(MT7615D) || defined(MT7915D) || defined(MT7916D)
			doWlanIfDownUp(wlan_info, 0);  /*close all interface*/
			if(!strcmp(flag, "1"))
				doWlanIfDownUp(wlan_info_other, 1);
#else
			downAllInterface(wlan_info); /*close all interface*/
#endif
			break;
		case 3:
			if (WIFI_2_4G == wlan_info.type)
			{
#if defined(MT7615D) || defined(MT7915D) || defined(MT7916D)
				if (1 == wifimgr_lib_get_11N_CMDflag())
					doWlanIfDownUp(wlan_info, 1);	/*Enable all interface*/
				else
				{
					doWlanIfDownUp(wlan_info, 0);/*close all interface*/
					doWlanIfDownUp(wlan_info_other, 0);/*close all interface*/	
#if defined(MT7615D)
					system("rmmod mt7615_ap");
					system("insmod /lib/modules/mt7615_ap.ko"); 
#endif
					doWlanIfDownUp(wlan_info_other, 1);/*Enable all interface*/	
					if(!strcmp(flag, "0"))		 /*IF 5G is disabled,then close all interface*/		
						doWlanIfDownUp(wlan_info_other, 0);

					doWlanIfDownUp(wlan_info, 1);/*Enable all interface*/
				}
					
#else
				if(!strcmp(flag, "1")){
					/*when select AutoChannel, we must reload the RT2860AP.dat*/
#if !defined(MT7615_11N) && !defined(MT7915N) && !defined(MT7916N)
					wifimgr_lib_restartInterface(wlan_info);
#endif
				}else{
					/*start all interface */
					ret = 1;
				}
#endif
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY) 
				fp = fopen(SVC_WLAN_ANT_PATH, "r");
				if(fp != NULL){
					fclose(fp);
					system(SVC_WLAN_ANT_PATH);
				}
#endif
			}
			else if (WIFI_5G == wlan_info.type)
			{
#if defined(MT7615D) || defined(MT7915D) || defined(MT7613E) || defined(MT7916D)
				if (1 == wifimgr_lib_get_11AC_CMDflag())
					doWlanIfDownUp(wlan_info, 1);	/*Enable all interface*/
				else
				{
					doWlanIfDownUp(wlan_info, 0);/*close all interface*/
#if defined(MT7615D) || defined(MT7915D)
					doWlanIfDownUp(wlan_info_other, 0);/*close all interface*/	
#if defined(MT7615D)
					system("rmmod mt7615_ap");
					system("insmod /lib/modules/mt7615_ap.ko"); 
#endif
					doWlanIfDownUp(wlan_info_other, 1);/*Enable all interface*/	
#endif
					if(!strcmp(flag, "0"))		 /*IF 5G is disabled,then close all interface*/		
						doWlanIfDownUp(wlan_info_other, 0);

					doWlanIfDownUp(wlan_info, 1);/*Enable all interface*/
				}
#else
				ret = 1;
				downAllInterface(wlan_info); /*close all interface*/
#endif
				
			}
			break;
		case 4:
#if defined(MT7615D) || defined(MT7915D) || defined(MT7916D)
			doWlanIfDownUp(wlan_info, 1);
#else
			if(!strcmp(flag, "1")){
				downAllInterface(wlan_info);
			}
#endif
			break;
	}

	return ret;
}
#endif
#endif
int wifimgr_lib_use_global_mac(unsigned char *mac_addr)
{

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_2G_CHIPYPTE[WLAN_2G_7592_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7916D_ID])
	{
#if defined(TCSUPPORT_NP_CMCC)
		mac_addr[5]=mac_addr[5]+0x02;
#else
		mac_addr[5]=mac_addr[5]+0x01;
#endif
	}
	else
	{

		mac_addr[0]=mac_addr[0]+0x02;
		mac_addr[3]=((mac_addr[3] & 0xF0) |(0x3));
	}
#else
#if defined(MT7592) || defined(MT7615_11N) || defined(MT7915N) || defined(MT7916N)  
	/*according to WCN suggestion to use global mac addr*/
#if defined(TCSUPPORT_NP_CMCC)
	mac_addr[5]=mac_addr[5]+0x02;
#else
	mac_addr[5]=mac_addr[5]+0x01;
#endif
#else
	mac_addr[0]=mac_addr[0]+0x02;
	mac_addr[3]=((mac_addr[3] & 0xF0) |(0x3));
#endif
#endif
	return 0;
}

int wifimgr_lib_set_WifiTxQueue(int BssidNum,int len)
{
#if defined(MT7592) || defined(TCSUPPORT_WLAN_AC)
#ifdef TCSUPPORT_MTD_ENCHANCEMENT
	int i 				= 0;
	char tempbuf[128]	= {0};

	/*Set WiFi TX Queue len 500, origin is 1000. To avoid skbuff exhaust when WiFi IGMP test in dirty environment*/
	for(i = 0; i < BssidNum; i++){
		snprintf(tempbuf, sizeof(tempbuf), "ifconfig ra%d txqueuelen %d", i, len);
		system(tempbuf);
	}
#endif
#endif
	return 0;
}

int wifimgr_lib_get_button_state(int type, char *file, char *output, int len)
{
	int fd, ret = -2;
	
	if (WIFI_2_4G == type)
	{
		fd = open(file, O_RDONLY|O_NONBLOCK);
		if(fd != -1)
		{
			ret = read(fd, output, len);
			output[len - 1] = '\0';
			close(fd);
			if (ret)
			{
#if defined(TCSUPPORT_WPS_BTN_DUALBAND) && !(defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528))
				ret = 1;
#else
				ret = 2;
#endif
			}
		}
	}
	else if (WIFI_5G == type)
	{
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7580)
		fd=open(file, O_RDONLY|O_NONBLOCK);
		if(fd != -1)
		{
			ret = read(fd, output, len);
			output[len - 1] = '\0';
			close(fd);
		}
		else
			ret = -1;
#endif
	}

	return ret;
}
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
int wifimgr_lib_do_restartInterface(WLan_info wlan_info, int nodeid, int resetFlag)
{
#ifndef MT7615_11N
	wifimgr_lib_restartInterface(wlan_info, nodeid, resetFlag);
#endif

	return 0;
}
#else
int wifimgr_lib_do_restartInterface(WLan_info wlan_info)
{
#ifndef MT7615_11N
	wifimgr_lib_restartInterface(wlan_info);
#endif

	return 0;
}

#endif
int wifimgr_lib_set_iswps5gstart_val(int *output, int val)
{
#if defined(TCSUPPORT_WPS_5G_BTN) || defined(TCSUPPORT_WPS_BTN_DUALBAND)
	*output = val;
#endif

	return 0;
}

int wifimgr_lib_set_AP2040Rescan(int wlan_id, char *wlan_bw, char *wlan_bssconexist, FILE *fp)
{


#if	defined(MT7592)
	char buf[200]			= {0};
	snprintf(buf, sizeof(buf), WLAN_SCRIPT_PREFIX, wlan_id, "AP2040Rescan", "1");
	fputs(buf,fp);
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && defined(TCSUPPORT_CT_WLAN_JOYME3)
	*wlan_bw = 1;
	*wlan_bssconexist = 1;
#endif
#endif

	return 0;
}

int wifimgr_lib_is_WHNAT_support(int type, char *val, int len)
{
	if (WIFI_2_4G == type)
	{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#if defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD) || defined(TCSUPPORT_WLAN_MT7915_TXOFFLOAD) 
		if(WLAN_2G_CHIPYPTE[WLAN_2G_7915D_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7915N_ID] || WLAN_2G_CHIPYPTE[WLAN_2G_7615N_ID])			
			snprintf(val, len, "Yes");
		else
			snprintf(val, len, "N/A");
#else
		snprintf(val, len, "N/A");
#endif
#else	
#if defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD) && defined(MT7615_11N) 
		snprintf(val, len, "Yes");
#else
		snprintf(val, len, "N/A");
#endif
#endif
	}
	else if (WIFI_5G == type)
	{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#if defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD) || defined(TCSUPPORT_WLAN_MT7915_TXOFFLOAD) 
		if(WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID])			
			snprintf(val, len, "Yes");
		else
			snprintf(val, len, "N/A");
#else
		snprintf(val, len, "N/A");
#endif
#else	
#if defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD) && defined(MT7615E) 
		snprintf(val, len, "Yes");
#else
		snprintf(val, len, "N/A");
#endif
#endif
	}

	return 0;
}

int wifimgr_lib_is_Tx_Rx_Stream_support(char *val, int len)
{
#if defined(TCSUPPORT_WLAN_MT7615D)
	snprintf(val, len, "Yes");
#else
	snprintf(val, len, "N/A");
#endif
	return 0;

}

int wifimgr_lib_get_max_sta_num_ui(char *val, int len)
{
#ifdef TCSUPPORT_WLAN
#if defined(TCSUPPORT_WLAN_MAXSTANUM_GUI)
#if (defined(RT3390) && defined(VENDOR_FEATURE3_SUPPORT)) || (defined(RT5392) && defined(BB_SOC))
	snprintf(val, len, "7");
#else
	snprintf(val, len, "35");
#endif
#endif
#endif
	return 0;
}

int wifimgr_lib_get_WLAN_BUTTON_PATH(char *val, int len)
{
#if defined(TCSUPPORT_WPS_BTN_DUALBAND) && !(defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528))
	snprintf(val, len, WLAN_WPS_BUTTON_PATH);
#else
	snprintf(val, len, WLAN_BUTTON_PATH);
#endif
	return 0;
}

int wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(int type, char *val, int len)
{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(chipflag5g || (chipflag2g & (~(BIT(WLAN_2G_7592_ID)))))
	{
		snprintf(val, len, "MbssMaxStaNum");
	}
	else
	{
		snprintf(val, len, "MaxStaNum");
	}
#else
	if (WIFI_2_4G == type)
	{
#if defined(MT7615_11N)||defined(MT7915N)||defined(MT7916N)
		snprintf(val, len, "MbssMaxStaNum");
#else
		snprintf(val, len, "MaxStaNum");
#endif
	}
	else if (WIFI_5G == type)
	{
#if  defined(MT7615E) || defined(MT7613E) || defined(MT7915E) || defined(MT7916E)
		snprintf(val, len, "MbssMaxStaNum");
#else
		snprintf(val, len, "MaxStaNum");
#endif
	}
#endif
	return 0;
}

int wifimgr_lib_get_CHANNEL_SELECT_DEF(char *val, int len)
{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(chipflag5g)
	{
		snprintf(val, len, "3");
	}
	else
	{
		snprintf(val, len, "2");
	}
#else
#if  defined(MT7615E) || defined(MT7613E) || defined(MT7915E) || defined(MT7916E)
		snprintf(val, len, "3");
#else
		snprintf(val, len, "2");
#endif
#endif
	return 0;
}

int wifimgr_lib_get_MESH_TRIGGER_WAIT(int *time)
{
	/*msecs*/
	*time = (15 * 1000);
	return 0;
}

int wifimgr_lib_get_IEEE8021X_VAL(char *val, int len)
{
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	if(WLAN_5G_CHIPYPTE[WLAN_5G_7615E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915E_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7915D_ID] || WLAN_5G_CHIPYPTE[WLAN_5G_7916D_ID])
	{
		snprintf(val, len, "0");
	}
	else
	{
		snprintf(val, len, "1");
	}
#else
#if defined(MT7615E) || defined(MT7915E) || defined(MT7916E)
	snprintf(val, len, "0");
#else
	snprintf(val, len, "1");
#endif	
#endif
	return 0;
}

wifi_attr_fun wlan_entry_default_parm[] =
{
	{"NoForwarding",         wifimgr_lib_set_noForwarding},
	{"PreAuth",              wifimgr_lib_set_preAuth},
	{"PMFMFPC",              wifimgr_lib_set_PMFMFPC},
	{"PMFMFPR",              wifimgr_lib_set_PMFMFPR},
	{"PMFSHA256",            wifimgr_lib_set_PMFSHA256},
	#if defined(MT7915N) || defined(MT7615_11N) || defined(MT7916N)
	{"APSDCapable",			wifimgr_lib_set_apsdCapable},
	#endif
	{"",                     NULL}
};

wifi_attr_fun wlan_ac_entry_default_parm[] =
{
	{"NoForwarding",         wifimgr_lib_set_noForwarding},
	{"PreAuth",              wifimgr_lib_set_preAuth},
	{"PMFMFPC",              wifimgr_lib_set_PMFMFPC},
	{"PMFMFPR",              wifimgr_lib_set_PMFMFPR},
	{"PMFSHA256",            wifimgr_lib_set_PMFSHA256},
	#if defined(MT7915E) || defined(MT7615E) || defined(MT7613E) || defined(MT7916E)
	{"APSDCapable",			wifimgr_lib_set_apsdCapable},
	#endif
	{"",                     NULL}
};

int wifimgr_lib_get_wlan_entry_attr(int type, wifi_attr_fun *output)
{
	int i = 0;

	if (WIFI_2_4G == type)
	{
		while(wlan_entry_default_parm[i].attr_name[0] != '\0')
		{
			snprintf(output[i].attr_name, ATTR_NAME_LENGTH, "%s", wlan_entry_default_parm[i].attr_name);
			output[i].wifi_api = wlan_entry_default_parm[i].wifi_api;
			i++;
		}
	}
	else if (WIFI_5G == type)
	{
		while(wlan_ac_entry_default_parm[i].attr_name[0] != '\0')
		{
			snprintf(output[i].attr_name, ATTR_NAME_LENGTH, "%s", wlan_ac_entry_default_parm[i].attr_name);
			output[i].wifi_api = wlan_ac_entry_default_parm[i].wifi_api;
			i++;
		}
	}

	return i;
}

wifi_attr_fun wlan_script_com_attr_config[] = 
{
	{"CountryRegion",		wifimgr_lib_set_countryRegion},
	{"AutoChannelSelect",	wifimgr_lib_set_autoChannelSelect},
	{"Channel",				wifimgr_lib_set_channel},
	{"BssidNum",			wifimgr_lib_set_bssidNum},
	{"BeaconPeriod",		wifimgr_lib_set_beaconPeriod},
	{"RTSThreshold",		wifimgr_lib_set_rtsThreshold},
	{"FragThreshold",		wifimgr_lib_set_fragThreshold},
	{"DtimPeriod",			wifimgr_lib_set_dtimPeriod},
	{"BasicRate",			wifimgr_lib_set_basicRate},
	{"HT_GI",				wifimgr_lib_set_HT_GI},
	{"HT_BW",				wifimgr_lib_set_HT_BW},
	{"HT_EXTCHA",			wifimgr_lib_set_HT_EXTCHA},
	{"HT_OpMode",			wifimgr_lib_set_HT_OpMode},
#ifdef TCSUPPORT_MTD_ENCHANCEMENT
	{"EfuseBufferMode",		wifimgr_lib_set_EfuseBufferMode},
#endif	
#if defined(IGMP_SNOOP_SUPPORT) && defined(MT7592)
	{"IgmpSnEnable",		wifimgr_lib_set_igmpSnEnable},
#endif
	{"AutoChannelSkipList",	wifimgr_lib_set_AutoChannelSkipList},
#if defined(MT7915N)||defined(MT7916N)	
	{"HT_LDPC",			wifimgr_lib_set_HT_LDPC},
	{"AMSDU_NUM",			wifimgr_lib_set_AMSDU_NUM},
#endif
#if defined(TCSUPPORT_WLAN_AX)	
	{"MuOfdmaDlEnable",	wifimgr_lib_set_MuOfdmaDlEnable},
	{"MuOfdmaUlEnable",	wifimgr_lib_set_MuOfdmaUlEnable},
	{"MuMimoDlEnable",	wifimgr_lib_set_MuMimoDlEnable},
	{"MuMimoUlEnable",	wifimgr_lib_set_MuMimoUlEnable},
#endif
	{"EAPifname",		wifimgr_lib_set_EAPifname},
	{"PreAuthifname",	wifimgr_lib_set_PreAuthifname},
#if defined(TCSUPPORT_WLAN_8021X)
	{"session_timeout_interval",wifimgr_lib_set_session_timeout_interval},
#endif	
	{"",NULL}					
};

wifi_attr_fun wlan_ac_script_com_attr_config[] = 
{
	{"CountryRegion",		wifimgr_lib_set_countryRegion},
	{"CountryRegionABand",	wifimgr_lib_set_countryRegionABand},
	{"AutoChannelSelect",	wifimgr_lib_set_autoChannelSelect},
	{"Channel",				wifimgr_lib_set_channel},
	{"BssidNum",			wifimgr_lib_set_bssidNum},
	{"BeaconPeriod",		wifimgr_lib_set_beaconPeriod},
	{"RTSThreshold",		wifimgr_lib_set_rtsThreshold},
	{"FragThreshold",		wifimgr_lib_set_fragThreshold},
	{"DtimPeriod",			wifimgr_lib_set_dtimPeriod},
	{"TxPower",				wifimgr_lib_set_txPower},
	{"TxPreamble",			wifimgr_lib_set_txPreamble},
	{"BasicRate",			wifimgr_lib_set_basicRate},
	{"HT_GI",				wifimgr_lib_set_HT_GI},
	{"HT_BW",				wifimgr_lib_set_HT_BW},
	{"HT_EXTCHA",			wifimgr_lib_set_HT_EXTCHA},
	{"HT_OpMode",			wifimgr_lib_set_HT_OpMode},
#ifdef TCSUPPORT_MTD_ENCHANCEMENT
	{"EfuseBufferMode",		wifimgr_lib_set_EfuseBufferMode},
#endif	
#if defined(TCSUPPORT_WLAN_8021X)
	{"session_timeout_interval",wifimgr_lib_set_session_timeout_interval},
	{"IdleTimeout",			NULL},
#endif
	{"VHT_BW",				wifimgr_lib_set_VHT_BW}, 
	{"VHT_SGI",				wifimgr_lib_set_VHT_SGI},
	{"VHT_LDPC",			wifimgr_lib_set_VHT_LDPC}, 
	{"VHT_STBC",			wifimgr_lib_set_VHT_STBC},
	{"AutoChannelSkipList",	wifimgr_lib_set_AutoChannelSkipList},
#if defined(MT7915E) || defined(MT7916E)
	{"HT_LDPC",			wifimgr_lib_set_HT_LDPC},
	{"AMSDU_NUM",			wifimgr_lib_set_AMSDU_NUM},
#endif
#if defined(TCSUPPORT_WLAN_AX)	
	{"MuOfdmaDlEnable",	wifimgr_lib_set_MuOfdmaDlEnable},
	{"MuOfdmaUlEnable",	wifimgr_lib_set_MuOfdmaUlEnable},
	{"MuMimoDlEnable",	wifimgr_lib_set_MuMimoDlEnable},
	{"MuMimoUlEnable",	wifimgr_lib_set_MuMimoUlEnable},
#endif	
	{"EAPifname",		wifimgr_lib_set_EAPifname},
	{"PreAuthifname",	wifimgr_lib_set_PreAuthifname},
	{"",NULL}					
};

int wifimgr_lib_get_wlan_script_com_attr_config(int type, wifi_attr_fun *output)
{
	int i = 0;

	if (WIFI_2_4G == type)
	{
		while(wlan_script_com_attr_config[i].attr_name[0] != '\0')
		{
			snprintf(output[i].attr_name, ATTR_NAME_LENGTH, "%s", wlan_script_com_attr_config[i].attr_name);
			output[i].wifi_api = wlan_script_com_attr_config[i].wifi_api;
			i++;
		}
	}
	else if (WIFI_5G == type)
	{
		while(wlan_ac_script_com_attr_config[i].attr_name[0] != '\0')
		{
			snprintf(output[i].attr_name, ATTR_NAME_LENGTH, "%s", wlan_ac_script_com_attr_config[i].attr_name);
			output[i].wifi_api = wlan_ac_script_com_attr_config[i].wifi_api;
			i++;
		}
	}

	return i;
}

int wifimgr_lib_get_wlan_igmp_snooping_support()
{
	#ifdef MT7592
		return 0;
	#else
		return 1;
	#endif
}

static int doValPutFile(char *path, char *value)
{
	FILE *fp = NULL;

	fp = fopen(path,"w");
	if(fp != NULL){
		fprintf(fp, "%s", value);
		fclose(fp);
	}else{
		return -1;
	}
	return 0;
}

int wifimgr_lib_get_WPS_BUTTON_PATH(char *val, int len)
{
	snprintf(val, len, WPS_BUTTON_PATH);
	return 0;
}

int wifimgr_lib_get_WPS_BUTTON(char *string, int len, int *ret)
{
	int fd = 0;

	fd = open(WPS_BUTTON_PATH, O_RDONLY|O_NONBLOCK);
	if(fd != -1)
	{
		*ret = read(fd, string, len);
		string[len - 1] = '\0';
		close(fd);
	}
	else
		return -1;

	return 0;
}

int wifimgr_lib_set_WPS_BUTTON(char *cmd)
{
	doValPutFile(WPS_BUTTON_PATH, cmd);

	return 0;
}

int wifimgr_lib_set_WPS_5G_BUTTON(char *cmd)
{
	doValPutFile(WPS_5G_BUTTON_PATH, cmd);

	return 0;
}

int wifimgr_lib_get_WPS_5G_BUTTON_PATH(char *val, int len)
{
	snprintf(val, len, WPS_5G_BUTTON_PATH);

	return 0;
}

int wifimgr_lib_get_AHB_STATUS(char *string, int len, int *ret)
{
	int fd = 0;

	fd = open(AHB_STATUS_PATH, O_RDONLY|O_NONBLOCK);
	if(fd != -1)
	{
		*ret = read(fd, string, len);
		string[len -1] = '\0';
		close(fd);
	}
	else
		return -1;

	return 0;
}

int wifimgr_lib_set_AHB_STATUS(char *cmd)
{
	doValPutFile(AHB_STATUS_PATH, cmd);

	return 0;
}

int wifimgr_lib_set_WLAN_WPS_BUTTON(char *cmd)
{
	doValPutFile(WLAN_WPS_BUTTON_PATH, cmd);

	return 0;
}

int wifimgr_lib_set_LED_WLAN(char *cmd)
{
	char cmdVal[64] = {0};

	snprintf(cmdVal, sizeof(cmdVal), "echo %s > /proc/tc3162/led_wlan", cmd);
	system(cmdVal);

	return 0;
}

int wifimgr_lib_get_WLAN11AC_BUTTON_PATH(char *val, int len)
{
	snprintf(val, len, WLAN11AC_BUTTON_PATH);

	return 0;
}

int wifimgr_lib_get_WLAN11AC_BUTTON(char *string, int len, int *ret)
{
	int fd = 0;

	fd = open(WLAN11AC_BUTTON_PATH, O_RDONLY|O_NONBLOCK);
	if(fd != -1)
	{
		*ret = read(fd, string, len);
		string[len - 1] = '\0';
		close(fd);
	}
	else
		return -1;

	return 0;
}

int wifimgr_lib_get_WPS_STATUS(wifi_type type, unsigned short *output)
{
	int socket_id	= 0;
	struct iwreq wrq;
	int ret			= 0;
	char data[768] 	= {0};
	WSC_CONFIGURED_VALUE * pwscprofile=NULL;
	unsigned long val = 0;
	
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == socket_id){
		printf("[%s]Create socket fail\n",__FUNCTION__);
		return -1;
	}
	
	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra0");
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai0");	
	}
	else{
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return ECNT_WIFIMGR_CMD_ERR;
	}

#if defined(BBUTOWBU) || defined(WLAN_MULTIDRIVER)
	val =	(SIOCIWFIRSTPRIV + 0x12);
#else
	val =	(SIOCIWFIRSTPRIV + 0x15);
#endif

	wrq.u.data.length=sizeof(data);
	wrq.u.data.pointer=data;
	wrq.u.data.flags = 0;
	ret=ioctl(socket_id, val, &wrq);
	if(ret!=0){
		printf("[%s]:ioctl open fail\n",__FUNCTION__);
		close(socket_id);
		return -1;
	}
	pwscprofile = (WSC_CONFIGURED_VALUE *)wrq.u.data.pointer;
	*output = pwscprofile->WscConfigured;
	close(socket_id);

	return 0;
}
int wifimgr_lib_get_itf_hwaddr(char *br_itf_name, char *mac, int len)
{
	int sock = -1;
	struct ifreq ifbuf;
	uint8_t *tmphwaddr = NULL;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sock < 0 ) {
		printf("%s %d: socket open fail\n", __FUNCTION__,__LINE__);
		return -1;
	}

	memset(&ifbuf, 0, sizeof(ifbuf));
	strncpy(ifbuf.ifr_name, br_itf_name, sizeof(ifbuf.ifr_name) - 1);
	if ( ioctl(sock, SIOCGIFHWADDR, &ifbuf) == -1 ) {
		printf("%s %d: ioctl open fail\n", __FUNCTION__,__LINE__);
		close(sock);
		return -1;
	}
	tmphwaddr = (uint8_t *)ifbuf.ifr_ifru.ifru_hwaddr.sa_data;
	snprintf(mac, len, "%02X:%02X:%02X:%02X:%02X:%02X",
		tmphwaddr[0], tmphwaddr[1], tmphwaddr[2], tmphwaddr[3], tmphwaddr[4], tmphwaddr[5]);

	close(sock);

	return 0;
}

int wifimgr_lib_get_interface_mac(unsigned char *br_itf_name, unsigned char *mac)
{
	int fd = 0;
	int ret = 0;
	struct ifreq ifr;
	
	memset(&ifr, 0, sizeof(struct ifreq));
	if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) 
	{
		ifr.ifr_addr.sa_family = AF_INET;
		snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", br_itf_name);
		ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
		if (0 == ret) 
		{
			memcpy(mac, ifr.ifr_hwaddr.sa_data, MAC_ADDR_LENGTH);
		}
		close(fd);
	}
	else
		return 0;

	return 0;
}

int wifimgr_lib_get_interface_mac0(char *br_itf_name, char *mac, int len)
{
	int sock = -1;
	struct ifreq ifbuf;
	uint8_t *tmphwaddr = NULL;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if ( sock < 0 ) {
		printf("%s %d: socket open fail\n", __FUNCTION__,__LINE__);
		return -1;
	}

	memset(&ifbuf, 0, sizeof(ifbuf));
	strncpy(ifbuf.ifr_name, br_itf_name, sizeof(ifbuf.ifr_name) - 1);
	if ( ioctl(sock, SIOCGIFHWADDR, &ifbuf) == -1 ) {
		close(sock);
		return -1;
	}
	
	tmphwaddr = (uint8_t *)ifbuf.ifr_ifru.ifru_hwaddr.sa_data;
	snprintf(mac, len, "%02X:%02X:%02X:%02X:%02X:%02X",
		tmphwaddr[0], tmphwaddr[1], tmphwaddr[2], tmphwaddr[3], tmphwaddr[4], tmphwaddr[5]);

	close(sock);
	return 0;
}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_BUTTONDETECT)
int devfd = 0;	
int wifimgr_lib_get_LED_BUTTON_STATUS(int args[2])
{
	int n = 0;

	if(devfd == 0){
		devfd = open("/dev/led_button",O_RDWR);
	}
	
	if(devfd > 0){
		n = ioctl(devfd, LEDBUTTON_IOCTL_SELECT, args);
		if(n){
			printf("getbutton list error\n");
			return -1;
		}
	}
	else
		return -1;
		
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/
#ifdef WSC_AP_SUPPORT
int wifimgr_lib_get_WPS_ACL_INFO(wifi_type type, int index, RT_802_11_ACL * output)
{
	RT_802_11_ACL *AclInfor = NULL;
	int socket_id_acl		= 0;
	struct iwreq wrq;
	int ret=0;
	char data[768] 			= {0};
	int i = 0;

	socket_id_acl=socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id_acl == -1){
		printf("[%s]:Create ACL socket fail\n",__FUNCTION__);
		return -1;
	}
	
	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra%d", index);
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai%d", index);	
	}
	else{
		close(socket_id_acl);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return -1;
	}

	memset(data, 0, sizeof(data));
	wrq.u.data.length=sizeof(data);
	wrq.u.data.pointer=data;
	wrq.u.data.flags = OID_802_11_ACL_LIST;
	ret=ioctl(socket_id_acl, RT_PRIV_IOCTL, &wrq);
	if(ret!=0){
		printf("[%s]:ioctl open fail\n",__FUNCTION__);
		close(socket_id_acl);
		return -1;
	}

	AclInfor = (RT_802_11_ACL *) wrq.u.data.pointer;
	output->Policy = AclInfor->Policy;
	output->Num = AclInfor->Num;
	for (i = 0; i < MAX_NUMBER_OF_ACL; i++)
	{
		strncpy((char *)output->Entry[i].Addr, (const char *)AclInfor->Entry[i].Addr, MAC_ADDR_LENGTH);
		output->Entry[i].Rsv = AclInfor->Entry[i].Rsv;
#ifdef TCSUPPORT_WLAN_ACL_BLK_COUNT
		output->Entry[i].Reject_Count= AclInfor->Entry[i].Reject_Count;
#endif
	}
	close(socket_id_acl);

	return 0;
}
#endif

int wifimgr_lib_set_OUI_FILTER(wifi_type type, char *input, int dataLen)
{
	int socket_id	= 0;
	struct iwreq wrq;
	int ret;
	
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == socket_id){
		printf("[%s]Create socket fail\n",__FUNCTION__);
		return -1;
	}
	
	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra0");
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai0");	
	}
	else{
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return -1;
	}

	wrq.u.data.length=dataLen;
	wrq.u.data.pointer=input;
	wrq.u.data.flags = OID_SET_OUI_FILTER;
	wrq.u.data.flags |= OID_GET_SET_TOGGLE;

	ret=ioctl(socket_id, RT_PRIV_IOCTL, &wrq);
	if(ret!=0){
		printf("[%s]:ioctl open fail\n",__FUNCTION__);
		close(socket_id);
		return -1;
	}
	
	close(socket_id);

	return 0;
}

int wifimgr_lib_OidQueryInformation(unsigned long OidQueryCode, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;
	int value ;
	int  socket_id;

	//2.send delete cmd to wifi driver
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if ( socket_id < 0 )
	{
		printf("%s %d: socket open fail\n", __FUNCTION__,__LINE__);
		return -1;
	}
   	strncpy(wrq.ifr_name, DeviceName, sizeof(wrq.ifr_name)-1);
    wrq.u.data.length = PtrLength;
    wrq.u.data.pointer = ptr;
    wrq.u.data.flags = OidQueryCode;
	value = ioctl(socket_id, RT_PRIV_IOCTL,&wrq);
	if (value < 0)
		printf("@@@ioctl fail\r\n");

	close(socket_id);

    return value;
}

int wifimgr_lib_get_CHANNEL(wifi_type type, char* outchannel)
{
	struct iwreq wrq;
	int ret=0;
	char data[16];
	int socket_id;
	int defaultchannel=6;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdateChannel: Create socket fail\n");
		sprintf(outchannel,"%d",defaultchannel);
		return -1;
	}
	
	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra0");
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai0");	
	}
	else{
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return -1;
	}

	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;
	ret=ioctl(socket_id, SIOCGIWFREQ , &wrq);

	if ( ret != 0 ) {
		printf("==>UpdateChannel: ioctl open fail\n");
    		close(socket_id);
		sprintf(outchannel,"%d",defaultchannel);
		return -1;
	}
	sprintf(outchannel,"%d",wrq.u.freq.m);
	close(socket_id);

	return 0;
}

int wifimgr_lib_get_WPS_PIN_CODE(wifi_type type, int index, char* outPinCode)
{
	struct iwreq wrq;
	int ret=0;
	char data[16];
	int socket_id;
	int defaultPinCode=0;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdatePinCode: Create socket fail\n");
		sprintf(outPinCode,"%d",defaultPinCode);
		return -1;
	}

	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra%d", index);
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai%d", index);	
	}
	else{
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return -1;
	}

	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = RT_OID_WSC_PIN_CODE;
	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);
	if ( ret != 0 ) {
		printf("==>UpdatePinCode: ioctl open fail\n");
    	close(socket_id);
		sprintf(outPinCode,"%d",defaultPinCode);
		return -1;
	}

	sprintf(outPinCode,"%08u",*((unsigned int *)(data)));
	close(socket_id);

	return 0;
}

#ifdef TCSUPPORT_WLAN_MULTI_WPS
int wifimgr_lib_get_WpsStatus(wifi_type type, int *outWpsStatus, char *outWpsMac, int i)
{
	struct iwreq wrq;
	int ret=0;
	char data[20];
	WSC_PEER_DEV_INFO datainfo = {0};
	int socket_id;
	int defaultWpsStatus=0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(socket_id==-1){
		printf("==>UpdateWpsStatus: Create socket fail\n");
		*outWpsStatus=defaultWpsStatus;
		outWpsMac[0] = '\0';
		return -1;
	}

	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra%d", i);
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai%d", i);	
	}
	else{
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return -1;
	}
	
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);
	
	if ( ret != 0 ) {
		printf("==>UpdateWpsStatus: ioctl open fail\n");
    	close(socket_id);
		*outWpsStatus=defaultWpsStatus;
		outWpsMac[0] = '\0';
		return -1;
	}
	*outWpsStatus=*((int *)data);

	/* get the mac when in progress or configured*/
	if(*outWpsStatus > 2){
		wrq.u.data.length = sizeof(datainfo);
		wrq.u.data.pointer = &datainfo;
		wrq.u.data.flags = RT_OID_WSC_QUERY_PEER_INFO_ON_RUNNING;
		ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

		if ( ret != 0 ) {
			printf("==>UpdateWpsMac: ioctl open fail:%d\n",ret);
	    	close(socket_id);
			outWpsMac[0] = '\0';
			return -1;
		}
		
		sprintf(data,"%02X:%02X:%02X:%02X:%02X:%02X",
			datainfo.WscPeerMAC[0],datainfo.WscPeerMAC[1],datainfo.WscPeerMAC[2],datainfo.WscPeerMAC[3],datainfo.WscPeerMAC[4],datainfo.WscPeerMAC[5]);
		memcpy(outWpsMac,data, 18);
	}
	close(socket_id);
	return 0;
}

#else
int wifimgr_lib_get_WpsStatus(wifi_type type, int * outWpsStatus)
{
	struct iwreq wrq;
	int ret=0;
	char data[16];
	int socket_id;
	int defaultWpsStatus=0;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>UpdateWpsStatusAC: Create socket fail\n");
		*outWpsStatus=defaultWpsStatus;
		return -1;
	}

	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra0");
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai0");	
	}
	else{
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return -1;
	}
	
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;
	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>UpdateWpsStatusAC: ioctl open fail\n");
    		close(socket_id);
		*outWpsStatus=defaultWpsStatus;
		return -1;
	}
	*outWpsStatus=*((int *)data);

	close(socket_id);

	return 0;
	
}
#endif

#ifdef TCSUPPORT_WLAN_BNDSTRG
int wifimgr_lib_get_BndStrgChanload(char* outStatus,unsigned int len)
{
	struct iwreq wrq;
	int ret=0;
	char data[16] ={0};
	int socket_id;
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		printf("==>info_UpdateBndStrgChanload: Create socket fail\n");
		return -1;
	}

	memset(data, 0, sizeof(data));
	strcpy(wrq.ifr_name, "rai0");
   	wrq.u.data.length = sizeof(data);
    	wrq.u.data.pointer = data;
   	wrq.u.data.flags = RT_OID_BNDSTRG_SET_CHANLOAD;
   	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
		printf("==>info_UpdateBndStrgChanload: ioctl open fail\n");
    		close(socket_id);
		return -1;
	}
	snprintf(outStatus, len,"%d",*((unsigned int *)data));
	close(socket_id);

	return 0;
	
}	
#endif

int wifimgr_lib_get_WIFI_STATS(char *ifname, char *buffer, int size)
{
	int sock = -1, ret = -1;
	struct iwreq wrq;

	/* get current wlan traffic statistics information. */
	sock = socket( AF_INET, SOCK_DGRAM, 0);
	if ( -1 == sock ) {
		printf("==>%s: Create %s socket fail\n", __FUNCTION__, ifname);
		return -1;
	}
	memset(&wrq, 0, sizeof(wrq));
	snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "%s", ifname);
	wrq.u.data.length = size;
	wrq.u.data.pointer = buffer;
	wrq.u.data.flags = 0;
   	ret = ioctl(sock, RTPRIV_IOCTL_STATISTICS , &wrq);

	if ( ret != 0 ) {
		close(sock);
		return -1;
	}
	close(sock);
	
	return 0;
}	

int wifimgr_lib_set_WIFI_CMD(char *ifr, int index, int len, char *input, char *output, int type)
{
	char ifrName[16] = {0};

	if (-1 == index)
		snprintf(ifrName, sizeof(ifrName), "%s", ifr);
	else
		snprintf(ifrName, sizeof(ifrName), "%s%d", ifr, index);

	if (type == 0)
		snprintf(output, len, "/userfs/bin/iwpriv %s set %s", ifrName, input);
	else if (type == 1)
		snprintf(output, len, "/userfs/bin/iwpriv %s %s", ifrName, input);

	return 0;
}

#define SVC_WLAN_SCRIPT_PREFIX		"/userfs/bin/iwpriv ra%%d set %%s=%%s\n"
#define SVC_WLAN11AC_SCRIPT_PREFIX	"/userfs/bin/iwpriv rai%%d set %%s=%%s\n"

int wifimgr_lib_get_WLAN_SCRIPT(wifi_type type, int len, char *output)
{
	if(WIFI_2_4G == type){
		snprintf(output, len, SVC_WLAN_SCRIPT_PREFIX);
	}
	else if(WIFI_5G == type){
		snprintf(output, len, SVC_WLAN11AC_SCRIPT_PREFIX);
	}
	else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}

	return 0;
}

int wifimgr_lib_proberesp_execute(wifi_type type, vsie_probresp_set_cfg_t *buffer, int size, int ssidEnable)
{
	struct iwreq wrq;
	int socket_id	= 0;
	int ret = 0;
	int i = 0;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == socket_id)
	{
		return -1;
	}
	for (i = 0; i < 8; i++)
	{
		memset(&wrq, 0, sizeof(wrq));
		if(WIFI_2_4G == type){
			if (ssidEnable & (1 << i))
				sprintf(wrq.ifr_name, "ra%d", i);				
		}
		else if(WIFI_5G == type || 2 == type){
			if (ssidEnable & (1 << i))
				sprintf(wrq.ifr_name, "rai%d", i);
		}
		else{
			RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		}
		
		wrq.u.data.length = size;
		wrq.u.data.pointer = buffer;
		wrq.u.data.flags = RT_OID_AP_PROBE_RSP_VENDOR_IE_SET;		
	   	ret=ioctl(socket_id, RT_PRIV_IOCTL, &wrq);
		if(ret!=0)
		{
			close(socket_id);
			return -1;
		}
	}
	
	close(socket_id);
	
	return 0;
}

int wifimgr_lib_proberesp_delete(wifi_type type, vsie_probresp_del_cfg_t *buffer, int size)
{
	struct iwreq wrq;
	int socket_id	= 0;
	int ret = 0;

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == socket_id)
	{
		return -1;
	}
	printf("function ==> [%s]  line ==> [%d]\n",__FUNCTION__, __LINE__);
	memset(&wrq, 0, sizeof(wrq));
	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra0");
			
	}
	else if(WIFI_5G == type || 2 == type){
		sprintf(wrq.ifr_name, "rai0");	
	}
	else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
		return -1;
	}
	printf("function ==> [%s]  line ==> [%d]\n",__FUNCTION__, __LINE__);
	wrq.u.data.length = size;
	wrq.u.data.pointer = buffer;
	wrq.u.data.flags = RT_OID_AP_PROBE_RSP_VENDOR_IE_DEL;		
   	ret=ioctl(socket_id, RT_PRIV_IOCTL, &wrq);
	if(ret!=0)
	{
		close(socket_id);
		return -1;
	}
	printf("function ==> [%s]  line ==> [%d]\n",__FUNCTION__, __LINE__);
	close(socket_id);
	
	return 0;
}

int wifimgr_lib_get_CHIP_ID(unsigned int* chidIdx)
{
	FILE *fp = NULL;
	int ret;
	
	if ((fp = fopen(CHIP_ID_PATH, "r")) == NULL){
		printf("Critical Bugs, Can't get package id!\n");
		return -1;
	}
	
	ret = fscanf(fp, "%d", chidIdx);
	if(ret == -1)
		printf("fscanf error!");
	fclose(fp);	

	return 0;
}
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
int wifimgr_lib_set_Diag_Tz(char *ifname, diag_TZ_parameter *buffer, int size)
{
	int  socket_id;
	struct iwreq wrq;
	int ret = 0;

	wrq.u.data.length = size;
	wrq.u.data.pointer = buffer;
	wrq.u.data.flags = OID_SET_DIAG_TZ;
	wrq.u.data.flags |= OID_GET_SET_TOGGLE;
	
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id < 0){
		return -1;
	}
	memset(wrq.ifr_name, 0, sizeof(wrq.ifr_name));
	strncpy(wrq.ifr_name, ifname, sizeof(wrq.ifr_name)-1);
	ret = ioctl(socket_id, RT_PRIV_IOCTL, &wrq);
	if (ret != 0) {
		close(socket_id);
		return -1;
	}
	
	close(socket_id);
	socket_id = 0;
	
	return 0;
}
#endif

int wifimgr_lib_get_br0_v6addr(unsigned char *ifid)
{
	char ifname[] = "br0";
    int fd6 = 0;
    struct ifreq ifr;

	memset(&ifr, 0, sizeof(ifr));
    if ((fd6 = socket(AF_INET6, SOCK_DGRAM, 0)) == -1)
    {
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);	
    if(ioctl(fd6, SIOCGIFHWADDR, &ifr) < 0)
    {
        close(fd6);
        return -1;
    }

    ifid[2] = ifr.ifr_hwaddr.sa_data[0];
    ifid[2] ^= 0x02; /* reverse the u/l bit*/
    ifid[3] = ifr.ifr_hwaddr.sa_data[2];
    ifid[4] = ifr.ifr_hwaddr.sa_data[1];
    ifid[5] = 0xe3;
    ifid[6] = ifr.ifr_hwaddr.sa_data[4];
    ifid[7] = 0xef;

    ifid[8] = ifr.ifr_hwaddr.sa_data[0];
    ifid[8] ^= 0x02; /* reverse the u/l bit*/
    ifid[9] = ifr.ifr_hwaddr.sa_data[1];
    ifid[10] = ifr.ifr_hwaddr.sa_data[2];
    ifid[11] = 0xff;
    ifid[12] = 0xfe;
    ifid[13] = ifr.ifr_hwaddr.sa_data[3];
    ifid[14] = ifr.ifr_hwaddr.sa_data[4];
    ifid[15] = ifr.ifr_hwaddr.sa_data[5];

    close(fd6);

	return 0;
}

int wifimgr_lib_get_ifaddr(int sock, char *mac)
{	
	struct ifreq ifr;
	
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, "br0", sizeof(ifr.ifr_name) - 1);
	
	if ( ioctl(sock, SIOCGIFHWADDR, &ifr) < 0 )
	{
		return -1;
	}
	memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);

	return 0;
}
int wifimgr_lib_get_WLANSHARE_CMD(char *cmd, int actType, int index, char *enableUserId, char *userId)
{
	snprintf(cmd, 64, 
		"echo %d %d %s %s > %s", 
		actType, index, enableUserId, userId, WLANSHARE_PROC_PATH);

	return 0;
}

int wifimgr_lib_get_test_itf_status(const char *_iface, short if_flag) 
{
	struct ifreq ifr;
	int sockfd;
	int ret = 0;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) 
	{
		return 0;
	}
	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name , _iface , 15);

	ret = ioctl(sockfd, SIOCGIFFLAGS, &ifr);
	if (ret < 0) 
	{
		close(sockfd);
		return 0;
	}
	close(sockfd);
	if (ifr.ifr_flags & if_flag) 
	{
		return 1;
	}
	
	return 0;
}

int wifimgr_lib_set_5G_PCIE(int flag)
{
	char value1[10] = {0};
	char value2[10] = {0};
	char temp[32] = {0};
	int chipIdx = 0;
	
	wifimgr_lib_get_CHIP_ID((unsigned int *)&chipIdx);
	if(0 == flag){
		strncpy(value1 , "80000000" , 10);
		strncpy(value2 , "c0000000" , 10);
	}
	else if(1 == flag)
	{
		strncpy(value1 , "0" , 10);
		strncpy(value2 , "40000000" , 10);
	}
	else
	{
		RETURN_WIFIMGR_FAIL_MSG("The flag value is invalid.\n");
	}
	memset(temp, 0, sizeof(temp));
	switch(chipIdx)
	{
		case EN7526D:
		case EN7526G:
		case EN7528HU:
		case EN7528DU:
		case EN7561DU:
			snprintf(temp, sizeof(temp), "sys memwl bfac030c %s", value1);
			system(temp);
			break;
		case EN7580GT:
		case EN7580ST:
		case EN7580GAT:
			snprintf(temp, sizeof(temp), "sys memwl bfac080c %s", value2);
			system(temp);
			break;
		default:
			printf("%d\n", __LINE__);
			printf("unknow package\n");
			return ECNT_WIFIMGR_FAILURE;
	}
	return 0;	
}

int
wifimgr_lib_get_CPU_TEMPERATURE(wifi_type type, int *output)
{
	struct iwreq wrq;
	int socket_id;
	int data = 0;
	
	if(WIFI_2_4G == type){
		sprintf(wrq.ifr_name, "ra0");
			
	}
	else if(WIFI_5G == type){
		sprintf(wrq.ifr_name, "rai0");	
	}
	else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}

	wrq.u.data.length = 4;
	wrq.u.data.pointer = &data;
	wrq.u.data.flags = OID_GET_CPU_TEMPERATURE;	
		
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1){
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_WPS_STA_MAC: Create socket fail\n");
	}
	
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_WPS_STA_MAC: ioctl open fail\n");
	}

	*output = data;

	close(socket_id); 

	return ECNT_WIFIMGR_SUCCESS;
}

int wifimgr_lib_get_ethaddr(unsigned char *mac)
{	
#ifdef TCSUPPORT_CPU_ARMV8
	unsigned char mac_addr[MAC_ADDR_LENGTH] = {0};
	if(wifimgr_lib_get_interface_mac((unsigned char *)"br0", mac_addr) != 0){
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_ethaddr: get ethaddr fail\n");
	}
	memcpy(mac, mac_addr, MAC_ADDR_LENGTH);
#else
	FILE *fptmp 					= NULL; 
	unsigned char flash_mac_addr[6] = {0};
	int res = 0;
	long addr = 0;
	
	/* Read bootloader from flash */
	system("cat /dev/mtd0 > /tmp/boot.bin");

	/*Read MAC*/
	fptmp = fopen("/tmp/boot.bin", "r+b");
	if( fptmp == NULL ){
		printf("open boot.bin file error!!\n");
		unlink("/tmp/boot.bin");
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_ethaddr: Read bootloader fail\n");
	}

#ifdef TCSUPPORT_BB_256KB
		/* The mi.conf has new offset(0x3FE00) in 256KB tcboot.bin,
		 * The mi.conf offset is 0xFF00 in 128KB tcboot.bin.
		 */
	addr = 0x0003fe48L;
#else
	addr = 0x0000ff48L;
#endif
	if((res = fseek(fptmp, addr, SEEK_SET)) != 0) {
		fclose(fptmp);
		RETURN_WIFIMGR_FAIL_MSG("wifimgr_lib_get_ethaddr: fseek to 0x0003fe48L fail fail\n");
	}

	res = fread(flash_mac_addr, 6, 1, fptmp);
	memcpy(mac, flash_mac_addr, MAC_ADDR_LENGTH);
	
	fclose(fptmp);
	unlink("/tmp/boot.bin");
#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int
wifimgr_lib_get_img_boot_flag_offset(unsigned int *result)
{

#ifdef TCSUPPORT_NOR_FLASH_USED
	*result = SYSLOG_RA_OFFSET;
#else
	*result = IMG_BOOT_FLAG_OFFSET_COVER;
#endif
	return ECNT_WIFIMGR_SUCCESS;
}


int
wifimgr_lib_get_img_commit_flag_offset(unsigned int *result)
{
	*result = (IMG_BOOT_FLAG_OFFSET_COVER + 1);
	return ECNT_WIFIMGR_SUCCESS;
}
#ifdef TCSUPPORT_CT_UBUS
enum { PARTIAL_SCAN_IDLE, PARTIAL_SCAN_ONGOING, PARTIAL_SCAN_DONE };

static int8_t wifimgr_lib_get_partial_scan_status(int rfband)
{
	struct iwreq wrq;
	int socket_id;
    int8_t status = 0;

	if(WIFI_2_4G == rfband){
		sprintf(wrq.ifr_name, "ra0");
			
	}else{
		sprintf(wrq.ifr_name, "rai0");	
	}

	wrq.u.data.length = 1;
	wrq.u.data.pointer = &status;
	wrq.u.data.flags = OID_GET_PARTIAL_SCAN_STATUS;	

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1){
        LIB_WIFI_ERROR("create socket failed!\n");
		return -1;
	}
	
	if(ioctl(socket_id, RT_PRIV_IOCTL , &wrq) != 0){
		close(socket_id);
        LIB_WIFI_ERROR("ioctlt failed!\n");
		return -1;;
	}

	close(socket_id);
	return status;
}
#endif

#define WIFI_NO_SUPRESS_PRINT "/tmp/wifimgr_no_supress_driver_print"

int
wifimgr_lib_set_PartialScan(wifi_type type)
{
    char cmd[128] = {0};
	
    if(WIFI_2_4G == type){
#ifdef TCSUPPORT_CT_UBUS
        if(access(WIFI_NO_SUPRESS_PRINT, F_OK) != 0) {
            system("iwpriv ra0 set Debug=0");
        }
#endif
		snprintf(cmd, sizeof(cmd), "%s", "/userfs/bin/iwpriv ra0 set PartialScan=1 &");
    }else if(WIFI_5G == type){
#ifdef TCSUPPORT_CT_UBUS
        if(access(WIFI_NO_SUPRESS_PRINT, F_OK) != 0) {
            system("iwpriv rai0 set Debug=0");
        }
#endif
		snprintf(cmd, sizeof(cmd), "%s", "/userfs/bin/iwpriv rai0 set PartialScan=1 &");
    }else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}
    system(cmd);
	
	return 0;
}

int
wifimgr_lib_get_neighbourinfo(wifi_type type, NEIFHBOR_INFO *neifhbor_info)
{
	int i = 0, offset = 0;
    char *line = NULL;
    FILE *fp = NULL;
    size_t size = 0;
	char cmd[128] = {0};
	char file_path[32] = {0};
	int ret = 0;

#ifdef TCSUPPORT_CT_UBUS
    struct timespec tm = { .tv_sec = 0, .tv_nsec = 200000000 }; // 200ms
    while((ret = wifimgr_lib_get_partial_scan_status(type) ) == PARTIAL_SCAN_ONGOING){
        nanosleep(&tm, NULL);
    }
#endif

	if(WIFI_2_4G == type){
		snprintf(file_path, sizeof(file_path), "%s", "/tmp/site_survey_ra0");
		snprintf(cmd, sizeof(cmd), "%s > %s", "/userfs/bin/iwpriv ra0 get_site_survey", file_path);
    }else if(WIFI_5G == type){
    	snprintf(file_path, sizeof(file_path), "%s", "/tmp/site_survey_rai0");
		snprintf(cmd, sizeof(cmd), "%s > %s", "/userfs/bin/iwpriv rai0 get_site_survey", file_path);
    }else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}
    system(cmd);

	fp = fopen(file_path, "r");   
    if(fp == NULL)
    {
        RETURN_WIFIMGR_FAIL_MSG("fp is NULL\n");
    }
	neifhbor_info->num = 0;
	offset = 4;
    while(getline(&line, &size, fp) != -1)
    {
    
		if(WIFI_2_4G == type)
		{
			snprintf(neifhbor_info->info_entry[neifhbor_info->num].band, sizeof(neifhbor_info->info_entry[neifhbor_info->num].band), "2.4G");
		}
		else
		{
			snprintf(neifhbor_info->info_entry[neifhbor_info->num].band, sizeof(neifhbor_info->info_entry[neifhbor_info->num].band), "5G");
		}

        i++;
        if(i < offset)
            continue;

#if defined(MT7915D) || defined(MT7916D) || defined(MT7613E) || (defined(MT7592) && !defined(TCSUPPORT_WLAN_MT7592_V41))
#if defined(TCSUPPORT_CT_JOYME2) || (defined(TCSUPPORT_CMCC_WLAN_ENHANCE) && !defined(MT7915D)) || (defined(TCSUPPORT_CMCC_WLAN_ENHANCE) && !defined(MT7916D))
	    ret = sscanf(line, "%*s %s %s %s %*s %s %*s %s", neifhbor_info->info_entry[neifhbor_info->num].channel, neifhbor_info->info_entry[neifhbor_info->num].ssid,
	    neifhbor_info->info_entry[neifhbor_info->num].bssid, neifhbor_info->info_entry[neifhbor_info->num].rssi, neifhbor_info->info_entry[neifhbor_info->num].wMode);
#else/*have no rssi*/
	    ret = sscanf(line, "%*s %s %s %s %*s %*s %s", neifhbor_info->info_entry[neifhbor_info->num].channel, neifhbor_info->info_entry[neifhbor_info->num].ssid,
	    neifhbor_info->info_entry[neifhbor_info->num].bssid, neifhbor_info->info_entry[neifhbor_info->num].wMode);
#endif
#elif defined(MT7615D)
#if defined(TCSUPPORT_WLAN_MT7615_TXOFFLOAD)
		ret = sscanf(line, "%*s %s %s %s %*s %s %*s %s", neifhbor_info->info_entry[neifhbor_info->num].channel, neifhbor_info->info_entry[neifhbor_info->num].ssid,
	    neifhbor_info->info_entry[neifhbor_info->num].bssid, neifhbor_info->info_entry[neifhbor_info->num].rssi, neifhbor_info->info_entry[neifhbor_info->num].wMode);
#elif defined(TCSUPPORT_WLAN_MT7615_V33)
		ret = sscanf(line, "%*s %s %s %s %*s %*s %s", neifhbor_info->info_entry[neifhbor_info->num].channel, neifhbor_info->info_entry[neifhbor_info->num].ssid,
	    neifhbor_info->info_entry[neifhbor_info->num].bssid, neifhbor_info->info_entry[neifhbor_info->num].wMode);
#else
#if defined(TCSUPPORT_CMCC_WLAN_ENHANCE)
	    ret = sscanf(line, "%s %s %s %*s %s %*s %s", neifhbor_info->info_entry[neifhbor_info->num].channel, neifhbor_info->info_entry[neifhbor_info->num].ssid,
	    neifhbor_info->info_entry[neifhbor_info->num].bssid, neifhbor_info->info_entry[neifhbor_info->num].rssi, neifhbor_info->info_entry[neifhbor_info->num].wMode);
#else
	    ret = sscanf(line, "%*s %s %s %s %*s %*s %s", neifhbor_info->info_entry[neifhbor_info->num].channel, neifhbor_info->info_entry[neifhbor_info->num].ssid,
	    neifhbor_info->info_entry[neifhbor_info->num].bssid, neifhbor_info->info_entry[neifhbor_info->num].wMode);
#endif
#endif
#elif defined(MT7592) && defined(TCSUPPORT_WLAN_MT7592_V41)
		ret = sscanf(line, "%s %s %*s %s %*s %*s %s", neifhbor_info->info_entry[neifhbor_info->num].channel, neifhbor_info->info_entry[neifhbor_info->num].ssid,
	    neifhbor_info->info_entry[neifhbor_info->num].bssid, neifhbor_info->info_entry[neifhbor_info->num].wMode);
#endif
		if (ret != 5 || neifhbor_info->info_entry[neifhbor_info->num].ssid[0]== '\0')
		{
			memset(&(neifhbor_info->info_entry[neifhbor_info->num]), 0, sizeof(NEIFHBOR_INFO_ENTRY));
			continue;
		}
		neifhbor_info->num++;
    }
	fclose(fp);
    if(line != NULL)
    {
        free(line);
    }
	return 0;
}

int wifimgr_lib_get_wps_info( char *ioctlCmd,  /*ioctl cmd like get_wps_regs_dyn_info*/
				 char *keyword,   /*get value accoding to this keyword like DeviceName = */
				 char *info,       /*return value*/
				 int infoLen,	  /*the length of info*/
				 char *nodename   /*the node name of wlan*/
			   )
{

	if(strstr(nodename, "WLan_Entry"))
	{
#if defined(RT3390) || defined(RT5392)|| defined(MT7592)	
		char cmd[256] = {0};
		char sf_iocltCmd[128] = {0};
		char buf[128];
		char *pStrStart= NULL, *pStrEnd = NULL;
		FILE *fp = NULL;
		int keywordLen;

		/*check input parameters*/
		if (ioctlCmd == NULL || keyword == NULL || info == NULL ) 
		{
			RETURN_WIFIMGR_FAIL_MSG("leave,parameter is invalid=====>\n");
		}

		keywordLen = strlen( keyword );
		
		memset( cmd, 0, sizeof(cmd) );
		memset( buf, 0, sizeof(buf) );

		api_escape_parameter(ioctlCmd, sf_iocltCmd, sizeof(sf_iocltCmd));
		/* 
		** Output the result of iwpriv cmd to a file, 
		** then parser this file to find the value of the keyword.
		*/
		snprintf( cmd, sizeof(cmd), "/userfs/bin/iwpriv ra0 ar9_show %s > %s", sf_iocltCmd, WPS_FILE );
		
		API_SYSTEM_RAW( cmd );
		
		if( (fp = fopen(WPS_FILE, "r" )) == NULL ) 
		{
			unlink( WPS_FILE );
			RETURN_WIFIMGR_FAIL_MSG("leave,open /tmp/wpsinfo fail=====>\n");
		}
		
		while ( fgets(buf, sizeof(buf), fp) != NULL ) 
		{
			
			/*find the keyword in this buf*/
			pStrStart = strstr( buf, keyword );
			if ( pStrStart == NULL ) 
			{
				memset( buf, 0, sizeof(buf) );
				continue;
			}
			
			/*the value is end of '\n', in general, it will not happen.*/
			pStrEnd = strchr( pStrStart, '\n' );
			if ( pStrEnd == NULL ) 
			{
				fclose(fp);
				unlink( WPS_FILE );
				RETURN_WIFIMGR_FAIL_MSG("leave,pStrEnd is NULL=====>\n");
				return -1;
			}

			/* if the lenth is more than info,just return, or tr69 will crash*/
			if ( (pStrEnd - (pStrStart + keywordLen)) > infoLen -1  ) 
			{
				fclose(fp);
				unlink( WPS_FILE );
				RETURN_WIFIMGR_FAIL_MSG("leave,the lenth is more than info=====>\n");
			}
			
			/*copy the value of this keyword to info*/
			strncpy( info, pStrStart + keywordLen, (pStrEnd - (pStrStart + keywordLen)) );
			
			info[ (pStrEnd - (pStrStart + keywordLen)) ] = '\0';
			memset( buf, 0, sizeof(buf) );
		}
		
		fclose(fp);
		unlink( WPS_FILE );
#endif
	}

	return 0;
}

int wifimgr_lib_set_needRebootFlag(int *needRebootFlag)
{
#if defined(TCSUPPORT_WLAN_MT7615_BUILD_IN) 
	*needRebootFlag = 1;
#endif

	return 0;
}

int wifimgr_lib_get_wifi_process_info(struct diag_wifi_process_info *process_info)
{	
#if defined(TCSUPPORT_WLAN)
	int socket_id = 0;
	struct iwreq req;
	int ret = 0;
	char data[768] 	= {0};
	struct diag_wifi_process_info local_process_info;
	struct diag_wifi_process_info *plocal_process_info = &local_process_info;
	int index = 0, i = 0;
	FILE *fp = NULL;
	char *line = NULL;
	size_t size = 0;
	int pid = 0;
	char task_name[32] = {0};

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id < 0)
	{
		return 1;
	}
	#if 0
	snprintf(name, sizeof(name), "ra0");
	req.u.data.length = sizeof(struct diag_wifi_process_info);
	strcpy(req.ifr_name, name);
	memset(process_info, 0, sizeof(struct diag_wifi_process_info));
	req.u.data.pointer = process_info;
	req.u.data.flags = 0;
	#endif

	strcpy(req.ifr_name, "rai0");
	req.u.data.length=sizeof(data);
	req.u.data.pointer=data;
	req.u.data.flags = 0;
	
	ret = ioctl(socket_id, RTPRIV_IOCTL_GET_PROCESS_INFO, &req);
	
	if (ret != 0)
	{
		close(socket_id);
		return 1;
	}

	memset(process_info, 0, sizeof(struct diag_wifi_process_info));
	plocal_process_info = (struct diag_wifi_process_info *)req.u.data.pointer;
	process_info->num = plocal_process_info->num + 1;
	/*add wlan.svc*/
	fp = fopen("/var/run/wlan_svc.pid", "r");
	if (fp)
	{
		if(getline(&line, &size, fp) != -1)
		{
			sscanf(line, "%d", &pid);
		}
		fclose(fp);
		if(line != NULL)
    {
        free(line);
    }
		snprintf(task_name, sizeof(task_name), "/proc/%d/cmdline", pid);
		fp = fopen(task_name, "r");
		if (fp)
		{
			snprintf((char *)process_info->entry[index].name, sizeof(process_info->entry[index].name), "wlan.svc");
			process_info->entry[index].is_process = 1;
			process_info->entry[index].pid = pid;
			i++;
			fclose(fp);
		}
	}

	for(index = i; index < 8 && index <= plocal_process_info->num; index++)
	{
		process_info->entry[index].is_process = plocal_process_info->entry[index - i].is_process;
		strncpy((char *)process_info->entry[index].name, (const char *)plocal_process_info->entry[index - i].name,
			sizeof(process_info->entry[index].name));		
		process_info->entry[index].pid = plocal_process_info->entry[index - i].pid;
	}	

	close(socket_id);
#endif
	return 0;
}


int wifimgr_lib_insmod_ko()
{
	char cmd[128] = {0};
#ifdef RT5392
	snprintf(cmd, sizeof(cmd), "rmmod rt5390ap.ko");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "insmod /lib/modules/rt5390ap.ko");
	system(cmd);
#elif MT7592
	snprintf(cmd, sizeof(cmd), "rmmod mt7603eap.ko");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "insmod /lib/modules/mt7603eap.ko");
	system(cmd);
#else
	snprintf(cmd, sizeof(cmd), "rmmod rt3390ap");
	system(cmd);
	snprintf(cmd, sizeof(cmd), "insmod /lib/modules/rt3390ap.ko");
	system(cmd);
#endif
	return 0;
}

#if defined(TCSUPPORT_CMCCV2)
int wifimgr_lib_get_wifiStaRssiInfo(PROBE_REQ_IOCTL_ROOT *IoctlData, wifi_type type)
{	
	struct iwreq wrq;		
	int socket_id = 0;
	int ret = 0;
		
	if( WIFI_2_4G == type )
		snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "ra0");
	else if( WIFI_5G == type )
		snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "rai0");
	else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);	
	if( socket_id == -1 )	
	{
		RETURN_WIFIMGR_FAIL_MSG("Create socket_id_StaRssi socket fail\n");
	}

	wrq.u.data.length=sizeof(PROBE_REQ_IOCTL_ROOT);	
	wrq.u.data.pointer=IoctlData;	
	wrq.u.data.flags = RT_OID_802_11_PROBE_REQ_STATISTICS;	
	ret = ioctl(socket_id, RT_PRIV_IOCTL, &wrq);

	if( ret != 0 )	
	{	
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("ioctl open fail\n");
	}	

	close(socket_id);
	
	return 0;
}
#endif

int wifimgr_lib_get_neighbourinfo_v1(PNDIS_802_11_GET_NEIGHBOUR_INFO IoctlData, wifi_type type)
{
	struct iwreq wrq;
	int socket_id = 0;
	int ret = 0;

	if( WIFI_2_4G == type )
		snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "ra0");
	else if( WIFI_5G == type )
		snprintf(wrq.ifr_name, sizeof(wrq.ifr_name), "rai0");
	else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);	
	if( socket_id == -1 )	
	{
		RETURN_WIFIMGR_FAIL_MSG("Create socket_id_StaRssi socket fail\n");
	}

	wrq.u.data.length=sizeof(NDIS_802_11_GET_NEIGHBOUR_INFO);	
	wrq.u.data.pointer=IoctlData;	
	wrq.u.data.flags = RT_OID_802_11_GET_NEIGHBOUR_INFO;
	ret = ioctl(socket_id, RT_PRIV_IOCTL, &wrq);

	if( ret != 0 )	
	{
		close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("ioctl open fail\n");
	}

	close(socket_id);
	
	return 0;
	
}

int
wifimgr_lib_set_DfsDedicatedZeroWait(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( 1 != type) {
		RETURN_WIFIMGR_FAIL_MSG("The type should be 1.\n");
	}
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);	// 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The DfsDedicatedZeroWait value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "DfsDedicatedZeroWait");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_DfsDedicatedZeroWait(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( 1 != type) {
		RETURN_WIFIMGR_FAIL_MSG("The type should be 1.\n");
	}

	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "DfsDedicatedZeroWait", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_DfsZeroWaitDefault(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	int  tmp = -1; 
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	if( 1 != type) {
		RETURN_WIFIMGR_FAIL_MSG("The type should be 1.\n");
	}
	
	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The ssid index is invalid.\n");
	
	tmp = atoi(input);	// 0 or 1
	if(doStrDecCheck(input) == 0 || tmp < 0 || tmp > 1)
		RETURN_WIFIMGR_FAIL_MSG("The DfsZeroWaitDefault value is invalid.\n");
	
	/*2. Construct suitable parameters*/
	sprintf (keyword, "DfsZeroWaitDefault");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}


int 
wifimgr_lib_get_DfsZeroWaitDefault(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	if( 1 != type) {
		RETURN_WIFIMGR_FAIL_MSG("The type should be 1.\n");
	}

	if( -1 != index) {
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");
	}
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, "DfsZeroWaitDefault", output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_MboSupport(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	
	/*get the BssidNum value and check ssid index is valid or not.*/
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));

	if( atoi(input) != 0 || atoi(input) != 1)
		RETURN_WIFIMGR_FAIL_MSG("The MboSupport value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "MboSupport%d",index);
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_MboSupport(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	char keyword[MAX_KEYWORD_LEN]  = {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	sprintf (keyword, "MboSupport%d", index);
	if(ECNT_WIFIMGR_SUCCESS == read_attr(type, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int
wifimgr_lib_set_MapMode(wifi_type type, int index, char *input)
{
	WIFI_FUNC_TRACE_ENTER;
	char keyword[MAX_KEYWORD_LEN]  = {0};
	/*1. Check the vaildity of input parameters*/
	if(NULL == input)		
		RETURN_WIFIMGR_FAIL;
	

	if(index != -1)
		RETURN_WIFIMGR_FAIL_MSG("The index should be -1.\n");

	if( atoi(input) < 0 || atoi(input) > 2)
		RETURN_WIFIMGR_FAIL_MSG("The MapMode value is invalid.\n");
	/*2. Construct suitable parameters*/
	sprintf (keyword, "MapMode");
	
	/*3. Write parameters to shm*/
	if(ECNT_WIFIMGR_SUCCESS == update_attr(type, index, keyword, input)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

int 
wifimgr_lib_get_MapMode(wifi_type type, int index, char *output, unsigned int size)
{
	WIFI_FUNC_TRACE_ENTER;
	char outbuffer[MAX_KEYWORD_LEN]= {0};
	char keyword[MAX_KEYWORD_LEN]  = {0};
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_bssidnum(type, -1, outbuffer, MAX_KEYWORD_LEN)){
		RETURN_WIFIMGR_FAIL;
	}
	if(index < 1 || index > atoi(outbuffer))
		RETURN_WIFIMGR_FAIL_MSG("The ssid index should be from 1 to BssidNum:%d.\n", atoi(outbuffer));
	sprintf (keyword, "MapMode");
	if(ECNT_WIFIMGR_SUCCESS == read_attr(0, index, keyword, output, size)) {
		RETURN_WIFIMGR_SUCCESS;
	}
	RETURN_WIFIMGR_FAIL;
}

#ifdef TCSUPPORT_CMCCV2
int wifimgr_lib_get_info_UpdateRadioStatics(RADIO_STATISTICS* pRadioStatics, wifi_type type)
{
	struct iwreq wrq;
	int ret=0;
	int socket_id;
	RADIO_STATISTICS radioStatics;
	unsigned long BcnFail=0;

	if(pRadioStatics == NULL){
		RETURN_WIFIMGR_FAIL_MSG("==>info_UpdateRadioStatics: pRadioStatics is NULL\n");
	}

	if( WIFI_2_4G == type )
		strcpy(wrq.ifr_name, "ra0");
	else if( WIFI_5G == type )
		strcpy(wrq.ifr_name, "rai0");
	else{
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}
	
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1){
		RETURN_WIFIMGR_FAIL_MSG("==>info_UpdateRadioStatics: Create socket fail\n");
	}
	wrq.u.data.length = sizeof(RADIO_STATISTICS);
	wrq.u.data.pointer = pRadioStatics;
	wrq.u.data.flags = OID_802_11_GET_RADIO_STATISTICS;
	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
    	close(socket_id);
		RETURN_WIFIMGR_FAIL_MSG("==>info_UpdateRadioStatics: ioctl open fail\n");
	}

	close(socket_id);

	return 0;
}

int wifimgr_lib_get_info_UpdateNoise(char* Noise, wifi_type type)
{
	struct iwreq wrq;
	int ret=0;
	int socket_id;
	int noise_floor = 0;

	if( WIFI_2_4G == type )
		strcpy(wrq.ifr_name, "ra0");
	else if( WIFI_5G == type )
		strcpy(wrq.ifr_name, "rai0");
	else{
		sprintf(Noise,"%d",noise_floor);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id == -1){
		sprintf(Noise,"%d",noise_floor);
		RETURN_WIFIMGR_FAIL_MSG("==>info_UpdateNoise: Create socket fail\n");
	}

	wrq.u.data.length = sizeof(noise_floor);
	wrq.u.data.pointer = &noise_floor;
	wrq.u.data.flags = OID_802_11_GET_NF;
	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
    	close(socket_id);
		sprintf(Noise,"%d",noise_floor);
		RETURN_WIFIMGR_FAIL_MSG("==>info_UpdateNoise: ioctl open fail\n");
	}

	sprintf(Noise,"%d",noise_floor);
	close(socket_id);
	return 0;

}

int wifimgr_lib_get_info_UpdateChanLoad(char* ChanLoad, wifi_type type)
{
	struct iwreq wrq;
	int ret=0;
	int socket_id;
	int channel_load = 0;

	if( WIFI_2_4G == type )
		strcpy(wrq.ifr_name, "ra0");
	else if( WIFI_5G == type )
		strcpy(wrq.ifr_name, "rai0");
	else{
		sprintf(ChanLoad,"%d",channel_load);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		sprintf(ChanLoad,"%d",channel_load);
		RETURN_WIFIMGR_FAIL_MSG("==>info_UpdateChanLoad: Create socket fail\n");
	}

	wrq.u.data.length = sizeof(channel_load);
	wrq.u.data.pointer = &channel_load;
	wrq.u.data.flags = RT_OID_GET_CHANLOAD;
	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
    	close(socket_id);
		sprintf(ChanLoad,"%d",channel_load);
		RETURN_WIFIMGR_FAIL_MSG("==>info_UpdateNoise: ioctl open fail\n");
	}
	
	channel_load /=10000;
	sprintf(ChanLoad,"%d",channel_load);
	close(socket_id);
	
	return 0;

}

int wifimgr_lib_get_info_UpdateChanIdle(char* ChanIdle, wifi_type type)
{
	struct iwreq wrq;
	int ret=0;
	int socket_id;
	int channel_load = 0;

	if( WIFI_2_4G == type )
		strcpy(wrq.ifr_name, "ra0");
	else if( WIFI_5G == type )
		strcpy(wrq.ifr_name, "rai0");
	else
	{
		sprintf(ChanIdle, "%d", channel_load);
		RETURN_WIFIMGR_FAIL_MSG("The wifi_type is wrong.\n");
	}

	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_id==-1){
		sprintf(ChanIdle, "%d", channel_load);
		RETURN_WIFIMGR_FAIL_MSG("==>info_UpdateChanIdle: Create socket fail\n");
	}

	wrq.u.data.length = sizeof(channel_load);
	wrq.u.data.pointer = &channel_load;
	wrq.u.data.flags = RT_OID_GET_CHANLOAD;
	ret=ioctl(socket_id, RT_PRIV_IOCTL , &wrq);

	if ( ret != 0 ) {
    	close(socket_id);
		sprintf(ChanIdle,"%d",channel_load);
		RETURN_WIFIMGR_FAIL_MSG("==>info_UpdateChanIdle: ioctl open fail\n");
	}

	channel_load /=10000;
	sprintf(ChanIdle,"%d",100-channel_load);
	close(socket_id);

	return 0;

}
#endif