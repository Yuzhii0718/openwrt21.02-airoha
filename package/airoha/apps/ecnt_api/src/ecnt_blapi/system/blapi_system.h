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
#ifndef _BLAPI_SYSTEM_H
#define _BLAPI_SYSTEM_H

/**
* \file  blapi_system.h  
* \brief  Definiations and BLAPIs related to system runtime status. These APIs are designed for Airoha's SDK products only.
* \author Zhengping.Zhang
* \date 2022-06-06
* \version  V1.0.1 
* \copyright Airoha Inc                                                              
*/


/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include "blapi_system_bsp.h"
#include "modules/service_manager/wan_mode_sel.h"

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

typedef struct blapi_intf_obj
{
	unsigned char idx;// wan idx
	unsigned short vid;
	unsigned short mvid;
	unsigned short used_vlan;
	char transMode[16];// ATM;PTM;Ethernet
	char linkMode[16];// PPPOE, IPOE
	char dev[8];
	char o_dev[16];
	char version;// 1:ipv4; 2:ipv6; 3:both
	char vtag;
	char dot1p; 
}blapi_intf_obj_t;

enum 
{ 
    UPFLAG_DOWN = 0,
    UPFLAG_ADSL, 
    UPFLAG_VDSL,
    UPFLAG_ETHERNET, 
    UPFLAG_SFP
};

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
 * \brief This function get the value of the env.
 *
 * This function get the value of the env. submit by Kang.Yang 2022.05.07
 * \param[in]   the index of env.
 * \param[out]	the value of the env.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the value of the env.
 * \par Example
 * \snippet blapi_system.c Get the value of the env.
 */

int blapi_system_get_envmsg(env_index index, char** envvalue);

/**
 * \brief This function get the offest of mac address.
 *
 * This function get the offest of mac address. submit by Zhen.Xu 2019.12.04
 * \param[out]	addr The offest of mac address.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the offest of mac address.
 * \par Example
 * \snippet blapi_system.c Get the offest of mac address
 */

int blapi_system_get_EtherAddr_offset(int *addr);

/**
 * \brief This function get the offest of mac address.
 *
 * This function get the offest of mac address. submit by Zhen.Xu 2019.12.04
 * \param[out]	addr The offest of mac address.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the offest of mac address.
 * \par Example
 * \snippet blapi_system.c Get the offest of mac address
 */

int blapi_system_get_EtherAddr_offset_L(long *addr);

/**
 * \brief This function get the offest of onu type.
 *
 * This function get the offest of onu type. submit by Zhen.Xu 2019.12.04
 * \param[out]	addr The offest of onu type.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the offest of onu type.
 * \par Example
 * \snippet blapi_system.c  Get the offest of onu type
 */

int blapi_system_get_OnuType_offset_L(long *addr);

/**
 * \brief This function get the size of tcboot.
 *
 * This function get the size of tcboot. submit by Zhen.Xu 2019.12.04
 * \param[out]	size The size of tcboot.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the size of tcboot.
 * \par Example
 * \snippet blapi_system.c Get the size of tcboot
 */

int blapi_system_get_tcboot_size(int *size);

/**
 * \brief This function get the offest of mac address range.
 *
 * This function get the offest of mac address range. submit by Lei.Zhang. 2019.12.25
 * \param[out]addr:The offest of mac address range.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the offest of mac address range.
 * \par Example
 * \snippet blapi_system.c the offest of mac address range.
 */
int blapi_system_get_EtherAddr_range_offset_L(long *addr);

/**
 * \brief This function get the offest of product and vendor name.
 *
 * This function get the offest of product and vendor name.. submit by Lei.Zhang. 2019.12.25
 * \param[out]productAddr:The offest of product name.
 * \param[out]vendorAddr:The offest of vendor name.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the offest of product and vendor name.
 * \par Example
 * \snippet blapi_system.c the offest of product and vendor name.
 */
int blapi_system_get_product_vendor_name_offset_L(long *productAddr, long* vendorAddr);

