#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#else
#include <drivers/delay_timer.h>
#include <asm/tc3162.h>
#endif
#include <asm/io.h>
#include <modules/avs/avs.h>


//RG address, base: 0x1fa20000
#define PLLRG_PROTECT      0x268
#define TADC_RST_OFST      0x4
#define TADC_MUX_OFST      0x1
#define TADC_SET_MASK      0xf3ff
#define TADC_MUX_MASK      0x07
//TADC
#define MUX_TADC           0x2ec
#define DOUT_TADC          0x2f8
//TADC_CPU
#define MUX_TADC_CPU       0x2fc
#define DOUT_TADC_CPU      0x308

#define PROTECT_KEY        0x12

//not sure
#define SCU_XTAL_SEL       0x254

static u16 ADC_SEL_AVS0_CPU1 = 1;
static u16 ADC_MUX_SEL_RES0_DIO1 = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
extern u32 get_chip_scu_data(u32 reg);
extern void set_chip_scu_data(u32 reg, u32 val);
#else
#define get_chip_scu_data(reg) readl((0x1fa20000 + reg))
#define set_chip_scu_data(reg, val) writel(val, (0x1fa20000 + reg))
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)

void thermal_TADC_mode(u16 tadc0_tcpu1,u16 res0_diode1,u16 delayCnt);
u32 rw_chip_scu_data(u32 addr,u32 value,u32 mask,u16 ofst)
{
	u32 newValue=(get_chip_scu_data(addr) &(~(mask <<ofst)) ) | ((value&mask)<<ofst);
	set_chip_scu_data(addr, newValue);
	return newValue;
}


//------------------thermal sensor API----------------
void thermal_init(void)
{
    u32 temp=0;
    ADC_SEL_AVS0_CPU1 = 1;
    ADC_MUX_SEL_RES0_DIO1 = 0;
    printk("thermal SCU version: 7581.1.202201011\n");
    //enable write protect reg, set 0x1fa20264 = 0x80
    temp=get_chip_scu_data(PLLRG_PROTECT);
    set_chip_scu_data(PLLRG_PROTECT,  PROTECT_KEY);
    //set TADC related registers
    //reset TADC, 0x1fa202ec = [4] = 0, 0x1fa202ec = [4] = 1
    rw_chip_scu_data(MUX_TADC,0,1,TADC_RST_OFST);
	rw_chip_scu_data(MUX_TADC,1,1,TADC_RST_OFST);
    //set TADC mode
    rw_chip_scu_data(MUX_TADC,0x0390,TADC_SET_MASK,0);
	
	//TADC_CPU
	rw_chip_scu_data(MUX_TADC_CPU,0,1,TADC_RST_OFST);
	rw_chip_scu_data(MUX_TADC_CPU,1,1,TADC_RST_OFST);
    //set TADC mode
	//default Resistor,0x33fe for diode
	rw_chip_scu_data(MUX_TADC_CPU,0x0390,TADC_SET_MASK,0);
    thermal_TADC_mode(0,0,10);
    set_chip_scu_data(PLLRG_PROTECT,  temp);
    
}
EXPORT_SYMBOL(thermal_init);

void thermal_TADC_mode(u16 tadc0_tcpu1,u16 res0_diode1,u16 delayCnt)
{   
    u32 temp=0;
	u32 addr = MUX_TADC;
    temp=get_chip_scu_data(PLLRG_PROTECT);
    set_chip_scu_data(PLLRG_PROTECT,  PROTECT_KEY);
    
	if(tadc0_tcpu1)
		addr = MUX_TADC_CPU;
	
	if(res0_diode1==1)
		rw_chip_scu_data(addr,0x7,TADC_MUX_MASK,TADC_MUX_OFST);
	else
		rw_chip_scu_data(addr,0x0,TADC_MUX_MASK,TADC_MUX_OFST);
	ADC_SEL_AVS0_CPU1 = tadc0_tcpu1;
    ADC_MUX_SEL_RES0_DIO1 = res0_diode1;
    
    set_chip_scu_data(PLLRG_PROTECT,  temp);
    
    mdelay(delayCnt);
    
}
EXPORT_SYMBOL(thermal_TADC_mode);

u32 GetTADC_Sel(void) //H: ADC_SEL_AVS0_CPU1 // L:ADC_MUX_SEL_RES0_DIO1
{
	u32 ret = ADC_SEL_AVS0_CPU1;
	
	return (ret<<16)+ADC_MUX_SEL_RES0_DIO1;
}
EXPORT_SYMBOL(GetTADC_Sel);

u32 get_thermal_ADC(void)
{   
	if(ADC_SEL_AVS0_CPU1)
		return get_chip_scu_data(DOUT_TADC_CPU);
	return get_chip_scu_data(DOUT_TADC);
}
EXPORT_SYMBOL(get_thermal_ADC);

u32 get_xtal_sel(void)
{
    return ((get_chip_scu_data(SCU_XTAL_SEL)>>19)&0x1);
}
EXPORT_SYMBOL(get_xtal_sel);

u32 get_pdid(void)
{
	return GET_PDIDR();
}
EXPORT_SYMBOL(get_pdid);
u32 get_pkg_type(void)
{
	return ((get_chip_scu_data(SCU_XTAL_SEL)>>14)&0x3);
}
EXPORT_SYMBOL(get_pkg_type);
#endif

/************************************************************************
*                 AVS API
*************************************************************************
*/
static void AVS_DAC_CHG(void)
{

}

/*get AVS_DAC value & return AVS_OUT_EN value
*/ 
u32 get_avs_DAC(u32 *AVS_DAC)
{
    return 0;
}
//EXPORT_SYMBOL(get_avs_DAC);

/*set AVS_DAC value & enable AVS_DAC output
*/ 
void set_avs_DAC(u32 *AVS_DAC)
{

}
//EXPORT_SYMBOL(set_avs_DAC);

void avs_DAC_en(u32 DAC_EN)
{

}

/*return TADC value by mux_num: 0=thermal, 1=AVSMON, 2=Vcore, 3=AVS_DAC
*/
u32 read_TADC(u32 mux_num)
{
    return 0;
}
//set TADC to temperature mode
void set_TADC_temp(void)
{

}

void set_BG_CKEN(void)
{

}


