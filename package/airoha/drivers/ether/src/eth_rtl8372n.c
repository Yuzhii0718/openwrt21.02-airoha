#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/if_vlan.h>

#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>

#include <rtk_switch.h>
#include <rtk_error.h>
#include <port.h>
#include <mib.h>
#include <svlan.h>
#include <vlan.h>
#include <acl.h>
#include <l2.h>
#include <igmp.h>
#include <isolation.h>
#include <mirror.h>
#include <phy.h>

#include <dal_rtl8373_switch.h>
#include <dal_rtl8373_svlan.h>
#include <dal_rtl8373_lut.h>

#include <Thermal_Meter.h>

#define SDS_PORT0 UTP_PORT3
#define SDS_PORT1 UTP_PORT8

#define LAN_PORT0 UTP_PORT4
#define LAN_PORT1 UTP_PORT5
#define LAN_PORT2 UTP_PORT6
#define LAN_PORT3 UTP_PORT7

#define RTL8372N_PHY_LINK_CHECK_INTERVAL (1000) /*ms*/

#define RTL8372N_FID_RESERVED_NUM (2) /* fid 0 & 1 not used */
#define RTL8372N_VID_RESERVED_NUM (2) /* vid 0 & 1 not used */

#define PORT_NUM (sizeof(ports) / sizeof(ports[0]))

extern u32 GET_GPIO_CTRL(void);
extern void SET_GPIO_CTRL(u32 data);
extern u32 GET_GPIO_CTRL1(void);
extern void SET_GPIO_CTRL1(u32 data);

extern u32 GET_GPIO_DATA(void);
extern void SET_GPIO_DATA(u32 data);
extern u32 GET_GPIO_OE(void);
extern void SET_GPIO_OE(u32 data);

extern void tcMiiStationWrite(uint32 phy_addr, uint32 phy_reg, uint32 phy_data);
extern uint32 tcMiiStationRead(uint32 phy_addr, uint32 phy_reg);

typedef enum
{
	PORT_IDX0 = 0,
	PORT_IDX1 = 1,
	PORT_IDX2 = 2,
	PORT_IDX3 = 3,
	PORT_IDX4 = 4,
	PORT_MAX_NUM,
} port_idx;

static const rtk_port_t rtl8372n_utp_port[PORT_MAX_NUM] = {
	LAN_PORT0,
	LAN_PORT1,
	LAN_PORT2,
	LAN_PORT3,
	SDS_PORT1,
};

static struct proc_dir_entry *procRegDir = NULL;
static const rtk_port_t rtl8372n_cpu_port = SDS_PORT0;

static const char *rtl8372n_dev_name[PORT_MAX_NUM] = {
	"eth0.8.2",
	"eth0.8.3",
	"eth0.8.4",
	"eth0.8.5",
	"eth0.8.6",
};

static int rtl8372n_br_group[PORT_MAX_NUM] = {
	-1,
	-1,
	-1,
	-1,
	-1,
};

static int rtl8372n_iso_mode[PORT_MAX_NUM] = {
	0,
	0,
	0,
	0,
	0,
};

static int rtl8372n_mc_router[PORT_MAX_NUM] = {
	1,
	1,
	1,
	1,
	1,
};

static int rtl8372n_link_status[PORT_MAX_NUM] = {
	-1,
	-1,
	-1,
	-1,
	-1,
};

static int rtl8372n_hw_bridge_port[PORT_MAX_NUM] = {
	1,
	1,
	1,
	1,
	1,
};

#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
static int rtl8372n_hw_bridge_vlan_port[PORT_MAX_NUM] = {
	0,
	0,
	0,
	0,
	0,
};
#endif /**/


static int rtl8372n_mii_type = -1;
static int rtl8372n_mcast_snoop = 1;
static int rtl8372n_hw_bridge = 1;
static int rtl8372n_mib_level = 0;
static int rtl8372n_eth_forward = 0; /* 0: init forbid forwarding */
unsigned char rtl8372n_cpu_mac[6] = {0, 0, 0, 0, 0, 0};
static int rtl8372n_debug_info = 0;
static int rtl8372n_phy_testmode[5] = {0, 0, 0, 0, 0};
switch_chip_t g_switch_chip = CHIP_END;
static spinlock_t rtl8372n_lock;

#define RTL8372N_LOCK_INIT()            \
	do                                  \
	{                                   \
		spin_lock_init(&rtl8372n_lock); \
	} while (0)

#define RTL8372N_LOCK()               \
	do                                \
	{                                 \
		spin_lock_bh(&rtl8372n_lock); \
	} while (0)


#define RTL8372N_UNLOCK()               \
	do                                  \
	{                                   \
		spin_unlock_bh(&rtl8372n_lock); \
	} while (0)

#define RTL8372N_LOG_ERR(fmt, ...)                                                  \
	do                                                                              \
	{                                                                               \
		printk("[RTL8372N ERROR] (%s %d) " fmt, __func__, __LINE__, ##__VA_ARGS__); \
	} while (0)

#define RTL8372N_LOG_INFO(fmt, ...)                    \
	do                                                 \
	{                                                  \
		printk("[RTL8372N INFO] " fmt, ##__VA_ARGS__); \
	} while (0)

#define RTL8372N_LOG_DEBUG(fmt, ...)                                                    \
	do                                                                                  \
	{                                                                                   \
		if (rtl8372n_debug_info)                                                        \
		{                                                                               \
			printk("[RTL8372N DEBUG] (%s %d) " fmt, __func__, __LINE__, ##__VA_ARGS__); \
		}                                                                               \
	} while (0)

static int gpio_set_oen(const unsigned int gpio)
{
	uint32 reg_value;
	uint32 ctrl_offset = 0;
	uint32 oe_offset = 0;
	u32 (*get_ctrl_reg)(void) = NULL;
	void (*set_ctrl_reg)(u32) = NULL;
	u32 (*get_oe_reg)(void) = NULL;
	void (*set_oe_reg)(u32) = NULL;

	if (gpio < 16)
	{
		get_ctrl_reg = GET_GPIO_CTRL;
		set_ctrl_reg = SET_GPIO_CTRL;
		get_oe_reg = GET_GPIO_OE;
		set_oe_reg = SET_GPIO_OE;
		ctrl_offset = gpio * 2;
		oe_offset = gpio;
	}
	else if (gpio < 32)
	{
		get_ctrl_reg = GET_GPIO_CTRL1;
		set_ctrl_reg = SET_GPIO_CTRL1;
		get_oe_reg = GET_GPIO_OE;
		set_oe_reg = SET_GPIO_OE;
		ctrl_offset = (gpio - 16) * 2;
		oe_offset = gpio;
	}
	else
	{
		printk("gpio_set_oen %d not implemented\n", gpio);
		return -1;
	}

	if (get_ctrl_reg && set_ctrl_reg && set_oe_reg)
	{
		reg_value = get_ctrl_reg();
		reg_value = reg_value & ~(0x3 << ctrl_offset);
		reg_value = reg_value | (0x1 << ctrl_offset);
		set_ctrl_reg(reg_value);

		reg_value = get_oe_reg();
		reg_value = reg_value | (0x1 << oe_offset);
		set_oe_reg(reg_value);
	}

	return 0;
}

static int gpio_set_low(const unsigned int gpio)
{
	uint32 reg_value;
	uint32 data_offset = 0;
	u32 (*get_data_reg)(void) = NULL;
	void (*set_data_reg)(u32) = NULL;

	if (gpio < 32)
	{
		get_data_reg = GET_GPIO_DATA;
		set_data_reg = SET_GPIO_DATA;
		data_offset = gpio;
	}
	else
	{
		printk("gpio_set_low %d not implemented\n", gpio);
		return -1;
	}

	if (get_data_reg && set_data_reg)
	{
		reg_value = get_data_reg();
		reg_value = reg_value & ~(0x1 << data_offset);
		set_data_reg(reg_value);
	}

	return 0;
}

static int gpio_set_high(const unsigned int gpio)
{
	uint32 reg_value;
	uint32 data_offset = 0;
	u32 (*get_data_reg)(void) = NULL;
	void (*set_data_reg)(u32) = NULL;

	if (gpio < 32)
	{
		get_data_reg = GET_GPIO_DATA;
		set_data_reg = SET_GPIO_DATA;
		data_offset = gpio;
	}
	else
	{
		printk("gpio_set_low %d not implemented\n", gpio);
		return -1;
	}

	if (get_data_reg && set_data_reg)
	{
		reg_value = get_data_reg();
		reg_value = reg_value | (0x1 << data_offset);
		set_data_reg(reg_value);
	}

	return 0;
}

static int rtl8372n_reset(const unsigned int gpio)
{
	gpio_set_oen(gpio);
	msleep(100);
	gpio_set_low(gpio);
	msleep(200);
	gpio_set_high(gpio);
	msleep(500);

	return 0;
}

void rtl8372_mdio_write(unsigned int phy_addr, unsigned int phy_reg, unsigned int phy_data)
{
	tcMiiStationWrite(phy_addr, phy_reg, phy_data);
}

void rtl8372_mdio_read(unsigned int phy_addr, unsigned int phy_reg, unsigned int *pData)
{
	*pData = tcMiiStationRead(phy_addr, phy_reg);
}

void rtl8372_mdio_read_h(unsigned int phy_addr, unsigned int phy_reg, unsigned short *pData)
{
	*pData = tcMiiStationRead(phy_addr, phy_reg);
}

static int _rtl8372n_acl_refresh(void)
{
	rtk_api_ret_t ret = RT_ERR_OK;
	rtk_filter_field_t filter_field;
	rtk_filter_cfg_t cfg;
	rtk_filter_action_t act;
	rtk_filter_number_t ruleNum;
	memset(&cfg, 0, sizeof(rtk_filter_cfg_t));
	memset(&act, 0, sizeof(rtk_filter_action_t));
	memset(&filter_field, 0, sizeof(rtk_filter_field_t));

	filter_field.fieldType = FILTER_FIELD_DMAC;
	filter_field.filter_pattern_union.dmac.dataType = FILTER_FIELD_DATA_MASK;
	filter_field.filter_pattern_union.dmac.value.octet[0] = 0x01; /* ieee 1905 message */
	filter_field.filter_pattern_union.dmac.value.octet[1] = 0x80;
	filter_field.filter_pattern_union.dmac.value.octet[2] = 0xC2;
	filter_field.filter_pattern_union.dmac.value.octet[3] = 0x00;
	filter_field.filter_pattern_union.dmac.value.octet[4] = 0x00;
	filter_field.filter_pattern_union.dmac.value.octet[5] = 0x13;

	filter_field.filter_pattern_union.dmac.mask.octet[0] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[1] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[2] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[3] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[4] = 0xFF;
	filter_field.filter_pattern_union.dmac.mask.octet[5] = 0xFF;

	if ((ret = rtk_filter_igrAcl_field_add(&cfg, &filter_field)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_filter_igrAcl_field_add fail...ret=%d\n", ret);
		return ret;
	}

	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT4);
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT5);
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT6);
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT7);
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, UTP_PORT8);
	RTK_PORTMASK_ALLPORT_SET(cfg.activeport.mask);
	cfg.invert = FALSE;
	act.actEnable[FILTER_ENACT_REDIRECT] = TRUE;
	act.filterPortmask.bits[0] = (1 << rtl8372n_cpu_port);
	if ((ret = rtk_filter_igrAcl_cfg_add(0, &cfg, &act, &ruleNum)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_filter_igrAcl_cfg_add fail...ret=%d\n", ret);
		return ret;
	}
	return ret;
}

static int rtl8372n_acl_init(void)
{
	int ret = 0;
	RTL8372N_LOG_DEBUG("init\r\n");
	if ((ret = rtk_filter_igrAcl_init()) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_filter_igrAcl_init fail...ret=%d\n", ret);
		return ret;
	}
	_rtl8372n_acl_refresh();
	return 0;
}