/**
 * \brief This function get the offest of mac addr ,snmp objID and country Code.
 *
 * This function get the offest of mac addr ,snmp objID and country Code. submit by Lei.Zhang. 2019.12.25
 * \param[out]etherAddr:The offest of mac addr.
 * \param[out]snmpAddr:The offest of snmp objID.
 * \param[out]countryCodeAddr:The offest of country Code.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the offest of mac addr ,snmp objID and country Code.
 * \par Example
 * \snippet blapi_system.c get the offest of mac addr ,snmp objID and country Code.
 */
int blapi_system_get_ether_snmp_countryCode_offset_L(long *etherAddr, long* snmpAddr, long* countryCodeAddr);

/**
 * \brief This function get the offest of SN.
 *
 * This function get the offest of SN. submit by Lei.Zhang. 2019.12.25
 * \param[out]addr:The offest of SN.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  Get the offest of SN.
 * \par Example
 * \snippet blapi_system.c get the offest SN.
 */
int blapi_system_get_sn_offset_L(long *addr);

/**
 * \brief This function will write bob to flash.
 *
 * This function write bob to flash. submit by Lei.Zhang. 2019.12.25.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  write bob to flash.
 * \par Example
 * \snippet blapi_system.c write bob to flash.
 */
int blapi_system_write_bob_to_flash(void);

/**
 * \brief This function check wifidevice for write alarm information.
 *
 * This function check wifidevice for write alarm information. submit by Zhen.Xu 2019.12.18
 * \param[out]	action the action of add/del alarm information.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  check wifidevice for write alarm information.
 * \par Example
 * \snippet blapi_system.c check wifidevice for write alarm information.
 */

int blapi_system_check_WifiDevice(int *action);

/**
 * \brief This function get the status of uplink.
 *
 * This function get the status of uplink.. submit by Zhen.Xu 2019.12.18
 * \param[in]	Mode the mode of pon type.
 * \param[in]	LinkSta the link status of pon.
 * \param[out]	tempValue the status of uplink.
 * \param[in]	len the size of parameter tempValue.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  get the status of uplink.
 * \par Example
 * \snippet blapi_system.c get the status of uplink.
 */

int blapi_system_get_UpLinkValue(char *Mode, char *LinkSta, char *tempValue, int len);

/**
 * \brief This function get cpu affinity
 *
 * This function get cpu affinity. submit by JinChao.Yao2019.12.20
 * \param[out]	the value of the affinity
 * \retval	0	Success.
 * \note
 * get the cpu affinity 
 * \par Example
 * \snippet blapi_system.c get cpu affinity
 */

int blapi_system_get_cpu_default_affinity(unsigned long *val);

/**
 * \brief This function get max receive file size  
 *
 * This function get This function get max receive file size. submit by JinChao.Yao2019.12.23
 * \param[out]	the value of the the max size
 * \retval	0	Success.
 * \note
 * get the max receive file size
 * \par Example
 * \snippet blapi_system.c get max receive file size
 */

int blapi_system_get_max_receive_file_size(unsigned long *val);

/**
 * \brief This function is used to set  lan led.
 *
 * This function is used to set  lan led.. submit by JinChao.Yao 2019.12.26
 * \param[in]	the lanPort
 * \param[in]	the led_st
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   This function is used to set  lan led.
 * \par Example
 * \snippet blapi_system.c  This function is used to set  lan led.
 */

int blapi_system_set_lan_led(int lanPort, int led_st);

/**
 * \brief This function set secure upgrade ioctl .
 *
 * This function get secure upgrade ioctl .
 * \param[in]	length: the length of command.
 * \param[in]	offset: the offset of command
 * \param[in]	file_path: the path of command
 * \param[in]	firmware_type: the type of firmware.
 * \retval	secure header offset.
 * \note
 *   This function get secure upgrade ioctl .
 * \par Example
 * \snippet blapi_system.c  This function get secure upgrade ioctl .
 */

int blapi_system_secure_trx(int length, int offset, char *file_path, unsigned char firmware_type);

