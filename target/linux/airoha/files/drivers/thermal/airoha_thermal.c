// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Airoha

/*
 *  thermal.c - Generic Thermal Management Sysfs support.
 *  Copyright (C) 2023 Airoha
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/thermal.h>
#include <linux/reset.h>
#include <linux/types.h>

//#define for EN7523
#define tempX10_default 600
#define BGA 3	//YMC add for EN7523 pkg type
#define EN7523_NUM_ZONES	 1
#define EN7523_NUM_SENSORS 1
// end of define for EN7523

//#define for EN7581
#define EN7581_NUM_ZONES	 1
#define EN7581_NUM_SENSORS 1
// end of define for EN7581

//#define for EN7552
#define EN7552_NUM_ZONES   2
#define EN7552_NUM_SENSORS 2
// end of define for EN7552

#define MAX_NUM_ZONES		4

#define get_ptp_dummy_iddq(x) get_ptp_dummy2(x)

enum {
	VTS1,
	VTS2,
	VTS3,
	VTS4,
	VTS5,
	MAX_NUM_VTS,
};

//static for 7523
static const int en7523_SLOPE_7523_X100_E2[EN7523_NUM_SENSORS] = { 1810, };
static const int en7523_SLOPE_7523_X100_E3[EN7523_NUM_SENSORS] = { 1750, };

static const int en7523_CODE_30_DEFAULT_7523_E2[EN7523_NUM_SENSORS] = { 34799, };
static const int en7523_CODE_30_DEFAULT_7523_E3[EN7523_NUM_SENSORS] = { 31662, };

static const int en7523_CODE_30_BIAS_E2QFP[EN7523_NUM_SENSORS] = { 130, };
static const int en7523_CODE_30_BIAS_E2BGA[EN7523_NUM_SENSORS] = { 366, };
static const int en7523_CODE_30_BIAS_E3BGA[EN7523_NUM_SENSORS] = { 346, };
static const int en7523_bank_data[EN7523_NUM_SENSORS] = {VTS1};
//end of static for 7523
//static for 7581
static const int en7581_SLOPE_7581_X100_DIO_DEFAULT[EN7581_NUM_SENSORS] = { 5710, };
static const int en7581_SLOPE_7581_X100_DIO_AVS[EN7581_NUM_SENSORS] = { 5645, };
static const int en7581_initTemperature_CPK_x10[EN7581_NUM_SENSORS]={300,};
static const int en7581_initTemperature_FTK_x10[EN7581_NUM_SENSORS]={620,};
static const int en7581_initTemperature_NONK_x10[EN7581_NUM_SENSORS]={550,};
static const int en7581_BIAS[EN7581_NUM_SENSORS] = { 0, };
static const int en7581_bank_data[EN7581_NUM_SENSORS] = {VTS1};
//end static for 7581
//static for 7552
static const int en7552_SLOPE_X100_DEFAULT[EN7552_NUM_SENSORS] = { 5565,6059, };
static const int en7552_initTemperature_CPK_x10[EN7552_NUM_SENSORS]={300,300,};
static const int en7552_initTemperature_NONK_x10[EN7552_NUM_SENSORS]={550,550,};
static const int en7552_BIAS[EN7552_NUM_SENSORS] = { 0,0, };
static const int en7552_bank_data[EN7552_NUM_SENSORS] = {VTS1,VTS2};
//end static for 7552
struct airoha_thermal;

struct airoha_thermal_bank {
	struct airoha_thermal *airT;
	int id;
};

struct thermal_bank_cfg {
	unsigned int num_sensors;
	const int *sensors;
};
 
struct airoha_thermal_data {
	void (*init)(struct airoha_thermal *airThermal_data);
	int (*get_temp)(struct airoha_thermal_bank *airBank_data);
	u32 DUMMY_REG_OSFT;
	u32 DUMMY_REG_1_OSFT;
	u32 DUMMY_REG_2_OSFT;
	s32 num_banks;
	s32 num_sensors;
	bool need_switch_bank;
	struct thermal_bank_cfg bank_data[MAX_NUM_ZONES];
};
 
struct airoha_thermal {
	struct device *dev;
	void __iomem *ptp_base;
	u32 irq;
	const u32 *SLOPE_X100;
	const u32 *bias;
	const u32 *initTemperature_x10;
	u32 offset[MAX_NUM_VTS];
	u32 thermal_efuse_valid;
	struct mutex lock;
	const struct airoha_thermal_data *conf;
	struct airoha_thermal_bank banks[MAX_NUM_ZONES];
};

/******************
* extern function *
******************/
//extern void thermal_init(void);
extern u32 get_pdid(void);
extern u32 get_pkg_type(void);
extern void thermal_TADC_mode(u16 tadcSel, u16 sensorSel, u16 delayCnt);
extern u32 get_thermal_ADC(void);

