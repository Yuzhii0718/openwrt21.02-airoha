
/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <linux/types.h>
#include <asm/io.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include "ecnt_sgmii.h"
#include <asm/string.h>
#include "sgmii_reg_pon.h"
#include <uapi/ecnt_event_global/ecnt_event_system.h>
#include <linux/delay.h>
#include <asm/delay.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
extern int get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel);

extern u32 sgmii_cmd_ro(sgmii_port_type port_id, sgmii_reg_type type, u32 reg);
extern void sgmii_cmd_wo(sgmii_port_type port_id, sgmii_reg_type type, u32 reg, u32 val);

extern void SET_SCU_RST_RG(u32 val);
extern void SET_SSR3(u32 val);
extern void SET_WAN_CONF(u32 val);
extern u32 GET_WAN_CONF(void);

extern void set_pon_phy_data(u32 reg, u32 val);
extern void get_pon_phy_data(u32 reg);

extern u32 set_pon_pcs2_data(u32 reg, u32 val);
extern u32 get_pon_pcs2_data(u32 reg);
extern void set_pon_an_data(u32 reg, u32 val);
extern u32 get_pon_an_data(u32 reg);
extern void set_pon_ra_data(u32 reg, u32 val);
extern u32 get_pon_ra_data(u32 reg);

const char* sgmii_port_name[] = {"PORT_PCIE0", "PORT_PCIE1", "PORT_USB0", "PORT_PON0","PORT_UNKNOW"};
const char* sgmii_speed_name[] = {"SPEED_2500M", "SPEED_1000M", "SPEED_100M","SPEED_10M","SPEED_UNKNOW"};
/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define PON_INT_EN 0x5f0
#define PON_INT_STA_CLR 0x5f4
/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/


/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int sgmii_api1(int argc, char *argv[], void *p);
int sgmii_api_mode(sgmii_api_method_type method, sgmii_port_type port, sgmii_speed_type speed);
int sgmii_api_info(sgmii_api_method_type method, sgmii_port_type port);
static int sgmii_test(unsigned long *reg, unsigned long *value);

static sgmii_port_type _covert_port(char* port_str);
static sgmii_speed_type _covert_speed(char* speed_str);
static sgmii_api_method_type _covert_method(char* method_str);
static sgmii_api_type _covert_api(char* api_str);

static int _sgmii_get_link(sgmii_port_type port);
static int _sgmii_set_speed(sgmii_port_type port, sgmii_speed_type speed);
static sgmii_speed_type _sgmii_get_speed(sgmii_port_type port);
static int sgmii_sel_flag(sgmii_port_type port);
static int sgmii_int_enable(sgmii_port_type port, bool enable);


static int sgmii_api_pcie1_force_sgmii(sgmii_speed_type speed);
static int sgmii_api_pcie0_force_sgmii(sgmii_speed_type speed);
static int sgmii_api_usb0_force_sgmii(sgmii_speed_type speed);
static int sgmii_api_pon0_force_sgmii(sgmii_speed_type speed);

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
/*API -------------------------- */ 
static int sgmii_test(unsigned long *reg, unsigned long *value)
{
	printk("sgmii_test: \n");
	printk("%lx:%lx\n",*reg, *value);
	u32 ro = sgmii_cmd_ro(SGMII_PORT_PCIE0, SGMII_REG_PCS2, (u32)*reg);
	printk("ro:%lx\n",ro);
	return 0;
}


int sgmii_api_pcie1_force_hsgmii(void)
{
	uint32 default_val = 0;
	uint32 mac_glb_cfg = 0;
	printk("sgmii_api_pcie1_force_hsgmii: inint \n");

	//default_val = GET_SSR3();
	//SET_SSR3(default_val & 0xBFFFFFFF); //0xa0002820 [30]=0
	
	sgmii_cmd_wo(SGMII_PORT_PCIE1, SGMII_REG_PHYA, SGMII_REG_PHYA_11, 0x14817);
	sgmii_cmd_wo(SGMII_PORT_PCIE1, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0, 0x0c000c11);
	sgmii_cmd_wo(SGMII_PORT_PCIE1, SGMII_REG_AN, SGMII_REG_AN0, 0x00000140);
	sgmii_cmd_wo(SGMII_PORT_PCIE1, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6, 0x3);

	mac_glb_cfg = get_xsi_data(0x1fa71000);
	default_val = GET_SCU_RST_RG();
	SET_SCU_RST_RG(default_val | 0x24000);
	SET_SCU_RST_RG(default_val & 0xFFFDBFFF);
	set_xsi_data(0x1fa71000, mac_glb_cfg);
	printk("sgmii_api_pcie1_force_hsgmii: exit \n");
	return 0;
}

static int sgmii_api_pcie1_force_sgmii(sgmii_speed_type speed)
{
	HAL_RG_TOP rg;
	uint32 data_t;
	uint32 default_val = 0;
	uint32 mac_glb_cfg = 0;
	printk("sgmii_api_pcie1_force_sgmii: inint \n");
	//default_val = GET_SSR3();
	//SET_SSR3(default_val & 0xBFFFFFFF); //0xa0002820 [30]=0
		
	sgmii_cmd_wo(SGMII_PORT_PCIE1, SGMII_REG_PHYA, SGMII_REG_PHYA_11, 0x14813);
	sgmii_cmd_wo(SGMII_PORT_PCIE1, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0, 0x0c000c11);
	sgmii_cmd_wo(SGMII_PORT_PCIE1, SGMII_REG_AN, SGMII_REG_AN0, 0x00000140);
	sgmii_cmd_wo(SGMII_PORT_PCIE1, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6, 0x3);

	mac_glb_cfg = get_xsi_data(0x1fa71000);
	default_val = GET_SCU_RST_RG();
	SET_SCU_RST_RG(default_val | 0x24000);
	SET_SCU_RST_RG(default_val & 0xFFFDBFFF);

	data_t = sgmii_cmd_ro(SGMII_PORT_PCIE1, SGMII_REG_AN, SGMII_REG_AN_13);
	rg.an.sgmii_reg_an_13 = &data_t;
	rg.an.sgmii_reg_an_13->sgmii_remote_fault_dis = 1;
	sgmii_cmd_wo(SGMII_PORT_PCIE1, SGMII_REG_AN, SGMII_REG_AN_13, data_t);

	_sgmii_set_speed(SGMII_PORT_PCIE1, speed);

	set_xsi_data(0x1fa71000, mac_glb_cfg);

	printk("sgmii_api_pcie1_force_sgmii: exit \n");
	return 0;
}