/**
 * \brief This function set secure upgrade ioctl .
 *
 * This function set secure upgrade ioctl . submit by Zhen.Xu 20200107
 * \param[in]	length: the length of command.
 * \param[in]	offset: the offset of command
 * \param[in]	file_path: the path of command
 * \param[in]	firmware_type: the type of firmware.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *   This function set secure upgrade ioctl .
 * \par Example
 * \snippet blapi_system.c  This function set secure upgrade ioctl .
 */

int blapi_system_secure_upgrade(int length, int offset, char *file_path, unsigned char firmware_type);

/**
 * \brief This function get the command of clean the hw_nat.
 *
 * This function get the command of clean the hw_nat. submit by Yifan.Chen 2020.1.19
 * \param[out]	buf: the command  of clean the hw_nat.
 * \param[in]	size: the size of buf.
 * \retval	0	Success.
 * \note
 *  This function get the command of clean the hw_nat.
 * \par Example
 * \snippet blapi_system.c get the command of clean the hw_nat.
 */

int blapi_system_get_clearHWNAT_str(char *buf, int size);

/**
 * \brief This function do bob carlibration.
 *
 * This function do bob carlibration. submit by Yifan.Chen 2020.1.17
 * \retval	0	Success.
 * \note
 *  This function do bob carlibration.
 * \par Example
 * \snippet blapi_system.c do bob carlibration.
 */

int blapi_system_doSysBobCarlibration();

/**
 * \brief This function set boot flag according to tclinux.bin or tclinux_slave.bin.
 *
 * This function set boot flag according to tclinux.bin or tclinux_slave.bin.submit by Shengqian.Wang 2020.05.26
 * \param[in]	mtd: tclinux.bin or tclinux_slave.bin.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set boot flag according to tclinux.bin or tclinux_slave.bin.
 * \par Example
 * \snippet blapi_system.c set boot flag according to tclinux.bin or tclinux_slave.bin.
 */

int blapi_system_set_boot_flag(char *mtd);

/**
 * \brief This function check CRC algorithm.
 *
 * This function calculate the CRC-32 checck code. submit by Yifan.Chen 2020.1.17
 * \param[in]	imagefd: the first address of the block to be calculated.
 * \param[in]	len: number of 32 words of data block to be calculated
 * \note
 *  This function calculate the CRC-32 checck code.
 * \par Example
 * \snippet blapi_system.c calculate the CRC-32 checck code.
 */

int blapi_system_calculate_crc32(int imagefd, int len, unsigned int* pcrc);

/**
 * \brief This function check image CRC.
 *
 * This function check image CRC. submit by Yifan.Chen 2020.1.17
 * \param[in]	img: the flag of boot.
 * \param[out]	checksum: the crc checksum.
 * \note
 *  This function check image CRC.
 * \par Example
 * \snippet blapi_system.c check image CRC.
 */

int blapi_system_checkimageCrc(int img, int *checksum);

/**
 * \brief This function get mtdsize.
 *
 * This function get mtdsize. submit by Zhen.Xu 2020.03.03
 * \param[in]	mtd: the name of image.
 * \param[out]	mtd_size: the sizeof of mtd.
 * \param[out]	mtd_erasesize: the size of mtd erase.
 * \note
 *  This function get mtdsize.
 * \par Example
 * \snippet blapi_system.c get mtdsize.
 */
 
int blapi_system_get_mtdsize(char *mtd, int *mtd_size, int *mtd_erasesize);

/**
 * \brief This function do RomReset.
 *
 * This function do RomReset.. submit by ShengQian.Wang 2020.05.19
 * \param[in]	:rebootFlag: 1:need reboot; 0:no need reboot.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do RomReset.
 * \par Example
 * \snippet blapi_system.c do RomReset.
 */


int blapi_system_doRomReset(int rebootFlag);

/**
 * \brief This function do Sys Atspv show.
 *
 * This function do Sys Atspv show. submit by ShengQian.Wang 2020.05.19
  * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do Sys Atspv.
 * \par Example
 * \snippet blapi_system.c do Sys Atspv.
 */


