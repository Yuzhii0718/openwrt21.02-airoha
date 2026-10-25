#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/trace_seq.h>
#include <linux/seq_file.h>
#include <linux/u64_stats_sync.h>
#include <linux/dma-mapping.h>
#include <linux/netdevice.h>
#include <linux/ctype.h>
#include <linux/mii.h>
#include <linux/mdio.h>
#include <linux/phy.h>
#include <linux/spinlock.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include "air_en8811h.h"
#include "air_en8811h_api.h"

extern spinlock_t en8811_phy_lock;
static struct air_lp_speed air_lp_ability_map[] = {
    { MII_STAT1000  ,  AIR_SPEED_1000, 10 },
    { MDIO_MMD_DTEXS,   AIR_SPEED_100,  7 },
};

volatile static TrRG_T _fldRW_tr_reg__EcVarTrainingGain_ECNC_C8h   = { .TrRGDesc.DescVal =0x81900302, .RgMask =   0x0000000C};
volatile static TrRG_T _fldRW_tr_reg__EcVarTrainingTime_ECNC_C8h   = { .TrRGDesc.DescVal =0x81900F04, .RgMask =   0x0000FFF0};

/* Airoha MII read function */
int __air_mii_cl22_read(struct mii_bus *ebus, int addr,unsigned int phy_register)
{
    return ETHER_MDIO_READ(addr, phy_register);
}
/* Airoha MII write function */
int __air_mii_cl22_write(struct mii_bus *ebus, int addr, unsigned int phy_register,unsigned int write_data)
{
    ETHER_MDIO_WRITE(addr, phy_register, write_data);
    return 0;
}
/* Airoha MII read function */
int air_mii_cl22_read(struct mii_bus *ebus, int addr,unsigned int phy_register)
{
    int read_data;
    
    spin_lock_bh(&en8811_phy_lock);
    read_data = __air_mii_cl22_read(ebus, addr, phy_register);
    spin_unlock_bh(&en8811_phy_lock);
    return read_data;
}
/* Airoha MII write function */
int air_mii_cl22_write(struct mii_bus *ebus, int addr, unsigned int phy_register,unsigned int write_data)
{
    spin_lock_bh(&en8811_phy_lock);
    __air_mii_cl22_write(ebus, addr, phy_register, write_data);
    spin_unlock_bh(&en8811_phy_lock);
    return 0;
}
int __air_mii_cl45_read(struct phy_device *phydev, int devad, u16 reg)
{
    int data;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    __air_mii_cl22_write(mbus, addr, MII_MMD_ACC_CTL_REG, devad);
    __air_mii_cl22_write(mbus, addr, MII_MMD_ADDR_DATA_REG, reg);
    __air_mii_cl22_write(mbus, addr, MII_MMD_ACC_CTL_REG, MMD_OP_MODE_DATA | devad);
    data = __air_mii_cl22_read(mbus, addr, MII_MMD_ADDR_DATA_REG);
    return data;
}

int air_mii_cl45_read(struct phy_device *phydev, int devad, u16 reg)
{
    int ret = 0;
    int data;
    struct device *dev = phydev_dev(phydev);

    spin_lock_bh(&en8811_phy_lock);
    data = __air_mii_cl45_read(phydev, devad, reg);
    spin_unlock_bh(&en8811_phy_lock);
    return data;
}

int __air_mii_cl45_write(struct phy_device *phydev, int devad, u16 reg, u16 write_data)
{
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    __air_mii_cl22_write(mbus, addr, MII_MMD_ACC_CTL_REG, devad);
    __air_mii_cl22_write(mbus, addr, MII_MMD_ADDR_DATA_REG, reg);
    __air_mii_cl22_write(mbus, addr, MII_MMD_ACC_CTL_REG, MMD_OP_MODE_DATA | devad);
    __air_mii_cl22_write(mbus, addr, MII_MMD_ADDR_DATA_REG, write_data);
    return 0;
}

int air_mii_cl45_write(struct phy_device *phydev, int devad, u16 reg, u16 write_data)
{
    spin_lock_bh(&en8811_phy_lock);
    __air_mii_cl45_write(phydev, devad, reg, write_data);
    spin_unlock_bh(&en8811_phy_lock);
    return 0;
}

unsigned long air_pbus_reg_read(struct phy_device *phydev, unsigned long pbus_address)
{
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);
    unsigned long pubs_data;
    unsigned int pbus_data_low, pbus_data_high;

    spin_lock_bh(&en8811_phy_lock);
    __air_mii_cl22_write(mbus, (addr + 8), 0x1F,(pbus_address >> 6));
    pbus_data_low = __air_mii_cl22_read(mbus, (addr + 8), ((pbus_address >> 2) & 0xf));
    pbus_data_high = __air_mii_cl22_read(mbus, (addr + 8), 0x10);
    pubs_data = (pbus_data_high << 16) + pbus_data_low;
    spin_unlock_bh(&en8811_phy_lock);
    return pubs_data;
}

