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
#ifndef _BLAPI_SYSTEM_CMD_H
#define _BLAPI_SYSTEM_CMD_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*               M A C R O S
*************************************************************************
*/

/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/

typedef struct _urlfilter_ioctl_info_s
{
	int index;
	unsigned long blocked_times;
}urlfilter_ioctl_info;

#define URL_TYPE_HOST	1
#define URL_TYPE_PATH	2

/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/
/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

int do_blapi_system_cmd(int argc,char* argv[],void *p);
int do_blapi_system_get_EtherAddr(int argc, char *argv[], void *p);
int do_blapi_system_get_serdes_sel(int argc, char *argv[], void *p);
int do_blapi_system_set_serdes_sel(int argc, char *argv[], void *p);
int do_blapi_system_set_serdes_sel_v2(int argc, char *argv[], void *p);
int do_blapi_system_set_bootloader(int argc, char *argv[], void *p);
int do_blapi_system_get_Value_from_file(int argc, char *argv[], void *p);
int do_blapi_system_get_sn_offset_L(int argc, char *argv[], void *p);
int do_blapi_system_get_tcboot_size(int argc, char *argv[], void *p);
int do_blapi_system_check_WifiDevice(int argc, char *argv[], void *p);
int do_blapi_system_get_UpLinkValue(int argc, char *argv[], void *p);
int do_blapi_system_get_cpu_default_affinity(int argc, char *argv[], void *p);
int do_blapi_system_get_max_receive_file_size(int argc, char *argv[], void *p);
int do_blapi_system_set_lan_led(int argc, char *argv[], void *p);
int do_blapi_system_get_clearHWNAT_str(int argc, char *argv[], void *p);
int do_blapi_system_set_boot_flag(int argc, char *argv[], void *p);
int do_blapi_system_get_mtdsize(int argc, char *argv[], void *p);
int do_blapi_system_vpn_free_ip_hashmap(int argc, char *argv[], void *p);
int do_blapi_system_set_urlfilter_enable(int argc, char *argv[], void *p);
int do_blapi_system_set_urlfilter_mode(int argc, char *argv[], void *p);
int do_blapi_system_set_urlfilter_url(int argc, char *argv[], void *p);
int do_blapi_system_create_mac_conf(int argc, char *argv[], void *p);
int do_blapi_system_get_eth0_mac(int argc, char *argv[], void *p);
int do_blapi_system_get_temperature(int argc, char *argv[], void *p);
int do_blapi_system_check_secure_tclinux(int argc, char *argv[], void *p);
int do_blapi_system_get_err_cnt(int argc, char *argv[], void *p);
int do_blapi_system_get_spf_linkup(int argc, char *argv[], void *p);
int do_blapi_system_get_crc(int argc, char *argv[], void *p);
int do_blapi_system_client_command(int argc, char *argv[], void *p);
int do_blapi_system_prepare_interface(int argc, char *argv[], void *p);
int do_blapi_system_del_prepared_interface(int argc, char *argv[], void *p);
int do_blapi_system_set_wan_mode(int argc, char *argv[], void *p);

#endif /*_BLAPI_SYSTEM_CMD_H*/
