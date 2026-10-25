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
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/
#ifndef _BLAPI_XDSL_H_
#define _BLAPI_XDSL_H_

/**
* \file  blapi_xdsl.h  
* \brief This file is blapi xdsl header file of business logic APIs that will be exported for others to use.
* \author Zhen.Xu
* \date     2020-01-06
* \version  A001 
* \copyright EcoNet Inc                                                              
*/

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <stdint.h>

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
/************************************************************************
*               M A C R O S
*************************************************************************
*/

#define ATMCMD_SOCK_PATH 	"/tmp/atmcmd_sock"

#define ATMCMD_MAX_CLIENT	5

/*ATMCMD OP_CMD */
#define ATMCMD_CREATE_PVC	0
#define ATMCMD_CLOSE_PVC	1

/*ATMCMD return message*/
#define ATMCMD_OK			0
#define ATMCMD_FAIL			-1

#define XDSL_STR_BUFF_SIZE  128
/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/

typedef struct blapi_atm_interface_obj
{
	unsigned char idx;
	char transMode[16];
	char str_qos[16];
	char str_encap[32];
	int vpi;
	int vci;
	char str_pcr[16];
	char str_scr[16];
	char str_mbs[16];
	char isp[4];

}blapi_atm_interface_obj_t;

typedef struct blapi_atm_device_obj
{
	unsigned char idx;
	char transMode[16];
	char dev[8];
	char mac[32];
}blapi_atm_device_obj_t;

typedef struct atmcmd_msg {
	unsigned int	op_cmd; 
	/*
		0:sucess, -1:fail
	*/
	int	retval;

  	int itfnum;
  	int vpi;
  	int vci;
  	int encap;
  	int sndbuf;
  	int payload;
  	int qos_type; 
  	int pcr; 
  	int scr; 
  	int mbs; 
} atmcmd_msg_t;

typedef struct DDSLLineNode 
{
	unsigned char	Enable;			/*bool*/	/*	DSLLine_Entry		*/
	char			Status[64];					/*	DSLLine_Entry		*/
	char			Name[16];					/*	always return None	*/
	uint32_t		LastChange;					/*	always return 0		*/
	char			LowerLayers[1024];			/*	DSLLine_Entry		*/
	unsigned char	Upstream;		/*bool*/	/*	always return true	*/
	char			FirmwareVersion[64];		/*	Info_Adsl node		*/
	char			LinkStatus[32];				/*	Info_Adsl node		*/
	char			StandardsSupported[512];	/*	return fixed string	*/
	char			StandardUsed[64];			/*	Info_Adsl node		*/
	char			LineEncoding[64];			/*	always return DMT	*/
	char			AllowedProfiles[64];		/*	return fixed string	*/
	char			CurrentProfile[64];			/*	Info_Adsl node		*/
	char			PowerManagementState[16];	/*	always return None	*/
	uint32_t		SuccessFailureCause;		/*	always return 0		*/
	uint32_t		UPBOKLE;					/*	Info_Adsl node		*/
	int				TRELLISds;					/*	Info_Adsl node		*/
	int				TRELLISus;					/*	Info_Adsl node		*/
	uint32_t		ACTSNRMODEds;				/*	Info_Adsl node		*/
	uint32_t		ACTSNRMODEus;				/*	Info_Adsl node		*/
	int				LineNumber;					/*	always return 1		*/
	uint32_t		UpstreamMaxBitRate;			/*	Info_Adsl node		*/
	uint32_t		DownstreamMaxBitRate;		/*	Info_Adsl node		*/
	int				UpstreamNoiseMargin;		/*	Info_Adsl node		*/
	int				DownstreamNoiseMargin;		/*	Info_Adsl node		*/
	char			SNRMpbus[64];				/*	Info_Adsl node		*/
	char			SNRMpbds[64];				/*	Info_Adsl node		*/
	uint32_t		INMIATOds;					/*	always return 0		*/
	uint32_t		INMIATSds;					/*	always return 0		*/
	uint32_t		INMCCds;					/*	always return 0		*/
	uint32_t		INMINPEQMODEds;				/*	always return 0		*/
	int				UpstreamAttenuation;		/*	Info_Adsl node		*/
	int				DownstreamAttenuation;		/*	Info_Adsl node		*/
	int				UpstreamPower;				/*	Info_Adsl node		*/
	int				DownstreamPower;			/*	Info_Adsl node		*/
	char			XTURVendor[64];				/*	Info_Adsl node		*/
	char			XTURCountry[64];			/*	Info_Adsl node		*/
	uint32_t		XTURANSIStd;				/*	Info_Adsl node		*/
	uint32_t		XTURANSIRev;				/*	Info_Adsl node		*/
	char			XTUCVendor[64];				/*	Info_Adsl node		*/
	char			XTUCCountry[64];			/*	Info_Adsl node		*/
	uint32_t		XTUCANSIStd;				/*	Info_Adsl node		*/
	uint32_t		XTUCANSIRev;				/*	Info_Adsl node		*/
	uint32_t		LastStateTransmittedDownstream;	/*	always return 0	*/
	uint32_t		LastStateTransmittedUpstream;	/*	always return 0	*/
	uint64_t		MREFPSDds;					/*	always return 0		*/
	uint64_t		MREFPSDus;					/*	always return 0		*/
	uint32_t		LIMITMASK;					/*	always return 0		*/
	uint32_t		US0MASK;					/*	always return 0		*/
	uint64_t		VirtualNoisePSDds;			/*	always return 0		*/
	uint64_t		VirtualNoisePSDus;			/*	always return 0		*/
	uint32_t		ACTUALCE;					/*	Info_Adsl node		*/
} DDSLLineNode_t;

