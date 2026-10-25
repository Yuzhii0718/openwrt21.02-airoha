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

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <libapi_lib_wifimgr.h>
#include "wifimgr_get.h"
#include <net/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>

#ifdef TCSUPPORT_CT_WLAN_JOYME3
int
do_get_WPS_STA_MAC(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	char output[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);

	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WPS_STA_MAC(type,output))
		printf("BssidNum=%s\n",output);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

int 
do_get_ssidnum(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);

	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssidnum(type,index,save,MAX_KEYWORD_LEN))
		printf("BssidNum=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_countryRegion(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);

	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_countryRegion(type,index,save,MAX_KEYWORD_LEN))
		printf("CountryRegion=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_countryRegionABand(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_countryRegionABand(type,index,save,MAX_KEYWORD_LEN))
		printf("CountryRegionABand=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_countryCode(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_countryCode(type,index,save,MAX_KEYWORD_LEN))
		printf("CountryCode=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_channel(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_channel(type,index,save,MAX_KEYWORD_LEN))
		printf("Channel=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_autoChannelSelect(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_autoChannelSelect(type,index,save,MAX_KEYWORD_LEN))
		printf("AutoChannelSelect=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

#ifdef TCSUPPORT_WLAN_ACS
int 
do_get_channelScanScore(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_channelScanScore(type,index,save,MAX_KEYWORD_LEN))
		printf("channelScanScore=%s\n",save);	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_bestChannel(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bestChannel(type,index,save,MAX_KEYWORD_LEN))
		printf("BestChannel=%s\n",save);	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

int 
do_get_beaconPeriod(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_beaconPeriod(type,index,save,MAX_KEYWORD_LEN))
		printf("BeaconPeriod=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_rtsThreshold(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_rtsThreshold(type,index,save,MAX_KEYWORD_LEN))
		printf("RTSThreshold=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_fragThreshold(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_fragThreshold(type,index,save,MAX_KEYWORD_LEN))
		printf("FragThreshold=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_dtimPeriod(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_dtimPeriod(type,index,save,MAX_KEYWORD_LEN))
		printf("DtimPeriod=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_txPower(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_txPower(type,index,save,MAX_KEYWORD_LEN))
		printf("TxPower=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_bgProtection(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bgProtection(type,index,save,MAX_KEYWORD_LEN))
		printf("BGProtection=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_disableOLBC(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_disableOLBC(type,index,save,MAX_KEYWORD_LEN))
		printf("DisableOLBC=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_txPreamble(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_txPreamble(type,index,save,MAX_KEYWORD_LEN))
		printf("TxPreamble=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_txBurst(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_txBurst(type,index,save,MAX_KEYWORD_LEN))
		printf("TxBurst=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_pktAggregate(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_pktAggregate(type,index,save,MAX_KEYWORD_LEN))
		printf("PktAggregate=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_turboRate(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_turboRate(type,index,save,MAX_KEYWORD_LEN))
		printf("TurboRate=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_noForwardingBTNBSSID(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_noForwardingBTNBSSID(type,index,save,MAX_KEYWORD_LEN))
		printf("NoForwardingBTNBSSID=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_shortSlot(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_shortSlot(type,index,save,MAX_KEYWORD_LEN))
		printf("ShortSlot=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wirelessEvent(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wirelessEvent(type,index,save,MAX_KEYWORD_LEN))
		printf("WirelessEvent=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_rekeyInterval(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_rekeyInterval(type,index,save,MAX_KEYWORD_LEN))
		printf("RekeyInterval=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_rekeyMethod(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_rekeyMethod(type,index,save,MAX_KEYWORD_LEN))
		printf("RekeyMethod=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_pmkCachePeriod(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_pmkCachePeriod(type,index,save,MAX_KEYWORD_LEN))
		printf("PMKCachePeriod=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wirelessMode(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wirelessMode(type,index,save,MAX_KEYWORD_LEN))
		printf("WirelessMode=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_basicRate(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_basicRate(type,index,save,MAX_KEYWORD_LEN))
		printf("BasicRate=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_BW(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_BW(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_BW=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_LDPC(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_LDPC(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_LDPC=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_VHT_BW(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_VHT_BW(type,index,save,MAX_KEYWORD_LEN))
		printf("VHT_BW=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_VHT_SGI(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_VHT_SGI(type,index,save,MAX_KEYWORD_LEN))
		printf("VHT_SGI=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_VHT_BW_SIGNAL(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_VHT_BW_SIGNAL(type,index,save,MAX_KEYWORD_LEN))
		printf("VHT_BW_SIGNAL=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_HT_EXTCHA(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_EXTCHA(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_EXTCHA=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_OpMode(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_OpMode(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_OpMode=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_BSSCoexistence(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_BSSCoexistence(type,index,save,
																MAX_KEYWORD_LEN))
		printf("HT_BSSCoexitence=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_TxStream(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_TxStream(type,index,save,
									MAX_KEYWORD_LEN)) {
			printf("HT_TxStream=%s\n",save);
		}

	#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
	#endif

	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_RxStream(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_RxStream(type,index,save,
									MAX_KEYWORD_LEN)) {
		printf("HT_RxStream=%s\n",save);
	}

	#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
	#endif

	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_HT_GI(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_GI(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_GI=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_MCS(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_MCS(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_MCS=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_AutoBA(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_AutoBA(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_AutoBA=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_RDG(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_RDG(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_RDG=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_MpduDensity(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_MpduDensity(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_MpduDensity=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_BAWinSize(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_BAWinSize(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_BAWinSize=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_HTC(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_HTC(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_HTC=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_LinkAdapt(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_LinkAdapt(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_LinkAdapt=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HT_AMSDU(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HT_AMSDU(type,index,save,MAX_KEYWORD_LEN))
		printf("HT_AMSDU=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_authMode(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_authMode(type,index,save,MAX_KEYWORD_LEN))
		printf("AuthMode=%s\n",save);
	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif

	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_txRate(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_txRate(type,index,save,MAX_KEYWORD_LEN))
		printf("TxRate=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_noForwarding(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_noForwarding(type,index,save,MAX_KEYWORD_LEN))
		printf("NoForwarding=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_PMFMFPC(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_PMFMFPC(type,index,save,MAX_KEYWORD_LEN))
		printf("PMFMFPC=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_PMFMFPR(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_PMFMFPR(type,index,save,MAX_KEYWORD_LEN))
		printf("PMFMFPR=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_PMFSHA256(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_PMFSHA256(type,index,save,MAX_KEYWORD_LEN))
		printf("PMFSHA256=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_defaultKeyID(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_defaultKeyID(type,index,save,MAX_KEYWORD_LEN))
		printf("DefaultKeyID=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WPAPSK(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "WPAPSK%d", index);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WPAPSK(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_maxStaNum(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_maxStaNum(type,index,save,MAX_KEYWORD_LEN))
		printf("MaxStaNum=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_mbssmaxStaNum(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_mbssMaxStaNum(type,index,save,MAX_KEYWORD_LEN))
		printf("MbssMaxStaNum=%s\n",save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_ssid(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "SSID%d", index);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_ssid(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_key1Type(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Key1Type");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_key1Type(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_key2Type(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Key2Type");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_key2Type(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_key3Type(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Key3Type");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_key3Type(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_key4Type(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Key4Type");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_key4Type(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_key1Str(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Key1Str%d", index);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_key1Str(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_key2Str(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Key2Str%d", index);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_key2Str(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_key3Str(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Key3Str%d", index);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_key3Str(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_key4Str(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Key4Str%d", index);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_key4Str(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_encrypType(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "EncrypType");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_encrypType(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_preAuth(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "PreAuth");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_preAuth(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_hideSSID(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "HideSSID");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_hideSSID(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wmmCapable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "WmmCapable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wmmCapable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_fixedTxMode(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "FixedTxMode");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_fixedTxMode(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_apsdCapable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "APSDCapable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_apsdCapable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_dlsCapable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "DLSCapable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_dlsCapable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_apAifsn(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "APAifsn");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_apAifsn(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_apCwmin(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "APCwmin");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_apCwmin(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_apCwmax(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "APCwmax");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_apCwmax(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_apTxop(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "APTxop");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_apTxop(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_apACM(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "APACM");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_apACM(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_ReloadFlag(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	char keyword[MAX_KEYWORD_LEN]	= {0};
	char output[MAX_KEYWORD_LEN]	= {0};

	sprintf (keyword, "ReloadFlag");

	if(ECNT_WIFIMGR_SUCCESS ==wifimgr_lib_get_reloadFlag(type, output, MAX_KEYWORD_LEN)) {
		printf("%s=%s\n", keyword, output);
        return ECNT_WIFIMGR_SUCCESS;
	}
	else return ECNT_WIFIMGR_FAILURE;
}


int 
do_get_macAddress(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MacAddress");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_macAddress(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_radiussever(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "RADIUS_Sever");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiusserver(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_get_radiusport(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "RADIUS_Port");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiusport(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_radiuskey(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "RADIUS_Key%d", index);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_radiuskey(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_ieee8021x(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "IEEE8021X");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_ieee8021x(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;

}

int 
do_get_session_timeout_interval(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "session_timeout_interval");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_session_timeout_interval(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;

}


int 
do_get_wdsenable(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "WdsEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wdsenable(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);
	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;

}

int 
do_get_wdslist(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "WdsList");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wdslist(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);
		
#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;

}

int 
do_get_wdsencryptype(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "WdsEncrypType");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wdsencryptype(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);
			
#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wdskey(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	sprintf (keyword, "Wds%dKey", index-1 );
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wdskey(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);
				
#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;

}

int 
do_get_wdsphymode(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "WdsPhyMode");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wdsphymode(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);
			
#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;

}

int
do_get_ownip(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "own_ip_addr");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_ownip(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;	
}

int
do_get_wscpincode(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "wscPinCode");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wscPinCode(type,index,save,MAX_KEYWORD_LEN))
		printf("Please input %s in station, %s=%s\n", keyword, keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;	


}


#if defined(TCSUPPORT_CMCCV2)
int
do_get_noise(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Noise");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_Noise(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_get_chanLoad(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	int chanLoad = 0;
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_chanLoad(type,index,save,MAX_KEYWORD_LEN)){
		sscanf(save,"%d",&chanLoad);
		printf("chanLoad=%d\n",chanLoad/10000);
	}


#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;
}

#endif


int 
do_get_bssAifsn(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "BSSAifsn");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssAifsn(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_bssCwmin(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "BSSCwmin");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssCwmin(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_bssCwmax(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "BSSCwmax");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssCwmax(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_bssTxop(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "BSSTxop");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssTxop(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_bssACM(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "BSSACM");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_bssACM(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_ackPolicy(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "AckPolicy");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_ackPolicy(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_accessPolicy(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "AccessPolicy%d" ,index-1);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_accessPolicy(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_accessControlList(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN] = {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "AccessControlList%d", index-1);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_accessControlList(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wscDefaultSSID(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "WscDefaultSSID%d", index);
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wscDefaultSSID(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wscV2Support(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "WscV2Support");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wscV2Support(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);

#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_EfuseBufferMode(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "EfuseBufferMode");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_EfuseBufferMode(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_igmpSnEnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "IgmpSnEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_igmpSnEnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_Sta11kCap(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = 0;
	int index  	   = 0;

	char *t = NULL;
	int macVal_1[6] = {0};
	unsigned char macVal[6] = {0};
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	t = strtok(argv[1], ",");
	if (t)
	{
		type = atoi(t);
		t = strtok(NULL, ",");
		if(t)
		{
			index = atoi(t);
		}
		else
		{
			printf("index is not exist\n");
			return ECNT_WIFIMGR_FAILURE;
		}
	}
	else
	{
		printf("type is not exist\n");
		return ECNT_WIFIMGR_FAILURE;	
	}

	sscanf(argv[2], "%02x:%02x:%02x:%02x:%02x:%02x", &macVal_1[0], &macVal_1[1], &macVal_1[2], &macVal_1[3], &macVal_1[4], &macVal_1[5]);
	macVal[0] = macVal_1[0];
	macVal[1] = macVal_1[1];
	macVal[2] = macVal_1[2];
	macVal[3] = macVal_1[3];
	macVal[4] = macVal_1[4];
	macVal[5] = macVal_1[5];
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_Sta11kCap(type, index, macVal, (unsigned char *)save, MAX_KEYWORD_LEN))
		printf("Sta11kCap=%d\n",save[0]);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_Sta11vCap(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = 0;
	int index  	   = 0;

	char *t = NULL;
	int macVal_1[6] = {0};
	unsigned char macVal[6] = {0};

	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}


	t = strtok(argv[1], ",");
	if (t)
	{
		type = atoi(t);
		t = strtok(NULL, ",");
		if(t)
		{
			index = atoi(t);
		}
		else
		{
			printf("index is not exist\n");
			return ECNT_WIFIMGR_FAILURE;
		}
	}
	else
	{
		printf("type is not exist\n");
		return ECNT_WIFIMGR_FAILURE;	
	}

		

	sscanf(argv[2], "%02x:%02x:%02x:%02x:%02x:%02x", &macVal_1[0], &macVal_1[1], &macVal_1[2], &macVal_1[3], &macVal_1[4], &macVal_1[5]);
	macVal[0] = macVal_1[0];
	macVal[1] = macVal_1[1];
	macVal[2] = macVal_1[2];
	macVal[3] = macVal_1[3];
	macVal[4] = macVal_1[4];
	macVal[5] = macVal_1[5];
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_Sta11vCap(type, index, macVal, (unsigned char *)save, MAX_KEYWORD_LEN))
		printf("Sta11vCap=%d\n", save[0]);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_AMSDU_NUM(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "AMSDU_NUM");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_AMSDU_NUM(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_bndstrgChanLoad(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "BndstrgChanLoad");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_BndstrgChanLoad(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

#if defined(TCSUPPORT_ECNT_MAP)
int 
do_get_MapEnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MapEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MapEnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_MAP_Turnkey(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MAP_Turnkey");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MAP_Turnkey(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_MAP_Ext(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] = {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MAP_Ext");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MAP_Ext(type,index,save,MAX_KEYWORD_LEN))
		printf("%s=%s\n", keyword, save);
		
#if WIFIMGR_CMD_DBG_SWITCH
	printf("exit %s.\n", __func__);
#endif	
	return ECNT_WIFIMGR_SUCCESS;

}
#endif

#if defined(TCSUPPORT_WLAN_AX)
int 
do_get_MuOfdmaDlEnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MuOfdmaDlEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MuOfdmaDlEnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_MuOfdmaUlEnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MuOfdmaUlEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MuOfdmaUlEnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_MuMimoDlEnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MuMimoDlEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MuMimoDlEnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_MuMimoUlEnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MuMimoUlEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MuMimoUlEnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_SREnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "SREnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_SREnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_SRMode(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "SRMode");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_SRMode(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_SRSDEnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "SRSDEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_SRSDEnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_PPEnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "PPEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_PPEnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_TWTSupport(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "TWTSupport");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_TWTSupport(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_HeraStbcPriority(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "HeraStbcPriority");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_HeraStbcPriority(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_MaxRuOfdma(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MaxRuOfdma");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MaxRuOfdma(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_MaxDLMuMimo(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MaxDLMuMimo");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MaxDLMuMimo(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_MaxULMuMimo(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MaxULMuMimo");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MaxULMuMimo(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_Disable160RuMu(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Disable160RuMu");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_Disable160RuMu(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_get_Vht1024QamSupport(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Vht1024QamSupport");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_Vht1024QamSupport(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

int 
do_get_DscpPriMapEnable(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	int index  	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "DscpPriMapEnable");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_DscpPriMapEnable(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

#if defined(TCSUPPORT_WLAN_APCLIENT)
	
int 
do_get_apcli_status(int argc, char *argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
#endif
	int ret = -1;
	wifi_type type = atoi(argv[1]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	ret = wifimgr_lib_get_apcli_status(type);
	if(ret == 1)
		printf("apcli status is disconnet");
#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_SiteSurvery(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[2048] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Site Survery");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_SiteSurvery(type, NULL, save, 2048))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_Statistics(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char save[2048] 	= {0};
	char keyword[MAX_KEYWORD_LEN] 	= {0};
	wifi_type type = atoi(argv[1]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "Statistics");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_Statistics(type,  save, 2048))
		printf("%s=%s\n",keyword,save);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

#endif

int 
do_get_Reload_WlanAC_Driver(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	reload_WlanAC_Driver();
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_version(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	wifi_type type = atoi(argv[1]);
	char version[8] = {0};

	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_version(type,version,8))
		printf("version=%s\n",version);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_ixia5gflag(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	int result = wifimgr_lib_get_ixia5gflag();
	if(ECNT_WIFIMGR_SUCCESS != result)
		printf("ixia5gflag=%d\n",result);
	else
		printf("ixia5gflag=-1\n");
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_check_wifi_ixia(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_check_wifi_ixia_test())
		printf("check wifi ixia test success\n");
	else
		printf("check wifi ixia test fail\n");
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wlan_script_attr(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	int num = -1;
	int i = 0;
	char output[64][20] = {0};
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	num = wifimgr_lib_get_wlan_script_attr(output);
	if(ECNT_WIFIMGR_SUCCESS <= num)
	{
		printf("wlan script attr : \n");
		for(;i < num; i++)
		{
			printf("%s\n",output[i]);
		}
	}else
		printf("get wlan script attr wrong.\n");
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wlan_attr(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	int num = -1;
	int i = 0;
	wifi_type type = atoi(argv[1]);
	wifi_attr_fun output[64] = {0};
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	num = wifimgr_lib_get_wlan_attr(type, output);
	if(ECNT_WIFIMGR_SUCCESS <= num)
	{
		printf("wlan attr : \n");
		for(; i < num; i++)
		{
			printf("attr_name[%d] = %s\n", i + 1, output[i].attr_name);
		}
	}else
		printf("get wlan attr wrong.\n");
		
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_11AC_CMDflag(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	int result = 0;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	result = wifimgr_lib_get_11AC_CMDflag();	
	printf("_11AC_CMDflag = %d\n", result);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_11N_CMDflag(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	int result = 0;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	result = wifimgr_lib_get_11N_CMDflag();
	printf("_11N_CMDflag = %d\n", result);

		
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_execute_wlan_script(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	FILE *fp = NULL;
	wifi_type type = atoi(argv[1]);
	int ret = 0;

	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	fp = fopen(argv[2], "w");
	if (NULL == fp){
		printf("%s open failed\n", argv[2]);
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if (WIFI_5G == type)
		fprintf(fp, "/userfs/bin/iwpriv rai0 set SSID=\"TEST_SSID_5G\"");	
	else
		fprintf(fp, "/userfs/bin/iwpriv ra0 set SSID=\"TEST_SSID_24G\"");	
		
	fclose(fp);
	ret = chmod(argv[2], S_IRUSR|S_IWUSR|S_IXUSR);
	if (ret < 0)
		printf("chmod file failed\n");
	
	if(0 != wifimgr_lib_execute_wlan_script(type, argv[2]))
		return ECNT_WIFIMGR_FAILURE;
	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_button_state(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	char output[128 + 1] = {0};
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	memset(output, 0, sizeof(output));
	if(wifimgr_lib_get_button_state(type, argv[2], output, 128) != -1)
	{
		output[128] = '\0';
		printf("%s\n", output);
	}
	else
		return ECNT_WIFIMGR_FAILURE;
	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_is_WHNAT_support(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	char val[16] = {0};
	int len = 16;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_is_WHNAT_support(type, val, len);
	printf("is whnat support:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_is_TX_RX_Stream_support(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[16] = {0};
	int len = 16;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_is_Tx_Rx_Stream_support(val, len);
	printf("is TX RX Stream support:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_max_sta_num_ui(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[8] = {0};
	int len = 8;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_get_max_sta_num_ui(val, len);
	printf("max sta number:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WLAN_BUTTON_PATH(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[64] = {0};
	int len = 64;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_get_WLAN_BUTTON_PATH(val, len);
	printf("WLAN BUTTON PATH:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WLAN_MAX_STA_NUM_NAME(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	char val[32] = {0};
	int len = 32;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_get_WLAN_MAX_STA_NUM_NAME(type, val, len);
	printf("WLAN BUTTON PATH:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_CHANNEL_SELECT_DEF(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[8] = {0};
	int len = 8;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_get_CHANNEL_SELECT_DEF(val, len);
	printf("CHANNEL SELECT:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_MESH_TRIGGER_WAIT(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	int time = 0;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_get_MESH_TRIGGER_WAIT(&time);
	printf("MESH TRIGGER WAIT TIME:%d\n", time);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_IEEE8021X_VAL(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[4] = {0};
	int len = 4;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_get_IEEE8021X_VAL(val, len);
	printf("IEEE8021X_VAL:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wlan_entry_attr(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	int num = -1;
	int i = 0;
	wifi_type type = atoi(argv[1]);
	wifi_attr_fun output[32] = {0};
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	num = wifimgr_lib_get_wlan_entry_attr(type, output);
	if(ECNT_WIFIMGR_SUCCESS <= num)
	{
		printf("wlan entry attr : \n");
		for(; i < num; i++)
		{
			printf("    %s\n", output[i].attr_name);
		}
	}else
		printf("get wlan entry attr wrong.\n");
		
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wlan_script_com_attr_config(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	int num = -1;
	int i = 0;
	wifi_type type = atoi(argv[1]);
	wifi_attr_fun output[32] = {0};
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	num = wifimgr_lib_get_wlan_script_com_attr_config(type, output);
	if(ECNT_WIFIMGR_SUCCESS <= num)
	{
		printf("wlan script com attr config: \n");
		for(; i < num; i++)
		{
			printf("    %s\n", output[i].attr_name);
		}
	}else
		printf("get wlan script com attr config wrong.\n");
		
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wlan_igmp_snooping_support(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	int result = -1;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	result = wifimgr_lib_get_wlan_igmp_snooping_support();

	if(-1 == result){
		printf("get wlan igmp snooping support wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	printf("wlan_igmp_snooping_support:%d\n", result);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WPS_BUTTON_PATH(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[32] = {0};
	int len = 32;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_get_WPS_BUTTON_PATH(val, len);
	printf("WPS BUTTON PATH:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WPS_BUTTON(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[128 + 1] = {0};
	int len = 128;
	int ret = -1;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	memset(val, 0, sizeof(val));
	if(0 == wifimgr_lib_get_WPS_BUTTON(val, len, &ret)){
		val[len] = '\0';
		printf("WPS_BUTTON:%s\n", val);
	}else{
		printf("get WPS_BUTTON wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WPS_5G_BUTTON_PATH(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[32] = {0};
	int len = 32;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_get_WPS_5G_BUTTON_PATH(val, len);
	printf("WPS 5G BUTTON PATH:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_AHB_STATUS(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[64 + 1] = {0};
	int len = 64;
	int ret = -1;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	memset(val, 0, sizeof(val));
	if(0 == wifimgr_lib_get_AHB_STATUS(val, len, &ret)){
		val[64] = '\0';
		printf("AHB_STATUS:%s\n", val);
	}else{
		printf("get AHB_STATUS wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WLAN11AC_BUTTON_PATH(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[64] = {0};
	int len = 64;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_get_WLAN11AC_BUTTON_PATH(val, len);
	printf("WLAN11AC_BUTTON_PATH:%s\n", val);
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WLAN11AC_BUTTON(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char val[64 + 1] = {0};
	int len = 64;
	int ret = -1;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	memset(val, 0, sizeof(val));
	if(0 == wifimgr_lib_get_WLAN11AC_BUTTON(val, len, &ret)){
		val[64] = '\0';
		printf("WLAN11AC_BUTTON:%s\n", val);
	}else{
		printf("get WLAN11AC_BUTTON wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WPS_STATUS(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	unsigned short output = 0;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(0 == wifimgr_lib_get_WPS_STATUS(type, &output)){
		printf("WPS_STATUS:%d\n", output);
	}else{
		printf("get WPS_STATUS wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_itf_hwaddr(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char br_itf_name[8] = {0};
	char mac[20] = {0};
	int type = atoi(argv[1]);
	int index = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if( index < 1 || (type != 0 && type != 1)){
		printf("The parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if( index >= 1 && index <= 9){
		if(0 == type){
			snprintf(br_itf_name, sizeof(br_itf_name), "ra%d", index - 1);
		}else if(1 == type){
			snprintf(br_itf_name, sizeof(br_itf_name), "rai%d", index - 1);
		}
	}
	
	if(0 == wifimgr_lib_get_itf_hwaddr(br_itf_name, mac, sizeof(mac))){
		printf("mac:%s\n", mac);
	}else{
		printf("get mac wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_use_global_mac(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	unsigned char mac_addr[]		= {0x00,0xaa,0xbb,0x01,0x23,0x45};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_use_global_mac(mac_addr))
		printf("mac = %02x:%02x:%02x:%02x:%02x:%02x\n", 
			mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_interface_mac(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	unsigned char br_itf_name[8] = {0};
	unsigned char mac[6] = {0};
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	snprintf((char *)br_itf_name, sizeof(br_itf_name), "br0");
	
	if(0 == wifimgr_lib_get_interface_mac(br_itf_name, mac)){
		printf("mac:%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}else{
		printf("get mac wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_interface_mac0(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	unsigned char br_itf_name[8] = {0};
	unsigned char mac[20] = {0};
	int type = atoi(argv[1]);
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if( type != 0 && type != 1 ){
		printf("The parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(0 == type){
		snprintf((char *)br_itf_name, sizeof(br_itf_name), "ra0");
	}else if(1 == type){
		snprintf((char *)br_itf_name, sizeof(br_itf_name), "rai0");
	}
	
	if(0 == wifimgr_lib_get_interface_mac0((char *)br_itf_name, (char *)mac, sizeof(mac))){
		printf("mac:%s\n", mac);
	}else{
		printf("get mac wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_BUTTONDETECT)
int 
do_get_LED_BUTTON_STATUS(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	/*args[0]:	1 :wlan button;  2: wps button;*/
	/*args[1]:	1:click; 2:double click; 3: long click.*/
	int args[2] = {0};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(0 == wifimgr_lib_get_LED_BUTTON_STATUS(args)){
		printf("button:%d\n", args[0]);
		printf("type:%d\n", args[1]);
	}else{
		printf("get mac wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif/*TCSUPPORT_COMPILE*/
int 
do_get_br0_v6addr(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	unsigned char ifid[16] = {0};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_br0_v6addr(ifid))
		printf("mac = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", 
			ifid[0], ifid[1], ifid[2], ifid[3], ifid[4], ifid[5],
			ifid[6], ifid[7], ifid[8], ifid[9], ifid[10], ifid[11],
			ifid[12], ifid[13], ifid[14], ifid[15]);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WLANSHARE_CMD(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char cmd[64] = {0};
	char *t = NULL;
	int actType = atoi(argv[1]);
	int index = 0;
	char enableUserId[8] = {0};
	char userId[8]= {0};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	t = strtok(argv[2], ",");
	if (t)
	{
		index = atoi(t);
	}
	else
	{
		printf("Set index failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		snprintf(enableUserId, sizeof(enableUserId), "%s", t);
	}
	else
	{
		printf("Set enableUserId failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		snprintf(userId, sizeof(userId), "%s", t);
	}
	else
	{
		printf("Set userId failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WLANSHARE_CMD(cmd, actType, index, enableUserId, userId))
		printf("cmd = [%s]\n", cmd);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_test_itf_status(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	short if_flag = atoi(argv[2]);
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(wifimgr_lib_get_test_itf_status(argv[1], if_flag))
		printf("interface up\n");
	else
		printf("interface down\n");

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_CHIP_ID(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	unsigned int chidIdx = 0;
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_CHIP_ID(&chidIdx))
		printf("chidIdx = [%u]\n", chidIdx);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WLAN_SCRIPT(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	char output[64] = {0};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WLAN_SCRIPT(type, sizeof(output), output))
		printf("WLAN_SCRIPT = [%s]\n", output);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WIFI_STATS(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char output[2048] = {0};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WIFI_STATS(argv[1], output, sizeof(output)))
		printf("WIFI_STATS = [%s]\n", output);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

#ifdef TCSUPPORT_WLAN_BNDSTRG
int 
do_get_BndStrgChanload(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char outStatus[32] = {0};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_BndStrgChanload(outStatus, sizeof(outStatus)))
		printf("outStatus = [%s]\n", outStatus);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif
int 
do_get_WpsStatus(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int outWpsStatus = 0;
	char outWpsMac[20] = {0};
	int index = atoi(argv[2]);
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
#ifdef TCSUPPORT_WLAN_MULTI_WPS
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WpsStatus(type, &outWpsStatus, outWpsMac, index - 1))
	{
		printf("outWpsStatus = [%d], outWpsMac = [%s]\n", outWpsStatus, outWpsMac);
		
	}
#else
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WpsStatus(type, &outWpsStatus))
	{
		printf("outWpsStatus = [%d]\n", outWpsStatus);
	}
#endif

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WPS_PIN_CODE(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index = atoi(argv[2]);
	char outPinCode[32] = {0};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WPS_PIN_CODE(type, index, outPinCode))
		printf("outPinCode = [%s]\n", outPinCode);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_CHANNEL(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	char outchannel[32] = {0};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_CHANNEL(type, outchannel))
		printf("outchannel = [%s]\n", outchannel);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

#ifdef WSC_AP_SUPPORT
int 
do_get_WPS_ACL_INFO(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index = atoi(argv[2]);
	RT_802_11_ACL *AclInfor= NULL;
	int i = 0;
	char AclList[8][18];

	if( WIFIMGR_GET_CMD_PARA_COUNT != argc )
	{
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	AclInfor = (RT_802_11_ACL *)malloc(sizeof(RT_802_11_ACL));
	if (!AclInfor)
		return -1;
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_WPS_ACL_INFO(type, index, AclInfor))
	{
		for (i = 0; i < 8; i++)
		{
			snprintf(AclList[i], sizeof(AclList[i]), "%02x:%02x:%02x:%02x:%02x:%02x",
					AclInfor->Entry[i].Addr[0],
					AclInfor->Entry[i].Addr[1],
					AclInfor->Entry[i].Addr[2],
					AclInfor->Entry[i].Addr[3],
					AclInfor->Entry[i].Addr[4],
					AclInfor->Entry[i].Addr[5]);
			if(strcmp(AclList[i], "00:00:00:00:00:00"))
			{
				printf("mac[%d] = %02x:%02x:%02x:%02x:%02x:%02x\n", i, 
					AclInfor->Entry[i].Addr[0],
					AclInfor->Entry[i].Addr[1],
					AclInfor->Entry[i].Addr[2],
					AclInfor->Entry[i].Addr[3],
					AclInfor->Entry[i].Addr[4],
					AclInfor->Entry[i].Addr[5]);
			}
		}
	}
	else
	{
		printf("get WPS ACL INFO fail\n");
	}

	if (AclInfor)
	{
		free(AclInfor);
		AclInfor = NULL;
	}

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

int 
do_get_ifaddr(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	int sock;
	unsigned char mac[6] = {0};
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) 
	{
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	if((sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0)
	{
		printf("error\n");
		return -1;
	}
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_ifaddr(sock, (char *)mac))
		printf("mac = %02x:%02x:%02x:%02x:%02x:%02x\n\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	close(sock);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_CPU_TEMPERATURE(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int cpu_temp = 0;
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) 
	{
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_CPU_TEMPERATURE(type, &cpu_temp))
		printf("cpu_temp = %d\n", cpu_temp);


	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}
int
do_get_img_commit_flag_offset(int argc, char *argv[], void *p)
	{
	#if WIFIMGR_CMD_DBG_SWITCH
			printf("enter %s.\n", __func__);
	#endif
	unsigned int syslog_ra_offset = 0;

		if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) 
		{
			printf("The count of parameters is wrong.\n");
			return ECNT_WIFIMGR_FAILURE;
		}

		if(wifimgr_lib_get_img_commit_flag_offset(&syslog_ra_offset) == 0)
			printf("syslog_ra_offset = %d\n", syslog_ra_offset);
		else
			printf("get syslog_ra_offset wrong!\n");
	
	
	#if WIFIMGR_CMD_DBG_SWITCH
			printf("exit %s.\n", __func__);
	#endif
		
		return ECNT_WIFIMGR_SUCCESS;
	}

int
do_get_img_boot_flag_offset(int argc, char *argv[], void *p)
	{
	#if WIFIMGR_CMD_DBG_SWITCH
			printf("enter %s.\n", __func__);
	#endif
	unsigned int img_boot_flag_offset = 0;

		if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) 
		{
			printf("The count of parameters is wrong.\n");
			return ECNT_WIFIMGR_FAILURE;
		}
	
	if (ECNT_WIFIMGR_SUCCESS != wifimgr_lib_get_img_boot_flag_offset(&img_boot_flag_offset))
			printf("get img_boot_flag_offset wrong!\n");
		else
			printf("img_boot_flag_offset = %d\n", img_boot_flag_offset);
	
	
	#if WIFIMGR_CMD_DBG_SWITCH
			printf("exit %s.\n", __func__);
	#endif
		
		return ECNT_WIFIMGR_SUCCESS;
	}

int 
do_get_neighbourinfo(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	NEIFHBOR_INFO *neighbor_info;
	neighbor_info = (NEIFHBOR_INFO *)malloc(sizeof(NEIFHBOR_INFO));
	if(neighbor_info == NULL)
		return -1;
	int i = 0;

	memset(neighbor_info, 0, sizeof(NEIFHBOR_INFO));
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) 
	{
		printf("The count of parameters is wrong.\n");
		free(neighbor_info);
		return ECNT_WIFIMGR_FAILURE;
	}

	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_neighbourinfo(type, neighbor_info)){
		for(; i < neighbor_info->num; i++)
		{
			printf("============entry idx%d================\n", i + 1);
			printf("band = %s\n", neighbor_info->info_entry[i].band);
			printf("ssid = %s\n", neighbor_info->info_entry[i].ssid);
			printf("bssid = %s\n", neighbor_info->info_entry[i].bssid);
			printf("channel = %s\n", neighbor_info->info_entry[i].channel);
			printf("rssi = %s\n", neighbor_info->info_entry[i].rssi);
			printf("wMode = %s\n", neighbor_info->info_entry[i].wMode);
		}
	}

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
		free(neighbor_info);
		return ECNT_WIFIMGR_SUCCESS;
	}

int 
do_get_ethaddr(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	unsigned char mac[6] = {0};
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(0 == wifimgr_lib_get_ethaddr(mac)){
		printf("mac:%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}else{
		printf("get mac wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_WPSinfo(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	char *t = NULL;
	char ioctlCmd[256] = {0}; 
	char keyword[64] = {0};  
	char info[33] = {0};     
	int infoLen;	
	char nodename[64] = {0}; 
	
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	t = strtok(argv[1], ",");
	if (t)
	{
		snprintf(ioctlCmd, sizeof(ioctlCmd), "%s", t);	
	}
	else
	{
		printf("Set ioctlCmd failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		snprintf(keyword, sizeof(keyword), "%s", t);
	}
	else
	{
		printf("Set keyword failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t =  strtok(NULL, ",");
	if(t)
	{
		infoLen = atoi(t);
	}
	else
	{
		printf("Set infoLen failed!\n");
		return ECNT_WIFIMGR_FAILURE;	
	}
	t = strtok(NULL, ",");
	if (t)
	{
		snprintf(nodename, sizeof(nodename), "%s", t);
	}
	else
	{
		printf("Set nodename failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wps_info(ioctlCmd, keyword, info, infoLen, nodename))
		printf("info = [%s]\n", info);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_wifi_process_info(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	struct diag_wifi_process_info process_info;
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wifi_process_info(&process_info)){
		printf("get wifi process ok\n");
	}else{
		printf("get process info wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}
int 
do_get_wifi_ap_count(int argc, char *argv[], void *p)
{
	wifi_type type = atoi(argv[1]);
	RT_AP_COUNT apcnt_out;

	memset(&apcnt_out, 0, sizeof(apcnt_out));
	getAPCount(type, &apcnt_out);
	printf("=========AP COUNT=========\n");
	printf("txPackets = %lu\n", apcnt_out.txPackets);
	printf("rxPackets = %lu\n", apcnt_out.rxPackets);
	printf("txBytes = %llu\n", apcnt_out.txBytes);
	printf("rxBytes = %llu\n", apcnt_out.rxBytes);
	printf("txErrPkts = %lu\n", apcnt_out.txErrPkts);
	printf("txDropPkts = %lu\n", apcnt_out.txDropPkts);
	printf("rxErrPkts = %lu\n", apcnt_out.rxErrPkts);
	printf("rxDropPkts = %lu\n", apcnt_out.rxDropPkts);
	
	return ECNT_WIFIMGR_SUCCESS;
}

#if defined(TCSUPPORT_CMCCV2)
int 
do_get_wifi_wifiStaRssiInfo(int argc, char *argv[], void *p)
{
	wifi_type type = atoi(argv[1]);
	PROBE_REQ_IOCTL_ROOT IoctlData;
	int i = 0;

	memset(&IoctlData, 0, sizeof(IoctlData));

	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_wifiStaRssiInfo(&IoctlData, type))
	{
		for (i = 0; i < MAX_PROBEREQ_ENTRY_NUM && i < IoctlData.entry_num; i++)
		{
			printf("=======index %d=======\n", i + 1);
			printf("RecvTime = %lld\n", IoctlData.entry[i].RecvTime);
			printf("MacAddr = %02x:%02x:%02x:%02x:%02x:%02x\n", 
				IoctlData.entry[i].MacAddr[0], IoctlData.entry[i].MacAddr[1], IoctlData.entry[i].MacAddr[2], 
				IoctlData.entry[i].MacAddr[3], IoctlData.entry[i].MacAddr[4], IoctlData.entry[i].MacAddr[5]);
			printf("SSIDName = %s\n", IoctlData.entry[i].SSIDName);
			printf("BSSID = %02x:%02x:%02x:%02x:%02x:%02x\n", 
				IoctlData.entry[i].BSSID[0], IoctlData.entry[i].BSSID[1], IoctlData.entry[i].BSSID[2], 
				IoctlData.entry[i].BSSID[3], IoctlData.entry[i].BSSID[4], IoctlData.entry[i].BSSID[5]);
			printf("Channel = %u\n", IoctlData.entry[i].Channel);
			printf("RSSI = %d\n", IoctlData.entry[i].RSSI);
			printf("VendorData = %s\n", IoctlData.entry[i].VendorData);
		}
	}
	else
	{
		printf("get process info wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

int 
do_get_neighbourinfo_v1(int argc, char *argv[], void *p)
{
	wifi_type type = atoi(argv[1]);
	PNDIS_802_11_GET_NEIGHBOUR_INFO IoctlData = NULL;
	int i = 0;

	IoctlData = (PNDIS_802_11_GET_NEIGHBOUR_INFO)malloc(sizeof(NDIS_802_11_GET_NEIGHBOUR_INFO));
	if (NULL == IoctlData)
		return ECNT_WIFIMGR_FAILURE;

	memset(IoctlData, 0, sizeof(NDIS_802_11_GET_NEIGHBOUR_INFO));

	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_neighbourinfo_v1(IoctlData, type))
	{
		for (i = 0; i < 128 && i < IoctlData->num; i++)
		{
			printf("=======index %d=======\n", i + 1);
			printf("No = %d\n", IoctlData->entry[i].No);
			printf("Channel = %d\n", IoctlData->entry[i].Channel);
			printf("SSID = %s\n", IoctlData->entry[i].SSID);
			printf("Bssid = %02x:%02x:%02x:%02x:%02x:%02x\n", 
				IoctlData->entry[i].Bssid[0], IoctlData->entry[i].Bssid[1], IoctlData->entry[i].Bssid[2], 
				IoctlData->entry[i].Bssid[3], IoctlData->entry[i].Bssid[4], IoctlData->entry[i].Bssid[5]);
			printf("Security = %s\n", IoctlData->entry[i].Security);
			printf("Rssi = %d\n", IoctlData->entry[i].Rssi);
			printf("WiressMode = %s\n", IoctlData->entry[i].WiressMode);
			printf("SSID_Len = %d\n", IoctlData->entry[i].SSID_Len);
			printf("WPS = %s\n", IoctlData->entry[i].WPS);
			printf("DPID = %s\n", IoctlData->entry[i].DPID);
			printf("FromBcnReport = %s\n", IoctlData->entry[i].FromBcnReport);
		}
	}
	else
	{
		printf("get process info wrong\n");
		free(IoctlData);
		return ECNT_WIFIMGR_FAILURE;
	}
	free(IoctlData);
	
	return ECNT_WIFIMGR_SUCCESS;
}

#ifdef TCSUPPORT_CMCCV2
int 
do_get_info_UpdateRadioStatics(int argc, char *argv[], void *p)
{
	wifi_type type = atoi(argv[1]);
	RADIO_STATISTICS IoctlData;

	memset(&IoctlData, 0, sizeof(RADIO_STATISTICS));
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_info_UpdateRadioStatics(&IoctlData, type))
	{
		printf("CurrentChannelWidth = %d\n", IoctlData.CurrentChannelWidth);
		printf("RadioRecvPackets = %lu\n", IoctlData.RadioRecvPackets);
		printf("RadioSendPackets = %lu\n", IoctlData.RadioSendPackets);
		printf("BcnFail = %lu\n", IoctlData.BcnFail);
		printf("RadioRecvErrorPackets = %lu\n", IoctlData.RadioRecvErrorPackets);
	}
	else
	{
		printf("get info UpdateRadioStatics wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_info_UpdateNoise(int argc, char *argv[], void *p)
{
	wifi_type type = atoi(argv[1]);
	char Noise[32] = {0};

	memset(Noise, 0, sizeof(Noise));
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_info_UpdateNoise(Noise, type))
	{
		printf("Noise = %s\n", Noise);
	}
	else
	{
		printf("get info UpdateNoise wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_info_UpdateChanLoad(int argc, char *argv[], void *p)
{
	wifi_type type = atoi(argv[1]);
	char ChanLoad[32] = {0};

	memset(ChanLoad, 0, sizeof(ChanLoad));
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_info_UpdateChanLoad(ChanLoad, type))
	{
		printf("ChanLoad = %s\n", ChanLoad);
	}
	else
	{
		printf("get info UpdateNoise wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_get_info_UpdateChanIdle(int argc, char *argv[], void *p)
{
	wifi_type type = atoi(argv[1]);
	char ChanIdle[32] = {0};

	memset(ChanIdle, 0, sizeof(ChanIdle));
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_info_UpdateChanIdle(ChanIdle, type))
	{
		printf("ChanIdle = %s\n", ChanIdle);
	}
	else
	{
		printf("get info UpdateNoise wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	return ECNT_WIFIMGR_SUCCESS;
}

#endif
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
int 
do_get_wifi_cap(int argc, char *argv[], void *p)
{
	wifi_type type = atoi(argv[1]);
	chip_info chipinfo;

	memset(&chipinfo, 0, sizeof(chip_info));
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_cap(type, &chipinfo))
	{
		printf("chipcap = %x\n", chipinfo.chipcapinfo.chipcap);
		printf("funccap = %x\n", chipinfo.funccapinfo.funccap);
	}
	else
	{
		printf("get wifi cap wrong\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

#if defined(TCSUPPORT_MAP_R2)
int
do_get_MapMode(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN]	= {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "MapMode");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_MapMode(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_get_DfsDedicatedZeroWait(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN]	= {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "DfsDedicatedZeroWait");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_DfsDedicatedZeroWait(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_get_DfsZeroWaitDefault(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char save[MAX_KEYWORD_LEN]	= {0};
	char keyword[MAX_KEYWORD_LEN]	= {0};
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	if( WIFIMGR_GET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	sprintf (keyword, "DfsZeroWaitDefault");
	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_get_DfsZeroWaitDefault(type, index, save, MAX_KEYWORD_LEN))
		printf("%s=%s\n",keyword,save);

#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif


cmds_t wifimgr_get_cmd[WIFIMGR_GET_CMD_NUM] = {
	/*	name,				func,	        			 	flags,	 minargu,	            argc_errmsg */
#ifdef TCSUPPORT_CT_WLAN_JOYME3
	{"WPS_STA_MAC",			do_get_WPS_STA_MAC,	  		0,      	2,		"get WPS_STA_MAC"},
#endif
	{"BssidNum",			do_get_ssidnum,	  		 	0,      	2,		"get wifi ssidNum"},
	{"CountryRegion",		do_get_countryRegion,	 	0,			2,		"get wifi countryRegion"},
	{"CountryRegionABand",	do_get_countryRegionABand,	0,			2,		"get wifi countryRegionABand"},
	{"CountryCode",			do_get_countryCode,			0,			2,		"get wifi CountryCode"},
	{"Channel",				do_get_channel,				0,			2,		"get wifi Channel"},
	{"AutoChannelSelect",	do_get_autoChannelSelect,	0,			2,		"get wifi AutoChannelSelect"},
#ifdef TCSUPPORT_WLAN_ACS
	{"ChannelScanScore",    do_get_channelScanScore,    0,          2,      "get wifi ChannelScanScore"},
	{"BestChannel",         do_get_bestChannel,         0,          2,      "get wifi BestChannel"},
#endif
	{"BeaconPeriod",		do_get_beaconPeriod,		0,			2,		"get wifi BeaconPeriod"},
	{"RTSThreshold",		do_get_rtsThreshold,		0,			2,		"get wifi RTSThreshold"},
	{"FragThreshold",		do_get_fragThreshold,		0,			2,		"get wifi FragThreshold"},
	{"DtimPeriod",			do_get_dtimPeriod,			0,			2,		"get wifi DtimPeriod"},
	{"TxPower",				do_get_txPower,				0,			2,		"get wifi TxPower"},
	{"BGProtection",		do_get_bgProtection,		0,			2,		"get wifi BGProtection"},
	{"DisableOLBC",			do_get_disableOLBC,			0,			2,		"get wifi DisableOLBC"},
	{"TxPreamble",			do_get_txPreamble,			0,			2,		"get wifi TxPreamble"},
	{"TxBurst",				do_get_txBurst,				0,			2,		"get wifi TxBurst"},
	{"PktAggregate",		do_get_pktAggregate,		0,			2,		"get wifi PktAggregate"},
	{"TurboRate",			do_get_turboRate,			0,			2,		"get wifi TurboRate"},
	{"NoForwarding",		do_get_noForwarding,		0,			2,		"get wifi NoForwarding"},
	{"NoForwardingBTNBSSID",do_get_noForwardingBTNBSSID,0,			2,		"get wifi NoForwardingBTNBSSID"},
	{"ShortSlot",			do_get_shortSlot,			0,			2,		"get wifi ShortSlot"},
	{"WirelessEvent",		do_get_wirelessEvent,		0,			2,		"get wifi WirelessEvent"},
	{"RekeyInterval",		do_get_rekeyInterval,		0,			2,		"get wifi RekeyInterval"},
	{"RekeyMethod",			do_get_rekeyMethod,			0,			2,		"get wifi RekeyMethod"},
	{"PMKCachePeriod",		do_get_pmkCachePeriod,		0,			2,		"get wifi PMKCachePeriod"},
	{"WirelessMode",		do_get_wirelessMode,		0,			2,		"get wifi WirelessMode"},
	{"BasicRate",			do_get_basicRate,			0,			2,		"get wifi BasicRate"},
	{"HT_BW",				do_get_HT_BW,				0,			2,		"get wifi HT_BW"},
	{"HT_LDPC",				do_get_HT_LDPC,				0,			2,		"get wifi HT_LDPC"},
	{"VHT_BW",				do_get_VHT_BW,				0,			2,		"get wifi VHT_BW"},
	{"VHT_SGI",				do_get_VHT_SGI,				0,			2,		"get wifi VHT_SGI"},
	{"VHT_BW_SIGNAL",		do_get_VHT_BW_SIGNAL,		0,			2,		"get wifi VHT_BW_SIGNAL"},
	{"HT_EXTCHA",			do_get_HT_EXTCHA,			0,			2,		"get wifi HT_EXTCHA"},
	{"HT_OpMode",			do_get_HT_OpMode,			0,			2,		"get wifi HT_OpMode"},
	{"HT_BSSCoexistence",	do_get_HT_BSSCoexistence,	0,			2,		"get wifi HT_BSSCoexistence"},
	{"HT_TxStream",			do_get_HT_TxStream,			0,			2,		"get wifi HT_TxStream"},
	{"HT_RxStream",			do_get_HT_RxStream,			0,			2,		"get wifi HT_RxStream"},
	{"HT_GI",				do_get_HT_GI,				0,			2,		"get wifi HT_GI"},
	{"HT_MCS",				do_get_HT_MCS,				0,			2,		"get wifi HT_MCS"},
	{"HT_AutoBA",			do_get_HT_AutoBA,			0,			2,		"get wifi HT_AutoBA"},
	{"HT_RDG",				do_get_HT_RDG,				0,			2,		"get wifi HT_RDG"},
	{"HT_MpduDensity",		do_get_HT_MpduDensity,		0,			2,		"get wifi HT_MpduDensity"},
	{"HT_BAWinSize",		do_get_HT_BAWinSize,		0,			2,		"get wifi HT_BAWinSize"},
	{"HT_HTC",				do_get_HT_HTC,				0,			2,		"get wifi HT_HTC"},
	{"HT_LinkAdapt",		do_get_HT_LinkAdapt,		0,			2,		"get wifi HT_LinkAdapt"},
	{"HT_AMSDU",			do_get_HT_AMSDU,			0,			2,		"get wifi HT_AMSDU"},
	{"AuthMode",			do_get_authMode,			0,			2,		"get wifi AuthMode"},
	{"TxRate",				do_get_txRate,				0,			2,		"get wifi TxRate"},
	{"PMFMFPC",				do_get_PMFMFPC,				0,			2,		"get wifi PMFMFPC"},
	{"PMFMFPR",				do_get_PMFMFPR,				0,			2,		"get wifi PMFMFPR"},
	{"PMFSHA256",			do_get_PMFSHA256,			0,			2,		"get wifi PMFSHA256"},
	{"DefaultKeyID",		do_get_defaultKeyID,		0,			2,		"get wifi DefaultKeyID"},
	{"WPAPSK",				do_get_WPAPSK,				0,			2,		"get wifi WPAPSK"},
	{"MaxStaNum",			do_get_maxStaNum,			0,			2,		"get wifi MaxStaNum"},
	{"MbssMaxStaNum",		do_get_mbssmaxStaNum,		0,			2,		"get wifi MbssMaxStaNum"},
	{"SSID",				do_get_ssid,				0,			2,		"get wifi SSID"},
	{"Key1Type",			do_get_key1Type,			0,			2,		"get wifi Key1Type"},
	{"Key2Type",			do_get_key2Type,			0,			2,		"get wifi Key2Type"},
	{"Key3Type",			do_get_key3Type,			0,			2,		"get wifi Key3Type"},
	{"Key4Type",			do_get_key4Type,			0,			2,		"get wifi Key4Type"},
	{"Key1Str",				do_get_key1Str,				0,			2,		"get wifi Key1Str"},
	{"Key2Str",				do_get_key2Str,				0,			2,		"get wifi Key2Str"},
	{"Key3Str",				do_get_key3Str,				0,			2,		"get wifi Key3Str"},
	{"Key4Str",				do_get_key4Str,				0,			2,		"get wifi Key4Str"},
	{"EncrypType",			do_get_encrypType,			0,			2,		"get wifi EncrypType"},
	{"PreAuth",				do_get_preAuth,				0,			2,		"get wifi PreAuth"},
	{"HideSSID",			do_get_hideSSID,			0,			2,		"get wifi HideSSID"},
	{"WmmCapable",			do_get_wmmCapable,			0,			2,		"get wifi WmmCapable"},
	{"FixedTxMode",			do_get_fixedTxMode,			0,			2,		"get wifi FixedTxMode"},
	{"APSDCapable",			do_get_apsdCapable,			0,			2,		"get wifi APSDCapable"},
	{"DLSCapable",			do_get_dlsCapable,			0,			2,		"get wifi DLSCapable"},
	{"APAifsn",				do_get_apAifsn,				0,			2,		"get wifi APAifsn"},
	{"APCwmin",				do_get_apCwmin,				0,			2,		"get wifi APCwmin"},
	{"APCwmax",				do_get_apCwmax,				0,			2,		"get wifi APCwmax"},
	{"APTxop",				do_get_apTxop,				0,			2,		"get wifi Txop"},
	{"APACM",				do_get_apACM,				0,			2,		"get wifi APACM"},
	{"BSSAifsn",			do_get_bssAifsn,			0,			2,		"get wifi BSSAifsn"},
	{"BSSCwmin",			do_get_bssCwmin,			0,			2,		"get wifi BSSCwmin"},
	{"BSSCwmax",			do_get_bssCwmax,			0,			2,		"get wifi BSSCwmax"},
	{"BSSTxop",				do_get_bssTxop,				0,			2,		"get wifi BSSTxop"},
	{"BSSACM",				do_get_bssACM,				0,			2,		"get wifi BSSACM"},
	{"AckPolicy",			do_get_ackPolicy,			0,			2,		"get wifi AckPolicy"},
	{"ReloadFlag",			do_get_ReloadFlag,			0,			2,		"get wifi ReloadFlag"},
	{"MacAddress",			do_get_macAddress,			0,			2,		"get wifi MacAddress"},
	{"RADIUS_Server",		do_get_radiussever, 		0,			2,		"get wifi radiusserver"},
	{"RADIUS_Port", 		do_get_radiusport,			0,			2,		"get wifi radiusport"},
	{"RADIUS_Key",			do_get_radiuskey,			0,			2,		"get wifi radiuskey"},
	{"IEEE8021X",			do_get_ieee8021x,			0,			2,		"get wifi ieee8021x"},
	{"session_timeout_interval", do_get_session_timeout_interval,		0,		2,		"get wifi session_timeout_interval"},
			
	{"WdsEnable",			do_get_wdsenable,	   		0,      	2,		"get wifi WdsEnable"},
	{"WdsList",				do_get_wdslist,	   			0,      	2,		"get wifi WdsList"},
	{"WdsEncrypType",		do_get_wdsencryptype,		0,      	2,		"get wifi WdsEncrypType"},
	{"WdsKey",				do_get_wdskey,	   			0,      	2,		"get wifi WdsKey"},
	{"WdsPhyMode",			do_get_wdsphymode,	   		0,      	2,		"get wifi WdsPhyMode"},			
	{"own_ip_addr", 		do_get_ownip,				0,			2,		"get wifi own_ip_addr"},
	{"wscPinCode",			do_get_wscpincode,			0,			2,		"get wifi wscPinCode"},
	{"AccessPolicy",		do_get_accessPolicy,		0,			2,		"get wifi AccessPolicy"},
	{"AccessControlList",	do_get_accessControlList,	0,			2,		"get wifi AccessPolicy"},
	{"WscDefaultSSID",		do_get_wscDefaultSSID,		0,			2,		"get wifi WscDefaultSSID"},
	{"WscV2Support",		do_get_wscV2Support,		0,			2,		"get wifi WscV2Support"},
	

	{"EfuseBufferMode", 	do_get_EfuseBufferMode,		0,			2,		"get wifi EfuseBufferMode"},
	{"IgmpSnEnable", 		do_get_igmpSnEnable,		0,			2,		"get wifi IgmpSnEnable"},
	{"Sta11kCap",			do_get_Sta11kCap,			0,			2,		"get wifi Sta11kCap"},
	{"Sta11vCap",			do_get_Sta11vCap,			0,			2,		"get wifi Sta11vCap"},
	{"AMSDU_NUM",			do_get_AMSDU_NUM,			0,			2,		"get wifi AMSDU_NUM"},
	{"BndstrgChanLoad", 	do_get_bndstrgChanLoad,		0,			2,		"get wifi BndstrgChanLoad"},
#if defined(TCSUPPORT_CMCCV2)
		{"Noise",				do_get_noise,				0,			2,		"get wifi Noise"},
		{"ChanLoad",			do_get_chanLoad,			0,			2,		"get wifi InterferencePercent"},
#endif
#if defined(TCSUPPORT_ECNT_MAP)
	{"MapEnable", 		    do_get_MapEnable,		    0,			2,		"get wifi MapEnable"},
	{"MAP_Turnkey",		    do_get_MAP_Turnkey,		    0,			2,		"get wifi MAP_Turnkey"},
	{"MAP_Ext", 		    do_get_MAP_Ext,		        0,			2,		"get wifi MAP_Ext"},
#endif
#if defined(TCSUPPORT_WLAN_AX)
	{"MuOfdmaDlEnable",			do_get_MuOfdmaDlEnable,			0,			2,		"get wifi MuOfdmaDlEnable"},
	{"MuOfdmaUlEnable", 		do_get_MuOfdmaUlEnable, 		0,			2,		"get wifi MuOfdmaUlEnable"},
	{"MuMimoDlEnable", 			do_get_MuMimoDlEnable, 			0,			2,		"get wifi MuMimoDlEnable"},
	{"MuMimoUlEnable", 			do_get_MuMimoUlEnable, 			0,			2,		"get wifi MuMimoUlEnable"},
	{"SREnable", 			do_get_SREnable, 			0,			2,		"get wifi SREnable"},
	{"SRMode", 			do_get_SRMode, 			0,			2,		"get wifi SRMode"},
	{"SRSDEnable", 			do_get_SRSDEnable, 			0,			2,		"get wifi SRSDEnable"},
	{"PPEnable", 			do_get_PPEnable, 			0,			2,		"get wifi PPEnable"},
	{"TWTSupport", 			do_get_TWTSupport, 			0,			2,		"get wifi TWTSupport"},
	{"HeraStbcPriority", 	do_get_HeraStbcPriority, 			0,			2,		"get wifi HeraStbcPriority"},
	{"MaxRuOfdma", 	do_get_MaxRuOfdma, 			0,			2,		"get wifi MaxRuOfdma"},
	{"MaxDLMuMimo", 	do_get_MaxDLMuMimo, 			0,			2,		"get wifi MaxDLMuMimo"},
	{"MaxULMuMimo", 	do_get_MaxULMuMimo, 			0,			2,		"get wifi MaxULMuMimo"},
	{"Disable160RuMu", 	do_get_Disable160RuMu, 			0,			2,		"get wifi Disable160RuMu"},
	{"Vht1024QamSupport", 	do_get_Vht1024QamSupport, 			0,			2,		"get wifi Vht1024QamSupport"},
	{"DscpPriMapEnable", 	do_get_DscpPriMapEnable, 			0,			2,		"get wifi DscpPriMapEnable"},	
#endif
#if defined(TCSUPPORT_WLAN_APCLIENT)
	{"apcli_status", 		do_get_apcli_status, 		0,			2,		"get apcli status"},
	{"SiteSurvery", 		do_get_SiteSurvery, 		0,			2,		"get site survery"},
	{"Statistics", 			do_get_Statistics, 			0,			2,		"get tatistics"},
#endif
	{"Reload_WlanAC_Driver",do_get_Reload_WlanAC_Driver,0,			2,		"Reload WlanAC Driver"},
	{"version",				do_get_version,	  		 	0,      	2,		"get wifi version"},
	{"ixia5gflag",			do_get_ixia5gflag,	  		0,      	2,		"get wifi ixia5gflag"},
	{"check_wifi_ixia",		do_get_check_wifi_ixia,	 	0,      	2,		"check wifi ixia test."},
	{"wlan_script_attr",	do_get_wlan_script_attr,	0,			2,		"get wlan script attr."},
	{"wlan_attr",			do_get_wlan_attr,			0,			2,		"get wlan attr."},
	{"11AC_CMDflag",		do_get_11AC_CMDflag,		0,			2,		"get _11AC_wifi CMDflag."},
	{"11N_CMDflag",			do_get_11N_CMDflag,			0,			2,		"get _11N_wifi CMDflag."},
	{"execute_wlan_script",	do_get_execute_wlan_script,	0,			2,		"execute wlan script."},
	{"button_state",		do_get_button_state,		0,			2,		"get button state."},
	{"is_WHNAT_support",	do_get_is_WHNAT_support,	0,			2,		"is WHNAT support."},
	{"is_TX_RX_Stream_support",	do_get_is_TX_RX_Stream_support,	0,			2,		"is TX RX Stream support."},
	{"max_sta_num_ui",		do_get_max_sta_num_ui,		0,			2,		"get max sta num ui."},
	{"WLAN_BUTTON_PATH",	do_get_WLAN_BUTTON_PATH,	0,			2,		"get WLAN PUTTON PATH."},
	{"WLAN_MAX_STA_NUM_NAME",do_get_WLAN_MAX_STA_NUM_NAME,	0,			2,		"get WLAN MAX STA NUM NAME."},
	{"CHANNEL", 			do_get_CHANNEL, 			0,			2,		"get CHANNEL"},
	{"CHANNEL_SELECT_DEF",	do_get_CHANNEL_SELECT_DEF,	0,			2,		"get CHANNEL SELECT DEF."},
	{"MESH_TRIGGER_WAIT",	do_get_MESH_TRIGGER_WAIT,	0,			2,		"get MESH TRIGGER WAIT TIME."},
	{"IEEE8021X_VAL",		do_get_IEEE8021X_VAL,		0,			2,		"get IEEE8021X VALUE."},
	{"wlan_entry_attr",		do_get_wlan_entry_attr,		0,			2,		"get wlan entry attr."},
	{"wlan_script_com_attr_config",do_get_wlan_script_com_attr_config,0,2,	"get script com attr config."},
	{"wlan_igmp_snooping_support",do_get_wlan_igmp_snooping_support,0,2,	"get wlan igmp snooping support."},
	{"WPS_BUTTON",			do_get_WPS_BUTTON,			0,			2,		"get WPS BUTTON ."},
	{"WPS_BUTTON_PATH",		do_get_WPS_BUTTON_PATH,		0,			2,		"get WPS BUTTON PATH."},
	{"WPS_5G_BUTTON_PATH",	do_get_WPS_5G_BUTTON_PATH,	0,			2,		"get WPS 5G BUTTON PATH."},
	{"AHB_STATUS",			do_get_AHB_STATUS,			0,			2,		"get AHB_STATUS."},
	{"WLAN11AC_BUTTON",		do_get_WLAN11AC_BUTTON,		0,			2,		"get WLAN11AC_BUTTON."},
	{"WLAN11AC_BUTTON_PATH",do_get_WLAN11AC_BUTTON_PATH,0,			2,		"get WLAN11AC_BUTTON_PATH."},
	{"WPS_STATUS",			do_get_WPS_STATUS,			0,			2,		"get WPS_STATUS."},
	{"itf_hwaddr",			do_get_itf_hwaddr,			0,			2,		"get itf_hwaddr."},
	{"use_global_mac", 		do_get_use_global_mac, 		0,			2,		"get use global mac"},
	{"interface_mac", 		do_get_interface_mac, 		0,			2,		"get interface mac"},
	{"interface_mac0", 		do_get_interface_mac0, 		0,			2,		"get interface mac0"},
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_BUTTONDETECT)
	{"LED_BUTTON_STATUS", 	do_get_LED_BUTTON_STATUS,	0,			2,		"get LED_BUTTON_STATUS"},
#endif/*TCSUPPORT_COMPILE*/

	{"test_itf_status", 	do_get_test_itf_status, 	0,			2,		"get test itf status"},
	{"WLANSHARE_CMD", 		do_get_WLANSHARE_CMD, 		0,			2,		"get WLANSHARE CMD"},
	{"br0_v6addr", 			do_get_br0_v6addr, 			0,			2,		"get br0 v6addr"},
	{"CHIP_ID", 			do_get_CHIP_ID, 			0,			2,		"get CHIP ID"},
	{"WLAN_SCRIPT", 		do_get_WLAN_SCRIPT, 		0,			2,		"get WLAN SCRIPT"},
	{"WIFI_STATS", 			do_get_WIFI_STATS, 			0,			2,		"get WIFI STATS"},
#ifdef TCSUPPORT_WLAN_BNDSTRG
	{"BndStrgChanload", 	do_get_BndStrgChanload, 	0,			2,		"get BndStrgChanload"},
#endif
	{"WpsStatus", 			do_get_WpsStatus, 			0,			2,		"get WpsStatus"},
	{"WPS_PIN_CODE", 		do_get_WPS_PIN_CODE, 		0,			2,		"get WPS PIN CODE"},
#ifdef WSC_AP_SUPPORT
	{"WPS_ACL_INFO", 		do_get_WPS_ACL_INFO, 		0,			2,		"get WPS ACL INFO"},
#endif
	{"ifaddr", 				do_get_ifaddr, 				0,			2,		"get ifaddr"},
	{"CPU_TEMPERATURE", 	do_get_CPU_TEMPERATURE, 	0,			2,		"get CPU_TEMPERATURE"},
	{"ethaddr", 			do_get_ethaddr, 			0,			2,		"get ethaddr"},
	{"img_commit_flag_offset", 	do_get_img_commit_flag_offset, 	0,			2,	"get img_commit_flag_offset"},
	{"img_boot_flag_offset",do_get_img_boot_flag_offset,0,			2,		"get IMG_BOOT_FLAG_OFFSET"},
	{"neighbourinfo", 		do_get_neighbourinfo, 		0,			2,		"get neighboutinfo"},
	{"WPS_info",			do_get_WPSinfo,				0,			2,		"get wps information"},
	{"process_info",		do_get_wifi_process_info,	0,			2,		"get process information"},
#if defined(TCSUPPORT_CT_JOYME2)
	{"ap_count",			do_get_wifi_ap_count,		0,			2,		"get ap count"},
#endif
#if defined(TCSUPPORT_CMCCV2)
	{"wifiStaRssiInfo",		do_get_wifi_wifiStaRssiInfo,	0,			2,		"get wifi Sta Rssi Info"},
#endif
	{"neighbourinfo_v1",		do_get_neighbourinfo_v1,	0,			2,		"get neighbourinfo v1"},
#ifdef TCSUPPORT_CMCCV2
	{"UpdateRadioStatics",		do_get_info_UpdateRadioStatics,	0,			2,		"get info UpdateRadioStatics"},
	{"UpdateNoise",				do_get_info_UpdateNoise,		0,			2,		"get info UpdateNoise"},
	{"UpdateChanLoad",			do_get_info_UpdateChanLoad,		0,			2,		"get info UpdateChanLoad"},
	{"UpdateChanIdle",			do_get_info_UpdateChanIdle,		0,			2,		"get info UpdateChanIdle"},
#endif
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	{"wifi_cap",				do_get_wifi_cap,				0,			2,		"get wifi cap"},
#endif
#if defined(TCSUPPORT_MAP_R2)
	{"MapMode",					do_get_MapMode,					0,			2,		"get MapMode."},
	{"DfsDedicatedZeroWait",	do_get_DfsDedicatedZeroWait,	0,			2,		"get DfsDedicatedZeroWait."},
	{"DfsZeroWaitDefault",		do_get_DfsZeroWaitDefault,		0,			2,		"get DfsZeroWaitDefault."},
#endif
	{"help",				api_lib_help,				0,			0,		"Show wifimgr usage."},
	{NULL, 					NULL, 			         	0, 			0, 		NULL}
};


