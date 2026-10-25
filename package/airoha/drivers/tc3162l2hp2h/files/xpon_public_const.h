/***************************************************************
Copyright Statement:

This software/firmware and related documentation (“EcoNet Software? 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (“EcoNet? and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (“ECONET SOFTWARE? RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN “AS IS?
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER’S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER’S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef _XPON_PUBLIC_CONST_H_
#define _XPON_PUBLIC_CONST_H_


#define	XPON_MODE_FLASH_ADDR		(0xff9c)
#define TRANS_VENDOR_NAME_SIZE      16
#define TRANS_VENDOR_PN_SIZE        16
#define TRANS_VENDOR_SN_SIZE        16
#define GPON_SN_LENS            (8)
#define GPON_PASSWD_LENS        (10)
#define GPON_REG_ID_LENS        (36)
#define GPON_ENCRYPT_KEY_LENS   (24)
#define GPON_MAX_GEM_ID         (4096)
#define GPON_UNASSIGN_ALLOC_ID  (0xFF)
#define GPON_MAX_ALLOC_ID       (4096)


#define GPON_10G_MAX_GEM_ID	        (65536)
#define GPON_10G_MAX_VALID_GEM_ID	(65534)
#define GPON_10G_UNASSIGN_ALLOC_ID	(0x3FF)
#define GPON_10G_MAX_ALLOC_ID		(16384)
#define GPON_MSK_LENS		    	(16)

#define GPON_GEMPORT_MAX_NUM     (256)
#define GPON_TCONT_MAX_NUM       32


typedef enum {
	XMCS_IF_WAN_DETECT_MODE_AUTO = 0,
	XMCS_IF_WAN_DETECT_MODE_GPON,
	XMCS_IF_WAN_DETECT_MODE_EPON,
	XMCS_IF_WAN_DETECT_MODE_10G_1G_EPON,
	XMCS_IF_WAN_DETECT_MODE_10G_10G_EPON,
	XMCS_IF_WAN_DETECT_MODE_1G_1G_EPON,
	XMCS_IF_WAN_DETECT_MODE_XGPON,
    XMCS_IF_WAN_DETECT_MODE_XGSPON,
    XMCS_IF_WAN_DETECT_MODE_NGPON2_10G_10G,
    XMCS_IF_WAN_DETECT_MODE_NGPON2_10G_2G,
    XMCS_IF_WAN_DETECT_MODE_NGPON2_2G_2G,
	/*add new mode above this line!*/
	XMCS_IF_WAN_DETECT_MODE_MAX,
} XMCSIF_WanDetectionMode_t ;

typedef enum {
	SYS_GPON_MODE = 0,
	SYS_EPON_MODE,
	SYS_PTM_MODE,
	SYS_ATM_MODE,
	SYS_reserved_MODE,
	SYS_GPON_P2P_MODE,
	SYS_10G_1G_EPON_MODE ,
	SYS_10G_10G_EPON_MODE,
	SYS_1G_1G_EPON_MODE,
	SYS_XGPON_MODE,
	SYS_XGSPON_MODE,
    SYS_NGPON2_10G_10G_MODE,
    SYS_NGPON2_10G_2G_MODE,
    SYS_NGPON2_2G_2G_MODE,
	/*add new mode above this line!*/
	SYS_WAN_MODE_MAX,
} SCU_WanMode_t ;

typedef enum {
	GPON_UNICAST_GEM = 0,
	GPON_MULTICAST_GEM,
} GPON_GemType_t ;

typedef enum {
    XPON_DISABLE = 0,
    XPON_ENABLE,
    XPON_POWER_DOWN,
    XPON_OTHER
} XPON_Mode_t;

typedef enum {
	XMCS_IF_WAN_LINK_OFF = 0,
	XMCS_IF_WAN_LINK_GPON,
	XMCS_IF_WAN_LINK_EPON
} XMCSIF_WanLinkStatus_t ;

typedef struct XMCS_PhyTransInfo_S { 
	uint16_t		txWaveLen;
    uint8_t		vendorName[TRANS_VENDOR_NAME_SIZE];
    uint8_t		vendorPN[TRANS_VENDOR_PN_SIZE];
    uint8_t		vendorSN[TRANS_VENDOR_SN_SIZE];
}PHY_TRANSINFO_T;
typedef struct XMCS_PhyTransParams_S {
	uint16_t			temperature ;
	uint16_t			voltage ;
	uint16_t			txCurrent ;
	uint16_t			txPower ;
	uint16_t			rxPower ;
	PHY_TRANSINFO_T transInfo;
} PHY_PARAMS_t;

typedef struct XMCS_WanLinkConfig_S {
	XPON_Mode_t					linkStart ;
	XMCSIF_WanDetectionMode_t	detectMode ;
	XMCSIF_WanLinkStatus_t		linkStatus ;
}WAN_LINKCFG_t;


struct XMCS_GemPortInfo_S {
	struct {
		GPON_GemType_t		gemType ;
		uint16_t 				gemPortId ;	//gem id
		uint16_t 				allocId ;	//alloc id
		uint16_t 				aniIdx ;	//interface index
		XPON_Mode_t 		lbMode ;	//downstream loopback mode
		XPON_Mode_t			enMode ;	//downstream encryption mode
		XPON_Mode_t         txEncrypt; /*10G gemport tx encryption*/
	} info[GPON_GEMPORT_MAX_NUM] ;
	uint16_t entryNum ; 
} ;


typedef struct XMCS_PhyRxFecConfig_S {
	XPON_Mode_t		fecMode ;
	XPON_Mode_t		fecStatus ;
	XPON_Mode_t		fecCntReset ;
	struct {
		uint32_t		corrBytes ;
		uint32_t 		corrCodeWords ;
		uint32_t		unCorrCodeWords ;
		uint32_t		totalRxCodeWords ;
		uint32_t		fecSeconds ;
	} fecCounter ;
} PHY_FECCONFIG_t;

typedef struct XMCS_WanCntStats_S {
	uint32_t							mask;
	uint32_t							txFrameCnt;
	unsigned long long				txFrameLen;
	uint32_t							txDropCnt;
	uint32_t							txBroadcastCnt;
	uint32_t							txMulticastCnt;
	uint32_t							txLess64Cnt;
	uint32_t							txMore1518Cnt;
	uint32_t							tx64Cnt;
	uint32_t							tx65To127Cnt;
	uint32_t							tx128To255Cnt;
	uint32_t							tx256To511Cnt;
	uint32_t							tx512To1023Cnt;
	uint32_t							tx1024To1518Cnt;
	
	uint32_t							rxFrameCnt;
	unsigned long long				rxFrameLen;
	uint32_t							rxDropCnt;
	uint32_t							rxBroadcastCnt;
	uint32_t							rxMulticastCnt;
	uint32_t							rxCrcCnt;
	uint32_t							rxFragFameCnt;
	uint32_t							rxJabberFameCnt;
	uint32_t							rxLess64Cnt;
	uint32_t							rxMore1518Cnt;
	uint32_t							rx64Cnt;
	uint32_t							rx65To127Cnt;
	uint32_t							rx128To255Cnt;
	uint32_t							rx256To511Cnt;
	uint32_t							rx512To1023Cnt;
	uint32_t							rx1024To1518Cnt;

	uint32_t							rxHecErrorCnt;
	uint32_t							rxFecErrorCnt;
	uint32_t							rxFecCerrorCnt;
	uint32_t							FecSeconds;
	uint32_t							BipError;
}WAN_STATISTIC_t;



#endif //_XPON_PUBLIC_CONST_H_