int blapi_system_do_Sys_Atspv_show();

/**
 * \brief This function do Sys Atspv.
 *
 * This function do Sys Atspv. submit by ShengQian.Wang 2020.05.19
 * \param[in]	ProductName: the name of ProductName.
 * \param[in]	VendorName: the sizeof of VendorName.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do Sys Atspv.
 * \par Example
 * \snippet blapi_system.c do Sys Atspv.
 */

int blapi_system_do_Sys_Atspv_write(char *ProductName, char *VendorName);

/**
 * \brief This function do Sys Onumode xxx.
 *
 * This function do Sys Onumode xxx. submit by ShengQian.Wang 2020.05.19
 * \param[in]	ponMode: onu mode.
 * \param[in]	wanRamMode: wan Ram mode.
 * \param[in]	wanPayloadMode: wanPayloadMode.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do Sys Onumode xxx.
 * \par Example
 * \snippet blapi_system.c  do Sys Onumode xxx.
 */

int blapi_system_doSysPONMode(unsigned char ponMode, unsigned char wanRamMode, unsigned char wanPayloadMode, int noReboot);

/**
 * \brief This function do setbootloader.
 *
 * This function do setbootloader. submit by ShengQian.Wang 2020.05.19
 * \param[in]	mac_addr: mac address.
 * \param[in]	mode_name: model name .
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do setbootloader.
 * \par Example
 * \snippet blapi_system.c  do setbootloader.
 */

int blapi_system_setbootloader(unsigned char *mac_addr, unsigned char *mode_name);

/**
 * \brief This function do Sys Atsn.
 *
 * This function do Sys Atsn. submit by ShengQian.Wang 2020.05.19
 * \param[in]	buf: new SN.
 * \param[in]	size: SN size.
 * \param[in]	addr: SN addr in boot.bin.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function do Sys Atsn.
 * \par Example
 * \snippet blapi_system.c do Sys Atsn.
 */

int blapi_system_doSysAtsn(char *buf, int size, long addr) ;

/**
 * \brief This function get ether address.
 *
 * This function get ether address. submit by ShengQian.Wang 2020.05.19
 * \param[out] macaddr: mac address.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function get ether address.
 * \par Example
 * \snippet blapi_system.c get ether address.
 */

int blapi_system_get_EtherAddr(unsigned char *macaddr);

/**
 * \brief This function show Vendor name.
 *
 * This function show Vendor name. submit by ShengQian.Wang 2020.05.19
 * \param[in]	fp: boot file.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function show Vendor name.
 * \par Example
 * \snippet blapi_system.c show Vendor name.
 */

int blapi_system_show_Vendor_Name(FILE *fp);

/**
 * \brief This function show Product name.
 *
 * This function show Product name. submit by ShengQian.Wang 2020.05.19
 * \param[in]	fp: boot file.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function show Product name.
 * \par Example
 * \snippet blapi_system.c show Product name.
 */

int blapi_system_show_Product_Name(FILE *fp);

/**
 * \brief This function show mac address.
 *
 * This function show mac address. submit by ShengQian.Wang 2020.05.19
 * \param[in]	fp: boot file.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function show mac address.
 * \par Example
 * \snippet blapi_system.c show mac address.
 */

int blapi_system_show_Mac_Addr(FILE *fp);

/**
 * \brief This function show snmpObjID.
 *
 * This function show snmpObjID. submit by ShengQian.Wang 2020.05.19
 * \param[in]	fp: boot file.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function show snmpObjID.
 * \par Example
 * \snippet blapi_system.c show snmpObjID.
 */

int blapi_system_show_snmpObjID(FILE *fp);

/**
 * \brief This function show country Code.
 *
 * This function show country Code. submit by ShengQian.Wang 2020.05.19
 * \param[in]	fp: boot file.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function show country Code.
 * \par Example
 * \snippet blapi_system.c show country Code.
 */

