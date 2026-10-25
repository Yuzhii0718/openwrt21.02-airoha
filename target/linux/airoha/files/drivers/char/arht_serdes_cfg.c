/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��AIROHA Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to AIROHA Limited (��AIROHA��) and/or 
its licensors. Without the prior written permission of AIROHA and/or its licensors, 
any reproduction, modification, use or disclosure of AIROHA Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

AIROHA Limited  AIROHA. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��AIROHA SOFTWARE��) RECEIVED FROM AIROHA 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES AIROHA PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE AIROHA SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN AIROHA SOFTWARE.

AIROHA SHALL NOT BE RESPONSIBLE FOR ANY AIROHA SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
AIROHA'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AIROHA 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT AIROHA'S SOLE OPTION, TO 
REVISE OR REPLACE THE AIROHA SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO AIROHA FOR SUCH 
AIROHA SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <asm/tc3162/tc3162.h>
#include <linux/uaccess.h>
#include <ecnt_event_global/ecnt_event_system.h>
#include <asm/uaccess.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define SERDES_IF_IGNORE	(0xFF)

#define SERDES_CFG_DBG_LEVEL_NONE	(1 << 0)
#define SERDES_CFG_DBG_LEVEL_ERROR	(1 << 1)
#define SERDES_CFG_DBG_LEVEL_NOTICE	(1 << 2)	//for verify
#define SERDES_CFG_DBG_LEVEL_INFO	(1 << 3)	//for dump array
#define SERDES_CFG_DBG_LEVEL_TRACE	(1 << 4)	//for trace code flow
#define SERDES_CFG_DBG_LEVEL_CFG	(SERDES_CFG_DBG_LEVEL_ERROR)

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define ARHT_SERDES_CFG_DEBUG
#define SERDES_MSG(lv, arg...) { \
	if(serdes_cfg_dbg_lv & lv) { \
		printk("[%s]:%d ", __func__, __LINE__); \
		printk(arg); \
	} \
}

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/


/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
static uint32_t serdes_cfg_dbg_lv = SERDES_CFG_DBG_LEVEL_CFG;

#if defined(TCSUPPORT_CPU_EN7581)
static int serdes_sel[ECNT_EVENT_SERDES_SEL_MAX];
static const char * const ic_serdes_if[][6] = { {"PON", "HSGMII", "XFI", "USXGMII", "NONE"}, 		/* Serdes-PON */
												{"XFI", "USXGMII", "HSGMII", "5G-BaseR", "NONE"},	/* Serdes-Ethernet */
												{"PCIE0(2-LANE)", "PCIE0(1-LANE)", "HSGMII", "USXGMII", "XFI", "NONE"},/* Serdes-WIFI1 */
												{"PCIE0(2-LANE)", "PCIE1(1-LANE)", "HSGMII", "USXGMII", "XFI", "NONE"},/* Serdes-WIFI2 */
												{"USB3", "HSGMII", "NONE"},							/* Serdes-USB1 */
												{"USB3", "PCIE2(1-LANE)", "NONE"}};					/* Serdes-USB2 */
static const int ic_serdes_if_cnt[] =			{ECNT_EVENT_SERDES_PON_MAX, 	/* Serdes-PON */
												ECNT_EVENT_SERDES_ETHER_MAX,	/* Serdes-Ethernet */
												ECNT_EVENT_SERDES_WIFI1_MAX,	/* Serdes-WIFI1 */
												ECNT_EVENT_SERDES_WIFI2_MAX,	/* Serdes-WIFI2 */
												ECNT_EVENT_SERDES_USB1_MAX, 	/* Serdes-USB1 */
												ECNT_EVENT_SERDES_USB2_MAX};	/* Serdes-USB2 */
static const char * const serdes_port_name[] =		{"SerDes-PON", "SerDes-Ethernet", "SerDes-WiFi1", "SerDes-WiFi2", "SerDes-USB1", "SerDes-USB2"};
static const char * const env_serdes_port_name[] =	{BOOTARGS_SERDES_PON_SEL_STR, 
													BOOTARGS_SERDES_ETHER_SEL_STR, 
													BOOTARGS_SERDES_WIFI1_SEL_STR, 
													BOOTARGS_SERDES_WIFI2_SEL_STR, 
													BOOTARGS_SERDES_USB1_SEL_STR, 
													BOOTARGS_SERDES_USB2_SEL_STR};
#elif defined(TCSUPPORT_CPU_AN7552)
static char serdes_sel[ECNT_EVENT_SERDES_SEL_MAX];
static const char * const ic_serdes_if[][3] = { {"PON", "HSGMII", "NONE"},	/* Serdes-PON */
												{"0", "0"},
												{"0", "0"},
												{"0", "0"}, 
												{"0", "0"},
												{"0", "0"}};