static rtk_uint32 _rtl8372_mii_type_set(rtk_port_speed_t speed, rtk_sds_mode_t mode)
{
	int ret = 0;
	rtk_port_ability_t ability;
	rtk_uint32 sds_port = SERDES_ID0;

	memset(&ability, 0x0, sizeof(ability));

	if (rtl8372n_cpu_port == UTP_PORT3)
	{
		sds_port = SERDES_ID0;
	}
	else if (rtl8372n_cpu_port == UTP_PORT8)
	{
		sds_port = SERDES_ID1;
	}
	else
	{
		return -1;
	}

	if ((ret = rtk_sdsMode_set(sds_port, mode)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_port_macForceLinkExt_set failed...ret=%d\n", ret);
		return ret;
	}

	ability.speed = speed;
	ability.forcemode = ENABLED;
	ability.duplex = PORT_FULL_DUPLEX;

	ability.link = PORT_LINKUP;
	ability.txpause = ENABLED;
	ability.rxpause = ENABLED;
	ability.media = PORT_MEDIA_FIBER;

	if ((ret = rtk_port_macForceLink_set(rtl8372n_cpu_port, &ability)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_port_macForceLink_set failed...ret=%d\n", ret);
		return ret;
	}

	if ((ret = rtk_port_sdsNway_set(sds_port, mode, DISABLED)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_port_sdsNway_set failed...ret=%d\n", ret);
		return ret;
	}

	return 0;
}

static rtk_uint32 _rtl8372n_mii_type_refresh(void)
{
	int ret = -1;

	if (rtl8372n_mii_type == 0)
	{
		/* RGMII, TODO */
	}
	else if (rtl8372n_mii_type == 1)
	{
		/* SGMII */
		ret = _rtl8372_mii_type_set(PORT_SPEED_1000M, SERDES_SG);
	}
	else if (rtl8372n_mii_type == 2)
	{
		/* HSGMII */
		ret = _rtl8372_mii_type_set(PORT_SPEED_2500M, SERDES_HSG);
	}
	else if (rtl8372n_mii_type == 3)
	{
		/* 10GR */
		ret = _rtl8372_mii_type_set(PORT_SPEED_10G, SERDES_10GR);
	}
	else
	{
		/* Unknown, skip here. Maybe config by proc later.*/
		RTL8372N_LOG_ERR("Unknown type %d, skip it\n", rtl8372n_mii_type);
		return -1;
	}

	if (ret != 0)
	{
		RTL8372N_LOG_ERR("Set type %d error\n", rtl8372n_mii_type);
		return ret;
	}

	return 0;
}

static rtk_uint32 _rtl8372n_lookup_calc_permit(int pidx)
{
	int p;

	rtk_uint32 ret = 0;
	if (!rtl8372n_eth_forward)
	{
		/* forbid forward */
		ret = 0;
		RTL8372N_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}

	ret = (1 << rtl8372n_cpu_port);
	if (!rtl8372n_hw_bridge)
	{
		ret |= (1 << rtl8372n_utp_port[pidx]);
		RTL8372N_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
	if (!rtl8372n_hw_bridge_port[pidx])
	{
		ret |= (1 << rtl8372n_utp_port[pidx]);
		RTL8372N_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
	if (rtl8372n_hw_bridge_vlan_port[pidx])
	{
		ret |= (1 << rtl8372n_utp_port[pidx]);
		RTL8372N_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
#endif /**/
	if (rtl8372n_br_group[pidx] < 0 ||
		rtl8372n_iso_mode[pidx])
	{
		ret |= (1 << rtl8372n_utp_port[pidx]);
		RTL8372N_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
#if 0 /* xmit to source port? */
		if(p == pidx)
		{
			continue;
		}
#endif
		if (rtl8372n_br_group[p] != rtl8372n_br_group[pidx])
		{
			continue;
		}
		if (!rtl8372n_hw_bridge_port[p])
		{
			continue;
		}
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
		if (rtl8372n_hw_bridge_vlan_port[p])
		{
			continue;
		}
#endif /**/
		if (rtl8372n_iso_mode[p])
		{
			continue;
		}
		ret |= (1 << rtl8372n_utp_port[p]);
	}
	RTL8372N_LOG_DEBUG("ret=%x\n", ret);
	return ret;
}

static rtk_uint32 _rtl8372n_lookup_calc_fid(int pidx)
{
	int p;
	rtk_uint32 ret = 0;
	if (!rtl8372n_hw_bridge)
	{
		ret = (pidx + RTL8372N_FID_RESERVED_NUM);
		RTL8372N_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
	if (!rtl8372n_hw_bridge_port[pidx])
	{
		ret = (pidx + RTL8372N_FID_RESERVED_NUM);
		RTL8372N_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
	if (rtl8372n_hw_bridge_vlan_port[pidx])
	{
		ret = (pidx + RTL8372N_FID_RESERVED_NUM);
		RTL8372N_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
#endif /**/
	if (rtl8372n_br_group[pidx] < 0 ||
		rtl8372n_iso_mode[pidx])
	{
		ret = (pidx + RTL8372N_FID_RESERVED_NUM);
		RTL8372N_LOG_DEBUG("ret=%x\n", ret);
		return ret;
	}
	/* find first port in the same br group */
	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		if (rtl8372n_br_group[p] != rtl8372n_br_group[pidx])
		{
			continue;
		}

		if (!rtl8372n_hw_bridge_port[p])
		{
			continue;
		}
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
		if (rtl8372n_hw_bridge_vlan_port[p])
		{
			continue;
		}
#endif /**/
		if (rtl8372n_iso_mode[p])
		{
			continue;
		}
		ret = (p + RTL8372N_FID_RESERVED_NUM);
		break;
	}
	RTL8372N_LOG_DEBUG("ret=%x\n", ret);
	return ret;
}

static int _rtl8372n_lookup_config_port(int pidx, rtk_uint32 fid, rtk_uint32 permit)
{
	rtk_uint32 permitPort = 0;
	rtk_svlan_memberCfg_t svlanCfg;
	rtk_uint32 vid = (pidx + RTL8372N_VID_RESERVED_NUM);
	int ret = 0;
	/* set iso of the port */

	permitPort = permit;
	RTL8372N_LOG_DEBUG("rtk_port_isolation_set %d %x\r\n", rtl8372n_utp_port[pidx], permit);
	if ((ret = rtk_port_isolation_set(rtl8372n_utp_port[pidx], permitPort)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_port_isolation_set fail...ret=%d\n", ret);
		return ret;
	}

	RTL8372N_LOG_DEBUG("rtk_vlan_portFid_set port=%d fid=%d\r\n", rtl8372n_utp_port[pidx], fid);
	if ((ret = rtk_vlan_portFid_set(rtl8372n_utp_port[pidx], ENABLED, fid)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_vlan_portFid_set fail...ret=%d\n", ret);
		return ret;
	}

	/* set vlan of the port */
	memset(&svlanCfg, 0x0, sizeof(svlanCfg));
	svlanCfg.svid = vid;

	svlanCfg.chk_ivl_svl = 0;
	svlanCfg.ivl_svl = 1;

	svlanCfg.memberport.bits[0] = ((1 << rtl8372n_cpu_port) | (1 << (rtl8372n_utp_port[pidx])));
	svlanCfg.untagport.bits[0] = (1 << (rtl8372n_utp_port[pidx]));
	RTL8372N_LOG_DEBUG("rtk_svlan_memberPortEntry_set vid=%d fid=%d\r\n", vid, fid);
	if ((ret = dal_rtl8373_svlanMbrPortEntry_set(vid, &svlanCfg)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_svlan_memberPortEntry_set fail...ret=%d\n", ret);
		return ret;
	}

	RTL8372N_LOG_DEBUG("rtk_svlan_defaultSvlan_set port=%d vid=%d\r\n", rtl8372n_utp_port[pidx], vid);
	if ((ret = rtk_svlan_defaultSvlan_set(rtl8372n_utp_port[pidx], vid)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_svlan_defaultSvlan_set fail...ret=%d\n", ret);
		return ret;
	}

	if ((ret = rtk_vlan_portPvid_set(rtl8372n_utp_port[pidx], vid)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_vlan_portPvid_set fail...ret=%d\n", ret);
		return ret;
	}

	return ret;
}
static int _rtl8372n_set_port_acl(int pidx, rtk_filter_id_t acl_filter_id, rtk_uint32 fid)
{
	rtk_filter_field_t field1;
	rtk_filter_cfg_t cfg;
	rtk_filter_action_t act;
	rtk_filter_number_t ruleNum = 0;
	int ret = 0;
	memset(&field1, 0x00, sizeof(rtk_filter_field_t));
	memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
	memset(&act, 0x00, sizeof(rtk_filter_action_t));
	/*All packets will be matched*/
	field1.fieldType = FILTER_FIELD_DMAC;
	if ((ret = rtk_filter_igrAcl_field_add(&cfg, &field1)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_filter_igrAcl_field_add fail...ret=%d\n", ret);
		return ret;
	}

	/* Set port1 to active port */
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, rtl8372n_utp_port[pidx]);
	RTK_PORTMASK_ALLPORT_SET(cfg.activeport.mask);

	cfg.invert = FALSE;
	act.actEnable[FILTER_ENACT_CVLAN_INGRESS] = TRUE;
	act.filterCvlanVid = fid; /* CVID = fid */
	if ((ret = rtk_filter_igrAcl_cfg_add(acl_filter_id, &cfg, &act, &ruleNum)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_filter_igrAcl_cfg_add fail...ret=%d\n", ret);
		return ret;
	}
	return ret;
}

static int _rtl8372n_set_cpuport_acl(int pidx, rtk_filter_id_t acl_filter_id, rtk_uint32 fid)
{
	rtk_filter_field_t field1;
	rtk_filter_cfg_t cfg;
	rtk_filter_action_t act;
	rtk_filter_number_t ruleNum = 0;
	rtk_uint32 vid = (pidx + RTL8372N_VID_RESERVED_NUM);
	int ret = 0;
	memset(&field1, 0x00, sizeof(rtk_filter_field_t));
	memset(&cfg, 0x00, sizeof(rtk_filter_cfg_t));
	memset(&act, 0x00, sizeof(rtk_filter_action_t));
	/*All packets will be matched*/
	field1.fieldType = FILTER_FIELD_STAG;
	field1.filter_pattern_union.stag.vid.value = vid;
	field1.filter_pattern_union.stag.vid.mask = 0xfff;

	if ((ret = rtk_filter_igrAcl_field_add(&cfg, &field1)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_filter_igrAcl_field_add fail...ret=%d\n", ret);
		return ret;
	}

	/* Set port1 to active port */
	RTK_PORTMASK_PORT_SET(cfg.activeport.value, rtl8372n_cpu_port);
	RTK_PORTMASK_ALLPORT_SET(cfg.activeport.mask);

	cfg.invert = FALSE;
	act.actEnable[FILTER_ENACT_CVLAN_INGRESS] = TRUE;
	act.filterCvlanVid = fid; /* CVID = fid */
	if ((ret = rtk_filter_igrAcl_cfg_add(acl_filter_id, &cfg, &act, &ruleNum)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_filter_igrAcl_cfg_add fail...ret=%d\n", ret);
		return ret;
	}
	return ret;
}

static int _rtl8372n_lookup_refresh(void)
{
	int p;
	rtk_uint32 fid;
	rtk_uint32 permit;
	rtk_filter_id_t acl_filter_id = 1;
	int ret = 0;

	rtl8372n_acl_init();

	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		permit = _rtl8372n_lookup_calc_permit(p);
		fid = _rtl8372n_lookup_calc_fid(p);
		RTL8372N_LOG_DEBUG("permit=%x fid=%d\n", permit, fid);
		if ((ret = _rtl8372n_lookup_config_port(p, fid, permit)) != RT_ERR_OK)
		{
			return ret;
		}

		if ((ret = _rtl8372n_set_port_acl(p, acl_filter_id, fid)) != RT_ERR_OK)
		{
			return ret;
		}
		else
		{
			acl_filter_id++;
		}
		if ((ret = _rtl8372n_set_cpuport_acl(p, acl_filter_id, fid)) != RT_ERR_OK)
		{
			return ret;
		}
		else
		{
			acl_filter_id++;
		}
	}
	return ret;
}

static int _rtl8372n_mcast_mcr_refresh(void)
{
	rtk_portmask_t portmask;
	int p;

	memset(&portmask, 0x0, sizeof(portmask));

	if (rtl8372n_mcast_snoop)
	{
		portmask.bits[0] |= (1 << rtl8372n_cpu_port);

		for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
		{
			if (rtl8372n_mc_router[p] == 2)
			{
				portmask.bits[0] |= (1 << rtl8372n_utp_port[p]);
			}
		}
	}

	if (RT_ERR_OK != rtk_igmp_static_router_port_set(&portmask))
	{
		RTL8372N_LOG_ERR("rtk_igmp_static_router_port_set error\r\n");
	}

	return 0;
}

static int _rtl8372n_mcast_refresh(void)
{
	int p;
	if (rtl8372n_mcast_snoop)
	{
		rtk_igmp_state_set(ENABLED);
#if 0
		if (RT_ERR_OK != rtk_l2_ipMcastForwardRouterPort_set(ENABLED))
		{
			RTL8372N_LOG_ERR("rtk_l2_ipMcastForwardRouterPort_set error\r\n");
		}
#endif
		for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
		{
			if (RT_ERR_OK != dal_rtl8373_l2_unknV4Mc_action_set(
								 rtl8372n_utp_port[p], 3))
			{
				RTL8372N_LOG_ERR("rtk_trap_unknownMcastPktAction_set error\r\n");
			}
			if (RT_ERR_OK != dal_rtl8373_l2_unknV6Mc_action_set(
								 rtl8372n_utp_port[p], 3))
			{
				RTL8372N_LOG_ERR("rtk_trap_unknownMcastPktAction_set error\r\n");
			}
		}
	}
	else
	{

		rtk_igmp_state_set(DISABLED);
		#ifdef CONFIG_TP_IMAGE
			/* the function rtk_l2_ipMcastForwardRouterPort_set is not implemented, temporary comments */
		#else
		if (RT_ERR_OK != rtk_l2_ipMcastForwardRouterPort_set(DISABLED))
		{
			RTL8372N_LOG_ERR("rtk_l2_ipMcastForwardRouterPort_set error\r\n");
		}
		#endif /* CONFIG_TP_IMAGE */
		for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
		{
			if (RT_ERR_OK != dal_rtl8373_l2_unknV4Mc_action_set(
								 rtl8372n_utp_port[p], 0))
			{
				RTL8372N_LOG_ERR("rtk_trap_unknownMcastPktAction_set error\r\n");
			}
			if (RT_ERR_OK != dal_rtl8373_l2_unknV6Mc_action_set(
								 rtl8372n_utp_port[p], 0))
			{
				RTL8372N_LOG_ERR("rtk_trap_unknownMcastPktAction_set error\r\n");
			}
		}
	}
	_rtl8372n_mcast_mcr_refresh();
	return 0;
}

static int _rtl8372n_cpumac_config_port(int pidx, rtk_uint32 fid, rtk_uint32 permit)
{
	rtk_l2_ucastAddr_t l2_entry;
	rtk_mac_t mac;
	int ret = 0;
	memset(&mac, 0x00, sizeof(mac));
	mac.octet[0] = rtl8372n_cpu_mac[0];
	mac.octet[1] = rtl8372n_cpu_mac[1];
	mac.octet[2] = rtl8372n_cpu_mac[2];
	mac.octet[3] = rtl8372n_cpu_mac[3];
	mac.octet[4] = rtl8372n_cpu_mac[4];
	mac.octet[5] = rtl8372n_cpu_mac[5];
	memset(&l2_entry, 0x00, sizeof(l2_entry));
	l2_entry.port = rtl8372n_cpu_port;
	l2_entry.vid_fid = fid;

	l2_entry.is_static = 1;
	if ((ret = rtk_l2_addr_add(&mac, &l2_entry)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_l2_addr_add fail...ret=%d\n", ret);
		return ret;
	}

	return ret;
}

static int _rtl8372n_cpumac_refresh(void)
{
	int p;
	rtk_uint32 fid;
	rtk_uint32 permit;
	int ret = 0;
	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		if (rtl8372n_br_group[p] < 0)
		{
			/* skip none bridge port */
			continue;
		}

		permit = _rtl8372n_lookup_calc_permit(p);
		fid = _rtl8372n_lookup_calc_fid(p);
		RTL8372N_LOG_DEBUG("permit=%x fid=%d\n", permit, fid);
		if ((ret = _rtl8372n_cpumac_config_port(p, fid, permit)) != RT_ERR_OK)
		{
			return ret;
		}
	}
	return ret;
}

static inline int _rtl8372n_is_switch_device(struct net_device *dev)
{
	if (dev->priv_switch_flags & IFF_TP_EXT_SWITCH_DEV)
	{
		return 1;
	}
	return 0;
}

static int _rtl8372n_device_to_pidx(struct net_device *dev)
{
	int ret = -1;
	if (!_rtl8372n_is_switch_device(dev))
	{
		return -1;
	}
	ret = (dev->priv_switch_flags & IFF_TP_EXT_SWITCH_IDX_MASK);
	if (ret >= PORT_MAX_NUM)
	{
		return -1;
	}
	return ret;
}

static int rtl8372n_br_should_forward(struct net_device *in, struct net_device *out)
{
	int pin, pout;

	if (!in || !out)
	{
		// RTL8372N_LOG_ERR("in or out null\r\n");
		return 1;
	}

	if (!rtl8372n_hw_bridge)
	{
		return 1;
	}

	pin = _rtl8372n_device_to_pidx(in);
	pout = _rtl8372n_device_to_pidx(out);
	if (pin < 0 || pout < 0)
	{
		return 1;
	}
	if (!rtl8372n_hw_bridge_port[pin] || !rtl8372n_hw_bridge_port[pout])
	{
		return 1;
	}
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
	if (rtl8372n_hw_bridge_vlan_port[pin] || rtl8372n_hw_bridge_vlan_port[pout])
	{
		return 1;
	}
#endif /**/
	/* forbid tx/rx between hw switch ports by linux bridge */
	return 0;
}

static int rtl8372n_notify_if_change(struct net_device *br, struct net_device *dev, int add)
{
	int p;
	int old_group;
	int new_group;
	int ret = 0;
	if (!br || !dev)
	{
		RTL8372N_LOG_ERR("br or dev null\r\n");
		return -1;
	}
	if (!_rtl8372n_is_switch_device(dev))
	{
		return 0;
	}
	RTL8372N_LOG_DEBUG("%s %s %d\r\n", br->name, dev->name, add);
	RTL8372N_LOCK();
	p = _rtl8372n_device_to_pidx(dev);
	if (p < 0)
	{
		RTL8372N_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		ret = -1;
		goto error;
	}

	old_group = rtl8372n_br_group[p];

	if (add)
	{
		if (old_group >= 0)
		{
			RTL8372N_LOG_ERR("wrong old group index %d\r\n", old_group);
			ret = -1;
			goto error;
		}
		new_group = br->ifindex;
	}
	else
	{
		if (old_group < 0)
		{
			RTL8372N_LOG_ERR("wrong old group index %d\r\n", old_group);
			ret = -1;
			goto error;
		}
		new_group = -1;
	}

	if (old_group == new_group)
	{
		RTL8372N_LOG_ERR("same group %d\r\n", old_group);
		ret = -1;
		goto error;
	}

	rtl8372n_br_group[p] = new_group;
	rtl8372n_iso_mode[p] = 0;
	rtl8372n_mc_router[p] = 1;

	if (rtl8372n_hw_bridge)
	{
		_rtl8372n_lookup_refresh();
	}

	if (rtl8372n_mcast_snoop)
	{
		_rtl8372n_mcast_mcr_refresh();
	}
	rtk_l2_table_clear();
	if (rtl8372n_cpu_mac[0] != 0 ||
		rtl8372n_cpu_mac[1] != 0 ||
		rtl8372n_cpu_mac[2] != 0 ||
		rtl8372n_cpu_mac[3] != 0 ||
		rtl8372n_cpu_mac[4] != 0 ||
		rtl8372n_cpu_mac[5] != 0)
	{
		_rtl8372n_cpumac_refresh();
	}
error:
	RTL8372N_UNLOCK();
	return ret;
}

static int rtl8372n_notify_iso_change(struct net_device *br, struct net_device *dev, int iso)
{
	int p;
	int cur_group;
	int tmp_group;
	int ret = 0;
	if (!br || !dev)
	{
		RTL8372N_LOG_ERR("br or dev null\r\n");
		return -1;
	}
	if (!_rtl8372n_is_switch_device(dev))
	{
		return 0;
	}
	RTL8372N_LOG_DEBUG("%s %s %d\r\n", br->name, dev->name, iso);
	RTL8372N_LOCK();
	p = _rtl8372n_device_to_pidx(dev);
	if (p < 0)
	{
		RTL8372N_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		ret = -1;
		goto error;
	}
	cur_group = rtl8372n_br_group[p];

	tmp_group = br->ifindex;
	if (tmp_group != cur_group)
	{
		RTL8372N_LOG_ERR("not same group %d\r\n", cur_group);
		ret = -1;
		goto error;
	}
	if (rtl8372n_iso_mode[p] == iso)
	{
		RTL8372N_LOG_ERR("same iso %d\r\n", iso);
		ret = -1;
		goto error;
	}
	rtl8372n_iso_mode[p] = iso;
	if (rtl8372n_hw_bridge)
	{
		_rtl8372n_lookup_refresh();
	}
	rtk_l2_table_clear();

	if (rtl8372n_cpu_mac[0] != 0 ||
		rtl8372n_cpu_mac[1] != 0 ||
		rtl8372n_cpu_mac[2] != 0 ||
		rtl8372n_cpu_mac[3] != 0 ||
		rtl8372n_cpu_mac[4] != 0 ||
		rtl8372n_cpu_mac[5] != 0)
	{
		_rtl8372n_cpumac_refresh();
	}
error:
	RTL8372N_UNLOCK();
	return ret;
}

static int rtl8372n_notify_mcr_change(struct net_device *br, struct net_device *dev, int mcr)
{
	int p;
	int cur_group;
	int tmp_group;
	int ret = 0;
	if (!br || !dev)
	{
		RTL8372N_LOG_ERR("br or dev null\r\n");
		return -1;
	}
	if (!_rtl8372n_is_switch_device(dev))
	{
		return 0;
	}
	RTL8372N_LOG_DEBUG("%s %s %d\r\n", br->name, dev->name, mcr);
	RTL8372N_LOCK();
	p = _rtl8372n_device_to_pidx(dev);
	if (p < 0)
	{
		RTL8372N_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		ret = -1;
		goto error;
	}

	cur_group = rtl8372n_br_group[p];
	tmp_group = br->ifindex;
	if (tmp_group != cur_group)
	{
		RTL8372N_LOG_ERR("not same bridge %d\r\n", cur_group);
		ret = -1;
		goto error;
	}

	if (rtl8372n_mc_router[p] == mcr)
	{
		RTL8372N_LOG_ERR("same mcr %d\r\n", mcr);
		ret = -1;
		goto error;
	}

	rtl8372n_mc_router[p] = mcr;
	if (rtl8372n_mcast_snoop)
	{
		_rtl8372n_mcast_mcr_refresh();
	}

error:
	RTL8372N_UNLOCK();
	return ret;
}

#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
static int rtl8372n_notify_vlan_change(struct net_device *dev, int add)
{
	int p;
	int ret = 0;
	if (!dev)
	{
		RTL8372N_LOG_ERR("br or dev null\r\n");
		return -1;
	}
	if (!_rtl8372n_is_switch_device(dev))
	{
		return 0;
	}
	RTL8372N_LOG_DEBUG("%s %d\r\n", dev->name, add);
	RTL8372N_LOCK();
	p = _rtl8372n_device_to_pidx(dev);
	if (p < 0)
	{
		RTL8372N_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		ret = -1;
		goto error;
	}

	if (add)
	{
		rtl8372n_hw_bridge_vlan_port[p]++;
	}
	else
	{
		rtl8372n_hw_bridge_vlan_port[p]--;
		if (rtl8372n_hw_bridge_vlan_port[p] < 0)
		{
			rtl8372n_hw_bridge_vlan_port[p] = 0;
			RTL8372N_LOG_ERR("vlan_port %d below zero\r\n", p);
		}
	}

	if (rtl8372n_hw_bridge)
	{
		_rtl8372n_lookup_refresh();
	}

	if (rtl8372n_mcast_snoop)
	{
		_rtl8372n_mcast_mcr_refresh();
	}
	rtk_l2_table_clear();
	if (rtl8372n_cpu_mac[0] != 0 ||
		rtl8372n_cpu_mac[1] != 0 ||
		rtl8372n_cpu_mac[2] != 0 ||
		rtl8372n_cpu_mac[3] != 0 ||
		rtl8372n_cpu_mac[4] != 0 ||
		rtl8372n_cpu_mac[5] != 0)
	{
		_rtl8372n_cpumac_refresh();
	}
error:
	RTL8372N_UNLOCK();
	return ret;
}
#endif /* RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT */

static int rtl8372n_ethtool_get_link_ksettings(struct net_device *dev,
														struct ethtool_link_ksettings *link_ksettings)
{
	rtk_api_ret_t ret;
	rtk_port_status_t status;
	int p;

	if(!dev || !link_ksettings)
	{
		RTL8372N_LOG_ERR("dev or link_ksettings null\r\n");
		return -1;
	}

	if(!_rtl8372n_is_switch_device(dev))
	{
		return 0;
	}

	RTL8372N_LOG_DEBUG("dev=%s\r\n", dev->name);

	p = _rtl8372n_device_to_pidx(dev);

	if(p < 0)
	{
		RTL8372N_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		return -EINVAL;
	}

	RTL8372N_LOCK();

	if((ret = rtk_port_macStatus_get(rtl8372n_utp_port[p], &status) != RT_ERR_OK))
	{
		RTL8372N_LOG_ERR("rtk_port_macStatus_get failed...ret=%d\n", ret);
		RTL8372N_UNLOCK();
		return -EINVAL;
	}

	RTL8372N_UNLOCK();

	if(status.link == 0)
	{
		link_ksettings->base.speed = 0;
		link_ksettings->base.duplex = DUPLEX_UNKNOWN;
		return 0;
	}

	switch(status.speed)
	{
		case PORT_SPEED_10M:
			link_ksettings->base.speed = SPEED_10;
			break;

		case PORT_SPEED_100M:
			link_ksettings->base.speed = SPEED_100;
			break;

		case PORT_SPEED_1000M:
			link_ksettings->base.speed = SPEED_1000;
			break;

		case PORT_SPEED_500M:
			link_ksettings->base.speed = 500; /* SPEED_500 not defined */
			break;

		case PORT_SPEED_10G:
			link_ksettings->base.speed = SPEED_10000;
			break;

		case PORT_SPEED_2500M:
			link_ksettings->base.speed = SPEED_2500;
			break;

		case PORT_SPEED_5G:
			link_ksettings->base.speed = SPEED_5000;
			break;

		default:
			RTL8372N_LOG_ERR("unknown ethernet speed (%d)\n", status.speed);
			link_ksettings->base.speed = 0;
			return -EINVAL;
	}

	switch(status.duplex)
	{
		case PORT_HALF_DUPLEX:
			link_ksettings->base.duplex = DUPLEX_HALF;
			break;

		case PORT_FULL_DUPLEX:
			link_ksettings->base.duplex = DUPLEX_FULL;
			break;

		default:
			link_ksettings->base.duplex = DUPLEX_UNKNOWN;
			return -EINVAL;
	}

	return 0;
}

static int rtl8372n_dev_get_stats(struct net_device *dev,
								  struct rtnl_link_stats64 *storage)
{
	int p;
	rtk_stat_counter_t cnt;
	if (!dev)
	{
		RTL8372N_LOG_ERR("dev or cmd null\r\n");
		return -1;
	}
	if (!_rtl8372n_is_switch_device(dev))
	{
		return 0;
	}

	RTL8372N_LOG_DEBUG("dev=%s\r\n", dev->name);
	p = _rtl8372n_device_to_pidx(dev);
	if (p < 0)
	{
		RTL8372N_LOG_DEBUG("wrong dev %s\r\n", dev->name);
		return -1;
	}
	memset(storage, 0x0, sizeof(*storage));
	RTL8372N_LOCK();

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], ifInUcastPkts_L, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->rx_packets += cnt;

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], ifInMulticastPkts_L, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->rx_packets += cnt;
	storage->multicast += cnt;

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], ifInBroadcastPkts_L, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->rx_packets += cnt;

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], ifInOctets_L, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->rx_bytes += cnt;

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], ifOutUcastPkts_L, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->tx_packets += cnt;

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], ifOutMulticastPkts_L, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->tx_packets += cnt;

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], ifOutBroadcastPkts_L, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->tx_packets += cnt;

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], ifOutOctets_L, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->tx_bytes += cnt;

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], RxErrorCnt, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->rx_errors += cnt;

	if (RT_ERR_OK != rtk_stat_port_get(rtl8372n_utp_port[p], TxErrorCnt, &cnt))
	{
		RTL8372N_LOG_ERR("rtk_stat_port_get fail\r\n");
		RTL8372N_UNLOCK();
		return -1;
	}
	storage->tx_errors += cnt;

	RTL8372N_UNLOCK();

	return 0;
}

static int rtl8372n_phy_link_change_notify(void)
{
	rtk_api_ret_t ret;
	rtk_port_status_t ability;
	int link_status = 0;
	int p;

	struct net_device *port_dev;

	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		RTL8372N_LOCK();
		if ((ret = rtk_port_macStatus_get(rtl8372n_utp_port[p], &ability) != RT_ERR_OK))
		{
			RTL8372N_LOG_ERR("rtk_port_macStatus_get failed...ret=%d\n", ret);
		}
		RTL8372N_UNLOCK();
		link_status = ability.link;

		if (link_status != rtl8372n_link_status[p])
		{
			RTL8372N_LOG_INFO("PORT%d (%s) link %s\r\n",
							  p, rtl8372n_dev_name[p], link_status ? "up" : "down");

			rtl8372n_link_status[p] = link_status;
		}

		port_dev = dev_get_by_name(&init_net, rtl8372n_dev_name[p]);

		if (port_dev)
		{
			if (_rtl8372n_is_switch_device(port_dev))
			{
				if (link_status)
				{
					netif_carrier_on(port_dev);
				}
				else
				{
					netif_carrier_off(port_dev);
				}
			}
			else
			{
				RTL8372N_LOG_ERR("not switch device: %s\n", port_dev->name);
			}
			dev_put(port_dev);
		}
	}
	return 0;
}

static struct timer_list rtl8372n_phy_link_timer;
static void rtl8372n_phy_link_work_cb(struct work_struct *work)
{
	/* Add by huangting, 20250315 */
	RTL8372N_LOCK();
	rtk_fw_reset_flow_tgr_tgx(SERDES_ID0);
	rtk_fw_reset_flow_tgr_tgx(SERDES_ID1);
	RTL8372N_UNLOCK();
	rtl8372n_phy_link_change_notify();
	mod_timer(&rtl8372n_phy_link_timer, jiffies + msecs_to_jiffies(RTL8372N_PHY_LINK_CHECK_INTERVAL));
}
DECLARE_WORK(_rtl8372n_phy_link_work, rtl8372n_phy_link_work_cb);

static void rtl8372n_phy_link_timer_cb(struct timer_list *tl)
{
	schedule_work(&_rtl8372n_phy_link_work);
}

static void rtl8372n_phy_link_timer_start(void)
{
	timer_setup(&rtl8372n_phy_link_timer, rtl8372n_phy_link_timer_cb, 0);
	rtl8372n_phy_link_timer.expires = jiffies + msecs_to_jiffies(RTL8372N_PHY_LINK_CHECK_INTERVAL);
	add_timer(&rtl8372n_phy_link_timer);
}

static ssize_t rtl8372n_switch_reg_write(struct file *file, const char __user *buffer,
										 size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 reg, val;
	int argc = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	argc = sscanf(val_string, "%x %x", &reg, &val);
	if (argc != 1 && argc != 2)
	{
		printk("usage: <action> reg [val]\n");
		return count;
	}
	RTL8372N_LOCK();
	if (argc == 2)
	{
		if (RT_ERR_OK != rtk_rtl8373_setAsicReg(reg, val))
		{
			RTL8372N_LOG_ERR("rtk_rtl8373_setAsicReg error\r\n");
		}
	}

	if (RT_ERR_OK != rtk_rtl8373_getAsicReg(reg, &val))
	{
		RTL8372N_LOG_ERR("rtk_rtl8373_getAsicReg error\r\n");
	}
	RTL8372N_UNLOCK();

	RTL8372N_LOG_INFO("reg[%08x]=%08x\r\n", reg, val);

	return count;
}

static struct file_operations rtl8372n_switch_reg_fops = {
	.owner = THIS_MODULE,
	.open = NULL,
	.read = NULL,
	.write = rtl8372n_switch_reg_write,
	.llseek = NULL,
	.release = NULL,
};

static ssize_t rtl8372n_phy_reg_write(struct file *file, const char __user *buffer,
									  size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 phy, dev, reg, val;
	int argc = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	argc = sscanf(val_string, "%d %x %x %x", &phy, &dev, &reg, &val);
	if (argc != 3 && argc != 4)
	{
		printk("usage: <action> phy dev reg [val]\n");
		return count;
	}
	RTL8372N_LOCK();
	if (argc == 4)
	{
		if (RT_ERR_OK != rtk_port_phyReg_set(1 << rtl8372n_utp_port[phy], dev, reg, val))
		{
			RTL8372N_LOG_ERR("rtk_port_phyReg_set error\r\n");
		}
	}

	if (RT_ERR_OK != rtk_port_phyReg_get(rtl8372n_utp_port[phy], dev, reg, &val))
	{
		RTL8372N_LOG_ERR("rtk_port_phyReg_get error\r\n");
	}
	RTL8372N_UNLOCK();

	RTL8372N_LOG_INFO("phy=%d reg[%08x]=%08x\r\n", phy, reg, val);

	return count;
}

static struct file_operations rtl8372n_phy_reg_fops = {
	.owner = THIS_MODULE,
	.open = NULL,
	.read = NULL,
	.write = rtl8372n_phy_reg_write,
	.llseek = NULL,
	.release = NULL,
};

static ssize_t rtl8372n_phy_reg_bits_write(struct file *file, const char __user *buffer,
									  size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 phy, dev, reg, bitsMask, val;
	int argc = 0;
	int ret = RT_ERR_OK;

	if (count > sizeof(val_string) - 1)
	{
		return -EINVAL;
	}
	if (copy_from_user(val_string, buffer, count))
	{
		return -EFAULT;
	}

	argc = sscanf(val_string, "%d %x %x %x %x", &phy, &dev, &reg, &bitsMask, &val);
	if (argc != 5)
	{
		printk("usage: <action> phy dev reg bitsMask val\n");
		return count;
	}

	RTL8372N_LOCK();
	ret = rtk_port_phyReg_setBits(1 << rtl8372n_utp_port[phy], dev, reg, bitsMask, val);
	RTL8372N_UNLOCK();

	if (RT_ERR_OK != ret)
	{
		RTL8372N_LOG_ERR("rtk_port_phyReg_setBits error\r\n");
		return -EFAULT;
	}

	return count;
}

static struct file_operations rtl8372n_phy_reg_bits_fops = {
	.owner = THIS_MODULE,
	.open = NULL,
	.read = NULL,
	.write = rtl8372n_phy_reg_bits_write,
	.llseek = NULL,
	.release = NULL,
};

static ssize_t rtl8372n_phy_reg_multi_bits_write(struct file *file, const char __user *buffer,
									  size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 phy, dev, reg, bitsMask, val, bits;
	int argc = 0;
	int ret = RT_ERR_OK;

	if (count > sizeof(val_string) - 1)
	{
		return -EINVAL;
	}
	if (copy_from_user(val_string, buffer, count))
	{
		return -EFAULT;
	}

	argc = sscanf(val_string, "%d %x %x %x %x", &phy, &dev, &reg, &bitsMask, &bits);
	if (argc != 5)
	{
		printk("usage: <action> phy dev reg bitsMask val\n");
		return count;
	}

	RTL8372N_LOCK();

	if ((ret = rtk_port_phyReg_get(rtl8372n_utp_port[phy], dev, reg, &val)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_port_phyReg_get error\r\n");
		goto err;
	}

	val &= ~bitsMask;
	val |= bits;

	if ((ret = rtk_port_phyReg_set(1 << rtl8372n_utp_port[phy], dev, reg, val)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_port_phyReg_set error\r\n");
		goto err;
	}

	RTL8372N_UNLOCK();
	return count;
err:
	RTL8372N_UNLOCK();
	return ret;
}

static struct file_operations rtl8372n_phy_reg_multi_bits_fops = {
	.owner = THIS_MODULE,
	.open = NULL,
	.read = NULL,
	.write = rtl8372n_phy_reg_multi_bits_write,
	.llseek = NULL,
	.release = NULL,
};


static ssize_t rtl8372n_sds_reg_write(struct file *file, const char __user *buffer,
									  size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 idx, page, reg, val;
	int argc = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	argc = sscanf(val_string, "%d %x %x %x", &idx, &page, &reg, &val);
	if (argc != 3 && argc != 4)
	{
		printk("usage: <action> sds page reg [val]\n");
		return count;
	}
	RTL8372N_LOCK();
	if (argc == 4)
	{
		if (RT_ERR_OK != rtk_rtl8373_sds_reg_write(idx, page, reg, val))
		{
			RTL8372N_LOG_ERR("rtk_rtl8373_sds_reg_write error\r\n");
		}
	}

	if (RT_ERR_OK != rtk_rtl8373_sds_reg_read(idx, page, reg, &val))
	{
		RTL8372N_LOG_ERR("rtk_rtl8373_sds_reg_read\r\n");
	}
	RTL8372N_UNLOCK();

	RTL8372N_LOG_INFO("sds=%d page[%08x] reg[%08x]=%08x\r\n", idx, page, reg, val);

	return count;
}

static struct file_operations rtl8372n_sds_reg_fops = {
	.owner = THIS_MODULE,
	.open = NULL,
	.read = NULL,
	.write = rtl8372n_sds_reg_write,
	.llseek = NULL,
	.release = NULL,
};

static int rtl8372n_phy_status_get(struct seq_file *s, void *unused)
{
	char *speed_str[] = {"10M", "100M", "1000M", "500M", "10G", "2.5G", "5G", "None"};
	char *duplex_str[] = {"HD", "FD", "None"};

	rtk_port_status_t pStatus[PORT_MAX_NUM] = {0};

	int p = 0;
	RTL8372N_LOCK();
	for (p = 0; p < PORT_MAX_NUM; p++)
	{
		if (RT_ERR_OK != rtk_port_macStatus_get(rtl8372n_utp_port[p], &pStatus[p]))
		{
			RTL8372N_LOG_ERR("rtk_port_phyStatus_get failed...\n");
			goto error;
		}
	}
	for (p = 0; p < PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%u ", pStatus[p].link);
	}
	seq_printf(s, "\n");

	for (p = 0; p < PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%s ", speed_str[pStatus[p].speed]);
	}
	seq_printf(s, "\n");
	for (p = 0; p < PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%s ", duplex_str[pStatus[p].duplex]);
	}
	seq_printf(s, "\n");
error:
	RTL8372N_UNLOCK();
	return 0;
}

static int rtl8372n_phy_status_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_phy_status_get, file->f_path.dentry->d_iname);
}

static struct file_operations rtl8372n_phy_status_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_phy_status_open,
	.read = seq_read,
	.write = NULL,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_mii_type_get(struct seq_file *s, void *unused)
{
	seq_printf(s, "%d\n", rtl8372n_mii_type);
	return 0;
}
static int rtl8372n_mii_type_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_mii_type_get, file->f_path.dentry->d_iname);
}
static ssize_t rtl8372n_mii_type_write(struct file *file, const char __user *buffer,
									   size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 val;
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}
	RTL8372N_LOCK();

	if (rtl8372n_mii_type == val)
	{
		RTL8372N_LOG_ERR("same mii_type %d\n", val);
		goto error;
	}
	rtl8372n_mii_type = val;
	_rtl8372n_mii_type_refresh();
error:
	RTL8372N_UNLOCK();
	return count;
}

static struct file_operations rtl8372n_mii_type_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_mii_type_open,
	.read = seq_read,
	.write = rtl8372n_mii_type_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_ucast_tbl_get(struct seq_file *s, void *unused)
{
	rtk_uint32 address = 0;
	rtk_l2_ucastAddr_t ucastAddr;
	RTL8372N_LOCK();

	while (1)
	{
		if (RT_ERR_OK != rtk_l2_addr_next_get(READMETHOD_NEXT_L2UC, UTP_PORT0, &address, &ucastAddr))
		{
			break;
		}
		seq_printf(s,
				   "%04d) mac=%02x-%02x-%02x-%02x-%02x-%02x, ivl=%d, vid_fid=%d, static=%d, port=%d, age=%d,\n",
				   ucastAddr.address,
				   ucastAddr.mac.octet[0],
				   ucastAddr.mac.octet[1],
				   ucastAddr.mac.octet[2],
				   ucastAddr.mac.octet[3],
				   ucastAddr.mac.octet[4],
				   ucastAddr.mac.octet[5],
				   ucastAddr.ivl,
				   ucastAddr.vid_fid,
				   ucastAddr.is_static,
				   ucastAddr.port,
				   ucastAddr.age);
		address++;
	}
	RTL8372N_UNLOCK();
	return 0;
}

static int rtl8372n_ucast_tbl_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_ucast_tbl_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8372n_ucast_tbl_write(struct file *file, const char __user *buffer,
										size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 val;
	rtk_uint32 address = 0;
	rtk_l2_ucastAddr_t ucastAddr;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action> -1: delete all entry, (0~...): delete specific entry\n");
		return count;
	}
	RTL8372N_LOCK();
	if (val < 0)
	{
		while (1)
		{
			address = 0;
			if (RT_ERR_OK != rtk_l2_addr_next_get(READMETHOD_NEXT_L2UC, UTP_PORT0, &address, &ucastAddr))
			{
				RTL8372N_LOG_ERR("rtk_l2_addr_next_get fail\r\n");
				break;
			}
			rtk_l2_addr_del(&ucastAddr.mac, &ucastAddr);
		}
		printk("rtk_l2_addr_del delete all done\r\n");
	}
	else
	{
		address = val;
		if (RT_ERR_OK != rtk_l2_addr_next_get(READMETHOD_NEXT_L2UC, UTP_PORT0, &address, &ucastAddr))
		{
			RTL8372N_LOG_ERR("rtk_l2_addr_next_get fail\r\n");
			RTL8372N_UNLOCK();
			return count;
		}
		rtk_l2_addr_del(&ucastAddr.mac, &ucastAddr);
		printk("rtk_l2_addr_del %02x-%02x-%02x-%02x-%02x-%02x done\r\n",
			   ucastAddr.mac.octet[0], ucastAddr.mac.octet[1], ucastAddr.mac.octet[2],
			   ucastAddr.mac.octet[3], ucastAddr.mac.octet[4], ucastAddr.mac.octet[5]);
	}
	RTL8372N_UNLOCK();

	return count;
}

static rtk_uint8 charToNumber(const char ch)
{
	if (ch >= '0' && ch <= '9')
	{
		return ch - '0';
	}
	else if (ch >= 'A' && ch <= 'F')
	{
		return ch - 'A' + 10;
	}
	else if (ch >= 'a' && ch <= 'f')
	{
		return ch - 'a' + 10;
	}
	else
	{
		/* Nothing. */
	}
	return 0;
}

static rtk_uint8 strToMac(const char *pSzMac, rtk_uint8 *pMac)
{
	const char *pTemp = NULL;
	int index = 0;
	if ((NULL == pSzMac) || (NULL == pMac))
	{
		return 0;
	}
	pTemp = pSzMac;

	for (index = 0; index < 6; index++)
	{
		pMac[index] = charToNumber(*pTemp++) * 16;
		pMac[index] += charToNumber(*pTemp++);
		pTemp++;
	}

	return 0;
}

static rtk_uint8 l_macReadfromUcastTbl[ETHER_ADDR_LEN];
#ifndef MAC_STRING_LEN
#define MAC_STRING_LEN (18)
#endif
#define USUAL_LAN_PORT_EFID_MAX (PORT_EFID_MAX)
#define PORT_EFID_MAX (0x7)

static ssize_t rtl8372n_ucast_tbl_single_write(struct file *pFile, const char __user *pBuffer,
											   size_t size, loff_t *pPos)
{
	char buf[MAC_STRING_LEN] = {0};

	if ((NULL == pFile) || (NULL == pBuffer) || (NULL == pPos))
	{
		printk("NUll\n");
		return -EFAULT;
	}
	if (size != MAC_STRING_LEN)
	{
		printk("Error count:%zd\n", size);
		return -EFAULT;
	}

	if (copy_from_user(buf, pBuffer, size))
	{
		printk("copy_from_user failed, count:%zd\n", size);
		return -EFAULT;
	}
	memset(l_macReadfromUcastTbl, 0, sizeof(l_macReadfromUcastTbl));
	strToMac(buf, l_macReadfromUcastTbl);
	return size;
}

static int rtl8372n_ucast_tbl_single_get(struct seq_file *s, void *unused)
{
	int ret = -1;
	rtk_mac_t mac;
	rtk_l2_ucastAddr_t l2_data;
	int i = 0;
	unsigned char find = FALSE;

	memcpy(&mac.octet, l_macReadfromUcastTbl, sizeof(mac.octet));

	for (i = 0; i < USUAL_LAN_PORT_EFID_MAX; i++)
	{
		memset(&l2_data, 0x0, sizeof(l2_data));

		l2_data.ivl = 1;
		l2_data.vid_fid = i;

		RTL8372N_LOCK();
		ret = rtk_l2_addr_get(&mac, &l2_data);
		RTL8372N_UNLOCK();
		if (RT_ERR_OK == ret)
		{
			find = TRUE;
			seq_printf(s, "%04d) mac=%02x-%02x-%02x-%02x-%02x-%02x, ivl=%d, fid=%d, static=%d, port=%d, age=%d\n",
					   l2_data.address, l2_data.mac.octet[0], l2_data.mac.octet[1], l2_data.mac.octet[2],
					   l2_data.mac.octet[3], l2_data.mac.octet[4], l2_data.mac.octet[5], l2_data.ivl,
					   l2_data.vid_fid, l2_data.is_static, l2_data.port, l2_data.age);
			/* We still need to use other efid to find other entrys,
			 maybe a client has many entrys which using different efid and port. */
		}
	}

	if (FALSE == find)
	{
		printk("Get l2 fail, ret[%d], mac[%02x-%02x-%02x-%02x-%02x-%02x]\n", ret, mac.octet[0],
			   mac.octet[1], mac.octet[2], mac.octet[3], mac.octet[4], mac.octet[5]);
		return -EFAULT;
	}
	return 0;
}

static int rtl8372n_ucast_tbl_single_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_ucast_tbl_single_get, file->f_path.dentry->d_iname);
}

