#ifndef WIFI_COMM_HEAD
#define WIFI_COMM_HEAD
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#include <uapi/modules/wifi_global_def.h>
#endif
#define EEPROM_READFLASH_SIZE       512
#define EEPROM_READFLASH_AC_SIZE    1024  

#define MAX_PATH_LEN        		512
#define MAX_KEYWORD_LEN    		    (ATTR_VALUE_SIZE+10)
#define MAX_PARA_NUM   		 		6

#define BUFFER_ITEM_SIZE 			(ATTR_NAME_SIZE+ATTR_VALUE_SIZE)  
#define BUFFER_ITEM_NUM  			320  
#define ATTR_ITEM_SIZE 				512
#define PRINT_SHM_SIZE				121

#define ATTR_NAME_SIZE 				36
#define ATTR_VALUE_SIZE 			150

#define	MAX_SSID_NUM 				8
#define MAX_WDS_ENTRY               4
#define MAC_ADDR_LEN                6
#define MAX_APCLIENT_NUM			1
#define M_APCLIENT_2G_NAME_FORMAT		"apcli%d"
#define M_APCLIENT_5G_NAME_FORMAT		"apclii%d"



#define CAL_FILE_PATH      		  	"/etc/RT30xxEEPROM.bin"
#define CAL_FILE_TMP_PATH         	"/tmp/RT30xxEEPROM.bin"
#if defined(TCSUPPORT_OPENWRT)
#define INSMOD_MT7603_MODULE_PATH   "insmod /lib/modules/4.4.115/mt7603eap.ko"
#define INSMOD_MT7612_MODULE_PATH   "insmod /lib/modules/4.4.115/mt7662e_ap.ko"
#define INSMOD_MT7613_MODULE_PATH   "insmod /lib/modules/4.4.115/mt7663_ap.ko"
#define INSMOD_MT7615_MODULE_PATH   "insmod /lib/modules/4.4.115/mt7615_ap.ko"
#else
#define INSMOD_MT7603_MODULE_PATH   "insmod /lib/modules/mt7603eap.ko"
#define INSMOD_MT7612_MODULE_PATH   "insmod /lib/modules/mt7662e_ap.ko"
#define INSMOD_MT7613_MODULE_PATH   "insmod /lib/modules/mt7663_ap.ko"
#define INSMOD_MT7615_MODULE_PATH   "insmod /lib/modules/mt7615_ap.ko"
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#define INSMOD_MT7915D_MODULE_PATH   "insmod /lib/modules/mt7915d_ap.ko"
#define INSMOD_MT7915A_MODULE_PATH   "insmod /lib/modules/mt7915a_ap.ko"
#define INSMOD_MT7916D_MODULE_PATH   "insmod /lib/modules/mt7916d_ap.ko"
#endif
#endif

extern char dat_path_2_4g[MAX_PATH_LEN];
extern char dat_path_5g[MAX_PATH_LEN];


#define READ_MODE  				    "r+"  
#define WRITE_MODE  				"w+"

#ifndef GNU_PACKED
#define GNU_PACKED  __attribute__ ((packed))
#endif /* GNU_PACKED */

/***************************************
*the type of return value
***************************************/
typedef enum tag_wifi_ret{
	ECNT_WIFIMGR_SUCCESS = 0,
	ECNT_WIFIMGR_FAILURE,
	KEYWORD_EXIST,
	KEYWORD_NOT_EXIST,
	ECNT_WIFIMGR_CMD_ERR,
	ECNT_WIFIMGR_INTENAL_ERR,
}ECNT_WIFIMGR_RET;

typedef enum tag_wifi_flag{
	ECNT_WIFIMGR_TRUE = 0,
	ECNT_WIFIMGR_FALSE,
}ECNT_WIFIMGR_FLAG;

/***************************************
*choose wifi working in 2.4G or 5G
***************************************/
typedef enum tag_wifi_type{
	WIFI_2_4G = 0,
	WIFI_5G,
	WIFI_MAX
}wifi_type;

