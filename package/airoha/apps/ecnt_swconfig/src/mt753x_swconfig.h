/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MT753X_SWCONFIG_H_
#define _MT753X_SWCONFIG_H_

#include <linux/bitops.h>
#include <linux/switch.h>
#include <ecnt_hook/ecnt_hook_ether.h>

#define MT753X_CPU_PORT	    6
#define MT753X_NUM_PORTS	7
#define MT753X_MAX_VID		4095
#define MT753X_MIN_VID		0
#define MT753X_NUM_VLANS	4096

#define MT753X_REG_BASE		0x1fb58000

/* Values of Egress TAG Control */
#define ETAG_CTRL_UNTAG			0
#define ETAG_CTRL_TAG			2

#define VTCR				0x90
#define VAWD1				0x94
#define VAWD2				0x98

#define SYS_CREV		0x7ffc
#define MFC			    0x10

/* Fields of VTCR */
#define VTCR_BUSY			BIT(31)
//#define IDX_INVLD			BIT(16)
#define VTCR_FUNC_S			12
#define VTCR_FUNC_M			0xf000
#define VTCR_VID_S			0
#define VTCR_VID_M			0xfff

/* Values of VTCR_FUNC */
#define VTCR_READ_VLAN_ENTRY		0
#define VTCR_WRITE_VLAN_ENTRY		1
#define VTCR_INVD_VLAN_ENTRY		2
#define VTCR_ENABLE_VLAN_ENTRY		3

/* VLAN entry fields */
/* VAWD1 */
#define PORT_STAG			BIT(31)
#define IVL_MAC				BIT(30)
#define EG_CON				BIT(29)
#define VTAG_EN				BIT(28)
#define PORT_MEM_S			16
#define PORT_MEM_M			0xff0000
#define VENTRY_VALID			BIT(0)

/* VAWD2 */
#define PORT_ETAG_S(p)			((p) * 2)
#define PORT_ETAG_M			0x03

#define PORT_CTRL_BASE			0x2000
#define PORT_CTRL_PORT_OFFSET		0x100
#define PORT_CTRL_REG(p, r)		(PORT_CTRL_BASE + \
					(p) * PORT_CTRL_PORT_OFFSET +  (r))
#define PCR(p)				PORT_CTRL_REG(p, 0x04)
#define PSC(p)				PORT_CTRL_REG(p, 0x0c)
#define PVC(p)				PORT_CTRL_REG(p, 0x10)
#define PPBV1(p)			PORT_CTRL_REG(p, 0x14)

/* Fields of PCR */
#define PORT_MATRIX_S			16
#define PORT_MATRIX_M			0xff0000

/* Values of PORT_VLAN */
#define PORT_MATRIX_MODE		0
#define SECURITY_MODE			3

/* Fields of PSC */
#define SA_DIS			4

/* Fields of PVC */
#define STAG_VPID_S			16
#define STAG_VPID_M			0xffff0000
#define DIS_PVID			BIT(15)
#define PVC_EG_TAG_S			8
#define PVC_EG_TAG_M			0x700
#define VLAN_ATTR_S			6
#define VLAN_ATTR_M			0xc0
#define PVC_PORT_STAG			BIT(5)

/* Values of VLAN_ATTR */
#define VA_USER_PORT			0
#define VA_TRANSPARENT_PORT		3

/* Fields of PPBV */
#define GRP_PORT_VID_S(g)		(((g) % 2) * 16)
#define GRP_PORT_VID_M			0xfff

#define PORT_MAC_CTRL_BASE		0x3000
#define PORT_MAC_CTRL_PORT_OFFSET	0x100
#define PORT_MAC_CTRL_REG(p, r)		(PORT_MAC_CTRL_BASE + \
					(p) * PORT_MAC_CTRL_PORT_OFFSET + (r))
#define PMCR(p)				PORT_MAC_CTRL_REG(p, 0x00)
#define PMSR(p)				PORT_MAC_CTRL_REG(p, 0x08)

