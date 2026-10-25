/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
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

/**
* @file  blapi_xdsl_cmd.c 
* @brief This program gives cmd apis from blapi_xdsl.c
* @author Jun.Wu
* @date     2020-08-12
* @version  A001 
* @copyright EcoNet Inc                                                              
*/


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ecnt_lib_dbg.h>
#include "blapi_xdsl.h"
#include "blapi_xdsl_cmd.h"
#include "libapi_lib_utility.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define MAX_BUF_SIZE 	2048

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static const cmds_t blapi_xdsl_cmd[] = 
{	//cmd							function										flag	argunum	description
	{"reload_ko",					do_blapi_xdsl_reload_ko,					0,	0,		NULL},
	{"get_fwver",					do_blapi_xdsl_get_fwver,					0,	0,		NULL},
	{"get_tsarm",					do_blapi_xdsl_get_tsarm,					0,	0,		NULL},
	{"get_linenum", 				do_blapi_xdsl_get_linenum,					0,	0,		NULL},
	{"get_used_standard",			do_blapi_xdsl_get_used_standard,			0,	0,		NULL},
	{"set_dslline_enable",			do_blapi_xdsl_set_dslline_enable,			0,	0,		NULL},
	{"set_xtmline_enable",			do_blapi_xdsl_set_xtmline_enable,			0,	0,		NULL},
	{"get_xtmline_enable",			do_blapi_xdsl_get_xtmline_enable,			0,	0,		NULL},
	{"get_DDSLLineNode",			do_blapi_xdsl_get_DDSLLineNode, 			0,	0,		NULL},
	{"get_DDSLLineStatsNode",		do_blapi_xdsl_get_DDSLLineStatsNode,		0,	0,		NULL},
	{"get_DDSLChannelInfoNode", 	do_blapi_xdsl_get_DDSLChannelInfoNode,		0,	0,		NULL},
	{"get_DDSLChannelStatsNode",	do_blapi_xdsl_get_DDSLChannelStatsNode, 	0,	0,		NULL},
	{"get_DATMLinkStatsNode",		do_blapi_xdsl_get_DATMLinkStatsNode,		0,	0,		NULL},
	{"get_DPTMLinkStatsNode",		do_blapi_xdsl_get_DPTMLinkStatsNode,		0,	0,		NULL},
	{"ptm_do_reset_sequence",		do_blapi_xdsl_ptm_do_reset_sequence,		0,	0,		NULL},
	{"get_interface_config_file",	do_blapi_xdsl_get_interface_config_file,	0,	0,		NULL},		
	{"atm_create_interface",		do_blapi_xdsl_atm_create_interface, 		0,	0,		NULL},
	{"create_device",				do_blapi_xdsl_create_device,				0,	0,		NULL},
	{"atm_delete_interface",		do_blapi_xdsl_atm_delete_interface,    		0,	0,		NULL},
	{"set_power_up_down",			do_blapi_xdsl_set_power_up_down,			0,	0,		NULL},
	{"set_sfp_power_up_down",		do_blapi_sfp_set_power_up_down,				0,	0,		NULL},
	{"get_adsl_mode",				do_blapi_get_adsl_mode,			    		0,	0,		NULL},
	{"get_adsl_annex",				do_blapi_get_adsl_annex,			    	0,	0,		NULL},
	{"get_vdsl_interface_config",	do_blapi_get_vdsl_interface_config,		  	0,	0,		NULL},
	{"get_adsl_rx_bear_tpstc_type",	do_blapi_get_adsl_rx_bear_tpstc_type,	  	0,	0,		NULL},
	{"get_adsl_attain_rate",		do_blapi_get_adsl_attain_rate,	  			0,	0,		NULL},
	{"get_adsl_ch_op_data",			do_blapi_get_adsl_ch_op_data,	  			0,	0,		NULL},
	{"get_adsl_far_op_data",		do_blapi_get_adsl_far_op_data,	  			0,	0,		NULL},
	{"get_adsl_near_op_data",		do_blapi_get_adsl_near_op_data,	  			0,	0,		NULL},
	{"get_adsl_pms_param",			do_blapi_get_adsl_pms_param,	  			0,	0,		NULL},
	{"get_adsl_pms_inp",			do_blapi_get_adsl_pms_inp,	  				0,	0,		NULL},
	{"get_adsl_wlan_dsl_inf_conf",	do_blapi_get_adsl_wlan_dsl_inf_conf,	  	0,	0,		NULL},
	{"get_adsl_defect_cnt",			do_blapi_get_adsl_defect_cnt,	  			0,	0,		NULL},
	{"get_adsl_cell_cnt1",			do_blapi_get_adsl_cell_cnt1,	  			0,	0,		NULL},
	{"get_adsl_near_ituid",			do_blapi_get_adsl_near_ituid,	  			0,	0,		NULL},
	{"get_adsl_statistic_2",		do_blapi_get_adsl_statistic_2,	  			0,	0,		NULL},
	{"get_adsl_last_drop_reason",	do_blapi_get_adsl_last_drop_reason,	  		0,	0,		NULL},
	{"get_adsl_far_ituid",			do_blapi_get_adsl_far_ituid,	  			0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic0",	do_blapi_get_adsl_cwmp_diagnostic0,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic1",	do_blapi_get_adsl_cwmp_diagnostic1,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic2",	do_blapi_get_adsl_cwmp_diagnostic2,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic3",	do_blapi_get_adsl_cwmp_diagnostic3,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic4",	do_blapi_get_adsl_cwmp_diagnostic4,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic5",	do_blapi_get_adsl_cwmp_diagnostic5,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic6",	do_blapi_get_adsl_cwmp_diagnostic6,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic7",	do_blapi_get_adsl_cwmp_diagnostic7,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic8",	do_blapi_get_adsl_cwmp_diagnostic8,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic9",	do_blapi_get_adsl_cwmp_diagnostic9,	  		0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic10",	do_blapi_get_adsl_cwmp_diagnostic10,	  	0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic11",	do_blapi_get_adsl_cwmp_diagnostic11,	  	0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic12",	do_blapi_get_adsl_cwmp_diagnostic12,	  	0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic13",	do_blapi_get_adsl_cwmp_diagnostic13,	  	0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic14",	do_blapi_get_adsl_cwmp_diagnostic14,	  	0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic15",	do_blapi_get_adsl_cwmp_diagnostic15,	  	0,	0,		NULL},
	{"get_adsl_cwmp_diagnostic16",	do_blapi_get_adsl_cwmp_diagnostic16,	  	0,	0,		NULL},
	{"get_adsl_oam_ping_status",	do_blapi_adsl_get_oam_ping_status,			0,	0,		NULL},	
	{"set_adsl_annex",  			do_blapi_set_adsl_annex,					0,  0,      NULL},
	{"set_adsl_low_power",  		do_blapi_set_adsl_low_power,				0,	0,		NULL},	
	{"set_adsl_aelem",  			do_blapi_set_adsl_aelem,					0,	0,		NULL},	
	{"set_adsl_testlab",  			do_blapi_set_adsl_testlab,					0,	0,		NULL},	
	{"set_adsl_sysvid",  			do_blapi_set_adsl_sysvid,					0,	0,		NULL},	
	{"set_adsl_version",  			do_blapi_set_adsl_version,					0,	0,		NULL},	
	{"set_adsl_ginp",  				do_blapi_set_adsl_ginp,						0,	0,		NULL},	
	{"set_adsl_sra",  				do_blapi_set_adsl_sra,						0,	0,		NULL},	
	{"set_adsl_gvector",  			do_blapi_set_adsl_gvector,					0,	0,		NULL},	
	{"set_adsl_bitswap",  			do_blapi_set_adsl_bitswap,					0,	0,		NULL},	
	{"set_adsl_sos_roc",  			do_blapi_set_adsl_sos_roc,					0,	0,		NULL},	
	{"set_adsl_us0",  				do_blapi_set_adsl_us0,						0,	0,		NULL},	
	{"set_adsl_dying_gasp",  		do_blapi_set_adsl_dying_gasp,				0,	0,		NULL},	
	{"set_adsl_virtual_noise",  	do_blapi_set_adsl_virtual_noise,			0,	0,		NULL},	
	{"set_adsl_mode",  				do_blapi_set_adsl_mode,						0,	0,		NULL},	
	{"set_adsl_profile",  			do_blapi_set_adsl_profile,					0,	0,		NULL},	
	{NULL,							NULL,										0,	0,		NULL}
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

int do_blapi_xdsl_reload_ko(int argc, char *argv[], void *p)
{
	int state = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\n blapi_cmd xdsl reload_ko state" 
			"\r\n state: 2 reload mt7510ptm.ko,  other reload reload mt7510sar.ko\n");
		
		return -1;
	}

	state = atoi(argv[1]);
	blapi_xdsl_reload_ko(state);
	
	ecnt_blapi_printf("\r\n[%s] [%d]: XDSL relod mt7510ptm.ko/mt7510sar.ko\n",__FUNCTION__, __LINE__);	
	return 0;
}