typedef struct DDSL_StatsNode 
{
	uint64_t		BytesSent; 
	uint64_t		BytesReceived;
	uint64_t		PacketsSent;
	uint64_t		PacketsReceived;
	uint32_t		ErrorsSent;
	uint32_t		ErrorsReceived;
	uint32_t		DiscardPacketsSent;
	uint32_t		DiscardPacketsReceived;
	uint32_t		TotalStart;
	uint32_t		ShowtimeStart;
	uint32_t		LastShowtimeStart;
	uint32_t		CurrentDayStart;
	uint32_t		QuarterHourStart;
} DDSL_StatsNode_t;

typedef struct DDSLChannelInfoNode 
{
	unsigned char	Enable;		/*bool*/
	char			Status[64];
	char			Alias[64];
	char			Name[64];
	uint32_t		LastChange;
	char			LowerLayers[1024];
	char			LinkEncapsulationSupported[64];
	char			LinkEncapsulationUsed[64];
	uint32_t		LPATH;
	uint32_t		INTLVDEPTH;
	int				INTLVBLOCK;
	uint32_t		ActualInterleavingDelay;
	int				ACTINP;
	unsigned char	INPREPORT;	/*bool*/
	int				NFEC;
	int				RFEC;
	int				LSYMB;
	uint32_t		UpstreamCurrRate;
	uint32_t		DownstreamCurrRate;
} DDSLChannelInfoNode_t;

typedef struct DPTMLinkNode
{
	unsigned char	Enable;		/*bool*/
	char			Status[64];
	char			Alias[64];
	char			Name[64];
	uint32_t		LastChange;
	char			LowerLayers[1024];
	char			MACAddress[17];
} DPTMLinkNode_t;

typedef struct DPTMLinkStatsNode 
{
	uint64_t		BytesSent; 
	uint64_t		BytesReceived;
	uint64_t		PacketsSent;
	uint64_t		PacketsReceived;
	uint32_t		ErrorsSent;
	uint32_t		ErrorsReceived;
	uint64_t		UnicastPacketsSent;
	uint64_t		UnicastPacketsReceived;	
	uint32_t		DiscardPacketsSent;
	uint32_t		DiscardPacketsReceived;
	uint64_t		MulticastPacketsSent;
	uint64_t		MulticastPacketsReceived;
	uint64_t		BroadcastPacketsSent;
	uint64_t		BroadcastPacketsReceived;
	uint32_t		UnknownProtoPacketsReceived;
} DPTMLinkStatsNode_t;

typedef struct DATMLinkNode
{
	unsigned char	Enable;			/*bool*/
	char			Status[64];
	char			Alias[64];
	char			Name[64];
	uint32_t		LastChange;
	char			LowerLayers[1024];
	char			LinkType[32];
	unsigned char	AutoConfig;		/*bool*/
	char			DestinationAddress[256];
	char			Encapsulation[32];
	unsigned char	FCSPreserved;	/*bool*/
	char			VCSearchList[256];
	char			AAL[32];
} DATMLinkNode_t;