int air_pbus_reg_write(struct phy_device *phydev, unsigned long pbus_address, unsigned long pbus_data)
{
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    spin_lock_bh(&en8811_phy_lock);
    __air_mii_cl22_write(mbus, (addr + 8), 0x1F, (unsigned int)(pbus_address >> 6));
    __air_mii_cl22_write(mbus, (addr + 8), (unsigned int)((pbus_address >> 2) & 0xf), (unsigned int)(pbus_data & 0xFFFF));
    __air_mii_cl22_write(mbus, (addr + 8), 0x10, (unsigned int)(pbus_data >> 16));
    spin_unlock_bh(&en8811_phy_lock);
    return 0;
}

/* EN8811H BUCK read function */
unsigned int __air_buckpbus_reg_read(struct phy_device *phydev, unsigned long pbus_address)
{
    unsigned int pbus_data = 0, pbus_data_low, pbus_data_high;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    __air_mii_cl22_write(mbus,addr, 0x1F, (unsigned int)4);        /* page 4 */
    __air_mii_cl22_write(mbus,addr, 0x10, (unsigned int)0);
    __air_mii_cl22_write(mbus,addr, 0x15, (unsigned int)((pbus_address >> 16) & 0xffff));
    __air_mii_cl22_write(mbus,addr, 0x16, (unsigned int)(pbus_address & 0xffff));
    pbus_data_high = __air_mii_cl22_read(mbus,addr, 0x17);
    pbus_data_low = __air_mii_cl22_read(mbus,addr, 0x18);
    pbus_data = (pbus_data_high << 16) + pbus_data_low;
    __air_mii_cl22_write(mbus,addr, 0x1F, 0);
    return pbus_data;
}

unsigned int air_buckpbus_reg_read(struct phy_device *phydev, unsigned long pbus_address)
{
    unsigned int pbus_data = 0, pbus_data_low, pbus_data_high;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);
    
    spin_lock_bh(&en8811_phy_lock);
    pbus_data = __air_buckpbus_reg_read(phydev, pbus_address);
    spin_unlock_bh(&en8811_phy_lock);
    return pbus_data;
}

int air_buckpbus_reg_write(struct phy_device *phydev, unsigned long pbus_address, unsigned int pbus_data)
{
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    spin_lock_bh(&en8811_phy_lock);
    __air_mii_cl22_write(mbus, addr, 0x1F, (unsigned int)4);        /* page 4 */
    __air_mii_cl22_write(mbus, addr, 0x10, (unsigned int)0);
    __air_mii_cl22_write(mbus, addr, 0x11, (unsigned int)((pbus_address >> 16) & 0xffff));
    __air_mii_cl22_write(mbus, addr, 0x12, (unsigned int)(pbus_address & 0xffff));
    __air_mii_cl22_write(mbus, addr, 0x13, (unsigned int)((pbus_data >> 16) & 0xffff));
    __air_mii_cl22_write(mbus, addr, 0x14, (unsigned int)(pbus_data & 0xffff));
    __air_mii_cl22_write(mbus, addr, 0x1F, 0);
    spin_unlock_bh(&en8811_phy_lock);
    return 0;
}

static void
air_token_ring_write(
    struct phy_device *phydev,
    TrRG_T TrRG,
    unsigned int WrtVal)
{
    int data;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);
    unsigned int TmpVal;
    HAL_TR_DATA_T TR_DATA;
    struct device *dev = phydev_dev(phydev);

    spin_lock_bh(&en8811_phy_lock);
    data = __air_mii_cl45_read(phydev, 0x1e, 0x148);
    AIR_EN8811H_SET_VALUE(data, 1, 9, 1);
    __air_mii_cl45_write(phydev, 0x1e, 0x148, data);
    /* change page to 0x52b5 */
    __air_mii_cl22_write(mbus, addr, 0x1F, 0x52B5);

    /* write addr */
    TrRG.TrRGDesc.param.WR_RD_CTRL = 1;
    __air_mii_cl22_write(mbus, addr, 0x10, TrRG.TrRGDesc.Raw[1]);
    /* read data */
    TR_DATA.data_hi = __air_mii_cl22_read(mbus, addr, 0x12);
    TR_DATA.data_lo = __air_mii_cl22_read(mbus, addr, 0x11);
    TmpVal = ((((unsigned int)TR_DATA.data_hi)<<16) + (unsigned int)TR_DATA.data_lo) & ~TrRG.RgMask;

    TrRG.TrRGDesc.param.WR_RD_CTRL = 0;
    WrtVal = (WrtVal<<TrRG.TrRGDesc.param.TrRG_LSB) | (TmpVal & ~TrRG.RgMask);
    /* write data */
    __air_mii_cl22_write(mbus, addr, 0x12, (WrtVal>>16)&0x00FF);
    __air_mii_cl22_write(mbus, addr, 0x11, WrtVal&0xFFFF);
    __air_mii_cl22_write(mbus, addr, 0x10, TrRG.TrRGDesc.Raw[1]);

    /* change page to 0 */
    __air_mii_cl22_write(mbus, addr, 0x1F, 0x0);

    data = __air_mii_cl45_read(phydev, 0x1E, 0x148);
    AIR_EN8811H_SET_VALUE(data, 0, 0x9, 0x1);
    __air_mii_cl45_write(phydev, 0x1E, 0x148, data);
    spin_unlock_bh(&en8811_phy_lock);
}

int en8811h_get_autonego(struct phy_device *phydev, int *an)
{
    int reg;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    reg = air_mii_cl22_read(mbus, addr, MII_BMCR);
    if (reg < 0)
        return -EINVAL;
    if (reg & BMCR_ANENABLE)
        *an = AUTONEG_ENABLE;
    else
        *an = AUTONEG_DISABLE;
    return 0;
}

