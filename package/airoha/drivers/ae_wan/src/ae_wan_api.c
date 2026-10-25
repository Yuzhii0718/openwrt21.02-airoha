/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include "ae_wan_ic_dis.h"
#include "ae_wan_mac.h"
#include "ae_wan_api.h"
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_fe.h>



/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define MAC_L_BYTE_LEN  4
#define MAC_H_BYTE_LEN  2

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
extern unsigned int WAN_LINK_MODE;
extern int xsi_speed;
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int xsi_check_index_valid(uint hsgmii_index)
{
	if(hsgmii_index > XSI_IDX_MAX)
		return 0;
	else 
		return 1 ;

}


int xsi_api_set_mac(uint hsgmii_index,uint8 * mac_addess )
{
    unsigned int macAddrL=0, macAddrH=0;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
    else{
	    memcpy(&macAddrL, mac_addess, MAC_L_BYTE_LEN);
	    memcpy(&macAddrH, mac_addess + MAC_L_BYTE_LEN, MAC_H_BYTE_LEN);
	    
	    xsiSetMacAddrL(hsgmii_index,macAddrL);
	    xsiSetMacAddrH(hsgmii_index,macAddrH);
		return 0;
    }
}

void xsi_mac_itf_reset(uint hsgmii_index)
{
	if(0 == xsi_check_index_valid(hsgmii_index))
		return ;
	else{
	    xsiSetTxmbiDisable(hsgmii_index);
	    xsiSetTxmpiDisable(hsgmii_index);
	    xsiSetRxmbiDisable(hsgmii_index);
	    xsiSetRxmpiDisable(hsgmii_index);
	    xsiSetTxmpiIdle(hsgmii_index);

	    xsiSetTxmpiNormal(hsgmii_index);
	    xsiSetTxmbiEnable(hsgmii_index);
	    xsiSetTxmpiEnable(hsgmii_index);
	    xsiSetRxmbiEnable(hsgmii_index);
	    xsiSetRxmpiEnable(hsgmii_index);
		return;
	}
    
}
void xsi_mac_itf_close(uint hsgmii_index)
{
	if(0 == xsi_check_index_valid(hsgmii_index))
		return ;
	else{
	    xsiSetTxmbiDisable(hsgmii_index);
	    xsiSetTxmpiDisable(hsgmii_index);
	    xsiSetRxmbiDisable(hsgmii_index);
	    xsiSetRxmpiDisable(hsgmii_index);
	    xsiSetTxmpiIdle(hsgmii_index);
		return;
	}

}


int xsi_mac_set_ipg(uint hsgmii_index, uint ipg)
{

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsiSetIpgNum(hsgmii_index,ipg);
		return 0;
	}
}

int xsi_mac_set_lpbk_enable(uint hsgmii_index, uint lpbk_enable)
{

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		if(lpbk_enable)
			xsiSetLpbkEnable(hsgmii_index);
		else
			xsiSetLpbkDisable(hsgmii_index);
		return 0;
	}
}


int xsi_mac_set_tx_frag_len(uint hsgmii_index, uint frag_len)
{

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsiSetTxFragLen(hsgmii_index,frag_len);
	}
	return 0;
}


int xsi_mac_set_rx_frag_len(uint hsgmii_index, uint frag_len)
{

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsiSetRxFragLen(hsgmii_index,frag_len);
		return 0;
	}
}

int xsi_mac_set_mpi_mbi_enable(uint hsgmii_index)
{

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsiSetRxmpiEnable(hsgmii_index);
		xsiSetRxmbiEnable(hsgmii_index);
		xsiSetTxmpiEnable(hsgmii_index);
		xsiSetTxmbiEnable(hsgmii_index);
		return 0;
	}
}

int xsi_mac_set_mpi_mbi_disable(uint hsgmii_index)
{

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsiSetRxmpiDisable(hsgmii_index);
		xsiSetRxmbiDisable(hsgmii_index);
		xsiSetTxmpiDisable(hsgmii_index);
		xsiSetTxmbiDisable(hsgmii_index);
		return 0;
	}
}




int get_bucketsize_shift(uint value, uint lo, uint hi, uint unit)
{
	int mid = 0;
	
	if(lo > hi )
		return -EINVAL;

	if((value > 0) && (value < unit))
		return 0;

	mid = (lo + hi) / 2;

	if((unit<<mid) == value){
		return mid;
		
	}else if((unit<<mid) > value){
		if((mid - lo) <= 1)
			return mid;
		
		return get_bucketsize_shift(value, lo, mid, unit);
	}else{
		if((hi - mid) <= 1)
			return hi;
		
		return get_bucketsize_shift(value, mid, hi, unit);
	}
}