typedef struct DATMLinkStatsNode 
{
	uint64_t		BytesSent; 
	uint64_t		BytesReceived;
	uint64_t		PacketsSent;
	uint64_t		PacketsReceived;
	uint32_t		ErrorsSent;
	uint32_t		ErrorsReceived;
	uint64_t		UnicastPacketsSent;
	uint64_t		UnicastPacketsReceived;	
	uint32_t		DiscardPacketsSent;
	uint32_t		DiscardPacketsReceived;
	uint64_t		MulticastPacketsSent;
	uint64_t		MulticastPacketsReceived;
	uint64_t		BroadcastPacketsSent;
	uint64_t		BroadcastPacketsReceived;
	uint32_t		UnknownProtoPacketsReceived;
	uint32_t		TransmittedBlocks;
	uint32_t		ReceivedBlocks;
	uint32_t		CRCErrors;
	uint32_t		HECErrors;
} DATMLinkStatsNode_t;

struct blapi_wan_info_stats {
	uint64_t	 rx_packets;	/* total packets received       */
	uint64_t	 tx_packets;	/* total packets transmitted    */
	uint64_t	 rx_bytes;	/* total bytes received         */
	uint64_t	 tx_bytes;	/* total bytes transmitted      */
	uint64_t rx_errors;		/* bad packets received         */
	uint64_t tx_errors;		/* packet transmit problems     */
	uint64_t rx_dropped;		/* no space in linux buffers    */
	uint64_t tx_dropped;		/* no space available in linux  */
	uint64_t rx_multicast;		/* multicast packets received   */
	uint64_t rx_compressed;
	uint64_t tx_compressed;
	uint64_t collisions;

	/* detailed rx_errors: */
	uint64_t rx_length_errors;
	uint64_t rx_over_errors;	/* receiver ring buff overflow  */
	uint64_t rx_crc_errors;	/* recved pkt with crc error    */
	uint64_t rx_frame_errors;	/* recv'd frame alignment error */
	uint64_t rx_fifo_errors;	/* recv'r fifo overrun          */
	uint64_t rx_missed_errors;	/* receiver missed packet     	*/
	/* detailed tx_errors */
	uint64_t tx_aborted_errors;
	uint64_t tx_carrier_errors;
	uint64_t tx_fifo_errors;
	uint64_t tx_heartbeat_errors;
	uint64_t tx_window_errors;
};

typedef struct blapi_adsl_info
{
	char attr[40][64];
}blapi_adsl_info_t;

typedef enum
{
	XDSL_ANNEX_A = 0,
	XDSL_ANNEX_B,
	XDSL_ANNEX_I,
	XDSL_ANNEX_J,
	XDSL_ANNEX_AL,
	XDSL_ANNEX_M,
	XDSL_ANNEX_BJ,
	XDSL_ANNEX_AIJLM,

	XDSL_ANNEX_DEF_NO
} XDSL_ANNEX;

typedef enum
{
	XDSL_MODE_GLITE = 0,
	XDSL_MODE_T1_413,
	XDSL_MODE_GDMT,
	XDSL_MODE_MULTIMODE,
	XDSL_MODE_ADSL2,
	XDSL_MODE_ADSL2PLUS,
	XDSL_MODE_VDSL2,
	XDSL_MODE_ADSL2PLUS_MULTI,

	XDSL_MODE_DEF_NO
} XDSL_MODE;

typedef enum
{
    XDSL_PROFILE_8A = 0,
    XDSL_PROFILE_8B,
    XDSL_PROFILE_8C,
    XDSL_PROFILE_8D,
    XDSL_PROFILE_12A,
    XDSL_PROFILE_12B,
    XDSL_PROFILE_17A,
    XDSL_PROFILE_30A,
    XDSL_PROFILE_35B,

    XDSL_PROFILE_DEF_NO
} XDSL_PROFILE;

/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/
/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

/**
 * \brief This function will reload adsl/vdsl .ko file when change the xdsl mode.
 *
 * This function will reload adsl/vdsl .ko file when change the xdsl mode. submit by Lei.Zhang 2019.12.10
 * \param[in]	state:the current xdsl mode.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Reload adsl/vdsl .ko file 
 * \par Example
 * \snippet blapi_xdsl.c Reload adsl/vdsl .ko file 
 */

int blapi_xdsl_reload_ko(int state);

/**
 * \brief This function is used to get fwver.
 *
 * This function is used to get fwver.. submit by jinChao.Yao2019.12.26
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get fwver.
 * \par Example
 * \snippet blapi_xdsl.c used to get fwver.
 */