int air_ref_clk_speed(struct phy_device *phydev)
{
    struct device *dev = phydev_dev(phydev);
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev), ret;

    spin_lock_bh(&en8811_phy_lock);
    ret = __air_mii_cl22_write(mbus, addr, 0x1f, 0x0);
    /* Get real speed from vendor register */
    ret = __air_mii_cl22_read(mbus, addr, AIR_AUX_CTRL_STATUS);
    if (ret < 0)
        goto unlock;
    switch (ret & AIR_AUX_CTRL_STATUS_SPEED_MASK) {
    case AIR_AUX_CTRL_STATUS_SPEED_2500:
        phydev->speed = SPEED_2500;
        break;
    case AIR_AUX_CTRL_STATUS_SPEED_1000:
        phydev->speed = SPEED_1000;
        break;
    case AIR_AUX_CTRL_STATUS_SPEED_100:
        phydev->speed = SPEED_100;
        break;
    default:
        phydev->speed = SPEED_2500;
        dev_err(dev, "Default SPEED: 0x%x\n", ret);
        break;
    }
unlock:
    spin_unlock_bh(&en8811_phy_lock);
    return ret;
}

int en8811h_read_status(struct phy_device *phydev)
{
    int ret = 0, reg = 0;
    u32 pbus_value = 0;
    struct device *dev = phydev_dev(phydev);
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    reg = air_mii_cl22_read(mbus, addr, MII_BMSR);
    if (reg < 0) {
        dev_err(dev, "MII_BMSR reg %d!\n", reg);
        ret = reg;
        goto error;
    }
    reg = air_mii_cl22_read(mbus, addr, MII_BMSR);
    if (reg < 0) {
        dev_err(dev, "MII_BMSR reg %d!\n", reg);
        ret = reg;
        goto error;
    }
    if (reg & BMSR_LSTATUS) {
        phydev->link = 1;
        ret = air_ref_clk_speed(phydev);
        if (ret < 0) {
            dev_err(dev, "air_ref_clk_speed fail: %d!\n", reg);
            goto error;
        }
        reg = air_mii_cl22_read(mbus,
                    addr, MII_ADVERTISE);
        if (reg < 0) {
            ret = reg;
            goto error;
        }
        phydev->pause = GET_BIT(reg, 10);
        phydev->asym_pause = GET_BIT(reg, 11);
    }
    else
    {
        phydev->link = 0;
        phydev->speed = SPEED_UNKNOWN;
        phydev->duplex = DUPLEX_UNKNOWN;
        phydev->pause = 0;
        phydev->asym_pause = 0;
    }
    phydev->duplex = DUPLEX_FULL;
    return 0;

error:
    phydev->speed = SPEED_UNKNOWN;
    phydev->duplex = DUPLEX_UNKNOWN;
    phydev->pause = 0;
    phydev->asym_pause = 0;

    return ret;
}