int sgmii_api_pcie0_force_hsgmii(void)
{
	printk("sgmii_api_pcie0_force_hsgmii: inint \n");
	uint32 default_val = 0;
	uint32 mac_glb_cfg = 0;
	//default_val = GET_SSR3();
	//SET_SSR3(default_val & 0x7FFFFFFF); //0x60002820 [31]=0
	
	sgmii_cmd_wo(SGMII_PORT_PCIE0, SGMII_REG_PHYA, SGMII_REG_PHYA_11, 0x14817);
	sgmii_cmd_wo(SGMII_PORT_PCIE0, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0, 0x0c000c11);
	sgmii_cmd_wo(SGMII_PORT_PCIE0, SGMII_REG_AN, SGMII_REG_AN0, 0x00000140);
	sgmii_cmd_wo(SGMII_PORT_PCIE0, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6, 0x3);

	mac_glb_cfg = get_xsi_data(0x1fa70000);
	default_val = GET_SCU_RST_RG();
	SET_SCU_RST_RG(default_val | 0x12000);
	SET_SCU_RST_RG(default_val & 0xFFFEDFFF);
	set_xsi_data(0x1fa70000, mac_glb_cfg);
	printk("sgmii_api_pcie0_force_hsgmii: exit \n");
	return 0;
}
static int sgmii_api_pcie0_force_sgmii(sgmii_speed_type speed)
{
	HAL_RG_TOP rg;
	uint32 data_t;
	uint32 default_val = 0;
	uint32 mac_glb_cfg = 0;
	printk("sgmii_api_pcie0_force_sgmii: inint \n");
	//default_val = GET_SSR3();
	//SET_SSR3(default_val & 0x7FFFFFFF); //0x60002820 [31]=0

	sgmii_cmd_wo(SGMII_PORT_PCIE0, SGMII_REG_PHYA, SGMII_REG_PHYA_11, 0x14813);
	sgmii_cmd_wo(SGMII_PORT_PCIE0, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0, 0x0c000c11);
	sgmii_cmd_wo(SGMII_PORT_PCIE0, SGMII_REG_AN, SGMII_REG_AN0, 0x00000140);
	sgmii_cmd_wo(SGMII_PORT_PCIE0, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6, 0x3);

	mac_glb_cfg = get_xsi_data(0x1fa70000);
	default_val = GET_SCU_RST_RG();
	SET_SCU_RST_RG(default_val | 0x12000);
	SET_SCU_RST_RG(default_val & 0xFFFEDFFF);

	data_t = sgmii_cmd_ro(SGMII_PORT_PCIE0, SGMII_REG_AN, SGMII_REG_AN_13);
	rg.an.sgmii_reg_an_13 = &data_t;
	rg.an.sgmii_reg_an_13->sgmii_remote_fault_dis = 1;
	sgmii_cmd_wo(SGMII_PORT_PCIE0, SGMII_REG_AN, SGMII_REG_AN_13, data_t);

	_sgmii_set_speed(SGMII_PORT_PCIE0, speed);

	set_xsi_data(0x1fa70000, mac_glb_cfg);

	printk("sgmii_api_pcie0_force_sgmii: exit \n");
	return 0;
}

int sgmii_api_usb0_force_hsgmii(void)
{
	printk("sgmii_api_usb0_force_hsgmii: inint \n");
	uint32 default_val = 0;
	uint32 mac_glb_cfg = 0;
	
	default_val = GET_SSR3();
	SET_SSR3(default_val & 0xDFFFFFFF); //0xc0002820 [29]=0
	
	sgmii_cmd_wo(SGMII_PORT_USB0, SGMII_REG_PHYA, SGMII_REG_PHYA_11, 0x14817);
	sgmii_cmd_wo(SGMII_PORT_USB0, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0, 0x0c000c11);
	sgmii_cmd_wo(SGMII_PORT_USB0, SGMII_REG_AN, SGMII_REG_AN0, 0x00000140);
	sgmii_cmd_wo(SGMII_PORT_USB0, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6, 0x3);

	mac_glb_cfg = get_xsi_data(0x1fa80000);
	default_val = GET_SCU_RST_RG();
	SET_SCU_RST_RG(default_val | 0x48000);
	SET_SCU_RST_RG(default_val & 0xFFFB7FFF);
	set_xsi_data(0x1fa80000, mac_glb_cfg);
	
	printk("sgmii_api_usb0_force_hsgmii: exit \n");
	return 0;
}
static int sgmii_api_usb0_force_sgmii(sgmii_speed_type speed)
{
	HAL_RG_TOP rg;
	uint32 data_t;
	uint32 default_val = 0;
	uint32 mac_glb_cfg = 0;
	printk("sgmii_api_usb0_force_sgmii: inint \n");

	default_val = GET_SSR3();
	SET_SSR3(default_val & 0xDFFFFFFF); //0xc0002820 [29]=0
	sgmii_cmd_wo(SGMII_PORT_USB0, SGMII_REG_PHYA, SGMII_REG_PHYA_11, 0x14813);
	sgmii_cmd_wo(SGMII_PORT_USB0, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0, 0x0c000c11);
	sgmii_cmd_wo(SGMII_PORT_USB0, SGMII_REG_AN, SGMII_REG_AN0, 0x00000140);
	sgmii_cmd_wo(SGMII_PORT_USB0, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6, 0x3);

	mac_glb_cfg = get_xsi_data(0x1fa80000);
	default_val = GET_SCU_RST_RG();
	SET_SCU_RST_RG(default_val | 0x48000);
	SET_SCU_RST_RG(default_val & 0xFFFB7FFF);

	data_t = sgmii_cmd_ro(SGMII_PORT_USB0, SGMII_REG_AN, SGMII_REG_AN_13);
	rg.an.sgmii_reg_an_13 = &data_t;
	rg.an.sgmii_reg_an_13->sgmii_remote_fault_dis = 1;
	sgmii_cmd_wo(SGMII_PORT_USB0, SGMII_REG_AN, SGMII_REG_AN_13, data_t);

	_sgmii_set_speed(SGMII_PORT_USB0, speed);

	set_xsi_data(0x1fa80000, mac_glb_cfg);
		
	printk("sgmii_api_usb0_force_sgmii: exit \n");
	return 0;
}

