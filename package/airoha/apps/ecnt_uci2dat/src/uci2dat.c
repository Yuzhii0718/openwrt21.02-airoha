/*****************************************************************************
 * $File:   uci2dat.c
 *
 * $Author: Hua Shao
 * $Date:   Feb, 2014
 *
 * Boring, Boring, Boring, Boring, Boring........
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <memory.h>
#include <getopt.h>
#include <uci.h>
#include <stdarg.h>
#include <ctype.h>
#ifdef OK
#undef OK
#endif
#define OK (0)
#ifdef NG
#undef NG
#endif
#define NG (-1)
#ifdef SHDBG
#undef SHDBG
#endif
#define SHDBG(...)   printf(__VA_ARGS__);
#define DEVNUM_MAX (8)
#define MBSSID_MAX (8)

int band=-1;
#define WLAN_DAT_PATH 					"/etc/Wireless/RT2860AP/RT2860AP.dat"
#define WLAN_AC_DAT_PATH 				"/etc/Wireless/RT2860AP_AC/RT2860AP.dat"

enum {
    MSG_EXCESSIVE, MSG_MSGDUMP, MSG_DEBUG, MSG_INFO, MSG_WARNING, MSG_ERROR
};

static unsigned int uci2dat_debug_level = MSG_ERROR;

void set_debug_level(int level)
{    
    if (level >= MSG_EXCESSIVE && level <= MSG_ERROR)
    {
        uci2dat_debug_level = level;
    }		
    else
        printf("invalid log level %d\n", level);
}

static void _uci2dat_ctrl_printf(char *fmt,...)
{
    FILE *proc_file = NULL;
    char msg[512];
    va_list args;
    
    va_start(args, fmt);
    vsnprintf(msg, 512, fmt, args);	

    proc_file = fopen("/proc/tc3162/dbg_msg", "w");
    if (NULL == proc_file) 
    {
        printf("open /proc/tc3162/dbg_msg fail\n");
        va_end(args);
        return;
    }

    fprintf(proc_file, "%s", msg);
    fclose(proc_file);
    va_end(args);

    return ;
}

#define UCI2DAT_DEBUG(Level, fmt, args...)   \
do {\
    if(Level >= uci2dat_debug_level)\
    {\
        _uci2dat_ctrl_printf("[%s]", __FUNCTION__);	\
        _uci2dat_ctrl_printf( fmt, ##args);          \
        _uci2dat_ctrl_printf("\n");\
    }\
} while(0)

int is_hex_string(char *str)
{
	int i;
	int len = strlen(str);
	for (i = 0; i < len; i++)
	{
		if (isxdigit(str[i]))
			continue;
		else
			return 0;
	}
	return 1;
}

int is_valid_mac(const char * mac)
{
	int i;
	int len = strlen(mac);

	if (len != 17 || 0 == strncmp(mac, "00:00:00:00:00:00", 17)) {
		UCI2DAT_DEBUG(MSG_WARNING, "mac:%s is invalid", mac);
		return 0;
	}

	for (i = 0; i < len; i++)
	{
		if ( (i+1) % 3 != 0 && !isxdigit(mac[i]) ) {
			UCI2DAT_DEBUG(MSG_WARNING, "mac:%s is invalid", mac);
			return 0;
		} else if ( (i+1) % 3 == 0 && mac[i] != ':') {
			UCI2DAT_DEBUG(MSG_WARNING, "mac:%s is invalid", mac);
			return 0;
		}
	}
	return 1;
}

#define FPRINT(fp, e, ...) \
            do {\
                char buffer[128] = {0}; \
                UCI2DAT_DEBUG(MSG_INFO,"%s(),%s=", __FUNCTION__, e->dat_key); \
                snprintf(buffer, sizeof(buffer), __VA_ARGS__); \
                if (strlen(buffer) == 0) { \
                    fprintf(fp, "%s", e->defvalue?e->defvalue:""); \
                    UCI2DAT_DEBUG(MSG_INFO,"%s", "<def>"); \
                } \
                else { \
                    fprintf(fp, "%s", buffer); \
                    UCI2DAT_DEBUG(MSG_INFO,"%s", buffer); \
                } \
                UCI2DAT_DEBUG(MSG_INFO,", def=\"%s\"\n", e->defvalue?e->defvalue:""); \
            }while(0)
#define WIFI_UCI_FILE "/etc/config/wireless"
#define PARSE_UCI_OPTION(dst, src) \
    do { \
        src = NULL; \
        src = uci_lookup_option_string(uci_ctx, s, dst.uci_key); \
        if(src) { \
            strncpy(dst.value, src, sizeof(dst.value)-1); \
            UCI2DAT_DEBUG(MSG_INFO,"%s(),    %s=%s\n", __FUNCTION__, dst.uci_key, dst.value); \
        } \
    }while(0)
struct _param;
typedef void (*ucihook)(FILE *,struct _param *, const char * devname);
typedef struct  _param
{
    const char *    dat_key;
    const char *    uci_key;
    char            value[256];
    ucihook         hook;
    const char *    defvalue;
} param;
typedef struct _vif
{
    param ssid;
    param wmm;             /* wmm capable */
    param authmode;        /* wep, wpa, ... */
    param hidden;          /* Hidden SSID */
    param maxassoc;        /* Maximum assoc stations */
    param cipher;          /* ccmp(aes),tkip */
    param key;             /* wpa psk */
    param wepkey[4];       /* wep key, ugly, yep! */
    param auth_server;
    param auth_port;
    param auth_secret;
    param rekeyinteval;
	param rekeymethod;
    param preauth;
    param pmkcacheperiod;
    param disabled;
    param macaddr;
    param s8021x;
	param ieee80211w;
	param wsc_v2;
} vif;
typedef struct
{
    char    devname[16];
    param * params;
    int     vifnum;
    vif     vifs[MBSSID_MAX];
} wifi_params;
void hooker(FILE * fp, param * p, const char * devname);
/* these are separated from CFG_ELEMENTS because they
   use a different represention structure.
*/
vif VIF =
{
    .ssid               = {NULL, "ssid", {0}, NULL,  NULL},
    .wmm                = {NULL, "wmm", {0}, NULL,  NULL},
    .authmode           = {NULL, "encryption", {0}, NULL,  NULL},
    .hidden             = {NULL, "hidden", {0}, NULL,  NULL},
    .maxassoc           = {NULL, "maxassoc", {0}, NULL,  NULL},
    .cipher             = {NULL, "cipher", {0}, NULL,  NULL},
    /* wpa key, or wep key index */
    .key                = {NULL, "key", {0}, NULL,  NULL},
    /* wep key group */
    .wepkey             = {{NULL, "key1", {0}, NULL,  NULL},
                           {NULL, "key2", {0}, NULL,  NULL},
                           {NULL, "key3", {0}, NULL,  NULL},
                           {NULL, "key4", {0}, NULL,  NULL}},
    /* wpa & 8021x */
    .auth_server        = {NULL, "auth_server", "0", NULL,  NULL},
    .auth_port          = {NULL, "auth_port", "1812", NULL,  NULL},
    .auth_secret        = {NULL, "auth_secret", {0}, NULL,  NULL},
    .pmkcacheperiod     = {NULL, "pmkcacheperiod", {0}, NULL,  NULL},
    .preauth            = {NULL, "preauth", {0}, NULL,  NULL},
    .rekeyinteval       = {NULL, "rekeyinteval", {0}, NULL,  NULL},
    .rekeymethod       = {NULL, "rekeymethod", {0}, NULL,  NULL},

    /*add*/
    .disabled       	= {NULL, "disabled", {0}, NULL,  NULL},
    .macaddr      	 	= {NULL, "macaddr",  {0}, NULL,  NULL},
    .ieee80211w      	= {NULL, "ieee80211w",  {0}, NULL,  NULL},
    .wsc_v2      		= {NULL, "wsc_v2",  {0}, NULL,  NULL},
};