static const int ic_serdes_if_cnt[] =			{ECNT_EVENT_SERDES_PON_MAX, 		/* Serdes-PON */
												0,		/* Serdes-WiFi1 */
												0,		/* Serdes-WiFi2 */
												0,
												0,
												0};
static const char * const serdes_port_name[] =			{"SerDes-PON", "0", "0", "0", "0", "0"};
static const char * const env_serdes_port_name[] =	{BOOTARGS_SERDES_PON_SEL_STR};
#elif defined(TCSUPPORT_CPU_EN7523)
static int serdes_sel = 0;
static const char * const ic_serdes_if[][2] = { {"PON", "HSGMII"},	/* Serdes-PON */
												{"USB3", "HSGMII"},	/* Serdes-USB */
												{"PCIE0", "HSGMII"}, /* Serdes-WiFi1 */
												{"PCIE1", "HSGMII"}, /* Serdes-WiFi2 */
												{"0", "0"},
												{"0", "0"}};
static const int ic_serdes_if_cnt[] =			{ECNT_EVENT_SERDES_PON_MAX, 		/* Serdes-PON */
												ECNT_EVENT_SERDES_USB1_MAX,			/* Serdes-USB */
												ECNT_EVENT_SERDES_WIFI1_MAX,		/* Serdes-WiFi1 */
												ECNT_EVENT_SERDES_WIFI2_MAX,		/* Serdes-WiFi2 */
												0,
												0};
static const char * const serdes_port_name[] =			{"SerDes-PON", "SerDes-USB3", "SerDes-WiFi1", "SerDes-WiFi2", "0", "0"};
static const char * const env_serdes_port_name[] =		{BOOTARGS_SERDES_SEL_STR};
#endif
static const char * const env_serdes_etyer_type_name[] = {
	"NONE", 
	"etherLAN", "etherWAN", "0"
};

static const char * const env_serdes_phy_name[] = {
	"NONE", "an8811", "A", "B", "C", "D", "E", "0"
};


/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern int get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel);

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
#ifdef ARHT_SERDES_CFG_DEBUG
void set_serdes_cfg_dbg_lv(uint32_t dbg_lv)
{
	serdes_cfg_dbg_lv = dbg_lv;
}
#else
#define set_serdes_cfg_dbg_lv(...) do{}while(0)
#endif

