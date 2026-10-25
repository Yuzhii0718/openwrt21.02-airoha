// SPDX-License-Identifier: GPL-2.0

#include <linux/if.h>
#include <linux/list.h>
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/bitops.h>
#include <net/genetlink.h>
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/lockdep.h>
#include <linux/workqueue.h>
#include <linux/of_device.h>
#include "mt753x_swconfig.h"

struct mt753x_mapping mt753x_def_mapping[] = {
	{
		.name = "llllw",
		.pvids = { 1, 1, 1, 1, 2, 2, 1 },
		.members = { 0, 0x4f, 0x30 },
		.etags = { 0, 0, 0 },
		.vids = { 0, 1, 2 },
	}, {
		.name = "wllll",
		.pvids = { 2, 1, 1, 1, 1, 2, 1 },
		.members = { 0, 0x5e, 0x21 },
		.etags = { 0, 0, 0 },
		.vids = { 0, 1, 2 },
	}, {
		.name = "lwlll",
		.pvids = { 1, 2, 1, 1, 1, 2, 1 },
		.members = { 0, 0x5d, 0x22 },
		.etags = { 0, 0, 0 },
		.vids = { 0, 1, 2 },
	}, {
		.name = "lllll",
		.pvids = { 1, 1, 1, 1, 1, 1, 1 },
		.members = { 0, 0x7f },
		.etags = { 0, 0 },
		.vids = { 0, 1 },
	},
};

static const struct mt753x_mib_desc mt753x_mibs[] = {
	MIB_DESC(1, STATS_TDPC, "TxDrop"),
	MIB_DESC(1, STATS_TCRC, "TxCRC"),
	MIB_DESC(1, STATS_TUPC, "TxUni"),
	MIB_DESC(1, STATS_TMPC, "TxMulti"),
	MIB_DESC(1, STATS_TBPC, "TxBroad"),
	MIB_DESC(1, STATS_TCEC, "TxCollision"),
	MIB_DESC(1, STATS_TSCEC, "TxSingleCol"),
	MIB_DESC(1, STATS_TMCEC, "TxMultiCol"),
	MIB_DESC(1, STATS_TDEC, "TxDefer"),
	MIB_DESC(1, STATS_TLCEC, "TxLateCol"),
	MIB_DESC(1, STATS_TXCEC, "TxExcCol"),
	MIB_DESC(1, STATS_TPPC, "TxPause"),
	MIB_DESC(1, STATS_TL64PC, "Tx64Byte"),
	MIB_DESC(1, STATS_TL65PC, "Tx65Byte"),
	MIB_DESC(1, STATS_TL128PC, "Tx128Byte"),
	MIB_DESC(1, STATS_TL256PC, "Tx256Byte"),
	MIB_DESC(1, STATS_TL512PC, "Tx512Byte"),
	MIB_DESC(1, STATS_TL1024PC, "Tx1024Byte"),
	MIB_DESC(2, STATS_TOC, "TxByte"),
	MIB_DESC(1, STATS_RDPC, "RxDrop"),
	MIB_DESC(1, STATS_RFPC, "RxFiltered"),
	MIB_DESC(1, STATS_RUPC, "RxUni"),
	MIB_DESC(1, STATS_RMPC, "RxMulti"),
	MIB_DESC(1, STATS_RBPC, "RxBroad"),
	MIB_DESC(1, STATS_RAEPC, "RxAlignErr"),
	MIB_DESC(1, STATS_RCEPC, "RxCRC"),
	MIB_DESC(1, STATS_RUSPC, "RxUnderSize"),
	MIB_DESC(1, STATS_RFEPC, "RxFragment"),
	MIB_DESC(1, STATS_ROSPC, "RxOverSize"),
	MIB_DESC(1, STATS_RJEPC, "RxJabber"),
	MIB_DESC(1, STATS_RPPC, "RxPause"),
	MIB_DESC(1, STATS_RL64PC, "Rx64Byte"),
	MIB_DESC(1, STATS_RL65PC, "Rx65Byte"),
	MIB_DESC(1, STATS_RL128PC, "Rx128Byte"),
	MIB_DESC(1, STATS_RL256PC, "Rx256Byte"),
	MIB_DESC(1, STATS_RL512PC, "Rx512Byte"),
	MIB_DESC(1, STATS_RL1024PC, "Rx1024Byte"),
	MIB_DESC(2, STATS_ROC, "RxByte"),
	MIB_DESC(1, STATS_RDPC_CTRL, "RxCtrlDrop"),
	MIB_DESC(1, STATS_RDPC_ING, "RxIngDrop"),
	MIB_DESC(1, STATS_RDPC_ARL, "RxARLDrop")
};