int hsgmii_set_ratelimit_param(uint hsgmii_index,uint paraType,uint cfg_id,uint valueLo)
{
	uint paraCfg = 0;
	uint rc_cfg_reg = 0;
	uint rc_data_L_reg = 0;
	
	if( (hsgmii_index >= XSI_PCIE0_IDX) && (hsgmii_index <= XSI_IDX_MAX))
	{
		if(cfg_id >= HSGMII_RX_UC_RATE && cfg_id <= HSGMII_RX_MC_RATE ){
			rc_cfg_reg = HSGMII_BASE_REG[hsgmii_index]+ RX_CFG ;
			rc_data_L_reg = HSGMII_BASE_REG[hsgmii_index]+ RC_WR_DATA_L;
		}else if(SUPPORT_TX_TOTAL_RATELIMIT && (cfg_id == HSGMII_TX_TOTAL_RATE)){
			rc_cfg_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_TX_CFG ;
			rc_data_L_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RC_WR_DATA_L;
		}else if(cfg_id == HSGMII_RX_TOTAL_RATE){
			rc_cfg_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RX_CFG ;
			rc_data_L_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RC_WR_DATA_L;
//			cfg_id = 0;

		}
		
	}
	else 
	{
		return 0;
		
	}

	//printk("1 %08x %08x \n",rc_cfg_reg, rc_data_L_reg);
	
	write_reg_word(rc_data_L_reg, valueLo);
	
	paraCfg = (HSGMII_RC_CFG_EN | HSGMII_RC_CFG_PARA_RWCMD | ((paraType<<HSGMII_RC_CFG_PARA_TYPE_SHIFT)&HSGMII_RC_CFG_PARA_TYPE_MASK) | 
		((cfg_id<<HSGMII_RC_CFG_ID_SHIFT)&HSGMII_RC_CFG_ID_MASK));

	write_reg_word(rc_cfg_reg, paraCfg);
	
	printk("rc_cfg_reg[%x] = 0x%x, rc_data_L_reg[%x]=0x%x\n", rc_cfg_reg, paraCfg, rc_data_L_reg, valueLo);
	
	return 0;
}

int hsgmii_get_ratelimit_param(uint hsgmii_index,uint paraType,uint cfg_id,uint *valueLo,uint *valueHi)
{
	uint paraCfg = 0;
	uint rc_cfg_reg = 0;
	uint rc_data_L_reg = 0;
	uint rc_data_H_reg = 0;

	if( (hsgmii_index >= XSI_PCIE0_IDX) && (hsgmii_index <= XSI_IDX_MAX))
	{
		if(cfg_id >= HSGMII_RX_UC_RATE && cfg_id <= HSGMII_RX_MC_RATE ){
			rc_cfg_reg = HSGMII_BASE_REG[hsgmii_index]+ RX_CFG ;
			rc_data_L_reg = HSGMII_BASE_REG[hsgmii_index]+ RC_RD_DATA_L;
			rc_data_H_reg = HSGMII_BASE_REG[hsgmii_index]+ RC_RD_DATA_H;
		}else if(SUPPORT_TX_TOTAL_RATELIMIT && (cfg_id == HSGMII_TX_TOTAL_RATE)){
			rc_cfg_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_TX_CFG ;
			rc_data_L_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RC_RD_DATA_L;
			rc_data_H_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RC_RD_DATA_H;
		}else if(cfg_id == HSGMII_RX_TOTAL_RATE){
			rc_cfg_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RX_CFG ;
			rc_data_L_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RC_RD_DATA_L;
			rc_data_H_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RC_RD_DATA_H;
			cfg_id = 0;
		}
	}
	else 
	{
		return 0;
		
	}

//	printk("1 %08x %08x \n",rc_cfg_reg, rc_data_L_reg);
	
	paraCfg = (((paraType<<HSGMII_RC_CFG_PARA_TYPE_SHIFT)&HSGMII_RC_CFG_PARA_TYPE_MASK) | 
		((cfg_id<<HSGMII_RC_CFG_ID_SHIFT)&HSGMII_RC_CFG_ID_MASK));

	write_reg_word(rc_cfg_reg, paraCfg);
	
	*valueLo = read_reg_word(rc_data_L_reg);
	*valueHi = read_reg_word(rc_data_H_reg);
	
	return 0;
}