int do_blapi_xdsl_get_fwver(int argc, char *argv[], void *p)
{
	char fwVer[50]={0};
	int ret = 0;
	
	ret = blapi_xdsl_get_fwver(fwVer, sizeof(fwVer));

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get fwver fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get fwver:  %s\n",__FUNCTION__, __LINE__,fwVer);
	}
	
	return 0;
}


int do_blapi_xdsl_get_tsarm(int argc, char *argv[], void *p)
{
	char *buf = NULL;
	int ret = 0;
	int i = 0;

	buf = (char *)malloc(MAX_BUF_SIZE);
	if(NULL == buf)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: malloc fail\n",__FUNCTION__, __LINE__);
		return -1;
	}
	memset(buf,0, MAX_BUF_SIZE);
	
	ret = blapi_xdsl_get_tsarm(buf, MAX_BUF_SIZE);

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get tsarm fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get tsarm:\n",__FUNCTION__, __LINE__);
		for(i = 0; i < MAX_BUF_SIZE; i++)
		{
			ecnt_blapi_printf("%c",buf[i]);
		}
		ecnt_blapi_printf("\r\n");		
	}

	if(NULL != buf)
	{
		free(buf);
		buf = NULL;
	}
		
	return 0;
}

int do_blapi_xdsl_get_linenum(int argc, char *argv[], void *p)
{
	int ret = 0, linenum = 0;
	ret = blapi_xdsl_get_linenum(&linenum);
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get linenum fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get linenum = %d\n",__FUNCTION__, __LINE__, linenum);
	}

	return ret;
}

int do_blapi_xdsl_get_used_standard(int argc, char *argv[], void *p)
{
	int ret = 0;
	char standard[64] = {0};
	memset(standard, 0, sizeof(standard));
	ret = blapi_xdsl_get_used_standard(standard);
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get used_standard fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get used_standard = %s\n",__FUNCTION__, __LINE__, standard);
	}
	
	return ret;
}

int do_blapi_xdsl_set_dslline_enable(int argc, char *argv[], void *p)
{
	int enable = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\n blapi_cmd xdsl set_dslline_enable" 
			"\r\n enable: 0 rmmod mt7510sar.ko.  enable: 1 insmod mt7510sar.ko.\n");
		
		return -1;
	}

	enable = atoi(argv[1]);
	blapi_xdsl_set_dslline_enable(&enable);
	if (0 == enable)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: XDSL rmmod mt7510sar.ko/tc3162_dmt.ko\n",__FUNCTION__, __LINE__);	
	}
	else if(1 == enable)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: XDSL insmod mt7510sar.ko/tc3162_dmt.ko\n",__FUNCTION__, __LINE__);	
	}
	else
	{
		ecnt_blapi_printf("\r\n blapi_cmd xdsl set_dslline_enable" 
			"\r\n enable: 0 rmmod mt7510sar.ko.  enable: 1 insmod mt7510sar.ko.\n");
		return -1;
	}
	
	return 0;
}

int do_blapi_xdsl_set_xtmline_enable(int argc, char *argv[], void *p)
{
	int enable = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\n blapi_cmd xdsl set_xtmline_enable" 
			"\r\n enable: 0 rmmod mt7510ptm.ko.  enable: 1 insmod mt7510ptm.ko.\n");
		
		return -1;
	}

	enable = atoi(argv[1]);
	blapi_xdsl_set_xtmline_enable(&enable);
	if (0 == enable)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: XDSL rmmod mt7510ptm.ko\n",__FUNCTION__, __LINE__);	
	}
	else if(1 == enable)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: XDSL insmod mt7510ptm.ko\n",__FUNCTION__, __LINE__);	
	}
	else
	{
		ecnt_blapi_printf("\r\n blapi_cmd xdsl set_xtmline_enable" 
			"\r\n enable: 0 rmmod mt7510ptm.ko.  enable: 1 insmod mt7510ptm.ko.\n");
		return -1;
	}
	
	return 0;
}