int airphy_set_mode(struct phy_device *phydev, AIR_PORT_MODE_T dbg_mode)
{
    int ret = 0, val = 0;
    unsigned int pbus_data = 0;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    switch(dbg_mode)
    {
        case AIR_PORT_MODE_FORCE_100:
            pr_notice("\nForce 100M\n");
            val = air_mii_cl22_read(mbus, addr, MII_ADVERTISE) | BIT(8);
            ret = air_mii_cl22_write(mbus, addr, MII_ADVERTISE, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl22_read(mbus, addr, MII_CTRL1000) & ~BIT(9);
            ret = air_mii_cl22_write(mbus, addr, MII_CTRL1000, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl45_read(phydev, 0x7, 0x20) & ~BIT(7);
            ret = air_mii_cl45_write(phydev, 0x7, 0x20, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl22_read(mbus, addr, MII_BMCR) | BIT(9);
            ret = air_mii_cl22_write(mbus, addr, MII_BMCR, val);
            AIR_RTN_ERR(ret);
            break;
        case AIR_PORT_MODE_FORCE_1000:
            pr_notice("\nForce 1000M\n");
            val = air_mii_cl22_read(mbus, addr, MII_ADVERTISE) & ~BIT(8);
            ret = air_mii_cl22_write(mbus, addr, MII_ADVERTISE, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl22_read(mbus, addr, MII_CTRL1000) | BIT(9);
            ret = air_mii_cl22_write(mbus, addr, MII_CTRL1000, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl45_read(phydev, 0x7, 0x20) & ~BIT(7);
            ret = air_mii_cl45_write(phydev, 0x7, 0x20, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl22_read(mbus, addr, MII_BMCR) | BIT(9);
            ret = air_mii_cl22_write(mbus, addr, MII_BMCR, val);
            AIR_RTN_ERR(ret);
            break;
        case AIR_PORT_MODE_FORCE_2500:
            pr_notice("\nForce 2500M\n");
            val = air_mii_cl22_read(mbus, addr, MII_ADVERTISE) & ~BIT(8);
            ret = air_mii_cl22_write(mbus, addr, MII_ADVERTISE, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl22_read(mbus, addr, MII_CTRL1000) & ~BIT(9);
            ret = air_mii_cl22_write(mbus, addr, MII_CTRL1000, val);
            AIR_RTN_ERR(ret);
            val =air_mii_cl45_read(phydev, 0x7, 0x20) | BIT(7);
            ret = air_mii_cl45_write(phydev, 0x7, 0x20, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl22_read(mbus, addr, MII_BMCR) | BIT(9);
            ret = air_mii_cl22_write(mbus, addr, MII_BMCR, val);
            AIR_RTN_ERR(ret);
            break;
        case AIR_PORT_MODE_AUTONEGO:
            pr_notice("\nAutonego mode\n");
            val = air_mii_cl22_read(mbus, addr, MII_ADVERTISE) | BIT(8);
            ret = air_mii_cl22_write(mbus, addr, MII_ADVERTISE, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl22_read(mbus, addr, MII_CTRL1000) | BIT(9);
            ret = air_mii_cl22_write(mbus, addr, MII_CTRL1000, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl45_read(phydev, 0x7, 0x20) | BIT(7);
            ret = air_mii_cl45_write(phydev, 0x7, 0x20, val);
            AIR_RTN_ERR(ret);
            val = air_mii_cl22_read(mbus, addr, MII_BMCR) | BIT(9);
            ret = air_mii_cl22_write(mbus, addr, MII_BMCR,val );
            AIR_RTN_ERR(ret);
            break;
        case AIR_PORT_MODE_POWER_DOWN:
            pr_notice("\nPower Down\n");
            val = air_mii_cl22_read(mbus, addr, MII_BMCR) | BIT(11);
            ret = air_mii_cl22_write(mbus, addr, MII_BMCR, val);
            AIR_RTN_ERR(ret);
            break;
        case AIR_PORT_MODE_POWER_UP:
            pr_notice("\nPower Up\n");
            val = air_mii_cl22_read(mbus, addr, MII_BMCR) & ~BIT(11);
            ret = air_mii_cl22_write(mbus, addr, MII_BMCR, val);
            AIR_RTN_ERR(ret);
            break;
        case AIR_PORT_MODE_SSC_DISABLE:
            pr_notice("\nHSGMII SSC Disabled\n");
            pbus_data = air_buckpbus_reg_read(phydev, 0xCA000);
            pbus_data &= ~BIT(21);
            ret = air_buckpbus_reg_write(phydev, 0xCA000, pbus_data);
            AIR_RTN_ERR(ret);
            break;
        case AIR_PORT_MODE_SSC_ENABLE:
            pr_notice("\nHSGMII SSC Enabled\n");
            pbus_data = air_buckpbus_reg_read(phydev, 0xCA000);
            pbus_data |= BIT(21);
            ret = air_buckpbus_reg_write(phydev, 0xCA000, pbus_data);
            AIR_RTN_ERR(ret);
            break;
        default:
            pr_notice("\nWrong Port mode\n");
            break;
    }
    return ret;
}

int airphy_fcm_counter_show(struct phy_device *phydev,
            char **buff, int *length)
{
    char *buf;
    int len = *length, ret = 0;
    u32 pkt_cnt = 0;
 
    buf = *buff;

    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "|\t<<FCM counter>>\n");
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Rx from Line side_S      :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe0090);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Rx from Line side_T      :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe0094);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Tx to System side_S      :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe009c);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Tx to System side_T      :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe00A0);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Rx from System side_S    :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe0078);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Rx from System side_T    :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe007C);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Tx to Line side_S        :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe0084);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Tx to Line side_T        :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe0088);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Pause from Line side     :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe0098);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Pause to System side     :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe00A4);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Pause from System side   :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe0080);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Pause to Line side       :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xe008c);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    /*Clear counter*/
    ret = air_buckpbus_reg_write(phydev, 0xe0074, 0x3);
    if (ret < 0)
        return ret;
    *length = len;
    return 0;
}