static struct file_operations rtl8372n_ucast_tbl_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_ucast_tbl_open,
	.read = seq_read,
	.write = rtl8372n_ucast_tbl_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations rtl8372n_ucast_tbl_single_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_ucast_tbl_single_open,
	.read = seq_read,
	.write = rtl8372n_ucast_tbl_single_write,
};

static int rtl8372n_mcast_tbl_get(struct seq_file *s, void *unused)
{
	rtk_uint32 address = 0;
	rtk_l2_ipMcastAddr_t ipMcastAddr;
#if 0
	rtk_l2_ipVidMcastAddr_t ipvidMcastAddr;
#endif
	rtk_l2_mcastAddr_t l2McastAddr;
	seq_printf(s, "================IPv4 Mcast Address===========\r\n");
	RTL8372N_LOCK();
	while (1)
	{
		if (RT_ERR_OK != rtk_l2_ipMcastAddr_next_get(&address, &ipMcastAddr))
		{
			break;
		}
		ipMcastAddr.dip |= 0xe0000000;

		seq_printf(s, "dip=0x%08x, sip=0x%08x, asic=%d index=%d ports=0x%08x\n",
				   ipMcastAddr.dip, ipMcastAddr.sip, ipMcastAddr.igmp_asic, ipMcastAddr.igmp_index, ipMcastAddr.portmask.bits[0]);

		address++;
	}

	seq_printf(s, "================IPv6 Mcast Address===========\r\n");

	address = 0;
	while (1)
	{
		if (RT_ERR_OK != rtk_l2_mcastAddr_next_get(&address, &l2McastAddr))
		{
			break;
		}

		seq_printf(s, "mac=%02x-%02x-%02x-%02x-%02x-%02x vid_fid=%d vid=%04x asic=%d index=%d ports=0x%08x\n",
				   l2McastAddr.mac.octet[0], l2McastAddr.mac.octet[1], l2McastAddr.mac.octet[2],
				   l2McastAddr.mac.octet[3], l2McastAddr.mac.octet[4], l2McastAddr.mac.octet[5],
				   l2McastAddr.vid_fid, l2McastAddr.ivl, l2McastAddr.igmp_asic, l2McastAddr.igmp_index,
				   l2McastAddr.portmask.bits[0]);

		address++;
	}
	RTL8372N_UNLOCK();

	return 0;
}