/*
dev_info();
dev_err();
dev_warn();
*/

static u32 get_ptp_dummy(struct airoha_thermal *airT)
{
	return readl(airT->ptp_base + (airT->conf->DUMMY_REG_OSFT));
}

static u32 get_ptp_dummy1(struct airoha_thermal *airT)
{
	return readl(airT->ptp_base + (airT->conf->DUMMY_REG_1_OSFT));
}

static u32 get_ptp_dummy2(struct airoha_thermal *airT)
{
	return readl(airT->ptp_base + (airT->conf->DUMMY_REG_2_OSFT));
}

//en7523 data and function
static void en7523_thermal_init(struct airoha_thermal *airT)
{
	//struct airoha_thermal_data *data = airT->conf;
	u32 i=0;
	u32 offset_array[EN7523_NUM_SENSORS]={0};
	const u32 *target_offset=offset_array;
	u32 offset_efuse = get_ptp_dummy(airT);

	airT->thermal_efuse_valid = (offset_efuse == 0)?0:1;
	if (airT->thermal_efuse_valid) 
	{
		offset_array[0]=offset_efuse;
	}
	if(get_pdid() == 2)
	{
		airT->SLOPE_X100 = en7523_SLOPE_7523_X100_E2;
		if(get_pkg_type() == BGA){  //PKG = BGA
		  airT->bias = en7523_CODE_30_BIAS_E2BGA;
		}
		else{ //PKG = QFP
		  airT->bias = en7523_CODE_30_BIAS_E2QFP;
		}
		if(airT->thermal_efuse_valid){
		  target_offset = en7523_CODE_30_DEFAULT_7523_E2;
		}
	}
	else{ //E3 avg. slope
		airT->SLOPE_X100 = en7523_SLOPE_7523_X100_E3;
		airT->bias = en7523_CODE_30_BIAS_E3BGA;
		if(airT->thermal_efuse_valid){
		  target_offset = en7523_CODE_30_DEFAULT_7523_E3;
		}
	}
	
	for(i=0;i<EN7523_NUM_SENSORS;i++)
		airT->offset[i]=target_offset[i]+airT->bias[i];
	
	dev_info(airT->dev,"This IC is %scalibrated",airT->thermal_efuse_valid?"":"not ");
	dev_info(airT->dev,"EN7523 Init check code_30:%d , Slope:%d",airT->offset[0],airT->SLOPE_X100[0]);
}

static int ThermalGetT_7523(struct airoha_thermal_bank *bank)
{
	int temp_x10;
	u32 sensor_index = bank->airT->conf->bank_data->sensors[bank->id];
	u32 ADC_code_30 = bank->airT->offset[sensor_index];
	s32 SLOPE_X100 = (s32)bank->airT->SLOPE_X100[sensor_index];

	if(bank->airT->thermal_efuse_valid == 0)
	{
		temp_x10 = tempX10_default + (get_ptp_dummy_iddq(bank->airT)&0xf)*8;
	}
	else
	{
		thermal_TADC_mode(0,0,1);
		temp_x10 = 1000 * ((signed int)(get_thermal_ADC()-ADC_code_30))/SLOPE_X100 + 300;
	}
	return temp_x10*100;
}