int airphy_ls_counter_show(struct phy_device *phydev,
            char **buff, int *length)
{
    char *buf;
    int len = *length, ret = 0;
    u32 pkt_cnt = 0;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    buf = *buff;

    len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "|\t<<LS counter>>\n");
    if (phydev->speed == SPEED_2500) {
        ret = air_buckpbus_reg_write(phydev, 0x30718, 0x10);
        if (ret < 0)
            return ret;
        ret = air_buckpbus_reg_write(phydev, 0x30718, 0x0);
        if (ret < 0)
            return ret;
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "|\tBefore EF\n");
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Tx to Line side_S        :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x3071C);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Tx to Line side_T        :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x30720);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Tx_ENC                   :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x30724);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Rx from Line side_S      :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x3072c);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Rx from Line side_T      :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x30730);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Rx_DEC                   :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x30728);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "|\tAfter EF\n");
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Tx to Line side_S        :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x30734);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Tx to Line side_T        :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x30738);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Rx from Line side_S      :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x30764);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Rx from Line side_T      :");
        pkt_cnt = air_buckpbus_reg_read(phydev, 0x30768);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n\n", pkt_cnt);
        ret = air_buckpbus_reg_write(phydev, 0x30718, 0x13);
        if (ret < 0)
            return ret;
        ret = air_buckpbus_reg_write(phydev, 0x30718, 0x3);
        if (ret < 0)
            return ret;
        ret = air_buckpbus_reg_write(phydev, 0x30718, 0x10);
        if (ret < 0)
            return ret;
        ret = air_buckpbus_reg_write(phydev, 0x30718, 0x0);
        if (ret < 0)
            return ret;
    } else if (phydev->speed != SPEED_2500) {
        ret = air_mii_cl22_write(mbus, addr, 0x1f, 0x1);
        if (ret < 0)
            return ret;
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Rx from Line side        :");
        pkt_cnt = air_mii_cl22_read(mbus, addr, 0x12) & ~BIT(15);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Rx Error from Line side  :");
        pkt_cnt = air_mii_cl22_read(mbus, addr, 0x17) & 0xff;
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        ret = air_mii_cl22_write(mbus, addr, 0x1f, 0x52b5);
        if (unlikely(ret < 0))
            return ret;
        ret = air_mii_cl22_write(mbus, addr, 0x10, 0xbf92);
        if (unlikely(ret < 0))
            return ret;
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Tx to Line side          :");
        pkt_cnt = (air_mii_cl22_read(mbus, addr, 0x11) & 0x7ffe) >> 1;
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n", pkt_cnt);
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "| Tx Error to Line side    :");
        pkt_cnt = air_mii_cl22_read(mbus, addr, 0x12) & 0x7f;
        len += snprintf(buf + len, BUFFER_LENGTH - len,
                    "%010u |\n\n", pkt_cnt);
        ret = air_mii_cl22_write(mbus, addr, 0x1f, 0x0);
        if (unlikely(ret < 0))
            return ret;
    }
    *length = len;
    return 0;
}
int airphy_mac_counter_show(struct phy_device *phydev,
            char **buff, int *length)
{
    char *buf;
    int len = *length;
    u32 pkt_cnt = 0;

    buf = *buff;
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "|\t<<MAC Counter>>\n");
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Tx Error from System side:");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0x131000);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Rx Error to System side  :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0x132000);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Tx from System side      :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0x131004);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Rx to System side        :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0x132004);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    *length = len;
    return 0;
}

int airphy_ss_counter_show(struct phy_device *phydev,
            char **buff, int *length)
{
    char *buf;
    int len = *length, ret = 0;
    u32 pkt_cnt = 0;

    buf = *buff;
    ret = air_buckpbus_reg_write(phydev, 0xC602C, 0x3);
        if (ret < 0)
            return ret;
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "|\t<<SS Counter>>\n");
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Tx Start                 :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xC60b0);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Tx Terminal              :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xC60b4);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Rx Start                 :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xC60bc);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Rx Terminal              :");
    pkt_cnt = air_buckpbus_reg_read(phydev, 0xC60c0);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010u |\n", pkt_cnt);
    ret = air_buckpbus_reg_write(phydev, 0xC602C, 0x4);
        if (ret < 0)
            return ret;
    *length = len;
    return 0;
}

int airphy_dbg_regs_show(struct phy_device *phydev,
            char **buff, int *length)
{
    char *buf;
    int len = *length;
    u32 data = 0;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    buf = *buff;
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_MII_BMCR             :");
    data = air_mii_cl22_read(mbus, addr, MII_BMCR);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_MII_BMSR             :");
    data = air_mii_cl22_read(mbus, addr, MII_BMSR);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_MII_ADVERTISE        :");
    data = air_mii_cl22_read(mbus, addr, MII_ADVERTISE);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_MII_LPA              :");
    data = air_mii_cl22_read(mbus, addr, MII_LPA);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_MII_CTRL1000         :");
    data = air_mii_cl22_read(mbus, addr, MII_CTRL1000);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_MII_STAT1000         :");
    data = air_mii_cl22_read(mbus, addr, MII_STAT1000);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_MII_2G5_ADV          :");
    data = air_mii_cl45_read(phydev, 0x7, 0x20);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_MII_2G5_LP           :");
    data = air_buckpbus_reg_read(phydev, 0x3b30);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_HW_STRAP1            :");
    data = air_buckpbus_reg_read(phydev, 0xcf910);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_HW_STRAP2            :");
    data = air_buckpbus_reg_read(phydev, 0xcf914);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_SYS_LINK_MODE        :");
    data = air_buckpbus_reg_read(phydev, 0xe0004);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_SS_PAUSE_TIME        :");
    data = air_buckpbus_reg_read(phydev, 0xe000c);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_CTRL_0               :");
    data = air_buckpbus_reg_read(phydev, 0xc0000);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_LINK_STATUS          :");
    data = air_buckpbus_reg_read(phydev, 0xc0b04);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_FN_PWR_CTRL_STATUS   :");
    data = air_buckpbus_reg_read(phydev, 0x1020c);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_MD32_FW_READY        :");
    data = air_mii_cl45_read(phydev, 0x1e, 0x8009);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| RG_WHILE_LOOP_COUNT     :");
    data = air_buckpbus_reg_read(phydev, 0x3a48);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "%010x |\n", data);
    *length = len;
    return 0;
}

