#include <Thermal_Meter.h>
//#include <math.h>
//#include <stdio.h>
//#include <string.h>
//#include <stdio.h>
#include <rtk_types.h>
#include <dal_rtl8373_drv.h>

rtk_uint32 pow2(rtk_uint16 base, rtk_uint32 bit)
{
	rtk_uint32 result = 0 ;
	if(base == 2)
	{
	result = (1 << bit); 
	}
	return result;
}

/*. #  Two's complement to truth value
* input: cplmt_mode and bit number
*	bit number is fixed 8
*	cplmt_mode is not content '-' or '+'. the 8th bit is sign bit
*	ture_val is sign int.
*/
rtk_api_ret_t c2t_true_val(rtk_int32 cplmt_mode, rtk_int32 sign_bit, rtk_int32* ture_val){
	rtk_int32 w ;
	rtk_int32 sign;
	//char front_sign[1] = {0};
	//rtk_bitmask_t bin;
	rtk_uint32 tmp, mask, val, val_hex;

	//bin.bits[0] = 0;
	
	w = sign_bit/4 + 1;
	sign = cplmt_mode &(1<< sign_bit);
	/*if (!sign){
		strcpy(front_sign, "+");
	}
	else{
		strcpy(front_sign, "-");
	}*/

	tmp = pow2(2, sign_bit) - 1;
	mask = 1 << (w*4);
	mask = mask -1;
	mask = mask & tmp;
	val = cplmt_mode & mask;
	if (!sign){
		val_hex = val & mask; 
	}
	else{
		val_hex = (~val & mask) +1;
	}

//	while(val_hex)
//	{
//		bin.bits[0] += ((val_hex % 2) ) *(pow(10, i));
//		val_hex = val_hex / 2;
//		i ++;
//	}

	if (!sign)
	{
		*ture_val = val_hex ;
	}
	else
	{
		*ture_val = -val_hex;
	}

	return RT_ERR_OK;
}

rtk_api_ret_t t2c_cplmt_mode(rtk_int32 ture_val, rtk_int32 sign_bit, rtk_int32* cplmt_mode){
		//rtk_bitmask_t bin;
	rtk_uint32 tmp;

	if (ture_val <= 0){
		ture_val = -ture_val;
		tmp = (~(ture_val-1))& 0xFF;
		tmp |= 0x1 << sign_bit;
	}
	else {
		ture_val = ture_val;
		tmp = ture_val;
	}

	*cplmt_mode = tmp;
	return RT_ERR_OK;
}

/* Function Name:
 *      unlatch_tm
 * Description:
 *      unlatch thermal meter 0/1 ouput immediately after power on for more accurate test
 * Input:
 *      tm_index              - thermal meter index 0 or 1 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT      - Invalid tmlist
 * Note:
 *      None
 */

rtk_api_ret_t unlatch_tm(rtk_uint16 tm_index){
    ret_t retVal;
	switch (tm_index){
		case 0:
			if ((retVal = rtl8373_setAsicRegBits(TM0_CTRL2_0876, 0x4, 0)) != RT_ERR_OK)
				return retVal;
			rtlglue_printf("UNLATCH TM_{%d} \n", tm_index);
			break;
		case 1:
			if ((retVal = rtl8373_setAsicRegBits(TM1_CTRL2_0876, 0x4, 0)) != RT_ERR_OK)
				return retVal;
			rtlglue_printf("UNLATCH TM_{%d} \n", tm_index);
			break;
		default:
			rtlglue_printf("Error TM index {%d};Should be one of {0,1}\n", tm_index);
			return RT_ERR_INPUT;
		}
	return RT_ERR_OK;
}

/* Function Name:
 *      latch_tm
 * Description:
 *      latch thermal meter 0/1 ouput immediately after power on for more accurate test
 * Input:
 *      tm_index              - thermal meter index 0 or 1 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid tmlist
 * Note:
 *      None
 */