struct airoha_thermal_data en7523_thermal_data ={
	.init = en7523_thermal_init,
	.get_temp = ThermalGetT_7523,
	.num_banks= EN7523_NUM_ZONES,
	.num_sensors= EN7523_NUM_SENSORS,
	.DUMMY_REG_OSFT = 0xf20,
	.DUMMY_REG_1_OSFT = 0xf24,
	.DUMMY_REG_2_OSFT = 0xf28,
	.need_switch_bank = false,
	.bank_data={
		{.num_sensors = 1,
		 .sensors = en7523_bank_data,},
	},
};
//end of en7523 data and function

//en7581 data and function
static void en7581_thermal_init(struct airoha_thermal *airT)
{
	//struct airoha_thermal_data *data = airT->conf;
	u32 i=0;
	u32 offset_array[EN7581_NUM_SENSORS]={0};
	const u32 *target_offset=offset_array;
	u32 offset_efuse = get_ptp_dummy(airT);

	airT->thermal_efuse_valid = (offset_efuse == 0)?0:1;
	if (airT->thermal_efuse_valid) 
	{
		offset_array[0]=(offset_efuse&0xffff0000)>>16;
		if(get_ptp_dummy2(airT)==0){
			airT->SLOPE_X100=en7581_SLOPE_7581_X100_DIO_AVS;
			airT->initTemperature_x10=en7581_initTemperature_CPK_x10;
		}
		else {
		  airT->SLOPE_X100=en7581_SLOPE_7581_X100_DIO_DEFAULT;
		  airT->initTemperature_x10=en7581_initTemperature_FTK_x10;
		}
	}else {
		thermal_TADC_mode(0,1,10);
		offset_array[0]=get_thermal_ADC();
		airT->SLOPE_X100=en7581_SLOPE_7581_X100_DIO_DEFAULT;
		airT->initTemperature_x10=en7581_initTemperature_NONK_x10;
	}
	airT->bias = en7581_BIAS;

	for(i=0;i<EN7581_NUM_SENSORS;i++)
		airT->offset[i]=target_offset[i]+airT->bias[i];

	dev_info(airT->dev,"This IC is %scalibrated",airT->thermal_efuse_valid?"":"not ");
	dev_info(airT->dev,"EN7581 Init check code_30:%d , Slope:%d",airT->offset[0],airT->SLOPE_X100[0]);
}

static int ThermalGetT_7581(struct airoha_thermal_bank *bank)
{
	int temp_x10=0;
	u32 getIndex=0;
	int min=0,max=0,avgTemp=0,temp_ADC=0;

	u32 sensor_index = bank->airT->conf->bank_data->sensors[bank->id];
	u32 ADC_code_30 = bank->airT->offset[sensor_index];
	s32 SLOPE_X100 = (s32)bank->airT->SLOPE_X100[sensor_index];
	int initTemperature_x10 = bank->airT->initTemperature_x10[sensor_index];
	thermal_TADC_mode(0,1,10);  
	min=max=avgTemp=temp_ADC=get_thermal_ADC();

	for(getIndex=1;getIndex<6;getIndex++)
	{
		temp_ADC=get_thermal_ADC();
		avgTemp+=temp_ADC;
		if (temp_ADC>max)
		  max=temp_ADC;
		else if (temp_ADC<min)
		  min=temp_ADC;
	}
	avgTemp=avgTemp-max-min;
	avgTemp=avgTemp>>2;

	temp_x10 = 1000 * ((signed int)(avgTemp-ADC_code_30))/SLOPE_X100 + initTemperature_x10;
	return temp_x10*100;
}

struct airoha_thermal_data en7581_thermal_data ={
	.init = en7581_thermal_init,
	.get_temp = ThermalGetT_7581,
	.num_banks= EN7581_NUM_ZONES,
	.num_sensors= EN7581_NUM_SENSORS,
	.DUMMY_REG_OSFT = 0xf20,
	.DUMMY_REG_1_OSFT = 0xf24,
	.DUMMY_REG_2_OSFT = 0xf28,
	.need_switch_bank = false,
	.bank_data={
		{.num_sensors = 1,
		 .sensors = en7581_bank_data,},
	},
};
//end of en7581 data and function