int do_blapi_xdsl_get_xtmline_enable(int argc, char *argv[], void *p)
{
	int ret = 0, xtm_enable = 0;
	ret = blapi_xdsl_get_xtmline_enable(&xtm_enable);
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get xtmline_enable fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get xtmline_enable = %d\n",__FUNCTION__, __LINE__, xtm_enable);
	}
	return ret;
}
	
int do_blapi_xdsl_get_DDSLLineNode(int argc, char *argv[], void *p)
{
	int ret = 0;
	DDSLLineNode_t DDSLLine;
	memset(&DDSLLine, 0, sizeof(DDSLLineNode_t) );
	ret = blapi_xdsl_get_DDSLLineNode(&DDSLLine);
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLLineNode fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLLineNode\n",__FUNCTION__, __LINE__);

		ecnt_blapi_printf("Enable = %d\n", DDSLLine.Enable);
		ecnt_blapi_printf("Status = %s\n", DDSLLine.Status);
		ecnt_blapi_printf("Name = %s\n", DDSLLine.Name);
		ecnt_blapi_printf("LastChange = %d\n", DDSLLine.LastChange);
		ecnt_blapi_printf("Upstream = %d\n", DDSLLine.Upstream);
		ecnt_blapi_printf("FirmwareVersion = %s\n", DDSLLine.FirmwareVersion);
		ecnt_blapi_printf("LinkStatus = %s\n", DDSLLine.LinkStatus);
		ecnt_blapi_printf("StandardsSupported = %s\n", DDSLLine.StandardsSupported);
		ecnt_blapi_printf("StandardUsed = %s\n", DDSLLine.StandardUsed);
		ecnt_blapi_printf("LineEncoding = %s\n", DDSLLine.LineEncoding);
		ecnt_blapi_printf("AllowedProfiles = %s\n", DDSLLine.AllowedProfiles);
		ecnt_blapi_printf("CurrentProfile = %s\n", DDSLLine.CurrentProfile);
		ecnt_blapi_printf("PowerManagementState = %s\n", DDSLLine.PowerManagementState);
		ecnt_blapi_printf("SuccessFailureCause = %d\n", DDSLLine.SuccessFailureCause);
		ecnt_blapi_printf("UPBOKLE = %d\n", DDSLLine.UPBOKLE);
		ecnt_blapi_printf("TRELLISds = %d\n", DDSLLine.TRELLISds);
		ecnt_blapi_printf("TRELLISus = %d\n", DDSLLine.TRELLISus);
		ecnt_blapi_printf("ACTSNRMODEds = %d\n", DDSLLine.ACTSNRMODEds);
		ecnt_blapi_printf("ACTSNRMODEus = %d\n", DDSLLine.ACTSNRMODEus);
		ecnt_blapi_printf("LineNumber = %d\n", DDSLLine.LineNumber);
		ecnt_blapi_printf("UpstreamMaxBitRate = %d\n", DDSLLine.UpstreamMaxBitRate);
		ecnt_blapi_printf("DownstreamMaxBitRate = %d\n", DDSLLine.DownstreamMaxBitRate);
		ecnt_blapi_printf("UpstreamNoiseMargin = %d\n", DDSLLine.UpstreamNoiseMargin);
		ecnt_blapi_printf("DownstreamNoiseMargin = %d\n", DDSLLine.DownstreamNoiseMargin);
		ecnt_blapi_printf("SNRMpbus = %s\n", DDSLLine.SNRMpbus);
		ecnt_blapi_printf("SNRMpbds = %s\n", DDSLLine.SNRMpbds);
		ecnt_blapi_printf("INMIATOds = %d\n", DDSLLine.INMIATOds);
		ecnt_blapi_printf("INMIATSds = %d\n", DDSLLine.INMIATSds);
		ecnt_blapi_printf("INMCCds = %d\n", DDSLLine.INMCCds);
		ecnt_blapi_printf("INMINPEQMODEds = %d\n", DDSLLine.INMINPEQMODEds);
		ecnt_blapi_printf("UpstreamAttenuation = %d\n", DDSLLine.UpstreamAttenuation);
		ecnt_blapi_printf("DownstreamAttenuation = %d\n", DDSLLine.DownstreamAttenuation);
		ecnt_blapi_printf("UpstreamPower = %d\n", DDSLLine.UpstreamPower);
		ecnt_blapi_printf("DownstreamPower = %d\n", DDSLLine.DownstreamPower);		
		ecnt_blapi_printf("XTURVendor = %s\n", DDSLLine.XTURVendor);
		ecnt_blapi_printf("XTURCountry = %s\n", DDSLLine.XTURCountry);
		ecnt_blapi_printf("XTURANSIStd = %d\n", DDSLLine.XTURANSIStd);
		ecnt_blapi_printf("XTURANSIRev = %d\n", DDSLLine.XTURANSIRev);
		ecnt_blapi_printf("XTUCVendor = %s\n", DDSLLine.XTUCVendor);
		ecnt_blapi_printf("XTUCCountry = %s\n", DDSLLine.XTUCCountry);
		ecnt_blapi_printf("XTUCANSIStd = %d\n", DDSLLine.XTUCANSIStd);
		ecnt_blapi_printf("XTUCANSIRev = %d\n", DDSLLine.XTUCANSIRev);
		ecnt_blapi_printf("LastStateTransmittedDownstream = %d\n", DDSLLine.LastStateTransmittedDownstream);
		ecnt_blapi_printf("LastStateTransmittedUpstream = %d\n", DDSLLine.LastStateTransmittedUpstream);
		ecnt_blapi_printf("MREFPSDds = %llu\n", DDSLLine.MREFPSDds);
		ecnt_blapi_printf("MREFPSDus = %llu\n", DDSLLine.MREFPSDus);
		ecnt_blapi_printf("LIMITMASK = %d\n", DDSLLine.LIMITMASK);
		ecnt_blapi_printf("US0MASK = %d\n", DDSLLine.US0MASK);
		ecnt_blapi_printf("VirtualNoisePSDds = %llu\n", DDSLLine.VirtualNoisePSDds);
		ecnt_blapi_printf("VirtualNoisePSDus = %llu\n", DDSLLine.VirtualNoisePSDus);
		ecnt_blapi_printf("ACTUALCE = %d\n", DDSLLine.ACTUALCE);
	}
	return ret;
}