rtk_api_ret_t latch_tm(rtk_uint16 tm_index){
	ret_t retVal;
	switch (tm_index){
		case 0:
			if ((retVal = rtl8373_setAsicRegBits(TM0_CTRL2_0876, 0x4, 1)) != RT_ERR_OK)
				return retVal;
			rtlglue_printf("LATCH TM_{%d} \n", tm_index);
			break;
		case 1:
			if ((retVal = rtl8373_setAsicRegBits(TM1_CTRL2_0876, 0x4, 1)) != RT_ERR_OK)
				return retVal;
			rtlglue_printf("LATCH TM_{%d} \n", tm_index);
			break;
		default:
			rtlglue_printf("Error TM index {%d};Should be one of {0,1}\n", tm_index);
			return RT_ERR_INPUT;
		}
	return RT_ERR_OK;
}

/* Function Name:
 *      cmp_low_temp_en
 * Description:
 *      enable compare TM_OUT with set Low threshold
 * Input:
 *      tm_index              - thermal meter index 0 or 1 
 *      enable              - enabled or disabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE        - Invalid enable input
 * Note:
 *      None
 */


rtk_api_ret_t cmp_low_temp_en(rtk_uint16 tm_index, rtk_enable_t enable){
	ret_t retVal;
		switch (tm_index){
		case 0:
			if ((retVal = rtl8373_setAsicRegBits(TM0_CTRL3_0876, 0x200, enable)) != RT_ERR_OK)
				return retVal;
		break;
		case 1:
			if ((retVal = rtl8373_setAsicRegBits(TM1_CTRL3_0876, 0x200, enable)) != RT_ERR_OK)
				return retVal;
		break;
		default:
			rtlglue_printf("Error TM index {%d};Should be one of {0,1} \n", tm_index);
		}

		switch (enable){
		case 0:
			rtlglue_printf("Disable TM_{%d} Low Temperature Compare Function \n", tm_index);
		break;
		case 1:
			rtlglue_printf("Enable TM_{%d} Low Temperature Compare Function \n", tm_index);
		break;
		default:
			rtlglue_printf("Error on_off {%d};Should be one of {0,1}\n", enable);
			return RT_ERR_ENABLE;
		}
	return RT_ERR_OK;
}

/* Function Name:
 *      cmp_high_temp_en
 * Description:
 *      enable compare TM_OUT with set High threshold 
 * Input:
 *      tm_index              - thermal meter index 0 or 1 
 *      enable                - enabled or disabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK             - Success
 *      RT_ERR_SMI            - SMI access error
 *      RT_ERR_ENABLE         - Invalid enable input
 * Note:
 *      None
 */

rtk_api_ret_t cmp_high_temp_en(rtk_uint16 tm_index, rtk_enable_t enable){
	ret_t retVal;
		switch (tm_index){
		case 0:
			if ((retVal = rtl8373_setAsicRegBits(TM0_CTRL3_0876, 0x8000, enable)) != RT_ERR_OK)
				return retVal;
		break;
		case 1:
			if ((retVal = rtl8373_setAsicRegBits(TM1_CTRL3_0876, 0x8000, enable)) != RT_ERR_OK)
				return retVal;
		break;
		default:
			rtlglue_printf("Error TM index {%d};Should be one of {0,1} \n", tm_index);
		}

		switch (enable){
		case 0:
			rtlglue_printf("Disable TM_{%d} High Temperature Compare Function \n", tm_index);
		break;
		case 1:
			rtlglue_printf("Enable TM_{%d} High Temperature Compare Function \n", tm_index);
		break;
		default:
			rtlglue_printf("Error on_off {%d};Should be one of {0,1}\n", enable);
			return RT_ERR_ENABLE;
		}
	return RT_ERR_OK;
}

/* Function Name:
 *      get_cmp_high_temp
 * Description:
 *      get the compared highest temperature 
 * Input:
 *      tm_index              - thermal meter index 0 or 1 
 *      HIGH_THR              - high threshold 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE        - Invalid enable input
 * Note:
 *      None
 */