//en7552 data and function
static void en7552_thermal_init(struct airoha_thermal *airT)
{
	//struct airoha_thermal_data *data = airT->conf;
	u32 i=0;
	u32 offset_array[EN7552_NUM_SENSORS]={0,0};
	const u32 *target_offset=offset_array;
	u32 offset_efuse = get_ptp_dummy(airT);
	airT->thermal_efuse_valid = (offset_efuse == 0)?0:1;
	thermal_TADC_mode(0,1,10);
	airT->SLOPE_X100=en7552_SLOPE_X100_DEFAULT;
	if (airT->thermal_efuse_valid)
	{
		offset_array[0]=(offset_efuse&0xffff);
		offset_array[1]=(offset_efuse&0xffff0000)>>16;
		airT->initTemperature_x10=en7552_initTemperature_CPK_x10;
	}else {
		offset_array[0]=get_thermal_ADC();
		thermal_TADC_mode(0,2,10);
		offset_array[1]=get_thermal_ADC();
		airT->initTemperature_x10=en7581_initTemperature_NONK_x10;
	}
	thermal_TADC_mode(0,1,10);
	airT->bias = en7552_BIAS;
	for(i=0;i<EN7552_NUM_SENSORS;i++)
		airT->offset[i]=target_offset[i]+airT->bias[i];
	dev_info(airT->dev,"This IC is %scalibrated",airT->thermal_efuse_valid?"":"not ");
	dev_info(airT->dev,"EN7552 Init check code_30:%d , Slope:%d",airT->offset[0],airT->SLOPE_X100[0]);
}

static int ThermalGetT_7552(struct airoha_thermal_bank *bank)
{
	int temp_x10=0;
	u32 getIndex=0;
	int min=0,max=0,avgTemp=0,temp_ADC=0;

	u32 sensor_index = bank->airT->conf->bank_data->sensors[bank->id];
	u32 ADC_code_30 = bank->airT->offset[sensor_index];
	s32 SLOPE_X100 = (s32)bank->airT->SLOPE_X100[sensor_index];
	int initTemperature_x10 = bank->airT->initTemperature_x10[sensor_index];

	thermal_TADC_mode(0,1+sensor_index,10);
	min=max=avgTemp=temp_ADC=get_thermal_ADC();

	for(getIndex=1;getIndex<6;getIndex++)
	{
		temp_ADC=get_thermal_ADC();
		avgTemp+=temp_ADC;
		if (temp_ADC>max)
		  max=temp_ADC;
		else if (temp_ADC<min)
		  min=temp_ADC;
	}
	avgTemp=avgTemp-max-min;
	avgTemp=avgTemp>>2;

	temp_x10 = 1000 * ((signed int)(avgTemp-ADC_code_30))/SLOPE_X100 + initTemperature_x10;
	return temp_x10*100;
}

struct airoha_thermal_data en7552_thermal_data ={
	.init = en7552_thermal_init,
	.get_temp = ThermalGetT_7552,
	.num_banks= EN7552_NUM_ZONES,
	.num_sensors= EN7552_NUM_SENSORS,
	.DUMMY_REG_OSFT = 0xf20,
	.DUMMY_REG_1_OSFT = 0xf24,
	.DUMMY_REG_2_OSFT = 0xf28,
	.need_switch_bank = false,
	.bank_data={
	{.num_sensors = 1,
	 .sensors = en7552_bank_data,},
	},
};
//end of en7552 data and function

static const struct of_device_id airoha_thermal_phy_match[] = {
	{.compatible = "airoha,en7523-thermal_phy",
	 .data =  (void *)&en7523_thermal_data}, //
	{.compatible = "airoha,en7581-thermal_phy",
	 .data =  (void *)&en7581_thermal_data}, //
	{.compatible = "airoha,en7552-thermal_phy",
	 .data =  (void *)&en7552_thermal_data}, //
	{ /* sentinel */},
};
MODULE_DEVICE_TABLE(of, airoha_thermal_phy_match);

/**
 * Airoha_thermal_get_bank - get bank
 * @bank:	The bank
 *
 * The bank registers are banked, we have to select a bank in the
 * PTPCORESEL register to access it.
 */
static void airoha_thermal_get_bank(struct airoha_thermal_bank *bank)
{
	struct airoha_thermal *airT = bank->airT;
	//u32 val;
	if (airT->conf->need_switch_bank) {
		mutex_lock(&airT->lock);
	}
}