int do_blapi_xdsl_get_DDSLLineStatsNode(int argc, char *argv[], void *p)
{
	int ret = 0;
	DDSL_StatsNode_t Line_Stats;
	memset(&Line_Stats, 0, sizeof(DDSL_StatsNode_t) );
	ret = blapi_xdsl_get_DDSLLineStatsNode(&Line_Stats);
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLLineStatsNode fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLLineStatsNode\n",__FUNCTION__, __LINE__);
		
		ecnt_blapi_printf("BytesSent = %llu\n", Line_Stats.BytesSent);
		ecnt_blapi_printf("BytesReceived = %llu\n", Line_Stats.BytesReceived);
		ecnt_blapi_printf("PacketsSent = %llu\n", Line_Stats.PacketsSent);
		ecnt_blapi_printf("PacketsReceived = %llu\n", Line_Stats.PacketsReceived);
		ecnt_blapi_printf("ErrorsSent = %d\n", Line_Stats.ErrorsSent);
		ecnt_blapi_printf("ErrorsReceived = %d\n", Line_Stats.ErrorsReceived);
		ecnt_blapi_printf("DiscardPacketsSent = 0x%08x\n", Line_Stats.DiscardPacketsSent);
		ecnt_blapi_printf("DiscardPacketsReceived = 0x%08x\n", Line_Stats.DiscardPacketsReceived);
		ecnt_blapi_printf("TotalStart = %d\n", Line_Stats.TotalStart);
		ecnt_blapi_printf("ShowtimeStart = %d\n", Line_Stats.ShowtimeStart);
		ecnt_blapi_printf("LastShowtimeStart = %d\n", Line_Stats.LastShowtimeStart);
		ecnt_blapi_printf("CurrentDayStart = %d\n", Line_Stats.CurrentDayStart);
		ecnt_blapi_printf("QuarterHourStart = %d\n", Line_Stats.QuarterHourStart);
	}

	return ret;
}

int do_blapi_xdsl_get_DDSLChannelInfoNode(int argc, char *argv[], void *p)
{
	int ret = 0;
	DDSLChannelInfoNode_t ChannelInfo;
	memset(&ChannelInfo, 0, sizeof(DDSLChannelInfoNode_t) );
	ret = blapi_xdsl_get_DDSLChannelInfoNode(&ChannelInfo);
	
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLChannelInfoNode fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLChannelInfoNode\n",__FUNCTION__, __LINE__);

		/*
		ecnt_blapi_printf("Enable = %d\n", ChannelInfo.Enable);
		ecnt_blapi_printf("Status = %c\n", ChannelInfo.Status);
		ecnt_blapi_printf("Alias = %c\n", ChannelInfo.Alias);
		*/
		
		ecnt_blapi_printf("Name = %s\n", ChannelInfo.Name);
		ecnt_blapi_printf("LastChange = %d\n", ChannelInfo.LastChange);
		
		/*
		ecnt_blapi_printf("LowerLayers = %c\n", ChannelInfo.LowerLayers);
		*/
		
		ecnt_blapi_printf("LinkEncapsulationSupported = %s\n", ChannelInfo.LinkEncapsulationSupported);		
		ecnt_blapi_printf("LinkEncapsulationUsed = %s\n", ChannelInfo.LinkEncapsulationUsed);		
		ecnt_blapi_printf("LPATH = %d\n", ChannelInfo.LPATH);		
		ecnt_blapi_printf("INTLVDEPTH = %d\n", ChannelInfo.INTLVDEPTH);		
		ecnt_blapi_printf("INTLVBLOCK = %d\n", ChannelInfo.INTLVBLOCK);		
		ecnt_blapi_printf("ActualInterleavingDelay = %d\n", ChannelInfo.ActualInterleavingDelay);		
		ecnt_blapi_printf("ACTINP = %d\n", ChannelInfo.ACTINP);		
		ecnt_blapi_printf("INPREPORT = %d\n", ChannelInfo.INPREPORT);		
		ecnt_blapi_printf("NFEC = %d\n", ChannelInfo.NFEC);	
		ecnt_blapi_printf("RFEC = %d\n", ChannelInfo.RFEC);		
		ecnt_blapi_printf("LSYMB = %d\n", ChannelInfo.LSYMB);		
		ecnt_blapi_printf("UpstreamCurrRate = %d kbps\n", ChannelInfo.UpstreamCurrRate);
		ecnt_blapi_printf("DownstreamCurrRate = %d kbps\n", ChannelInfo.DownstreamCurrRate);
		
		/*
		ecnt_blapi_printf("ACTNDR = %d\n", ChannelInfo.ACTNDR);
		ecnt_blapi_printf("ACTINPREIN = %d\n", ChannelInfo.ACTINPREIN);
		*/
	}

	return ret;
}