int airphy_led_control(struct phy_device *phydev, int force_mode)
{
    int ret = 0, id, on_evt;
    struct en8811h_priv *priv = phydev->priv;

    for (id = 0; id < EN8811H_LED_COUNT; id++) {
        if (force_mode) {
            on_evt = air_mii_cl45_read(phydev, 0x1f,
					LED_ON_CTRL(id));
            on_evt &= ~(LED_ON_EVT_LINK_2500M |
				LED_ON_EVT_LINK_100M | LED_ON_EVT_LINK_1000M);
            if (force_mode == AIR_LED_FORCE_ON)
                on_evt |= LED_ON_EVT_FORCE;
            else
                on_evt &= ~LED_ON_EVT_FORCE;

            ret = air_mii_cl45_write(phydev, 0x1f,
					LED_ON_CTRL(id), on_evt);
            if (ret < 0)
                return ret;
            ret = air_mii_cl45_write(phydev, 0x1f,
					LED_BLK_CTRL(id), 0);
        } else {
            ret = air_mii_cl45_write(phydev, 0x1f,
					LED_ON_CTRL(id), priv->on_crtl[id]);
            ret |= air_mii_cl45_write(phydev, 0x1f,
					LED_BLK_CTRL(id), priv->blk_crtl[id]);
        }
        if (ret < 0)
            return ret;
    }
    return 0;
}

int air_get_autonego(struct phy_device *phydev, int *an)
{
    int reg;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    reg = air_mii_cl22_read(mbus, addr, MII_BMCR);
    if (reg < 0)
        return -EINVAL;
    if (reg & BMCR_ANENABLE)
        *an = AUTONEG_ENABLE;
    else
        *an = AUTONEG_DISABLE;
    return 0;
}

int air_read_lp_ability(struct phy_device *phydev)
{
    int ret = 0, val = 0, an = AUTONEG_DISABLE;
    int count = 30, i;
    struct air_lp_speed *m;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    val = air_mii_cl22_read(mbus, addr, MII_BMCR) | BIT(9);
    ret = air_mii_cl22_write(mbus, addr, MII_BMCR, val);
    if (unlikely(ret < 0))
        return ret;
    msleep(1000);
    do {
        msleep(100);
        val = air_mii_cl45_read(phydev, MDIO_MMD_AN, 0x21);
        val = (val & BIT(5)) >> 5;
        if (val) {
            return AIR_SPEED_2500;
        }
        count--;
    } while (count);

    val = air_mii_cl22_read(mbus, addr, MII_BMSR);
    if (val < 0) {
        return val;
    }
    val = air_mii_cl22_read(mbus, addr, MII_BMSR);
    if (val < 0) {
        return val;
    }
    if (val & BMSR_LSTATUS) {
        ret = air_get_autonego(phydev, &an);
        if (an == AUTONEG_ENABLE) {
            for (i = 0; i < ARRAY_SIZE(air_lp_ability_map); i++) {
                m = &air_lp_ability_map[i];
                val = air_mii_cl22_read(mbus, addr, m->rg);
                if (val < 0)
                    return val;
                val = (val & (0x3 << m->bit)) >> m->bit;
                if (val)
                    return m->speed;
            }
        }
    }

    return ret;
}

int airphy_temp_show(struct phy_device *phydev,
            char **buff, int *length)
{
    char *buf;
    int len = *length, ret = 0;
    u32 data = 0;

    buf = *buff;
    ret = air_mii_cl45_write(phydev, 0x1e, 0x800e, 0x1100);
            AIR_RTN_ERR(ret);
    ret = air_mii_cl45_write(phydev, 0x1e, 0x800f, 0xe5);
            AIR_RTN_ERR(ret);
    data = air_buckpbus_reg_read(phydev, 0x3b38);
    len += snprintf(buf + len, BUFFER_LENGTH - len,
                "| Temperature %02dC |\n", data);
    *length = len;
    return 0;
}

static void
air_cable_pair_swap(int pair0, int pair1,
    struct air_cable_test_rsl *ptr_cable)
{
    int tmp_status;
    unsigned int tmp_length;

    tmp_status = ptr_cable->status[pair0];
    tmp_length = ptr_cable->length[pair0];
    ptr_cable->status[pair0] = ptr_cable->status[pair1];
    ptr_cable->length[pair0] = ptr_cable->length[pair1];
    ptr_cable->status[pair1] = tmp_status;
    ptr_cable->length[pair1] = tmp_length;
}

int air_wait_md32_fw(struct phy_device *phydev)
{
    int ret = 0, reg;
    struct device *dev = phydev_dev(phydev);
    int retry = 60;

    do {
        msleep(1000);
        reg = air_mii_cl45_read(phydev, 0x1e, 0x8009);
        if (reg == EN8811H_PHY_READY) {
            dev_dbg(dev, "%s: PHY ready!\n", __func__);
            break;
        }
        if(!(retry % 10))
            dev_info(dev, "Waiting for MD32 FW ready....(%d)", reg);

        if (!retry) {
            dev_err(dev, "%s: MD32 FW is not ready.(Status: 0x%x)\n", 
                            __func__, reg);
            return -EPERM;
        }
    } while (retry--);
    return 0;
}

