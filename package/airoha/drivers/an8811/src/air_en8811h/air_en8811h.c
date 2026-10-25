// SPDX-License-Identifier: GPL-2.0+

/* FILE NAME:  air_en8811h.c
 * PURPOSE:
 *      EN8811H phy driver for Linux
 * NOTES:
 *
 */

/* INCLUDE FILE DECLARATIONS
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/phy.h>
#include <linux/delay.h>
#include <linux/ethtool.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/firmware.h>
#include <linux/crc32.h>
#include <linux/spinlock.h>
#include <linux/of_mdio.h>
#include <linux/of_address.h>

#include "air_en8811h.h"

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
#define phydev_mdio_bus(_dev) (_dev->bus)
#define phydev_addr(_dev) (_dev->addr)
#define phydev_dev(_dev) (&_dev->dev)
#else
#define phydev_mdio_bus(_dev) (_dev->mdio.bus)
#define phydev_addr(_dev) (_dev->mdio.addr)
#define phydev_dev(_dev) (&_dev->mdio.dev)
#endif

MODULE_DESCRIPTION("Airoha EN8811H PHY Drivers");
MODULE_AUTHOR("Airoha");
MODULE_LICENSE("GPL");

struct phy_device *en8811_phy_dev = NULL;
spinlock_t en8811_phy_lock;
EXPORT_SYMBOL(en8811_phy_dev);
EXPORT_SYMBOL(en8811_phy_lock);

/*
GPIO5  <-> BASE_T_LED0,
GPIO4  <-> BASE_T_LED1,
GPIO3  <-> BASE_T_LED2,
*/
/* User-defined.B */
/*#define AIR_MD32_FW_CHECK*/
#define AIR_LED_SUPPORT
#ifdef AIR_LED_SUPPORT
static const AIR_BASE_T_LED_CFG_T led_cfg[3] = {
/********************************************************************
 *Enable,     GPIO,        LED Polarity,    LED ON,     LED Blink
*********************************************************************/
    {1,	 AIR_LED0_GPIO5, AIR_ACTIVE_HIGH, AIR_LED0_ON, AIR_LED0_BLK},
    {1,	 AIR_LED1_GPIO4, AIR_ACTIVE_HIGH, AIR_LED1_ON, AIR_LED1_BLK},
    {0,	 AIR_LED2_GPIO3, AIR_ACTIVE_HIGH, AIR_LED2_ON, AIR_LED2_BLK},
};
static const u16 led_dur = UNIT_LED_BLINK_DURATION << AIR_LED_BLK_DUR_64M;
#endif
/* User-defined.E */

/************************************************************************
*                  F U N C T I O N S
************************************************************************/

/* Airoha MII read function */
static int air_mii_cl22_read(struct mii_bus *ebus, unsigned int phy_addr,unsigned int phy_register)
{
    int read_data;
    read_data = mdiobus_read(ebus, phy_addr, phy_register);
    return read_data;
}

/* Airoha MII write function */
static int air_mii_cl22_write(struct mii_bus *ebus, unsigned int phy_addr, unsigned int phy_register,unsigned int write_data)
{
    int ret = 0;
    ret = mdiobus_write(ebus, phy_addr, phy_register, write_data);
    return ret;
}

static int air_mii_cl45_read(struct phy_device *phydev, int devad, u16 reg)
{
    int ret = 0;
    int data;
    struct device *dev = phydev_dev(phydev);
    ret = phy_write(phydev, MII_MMD_ACC_CTL_REG, devad);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return INVALID_DATA;
    }
    ret = phy_write(phydev, MII_MMD_ADDR_DATA_REG, reg);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return INVALID_DATA;
    }
    ret = phy_write(phydev, MII_MMD_ACC_CTL_REG, MMD_OP_MODE_DATA | devad);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return INVALID_DATA;
    }
    data = phy_read(phydev, MII_MMD_ADDR_DATA_REG);
    return data;
}