int xsi_mac_set_ratelimit(uint hsgmii_index, unsigned int type, uint rate, uint mode)
{

	unsigned int val = 0;
	unsigned int meter_en = 0;
	unsigned int tickSel = 0;
	unsigned int rateLimitUnit = 0;
	int bucketSize_shift = 0;
	int curTicksel = 0,bucketSize=0;
	uint tokenRate = 0;
	unsigned int rx_rc_cfg_reg = 0;
	unsigned int rc_data_h_reg = 0;
	uint tokenRate_integer = 0;
	unsigned int tokenRate_fraction = 0;
	uint valueLo = 0, valueHi = 0;

	
	if( (hsgmii_index >= XSI_PCIE0_IDX) && (hsgmii_index <= XSI_IDX_MAX))
	{
		if(type >= HSGMII_RX_UC_RATE && type <= HSGMII_RX_MC_RATE ){
			rx_rc_cfg_reg = HSGMII_BASE_REG[hsgmii_index]+ RX_RC_CFG ;
			rc_data_h_reg = HSGMII_BASE_REG[hsgmii_index]+ RC_WR_DATA_H;
		}else if(SUPPORT_TX_TOTAL_RATELIMIT && (type == HSGMII_TX_TOTAL_RATE)){
			rx_rc_cfg_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_TX_RC_CFG ;
			rc_data_h_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RC_WR_DATA_H;
		}else if(type == HSGMII_RX_TOTAL_RATE){
			rx_rc_cfg_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RX_RC_CFG ;
			rc_data_h_reg = HSGMII_BASE_REG[hsgmii_index]+ TOTAL_RC_WR_DATA_H;
		}
	}
	else 
	{
		return 0;
		
	}

//	printk("1 %08x %08x \n",rx_rc_cfg_reg, rc_data_h_reg);
	
	/* check rate value */
	if(0 == rate)
	{
		meter_en = HSGMII_RC_CFG_METER_DISABLE;
	}
	else
	{
		meter_en = HSGMII_RC_CFG_METER_ENABLE;
	}
	
	if(rate == 0){
		write_reg_word(rx_rc_cfg_reg, 0);
		printk("disable use rc_cfg_reg[%x] = 0x%x, rc_data_h_reg[%x]=0\n", rx_rc_cfg_reg, 0, rc_data_h_reg);
	}
	else if((mode == HSGMII_RC_CFG_BYTE_MODE) || ((mode == HSGMII_RC_CFG_PKT_MODE) && (rate > 20000)))
	{
		/* default tick 125us */
		curTicksel = 125;
		tickSel = HSGMII_RC_CFG_FAST_TICK;
		write_reg_word(rx_rc_cfg_reg,0x8001007d);
		printk("byte mode user c_cfg_reg[%x] = 0x%x, rc_data_h_reg[%x]=0\n", rx_rc_cfg_reg, 0x8001007d, rc_data_h_reg);
	}
	else  //HSGMII_RC_CFG_PKT_MODE && rate < 20000
	{
		curTicksel = 125*32;
		tickSel = HSGMII_RC_CFG_SLOW_TICK;
		write_reg_word(rx_rc_cfg_reg,0x8020007d);
		printk("packet mode use rc_cfg_reg[%x] = 0x%x, rc_data_h_reg[%x]=0\n", rx_rc_cfg_reg, 0x8020007d, rc_data_h_reg);
	}
	write_reg_word(rc_data_h_reg,0x0);

	/* set basic parameters */
	hsgmii_get_ratelimit_param(hsgmii_index,HSGMII_RC_CFG_PARA_MISC, type, &valueLo, &valueHi);
	valueLo = (meter_en == HSGMII_RC_CFG_METER_ENABLE) ? (valueLo|HSGMII_RC_CFG_PARA_METER_EN):(valueLo &(~HSGMII_RC_CFG_PARA_METER_EN));
	valueLo = (mode == HSGMII_RC_CFG_PKT_MODE) ? (valueLo|HSGMII_RC_CFG_PARA_PPS_MODE):(valueLo &(~HSGMII_RC_CFG_PARA_PPS_MODE));
	valueLo = (tickSel == HSGMII_RC_CFG_SLOW_TICK) ? (valueLo|HSGMII_RC_CFG_PARA_TICK_SEL):(valueLo &(~HSGMII_RC_CFG_PARA_TICK_SEL));
	printk("set basicParameter: \t");
	hsgmii_set_ratelimit_param(hsgmii_index,HSGMII_RC_CFG_PARA_MISC, type, valueLo);
	
	/* set token unit  */
	if(HSGMII_RC_CFG_BYTE_MODE == mode)
	{
		/* 8bits X 1000 / (curTicksel X 10e-6 s)  kbps */
		rateLimitUnit = (8000 / curTicksel);
	}
	else
	{
		/* 1 / (curTicksel X 10e-6 s) pps */
		rateLimitUnit = (1000000 / curTicksel);
	}
	
	if(0 == rateLimitUnit)
	{
		printk("rateLimitUnit = 0 , set error.\n");
		return -EINVAL;
	}
	
	/* calculate tokenRate */
	tokenRate_integer = (rate / rateLimitUnit);
	tokenRate_fraction = ((rate % rateLimitUnit) * 64 / rateLimitUnit);
	if((tokenRate_integer > 0x3FFFF) || (tokenRate_fraction > 0x3F))
	{
		printk("tokenRate overflow.\n");
		return -EINVAL;
	}
	
	/* set token rate */
	tokenRate = ((tokenRate_integer << RC_TOKEN_RATE_INTEGER_SHIFT) | tokenRate_fraction);
	printk("set tokenRate: \t\t");
	hsgmii_set_ratelimit_param(hsgmii_index,HSGMII_RC_CFG_PARA_TOKEN_RATE, type, tokenRate);
	
	
	/* get bucket size  */
	if(HSGMII_RC_CFG_BYTE_MODE == mode)
	{
		bucketSize = (rate<<RC_BYTE_MODE_BUCKET_SHIFT);
	}
	else
	{
		bucketSize = (rate<<RC_PKT_MODE_BUCKET_SHIFT);
	}

	/*max: 32M bucket size */
	if(bucketSize > 0x2000000)
	{
		bucketSize = 0x2000000;
	}
	
	/* set bucketsize_shift */
	bucketSize_shift = get_bucketsize_shift(bucketSize, 0, 15, 1024);
	printk("set bucketSize_shift: \t");
	hsgmii_set_ratelimit_param(hsgmii_index,HSGMII_RC_CFG_PARA_BUCK_SHIFT, type, bucketSize_shift);
	
	return 0;
}