/**/
int sgmii_api_pon0_force_hsgmii(void)
{
	uint32 default_val = 0;
	uint32 mac_glb_cfg = 0;
	printk("sgmii_api_pon0_force_hsgmii: inint \n");
	mac_glb_cfg = get_xsi_data(0x1fa60000);
	SET_SCU_RST_RG(default_val | 1);
	SET_SCU_RST_RG(default_val & (~1));
    mdelay(10);
	//****/Walter Add
	SET_SCU_RST_RG(0x180);
	SET_SCU_RST_RG(0);
	//****/Walter Add
	/* XTAL SET ------------------------------------------------------------------ */
	//RGS_ECC_SEL[19]
	//RGS_XTAL_FREQ =1'b1 internal XTAL 25Mhz; =1'b0 internal XTAL 20Mhz
	//uint32 reg;
	//reg = mmio_read_32(0x1fa20254); 
	//reg = (reg >> 19) & 0x1;
	set_pon_phy_data(SS_LCPLL_TDC_FLT_2_ADDR, 0x64000000 ); //25Mhz
	//============================================================
	//default_val = GET_SSR3();
	//SET_SSR3((default_val & 0xFFFF9FFF) | 0x00004000); //0xe0004820 [14:13]=0
	
    SET_WAN_CONF(0xe0000011); //0x1fb00070 6 --> 11 , wan_sel as hsgmii
	
    set_pon_an_data(0x00, 0x140); //1140 -> 140 AN DIS
    //IO_SPHYA_REG_BITS( PON_SERDES_CTRL_10_ADDR, 8 , 8,  0x0);  //4228 rg_tx_bit_polarity_tmp
    set_pon_phy_data(PON_SERDES_CTRL_10_ADDR, 0x103 ); //0x3 => E2
    //IO_SPHYA_REG_BITS( FRQ_CTRL_2_ADDR , 11, 10, 0x0);  //4364 rg_hg_rx_freq_det_mux_tmp rg_hg_tx_freq_det_mux_tmp 
    set_pon_phy_data(FRQ_CTRL_2_ADDR, 0x3003 ); //3c03 -> 3003 
    //IO_SPHYA_REG_BITS( FRQ_CTRL_4_ADDR , 11,  0, 0x618);  //4364   rg_hg_tx_freq_cnt_tmp
    //IO_SPHYA_REG_BITS( FRQ_CTRL_4_ADDR , 27, 16, 0x618);  //4364   rg_hg_rx_freq_cnt_tmp
    set_pon_phy_data(FRQ_CTRL_4_ADDR, 0x15601560 ); //6180618 => E2
    //IO_SPHYA_REG_BITS( RG_SSUSB_LN0_CDR_PD_DIV_BYPASS_ADDR , 12, 12, 0x1);  //3028 RG_SSUSB_LN0_CDR_EPEN_tmp
    set_pon_phy_data(RG_SSUSB_LN0_CDR_PD_DIV_BYPASS_ADDR, 0x18001722 ); //18000722 -> 18001722
    //IO_SPHYA_REG_BITS( PON_SYS_CTRL_0_ADDR , 9,  4, 0x3c);  //4608 ck_en
    //IO_SPHYA_REG_BITS( PON_SYS_CTRL_0_ADDR , 29, 25, 0xa);  //4608 ck_sel
    set_pon_phy_data(PON_SYS_CTRL_0_ADDR, 0x250003c0 ); //150003f0 => E2
    //IO_SPHYA_REG_BITS( HG_RST_CTRL_0_ADDR , 5, 0, 0x3f);  // rst
    set_pon_phy_data(HG_RST_CTRL_0_ADDR, 0x3f ); //0 -> 3f
    //IO_SPHYA_REG_BITS( PON_DA_CTRL_2_ADDR , 25, 25, 0x0);  //RG_SSUSB_CDR_PI_PWD_tmp
    set_pon_phy_data(PON_DA_CTRL_2_ADDR, 0x54101801 ); //56101801 -> 54101801
    //IO_SPHYA_REG_BITS( PON_RXFEDIG_CTRL_12_ADDR , 22, 22, 0x0);  //RG_SSUSB_EQ_REV_tmp
    set_pon_phy_data(PON_RXFEDIG_CTRL_12_ADDR, 0x380013 ); //780013 -> 380013
    //IO_SPHYA_REG_BITS( PON_OSR_SEL_CTRL_ADDR , 1, 0, 0x1);  //
    //IO_SPHYA_REG_BITS( PON_OSR_SEL_CTRL_ADDR , 4, 4, 0x1);  //
    set_pon_phy_data(PON_OSR_SEL_CTRL_ADDR, 0x11 ); //0 -> 11
    //IO_SPHYA_REG_BITS( HG_MODE_CTRL_0_ADDR   , 0, 0, 0x1);  // rg_serdes_mode_tmp
    set_pon_phy_data(HG_MODE_CTRL_0_ADDR, 0x1 ); //0 -> 1
    //IO_SPHYA_REG_BITS( PON_SYS_CTRL_0_ADDR   ,24,24, 0x0);  // rg_rx_pma_clk_div_sel_tmp
    set_pon_phy_data(PON_SYS_CTRL_0_ADDR, 0x240003c0 ); //140003f0 => E2
	/* ---------------------------------------------------------------------------*/
    //IO_SPHYA_REG_BITS( SS_LCPLL_TDC_FLT_5_ADDR   ,24,24, 0);  // 413c
    set_pon_phy_data(SS_LCPLL_TDC_FLT_5_ADDR, 0x10100 ); //101_0100 --> 10100
    //IO_SPHYA_REG_BITS( SS_LCPLL_TDC_FLT_3_ADDR   ,8,8, 1);  // 4134
    set_pon_phy_data(SS_LCPLL_TDC_FLT_3_ADDR, 0x20000100 ); //20000000 --> 2000_0100
    //IO_SPHYA_REG_BITS( SS_LCPLL_TDC_FLT_5_ADDR   ,24,24, 1);  // 413c
    set_pon_phy_data(SS_LCPLL_TDC_FLT_5_ADDR, 0x1010100 ); //101_0100 --> 10100
    //IO_SPHYA_REG_BITS( PON_SERDES_CTRL_0_ADDR  ,0,0, 1);  // 4200
    set_pon_phy_data(PON_SERDES_CTRL_0_ADDR, 0x100381 ); //100380 -> 100381
    //udelay(500);
	set_xsi_data(0x1fa60000, mac_glb_cfg);
	
	printk("sgmii_api_pon0_force_hsgmii: exit \n");
	return 0;
}