param CFG_ELEMENTS[] =
{
    /* Default configurations described in :
        MTK_Wi-Fi_SoftAP_Software_Programmming_Guide_v3.6.pdf
    */

    {"AccessPolicy0", NULL, {0}, NULL, "0"},
    {"AccessControlList0", NULL, {0}, NULL, NULL},
    {"AccessPolicy1", NULL, {0}, NULL, "0"},
    {"AccessControlList1", NULL, {0}, NULL, NULL},
    {"AccessPolicy2", NULL, {0}, NULL, "0"},
    {"AccessControlList2", NULL, {0}, NULL, NULL},
    {"AccessPolicy3", NULL, {0}, NULL, "0"},
    {"AccessControlList3", NULL, {0}, NULL, NULL},
	{"AccessPolicy4", NULL, {0}, NULL, "0"},
	{"AccessControlList4", NULL, {0}, NULL, NULL},
	{"AccessPolicy5", NULL, {0}, NULL, "0"},
	{"AccessControlList5", NULL, {0}, NULL, NULL},
	{"AccessPolicy6", NULL, {0}, NULL, "0"},
	{"AccessControlList6", NULL, {0}, NULL, NULL},
	{"AccessPolicy7", NULL, {0}, NULL, "0"},
	{"AccessControlList7", NULL, {0}, NULL, NULL},
    
    {"ApCliAuthMode", NULL, {0}, NULL, NULL},
    {"ApCliBssid", NULL, {0}, NULL, NULL},
    {"ApCliDefaultKeyID", NULL, {0}, NULL, "0"},
    {"ApCliEnable", NULL, {0}, NULL, "0"},
    {"ApCliEncrypType", NULL, {0}, NULL, NULL},
    {"ApCliKey1Type", NULL, {0}, NULL, "0"},
    {"ApCliKey1Str", NULL, {0}, NULL, NULL},
    {"ApCliKey2Type", NULL, {0}, NULL, "0"},
    {"ApCliKey2Str", NULL, {0}, NULL, NULL},
    {"ApCliKey3Type", NULL, {0}, NULL, "0"},
    {"ApCliKey3Str", NULL, {0}, NULL, NULL},
    {"ApCliKey4Type", NULL, {0}, NULL, "0"},
    {"ApCliKey4Str", NULL, {0}, NULL, NULL},
    {"ApCliSsid", NULL, {0}, NULL, NULL},
    {"ApCliWirelessMode", NULL, {0}, NULL, NULL},
    {"ApCliWPAPSK", NULL, {0}, NULL, NULL},

    {"ApCliMuOfdmaDlEnable",NULL, {0}, hooker, "0"},
    {"ApCliMuOfdmaUlEnable",NULL, {0}, hooker, "0"},
    {"ApCliMuMimoDlEnable",NULL, {0}, hooker, "0"},
    {"ApCliMuMimoUlEnable", NULL,{0}, hooker, "0"},
    {"APCwmax", "apcwmax", {0}, hooker, "6;10;4;3"},
    {"APCwmin", "apcwmin", {0}, hooker, "4;4;3;2"},
    // {"UAPSDCapable",NULL, {0}, hooker, "1"},
    {"APTxop", "aptxop", {0}, hooker, "0;0;94;47"},
    {"AuthMode", NULL, {0}, hooker,  "OPEN"},
    {"AutoChannelSelect", "channel", {0}, hooker, "2"},
    {"AutoChannelSkipList", "autoch_skip", {0}, hooker, NULL},
    {"AutoProvisionEn", "autoprovision", {0}, hooker, "0"},
    {"BandSteering", NULL, {0}, hooker, "0;0;0;0;0;0;0;0"},
    {"BasicRate",  NULL, {0}, hooker, "15"},
    {"BeaconPeriod", "beacon_int", {0}, hooker,  "100"},
    {"BFBACKOFFenable",NULL, {0}, hooker, "0"},
    {"BgndScanSkipCh",	NULL, {0}, hooker,	NULL},
    {"BGProtection", "bgprotect", {0}, hooker,	"0"},
    {"BndStrgBssIdx",  NULL, {0}, hooker,  NULL},
    {"BSSACM", "bssacm", {0}, hooker, "0;0;0;0"},
    {"BSSAifsn", "bssaifsn", {0}, hooker, "3;7;2;2"},
    {"BSSCwmax", "bsscwmax", {0}, hooker, "10;10;4;3"},
    {"BSSCwmin", "bsscwmin", {0}, hooker, "4;4;3;2"},
    {"BssidNum", "bssidnum", {0}, hooker,  "4"},
    {"BSSTxop", "bsstxop", {0}, hooker, "0;0;94;47"},
    {"BW_Enable", "bw_enable", {0}, hooker, "0"},
    {"BW_Guarantee_Rate",  NULL, {0}, hooker,  NULL},
    {"BW_Maximum_Rate",  NULL, {0}, hooker,  NULL},
    {"BW_Priority",  NULL, {0}, hooker,  NULL},
    {"BW_Root",  NULL, {0}, hooker,  NULL},
    {"CalCacheApply", NULL, {0}, hooker,  "0"},
    {"CarrierDetect", "carrierdetect", {0}, hooker, "0"},
    {"Channel", "channel", {0}, hooker,  "0"},
    {"ChannelGrp", "channels", {0}, hooker,  NULL},
    {"CountryCode", "country", {0}, hooker, NULL},
    {"CountryRegion", "region", {0}, hooker,  "1"},
    {"CountryRegionABand", "aregion", {0}, hooker, "7"},
    {"CP_SUPPORT", NULL, {0}, hooker,  "2"},
    {"CSPeriod", "csperiod", {0}, hooker, "10"},
    {"DBDC_MODE", NULL, {0}, hooker,  "1"},
    {"DebugFlags", "debugflags", {0}, hooker, "0"},
    {"DefaultKeyID", NULL, {0}, hooker, "1"},
    {"DfsCalibration", NULL, {0}, hooker, "0"},
    {"DfsEnable", NULL, {0}, hooker, NULL},
    {"DfsFalseAlarmPrevent", NULL, {0}, hooker, "1"},
    {"DfsZeroWait", NULL, {0}, hooker, "1"},
    {"DfsZeroWaitCacTime", NULL, {0}, hooker, "255"},
    {"DfsDedicatedZeroWait", NULL, {0}, hooker, NULL},
    {"DfsZeroWaitDefault", NULL, {0}, hooker,NULL},
    {"DisableOLBC", "disolbc", {0}, hooker, "0"},
    {"DtimPeriod", "dtim", {0}, hooker,  "1"},
    {"EfuseBufferMode", "efusebufmode", {0}, hooker, "0"},
    {"EAPifname", "eapifname", {0}, hooker, NULL},
    {"EDCCAEnable", NULL, {0}, hooker,	"1"},
    {"EncrypType", NULL, {0}, hooker,  "NONE"},
    {"EthConvertMode", NULL, {0}, hooker,  "dongle"},
    {"EtherTrafficBand", NULL, {0}, hooker,  "0"},
    {"Ethifname", "ethifname", {0}, hooker, NULL},
    {"ETxBfEnCond", "etxbfencond", {0}, NULL, "0"},
    {"FineAGC", "fineagc", {0}, hooker, "0"},
    {"FixedTxMode", "fixedtxmode", {0}, hooker, "0"},
    {"ForceRoamSupport", NULL,	{0}, hooker, NULL},
    {"FragThreshold", "fragthres", {0}, hooker,  "2346"},
    {"FreqDelta", "freqdelta", {0}, hooker, "0"},
    {"FtSupport", NULL, {0}, hooker,  "0"},
    {"GreenAP", "greenap", {0}, hooker, "0"},
    {"G_BAND_256QAM", NULL, {0}, hooker,  "1"},
    {"HideSSID", "hidden", {0}, hooker,  "0"},
    {"HT_AMSDU", "ht_amsdu", {0}, hooker, NULL},
    {"AMSDU_NUM", NULL, {0}, hooker,  "8"},
    {"HT_AutoBA", "ht_autoba", {0}, hooker, "1"},
    {"HT_BADecline", "ht_badec", {0}, hooker, "0"},
    {"HT_BAWinSize", "ht_bawinsize", {0}, hooker, "64"},
    {"HT_BSSCoexistence", "ht_coex", {0}, hooker, "0"},
    {"HT_BW", "htmode", {0}, hooker,  "0"},
    {"HT_DisallowTKIP", "ht_distkip", {0}, hooker, "0"},
    {"HT_EXTCHA", NULL, {0}, hooker,  "0"},
    {"HT_GI", "ht_gi", {0}, hooker,  "1"},
    {"HT_HTC", "ht_htc", {0}, hooker, "0"},
    {"HT_LDPC", "ht_ldpc", {0}, hooker, "1"},
    {"HT_LinkAdapt", "ht_linkadapt", {0}, hooker, "0"},
    {"HT_MCS", "ht_mcs", {0}, hooker, "33"},
    {"HT_MpduDensity", NULL, {0}, hooker, "5"},
    {"HT_OpMode", "ht_opmode", {0}, hooker, "0"},
    {"HT_PROTECT", NULL, {0}, hooker, "1"},
    {"HT_RDG", "ht_rdg", {0}, hooker,  "0"},
    {"HT_RxStream", "ht_rxstream", {0}, hooker, "1"},
    {"HT_STBC", "ht_stbc", {0}, hooker, "0"},
    {"HT_TxStream", "ht_txstream", {0}, hooker, "1"},
    {"IcapMode", NULL, {0}, hooker, "0"},
    {"idle_timeout_interval", "idle_intv", {0}, hooker, "0"},
    {"IEEE80211H", "doth", {0}, hooker,  "0"},
    {"IEEE8021X", "ieee8021x", {0}, hooker, "0"},
    {"IgmpSnEnable", NULL, {0}, NULL, "1"},
    {"ITxBfEn", "itxbf", {0}, hooker, "0"},
    
    {"Key1Type", NULL, {0}, hooker, "1"},
    {"Key1Str1", NULL, {0}, hooker, NULL},
    {"Key2Str1", NULL, {0}, hooker, NULL},
    {"Key3Str1", NULL, {0}, hooker, NULL},
    {"Key4Str1", NULL, {0}, hooker, NULL},
    {"Key2Type", NULL, {0}, hooker, "1"},
	{"Key1Str2", NULL, {0}, hooker, NULL},
	{"Key2Str2", NULL, {0}, hooker, NULL},
	{"Key3Str2", NULL, {0}, hooker, NULL},
	{"Key4Str2", NULL, {0}, hooker, NULL},
    {"Key3Type", NULL, {0}, hooker, "1"},
	{"Key1Str3", NULL, {0}, hooker, NULL},
	{"Key2Str3", NULL, {0}, hooker, NULL},
	{"Key3Str3", NULL, {0}, hooker, NULL},
	{"Key4Str3", NULL, {0}, hooker, NULL},
    {"Key4Type", NULL, {0}, hooker, "1"},
	{"Key1Str4", NULL, {0}, hooker, NULL},
	{"Key2Str4", NULL, {0}, hooker, NULL},
	{"Key3Str4", NULL, {0}, hooker, NULL},
	{"Key4Str4", NULL, {0}, hooker, NULL},
	{"Key5Type", NULL, {0}, hooker, "1"},
	{"Key1Str5", NULL, {0}, hooker, NULL},
	{"Key2Str5", NULL, {0}, hooker, NULL},
	{"Key3Str5", NULL, {0}, hooker, NULL},
	{"Key4Str5", NULL, {0}, hooker, NULL},
	{"Key6Type", NULL, {0}, hooker, "1"},
	{"Key1Str6", NULL, {0}, hooker, NULL},
	{"Key2Str6", NULL, {0}, hooker, NULL},
	{"Key3Str6", NULL, {0}, hooker, NULL},
	{"Key4Str6", NULL, {0}, hooker, NULL},
	{"Key7Type", NULL, {0}, hooker, "1"},
	{"Key1Str7", NULL, {0}, hooker, NULL},
	{"Key2Str7", NULL, {0}, hooker, NULL},
	{"Key3Str7", NULL, {0}, hooker, NULL},
	{"Key4Str7", NULL, {0}, hooker, NULL},
	{"Key8Type", NULL, {0}, hooker, "1"},
	{"Key1Str8", NULL, {0}, hooker, NULL},
	{"Key2Str8", NULL, {0}, hooker, NULL},
	{"Key3Str8", NULL, {0}, hooker, NULL},
	{"Key4Str8", NULL, {0}, hooker, NULL},

    {"LinkTestSupport", NULL, {0}, hooker, "0"},
    {"MACRepeaterEn", NULL, {0}, hooker, NULL},
    {"MACRepeaterOuiMode", NULL, {0}, hooker, "0"},
    {"MeshAuthMode", "mesh_authmode", {0}, hooker, NULL},
    {"MeshAutoLink", "mesh_autolink", {0}, hooker, "0"},
    {"MeshDefaultkey", "mesh_defkey", {0}, hooker, "0"},
    {"MeshEncrypType", "mesh_enctype", {0}, hooker, NULL},
    {"MeshId", "mesh_id", {0}, hooker, NULL},
    {"MeshWEPKEY", "mesh_wepkey", {0}, hooker, NULL},
    {"MeshWPAKEY", "mesh_wpakey", {0}, hooker, NULL},
    {"MUTxRxEnable", "mutxrxenable", {0}, hooker, "1"},
    {"NoForwarding", "noforward", {0}, hooker, "0"},
    {"NoForwardingBTNBSSID", NULL, {0}, NULL, "0"},
    {"own_ip_addr", "own_ip_addr", {0}, hooker, "192.168.29.1"},
    {"PcieAspm", NULL, {0}, hooker, "0"},
    {"PERCENTAGEenable", NULL, {0}, hooker, "1"},
    {"PhyRateLimit", "phyratelimit", {0}, hooker, "0"},
    {"PMFMFPC", "ieee80211w", {0}, hooker,	"0"},
    {"PMFMFPR", "ieee80211w", {0}, hooker,	"0"},
    {"PMFSHA256", NULL, {0}, hooker, "0"},
    {"PMKCachePeriod", "pmkcacheperiod", {0}, hooker, "10"},
    {"PowerUpCckOfdm", NULL, {0}, hooker, "0:0:0:0:0:0:0"},
    {"PowerUpHT20", NULL, {0}, hooker, "0:0:0:0:0:0:0"},
    {"PowerUpHT40", NULL, {0}, hooker, "0:0:0:0:0:0:0"},
    {"PowerUpVHT160", NULL, {0}, hooker, "0:0:0:0:0:0:0"},
    {"PowerUpVHT20", NULL, {0}, hooker, "0:0:0:0:0:0:0"},
    {"PowerUpVHT40", NULL, {0}, hooker, "0:0:0:0:0:0:0"},
    {"PowerUpVHT80", NULL, {0}, hooker, "0:0:0:0:0:0:0"},
    {"PreAntSwitch", NULL, {0}, hooker, "0:0:0:0:0:0:0"},
    {"PreAuth", "preauth", {0}, hooker, "0"},
    {"PreAuthifname", "preauthifname", {0}, hooker, "br-lan"},
    {"RadioLinkSelection", NULL, {0}, hooker, "0"},
    {"RadioOn", "radio", {0}, hooker, "1"},
    {"RADIUS_Acct_Key", "radius_acctkey", {0}, hooker, NULL},
    {"RADIUS_Acct_Port", "radius_acctport", {0}, hooker, "1813"},
    {"RADIUS_Acct_Server", "radius_acctserver", {0}, hooker, NULL},

    {"RADIUS_Key1", "radius_key1", {0}, hooker, NULL},
    {"RADIUS_Key2", "radius_key2", {0}, hooker, NULL},
    {"RADIUS_Key3", "radius_key3", {0}, hooker, NULL},
    {"RADIUS_Key4", "radius_key4", {0}, hooker, NULL},
	{"RADIUS_Key5", "radius_key5", {0}, hooker, NULL},
	{"RADIUS_Key6", "radius_key6", {0}, hooker, NULL},
	{"RADIUS_Key7", "radius_key7", {0}, hooker, NULL},
	{"RADIUS_Key8", "radius_key8", {0}, hooker, NULL},
    {"RADIUS_Port", "auth_port", {0}, hooker, NULL},
    {"RADIUS_Server", "auth_server", {0}, hooker, NULL},
    {"RDRegion", NULL, {0}, NULL, NULL},
    {"RED_Enable", NULL, {0}, NULL, "1"},
    {"RekeyInterval", "rekeyinteval", {0}, hooker, "0"},
    {"RekeyMethod", "rekeymethod", {0}, hooker, "TIME"},
    {"RRMEnable", NULL, {0}, NULL, "1;0;0;0;0;0;0;0"},
    {"RTSThreshold", "rtsthres", {0}, hooker,  "2347"},
    {"session_timeout_interval", "session_intv", {0}, hooker, "0"},
    {"ShortSlot", "shortslot", {0}, hooker,  "1"},
    {"SkuTableIdx", NULL, {0}, NULL, "0"},
    {"SKUenable", NULL, {0}, NULL, "0"},
    {"SREnable", "srenable", {0}, NULL, "1"},
    {"SRMode", "srmode", {0}, NULL, "0"},
    {"SRSDEnable", "srsdenable", {0}, NULL, "1"},
    {"PPEnable", NULL, {0}, NULL, "0"},
    {"SSID1", NULL, {0}, hooker,  "OpenWrt"},
    {"SSID2", NULL, {0}, hooker,  NULL},
    {"SSID3", NULL, {0}, hooker,  NULL},
    {"SSID4", NULL, {0}, hooker,  NULL},
    {"SSID5", NULL, {0}, hooker,  NULL},
    {"SSID6", NULL, {0}, hooker,  NULL},
    {"SSID7", NULL, {0}, hooker,  NULL},
    {"SSID8", NULL, {0}, hooker,  NULL},
    {"StationKeepAlive", NULL, {0}, NULL, "0"},
    {"StreamMode", "streammode", {0}, hooker, "0"},
    {"StreamModeMac0", NULL, {0}, NULL, NULL},
    {"StreamModeMac1", NULL, {0}, NULL, NULL},
    {"StreamModeMac2", NULL, {0}, NULL, NULL},
    {"StreamModeMac3", NULL, {0}, NULL, NULL},
    {"TGnWifiTest", "tgnwifitest", {0}, hooker, "0"},
    {"ThermalRecal", NULL, {0}, NULL, "0"},
    {"CCKTxStream", NULL, {0}, NULL, "4"},
    {"TWTSupport", "twtsupport", {0}, NULL, "1"},
    {"TxBurst", "txburst", {0}, hooker,  "1"},
    {"TxPower", "txpower", {0}, hooker,  "100"},
    {"TxPreamble", "txpreamble", {0}, hooker,  "0"},
    {"VHT_BW", "htmode", {0}, hooker,  "0"},
    {"VHT_BW_SIGNAL", "vht_bw_sig", {0}, hooker,  "0"},
    {"VHT_LDPC", "vht_ldpc", {0}, hooker, "1"},
    {"VHT_Sec80_Channel", NULL, {0}, NULL, "0"},
    {"VHT_SGI", "vht_sgi", {0}, hooker,  "1"},
    {"VHT_STBC", "vht_stbc", {0}, hooker, "0"},
    {"Vht1024QamSupport", NULL, {0}, hooker, "1"},
    {"VLANID", NULL, {0}, hooker, "0"},
    {"VLANPriority", NULL, {0}, hooker, "0"},
    {"VLANTag", NULL, {0}, hooker, "0"},

    {"VOW_Airtime_Ctrl_En", NULL, {0}, NULL, NULL},
    {"VOW_Airtime_Fairness_En", NULL, {0}, NULL, "1"},
    {"VOW_BW_Ctrl", NULL, {0}, NULL, "0"},
    {"VOW_Group_Backlog", NULL, {0}, NULL, NULL},
    {"VOW_Group_DWRR_Max_Wait_Time", NULL, {0}, NULL, NULL},
    {"VOW_Group_DWRR_Quantum", NULL, {0}, NULL, NULL},
    {"VOW_Group_Max_Airtime_Bucket_Size", NULL, {0}, NULL, NULL},
    {"VOW_Group_Max_Rate", NULL, {0}, NULL, NULL},
    {"VOW_Group_Max_Rate_Bucket_Size", NULL, {0}, NULL, NULL},
    {"VOW_Group_Max_Ratio", NULL, {0}, NULL, NULL},
    {"VOW_Group_Max_Wait_Time", NULL, {0}, NULL, NULL},
    {"VOW_Group_Min_Airtime_Bucket_Size", NULL, {0}, NULL, NULL},
    {"VOW_Group_Min_Rate", NULL, {0}, NULL, NULL},
    {"VOW_Group_Min_Rate_Bucket_Size", NULL, {0}, NULL, NULL},
    {"VOW_Group_Min_Ratio", NULL, {0}, NULL, NULL},
    {"VOW_Rate_Ctrl_En", NULL, {0}, NULL, NULL},
    {"VOW_Refill_Period", NULL, {0}, NULL, NULL},
    {"VOW_RX_En", NULL, {0}, NULL, "1"},
    {"VOW_Sta_BE_DWRR_Quantum", NULL, {0}, NULL, NULL},
    {"VOW_Sta_BK_DWRR_Quantum", NULL, {0}, NULL, NULL},
    {"VOW_Sta_DWRR_Max_Wait_Time", NULL, {0}, NULL, NULL},
    {"VOW_Sta_VI_DWRR_Quantum", NULL, {0}, NULL, NULL},
    {"VOW_Sta_VO_DWRR_Quantum", NULL, {0}, NULL, NULL},
    {"VOW_WATF_Enable", NULL, {0}, NULL, NULL},
    {"VOW_WATF_MAC_LV0", NULL, {0}, NULL, NULL},
    {"VOW_WATF_MAC_LV1", NULL, {0}, NULL, NULL},
    {"VOW_WATF_MAC_LV2", NULL, {0}, NULL, NULL},
    {"VOW_WATF_MAC_LV3", NULL, {0}, NULL, NULL},
    {"VOW_WATF_Q_LV0", NULL, {0}, NULL, NULL},
    {"VOW_WATF_Q_LV1", NULL, {0}, NULL, NULL},
    {"VOW_WATF_Q_LV2", NULL, {0}, NULL, NULL},
    {"VOW_WATF_Q_LV3", NULL, {0}, NULL, NULL},
    {"VOW_WMM_Search_Rule_Band0", NULL, {0}, NULL, NULL},
    {"VOW_WMM_Search_Rule_Band1", NULL, {0}, NULL, NULL},

    {"WapiAsCertPath", NULL, {0}, NULL, NULL},
    {"WapiAsIpAddr", NULL, {0}, NULL, NULL},
    {"WapiAsPort", NULL, {0}, NULL, NULL},
    {"Wapiifname", NULL, {0}, NULL, NULL},
    {"WapiPsk1", NULL, {0}, NULL, NULL},
    {"WapiPsk2", NULL, {0}, NULL, NULL},
    {"WapiPsk3", NULL, {0}, NULL, NULL},
    {"WapiPsk4", NULL, {0}, NULL, NULL},
    {"WapiPsk5", NULL, {0}, NULL, NULL},
    {"WapiPsk6", NULL, {0}, NULL, NULL},
    {"WapiPsk7", NULL, {0}, NULL, NULL},
    {"WapiPsk8", NULL, {0}, NULL, NULL},
    {"WapiPskType", NULL, {0}, NULL, NULL},
    {"WapiUserCertPath", NULL, {0}, NULL, NULL},
    {"WCNTest", "wcntest", {0}, hooker, "0"},
    {"Wds0Key", NULL, {0}, NULL, NULL},
    {"Wds1Key", NULL, {0}, NULL, NULL},
    {"Wds2Key", NULL, {0}, NULL, NULL},
    {"Wds3Key", NULL, {0}, NULL, NULL},
    {"Wds4Key", NULL, {0}, NULL, NULL},
    {"Wds5Key", NULL, {0}, NULL, NULL},
    {"Wds6Key", NULL, {0}, NULL, NULL},
    {"Wds7Key", NULL, {0}, NULL, NULL},
    {"WdsEnable", "wds_enable", {0}, hooker, "0"},
    {"WdsEncrypType", "wds_enctype", {0}, hooker, "NONE"},
    {"WdsList", NULL, {0}, NULL, NULL},
    {"WdsPhyMode", "wds_phymode", {0}, hooker, NULL},
    {"WHNAT", NULL, {0}, NULL, "1"},
    {"WOEOFF", NULL, {0}, NULL, "0"},
    {"WiFiTest", NULL, {0}, NULL, "0"},
    {"WirelessMode", "htmode", {0}, hooker,  "9"},
    {"WmmCapable", "wmm", {0}, hooker, "1"},
    
    {"WPAPSK1", NULL, {0}, hooker,	NULL},
    {"WPAPSK2", NULL, {0}, hooker,	NULL},
    {"WPAPSK3", NULL, {0}, hooker,	NULL},
    {"WPAPSK4", NULL, {0}, hooker,	NULL},
    {"WPAPSK5", NULL, {0}, hooker,	NULL},
    {"WPAPSK6", NULL, {0}, hooker,	NULL},
    {"WPAPSK7", NULL, {0}, hooker,	NULL},
    {"WPAPSK8", NULL, {0}, hooker,	NULL},
    
    {"WscConfMode", "wsc_confmode", {0}, hooker, "0"},
    {"WscConfStatus", "wsc_confstatus", {0}, hooker, "2"},
    {"TxCmdMode", NULL, {0}, hooker, "0"},
    {"MuOfdmaDlEnable", "muofdmadlenable", {0}, hooker, "1"},
    {"MuOfdmaUlEnable", "muofdmaulenable", {0}, hooker, "0"},
    {"MuMimoDlEnable", "mumimodlenable", {0}, hooker,  "1"},
    {"MuMimoUlEnable", "mumimoulenable", {0}, hooker, "0"},
    {"MapMode", NULL, {0}, hooker, "1"},
    {"IsICAPFW", NULL, {0}, hooker, "0"},
    {"KernelRps", NULL, {0}, NULL, NULL},
    {"MboSupport", NULL, {0}, hooker, "0"},
    {"QuickChannelSwitch", NULL, {0}, hooker, "1"},
    {"MlmeMultiQEnable", NULL, {0}, hooker, "1"},
    {"MlmeMultiQCtrl", NULL, {0}, hooker, "5-3-1"},
    {"BSSColorValue", NULL, {0}, hooker, "255"},
    {"QoSR1Enable", NULL, {0}, hooker, "1"},
    {"DscpPriMapEnable", "dscpprimapenable", {0}, hooker, "0"},
    {"ScsEnable", NULL, {0}, hooker, "1"},
    {"BcnProt", NULL, {0}, hooker, "0"},
    {"ApCliBcnProt", NULL, {0}, hooker, "0"},
    {"PktAggregate", "pktaggre", {0}, hooker,  "0"},
    {"TurboRate", "turborate", {0}, hooker, "0"},
    {"APSDCapable", "apsd", {0}, hooker, "1"},
    {"WirelessEvent", "wirelessevent", {0}, hooker, "0"},
    {"VHT_DisallowNonVHT", "vht_disnonvht", {0}, hooker, NULL},
    {"ETxBfIncapable", "etxbfincapable", {0}, NULL, NULL},
    {"DfsTPDutyRatio", NULL, {0}, NULL, NULL},
    
    {"ApCliMacManual_Flag", NULL, {0}, hooker, "0"},
    {"ApCliMacManual", NULL, {0}, hooker, "12:34:56:78:9a:bd"},
    
    {"TxRate", "txrate", {0}, hooker, "0"},
    {"SHORTGIFLAG", NULL, {0}, NULL, "0"},
    {"RegDomain", NULL, {0}, NULL, "Global"},	
    {"MbssMaxStaNum", "maxassoc", {0}, hooker,	"0"},
    {"EnableSSID", "disabled", {0}, hooker,  "0"},
    
    {"WscKeyASCII", "wsc_keyascii", {0}, hooker, NULL},
    {"WscV2Support", "wsc_v2", {0}, hooker, NULL},
    {"WscMaxPinAttack", NULL, {0}, hooker,"10;10;10;10;10;10;10;10"},
    {"WscSetupLockTime", NULL, {0}, NULL, NULL},
    
    {"WscDefaultSSID1", NULL, {0}, hooker, NULL},
    {"WscDefaultSSID2", NULL, {0}, hooker, NULL},
    {"WscDefaultSSID3", NULL, {0}, hooker, NULL},
    {"WscDefaultSSID4", NULL, {0}, hooker, NULL},
    {"WscDefaultSSID5", NULL, {0}, hooker, NULL},
    {"WscDefaultSSID6", NULL, {0}, hooker, NULL},
    {"WscDefaultSSID7", NULL, {0}, hooker, NULL},
    {"WscDefaultSSID8", NULL, {0}, hooker, NULL},
    {"WSC_UUID_E1", NULL, {0}, hooker, NULL},
    {"WSC_UUID_E2", NULL, {0}, hooker, NULL},
    {"WSC_UUID_E3", NULL, {0}, hooker, NULL},
    {"WSC_UUID_E4", NULL, {0}, hooker, NULL},
    {"WSC_UUID_E5", NULL, {0}, hooker, NULL},
    {"WSC_UUID_E6", NULL, {0}, hooker, NULL},
    {"WSC_UUID_E7", NULL, {0}, hooker, NULL},
    {"WSC_UUID_E8", NULL, {0}, hooker, NULL},
    {"WSC_UUID_Str1", NULL, {0}, hooker, NULL},
    {"WSC_UUID_Str2", NULL, {0}, hooker, NULL},
    {"WSC_UUID_Str3", NULL, {0}, hooker, NULL},
    {"WSC_UUID_Str4", NULL, {0}, hooker, NULL},	
    {"WSC_UUID_Str5", NULL, {0}, hooker, NULL},
    {"WSC_UUID_Str6", NULL, {0}, hooker, NULL},
    {"WSC_UUID_Str7", NULL, {0}, hooker, NULL},
    {"WSC_UUID_Str8", NULL, {0}, hooker, NULL},
    
    {"MacAddress", "macaddr", {0}, hooker,0},
    {"LoadCodeMethod", NULL, {0}, hooker, "0"},
    
    {"ReloadFlag", NULL, {0}, hooker, "0"},
    {"MAP_Turnkey", NULL, {0}, hooker, "1"},
    {"MAP_Ext", NULL, {0}, hooker, "64;32;32;32;0;0;0;0"},
    {"WNMEnable", NULL, {0}, hooker, "0"},
    {"HeraStbcPriority", NULL, {0}, hooker, "0"},
    {"MaxRuOfdma", NULL, {0}, hooker, "8"},
    {"MaxDLMuMimo", NULL, {0}, hooker, "2"},
    {"MaxULMuMimo", NULL, {0}, hooker, "2"},
    {"Disable160RuMu", NULL, {0}, hooker, "1"},	

    /*
    {"AckPolicy", "ackpolicy", {0}, hooker, "0;0;0;0"},
    {"APACM", "apacm", {0}, hooker, "0;0;0;0"},
    {"APAifsn", "apaifsn", {0}, hooker, "3;7;1;1"},
    
    {"TxAntenna", "txantenna", {0}, hooker, NULL},
    {"RxAntenna", "rxantenna", {0}, hooker, NULL},
    
    {"APSDCapable", "apsd", {0}, hooker, "1"},
    {"DLSCapable", "dls", {0}, hooker, "0"},
    
    {"IdsEnable", "idsenable", {0}, hooker, "0"},
    {"AuthFloodThreshold", NULL, {0}, NULL, "32"},
    {"AssocReqFloodThreshold", NULL, {0}, NULL, "32"},
    {"ReassocReqFloodThreshold", NULL, {0}, NULL, "32"},
    {"ProbeReqFloodThreshold", NULL, {0}, NULL, "32"},
    {"DisassocFloodThreshold", NULL, {0}, NULL, "32"},
    {"DeauthFloodThreshold", NULL, {0}, NULL, "32"},
    {"EapReqFooldThreshold", NULL, {0}, NULL, "32"},

    {"WscManufacturer", "wscmanufacturer", {0}, hooker, NULL},
    {"WscModelName", "wscmodelname", {0}, hooker, NULL},
    {"WscDeviceName", "wscdevicename", {0}, hooker, NULL},
    {"WscModelNumber", "wscmodelnumber", {0}, hooker, NULL},
    {"WscSerialNumber", "wscserialnumber", {0}, hooker, NULL},

    {"ITxBfTimeout", NULL, {0}, NULL, "0"},
    {"ETxBfNoncompress", NULL, {0}, NULL, "0"},

    {"DfsLowerLimit", "dfs_lowlimit", {0}, hooker, "0"},
    {"DfsUpperLimit", "dfs_uplimit", {0}, hooker, "0"},
    {"DfsOutdoor", "dfs_outdoor", {0}, hooker, "0"},
    {"SymRoundFromCfg", NULL, {0}, NULL, "0"},
    {"BusyIdleFromCfg", NULL, {0}, NULL, "0"},
    {"DfsRssiHighFromCfg", NULL, {0}, NULL, "0"},
    {"DfsRssiLowFromCfg", NULL, {0}, NULL, "0"},
    {"DFSParamFromConfig", NULL, {0}, NULL, "0"},
    {"FCCParamCh0", NULL, {0}, NULL, NULL},
    {"FCCParamCh1", NULL, {0}, NULL, NULL},
    {"FCCParamCh2", NULL, {0}, NULL, NULL},
    {"FCCParamCh3", NULL, {0}, NULL, NULL},
    {"CEParamCh0", NULL, {0}, NULL, NULL},
    {"CEParamCh1", NULL, {0}, NULL, NULL},
    {"CEParamCh2", NULL, {0}, NULL, NULL},
    {"CEParamCh3", NULL, {0}, NULL, NULL},
    {"JAPParamCh0", NULL, {0}, NULL, NULL},
    {"JAPParamCh1", NULL, {0}, NULL, NULL},
    {"JAPParamCh2", NULL, {0}, NULL, NULL},
    {"JAPParamCh3", NULL, {0}, NULL, NULL},
    {"JAPW53ParamCh0", NULL, {0}, NULL, NULL},
    {"JAPW53ParamCh1", NULL, {0}, NULL, NULL},
    {"JAPW53ParamCh2", NULL, {0}, NULL, NULL},
    {"JAPW53ParamCh3", NULL, {0}, NULL, NULL},
    {"FixDfsLimit", "fixdfslimit", {0}, hooker, "0"},
    {"LongPulseRadarTh", "lpsradarth", {0}, hooker, "0"},
    {"AvgRssiReq", "avgrssireq", {0}, hooker, "0"},
    {"DFS_R66", "dfs_r66", {0}, hooker, "0"},
    {"BlockCh", "blockch", {0}, hooker, NULL},

    {"HSCounter", "hscount", {0}, hooker, "0"},

    {"E2pAccessMode", "e2paccmode", {0}, hooker, "2"},

    {"WscConfMethod", "wsc_confmethod", {0}, hooker, NULL},
    {"WscSecurityMode", "wsc_security", {0}, hooker, NULL},
    {"Wsc4digitPinCode", "wsc_4digitpin", {0}, hooker, NULL},
    {"WscVendorPinCode", "wsc_vendorpin", {0}, hooker, NULL},

    {"HT_MIMOPS", "mimops", {0}, hooker, "3"},

    */
};

