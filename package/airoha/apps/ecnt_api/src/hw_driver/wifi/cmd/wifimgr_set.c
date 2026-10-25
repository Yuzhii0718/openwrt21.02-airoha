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
#include <string.h>
#include <libapi_lib_utility.h>
#include <libapi_lib_wifimgr.h>
#include "wifimgr_set.h"

int
do_set_wifi_level(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	
	if(argc != 2) {	
		printf("The count of arguments is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
		
	char *str = argv[1];
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_dbg_level(str)) {
		printf("Set the debug level failed!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wifi_path(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	if(argc != 3) {	
		printf("The count of arguments is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	wifi_type type =atoi(argv[1]); 
	char *str = argv[2];
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wifi_path(type, str)) {
		printf("Set the datafile path failed!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	return ECNT_WIFIMGR_SUCCESS;	
}

int 
do_set_wifi_ssid(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ssid(type,index,argv[3])) {
		printf("set ssid cfg ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_countryRegion(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_countryRegion(type,index,argv[3])) {
		printf("set CountryRegion value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_countryRegionABand(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_countryRegionABand(type,index,argv[3])) {
		printf("set CountryRegionABand value...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_countryCode(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_countryCode(type,index,argv[3])) {
		printf("set CountryCode value...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_set_channel(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_channel(type,index,argv[3])) {
		printf("set Channel value...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_autoChannelSelect(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_autoChannelSelect(type,index,argv[3])) {
		printf("set AutoChannelSelect value...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

#ifdef TCSUPPORT_WLAN_ACS
int 
do_set_autoChannelSel(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_autoChannelSel(type,index,argv[3])) {
		printf("set AutoChannelSelect value...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

int 
do_set_beaconPeriod(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_beaconPeriod(type,index,argv[3])) {
		printf("set BeaconPeriod value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_rtsThreshold(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_rtsThreshold(type,index,argv[3])) {
		printf("set RTSThreshold value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_fragThreshold(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_fragThreshold(type,index,argv[3])) {
		printf("set FragThreshold value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_dtimPeriod(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_dtimPeriod(type,index,argv[3])) {
		printf("set DtimPeriod value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_txPower(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_txPower(type,index,argv[3])) {
		printf("set TxPower value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_PERCENTAGEenable(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_PERCENTAGEenable(type,index,argv[3])) {
		printf("set PERCENTAGEenable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_bgProtection(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_bgProtection(type,index,argv[3])) {
		printf("set BGProtection value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_disableOLBC(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_disableOLBC(type,index,argv[3])) {
		printf("set DisableOLBC value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_set_txPreamble(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_txPreamble(type,index,argv[3])) {
		printf("set TxPreamble value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_txBurst(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_txBurst(type,index,argv[3])) {
		printf("set TxBurst value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_pktAggregate(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_pktAggregate(type,index,argv[3])) {
		printf("set PktAggregate value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_turboRate(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_turboRate(type,index,argv[3])) {
		printf("set TurboRate value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_noForwardingBTNBSSID(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_noForwardingBTNBSSID(type,index,argv[3])) {
		printf("set NoForwardingBTNBSSID value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_shortSlot(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_shortSlot(type,index,argv[3])) {
		printf("set PktAggregate value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wirelessEvent(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wirelessEvent(type,index,argv[3])) {
		printf("set WirelessEvent value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_set_rekeyInterval(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_rekeyInterval(type,index,argv[3])) {
		printf("set RekeyInterval value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_rekeyMethod(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_rekeyMethod(type,index,argv[3])) {
		printf("set RekeyMethod value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_pmkCachePeriod(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_pmkCachePeriod(type,index,argv[3])) {
		printf("set PMKCachePeriod value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_set_wirelessMode(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wirelessMode(type,index,argv[3])) {
		printf("set WirelessMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_basicRate(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_basicRate(type,index,argv[3])) {
		printf("set BasicRate value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_BW(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_BW(type,index,argv[3])) {
		printf("set HT_BW value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_LDPC(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_LDPC(type,index,argv[3])) {
		printf("set HT_LDPC value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_AMSDU_NUM(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_AMSDU_NUM(type,index,argv[3])) {
		printf("set AMSDU_NUM value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_VHT_BW(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_VHT_BW(type,index,argv[3])) {
		printf("set VHT_BW value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_VHT_SGI(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_VHT_SGI(type,index,argv[3])) {
		printf("set VHT_SGI value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_VHT_LDPC(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_VHT_LDPC(type,index,argv[3])) {
		printf("set VHT_LDPC value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_VHT_STBC(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_VHT_STBC(type,index,argv[3])) {
		printf("set VHT_STBC value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_VHT_BW_SIGNAL(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_VHT_BW_SIGNAL(type,index,argv[3])) {
		printf("set HT_BW_SIGNAL value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_set_HT_EXTCHA(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_EXTCHA(type,index,argv[3])) {
		printf("set HT_EXTCHA value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_OpMode(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_OpMode(type,index,argv[3])) {
		printf("set HT_OpMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_set_HT_BSSCoexistence(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_BSSCoexistence(type,index,argv[3])) {
		printf("set HT_BSSCoexistence value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_set_HT_GI(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_GI(type,index,argv[3])) {
		printf("set HT_GI value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_MCS(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_MCS(type,index,argv[3])) {
		printf("set HT_MCS value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_AutoBA(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_AutoBA(type,index,argv[3])) {
		printf("set HT_AutoBA value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_RDG(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_RDG(type,index,argv[3])) {
		printf("set HT_RDG value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_MpduDensity(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_MpduDensity(type,index,argv[3])) {
		printf("set HT_MpduDensity value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_BAWinSize(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_BAWinSize(type,index,argv[3])) {
		printf("set HT_BAWinSize value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_HTC(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_HTC(type,index,argv[3])) {
		printf("set HT_HTC value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_LinkAdapt(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_LinkAdapt(type,index,argv[3])) {
		printf("set HT_LinkAdapt value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_AMSDU(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_AMSDU(type,index,argv[3])) {
		printf("set HT_AMSDU value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_STBC(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_STBC(type,index,argv[3])) {
		printf("set HT_STBC value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_IEEE80211H(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_IEEE80211H(type,index,argv[3])) {
		printf("set IEEE80211H value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_G_BAND_256QAM(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_G_BAND_256QAM(type,index,argv[3])) {
		printf("set G_BAND_256QAM value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_WHNAT(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_WHNAT(type,index,argv[3])) {
		printf("set WHNAT value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_WOEOFF(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_WOEOFF(type,index,argv[3])) {
		printf("set WOEOFF value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_ProbeHideSSID(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ProbeHideSSID(type,index,argv[3])) {
		printf("set ProbeHideSSID value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_SHORTGIFLAG(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_SHORTGIFLAG(type,index,argv[3])) {
		printf("set SHORTGIFLAG value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_AutoChannelSkipList(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_AutoChannelSkipList(type,index,argv[3])) {
		printf("set AutoChannelSkipList value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_TxStream(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_TxStream(type,index,argv[3])) {
		printf("set HT_TxStream value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_HT_RxStream(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HT_RxStream(type,index,argv[3])) {
		printf("set HT_RxStream value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_ITxBfEn(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ITxBfEn(type,index,argv[3])) {
		printf("set ITxBfEn value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_ETxBfEnCond(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ETxBfEnCond(type,index,argv[3])) {
		printf("set ETxBfEnCond value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_ETxBfIncapable(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ETxBfIncapable(type,index,argv[3])) {
		printf("set ETxBfIncapable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_MUTxRxEnable(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MUTxRxEnable(type,index,argv[3])) {
		printf("set MUTxRxEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_bssidNum(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_bssidNum(type,index,argv[3])) {
		printf("set BssidNum value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_authMode(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_authMode(type,index,argv[3])) {
		printf("set AuthMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_txRate(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_txRate(type,index,argv[3])) {
		printf("set TxRate value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_set_noForwarding(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_noForwarding(type,index,argv[3])) {
		printf("set NoForwarding value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_PMFMFPC(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_PMFMFPC(type,index,argv[3])) {
		printf("set PMFMFPC value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_set_PMFMFPR(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_PMFMFPR(type,index,argv[3])) {
		printf("set PMFMFPR value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_PMFSHA256(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_PMFSHA256(type,index,argv[3])) {
		printf("set PMFSHA256 value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}



int
do_set_defaultKeyID(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_defaultKeyID(type,index,argv[3])) {
		printf("set DefaultKeyID value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_WPAPSK(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_WPAPSK(type,index,argv[3])) {
		printf("set WPAPSK value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_maxStaNum(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_maxStaNum(type,index,argv[3])) {
		printf("set MaxStaNum value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_mbssMaxStaNum(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_mbssMaxStaNum(type,index,argv[3])) {
		printf("set mbssMaxStaNum value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_key1Type(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_key1Type(type,index,argv[3])) {
		printf("set Key1Type value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_key2Type(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_key2Type(type,index,argv[3])) {
		printf("set Key2Type value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_key3Type(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_key3Type(type,index,argv[3])) {
		printf("set Key3Type value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_key4Type(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_key4Type(type,index,argv[3])) {
		printf("set Key4Type value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_key1Str(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_key1Str(type,index,argv[3])) {
		printf("set Key1Str value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_key2Str(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_key2Str(type,index,argv[3])) {
		printf("set Key2Str value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_key3Str(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_key3Str(type,index,argv[3])) {
		printf("set Key3Str value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_key4Str(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_key4Str(type,index,argv[3])) {
		printf("set Key4Str value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_encrypType(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_encrypType(type,index,argv[3])) {
		printf("set EncrypType value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_preAuth(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_preAuth(type,index,argv[3])) {
		printf("set PreAuth value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_hideSSID(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_hideSSID(type,index,argv[3])) {
		printf("set HideSSID value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_ftSupport(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ftSupport(type,index,argv[3])) {
		printf("set ftSupport value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_rrmEnable(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_rrmEnable(type,index,argv[3])) {
		printf("set rrmEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wnmEnable(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wnmEnable(type,index,argv[3])) {
		printf("set wnmEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wmmCapable(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wmmCapable(type,index,argv[3])) {
		printf("set WmmCapable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_fixedTxMode(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_fixedTxMode(type,index,argv[3])) {
		printf("set FixedTxMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int
do_set_apsdCapable(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_apsdCapable(type,index,argv[3])) {
		printf("set APSDCapable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_dlsCapable(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_dlsCapable(type,index,argv[3])) {
		printf("set DLSCapable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_apAifsn(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_apAifsn(type,index,argv[3])) {
		printf("set APAifsn value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_apCwmin(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_apCwmin(type,index,argv[3])) {
		printf("set APCwmin value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_apCwmax(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_apCwmax(type,index,argv[3])) {
		printf("set APCwmax value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_apTxop(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_apTxop(type,index,argv[3])) {
		printf("set APTxop value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_apACM(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_apACM(type,index,argv[3])) {
		printf("set APACM value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_bssAifsn(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_bssAifsn(type,index,argv[3])) {
		printf("set BSSAifsn value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_bssCwmin(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_bssCwmin(type,index,argv[3])) {
		printf("set BSSCwmin value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_bssCwmax(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_bssCwmax(type,index,argv[3])) {
		printf("set BSSCwmax value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_bssTxop(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_bssTxop(type,index,argv[3])) {
		printf("set BSSTxop value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_bssACM(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_bssACM(type,index,argv[3])) {
		printf("set BSSACM value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_ackPolicy(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ackPolicy(type,index,argv[3])) {
		printf("set AckPolicy value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscGetConf(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT-1 != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscGetConf(type,index)) {
		printf("set WscGetConf value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscStop(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT-1 != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscStop(type,index)) {
		printf("set WscStop value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscConfMode(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscConfMode(type,index,argv[3])) {
		printf("set WscConfMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscConfStatus(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscConfStatus(type,index,argv[3])) {
		printf("set WscConfStatus value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscKeyASCII(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscKeyASCII(type,index,argv[3])) {
		printf("set WscKeyASCII value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_WscUUIDE(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_WscUUIDE(type,index,argv[3])) {
		printf("set WscUUIDE value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscV2Support(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscV2Support(type,index,argv[3])) {
		printf("set WscV2Support value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscMaxPinAttack(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscMaxPinAttack(type,index,argv[3])) {
		printf("set WscMaxPinAttack value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscSetupLockTime(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscSetupLockTime(type,index,argv[3])) {
		printf("set WscSetupLockTime value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscMode(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscMode(type,index,argv[3])) {
		printf("set WscMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscPinCode(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscPinCode(type,index,argv[3])) {
		printf("set WscPinCode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscGenPinCode(int argc, char* argv[], void *p)
{	
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscGenPinCode(type,index,argv[3])) {
		printf("set WscPinCode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_wscOOB(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	if(WIFIMGR_SET_CMD_PARA_COUNT -1 != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscOOB(type,index)) {
		printf("set WscOOB value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;	
}



int 
do_set_wifi_radiusserver(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_radiusserver(type,index,argv[3])) {
		printf("set RADIUS_Server value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_radiusport(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_radiusport(type,index,argv[3])) {
		printf("set RADIUS_Port value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_radiuskey(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_radiuskey(type,index,argv[3])) {
		printf("set RADIUS_Key%d value ...fail.\n", index);
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_ieee8021x(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ieee8021x(type,index,argv[3])) {
		printf("set IEEE8021X value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_session_timeout_interval(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_session_timeout_interval(type,index,argv[3])) {
		printf("set session_timeout_interval value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_wdsenable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wdsenable(type,index,argv[3])) {
		printf("set WdsEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_wdslist(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wdslist(type,index,argv[3])) {
		printf("set WdsList value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_wdsencryptype(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wdsencryptype(type,index,argv[3])) {
		printf("set WdsEncrypType value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_wdskey(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wdskey(type,index,argv[3])) {
		printf("set WdsKey value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_wdsphymode(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wdsphymode(type,index,argv[3])) {
		printf("set WdsPhyMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_ownip(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ownip(type,index,argv[3])) {
		printf("set own_ip_addr value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_macaddress(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MacAddress(type,index,argv[3])) {
		printf("set MacAddress value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_accessPolicy(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_accessPolicy(type,index,argv[3])) {
		printf("set AccessPolicy value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_accessControlList(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_accessControlList(type,index,argv[3])) {
		printf("set AccessControlList value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_ReloadFlag(int argc, char* argv[], void *p)
{
#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_reloadFlag(type,argv[3])){
			printf("set ReloadFlag err!\n");
			return ECNT_WIFIMGR_FAILURE;
		}
	
	return ECNT_WIFIMGR_SUCCESS;

}



int 
do_set_wscDefaultSSID(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index 	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_wscDefaultSSID(type,index,argv[3])) {
		printf("set WscDefaultSSID%d value ...fail.\n", index);
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_set_wifi_EfuseBufferMode(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_EfuseBufferMode(type,index,argv[3])) {
		printf("set EfuseBufferMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_igmpSnEnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_igmpSnEnable(type,index,argv[3])) {
		printf("set IgmpSnEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

#if defined(TCSUPPORT_ECNT_MAP)
int 
do_set_MapEnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MapEnable(type,index,argv[3])) {
		printf("set MapEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_MAP_Turnkey(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MAP_Turnkey(type,index,argv[3])) {
		printf("set MAP_Turnkey value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_MAP_Ext(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MAP_Ext(type,index,argv[3])) {
		printf("set MAP_Ext value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

#endif

#if defined(TCSUPPORT_WLAN_AX)
int 
do_set_MuOfdmaDlEnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MuOfdmaDlEnable(type,index,argv[3])) {
		printf("set MuOfdmaDlEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_MuOfdmaUlEnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MuOfdmaUlEnable(type,index,argv[3])) {
		printf("set MuOfdmaUlEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_MuMimoDlEnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MuMimoDlEnable(type,index,argv[3])) {
		printf("set MuMimoDlEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_MuMimoUlEnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MuMimoUlEnable(type,index,argv[3])) {
		printf("set MuMimoUlEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_SREnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_SREnable(type,index,argv[3])) {
		printf("set SREnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_SRMode(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_SRMode(type,index,argv[3])) {
		printf("set SRMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_SRSDEnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_SRSDEnable(type,index,argv[3])) {
		printf("set SRSDEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_PPEnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_PPEnable(type,index,argv[3])) {
		printf("set PPEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_TWTSupport(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_TWTSupport(type,index,argv[3])) {
		printf("set TWTSupport value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_HeraStbcPriority(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_HeraStbcPriority(type,index,argv[3])) {
		printf("set HeraStbcPriority value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_MaxRuOfdma(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MaxRuOfdma(type,index,argv[3])) {
		printf("set MaxRuOfdma value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_MaxDLMuMimo(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MaxDLMuMimo(type,index,argv[3])) {
		printf("set MaxDLMuMimo value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_MaxULMuMimo(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MaxULMuMimo(type,index,argv[3])) {
		printf("set MaxULMuMimo value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_Disable160RuMu(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_Disable160RuMu(type,index,argv[3])) {
		printf("set Disable160RuMu value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_Vht1024QamSupport(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_Vht1024QamSupport(type,index,argv[3])) {
		printf("set Vht1024QamSupport value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

int 
do_set_DscpPriMapEnable(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_DscpPriMapEnable(type,index,argv[3])) {
		printf("set DscpPriMapEnable value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


#if defined(TCSUPPORT_WLAN_APCLIENT)
int 
do_set_SiteSurvery(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_SiteSurvery(type, argv[2])) {
		printf("set TWTSupport value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

int 
do_set_TxBurst_to_file(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	int index	   = atoi(argv[1]);
	int flag	   = atoi(argv[3]);

	char buf[64] = {0};
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_TxBurst_to_file(buf, sizeof(buf), index, argv[2], flag)) {
		printf("set TxBurst to file ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("TxBurst:%s\n", buf);
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_MaxStartNum(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MaxStaNum(type,index,argv[3])) {
		printf("set MaxStaNum value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_ixia5gflag(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	int value	   = atoi(argv[1]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_ixia5gflag(value)) {
		printf("set ixia5gflag value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_11AC_CMDflag(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	int value	   = atoi(argv[1]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_11AC_CMDflag(value)) {
		printf("set 11AC_CMDflag value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_11N_CMDflag(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	int value	   = atoi(argv[1]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_11N_CMDflag(value)) {
		printf("set 11N_CMDflag value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_WifiTxQueue(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	int BssidNum = atoi(argv[1]);
	int len = atoi(argv[2]);
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_WifiTxQueue(BssidNum, len)) {
		printf("set WifiTxQueue value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_iswps5gstart_val(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	int val = atoi(argv[1]);
	int output = 0;
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	wifimgr_lib_set_iswps5gstart_val(&output, val);
	printf("value = %d\n", output);
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_WPS_BUTTON(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_WPS_BUTTON(argv[1])) {
		printf("set WPS_BUTTON value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_WPS_5G_BUTTON(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_WPS_5G_BUTTON(argv[1])) {
		printf("set WPS_5G_BUTTON value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_AHB_STATUS(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char cmd[10] = {0};
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	snprintf(cmd, sizeof(cmd), "%s %s %s", argv[1], argv[2], argv[3]);
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_AHB_STATUS(cmd)) {
		printf("set AHB_STATUS value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_WLAN_WPS_BUTTON(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_WLAN_WPS_BUTTON(argv[1])) {
		printf("set WLAN_WPS_BUTTON value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}


int 
do_set_LED_WLAN(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_LED_WLAN(argv[1])) {
		printf("set LED_WLAN value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_reboot(int argc, char *argv[], void *p)
{
	wifi_type type = atoi(argv[1]);
	char *t = NULL;
	char oldNum[32] = {0};
	int newNum = 0, wdsNum = 0;
	char bssidNum[32] = {0};
	
	if( WIFIMGR_SET_CMD_PARA_COUNT != argc ) {
		printf("The count of parameters is wrong.\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	t = strtok(argv[2], ",");
	if (t)
	{
		snprintf(oldNum, sizeof(oldNum), "%s", t);
	}
	else
	{
		printf("Set oldNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
	
	t = strtok(NULL, ",");
	if (t)
	{
		newNum = atoi(t);
	}
	else
	{
		printf("Set newNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
	
	t = strtok(NULL, ",");
	if (t)
	{
		wdsNum = atoi(t);
	}
	else
	{
		printf("Set wdsNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		snprintf(bssidNum, sizeof(bssidNum), "%s", t);
	}
	else
	{
		printf("Set bssidNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_reboot(type, oldNum, newNum, wdsNum, bssidNum, argv[3]))
		printf("set reboot ...fail.\n");
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_AP2040Rescan(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif

	int index	   = atoi(argv[1]);
	char wlan_bw = 0, wlan_bssconexist = 0;
  	FILE *fp				= NULL;
		
	fp = fopen("/etc/Wireless/RT2860AP/WLAN_exec.sh", "w");
	if(NULL == fp){
		printf("/etc/Wireless/RT2860AP/WLAN_exec.sh write failed\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		fclose(fp);
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_AP2040Rescan(index, &wlan_bw, &wlan_bssconexist, fp))
	{
		printf("set AP2040Rescan value ...fail.\n");
		fclose(fp);
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("wlan_bw = [%d], wlan_bssconexist = [%d]\n", wlan_bw, wlan_bssconexist);
	fclose(fp);
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_restartInterface(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	WLan_info wlan_info;
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
	int nodeid = atoi(argv[2]);
	int resetFlag = atoi(argv[3]);
#endif
	char *t = NULL;
 
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	memset(&wlan_info, 0, sizeof(WLan_info));
	t = strtok(argv[1], ",");
	if (t)
	{
		wlan_info.type = atoi(t);
	}
	else
	{
		printf("Set type failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.oldNum= atoi(t);
	}
	else
	{
		printf("Set oldNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.newNum= atoi(t);
	}
	else
	{
		printf("Set newNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.index= atoi(t);
	}
	else
	{
		printf("Set index failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

#ifdef TCSUPPORT_WLAN_WDS	
	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.wdsActive= atoi(t);
	}
	else
	{
		printf("Set wdsActive failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.isWDSReloadWIFI= atoi(t);
	}
	else
	{
		printf("Set isWDSReloadWIFI failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
#endif

	
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_do_restartInterface(wlan_info, nodeid, resetFlag) )
#else
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_do_restartInterface(wlan_info) )
#endif
	{
		printf("set AP2040Rescan value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_restart_wlan_Interface(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	WLan_info wlan_info, wlan_info_other;
	int nodeid = 0;
	int resetFlag = 0;
	int type = 0;
	char *t = NULL;
	char flag[4] = {0};
 
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	memset(&wlan_info, 0, sizeof(WLan_info));
	t = strtok(argv[1], ",");
	if (t)
	{
		wlan_info.type = atoi(t);
	}
	else
	{
		printf("Set type failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.oldNum= atoi(t);
	}
	else
	{
		printf("Set oldNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.newNum= atoi(t);
	}
	else
	{
		printf("Set newNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.index= atoi(t);
	}
	else
	{
		printf("Set index failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

#ifdef TCSUPPORT_WLAN_WDS	
	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.wdsActive= atoi(t);
	}
	else
	{
		printf("Set wdsActive failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info.isWDSReloadWIFI= atoi(t);
	}
	else
	{
		printf("Set isWDSReloadWIFI failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
#endif

	memset(&wlan_info_other, 0, sizeof(wlan_info_other));
	t = strtok(argv[2], ",");
	if (t)
	{
		wlan_info_other.type = atoi(t);
	}
	else
	{
		printf("Set type failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info_other.oldNum= atoi(t);
	}
	else
	{
		printf("Set oldNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info_other.newNum= atoi(t);
	}
	else
	{
		printf("Set newNum failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info_other.index= atoi(t);
	}
	else
	{
		printf("Set index failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

#ifdef TCSUPPORT_WLAN_WDS	
	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info_other.wdsActive= atoi(t);
	}
	else
	{
		printf("Set wdsActive failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		wlan_info_other.isWDSReloadWIFI= atoi(t);
	}
	else
	{
		printf("Set isWDSReloadWIFI failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
#endif

	t = strtok(argv[3], ",");
	if (t)
	{
		snprintf(flag, sizeof(flag), "%s", t);
	}
	else
	{
		printf("Set flag failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		type = atoi(t);
	}
	else
	{
		printf("Set type failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		nodeid = atoi(t);
	}
	else
	{
		printf("Set nodeid failed,%d!\n",nodeid);
		return ECNT_WIFIMGR_FAILURE;		
	}
	
	t = strtok(NULL, ",");
	if (t)
	{
		resetFlag = atoi(t);
	}
	else
	{
		printf("Set resetFlag failed,%d!\n",resetFlag);
		return ECNT_WIFIMGR_FAILURE;		
	}
#if defined(TCSUPPORT_WLAN_PERSSID_SWITCH_ENHANCE) && (defined(TCSUPPORT_CT_WLAN_JOYME3) || defined(TCSUPPORT_CMCCV2) || defined(TCSUPPORT_NP))
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, type, nodeid, resetFlag) )
#else
	if( ECNT_WIFIMGR_SUCCESS != wifimgr_lib_restart_wlan_Interface(wlan_info, wlan_info_other, flag, type) )
#endif
	{
		printf("set AP2040Rescan value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	
	return ECNT_WIFIMGR_SUCCESS;
}
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
int 
do_set_Diag_Tz(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	diag_TZ_parameter buffer;
	char *t = NULL;
	
	memset(&buffer, 0, sizeof(diag_TZ_parameter));
	t = strtok(argv[2], ",");
	if (t)
	{
		buffer.diag_tz_flag = atoi(t);
	}
	else
	{
		printf("Set diag_tz_flag failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		buffer.diag_tz_hour= atoi(t);
	}
	else
	{
		printf("Set diag_tz_hour failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		buffer.diag_tz_minute= atoi(t);
	}
	else
	{
		printf("Set diag_tz_minute failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_Diag_Tz(argv[1], &buffer, sizeof(diag_TZ_parameter)))
	{
		printf("set Diag_Tz value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

static int chtoi(char c)
{
	if('0' <= c && c <= '9')
		return c - '0';
	else if('a' <= c && c <= 'f')
		return c - 'a' + 10;
	else if('A' <= c && c <= 'F')
		return c - 'A' + 10;
	else
		return 0;
}

int 
do_set_proberesp_delete(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	vsie_probresp_del_cfg_t buffer;
	wifi_type type = atoi(argv[1]);
	int i = 0;

	for(i = 0; i < 6; i++)
	{
		buffer.stamac[i] = chtoi(argv[2][i * 2]) * 16 + chtoi(argv[2][i * 2 + 1]);
	}
	buffer.band = atoi(argv[3]);
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_proberesp_delete(type, &buffer, sizeof(vsie_probresp_del_cfg_t)))
	{
		printf("set proberesp delete ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_proberesp_execute(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	vsie_probresp_set_cfg_t buffer;
	wifi_type type = atoi(argv[1]);
	int i = 0;
	char *t = NULL;

	for(i = 0; i < 6; i++)
	{
		buffer.stamac[i] = chtoi(argv[2][i * 2]) * 16 + chtoi(argv[2][i * 2 + 1]);
	}
	
	t = strtok(argv[3], ",");
	if (t)
	{
		buffer.band = atoi(t);
	}
	else
	{
		printf("Set band failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		snprintf(buffer.IEDATA, sizeof(buffer.IEDATA), "%s", t);
	}
	else
	{
		printf("Set IEDATA failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		buffer.length= atoi(t);
	}
	else
	{
		printf("Set length failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_proberesp_execute(type, &buffer, sizeof(vsie_probresp_set_cfg_t), 1))
	{
		printf("set proberesp execute ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_WIFI_CMD(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char output[128] = {0};
	char *t = NULL;
	char ifr[16] = {0};
	int index = 0;
	int type = atoi(argv[3]);
	
	t = strtok(argv[1], ",");
	if (t)
	{
		snprintf(ifr, sizeof(ifr), "%s", t);
	}
	else
	{
		printf("Set ifr failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}

	t = strtok(NULL, ",");
	if (t)
	{
		index = atoi(t);
	}
	else
	{
		printf("Set index failed!\n");
		return ECNT_WIFIMGR_FAILURE;		
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_WIFI_CMD(ifr, index, sizeof(output), argv[2], output, type))
	{
		printf("set WIFI CMD ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	printf("WIFI CMD = [%s]\n", output);
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_OUI_FILTER(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	char data[20] 	= {0};
	char buf[3] = {0};
	
	buf[0] = chtoi(argv[2][0]) * 16 + chtoi(argv[2][1]);
	buf[1] = chtoi(argv[2][2]) * 16 + chtoi(argv[2][3]);
	buf[2] = chtoi(argv[2][4]) * 16 + chtoi(argv[2][5]);
	snprintf(data,sizeof(data),"%c%c%c",buf[0],buf[1],buf[2]);
	printf("data :%d%d%d \n", buf[0], buf[1], buf[2]);
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_OUI_FILTER(type, data, 3))
	{
		printf("set OUI FILTER ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	return ECNT_WIFIMGR_SUCCESS;
}

#define RT_OID_WSC_PIN_CODE							0x0752

int 
do_set_OidQueryInformation(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	int OidQueryCode_type = atoi(argv[1]);
	unsigned long OidQueryCode = 0;
	char DeviceName[16] = {0};
	char data[32] = {0};

	if (1 == OidQueryCode_type)
	{
		OidQueryCode = RT_OID_WSC_PIN_CODE;
		snprintf(DeviceName, sizeof(DeviceName), "%s", argv[2]);
	}
	else
	{
		printf("OidQueryCode type error\n");
		return ECNT_WIFIMGR_FAILURE;
	}


	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_OidQueryInformation(OidQueryCode, DeviceName, (void *)data, sizeof(data)))
	{
		printf("set OUI FILTER ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	else
	{
		if (1 == OidQueryCode_type)
			printf("data = [%d]\n", *((unsigned int *)(data)));		
	}

	return ECNT_WIFIMGR_SUCCESS;
}

int
do_set_5G_PCIE(int argc, char* argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
		int flag = atoi(argv[1]);
	
		if (1 != flag && 0 != flag)
		{
			printf("flag error\n");
			return ECNT_WIFIMGR_FAILURE;
		}
	
		if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_5G_PCIE(flag))
		{
			printf("set 5G PCIE ...fail.\n");
			return ECNT_WIFIMGR_FAILURE;
		}	
		return ECNT_WIFIMGR_SUCCESS;
}
int 
do_set_PartialScan(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_PartialScan(type)) {
		printf("set PartialScan value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}
int 
do_set_needRebootFlag(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif
	int index = 0;

	if(0 == wifimgr_lib_set_needRebootFlag(&index))
		printf("needRebootFlag  %d\n", index);

	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_insmod_ko_status(int argc, char *argv[], void *p)
{
	#if WIFIMGR_CMD_DBG_SWITCH
		printf("enter %s.\n", __func__);
	#endif

	if(ECNT_WIFIMGR_SUCCESS == wifimgr_lib_insmod_ko())
		printf("insmod ko!\n");


	#if WIFIMGR_CMD_DBG_SWITCH
		printf("exit %s.\n", __func__);
	#endif
	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_EAPifname(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char ifname[16] = {0};
	wifi_type type = atoi(argv[1]);
	snprintf(ifname,sizeof(ifname),"%s",argv[2]);	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_EAPifname(type,-1,ifname)) {
		printf("set EAPifname value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_PreAuthifname(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	char ifname[16] = {0};
	wifi_type type = atoi(argv[1]);
	snprintf(ifname,sizeof(ifname),"%s",argv[2]);	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_PreAuthifname(type,-1,ifname)) {
		printf("set PreAuthifname value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_check_wifi_value(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);	

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_check_wifi_value_befor_write_dat(type))
	{
		printf("set check wifi value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_execute(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int downup = atoi(argv[2]);
	int downup_other = atoi(argv[3]);

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_wifi_execute(type, downup, downup_other))
	{
		printf("set wifi execute ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_start_upnp(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index = atoi(argv[2]);

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_service_wpsstart_wifiupnp(type, index, argv[3]))
	{
		printf("set start upnp ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_wifi_config_oper(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int action = atoi(argv[2]);

	printf("results is %d\n", wifimgr_lib_wifi_config_oper(type, action));

	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_write_wifi_execute_sh(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index = atoi(argv[2]);

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_write_wifi_execute_sh(type, index - 1))
	{
		printf("set wifi config oper ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_service_stop_wps(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index = atoi(argv[2]);

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_service_stop_wps((unsigned char *)argv[3], type, index - 1))
	{
		printf("set service stop wps ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}
int 
do_set_service_start_wps(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index = atoi(argv[2]);
	char *t = NULL;
	wps_info wpsinfo;
	char fileName[64] = {0};

	memset(&wpsinfo, 0, sizeof(wps_info));

	t = strtok(argv[3], ",");
	if (t)
	{
		snprintf(fileName, sizeof(fileName), "%s", t);
	}
	else
	{
		printf("fileName is not exist\n");
		return ECNT_WIFIMGR_FAILURE;	
	}
	
	t = strtok(NULL, ",");
	if(t)
	{
		wpsinfo.wpsMode = atoi(t);
		printf("wpsMode = %d\n", wpsinfo.wpsMode);
	}
	else
	{
		printf("wpsMode is not exist\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	t = strtok(NULL, ",");
	if(t)
	{
		sscanf(t, "%u", &(wpsinfo.wpsPinCode));
		printf("wpsPinCode = %u\n", wpsinfo.wpsPinCode);
	}
	else
	{
		printf("wpsPinCode is not exist\n");
		return ECNT_WIFIMGR_FAILURE;
	}

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_service_start_wps((unsigned char *)fileName, type, index - 1, &wpsinfo))
	{
		printf("set service start wps ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_service_genpincode(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index = atoi(argv[2]);

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_service_genpincode((unsigned char *)argv[3], type, index - 1))
	{
		printf("set service genpincode ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_service_resetoob(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index = atoi(argv[2]);

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_service_resetoob((unsigned char *)argv[3], type, index - 1))
	{
		printf("set service resetoob ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
int 
do_set_preinit(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);

	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_preinit(type))
	{
		printf("set preinit ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_multichip_init(int argc, char *argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif

	wifimgr_lib_multichip_init();

	printf("success");
	return ECNT_WIFIMGR_SUCCESS;
}

#endif

#if defined(TCSUPPORT_MAP_R2)
int 
do_set_MapMode(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_MapMode(type,index,argv[3])) {
		printf("set MapMode value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_DfsDedicatedZeroWait(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_DfsDedicatedZeroWait(type,index,argv[3])) {
		printf("set DfsDedicatedZeroWait value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}

int 
do_set_DfsZeroWaitDefault(int argc, char* argv[], void *p)
{	
#if WIFIMGR_CMD_DBG_SWITCH
	printf("enter %s.\n", __func__);
#endif
	wifi_type type = atoi(argv[1]);
	int index	   = atoi(argv[2]);
	
	if(WIFIMGR_SET_CMD_PARA_COUNT != argc) {	
		printf("The count of parameters is wrong!\n");
		return ECNT_WIFIMGR_FAILURE;
	}
	
	if(ECNT_WIFIMGR_SUCCESS != wifimgr_lib_set_DfsZeroWaitDefault(type,index,argv[3])) {
		printf("set TWTSupport value ...fail.\n");
		return ECNT_WIFIMGR_FAILURE;
	}	
	return ECNT_WIFIMGR_SUCCESS;
}
#endif

cmds_t wifimgr_set_cmd[WIFIMGR_SET_CMD_NUM] = {
	/*	name,					func,					flags,min argnum,	argc_errmsg */
	{"level",				do_set_wifi_level,			0,		1,		"Set debug level"},
	{"path",				do_set_wifi_path,			0,		2,		"Set wifi datafile path"},
	{"SSID",				do_set_wifi_ssid,			0,		3,		"Set wifi ssid"},
	{"CountryRegion",		do_set_countryRegion,		0,		3,		"Set wifi CountryRegion"},
	{"CountryRegionABand",	do_set_countryRegionABand,	0,		3,		"Set wifi CountryRegionABand"},
	{"CountryCode",			do_set_countryCode,			0,		3,		"Set wifi CountryCode"},
	{"Channel",				do_set_channel,				0,		3,		"Set wifi Channel"},
	{"AutoChannelSelect",	do_set_autoChannelSelect,	0,		3,		"Set wifi Channel"},
#ifdef TCSUPPORT_WLAN_ACS
	{"AutoChannelSel",		do_set_autoChannelSel,		0,		3,		"Set wifi Channel"},//iwpriv set AutoChannelSel
#endif
	{"BeaconPeriod",		do_set_beaconPeriod,		0,		3,		"Set wifi BeaconPeriod"},
	{"RTSThreshold",		do_set_rtsThreshold,		0,		3,		"Set wifi RTSThreshold"},
	{"FragThreshold",		do_set_fragThreshold,		0,		3,		"Set wifi FragThreshold"},
	{"DtimPeriod",			do_set_dtimPeriod,			0,		3,		"Set wifi DtimPeriod"},
	{"TxPower",				do_set_txPower,				0,		3,		"Set wifi TxPower"},
	{"PERCENTAGEenable",	do_set_PERCENTAGEenable,	0,		3,		"Set wifi PERCENTAGEenable"},
	{"BGProtection",		do_set_bgProtection,		0,		3,		"Set wifi BGProtection"},
	{"DisableOLBC",			do_set_disableOLBC,			0,		3,		"Set wifi DisableOLBC"},
	{"TxPreamble",			do_set_txPreamble,			0,		3,		"Set wifi TxPreamble"},
	{"TxBurst",				do_set_txBurst,				0,		3,		"Set wifi TxBurst"},
	{"PktAggregate",		do_set_pktAggregate,		0,		3,		"Set wifi PktAggregate"},
	{"TurboRate",			do_set_turboRate,			0,		3,		"Set wifi TurboRate"},
	{"NoForwarding",		do_set_noForwarding,		0,		3,		"Set wifi NoForwarding"},
	{"NoForwardingBTNBSSID",do_set_noForwardingBTNBSSID,0,		3,		"Set wifi NoForwardingBTNBSSID"},
	{"ShortSlot",			do_set_shortSlot,			0,		3,		"Set wifi ShortSlot"},
	{"WirelessEvent",		do_set_wirelessEvent,		0,		3,		"Set wifi WirelessEvent"},
	{"RekeyInterval",		do_set_rekeyInterval,		0,		3,		"Set wifi RekeyInterval"},
	{"RekeyMethod",			do_set_rekeyMethod,			0,		3,		"Set wifi RekeyMethod"},
	{"PMKCachePeriod",		do_set_pmkCachePeriod,		0,		3,		"Set wifi PMKCachePeriod"},	
	{"WirelessMode",		do_set_wirelessMode,		0,		3,		"Set wifi WirelessMode"},
	{"BasicRate",			do_set_basicRate,			0,		3,		"Set wifi BasicRate"},
	{"HT_BW",				do_set_HT_BW,				0,		3,		"Set wifi HT_BW"},
	{"HT_LDPC",				do_set_HT_LDPC,				0,		3,		"Set wifi HT_LDPC"},
	{"AMSDU_NUM",			do_set_AMSDU_NUM,			0,		3,		"Set wifi AMSDU_NUM"},
	{"VHT_BW",				do_set_VHT_BW,				0,		3,		"Set wifi VHT_BW"},
	{"VHT_SGI",				do_set_VHT_SGI,				0,		3,		"Set wifi VHT_SGI"},
	{"VHT_LDPC",			do_set_VHT_LDPC,			0,		3,		"Set wifi VHT_LDPC"},
	{"VHT_STBC",			do_set_VHT_STBC,			0,		3,		"Set wifi VHT_STBC"},
	{"VHT_BW_SIGNAL",		do_set_VHT_BW_SIGNAL,		0,		3,		"Set wifi VHT_BW_SIGNAL"},
	{"HT_EXTCHA",			do_set_HT_EXTCHA,			0,		3,		"Set wifi HT_EXTCHA"},
	{"HT_OpMode",			do_set_HT_OpMode,			0,		3,		"Set wifi HT_OpMode"},
	{"HT_BSSCoexistence",	do_set_HT_BSSCoexistence,	0,		3,		"Set wifi HT_BSSCoexistence"},
	{"HT_GI",				do_set_HT_GI,				0,		3,		"Set wifi HT_GI"},
	{"HT_MCS",				do_set_HT_MCS,				0,		3,		"Set wifi HT_MCS"},
	{"HT_AutoBA",			do_set_HT_AutoBA,			0,		3,		"Set wifi HT_AutoBA"},
	{"HT_RDG",				do_set_HT_RDG,				0,		3,		"Set wifi HT_RDG"},
	{"HT_MpduDensity",		do_set_HT_MpduDensity,		0,		3,		"Set wifi HT_MpduDensity"},
	{"HT_BAWinSize",		do_set_HT_BAWinSize,		0,		3,		"Set wifi HT_BAWinSize"},
	{"HT_HTC",				do_set_HT_HTC,				0,		3,		"Set wifi HT_HTC"},
	{"HT_LinkAdapt",		do_set_HT_LinkAdapt,		0,		3,		"Set wifi HT_LinkAdapt"},
	{"HT_AMSDU",			do_set_HT_AMSDU,			0,		3,		"Set wifi HT_AMSDU"},
	{"HT_STBC",				do_set_HT_STBC,				0,		3,		"Set wifi HT_STBC"},
	{"IEEE80211H",			do_set_IEEE80211H,			0,		3,		"Set wifi IEEE80211H"},
	{"G_BAND_256QAM",		do_set_G_BAND_256QAM,		0,		3,		"Set wifi G_BAND_256QAM"},
	{"WHNAT",				do_set_WHNAT,				0,		3,		"Set wifi WHNAT"},
	{"ProbeHideSSID",		do_set_ProbeHideSSID,		0,		3,		"Set wifi ProbeHideSSID"},
	{"SHORTGIFLAG",			do_set_SHORTGIFLAG,			0,		3,		"Set wifi SHORTGIFLAG"},
	{"AutoChannelSkipList",	do_set_AutoChannelSkipList,	0,		3,		"Set wifi AutoChannelSkipList"},
	{"HT_TxStream",			do_set_HT_TxStream,			0,		3,		"Set wifi HT_TxStream"},
	{"HT_RxStream",			do_set_HT_RxStream,			0,		3,		"Set wifi HT_RxStream"},
	{"ITxBfEn",				do_set_ITxBfEn,				0,		3,		"Set wifi ITxBfEn"},
	{"ETxBfEnCond",			do_set_ETxBfEnCond,			0,		3,		"Set wifi ETxBfEnCond"},
	{"ETxBfIncapable",		do_set_ETxBfIncapable,		0,		3,		"Set wifi ETxBfIncapable"},
	{"MUTxRxEnable",		do_set_MUTxRxEnable,		0,		3,		"Set wifi MUTxRxEnable"},
	{"BssidNum",			do_set_bssidNum,			0,		3,		"Set wifi BssidNum"},
	{"AuthMode",			do_set_authMode,			0,		3,		"Set wifi AuthMode"},
	{"TxRate",				do_set_txRate,				0,		3,		"Set wifi TxRate"},
	{"PMFMFPC",				do_set_PMFMFPC,				0,		3,		"Set wifi PMFMFPC"},
	{"PMFMFPR",				do_set_PMFMFPR,				0,		3,		"Set wifi PMFMFPR"},
	{"PMFSHA256",			do_set_PMFSHA256,			0,		3,		"Set wifi PMFSHA256"},
	{"DefaultKeyID",		do_set_defaultKeyID,		0,		3,		"Set wifi DefaultKeyID"},
	{"WPAPSK",				do_set_WPAPSK,				0,		3,		"Set wifi WPAPSK"},
	{"MaxStaNum",			do_set_maxStaNum,			0,		3,		"Set wifi MaxStaNum"},
	{"MbssMaxStaNum",		do_set_mbssMaxStaNum,		0,		3,		"Set wifi MaxStaNum"},
	{"Key1Type",			do_set_key1Type,			0,		3,		"Set wifi Key1Type"},
	{"Key2Type",			do_set_key2Type,			0,		3,		"Set wifi Key2Type"},
	{"Key3Type",			do_set_key3Type,			0,		3,		"Set wifi Key3Type"},
	{"Key4Type",			do_set_key4Type,			0,		3,		"Set wifi Key4Type"},
	{"Key1Str",				do_set_key1Str,				0,		3,		"Set wifi Key1Str"},
	{"Key2Str",				do_set_key2Str,				0,		3,		"Set wifi Key2Str"},
	{"Key3Str",				do_set_key3Str,				0,		3,		"Set wifi Key3Str"},
	{"Key4Str",				do_set_key4Str,				0,		3,		"Set wifi Key4Str"},
	{"EncrypType",			do_set_encrypType,			0,		3,		"Set wifi EncrypType"},
	{"PreAuth",				do_set_preAuth,				0,		3,		"Set wifi PreAuth"},
	{"HideSSID",			do_set_hideSSID,			0,		3,		"Set wifi HideSSID"},
	{"FtSupport",			do_set_ftSupport,			0,		3,		"Set wifi ftSupport"},
	{"RrmEnable",			do_set_rrmEnable,			0,		3,		"Set wifi rrmEnable"},
	{"WnmEnable",			do_set_wnmEnable,			0,		3,		"Set wifi wnmEnable"},
	{"WmmCapable",			do_set_wmmCapable,			0,		3,		"Set wifi WmmCapable"},
	{"FixedTxMode",			do_set_fixedTxMode,			0,		3,		"Set wifi FixedTxMode"},
	{"APSDCapable",			do_set_apsdCapable,			0,		3,		"Set wifi APSDCapable"},
	{"DLSCapable",			do_set_dlsCapable,			0,		3,		"Set wifi DLSCapable"},
	{"APAifsn",				do_set_apAifsn,				0,		3,		"Set wifi APAifsn"},
	{"APCwmin",				do_set_apCwmin,				0,		3,		"Set wifi APCwmin"},
	{"APCwmax",				do_set_apCwmax,				0,		3,		"Set wifi APCwmax"},
	{"APTxop",				do_set_apTxop,				0,		3,		"Set wifi APTxop"},
	{"APACM",				do_set_apACM,				0,		3,		"Set wifi APACM"},
	{"BSSAifsn",			do_set_bssAifsn,			0,		3,		"Set wifi BSSAifsn"},
	{"BSSCwmin",			do_set_bssCwmin,			0,		3,		"Set wifi BSSCwmin"},
	{"BSSCwmax",			do_set_bssCwmax,			0,		3,		"Set wifi BSSCwmax"},
	{"BSSTxop",				do_set_bssTxop,				0,		3,		"Set wifi BSSTxop"},
	{"BSSACM",				do_set_bssACM,				0,		3,		"Set wifi BSSACM"},
	{"AckPolicy",			do_set_ackPolicy,			0,		3,		"Set wifi AckPolicy"},
	{"WscGetConf",			do_set_wscGetConf,			0,		2,		"Set wifi WPS WscGetConf"},
	{"WscStop",				do_set_wscStop,				0,		2,		"Set wifi WPS WscStop"},
	{"WscConfMode",			do_set_wscConfMode,			0,		3,		"Set wifi WPS WscConfMode"},
	{"WscConfStatus",		do_set_wscConfStatus,		0,		3,		"Set wifi WPS WscConfStatus"},
	{"WscKeyASCII",			do_set_wscKeyASCII,			0,		3,		"Set wifi WPS WscKeyASCII"},
	{"WscUUIDE",			do_set_WscUUIDE,			0,		3,		"Set wifi WPS WscUUIDE"},
	{"WscV2Support",		do_set_wscV2Support,		0,		3,		"Set wifi WPS WscV2Support"},
	{"WscMaxPinAttack",		do_set_wscMaxPinAttack,		0,		3,		"Set wifi WPS WscMaxPinAttack"},
	{"WscSetupLockTime",	do_set_wscSetupLockTime,	0,		3,		"Set wifi WPS WscSetupLockTime"},
	{"WscMode",				do_set_wscMode,				0,		3,		"Set wifi WPS WscMode"},
	{"WscPinCode",			do_set_wscPinCode,			0,		3,		"Set wifi WPS WscPinCode"},
	{"WscGenPinCode",		do_set_wscGenPinCode,		0,		3,		"Set wifi WPS WscGenPinCode"},
	{"WscOOB",              do_set_wscOOB,              0,      2,      "Set wifi WPS WscOOB" },	
	{"RADIUS_Server",   	do_set_wifi_radiusserver,   0,      3,      "Set wifi radiusserver"},
    {"RADIUS_Port",   	    do_set_wifi_radiusport,   	0,      3,      "Set wifi radiusport"},
	{"RADIUS_Key",		    do_set_wifi_radiuskey,		0,		3,		"Set wifi radiuskey"},
	{"IEEE8021X",			do_set_wifi_ieee8021x,		0,		3,		"Set wifi ieee8021x"},
	{"session_timeout_interval", do_set_wifi_session_timeout_interval,		0,		3,		"Set wifi session_timeout_interval"},		
	{"WdsEnable",			do_set_wifi_wdsenable,		0,		3,		"Set wifi WdsEnable"},	
	{"WdsList",				do_set_wifi_wdslist,		0,		3,		"Set wifi WdsList"},
	{"WdsEncrypType",		do_set_wifi_wdsencryptype,	0,		3,		"Set wifi WdsEncrypType"},
	{"WdsKey",				do_set_wifi_wdskey,			0,		3,		"Set wifi WdsKey"},
	{"WdsPhyMode",			do_set_wifi_wdsphymode,		0,		3,		"Set wifi WdsPhyMode"},		
	{"own_ip_addr",			do_set_wifi_ownip,			0,		3,		"Set wifi own_ip_addr"},
	{"MacAddress",			do_set_wifi_macaddress, 	0,		3,		"Set wifi MacAddress"},
	{"AccessPolicy",		do_set_accessPolicy, 		0,		3,		"Set wifi AccessPolicy"},
	{"AccessControlList",	do_set_accessControlList, 	0,		3,		"Set wifi AccessControlList"},
	{"WscDefaultSSID",		do_set_wscDefaultSSID,  	0,		3,		"Set wifi WscDefaultSSID"},
	{"ReloadFlag",		    do_set_ReloadFlag,			0,		3,		"Set wifi ReloadFlag"},

	{"EfuseBufferMode", 	do_set_wifi_EfuseBufferMode,0,		3,		"Set wifi EfuseBufferMode"},
	{"IgmpSnEnable", 		do_set_igmpSnEnable,		0,		3,		"Set wifi IgmpSnEnable"},
#if defined(TCSUPPORT_ECNT_MAP)
	{"MapEnable", 		    do_set_MapEnable,		    0,		3,		"Set wifi MapEnable"},
	{"MAP_Turnkey",		    do_set_MAP_Turnkey,		    0,		3,		"Set wifi MAP_Turnkey"},
	{"MAP_Ext", 		    do_set_MAP_Ext,		        0,		3,		"Set wifi MAP_Ext"},
#endif
#if defined(TCSUPPORT_WLAN_AX)
	{"MuOfdmaDlEnable", 		do_set_MuOfdmaDlEnable, 		0,			3,		"set wifi MuOfdmaDlEnable"},
	{"MuOfdmaUlEnable", 		do_set_MuOfdmaUlEnable, 		0,			3,		"set wifi MuOfdmaUlEnable"},
	{"MuMimoDlEnable",			do_set_MuMimoDlEnable,			0,			3,		"set wifi MuMimoDlEnable"},
	{"MuMimoUlEnable",			do_set_MuMimoUlEnable,			0,			3,		"set wifi MuMimoUlEnable"},
	{"SREnable",			do_set_SREnable,			0,			3,		"set wifi SREnable"},
	{"PPEnable",			do_set_PPEnable,			0,			3,		"set wifi PPEnable"},
	{"TWTSupport",			do_set_TWTSupport,			0,			3,		"set wifi TWTSupport"},
	{"HeraStbcPriority",			do_set_HeraStbcPriority,			0,			3,		"set wifi MaxRuOfdma"},
	{"MaxRuOfdma",			do_set_MaxRuOfdma,			0,			3,		"set wifi HeraStbcPriority"}, 
	{"MaxDLMuMimo",			do_set_MaxDLMuMimo,			0,			3,		"set wifi MaxDLMuMimo"}, 
	{"MaxULMuMimo",			do_set_HeraStbcPriority,			0,			3,		"set wifi HeraStbcPriority"}, 
	{"Disable160RuMu",		do_set_Disable160RuMu,			0,			3,		"set wifi Disable160RuMu"}, 
	{"Vht1024QamSupport",	do_set_Vht1024QamSupport,			0,			3,		"set wifi Vht1024QamSupport"}, 
	{"DscpPriMapEnable",	do_set_DscpPriMapEnable,			0,			3,		"set wifi DscpPriMapEnable"}, 	
#endif
#if defined(TCSUPPORT_WLAN_APCLIENT)
	{"SiteSurvery",			do_set_SiteSurvery,			0,			3,		"set SiteSurvery"},
#endif
	{"TxBurst_to_file", do_set_TxBurst_to_file,		0,		3,		"set TxBurst to file."},
	{"MaxStartNum", 		do_set_MaxStartNum,			0,		3,		"set wifi MaxStaNum."},
	{"ixia5gflag",		do_set_ixia5gflag,			0,		3,		"set wifi ixia5gflag."},
	{"11AC_CMDflag",	do_set_11AC_CMDflag,		0,		3,		"set _11AC_CMDflag."},
	{"11N_CMDflag",		do_set_11N_CMDflag,			0,		3,		"set _11N_CMDflag."},
	{"WifiTxQueue",		do_set_WifiTxQueue,			0,		3,		"set wifi tx queue."},
	{"iswps5gstart_val",do_set_iswps5gstart_val,	0,		3,		"setiswps5gstart value."},
	{"WPS_BUTTON",		do_set_WPS_BUTTON,			0,		3,		"set WPS BUTTON."},
	{"WPS_5G_BUTTON",	do_set_WPS_5G_BUTTON,		0,		3,		"set WPS 5G BUTTON."},
	{"AHB_STATUS",		do_set_AHB_STATUS,			0,		3,		"set AHB STATUS."},
	{"WLAN_WPS_BUTTON",	do_set_WLAN_WPS_BUTTON,		0,		3,		"set WLAN WPS BUTTON."},
	{"LED_WLAN",		do_set_LED_WLAN,			0,		3,		"set LED_WLAN."},
	{"reboot",				do_set_reboot,				0,			3,		"set wifi reboot"},
	{"AP2040Rescan",		do_set_AP2040Rescan,		0,			3,		"set wifi AP2040Rescan"},
	{"restartInterface",	do_set_restartInterface,	0,			3,		"set wifi restartInterface"},
	{"restart_wlan_Interface",	do_set_restart_wlan_Interface,	0,			3,		"set restart wlan Interface"},
#if defined(TCSUPPORT_CT_WLAN_JOYME3)
	{"Diag_Tz",				do_set_Diag_Tz,				0,			3,		"set Diag Tz"},
#endif
	{"proberesp_delete",	do_set_proberesp_delete,	0,			3,		"set proberesp delete"},
	{"proberesp_execute",	do_set_proberesp_execute,	0,			3,		"set proberesp execute"},
	{"WIFI_CMD",			do_set_WIFI_CMD,			0,			3,		"set WIFI CMD"},
	{"OUI_FILTER",			do_set_OUI_FILTER,			0,			3,		"set OUI FILTER"},
	{"OidQueryInformation", do_set_OidQueryInformation,	0,			3,		"set OidQueryInformation"},
	{"5G_PCIE",				do_set_5G_PCIE,				0,			3,		"set 5G PCIE"},
	{"PartialScan",			do_set_PartialScan,			0,			3,		"set wifi PartialScan"},
        {"needRebootFlag",		do_set_needRebootFlag,		0,			3,		"set needRebootFlag"},
	{"insmod_ko",			do_set_insmod_ko_status,	0,			3,		"set ko status"},
	{"EAPifname",			do_set_EAPifname,			0,			3,		"set EAPifname"},
	{"PreAuthifname",		do_set_PreAuthifname,		0,			3,		"set PreAuthifname"},

	{"check_wifi_value",		do_set_check_wifi_value,		0,			3,		"set check wifi value"},
	{"wifi_execute",			do_set_wifi_execute,			0,			3,		"set wifi execute"},
	{"start_upnp",				do_set_start_upnp,				0,			3,		"set start upnp"},
	{"wifi_config_oper",		do_set_wifi_config_oper,		0,			3,		"set wifi config oper"},
	{"write_wifi_execute_sh",	do_set_write_wifi_execute_sh,	0,			3,		"set write wifi execute sh"},
	{"service_stop_wps",		do_set_service_stop_wps,		0,			3,		"set service stop wps"},
	{"service_start_wps",		do_set_service_start_wps,		0,			3,		"set service start wps"},
	{"service_genpincode",		do_set_service_genpincode,		0,			3,		"set service genpincode"},
	{"service_resetoob",		do_set_service_resetoob,		0,			3,		"set service resetoob"},
#ifdef TCSUPPORT_WLAN_MULTI_CHIP
	{"preinit",					do_set_preinit,					0,			3,		"set preinit"},
	{"multichip_init",			do_set_multichip_init,			0,			3,		"set multichip init"},
#endif
#if defined(TCSUPPORT_MAP_R2)
	{"MapMode", 				do_set_MapMode, 				0,			3,		"set MapMode."},
	{"DfsDedicatedZeroWait",	do_set_DfsDedicatedZeroWait,	0,			3,		"set DfsDedicatedZeroWait."},
	{"DfsZeroWaitDefault",		do_set_DfsZeroWaitDefault,		0,			3,		"set DfsZeroWaitDefault."},
#endif

	{"help",				api_lib_help,				0,		0,		"Show gponmgr usage."},
	{NULL, 					NULL, 						0, 		0, 		NULL}
};	

/**************************************************
*
**************************************************/