int do_blapi_xdsl_get_DDSLChannelStatsNode(int argc, char *argv[], void *p)
{
	int ret = 0;
	DDSL_StatsNode_t Channel_Stats;
	memset(&Channel_Stats, 0, sizeof(DDSL_StatsNode_t) );
	ret = blapi_xdsl_get_DDSLChannelStatsNode(&Channel_Stats);
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLChannelStatsNode fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLChannelStatsNode\n",__FUNCTION__, __LINE__);
		
		ecnt_blapi_printf("BytesSent = %llu\n", Channel_Stats.BytesSent);
		ecnt_blapi_printf("BytesReceived = %llu\n", Channel_Stats.BytesReceived);
		ecnt_blapi_printf("PacketsSent = %llu\n", Channel_Stats.PacketsSent);
		ecnt_blapi_printf("PacketsReceived = %llu\n", Channel_Stats.PacketsReceived);
		ecnt_blapi_printf("ErrorsSent = %d\n", Channel_Stats.ErrorsSent);
		ecnt_blapi_printf("ErrorsReceived = %d\n", Channel_Stats.ErrorsReceived);
		ecnt_blapi_printf("DiscardPacketsSent = 0x%08x\n", Channel_Stats.DiscardPacketsSent);
		ecnt_blapi_printf("DiscardPacketsReceived = 0x%08x\n", Channel_Stats.DiscardPacketsReceived);
		ecnt_blapi_printf("TotalStart = %d\n", Channel_Stats.TotalStart);
		ecnt_blapi_printf("ShowtimeStart = %d\n", Channel_Stats.ShowtimeStart);
		ecnt_blapi_printf("LastShowtimeStart = %d\n", Channel_Stats.LastShowtimeStart);
		ecnt_blapi_printf("CurrentDayStart = %d\n", Channel_Stats.CurrentDayStart);
		ecnt_blapi_printf("QuarterHourStart = %d\n", Channel_Stats.QuarterHourStart);
	}

	return ret;
}
int do_blapi_xdsl_get_DPTMLinkStatsNode(int argc, char *argv[], void *p)
{
	int ret = 0;
	DPTMLinkStatsNode_t DPTMLinkStats;
	memset(&DPTMLinkStats, 0, sizeof(DPTMLinkStatsNode_t) );
	ret = blapi_xdsl_get_DPTMLinkStatsNode(&DPTMLinkStats);	
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DPTMLinkStatsNode fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DPTMLinkStatsNode\n",__FUNCTION__, __LINE__);

		ecnt_blapi_printf("BytesSent = %llu\n", DPTMLinkStats.BytesSent);
		ecnt_blapi_printf("BytesReceived = %llu\n", DPTMLinkStats.BytesReceived);
		ecnt_blapi_printf("PacketsSent = %llu\n", DPTMLinkStats.PacketsSent);
		ecnt_blapi_printf("PacketsReceived = %llu\n", DPTMLinkStats.PacketsReceived);
		ecnt_blapi_printf("ErrorsSent = %d\n", DPTMLinkStats.ErrorsSent);
		ecnt_blapi_printf("ErrorsReceived = %d\n", DPTMLinkStats.ErrorsReceived);
		ecnt_blapi_printf("UnicastPacketsSent = %llu\n", DPTMLinkStats.UnicastPacketsSent);
		ecnt_blapi_printf("UnicastPacketsReceived = %llu\n", DPTMLinkStats.UnicastPacketsReceived);
		ecnt_blapi_printf("DiscardPacketsSent = %d\n", DPTMLinkStats.DiscardPacketsSent);
		ecnt_blapi_printf("DiscardPacketsReceived = 0x%08x\n", DPTMLinkStats.DiscardPacketsReceived);
		ecnt_blapi_printf("MulticastPacketsSent = %llu\n", DPTMLinkStats.MulticastPacketsSent);
		ecnt_blapi_printf("MulticastPacketsReceived = %llu\n", DPTMLinkStats.MulticastPacketsReceived);
		ecnt_blapi_printf("BroadcastPacketsSent = %llu\n", DPTMLinkStats.BroadcastPacketsSent);
		ecnt_blapi_printf("BroadcastPacketsReceived = %llu\n", DPTMLinkStats.BroadcastPacketsReceived);
		ecnt_blapi_printf("UnknownProtoPacketsReceived = %d\n", DPTMLinkStats.UnknownProtoPacketsReceived);
	}
	
	return ret;
}

int do_blapi_xdsl_get_DATMLinkStatsNode(int argc, char *argv[], void *p)
{
	int ret = 0;
	DATMLinkStatsNode_t DATMLinkStats;
	memset(&DATMLinkStats, 0, sizeof(DATMLinkStatsNode_t) );
	ret = blapi_xdsl_get_DATMLinkStatsNode(&DATMLinkStats);	
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLChannelStatsNode fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: get DDSLChannelStatsNode\n",__FUNCTION__, __LINE__);

		ecnt_blapi_printf("BytesSent = %llu\n", DATMLinkStats.BytesSent);
		ecnt_blapi_printf("BytesReceived = %llu\n", DATMLinkStats.BytesReceived);
		ecnt_blapi_printf("PacketsSent = %llu\n", DATMLinkStats.PacketsSent);
		ecnt_blapi_printf("PacketsReceived = %llu\n", DATMLinkStats.PacketsReceived);
		ecnt_blapi_printf("ErrorsSent = %d\n", DATMLinkStats.ErrorsSent);
		ecnt_blapi_printf("ErrorsReceived = %d\n", DATMLinkStats.ErrorsReceived);
		ecnt_blapi_printf("UnicastPacketsSent = %llu\n", DATMLinkStats.UnicastPacketsSent);
		ecnt_blapi_printf("UnicastPacketsReceived = %llu\n", DATMLinkStats.UnicastPacketsReceived);
		ecnt_blapi_printf("DiscardPacketsSent = %d\n", DATMLinkStats.DiscardPacketsSent);
		ecnt_blapi_printf("DiscardPacketsReceived = %d\n", DATMLinkStats.DiscardPacketsReceived);
		ecnt_blapi_printf("MulticastPacketsSent = %llu\n", DATMLinkStats.MulticastPacketsSent);
		ecnt_blapi_printf("MulticastPacketsReceived = %llu\n", DATMLinkStats.MulticastPacketsReceived);
		ecnt_blapi_printf("BroadcastPacketsSent = %llu\n", DATMLinkStats.BroadcastPacketsSent);
		ecnt_blapi_printf("BroadcastPacketsReceived = %llu\n", DATMLinkStats.BroadcastPacketsReceived);
		ecnt_blapi_printf("UnknownProtoPacketsReceived = %d\n", DATMLinkStats.UnknownProtoPacketsReceived);
		ecnt_blapi_printf("TransmittedBlocks = %d\n", DATMLinkStats.TransmittedBlocks);
		ecnt_blapi_printf("ReceivedBlocks = %d\n", DATMLinkStats.ReceivedBlocks);
		ecnt_blapi_printf("CRCErrors = %d\n", DATMLinkStats.CRCErrors);
		ecnt_blapi_printf("HECErrors = %d\n", DATMLinkStats.HECErrors);
	}
	
	return ret;
}

int do_blapi_xdsl_ptm_do_reset_sequence(int argc, char *argv[], void *p)
{
	int adsl_state = 0;
	int gdsl_state = 0;
	int ret = 0;

	if(argc < 3)
	{
		ecnt_blapi_printf("\r\n blapi_cmd xdsl ptm_do_reset_sequence adsl_state gdsl_state" 
			"\r\n adsl_status = 1 and gdsl_status = 2: reset release, adsl_status = 0 and gdsl_status !=0: reset hold\n");
		
		return -1;
	}

	adsl_state = atoi(argv[1]);
	gdsl_state = atoi(argv[2]);
	
	ret = blapi_xdsl_ptm_do_reset_sequence(adsl_state, gdsl_state);
	
	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: ptm do nothing\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: ptm do reset sequence release/hold\n",__FUNCTION__, __LINE__);
	}
			
	return 0;
}