enum {
	/* Global attributes. */
	MT753X_ATTR_ENABLE_VLAN,
};

static int __mt753x_set_vlan_enable(u32 enable)
{
	gsw.global_vlan_enable = enable;

	if(ecnt_global_vlan_enable_set)
		ecnt_global_vlan_enable_set(enable);

	return 0;
}

static int mt753x_get_vlan_enable(struct switch_dev *dev,
				  const struct switch_attr *attr,
				  struct switch_val *val)
{
	val->value.i = gsw.global_vlan_enable;

	return 0;
}

static int mt753x_set_vlan_enable(struct switch_dev *dev,
				  const struct switch_attr *attr,
				  struct switch_val *val)
{
	__mt753x_set_vlan_enable(val->value.i);

	return 0;
}

static int mt753x_get_port_pvid(struct switch_dev *dev, int lan_port, int *val)
{
	int port;

	if(lan_port != MT753X_CPU_PORT) {
	    port = lan_port_map[lan_port];
		if(port == 0xff)
			return 0;
	} else
	 	port = lan_port;
	
	if (port >= MT753X_NUM_PORTS)
		return -EINVAL;

	*val = regRead32(PPBV1(port));
	*val &= GRP_PORT_VID_M;

	return 0;
}

static int mt753x_set_port_pvid(struct switch_dev *dev, int lan_port, int pvid)
{
	int port;
	
	if(lan_port != MT753X_CPU_PORT) {
	    port = lan_port_map[lan_port];
		if(port == 0xff)
			return 0;
	} else
	 	port = lan_port;
	
	if (port >= MT753X_NUM_PORTS)
		return -EINVAL;

	if (pvid < MT753X_MIN_VID || pvid > MT753X_MAX_VID)
		return -EINVAL;

	gsw.port_entries[port].pvid = pvid;
	
	return 0;
}

void mt753x_vlan_ctrl(u32 cmd, u32 val)
{
	int i;

	regWrite32(VTCR,
			 VTCR_BUSY | ((cmd << VTCR_FUNC_S) & VTCR_FUNC_M) |
			 (val & VTCR_VID_M));

	for (i = 0; i < 300; i++) {
		u32 val = regRead32(VTCR);

		if ((val & VTCR_BUSY) == 0)
			break;

		usleep_range(1000, 1100);
	}

	if (i == 300)
		printk("vtcr timeout\n");
}

static void mt753x_write_vlan_entry(int vlan, u16 vid,
				    u8 ports, u8 etags)
{
	int port;
	u32 val;
		
	/* vlan port membership */
	if (ports)
		regWrite32(VAWD1,
				 IVL_MAC | VTAG_EN | VENTRY_VALID |
				 ((ports << PORT_MEM_S) & PORT_MEM_M));
	else
		regWrite32(VAWD1, 0);

	/* egress mode */
	val = 0;
	for (port = 0; port < MT753X_NUM_PORTS; port++) {
		if (etags & BIT(port))
			val |= ETAG_CTRL_TAG << PORT_ETAG_S(port);
		else
			val |= ETAG_CTRL_UNTAG << PORT_ETAG_S(port);
	}
	regWrite32(VAWD2, val);

	/* write to vlan table */
	mt753x_vlan_ctrl(VTCR_WRITE_VLAN_ENTRY, vid);
}