static int rtl8372n_mcast_tbl_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_mcast_tbl_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8372n_mcast_tbl_write(struct file *file, const char __user *buffer,
										size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 val;
	rtk_uint32 address = 0;
	rtk_l2_ipMcastAddr_t ipMcastAddr;
	rtk_l2_mcastAddr_t l2McastAddr;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}

	memset(&ipMcastAddr, 0x0, sizeof(ipMcastAddr));
	RTL8372N_LOCK();

	while (1)
	{
		address = 0;
		if (RT_ERR_OK != rtk_l2_ipMcastAddr_next_get(&address, &ipMcastAddr))
		{
			break;
		}
		ipMcastAddr.dip |= 0xe0000000;

		if (RT_ERR_OK != rtk_l2_ipMcastAddr_del(&ipMcastAddr))
		{
			RTL8372N_LOG_ERR("rtk_l2_ipMcastAddr_del fail\r\n");
		}
	}
	while (1)
	{
		address = 0;
		if (RT_ERR_OK != rtk_l2_mcastAddr_next_get(&address, &l2McastAddr))
		{
			break;
		}

		if (RT_ERR_OK != rtk_l2_mcastAddr_del(&l2McastAddr))
		{
			RTL8372N_LOG_ERR("rtk_l2_mcastAddr_del fail\r\n");
		}
	}
	RTL8372N_UNLOCK();
	return count;
}