int xsi_mac_api_set_rxuc_rate(ECNT_AE_WAN_Data_s *xsi_data)
{
	uint hsgmii_index = 0,  rate = 0,  mode = 0;
	hsgmii_index = xsi_data->hsgmii_index;
	rate = xsi_data->xsi_private.rate_cfg.rate;
	mode = xsi_data->xsi_private.rate_cfg.mode;


	
	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsi_mac_set_ratelimit(hsgmii_index, HSGMII_RX_UC_RATE, rate, mode);
		return 0;
	}
}

int xsi_mac_api_set_rxbc_rate(ECNT_AE_WAN_Data_s *xsi_data)
{
	uint hsgmii_index = 0,	rate = 0,  mode = 0;
	hsgmii_index = xsi_data->hsgmii_index;
	rate = xsi_data->xsi_private.rate_cfg.rate;
	mode = xsi_data->xsi_private.rate_cfg.mode;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsi_mac_set_ratelimit(hsgmii_index, HSGMII_RX_BC_RATE, rate, mode);
		return 0;
	}
}

int xsi_mac_api_set_rxmc_rate(ECNT_AE_WAN_Data_s *xsi_data)
{
	
	uint hsgmii_index = 0,	rate = 0,  mode = 0;
	hsgmii_index = xsi_data->hsgmii_index;
	rate = xsi_data->xsi_private.rate_cfg.rate;
	mode = xsi_data->xsi_private.rate_cfg.mode;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsi_mac_set_ratelimit(hsgmii_index, HSGMII_RX_MC_RATE, rate, mode);
		return 0;
	}
}


int xsi_mac_api_set_rxtotal_rate(ECNT_AE_WAN_Data_s *xsi_data)
{
	
	uint hsgmii_index = 0,	rate = 0,  mode = 0;
	hsgmii_index = xsi_data->hsgmii_index;
	rate = xsi_data->xsi_private.rate_cfg.rate;
	mode = xsi_data->xsi_private.rate_cfg.mode;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsi_mac_set_ratelimit(hsgmii_index, HSGMII_RX_TOTAL_RATE, rate, mode);
		return 0;
	}
}

int xsi_mac_api_set_txtotal_rate(ECNT_AE_WAN_Data_s *xsi_data)
{
	
	uint hsgmii_index = 0,	rate = 0,  mode = 0;
	hsgmii_index = xsi_data->hsgmii_index;
	rate = xsi_data->xsi_private.rate_cfg.rate;
	mode = xsi_data->xsi_private.rate_cfg.mode;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		xsi_mac_set_ratelimit(hsgmii_index, HSGMII_TX_TOTAL_RATE, rate, mode);
		return 0;
	}
}

#if SUPPORT_AE_WAN_NEW_ARCH

/*
hsgmii index in mac driver:
#define	XSI_PCIE0_IDX	0
#define	XSI_PCIE1_IDX	1
#define	XSI_USB_IDX		2
#define XSI_AE_IDX		3
#define XSI_ETH_IDX		4

in put for phy api
typedef enum{
	ENUM_VERSION,
	ENUM_ETH_SERDES,
	ENUM_XPON_SERDES,
	ENUM_PCIE0_SERDES,
	ENUM_PCIE1_SERDES,
	ENUM_USB_SERDES,
	ENUM_UNKNOW_SERDES,
}xsgmii_serdes_inphy;	

xsgmii_speed :
typedef enum{
	USXGII_10G,
	USXGII_5G,
	USXGII_2p5G,
	USXGII_1G,
	USXGII_100M,
	HSGMII_2p5G,
	SGMII_1000M,
	SGMII_100M,
	SGMII_10M,
	SPEED_UNKNOW,
}xsgmii_speed_type;

mod:
0:force mode
1:an auto mode
2: linkst

an:
0: an disable
1: an enable

*/

int xsi_mac_api_set_xsgmii_phy_speed(uint hsgmii_index , uint xsgmii_speed, uint mod ,uint an )
{
	unsigned char serdes = 0,xsgmii = 0,  rate = 0;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	
	if(hsgmii_index == XSI_PCIE0_IDX){
		serdes = ENUM_PCIE0_SERDES;
		WAN_LINK_MODE = xsgmii_speed;
	}else if(hsgmii_index == XSI_PCIE1_IDX){
		serdes = ENUM_PCIE1_SERDES;
		WAN_LINK_MODE = xsgmii_speed;
	}else if(hsgmii_index == XSI_USB_IDX){
		serdes = ENUM_USB_SERDES;
		WAN_LINK_MODE = xsgmii_speed;
	}else if(hsgmii_index == XSI_AE_IDX){
		serdes = ENUM_XPON_SERDES;//lan driver not support set pon serdes
		WAN_LINK_MODE = xsgmii_speed;
	}else if(hsgmii_index == XSI_ETH_IDX){
		serdes = ENUM_ETH_SERDES;
		WAN_LINK_MODE = xsgmii_speed;
	}else 
		return 0;

	if(xsgmii_speed == USXGII_10G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_10G;
	}else if(xsgmii_speed == USXGII_5G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_5G;
	}else if(xsgmii_speed == USXGII_2p5G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_2p5G;
	}else if(xsgmii_speed == USXGII_1G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_1G;
	}else if(xsgmii_speed == USXGII_100M){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_100M;
	}else if(xsgmii_speed == HSGMII_2p5G){
		xsgmii = HSGMII;
		rate = 0;/*hsgmii no need set rate*/
	}else if(xsgmii_speed == SGMII_1000M){
		xsgmii = SGMII;
		rate = ENUM_SGMII_1000M;
	}else if(xsgmii_speed == SGMII_100M){
		xsgmii = SGMII;
		rate = ENUM_SGMII_100M;
	}else if(xsgmii_speed == SGMII_10M){
		xsgmii = SGMII;
		rate = ENUM_SGMII_10M;
	}else 
		return 0;

	xsgmii_api(serdes,xsgmii,mod,rate,an);
	
	return 0 ;

}