static int air_mii_cl45_write(struct phy_device *phydev, int devad, u16 reg, u16 write_data)
{
    int ret = 0;
    struct device *dev = phydev_dev(phydev);
    ret = phy_write(phydev, MII_MMD_ACC_CTL_REG, devad);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, MII_MMD_ADDR_DATA_REG, reg);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, MII_MMD_ACC_CTL_REG, MMD_OP_MODE_DATA | devad);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, MII_MMD_ADDR_DATA_REG, write_data);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    return 0;
}
/* Use default PBUS_PHY_ID */
/* EN8811H PBUS write function */
static int air_pbus_reg_write(struct phy_device *phydev, unsigned long pbus_address, unsigned long pbus_data)
{
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);
    int ret = 0;
    ret = air_mii_cl22_write(mbus, (addr + 8), 0x1F, (unsigned int)(pbus_address >> 6));
    AIR_RTN_ERR(ret);
    ret = air_mii_cl22_write(mbus, (addr + 8), (unsigned int)((pbus_address >> 2) & 0xf), (unsigned int)(pbus_data & 0xFFFF));
    AIR_RTN_ERR(ret);
    ret = air_mii_cl22_write(mbus, (addr + 8), 0x10, (unsigned int)(pbus_data >> 16));
    AIR_RTN_ERR(ret);
    return 0;
}

/* EN8811H BUCK write function */
static int air_buckpbus_reg_write(struct phy_device *phydev, unsigned long pbus_address, unsigned int pbus_data)
{
    int ret = 0;
    struct device *dev = phydev_dev(phydev);
    ret = phy_write(phydev, 0x1F, (unsigned int)4);        /* page 4 */
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, 0x10, (unsigned int)0);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, 0x11, (unsigned int)((pbus_address >> 16) & 0xffff));
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, 0x12, (unsigned int)(pbus_address & 0xffff));
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, 0x13, (unsigned int)((pbus_data >> 16) & 0xffff));
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, 0x14, (unsigned int)(pbus_data & 0xffff));
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, 0x1F, 0);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    return 0;
}

/* EN8811H BUCK read function */
static unsigned int air_buckpbus_reg_read(struct phy_device *phydev, unsigned long pbus_address)
{
    unsigned int pbus_data = 0, pbus_data_low, pbus_data_high;
    int ret = 0;
    struct device *dev = phydev_dev(phydev);
    ret = phy_write(phydev, 0x1F, (unsigned int)4);        /* page 4 */
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return PBUS_INVALID_DATA;
    }
    ret = phy_write(phydev, 0x10, (unsigned int)0);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return PBUS_INVALID_DATA;
    }
    ret = phy_write(phydev, 0x15, (unsigned int)((pbus_address >> 16) & 0xffff));
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return PBUS_INVALID_DATA;
    }
    ret = phy_write(phydev, 0x16, (unsigned int)(pbus_address & 0xffff));
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return PBUS_INVALID_DATA;
    }

    pbus_data_high = phy_read(phydev, 0x17);
    pbus_data_low = phy_read(phydev, 0x18);
    pbus_data = (pbus_data_high << 16) + pbus_data_low;
    ret = phy_write(phydev, 0x1F, 0);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    return pbus_data;
}

static int MDIOWriteBuf(struct phy_device *phydev, unsigned long address, const struct firmware *fw)
{
    unsigned int write_data, offset ;
    int ret = 0;
    struct device *dev = phydev_dev(phydev);
    ret = phy_write(phydev, 0x1F, (unsigned int)4);            /* page 4 */
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, 0x10, (unsigned int)0x8000);        /* address increment*/
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, 0x11, (unsigned int)((address >> 16) & 0xffff));
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    ret = phy_write(phydev, 0x12, (unsigned int)(address & 0xffff));
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }

    for (offset = 0; offset < fw->size; offset += 4)
    {
        write_data = (fw->data[offset + 3] << 8) | fw->data[offset + 2];
        ret = phy_write(phydev, 0x13, write_data);
        if (ret < 0) {
            dev_err(dev, "phy_write, ret: %d\n", ret);
            return ret;
        }
        write_data = (fw->data[offset + 1] << 8) | fw->data[offset];
        ret = phy_write(phydev, 0x14, write_data);
        if (ret < 0) {
            dev_err(dev, "phy_write, ret: %d\n", ret);
            return ret;
        }
    }
    ret = phy_write(phydev, 0x1F, (unsigned int)0);
    if (ret < 0) {
        dev_err(dev, "phy_write, ret: %d\n", ret);
        return ret;
    }
    return 0;
}