int do_blapi_xdsl_get_interface_config_file(int argc, char *argv[], void *p)
{
	char filePath[64] = {0};
	
	blapi_xdsl_get_interface_config_file(filePath, sizeof(filePath));	

	ecnt_blapi_printf("\r\n[%s] [%d]: get interface config file:  %s\n",__FUNCTION__, __LINE__,filePath);
	
	return 0;
}


int do_blapi_xdsl_atm_create_interface(int argc, char *argv[], void *p)
{
	int ret = 0, pvcactive_flag = 0;
	blapi_atm_interface_obj_t obj = {0};

	if(argc < 11)
	{
		ecnt_blapi_printf("\r\n blapi_cmd xdsl ptm_do_reset_sequence wanidx transMode str_qos str_encap vpi vci pcr scr mbs isp\n");
		return -1;
	}

	obj.idx = atoi(argv[1]);
	strncpy(obj.transMode, argv[2], sizeof(obj.transMode) - 1);
	strncpy(obj.str_qos, argv[3], sizeof(obj.str_qos) - 1);
	strncpy(obj.str_encap, argv[4], sizeof(obj.str_encap) - 1);
	obj.vpi = atoi(argv[5]);
	obj.vci = atoi(argv[6]);

	strncpy(obj.str_pcr, argv[7], sizeof(obj.str_pcr) - 1);
	strncpy(obj.str_scr, argv[8], sizeof(obj.str_scr) - 1);
	strncpy(obj.str_mbs, argv[9], sizeof(obj.str_mbs) - 1);
	strncpy(obj.isp, argv[10], sizeof(obj.isp) - 1);

	ret = blapi_xdsl_atm_create_interface(&obj, &pvcactive_flag);

	ecnt_blapi_printf("\r\n[%s] [%d]: pvcactive_flag:  %d\n",__FUNCTION__, __LINE__, pvcactive_flag);

	return ret;

}

int do_blapi_xdsl_atm_delete_interface(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0, ifnum = 0;

	if(argc < 2)
	{
		ecnt_blapi_printf("\r\n blapi_cmd xdsl \n");
		return -1;
	}

	ifnum = atoi(argv[1]);

	ret = blapi_xdsl_atm_delete_interface(ifnum);

	ecnt_blapi_printf("\r\n[%s] [%d]: do_blapi_xdsl_atm_delete_interface:  %d\n",__FUNCTION__, __LINE__, ret);

	return 0;
#else
	return -1;
#endif
}

int do_blapi_xdsl_create_device(int argc, char *argv[], void *p)
{
	int ret = 0;
	blapi_atm_device_obj_t obj = {0};

	if(argc < 4)
	{
		ecnt_blapi_printf("\r\n blapi_cmd xdsl ptm_do_reset_sequence wanidx transMode dev\n");
		return -1;
	}

	obj.idx = atoi(argv[1]);
	strncpy(obj.transMode, argv[2], sizeof(obj.transMode) - 1);
	strncpy(obj.dev, argv[3], sizeof(obj.dev) - 1);
	if( argc > 4 )
		strncpy(obj.mac, argv[4], sizeof(obj.mac) - 1);
	ret = blapi_xdsl_create_device(&obj);

	return ret;
}

int do_blapi_set_adsl_annex(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    char *annex_str[] = {"a", "b", "i", "j", "al", "m", "bj", "aijlm"};
    unsigned char annex = XDSL_ANNEX_DEF_NO;
	int i = 0;

    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_annex annex \n");
    }
	for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

	annex = (unsigned char) atoi(argv[1]);
	switch(annex)
	{
		case XDSL_ANNEX_A:
		case XDSL_ANNEX_B:
		case XDSL_ANNEX_I:
		case XDSL_ANNEX_J:
		case XDSL_ANNEX_AL:
		case XDSL_ANNEX_M:
		case XDSL_ANNEX_BJ:
		case XDSL_ANNEX_AIJLM:
		    return blapi_xdsl_set_adsl_annex(annex_str[annex]);	
		default:
			printf("invalid annex %s\n", argv[1]);
			return -1;
	}
#else
	return -1;
#endif
}

int do_blapi_set_adsl_low_power(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    char* power_str[] = {"on", "off"};
    unsigned char power = 0;
    int i = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_low_power on / off \n");
        return -1;
    }

    for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

    power = (unsigned char)atoi(argv[1]);
    if(power < 2)
    {
        ret = blapi_xdsl_set_adsl_low_power(power_str[power]);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid low_power %s \n", argv[1]);
        return -1;
    }

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_aelem(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    char* aelem_str[] = {"on", "off"};
    unsigned char aelem = 0;
    int i = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_aelem on / off \n");
        return -1;
    }

    for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

    aelem = (unsigned char)atoi(argv[1]);
    if(aelem < 2)
    {
        ret = blapi_xdsl_set_adsl_aelem(aelem_str[aelem]);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid aelem %s \n", argv[1]);
        return -1;
    }

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_testlab(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_testlab {0~255} \n");
        return -1;
    }

    ret = blapi_xdsl_set_adsl_testlab(argv[1]);

    return ret;  
#else
	return -1;
#endif
}