//normal mode force amd an disable
int xsi_set_xsgmii_serdes_speed_forcemode_andisable(uint hsgmii_index , uint xsgmii_speed )
{
	return xsi_mac_api_set_xsgmii_phy_speed(hsgmii_index, xsgmii_speed, 0,0);
}

//set phy force and an auto
int xsi_set_xsgmii_serdes_speed_forcemode_anauto(uint hsgmii_index , uint xsgmii_speed )
{
	return xsi_mac_api_set_xsgmii_phy_speed(hsgmii_index, xsgmii_speed, 1,0);
}

//set phy force mode and an auon
int xsi_set_xsgmii_serdes_speed_forcemode_anon(uint hsgmii_index, uint xsgmii_speed )
{
	return xsi_mac_api_set_xsgmii_phy_speed(hsgmii_index, xsgmii_speed, 0,1);
}

//enable phy interrput xsgmii_api(1,0,7,1,1)
int xsi_set_xsgmii_serdes_enable_interrupt(uint hsgmii_index)
{
	unsigned char serdes = 0,xsgmii = 0,  rate = 0;
	
	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	
	if(hsgmii_index == XSI_PCIE0_IDX){
		serdes = ENUM_PCIE0_SERDES;
	}else if(hsgmii_index == XSI_PCIE1_IDX){
		serdes = ENUM_PCIE1_SERDES;
	}else if(hsgmii_index == XSI_USB_IDX){
		serdes = ENUM_USB_SERDES;
	}else if(hsgmii_index == XSI_AE_IDX){
		serdes = ENUM_XPON_SERDES;
	}else if(hsgmii_index == XSI_ETH_IDX){
		serdes = ENUM_ETH_SERDES;
	}else 
		return 0;

	xsgmii_api(serdes, USXGMII, 7, 1, 1);
	return 0;
}

//enable phy interrput xsgmii_api(1,0,252,11,1)
int xsi_set_xsgmii_serdes_aeonsemi_phy_preset(uint hsgmii_index)
{
	unsigned char serdes = 0,xsgmii = 0;
	
	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	
	if(hsgmii_index == XSI_PCIE0_IDX){
		serdes = ENUM_PCIE0_SERDES;
	}else if(hsgmii_index == XSI_PCIE1_IDX){
		serdes = ENUM_PCIE1_SERDES;
	}else if(hsgmii_index == XSI_USB_IDX){
		serdes = ENUM_USB_SERDES;
	}else if(hsgmii_index == XSI_AE_IDX){
		serdes = ENUM_XPON_SERDES;
	}else if(hsgmii_index == XSI_ETH_IDX){
		serdes = ENUM_ETH_SERDES;
	}else 
		return 0;

	xsgmii_api(serdes, USXGMII, 252, 11, 1);
	return 0;
}


unsigned int xsi_mac_api_set_xsgmii_phy_linkstats(uint hsgmii_index, uint linkstats)
{
	unsigned char serdes = 0,xsgmii = 0,  rate = 0;
	unsigned int xsgmii_speed = 0;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return 0;
	
	if(hsgmii_index == XSI_PCIE0_IDX){
		serdes = ENUM_PCIE0_SERDES;
		xsgmii_speed = WAN_LINK_MODE;
	}else if(hsgmii_index == XSI_PCIE1_IDX){
		serdes = ENUM_PCIE1_SERDES;
		xsgmii_speed = WAN_LINK_MODE;
	}else if(hsgmii_index == XSI_USB_IDX){
		serdes = ENUM_USB_SERDES;
		xsgmii_speed = WAN_LINK_MODE;
	}else if(hsgmii_index == XSI_AE_IDX){
		serdes = ENUM_XPON_SERDES;//lan driver not support set pon serdes
		return 0;
	}else if(hsgmii_index == XSI_ETH_IDX){
		serdes = ENUM_ETH_SERDES;
		xsgmii_speed = WAN_LINK_MODE;
	}else 
		return 0;


	if(xsgmii_speed == USXGII_10G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_10G;
	}else if(xsgmii_speed == USXGII_5G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_5G;
	}else if(xsgmii_speed == USXGII_2p5G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_2p5G;
	}else if(xsgmii_speed == USXGII_1G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_1G;
	}else if(xsgmii_speed == USXGII_100M){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_100M;
	}else if(xsgmii_speed == HSGMII_2p5G){
		xsgmii = HSGMII;
		rate = 0;/*hsgmii no need set rate*/
	}else if(xsgmii_speed == SGMII_1000M){
		xsgmii = SGMII;
		rate = ENUM_SGMII_1000M;
	}else if(xsgmii_speed == SGMII_100M){
		xsgmii = SGMII;
		rate = ENUM_SGMII_100M;
	}else if(xsgmii_speed == SGMII_10M){
		xsgmii = SGMII;
		rate = ENUM_SGMII_10M;
	}else 
		return 0;

	xsgmii_api(20, serdes, linkstats, xsgmii, 0);

	return 0;

}