rtk_api_ret_t get_cmp_high_temp(rtk_uint32 tm_index ,rtk_int32* HIGH_THR){
	ret_t retVal;
	rtk_uint32 cmp_high ;
	rtk_int32 tmp;

	switch (tm_index){
		case 0:
			if ((retVal = rtl8373_getAsicRegBits(TM0_CTRL3_0876, 0x7FC00, &cmp_high)) != RT_ERR_OK)
			return retVal;
			c2t_true_val(cmp_high, 8, &tmp) ; //lindex [true_val $cmp_high 8] 1 true_val_bin
			*HIGH_THR = tmp;
			return RT_ERR_OK;
		case 1:
			if ((retVal = rtl8373_getAsicRegBits(TM1_CTRL3_0876, 0x7FC00, &cmp_high)) != RT_ERR_OK)
			return retVal;
			c2t_true_val(cmp_high, 8, &tmp) ;
			*HIGH_THR = tmp;
			return RT_ERR_OK;
		}
	return RT_ERR_OK;
}


/* Function Name:
 *      get_cmp_low_temp
 * Description:
 *      get the compared lowest temperature  
 * Input:
 *      tm_index              - thermal meter index 0 or 1 
 *      LOW_THR              - low threshold 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE        - Invalid enable input
 * Note:
 *      None
 */

rtk_api_ret_t get_cmp_low_temp(rtk_uint32 tm_index ,rtk_int32* LOW_THR){
	ret_t retVal;
	rtk_uint32 cmp_low;
	rtk_int32 tmp;

	switch (tm_index){
		case 0:
			if ((retVal = rtl8373_getAsicRegBits(TM0_CTRL3_0876, 0x1FF, &cmp_low)) != RT_ERR_OK)
			return retVal;
			c2t_true_val(cmp_low, 8, &tmp) ;
			*LOW_THR = tmp;
			return RT_ERR_OK;
		case 1:
			if ((retVal = rtl8373_getAsicRegBits(TM1_CTRL3_0876, 0x1FF, &cmp_low)) != RT_ERR_OK)
			return retVal;
			c2t_true_val(cmp_low, 8, &tmp) ;
			*LOW_THR = tmp;
			return RT_ERR_OK;
		}
	return RT_ERR_OK;
}


/* Function Name:
 *      set_cmp_high_temp
 * Description:
 *      set the compared highest temperature  
 * Input:
 *      tm_index              - thermal meter index 0 or 1 
 *      HIGH_THR              - high threshold 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE        - Invalid enable input
 * Note:
 *      None
 */

rtk_api_ret_t set_cmp_high_temp(rtk_uint32 tm_index ,rtk_uint32 HIGH_THR){
	ret_t retVal;
	rtk_int32 TM_HIGH_THR;

	t2c_cplmt_mode(HIGH_THR, 8, &TM_HIGH_THR);

	switch (tm_index){
		case 0:
	if ((retVal = rtl8373_setAsicRegBits(TM0_CTRL3_0876, 0x7FC00, TM_HIGH_THR)) != RT_ERR_OK) //set temperature
	return retVal;
	if ((retVal = rtl8373_setAsicRegBit(TM0_CTRL3_0876, 19, 1)) != RT_ERR_OK) //en compare
	return retVal;
		break;

		case 1:
	if ((retVal = rtl8373_setAsicRegBits(TM1_CTRL3_0876, 0x7FC00, TM_HIGH_THR)) != RT_ERR_OK) //set temperature
	return retVal;
	if ((retVal = rtl8373_setAsicRegBit(TM1_CTRL3_0876, 19, 1)) != RT_ERR_OK) //en compare
	return retVal;
		break;
	}
	return retVal;
}


/* Function Name:
 *      set_cmp_low_temp
 * Description:
 *      set the compared lowest temperature  
 * Input:
 *      tm_index              - thermal meter index 0 or 1 
 *      LOW_THR               - low threshold 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Success
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE        - Invalid enable input
 * Note:
 *      None
 */

rtk_api_ret_t set_cmp_low_temp(rtk_uint32 tm_index ,rtk_uint32 LOW_THR){
	ret_t retVal;
	rtk_int32 TM_LOW_THR;

	t2c_cplmt_mode(LOW_THR, 8, &TM_LOW_THR);

	switch (tm_index){
		case 0:
	if ((retVal = rtl8373_setAsicRegBits(TM0_CTRL3_0876, 0x1FF, TM_LOW_THR)) != RT_ERR_OK) //set temperature
	return retVal;
	if ((retVal = rtl8373_setAsicRegBit(TM0_CTRL3_0876, 9, 1)) != RT_ERR_OK) //en compare
	return retVal;
		break;
	
		case 1:
	if ((retVal = rtl8373_setAsicRegBits(TM1_CTRL3_0876, 0x1FF, TM_LOW_THR)) != RT_ERR_OK) //set temperature
	return retVal;
	if ((retVal = rtl8373_setAsicRegBit(TM1_CTRL3_0876, 9, 1)) != RT_ERR_OK) //en compare
	return retVal;
		break;
		}
	return retVal;
}