static struct file_operations rtl8372n_mcast_tbl_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_mcast_tbl_open,
	.read = seq_read,
	.write = rtl8372n_mcast_tbl_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_mcast_snoop_get(struct seq_file *s, void *unused)
{
	int p;

	seq_printf(s, "mcast_snoop = %d\n", rtl8372n_mcast_snoop);
	seq_printf(s, "============================\n");

	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		seq_printf(s, "port=%d dev=%s mcr=%d\n",
				   p, rtl8372n_dev_name[p], rtl8372n_mc_router[p]);
	}

	return 0;
}

static int rtl8372n_mcast_snoop_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_mcast_snoop_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8372n_mcast_snoop_write(struct file *file, const char __user *buffer,
										  size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 val, pidx;
	int argc = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	argc = sscanf(val_string, "%d %d", &val, &pidx);
	if (argc != 1 && argc != 2)
	{
		printk("usage: <action> enable [ port ]\n");
		return count;
	}
	RTL8372N_LOCK();
	if (argc == 1)
	{
		if (rtl8372n_mcast_snoop == val)
		{
			RTL8372N_LOG_ERR("same mcast_snoop %d\r\n", val);
			goto error;
		}
		rtl8372n_mcast_snoop = val;
		_rtl8372n_mcast_refresh();
	}
	else if (argc == 2)
	{
		if (pidx < 0 || pidx >= PORT_MAX_NUM)
		{
			RTL8372N_LOG_ERR("pidx out of range %d\n", pidx);
			goto error;
		}
		if (rtl8372n_br_group[pidx] >= 0)
		{
			RTL8372N_LOG_ERR("do not change bridge port's mc_router\n");
			goto error;
		}

		if (rtl8372n_mc_router[pidx] == val)
		{
			RTL8372N_LOG_ERR("same mc_router %d\n", val);
			goto error;
		}
		rtl8372n_mc_router[pidx] = val;
		if (rtl8372n_mcast_snoop)
		{
			_rtl8372n_mcast_mcr_refresh();
		}
	}
	else
	{
		RTL8372N_LOG_ERR("wrong argc %d\n", argc);
		goto error;
	}
error:
	RTL8372N_UNLOCK();

	return count;
}

static struct file_operations rtl8372n_mcast_snoop_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_mcast_snoop_open,
	.read = seq_read,
	.write = rtl8372n_mcast_snoop_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_port_mirror_get(struct seq_file *s, void *unused)
{
	rtk_enable_t enable;

#if 0
	rtk_port_mir_set_t mirset;
	rtk_enable_t tx_enable;
	rtk_enable_t rx_enable;
	rtk_enable_t acl_enable;

	rtk_mirror_keep_t keep_mode;
	rtk_uint32 rate = 0;
	rtk_uint32 count = 0;

	rtk_rspan_tag_t rspan_tag;
	rtk_portmask_t pmask;
#endif

	RTL8372N_LOCK();

	if (RT_ERR_OK != rtk_mirror_setStatus_get(&enable))
	{
		RTL8372N_LOG_ERR("rtk_mirror_setStatus_get fail\r\n");
	}

	seq_printf(s, "port_mirror = %d\n", enable);

#if 0
	if (RT_ERR_OK != rtk_mirror_portBased_get(&mirset))
	{
		RTL8372N_LOG_ERR("rtk_mirror_portBased_set fail\r\n");
	}

	seq_printf(s, "mirset.mtp_port = %u\n", mirset.mtp_port);
	seq_printf(s, "mirset.tx_pmsk.bit[0] = 0x%x\n", mirset.tx_pmsk.bits[0]);
	seq_printf(s, "mirset.rx_pmsk.bit[0] = 0x%x\n", mirset.rx_pmsk.bits[0]);
	seq_printf(s, "mirset.rx_tx_sel = %d\n", mirset.rx_tx_sel);

	if (RT_ERR_OK != rtk_mirror_portIso_get(&enable))
	{
		RTL8372N_LOG_ERR("rtk_mirror_portIso_get fail\r\n");
	}

	seq_printf(s, "mirror port iso = %d\n", enable);

	if (RT_ERR_OK != rtk_mirror_vlanLeaky_get(&tx_enable, &rx_enable))
	{
		RTL8372N_LOG_ERR("rtk_mirror_vlanLeaky_get fail\r\n");
	}

	seq_printf(s, "mirror vlan leaky tx = %d, rx = %d\n", tx_enable, rx_enable);

	if (RT_ERR_OK != rtk_mirror_isolationLeaky_get(&tx_enable, &rx_enable))
	{
		RTL8372N_LOG_ERR("rtk_mirror_isolationLeaky_get fail\r\n");
	}

	seq_printf(s, "mirror isolation leaky tx = %d, rx = %d\n", tx_enable, rx_enable);

	if (RT_ERR_OK != rtk_mirror_keep_get(&keep_mode))
	{
		RTL8372N_LOG_ERR("rtk_mirror_keep_get fail\r\n");
	}

	seq_printf(s, "mirror keep = %d,\n", keep_mode);

	if (RT_ERR_OK != rtk_mirror_override_get(&rx_enable, &tx_enable, &acl_enable))
	{
		RTL8372N_LOG_ERR("rtk_mirror_override_get fail\r\n");
	}

	seq_printf(s, "mirror override rx = %d, tx = %d, acl = %d\n", rx_enable, tx_enable, acl_enable);

	if (RT_ERR_OK != rtk_mirror_sampleRate_get(&rate))
	{
		RTL8372N_LOG_ERR("rtk_mirror_sampleRate_get fail\r\n");
	}

	seq_printf(s, "mirror sample rate = %u,\n", rate);

	if (RT_ERR_OK != rtk_mirror_pktCnt_get(&count))
	{
		RTL8372N_LOG_ERR("rtk_mirror_pktCnt_get fail\r\n");
	}

	seq_printf(s, "mirror pkt cnt = %u,\n", count);

	if (RT_ERR_OK != rtk_mirror_samplePktCnt_get(&count))
	{
		RTL8372N_LOG_ERR("rtk_mirror_samplePktCnt_get fail\r\n");
	}

	seq_printf(s, "mirror sample pkt cnt = %u,\n", count);

	if (RT_ERR_OK != rtk_rspan_rxTagEnStatus_get(&enable))
	{
		RTL8372N_LOG_ERR("rtk_rspan_rxTagEnStatus_get fail\r\n");
	}

	seq_printf(s, "rspan rx tag en status = %d\n", enable);

	if (RT_ERR_OK != rtk_rspan_tagCtxt_get(&rspan_tag))
	{
		RTL8372N_LOG_ERR("rtk_rspan_tagCtxt_get fail\r\n");
	}

	seq_printf(s, "rspan_tag tpid = 0x%x, cfi = 0x%x, vid = %u, pri = %u\n", rspan_tag.tpid, rspan_tag.cfi, rspan_tag.vid, rspan_tag.pri);

	if (RT_ERR_OK != rtk_rspan_tagAdd_get(&pmask))
	{
		RTL8372N_LOG_ERR("rtk_rspan_tagAdd_get fail\r\n");
	}

	seq_printf(s, "rspan tag add = %0x%x\n", pmask.bits[0]);

	if (RT_ERR_OK != rtk_rspan_tagRemove_get(&enable))
	{
		RTL8372N_LOG_ERR("rtk_rspan_tagRemove_get fail\r\n");
	}

	seq_printf(s, "rspan tag remove = %d\n", enable);
#endif

	RTL8372N_UNLOCK();

	return 0;
}

static int rtl8372n_port_mirror_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_port_mirror_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8372n_port_mirror_write(struct file *file, const char __user *buffer,
										  size_t count, loff_t *pPos)
{
	char val_string[128];
	int argc;
	rtk_port_mir_set_t mirset;
	int val, ming_port, mred_port, isolation;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	argc = sscanf(val_string, "%d %d %d %d", &val, &ming_port, &mred_port, &isolation);

	if (argc != 1 && argc != 4)
	{
		printk("usage: <action> mirror_en [mirroring_port] [mirrored_port] [isolation]\n");
		return count;
	}

	memset(&mirset, 0, sizeof(mirset));

	RTK_PORTMASK_CLEAR(mirset.rx_pmsk);
	RTK_PORTMASK_CLEAR(mirset.tx_pmsk);

	if (val)
	{
		if (argc < 4)
		{
			RTL8372N_LOG_ERR("missing mirroring_port mirrored_port isolation\r\n");
			return count;
		}

		if (ming_port == mred_port)
		{
			RTL8372N_LOG_ERR("mirroring_port same with mirrored_port\r\n");
			return count;
		}

		if (ming_port < PORT_IDX0 || ming_port >= PORT_MAX_NUM)
		{
			RTL8372N_LOG_ERR("mirroring_port out of range\r\n");
			return count;
		}

		if (mred_port < PORT_IDX0 || mred_port >= PORT_MAX_NUM)
		{
			RTL8372N_LOG_INFO("mirror cpu port -> utp port%d\r\n", ming_port);

			RTK_PORTMASK_PORT_SET(mirset.tx_pmsk, rtl8372n_cpu_port);
			RTK_PORTMASK_PORT_SET(mirset.rx_pmsk, rtl8372n_cpu_port);
		}
		else
		{
			RTL8372N_LOG_INFO("mirror utp port%d -> utp port%d\r\n", mred_port, ming_port);

			RTK_PORTMASK_PORT_SET(mirset.tx_pmsk, rtl8372n_utp_port[mred_port]);
			RTK_PORTMASK_PORT_SET(mirset.rx_pmsk, rtl8372n_utp_port[mred_port]);
		}

		mirset.mtp_port = rtl8372n_utp_port[ming_port];

		RTL8372N_LOCK();
		if (RT_ERR_OK != rtk_mirror_portBased_set(&mirset))
		{
			RTL8372N_LOG_ERR("rtk_mirror_portBased_set fail\r\n");
		}
		if (RT_ERR_OK != rtk_mirror_portIso_set(isolation ? ENABLED : DISABLED))
		{
			RTL8372N_LOG_ERR("rtk_mirror_portIso_set fail\r\n");
		}
		if (RT_ERR_OK != rtk_mirror_isolationLeaky_set(ENABLED, ENABLED))
		{
			RTL8372N_LOG_ERR("rtk_mirror_isolationLeaky_set fail\r\n");
		}
		if (RT_ERR_OK != rtk_mirror_set_en(ENABLED))
		{
			RTL8372N_LOG_ERR("rtk_mirror_set_en fail\r\n");
		}
		RTL8372N_UNLOCK();
	}
	else
	{
		mirset.mtp_port = 0;

		RTL8372N_LOCK();
		if (RT_ERR_OK != rtk_mirror_set_en(DISABLED))
		{
			RTL8372N_LOG_ERR("rtk_mirror_set_en fail\r\n");
		}
		if (RT_ERR_OK != rtk_mirror_portBased_set(&mirset))
		{
			RTL8372N_LOG_ERR("rtk_mirror_portBased_set fail\r\n");
		}
		if (RT_ERR_OK != rtk_mirror_portIso_set(DISABLED))
		{
			RTL8372N_LOG_ERR("rtk_mirror_portIso_set fail\r\n");
		}
		if (RT_ERR_OK != rtk_mirror_isolationLeaky_set(ENABLED, DISABLED))
		{
			RTL8372N_LOG_ERR("rtk_mirror_isolationLeaky_set fail\r\n");
		}
		RTL8372N_UNLOCK();
	}

	return count;
}