static void MDIOReadBuf(struct phy_device *phydev, unsigned long address,
            const struct firmware *fw, unsigned int *crc32)
{
    unsigned int write_data, offset;
    int ret = 0, len = 0;
    unsigned int pbus_data_low, pbus_data_high;
    struct device *dev = phydev_dev(phydev);
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);
    char *buf = kmalloc(fw->size, GFP_KERNEL);

    memset(buf, '\0', fw->size);
    /* page 4 */
    ret |= phy_write(phydev, 0x1F, 4);
    /* address increment*/
    ret |= phy_write(phydev, 0x10, 0x8000);
    ret |= phy_write(phydev,
            0x15, (unsigned int)((address >> 16) & 0xffff));
    ret |= phy_write(phydev,
            0x16, (unsigned int)(address & 0xffff));
    for (offset = 0; offset < fw->size; offset += 4) {
        pbus_data_high = phy_read(phydev, 0x17);
        pbus_data_low = phy_read(phydev, 0x18);
        buf[offset + 0] = pbus_data_low & 0xff;
        buf[offset + 1] = (pbus_data_low & 0xff00) >> 8;
        buf[offset + 2] = pbus_data_high & 0xff;
        buf[offset + 3] = (pbus_data_high & 0xff00) >> 8;
    }
    msleep(100);
    *crc32 = ~crc32(~0, buf, fw->size);
    ret |= phy_write(phydev, 0x1F, 0);
    kfree(buf);
    if (ret) {
        dev_info(dev, "%s 0x%lx FAIL(ret:%d)\n",
                __func__, address, ret);
    }
}

#ifdef CONFIG_OF
int en8811h_of_init(struct phy_device *phydev)
{
    struct device *dev = phydev_dev(phydev);
    struct device_node *of_node = dev->of_node;
    struct en8811h_priv *priv = phydev->priv;
    u32 val = 0;
    u16 temp, child_item = 0;

    dev_info(dev, "%s: start\n", __func__);
    if(!of_node) {
        dev_info(dev, "%s: of_node is NULL\n", __func__);
        return -ENODEV;
    }

    if (of_find_property(of_node, "airoha,polarity", NULL)) {
        dev_info(dev, "%s: of_find_property\n", __func__);
        if (of_property_read_u32(of_node, "airoha,polarity",
                     &val) != 0) {
            dev_err(dev, "airoha,polarity value is invalid.");
            return -1;
        }
        if (val < AIR_POL_TX_REV_RX_NOR ||
            val > AIR_POL_TX_NOR_RX_REV) {
            dev_err(dev,
                   "airoha,polarity value %u out of range.",
                   val);
            return -1;
        }
        priv->pol = val;
    } else
        priv->pol = AIR_POL_TX_NOR_RX_NOR;

    if (of_find_property(of_node, "airoha,surge", NULL)) {
        if (of_property_read_u32(of_node, "airoha,surge",
                     &val) != 0) {
            dev_err(dev, "airoha,surge value is invalid.");
            return -1;
        }
        if (val < AIR_SURGE_0R ||
            val > AIR_SURGE_5R) {
            dev_err(dev,
                   "airoha,surge value %u out of range.",
                   val);
            return -1;
        }
        priv->surge = val;
    } else
        priv->surge = AIR_SURGE_0R;

    if (of_find_property(of_node, "airoha,cko-en", NULL)) {
        if (of_property_read_u32(of_node, "airoha,cko-en",
                     &val) != 0) {
            dev_err(dev, "airoha,cko-en value is invalid.");
            return -1;
        }
        if (val < AIR_CKO_DIS ||
            val > AIR_CKO_ENA) {
            dev_err(dev,
                   "airoha,cko-en value %u out of range.",
                   val);
            return -1;
        }
        priv->cko = val;
    } else
        priv->cko = AIR_CKO_DIS;

    return 0;
}
#else
int en8811h_of_init(struct phy_device *phydev)
{
    return 0;
}
#endif /* CONFIG_OF */