/* Fields of PMSR */
#define MAC_SPD_STS_S			2
#define MAC_SPD_STS_M			0x0c
#define MAC_DPX_STS			BIT(1)
#define MAC_LNK_STS			BIT(0)

/* Values of MAC_SPD_STS */
#define MAC_SPD_10			0
#define MAC_SPD_100			1
#define MAC_SPD_1000			2
#define MAC_SPD_2500			3

#define MIB_COUNTER_BASE		0x4000
#define MIB_COUNTER_PORT_OFFSET		0x100
#define MIB_COUNTER_REG(p, r)		(MIB_COUNTER_BASE + \
					(p) * MIB_COUNTER_PORT_OFFSET + (r))
#define STATS_TDPC			0x00
#define STATS_TCRC			0x04
#define STATS_TUPC			0x08
#define STATS_TMPC			0x0C
#define STATS_TBPC			0x10
#define STATS_TCEC			0x14
#define STATS_TSCEC			0x18
#define STATS_TMCEC			0x1C
#define STATS_TDEC			0x20
#define STATS_TLCEC			0x24
#define STATS_TXCEC			0x28
#define STATS_TPPC			0x2C
#define STATS_TL64PC			0x30
#define STATS_TL65PC			0x34
#define STATS_TL128PC			0x38
#define STATS_TL256PC			0x3C
#define STATS_TL512PC			0x40
#define STATS_TL1024PC			0x44
#define STATS_TOC			0x48
#define STATS_RDPC			0x60
#define STATS_RFPC			0x64
#define STATS_RUPC			0x68
#define STATS_RMPC			0x6C
#define STATS_RBPC			0x70
#define STATS_RAEPC			0x74
#define STATS_RCEPC			0x78
#define STATS_RUSPC			0x7C
#define STATS_RFEPC			0x80
#define STATS_ROSPC			0x84
#define STATS_RJEPC			0x88
#define STATS_RPPC			0x8C
#define STATS_RL64PC			0x90
#define STATS_RL65PC			0x94
#define STATS_RL128PC			0x98
#define STATS_RL256PC			0x9C
#define STATS_RL512PC			0xA0
#define STATS_RL1024PC			0xA4
#define STATS_ROC			0xA8
#define STATS_RDPC_CTRL			0xB0
#define STATS_RDPC_ING			0xB4
#define STATS_RDPC_ARL			0xB8

#define MT753X_PORT_MIB_TXB_ID	18	/* TxByte */
#define MT753X_PORT_MIB_RXB_ID	37	/* RxByte */

extern char lan_port_map[];
extern char switch_port_map[];
extern unsigned int get_frame_engine_data(unsigned int reg);
extern void set_frame_engine_data(unsigned int reg, unsigned int val);
int __attribute__((weak)) ecnt_global_vlan_enable_set(int enable);

#define regRead32(reg)				get_frame_engine_data(reg + MT753X_REG_BASE)
#define regWrite32(reg, wdata)		set_frame_engine_data(reg + MT753X_REG_BASE, wdata)

struct mt753x_port_entry {
	u16	pvid;
};

struct mt753x_vlan_entry {
	u16	vid;
	u8	member;
	u8	etags;
};

struct mt753x_mapping {
	char	*name;
	u16	pvids[MT753X_NUM_PORTS];
	u8	members[MT753X_NUM_VLANS];
	u8	etags[MT753X_NUM_VLANS];
	u16	vids[MT753X_NUM_VLANS];
};

#define MIB_DESC(_s, _o, _n)   \
	{                       \
		.size = (_s),   \
		.offset = (_o), \
		.name = (_n),   \
	}

struct mt753x_mib_desc {
	unsigned int size;
	unsigned int offset;
	const char *name;
};

struct gsw_mt753x {
	struct switch_dev swdev;
	int global_vlan_enable;
	struct mt753x_vlan_entry vlan_entries[MT753X_NUM_VLANS];
	struct mt753x_port_entry port_entries[MT753X_NUM_PORTS];
}gsw;

#endif /* _MT753X_SWCONFIG_H_ */