int blapi_xdsl_get_fwver(char *fwver, const int size);

/**
 * \brief This function is used to get tsarm.
 *
 * This function is used to get fwver.. submit by jinChao.Yao2019.12.26
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get tsarm.
 * \par Example
 * \snippet blapi_xdsl.c used to get tsarm.
 */
int blapi_xdsl_get_tsarm(char *tsarm, const int size);

/**
 * \brief This function is used to wirte message to ptm_do_reset_sequence.
 *
 * This function is used to wirte message to ptm_do_reset_sequence.. submit by jinChao.Yao2019.12.26
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to wirte message to ptm_do_reset_sequence.
 * \par Example
 * \snippet blapi_xdsl.c wirte message to ptm_do_reset_sequence.
 */

int blapi_xdsl_ptm_do_reset_sequence(const int adsl_status, const int gdsl_status);

/**
 * \brief This function is used to get get the name of interface onfig file.
 *
 * This function is used to get get the name of interface onfig file. submit by jinChao.Yao2019.12.26
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get get the name of interface onfig file
 * \par Example
 * \snippet blapi_xdsl.c used to get the name of interface onfig file.
 */


int blapi_xdsl_get_interface_config_file(char *fileName, const unsigned int size);

/**
 * \brief This function is used to create interface for atm.
 *
 * This function is used to create interface.. submit by jiawei.xu2021.3.4
 * \param[out]	pvcactive_flag:the flag active pvc.
 * \param[in]	 idx: wan idx, transMode: trans mode, str_qos: qos type, str_encap: ecnap type,vpi: wan vpi,
 				 vci: wan vci, pcr: wanpvc pcr, scr: wanpvc scr, mbs: wanpvc mbs, isp: wan isp.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to create interface for atm.
 * \par Example
 * \snippet blapi_xdsl.c used to create interface for atm.
 */

int blapi_xdsl_atm_create_interface(blapi_atm_interface_obj_t * obj, int * pvcactive_flag);

/**
 * \brief This function is used to create device in advd.
 *
 * This function is used to create device.. submit by jiawei.xu2021.3.4
 * \param[in]	 idx: wan idx, transMode: trans mode, dev: devname.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to create device in advd.
 * \par Example
 * \snippet blapi_xdsl.c used to create device in advd.
 */

int blapi_xdsl_create_device(blapi_atm_device_obj_t * obj);

/**
 * \brief This function is used to delete process for atm.
 *
 * This function is used to delete interface.. submit by cwshih.shih2021.6.23
 * \param[in]	 int ifnum: the process's ID
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to delete process for atm.
 * \par Example
 * \snippet blapi_xdsl.c used to create interface for atm.
 */

int blapi_xdsl_atm_delete_interface(int ifnum);

/**
 * \brief This function is used to get info from /proc/tc3162/adsl_stats.
 *
 * This function is used to get info from /proc/tc3162/adsl_stats...
 *	submit by jiawei.xu2021.3.4
 * \param[out]	adsl_info:39 needed values from /proc/tc3162/adsl_stats.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get info from /proc/tc3162/adsl_stats.
 * \par Example
 * \snippet blapi_xdsl.c used to get info from /proc/tc3162/adsl_stats.
 */

int blapi_xdsl_set_all_key_val_to_attr(blapi_adsl_info_t *adsl_info);

/**
 * \brief This function is used to get data by client_info module
 *
 * This function is used to get data by client_info module.. submit by swark.yang 2021.7.19
 * \param[in]	 proc_path: client_info proc path buf: get data buffer read_size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get data by client_info module
 * \par Example
 * \snippet blapi_xdsl.c used to get data by client_info module
 */

int get_proc_data(char *proc_path,char* buf,int read_size);

/**
 * \brief This function is used to get adsl mode
 *
 * This function is used to get adsl mode.. submit by swark.yang 2021.7.19
 * \param[in]	 mode: adsl mode size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl mode
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl mode
 */

int blapi_xdsl_get_adsl_mode(char *mode, const int size);

/**
 * \brief This function is used to get adsl annex
 *
 * This function is used to get adsl annex.. submit by swark.yang 2021.7.19
 * \param[in]	 annex: adsl annex size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl annex
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl annex
 */

int blapi_xdsl_get_adsl_annex(char *annex, const int size);