static int air_cable_normal(struct phy_device *phydev, int step_col_12, int step_col_13)
{
    int ret = 0, reg;
    struct device *dev = phydev_dev(phydev);
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);

    ret |= air_mii_cl22_write(mbus, addr, 0x1f, 0);
    reg = ((air_mii_cl22_read(mbus, addr, 0x11)  & BIT(12)) >> 12);
    if(!reg) {
        reg = air_mii_cl22_read(mbus,
                addr, MII_BMCR);
        ret |= air_mii_cl22_write(mbus,
                addr, MII_BMCR, reg | BMCR_PDOWN);
        ret |= air_mii_cl22_write(mbus,
                addr, MII_BMCR, reg & ~BMCR_PDOWN);
        dev_dbg(dev, "%s Power Down -> Power UP.\n", __func__);
        msleep(1000);
        ret |= air_mii_cl22_write(mbus, addr, 0x1f, 0x1);
        ret |= air_mii_cl22_write(mbus, addr, 0x18, 0x8000);
        ret |= air_wait_md32_fw(phydev);
        if (ret < 0) {
            dev_err(dev,
                "%s: cable normal 1-1 fail!\n", __func__);
            return -EPERM;
        }
    }
    ret |= air_mii_cl45_write(phydev, 0x1e, 0x800e, 0x1100);
    ret |= air_mii_cl45_write(phydev, 0x1e, 0x800f, 0xd4);
    ret |= air_wait_md32_fw(phydev);
    if (ret < 0) {
        dev_err(dev,
            "%s: cable normal 1-2 fail!\n", __func__);
        return -EPERM;
    }
    return 0;
}

static int airphy_cable_step2(struct phy_device *phydev)
{
    int ret = 0;
    int step_col_12 = 0;
    int step_col_13 = 0;
    struct device *dev = phydev_dev(phydev);

    ret = air_cable_normal(phydev, step_col_12, step_col_13);
    if (ret < 0) {
        dev_err(dev, "%s: air_cable_normal fail (ret=%d)\n",
                __func__, ret);
        return -EPERM;
    }
    step_col_12 = air_mii_cl45_read(phydev, 0x1e, 0x800c);
    step_col_13 = air_mii_cl45_read(phydev, 0x1e, 0x800d);
    dev_dbg(dev, "%s step_col_12 %d, step_col_13 %d\n",
                __func__, step_col_12, step_col_13);
    dev_dbg(dev, "%s: successfull\n", __func__);
    return 0;
}

static int airphy_cable_step4(struct phy_device *phydev, struct air_cable_test_rsl *cable_rsl)
{
    int reg = 0, step_col_12 = 0, step_col_13 = 0, status;
    struct device *dev = phydev_dev(phydev);
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev), test_pair = 0;
    unsigned int pbus_value = 0, cable_len, cable_info;
    int retry = MAX_RETRY, ret = 0;
    const char *status_str[] = {"Error", "Open", "Short", "Load"};
    const char *mode_str[] = {"Error", "100M", "1G", "2.5G", "X"};
    const char *link_str[] = {"Link-Down", "Link-Up"};
    const char *pair_str[] = {"A", "B", "C", "D"};

    msleep(1000);
    ret |= air_mii_cl45_write(phydev, 0x1e, 0x800e, 0x1100);
    ret |= air_mii_cl45_write(phydev, 0x1e, 0x800f, 0xFA);
    ret |= air_wait_md32_fw(phydev);
    if (ret < 0) {
        dev_err(dev,
            "%s: cable step4 fail!\n", __func__);
        return -EPERM;
    }
    ret |= air_mii_cl22_write(mbus, addr, 0x1f, 0x0);
    reg = ((air_mii_cl22_read(mbus, addr, 0x11)  & BIT(12)) >> 12);
    for (test_pair = 0; test_pair < 4; test_pair ++) {
        cable_info = air_buckpbus_reg_read(phydev, 0x11e0 + test_pair * 4);
        dev_dbg(dev, "pair%s cable_info(0x%x) 0x%x\n", pair_str[test_pair], 0x11e0 + test_pair * 4, pbus_value);
        cable_len = cable_info & 0xffff;
        status = cable_info >> 16;
        if (!status) {
            dev_dbg(dev, "Pair %d, term=%d, len=%d \n", test_pair, status, cable_len);
        } else {
        cable_rsl->status[test_pair] = status;
        cable_rsl->length[test_pair] = cable_len;
        dev_dbg(dev, "2.pair %s, status %d, cable_len %d\n",
                    pair_str[test_pair], cable_rsl->status[test_pair], cable_rsl->length[test_pair]);
        }
    }
    if(!reg) {
        dev_dbg(dev, "%s: air_cable_pair_swap\n", __func__);
        air_cable_pair_swap(AIR_PORT_CABLE_TEST_PAIR_A, AIR_PORT_CABLE_TEST_PAIR_B, cable_rsl);
        air_cable_pair_swap(AIR_PORT_CABLE_TEST_PAIR_C, AIR_PORT_CABLE_TEST_PAIR_D, cable_rsl);
    }
    ret |= air_mii_cl45_write(phydev, 0x1E, 0x800D, 0x0);
    if(ret < 0)
        return ret;
    dev_dbg(dev, "%s successfull\n", __func__);
    return 0;
}