static int sgmii_api_pon0_force_sgmii(sgmii_speed_type speed)
{
	HAL_RG_TOP rg;
	uint32 reg_value = 0;
	uint32 default_val = 0;
	uint32 mac_glb_cfg = 0;

	printk("sgmii_api_pon0_force_sgmii: ***\n");
	printk("sgmii_api_pon0_force_sgmii: ver:2021-06-28(1)\n");
	mac_glb_cfg = get_xsi_data(0x1fa60000);
	SET_SCU_RST_RG(default_val | 1);
	SET_SCU_RST_RG(default_val & (~1));
    mdelay(10);
	//****/Walter Add
	SET_SCU_RST_RG(0x180);
	SET_SCU_RST_RG(0);
	//****/Walter Add
	/* XTAL SET ------------------------------------------------------------------ */
	//RGS_ECC_SEL[19]
	//RGS_XTAL_FREQ =1'b1 internal XTAL 25Mhz; =1'b0 internal XTAL 20Mhz
	//uint32 reg;
	//reg = mmio_read_32(0x1fa20254); 
	//reg = (reg >> 19) & 0x1;
	set_pon_phy_data(SS_LCPLL_TDC_FLT_2_ADDR, 0x64000000 ); //25Mhz
	//============================================================
	default_val = GET_SSR3();
	SET_SSR3((default_val & 0xFFFF9FFF) | 0x00004000); //0xe0004820 [14:13]=0
	SET_WAN_CONF(0xe0000010); //0x1fb00070 6 --> 11 , wan_sel as hsgmii 10,SGMII

	set_pon_phy_data(HG_RG_CTRL_0_ADDR, 0x220);
	set_pon_an_data(0x00, 0x140); //1140 -> 140 AN DIS
	//IO_SPHYA_REG_BITS( PON_SERDES_CTRL_10_ADDR, 8 , 8,  0x0);  //4228 rg_tx_bit_polarity_tmp
	
	set_pon_phy_data(PON_SERDES_CTRL_10_ADDR, 0x103 ); //0x3 => E2
	//IO_SPHYA_REG_BITS( FRQ_CTRL_2_ADDR , 11, 10, 0x0);  //4364 rg_hg_rx_freq_det_mux_tmp rg_hg_tx_freq_det_mux_tmp 
	
	set_pon_phy_data(FRQ_CTRL_2_ADDR, 0x3003 ); //3c03 -> 3003 
	//IO_SPHYA_REG_BITS( FRQ_CTRL_4_ADDR , 11,	0, 0x618);	//4364	 rg_hg_tx_freq_cnt_tmp
	//IO_SPHYA_REG_BITS( FRQ_CTRL_4_ADDR , 27, 16, 0x618);	//4364	 rg_hg_rx_freq_cnt_tmp
	
	set_pon_phy_data(FRQ_CTRL_4_ADDR, 0x2710271 ); //6180618 => E2
	//IO_SPHYA_REG_BITS( RG_SSUSB_LN0_CDR_PD_DIV_BYPASS_ADDR , 12, 12, 0x1);  //3028 RG_SSUSB_LN0_CDR_EPEN_tmp
	
	set_pon_phy_data(RG_SSUSB_LN0_CDR_PD_DIV_BYPASS_ADDR, 0x18001722 ); //18000722 -> 18001722
	//IO_SPHYA_REG_BITS( PON_SYS_CTRL_0_ADDR , 9,  4, 0x3c);  //4608 ck_en
	//IO_SPHYA_REG_BITS( PON_SYS_CTRL_0_ADDR , 29, 25, 0xa);  //4608 ck_sel
	
	//set_pon_phy_data(PON_SYS_CTRL_0_ADDR, 0x250003c0 ); //150003f0 => E2
	
	//IO_SPHYA_REG_BITS( HG_RST_CTRL_0_ADDR , 5, 0, 0x3f);	// rst
	set_pon_phy_data(HG_RST_CTRL_0_ADDR, 0x3f ); //0 -> 3f
	
	//IO_SPHYA_REG_BITS( PON_DA_CTRL_2_ADDR , 25, 25, 0x0);  //RG_SSUSB_CDR_PI_PWD_tmp
	set_pon_phy_data(PON_DA_CTRL_2_ADDR, 0x54101801 ); //56101801 -> 54101801
	
	//IO_SPHYA_REG_BITS( PON_RXFEDIG_CTRL_12_ADDR , 22, 22, 0x0);  //RG_SSUSB_EQ_REV_tmp
	set_pon_phy_data(PON_RXFEDIG_CTRL_12_ADDR, 0x380013 ); //780013 -> 380013
	
	//IO_SPHYA_REG_BITS( PON_OSR_SEL_CTRL_ADDR , 1, 0, 0x1);  //
	//IO_SPHYA_REG_BITS( PON_OSR_SEL_CTRL_ADDR , 4, 4, 0x1);  //
	
	//set_pon_phy_data(PON_OSR_SEL_CTRL_ADDR, 0x11 ); //0 -> 11
	
	
	
	//IO_SPHYA_REG_BITS( HG_MODE_CTRL_0_ADDR   , 0, 0, 0x1);  // rg_serdes_mode_tmp
	set_pon_phy_data(HG_MODE_CTRL_0_ADDR, 0x1 ); //0 -> 1
	//IO_SPHYA_REG_BITS( PON_SYS_CTRL_0_ADDR   ,24,24, 0x0);  // rg_rx_pma_clk_div_sel_tmp
	
	//set_pon_phy_data(PON_SYS_CTRL_0_ADDR, 0x240003c0 ); //140003f0 => E2
	
	/* ---------------------------------------------------------------------------*/
	//IO_SPHYA_REG_BITS( SS_LCPLL_TDC_FLT_5_ADDR   ,24,24, 0);	// 413c
	//set_pon_phy_data(SS_LCPLL_TDC_FLT_5_ADDR, 0x10100 ); //101_0100 --> 10100
	//IO_SPHYA_REG_BITS( SS_LCPLL_TDC_FLT_3_ADDR   ,8,8, 1);  // 4134
	set_pon_phy_data(SS_LCPLL_TDC_FLT_3_ADDR, 0x20000100 ); //20000000 --> 2000_0100
	//IO_SPHYA_REG_BITS( SS_LCPLL_TDC_FLT_5_ADDR   ,24,24, 1);	// 413c
	set_pon_phy_data(SS_LCPLL_TDC_FLT_5_ADDR, 0x1010100 ); //101_0100 --> 10100
	//IO_SPHYA_REG_BITS( PON_SERDES_CTRL_0_ADDR  ,0,0, 1);	// 4200
	set_pon_phy_data(PON_SERDES_CTRL_0_ADDR, 0x100381 ); //100380 -> 100381
	//udelay(500);
	
	
	set_pon_phy_data(PON_SYS_CTRL_0_ADDR, 0x3f0003c0 );

	reg_value = get_pon_an_data(SGMII_REG_AN_13);
	rg.an.sgmii_reg_an_13 = &reg_value;
	rg.an.sgmii_reg_an_13->sgmii_remote_fault_dis = 1;
	set_pon_an_data(SGMII_REG_AN_13, reg_value);

	_sgmii_set_speed(SGMII_PORT_PON0, speed);

	set_xsi_data(0x1fa60000, mac_glb_cfg);

	printk("sgmii_api_pon0_force_sgmii: &&&\n");
	return 0;
}
static int sgmii_sel_flag(sgmii_port_type port)
{
	int ret;
	
	switch(port)
	{
		case SGMII_PORT_PCIE0:
			ret = get_serdes_sel(ECNT_EVENT_PCIE0_HSGMII);
			break;
		case SGMII_PORT_PCIE1:
			ret = get_serdes_sel(ECNT_EVENT_PCIE1_HSGMII);
			break;
		case SGMII_PORT_USB0:
			ret = get_serdes_sel(ECNT_EVENT_USB3_HSGMII);
			break;
		case SGMII_PORT_PON0:
			ret = get_serdes_sel(ECNT_EVENT_PON_HSGMII);
			break;
		default: //unknow format
			return 0;
			break;
	}
	
	if(ret == 1)
		//sel hsgmii
		return 1;
	else
		//close hsgmii
		return 0;
	
}

static int sgmii_force_mode(sgmii_port_type port, sgmii_speed_type speed)
{
	//check flag
	int check_sel = sgmii_sel_flag(port);

	//filter
	//unknow format, now only hsgmii = SGMII_SPEED_2500M = 0
	if(speed >= SGMII_SPEED_UNKNOW || speed <0 || check_sel !=1 )
	{	
		return -1;
	}

	if (speed != SGMII_SPEED_2500M) {
		switch(port)
		{
			case SGMII_PORT_PCIE0:
				sgmii_api_pcie0_force_sgmii(speed);
				break;
			case SGMII_PORT_PCIE1:
				sgmii_api_pcie1_force_sgmii(speed);
				break;
			case SGMII_PORT_USB0:
				sgmii_api_usb0_force_sgmii(speed);
				break;
			case SGMII_PORT_PON0:
				sgmii_api_pon0_force_sgmii(speed);
				break;
			default: //unknow format
				return -1;
				break;	
		}
	} else {
		switch(port)
		{
			case SGMII_PORT_PCIE0:
				sgmii_api_pcie0_force_hsgmii();
				break;
			case SGMII_PORT_PCIE1:
				sgmii_api_pcie1_force_hsgmii();
				break;
			case SGMII_PORT_USB0:
				sgmii_api_usb0_force_hsgmii();
				break;
			case SGMII_PORT_PON0:
				sgmii_api_pon0_force_hsgmii();
				break;
			default: //unknow format
				return -1;
				break;
		}
	}
	return 0;
}

/**/
int sgmii_api_force_mode(sgmii_port_type port, sgmii_speed_type speed)
{
	/*ex. ---------------------------------------------------
	int port = 0; //{pcie0=0, pcie1, usb0, pon0};
	int speed = 0; //{HSGMII=0, SGMII, RA100M, RA10M};
	int ret = sgmii_api_force_mode(0, speed);
	if(ret == 1)
		printk("set_done!\n");
	else
		printk("fail!\n");
	// ---------------------------------------------------*/
	uint32 default_val = GET_SCU_RST_RG();
	int ret = 0;

	if(port == SGMII_PORT_PON0) {
		ret = sgmii_force_mode(port, speed);
		set_pon_an_data(0x00,0x8140);
		udelay(10);
		set_pon_an_data(0x00,0x140);
		sgmii_int_enable(port, false);
	} else {
		sgmii_int_enable(port, false);
		ret = sgmii_force_mode(port, speed);
		sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00008140);
		udelay(10);
		sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00000140);
	}

	return ret;
}
EXPORT_SYMBOL(sgmii_api_force_mode);

static int sgmii_int_enable(sgmii_port_type port, bool enable)
{
	HAL_RG_TOP rg;
	uint32 data;

	if (enable) {
		if (port == SGMII_PORT_PON0) {
			//set_pon_pcs1_data(PON_INT_STA_CLR, 0x3ff);
			set_pon_pcs1_data(PON_INT_EN, 0x22);
		} else {
			/* clear int */
			data = 0x00000000;
			rg.pcs2.rg_hsgmii_mode_interrupt = &data;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_done_int                  =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_done_int             =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_cl37_timerdone_int        =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_loss_int             =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_done_int_clear            =0x1;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_done_int_clear       =0x1;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_cl37_timerdone_int_clear  =0x1;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_loss_int_clear       =0x1;
			sgmii_cmd_wo(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_MODE_INTERRUPT, data);

			/* set int */
			data = 0x00000000;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_done_int                  =0x1;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_done_int             =0x1;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_cl37_timerdone_int        =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_loss_int             =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_done_int_clear            =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_done_int_clear       =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_cl37_timerdone_int_clear  =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_loss_int_clear       =0x0;
			sgmii_cmd_wo(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_MODE_INTERRUPT, data);
		}
	} else {
		if (port == SGMII_PORT_PON0) {
			set_pon_pcs1_data(PON_INT_STA_CLR, 0x3ff);
			set_pon_pcs1_data(PON_INT_EN, 0x0);
		} else {
			/* clear int */
			data = 0x00000000;
			rg.pcs2.rg_hsgmii_mode_interrupt = &data;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_done_int                  =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_done_int             =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_cl37_timerdone_int        =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_loss_int             =0x0;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_done_int_clear            =0x1;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_done_int_clear       =0x1;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_an_cl37_timerdone_int_clear  =0x1;
			rg.pcs2.rg_hsgmii_mode_interrupt->rg_hsgmii_mode2_rx_sync_loss_int_clear       =0x1;
			sgmii_cmd_wo(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_MODE_INTERRUPT, data);
		}
	}

	return 0;
}