#if 0
param CFG_ELEMENTS[] =
{
    /* Default configurations described in :
           MTK_Wi-Fi_SoftAP_Software_Programmming_Guide_v3.6.pdf
    */
    {"CountryRegion", "region", {0}, hooker,  "1"},
    {"CountryRegionABand", "aregion", {0}, hooker, "7"},
    {"CountryCode", "country", {0}, hooker, NULL},
    {"BssidNum", NULL, {0}, hooker,  "1"},
    {"SSID1", NULL, {0}, hooker,  "OpenWrt"},
    {"SSID2", NULL, {0}, hooker,  NULL},
    {"SSID3", NULL, {0}, hooker,  NULL},
    {"SSID4", NULL, {0}, hooker,  NULL},
    {"TxRate", "txrate", {0}, hooker, "0"},
    {"Channel", "channel", {0}, hooker,  "0"},

    {"WirelessMode", "hwmode", {0}, hooker,  "9"},
    {"ChannelGrp", "channels", {0}, hooker,  NULL},
    {"HT_BW", "htmode", {0}, hooker,  "0"},
    {"VHT_BW", "htmode", {0}, hooker,  "0"},
    {"BeaconPeriod", "beacon_int", {0}, hooker,  "100"},

    {"MbssMaxStaNum", "maxassoc", {0}, hooker,  "0"},

    {"EnableSSID", "disabled", {0}, hooker,  "0"},
    {"IEEE80211H", "doth", {0}, hooker,  "0"},

    {"PMFMFPC", "ieee80211w", {0}, hooker,  "0"},
    {"PMFMFPR", "ieee80211w", {0}, hooker,  "0"},

    {"BasicRate", "basicrate", {0}, hooker, "15"},
    {"DtimPeriod", "dtim", {0}, hooker,  "1"},
    {"TxPower", "txpower", {0}, hooker,  "100"},
    {"DisableOLBC", "disolbc", {0}, hooker, "0"},
    {"BGProtection", "bgprotect", {0}, hooker,  "0"},
    {"TxAntenna", "txantenna", {0}, hooker, NULL},
    {"RxAntenna", "rxantenna", {0}, hooker, NULL},
    {"TxPreamble", "txpreamble", {0}, hooker,  "0"},
    {"RTSThreshold", "rtsthres", {0}, hooker,  "2347"},
    {"FragThreshold", "fragthres", {0}, hooker,  "2346"},
    {"TxBurst", "txburst", {0}, hooker,  "1"},
    {"PktAggregate", "pktaggre", {0}, hooker,  "0"},
    {"TurboRate", "turborate", {0}, hooker, "0"},
    {"WmmCapable", "wmm", {0}, hooker, "1"},
    {"APSDCapable", "apsd", {0}, hooker, "1"},
    {"DLSCapable", "dls", {0}, hooker, "0"},
    {"APAifsn", "apaifsn", {0}, hooker, "3;7;1;1"},
    {"APCwmin", "apcwmin", {0}, hooker, "4;4;3;2"},
    {"APCwmax", "apcwmax", {0}, hooker, "6;10;4;3"},
    {"APTxop", "aptxop", {0}, hooker, "0;0;94;47"},
    {"APACM", "apacm", {0}, hooker, "0;0;0;0"},
    {"BSSAifsn", "bssaifsn", {0}, hooker, "3;7;2;2"},
    {"BSSCwmin", "bsscwmin", {0}, hooker, "4;4;3;2"},
    {"BSSCwmax", "bsscwmax", {0}, hooker, "10;10;4;3"},
    {"BSSTxop", "bsstxop", {0}, hooker, "0;0;94;47"},
    {"BSSACM", "bssacm", {0}, hooker, "0;0;0;0"},
    {"AckPolicy", "ackpolicy", {0}, hooker, "0;0;0;0"},
    {"NoForwarding", "noforward", {0}, hooker, "0"},
    {"NoForwardingBTNBSSID", NULL, {0}, NULL, "0"},
    {"HideSSID", "hidden", {0}, hooker,  "0"},
    {"MaxStaNum", "maxassoc", {0}, hooker,  "0"},
    {"MacAddress", "macaddr", {0}, hooker,  "0"},
    {"StationKeepAlive", NULL, {0}, NULL, "0"},
    {"ShortSlot", "shortslot", {0}, hooker,  "1"},
    {"AutoChannelSelect", "channel", {0}, hooker, "2"},
    {"IEEE8021X", "ieee8021x", {0}, hooker, "0"},
    {"CSPeriod", "csperiod", {0}, hooker, "10"},
    {"WirelessEvent", "wirelessevent", {0}, hooker, "0"},
    {"IdsEnable", "idsenable", {0}, hooker, "0"},
    {"AuthFloodThreshold", NULL, {0}, NULL, "32"},
    {"AssocReqFloodThreshold", NULL, {0}, NULL, "32"},
    {"ReassocReqFloodThreshold", NULL, {0}, NULL, "32"},
    {"ProbeReqFloodThreshold", NULL, {0}, NULL, "32"},
    {"DisassocFloodThreshold", NULL, {0}, NULL, "32"},
    {"DeauthFloodThreshold", NULL, {0}, NULL, "32"},
    {"EapReqFooldThreshold", NULL, {0}, NULL, "32"},
    {"PreAuth", "preauth", {0}, hooker, "0"},
    {"AuthMode", NULL, {0}, hooker,  "OPEN"},
    {"EncrypType", NULL, {0}, hooker,  "NONE"},
    {"RekeyInterval", "rekeyinteval", {0}, hooker, "0"},
    {"PMKCachePeriod", "pmkcacheperiod", {0}, hooker, "10"},
    {"WPAPSK1", NULL, {0}, hooker,  NULL},
    {"WPAPSK2", NULL, {0}, hooker,  NULL},
    {"WPAPSK3", NULL, {0}, hooker,  NULL},
    {"WPAPSK4", NULL, {0}, hooker,  NULL},
    {"DefaultKeyID", NULL, {0}, hooker, "1"},
    {"Key1Type", NULL, {0}, hooker, "1"},
    {"Key1Str1", NULL, {0}, hooker, NULL},
    {"Key1Str2", NULL, {0}, hooker, NULL},
    {"Key1Str3", NULL, {0}, hooker, NULL},
    {"Key1Str4", NULL, {0}, hooker, NULL},
    {"Key2Type", NULL, {0}, hooker, "1"},
    {"Key2Str1", NULL, {0}, hooker, NULL},
    {"Key2Str2", NULL, {0}, hooker, NULL},
    {"Key2Str3", NULL, {0}, hooker, NULL},
    {"Key2Str4", NULL, {0}, hooker, NULL},
    {"Key3Type", NULL, {0}, hooker, "1"},
    {"Key3Str1", NULL, {0}, hooker, NULL},
    {"Key3Str2", NULL, {0}, hooker, NULL},
    {"Key3Str3", NULL, {0}, hooker, NULL},
    {"Key3Str4", NULL, {0}, hooker, NULL},
    {"Key4Type", NULL, {0}, hooker, "1"},
    {"Key4Str1", NULL, {0}, hooker, NULL},
    {"Key4Str2", NULL, {0}, hooker, NULL},
    {"Key4Str3", NULL, {0}, hooker, NULL},
    {"Key4Str4", NULL, {0}, hooker, NULL},
    {"AccessPolicy0", NULL, {0}, NULL, "0"},
    {"AccessControlList0", NULL, {0}, NULL, NULL},
    {"AccessPolicy1", NULL, {0}, NULL, "0"},
    {"AccessControlList1", NULL, {0}, NULL, NULL},
    {"AccessPolicy2", NULL, {0}, NULL, "0"},
    {"AccessControlList2", NULL, {0}, NULL, NULL},
    {"AccessPolicy3", NULL, {0}, NULL, "0"},
    {"AccessControlList3", NULL, {0}, NULL, NULL},
    {"WdsEnable", "wds_enable", {0}, hooker, "0"},
    {"WdsEncrypType", "wds_enctype", {0}, hooker, "NONE"},
    {"WdsList", NULL, {0}, NULL, NULL},
    {"Wds0Key", NULL, {0}, NULL, NULL},
    {"Wds1Key", NULL, {0}, NULL, NULL},
    {"Wds2Key", NULL, {0}, NULL, NULL},
    {"Wds3Key", NULL, {0}, NULL, NULL},
    {"RADIUS_Server", "auth_server", {0}, hooker, NULL},
    {"RADIUS_Port", "auth_port", {0}, hooker, NULL},
    {"RADIUS_Key1", "radius_key1", {0}, hooker, NULL},
    {"RADIUS_Key2", "radius_key2", {0}, hooker, NULL},
    {"RADIUS_Key3", "radius_key2", {0}, hooker, NULL},
    {"RADIUS_Key4", "radius_key4", {0}, hooker, NULL},
    {"own_ip_addr", "own_ip_addr", {0}, hooker, "192.168.5.234"},
    {"EAPifname", "eapifname", {0}, hooker, NULL},
    {"PreAuthifname", "preauthifname", {0}, hooker, "br-lan"},
    {"HT_HTC", "ht_htc", {0}, hooker, "0"},
    {"HT_RDG", "ht_rdg", {0}, hooker,  "0"},
    {"HT_EXTCHA", "ht_extcha", {0}, hooker, "0"},
    {"HT_LinkAdapt", "ht_linkadapt", {0}, hooker, "0"},
    {"HT_OpMode", "ht_opmode", {0}, hooker, "0"},
    {"HT_MpduDensity", NULL, {0}, hooker, "5"}, 
    {"VHT_SGI", "vht_sgi", {0}, hooker,  "1"},
    {"VHT_STBC", "vht_stbc", {0}, hooker, "0"},
    {"VHT_BW_SIGNAL", "vht_bw_sig", {0}, hooker,  "0"},
    {"VHT_DisallowNonVHT", "vht_disnonvht", {0}, hooker, NULL},
    {"VHT_LDPC", "vht_ldpc", {0}, hooker, "1"},
    {"HT_AutoBA", "ht_autoba", {0}, hooker, "1"},
    {"HT_AMSDU", "ht_amsdu", {0}, hooker, NULL},
    {"HT_BAWinSize", "ht_bawinsize", {0}, hooker, "64"},
    {"HT_GI", "ht_gi", {0}, hooker,  "1"},
    {"HT_MCS", "ht_mcs", {0}, hooker, "33"},
    {"WscManufacturer", "wscmanufacturer", {0}, hooker, NULL},
    {"WscModelName", "wscmodelname", {0}, hooker, NULL},
    {"WscDeviceName", "wscdevicename", {0}, hooker, NULL},
    {"WscModelNumber", "wscmodelnumber", {0}, hooker, NULL},
    {"WscSerialNumber", "wscserialnumber", {0}, hooker, NULL},
    /* Extra configurations found in 76x2e */
    {"FixedTxMode", "fixedtxmode", {0}, hooker, "0"},
    {"AutoProvisionEn", "autoprovision", {0}, hooker, "0"},
    {"FreqDelta", "freqdelta", {0}, hooker, "0"},
    {"CarrierDetect", "carrierdetect", {0}, hooker, "0"},
    {"ITxBfEn", "itxbf", {0}, hooker, "0"},
    {"PreAntSwitch", "preantswitch", {0}, hooker, "1"},
    {"PhyRateLimit", "phyratelimit", {0}, hooker, "0"},
    {"DebugFlags", "debugflags", {0}, hooker, "0"},
    {"ETxBfEnCond", NULL, {0}, NULL, "0"},
    {"ITxBfTimeout", NULL, {0}, NULL, "0"},
    {"ETxBfNoncompress", NULL, {0}, NULL, "0"},
    {"ETxBfIncapable", NULL, {0}, NULL, "0"},
    {"FineAGC", "fineagc", {0}, hooker, "0"},
    {"StreamMode", "streammode", {0}, hooker, "0"},
    {"StreamModeMac0", NULL, {0}, NULL, NULL},
    {"StreamModeMac1", NULL, {0}, NULL, NULL},
    {"StreamModeMac2", NULL, {0}, NULL, NULL},
    {"StreamModeMac3", NULL, {0}, NULL, NULL},
    {"RDRegion", NULL, {0}, NULL, NULL},
    {"DfsLowerLimit", "dfs_lowlimit", {0}, hooker, "0"},
    {"DfsUpperLimit", "dfs_uplimit", {0}, hooker, "0"},
    {"DfsOutdoor", "dfs_outdoor", {0}, hooker, "0"},
    {"SymRoundFromCfg", NULL, {0}, NULL, "0"},
    {"BusyIdleFromCfg", NULL, {0}, NULL, "0"},
    {"DfsRssiHighFromCfg", NULL, {0}, NULL, "0"},
    {"DfsRssiLowFromCfg", NULL, {0}, NULL, "0"},
    {"DFSParamFromConfig", NULL, {0}, NULL, "0"},
    {"FCCParamCh0", NULL, {0}, NULL, NULL},
    {"FCCParamCh1", NULL, {0}, NULL, NULL},
    {"FCCParamCh2", NULL, {0}, NULL, NULL},
    {"FCCParamCh3", NULL, {0}, NULL, NULL},
    {"CEParamCh0", NULL, {0}, NULL, NULL},
    {"CEParamCh1", NULL, {0}, NULL, NULL},
    {"CEParamCh2", NULL, {0}, NULL, NULL},
    {"CEParamCh3", NULL, {0}, NULL, NULL},
    {"JAPParamCh0", NULL, {0}, NULL, NULL},
    {"JAPParamCh1", NULL, {0}, NULL, NULL},
    {"JAPParamCh2", NULL, {0}, NULL, NULL},
    {"JAPParamCh3", NULL, {0}, NULL, NULL},
    {"JAPW53ParamCh0", NULL, {0}, NULL, NULL},
    {"JAPW53ParamCh1", NULL, {0}, NULL, NULL},
    {"JAPW53ParamCh2", NULL, {0}, NULL, NULL},
    {"JAPW53ParamCh3", NULL, {0}, NULL, NULL},
    {"FixDfsLimit", "fixdfslimit", {0}, hooker, "0"},
    {"LongPulseRadarTh", "lpsradarth", {0}, hooker, "0"},
    {"AvgRssiReq", "avgrssireq", {0}, hooker, "0"},
    {"DFS_R66", "dfs_r66", {0}, hooker, "0"},
    {"BlockCh", "blockch", {0}, hooker, NULL},
    {"GreenAP", "greenap", {0}, hooker, "0"},
    {"WapiPsk1", NULL, {0}, NULL, NULL},
    {"WapiPsk2", NULL, {0}, NULL, NULL},
    {"WapiPsk3", NULL, {0}, NULL, NULL},
    {"WapiPsk4", NULL, {0}, NULL, NULL},
    {"WapiPsk5", NULL, {0}, NULL, NULL},
    {"WapiPsk6", NULL, {0}, NULL, NULL},
    {"WapiPsk7", NULL, {0}, NULL, NULL},
    {"WapiPsk8", NULL, {0}, NULL, NULL},
    {"WapiPskType", NULL, {0}, NULL, NULL},
    {"Wapiifname", NULL, {0}, NULL, NULL},
    {"WapiAsCertPath", NULL, {0}, NULL, NULL},
    {"WapiUserCertPath", NULL, {0}, NULL, NULL},
    {"WapiAsIpAddr", NULL, {0}, NULL, NULL},
    {"WapiAsPort", NULL, {0}, NULL, NULL},
    {"RekeyMethod", "rekeymethod", {0}, hooker, "TIME"},
    {"MeshAutoLink", "mesh_autolink", {0}, hooker, "0"},
    {"MeshAuthMode", "mesh_authmode", {0}, hooker, NULL},
    {"MeshEncrypType", "mesh_enctype", {0}, hooker, NULL},
    {"MeshDefaultkey", "mesh_defkey", {0}, hooker, "0"},
    {"MeshWEPKEY", "mesh_wepkey", {0}, hooker, NULL},
    {"MeshWPAKEY", "mesh_wpakey", {0}, hooker, NULL},
    {"MeshId", "mesh_id", {0}, hooker, NULL},
    {"HSCounter", "hscount", {0}, hooker, "0"},
    {"HT_BADecline", "ht_badec", {0}, hooker, "0"},
    {"HT_STBC", "ht_stbc", {0}, hooker, "0"},
    {"HT_LDPC", "ht_ldpc", {0}, hooker, "1"},
    {"HT_TxStream", "ht_txstream", {0}, hooker, "1"},
    {"HT_RxStream", "ht_rxstream", {0}, hooker, "1"},
    {"HT_PROTECT", "ht_protect", {0}, hooker, "1"},
    {"HT_DisallowTKIP", "ht_distkip", {0}, hooker, "0"},
    {"HT_BSSCoexistence", "ht_bsscoexist", {0}, hooker, "0"},
    {"WscConfMode", "wsc_confmode", {0}, hooker, "0"},
    {"WscConfStatus", "wsc_confstatus", {0}, hooker, "2"},
    {"WCNTest", "wcntest", {0}, hooker, "0"},
    {"WdsPhyMode", "wds_phymode", {0}, hooker, NULL},
    {"RADIUS_Acct_Server", "radius_acctserver", {0}, hooker, NULL},
    {"RADIUS_Acct_Port", "radius_acctport", {0}, hooker, "1813"},
    {"RADIUS_Acct_Key", "radius_acctkey", {0}, hooker, NULL},
    {"Ethifname", "ethifname", {0}, hooker, NULL},
    {"session_timeout_interval", "session_intv", {0}, hooker, "0"},
    {"idle_timeout_interval", "idle_intv", {0}, hooker, "0"},
    {"WiFiTest", NULL, {0}, NULL, "0"},
    {"TGnWifiTest", "tgnwifitest", {0}, hooker, "0"},
    {"ApCliEnable", NULL, {0}, NULL, "0"},
    {"ApCliSsid", NULL, {0}, NULL, NULL},
    {"ApCliBssid", NULL, {0}, NULL, NULL},
    {"ApCliAuthMode", NULL, {0}, NULL, NULL},
    {"ApCliEncrypType", NULL, {0}, NULL, NULL},
    {"ApCliWPAPSK", NULL, {0}, NULL, NULL},
    {"ApCliDefaultKeyID", NULL, {0}, NULL, "0"},
    {"ApCliKey1Type", NULL, {0}, NULL, "0"},
    {"ApCliKey1Str", NULL, {0}, NULL, NULL},
    {"ApCliKey2Type", NULL, {0}, NULL, "0"},
    {"ApCliKey2Str", NULL, {0}, NULL, NULL},
    {"ApCliKey3Type", NULL, {0}, NULL, "0"},
    {"ApCliKey3Str", NULL, {0}, NULL, NULL},
    {"ApCliKey4Type", NULL, {0}, NULL, "0"},
    {"ApCliKey4Str", NULL, {0}, NULL, NULL},
    {"EfuseBufferMode", "efusebufmode", {0}, hooker, "0"},
    {"E2pAccessMode", "e2paccmode", {0}, hooker, "2"},
    {"RadioOn", "radio", {0}, hooker, "1"},
    {"BW_Enable", "bw_enable", {0}, hooker, "0"},
    {"BW_Root", "bw_root", {0}, hooker, "0"},
    {"BW_Priority", "bw_priority", {0}, hooker, NULL},
    {"BW_Guarantee_Rate", "bw_grtrate", {0}, hooker, NULL},
    {"BW_Maximum_Rate", "bw_maxrate", {0}, hooker, NULL},
    /* add more configurations */
    {"AutoChannelSkipList", "autoch_skip", {0}, hooker, NULL},
    {"WscConfMethod", "wsc_confmethod", {0}, hooker, NULL},
    {"WscKeyASCII", "wsc_keyascii", {0}, hooker, NULL},
    {"WscSecurityMode", "wsc_security", {0}, hooker, NULL},
    {"Wsc4digitPinCode", "wsc_4digitpin", {0}, hooker, NULL},
    {"WscVendorPinCode", "wsc_vendorpin", {0}, hooker, NULL},
    {"WscV2Support", "wsc_v2", {0}, hooker, NULL},
    {"HT_MIMOPS", "mimops", {0}, hooker, "3"},
    {"IgmpSnEnable", NULL, {0}, NULL, "1"},
};
#endif	