#ifdef TCSUPPORT_CPU_EN7581
static int __init early_serdes_pon_sel(char *p)
{

	int ret;

	ret = sscanf(p, "%hx", &serdes_sel[ECNT_EVENT_SERDES_SEL_PON]);
	if(ret != 1) {
		printk("parse %s error:%d\n", BOOTARGS_SERDES_PON_SEL_STR, ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_SERDES_PON_SEL_STR, early_serdes_pon_sel);

static int __init early_serdes_ether_sel(char *p)
{

	int ret;

	ret = sscanf(p, "%hx", &serdes_sel[ECNT_EVENT_SERDES_SEL_ETHER]);
	if(ret != 1) {
		printk("parse %s error:%d\n", BOOTARGS_SERDES_ETHER_SEL_STR, ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_SERDES_ETHER_SEL_STR, early_serdes_ether_sel);

static int __init early_serdes_wifi1_sel(char *p)
{

	int ret;

	ret = sscanf(p, "%hx", &serdes_sel[ECNT_EVENT_SERDES_SEL_WIFI1]);
	if(ret != 1) {
		printk("parse %s error:%d\n", BOOTARGS_SERDES_WIFI1_SEL_STR, ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_SERDES_WIFI1_SEL_STR, early_serdes_wifi1_sel);

static int __init early_serdes_wifi2_sel(char *p)
{

	int ret;

	ret = sscanf(p, "%hx", &serdes_sel[ECNT_EVENT_SERDES_SEL_WIFI2]);
	if(ret != 1) {
		printk("parse %s error:%d\n", BOOTARGS_SERDES_WIFI2_SEL_STR, ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_SERDES_WIFI2_SEL_STR, early_serdes_wifi2_sel);

static int __init early_serdes_usb1_sel(char *p)
{

	int ret;

	ret = sscanf(p, "%hx", &serdes_sel[ECNT_EVENT_SERDES_SEL_USB1]);
	if(ret != 1) {
		printk("parse %s error:%d\n", BOOTARGS_SERDES_USB1_SEL_STR, ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_SERDES_USB1_SEL_STR, early_serdes_usb1_sel);

static int __init early_serdes_usb2_sel(char *p)
{

	int ret;

	ret = sscanf(p, "%hx", &serdes_sel[ECNT_EVENT_SERDES_SEL_USB2]);
	if(ret != 1) {
		printk("parse %s error:%d\n", BOOTARGS_SERDES_USB2_SEL_STR, ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_SERDES_USB2_SEL_STR, early_serdes_usb2_sel);

int get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel)
{
	int _serdes_sel = 0;

	if(sel >= ECNT_EVENT_SERDES_SEL_MAX) {
		printk("Error, Unknow serdes port.\n");
		return -1;
	}

	_serdes_sel = serdes_sel[sel];

	return _serdes_sel;
}
EXPORT_SYMBOL(get_serdes_sel);

#elif TCSUPPORT_CPU_AN7552

static int __init early_serdes_pon_sel(char *p)
{

	int ret;

	ret = sscanf(p, "%hhx", &serdes_sel[ECNT_EVENT_SERDES_SEL_PON]);
	if(ret != 1) {
		printk("parse %s error:%d\n", BOOTARGS_SERDES_PON_SEL_STR, ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_SERDES_PON_SEL_STR, early_serdes_pon_sel);

int get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel)
{
	char _serdes_sel = 0;

	if(sel >= ECNT_EVENT_SERDES_SEL_MAX) {
		printk("Error, Unknow serdes port.\n");
		return -1;
	}

	_serdes_sel = serdes_sel[sel];

	return (int)_serdes_sel;
}
EXPORT_SYMBOL(get_serdes_sel);


#else
static int __init early_serdes_sel(char *p)
{
	int ret;

	ret = sscanf(p, "%hx", &serdes_sel);
	if(ret != 1) {
		printk("parse %s error:%d\n", BOOTARGS_SERDES_SEL_STR, ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_SERDES_SEL_STR, early_serdes_sel);

int get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel)
{
	int bit_idx = (int)sel;
	
	if(sel >= ECNT_EVENT_SERDES_SEL_MAX) {
		return -1;
	}

	return serdes_sel;
}
EXPORT_SYMBOL(get_serdes_sel);
#endif


static int check_serdes_if(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx, uint8_t serdes_if)
{
	int max_if;

	if(port_idx >= ECNT_EVENT_SERDES_SEL_MAX) {
		return -1;
	}

	max_if = ic_serdes_if_cnt[port_idx];

	if(serdes_if != SERDES_IF_IGNORE && serdes_if >= max_if) {
		return -1;
	}

	return 0;
}

static int get_serdes_interface_count(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx)
{
	if(check_serdes_if(port_idx, SERDES_IF_IGNORE)) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", __func__, __LINE__, (int)port_idx);
		return -1;
	}

	return ic_serdes_if_cnt[(int)port_idx];
}

int get_serdes_port_count(void)
{
	return SERDES_PORT_CNT;
}
EXPORT_SYMBOL(get_serdes_port_count);

static int get_serdes_ether_type_count(void)
{
	int ether_type_cnt = 0;

	ether_type_cnt = sizeof(env_serdes_etyer_type_name)/sizeof(env_serdes_etyer_type_name[0]);

	return ether_type_cnt;
}

ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t get_serdes_ether_type_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx)
{
	int _serdes_ethernet_sel = 0;

	if(check_serdes_if(port_idx, SERDES_IF_IGNORE)) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", __func__, __LINE__, (int)port_idx);
		return ECNT_EVENT_SERDES_ETHER_TYPE_NONE;
	}

	_serdes_ethernet_sel = ((get_serdes_sel(port_idx) >> SERDES_ETHER_TYPE_SEL_OFFSET) & SERDES_ETHER_TYPE_SEL_MASK);

	if(isEN7523) {
		_serdes_ethernet_sel = ((_serdes_ethernet_sel & (0x1 << port_idx)) >> port_idx) + 1;
	}
	
	switch(_serdes_ethernet_sel) {
		case ECNT_EVENT_SERDES_ETHER_TYPE_LAN:
			return ECNT_EVENT_SERDES_ETHER_TYPE_LAN;
		case ECNT_EVENT_SERDES_ETHER_TYPE_WAN:
			return ECNT_EVENT_SERDES_ETHER_TYPE_WAN;
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
		default :
			return ECNT_EVENT_SERDES_ETHER_TYPE_NONE;
#elif defined(TCSUPPORT_CPU_EN7523)
		default :
			return ECNT_EVENT_SERDES_ETHER_TYPE_LAN; // fix coverity issue, 7523 default ether lan  
#endif
	}
}
EXPORT_SYMBOL(get_serdes_ether_type_sel);

uint8_t get_serdes_interface_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx)
{
	uint8_t _serdes_if_sel = UNKNOW_SERDES_IF;

	if(check_serdes_if(port_idx, SERDES_IF_IGNORE)) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", __func__, __LINE__, (int)port_idx);
		return UNKNOW_SERDES_IF;
	}

	_serdes_if_sel = (((get_serdes_sel(port_idx) & 0xFF) >> SERDES_IF_SEL_OFFSET) & SERDES_IF_SEL_MASK);

	if(isEN7523) {
		_serdes_if_sel = ((_serdes_if_sel & (0x1 << port_idx)) >> port_idx);
	}

	return _serdes_if_sel;
}
EXPORT_SYMBOL(get_serdes_interface_sel);

ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t get_serdes_phy_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx)
{
	uint8_t _serdes_phy_sel = 0;

	if(check_serdes_if(port_idx, SERDES_IF_IGNORE)) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", __func__, __LINE__, (int)port_idx);
		return UNKNOW_SERDES_IF;
	}

	_serdes_phy_sel = (((get_serdes_sel(port_idx) & 0xFFF) >> SERDES_PHY_SEL_OFFSET) & SERDES_PHY_SEL_MASK);

	return _serdes_phy_sel;
}
EXPORT_SYMBOL(get_serdes_phy_sel);


static const char *get_env_serdes_port_name(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx)
{
	if(check_serdes_if(port_idx, SERDES_IF_IGNORE)) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", __func__, __LINE__, (int)port_idx);
		return NULL;
	}

	return env_serdes_port_name[port_idx];
}

const char *get_serdes_port_name(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx)
{
	if(check_serdes_if(port_idx, SERDES_IF_IGNORE)) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", __func__, __LINE__, (int)port_idx);
		return NULL;
	}

	return serdes_port_name[port_idx];;
}
EXPORT_SYMBOL(get_serdes_port_name);

const char *get_serdes_interface_name(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx, uint8_t serdes_if)
{
	if(check_serdes_if(port_idx, serdes_if)) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d, if_idx:%d.\n", (int)port_idx, serdes_if);
		return NULL;
	}
	
	return ic_serdes_if[port_idx][serdes_if];
}
EXPORT_SYMBOL(get_serdes_interface_name);

static const char *get_serdes_ether_type_name(ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t ether_type_sel)
{
	if(ether_type_sel >= ECNT_EVENT_SERDES_ETHER_TYPE_MAX) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error ether_type_sel:%d.\n", __func__, __LINE__, (int)ether_type_sel);
		return NULL;
	}
	
	return env_serdes_etyer_type_name[ether_type_sel];
}

static const char *get_serdes_phy_name(ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t phy_sel)
{
	if(phy_sel >= ECNT_EVENT_SERDES_PHY_MAX) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error phy_sel:%d.\n", __func__, __LINE__, (int)phy_sel);
		return NULL;
	}

	return env_serdes_phy_name[phy_sel];
}

static int getSerdesSelUsage(serdesCfgIoctl_t *serdes)
{
	int serdesPortCnt = 0, serdesEtherTypeCnt = 0, serdesPhyCnt = 0;
	int port_idx = 0, serdes_if_cnt = 0, ethernet_idx = 0;
	uint8_t serdes_if = 0;
	int serdesEtherTypeIdx = 0, serdesPhyIdx = 0;
	char *usage, *name;
	int ret = 0;
	size_t usage_len = 0;

	usage = vmalloc(RET_STRING_LEN_MAX);
	if(!usage) {
		ret = -1;
		goto get_usage_finish;
	}
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "\n");
	
	usage_len += snprintf(usage, RET_STRING_LEN_MAX - usage_len - 1, "Usage:\n");
	if(usage_len == (RET_STRING_LEN_MAX - 1)) {
		ret = -1;
		goto get_usage_finish;
	}
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);
	
	usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "  get serdes selection: sys serdes\n");
	if(usage_len == (RET_STRING_LEN_MAX - 1)) {
		ret = -1;
		goto get_usage_finish;
	}
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);
	
	usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "  set serdes selection: sys serdes <sel> <I/F> [ether type] [phy type] [-n]\n");
	if(usage_len == (RET_STRING_LEN_MAX - 1)) {
		ret = -1;
		goto get_usage_finish;
	}
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);
	
	usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "  sel:\n");
	if(usage_len == (RET_STRING_LEN_MAX - 1)) {
		ret = -1;
		goto get_usage_finish;
	}
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);

	serdesPortCnt = ECNT_EVENT_SERDES_SEL_MAX;
	for(port_idx = 0; port_idx < serdesPortCnt; port_idx++) {
		name = get_serdes_port_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)port_idx);
		if(name == NULL) {
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "get serdes port:%d name failed!\n", port_idx);
			ret = -1;
			goto get_usage_finish;
		}
		usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "    %d:%s port\n", port_idx, name);
		if(usage_len == (RET_STRING_LEN_MAX - 1)) {
			ret = -1;
			goto get_usage_finish;
		}
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);
		
		usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "      I/F:\n");
		if(usage_len == (RET_STRING_LEN_MAX - 1)) {
			ret = -1;
			goto get_usage_finish;
		}
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);

		serdes_if_cnt = get_serdes_interface_count((ECNT_EVENT_SYSTEM_SERDES_SEL_t)port_idx);
		for(serdes_if = 0; serdes_if < serdes_if_cnt; serdes_if++) {
			name = get_serdes_interface_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)port_idx, serdes_if);
			if(name == NULL) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "get serdes port:%d interface:%d name failed!\n", port_idx, serdes_if);
				ret = -1;
				goto get_usage_finish;
			}
			usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "        %d:%s\n", serdes_if, name);
			if(usage_len == (RET_STRING_LEN_MAX - 1)) {
				ret = -1;
				goto get_usage_finish;
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);
		}
	}

	usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "  ether type:optional parameter.\n");
	if(usage_len == (RET_STRING_LEN_MAX - 1)) {
		ret = -1;
		goto get_usage_finish;
	}
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);

	usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "    - This field at EN7523 is only valid when the serdes interface is selected to HSGMII.\n");
	if(usage_len == (RET_STRING_LEN_MAX - 1)) {
		ret = -1;
		goto get_usage_finish;
	}
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);

	serdesEtherTypeCnt = ECNT_EVENT_SERDES_ETHER_TYPE_MAX;
	for(serdesEtherTypeIdx = 0; serdesEtherTypeIdx < serdesEtherTypeCnt; serdesEtherTypeIdx++) {
		name = get_serdes_ether_type_name((ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t)serdesEtherTypeIdx);
		if(name == NULL) {
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "get serdes ether type:%d name failed!\n", serdesEtherTypeIdx);
			ret = -1;
			goto get_usage_finish;
		}
		usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "    %d:%s\n", serdesEtherTypeIdx, name);
		if(usage_len == (RET_STRING_LEN_MAX - 1)) {
			ret = -1;
			goto get_usage_finish;
		}
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);
	}

	usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "  phy type:optional parameter.\n");
	if(usage_len == (RET_STRING_LEN_MAX - 1)) {
		ret = -1;
		goto get_usage_finish;
	}
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);

	serdesPhyCnt = ECNT_EVENT_SERDES_PHY_MAX;
	for(serdesPhyIdx = 0; serdesPhyIdx < serdesPhyCnt; serdesPhyIdx++) {
		name = get_serdes_phy_name((ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t)serdesPhyIdx);
		if(name == NULL) {
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "get serdes ether type:%d name failed!\n", serdesPhyIdx);
			ret = -1;
			goto get_usage_finish;
		}
		usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "    %d:%s\n", serdesPhyIdx, name);
		if(usage_len == (RET_STRING_LEN_MAX - 1)) {
			ret = -1;
			goto get_usage_finish;
		}

		SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);
	}

	usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "  -n: optional parameter, auto reboot after set.\n");
	if(usage_len == (RET_STRING_LEN_MAX - 1)) {
		ret = -1;
		goto get_usage_finish;
	}
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);

	ret = 0;
		