int sgmii_api_an_force_mode(sgmii_port_type port, sgmii_speed_type speed, bool an_enable)
{
	u32 default_val = GET_SCU_RST_RG();
	u8 status = 0;
	uint32 data_t, speed_t;
	HAL_rg_hsgmii_mode_interrupt rg_hsgmii_mode_interrupt = {0};

	printk("sgmii_api_an_force_mode\n");

	if(speed >= 10)
		speed %= 10;

	if(port == SGMII_PORT_PON0) {
	    if(an_enable == 1) {
			if (speed == SGMII_SPEED_AN) {
				printk("AN_Mode \n");
				sgmii_force_mode(port, SGMII_SPEED_1000M);
				printk("[%d]%s: PON_INT_EN: %x\n",__LINE__, __func__, get_pon_pcs1_data(PON_INT_EN));
				set_pon_an_data(0x00,0x9340);
				udelay(10);
				set_pon_an_data(0x00,0x1140);
				sgmii_int_enable(port, false);
				printk("[%d]%s: PON_INT_EN: %x\n",__LINE__, __func__, get_pon_pcs1_data(PON_INT_EN));
			} else {
				printk("AN_Force_Mode \n");
				status = sgmii_api_force_mode(port,speed);
				set_pon_an_data(0x00,0x9340);
				udelay(10);
				set_pon_an_data(0x00,0x1140);
			}
		} else {
			printk("Force_Mode \n");
			status = sgmii_api_force_mode(port,speed);
			set_pon_an_data(0x00,0x8140);
	    	udelay(10);
			set_pon_an_data(0x00,0x140);
		}
		//****/Walter Add 
	} else {
		if(an_enable == 1) {
			if (speed == SGMII_SPEED_AN) {
				sgmii_int_enable(port, true);
				sgmii_force_mode(port, SGMII_SPEED_1000M);
				sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00009140);
				udelay(10);
				sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00001140);
				printk("AN_Mode SGMII\n");
			} else {
				sgmii_int_enable(port, false);
				sgmii_force_mode(port, speed);
				sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00009140);
				udelay(10);
				sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00001140);
				printk("AN_Force_Mode SGMII\n");
			}
		}
	}
		
	return status;
}
EXPORT_SYMBOL(sgmii_api_an_force_mode);


int sgmii_api_set_speed(sgmii_port_type port, sgmii_speed_type speed)
{
	u32 default_val = GET_SCU_RST_RG();
	u8 status = 0;

	if (port < SGMII_PORT_PCIE0 || port >= SGMII_PORT_UNKNOW) {
		printk("[%s] Port %d not supported.\n", __func__, port);
		return -1;
	}

	if (speed < SGMII_SPEED_2500M || speed >= SGMII_SPEED_UNKNOW) {
		printk("[%s] Speed %d not supported.\n", __func__, speed);
		return -1;
	}

	if(port == SGMII_PORT_PON0) {
		if (speed == SGMII_SPEED_AN) {
			printk("[%s] Set port %d to AN mode\n", __func__, port);
			status = sgmii_force_mode(port, SGMII_SPEED_1000M);
			set_pon_an_data(0x00,0x9340);
			udelay(10);
			set_pon_an_data(0x00,0x1140);
			sgmii_int_enable(port, true);
		} else {
			printk("[%s] Set port %d to Force mode speed %d\n", __func__, port, speed);
			status = sgmii_force_mode(port, speed);
			set_pon_an_data(0x00,0x8140);
			udelay(10);
			set_pon_an_data(0x00,0x140);
			sgmii_int_enable(port, false);
		}
	} else {
		if (speed == SGMII_SPEED_AN) {
			printk("[%s] Set port %d to AN mode\n", __func__, port);
			sgmii_int_enable(port, true);
			status = sgmii_force_mode(port, SGMII_SPEED_1000M);
			sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00009140);
			udelay(10);
			sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00001140);
		} else {
			printk("[%s] Set port %d to Force mode speed %d\n", __func__, port, speed);
			sgmii_int_enable(port, false);
			status = sgmii_force_mode(port, speed);
			sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00008140);
			udelay(10);
			sgmii_cmd_wo(port, SGMII_REG_AN, SGMII_REG_AN0, 0x00000140);
		}
	}

	return status;
}
EXPORT_SYMBOL(sgmii_api_set_speed);

int sgmii_api_reset(sgmii_port_type port, int reset_type)	   // For PON port debug, Walter
{
	u32 default_val = GET_SCU_RST_RG();
	printk("Reset Function In ! \n");

	if (port == SGMII_PORT_PON0) {
        if(reset_type == 1) {
		    SET_SCU_RST_RG(default_val | 1);
			SET_SCU_RST_RG(default_val & (~1));
			mdelay(10);
        } else if(reset_type == 180) {
			SET_SCU_RST_RG(0x180);
			SET_SCU_RST_RG(0x0);
			mdelay(10);
		} else {
			printk("Error Command! Try value 0 or 180 \n");
			return -1;
		}
	}
	return 0;
}
EXPORT_SYMBOL(sgmii_api_reset);

/**/
int sgmii_api_get_info(sgmii_port_type port, int *argc, int *argv[])
{
	 
	/*ex. ---------------------------------------------------
	int argc=6, sync, an, link_sts, an_enable, dump, sgmii_speed;
	int *argv[] = {&sync, &an, &link_sts, &an_enable, &dump, &sgmii_speed};
	int port = 0; //{pcie0=0, pcie1, usb0, pon0};
	int ret = sgmii_api_get_info(0, &argc, argv);
	if(ret == 0 && link_sts == 1)
		printk("linkup\n");
	else
		printf("linkdown\n");
	// ---------------------------------------------------*/ 
	
	//check flag
	int check_sel = sgmii_sel_flag(port);
	uint32 link = 0;
	uint32 sgmii_speed = _sgmii_get_speed(port);
	
	//filter
	//unknow format, now only hsgmii = SGMII_SPEED_2500M = 0
	if (port >= SGMII_PORT_UNKNOW || port < 0 || check_sel != 1) {
		printk("[%s] Port type %d not supported\n", __func__, port);
		return -1;
	}

	if (*argc == NULL || *argc < 6) {
		printk("[%s] Invalid argc\n", __func__);
		return -1;
	}

	*argc = 6;
	link = _sgmii_get_link(port);
	//ro_rxdump <<4 + ro_an_done<<1 + ro_sync<<0;
	*argv[0] = link>>0 & 0x1;		//[5]sync
	*argv[1] = link>>1 & 0x1; 		//[0]an
	*argv[2] = link>>2 & 0x1; 		//link_sts
	*argv[3] = link>>3 & 0x1; 		//an_enable
	*argv[4] = link>>4 & 0xf;		//dump
	*argv[5] = sgmii_speed;		    //sgmii speed
	
	return 0;
}
EXPORT_SYMBOL(sgmii_api_get_info);