static struct uci_context * uci_ctx;
static struct uci_package * uci_wireless;
static wifi_params wifi_cfg[DEVNUM_MAX];
char * __get_value(char * datkey)
{
    int i;
    for(i=0; i<sizeof(CFG_ELEMENTS)/sizeof(CFG_ELEMENTS[0]); i++)
    {
        if(0 == strcmp(datkey, CFG_ELEMENTS[i].dat_key))
            return CFG_ELEMENTS[i].value;
    }
    return NULL;
}
int strmatch(const char * str, const char * pattern)
{
    int i = strlen(str);
    int j = 0;
    if (i != strlen(pattern))
        return -1;
    for(j=0; j<i; j++)
        if(pattern[j] != '?' && pattern[j] != str[j])
            return -1;
    return 0;
}
char * __dump_all(void)
{
    int i, j;
    param * p = NULL;
    for(i=0; i<DEVNUM_MAX; i++)
    {
        if(strlen(wifi_cfg[i].devname) == 0) break;
        printf("%s      %-16s\t%-16s\t%-16s\t%-8s\t%s\n",
               wifi_cfg[i].devname, "[dat-key]", "[uci-key]", "[value]", "[hook]", "[default]");
        for(j=0; j<sizeof(CFG_ELEMENTS)/sizeof(CFG_ELEMENTS[0]); j++)
        {
            p = &wifi_cfg[i].params[j];
            printf("%s %3d. %-16s\t%-16s\t%-16s\t%-8s\t%s\n",
                   wifi_cfg[i].devname, j, p->dat_key, p->uci_key,
                   p->value[0]?p->value:"(null)", p->hook?"Y":"-", p->defvalue);
        }
    }
    return NULL;
}
void parse_dat(char * devname, char * datpath)
{
    FILE * fp = NULL;
    char buffer[128] = {0};
    char k[32] = {0};
    char v[256] = {0};
    int i = 0, n = 0;
    char * p = NULL;
    char * q = NULL;
    wifi_params * cfg = NULL;
    printf("API: %s(%s, %s)\n", __FUNCTION__, devname, datpath);
    for (i=0; i<DEVNUM_MAX; i++)
    {
        if(0 == strcmp(devname, wifi_cfg[i].devname))
            cfg = &wifi_cfg[i];
    }
    if(!cfg)
    {
        printf("%s(), device (%s) not found!\n", __FUNCTION__, devname);
        goto __error;
    }
    fp = fopen(datpath, "rb");
    if(!fp)
    {
        printf("%s() error: %s!\n", __FUNCTION__, strerror(errno));
        goto __error;
    }
    memset(buffer, 0, sizeof(buffer));
    i=0;
    do
    {
        memset(buffer, 0, sizeof(buffer));
        p = fgets(buffer, sizeof(buffer), fp);
        if(!p) break;
        // skip empty lines
        while(*p == ' '|| *p == '\t') p++;
        if(*p == 0 || *p == '\r' || *p == '\n') continue;
        // skipe lines starts with "#"
        if(*p == '#') continue;
        // cut the \r\n tail!
        q = strchr(buffer, '\n');
        if(q) *q = 0;
        q = strchr(buffer, '\r');
        if(q) *q = 0;
        q = strstr(buffer, "=");
        if(!q) continue; // a valid line should contain "="
        UCI2DAT_DEBUG(MSG_INFO,"%3d.\"%s\", ", i, buffer);
        i++;
        *q = 0;
        q++; // split it!
        strncpy(k, p, sizeof(k)-1);
        strncpy(v, q, sizeof(v)-1);
        for ( n=0; n<sizeof(CFG_ELEMENTS)/sizeof(CFG_ELEMENTS[0]); n++)
        {
            if(0 == strcmp(CFG_ELEMENTS[n].dat_key, k))
            {
                strncpy(cfg->params[n].value, v, sizeof(CFG_ELEMENTS[n].value)-1);
                break;
            }
        }
        if (n >= sizeof(CFG_ELEMENTS)/sizeof(CFG_ELEMENTS[0]))
            UCI2DAT_DEBUG(MSG_INFO,"!!! <%s> not supported\n", k);
        else
        {
            UCI2DAT_DEBUG(MSG_INFO,"\n _%s():%d <%s>=<%s> uci2dat_debug_level :<%d>\n", __FUNCTION__, __LINE__,k, v,uci2dat_debug_level);
        }
    }
    while(1);
#if 0
    /* dump and check */
    for ( n=0; n<sizeof(CFG_ELEMENTS)/sizeof(CFG_ELEMENTS[0]); n++)
    {
        if(strlen(cfg->params[n].value)>0)
            printf("inited: <%s>=<%s>\n", CFG_ELEMENTS[n].dat_key, cfg->params[n].value);
        else
            printf("empty : <%s>=<%s>\n", CFG_ELEMENTS[n].dat_key, cfg->params[n].value);
    }
#endif
__error:
    if(fp) fclose(fp);
    return;
}
void parse_uci(char * arg)
{
    struct uci_element *e   = NULL;
    const char * value = NULL;
    int i = 0;
    int cur_dev, cur_vif;
	int len = 0;
    UCI2DAT_DEBUG(MSG_INFO,"API: %s(%s)\n", __FUNCTION__, arg);
    if (!uci_wireless || !uci_ctx)
    {
        printf("%s() uci context not ready!\n", __FUNCTION__);
        return;
    }
    /* scan wireless devices ! */
    uci_foreach_element(&uci_wireless->sections, e)
    {
        struct uci_section *s = uci_to_section(e);
        if(0 == strcmp(s->type, "wifi-device"))
        {
            printf("%s(), wifi-device: %s\n", __FUNCTION__, s->e.name);
            for(cur_dev=0; cur_dev<DEVNUM_MAX; cur_dev++)
            {
                if(0 == strcmp(s->e.name, wifi_cfg[cur_dev].devname))
                    break;
            }
            if(cur_dev>=DEVNUM_MAX)
            {
                printf("%s(), device (%s) not found!\n", __FUNCTION__, s->e.name);
                break;
            }
            for( i = 0; i<sizeof(CFG_ELEMENTS)/sizeof(CFG_ELEMENTS[0]); i++)
            {
                if (CFG_ELEMENTS[i].uci_key)
                {
                    value = NULL;
                    value = uci_lookup_option_string(uci_ctx, s, CFG_ELEMENTS[i].uci_key);
                    if(value)
                    {

						if (0 == strcmp(CFG_ELEMENTS[i].uci_key, "macaddr") && !is_valid_mac(value)) 
							continue;

                        strncpy(wifi_cfg[cur_dev].params[i].value,
                                value, sizeof(CFG_ELEMENTS[i].value)-1);
                        UCI2DAT_DEBUG(MSG_INFO,"%s(),    %s=%s\n", __FUNCTION__, CFG_ELEMENTS[i].uci_key, value);
                    }
					/* when /etc/config/wireless is not exist macaddr, init macaddr value */
					else if (0 == strcmp(CFG_ELEMENTS[i].uci_key, "macaddr"))
					{
						memset(wifi_cfg[cur_dev].params[i].value, 0, sizeof(wifi_cfg[cur_dev].params[i].value));
					}
                }
            }
        }
    }
    /* scan wireless network interfaces ! */
    uci_foreach_element(&uci_wireless->sections, e)
    {
        struct uci_section *s = uci_to_section(e);
        if(0 == strcmp(s->type, "wifi-iface"))
        {
            value = NULL;
            value = uci_lookup_option_string(uci_ctx, s, "device");
            if(value == NULL)
            {
                continue;
            }
            for(cur_dev=0; cur_dev<DEVNUM_MAX; cur_dev++)
            {
                if(0 == strcmp(value, wifi_cfg[cur_dev].devname))
                    break;
            }
            if(cur_dev >= DEVNUM_MAX)
            {
                printf("%s(), device (%s) not found!\n", __FUNCTION__, value);
                break;
            }
            value = NULL;
            value = uci_lookup_option_string(uci_ctx, s, "ifname");
            UCI2DAT_DEBUG(MSG_INFO,"%s(), wifi-iface: %s\n", __FUNCTION__, value);
            cur_vif = wifi_cfg[cur_dev].vifnum;
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].ssid, value);
	    	PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].wmm, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].hidden, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].maxassoc, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].key, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].wepkey[0], value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].wepkey[1], value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].wepkey[2], value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].wepkey[3], value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].auth_server, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].auth_port, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].auth_secret, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].rekeyinteval, value);
			PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].rekeymethod, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].preauth, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].pmkcacheperiod, value);

            /*add*/
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].disabled, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].macaddr, value);

			PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].ieee80211w, value);
			PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].wsc_v2, value);