int xsi_get_xsgmii_sfp_linkstatus(uint hsgmii_index)
{
	unsigned int linkstatus = 0;
	linkstatus= (xsgmii_api(ENUM_XPON_SERDES,USXGMII,3,1,0)&0x8000)>>15;
	
	return linkstatus;

}

//return value bit0 sync, bit1 an 
unsigned int xsi_mac_api_get_xsgmii_phy_linkstats(uint hsgmii_index)
{
	unsigned char serdes = 0,xsgmii = 0,  rate = 0;
	unsigned int xsgmii_speed = 0, linkstats = 0;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	
	if(hsgmii_index == XSI_PCIE0_IDX){
		serdes = ENUM_PCIE0_SERDES;
	}else if(hsgmii_index == XSI_PCIE1_IDX){
		serdes = ENUM_PCIE1_SERDES;
	}else if(hsgmii_index == XSI_USB_IDX){
		serdes = ENUM_USB_SERDES;
	}else if(hsgmii_index == XSI_AE_IDX){
		serdes = ENUM_XPON_SERDES;
	}else if(hsgmii_index == XSI_ETH_IDX){
		serdes = ENUM_ETH_SERDES;
	}else 
		return 0;

	xsgmii_speed = WAN_LINK_MODE;


	if(xsgmii_speed == USXGII_10G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_10G;
	}else if(xsgmii_speed == USXGII_5G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_5G;
	}else if(xsgmii_speed == USXGII_2p5G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_2p5G;
	}else if(xsgmii_speed == USXGII_1G){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_1G;
	}else if(xsgmii_speed == USXGII_100M){
		xsgmii = USXGMII;
		rate = ENUM_USXGII_100M;
	}else if(xsgmii_speed == HSGMII_2p5G){
		xsgmii = HSGMII;
		rate = 0;/*hsgmii no need set rate*/
	}else if(xsgmii_speed == SGMII_1000M){
		xsgmii = SGMII;
		rate = ENUM_SGMII_1000M;
	}else if(xsgmii_speed == SGMII_100M){
		xsgmii = SGMII;
		rate = ENUM_SGMII_100M;
	}else if(xsgmii_speed == SGMII_10M){
		xsgmii = SGMII;
		rate = ENUM_SGMII_10M;
	}else 
		return 0;

	linkstats = xsgmii_api(serdes,xsgmii,2,rate,0);
	
	return linkstats ;

}

int xsi_mac_api_logic_reset(int hsgmii_index)
{
	uint fe_arb_sts = 0;
	int i = 0;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	//check mac itf sts
	if((xsiGetTxmbiStopSts(hsgmii_index) == 0) && 
		(xsiGetRxmbiStopSts(hsgmii_index) == 0) &&
		(xsiGetRxmpiStopSts(hsgmii_index) == 0))
	{
		xsi_mac_set_mpi_mbi_disable(hsgmii_index);
		mdelay(1);
	}
	else
	{
		printk("%s:%d, error xfi_if_sts!\n",__FUNCTION__,__LINE__);
		return -1;
	}
	//logic reset
	xsimaclogicrst(hsgmii_index);

	//release sequence as follow
	//release mac logic_rst
	xsimaclogicrstenable(hsgmii_index);
	//do cnt clear	
	xsiClearAllCnt(hsgmii_index);

	//mbi & mpi enable
	xsi_mac_set_mpi_mbi_enable(hsgmii_index);
	
	return 0;
}
int xsi_mac_api_reset(int hsgmii_index)
{
	uint32 default_val = 0;
	if(hsgmii_index == XSI_PCIE0_IDX)
	{
		default_val = GET_SCU_RST_RG();
		SET_SCU_RST_RG(default_val | XFI_MAC_PCIE0_RST);
		SET_SCU_RST_RG(default_val & (~XFI_MAC_PCIE0_RST));
	}
	else if(hsgmii_index == XSI_PCIE1_IDX)
	{
		default_val = GET_SCU_RST_RG();
		SET_SCU_RST_RG(default_val | XFI_MAC_PCIE1_RST);
		SET_SCU_RST_RG(default_val & (~XFI_MAC_PCIE1_RST));
	}
	else if(hsgmii_index == XSI_USB_IDX)
	{
		default_val = GET_SCU_RST_RG();
		SET_SCU_RST_RG(default_val | XFI_MAC_USB_RST);
		SET_SCU_RST_RG(default_val & (~XFI_MAC_PCIE0_RST));
	}
	else if(hsgmii_index == XSI_AE_IDX)
	{
		default_val = GET_SCU_RST_RG();
		SET_SCU_RST_RG(default_val | XFI_MAC_AE_RST);
		SET_SCU_RST_RG(default_val & (~XFI_MAC_AE_RST));
	}
	else if(hsgmii_index == XSI_ETH_IDX)
	{
		default_val = GET_SCU_RST_RG();
		SET_SCU_RST_RG(default_val | XFI_MAC_ETH_RST);
		SET_SCU_RST_RG(default_val & (~XFI_MAC_ETH_RST));
	}
	else
	{
		return 0;
	}
	return 0;
}