void mt753x_apply_vlan_config(void)
{
	int i, j;
	u8 tag_ports;
	u8 untag_ports;
	u32 val = 0;

	/* set all ports as security mode */
	for (i = 0; i < MT753X_NUM_PORTS; i++)
		regWrite32(PCR(i),
				 PORT_MATRIX_M | SECURITY_MODE);

	regWrite32(MFC, 0xffffffe0);

	/* check if a port is used in tag/untag vlan egress mode */
	tag_ports = 0;
	untag_ports = 0;

	for (i = 0; i < MT753X_NUM_VLANS; i++) {
		u8 member = gsw.vlan_entries[i].member;
		u8 etags = gsw.vlan_entries[i].etags;

		if (!member)
			continue;

		for (j = 0; j < MT753X_NUM_PORTS; j++) {
			if (!(member & BIT(j)))
				continue;

			if (etags & BIT(j))
				tag_ports |= 1u << j;
			else
				untag_ports |= 1u << j;
		}
	}

	/* enable cpu mac address learning */
	val = regRead32(PSC(MT753X_CPU_PORT));
	val &= ~(1 << SA_DIS);
	regWrite32(PSC(MT753X_CPU_PORT), val);

	/* set all untag-only ports as transparent and the rest as user port */
	for (i = 0; i < MT753X_NUM_PORTS; i++) {
		u32 pvc_mode = 0x8100 << STAG_VPID_S;

		if (untag_ports & BIT(i) && !(tag_ports & BIT(i)))
			pvc_mode = (0x8100 << STAG_VPID_S) |
				(VA_TRANSPARENT_PORT << VLAN_ATTR_S);

#if defined(TCSUPPORT_SEPERATED_ETH_ITF)
		if (i == MT753X_CPU_PORT)
			pvc_mode = (0x8100 << STAG_VPID_S) | DIS_PVID |
					   (1 << PVC_EG_TAG_S) | PVC_PORT_STAG;
#endif
		regWrite32(PVC(i), pvc_mode);
	}

	/* first clear the switch vlan table */
	for (i = 0; i < MT753X_NUM_VLANS; i++)
		mt753x_write_vlan_entry(i, i, 0, 0);

	/* now program only vlans with members to avoid
	 * clobbering remapped entries in later iterations
	 */
	for (i = 0; i < MT753X_NUM_VLANS; i++) {
		u16 vid = gsw.vlan_entries[i].vid;
		u8 member = gsw.vlan_entries[i].member;
		u8 etags = gsw.vlan_entries[i].etags;

		if (member)
			mt753x_write_vlan_entry(i, vid, member, etags);
	}

	/* Port Default PVID */
	for (i = 0; i < MT753X_NUM_PORTS; i++) {
		int vlan = gsw.port_entries[i].pvid;
		u16 pvid = 0;
		u32 val;

		if (vlan < MT753X_NUM_VLANS && gsw.vlan_entries[vlan].member)
			pvid = gsw.vlan_entries[vlan].vid;

		val = regRead32(PPBV1(i));
		val &= ~GRP_PORT_VID_M;
		val |= pvid;
		regWrite32(PPBV1(i), val);
	}
}

struct mt753x_mapping *mt753x_find_mapping(const char *map)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(mt753x_def_mapping); i++)
		if (!strcmp(map, mt753x_def_mapping[i].name))
			return &mt753x_def_mapping[i];

	return NULL;
}

void mt753x_apply_mapping(struct mt753x_mapping *map)
{
	int i = 0;

	for (i = 0; i <= MT753X_NUM_PORTS; i++)
		gsw.port_entries[i].pvid = map->pvids[i];

	for (i = 0; i < MT753X_NUM_VLANS; i++) {
		gsw.vlan_entries[i].member = map->members[i];
		gsw.vlan_entries[i].etags = map->etags[i];
		gsw.vlan_entries[i].vid = map->vids[i];
	}
}