#if 0
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].authmode, value);
            PARSE_UCI_OPTION(wifi_cfg[cur_dev].vifs[cur_vif].cipher, value);
#else
#define STRNCPPP(dst,src) \
                do {\
                    strncpy(dst.value, src, sizeof(dst.value)-1); \
                    UCI2DAT_DEBUG(MSG_INFO,"%s(),    %s=%s\n", __FUNCTION__, dst.uci_key, src); \
                } while(0)
            /* cipher */
            value = uci_lookup_option_string(uci_ctx, s, "encryption");
            if(value)
            {
                const char * p = NULL;
                if (0 == strncmp("8021x", value, strlen("8021x")))
                {
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "8021x");
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, "ccmp");
					STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].s8021x, "1");
                }
                if (0 == strncmp("none", value, strlen("none")))
                {
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "none");
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, "NONE");
                }
                else if (0 == strncmp("wep+open", value, strlen("wep+open")))
                {
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "wep+open");
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, "wep");
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].s8021x, "1");
                }
                else if (0 == strncmp("wep+shared", value, strlen("wep+shared")))
                {
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "wep+shared");
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, "wep");
                }
                else if (0 == strncmp("mixed-psk", value, strlen("mixed-psk")))
                {
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "psk-mixed");
                    p = value+strlen("mixed-psk");
                    if (*p == '+' && *(p+1) != 0)
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, p+1);
                    else
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, "ccmp");
                }
                else if(0 == strncmp("psk", value, strlen("psk")))
                {
                    if (0 == strncmp("psk-mixed", value, strlen("psk-mixed")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "psk-mixed");
                        p = value+strlen("psk-mixed");
                    }
                    else if (0 == strncmp("psk+psk2", value, strlen("psk+psk2")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "psk-mixed");
                        p = value+strlen("psk+psk2");
                    }
                    else if (0 == strncmp("psk2+psk3", value, strlen("psk2+psk3")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "psk2+psk3");
                        p = value+strlen("psk2+psk3");
                    }
                    else if (0 == strncmp("psk3", value, strlen("psk3")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "psk3");
                        p = value+strlen("psk3");
                    }
                    else if (0 == strncmp("psk2", value, strlen("psk2")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "psk2");
                        p = value+strlen("psk2");
                    }
                    else if (0 == strncmp("psk", value, strlen("psk")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "psk");
                        p = value+strlen("psk");
                    }
                    if (*p == '+' && *(p+1) != 0)
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, p+1);
                    else
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, "ccmp");
                }
                else if(0 == strncmp("wpa", value, strlen("wpa")))
                {
                    STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].s8021x, "1");
                    if (0 == strncmp("wpa-mixed", value, strlen("wpa-mixed")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "wpa-mixed");
                        p = value+strlen("wpa-mixed");
                    }
                    else if (0 == strncmp("wpa+wpa2", value, strlen("wpa+wpa2")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "wpa-mixed");
                        p = value+strlen("wpa+wpa2");
                    }
                    else if (0 == strncmp("wpa2", value, strlen("wpa2")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "wpa2");
                        p = value+strlen("wpa2");
                    }
                    else if (0 == strncmp("wpa", value, strlen("wpa")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "wpa");
                        p = value+strlen("wpa");
                    }
                    if (*p == '+' && *(p+1) != 0)
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, p+1);
                    else
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, "ccmp");
                }
				else if(0 == strncmp("sae", value, strlen("sae")))
                {
					if (0 == strncmp("sae-mixed", value, strlen("sae-mixed")))
                    {
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "sae-mixed");
                        p = value+strlen("sae-mixed");
                    }
					else if (0 == strncmp("sae", value, strlen("sae")))
					{
						STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].authmode, "sae");
					   	p = value+strlen("sae");
					}
					if (*p == '+' && *(p+1) != 0)
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, p+1);
                    else
                        STRNCPPP(wifi_cfg[cur_dev].vifs[cur_vif].cipher, "ccmp");

				}
            }
            /* key */