/* Function Name:
 *      get_junc_temp
 * Description:
 *      get thermal meter 0/1_OUT value  
 * Input:
 *      tm_index              - thermal meter index 0 or 1 
 *      temp                  - thermal meter 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - Success
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT        - Invalid input
 * Note:
 *      None
 */

#ifdef NO_FLOAT
rtk_api_ret_t  get_junc_temp(rtk_uint32 tm_index, rtk_int32* temp)
#else
rtk_api_ret_t  get_junc_temp(rtk_uint32 tm_index, float* temp)
#endif
{
	rtk_uint32 tm_out = 0;
	rtk_uint32 tm_out_18_3 = 0;
	rtk_uint32 tm_out_2_0 = 0;
	rtk_uint32 tm_out_18_0 = 0;
	rtk_uint32 tm_out_17_0 = 0;
	rtk_uint32 sign;
	
	switch (tm_index){
		case 0:
			rtl8373_getAsicReg(TM0_RESULT0_0876, &tm_out);
			rtl8373_getAsicRegBits(TM0_RESULT0_0876, 0xFFFF, &tm_out_18_3); //15:0
			rtl8373_getAsicRegBits(TM0_RESULT0_0876, 0x70000, &tm_out_2_0);  //18:16
			break;
		case 1:
			rtl8373_getAsicReg(TM1_RESULT0_0876, &tm_out);
			rtl8373_getAsicRegBits(TM1_RESULT0_0876, 0xFFFF, &tm_out_18_3);//15:0
			rtl8373_getAsicRegBits(TM1_RESULT0_0876, 0x70000, &tm_out_2_0);//18:16
			break;
		default:
			rtlglue_printf("Error TM index {%d}: Should be one of {0,1}\n", tm_index);
			return RT_ERR_INPUT;
	}

	
	tm_out_18_0 = tm_out_18_3 << 3 | tm_out_2_0;
	sign = (tm_out_18_0 >> 18) ;
	tm_out_17_0 = tm_out_18_0 & 0x3FFFF;
	if (sign == 1)
		{
#ifdef NO_FLOAT
		*temp = -(pow2(2, 18) - tm_out_17_0);
#else
		*temp = -((float)pow2(2, 18) - tm_out_17_0)/(float)pow2(2, 10);
#endif
	}
	else
		{
#ifdef NO_FLOAT
		*temp = tm_out_17_0;
#else
		*temp = tm_out_17_0/(float)pow2(2, 10);
#endif
	}

	return RT_ERR_OK;
}

/* Function Name:
 *      get_reg_a
 * Description:
 *      get TM 0/1_REG_A value   
 * Input:
 *      tmid                   - thermal meter index 0 or 1 
 *      reg_a                  - REG_A value 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - Success
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT        - Invalid input
 * Note:
 *      None
 */
#ifdef NO_FLOAT
rtk_api_ret_t get_reg_a(rtk_uint32 tmid, rtk_uint32* reg_a)
#else
rtk_api_ret_t get_reg_a(rtk_uint32 tmid, float* reg_a)
#endif
{
	rtk_uint32 phyid = 5;
	rtk_uint32 device_addr = 31;
	rtk_uint32 regbfa0, regbfa2, REG_A;

	switch(tmid)
	{
		case 0:
		{
			dal_rtl8373_phy_regbits_read(phyid, device_addr, 0xbfa0,0xFFF8, &regbfa0); //15:3
			dal_rtl8373_phy_regbits_read(phyid, device_addr, 0xbfa2, 0xFFFF, &regbfa2);
			REG_A = regbfa2 << 13 | regbfa0;
			break;
		}
		case 1:
		{
			rtl8373_getAsicReg(TM1_CTRL0_0876, &REG_A);
			break;
		}
		default:
			rtlglue_printf("Error TM index {%d}: Should be one of {0,1}\n", tmid);
			return RT_ERR_INPUT;
	}

#ifdef NO_FLOAT
	*reg_a = REG_A;
#else
	*reg_a = REG_A / (float)pow2(2, 14);
#endif
	return RT_ERR_OK;
}