static int mt753x_get_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	u32 member;
	u32 etags;
	int i;

	val->len = 0;

	if (val->port_vlan < 0 || val->port_vlan >= MT753X_NUM_VLANS)
		return -EINVAL;

	mt753x_vlan_ctrl(VTCR_READ_VLAN_ENTRY, val->port_vlan);

	member = regRead32(VAWD1);
	member &= PORT_MEM_M;
	member >>= PORT_MEM_S;

	etags = regRead32(VAWD2);

	for (i = 0; i < MT753X_NUM_PORTS; i++) {
		struct switch_port *p;
		int etag;

		if (!(member & BIT(i)))
			continue;

		if((switch_port_map[i] == 0xff) && (i != MT753X_CPU_PORT))
			continue;

		p = &val->value.ports[val->len++];

		if(i != MT753X_CPU_PORT)
			p->id = switch_port_map[i];
		else
		 	p->id = i;

		etag = (etags >> PORT_ETAG_S(i)) & PORT_ETAG_M;

		if (etag == ETAG_CTRL_TAG)
			p->flags |= BIT(SWITCH_PORT_FLAG_TAGGED);
		else if (etag != ETAG_CTRL_UNTAG)
			printk("vlan egress tag control neither untag nor tag.\n");
	}

	return 0;
}

static int mt753x_set_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	u8 member = 0;
	u8 etags = 0;
	int i;

	if (val->port_vlan < 0 || val->port_vlan >= MT753X_NUM_VLANS ||
	    val->len > MT753X_NUM_PORTS)
		return -EINVAL;

	for (i = 0; i < val->len; i++) {
		struct switch_port *p = &val->value.ports[i];

		if(p->id != MT753X_CPU_PORT) {
			p->id = lan_port_map[p->id];
			if(p->id == 0xff)
				continue;
		} else
		 	;

		if (p->id >= MT753X_NUM_PORTS)
			return -EINVAL;

		member |= BIT(p->id);

		if (p->flags & BIT(SWITCH_PORT_FLAG_TAGGED))
			etags |= BIT(p->id);
	}

	gsw.vlan_entries[val->port_vlan].member = member;
	gsw.vlan_entries[val->port_vlan].etags = etags;

	return 0;
}

static int mt753x_set_vid(struct switch_dev *dev,
			  const struct switch_attr *attr,
			  struct switch_val *val)
{
	int vlan;
	u16 vid;

	vlan = val->port_vlan;
	vid = (u16)val->value.i;

	if (vlan < 0 || vlan >= MT753X_NUM_VLANS)
		return -EINVAL;

	if (vid < MT753X_MIN_VID || vid > MT753X_MAX_VID)
		return -EINVAL;

	gsw.vlan_entries[vlan].vid = vid;

	return 0;
}

static int mt753x_get_vid(struct switch_dev *dev,
			  const struct switch_attr *attr,
			  struct switch_val *val)
{
	val->value.i = val->port_vlan;
	return 0;
}

static int mt753x_get_port_link(struct switch_dev *dev, int lan_port,
				struct switch_port_link *link)
{
	u32 speed, pmsr, port;

	if(lan_port != MT753X_CPU_PORT) {
	    port = lan_port_map[lan_port];
		if(port == 0xff)
			return 0;
	} else
	 	port = lan_port;
	
	if (port < 0 || port >= MT753X_NUM_PORTS)
		return -EINVAL;

	pmsr = regRead32(PMSR(port));

	link->link = pmsr & MAC_LNK_STS;
	link->duplex = pmsr & MAC_DPX_STS;
	speed = (pmsr & MAC_SPD_STS_M) >> MAC_SPD_STS_S;

	switch (speed) {
	case MAC_SPD_10:
		link->speed = SWITCH_PORT_SPEED_10;
		break;
	case MAC_SPD_100:
		link->speed = SWITCH_PORT_SPEED_100;
		break;
	case MAC_SPD_1000:
		link->speed = SWITCH_PORT_SPEED_1000;
		break;
	case MAC_SPD_2500:
		link->speed = SWITCH_PORT_SPEED_UNKNOWN;
		break;
	}

	return 0;
}