get_usage_finish:
	if(usage) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d, strlen(usage):%d\n", usage, usage_len, strlen(usage));
		if(copy_to_user((void __user *)serdes->name, usage, strlen(usage) + 1)) {
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes usage error.\n");
			ret = -1;
		}

		vfree(usage);
	}
	return ret;
}

static int getSerdesSelStatus(serdesCfgIoctl_t *serdes)
{
	int serdes_port = 0;
	int ret = 0;
	unsigned long val = 0;
	
	int serdesCnt = 0;
	char *usage, *name;
	size_t usage_len = 0;
	uint8_t serdes_if = 0;
	char *etherType = NULL;
	char *phyType = NULL;
	ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t ether_type;
	ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t phy_sel;

	usage = vmalloc(RET_STRING_LEN_MAX);
	if(!usage) {
		ret = -1;
		goto get_status_finish;
	}

	serdesCnt = ECNT_EVENT_SERDES_SEL_MAX;
	for(serdes_port = 0; serdes_port < serdesCnt; serdes_port++) {
		serdes_if = get_serdes_interface_sel((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
		if(serdes_if == UNKNOW_SERDES_IF) {
			name = get_serdes_port_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
			if(name == NULL) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "get serdes port:%d name failed!\n", serdes_port);
			} else {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "get serdes port %d:%s error.\n", serdes_port, name);
			}
			ret = -1;
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s\n", usage);
			goto get_status_finish;
		} else {
			/* show ether type, serdes port=serdes interface
			 * ex: etherLAN, SerDes-Ethernet=XFI
			 */

			/* show serdes phy tpye */
			phy_sel = get_serdes_phy_sel((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
			if(phy_sel < 0) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "\nget serdes port:%d phy failed!\n", serdes_port);
				ret = -1;
				goto get_status_finish;
			}
			name = get_serdes_phy_name(phy_sel);
			if(name == 0) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "\nget serdes port:%d phy_sel:%d name failed!\n", serdes_port, (int)phy_sel);
				ret = -1;
				goto get_status_finish;
			}
			phyType = name;

			usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "%s, ", phyType);
			if(usage_len == (RET_STRING_LEN_MAX - 1)) {
				ret = -1;
				goto get_status_finish;
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);
			
			/* show ether type */
			if(isEN7523 &&
				(serdes_if == ECNT_EVENT_SERDES_PON_PON ||
				serdes_if == ECNT_EVENT_SERDES_WIFI1_PCIE ||
				serdes_if == ECNT_EVENT_SERDES_WIFI2_PCIE ||
				serdes_if == ECNT_EVENT_SERDES_USB1_USB30)) {
				/* This field at EN7523 is only valid when the serdes interface is selected to HSGMII. */
				etherType = "NONE";
			} else {
				ether_type = get_serdes_ether_type_sel((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
				if(ether_type < 0) {
					SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "\nget serdes port:%d etyer type failed!\n", serdes_port);
					ret = -1;
					goto get_status_finish;
				}
				name = get_serdes_ether_type_name(ether_type);
				if(name == 0) {
					SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "\nget serdes port:%d etyer type:%d name failed!\n", serdes_port, (int)ether_type);
					ret = -1;
					goto get_status_finish;
				}
				etherType = name;
			}
			usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - usage_len - 1, "%s, ", etherType);
			if(usage_len == (RET_STRING_LEN_MAX - 1)) {
				ret = -1;
				goto get_status_finish;
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);

			/* show port=serdes interface */
			name = get_serdes_port_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
			if(name == NULL) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "get serdes port:%d name failed!\n", serdes_port);
				ret = -1;
				goto get_status_finish;
			}
			usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - 1, "%s=", name);
			if(usage_len == (RET_STRING_LEN_MAX - 1)) {
				ret = -1;
				goto get_status_finish;
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);

			name = get_serdes_interface_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port, serdes_if);
			if(name == 0) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "\nget serdes port:%d interface:%d name failed!\n", serdes_port, (int)serdes_if);
				ret = -1;
				goto get_status_finish;
				
			}

			usage_len += snprintf(usage + usage_len, RET_STRING_LEN_MAX - 1, "%s\n", name);
			if(usage_len == (RET_STRING_LEN_MAX - 1)) {
				ret = -1;
				goto get_status_finish;
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d\n", usage, usage_len);
		}
	}

	ret = 0;