/**
 * airoha_thermal_put_bank - release bank
 * @bank:	The bank
 *
 * release a bank previously taken with mtk_thermal_get_bank,
 */
static void airoha_thermal_put_bank(struct airoha_thermal_bank *bank)
{
	struct airoha_thermal *airT = bank->airT;

	if (airT->conf->need_switch_bank)
		mutex_unlock(&airT->lock);
}

static int airoha_read_temp(void *data, int *temperature)
{
	struct airoha_thermal *airT = data;
	int i=0;
	int tempmax = INT_MIN;

	for (i = 0; i < airT->conf->num_banks; i++) {
		struct airoha_thermal_bank *bank = &airT->banks[i];
		airoha_thermal_get_bank(bank);
		tempmax = max(tempmax, airT->conf->get_temp(bank));
		airoha_thermal_put_bank(bank);
	}

	*temperature = tempmax;

	return 0;
}

static const struct thermal_zone_of_device_ops airoha_thermal_ops = {
	.get_temp = airoha_read_temp,
};

static void airoha_thermal_init_bank(struct airoha_thermal *airT, int num)
{
	struct airoha_thermal_bank *bank = &airT->banks[num];
	//const struct airoha_thermal_data *conf = airT->conf;
	bank->id = num;
	bank->airT = airT;
	airoha_thermal_get_bank(bank);
	/*setting bank rg here*/
	/* reserve for ptp setting here*/
	airoha_thermal_put_bank(bank);

}

static int airoha_thermal_probe(struct platform_device *pdev)
{
	int ret=0;
	int i=0;
	//struct device_node *auxadc, *apmixedsys, *np = pdev->dev.of_node;
	struct airoha_thermal *airT;
	struct resource *res;
	//u64 auxadc_phys_base, apmixed_phys_base;
	struct thermal_zone_device *tzdev;
	dev_info(&pdev->dev,"Thermal Sensor address probe start\n");
	dev_info(&pdev->dev,"thermal driver version : 20231122\n");
	airT = devm_kzalloc(&pdev->dev, sizeof(struct airoha_thermal), GFP_KERNEL);
	if (!airT){
		dev_err(&pdev->dev,"alloc fail\n");
		return -ENOMEM;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	airT->ptp_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(airT->ptp_base))
	{
		dev_err(&pdev->dev," PTP_BASE fail\n");
		return PTR_ERR(airT->ptp_base);
	}

	airT->dev = &pdev->dev;
	airT->conf = of_device_get_match_data(&pdev->dev);
	airT->conf->init(airT);
	mutex_init(&airT->lock);
	for (i = 0; i < airT->conf->num_banks; i++)
	{
		airoha_thermal_init_bank(airT, i);
	}

	platform_set_drvdata(pdev, airT);

	tzdev = devm_thermal_zone_of_sensor_register(&pdev->dev, 0, airT,						     &airoha_thermal_ops);
	if (IS_ERR(tzdev)) {
		ret = PTR_ERR(tzdev);
		dev_err(&pdev->dev,"tzdev fail. Error Code : %d\n",ret);
		return ret;
	}

	dev_info(&pdev->dev,"Thermal Sensor address probe done\n");
	return 0;
}

static int airoha_thermal_phy_drv_remove(struct platform_device *pdev)
{
	struct airoha_thermal *airT = platform_get_drvdata(pdev);
	if(airT)  {
		dev_info(&pdev->dev,"airoha_thermal_phy_drv_remove\n");
	}
	return 0;
}

static struct platform_driver airoha_thermal_phy_driver = {
	.probe = airoha_thermal_probe,
	.remove = airoha_thermal_phy_drv_remove,
	.driver = {
		.name = "airoha-thermal_phy",
		.of_match_table = airoha_thermal_phy_match,
	},
};
 
module_platform_driver(airoha_thermal_phy_driver); 

MODULE_AUTHOR("Elijah Yu <elijah.yu@airoha.com>");
MODULE_DESCRIPTION("Airoha thermal driver");
MODULE_LICENSE("GPL v2");