/**
 * \brief This function is used to get vdsl config
 *
 * This function is used to get vdsl config.. submit by swark.yang 2021.7.19
 * \param[in]	 context: vdsl config size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get vdsl config
 * \par Example
 * \snippet blapi_xdsl.c used to get vdsl config
 */

int blapi_xdsl_get_vdsl_interface_config(char *context, const int size);

/**
 * \brief This function is used to get adsl rx bear tpstc type
 *
 * This function is used to get adsl rx bear tpstc type.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl rx bear tpstc size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl rx bear tpstc type
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl rx bear tpstc type
 */

int blapi_xdsl_get_adsl_rx_bear_tpstc_type(char *context, const int size);

/**
 * \brief This function is used to get attain upstream and downstream
 *
 * This function is used to get attain upstream and downstream.. submit by swark.yang 2021.7.19
 * \param[in]	 context: attain upstream and downstream data size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get attain upstream and downstream
 * \par Example
 * \snippet blapi_xdsl.c used to get attain upstream and downstream
 */

int blapi_xdsl_get_adsl_attain_rate(char *context, const int size);

/**
 * \brief This function is used to get channel bit rate of near-end interleaved, near-end fast, far-end interleaved and far-end fast
 *
 * This function is used to get channel bit rate.. submit by swark.yang 2021.7.19
 * \param[in]	 context: channel bit rate size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get channel bit rate
 * \par Example
 * \snippet blapi_xdsl.c used to get channel bit rate
 */

int blapi_xdsl_get_adsl_ch_op_data(char *context, const int size);

/**
 * \brief This function is used to get adsl far op data
 *
 * This function is used to get get adsl far op data.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl far op data size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl far op data
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl far op data
 */

int blapi_xdsl_get_adsl_far_op_data(char *context, const int size);

/**
 * \brief This function is used to get adsl near op data
 *
 * This function is used to get adsl near op data.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl near op data size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl near op data
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl near op data
 */

int blapi_xdsl_get_adsl_near_op_data(char *context, const int size);

/**
 * \brief This function is used to get adsl pms parameter
 *
 * This function is used to get adsl pms parameter.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl pms parameter size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl pms parameter
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl pms parameter
 */

int blapi_xdsl_get_adsl_pms_param(char *context, const int size);

/**
 * \brief This function is used to get adsl pms us_inp and ds_inp
 *
 * This function is used to get adsl pms us_inp and ds_inp.. submit by tonyxiang.wu 2021.9.1
 * \param[in]	 context: adsl pms us_inp and ds_inp size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl pms us_inp and ds_inp
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl pms us_inp and ds_inp
 */

int blapi_xdsl_get_adsl_pms_inp(char *context, const int size);

/**
 * \brief This function is used to get oam ping status
 *
 * This function is used to get oam ping status.. submit by tonyxiang.wu 2021.9.1
 * \param[in]	 context: oam ping status size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get oam ping status
 * \par Example
 * \snippet blapi_xdsl.c used to get oam ping status
 */

int blapi_xdsl_get_oam_ping_status(char *context, const int size);

/**
 * \brief This function is used to get adsl tr69 wlan dsl interface config
 *
 * This function is used to get adsl tr69 wlan dsl interface config.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl tr69 wlan dsl interface config size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl tr69 wlan dsl interface config
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl tr69 wlan dsl interface config
 */

int blapi_xdsl_get_adsl_wlan_dsl_inf_conf(char *context, const int size);

/**
 * \brief This function is used to get adsl near-end error count
 *
 * This function is used to get adsl near-end error count.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl near-end error count size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl near-end error count
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl near-end error count
 */

int blapi_xdsl_get_adsl_defect_cnt(char *context, const int size);

/**
 * \brief This function is used to get adsl cell operation data
 *
 * This function is used to get adsl cell operation data.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl cell operation data size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl cell operation data
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl cell operation data
 */

int blapi_xdsl_get_adsl_cell_cnt1(char *context, const int size);

/**
 * \brief This function is used to get adsl near end itu identification
 *
 * This function is used to get adsl near end itu identification.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl near end itu identification size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl near end itu identification
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl near end itu identification
 */

int blapi_xdsl_get_adsl_near_ituid(char *context, const int size);

/**
 * \brief This function is used to get adsl UAS downstream and upstream , TX and RX BitError Count
 *
 * This function is used to get adsl UAS downstream and upstream , TX and RX BitError Count.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl UAS downstream and upstream , TX and RX BitError Count size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl UAS downstream and upstream , TX and RX BitError Count
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl UAS downstream and upstream , TX and RX BitError Count
 */