#ifdef TCSUPPORT_WLAN_MULTI_CHIP
#define PRINT_MAC(addr)	\
	addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

#ifndef BIT
#define BIT(x) (1U << (x))
#endif
#define BITS(m, n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))

typedef enum _shardcode{
	SHAREDNOTCONFIG = 0,
	SHAREDCONFIGING,
	SHAREDCONFIG,
}shardcode;

typedef enum _failcode{
	WLAN_LIBAPI_CREATE_SHARED_FAIL = -4,
	WLAN_LIBAPI_WRITE_INFO_FAIL = -3,
	//WLAN_LIBAPI_MTD_FAIL = -2,
	WLAN_LIBAPI_OPER_FAIL = -1,	
	WLAN_LIBAPI_SUCCESS = 0,
}failcode;

#define BNADNUM 2

typedef enum _bandcode{
	LIB_BAND0 = 0,
	LIB_BAND1
}bandcode;

typedef enum _e2pcode{
	EEPROM_IPAILNA = 1,
	EEPROM_IPAELNA,
	EEPROM_EPAILNA,
	EEPROM_EPAELNA
}e2pcode;

typedef enum _bwcode{
	BW40 = 1,
	BW80,
	BW160,
	BW160C,
}bwcode;

typedef enum _nsscode{
	NSS2 = 1,
	NSS4,
	NSS8,
}nsscode;

typedef enum _txbfcode{
	EBF = 1,
	IBF,
	EBFIBF,
}txbfcode;

typedef enum _mimocode{
	DLMIMO = 1,
	ULMIMO,
	DLULMIMO,
}mimocode;

typedef enum _ofdmacode{
	DLOFDMA = 1,
	ULOFDMA,
	DLULOFDMA,
}ofdmacode;

typedef enum _wpscode{
	WPSDISABLE = 0,
	WPSPBC,
	WPSPINCODE,
	WPSPBCPINCODE,
}wpscode;


/***********************************************************/
/********************	CHIP CAP BEGIN	********************/
/***********************************************************/
#define ECNT_CHIP_CAP_SUPPORT_24G          			BIT(0)
#define ECNT_CHIP_CAP_SUPPORT_24G_OFFSET  	 		0

#define ECNT_CHIP_CAP_SUPPORT_5G         			BIT(1)
#define ECNT_CHIP_CAP_5G_SUPPORT_OFFSET   			1

#define ECNT_CHIP_CAP_HT_SUPPORT          			BIT(2)
#define ECNT_CHIP_CAP_HT_SUPPORT_OFFSET   			2

#define ECNT_CHIP_CAP_VHT_SUPPORT          			BIT(3)
#define ECNT_CHIP_CAP_VHT_SUPPORT_OFFSET   			3

#define ECNT_CHIP_CAP_HE_SUPPORT          			BIT(4)
#define ECNT_CHIP_CAP_HE_SUPPORT_OFFSET   			4

/*
1:BW40
2:BW80
3:BW160
4:BW160(160+80/80)
*/
#define ECNT_CHIP_CAP_BW_SUPPORT          			BITS(5,7)
#define ECNT_CHIP_CAP_BW_SUPPORT_OFFSET   			5

/*
1:2x2
2:4x4
3:8x8
4:16x16
*/
#define ECNT_CHIP_CAP_MAX_NSS_SUPPORT          		BITS(8,11)
#define ECNT_CHIP_CAP_MAX_NSS_SUPPORT_OFFSET   		8

/*
1:ebf
2:ibf
*/
#define ECNT_CHIP_CAP_TXBF_SUPPORT          		BITS(12,13)
#define ECNT_CHIP_CAP_TXBF_SUPPORT_OFFSET   		12

#define ECNT_CHIP_CAP_LDPC_SUPPORT          		BIT(14)
#define ECNT_CHIP_CAP_LDPC_SUPPORT_OFFSET   		14

#define ECNT_CHIP_CAP_STBC_SUPPORT          		BIT(15)
#define ECNT_CHIP_CAP_STBC_SUPPORT_OFFSET   		15