/* Function Name:
 *      get_reg_b
 * Description:
 *      get TM 0/1_REG_B value   
 * Input:
 *      tmid                   - thermal meter index 0 or 1 
 *      reg_b                  - REG_B value 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - Success
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT        - Invalid input
 * Note:
 *      None
 */

#ifdef NO_FLOAT
rtk_api_ret_t get_reg_b(rtk_uint32 tmid, rtk_int32* reg_b)
#else
rtk_api_ret_t get_reg_b(rtk_uint32 tmid, float* reg_b)
#endif
{
	rtk_api_ret_t retVal;
	rtk_uint32 phyid = 5;
	rtk_uint32 device_addr = 31;
	rtk_uint32 signbit = 21;
	rtk_uint32 int_lsb = 11;
	rtk_uint32 regbfa4, regbfa6, sign, hex;
	rtk_uint32 REG_B;
	switch(tmid)
	{
		case 0:
		{
		if ((retVal = dal_rtl8373_phy_regbits_read(phyid, device_addr, 0xbfa4, 0xFC00, &regbfa4)) != RT_ERR_OK)//15:10
			return retVal;
		if ((retVal = dal_rtl8373_phy_regbits_read(phyid, device_addr, 0xbfa6, 0xFFFF, &regbfa6)) != RT_ERR_OK)
			return retVal;
			
		REG_B = regbfa6 << 6 | regbfa4;
		break;
		}
		case 1:
		{
			rtl8373_getAsicRegBits(TM1_CTRL1_0876 ,0x3FFFFF, &REG_B); // 21:0
			break;
		}
		default:
			rtlglue_printf("Error TM index {%d}: Should be one of {0,1}\n", tmid);
			return RT_ERR_INPUT;
	}

	sign = (REG_B >> signbit ) ;
	hex = REG_B & ((1 << signbit) -1); 

	if (sign == 1)
		{
#ifdef NO_FLOAT
		*reg_b = -(pow2(2, signbit) - hex);
#else
		*reg_b = -((float)pow2(2, signbit) - hex)/(float)pow2(2, int_lsb);
#endif
	}
	else
		{
#ifdef NO_FLOAT
		*reg_b = hex;
#else
		*reg_b = hex/(float)pow2(2, int_lsb);
#endif
	}
	
	return RT_ERR_OK;
}

/* Function Name:
 *      update_reg_a
 * Description:
 *      refresh TM 0/1_REG_A value    
 * Input:
 *      tmid                   - thermal meter index 0 or 1 
 *      reg_a_new              - new REG_A value 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - Success
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT        - Invalid input
 * Note:
 *      None
 */

#ifdef NO_FLOAT
rtk_api_ret_t update_reg_a(rtk_uint32 tmid, rtk_uint32* reg_a_new)
#else
rtk_api_ret_t update_reg_a(rtk_uint32 tmid, float* reg_a_new)
#endif
{
	rtk_api_ret_t retVal;
	rtk_uint32 reg_a = 0x8020000; //Reg_A update value，hex form 0x8020000 → 8320.00
	rtk_uint32 phyid = 1<<5;
	rtk_uint32 device_addr = 31;
	rtk_uint32 reg_a_12_0, reg_a_28_13;

	switch(tmid)
	{
		case 0:
		{
			reg_a_12_0 = reg_a & 0x1FFF; //12:0
			reg_a_28_13 = (reg_a & 0x1FFFE000) >> 13 ;// 28:13
			if ((retVal = dal_rtl8373_phy_regbits_write(phyid, device_addr, 0xbfa0, 0xFFF8, reg_a_12_0)) != RT_ERR_OK)// 15:3
			return retVal;
			if ((retVal = dal_rtl8373_phy_regbits_write(phyid, device_addr, 0xbfa2, 0xFFFF, reg_a_28_13)) != RT_ERR_OK)// 15:0
			return retVal;
			break;
		}
		case 1:
		{
			rtl8373_setAsicRegBits(TM1_CTRL0_0876, 0x1FFFFFFF, reg_a); // 28:0
			break;
		}
		default:
		{
			rtlglue_printf("Error TM index {%d}: Should be one of {0,1}\n", tmid);
			return RT_ERR_INPUT;
		}
	}

	get_reg_a(tmid, reg_a_new);
	return RT_ERR_OK;
}