int do_blapi_set_adsl_sysvid(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_sysvid sysvid \n");
        return -1;
    }

    ret = blapi_xdsl_set_adsl_sysvid(argv[1]);

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_version(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_version version \n");
        return -1;
    }

    ret = blapi_xdsl_set_adsl_version(argv[1]);

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_ginp(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    char* ginp_str[] = {"on", "off"};
    unsigned char ginp = 0;
    int i = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_ginp on / off \n");
        return -1;
    }

    for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

    ginp = (unsigned char)atoi(argv[1]);
    if(ginp < 2)
    {
        ret = blapi_xdsl_set_adsl_ginp(ginp_str[ginp]);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid ginp %s \n", argv[1]);
        return -1;
    }

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_sra(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    char* sra_str[] = {"3", "2"}; //  3-- SRAON, 2 -- SRAOFF
    unsigned char sra = 0;
    int i = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_sra on / off \n");
        return -1;
    }

    for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

    sra = (unsigned char)atoi(argv[1]);
    if(sra < 2)
    {
        ret = blapi_xdsl_set_adsl_sra(sra_str[sra]);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid sra %s \n", argv[1]);
        return -1;
    }

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_gvector(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    char* gvector_str[] = {"on", "off"};
    unsigned char gvector = 0;
    int i = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_gvector on / off \n");
        return -1;
    }

    for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

    gvector = (unsigned char)atoi(argv[1]);
    if(gvector < 2)
    {
        ret = blapi_xdsl_set_adsl_gvector(gvector_str[gvector]);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid gvector %s \n", argv[1]);
        return -1;
    }

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_bitswap(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    char* bitswap_str[] = {"1", "0"}; // 1 -- bitswap on, 0 -- bitswap off
    unsigned char bitswap = 0;
    int i = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_bitswap on / off \n");
        return -1;
    }

    for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

    bitswap = (unsigned char)atoi(argv[1]);
    if(bitswap < 2)
    {
        ret = blapi_xdsl_set_adsl_bitswap(bitswap_str[bitswap]);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid bitswap %s \n", argv[1]);
        return -1;
    }

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_sos_roc(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	int sos = -1;
	int roc = -1;
	
	if(argc < 3){
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_sos_roc on/off on/off \n");
		return -1;
	}

	sos = atoi(argv[1]);
	roc = atoi(argv[2]);
	
    if(sos < 2 && roc < 2)
    {
		ret = blapi_xdsl_set_adsl_sos_roc(sos, roc);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid sos %s roc %s\n", argv[1], argv[2]);
        return -1;
    }
	

	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set dsl sos roc fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set dsl sos roc success\n", __FUNCTION__, __LINE__);

	return 0;
#else
	return -1;
#endif
}
int do_blapi_set_adsl_us0(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    char* us0_str[] = {"on", "off"};
    unsigned char us0 = 0;
    int i = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_us0 on / off \n");
        return -1;
    }

    for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

    us0 = (unsigned char)atoi(argv[1]);
    if(us0 < 2)
    {
        ret = blapi_xdsl_set_adsl_us0(us0_str[us0]);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid us0 %s \n", argv[1]);
        return -1;
    }

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_dying_gasp(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    char* dying_gasp_str[] = {"on", "off"};
    unsigned char dying_gasp = 0;
    int i = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_dying_gasp on / off \n");
        return -1;
    }

    for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

    dying_gasp = (unsigned char)atoi(argv[1]);
    if(dying_gasp < 2)
    {
        ret = blapi_xdsl_set_adsl_dying_gasp(dying_gasp_str[dying_gasp]);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid dying_gasp %s \n", argv[1]);
        return -1;
    }

	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_virtual_noise(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
    int ret = 0;
    char* virtual_noise_str[] = {"on", "off"};
    unsigned char virtual_noise = 0;
    int i = 0;
    
    if(argc < 2)
    {
        ecnt_blapi_printf("\r\n blapi_cmd xdsl set_adsl_virtual_noise on / off \n");
        return -1;
    }

    for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return -1;
	    }
	}

    virtual_noise = (unsigned char)atoi(argv[1]);
    if(virtual_noise < 2)
    {
        ret = blapi_xdsl_set_adsl_virtual_noise(virtual_noise_str[virtual_noise]);
    }
    else 
    {
        ecnt_blapi_printf("\r\n invalid virtual_noise %s \n", argv[1]);
        return -1;
    }
	return ret;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_mode(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;

	unsigned char adsl1_flag, issue2_flag, Glite_flag, adsl2_flag, adsl2p_flag, vdsl2_flag;
	char buf[16] = {0};

	if (argc < 8)
	{
		adsl1_flag=atoi(argv[1]);//G.dmt
		issue2_flag=atoi(argv[2]);//T1.413
		Glite_flag=atoi(argv[3]);//G.lite
		adsl2_flag=atoi(argv[4]);//Adsl2
		adsl2p_flag=atoi(argv[5]);//Adsl2plus
		vdsl2_flag=atoi(argv[6]);//Vdsl2
		
		ecnt_blapi_printf("adsl1_flag, issue2_flag, Glite_flag, adsl2_flag, adsl2p_flag, vdsl2_flag = %d %d %d %d %d %d\n", adsl1_flag, issue2_flag, Glite_flag, adsl2_flag, adsl2p_flag, vdsl2_flag);
	}
	else
	{
		ecnt_blapi_printf("usage: set_adsl_mode adsl1_flag issue2_flag Glite_flag adsl2_flag adsl2p_flag vdsl2_flag");
		return -1;
	}
						

    snprintf(buf, sizeof(buf), "%d %d %d %d %d %d", adsl1_flag, issue2_flag, Glite_flag, adsl2_flag, adsl2p_flag, vdsl2_flag);

	ret = blapi_xdsl_set_adsl_mode(buf);
	ecnt_blapi_printf("ret = %d\n", ret);

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: set adsl mode fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: set adsl mode success\n",__FUNCTION__, __LINE__);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_set_adsl_profile(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;

	unsigned char pro_8a_flag,pro_8b_flag,pro_8c_flag,pro_8d_flag,pro_12a_flag,pro_12b_flag,pro_17a_flag,pro_30a_flag,pro_35b_flag = 0;
	char buf[64] = {0};

	if (argc < 11)
	{	
		pro_8a_flag=atoi(argv[1]);
		pro_8b_flag=atoi(argv[2]);
		pro_8c_flag=atoi(argv[3]);
		pro_8d_flag=atoi(argv[4]);
		pro_12a_flag=atoi(argv[5]);
		pro_12b_flag=atoi(argv[6]);
		pro_17a_flag=atoi(argv[7]);
		pro_30a_flag=atoi(argv[8]);
		pro_35b_flag=atoi(argv[9]);
		ecnt_blapi_printf("pro_8a_flag,pro_8b_flag,pro_8c_flag,pro_8d_flag,pro_12a_flag,pro_12b_flag,pro_17a_flag,pro_30a_flag,pro_35b_flag = %d %d %d %d %d %d %d %d %d\n", 
			pro_8a_flag,pro_8b_flag,pro_8c_flag,pro_8d_flag,pro_12a_flag,pro_12b_flag,pro_17a_flag,pro_30a_flag,pro_35b_flag);
	}
	else
	{
		ecnt_blapi_printf("usage: set_adsl_profile pro_8a_flag,pro_8b_flag,pro_8c_flag,pro_8d_flag,pro_12a_flag,pro_12b_flag,pro_17a_flag,pro_30a_flag,pro_35b_flag");
		return -1;
	}
						

    snprintf(buf, sizeof(buf), "%d %d %d %d %d %d %d %d %d", pro_8a_flag,pro_8b_flag,pro_8c_flag,pro_8d_flag,pro_12a_flag,pro_12b_flag,pro_17a_flag,pro_30a_flag,pro_35b_flag);

	ret = blapi_xdsl_set_adsl_profile(buf);

	if(ret < 0)
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: set adsl profile fail\n",__FUNCTION__, __LINE__);
	}
	else
	{
		ecnt_blapi_printf("\r\n[%s] [%d]: set adsl profile success\n",__FUNCTION__, __LINE__);
	}
	return 0;