int blapi_xdsl_get_adsl_statistic_2(char *context, const int size);

/**
 * \brief This function is used to get adsl last drop reason
 *
 * This function is used to get adsl last drop reason.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl last drop reason size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl last drop reason
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl last drop reason
 */

int blapi_xdsl_get_adsl_last_drop_reason(char *context, const int size);

/**
 * \brief This function is used to get adsl far end itu identification
 *
 * This function is used to get adsl far end itu identification.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl far end itu identification size: read data size
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl far end itu identification
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl far end itu identification
 */

int blapi_xdsl_get_adsl_far_ituid(char *context, const int size);

/**
 * \brief This function is used to get adsl Cwmp Diagnostic status
 *
 * This function is used to get adsl Cwmp Diagnostic status.. submit by swark.yang 2021.7.19
 * \param[in]	 context: adsl Cwmp Diagnostic status size: read data size idx: Index
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get adsl Cwmp Diagnostic status
 * \par Example
 * \snippet blapi_xdsl.c used to get adsl Cwmp Diagnostic status
 */

int blapi_xdsl_get_adsl_cwmp_diagnostic(char *context, const int size ,int idx);

/**
 * \brief This function is get client uplink ratelimit.
 *
 * This function is used to get client uplink ratelimit. submit by jiawei.xu2021.8.6
 * \param[out]	 ratelimit: client's uplink ratelimit
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get client uplink ratelimit.
 * \par Example
 * \snippet blapi_xdsl.c used to get client uplink ratelimit.
 */

int blapi_xdsl_get_client_uplink_ratelimit(unsigned int *ratelimit);

/**
 * \brief This function is used to set xdsl power
 *
 * This function is used to set xdsl power mode.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 power_st: power setting
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set xdsl power mode
 * \par Example
 * \snippet blapi_xdsl.c used to set xdsl power mode
 */
 
int blapi_xdsl_set_power_up_down(int power_st);

/**
 * \brief This function is used to set sfp power
 *
 * This function is used to set sfp power mode.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 power_st: power setting
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set sfp power mode
 * \par Example
 * \snippet blapi_xdsl.c used to set sfp power mode
 */

int blapi_sfp_set_power_up_down(int power_st);

/**
 * \brief This function is used to set adsl annex
 *
 * This function is used to set adsl annex.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: annex name
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set adsl annex
 * \par Example
 * \snippet blapi_xdsl.c used to set adsl annex
 */

int blapi_xdsl_set_adsl_annex(char* context);

/**
 * \brief This function is used to set dmt2 pm setting
 *
 * This function is used to set dmt2 pm setting.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: on / off 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set dmt2 pm setting
 * \par Example
 * \snippet blapi_xdsl.c used to set dmt2 pm setting
 */

int blapi_xdsl_set_adsl_low_power(char* context);

/**
 * \brief This function is used to set tcif aelem setting
 *
 * This function is used to set tcif aelem setting.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: on / off 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set tcif aelem setting
 * \par Example
 * \snippet blapi_xdsl.c used to set tcif aelem setting
 */

int blapi_xdsl_set_adsl_aelem(char* context);

/**
 * \brief This function is used to set dmt db tlb
 *
 * This function is used to set dmt db tlb.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: string and it is 0 ~ 255
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set dmt db tlb
 * \par Example
 * \snippet blapi_xdsl.c used to set dmt db tlb
 */

int blapi_xdsl_set_adsl_testlab(char* context);

/**
 * \brief This function is used to set tcif sysvid
 *
 * This function is used to set tcif sysvid.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: sysvid setting
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set tcif sysvid
 * \par Example
 * \snippet blapi_xdsl.c used to set tcif sysvid
 */

int blapi_xdsl_set_adsl_sysvid(char* context);

/**
 * \brief This function is used to set tcif version
 *
 * This function is used to set tcif version.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: version setting
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set tcif version
 * \par Example
 * \snippet blapi_xdsl.c used to set tcif version
 */

int blapi_xdsl_set_adsl_version(char* context);

/**
 * \brief This function is used to set tcif ginp
 *
 * This function is used to set tcif ginp.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: on / off setting for ginp
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set tcif ginp
 * \par Example
 * \snippet blapi_xdsl.c used to set tcif ginp
 */

int blapi_xdsl_set_adsl_ginp(char* context);