static struct file_operations rtl8372n_port_mirror_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_port_mirror_open,
	.read = seq_read,
	.write = rtl8372n_port_mirror_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_mib_cnt_get(struct seq_file *s, void *unused)
{
	const char *port_strs[] = {
		"UTP3",
		"UTP4",
		"UTP5",
		"UTP6",
		"UTP7",
		"UTP8",
	};
	const rtk_port_t ports[] = {
		UTP_PORT3,
		UTP_PORT4,
		UTP_PORT5,
		UTP_PORT6,
		UTP_PORT7,
		UTP_PORT8,
	};

	static const char *ptype_strs[] = {
		"InUcastPkts",
		"InMulticastPkts",
		"InBroadcastPkts",
		"OutUcastPkts",
		"OutMulticastPkts",
		"OutBroadcastPkts",
		"InOctets",
		"OutOctets",



		"ifOutDiscards",
		"dot1dTpPortInDiscards",
		"dot3StatsSingleCollisionFrames",
		"dot3StatMultipleCollisionFrames",
		"dot3sDeferredTransmissions",
		"dot3StatsLateCollisions",
		"dot3StatsExcessiveCollisions",
		"dot3StatsSymbolErrors",
		"dot3ControlInUnknownOpcodes",
		"dot3InPauseFrames",
		"dot3OutPauseFrames",
		"etherStatsDropEvents",
		"tx_etherBroadcastPkts",
		"tx_etherMulticastPkts",
		"tx_etherCRCAlignErrors",
		"rx_etherCRCAlignErrors",
		"tx_etherUndersizePkts",
		"rx_etherUndersizePkts",
		"tx_etherOversizePkts",
		"rx_etherOversizePkts",
		"tx_etherFragments",
		"rx_etherFragments",
		"tx_etherJabbers",
		"rx_etherJabbers",
		"tx_etherCollisions",
		"tx_etherPkts64Octets",
		"rx_etherPkts64Octets",
		"tx_etherPkts65to127Octets",
		"rx_etherPkts65to127Octets",
		"tx_etherPkts128to255Octets",
		"rx_etherPkts128to255Octets",
		"tx_etherPkts256to511Octets",
		"rx_etherPkts256to511Octets",
		"tx_etherPkts512to1023Octets",
		"rx_etherPkts512to1023Octets",
		"tx_etherPkts1024to1518Octets",
		"rx_etherPkts1024to1518Octets",

		"rx_etherUndersizedropPkts",
		"tx_etherPkts1519toMaxOctets",
		"rx_etherPkts1519toMaxOctets",
		"tx_etherPktsOverMaxOctets",
		"rx_etherPktsOverMaxOctets",
		"tx_etherPktsFlexibleOctetsSET1",
		"rx_etherPktsFlexibleOctetsSET1",
		"tx_etherPktsFlexibleOctetsCRCSET1",
		"rx_etherPktsFlexibleOctetsCRCSET1",
		"tx_etherPktsFlexibleOctetsSET0",
		"rx_etherPktsFlexibleOctetsSET0",
		"tx_etherPktsFlexibleOctetsCRSET0C",
		"rx_etherPktsFlexibleOctetsCRSET0C",
		"lengthFieldError",
		"falseCarrieimes",
		"underSizeOctets",
		"framingErrors",

		"rxMacDiscards",
		"rxMacIPGShortDropRT",

		"dot1dTpLearnedEntryDiscards",
		"egrQueue7DropPktRT",
		"egrQueue6DropPktRT",
		"egrQueue5DropPktRT",
		"egrQueue4DropPktRT",
		"egrQueue3DropPktRT",
		"egrQueue2DropPktRT",
		"egrQueue1DropPktRT",
		"egrQueue0DropPktRT",
		"egrQueue7OutPktRT",
		"egrQueue6OutPktRT",
		"egrQueue5OutPktRT",
		"egrQueue4OutPktRT",
		"egrQueue3OutPktRT",
		"egrQueue2OutPktRT",
		"egrQueue1OutPktRT",
		"egrQueue0OutPktRT",
		"TxGoodCnt",
		"RxGoodCnt",
		"RxErrorCnt",
		"TxErrorCnt",
		"TxGoodCnt_phy",
		"RxGoodCnt_phy",
		"RxErrorCnt_phy",
		"TxErrorCnt_phy"
	};

	static const rtk_stat_port_type_t ptypes[] = {
		ifInUcastPkts_L,
		ifInMulticastPkts_L,
		ifInBroadcastPkts_L,
		ifOutUcastPkts_L,
		ifOutMulticastPkts_L,
		ifOutBroadcastPkts_L,
		ifInOctets_L,
		ifOutOctets_L,



		ifOutDiscards,
		dot1dTpPortInDiscards,
		dot3StatsSingleCollisionFrames,
		dot3StatMultipleCollisionFrames,
		dot3sDeferredTransmissions,
		dot3StatsLateCollisions,
		dot3StatsExcessiveCollisions,
		dot3StatsSymbolErrors,
		dot3ControlInUnknownOpcodes,
		dot3InPauseFrames,
		dot3OutPauseFrames,
		etherStatsDropEvents,
		tx_etherStatsBroadcastPkts,
		tx_etherStatsMulticastPkts,
		tx_etherStatsCRCAlignErrors,
		rx_etherStatsCRCAlignErrors,
		tx_etherStatsUndersizePkts,
		rx_etherStatsUndersizePkts,
		tx_etherStatsOversizePkts,
		rx_etherStatsOversizePkts,
		tx_etherStatsFragments,
		rx_etherStatsFragments,
		tx_etherStatsJabbers,
		rx_etherStatsJabbers,
		tx_etherStatsCollisions,
		tx_etherStatsPkts64Octets,
		rx_etherStatsPkts64Octets,
		tx_etherStatsPkts65to127Octets,
		rx_etherStatsPkts65to127Octets,
		tx_etherStatsPkts128to255Octets,
		rx_etherStatsPkts128to255Octets,
		tx_etherStatsPkts256to511Octets,
		rx_etherStatsPkts256to511Octets,
		tx_etherStatsPkts512to1023Octets,
		rx_etherStatsPkts512to1023Octets,
		tx_etherStatsPkts1024to1518Octets,
		rx_etherStatsPkts1024to1518Octets,

		rx_etherStatsUndersizedropPkts,
		tx_etherStatsPkts1519toMaxOctets,
		rx_etherStatsPkts1519toMaxOctets,
		tx_etherStatsPktsOverMaxOctets,
		rx_etherStatsPktsOverMaxOctets,
		tx_etherStatsPktsFlexibleOctetsSET1,
		rx_etherStatsPktsFlexibleOctetsSET1,
		tx_etherStatsPktsFlexibleOctetsCRCSET1,
		rx_etherStatsPktsFlexibleOctetsCRCSET1,
		tx_etherStatsPktsFlexibleOctetsSET0,
		rx_etherStatsPktsFlexibleOctetsSET0,
		tx_etherStatsPktsFlexibleOctetsCRSET0C,
		rx_etherStatsPktsFlexibleOctetsCRSET0C,
		lengthFieldError,
		falseCarrieimes,
		underSizeOctets,
		framingErrors,

		rxMacDiscards,
		rxMacIPGShortDropRT,

		dot1dTpLearnedEntryDiscards,
		egrQueue7DropPktRT,
		egrQueue6DropPktRT,
		egrQueue5DropPktRT,
		egrQueue4DropPktRT,
		egrQueue3DropPktRT,
		egrQueue2DropPktRT,
		egrQueue1DropPktRT,
		egrQueue0DropPktRT,
		egrQueue7OutPktRT,
		egrQueue6OutPktRT,
		egrQueue5OutPktRT,
		egrQueue4OutPktRT,
		egrQueue3OutPktRT,
		egrQueue2OutPktRT,
		egrQueue1OutPktRT,
		egrQueue0OutPktRT,
		TxGoodCnt_L,
		RxGoodCnt_L,
		RxErrorCnt,
		TxErrorCnt,
		TxGoodCnt_phy_L,
		RxGoodCnt_phy_L,
		RxErrorCnt_phy,
		TxErrorCnt_phy
	};

	int p = 0;
	int t = 0;
	int ret = -1;

	rtk_stat_counter_t cnt;
	// RTL8372N_LOCK();
	seq_printf(s, "%-40s", "MIB Counter");

	for (p = 0; p < PORT_NUM; p++)
	{
		seq_printf(s, "%-24s", port_strs[p]);
	}
	seq_printf(s, "\r\n");

	for (t = 0; t < (sizeof(ptypes) / sizeof(ptypes[0])); t++)
	{
		if (rtl8372n_mib_level == 0 && t >= 8)
		{
			break;
		}
		seq_printf(s, "%-40s", ptype_strs[t]);
		for (p = 0; p < PORT_NUM; p++)
		{
			RTL8372N_LOCK();
			ret = rtk_stat_port_get(ports[p], ptypes[t], &cnt);
			RTL8372N_UNLOCK();
			if (RT_ERR_OK != ret)
			{
				RTL8372N_LOG_ERR("rtk_stat_port_get fail %s %s\r\n", port_strs[p], ptype_strs[t]);
				goto next;
			}
			seq_printf(s, "%-24llu", cnt);
		}
		seq_printf(s, "\r\n");
	}
next:
#if 0
	/* reset counter */
	for(p=0; p<PORT_NUM; p++)
	{
		RTL8372N_LOCK();
		ret = rtk_stat_port_reset(ports[p]);
		RTL8372N_UNLOCK();
		if(RT_ERR_OK != ret)
		{
			RTL8372N_LOG_ERR("rtk_stat_port_reset fail %s\r\n", port_strs[p]);
		}
	}
#endif
	// RTL8372N_UNLOCK();
	return 0;
}

static int rtl8372n_mib_cnt_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_mib_cnt_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8372n_mib_cnt_write(struct file *file, const char __user *buffer,
									  size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 val;
	rtk_port_t p;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}

	RTL8372N_LOCK();

	if (2 == val || 0 == val)
	{
		rtl8372n_mib_level = val;
		RTL8372N_UNLOCK();
		return count;
	}

	/* reset counter */
	RTK_SCAN_ALL_LOG_PORT(p)
	{
		if (RT_ERR_OK != rtk_stat_port_reset(p))
		{
			RTL8372N_LOG_ERR("rtk_stat_port_reset fail\r\n");
			RTL8372N_UNLOCK();
			return 0;
		}
	}
	RTL8372N_UNLOCK();

	return count;
}

static struct file_operations rtl8372n_mib_cnt_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_mib_cnt_open,
	.read = seq_read,
	.write = rtl8372n_mib_cnt_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_hw_bridge_get(struct seq_file *s, void *unused)
{
	rtk_uint32 permitPort;
	rtk_svlan_memberCfg_t svlanCfg;
	rtk_uint32 vid;
	rtk_uint32 fid;
	rtk_uint32 enabled;
	int p;
	int ret = 0;
	seq_printf(s, "hw_bridge=%d\n", rtl8372n_hw_bridge);
	seq_printf(s, "hw_bridge_port=");
	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%d ", rtl8372n_hw_bridge_port[p]);
	}
	seq_printf(s, "\n");
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
	seq_printf(s, "hw_bridge_vlan_port=");
	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%d ", rtl8372n_hw_bridge_vlan_port[p]);
	}
	seq_printf(s, "\n");
#endif /**/
	seq_printf(s, "============================\n");

	seq_printf(s, "UTP\tpermit\t\tpfid\tvfid\tefid\n");

	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		seq_printf(s, "UTP%d\t", rtl8372n_utp_port[p]);

		/* get iso of the port */
		memset(&permitPort, 0x0, sizeof(permitPort));
		RTL8372N_LOCK();

		if ((ret = rtk_port_isolation_get(rtl8372n_utp_port[p], &permitPort)) != RT_ERR_OK)
		{
			RTL8372N_LOG_ERR("rtk_port_isolation_set fail...ret=%d\n", ret);
		}
		seq_printf(s, "0x%08x\t", permitPort);

		/* get fid of the port */
		if ((ret = rtk_vlan_portFid_get(rtl8372n_utp_port[p], &enabled, &fid)) != RT_ERR_OK)
		{
			RTL8372N_LOG_ERR("rtk_port_efid_get fail...ret=%d\n", ret);
			RTL8372N_UNLOCK();
			return ret;
		}
		seq_printf(s, "%0d\t", fid);

		/* get vlan of the port */
		vid = (p + RTL8372N_VID_RESERVED_NUM);
		memset(&svlanCfg, 0x0, sizeof(svlanCfg));
		if ((ret = dal_rtl8373_svlanMbrPortEntry_get(vid, &svlanCfg)) != RT_ERR_OK)
		{
			RTL8372N_LOG_ERR("dal_rtl8373_svlanMbrPortEntry_get fail...ret=%d\n", ret);
		}
		RTL8372N_UNLOCK();

		seq_printf(s, "%0d\t", svlanCfg.fid);
		seq_printf(s, "%0d\n", svlanCfg.efid);
	}
	seq_printf(s, "============================\n");

	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		seq_printf(s, "port=%d dev=%s group=%d iso=%d\n",
				   p, rtl8372n_dev_name[p], rtl8372n_br_group[p], rtl8372n_iso_mode[p]);
	}

	return 0;
}


static int rtl8372n_hw_bridge_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_hw_bridge_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8372n_hw_bridge_write(struct file *file, const char __user *buffer,
										size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 val, pidx;
	int argc = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	argc = sscanf(val_string, "%d %d", &val, &pidx);
	if (argc != 1 && argc != 2)
	{
		printk("usage: <action> enable [ port ]\n");
		return count;
	}
	RTL8372N_LOCK();
	if (argc == 1)
	{
		if (rtl8372n_hw_bridge == val)
		{
			RTL8372N_LOG_ERR("same hw_bridge %d\n", val);
			goto error;
		}

		rtl8372n_hw_bridge = val;
	}
	else if (argc == 2)
	{
		if (pidx < 0 || pidx >= PORT_MAX_NUM)
		{
			RTL8372N_LOG_ERR("pidx out of range %d\n", pidx);
			goto error;
		}
		if (rtl8372n_hw_bridge_port[pidx] == val)
		{
			RTL8372N_LOG_ERR("same hw_bridge_port %d\n", val);
			goto error;
		}
		rtl8372n_hw_bridge_port[pidx] = val;
	}
	else
	{
		RTL8372N_LOG_ERR("wrong argc %d\n", argc);
		goto error;
	}

	_rtl8372n_lookup_refresh();
	rtk_l2_table_clear();
	if (rtl8372n_cpu_mac[0] != 0 ||
		rtl8372n_cpu_mac[1] != 0 ||
		rtl8372n_cpu_mac[2] != 0 ||
		rtl8372n_cpu_mac[3] != 0 ||
		rtl8372n_cpu_mac[4] != 0 ||
		rtl8372n_cpu_mac[5] != 0)
	{
		_rtl8372n_cpumac_refresh();
	}
error:
	RTL8372N_UNLOCK();
	return count;
}
static struct file_operations rtl8372n_hw_bridge_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_hw_bridge_open,
	.read = seq_read,
	.write = rtl8372n_hw_bridge_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_eth_forward_get(struct seq_file *s, void *unused)
{
	seq_printf(s, "eth_forward=%d\n", rtl8372n_eth_forward);
	return 0;
}

static int rtl8372n_eth_forward_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_eth_forward_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8372n_eth_forward_write_proc(struct file *file, const char __user *buffer,
											   size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 val;
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}
	RTL8372N_LOCK();
	if (rtl8372n_eth_forward == val)
	{
		RTL8372N_LOG_ERR("same eth_forward %d\n", val);
		goto error;
	}
	rtl8372n_eth_forward = val;
	_rtl8372n_lookup_refresh();
	rtk_l2_table_clear();
	if (rtl8372n_cpu_mac[0] != 0 ||
		rtl8372n_cpu_mac[1] != 0 ||
		rtl8372n_cpu_mac[2] != 0 ||
		rtl8372n_cpu_mac[3] != 0 ||
		rtl8372n_cpu_mac[4] != 0 ||
		rtl8372n_cpu_mac[5] != 0)
	{
		_rtl8372n_cpumac_refresh();
	}
error:
	RTL8372N_UNLOCK();

	return count;
}

static struct file_operations rtl8372n_eth_forward_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_eth_forward_open,
	.read = seq_read,
	.write = rtl8372n_eth_forward_write_proc,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_cpu_mac_get(struct seq_file *s, void *unused)
{
	seq_printf(s, "cpu_mac=%02x:%02x:%02x:%02x:%02x:%02x\n",
			   rtl8372n_cpu_mac[0], rtl8372n_cpu_mac[1], rtl8372n_cpu_mac[2],
			   rtl8372n_cpu_mac[3], rtl8372n_cpu_mac[4], rtl8372n_cpu_mac[5]);
	return 0;
}

static int rtl8372n_cpu_mac_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_cpu_mac_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8372n_cpu_mac_write_proc(struct file *file, const char __user *buffer,
										   size_t count, loff_t *pPos)
{
	char val_string[128];
	unsigned char mac[6];
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	if (sscanf(val_string, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
			   &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) != 6)
	{
		printk("usage: <action>\n");
		return count;
	}
	RTL8372N_LOCK();

	if (memcmp(rtl8372n_cpu_mac, mac, sizeof(rtl8372n_cpu_mac)) == 0)
	{
		RTL8372N_LOG_ERR("same cpu_mac %02x%02x%02x%02x%02x%02x\n",
						 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		goto error;
	}

	memcpy(rtl8372n_cpu_mac, mac, sizeof(rtl8372n_cpu_mac));

	rtk_l2_table_clear();

	if (rtl8372n_cpu_mac[0] != 0 ||
		rtl8372n_cpu_mac[1] != 0 ||
		rtl8372n_cpu_mac[2] != 0 ||
		rtl8372n_cpu_mac[3] != 0 ||
		rtl8372n_cpu_mac[4] != 0 ||
		rtl8372n_cpu_mac[5] != 0)
	{
		_rtl8372n_cpumac_refresh();
	}
error:
	RTL8372N_UNLOCK();

	return count;
}

static struct file_operations rtl8372n_cpu_mac_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_cpu_mac_open,
	.read = seq_read,
	.write = rtl8372n_cpu_mac_write_proc,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_debug_info_get(struct seq_file *s, void *unused)
{
	seq_printf(s, "debug_info=%d\n", rtl8372n_debug_info);

	return 0;
}

static int rtl8372n_debug_info_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_debug_info_get, file->f_path.dentry->d_iname);
}