static int en8811h_load_firmware(struct phy_device *phydev)
{
    struct device *dev = phydev_dev(phydev);
    const struct firmware *fw;
    const char *firmware;
    int ret = 0, retry;
    u32 pbus_value = 0;
    struct en8811h_priv *priv = phydev->priv;
#ifdef AIR_MD32_FW_CHECK
    unsigned int d_crc32 = 0, crc32 = 0;
#endif

    ret = air_buckpbus_reg_write(phydev, 0x0f0018, 0x0);
    AIR_RTN_ERR(ret);
    pbus_value = air_buckpbus_reg_read(phydev, 0x800000);
    pbus_value |= BIT(11);
    ret = air_buckpbus_reg_write(phydev, 0x800000, pbus_value);
    AIR_RTN_ERR(ret);
    firmware = EN8811H_MD32_DM;
    ret = request_firmware_direct(&fw, firmware, dev);
    if (ret < 0) {
        dev_err(dev, "failed to load firmware %s, ret: %d\n", firmware, ret);
        return ret;
    }
    priv->dm_crc32 = ~crc32(~0, fw->data, fw->size);
    dev_info(dev, "%s: crc32=0x%x\n", firmware, priv->dm_crc32);
    /* Download DM */
    ret = MDIOWriteBuf(phydev, 0x00000000, fw);
    release_firmware(fw);
    if (ret < 0) {
        dev_err(dev, "MDIOWriteBuf 0x00000000 fail, ret: %d\n", ret);
        goto release;
    }
    firmware = EN8811H_MD32_DSP;
    ret = request_firmware_direct(&fw, firmware, dev);
    if (ret < 0) {
        dev_info(dev, "failed to load firmware %s, ret: %d\n", firmware, ret);
        return ret;
    }
    priv->dsp_crc32 = ~crc32(~0, fw->data, fw->size);
    dev_info(dev, "%s: crc32=0x%x\n", firmware, priv->dsp_crc32);
    /* Download PM */
    ret = MDIOWriteBuf(phydev, 0x00100000, fw);
    if (ret < 0) {
        dev_err(dev, "MDIOWriteBuf 0x00100000 fail , ret: %d\n", ret);
        goto release;
    }
    pbus_value = air_buckpbus_reg_read(phydev, 0x800000);
    pbus_value &= ~BIT(11);
    ret = air_buckpbus_reg_write(phydev, 0x800000, pbus_value);
    AIR_RTN_ERR(ret);
#ifdef AIR_MD32_FW_CHECK
    crc32 = priv->dsp_crc32;
    /* Check PM */
    MDIOReadBuf(phydev, 0x100000, fw, &d_crc32);
    if (d_crc32 == crc32)
        dev_info(dev, "0x00100000 Check Sum Pass.\n");
    else {
        dev_err(dev, "0x00100000 Check Sum Fail.\n");
        dev_err(dev, "CRC32 0x%x != Caculated CRC32 0x%x\n",
                    crc32, d_crc32);
    }
    release_firmware(fw);
    retry = MAX_RETRY;
    do {
        ret = air_buckpbus_reg_write(phydev, 0x0f0018, 0x01);
        if (ret < 0)
            return ret;
        msleep(100);
        pbus_value = air_buckpbus_reg_read(phydev, 0x0f0018);
        if (retry == 0) {
            dev_err(dev,
                "Release Software Reset fail , ret: %d\n",
                        pbus_value);
            break;
        }
        retry--;
    } while (pbus_value != 0x1);
    dev_info(dev,
        "Release Software Reset successful.\n");
#else
    release_firmware(fw);
    ret = air_buckpbus_reg_write(phydev, 0x0f0018, 0x01);
    if (ret < 0)
        return ret;
#endif
    return 0;
release:
    release_firmware(fw);
    return ret;
}

#ifdef  AIR_LED_SUPPORT
static int airoha_led_set_usr_def(struct phy_device *phydev, u8 entity, int polar,
                                   u16 on_evt, u16 blk_evt)
{
    int ret = 0;
    if (AIR_ACTIVE_HIGH == polar) {
        on_evt |= LED_ON_POL;
    } else {
        on_evt &= ~LED_ON_POL ;
    }
    ret = air_mii_cl45_write(phydev, 0x1f, LED_ON_CTRL(entity), on_evt | LED_ON_EN);
    AIR_RTN_ERR(ret);
    ret = air_mii_cl45_write(phydev, 0x1f, LED_BLK_CTRL(entity), blk_evt);
    AIR_RTN_ERR(ret);
    return 0;
}