int airphy_cable_step1(struct phy_device *phydev)
{
    int ret = 0, pair, len = 0, reg, retry;
    const char *link_str[] = {"X", "100M", "1G", "2.5G", "LinkDown"};
    struct device *dev = phydev_dev(phydev);
    unsigned int pbus_value = 0;
    struct air_cable_test_rsl cable_rsl = {0};
    char str_out[CMD_MAX_LENGTH] = {0};
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev), status;

    msleep(1000);
    ret |= air_mii_cl45_write(phydev, 0x1E, 0x800e, 0x1100);
    ret |= air_mii_cl45_write(phydev, 0x1E, 0x800f, 0xd6);
    if(ret < 0)
        return ret;
    retry = 5;
    do {
        status = air_wait_md32_fw(phydev);
        if(!retry) {
            dev_info(dev, "%s: md32 fw is not ready(%d)", __func__, status);
            return -EPERM;
        }
        retry--;
    } while (status);
    pbus_value = air_buckpbus_reg_read(phydev, 0x11fc);
    if ((pbus_value == 1) || (pbus_value == 3)) {
        dev_info(dev, "%s -> No Support!\n",
                    link_str[pbus_value]);
    } else if (pbus_value == 2) {
        dev_info(dev, "%s - Link-Up Mode!\n",
                    link_str[pbus_value]);
    } else if (pbus_value == 4) {
        dev_info(dev, "Link-Down Mode!\n");
    }
    if ((pbus_value == 2) || (pbus_value == 4)) {
        ret = airphy_cable_step2(phydev);
        if (ret < 0)
            goto phy_reset;
        ret = airphy_cable_step4(phydev, &cable_rsl);
        if (ret < 0)
            goto phy_reset;
    }
    dev_info(dev, "%7s %15s %15s %15s\n", "pair-a", "pair-b", "pair-c", "pair-d");
    dev_info(dev, "%7s %7s %7s %7s %7s %7s %7s %7s\n",
    "status", "length", "status", "length", "status", "length", "status", "length");
    for (pair = 0; pair < 4; pair++) {
        if (AIR_PORT_CABLE_STATUS_ERROR == cable_rsl.status[pair])
            len += snprintf(str_out + len, CMD_MAX_LENGTH-len, "%7s", "error");
        else if (AIR_PORT_CABLE_STATUS_OPEN == cable_rsl.status[pair])
            len += snprintf(str_out + len, CMD_MAX_LENGTH-len, "%7s", " open");
        else if (AIR_PORT_CABLE_STATUS_SHORT == cable_rsl.status[pair])
            len += snprintf(str_out + len, CMD_MAX_LENGTH-len, "%7s", " short");
        else if (AIR_PORT_CABLE_STATUS_NORMAL == cable_rsl.status[pair])
            len += snprintf(str_out + len, CMD_MAX_LENGTH-len, "%7s", "normal");

        len += snprintf(str_out + len, CMD_MAX_LENGTH-len, "  %3d.%dm ",
            cable_rsl.length[pair] / 10, cable_rsl.length[pair] % 10);
    }
    dev_info(dev, "%s", str_out);
    dev_info(dev, "%s air_cable_diag sucessfull.\n", __func__);

    return 0;
phy_reset:
    dev_info(dev, "%s phy_reset.\n", __func__);
    ret |= air_mii_cl22_write(mbus, addr, 0x1f, 0x0);
    reg = air_mii_cl22_read(mbus,
            addr, MII_BMCR);
    ret |= air_mii_cl22_write(mbus,
            addr, MII_BMCR, reg | BMCR_PDOWN);
    ret |= air_mii_cl22_write(mbus,
            addr, MII_BMCR, reg & ~BMCR_PDOWN);
    if(ret < 0)
        return ret;
    msleep(1000);
    dev_dbg(dev, "%s Power Down -> Power UP.\n", __func__);

    return 0;
}

int airphy_trigger_cable_diag(struct phy_device *phydev)
{
    int ret = 0;

    air_token_ring_write(phydev, _fldRW_tr_reg__EcVarTrainingTime_ECNC_C8h, 0x2);
    air_token_ring_write(phydev, _fldRW_tr_reg__EcVarTrainingGain_ECNC_C8h, 0x0);
    ret = airphy_cable_step1(phydev);
    air_token_ring_write(phydev, _fldRW_tr_reg__EcVarTrainingTime_ECNC_C8h, 0xf4);
    air_token_ring_write(phydev, _fldRW_tr_reg__EcVarTrainingGain_ECNC_C8h, 0x1);
    return ret;
}

int airphy_dump_ec_cable_diag(struct phy_device *phydev)
{
    int rv = 0;
    u32 PMEM_addr = 0;
    u32 PMEM_value = 0;
    short EC_COEF;
    u8 pair, Test_Cnt;
    struct device *dev = phydev_dev(phydev);

    dev_info(dev, "\r\n Cable Diag EC Dump \n");
    for(pair = 0; pair < 4; pair ++) {
        for(Test_Cnt = 0; Test_Cnt < 240; Test_Cnt++) { /* EC_FULL_TAPS = 240*/
            PMEM_addr = ((0x118000 + ((Test_Cnt >> 1) * 4)) + (0x200 * pair));
            PMEM_value = air_buckpbus_reg_read(phydev, PMEM_addr);
            EC_COEF = 0;
            if((Test_Cnt%2) == 0) {
                EC_COEF = (short)(PMEM_value & 0xffff);
            }
            else {
                EC_COEF = (short)((PMEM_value >> 16) & 0xffff);
            }
            dev_info(dev, "Pair%d: EC_COEF=  %d \n", pair, EC_COEF);
        }
    }
    dev_info(dev, "\r\nCable Diag EC Dump Finish \n");
    return rv;
}
