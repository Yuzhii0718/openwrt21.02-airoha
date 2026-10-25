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
#ifndef _BLAPI_SYSTEM_BSP_H
#define _BLAPI_SYSTEM_BSP_H

/**
* \file  blapi_system_bsp.h  
* \brief Definiations and BLAPIs related to system runtime status. These APIs are designed for both BSP and SDK users.
* \author Zhengping.Zhang
* \date 2022-06-06
* \version  V1.0.1
* \copyright Airoha Inc                                                              
*/


/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <stdint.h>
#include "trx.h"
#include "ecnt_event_global/ecnt_event_system.h"


/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define SYSTEM_STR_BUF_SIZE 128
#define PRODUCTNAMELEN 32
#define VENDORNAMELEN 32

#define ONUTYPEMASK 0x3
#define COMBOPONMASK 0x4
#define BBF247MASK 0x8
#define ONUMODEMASK 0xf0

#define QDMA_MASK_LAN_PAYLOAD 0x03
#define QDMA_MASK_LAN_DSCP    0x08
#define QDMA_MASK_WAN_PAYLOAD 0x30
#define QDMA_MASK_WAN_DSCP    0x80
#define QDMA_MASK_FAST_WAN    0x100
#define QDMA_MASK_FAST_LAN    0x200
#define QDMA_MASK_FAST_XSI_PCIE0    0x400
#define QDMA_MASK_FAST_XSI_PCIE1    0x800
#define QDMA_MASK_FAST_XSI_USB      0x1000
#define QDMA_MASK_FAST_XSI_ETHER    0x2000
#define QDMA_MASK_FAST_XSI_PON      0x4000
#define QDMA_MASK_FQOS_XSI_PCIE0	0x8000
#define QDMA_MASK_FQOS_XSI_PCIE1    0x10000
#define QDMA_MASK_FQOS_XSI_USB      0x20000
#define QDMA_MASK_FQOS_XSI_ETHER    0x40000
#define QDMA_MASK_FQOS_XSI_PON      0x80000
#define QDMA_MASK_AUTO_XSI_PCIE0	0x100000
#define QDMA_MASK_AUTO_XSI_PCIE1    0x200000
#define QDMA_MASK_AUTO_XSI_USB      0x400000
#define QDMA_MASK_AUTO_XSI_ETHER    0x800000
#define QDMA_MASK_AUTO_XSI_PON      0x1000000

#define QDMA_MASK_WHOLE_VALUE 0xffffffff

#define ETHERLEN 6
#define SNMPSYSOBJLEN 22
#define COUNTRYCODELEN 1

#define DUAL_IMG_BOOT_FLAG_FILE 	"/tmp/dual_image_boot_flag"

typedef enum 
{
	ARCH_ENV = 0x00,
	BAUDRATE_ENV,
	BOARD_ENV,
	BOARD_NAME_ENV,
	BOOTCMD_ENV,
	BOOTDELAY_ENV,
	BOOTFILE_ENV, 
	CONSOLE_ENV,
	COUNTRY_CODE_ENV,
	CPU_ENV,
	DRAM_LIMIT_ENV,
	DSL_GPIO_ENV, 
	ETHACT_ENV, 
	ETHADDR_ENV, 
	ETHER_GPIO_ENV,
	FDT_HIGH_ENV, 
	GPIO_ACTIVE_HIGH_ENV, 
	INTERNET_GPIO_ENV, 
	INVALID_ENV, 
	IPADDR_ENV, 
	KERNEL_FILENAME_ENV, 
	LOADADDR_ENV, 
	MULTI_UPGRADE_GPIO_ENV, 
	ONU_TYPE_ENV, 
	PASSWORD_ENV, 
	POWER_GPIO_ENV, 
	PRODUCT_NAME_ENV, 
	QDMA_INIT_ENV, 
	ROOT_ENV, 
	SDRAM_CONF_ENV, 
	SERDES_SEL_ENV, 
	SERVERIP_ENV, 
	SNMP_SYSOBJID_ENV, 
	SOC_ENV, 
	STDERR_ENV, 
	STDIN_ENV, 
	STDOUT_ENV, 
	UBOOT_FILENAME_ENV,
	USERNAME_ENV,
	VENDOR_ENV, 
	VENDOR_NAME_ENV, 
	HYBRID_ENV,
	ARHT_LOWPOWER_ENV,
	MAX_ENV_NUM,
}env_index;

/************************************************************************
*               M A C R O S
*************************************************************************
*/

/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
typedef struct _dnsfilter_ioctl_cfg_lanip_s
{
	char lanip[16];
	char lanip6[48];
}dnsfilter_ioctl_cfg_lanip_t;
typedef struct _url_info_cfg
{
	unsigned char index;
	unsigned char type;		/*host or path*/
	char host[32];
	char path[32];
	unsigned int blockedtimes;
	char MacAddr[260];
}url_info_cfg;

typedef struct _urlfilter_obj_cfg_data_s
{
	int cmd_type;
	int index;
	int mode; /* 0:black, 1:white. */
	int match_all;
	int action;/* 0:drop, 1:response lanip 2:response name error. */
	int port;
	int times; /* blocktimes */
	char domain[256];
}urlfilter_obj_cfg_data;

typedef struct _urlfilter_ioctl_cfg_data_s
{
	int cmd_type;
	int urlfilter_obj_cnt;
	int clearbtflag;
	urlfilter_obj_cfg_data *obj;
	char mac_addr[20];
}urlfilter_ioctl_cfg_data;