static int mt753x_set_port_link(struct switch_dev *dev, int lan_port,
				struct switch_port_link *link)
{
	u8 port;
	
	if(lan_port != MT753X_CPU_PORT) {
	    port = lan_port_map[lan_port];
		if(port == 0xff)
			return 0;
	} else
	 	port = lan_port;

	if (port >= MT753X_NUM_PORTS)
		return -EINVAL;

	return switch_generic_set_link(dev, port, link);
}

static u64 get_mib_counter(int i, int lan_port)
{
	unsigned int offset, port;
	u64 lo, hi, hi2;

	if(lan_port != MT753X_CPU_PORT) {
	    port = lan_port_map[lan_port];
		if(port == 0xff)
			return 0;
	} else
	 	port = lan_port;
	
	offset = mt753x_mibs[i].offset;

	if (mt753x_mibs[i].size == 1)
		return regRead32(MIB_COUNTER_REG(port, offset));

	do {
		hi = regRead32(MIB_COUNTER_REG(port, offset + 4));
		lo = regRead32(MIB_COUNTER_REG(port, offset));
		hi2 = regRead32(MIB_COUNTER_REG(port, offset + 4));
	} while (hi2 != hi);

	return (hi << 32) | lo;
}

static int mt753x_get_port_mib(struct switch_dev *dev,
			       const struct switch_attr *attr,
			       struct switch_val *val)
{
	static char buf[4096];
	int i, len = 0;

	if (val->port_vlan >= MT753X_NUM_PORTS)
		return -EINVAL;

	len += snprintf(buf + len, sizeof(buf) - len,
			"Port %d MIB counters\n", val->port_vlan);

	for (i = 0; i < ARRAY_SIZE(mt753x_mibs); ++i) {
		u64 counter;

		len += snprintf(buf + len, sizeof(buf) - len,
				"%-11s: ", mt753x_mibs[i].name);
		counter = get_mib_counter(i, val->port_vlan);
		len += snprintf(buf + len, sizeof(buf) - len, "%llu\n",
				counter);
	}

	val->value.s = buf;
	val->len = len;

	return 0;
}

static int mt753x_get_port_stats(struct switch_dev *dev, int lan_port,
				 struct switch_port_stats *stats)
{
	int port;
	
	if(lan_port != MT753X_CPU_PORT) {
	    port = lan_port_map[lan_port];
		if(port == 0xff)
			return 0;
	} else
	 	port = lan_port;

	if (port < 0 || port >= MT753X_NUM_PORTS)
		return -EINVAL;

	stats->tx_bytes = get_mib_counter(MT753X_PORT_MIB_TXB_ID, port);
	stats->rx_bytes = get_mib_counter(MT753X_PORT_MIB_RXB_ID, port);

	return 0;
}

static void mt753x_port_isolation(void)
{
	int i;
	u32 val = 0;

	/* set all ports as matrix mode */
	for (i = 0; i < MT753X_NUM_PORTS; i++)
		regWrite32(PCR(i), PORT_MATRIX_M);

	regWrite32(MFC, 0x404040e0);

	for (i = 0; i < MT753X_NUM_PORTS; i++) {
		u32 pvc_mode = 0x8100 << STAG_VPID_S;

		if (i == MT753X_CPU_PORT)
			continue;
		
		pvc_mode |= (VA_TRANSPARENT_PORT << VLAN_ATTR_S);

		regWrite32(PVC(i), pvc_mode);
	}
}

static int mt753x_apply_config(struct switch_dev *dev)
{
	if (!gsw.global_vlan_enable) {
		mt753x_port_isolation();
		return 0;
	}

	mt753x_apply_vlan_config();

	return 0;
}

static int mt753x_reset_switch(struct switch_dev *dev)
{
	int i;

	memset(gsw.port_entries, 0, sizeof(gsw.port_entries));
	memset(gsw.vlan_entries, 0, sizeof(gsw.vlan_entries));

	/* set default vid of each vlan to the same number of vlan, so the vid
	 * won't need be set explicitly.
	 */
	for (i = 0; i < MT753X_NUM_VLANS; i++)
		gsw.vlan_entries[i].vid = i;

	return 0;
}