/*
1:dl-mimo
2:ul-mimo
*/
#define ECNT_CHIP_CAP_MUMIMO_SUPPORT          		BITS(16,17)
#define ECNT_CHIP_CAP_MUMIMO_SUPPORT_OFFSET   		16

/*
1:dl-ofdma
2:ul-ofdma
*/
#define ECNT_CHIP_CAP_OFDMA_SUPPORT          		BITS(18,19)
#define ECNT_CHIP_CAP_OFDMA_SUPPORT_OFFSET   		18

#define ECNT_CHIP_CAP_SR_SUPPORT          			BIT(20)
#define ECNT_CHIP_CAP_SR_SUPPORT_OFFSET   			20

#define ECNT_CHIP_CAP_TWT_SUPPORT          			BIT(21)
#define ECNT_CHIP_CAP_TWT_SUPPORT_OFFSET   			21

#define ECNT_CHIP_CAP_PPE_SUPPORT          			BIT(22)
#define ECNT_CHIP_CAP_PPE_SUPPORT_OFFSET   			22

#define ECNT_CHIP_CAP_ER_SUPPORT          			BIT(23)
#define ECNT_CHIP_CAP_ER_SUPPORT_OFFSET   			23
/***********************************************************/
/********************	CHIP CAP END	********************/
/***********************************************************/

/***********************************************************/
/********************	FUNC CAP BEGIN	********************/
/***********************************************************/
/*
0:WPS disable
1:pbc
2:pin code
3:all
*/
#define ECNT_FUNC_CAP_SUPPORT_WPS          			BITS(0,2)
#define ECNT_FUNC_CAP_SUPPORT_WPS_OFFSET  	 		0


#define ECNT_FUNC_CAP_SUPPORT_DOT1X          		BIT(3)
#define ECNT_FUNC_CAP_SUPPORT_DOT1X_OFFSET  	 	3

#define ECNT_FUNC_CAP_SUPPORT_ACL          			BIT(4)
#define ECNT_FUNC_CAP_SUPPORT_ACL_OFFSET  	 		4

#define ECNT_FUNC_CAP_SUPPORT_ATE          			BIT(5)
#define ECNT_FUNC_CAP_SUPPORT_ATE_OFFSET  	 		5

#define ECNT_FUNC_CAP_SUPPORT_PMF          			BIT(6)
#define ECNT_FUNC_CAP_SUPPORT_PMF_OFFSET  	 		6

#define ECNT_FUNC_CAP_SUPPORT_IGMPSNOOP          	BIT(7)
#define ECNT_FUNC_CAP_SUPPORT_IGMPSNOOP_OFFSET  	7

#define ECNT_FUNC_CAP_SUPPORT_11K          			BIT(8)
#define ECNT_FUNC_CAP_SUPPORT_11K_OFFSET  	 		8

#define ECNT_FUNC_CAP_SUPPORT_11V          			BIT(9)
#define ECNT_FUNC_CAP_SUPPORT_11V_OFFSET  	 		9

#define ECNT_FUNC_CAP_SUPPORT_11R          			BIT(10)
#define ECNT_FUNC_CAP_SUPPORT_11R_OFFSET  	 		10

#define ECNT_FUNC_CAP_SUPPORT_11U          			BIT(11)
#define ECNT_FUNC_CAP_SUPPORT_11U_OFFSET  	 		11

#define ECNT_FUNC_CAP_SUPPORT_MBO          			BIT(12)
#define ECNT_FUNC_CAP_SUPPORT_MBO_OFFSET  	 		12

#define ECNT_FUNC_CAP_SUPPORT_WDS          			BIT(13)
#define ECNT_FUNC_CAP_SUPPORT_WDS_OFFSET  	 		13

#define ECNT_FUNC_CAP_SUPPORT_WPA3          		BIT(14)
#define ECNT_FUNC_CAP_SUPPORT_WPA3_OFFSET  	 		14

#define ECNT_FUNC_CAP_SUPPORT_MAP          			BIT(15)
#define ECNT_FUNC_CAP_SUPPORT_MAP_OFFSET  	 		15