int sgmii_api_get_serdes_counter(sgmii_port_type port, int *argc, unsigned int *argv[])
{
	/*ex. ---------------------------------------------------
	int argc = 12;
	int rx_fb, rx_fd, rx_err, rx_idle, rx_seq, rx_fbo, rx_fdo;
	int tx_fb, tx_fd, tx_err, tx_idle, tx_seq;
	int *argv[] = {&rx_fb, &rx_fd, &rx_err, &rx_idle, &rx_seq, &rx_fbo, &rx_fdo,
					&tx_fb, &tx_fd, &tx_err, &tx_idle, &tx_seq};
	int port = 0; //{pcie0=0, pcie1, usb0, pon0};
	int ret = sgmii_api_get_counter(0, &argc, argv);
	// ---------------------------------------------------*/

	//check flag
	int check_sel = sgmii_sel_flag(port);

	//filter
	//unknow format, now only hsgmii = SGMII_SPEED_2500M = 0
	if (port >= SGMII_PORT_UNKNOW || port < 0 || check_sel != 1) {
		printk("[%s] Port type %d not supported\n", __func__, port);
		return -1;
	}

	if (*argc == NULL || *argc < 12) {
		printk("[%s] Invalid argc\n", __func__);
		return -1;
	}

	*argc = 12;
	if (port == SGMII_PORT_PON0) {
		*argv[0] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_1);		//rx fb
		*argv[1] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_2);		//rx fd
		*argv[2] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_3);		//rx err
		*argv[3] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_4);		//rx idle
		*argv[4] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_5);		//rx seq
		*argv[5] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_6);		//rx fbo
		*argv[6] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_7);		//rx fdo
		*argv[7] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_8); 	//tx fb
		*argv[8] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_9); 	//tx fd
		*argv[9] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_10); 	//tx err
		*argv[10] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_11);	//tx idle
		*argv[11] = get_pon_pcs2_data(SGMII_RG_HSGMII_GPII_STATE_12);	//tx seq
	} else {
		*argv[0] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_1);	//rx fb
		*argv[1] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_2); 	//rx fd
		*argv[2] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_3); 	//rx err
		*argv[3] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_4); 	//rx idle
		*argv[4] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_5);	//rx seq
		*argv[5] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_6);	//rx fbo
		*argv[6] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_7);	//rx fdo
		*argv[7] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_8); 	//tx fb
		*argv[8] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_9); 	//tx fd
		*argv[9] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_10); 	//tx err
		*argv[10] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_11);	//tx idle
		*argv[11] = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_GPII_STATE_12);	//tx seq
	}

	return 0;
}
EXPORT_SYMBOL(sgmii_api_get_serdes_counter);

int sgmii_api_clear_serdes_rx_counter(sgmii_port_type port)
{
	HAL_RG_TOP rg;
	uint32 data;

	//check flag
	int check_sel = sgmii_sel_flag(port);

	//filter
	//unknow format, now only hsgmii = SGMII_SPEED_2500M = 0
	if (port >= SGMII_PORT_UNKNOW || port < 0 || check_sel != 1) {
		printk("[%s] Port type %d not supported\n", __func__, port);
		return -1;
	}

	if (port == SGMII_PORT_PON0) {
		data = get_pon_pcs2_data(SGMII_RG_HSGMII_PCS_GPII_0);
		rg.pcs2.rg_hsgmii_pcs_gpii_0 = &data;
		rg.pcs2.rg_hsgmii_pcs_gpii_0->rg_gpii_cnt_clr_rx_pma =0x1;
		rg.pcs2.rg_hsgmii_pcs_gpii_0->rg_gpii_cnt_clr_rx_pcs =0x1;
		set_pon_pcs2_data(SGMII_RG_HSGMII_PCS_GPII_0, data);
		rg.pcs2.rg_hsgmii_pcs_gpii_0->rg_gpii_cnt_clr_rx_pma =0x0;
		rg.pcs2.rg_hsgmii_pcs_gpii_0->rg_gpii_cnt_clr_rx_pcs =0x0;
		set_pon_pcs2_data(SGMII_RG_HSGMII_PCS_GPII_0, data);
	} else {
		data = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_GPII_0);
		rg.pcs2.rg_hsgmii_pcs_gpii_0 = &data;
		rg.pcs2.rg_hsgmii_pcs_gpii_0->rg_gpii_cnt_clr_rx_pma =0x1;
		rg.pcs2.rg_hsgmii_pcs_gpii_0->rg_gpii_cnt_clr_rx_pcs =0x1;
		sgmii_cmd_wo(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_GPII_0, data);
		rg.pcs2.rg_hsgmii_pcs_gpii_0->rg_gpii_cnt_clr_rx_pma =0x0;
		rg.pcs2.rg_hsgmii_pcs_gpii_0->rg_gpii_cnt_clr_rx_pcs =0x0;
		sgmii_cmd_wo(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_GPII_0, data);
	}

	return 0;
}
EXPORT_SYMBOL(sgmii_api_clear_serdes_rx_counter);

int sgmii_api_mode(sgmii_api_method_type method, sgmii_port_type port, sgmii_speed_type speed)
{
	//sgmii mode set pcie0 0/1/2/3
	
	
	if(port == SGMII_PORT_UNKNOW || method == SGMII_API_METHOD_UNKNOW || (method == SGMII_API_METHOD_SET  && speed == SGMII_SPEED_UNKNOW)){
		printk("SGMII_API_MODE:UNKNOW\n");
		return 0;
	}
	
	//printk("sgmii_api_mode: \n");
	//printk("method:%d, port:%d, speed:%d\n",method, port, speed);
	
	
	switch(method){
		case SGMII_API_METHOD_GET:
			printk("sgmii_mode_get:\n");
			break;
		case SGMII_API_METHOD_SET:
			_sgmii_set_speed(port, speed);
			break;
	}
	
	_sgmii_get_speed(port);
	
	return 0;
}
EXPORT_SYMBOL(sgmii_api_mode);

int sgmii_api_info(sgmii_api_method_type method, sgmii_port_type port)
{
	//sgmii info get pcie0
	if(port == SGMII_PORT_UNKNOW || method == SGMII_API_METHOD_UNKNOW){
		printk("SGMII_API_INFO:UNKNOW\n");
		return 0;
	}
	
	//printk("sgmii_api_mode: \n");
	//printk("method:%d, port:%d\n",method, port);
	
	
	switch(method){
		case SGMII_API_METHOD_GET:
			printk("sgmii_mode_get:\n");
			_sgmii_get_speed(port);
			_sgmii_get_link(port);
			break;
		case SGMII_API_METHOD_SET:
			printk("sgmii_mode_set: only get\n");
			break;
	}
	
	return 0;
}
EXPORT_SYMBOL(sgmii_api_info);