static int airoha_led_set_mode(struct phy_device *phydev, u8 mode)
{
    u16 cl45_data;
    int err = 0;
    struct device *dev = phydev_dev(phydev);
    cl45_data = air_mii_cl45_read(phydev, 0x1f, LED_BCR);
    switch (mode) {
    case AIR_LED_MODE_DISABLE:
        cl45_data &= ~LED_BCR_EXT_CTRL;
        cl45_data &= ~LED_BCR_MODE_MASK;
        cl45_data |= LED_BCR_MODE_DISABLE;
        break;
    case AIR_LED_MODE_USER_DEFINE:
        cl45_data |= LED_BCR_EXT_CTRL;
        cl45_data |= LED_BCR_CLK_EN;
        break;
    default:
        dev_err(dev, "LED mode%d is not supported!\n", mode);
        return -EINVAL;
    }
    err = air_mii_cl45_write(phydev, 0x1f, LED_BCR, cl45_data);
    AIR_RTN_ERR(err);
    return 0;
}

static int airoha_led_set_state(struct phy_device *phydev, u8 entity, u8 state)
{
    u16 cl45_data = 0;
    int err;

    cl45_data = air_mii_cl45_read(phydev, 0x1f, LED_ON_CTRL(entity));
    if (LED_ENABLE == state) {
        cl45_data |= LED_ON_EN;
    } else {
        cl45_data &= ~LED_ON_EN;
    }

    err = air_mii_cl45_write(phydev, 0x1f, LED_ON_CTRL(entity), cl45_data);
    AIR_RTN_ERR(err);
    return 0;
}

static int en8811h_led_init(struct phy_device *phydev)
{

    unsigned long led_gpio = 0, reg_value = 0;
    u16 cl45_data = led_dur;
    int ret = 0, led_id;
    struct device *dev = phydev_dev(phydev);
    struct en8811h_priv *priv = phydev->priv;

    ret = air_mii_cl45_write(phydev, 0x1f, LED_BLK_DUR, cl45_data);
    AIR_RTN_ERR(ret);
    cl45_data >>= 1;
    ret = air_mii_cl45_write(phydev, 0x1f, LED_ON_DUR, cl45_data);
    AIR_RTN_ERR(ret);
    ret = airoha_led_set_mode(phydev, AIR_LED_MODE_USER_DEFINE);
    if (ret != 0) {
        dev_err(dev, "LED fail to set mode, ret %d !\n", ret);
        return ret;
    }
    for(led_id = 0; led_id < EN8811H_LED_COUNT; led_id++)
    {
        /* LED0 <-> GPIO5, LED1 <-> GPIO4, LED0 <-> GPIO3 */
        if (led_cfg[led_id].gpio != (led_id + (AIR_LED0_GPIO5 - (2 * led_id))))
        {
            dev_err(dev, "LED%d uses incorrect GPIO%d !\n", led_id, led_cfg[led_id].gpio);
            return -EINVAL;
        }
        ret = airoha_led_set_state(phydev, led_id, led_cfg[led_id].en);
        if (ret != 0)
        {
            dev_err(dev, "LED fail to set state, ret %d !\n", ret);
            return ret;
        }
        if (LED_ENABLE == led_cfg[led_id].en)
        {
            led_gpio |= BIT(led_cfg[led_id].gpio);
            ret = airoha_led_set_usr_def(phydev, led_id, led_cfg[led_id].pol, led_cfg[led_id].on_cfg, led_cfg[led_id].blk_cfg);
            if (ret != 0)
            {
                dev_err(dev, "LED fail to set default, ret %d !\n", ret);
                return ret;
            }
        }
        priv->on_crtl[led_id] = air_mii_cl45_read(phydev, 0x1f,
        		LED_ON_CTRL(led_id));
        priv->blk_crtl[led_id] = air_mii_cl45_read(phydev, 0x1f,
        		LED_BLK_CTRL(led_id));
    }
    reg_value = air_buckpbus_reg_read(phydev, 0xcf8b8) | led_gpio;
    ret = air_buckpbus_reg_write(phydev, 0xcf8b8, reg_value);
    AIR_RTN_ERR(ret);

    dev_info(dev, "LED initialize OK !\n");
    return 0;
}
#endif /* AIR_LED_SUPPORT */
/* EN8811H 0R or 5R */
static int air_surge_protect_cfg(struct phy_device *phydev)
{
    int ret = 0;
    struct device *dev = phydev_dev(phydev);
    struct en8811h_priv *priv = phydev->priv;

    if (priv->surge){
        ret |= air_mii_cl45_write(phydev, 0x1e, 0x800c, 0x0);
        ret |= air_mii_cl45_write(phydev, 0x1e, 0x800d, 0x0);
        ret |= air_mii_cl45_write(phydev, 0x1e, 0x800e, 0x1100);
        ret |= air_mii_cl45_write(phydev, 0x1e, 0x800f, 0x00b0);
        if(ret < 0)
            return ret;
        dev_info(dev, "Surge Protection mode - 5R\n");
    } else
        dev_info(dev, "Surge Protection mode - 0R\n");
    return 0;
}