int blapi_system_show_country_Code(FILE *fp);

/**
 * \brief This function gets bosa temp.
 *
 * This function gets bosa temp. submit by ShengQian.Wang 2020.05.19
 * \param[in]	temp: temperature.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function gets bosa temp.
 * \par Example
 * \snippet blapi_system.c GetBosaTemp.
 */

int blapi_system_GetBosaTemp(int *temp);


#if  defined(TCSUPPORT_CT_STBMAC_REPORT)
/**
 * \brief This function check stb change.
 *
 * This function check stb change. submit by Peng.Ni 2020.06.23
 * \param[in]	void.
 * \retval	0	STB not change.
 * \retval	1	STB change.
 * \note
 *  This function check stb change.
 * \par Example
 * \snippet blapi_system.c check_stb_change.
 */
int blapi_system_check_stb_change(void);
#endif

#if defined(TCSUPPORT_CT_PON)
int blapi_system_get_bob(char *bob, int len);
int blapi_system_set_bob(char *bob, int len);
#endif

/**
 * \brief This function gets correct CPU mask when using taskset in EN7523.
 *
 * This function gets correct CPU mask when using taskset in EN7523. submit by Gareth.Kuo 2020.07.31
 * \param[in]	mask: the cpu mask of taskset.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function gets gets correct CPU mask when using taskset in EN7523.
 * \par Example
 * \snippet blapi_system.c CpuRemapTaskset.
 */

int blapi_system_cpu_remap_taskset(unsigned long *mask);

/**
 * \brief This function is used to free vpn ip hashmap.
 *
 * TThis function is used to free vpn ip hashmap. submit by Jun.Wu 2020.08.20
 * \param[in]	idx: vpn index.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to free vpn ip hashmap.
 * \par Example
 * \snippet blapi_system.c free ip hashmap.
 */

int blapi_system_vpn_free_ip_hashmap(int idx);

/**
 * \brief This function set urlfilter enable value.
 *
 * This function set urlfilter enable status. submit by Lei.Zhang 2020.08.20
 * \param[in]	datealen: urlfilter data lenth
 * \param[in]	data: urlfilter enable value
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set urlfilter enable status.
 * \par Example
 */
int blapi_system_url_filter_set_enable(void *data, int datealen);

/**
 * \brief This function set urlfilter mode.
 *
 * This function set urlfilter mode. submit by Lei.Zhang 2020.08.20
 * \param[in]	datealen: urlfilter data lenth
 * \param[in]	data: urlfilter mode value
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set urlfilter mode.
 * \par Example
 */
int blapi_system_url_filter_set_mode(void *data, int datealen);

/**
 * \brief This function set urlfilter url host.
 *
 * This function set urlfilter url host. submit by Lei.Zhang 2020.08.20
 * \param[in]	cfg_urlInfo: urlfilter url host data.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set urlfilter url host.
 * \par Example
 */
int blapi_system_url_filter_set_url_host(url_info_cfg* cfg_urlInfo);

/**
 * \brief This function set urlfilter url path.
 *
 * This function set urlfilter url host. submit by Lei.Zhang 2020.08.20
 * \param[in]	cfg_urlInfo: urlfilter url path data.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set urlfilter url path.
 * \par Example
 */
int blapi_system_url_filter_set_url_path(url_info_cfg* cfg_urlInfo);

/**
 * \brief This function gets urlfilter info.
 *
 * This function gets urlfilter info. submit by Lei.Zhang 2020.08.20
 * \param[in]	datealen: urlfilter data lenth
 * \param[out]	data: urlfilter data
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function gets urlfilter info.
 * \par Example
 */
int blapi_system_url_filter_get_blocktimes(void *data, int datealen);

/**
 * \brief This function clear all urlfilter rule.
 *
 * This function clear all urlfilter rule. submit by Lei.Zhang 2020.08.20
 * \param[in]	void
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function clear all urlfilter rule.
 * \par Example
 */
int blapi_system_url_filter_clear_all_rule(void);