//set phy A through mdio
int xsi_set_mdio(void)
{	
	ETHER_MDIO_WRITE(0,0x1f,0);
	ETHER_MDIO_WRITE(0,0xd,4);
	ETHER_MDIO_WRITE(0,0xe,0xc441);
	ETHER_MDIO_WRITE(0,0xd,0x8004);
	ETHER_MDIO_WRITE(0,0xe,0x8);
	return 0;
}

#endif

//set fe gdm frag
int xsi_mac_api_set_rx_frag(uint hsgmii_index)
{
	if(0 == xsi_check_index_valid(hsgmii_index))
		return 0;

	//link rate > 1G, frag set 8(16*8bytes), link rate <= 1G frag set 0 (16bytes);
	//0:10g, 1:5g, 2:2.5g, 3:1g, 4:100M, 5:10M
	switch (hsgmii_index)
	{
		case XSI_PCIE0_IDX:
		{
			if(xsi_speed < 2)
			{
				xsi_mac_api_set_pcie_rx_frag(XSI_PCIE0_IDX, 8);
			}
			else if(xsi_speed == 2)
			{
				xsi_mac_api_set_pcie_rx_frag(XSI_PCIE0_IDX, 1);
			}
			else
			{
				xsi_mac_api_set_pcie_rx_frag(XSI_PCIE0_IDX, 0);
			}
		}
		break;
		
		case XSI_PCIE1_IDX:
		{
			if(xsi_speed < 2)
			{
				xsi_mac_api_set_pcie_rx_frag(XSI_PCIE1_IDX, 8);
			}
			else if(xsi_speed == 2)
			{
				xsi_mac_api_set_pcie_rx_frag(XSI_PCIE1_IDX, 1);
			}
			else
			{
				xsi_mac_api_set_pcie_rx_frag(XSI_PCIE1_IDX, 0);
			}
		}
		break;

		case XSI_USB_IDX:
		{
			if(xsi_speed < 2)
			{
				FE_API_SET_RMBI_FRAG(FE_GDM_SEL_GDMA4, 1, 8);
			}
			else if(xsi_speed == 2)
			{
				FE_API_SET_RMBI_FRAG(FE_GDM_SEL_GDMA4, 1, 1);
			}
			else
			{
				FE_API_SET_RMBI_FRAG(FE_GDM_SEL_GDMA4, 1, 0);
			}
		}
		break;

		case XSI_ETH_IDX:
		{
			if(xsi_speed < 3)
			{
				FE_API_SET_RMBI_FRAG(FE_GDM_SEL_GDMA4, 0, 8);
			}
			else if(xsi_speed == 2)
			{
				FE_API_SET_RMBI_FRAG(FE_GDM_SEL_GDMA4, 0, 1);
			}
			else
			{
				FE_API_SET_RMBI_FRAG(FE_GDM_SEL_GDMA4, 0, 0);
			}
		}
		break;

		default :
            break;
			
	}

	return 0;

}

//set fe gdm frag
int xsi_mac_api_set_tx_frag(uint hsgmii_index)
{
	if(0 == xsi_check_index_valid(hsgmii_index))
		return 0;

	//link rate > 2.5G, frag set 8(16*8bytes), link rate == 2.5G, frag set 2(16*2bytes), link rate <= 1G frag set 1 (16bytes);
	//0:10g, 1:5g, 2:2.5g, 3:1g, 4:100M, 5:10M
	switch (hsgmii_index)
	{
		case XSI_PCIE0_IDX:
		{
			if(xsi_speed < 2)
			{
				xsi_mac_api_set_pcie_tx_frag(XSI_PCIE0_IDX, 8);
			}
			else if(xsi_speed == 2)
			{
				xsi_mac_api_set_pcie_tx_frag(XSI_PCIE0_IDX, 2);
			}
			else
			{
				xsi_mac_api_set_pcie_tx_frag(XSI_PCIE0_IDX, 1);
			}
		}

		break;
		
		case XSI_PCIE1_IDX:
		{
			if(xsi_speed < 2)
			{
				xsi_mac_api_set_pcie_tx_frag(XSI_PCIE1_IDX, 8);
			}
			else if(xsi_speed == 2)
			{
				xsi_mac_api_set_pcie_tx_frag(XSI_PCIE1_IDX, 2);
			}
			else
			{
				xsi_mac_api_set_pcie_tx_frag(XSI_PCIE1_IDX, 1);
			}
		}
		break;

		case XSI_USB_IDX:
		{
			if(xsi_speed < 2)
			{
				FE_API_SET_TMBI_FRAG(FE_GDM_SEL_GDMA4, 1, 8);
			}
			else if(xsi_speed == 2)
			{
				FE_API_SET_TMBI_FRAG(FE_GDM_SEL_GDMA4, 1, 2);
			}
			else
			{
				FE_API_SET_TMBI_FRAG(FE_GDM_SEL_GDMA4, 1, 1);
			}
		}
		break;

		case XSI_ETH_IDX:
		{
			if(xsi_speed < 2)
			{
				FE_API_SET_TMBI_FRAG(FE_GDM_SEL_GDMA4, 0, 8);
			}
			else if(xsi_speed == 2)
			{
				FE_API_SET_TMBI_FRAG(FE_GDM_SEL_GDMA4, 0, 2);
			}
			else
			{
				FE_API_SET_TMBI_FRAG(FE_GDM_SEL_GDMA4, 0, 1);
			}
		}
		break;

		default :
            break;
			
	}

	return 0;

}