static int rtl8372n_dump_phy_status(void)
{
	char *speed_str[] = {"10M", "100M", "1000M", "500M", "10G", "2.5G", "5G", "None"};
	int ports[] = {UTP_PORT3, UTP_PORT4, UTP_PORT5, UTP_PORT6, UTP_PORT7, UTP_PORT8};

	rtk_port_status_t pStatus[PORT_NUM] = {0};

	int p = 0;
	int len = 0;
	char s[256];

	memset(s, 0, sizeof(s)); len = 0;

	len += sprintf(s + len, "%-16s", "Phy Status:");

	for (p = 0; p < PORT_NUM; p++)
	{
		RTL8372N_LOCK();
		if (RT_ERR_OK != rtk_port_macStatus_get(ports[p], &pStatus[p]))
		{
			RTL8372N_UNLOCK();
			RTL8372N_LOG_ERR("rtk_port_phyStatus_get failed...\n");
			goto error;
		}
		RTL8372N_UNLOCK();
		len += sprintf(s + len, "UTP%-9d", ports[p]);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Link Status:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pStatus[p].link);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Speed:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12s", speed_str[pStatus[p].speed]);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Duplex:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pStatus[p].duplex);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Tx Pause:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pStatus[p].txpause);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Rx Pause:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pStatus[p].rxpause);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Media:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pStatus[p].media);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "EEE:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pStatus[p].eee);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Master:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pStatus[p].master);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Master Slave:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pStatus[p].master_slave);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

error:

	return 0;
}

static int rtl8372n_dump_isolation_table(void)
{
	int p = 0;
	int q = 0;
	int len = 0;
	char s[256];

	int ports[] = {UTP_PORT3, UTP_PORT4, UTP_PORT5, UTP_PORT6, UTP_PORT7, UTP_PORT8};

	rtk_uint32 portIso[PORT_NUM] = {0};


	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "To Port:");

	for (p = 0; p < PORT_NUM; p++)
	{
		RTL8372N_LOCK();
		if (RT_ERR_OK != rtk_port_isolation_get(ports[p], &portIso[p]))
		{
			RTL8372N_UNLOCK();
			RTL8372N_LOG_ERR("rtk_port_phyStatus_get failed...\n");
			goto error;
		}
		RTL8372N_UNLOCK();
		len += sprintf(s + len, "UTP%-9d", ports[p]);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	for (p = 0; p < PORT_NUM; p++)
	{
		memset(s, 0, sizeof(s)); len = 0;
		len += sprintf(s + len, "From UTP%-8d", ports[p]);
		for (q = 0; q < PORT_NUM; q++)
		{
			len += sprintf(s + len, "%-12s", (portIso[p] & (1 << ports[q])) ? "Y":"N");
		}
		len += sprintf(s + len, "\r\n");
		printk(s);
	}

error:

	return 0;
}

static int rtl8372n_dump_svlan_table(void)
{
	char *attr_str[] = {"svid", "memberport", "untagport", "fiden", "fid", "priority", "efiden", "efid", "chk_ivl_svl", "ivl_svl"};
	int ports[] = {UTP_PORT3, UTP_PORT4, UTP_PORT5, UTP_PORT6, UTP_PORT7, UTP_PORT8};
	int svlan_start = RTL8372N_VID_RESERVED_NUM;
	const int svlan_num = RTL8372N_VID_RESERVED_NUM + PORT_NUM - 1;/*exclude cpu port*/
	rtk_svlan_memberCfg_t svlanCfg;

	int p = 0;
	int len = 0;
	char s[256] = {0};

	memset(s, 0, sizeof(s));
	len += sprintf(s + len, "%-8s", "SVLAN");

	for (p = 0; p < sizeof(attr_str)/sizeof(attr_str[0]); p++)
	{
		len += sprintf(s + len, "%-12s", attr_str[p]);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	for (p = svlan_start; p < svlan_num; p++)
	{
		RTL8372N_LOCK();
		if (RT_ERR_OK != dal_rtl8373_svlanMbrPortEntry_get(p, &svlanCfg))
		{
			RTL8372N_UNLOCK();
			RTL8372N_LOG_ERR("dal_rtl8373_svlanMbrPortEntry_get fail...\n");
			goto error;
		}
		RTL8372N_UNLOCK();

		memset(s, 0, sizeof(s)); len = 0;
		len += sprintf(s + len, "%-8d", p);
		len += sprintf(s + len, "%-12d", svlanCfg.svid);
		len += sprintf(s + len, "0x%-10x", svlanCfg.memberport.bits[0]);
		len += sprintf(s + len, "0x%-10x", svlanCfg.untagport.bits[0]);
		len += sprintf(s + len, "%-12d", svlanCfg.fiden);
		len += sprintf(s + len, "%-12d", svlanCfg.fid);
		len += sprintf(s + len, "%-12d", svlanCfg.priority);
		len += sprintf(s + len, "%-12d", svlanCfg.efiden);
		len += sprintf(s + len, "%-12d", svlanCfg.efid);
		len += sprintf(s + len, "%-12d", svlanCfg.chk_ivl_svl);
		len += sprintf(s + len, "%-12d", svlanCfg.ivl_svl);
		len += sprintf(s + len, "\r\n");
		printk(s);
	}

error:

	return 0;
}

static int rtl8372n_port_vlan_dump(void)
{
	int p = 0;
	int len = 0;
	char s[256];

	int ports[] = {UTP_PORT3, UTP_PORT4, UTP_PORT5, UTP_PORT6, UTP_PORT7, UTP_PORT8};

	rtk_enable_t pfid_enable[PORT_NUM] = {0};
	rtk_fid_t pfid[PORT_NUM] = {0};
	rtk_vlan_t svid[PORT_NUM] = {0};
	rtk_vlan_t pvid[PORT_NUM] = {0};

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Port");

	for (p = 0; p < PORT_NUM; p++)
	{
		RTL8372N_LOCK();
		if (RT_ERR_OK != rtk_vlan_portFid_get(ports[p], &pfid_enable[p], &pfid[p]))
		{
			RTL8372N_UNLOCK();
			RTL8372N_LOG_ERR("rtk_vlan_portFid_get failed...\n");
			goto error;
		}

		if (RT_ERR_OK != rtk_svlan_defaultSvlan_get(ports[p], &svid[p]))
		{
			RTL8372N_UNLOCK();
			RTL8372N_LOG_ERR("rtk_svlan_defaultSvlan_get failed...\n");
			goto error;
		}

		if (RT_ERR_OK != rtk_vlan_portPvid_get(ports[p], &pvid[p]))
		{
			RTL8372N_UNLOCK();
			RTL8372N_LOG_ERR("rtk_vlan_portPvid_get failed...\n");
			goto error;
		}
		RTL8372N_UNLOCK();

		len += sprintf(s + len, "UTP%-9d", ports[p]);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "Pfid_enable:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pfid_enable[p]);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "fid:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pfid[p]);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "svid:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", svid[p]);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

	memset(s, 0, sizeof(s)); len = 0;
	len += sprintf(s + len, "%-16s", "pvid:");
	for (p = 0; p < PORT_NUM; p++)
	{
		len += sprintf(s + len, "%-12u", pvid[p]);
	}
	len += sprintf(s + len, "\r\n");
	printk(s);

error:

	return 0;
}

static ssize_t rtl8372n_debug_info_write(struct file *file, const char __user *buffer,
										 size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 val;
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}

	if (1 == val || 0 == val)
	{
		rtl8372n_debug_info = val;
	}
	else
	{
		switch (val)
		{
			case 2: /* permmit表*/
				rtl8372n_dump_isolation_table();
				break;
			case 3:	/* 端口状态 */
				rtl8372n_dump_phy_status();
				break;
			case 4: /* svlan表*/
				rtl8372n_dump_svlan_table();
				break;
			case 5: /* portvlan表 */
				rtl8372n_port_vlan_dump();
				break;
			default:
				break;
		}
	}

	return count;
}

static struct file_operations rtl8372n_debug_info_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_debug_info_open,
	.read = seq_read,
	.write = rtl8372n_debug_info_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_phy_testmode_get(struct seq_file *s, void *unused)
{
	seq_printf(s, "port 0=%d port 1=%d port 2=%d port 3=%d port 4=%d\n",
			   rtl8372n_phy_testmode[0], rtl8372n_phy_testmode[1], rtl8372n_phy_testmode[2],
			   rtl8372n_phy_testmode[3], rtl8372n_phy_testmode[4]);
	return 0;
}

static int rtl8372n_phy_testmode_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_phy_testmode_get, file->f_path.dentry->d_iname);
}

static ssize_t rtl8372n_phy_testmode_write(struct file *file, const char __user *buffer,
										   size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 phy, mode;
	int argc = 0;
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	argc = sscanf(val_string, "%d %d", &phy, &mode);
	if (argc != 2)
	{
		printk("usage:<action> port mode\n");
	}
#if 0
/* To Do*/
	if(RT_ERR_OK != rtk_port_phyTestMode_set(rtl8372n_utp_port[phy], mode))
	{
		RTL8372N_LOG_ERR("rtk_port_phyTestMode_set error\r\n");
	}
	if(RT_ERR_OK != rtk_port_phyTestMode_get(rtl8372n_utp_port[phy], &mode))
	{
		RTL8372N_LOG_ERR("rtk_port_phyTestMode_get error\r\n");
	}
#endif
	RTL8372N_LOG_INFO("port=%d mode=%d\r\n", phy, mode);
	rtl8372n_phy_testmode[phy] = mode;
	return count;
}
static struct file_operations rtl8372n_phy_testmode_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_phy_testmode_open,
	.read = seq_read,
	.write = rtl8372n_phy_testmode_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static ssize_t rtl8372n_set_port_iso_write_proc(struct file *file, const char __user *buffer,
												size_t count, loff_t *pPos)
{
	char val_string[128];
	int argc;

	int port_index, port_iso;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	argc = sscanf(val_string, "%d %d", &port_index, &port_iso);
	if (argc != 2)
	{
		printk("usage: port_index port_iso\nFor port_index and port_iso info, check hw_bridge.\n");
		return count;
	}

	if (port_iso != 0 && port_iso != 1)
	{
		RTL8372N_LOG_ERR("invalid port_iso\r\n");
		return count;
	}
	if (port_index < PORT_IDX0 || port_index >= PORT_MAX_NUM)
	{
		RTL8372N_LOG_ERR("port_index out of range\r\n");
		return count;
	}

	if (rtl8372n_iso_mode[port_index] == port_iso)
	{
		RTL8372N_LOG_ERR("same iso %d\r\n", port_iso);
		return count;
	}
	rtl8372n_iso_mode[port_index] = port_iso;

	RTL8372N_LOCK();

	if (rtl8372n_hw_bridge)
	{
		_rtl8372n_lookup_refresh();
	}
	rtk_l2_table_clear();
	RTL8372N_UNLOCK();

	return count;
}

static struct file_operations rtl8372n_set_port_iso_fops = {
	.owner = THIS_MODULE,
	.read = NULL,
	.write = rtl8372n_set_port_iso_write_proc};

static int rtl8372n_hw_init(void)
{
	return rtl8372n_reset(6);
}

static int rtl8372n_mii_init(void)
{
	int ret = 0;
	RTL8372N_LOG_DEBUG("init\r\n");
	/*初始化switch*/
	ret = rtk_switch_init();

	if (ret != RT_ERR_OK)
	{
		printk("!!!rtk_switch_init failed...ret=%d\n", ret);
		return -1;
	}

	ret = rtk_sdsMode_set(SERDES_ID0, SERDES_10GR);

	if (ret != RT_ERR_OK)
	{
		printk("!!!sdsMode set SERDES_ID0 failed...ret=%d\n", ret);
		return -1;
	}

	ret = rtk_sdsMode_set(SERDES_ID1, SERDES_10GUSXG);

	if (ret != RT_ERR_OK)
	{
		printk("!!!sdsMode set SERDES_ID1 failed...ret=%d\n", ret);
		return -1;
	}

	// disable UTP4 which is not used by XGB834v
#if defined(INCLUDE_SPEC_XGB834v) || defined(INCLUDE_SPEC_XGB830v)
	rtk_phy_common_c45_enable_set(UTP_PORT4, DISABLED);
#endif /* INCLUDE_SPEC_XGB834v || INCLUDE_SPEC_XGB830v */

	rtl8372n_mii_type = -1;

	return ret;
}