#endif
            wifi_cfg[cur_dev].vifnum++;
        }
    }
    return;
}
void hooker(FILE * fp, param * p, const char * devname)
{
    int N = 0;
    int i = 0;
	int len = 0;
    //printf("API: %s(%s)\n", __FUNCTION__, p->element);
    if (!uci_wireless || !uci_ctx)
    {
        printf("%s() uci context not ready!\n", __FUNCTION__);
        return;
    }
    for(N=0; N<MBSSID_MAX; N++)
    {
        if(0 == strcmp(devname, wifi_cfg[N].devname))
            break;
    }
    if(N >= MBSSID_MAX)
    {
        printf("%s() device (%s) not found!\n", __FUNCTION__, devname);
        return;
    }
    if(0 == strmatch(p->dat_key, "SSID?"))
    {
        i = atoi(p->dat_key+4)-1;
        if(i<0 || i >= MBSSID_MAX)
        {
            printf("%s() array index error, L%d\n", __FUNCTION__, __LINE__);
            return;
        }
        FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].ssid.value);
    }
 /*  it should be radio paramter
	else if(0 == strcmp(p->dat_key, "BssidNum"))
    {
        FPRINT(fp, p, "%d", wifi_cfg[N].vifnum);
    }
*/	
    else if(0 == strcmp(p->dat_key, "EncrypType"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "none"))
                FPRINT(fp, p, "NONE");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wep+open")
                     || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wep+shared")
                     || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "8021x"))
                FPRINT(fp, p, "WEP");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].cipher.value, "ccmp"))
                FPRINT(fp, p, "AES");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].cipher.value, "tkip"))
                FPRINT(fp, p, "TKIP");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].cipher.value, "ccmp+tkip")
                     || 0 == strcasecmp(wifi_cfg[N].vifs[i].cipher.value, "tkip+ccmp"))
                FPRINT(fp, p, "TKIPAES");
            else
                FPRINT(fp, p, "NONE");
        }
    }
    else if(0 == strcmp(p->dat_key, "AuthMode"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "none"))
                FPRINT(fp, p, "OPEN");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wep+open"))
                FPRINT(fp, p, "OPEN");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "8021x"))
                FPRINT(fp, p, "OPEN");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wep+shared"))
                FPRINT(fp, p, "SHARED");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wep-auto"))
                FPRINT(fp, p, "WEPAUTO");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk"))
                FPRINT(fp, p, "WPAPSK");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk2"))
                FPRINT(fp, p, "WPA2PSK");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk-mixed")
                     || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk+psk2"))
                FPRINT(fp, p, "WPAPSKWPA2PSK");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk3"))
                FPRINT(fp, p, "WPA3PSK");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk2+psk3"))
                FPRINT(fp, p, "WPA2PSKWPA3PSK");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wpa"))
                FPRINT(fp, p, "WPA");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wpa2"))
                FPRINT(fp, p, "WPA2");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wpa-mixed")
                     || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wpa+wpa2"))
                FPRINT(fp, p, "WPA1WPA2");
			else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "sae"))
                FPRINT(fp, p, "WPA3PSK");
            else if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "sae-mixed"))
                FPRINT(fp, p, "WPA2PSKWPA3PSK");
            else
                FPRINT(fp, p, "OPEN");
        }
    }
    else if(0 == strcmp(p->dat_key, "HideSSID"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, wifi_cfg[N].vifs[i].hidden.value[0]=='1'?"1":"0");
        }
    }
    else if(0 == strcmp(p->dat_key, "WmmCapable"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, wifi_cfg[N].vifs[i].wmm.value[0]=='1'?"1":"0");
        }
    }
    else if(0 == strcmp(p->dat_key, "MaxStaNum"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].maxassoc.value);
        }
    }
    else if(0 == strcmp(p->dat_key, "HideSSID"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, wifi_cfg[N].vifs[i].hidden.value[0]=='1'?"1":"0");
        }
    }
    else if(0 == strcmp(p->dat_key, "EnableSSID"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, wifi_cfg[N].vifs[i].disabled.value[0]=='1'?"0":"1");
        }
    }	
    else if(0 == strcmp(p->dat_key, "MbssMaxStaNum"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].maxassoc.value);
        }
    }
     else if(0 == strcmp(p->dat_key, "MacAddress"))
    {   
		if(p->value[0] == '\0')
		{			
			FILE* macInfo=popen("ifconfig br-lan | grep HWaddr | awk '{ print $5 }'","r");

			if(!macInfo)
			{
				printf("%s,can not get br-lan mac info!\n",__FUNCTION__);
				return;
			}
			char mac_address[32];

			memset(mac_address, 0, sizeof(mac_address));
			if(fgets(mac_address,sizeof(mac_address),macInfo)!=NULL)
			{
				int mac_len = 17;
				mac_address[mac_len] = '\0';
				if(band == 0)
				{
					mac_address[mac_len-1]=mac_address[mac_len-1]+1;
				}
				else
				{					
					mac_address[mac_len-2]=mac_address[mac_len-2]+1;
				}
			}
			strncpy(p->value, mac_address, sizeof(mac_address)-1);
			pclose(macInfo);

		}

        FPRINT(fp, p, "%s", p->value);			

        //FPRINT(fp, p, "%s", p->value);
    }
    else if(0 == strcmp(p->dat_key, "Channel"))
    {
        if(0 == strcmp(p->value, "auto"))
            FPRINT(fp, p, "0");
        else
            FPRINT(fp, p, "%s", p->value);
    }
    else if(0 == strcmp(p->dat_key, "AutoChannelSelect"))
    {
        if(0 == strcmp(p->value, "0"))
            FPRINT(fp, p, "3");
        else
            FPRINT(fp, p, "0");
    }
    else if(0 == strcmp(p->dat_key, "HT_BW"))
    {
#if 0
        if(0 == strcmp(p->value, "20"))
        {
            FPRINT(fp, p, "0");
        }
        else if( (0 == strcmp(p->value, "40")) || (0 == strcmp(p->value, "80")) || (0 == strcmp(p->value, "160")) )
        {
            FPRINT(fp, p, "1");
        }         
        else
            FPRINT(fp, p, "0");
#else
		if( (0 == strcmp(p->value, "NOHT")) || (0 == strcmp(p->value, "HT20")) 
			|| (0 == strcmp(p->value, "VHT20")) || (0 == strcmp(p->value, "HE20")) )
        {
            FPRINT(fp, p, "0");
        }
        else if( (0 == strcmp(p->value, "HT40")) || (0 == strcmp(p->value, "VHT40")) 
			|| (0 == strcmp(p->value, "VHT80")) || (0 == strcmp(p->value, "VHT80_80")) 
			|| (0 == strcmp(p->value, "VHT160")) || (0 == strcmp(p->value, "HE40")) 
			|| (0 == strcmp(p->value, "HE80")) || (0 == strcmp(p->value, "HE160")) )
        {
            FPRINT(fp, p, "1");
        }         
        else
            FPRINT(fp, p, "0");
#endif
    }
    else if(0 == strcmp(p->dat_key, "VHT_BW"))
    {
#if 0
        if( (0 == strcmp(p->value, "20")) || (0 == strcmp(p->value, "40")) )
        {
            FPRINT(fp, p, "0");
        }
        else if( 0 == strcmp(p->value, "80") )
        {
            FPRINT(fp, p, "1");
        }
        else if( 0 == strcmp(p->value, "160") )
        {
            FPRINT(fp, p, "2");
        }
        else
            FPRINT(fp, p, "0");
#else
		if( (0 == strcmp(p->value, "NOHT")) || (0 == strcmp(p->value, "HT20")) || (0 == strcmp(p->value, "HT40"))
			|| (0 == strcmp(p->value, "VHT20")) || (0 == strcmp(p->value, "VHT40")) )
        {
            FPRINT(fp, p, "0");
        }
        else if( (0 == strcmp(p->value, "VHT80")) || (0 == strcmp(p->value, "HE80")) )
        {
            FPRINT(fp, p, "1");
        }
		else if( (0 == strcmp(p->value, "VHT160")) || (0 == strcmp(p->value, "HE160")) )
        {
            FPRINT(fp, p, "2");
        }
		else if( 0 == strcmp(p->value, "VHT80_80") )
        {
            FPRINT(fp, p, "3");
        }
        else
            FPRINT(fp, p, "0");
#endif
    }
	else if(0 == strcmp(p->dat_key, "WirelessMode"))
    {
    	if(band==0)
    	{
			if(0 == strcmp(p->value, "NOHT"))
	        {
				struct uci_element *e	= NULL;
			
				uci_foreach_element(&uci_wireless->sections, e)
				{
					struct uci_section *s	= uci_to_section(e);
					struct uci_element *ee	= NULL;
					struct uci_option  *o	= NULL;
					uci_foreach_element(&s->options, ee)
					{
						o = uci_to_option(ee);
						if ( (0 == strcmp(ee->name, "hwmode")) && (0 == strcmp(o->v.string, "11b")) )
						{
							FPRINT(fp, p, "1");
							return;
						}
					}
				}
	            FPRINT(fp, p, "4");
	        }
	        else if( (0 == strcmp(p->value, "HT20")) || (0 == strcmp(p->value, "HT40")) )
	        {
	            FPRINT(fp, p, "9");
	        }
			else if( (0 == strcmp(p->value, "VHT20")) || (0 == strcmp(p->value, "VHT40")) )
	        {
	            FPRINT(fp, p, "12");
	        }
			else if( (0 == strcmp(p->value, "HE20")) || (0 == strcmp(p->value, "HE40")) )
	        {
	            FPRINT(fp, p, "16");
	        }
	        else
	            FPRINT(fp, p, "9");

    	}
		else if(band==1)
		{
			if(0 == strcmp(p->value, "NOHT"))
	        {
	            FPRINT(fp, p, "2");
	        }
	        else if( (0 == strcmp(p->value, "HT20")) || (0 == strcmp(p->value, "HT40")) )
	        {
	            FPRINT(fp, p, "8");
	        }
			else if( (0 == strcmp(p->value, "VHT20")) || (0 == strcmp(p->value, "VHT40")) 
				|| (0 == strcmp(p->value, "VHT80")) || (0 == strcmp(p->value, "VHT80_80")) 
				|| (0 == strcmp(p->value, "VHT160")) )
	        {
	            FPRINT(fp, p, "14");
	        }
			else if( (0 == strcmp(p->value, "HE20")) || (0 == strcmp(p->value, "HE40")) 
				|| (0 == strcmp(p->value, "HE80")) || (0 == strcmp(p->value, "HE160")) )
	        {
	            FPRINT(fp, p, "17");
	        }
	        else
	            FPRINT(fp, p, "14");
		}
		else 
			FPRINT(fp, p, "9");
    }
    else if(0 == strcmp(p->dat_key, "IEEE80211H"))
    {
        FPRINT(fp, p, "%s", p->value);      
    }
    else if(0 == strcmp(p->dat_key, "PMFMFPC"))
    {
		for(i=0; i<wifi_cfg[N].vifnum; i++)
		{
			if(i>0) FPRINT(fp, p, ";");
			if(0 == strcmp(wifi_cfg[N].vifs[i].ieee80211w.value, "0"))
	        {
	            FPRINT(fp, p, "0");
	        }
	        else if( (0 == strcmp(wifi_cfg[N].vifs[i].ieee80211w.value, "1")) || (0 == strcmp(wifi_cfg[N].vifs[i].ieee80211w.value, "2")) )
	        {
	            FPRINT(fp, p, "1");
	        }			
	        else
	        {
	            FPRINT(fp, p, "0");
	        }
		}

    }
    else if(0 == strcmp(p->dat_key, "PMFMFPR"))
    {             				
		for(i=0; i<wifi_cfg[N].vifnum; i++)
		{
			if(i>0) FPRINT(fp, p, ";");
			if( (0 == strcmp(wifi_cfg[N].vifs[i].ieee80211w.value, "0")) || (0 == strcmp(wifi_cfg[N].vifs[i].ieee80211w.value, "1")) )
			{
				FPRINT(fp, p, "0");
			}
			else if(0 == strcmp(wifi_cfg[N].vifs[i].ieee80211w.value, "2") )
			{
				FPRINT(fp, p, "1");
			}			
			else
			{
				FPRINT(fp, p, "0");
			}  
		}

    }	
    else if(0 == strmatch(p->dat_key, "WPAPSK?"))//(0 == strncmp(p->dat_key, "WPAPSK", 6))
    {
        i = atoi(p->dat_key+6)-1;
        if(i<0 || i >= MBSSID_MAX)
        {
            printf("%s() array index error, L%d\n", __FUNCTION__, __LINE__);
            return;
        }
        if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk")
            || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk2")
            || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk3")
            || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk+psk2")
            || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk2+psk3")
            || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "psk-mixed")
            || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "sae")
            || 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "sae-mixed")) {
            len = strlen(wifi_cfg[N].vifs[i].key.value);
            if( (len >=8 && len <= 63) || (len == 64 && is_hex_string(wifi_cfg[N].vifs[i].key.value)) ) 
            	FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].key.value);
			else {
				UCI2DAT_DEBUG(MSG_WARNING, "key:%s is invalid!", wifi_cfg[N].vifs[i].key.value);
				FPRINT(fp, p, "%s", p->value);
			}
		}
    }
    else if(0 == strcmp(p->dat_key, "RADIUS_Server"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if( i > 0 ) FPRINT(fp, p, ";");
            if (0 == strcmp(wifi_cfg[N].vifs[i].s8021x.value, "1"))
                FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].auth_server.value);
        }
    }
    else if(0 == strcmp(p->dat_key, "RADIUS_Port"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            if (0 == strcmp(wifi_cfg[N].vifs[i].s8021x.value, "1"))
                FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].auth_port.value);
        }
    }
    else if(0 == strmatch(p->dat_key, "RADIUS_Key?"))//(0 == strncmp(p->dat_key, "WPAPSK", 6))
    {
        i = atoi(p->dat_key+10)-1;
        if(i<0 || i >= MBSSID_MAX)
        {
            printf("%s() array index error, L%d\n", __FUNCTION__, __LINE__);
            return;
        }
        if (0 == strcmp(wifi_cfg[N].vifs[i].s8021x.value, "1"))
            FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].auth_secret.value);
    }
    else if(0 == strcmp(p->dat_key, "PreAuth"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].preauth.value);
        }
    }
    else if(0 == strcmp(p->dat_key, "RekeyInterval"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].rekeyinteval.value);
        }
    }
    else if(0 == strcmp(p->dat_key, "PMKCachePeriod"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].pmkcacheperiod.value);
        }
    }
    else if(0 == strcmp(p->dat_key, "RekeyMethod"))
    {
    	for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].rekeymethod.value);
        }
    }
    else if(0 == strcmp(p->dat_key, "IEEE8021X"))
    {
        for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
            if(0 == strcasecmp(wifi_cfg[N].vifs[i].s8021x.value, "1"))
                FPRINT(fp, p, "%s", "1");
            else
                FPRINT(fp, p, "%s", "0");
        }
    }
    else if(0 == strmatch(p->dat_key, "DefaultKeyID"))  /* WEP */
    {
    	for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
	    	if (0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wep+open")
				|| 0 == strcasecmp(wifi_cfg[N].vifs[i].authmode.value, "wep+shared"))
			{
		            FPRINT(fp, p, "%s;", wifi_cfg[N].vifs[i].key.value);
			}
			else 
				FPRINT(fp, p, "%s;", "1");//default value
		}
    }