get_status_finish:
	if(usage) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "usage:%s, usage_len:%d, strlen(usage):%d\n", usage, usage_len, strlen(usage));
		if(copy_to_user((void __user *)serdes->name, usage, strlen(usage) + 1)) {
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes usage error.\n");
			ret = -1;
		}

		vfree(usage);
	}
	return ret;
}

#if defined(TCSUPPORT_CPU_EN7581)
/*set_serdes_ether_sel is forced updating ether type region of serdes-Ethernet in struct serdes_sel[] */
int set_serdes_ether_sel(int port_idx, int value) // lan or wan)
{
	int _serdes_ethernet_sel = 0;

	if(check_serdes_if(port_idx, SERDES_IF_IGNORE)) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", __func__, __LINE__, (int)port_idx);
		return -1; 
	}
	_serdes_ethernet_sel = get_serdes_sel(port_idx);
	_serdes_ethernet_sel &= 0xF0F;
	
	if (value > ECNT_EVENT_SERDES_ETHER_TYPE_MAX) 
	{
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error cannot set ether type greater than ECNT_EVENT_SERDES_ETHER_TYPE_MAX.\n", __func__, __LINE__);
		return -1;
	}
	
	_serdes_ethernet_sel |=((value & SERDES_ETHER_TYPE_SEL_MASK) << SERDES_ETHER_TYPE_SEL_OFFSET);
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "serdes sel ether type is updated with value %d\n", __func__, __LINE__,  value);
	serdes_sel[(ECNT_EVENT_SYSTEM_SERDES_SEL_t)port_idx]=_serdes_ethernet_sel;
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "serdes sel struct updated \n", __func__, __LINE__);
return 0;		
}
#endif

long serdesCfgIoctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long ret = 0;
	serdesCfgIoctl_t __serdesCfg;
	serdesCfgIoctl_t *userCfg = (serdesCfgIoctl_t *)arg;
	int result;
	uint8_t sel;
	const char *name;

	if (_IOC_TYPE(cmd) != SERDES_CFG_IOC_MAGIC) {
		return -ENOTTY;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	ret = !access_ok(VERIFY_WRITE, (void __user *)userCfg, _IOC_SIZE(cmd));
	if (ret) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "_IOC_WRITE check error.\n");
		return -EFAULT;
	}
	ret = !access_ok(VERIFY_READ, (void __user *)userCfg, _IOC_SIZE(cmd));
	if (ret) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "_IOC_WRITE check error.\n");
		return -EFAULT;
	}
#else
	if ((_IOC_DIR(cmd) & _IOC_READ) || 
		(_IOC_DIR(cmd) & _IOC_WRITE)) {
		ret = !access_ok( (void __user *)userCfg, _IOC_SIZE(cmd));
		if (ret) {
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "_IOC_READ or _IOC_WRITE check error.\n");
			return -EFAULT;
		}
	}
#endif

	if(copy_from_user(&__serdesCfg, userCfg, sizeof(serdesCfgIoctl_t))) {
		SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy_from_user failed\n",__func__);
		return -EFAULT;
	}

	
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "__serdesCfg.port_idx:%d\n", __serdesCfg.port_idx);
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "__serdesCfg.port_cnt:%d\n", __serdesCfg.port_cnt);
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "__serdesCfg.ether_type:%d\n", __serdesCfg.ether_type);
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "__serdesCfg.ether_type_cnt:%d\n", __serdesCfg.ether_type_cnt);
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "__serdesCfg.serdes_if:%d\n", __serdesCfg.serdes_if);
	SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "__serdesCfg.serdes_if_cnt:%d\n", __serdesCfg.serdes_if_cnt);

	switch(cmd)
	{
#if 0 /* Unused */
	    case SERDES_CFG_IOCTL_GET_PORT_CNT:
			result = get_serdes_port_count();
			ret = copy_to_user((void __user *)(&userCfg->port_cnt), &result, sizeof(result));			
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes port count error.\n");
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "serdesCfg.port_cnt:%d\n", userCfg->port_cnt);
			break;
		case SERDES_CFG_IOCTL_GET_IF_CNT:
			if(check_serdes_if(__serdesCfg.port_idx, SERDES_IF_IGNORE)) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", (int)__serdesCfg.port_idx);
				return -EFAULT;
			}
			result = get_serdes_interface_count(__serdesCfg.port_idx);
			ret = copy_to_user((void __user *)(&userCfg->serdes_if_cnt), &result, sizeof(result));			
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes interface count error.\n");
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "serdes_if_cnt:%d\n", userCfg->serdes_if_cnt);
			break;
		case SERDES_CFG_IOCTL_GET_PORT_NAME:
			if(check_serdes_if(__serdesCfg.port_idx, SERDES_IF_IGNORE)) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", (int)__serdesCfg.port_idx);
				return -EFAULT;
			}
			name = get_serdes_port_name(__serdesCfg.port_idx);
			ret = copy_to_user((void __user *)userCfg->name, name, strlen(name) + 1);			
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes port name error.\n");
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "SERDES_CFG_IOCTL_GET_PORT_NAME name:%s\n", name);
			break;
		case SERDES_CFG_IOCTL_GET_IF_NAME:
			if(check_serdes_if(__serdesCfg.port_idx, SERDES_IF_IGNORE)) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", (int)__serdesCfg.port_idx);
				return -EFAULT;
			}
			name = get_serdes_interface_name(__serdesCfg.port_idx, __serdesCfg.serdes_if);
			ret = copy_to_user((void __user *)userCfg->name, name, strlen(name) + 1);			
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes interface name error.\n");
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "SERDES_CFG_IOCTL_GET_IF_NAME name:%s\n", name);
			break;
		case SERDES_CFG_IOCTL_GET_ETHER_TYPE_NAME:
			name = get_serdes_ether_type_name(__serdesCfg.ether_type);
			if(name == NULL) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy ether type name error.\n");
				return -EFAULT;
			}
			ret = copy_to_user((void __user *)userCfg->name, name, strlen(name) + 1);			
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy data error.\n");
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "SERDES_CFG_IOCTL_GET_ETHER_TYPE_NAME name:%s\n", name);
			break;
		case SERDES_CFG_IOCTL_GET_ETHER_TYPE_CNT:
			result = get_serdes_ether_type_count();
			ret = copy_to_user((void __user *)(&userCfg->ether_type_cnt), &result, sizeof(result));			
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes ether type count error.\n");
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "ether_type_cnt:%d\n", userCfg->ether_type_cnt);
			break;
		case SERDES_CFG_IOCTL_CHECK_SERDES_ARG:
			if(check_serdes_if(__serdesCfg.port_idx, __serdesCfg.serdes_if)) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d or serdes_if:%d.\n", (int)__serdesCfg.port_idx, (int)__serdesCfg.serdes_if);
				return -EFAULT;
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "\n");
			break;