/**
 * \brief This function clear urlfilter global setting.
 *
 * This function clear urlfilter global setting. submit by Lei.Zhang 2020.08.20
 * \param[in]	cfg_data: urlfilter data
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function clear all urlfilter rule.
 * \par Example
 */
int blapi_system_url_filter_clear_global_setting(urlfilter_ioctl_cfg_data* cfg_data);

/**
 * \brief This function clear urlfilter all basic mac rule.
 *
 * This function clear urlfilter all basic mac rule. submit by Lei.Zhang 2020.08.20
 * \param[in]	cfg_data: urlfilter data
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function clear urlfilter all basic mac rule.
 * \par Example
 */
int blapi_system_url_filter_clear_all_basicmac_rule(urlfilter_ioctl_cfg_data* cfg_data);

/**
 * \brief This function add urlfilter url rule
 *
 * This function add urlfilter url rule. submit by Lei.Zhang 2020.08.20
 * \param[in]	cfg_data: urlfilter data
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function add urlfilter url rule.
 * \par Example
 */
int blapi_system_url_filter_add_url_rule(urlfilter_ioctl_cfg_data* cfg_data);

/**
 * \brief This function clear all dnsfilter rule.
 *
 * This function clear all dnsfilter rule. submit by Bin.Wang 2020.09.29
 * \param[in]	void
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function clear all dnsfilter rule.
 * \par Example
 */
int blapi_system_dns_filter_clear_all_rule(void);

/**
 * \brief This function set lan ip.
 *
 * This function set lan ip. submit by Bin.Wang 2020.09.29
 * \param[in]	cfg_data: lanip data
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set lan ip.
 * \par Example
 */
int blapi_system_dns_filter_set_lanip(dnsfilter_ioctl_cfg_lanip_t* cfg_data);

/**
 * \brief This function add dnsfilter url rule
 *
 * This function add dnsfilter rule. submit by Bin.Wang 2020.09.29
 * \param[in]	cfg_data: dnsfilter data
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function add dnsfilter url rule.
 * \par Example
 */
int blapi_system_dns_filter_add_dns_rule(urlfilter_ioctl_cfg_data* cfg_data);

#if 0 /* Unused */
/**
 * \brief This function get serdes port number.
 *
 * This function get serdes port number.
 * \retval	the total serdes port number.
 * \note
 *  This function get serdes port number.
 * \par Example
 */
int blapi_system_get_serdes_port_cnt(void);

/**
 * \brief This function get the serdes interface count of specfic serdes port.
 *
 * This function get the serdes interface count of specfic serdes port.
 * \param[in]	port_idx: serdes port selection.
 * \retval	the total serdes interface count of serdes port.
 * \note
 *  This function get the serdes interface count of specfic serdes port.
 * \par Example
 */
int blapi_system_get_serdes_if_cnt(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx);

/**
 * \brief This function get the total serdes ether type count.
 *
 * This function get the total serdes ether type count.
 * \retval	the total serdes ether type count of serdes port.
 * \note
 *  This function get the total serdes ether type count.
 * \par Example
 */
int blapi_system_get_ether_type_cnt(void);

/**
 * \brief This function get serdes port name.
 *
 * This function get serdes port name.
 * \param[in]	serdes->port_idx: serdes port selection.
 * \retval	0: get serdes port name success and store it into serdes->name.
 * \retval	<0	Fail.
 * \note
 *  This function get serdes port name.
 * \par Example
 */
int blapi_system_get_serdes_port_name(serdesCfgIoctl_t *serdes);

/**
 * \brief This function get serdes interface selection name at specific port.
 *
 * This function get serdes interface selection name at specific port.
 * \param[in]	port_idx: serdes port selection.
 * \param[in]	serdes_if: serdes interface at port_idx port.
 * \retval	0: get serdes interface name success and store it into serdes->name.
 * \retval	<0	Fail.
 * \note
 *  This function get serdes interface selection name at specific port.
 * \par Example
 */