/*INTERNAL -------------------------- */ 
static int _sgmii_get_link(sgmii_port_type port)
{
	HAL_RG_TOP rg;
	uint32 data_t;
	uint32 ro_an_done;
	uint32 ro_sync;
	uint32 ro_rxdump;
	uint32 ro_txdump;
	uint32 ro_an_1;
	uint32 link_status;
	uint32 an_enable;
	uint32 ls_speed = SGMII_SPEED_UNKNOW;
	int v_reg;
	
	//RO
	if(port == SGMII_PORT_PON0) {
		data_t = get_pon_an_data(SGMII_REG_AN0);
		rg.an.sgmii_reg_an0 = &data_t;
		an_enable = rg.an.sgmii_reg_an0->sgmii_an_enable;
		data_t = get_pon_pcs2_data(SGMII_RG_HSGMII_PCS_STATE_2); //0xb04 pcs2_base
		rg.pcs2.rg_hsgmii_pcs_state_2 = &data_t;
		ro_sync    = rg.pcs2.rg_hsgmii_pcs_state_2->ro_rx_sync;
		ro_an_done = rg.pcs2.rg_hsgmii_pcs_state_2->ro_an_done;
		data_t = get_pon_an_data(SGMII_REG_AN_5);
		ro_rxdump    = data_t;
		data_t = get_pon_an_data(SGMII_REG_AN_4);
		ro_txdump    = data_t;
		data_t = get_pon_an_data(SGMII_REG_AN_1);
		ro_an_1      = data_t;
	} else {
		data_t = sgmii_cmd_ro(port, SGMII_REG_AN, SGMII_REG_AN0);
		rg.an.sgmii_reg_an0 = &data_t;
		an_enable = rg.an.sgmii_reg_an0->sgmii_an_enable;

		data_t = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_STATE_2);
		rg.pcs2.rg_hsgmii_pcs_state_2 = &data_t;
		ro_sync    = rg.pcs2.rg_hsgmii_pcs_state_2->ro_rx_sync;
		ro_an_done = rg.pcs2.rg_hsgmii_pcs_state_2->ro_an_done;

		data_t = sgmii_cmd_ro(port, SGMII_REG_AN, SGMII_REG_AN_5);
		ro_rxdump    = data_t;

		data_t = sgmii_cmd_ro(port, SGMII_REG_AN, SGMII_REG_AN_4);
		ro_txdump    = data_t;

		data_t = sgmii_cmd_ro(port, SGMII_REG_AN, SGMII_REG_AN_1);
		ro_an_1    = data_t;
	}

	if ((ro_txdump & 0x1) == 0x1) {
		link_status = (ro_rxdump & 0x8000) >> 15;
	} else {
		//link_status = (ro_an_1 & 0x0004) >> 2;
		link_status = ro_sync & ro_an_done;
	}

	printk("PORT:%s, SYNC:%d, AN:%d, TXAN:%x, RXAN:%x, LINK_STS:%d\n", sgmii_port_name[port], ro_sync, ro_an_done, ro_txdump, ro_rxdump, link_status);

	v_reg = (ro_rxdump << 4) + (an_enable << 3) + (link_status << 2) + (ro_an_done << 1) + (ro_sync << 0);

	return v_reg;
}


static int _sgmii_set_speed(sgmii_port_type port, sgmii_speed_type speed)
{
	HAL_RG_TOP rg;
	uint32 data_t;
	uint32 wo_speed;
	uint32 wo_rateadapt;
	uint32 wo_ra_bypass;
	uint32 wo_an_en;
	uint32 wo_ra_en;
	uint32 default_val = 0;

	printk("sgmii_set_speed port = %x, speed = %x\n",port,speed);
	switch(speed){
		case SGMII_SPEED_2500M:
			wo_speed     = 1;
			wo_rateadapt = 0;
			wo_ra_bypass = 1;
			wo_an_en     = 0;
			wo_ra_en     = 0;
			break;
		case SGMII_SPEED_1000M:
			wo_speed 	 = 0;
			wo_rateadapt = 0;
			wo_ra_bypass = 1;
			wo_an_en     = 1;
			wo_ra_en     = 1;
			break;
		case SGMII_SPEED_100M:
			wo_speed     = 0;
			wo_rateadapt = 1;
			wo_ra_bypass = 0;
			wo_an_en     = 1;
			wo_ra_en     = 1;
			break;
		case SGMII_SPEED_10M:
			wo_speed     = 0;
			wo_rateadapt = 2;
			wo_ra_bypass = 0;
			wo_an_en     = 1;
			wo_ra_en     = 1;
			break;			
		default:
			printk("sgmii_set_speed:fail\n");
			return 0;	
		
	}
	
	//RW
	//data_t = sgmii_cmd_ro(port, SGMII_REG_PHYA, SGMII_REG_PHYA_11);
	//rg.phya.sgmii_reg_phya_11 = &data_t;
	//rg.phya.sgmii_reg_phya_11->rg_tphy_speed = wo_speed;
	//sgmii_cmd_wo(port, SGMII_REG_PHYA, SGMII_REG_PHYA_11, data_t);

#if 0
	data_t = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6);
	rg.pcs2.rg_hsgmii_pcs_ctrol_6 = &data_t;
	rg.pcs2.rg_hsgmii_pcs_ctrol_6->rg_sgmii_force_rateadapt_value= wo_rateadapt;
	sgmii_cmd_wo(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6, data_t);

	data_t = sgmii_cmd_ro(port, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0);
	rg.ra.rg_rate_adapt_ctrl_0 = &data_t;
	rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_tx_bypass= wo_ra_bypass;
	rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_rx_bypass= wo_ra_bypass;
	rg.ra.rg_rate_adapt_ctrl_0-> rg_rate_adapt_tx_en= wo_ra_en;
	rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_rx_en= wo_ra_en;
	sgmii_cmd_wo(port, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0, data_t);

	data_t = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_1);
	rg.pcs2.rg_hsgmii_pcs_ctrol_1 = &data_t;
	rg.pcs2.rg_hsgmii_pcs_ctrol_1->rg_an_enable= wo_an_en;
	sgmii_cmd_wo(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_1, data_t);
#else
    if (port != SGMII_PORT_PON0){
        data_t = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6);
        rg.pcs2.rg_hsgmii_pcs_ctrol_6 = &data_t;
        rg.pcs2.rg_hsgmii_pcs_ctrol_6->rg_sgmii_force_rateadapt_value= wo_rateadapt;
        sgmii_cmd_wo(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6, data_t);

        data_t = sgmii_cmd_ro(port, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0);
        rg.ra.rg_rate_adapt_ctrl_0 = &data_t;
        rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_tx_bypass= wo_ra_bypass;
        rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_rx_bypass= wo_ra_bypass;
        rg.ra.rg_rate_adapt_ctrl_0-> rg_rate_adapt_tx_en= wo_ra_en;
        rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_rx_en= wo_ra_en;
        sgmii_cmd_wo(port, SGMII_REG_RATEADAPT, SGMII_RG_RATE_ADAPT_CTRL_0, data_t);

        data_t = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_1);
        rg.pcs2.rg_hsgmii_pcs_ctrol_1 = &data_t;
        rg.pcs2.rg_hsgmii_pcs_ctrol_1->rg_an_enable= wo_an_en;
        sgmii_cmd_wo(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_1, data_t);
    } else {
        //****/Walter Add
        default_val = GET_SSR3();

        // maybe no necessary
        SET_SSR3((default_val & 0xF3FFFFFF) | (wo_rateadapt << 26));      // 100M : 0xe4004820 , 10M : 0xe8004820 , 1G : 0xe0004820

        set_pon_phy_data(HG_MODE_CTRL_0_ADDR, 0x1 | (wo_rateadapt << 4)); // 100M : 0x11, 10M : 0x21, 1G : 0x1
        //****/Walter Add

        // maybe no necessary
        data_t = get_pon_pcs2_data(SGMII_RG_HSGMII_PCS_CTROL_6);
        rg.pcs2.rg_hsgmii_pcs_ctrol_6 = &data_t;
        rg.pcs2.rg_hsgmii_pcs_ctrol_6->rg_sgmii_force_rateadapt_value= wo_rateadapt;
        set_pon_pcs2_data(SGMII_RG_HSGMII_PCS_CTROL_6, data_t);

        data_t = get_pon_ra_data(SGMII_RG_RATE_ADAPT_CTRL_0);
        rg.ra.rg_rate_adapt_ctrl_0 = &data_t;
        rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_tx_bypass= wo_ra_bypass;
        rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_rx_bypass= wo_ra_bypass;
        rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_tx_en= wo_ra_en;
        rg.ra.rg_rate_adapt_ctrl_0->rg_rate_adapt_rx_en= wo_ra_en;
        set_pon_ra_data(SGMII_RG_RATE_ADAPT_CTRL_0, data_t);

        // maybe no necessary
        data_t = get_pon_pcs2_data(SGMII_RG_HSGMII_PCS_CTROL_1);
        rg.pcs2.rg_hsgmii_pcs_ctrol_1 = &data_t;
        rg.pcs2.rg_hsgmii_pcs_ctrol_1->rg_an_enable= wo_an_en;
        set_pon_pcs2_data(SGMII_RG_HSGMII_PCS_CTROL_1, data_t);
    }
