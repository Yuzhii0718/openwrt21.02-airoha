#ifndef _AIR_EN8811H_API_H
#define _AIR_EN8811H_API_H

#include <linux/mii.h>
#include <linux/phy.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
#define phydev_mdio_bus(_dev) (_dev->bus)
#define phydev_addr(_dev) (_dev->addr)
#define phydev_dev(_dev) (&_dev->dev)
#else
#define phydev_mdio_bus(_dev) (_dev->mdio.bus)
#define phydev_addr(_dev) (_dev->mdio.addr)
#define phydev_dev(_dev) (&_dev->mdio.dev)
#endif

#define BUFFER_LENGTH 1024
#define CMD_MAX_LENGTH 128
/* MII Registers */
#define AIR_AUX_CTRL_STATUS		0x1d
#define AIR_AUX_CTRL_STATUS_SPEED_MASK	0x1C
#define AIR_AUX_CTRL_STATUS_SPEED_100	0x4
#define AIR_AUX_CTRL_STATUS_SPEED_1000	0x8
#define AIR_AUX_CTRL_STATUS_SPEED_2500	0xc

#define GET_BIT(val, bit) ((val & BIT(bit)) >> bit)

struct air_lp_speed {
	int rg;
	int speed;
	int bit;
};

struct air_cable_test_rsl {
    int status[4];
    unsigned int length [4];
};

typedef struct
{
    unsigned int TrRG_LSB     :5;
    unsigned int Reserved_21  :3;
    unsigned int TrRG_MSB     :5;
    unsigned int Reserved_29  :3;
    unsigned int Reserved_0   :1;
    unsigned int DATA_ADDR    :6;
    unsigned int NODE_ADDR    :4;
    unsigned int CH_ADDR      :2;
    unsigned int WR_RD_CTRL   :1;
    unsigned int Reserved_14  :1;
    unsigned int PKT_XMT_STA  :1;
}TrRG_PARAM_T;

typedef union
{
    TrRG_PARAM_T param;
    unsigned short     Raw[2];
    unsigned int       DescVal;
}TrRGDesc_T;

typedef struct
{
    TrRGDesc_T TrRGDesc;
    unsigned int     RgMask;
}TrRG_T;

typedef struct
{
    unsigned short data_lo;
    unsigned char data_hi;
}HAL_TR_DATA_T;

typedef enum
{
    AIR_PORT_MODE_FORCE_100,
    AIR_PORT_MODE_FORCE_1000,
    AIR_PORT_MODE_FORCE_2500,
    AIR_PORT_MODE_AUTONEGO,
    AIR_PORT_MODE_POWER_DOWN,
    AIR_PORT_MODE_POWER_UP,
    AIR_PORT_MODE_SSC_DISABLE,
    AIR_PORT_MODE_SSC_ENABLE,
    AIR_PORT_MODE_LAST = 0xff,
} AIR_PORT_MODE_T;

enum air_led_force {
	AIR_LED_NORMAL = 0,
	AIR_LED_FORCE_OFF,
	AIR_LED_FORCE_ON,
	AIR_LED_FORCE_LAST = 0xff,
};

enum air_port_speed {
	AIR_SPEED_100  = 1,
	AIR_SPEED_1000 = 2,
	AIR_SPEED_2500 = 3,
	AIR_SPEED_LAST = 0xFF,
};

enum air_port_cable_status {
    AIR_PORT_CABLE_STATUS_ERROR,
    AIR_PORT_CABLE_STATUS_OPEN,
    AIR_PORT_CABLE_STATUS_SHORT,
    AIR_PORT_CABLE_STATUS_NORMAL,
    AIR_PORT_CABLE_STATUS_LAST = 0xff
};