/**
 * \brief This function is used to set dmt2 olr for SRA
 *
 * This function is used to set dmt2 olr for SRA.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: 3-- SRAON, 2 -- SRAOFF
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set dmt2 olr for SRA
 * \par Example
 * \snippet blapi_xdsl.c used to set dmt2 olr for SRA
 */
 
int blapi_xdsl_set_adsl_sra(char* context);

/**
 * \brief This function is used to set tcif gvector
 *
 * This function is used to set tcif gvector.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: on / off setting for tcif gvector
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set tcif gvector
 * \par Example
 * \snippet blapi_xdsl.c used to set tcif gvector
 */

int blapi_xdsl_set_adsl_gvector(char* context);

/**
 * \brief This function is used to set dmt2 olr for bitswap
 *
 * This function is used to set dmt2 olr for bitswap.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: 1 -- bitswap on, 0 -- bitswap off
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set dmt2 olr for bitswap
 * \par Example
 * \snippet blapi_xdsl.c used to set dmt2 olr for bitswap
 */
 
int blapi_xdsl_set_adsl_bitswap(char* context);

/**
 * \brief This function is used to set vdsl2 sos and roc
 *
 * This function is used to set vdsl2 sos and roc.. submit by tonyxiang.wu 2021.9.1
 * \param[in]	 sos: PTM sos setting roc: PTM roc setting 
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set vdsl2 sos and roc
 * \par Example
 * \snippet blapi_xdsl.c used to set vdsl2 sos and roc
 */
 
int blapi_xdsl_set_adsl_sos_roc(int sos, int roc);

/**
 * \brief This function is used to set ghs us0
 *
 * This function is used to set ghs us0.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: on /off setting for us0
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set set ghs us0
 * \par Example
 * \snippet blapi_xdsl.c used to set set ghs us0
 */

int blapi_xdsl_set_adsl_us0(char* context);

/**
 * \brief This function is used to set adsl dying gasp
 *
 * This function is used to set adsl dying gasp.. submit by tonyxiang.wu 2021.9.1
 * \param[in]	 context: on /off setting for adsl dying gasp
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set set adsl dying gasp
 * \par Example
 * \snippet blapi_xdsl.c used to set set adsl dying gasp
 */

int blapi_xdsl_set_adsl_dying_gasp(char* context);

/**
 * \brief This function is used to set vdsl2 v_noise
 *
 * This function is used to set vdsl2 v_noise.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: on /off setting for vdsl2 v_noise
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set vdsl2 v_noise
 * \par Example
 * \snippet blapi_xdsl.c used to set vdsl2 v_noise
 */

int blapi_xdsl_set_adsl_virtual_noise(char* context);

/**
 * \brief This function is used to set adsl mode
 *
 * This function is used to set adsl mode.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: adsl mode setting
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set adsl mode
 * \par Example
 * \snippet blapi_xdsl.c used to set adsl mode
 */

int blapi_xdsl_set_adsl_mode(char* context);

/**
 * \brief This function is used to set adsl profile
 *
 * This function is used to set adsl profile.. submit by tonyxiang.wu 2021.8.3
 * \param[in]	 context: adsl profile setting
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set adsl profile
 * \par Example
 * \snippet blapi_xdsl.c used to set adsl profile
 */

int blapi_xdsl_set_adsl_profile(char* context);

/**
 * \brief This function is used to get linenum.
 *
 * This function is used to get linenum.. submit by zhichao.Chen2022.03.23
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get linenum.
 * \par Example
 * \snippet blapi_xdsl.c used to get linenum.
 */
int blapi_xdsl_get_linenum(int *linenum);

/**
 * \brief This function is used to get used_standard.
 *
 * This function is used to get used_standard.. submit by zhichao.Chen2022.03.23
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get used_standard.
 * \par Example
 * \snippet blapi_xdsl.c used to get used_standard.
 */
int blapi_xdsl_get_used_standard(char *str);

/**
 * \brief This function will insmod/rmmod adsl/vdsl .ko file when change the xdsl mode.
 *
 * This function will insmod/rmmod adsl/vdsl .ko file when change the xdsl mode. submit by zhichao.Chen2022.03.23
 * \param[in]	state:the current xdsl mode.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  insmod/rmmod adsl/vdsl .ko file 
 * \par Example
 * \snippet blapi_xdsl.c insmod/rmmod adsl/vdsl .ko file 
 */
int blapi_xdsl_set_dslline_enable(int *enable);