int blapi_system_get_serdes_if_name(serdesCfgIoctl_t *serdes);
#endif

/**
 * \brief This function get serdes interface selection of specfic serdes port.
 *
 * This function get serdes interface at serdes port(port_idx).
 * \param[in]	port_idx: serdes port selection.
 * \retval		the total serdes interface count at [port_idx] port.
 * \retval	<0	Fail.
 * \note
 *  This function get serdes interface at serdes port(port_idx).
 * \par Example
 */
int blapi_system_get_serdes_if_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx);

/**
 * \brief This function get serdes interface ether type selection of specfic serdes port.
 *
 * This function get serdes interface ether type selection of specfic serdes port.
 * \param[in]	port_idx: serdes port selection.
 * \retval	>=0	serdes interface ether type selection at [port_idx] port.
 * \retval	<0	Fail.
 * \note
 *  This function get serdes interface ether type selection of specfic serdes port.
 * \par Example
 */
ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t blapi_system_get_serdes_ether_type_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx);

/**
 * \brief This function set serdes selection.
 *
 * This function set serdes selection.
 * \param[in]	sel: serdes selection.
 * \param[in]	rebootFlag: 1:need reboot; 0:no need reboot.
 * \param[in]isHSGMII: 1: select to HSGMII; 0:non-HSGMII(pon, usb3, pcie0 or pcie1)
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set serdes selection.
 * \par Example
 */
int blapi_system_set_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel, int rebootFlag, int isHSGMII);

/**
 * \brief This function set serdes selection.
 *
 * This function set serdes interface(serdes_if) at serdes port(port_idx).
 * \param[in]	serdes->port_idx: serdes port selection.
 * \param[in]	serdes->serdes_if: select to serdes interface at [port_idx] port.
 * \param[in]	serdes->ether_type: select to ether type at the serdes_if of [port_idx] port.
 * \param[in]	rebootFlag: 1:need reboot; 0:no need reboot.
 * \retval	0:	Success.
 * \retval	<0	Fail.
 * \note
 *  This function set serdes interface(serdes_if) at serdes port(port_idx).
 * \par Example
 */
int blapi_system_set_serdes_sel_v2(serdesCfgIoctl_t *serdes, int rebootFlag);

/**
 * \brief This function set/get serdes selection.
 *
 * This function set/get serdes interface(serdes_if) at serdes port(port_idx).
 * command: get serdes selection: sys serdes.
 *          set serdes selection: sys serdes <sel> <I/F> <ether type> [-n]
 * \param[in]	argc: the number of input parameters.
 * \param[in]	argv: input parameters.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function set/get serdes interface(serdes_if) at serdes port(port_idx).
 * \par Example
 */
int blapi_system_doSysSerdesSel(int argc, char *argv[]);

/**
 * \brief This function is used to get linkmode
 *
 * This function is used to get linkmode.. submit by jiawei.xu2021.3.25
 * \param[out]	upflag: 0->down, 1->adsl, 2->vdsl, 3->ether, 4->SFP.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get linkmode.
 * \par Example
 * \snippet blapi_system.c used to get linkmode.
 */
int blapi_system_get_spf_linkup(int *upflag);

/**
 * \brief This function is used to exec client command
 *
 * This function is used to exec client command.. submit by gareth.kuo 2021.5.03
 * \param[out]	cmd.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to exec client command.
 * \par Example
 * \snippet blapi_system.c used to exec client command.
 */
int blapi_system_client_command(char *cmd);

/**
 * \brief This function is used to exec crc command
 *
 * This function is used to exec crc command.. submit by gareth.kuo 2021.5.03
 * \param[int]	crcFile.
 * \param[out]	crc32.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to exec crc command.
 * \par Example
 * \snippet blapi_system.c used to exec crcFile command.
 */
int blapi_system_get_crc(char *crcFile, unsigned int *crc32);

/**
 * \brief This function is used to set wan mode
 *
 * This function is used to set wan mode.. submit by gareth.kuo 2021.5.03
 * \param[in]	wan_mode.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to set wan mode.
 * \par Example
 * \snippet blapi_system.c used to set wan mode.
 */