static int mt753x_phy_read16(struct switch_dev *dev, int addr, u8 reg,
			     u16 *value)
{	
	*value = ETHER_MDIO_READ(addr+8, reg);

	return 0;
}

static int mt753x_phy_write16(struct switch_dev *dev, int addr, u8 reg,
			      u16 value)
{	
	ETHER_MDIO_WRITE(addr+8, reg, value);

	return 0;
}

static const struct switch_attr mt753x_global[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "VLAN mode (1:enabled)",
		.max = 1,
		.id = MT753X_ATTR_ENABLE_VLAN,
		.get = mt753x_get_vlan_enable,
		.set = mt753x_set_vlan_enable,
	}
};

static const struct switch_attr mt753x_port[] = {
	{
		.type = SWITCH_TYPE_STRING,
		.name = "mib",
		.description = "Get MIB counters for port",
		.get = mt753x_get_port_mib,
		.set = NULL,
	},
};

static const struct switch_attr mt753x_vlan[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "vid",
		.description = "VLAN ID (0-4094)",
		.set = mt753x_set_vid,
		.get = mt753x_get_vid,
		.max = 4094,
	},
};

static const struct switch_dev_ops mt753x_swdev_ops = {

	.attr_global = {
		.attr = mt753x_global,
		.n_attr = ARRAY_SIZE(mt753x_global),
	},
	.attr_port = {
		.attr = mt753x_port,
		.n_attr = ARRAY_SIZE(mt753x_port),
	},
	.attr_vlan = {
		.attr = mt753x_vlan,
		.n_attr = ARRAY_SIZE(mt753x_vlan),
	},
	.get_vlan_ports = mt753x_get_vlan_ports,
	.set_vlan_ports = mt753x_set_vlan_ports,
	.get_port_pvid = mt753x_get_port_pvid,
	.set_port_pvid = mt753x_set_port_pvid,
	.get_port_link = mt753x_get_port_link,
	.set_port_link = mt753x_set_port_link,
	.get_port_stats = mt753x_get_port_stats,
	.apply_config = mt753x_apply_config,
	.reset_switch = mt753x_reset_switch,
	.phy_read16 = mt753x_phy_read16,
	.phy_write16 = mt753x_phy_write16,	
};

int mt753x_swconfig_init(void)
{
	struct mt753x_mapping *map;
	const char *def_map = "lllll";
	int ret;

	ret = regRead32(SYS_CREV);
	gsw.swdev.name = ((ret & 0xFFFF0000) == 0x75300000) ? "MT7530" : NULL;
	gsw.swdev.alias = gsw.swdev.name;
	gsw.swdev.cpu_port = MT753X_CPU_PORT;
	gsw.swdev.ports = MT753X_NUM_PORTS;
	gsw.swdev.vlans = MT753X_NUM_VLANS;
	gsw.swdev.ops = &mt753x_swdev_ops;
#if defined(TCSUPPORT_SEPERATED_ETH_ITF)
	gsw.global_vlan_enable = 0;
#else
	gsw.global_vlan_enable = 1;
#endif
	__mt753x_set_vlan_enable(gsw.global_vlan_enable);

	ret = register_switch(&gsw.swdev, NULL);
	if (ret) {
		printk("Failed to register switch %s\n",gsw.swdev.name);
		return ret;
	}

	if(gsw.global_vlan_enable) {
		map = mt753x_find_mapping(def_map);
		if (map) {
			mt753x_apply_mapping(map);
			printk("LAN/WAN VLAN setting=%s\n", map->name);
		}
	}

	mt753x_apply_config(&gsw.swdev);
	
	return 0;
}

void mt753x_swconfig_destroy(void)
{
	unregister_switch(&gsw.swdev);
}

module_init(mt753x_swconfig_init);
module_exit(mt753x_swconfig_destroy);
MODULE_LICENSE("GPL");