static int air_cko_cfg(struct phy_device *phydev)
{
    int ret = 0;
    struct device *dev = phydev_dev(phydev);
    struct en8811h_priv *priv = phydev->priv;
    u32 pbus_value = 0;

    if (!priv->cko){
        pbus_value = air_buckpbus_reg_read(phydev, 0xcf958);
        pbus_value &= ~BIT(26);
        ret = air_buckpbus_reg_write(phydev, 0xcf958, pbus_value);
        if (ret < 0)
            return ret;
        dev_info(dev, "CKO Output Mode - Disabled\n");
    } else
        dev_info(dev, "CKO Output Mode - Enabled\n");
    return 0;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 5, 0))
static int en8811h_get_features(struct phy_device *phydev)
{
    int ret;
    struct device *dev = phydev_dev(phydev);
    dev_info(dev, "%s()\n", __func__);
    ret = air_pbus_reg_write(phydev, 0xcf928 , 0x0);
    AIR_RTN_ERR(ret);
    ret = genphy_read_abilities(phydev);
    if (ret)
        return ret;

    /* EN8811H supports 100M/1G/2.5G speed. */
    linkmode_clear_bit(ETHTOOL_LINK_MODE_10baseT_Half_BIT,
               phydev->supported);
    linkmode_clear_bit(ETHTOOL_LINK_MODE_10baseT_Full_BIT,
               phydev->supported);
    linkmode_set_bit(ETHTOOL_LINK_MODE_100baseT_Half_BIT,
               phydev->supported);
    linkmode_set_bit(ETHTOOL_LINK_MODE_100baseT_Full_BIT,
               phydev->supported);
    linkmode_set_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
               phydev->supported);
    linkmode_set_bit(ETHTOOL_LINK_MODE_2500baseX_Full_BIT,
               phydev->supported);
    return 0;
}
#endif
static int en8811h_phy_probe(struct phy_device *phydev)
{
    int ret = 0;
    int reg_value, pid1 = 0, pid2 = 0;
    u32 pbus_value = 0, retry;
    struct device *dev = phydev_dev(phydev);
    en8811_phy_dev = phydev;
    struct en8811h_priv *priv;

    spin_lock_init(&en8811_phy_lock);
    ret = air_pbus_reg_write(phydev, 0xcf928 , 0x0);
    AIR_RTN_ERR(ret);
    pid1 = phy_read(phydev, MII_PHYSID1);
    if (pid1 < 0)
        return pid1;
    pid2 = phy_read(phydev, MII_PHYSID2);
    if (pid2 < 0)
        return pid2;
    dev_info(dev, "PHY = %x - %x\n", pid1, pid2);
    if ((EN8811H_PHY_ID1 != pid1) || (EN8811H_PHY_ID2 != pid2))
    {
        dev_err(dev, "EN8811H dose not exist!!\n");
        return -ENODEV;
    }
    priv = kzalloc(sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;
    phydev->priv = priv;
    ret = air_buckpbus_reg_write(phydev, 0x1e00d0, 0xf);
    ret |= air_buckpbus_reg_write(phydev, 0x1e0228, 0xf0);
    if (ret < 0)
        goto priv_free;

    ret = en8811h_load_firmware(phydev);
    if (ret)
    {
        dev_err(dev,"EN8811H load firmware fail.\n");
        goto priv_free;
    }
    retry = MAX_RETRY;
    do {
        mdelay(300);
        reg_value = air_mii_cl45_read(phydev, 0x1e, 0x8009);
        if (EN8811H_PHY_READY == reg_value)
        {
            dev_info(dev, "EN8811H PHY ready!\n");
            break;
        }
        retry--;
        if (0 == retry)
        {
            dev_err(dev, "MD32 FW is not ready.(Status 0x%x)\n", reg_value);
            pbus_value = air_buckpbus_reg_read(phydev, 0x3b3c);
            dev_err(dev,
                "Check MD32 FW Version(0x3b3c) : %08x\n", pbus_value);
            dev_err(dev,
                "EN8811H initialize fail!\n");
            goto priv_free;
        }
    } while (retry);
    ret |= air_mii_cl45_write(phydev, 0x1e, 0x800c, 0x0);
    ret |= air_mii_cl45_write(phydev, 0x1e, 0x800d, 0x0);
    ret |= air_mii_cl45_write(phydev, 0x1e, 0x800e, 0x1101);
    ret |= air_mii_cl45_write(phydev, 0x1e, 0x800f, 0x0002);
    if(ret < 0)
        goto priv_free;
    ret = en8811h_of_init(phydev);
    if(ret < 0) {
        dev_info(dev, "en8811h_of_init fail!\n");
        goto priv_free;
    }
    /* Serdes polarity */
    pbus_value = air_buckpbus_reg_read(phydev, 0xca0f8);
    pbus_value &= ~0x3;
#if defined(CONFIG_OF)
    pbus_value |= priv->pol;
#else
    pbus_value |= (EN8811H_RX_POL_NORMAL | EN8811H_TX_POL_NORMAL);
#endif
    ret = air_buckpbus_reg_write(phydev, 0xca0f8, pbus_value);
    if (ret < 0)
        goto priv_free;
    pbus_value = air_buckpbus_reg_read(phydev, 0xca0f8);
    dev_info(dev, "Tx, Rx Polarity(0xca0f8): %08x\n", pbus_value);
    pbus_value = air_buckpbus_reg_read(phydev, 0x3b3c);
    dev_info(dev, "MD32 FW Version(0x3b3c) : %08x\n", pbus_value);
    ret = air_surge_protect_cfg(phydev);
    if (ret < 0) {
        dev_err(dev, "air_surge_protect_cfg fail. (ret=%d)\n", ret);
        goto priv_free;
    }
    ret = air_cko_cfg(phydev);
    if (ret < 0) {
        dev_err(dev, "air_cko_cfg fail. (ret=%d)\n", ret);
        goto priv_free;
    }
#if defined(AIR_LED_SUPPORT)
    ret = en8811h_led_init(phydev);
    if (ret < 0)
    {
        dev_err(dev, "en8811h_led_init fail. (ret=%d)\n", ret);
        goto priv_free;
    }
#endif
    dev_info(dev, "EN8811H initialize OK! (%s)\n", EN8811H_DRIVER_VERSION);
    return 0;
priv_free:
    kfree(priv);
    return 0;
}

static int en8811h_get_autonego(struct phy_device *phydev, int *an)
{
    int reg;
    reg = phy_read(phydev, MII_BMCR);
    if (reg < 0)
        return -EINVAL;
    if (reg & BMCR_ANENABLE)
        *an = AUTONEG_ENABLE;
    else
        *an = AUTONEG_DISABLE;
    return 0;
}

void en8811h_remove(struct phy_device *phydev)
{

    struct en8811h_priv *priv = phydev->priv;
    struct device *dev = phydev_dev(phydev);

    dev_dbg(dev, "%s: start\n", __func__);
    if (priv) {
        kfree(priv);
        dev_info(dev, "%s: free priv\n", __func__);
    }
}

static struct phy_driver en8811h_driver[] = {
{
    .phy_id         = EN8811H_PHY_ID,
    .name           = "Airoha EN8811H",
    .phy_id_mask    = 0x0ffffff0,
    .probe          = en8811h_phy_probe,
    .remove         = en8811h_remove,
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 5, 0))
    .get_features   = en8811h_get_features,
#endif
} };

int __init en8811h_phy_driver_register(void)
{
    int ret;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
    ret = phy_driver_register(en8811h_driver);
#else
    ret = phy_driver_register(en8811h_driver, THIS_MODULE);
#endif
    if (!ret)
        return 0;

    phy_driver_unregister(en8811h_driver);
    return ret;
}

void __exit en8811h_phy_driver_unregister(void)
{
    phy_driver_unregister(en8811h_driver);
}

module_init(en8811h_phy_driver_register);
module_exit(en8811h_phy_driver_unregister);