#else
	return -1;
#endif
}

#define SMALL_BUF_SIZE 160
char context_buf[MAX_BUF_SIZE];
char context_small_buf[SMALL_BUF_SIZE];
int do_blapi_get_adsl_mode(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_small_buf,0, SMALL_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_mode(context_small_buf, sizeof(context_small_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl mode fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]: Mode:  %s\n",__FUNCTION__, __LINE__,context_small_buf);
	}
	return 0;
#else
	return -1;
#endif
}
int do_blapi_xdsl_set_power_up_down(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	int power_st = -1;
	
	if(argc < 2){
		return -1;
	}
	
	power_st = atoi(argv[1]);
	
	ret = blapi_xdsl_set_power_up_down(power_st);

	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set dsl power fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set dsl power sucess\n", __FUNCTION__, __LINE__);

	return 0;
#else
	return -1;
#endif
}

int do_blapi_sfp_set_power_up_down(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = -1;
	int power_st = -1;
	
	if(argc < 2){
		return -1;
	}
	
	power_st = atoi(argv[1]);
	
	ret = blapi_sfp_set_power_up_down(power_st);

	if(ret < 0)
		ecnt_blapi_printf("\r\n[%s]%d: set sfp power fail\n", __FUNCTION__, __LINE__);
	else
		ecnt_blapi_printf("\r\n[%s]%d: set sfp power sucess\n", __FUNCTION__, __LINE__);

	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_annex(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_small_buf,0, SMALL_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_annex(context_small_buf, sizeof(context_small_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl annex fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]: %s\n",__FUNCTION__, __LINE__,context_small_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_vdsl_interface_config(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_buf,0, MAX_BUF_SIZE);
	ret = blapi_xdsl_get_vdsl_interface_config(context_buf, sizeof(context_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get vdsl interface config fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_rx_bear_tpstc_type(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_small_buf,0, SMALL_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_rx_bear_tpstc_type(context_small_buf, sizeof(context_small_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl rx bear tpstc type fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_small_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_attain_rate(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_small_buf,0, SMALL_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_attain_rate(context_small_buf, sizeof(context_small_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl attain rate fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_small_buf);
	}
	return 0;
#else
	return -1;
#endif
}
int do_blapi_get_adsl_ch_op_data(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_buf,0, MAX_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_ch_op_data(context_buf, MAX_BUF_SIZE);

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl ch op data fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}
int do_blapi_get_adsl_far_op_data(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_far_op_data(context_buf, MAX_BUF_SIZE);

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl far op data fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_near_op_data(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_small_buf,0, SMALL_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_near_op_data(context_small_buf, sizeof(context_small_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl near op data fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_small_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_pms_param(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_buf,0, MAX_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_pms_param(context_buf, sizeof(context_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl pms param fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_pms_inp(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_buf,0, MAX_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_pms_inp(context_buf, sizeof(context_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl pms inp fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_adsl_get_oam_ping_status(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_buf,0, MAX_BUF_SIZE);
	ret = blapi_xdsl_get_oam_ping_status(context_buf, sizeof(context_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl oam_ping inp fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_wlan_dsl_inf_conf(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_buf,0, MAX_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_wlan_dsl_inf_conf(context_buf, MAX_BUF_SIZE);

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl wlan dsl inf config fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_defect_cnt(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_defect_cnt(context_buf, MAX_BUF_SIZE);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl pms param fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cell_cnt1(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_buf,0, MAX_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_cell_cnt1(context_buf, MAX_BUF_SIZE);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl cell cnt1 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_near_ituid(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_small_buf,0, SMALL_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_near_ituid(context_small_buf, sizeof(context_small_buf));
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl near ituid fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_small_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_statistic_2(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_small_buf,0, SMALL_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_statistic_2(context_small_buf, sizeof(context_small_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl statistic2 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_small_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_last_drop_reason(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	
	memset(context_small_buf,0, SMALL_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_last_drop_reason(context_small_buf, sizeof(context_small_buf));

	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl last drop reason fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_small_buf);
	}
	return 0;
#else
	return -1;
#endif
}
int do_blapi_get_adsl_far_ituid(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;

	memset(context_small_buf,0, SMALL_BUF_SIZE);
	ret = blapi_xdsl_get_adsl_far_ituid(context_small_buf, sizeof(context_small_buf));
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl far ituid fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_small_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic0(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 0);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic1(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 1);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic1 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic2(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 2);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic2 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic3(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 3);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic3 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic4(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 4);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic4 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic5(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 5);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic5 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic6(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 6);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic6 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}
int do_blapi_get_adsl_cwmp_diagnostic7(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 7);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic7 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic8(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 8);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic8 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic9(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 9);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic9 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic10(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 10);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic10 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic11(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 11);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic11 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic12(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 12);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic12 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic13(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 13);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic13 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic14(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 14);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic14 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic15(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 15);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsl diagnostic15 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_get_adsl_cwmp_diagnostic16(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_SISM_HOST)
	int ret = 0;
	memset(context_buf,0, MAX_BUF_SIZE);

	ret = blapi_xdsl_get_adsl_cwmp_diagnostic(context_buf, MAX_BUF_SIZE, 16);
	if(ret < 0){
		printf("\r\n[%s] [%d]: get adsldiagnostic16 fail\n",__FUNCTION__, __LINE__);
	}else{
		printf("\r\n[%s] [%d]:\n %s\n",__FUNCTION__, __LINE__,context_buf);
	}
	return 0;
#else
	return -1;
#endif
}

int do_blapi_xdsl_cmd(int argc,char* argv[],void *p)
{
    return subcmd(blapi_xdsl_cmd,argc,argv,(void *)blapi_xdsl_cmd);
}