int xsi_mac_api_set_pcie_tx_frag(uint hsgmii_index, int frag_size)
{
	uint32 val = 0;
	if(0 == xsi_check_index_valid(hsgmii_index))
		return 0;
	val = read_reg_word(XFI_MAC_PCIE_TXARB_FRAG);
	if(hsgmii_index == XSI_PCIE0_IDX)
	{
		val = val & (~XFI_MAC_PCIE0_TXARB_FRAG_MASK) | (frag_size<<XFI_MAC_PCIE0_TXARB_FRAG_SHIFT);
	}
	else if(hsgmii_index == XSI_PCIE1_IDX)
	{
		val = val & (~XFI_MAC_PCIE1_TXARB_FRAG_MASK) | (frag_size<<XFI_MAC_PCIE1_TXARB_FRAG_SHIFT);
	}
	write_reg_word(XFI_MAC_PCIE_TXARB_FRAG,val);

	return 0;
}

int xsi_mac_api_set_pcie_rx_frag(uint hsgmii_index, int frag_size)
{
	uint32 val = 0;
	if(0 == xsi_check_index_valid(hsgmii_index))
		return 0;
	val = read_reg_word(XFI_MAC_PCIE_RXARB_FRAG);
	if(hsgmii_index == XSI_PCIE0_IDX)
	{
		val = val & (~XFI_MAC_PCIE0_RXARB_FRAG_MASK) | (frag_size<<XFI_MAC_PCIE0_RXARB_FRAG_SHIFT);
	}
	else if(hsgmii_index == XSI_PCIE1_IDX)
	{
		val = val & (~XFI_MAC_PCIE1_RXARB_FRAG_MASK) | (frag_size<<XFI_MAC_PCIE1_RXARB_FRAG_SHIFT);
	}
	write_reg_word(XFI_MAC_PCIE_RXARB_FRAG,val);

	return 0;
}

int xsi_mac_set_lpi(uint hsgmii_index, uint enable, uint direction)
{

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		if(direction == MAC_TX){
			if(enable == 1){
				xsiSetTxLpiTimeoutThr(hsgmii_index, 0x2);
				xsiSetTxLpiGen(hsgmii_index, enable);	
			}else{
				xsiSetTxLpiTimeoutThr(hsgmii_index, 0xFF);
				xsiSetTxLpiGen(hsgmii_index, 0);
			}			
		}else if(direction == MAC_RX){
			if(enable == 1){
				xsiSetRxParserID(hsgmii_index, 0);
				xsiSetRxParserRXCCoding(hsgmii_index, 0xFF);
				xsiSetRxParserRXDCodingL(hsgmii_index, 0x07070707);
				xsiSetRxParserRXDCodingH(hsgmii_index, 0x07070707);
				xsiSetRxParserEn(hsgmii_index, 0xFF);
				
				xsiSetRxParserID(hsgmii_index, 1);
				xsiSetRxParserRXCCoding(hsgmii_index, 0xFF);
				xsiSetRxParserRXDCodingL(hsgmii_index, 0x06060606);
				xsiSetRxParserRXDCodingH(hsgmii_index, 0x06060606);
				xsiSetRxParserEn(hsgmii_index, 0xFF);
				
				xsiSetRxLpiChkEn(hsgmii_index, enable);
			}else{
				xsiSetRxLpiChkEn(hsgmii_index, 0);
			}
		}
	}
	return 0;
}

int xsi_mac_get_lpi(uint hsgmii_index,  uint direction)
{
	uint status = 0;

	if(0 == xsi_check_index_valid(hsgmii_index))
		return -1;
	else{
		if(direction == MAC_TX){
			status = xsiGetTxLpiOn(hsgmii_index);
			printk("lpi Tx status %s", status?"on":"off");
			printk("event = %d, lpi duration = 0x%x \n", xsiGetTxLpiEventCnt(hsgmii_index), xsiGetTxLpiDurationCnt(hsgmii_index));
		}else if(direction == MAC_RX){		
			status = xsiGetRxLpiOn(hsgmii_index);
			printk("lpi Rx status %s, ", status?"on":"off");
			printk("event = %d, lpi duration = 0x%x \n", xsiGetRxLpiEventCnt(hsgmii_index), xsiGetRxLpiDurationCnt(hsgmii_index));
		}

		return status;
	}
}