#endif
		case SERDES_CFG_IOCTL_GET_PORT_ENV_NAME:
			if(check_serdes_if(__serdesCfg.port_idx, SERDES_IF_IGNORE)) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", (int)__serdesCfg.port_idx);
				return -EFAULT;
			}
			name = get_env_serdes_port_name(__serdesCfg.port_idx);
			ret = copy_to_user((void __user *)userCfg->name, name, strlen(name) + 1);			
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes port env name error.\n");
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "SERDES_CFG_IOCTL_GET_PORT_ENV_NAME name:%s\n", name);
			break;
		case SERDES_CFG_IOCTL_GET_IF_SEL:
			if(check_serdes_if(__serdesCfg.port_idx, SERDES_IF_IGNORE)) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", (int)__serdesCfg.port_idx);
				return -EFAULT;
			}
			sel = get_serdes_interface_sel(__serdesCfg.port_idx);
			ret = copy_to_user((void __user *)(&userCfg->serdes_if), &sel, sizeof(sel));			
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes interface selection error.\n");
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "serdes_if:%d\n", userCfg->serdes_if);
			break;
		case SERDES_CFG_IOCTL_GET_ETHER_TYPE_SEL:
			if(check_serdes_if(__serdesCfg.port_idx, SERDES_IF_IGNORE)) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error port_idx:%d.\n", (int)__serdesCfg.port_idx);
				return -EFAULT;
			}
			result = get_serdes_ether_type_sel(__serdesCfg.port_idx);
			ret = copy_to_user((void __user *)(&userCfg->ether_type), &result, sizeof(result));			
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes ether type selection error.\n");
			}
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_TRACE, "ether_type:%d\n", userCfg->ether_type);
			break;
		case SERDES_CFG_IOCTL_GET_USAGE:
			ret = getSerdesSelUsage(userCfg);
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes usage error.\n");
			}
			break;
		case SERDES_CFG_IOCTL_GET_STATUS:
			ret = getSerdesSelStatus(userCfg);
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "copy serdes status error.\n");
			}
			break;
            #if defined(TCSUPPORT_CPU_EN7581)
		case SERDES_CFG_IOCTL_SET_ETHSEL_ETHTYPE:
			ret = set_serdes_ether_sel(ECNT_EVENT_SERDES_SEL_ETHER, __serdesCfg.ether_type);
			if(ret) {
				SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "error setting ether type in serdes-ethernet .\n");
			}
			break;
            #endif
		default:
			SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "unknown cmd:%d.\n", cmd);
			return -EFAULT;
	}
	return ret;
}


static struct file_operations serdesCfgFops = {
	.owner =			THIS_MODULE,
	.write =			NULL,
	.read =				NULL,
	.unlocked_ioctl =	serdesCfgIoctl,	
#ifdef TCSUPPORT_CPU_ARMV8_64
	.compat_ioctl	= 	serdesCfgIoctl,
#endif
	.open =				NULL,
	.release =			NULL,
};

static int ECNT_SERDES_CFG_PROBE(void)
{
	int ret = 0;
		
	ret = register_chrdev(SERDES_CFG_MAJOR, "serdes_cfg", &serdesCfgFops);
	if (ret < 0) {    
    	SERDES_MSG(SERDES_CFG_DBG_LEVEL_ERROR, "serdes cfg create ioctrl fail\n");
		return ret;
    }
	
	return 0;
}
subsys_initcall(ECNT_SERDES_CFG_PROBE);