#endif

	return 0;
}


static sgmii_speed_type _sgmii_get_speed(sgmii_port_type port)
{
	HAL_RG_TOP rg;
	uint32 data_t;
	uint32 ro_speed_lsb=0;
	uint32 ro_rateadapt;
	
	if (port != SGMII_PORT_PON0) {
		//RO
		data_t = sgmii_cmd_ro(port, SGMII_REG_PHYA, SGMII_REG_PHYA_11);
		rg.phya.sgmii_reg_phya_11 = &data_t;
		ro_speed_lsb = rg.phya.sgmii_reg_phya_11->rg_tphy_speed;

		data_t = sgmii_cmd_ro(port, SGMII_REG_PCS2, SGMII_RG_HSGMII_PCS_CTROL_6);
		rg.pcs2.rg_hsgmii_pcs_ctrol_6 = &data_t;
		ro_rateadapt = rg.pcs2.rg_hsgmii_pcs_ctrol_6->rg_sgmii_force_rateadapt_value;
	} else {
		data_t = GET_WAN_CONF();
		ro_speed_lsb = data_t & 0x1;

		data_t = get_pon_pcs2_data(SGMII_RG_HSGMII_PCS_CTROL_6);
		rg.pcs2.rg_hsgmii_pcs_ctrol_6 = &data_t;
		ro_rateadapt = rg.pcs2.rg_hsgmii_pcs_ctrol_6->rg_sgmii_force_rateadapt_value;
	}
	
	if(ro_speed_lsb == 1){
		printk("PORT:%s, SPEED:%s\n",sgmii_port_name[port], sgmii_speed_name[SGMII_SPEED_2500M]);
		return SGMII_SPEED_2500M;
	}
	else if(ro_speed_lsb == 0 && ro_rateadapt == 0){
		printk("PORT:%s, SPEED:%s\n",sgmii_port_name[port], sgmii_speed_name[SGMII_SPEED_1000M]);
		return SGMII_SPEED_1000M;
	}
	else if(ro_speed_lsb == 0 && ro_rateadapt == 1){
		printk("PORT:%s, SPEED:%s\n",sgmii_port_name[port], sgmii_speed_name[SGMII_SPEED_100M]);
		return SGMII_SPEED_100M;
	}
	else if(ro_speed_lsb == 0 && ro_rateadapt == 2){
		printk("PORT:%s, SPEED:%s\n",sgmii_port_name[port], sgmii_speed_name[SGMII_SPEED_10M]);
		return SGMII_SPEED_10M;
	}
	else{
		printk("sgmii_api_mode:ERROR!\n");
		printk("ro_speed_lsb:%d, ro_rateadapt:%d\n", ro_speed_lsb, ro_rateadapt);
		return SGMII_SPEED_UNKNOW;
	}
}


static sgmii_api_method_type _covert_method(char* method_str)
{
	sgmii_api_method_type method;
	
	if (strncmp(method_str, "get",3)==0)
		method = SGMII_API_METHOD_GET;
	else if (strncmp(method_str, "set",3)==0)
		method = SGMII_API_METHOD_SET;
	else
		method = SGMII_API_METHOD_UNKNOW;
	
	return method;
}

static sgmii_port_type _covert_port(char* port_str)
{
	sgmii_port_type port;
	
	if (strncmp(port_str, "pcie0",5)==0)
		port = SGMII_PORT_PCIE0;
	else if (strncmp(port_str, "pcie1",5)==0)
		port = SGMII_PORT_PCIE1;
	else if (strncmp(port_str, "usb0",4)==0)
		port = SGMII_PORT_USB0;
	else
		port = SGMII_PORT_UNKNOW;
	
	return port;
}

static sgmii_speed_type _covert_speed(char* speed_str)
{
	sgmii_speed_type speed;
	
	if (strncmp(speed_str, "2500",4)==0)
		speed = SGMII_SPEED_2500M;
	else if (strncmp(speed_str, "1000",4)==0)
		speed = SGMII_SPEED_1000M;
	else if (strncmp(speed_str, "100",3)==0)
		speed = SGMII_SPEED_100M;
	else if (strncmp(speed_str, "10",2)==0)
		speed = SGMII_SPEED_10M;
	else
		speed = SGMII_SPEED_UNKNOW;
	return speed;
}


static sgmii_api_type _covert_api(char* api_str)
{
	sgmii_api_type api;
	//printk("api_str\n");
	if (strncmp(api_str, "mode",4)==0)
		api = SGMII_API_MODE;
	else if (strncmp(api_str, "info",4)==0)
		api = SGMII_API_INFO;
	else if (strncmp(api_str, "test",4)==0)
		api = SGMII_API_TEST;
	else
		api = SGMII_API_UNKNOW;
	//printk("api:%d\n",api);
	return api;
}

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
//int sgmii_api(unsigned long *reg, unsigned long *value)
int sgmii_api(int argc, char *argv[], void *p)
{
	//sgmii mode set pcie0 0/1/2/3
	//sgmii mode get pcie0
	//sgmii info get pcie0
	//
	//printk("sgmii_api:inin\n");
	
	
	unsigned long reg=0;
	unsigned long value=0;
	sgmii_port_type port;
	sgmii_speed_type speed;
	sgmii_api_method_type method;
	sgmii_api_type api;
	int flag;
	if(argc<1)
	{
		printk("sgmii_api:error: input:mode/info \n");
		return 0;
	}
	api = _covert_api(argv[1]);
	//printk("top_api:%d\n",api);
	switch(api)
	{
		case SGMII_API_VERSION:
				sgmii_ver();//EN7523_YT_20211020_001
			break;
		case SGMII_API_TEST:
				printk("SGMII_API_TEST\n");
				
				sgmii_api_pon0_force_hsgmii();
				
				//
				//reg = (unsigned long)simple_strtoul(argv[1], NULL, 16);
				//value = (unsigned long)simple_strtoul(argv[2], NULL, 16);
				//printk("%lx:%lx\n",reg, value);
				//flag = sgmii_test(&reg, &value);
			break;
		case SGMII_API_MODE:
				//printk("SGMII_API_MODE\n");
				if(argc<4){
					printk("sgmii_api:error: ex. sgmii mode set pcie0 2500M \n");return 0;
				}
				else if(argc<5){
					speed = SGMII_SPEED_UNKNOW;
					printk("sgmii_api:speed_empty.\n");
				}
				else{
					speed = _covert_speed(argv[4]);
				}
				method = _covert_method(argv[2]);
				port = _covert_port(argv[3]);
				flag = sgmii_api_mode(method, port, speed);
			break;
		case SGMII_API_INFO:
		
				//printk("SGMII_API_INFO\n");
				if(argc<3){
					printk("sgmii_api:error: ex. sgmii info get pcie0\n");return 0;
				}
				method = _covert_method(argv[2]);
				port = _covert_port(argv[3]);
				flag = sgmii_api_info(method, port);
			break;
		default:
			printk("sgmii_api:UNKNOW?? \n");
			break;
		
	}
	
	
	//printk("sgmii_api:exit\n");
	return 0;
}


EXPORT_SYMBOL(sgmii_api);

void sgmii_ver(void)
{
	printk("SGMII_Version : %s\n",SGMII_VERSION);
}
EXPORT_SYMBOL(sgmii_ver);