static int rtl8372n_vlan_init(void)
{
	int ret = 0;
	RTL8372N_LOG_DEBUG("init\r\n");
	if ((ret = rtk_svlan_init()) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_svlan_init fail...ret=%d\n", ret);
		return ret;
	}
	if ((ret = rtk_svlan_tpidEntry_set(ETH_P_8021Q)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_svlan_tpidEntry_set fail...ret=%d\n", ret);
		return ret;
	}
	if ((ret = rtk_svlan_servicePort_add(rtl8372n_cpu_port)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_svlan_servicePort_add fail...ret=%d\n", ret);
		return ret;
	}
	if ((ret = rtk_svlan_untag_action_set(UNTAG_DROP, 0)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_svlan_untag_action_set fail...ret=%d\n", ret);
		return ret;
	}

	_rtl8372n_lookup_refresh();
	return 0;
}

static int rtl8372n_mcast_init(void)
{
	rtk_api_ret_t ret;
	int p;
	RTL8372N_LOG_DEBUG("init\r\n");

	if ((ret = rtk_igmp_init() != RT_ERR_OK))
	{
		RTL8372N_LOG_ERR("rtk_igmp_init fail...ret=%d\r\n", ret);
	}
#if 0
	if((ret = rtk_l2_ipMcastAddrLookup_set(LOOKUP_IP_VID)) != RT_ERR_OK)
	{
		RTL8372N_LOG_ERR("rtk_l2_ipMcastAddrLookup_set fail...ret=%d\n", ret);
		return ret;
	}
#endif
	for (p = PORT_IDX0; p < PORT_MAX_NUM; p++)
	{
		if ((ret = rtk_igmp_protocol_set(rtl8372n_utp_port[p],
										 PROTOCOL_IGMPv1, IGMP_ACTION_ASIC) != RT_ERR_OK))
		{
			RTL8372N_LOG_ERR("rtk_igmp_protocol_set fail...ret=%d\r\n", ret);
			return ret;
		}

		if ((ret = rtk_igmp_protocol_set(rtl8372n_utp_port[p],
										 PROTOCOL_IGMPv2, IGMP_ACTION_ASIC) != RT_ERR_OK))
		{
			RTL8372N_LOG_ERR("rtk_igmp_protocol_set fail\r\n");
			return ret;
		}

		if ((ret = rtk_igmp_protocol_set(rtl8372n_utp_port[p],
										 PROTOCOL_IGMPv3, IGMP_ACTION_ASIC) != RT_ERR_OK))
		{
			RTL8372N_LOG_ERR("rtk_igmp_protocol_set fail\r\n");
			return ret;
		}

		if ((ret = rtk_igmp_protocol_set(rtl8372n_utp_port[p],
										 PROTOCOL_MLDv1, IGMP_ACTION_ASIC) != RT_ERR_OK))
		{
			RTL8372N_LOG_ERR("rtk_igmp_protocol_set fail\r\n");
			return ret;
		}

		if ((ret = rtk_igmp_protocol_set(rtl8372n_utp_port[p],
										 PROTOCOL_MLDv2, IGMP_ACTION_ASIC) != RT_ERR_OK))
		{
			RTL8372N_LOG_ERR("rtk_igmp_protocol_set fail\r\n");
			return ret;
		}
	}

	_rtl8372n_mcast_refresh();
	return 0;
}

int rtl8372n_event_init(void)
{
	extern int (*tp_ext_switch_notify_if_change)(struct net_device *br, struct net_device *dev, int add);
	extern int (*tp_ext_switch_notify_iso_change)(struct net_device *br, struct net_device *dev, int iso);
	extern int (*tp_ext_switch_notify_mcr_change)(struct net_device *br, struct net_device *dev, int mcr);
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
	extern int (*tp_ext_switch_notify_vlan_change)(struct net_device *dev, int add);
#endif /**/
	extern int (*tp_ext_switch_br_should_forward)(struct net_device *in, struct net_device *out);
	extern int (*tp_ext_switch_ethtool_get_link_ksettings)(struct net_device * dev, struct ethtool_link_ksettings *cmd);
	extern int (*tp_ext_switch_dev_get_stats)(struct net_device *dev, struct rtnl_link_stats64 *storage);
	RTL8372N_LOG_DEBUG("init\r\n");
	tp_ext_switch_notify_if_change = rtl8372n_notify_if_change;
	tp_ext_switch_notify_iso_change = rtl8372n_notify_iso_change;
	tp_ext_switch_notify_mcr_change = rtl8372n_notify_mcr_change;
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
	tp_ext_switch_notify_vlan_change = rtl8372n_notify_vlan_change;
#endif /**/
	tp_ext_switch_br_should_forward = rtl8372n_br_should_forward;
	tp_ext_switch_ethtool_get_link_ksettings = rtl8372n_ethtool_get_link_ksettings;
	tp_ext_switch_dev_get_stats = rtl8372n_dev_get_stats;

	rtl8372n_phy_link_timer_start();
	return 0;
}

int rtl8372n_event_exit(void)
{
	extern int (*tp_ext_switch_notify_if_change)(struct net_device *br, struct net_device *dev, int add);
	extern int (*tp_ext_switch_notify_iso_change)(struct net_device *br, struct net_device *dev, int iso);
	extern int (*tp_ext_switch_notify_mcr_change)(struct net_device *br, struct net_device *dev, int mcr);
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
	extern int (*tp_ext_switch_notify_vlan_change)(struct net_device *dev, int add);
#endif /**/
	extern int (*tp_ext_switch_br_should_forward)(struct net_device *in, struct net_device *out);
	extern int (*tp_ext_switch_ethtool_get_link_ksettings)(struct net_device * dev, struct ethtool_link_ksettings * cmd);
	extern int (*tp_ext_switch_dev_get_stats)(struct net_device *dev, struct rtnl_link_stats64 *storage);
	RTL8372N_LOG_DEBUG("init\r\n");
	tp_ext_switch_notify_if_change = NULL;
	tp_ext_switch_notify_iso_change = NULL;
	tp_ext_switch_notify_mcr_change = NULL;
#ifdef RTL8372N_HW_BRIDGE_PORT_VLAN_DETECT
	tp_ext_switch_notify_vlan_change = NULL;
#endif /**/
	tp_ext_switch_br_should_forward = NULL;
	tp_ext_switch_ethtool_get_link_ksettings = NULL;
	tp_ext_switch_dev_get_stats = NULL;

	del_timer(&rtl8372n_phy_link_timer);

	return 0;
}

static ssize_t rtl8372n_reset_write_proc(struct file *file, const char __user *buffer,
										 size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 val;
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	if (sscanf(val_string, "%d", &val) != 1)
	{
		printk("usage: <action>\n");
		return count;
	}
	rtl8372n_hw_init();

	RTL8372N_LOCK();

	rtl8372n_mii_init();
	rtl8372n_vlan_init();

	rtl8372n_mcast_init();

	RTL8372N_UNLOCK();

	return count;
}

static struct file_operations rtl8372n_reset_fops = {
	.owner = THIS_MODULE,
	.open = NULL,
	.read = NULL,
	.write = rtl8372n_reset_write_proc,
	.llseek = NULL,
	.release = NULL,
};

static ssize_t rtl8372n_fw_reset_write_proc(struct file *file, const char __user *buffer,
										 size_t count, loff_t *pPos)
{
	char val_string[128];
	rtk_uint32 sdsid;
	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	if (sscanf(val_string, "%d", &sdsid) != 1)
	{
		printk("usage: <sdsid>\n");
		return count;
	}

	if ((sdsid < 0) || (sdsid > 1))
	{
		printk("invalid sdsid\n");
		return count;
	}

	RTL8372N_LOCK();

	if (RT_ERR_OK != rtk_fw_reset_flow_tgr_tgx(sdsid))
	{
		RTL8372N_LOG_ERR("rtk_fw_reset_flow_tgr_tgx error\r\n");
	}

	RTL8372N_UNLOCK();

	return count;
}

static struct file_operations rtl8372n_fw_reset_fops = {
	.owner = THIS_MODULE,
	.open = NULL,
	.read = NULL,
	.write = rtl8372n_fw_reset_write_proc,
	.llseek = NULL,
	.release = NULL,
};

static int rtl8372n_tm0_get(struct seq_file *s, void *unused)
{
	rtk_int32 temp = 0;

	RTL8372N_LOCK();
	if (RT_ERR_OK != get_junc_temp(0, &temp))
	{
		RTL8372N_LOG_ERR("get_junc_temp failed...\n");
		goto error;
	}
	seq_printf(s, "%d\n", temp);

error:
	RTL8372N_UNLOCK();
	return 0;
}

static int rtl8372n_tm1_get(struct seq_file *s, void *unused)
{
	rtk_int32 temp = 0;

	RTL8372N_LOCK();
	if (RT_ERR_OK != get_junc_temp(1, &temp))
	{
		RTL8372N_LOG_ERR("get_junc_temp failed...\n");
		goto error;
	}
	seq_printf(s, "%d\n", temp);

error:
	RTL8372N_UNLOCK();
	return 0;
}

static int rtl8372n_tm0_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_tm0_get, file->f_path.dentry->d_iname);
}

static int rtl8372n_tm1_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_tm1_get, file->f_path.dentry->d_iname);
}

static struct file_operations rtl8372n_tm0_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_tm0_open,
	.read = seq_read,
	.write = NULL,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations rtl8372n_tm1_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_tm1_open,
	.read = seq_read,
	.write = NULL,
	.llseek = seq_lseek,
	.release = single_release,
};

static int rtl8372n_lan_nego_fc_status_get(struct seq_file *s, void *unused)
{
	rtk_port_phy_ability_t portAbility[PORT_MAX_NUM];
	int p = 0;
	int ret = 0;

	memset(portAbility, 0, PORT_MAX_NUM * sizeof(rtk_port_phy_ability_t));
	
	for (p = 0; p < PORT_MAX_NUM; p++)
	{
		RTL8372N_LOCK();
		ret = rtk_phy_autoNegoAbility_get(rtl8372n_utp_port[p], &portAbility[p]);
		if (RT_ERR_OK != ret)
		{
			RTL8372N_LOG_ERR("rtk_port(%d), index(%d) get nego fc fail, ret(%d)...\n", rtl8372n_utp_port[p], p, ret);
			RTL8372N_UNLOCK();
			continue;
		}
		RTL8372N_UNLOCK();
	}

	for (p = 0; p < PORT_MAX_NUM; p++)
	{
		seq_printf(s, "%u %u %u %u %u %u %u %u %u %u\n", 
					portAbility[p].Half_10,
					portAbility[p].Full_10,
					portAbility[p].Half_100,
					portAbility[p].Full_100,
					portAbility[p].Full_1000,
					portAbility[p].adv_2_5G, 
					portAbility[p].adv_5G, 
					portAbility[p].adv_10GBase_T, 
					portAbility[p].FC, 
					portAbility[p].AsyFC);
	}

	return 0;
}

static int rtl8372n_lan_nego_fc_open(struct inode *inode, struct file *file)
{
	return single_open(file, rtl8372n_lan_nego_fc_status_get, file->f_path.dentry->d_iname);
}

static int rtl8372n_lan_nego_fc_write(struct file *file, const char __user *buffer,
								 size_t count, loff_t *pPos)
{
	char val_string[128] = {0};
	rtk_port_t port = 0;
	rtk_port_phy_ability_t portAbility;
	int ret = 0;
	unsigned int fc = 0;
	unsigned int asyFc = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	memset(&portAbility, 0, sizeof(rtk_port_phy_ability_t));

	if (3 != sscanf(val_string, "%u %u %u", &port, &fc, &asyFc))
	{
		RTL8372N_LOG_ERR("usage: <port> <fc> <asyFc>\n");
		return count;
	}

	RTL8372N_LOG_DEBUG("port(%d), fc(%u), asynfc(%u)", port, fc, asyFc);

	RTL8372N_LOCK();
	ret = rtk_phy_autoNegoAbility_get(rtl8372n_utp_port[port], &portAbility);
	if (RT_ERR_OK != ret)
	{
		RTL8372N_LOG_ERR("rtk_port(%d) get nego fc fail, ret(%d)...\n", rtl8372n_utp_port[port], ret);
		goto error;
	}

	portAbility.FC = fc;
	portAbility.AsyFC = asyFc;

	ret = rtk_phy_autoNegoAbility_set(rtl8372n_utp_port[port], &portAbility);
	if (RT_ERR_OK != ret)
	{
		RTL8372N_LOG_ERR("rtk_port(%d) set nego fc fail, ret(%d)...\n", rtl8372n_utp_port[port], ret);
		goto error;
	}

error:
	RTL8372N_UNLOCK();
	return count;
}

static struct file_operations rtl8372n_nego_fc_fops = {
	.owner = THIS_MODULE,
	.open = rtl8372n_lan_nego_fc_open,
	.read = seq_read,
	.write = rtl8372n_lan_nego_fc_write,
	.llseek = seq_lseek,
	.release = single_release,
};

int rtl8372n_proc_init(void)
{
	procRegDir = proc_mkdir("rtl8372", NULL);

	proc_create("switch_reg", 0, procRegDir, &rtl8372n_switch_reg_fops);
	proc_create("phy_reg", 0, procRegDir, &rtl8372n_phy_reg_fops);
	proc_create("phy_reg_bits", 0, procRegDir, &rtl8372n_phy_reg_bits_fops);
	proc_create("phy_reg_multi_bits", 0, procRegDir, &rtl8372n_phy_reg_multi_bits_fops);
	proc_create("sds_reg", 0, procRegDir, &rtl8372n_sds_reg_fops);
	proc_create("phy_status", 0, procRegDir, &rtl8372n_phy_status_fops);
	proc_create("mii_type", 0, procRegDir, &rtl8372n_mii_type_fops);
	proc_create("ucast_tbl", 0, procRegDir, &rtl8372n_ucast_tbl_fops);
	proc_create("ucast_tbl_single", 0, procRegDir, &rtl8372n_ucast_tbl_single_fops);
	proc_create("mcast_tbl", 0, procRegDir, &rtl8372n_mcast_tbl_fops);
	proc_create("mib_cnt", 0, procRegDir, &rtl8372n_mib_cnt_fops);
	proc_create("mcast_snoop", 0, procRegDir, &rtl8372n_mcast_snoop_fops);
	proc_create("port_mirror", 0, procRegDir, &rtl8372n_port_mirror_fops);
	proc_create("hw_bridge", 0, procRegDir, &rtl8372n_hw_bridge_fops);
	proc_create("eth_forward", 0, procRegDir, &rtl8372n_eth_forward_fops);
	proc_create("cpu_mac", 0, procRegDir, &rtl8372n_cpu_mac_fops);
	proc_create("reset", 0, procRegDir, &rtl8372n_reset_fops);
	proc_create("fw_reset", 0, procRegDir, &rtl8372n_fw_reset_fops);
	proc_create("debug_info", 0, procRegDir, &rtl8372n_debug_info_fops);
	proc_create("phy_testmode", 0, procRegDir, &rtl8372n_phy_testmode_fops);
	proc_create("set_port_iso", 0, procRegDir, &rtl8372n_set_port_iso_fops);
	proc_create("tm0", 0, procRegDir, &rtl8372n_tm0_fops);
	proc_create("tm1", 0, procRegDir, &rtl8372n_tm1_fops);
	proc_create("nego_fc", 0, procRegDir, &rtl8372n_nego_fc_fops);

	return 0;
}

int rtl8372n_proc_exit(void)
{
	remove_proc_entry("switch_reg", procRegDir);
	remove_proc_entry("phy_reg", procRegDir);
	remove_proc_entry("phy_reg_bits", procRegDir);
	remove_proc_entry("phy_reg_multi_bits", procRegDir);
	remove_proc_entry("sds_reg", procRegDir);
	remove_proc_entry("phy_status", procRegDir);
	remove_proc_entry("mii_type", procRegDir);
	remove_proc_entry("ucast_tbl", procRegDir);
	remove_proc_entry("ucast_tbl_single", procRegDir);
	remove_proc_entry("mcast_tbl", procRegDir);
	remove_proc_entry("mib_cnt", procRegDir);
	remove_proc_entry("mcast_snoop", procRegDir);
	remove_proc_entry("port_mirror", procRegDir);
	remove_proc_entry("hw_bridge", procRegDir);
	remove_proc_entry("eth_forward", procRegDir);
	remove_proc_entry("cpu_mac", procRegDir);
	remove_proc_entry("reset", procRegDir);
	remove_proc_entry("fw_reset", procRegDir);
	remove_proc_entry("debug_info", procRegDir);
	remove_proc_entry("phy_testmode", procRegDir);
	remove_proc_entry("set_port_iso", procRegDir);
	remove_proc_entry("tm0", procRegDir);
	remove_proc_entry("tm1", procRegDir);
	remove_proc_entry("nego_fc", procRegDir);

	remove_proc_entry("rtl8372", NULL);

	return 0;
}

static int rtl8372n_tm_init(void)
{
	RTL8372N_LOCK();
	tm_init(0);
	tm_init(1);
	RTL8372N_UNLOCK();
}

int rtl8372n_switch_init(void)
{
	RTL8372N_LOCK_INIT();

	rtl8372n_hw_init();
	rtl8372n_mii_init();

	rtl8372n_vlan_init();

	rtl8372n_acl_init();
	rtl8372n_mcast_snoop = 0;
	rtl8372n_mcast_init();
	rtl8372n_tm_init();

	rtl8372n_event_init();
	rtl8372n_proc_init();

	return 0;
}

static int rtl8372n_switch_exit(void)
{
	rtl8372n_event_exit();

	rtl8372n_proc_exit();

	return 0;
}