/***********************************************************/
/********************	FUNC CAP END	********************/
/***********************************************************/

typedef struct chipinfo{
	unsigned int id;/*BIT operation*/
}CHIP_INFO_NODE;


typedef struct global_para_node{
	unsigned char isconfig;
	int user;
	int shmid;
	int	semid;
	CHIP_INFO_NODE chippara[BNADNUM];
}PARA_NODE,*PPARA_NODE;
#endif

typedef struct 
{
	char attr_name[32];
	int (*wifi_api)(wifi_type, int, char*, unsigned int);
	char checkflag;/*0:do nothing 1:check reload 2:check reboot*/
}wifi_status_check_fun;

typedef struct tag_attr_node{
	char attr_name[ATTR_NAME_SIZE];
	char attr_value[MAX_SSID_NUM][ATTR_VALUE_SIZE];
	
}ATTR_NODE;

typedef struct tag_wifi_datpath{
	char 			    path_2_4g[MAX_PATH_LEN];
	char 			    path_5g[MAX_PATH_LEN];
}wifi_datpath;

typedef struct wlan_info{
	int type;
	int oldNum;
	int newNum;
	int index;
#ifdef TCSUPPORT_WLAN_WDS	
	int wdsActive;
	int isWDSReloadWIFI;
#endif
}WLan_info;

typedef struct GNU_PACKED rrm_command_s {
	unsigned char command_id;
	unsigned int command_len;
	unsigned char command_body[0];
} rrm_command_t, *p_rrm_command_t;

struct GNU_PACKED wnm_command {
	unsigned char command_id;
#ifdef TCSUPPORT_MAP_R2	
	unsigned int command_len;
#else
	unsigned char command_len;
#endif
	unsigned char command_body[0];
};
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2)  || defined(TCSUPPORT_NP))
typedef struct wifi_common_info{
	int  wlan_BeaconPeriod;
	int  wlan_DtimPeriod;
	int  wlan_TxPower;
	char wlan_HtTxstream;
	char wlan_HtRxstream;
	char wlan_HtBssCoex;
	char wlan_WirelessMode;
	char wlan_HtExchar;
	char wlan_HTBw;
	char wlan_HtGi;
	int wlan_chann;
	int wlan_CountryRegion;
	int  wlan11ac_BeaconPeriod;
	int  wlan11ac_DtimPeriod;
	int  wlan11ac_TxPower;
	char wlan11ac_HtBssCoex;
	char wlan11ac_WirelessMode;
	char wlan11ac_HtExchar;
	char wlan11ac_HTBw;
	char wlan11ac_HtGi;
	char wlan11ac_VhtBw;
	char wlan11ac_VhtGi;
	unsigned char wlan11ac_chann;
	char wlan11ac_HtTxstream;
	char wlan11ac_HtRxstream;
	char wlan11ac_CountryRegionABand;
#if defined(TCSUPPORT_WLAN_AX)
	char wlan_MuOfdmaDlEnable;
	char wlan_MuOfdmaUlEnable;
	char wlan_MuMimoDlEnable;
	char wlan_MuMimoUlEnable;
	char wlan_TWTSupport;
	char wlan_SREnable;
	char wlan_SRMode;
	char wlan_SRSDEnable;
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)	
	char wlan_ant_dis;
	char wlan_ant_convert;
	char wlan_ant_sel;
#endif
	char wlan11ac_MuOfdmaDlEnable;
	char wlan11ac_MuOfdmaUlEnable;
	char wlan11ac_MuMimoDlEnable;
	char wlan11ac_MuMimoUlEnable;
	char wlan11ac_TWTSupport;
	char wlan11ac_SREnable;
	char wlan11ac_SRMode;
	char wlan11ac_SRSDEnable;
#if defined(TCSUPPORT_WLAN_ANTENNA_DIVERSITY)
	char wlan11ac_ant_dis;
	char wlan11ac_ant_convert;
	char wlan11ac_ant_sel;
#endif
#endif
}Wifi_common_info;
#endif
#endif