/**
 * \brief This function is used to get dslline_enable.
 *
 * This function is used to get dslline_enable.. submit by Harvi.Huang2023.12.14
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get dslline_enable.
 * \par Example
 * \snippet blapi_xdsl.c used to get dslline_enable.
 */
int blapi_xdsl_get_dslline_enable(int *enable);

/**
 * \brief This function will insmod/rmmod adsl/vdsl .ko file when change the xdsl mode.
 *
 * This function will insmod/rmmod adsl/vdsl .ko file when change the xdsl mode. submit by zhichao.Chen2022.03.23
 * \param[in]	state:the current xdsl mode.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  insmod/rmmod adsl/vdsl .ko file 
 * \par Example
 * \snippet blapi_xdsl.c insmod/rmmod adsl/vdsl .ko file 
 */
int blapi_xdsl_set_xtmline_enable(int *enable);

/**
 * \brief This function is used to get xtmline_enable.
 *
 * This function is used to get xtmline_enable.. submit by zhichao.Chen2022.03.23
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get xtmline_enable.
 * \par Example
 * \snippet blapi_xdsl.c used to get xtmline_enable.
 */
int blapi_xdsl_get_xtmline_enable(int *enable);

/**
 * \brief This function is used to get DDSLLineNode.
 *
 * This function is used to get DDSLLineNode.. submit by zhichao.Chen2022.03.23
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get DDSLLineNode.
 * \par Example
 * \snippet blapi_xdsl.c used to get DDSLLineNode.
 */
int blapi_xdsl_get_DDSLLineNode(DDSLLineNode_t *DDSLLine);

/**
 * \brief This function is used to get DDSLLineStatsNode.
 *
 * This function is used to get DDSLLineStatsNode.. submit by zhichao.Chen2022.03.23
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get DDSLLineStatsNode.
 * \par Example
 * \snippet blapi_xdsl.c used to get DDSLLineStatsNode.
 */
int blapi_xdsl_get_DDSLLineStatsNode(DDSL_StatsNode_t *Line_Stats);

/**
 * \brief This function is used to get DDSLChannelInfoNode.
 *
 * This function is used to get DDSLChannelInfoNode.. submit by zhichao.Chen2022.03.23
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get DDSLChannelInfoNode.
 * \par Example
 * \snippet blapi_xdsl.c used to get DDSLChannelInfoNode.
 */
int blapi_xdsl_get_DDSLChannelInfoNode(DDSLChannelInfoNode_t *ChannelInfo);

/**
 * \brief This function is used to get DDSLChannelStatsNode.
 *
 * This function is used to get DDSLChannelStatsNode.. submit by zhichao.Chen2022.03.23
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get DDSLChannelStatsNode.
 * \par Example
 * \snippet blapi_xdsl.c used to get DDSLChannelStatsNode.
 */
int blapi_xdsl_get_DDSLChannelStatsNode(DDSL_StatsNode_t *Channel_Stats);

/**
 * \brief This function is used to get DPTMLinkNode_t.
 *
 * This function is used to get DPTMLinkNode_t.. submit by Harvi.Huang2023.12.13
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get DPTMLinkNode_t.
 * \par Example
 * \snippet blapi_xdsl.c used to get DPTMLinkNode_t.
 */
int blapi_xdsl_get_DPTMLinkNode(DPTMLinkNode_t *DPTMLink);

/**
 * \brief This function is used to get DPTMLinkStatsNode.
 *
 * This function is used to get DPTMLinkStatsNode.. submit by zhichao.Chen2022.03.23
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get DPTMLinkStatsNode.
 * \par Example
 * \snippet blapi_xdsl.c used to get DPTMLinkStatsNode.
 */
int blapi_xdsl_get_DPTMLinkStatsNode(DPTMLinkStatsNode_t *DPTMLinkStats);

/**
 * \brief This function is used to get DATMLinkStatsNode.
 *
 * This function is used to get DATMLinkStatsNode.. submit by zhichao.Chen2022.03.23
 * \param[out]	state:the current xdsl mode.
 * \param[in]	 the size of file
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get DATMLinkStatsNode.
 * \par Example
 * \snippet blapi_xdsl.c used to get DATMLinkStatsNode.
 */
int blapi_xdsl_get_DATMLinkStatsNode(DATMLinkStatsNode_t *DATMLinkStats);

#endif /*_BLAPI_XDSL_H_*/