/* Function Name:
 *      update_reg_b
 * Description:
 *      refresh TM 0/1_REG_B value    
 * Input:
 *      tmid                   - thermal meter index 0 or 1 
 *      reg_b_new              - new REG_B value 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - Success
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT        - Invalid input
 * Note:
 *      None
 */

#ifdef NO_FLOAT
rtk_api_ret_t update_reg_b(rtk_uint32 tmid, rtk_int32* reg_b_new)
#else
rtk_api_ret_t update_reg_b(rtk_uint32 tmid, float* reg_b_new)
#endif
{
	rtk_api_ret_t retVal;
	rtk_uint32 reg_b_0 = 0x374000; //TM0_Reg_B update value，hex form 0x374000 → -280.0
	rtk_uint32 reg_b_1 = 0x374800; //TM0_Reg_B update value，hex form 0x374800 → -279.0
	rtk_uint32 phyid = 1<<5;
	rtk_uint32 device_addr = 31;
	rtk_uint32 reg_b0_5_0, reg_b0_21_6;

	reg_b0_5_0 = reg_b_0 & 0x3F;// 5:0
	reg_b0_21_6 = (reg_b_0 & 0x3FFFC0) >> 6;// 21:6

	switch(tmid)
	{
		case 0:
		{
			if ((retVal = dal_rtl8373_phy_regbits_write(phyid, device_addr, 0xbfa4, 0xFC00, reg_b0_5_0)) != RT_ERR_OK)// 15:10
			return retVal;
			if ((retVal = dal_rtl8373_phy_regbits_write(phyid, device_addr, 0xbfa6, 0xFFFF, reg_b0_21_6)) != RT_ERR_OK)// 15:0
			return retVal;
			break;
		}
		case 1:
		{
			rtl8373_setAsicRegBits(TM1_CTRL1_0876, 0x3FFFFF, reg_b_1); // 21:0
			break;
		}
		default:
		{
			rtlglue_printf("Error TM index {%d}: Should be one of {0,1}\n", tmid);
			return RT_ERR_INPUT;
		}
	}

	get_reg_b(tmid, reg_b_new);
	return RT_ERR_OK;
}

/* Function Name:
 *      tm_init
 * Description:
 *      init thermal meter    
 * Input:
 *      tmid                   - thermal meter index 0 or 1 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK            - Success
 *      RT_ERR_SMI           - SMI access error
 *      RT_ERR_INPUT        - Invalid input
 * Note:
 * Function list:
 # 1. tm_init_test  ——># TM0&1初始化(0,1分别)
 # Flow：
 # (1)读Latch温度 → (2)unlatch_tm  → (3)读当前温度  → (4)打开SMI，读取当前TM0/1 reg_a,reg_b值 → (5)更新TM0/1_reg_a,reg_b值
 # 更新TM0/1 Reg_A/B到新设定值
 # option：设定Low_temp & High_temp 为具体值,  TM_LOWCMP/TM_HIGHCMP 默认DISABLE
 */
 