#if 0
    else if(0 == strmatch(p->dat_key, "Key?Type"))  /* WEP */
    {
        /* TODO: do we need to support byte sequence?  */
        FPRINT(fp, p, "%s", "0");
    }
#endif
    else if(0 == strmatch(p->dat_key, "Key?Type"))  /* WEP */
    {
	int j;
	i = atoi(p->dat_key+3)-1;
	for(j=0; j<wifi_cfg[N].vifnum; j++)
	{
		if(0 == strncmp("s:", wifi_cfg[N].vifs[j].wepkey[i].value, 2))
			strcpy(wifi_cfg[N].vifs[j].wepkey[i].value,wifi_cfg[N].vifs[j].wepkey[i].value+2);
		if( 5 == strlen(wifi_cfg[N].vifs[j].wepkey[i].value) || 13 == strlen(wifi_cfg[N].vifs[j].wepkey[i].value))
			FPRINT(fp, p, "%s;", "1");//wep key type is asic
		else
			FPRINT(fp, p, "%s;", "0");//wep key type is hex
	}
    }
    else if(0 == strmatch(p->dat_key, "Key?Str?"))  /* WEP */
    {
        int j;
        i = atoi(p->dat_key+3)-1;
        j = atoi(p->dat_key+7)-1;
        if(0 == strncmp("s:", wifi_cfg[N].vifs[j].wepkey[i].value, 2))
            FPRINT(fp, p, "%s", wifi_cfg[N].vifs[j].wepkey[i].value+2);
        else
            FPRINT(fp, p, "%s", wifi_cfg[N].vifs[j].wepkey[i].value);
    }
    else if(0 == strcmp(p->dat_key, "HT_BSSCoexistence"))
    {
        FPRINT(fp, p, p->value[0]=='1'?"0":"1");
    }
	else if(0 == strcmp(p->dat_key, "WscV2Support"))
    {
		for(i=0; i<wifi_cfg[N].vifnum; i++)
        {
            if(i>0) FPRINT(fp, p, ";");
           FPRINT(fp, p, "%s", wifi_cfg[N].vifs[i].wsc_v2.value);
        }   
    }
	