enum air_port_cable_test_pair
{
    AIR_PORT_CABLE_TEST_PAIR_A,
    AIR_PORT_CABLE_TEST_PAIR_B,
    AIR_PORT_CABLE_TEST_PAIR_C,
    AIR_PORT_CABLE_TEST_PAIR_D,
    AIR_PORT_CABLE_TEST_PAIR_ALL,
    AIR_PORT_CABLE_TEST_PAIR_LAST
};

/* bits range: for example BITS(16,23) = 0xFF0000*/
#ifndef AIR_BITS
#define AIR_BITS(m, n)   (~(BIT(m) - 1) & ((BIT(n) - 1) | BIT(n)))
#endif  /* End of BITS */

/* bits range: for example BITS_RANGE(16,4) = 0x0F0000*/
#ifndef AIR_BITS_RANGE
#define AIR_BITS_RANGE(offset, range)           AIR_BITS((offset), ((offset)+(range)-1))
#endif  /* End of BITS_RANGE */

/* bits offset right: for example BITS_OFF_R(0x1234, 8, 4) = 0x2 */
#ifndef AIR_BITS_OFF_R
#define AIR_BITS_OFF_R(val, offset, range)      (((val) >> offset) & (AIR_BITS(0, (range) - 1)))
#endif  /* End of BITS_OFF_R */

/* bits offset left: for example BITS_OFF_L(0x1234, 8, 4) = 0x400 */
#ifndef AIR_BITS_OFF_L
#define AIR_BITS_OFF_L(val, offset, range)      (((val) & (AIR_BITS(0, (range) - 1))) << (offset))
#endif  /* End of BITS_OFF_L */

#define AIR_EN8811H_SET_VALUE(__out__, __val__, __offset__, __length__) do              \
{                                                                                       \
    (__out__) &= ~AIR_BITS_RANGE((__offset__), (__length__));                               \
    (__out__) |= AIR_BITS_OFF_L((__val__), (__offset__), (__length__));                     \
}while(0)



#ifndef unlikely
#  define unlikely(x)	(x)
#endif

/* Airoha MII read function */
int air_mii_cl22_read(struct mii_bus *ebus, int addr,unsigned int phy_register);
/* Airoha MII write function */
int air_mii_cl22_write(struct mii_bus *ebus, int addr, unsigned int phy_register,unsigned int write_data);
int air_mii_cl45_read(struct phy_device *phydev, int devad, u16 reg);
int air_mii_cl45_write(struct phy_device *phydev, int devad, u16 reg, u16 write_data);

/* EN8811H pbus read function */
unsigned long air_pbus_reg_read(struct phy_device *phydev, unsigned long pbus_address);
int air_pbus_reg_write(struct phy_device *phydev, unsigned long pbus_address, unsigned long pbus_data);

/* EN8811H BUCK read function */
unsigned int air_buckpbus_reg_read(struct phy_device *phydev, unsigned long pbus_address);
int air_buckpbus_reg_write(struct phy_device *phydev, unsigned long pbus_address, unsigned int pbus_data);

int en8811h_get_autonego(struct phy_device *phydev, int *an);
int en8811h_read_status(struct phy_device *phydev);
int airphy_set_mode(struct phy_device *phydev, AIR_PORT_MODE_T dbg_mode);
int airphy_fcm_counter_show(struct phy_device *phydev, char **buff, int *length);
int airphy_ls_counter_show(struct phy_device *phydev,
            char **buff, int *length);
int airphy_mac_counter_show(struct phy_device *phydev,
            char **buff, int *length);
int airphy_ss_counter_show(struct phy_device *phydev,
            char **buff, int *length);
int airphy_dbg_regs_show(struct phy_device *phydev,
            char **buff, int *length);
int airphy_temp_show(struct phy_device *phydev,
            char **buff, int *length);
int airphy_cable_step1(struct phy_device *phydev);
int airphy_led_control(struct phy_device *phydev, int force_mode);
int airphy_trigger_cable_diag(struct phy_device *phydev);
int airphy_dump_ec_cable_diag(struct phy_device *phydev);
#endif