int blapi_system_set_wan_mode(WanMode_t wan_mode);

/**
 * \brief This function is used to create prepared interface.
 *
 * This function is used to create prepared interface.. submit by jiawei.xu2021.5.10
 * \param[in]	 idx: wan idx, transMode:wan linkmode, devname:wan devname, version: ip version.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to create prepared interface.
 * \par Example
 * \snippet blapi_system.c used to create prepared interface.
 */
int blapi_system_prepare_interface(blapi_intf_obj_t *intf);

/**
 * \brief This function is used to delete prepared interface.
 *
 * This function is used to delete prepared interface.. submit by jiawei.xu2021.5.10
 * \param[in]	 idx: wan idx, transMode:wan linkmode, devname:wan devname, version: ip version.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to delete prepared interface.
 * \par Example
 * \snippet blapi_system.c used to delete prepared interface.
 */
int blapi_system_del_prepared_interface(blapi_intf_obj_t *intf);

/**
 * \brief This function is used to create interface by smuxctl.
 *
 * This function is used to delete prepared interface.. submit by jiawei.xu2021.5.18
 * \param[in]	 dev: dev, o_dev:obj dev, linkMode:link mode, vtag: tag mode, 
 				vid: vlan id, dot1p: dot1p, mvid: mvid.
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to create interface by smuxctl.
 * \par Example
 * \snippet blapi_system.c used to create interface by smuxctl.
 */
int blapi_system_create_interface(blapi_intf_obj_t *intf);

/**
 * \brief This function is used to create etc/mac.conf for bsp sdk.
 *
 * This function is used to create etc/mac.conf for bsp sdk.. submit by jiawei.xu2021.3.4
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to create etc/mac.conf for bsp sdk.
 * \par Example
 * \snippet blapi_system.c used to create etc/mac.conf for bsp sdk.
 */
int blapi_system_create_mac_conf();

/**
 * \brief This function is used to get mac from etc/mac.conf?
 *
 * This function is used to get mac from etc/mac.conf.. submit by jiawei.xu2021.3.4
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  This function is used to get mac from etc/mac.conf.
 * \par Example
 * \snippet blapi_system.c used to get mac from etc/mac.conf.
 */
int blapi_system_get_eth0_mac(char * mac);

/**
 * \brief This function set app_route data.
 *
 * This function set app_route rules data.
 * \param[in]	cmd
 * \param[in]	data ptr
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  
 */
int blapi_system_app_route_data_ctl(int cmd, void *data);

/**
 * \brief This function set app_filter data.
 *
 * This function set app_filter rules data.
 * \param[in]	cmd
 * \param[in]	dev
 * \param[in]	data ptr
 * \retval	0	Success.
 * \retval	1	Fail.
 * \note
 *  
 */
int blapi_system_app_filter_data_ctl(int cmd, char *dev, void *data);

/**
 * \brief This function get cpu mask.
 * \param[in]	cpu_index
 * \param[in]	cpu_mask
 * \retval	0	Success.
 * \retval	-1	Fail.
 * \note
 *  
 */
int blapi_system_app_get_cpu_mask(int cpu_index, int *cpu_mask);


#if defined(TCSUPPORT_CT_BACKUPROMFILEENCHANCEMENT)
/**
 * \brief This function cpmpute crc32.
 * \param[in]	buf
 * \param[in]	len
 * \retval	    the crc	of result of compute.
 * \note
 *  
 */
uint32_t blapi_system_compute_crc32buf(char *buf, size_t len);
#endif


/**
 * \brief This function check mac in stb list.
 *
 * This function check mac in stb list.
 * \param[in]	mac
 * \retval	1	Success. (this mac in stb list)
 * \retval	0	Fail. (this mac not in stb list)
 * \note
 *  
 */
int blapi_system_check_mac_in_stb_list(unsigned char *mac);

#endif /*_BLAPI_SYSTEM_H*/