#if 0
    else if(0 == strmatch(p->dat_key, "ApCliKey?Type"))  /* Ap Client Mode */
    {
        i = atoi(p->dat_key+8)-1;
        /* TODO */
    }
    else if(0 == strmatch(p->dat_key, "ApCliKey?Str"))  /* Ap Client Mode */
    {
        i = atoi(p->dat_key+8)-1;
        /* TODO */
    }
    else if(0 == strmatch(p->dat_key, "Wds?Key"))  /* Ap Client Mode */
    {
        i = atoi(p->dat_key+3)-1;
        /* TODO */
    }
    else if(0 == strmatch(p->dat_key, "RADIUS_Key?"))  /* Radius */
    {
        i = atoi(p->dat_key+10)-1;
        /* TODO */
    }
#endif
    /* the rest part is quite simple! */
    else
    {
        FPRINT(fp, p, "%s", p->value);
    }
}
void gen_dat(char * devname, char * datpath)
{
    FILE       * fp = NULL;
    char buffer[64] = {0};
    int           i = 0;
    param       * p = NULL;
    wifi_params * cfg = NULL;
    printf("API: %s(%s, %s)\n", __FUNCTION__, devname, datpath);
    for (i=0; i<DEVNUM_MAX; i++)
    {
        if(0 == strcmp(devname, wifi_cfg[i].devname))
            cfg = &wifi_cfg[i];
    }
    if(!cfg)
    {
        printf("%s(), device (%s) not found!\n", __FUNCTION__, devname);
        return;
    }
    if (datpath)
    {
        fp = fopen(datpath, "wb");
    }
    else
    {
        snprintf(buffer, sizeof(buffer), "mkdir -p /etc/wireless/%s", cfg->devname);
        system(buffer);
        snprintf(buffer, sizeof(buffer), "/etc/wireless/%s/%s.dat", cfg->devname, cfg->devname);
        // snprintf(buffer, sizeof(buffer), "/%s.dat", cfg->devname); //test only
        fp = fopen(buffer, "wb");
    }
    if(!fp)
    {
        printf("Failed to open %s, %s!\n", buffer, strerror(errno));
        return;
    }
    fprintf(fp, "# Generated by uci2dat\n");
    fprintf(fp, "# The word of \"Default\" must not be removed\n");
    fprintf(fp, "Default\n");
    for(i=0; i<sizeof(CFG_ELEMENTS)/sizeof(CFG_ELEMENTS[0]); i++)
    {
        p = &cfg->params[i];
        UCI2DAT_DEBUG(MSG_INFO,"%s(), %s\n", __FUNCTION__, p->dat_key);
        fprintf(fp, "%s=", p->dat_key);
        if(p->hook)
            p->hook(fp, p, cfg->devname);
        else if(strlen(p->value) > 0)
            fprintf(fp, "%s", p->value);
        else if(p->defvalue)
            fprintf(fp, "%s", p->defvalue);
        fprintf(fp, "\n");
    }
    fclose(fp);
    return;
}
void init_wifi_cfg(void)
{
    struct uci_element *e   = NULL;
    int i,j;
    for(i=0; i<DEVNUM_MAX; i++)
    {
        memset(&wifi_cfg[i], 0, sizeof(wifi_params));
        wifi_cfg[i].params = (param *)malloc(sizeof(CFG_ELEMENTS));
        memcpy(wifi_cfg[i].params, CFG_ELEMENTS, sizeof(CFG_ELEMENTS));
        for(j=0; j<MBSSID_MAX; j++)
            memcpy(&wifi_cfg[i].vifs[j], &VIF, sizeof(VIF));
    }
    uci_foreach_element(&uci_wireless->sections, e)
    {
        struct uci_section *s = uci_to_section(e);
        if(0 == strcmp(s->type, "wifi-device"))
        {
            for(i=0; i<DEVNUM_MAX; i++)
            {
                if(0 == strlen(wifi_cfg[i].devname))
                {
                    strncpy(wifi_cfg[i].devname, s->e.name, sizeof(wifi_cfg[i].devname)-1);
                    break;
                }
            }
            if(i>=DEVNUM_MAX)
            {
                printf("%s(), too many devices!\n", __FUNCTION__);
                break;
            }
        }
    }
}
void usage(void)
{
    int i, j;
    param * p = NULL;
    printf("uci2dat  -- a tool to translate uci config (/etc/config/wireless)\n");
    printf("            into ralink driver dat.\n");
    printf("\nUsage:\n");
    printf("    uci2dat -d <dev-name> -f <dat-path>\n");
    printf("\nArguments:\n");
    printf("    -d <dev-name>   device name, mt7620, eg.\n");
    printf("    -f <dat-path>   dat file path.\n");
    printf("\nSupported keywords:\n");
    printf("     %-16s\t%-16s\t%s\n", "[uci-key]", "[dat-key]", "[default]");
    for(i=0, j=0; i<sizeof(CFG_ELEMENTS)/sizeof(CFG_ELEMENTS[0]); i++)
    {
        p = &CFG_ELEMENTS[i];
        if(p->uci_key)
        {
            printf("%3d. %-16s\t%-16s\t%s\n",j, p->uci_key, p->dat_key, p->defvalue);
            j++;
        }
    }
    printf("%2d. %s\n", j++, VIF.ssid.uci_key);
    printf("%2d. %s\n", j++, VIF.authmode.uci_key);
    printf("%2d. %s\n", j++, VIF.hidden.uci_key);
    printf("%2d. %s\n", j++, VIF.cipher.uci_key);
    printf("%2d. %s\n", j++, VIF.key.uci_key);
}
int main(int argc, char ** argv)
{
    int opt = 0;
    char * dev = NULL;
    char * dat = NULL;
    //char olddat[128] = {0};
    int test = 0;
    char * tmp_debug_level = NULL;
    while ((opt = getopt(argc, argv, "htf:d:e:")) != -1)
    {
        switch (opt)
        {
            case 'f':
                dat = optarg;
                printf("---- datpath=\"%s\"\n", dat);
				if (0 == strncmp(WLAN_DAT_PATH, dat, strlen(dat)))
					band=0;
				else if (0 == strncmp(WLAN_AC_DAT_PATH, dat, strlen(dat)))
					band=1;
				else
					band=-1;
                break;
            case 'd':
                dev = optarg;
                printf("---- devname=\"%s\"\n", dev);
                break;
            case 't':
                test = 1;
                printf("---- TEST MODE ----\n");
                break;
            case 'h':
                usage();
                return OK;
            case 'e':
                tmp_debug_level = optarg;
                UCI2DAT_DEBUG(MSG_INFO,"---- level=\"%s, %d\"\n", tmp_debug_level, atoi(tmp_debug_level) );
                set_debug_level( atoi(tmp_debug_level) );
                UCI2DAT_DEBUG(MSG_INFO,"---- level=\"%d\"\n", atoi(tmp_debug_level));
                break;
            default:
                usage();
                return NG;
        }
    }
    if (!uci_ctx)
    {
        uci_ctx = uci_alloc_context();
    }
    else
    {
        uci_wireless = uci_lookup_package(uci_ctx, "wireless");
        if (uci_wireless)
            uci_unload(uci_ctx, uci_wireless);
    }
    if (uci_load(uci_ctx, "wireless", &uci_wireless))
    {
        return NG;
    }
#if 0
    uci_foreach_element(&uci_wireless->sections, e)
    {
        struct uci_section *s   = uci_to_section(e);
        struct uci_element *ee  = NULL;
        struct uci_option  *o   = NULL;
        printf("%s() === %s\n", __FUNCTION__, s->type);
        uci_foreach_element(&s->options, ee)
        {
            o = uci_to_option(ee);
            printf("%s() : <%s>=<%s>\n", __FUNCTION__, ee->name, o->v.string);
        }
    }
#endif
    init_wifi_cfg();
    if(dev && dat)
    {
        //snprintf(olddat, sizeof(olddat), "/rom%s", dat);
        parse_dat(dev, dat);
        parse_uci(NULL);
    }
    if(test)
    {
        FILE * fp = NULL;
        char * p = NULL;
        char device[32] = {0};
        char profile[64] = {0};
        fp = popen("cat /etc/config/wireless"
                   " | grep \"wifi-device\""
                   " | sed  -n \"s/config[ \t]*wifi-device[\t]*//gp\"",
                   "r");
        if(!fp)
        {
            printf("%s(), error L%d\n", __FUNCTION__, __LINE__);
            return NG;
        }
        while(fgets(device, sizeof(device), fp))
        {
            if (strlen(device) > 0)
            {
                p = device+strlen(device)-1;
                while(*p < ' ')
                {
                    *p=0;    // trim newline
                    p++;
                }
                snprintf(profile, sizeof(profile), "/etc/wireless/%s/%s.dat", device, device);
                parse_dat(device, profile);
            }
            else
                printf("%s(), error L%d\n", __FUNCTION__, __LINE__);
        }
        pclose(fp);
        parse_uci(NULL);
        __dump_all();
    }
    else if(dev && dat)
        gen_dat(dev, dat);
    else
        usage();
    uci_unload(uci_ctx, uci_wireless);
    return OK;
}