typedef struct _dns_urlfilter_cfg_data_s
{
	int cmd_type;
	int index;
	int mode; /* 0:black, 1:white. */
	int match_all;
	int action;/* 0:drop, 1:response lanip 2:response name error. */
	int times; /* blocktimes */
	char domain[256];
}dns_urlfilter_cfg_data;

typedef struct _dns_urlfilter_cfg_info_s
{
	int index;
	unsigned long blocked_times;
}dns_urlfilter_cfg_info;
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
 * \brief This function do sys mac.
 *
 * This function do do sys mac.. submit by ShengQian.Wang 2020.05.19
 * \param[in]	:mac_addr: new mac addr.
 * \param[in]	:mac_range: mac range.
 * \param[in]	:rebootFlag: 1:need reboot; 0:no need reboot.
  * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do RomReset.
 * \par Example
 * \snippet blapi_system.c do do sys mac.
 */

int blapi_system_doSysMac(unsigned char *mac_addr, unsigned char mac_range, int rebootFlag);

/**
 * \brief This function do Sys Onu type.
 *
 * This function do Sys Onu type. submit by ShengQian.Wang 2020.05.19
 * \param[in]	type: onu type.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do Sys Onu type.
 * \par Example
 * \snippet blapi_system.c  do Sys Onu type.
 */

int blapi_system_doSysOnuType(unsigned char type);

/**
 * \note
 *  This function do Sys bbf247 test enable/disable.
 * \par Example
 * \snippet blapi_system.c  do Sys bbf247 xxx.
 */

int blapi_system_doSysBBF247(unsigned char enable);

/**
 * \note
 *  This function do Sys Combo Pon test enable/disable.
 * \par Example
 * \snippet blapi_system.c  do Sys Combo Pon.
 */

int blapi_system_doSysComboPon(unsigned char enable);

/**
 * \brief This function do Sys SysQdma Init.
 *
 * This function  do Sys SysQdma Init. submit by ShengQian.Wang 2020.05.19
 * \param[in]	qdmaValue: qdma value.
 * \param[in]	qdmaMask: qdma mask.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do Sys SysQdma Init.
 * \par Example
 * \snippet blapi_system.c do Sys SysQdma Init.
 */

int blapi_system_doSysQdmaInit(unsigned char qdmaValue, unsigned char qdmaMask);

/**
 * \brief This function do Sys SysQdma Init,expand qdma init to 32bit.
 *
 * This function  do Sys SysQdma Init. submit by Feijie.Wang 2022.10.10
 * \param[in]	qdmaValue: qdma value.
 * \param[in]	qdmaMask: qdma mask.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do Sys SysQdma Init.
 * \par Example
 * \snippet blapi_system.c do Sys SysQdma Init.
 */
int blapi_system_doSysQdmaInit_int32(unsigned int qdmaValue, unsigned int qdmaMask);

/**
 * \brief This function set boot flag.
 *
 * This function set boot flag. submit by Yifan.Chen 2020.1.17
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set boot flag.
 * \par Example
 * \snippet blapi_system.c set boot flag.
 */

int blapi_system_set_boot_flag_swap(void);

/**
 * \brief This function get current boot flag.
 *
 * This function get current boot flag. submit by Yifan.Chen 2020.1.17
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set get current boot flag.
 * \par Example
 * \snippet blapi_system.c get current boot flag.
 */

int blapi_system_get_current_boot_flag(char *boot_flag);

/**
 * \brief This function checks whether the CRC value of the firmware is correct or not
 * \retval	0	Correct.
 * \retval	-1	Wrong.
 */

int blapi_system_check_crcflag();

/**
 * \brief This function get serdes selection.
 *
 * This function get serdes selection.
 * \param[in]	sel: serdes selection.
 * \retval	0	pon when sel = 0, 
 *				usb3 when sel = 1, 
 *				pcie0 when sel = 2, 
 *				pcie1 when sel = 3, .
 * \retval	1	HSGMII.
 * \retval	-1	Fail.
 * \note
 *  This function get serdes selection.
 * \par Example
 */
int blapi_system_get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel);

/**
 * \brief This function get the temperature of the thermal sensor.
 *
 * This function get the temperature of the thermal sensor. submit by Devin.Deng 2021.06.29
 * \param[out]	the temperature of the thermal sensor.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the temperature of the thermal sensor.
 * \par Example
 * \snippet blapi_system.c Get the temperature of the thermal sensor.
 */

int blapi_system_get_temperature(int* temp_x10);

/**
 * \brief This function get err cnt.
 *
 * This function get err cnt.
 * \param[in]	error_flag: there is err count or not.
 * \param[in]	prbs_error_count: the count of err.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function get err cnt.
 * \par Example
 */
int blapi_system_get_err_cnt(unsigned char *error_flag, uint64_t *prbs_error_count);

/**
 * \brief This function is used to get if secureboot is enabled.
 *
 * This function is used to get if secureboot is enabled
 * \param[in]	 enable
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get if secureboot is enabled.
 * \par Example
 * \snippet blapi_system.c used to get if secureboot is enabled.
 */
int blapi_system_get_secureboot_en(unsigned int *enable);

/**
 * \note
 *  This function do Sys hybrid mode enable/disable.
 * \par Example
 * \snippet blapi_system.c  do Sys hybrid xxx.
 */
int blapi_system_doSysHybrid (unsigned char enable);

#endif /*_BLAPI_SYSTEM_BSP_H*/