rtk_api_ret_t tm_init(rtk_uint32 tmid)
{
	rtk_uint32 retVal;
#ifdef NO_FLOAT
	rtk_int32 Latch_TM, TM;
	rtk_uint32 tm_reg_a,  tm_reg_a_new;
	rtk_int32 tm_reg_b, tm_reg_b_new;
#else
	float Latch_TM, TM;
	float tm_reg_a, tm_reg_b, tm_reg_a_new, tm_reg_b_new;
#endif
	rtk_int32 HIGH_THR;
	rtk_int32 LOW_THR;

	rtlglue_printf("---Temp_Init_Begin--- \n");
	if ((retVal = get_junc_temp(tmid, &Latch_TM)) != RT_ERR_OK)
		return retVal;
#ifdef NO_FLOAT
	rtlglue_printf("Latch_TM_{%d} = %d/1024\n", tmid, Latch_TM);
#else
	rtlglue_printf("Latch_TM_{%d} = %f\n", tmid, Latch_TM);
#endif
	
	if ((retVal = unlatch_tm(tmid )) != RT_ERR_OK)
		return retVal;

	if ((retVal = get_junc_temp(tmid ,&TM)) != RT_ERR_OK)
		return retVal;
#ifdef NO_FLOAT
	rtlglue_printf("TM_{%d} = %d/1024 \n", tmid, TM);
#else
	rtlglue_printf("TM_{%d} = %f \n", tmid, TM);
#endif

	if ((retVal = rtl8373_setAsicRegBits(0x6454, 0x7000, 7)) != RT_ERR_OK) //open SMI，access PHY register 
	return retVal;

	if ((retVal = get_reg_a(tmid, &tm_reg_a)) != RT_ERR_OK) 
	return retVal;
	if ((retVal = get_reg_b(tmid, &tm_reg_b)) != RT_ERR_OK) 
	return retVal;

	rtlglue_printf("---Initial_Value: REG_A_{%d} & Reg_B_{%d}--- \n", tmid, tmid);
#ifdef NO_FLOAT
	rtlglue_printf("%u/16384 \t %d/2048\t \n", tm_reg_a, tm_reg_b);
#else
	rtlglue_printf("%f \t %f\t \n", tm_reg_a, tm_reg_b);
#endif
	rtlglue_printf("---Start_Updating: REG_A_{%d} & Reg_B_{%d}--- \n", tmid, tmid);
	
	if ((retVal = update_reg_a(tmid, &tm_reg_a)) != RT_ERR_OK) 
	return retVal;
	if ((retVal = update_reg_b(tmid, &tm_reg_b)) != RT_ERR_OK) 
	return retVal;

	if ((retVal = get_reg_a(tmid, &tm_reg_a_new)) != RT_ERR_OK) 
	return retVal;
	if ((retVal = get_reg_b(tmid, &tm_reg_b_new)) != RT_ERR_OK) 
	return retVal;

#ifdef NO_FLOAT
	rtlglue_printf("%u/16384\t %d/2048\t \n", tm_reg_a_new, tm_reg_b_new);
#else
	rtlglue_printf("%f\t %f\t \n", tm_reg_a_new, tm_reg_b_new);
#endif
	if ((retVal = get_junc_temp(tmid, &Latch_TM)) != RT_ERR_OK)
		return retVal;
#ifdef NO_FLOAT
	rtlglue_printf("Latch_TM_{%d} = %u/1024 \n", tmid, Latch_TM);
#else
	rtlglue_printf("Latch_TM_{%d} = %f \n", tmid, Latch_TM);
#endif

	rtlglue_printf("---TM interrupt related--- \n" );

	/*  # #tm_int_en $tmid all */

	if ((retVal = get_cmp_low_temp(tmid, &LOW_THR)) != RT_ERR_OK) 
		return retVal;
	if ((retVal = get_cmp_high_temp(tmid, &HIGH_THR)) != RT_ERR_OK) 
	return retVal;

	rtlglue_printf("LOW_THR_TM{%d} = %d  HIGH_THR_TM{%d} = %d \n", tmid, LOW_THR, tmid, HIGH_THR);

	if ((retVal = set_cmp_low_temp(tmid, -45)) != RT_ERR_OK) 
		return retVal;

	if ((retVal = set_cmp_high_temp(tmid, 125)) != RT_ERR_OK) 
		return retVal;

	if ((retVal = get_cmp_low_temp(tmid, &LOW_THR)) != RT_ERR_OK) 
		return retVal;
	if ((retVal = get_cmp_high_temp(tmid, &HIGH_THR)) != RT_ERR_OK) 
	return retVal;

	rtlglue_printf("LOW_THR_TM{%d} = %d  HIGH_THR_TM{%d} = %d \n", tmid, LOW_THR, tmid, HIGH_THR);

//# compare的功能完成High、Low设定后，默认DISABLE状态
	if ((retVal = cmp_low_temp_en(tmid, DISABLED)) != RT_ERR_OK) 
	return retVal;
	if ((retVal = cmp_high_temp_en(tmid, DISABLED)) != RT_ERR_OK) 
	return retVal;

	return RT_ERR_OK;
}


