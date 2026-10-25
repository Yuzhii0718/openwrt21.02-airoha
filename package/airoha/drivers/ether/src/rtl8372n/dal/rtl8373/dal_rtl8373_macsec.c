/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : RTK switch high-level API for RTL8367/RTL8373
 * Feature : Here is a list of all functions and variables in LUT module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8373/dal_rtl8373_macsec.h>
#include <dal/rtl8373/dal_rtl8373_macsec_ip_reg.h>
#include <dal/rtl8373/dal_rtl8373_drv.h>
#include <rtl8373_asicdrv.h>
#include <linux/string.h>
#include <aes.h>



rtl8373_macsec_info_t    *port_macsec_info = { NULL };
rtl8373_macsec_info_t macsec_info;
rtl8373_macsec_port_info_t macsec_portinfo[RTL8373_MACSEC_PORT_NUM];





/* Function Name:
 *      dal_rtl8373_macsec_enable_set
 * Description:
 *      Configure macsec enable.
 * Input:
 *      port   - port id
 *      ingress_en  - ingress enable
 *      egress_en  -  egress enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec enable.
 */
rtk_api_ret_t dal_rtl8373_macsec_enable_set(rtk_uint32 port, rtk_uint32 ingress_en, rtk_uint32 egress_en)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_PM_CTRL_PORT4_ADDR, RTL8373_MACSEC_PM_CTRL_PORT4_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_PM_CTRL_PORT4_ADDR, RTL8373_MACSEC_PM_CTRL_PORT4_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_PM_CTRL_PORT5_ADDR, RTL8373_MACSEC_PM_CTRL_PORT5_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_PM_CTRL_PORT5_ADDR, RTL8373_MACSEC_PM_CTRL_PORT5_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_PM_CTRL_PORT6_ADDR, RTL8373_MACSEC_PM_CTRL_PORT6_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_PM_CTRL_PORT6_ADDR, RTL8373_MACSEC_PM_CTRL_PORT6_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_PM_CTRL_PORT7_ADDR, RTL8373_MACSEC_PM_CTRL_PORT7_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_PM_CTRL_PORT7_ADDR, RTL8373_MACSEC_PM_CTRL_PORT7_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
                return retVal;
    }

    else if(port == 4)
       {
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT4_ADDR, RTL8373_MACSEC_PM_CTRL_PORT4_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT4_ADDR, RTL8373_MACSEC_PM_CTRL_PORT4_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
        		return retVal;
       }
	else if(port == 5)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT5_ADDR, RTL8373_MACSEC_PM_CTRL_PORT5_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT5_ADDR, RTL8373_MACSEC_PM_CTRL_PORT5_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
        		return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT6_ADDR, RTL8373_MACSEC_PM_CTRL_PORT6_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT6_ADDR, RTL8373_MACSEC_PM_CTRL_PORT6_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
        		return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT7_ADDR, RTL8373_MACSEC_PM_CTRL_PORT7_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT7_ADDR, RTL8373_MACSEC_PM_CTRL_PORT7_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
        		return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_enable_get
 * Description:
 *      get macsec enable status.
 * Input:
 *      port   - port id
 * Output:
 *      ingress_en  - ingress enable
 *      egress_en  -  egress enable
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec enable status.
 */
rtk_api_ret_t dal_rtl8373_macsec_enable_get(rtk_uint32 port, rtk_uint32 *ingress_en, rtk_uint32 *egress_en)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_PM_CTRL_PORT4_ADDR, RTL8373_MACSEC_PM_CTRL_PORT4_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_PM_CTRL_PORT4_ADDR, RTL8373_MACSEC_PM_CTRL_PORT4_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_PM_CTRL_PORT5_ADDR, RTL8373_MACSEC_PM_CTRL_PORT5_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_PM_CTRL_PORT5_ADDR, RTL8373_MACSEC_PM_CTRL_PORT5_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_PM_CTRL_PORT6_ADDR, RTL8373_MACSEC_PM_CTRL_PORT6_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_PM_CTRL_PORT6_ADDR, RTL8373_MACSEC_PM_CTRL_PORT6_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_PM_CTRL_PORT7_ADDR, RTL8373_MACSEC_PM_CTRL_PORT7_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_PM_CTRL_PORT7_ADDR, RTL8373_MACSEC_PM_CTRL_PORT7_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 4)
       {
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT4_ADDR, RTL8373_MACSEC_PM_CTRL_PORT4_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT4_ADDR, RTL8373_MACSEC_PM_CTRL_PORT4_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
        		return retVal;
       }
	else if(port == 5)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT5_ADDR, RTL8373_MACSEC_PM_CTRL_PORT5_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT5_ADDR, RTL8373_MACSEC_PM_CTRL_PORT5_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
        		return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT6_ADDR, RTL8373_MACSEC_PM_CTRL_PORT6_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT6_ADDR, RTL8373_MACSEC_PM_CTRL_PORT6_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
        		return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT7_ADDR, RTL8373_MACSEC_PM_CTRL_PORT7_MACSEC_TX_ICG_EN_OFFSET, egress_en)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_PM_CTRL_PORT7_ADDR, RTL8373_MACSEC_PM_CTRL_PORT7_MACSEC_RX_ICG_EN_OFFSET, ingress_en)) != RT_ERR_OK)
        		return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_reset
 * Description:
 *      reset macsec ip.
 * Input:
 *      port   - port id
 *      ingress_rst  - ingress reset
 *      egress_rst  -  egress reset
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will reset macsec.
 */
rtk_api_ret_t dal_rtl8373_macsec_reset(rtk_uint32 port, rtk_uint32 ingress_rst, rtk_uint32 egress_rst)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_IPRST_N_OFFSET, egress_rst)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_IPRST_N_OFFSET, ingress_rst)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_IPRST_N_OFFSET, egress_rst)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_IPRST_N_OFFSET, ingress_rst)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_IPRST_N_OFFSET, egress_rst)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_IPRST_N_OFFSET, ingress_rst)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_IPRST_N_OFFSET, egress_rst)) != RT_ERR_OK)
                return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_IPRST_N_OFFSET, ingress_rst)) != RT_ERR_OK)
                return retVal;
    }
    else if(port == 4)
       {
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_IPRST_N_OFFSET, egress_rst)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_IPRST_N_OFFSET, ingress_rst)) != RT_ERR_OK)
        		return retVal;
       }
	else if(port == 5)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_IPRST_N_OFFSET, egress_rst)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_IPRST_N_OFFSET, ingress_rst)) != RT_ERR_OK)
        		return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_IPRST_N_OFFSET, egress_rst)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_IPRST_N_OFFSET, ingress_rst)) != RT_ERR_OK)
        		return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_IPRST_N_OFFSET, egress_rst)) != RT_ERR_OK)
        		return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_IPRST_N_OFFSET, ingress_rst)) != RT_ERR_OK)
        		return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8373_macsec_egress_set
 * Description:
 *      Configure macsec egress rule.
 * Input:
 *      port  - port id
 *      addr - macsec ip core register address
 *      value - data for rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec egress rule:SA match rule, flow control register and transform record.
 */
rtk_api_ret_t dal_rtl8373_macsec_egress_set(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 value)
{
	rtk_api_ret_t retVal;
	rtk_uint32 phy_data;

	 if(port == 0)
    {
#if 1 //pending
        if ((retVal = dal_rtl8373_phy_read(0, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
#endif
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AE_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT4_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
                    return retVal;
        
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDH_AE_PORT4_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT4_REG_DATA_AE_L_MASK, value & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDH_AE_PORT4_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT4_REG_DATA_AE_H_MASK, (value >> 16) & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_CMD_AE_PORT4_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT4_REG_WR_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 1)
    {
#if 1 //pending    
        if ((retVal = dal_rtl8373_phy_read(1, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
#endif
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDH_AE_PORT5_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT5_REG_DATA_AE_L_MASK, value & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDH_AE_PORT5_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT5_REG_DATA_AE_H_MASK, (value >> 16) & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AE_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT5_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_CMD_AE_PORT5_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT5_REG_WR_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8373_phy_read(2, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
        
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDH_AE_PORT6_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT6_REG_DATA_AE_L_MASK, value & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDH_AE_PORT6_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT6_REG_DATA_AE_H_MASK, (value >> 16) & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AE_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT6_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_CMD_AE_PORT6_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT6_REG_WR_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8373_phy_read(3, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
        
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDH_AE_PORT7_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT7_REG_DATA_AE_L_MASK, value & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDH_AE_PORT7_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT7_REG_DATA_AE_H_MASK, (value >> 16) & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AE_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT7_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_CMD_AE_PORT7_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT7_REG_WR_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
    }
	else if(port == 4)
       {
       	if ((retVal = dal_rtl8373_phy_read(4, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AE_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT4_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
        			return retVal;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDH_AE_PORT4_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT4_REG_DATA_AE_L_MASK, value & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDH_AE_PORT4_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT4_REG_DATA_AE_H_MASK, (value >> 16) & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_CMD_AE_PORT4_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT4_REG_WR_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
       }
	else if(port == 5)
	{
		if ((retVal = dal_rtl8373_phy_read(5, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDH_AE_PORT5_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT5_REG_DATA_AE_L_MASK, value & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDH_AE_PORT5_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT5_REG_DATA_AE_H_MASK, (value >> 16) & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AE_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT5_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_CMD_AE_PORT5_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT5_REG_WR_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = dal_rtl8373_phy_read(6, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDH_AE_PORT6_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT6_REG_DATA_AE_L_MASK, value & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDH_AE_PORT6_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT6_REG_DATA_AE_H_MASK, (value >> 16) & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AE_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT6_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_CMD_AE_PORT6_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT6_REG_WR_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = dal_rtl8373_phy_read(7, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDH_AE_PORT7_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT7_REG_DATA_AE_L_MASK, value & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDH_AE_PORT7_ADDR, RTL8373_MACSEC_REG_RWDH_AE_PORT7_REG_DATA_AE_H_MASK, (value >> 16) & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AE_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT7_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_CMD_AE_PORT7_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT7_REG_WR_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_egress_get
 * Description:
 *      get macsec egress rule.
 * Input:
 *      port  -  port id
 *      addr -  macsec ip core register address
 * Output:
 *      value  - data for rule
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec egress rule:SA match rule, flow control register and transform record.
 */
rtk_api_ret_t dal_rtl8373_macsec_egress_get(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 *value)
{
	rtk_api_ret_t retVal;
	rtk_uint32 phy_data,tmp;

	if(port == 0)
    {
#if 1 //pending

           if ((retVal = dal_rtl8373_phy_read(0, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
#endif  

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AE_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT4_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_CMD_AE_PORT4_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT4_REG_RD_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_RWDH_AE_PORT4_ADDR, &tmp)) != RT_ERR_OK)
                    return retVal;
        //rtlglue_printf("4\n");
        *value = ((tmp >> 16) & 0xffff) | ((tmp & 0xffff) << 16);
    }
    else if(port == 1)
    {
#if 1 //pending

        if ((retVal = dal_rtl8373_phy_read(1, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
#endif
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AE_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT5_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
                    return retVal;
        
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_CMD_AE_PORT5_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT5_REG_RD_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_RWDH_AE_PORT5_ADDR, &tmp)) != RT_ERR_OK)
                    return retVal;

        *value = ((tmp >> 16) & 0xffff) | ((tmp & 0xffff) << 16);
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8373_phy_read(2, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
        
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AE_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT6_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
                    return retVal;
        
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_CMD_AE_PORT6_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT6_REG_RD_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_RWDH_AE_PORT6_ADDR, &tmp)) != RT_ERR_OK)
                    return retVal;

        *value = ((tmp >> 16) & 0xffff) | ((tmp & 0xffff) << 16);
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8373_phy_read(3, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
        
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AE_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT7_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_CMD_AE_PORT7_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT7_REG_RD_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_RWDH_AE_PORT7_ADDR, &tmp)) != RT_ERR_OK)
                    return retVal;

        *value = ((tmp >> 16) & 0xffff) | ((tmp & 0xffff) << 16);
    }
	else if(port == 4)
       {
       	if ((retVal = dal_rtl8373_phy_read(4, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AE_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT4_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_CMD_AE_PORT4_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT4_REG_RD_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_RWDH_AE_PORT4_ADDR, &tmp)) != RT_ERR_OK)
        			return retVal;
		
		*value = ((tmp >> 16) & 0xffff) | ((tmp & 0xffff) << 16);
       }
	else if(port == 5)
	{
		if ((retVal = dal_rtl8373_phy_read(5, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AE_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT5_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
        			return retVal;
		
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_CMD_AE_PORT5_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT5_REG_RD_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_RWDH_AE_PORT5_ADDR, &tmp)) != RT_ERR_OK)
        			return retVal;

		*value = ((tmp >> 16) & 0xffff) | ((tmp & 0xffff) << 16);
	}
	else if(port == 6)
	{
		if ((retVal = dal_rtl8373_phy_read(6, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AE_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT6_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
        			return retVal;
		
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_CMD_AE_PORT6_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT6_REG_RD_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_RWDH_AE_PORT6_ADDR, &tmp)) != RT_ERR_OK)
        			return retVal;

		*value = ((tmp >> 16) & 0xffff) | ((tmp & 0xffff) << 16);
	}
	else if(port == 7)
	{
		if ((retVal = dal_rtl8373_phy_read(7, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AE_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AE_PORT7_REG_ADDR_AE_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_CMD_AE_PORT7_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT7_REG_RD_REQ_AE_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_RWDH_AE_PORT7_ADDR, &tmp)) != RT_ERR_OK)
        			return retVal;

		*value = ((tmp >> 16) & 0xffff) | ((tmp & 0xffff) << 16);
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_ingress_set
 * Description:
 *      Configure macsec ingress rule.
 * Input:
 *      port  - port id
 *      addr - macsec ip core register address
 *      value - data for rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec ingress rule:SA match rule, flow control register and transform record.
 */
rtk_api_ret_t dal_rtl8373_macsec_ingress_set(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 value)
{
	rtk_api_ret_t retVal;
	rtk_uint32 phy_data;

	if(port == 0)
    {
#if 1 //pending

        if ((retVal = dal_rtl8373_phy_read(0, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
        
#endif
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_CMD_AE_PORT4_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT4_REG_DATA_AI_H_MASK, (value >> 16) &0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDL_AI_PORT4_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT4_REG_DATA_AI_L_MASK, value & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AI_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT4_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_ADDR_AI_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT4_REG_WR_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 1)
    {
#if 1 //pending

        if ((retVal = dal_rtl8373_phy_read(1, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
#endif 
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_CMD_AE_PORT5_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT5_REG_DATA_AI_H_MASK, (value >> 16) &0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDL_AI_PORT5_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT5_REG_DATA_AI_L_MASK, value & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AI_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT5_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_ADDR_AI_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT5_REG_WR_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8373_phy_read(2, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
        
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_CMD_AE_PORT6_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT6_REG_DATA_AI_H_MASK, (value >> 16) &0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDL_AI_PORT6_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT6_REG_DATA_AI_L_MASK, value & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AI_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT6_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_ADDR_AI_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT6_REG_WR_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8373_phy_read(3, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
        
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_CMD_AE_PORT7_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT7_REG_DATA_AI_H_MASK, (value >> 16) &0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_RWDL_AI_PORT7_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT7_REG_DATA_AI_L_MASK, value & 0xffff)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AI_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT7_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_ADDR_AI_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT7_REG_WR_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
    }
	else if(port == 4)
    {
       	if ((retVal = dal_rtl8373_phy_read(4, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_CMD_AE_PORT4_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT4_REG_DATA_AI_H_MASK, (value >> 16) &0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDL_AI_PORT4_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT4_REG_DATA_AI_L_MASK, value & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AI_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT4_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_ADDR_AI_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT4_REG_WR_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
    }
	else if(port == 5)
	{
		if ((retVal = dal_rtl8373_phy_read(5, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_CMD_AE_PORT5_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT5_REG_DATA_AI_H_MASK, (value >> 16) &0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDL_AI_PORT5_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT5_REG_DATA_AI_L_MASK, value & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AI_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT5_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_ADDR_AI_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT5_REG_WR_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = dal_rtl8373_phy_read(6, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_CMD_AE_PORT6_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT6_REG_DATA_AI_H_MASK, (value >> 16) &0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDL_AI_PORT6_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT6_REG_DATA_AI_L_MASK, value & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AI_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT6_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_ADDR_AI_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT6_REG_WR_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = dal_rtl8373_phy_read(7, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_CMD_AE_PORT7_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT7_REG_DATA_AI_H_MASK, (value >> 16) &0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_RWDL_AI_PORT7_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT7_REG_DATA_AI_L_MASK, value & 0xffff)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AI_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT7_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_ADDR_AI_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT7_REG_WR_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_ingress_get
 * Description:
 *      get macsec egress rule.
 * Input:
 *      port  -  port id
 *      addr -  macsec ip core register address
 * Output:
 *      value  - data for rule
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec ingress rule:SA match rule, flow control register and transform record.
 */
rtk_api_ret_t dal_rtl8373_macsec_ingress_get(rtk_uint32 port, rtk_uint32 addr, rtk_uint32 *value)
{
	rtk_api_ret_t retVal;
       rtk_uint32 phy_data;
	rtk_uint32 value_H,value_L;

	if(port == 0)
    {
#if 1 //pending

        if ((retVal = dal_rtl8373_phy_read(0, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
#endif
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AI_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT4_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_ADDR_AI_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT4_REG_RD_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_read(RTL8373_MACSEC_REG_CMD_AE_PORT4_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT4_REG_DATA_AI_H_MASK, &value_H)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_read(RTL8373_MACSEC_REG_RWDL_AI_PORT4_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT4_REG_DATA_AI_L_MASK, &value_L)) != RT_ERR_OK)
                    return retVal;

        *value = (value_H << 16) | (value_L & 0xffff);

    }
    else if(port == 1)
    {
#if 1 //pending

        if ((retVal = dal_rtl8373_phy_read(1, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
#endif
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AI_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT5_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_ADDR_AI_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT5_REG_RD_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_read(RTL8373_MACSEC_REG_CMD_AE_PORT5_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT5_REG_DATA_AI_H_MASK, &value_H)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_read(RTL8373_MACSEC_REG_RWDL_AI_PORT5_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT5_REG_DATA_AI_L_MASK, &value_L)) != RT_ERR_OK)
                    return retVal;

        *value = (value_H << 16) | (value_L & 0xffff);
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8373_phy_read(2, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
        
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AI_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT6_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_ADDR_AI_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT6_REG_RD_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_read(RTL8373_MACSEC_REG_CMD_AE_PORT6_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT6_REG_DATA_AI_H_MASK, &value_H)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_read(RTL8373_MACSEC_REG_RWDL_AI_PORT6_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT6_REG_DATA_AI_L_MASK, &value_L)) != RT_ERR_OK)
                    return retVal;

        *value = (value_H << 16) | (value_L & 0xffff);
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8373_phy_read(3, 7, 1, &phy_data)) != RT_ERR_OK)
                      return retVal;

        if(((phy_data >> 2) & 0x1) != 1)
                return RT_ERR_PHY_LINK_DOWN;
        
        if ((retVal = dal_rtl8224_top_regbits_write(RTL8373_MACSEC_REG_ADDR_AI_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT7_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_ADDR_AI_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT7_REG_RD_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_read(RTL8373_MACSEC_REG_CMD_AE_PORT7_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT7_REG_DATA_AI_H_MASK, &value_H)) != RT_ERR_OK)
                    return retVal;

        if ((retVal = dal_rtl8224_top_regbits_read(RTL8373_MACSEC_REG_RWDL_AI_PORT7_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT7_REG_DATA_AI_L_MASK, &value_L)) != RT_ERR_OK)
                    return retVal;

        *value = (value_H << 16) | (value_L & 0xffff);
    }
	else if(port == 4)
       {
       	if ((retVal = dal_rtl8373_phy_read(4, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AI_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT4_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_ADDR_AI_PORT4_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT4_REG_RD_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicRegBits(RTL8373_MACSEC_REG_CMD_AE_PORT4_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT4_REG_DATA_AI_H_MASK, &value_H)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicRegBits(RTL8373_MACSEC_REG_RWDL_AI_PORT4_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT4_REG_DATA_AI_L_MASK, &value_L)) != RT_ERR_OK)
        			return retVal;

		*value = (value_H << 16) | (value_L & 0xffff);

       }
	else if(port == 5)
	{
		if ((retVal = dal_rtl8373_phy_read(5, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AI_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT5_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_ADDR_AI_PORT5_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT5_REG_RD_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicRegBits(RTL8373_MACSEC_REG_CMD_AE_PORT5_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT5_REG_DATA_AI_H_MASK, &value_H)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicRegBits(RTL8373_MACSEC_REG_RWDL_AI_PORT5_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT5_REG_DATA_AI_L_MASK, &value_L)) != RT_ERR_OK)
        			return retVal;

		*value = (value_H << 16) | (value_L & 0xffff);
	}
	else if(port == 6)
	{
		if ((retVal = dal_rtl8373_phy_read(6, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AI_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT6_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_ADDR_AI_PORT6_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT6_REG_RD_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicRegBits(RTL8373_MACSEC_REG_CMD_AE_PORT6_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT6_REG_DATA_AI_H_MASK, &value_H)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicRegBits(RTL8373_MACSEC_REG_RWDL_AI_PORT6_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT6_REG_DATA_AI_L_MASK, &value_L)) != RT_ERR_OK)
        			return retVal;

		*value = (value_H << 16) | (value_L & 0xffff);
	}
	else if(port == 7)
	{
		if ((retVal = dal_rtl8373_phy_read(7, 7, 1, &phy_data)) != RT_ERR_OK)
		      		return retVal;

		if(((phy_data >> 2) & 0x1) != 1)
				return RT_ERR_PHY_LINK_DOWN;
		
		if ((retVal = rtl8373_setAsicRegBits(RTL8373_MACSEC_REG_ADDR_AI_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT7_REG_ADDR_AI_MASK, addr)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_ADDR_AI_PORT7_ADDR, RTL8373_MACSEC_REG_ADDR_AI_PORT7_REG_RD_REQ_AI_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicRegBits(RTL8373_MACSEC_REG_CMD_AE_PORT7_ADDR, RTL8373_MACSEC_REG_CMD_AE_PORT7_REG_DATA_AI_H_MASK, &value_H)) != RT_ERR_OK)
        			return retVal;

		if ((retVal = rtl8373_getAsicRegBits(RTL8373_MACSEC_REG_RWDL_AI_PORT7_ADDR, RTL8373_MACSEC_REG_RWDL_AI_PORT7_REG_DATA_AI_L_MASK, &value_L)) != RT_ERR_OK)
        			return retVal;

		*value = (value_H << 16) | (value_L & 0xffff);
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_rxgating_set
 * Description:
 *      Configure macsec rx gating value
 * Input:
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec rx gating value,before set the packet flow path called this API.
 */
rtk_api_ret_t dal_rtl8373_macsec_rxgating_set(rtk_uint32 port)
{
	rtk_api_ret_t retVal;
	rtk_uint32 tmp_value,tmp_gvalue;
	rtk_uint32 pollcnt;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_GATING_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
        
              for(pollcnt = 0; pollcnt < 0xffff; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;

                if((tmp_value == 1) && (tmp_gvalue == 1))
                    break;
              }
        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_GATING_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;

                if((tmp_value == 1) && (tmp_gvalue == 1))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_GATING_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;

                if((tmp_value == 1) && (tmp_gvalue == 1))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_GATING_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;

                if((tmp_value == 1) && (tmp_gvalue == 1))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
	else if(port == 4)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_GATING_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;

				if((tmp_value == 1) && (tmp_gvalue == 1))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_GATING_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;

				if((tmp_value == 1) && (tmp_gvalue == 1))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_GATING_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;

				if((tmp_value == 1) && (tmp_gvalue == 1))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_GATING_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;

				if((tmp_value == 1) && (tmp_gvalue == 1))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_rxgating_cancel
 * Description:
 *      Configure macsec rx gating value
 * Input:
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec rx gating value, called after packet flow path changed.
 */
rtk_api_ret_t dal_rtl8373_macsec_rxgating_cancel(rtk_uint32 port)
{
	rtk_api_ret_t retVal;
	rtk_uint32 tmp_value,tmp_gvalue;
	rtk_uint32 pollcnt;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_GATING_OFFSET, 0)) != RT_ERR_OK)
                    return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;

                if((tmp_value == 0) && (tmp_gvalue == 0))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_GATING_OFFSET, 0)) != RT_ERR_OK)
                    return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;

                if((tmp_value == 0) && (tmp_gvalue == 0))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_GATING_OFFSET, 0)) != RT_ERR_OK)
                    return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;

                if((tmp_value == 0) && (tmp_gvalue == 0))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_GATING_OFFSET, 0)) != RT_ERR_OK)
                    return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;

                if((tmp_value == 0) && (tmp_gvalue == 0))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
	else if(port == 4)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_GATING_OFFSET, 0)) != RT_ERR_OK)
        			return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;

				if((tmp_value == 0) && (tmp_gvalue == 0))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_GATING_OFFSET, 0)) != RT_ERR_OK)
        			return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;

				if((tmp_value == 0) && (tmp_gvalue == 0))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_GATING_OFFSET, 0)) != RT_ERR_OK)
        			return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;

				if((tmp_value == 0) && (tmp_gvalue == 0))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_GATING_OFFSET, 0)) != RT_ERR_OK)
        			return retVal;
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_RX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_RXDV_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;

				if((tmp_value == 0) && (tmp_gvalue == 0))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_txgating_set
 * Description:
 *      Configure macsec tx gating value
 * Input:
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec tx gating value, before set the packet flow path called this API.
 */
rtk_api_ret_t dal_rtl8373_macsec_txgating_set(rtk_uint32 port)
{
	rtk_api_ret_t retVal;
	rtk_uint32 tmp_value,tmp_gvalue;
	rtk_uint32 pollcnt;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_GATING_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
        
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;
                
                       if((tmp_value == 1) && (tmp_gvalue == 1))
                    break;
              }
        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_GATING_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
        
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;
                
                       if((tmp_value == 1) && (tmp_gvalue == 1))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_GATING_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
        
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;
                
                       if((tmp_value == 1) && (tmp_gvalue == 1))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_GATING_OFFSET, 1)) != RT_ERR_OK)
                    return retVal;
        
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;
                
                       if((tmp_value == 1) && (tmp_gvalue == 1))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
	else if(port == 4)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_GATING_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
		
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;
				
               		if((tmp_value == 1) && (tmp_gvalue == 1))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_GATING_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
		
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;
				
               		if((tmp_value == 1) && (tmp_gvalue == 1))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_GATING_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
		
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;
				
               		if((tmp_value == 1) && (tmp_gvalue == 1))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_GATING_OFFSET, 1)) != RT_ERR_OK)
        			return retVal;
		
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;
				
               		if((tmp_value == 1) && (tmp_gvalue == 1))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_txgating_cancel
 * Description:
 *      Configure macsec tx gating value
 * Input:
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec tx gating value, called after packet flow path changed..
 */
rtk_api_ret_t dal_rtl8373_macsec_txgating_cancel(rtk_uint32 port)
{
	rtk_api_ret_t retVal;
	rtk_uint32 tmp_value,tmp_gvalue;
	rtk_uint32 pollcnt;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_GATING_OFFSET, 0)) != RT_ERR_OK)
                    return retVal;
        
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;
                
                       if((tmp_value == 0) && (tmp_gvalue == 0))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_GATING_OFFSET, 0)) != RT_ERR_OK)
                    return retVal;
              
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;
                
                       if((tmp_value == 0) && (tmp_gvalue == 0))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_GATING_OFFSET, 0)) != RT_ERR_OK)
                    return retVal;
              
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;
                
                       if((tmp_value == 0) && (tmp_gvalue == 0))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_GATING_OFFSET, 0)) != RT_ERR_OK)
                    return retVal;
              
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
                        return retVal;

                if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
                        return retVal;
                
                       if((tmp_value == 0) && (tmp_gvalue == 0))
                    break;
              }

        if(pollcnt == RTL8373_MACSEC_POLLCNT)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
	else if(port == 4)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_GATING_OFFSET, 0)) != RT_ERR_OK)
        			return retVal;
		
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT4_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;
				
               		if((tmp_value == 0) && (tmp_gvalue == 0))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_GATING_OFFSET, 0)) != RT_ERR_OK)
        			return retVal;
              
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT5_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;
				
               		if((tmp_value == 0) && (tmp_gvalue == 0))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_GATING_OFFSET, 0)) != RT_ERR_OK)
        			return retVal;
              
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT6_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;
				
               		if((tmp_value == 0) && (tmp_gvalue == 0))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_GATING_OFFSET, 0)) != RT_ERR_OK)
        			return retVal;
              
              for(pollcnt = 0; pollcnt < RTL8373_MACSEC_POLLCNT; pollcnt++)
              {
				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_TX_XGMASK_OFFSET, &tmp_value)) != RT_ERR_OK)
        				return retVal;

				if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_MASK_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_MASK_PORT7_TXEN_GMASK_OFFSET, &tmp_gvalue)) != RT_ERR_OK)
        				return retVal;
				
               		if((tmp_value == 0) && (tmp_gvalue == 0))
					break;
              }

		if(pollcnt == RTL8373_MACSEC_POLLCNT)
			return RT_ERR_BUSYWAIT_TIMEOUT;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_rxIPbypass_set
 * Description:
 *      Configure macsec bypass in MACsec IP function.
 * Input:
 *      port  - port id
 *      enable  - enable ip bypass
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec bypass in MACsec IP function.
 */
rtk_api_ret_t dal_rtl8373_macsec_rxIPbypass_set(rtk_uint32 port, rtk_uint32 enable)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        dal_rtl8373_macsec_rxgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_rxgating_cancel(port);
    }
    else if(port == 1)
    {
        dal_rtl8373_macsec_rxgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_rxgating_cancel(port);
    }
    else if(port == 2)
    {
        dal_rtl8373_macsec_rxgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_rxgating_cancel(port);
    }
    else if(port == 3)
    {
        dal_rtl8373_macsec_rxgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_rxgating_cancel(port);
    }
    else if(port == 4)
	{
		dal_rtl8373_macsec_rxgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_rxgating_cancel(port);
	}
	else if(port == 5)
	{
		dal_rtl8373_macsec_rxgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_rxgating_cancel(port);
	}
	else if(port == 6)
	{
		dal_rtl8373_macsec_rxgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_rxgating_cancel(port);
	}
	else if(port == 7)
	{
		dal_rtl8373_macsec_rxgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_rxgating_cancel(port);
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_rxIPbypass_get
 * Description:
 *      get macsec bypass in MACsec IP function status.
 * Input:
 *      port  - port id
 * Output:
 *      enable  - enable ip bypass
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec bypass in MACsec IP function status.
 */
rtk_api_ret_t dal_rtl8373_macsec_rxIPbypass_get(rtk_uint32 port, rtk_uint32 *enable)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 4)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_txIPbypass_set
 * Description:
 *      Configure macsec bypass in MACsec IP function.
 * Input:
 *      port  - port id
 *      enable  - enable ip bypass
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec bypass in MACsec IP function.
 */
rtk_api_ret_t dal_rtl8373_macsec_txIPbypass_set(rtk_uint32 port, rtk_uint32 enable)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        dal_rtl8373_macsec_txgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_txgating_cancel(port);
    }
    else if(port == 1)
    {
        dal_rtl8373_macsec_txgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_txgating_cancel(port);
    }
    else if(port == 2)
    {
        dal_rtl8373_macsec_txgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_txgating_cancel(port);
    }
    else if(port == 3)
    {
        dal_rtl8373_macsec_txgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_txgating_cancel(port);
    }
    else if(port == 4)
	{
		dal_rtl8373_macsec_txgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_txgating_cancel(port);
	}
	else if(port == 5)
	{
		dal_rtl8373_macsec_txgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_txgating_cancel(port);
	}
	else if(port == 6)
	{
		dal_rtl8373_macsec_txgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_txgating_cancel(port);
	}
	else if(port == 7)
	{
		dal_rtl8373_macsec_txgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_txgating_cancel(port);
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_txIPbypass_get
 * Description:
 *      get macsec bypass MACsec IP function status.
 * Input:
 *      port  - port id
 * Output:
 *      enable  - enable ip bypass
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec bypass MACsec IP function status.
 */
rtk_api_ret_t dal_rtl8373_macsec_txIPbypass_get(rtk_uint32 port, rtk_uint32 *enable)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 4)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_IPBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_rxIPbypass_set
 * Description:
 *      Configure macsec bypass MACsec IP function.
 * Input:
 *      port  - port id
 *      enable  - enable ip bypass
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec bypass MACsec IP function.
 */
rtk_api_ret_t dal_rtl8373_macsec_rxbypass_set(rtk_uint32 port, rtk_uint32 enable)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        dal_rtl8373_macsec_rxgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_rxgating_cancel(port);
    }
    else if(port == 1)
    {
        dal_rtl8373_macsec_rxgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_rxgating_cancel(port);
    }
    else if(port == 2)
    {
        dal_rtl8373_macsec_rxgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_rxgating_cancel(port);
    }
    else if(port == 3)
    {
        dal_rtl8373_macsec_rxgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_rxgating_cancel(port);
    }
    else if(port == 4)
	{
		dal_rtl8373_macsec_rxgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_rxgating_cancel(port);
	}
	else if(port == 5)
	{
		dal_rtl8373_macsec_rxgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_rxgating_cancel(port);
	}
	else if(port == 6)
	{
		dal_rtl8373_macsec_rxgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_rxgating_cancel(port);
	}
	else if(port == 7)
	{
		dal_rtl8373_macsec_rxgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_rxgating_cancel(port);
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_rxbypass_get
 * Description:
 *      get macsec bypass MACsec IP function status.
 * Input:
 *      port  - port id
 * Output:
 *      enable  - enable ip bypass
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec bypass  MACsec IP function status.
 */
rtk_api_ret_t dal_rtl8373_macsec_rxbypass_get(rtk_uint32 port, rtk_uint32 *enable)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 4)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_txbypass_set
 * Description:
 *      Configure macsec bypass MACsec IP function.
 * Input:
 *      port  - port id
 *      enable  - enable ip bypass
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure macsec bypass MACsec IP function.
 */
rtk_api_ret_t dal_rtl8373_macsec_txbypass_set(rtk_uint32 port, rtk_uint32 enable)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        dal_rtl8373_macsec_txgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_txgating_cancel(port);
    }
    else if(port == 1)
    {
        dal_rtl8373_macsec_txgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_txgating_cancel(port);
    }
    else if(port == 2)
    {
        dal_rtl8373_macsec_txgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_txgating_cancel(port);
    }
    else if(port == 3)
    {
        dal_rtl8373_macsec_txgating_set(port);
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
        dal_rtl8373_macsec_txgating_cancel(port);
    }
    else if(port == 4)
	{
		dal_rtl8373_macsec_txgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_txgating_cancel(port);
	}
	else if(port == 5)
	{
		dal_rtl8373_macsec_txgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_txgating_cancel(port);
	}
	else if(port == 6)
	{
		dal_rtl8373_macsec_txgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_txgating_cancel(port);
	}
	else if(port == 7)
	{
		dal_rtl8373_macsec_txgating_set(port);
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
		dal_rtl8373_macsec_txgating_cancel(port);
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_txbypass_get
 * Description:
 *      get macsec bypass MACsec IP function status.
 * Input:
 *      port  - port id
 * Output:
 *      enable  - enable ip bypass
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get macsec bypass MACsec IP function status.
 */
rtk_api_ret_t dal_rtl8373_macsec_txbypass_get(rtk_uint32 port, rtk_uint32 *enable)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_read(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 4)
    {
        if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 4)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_getAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_MACSECBYPASS_EN_OFFSET, enable)) != RT_ERR_OK)
        			return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_wrapper_int_control_set
 * Description:
 *      Configure MACsec interrupt.
 * Input:
 *      port  -  port id
 *      type  -  interrupt type
 *      enable - enable interrupt
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure MACsec interrupt.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_TX_IPE_GLB
 *      - INT_TYPE_TX_IPESECFAIL
 *      - INT_TYPE_TX_IPELOCK
 *      - INT_TYPE_TX_IPELOCK_XG
 *      - INT_TYPE_RX_IPI_GLB
 *      - INT_TYPE_RX_IPISECFAIL
 *      - INT_TYPE_RX_IPILOCK
 *      - INT_TYPE_RX_IPILOCK_XG
 */
rtk_api_ret_t dal_rtl8373_wrapper_int_control_set(rtk_uint32 port, rtk_macsec_int_type_t type, rtk_enable_t enable)
{
	rtk_api_ret_t retVal;
	rtk_uint32 tmp_value;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_IMR_PORT4_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;

        if(enable == ENABLED)
        {
            tmp_value |=(1 << type);
            
            if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_IMR_PORT4_ADDR,  tmp_value)) != RT_ERR_OK)
                    return retVal;
        }
        else if(enable == DISABLED)
        {
            tmp_value &= ~(1 << type);
            
            if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_IMR_PORT4_ADDR,  tmp_value)) != RT_ERR_OK)
                    return retVal;
        }         
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_IMR_PORT5_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;

        if(enable == ENABLED)
        {
            tmp_value |=(1 << type);
            
            if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_IMR_PORT5_ADDR,  tmp_value)) != RT_ERR_OK)
                    return retVal;
        }
        else if(enable == DISABLED)
        {
            tmp_value &= ~(1 << type);
            
            if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_IMR_PORT5_ADDR,  tmp_value)) != RT_ERR_OK)
                    return retVal;
        }         
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_IMR_PORT6_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;

        if(enable == ENABLED)
        {
            tmp_value |=(1 << type);
            
            if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_IMR_PORT6_ADDR,  tmp_value)) != RT_ERR_OK)
                    return retVal;
        }
        else if(enable == DISABLED)
        {
            tmp_value &= ~(1 << type);
            
            if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_IMR_PORT6_ADDR,  tmp_value)) != RT_ERR_OK)
                    return retVal;
        }         
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_IMR_PORT7_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;

        if(enable == ENABLED)
        {
            tmp_value |=(1 << type);
            
            if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_IMR_PORT7_ADDR,  tmp_value)) != RT_ERR_OK)
                    return retVal;
        }
        else if(enable == DISABLED)
        {
            tmp_value &= ~(1 << type);
            
            if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_IMR_PORT7_ADDR,  tmp_value)) != RT_ERR_OK)
                    return retVal;
        }         
    }
    else if(port == 4)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT4_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;

		if(enable == ENABLED)
		{
			tmp_value |=(1 << type);
			
			if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT4_ADDR,  tmp_value)) != RT_ERR_OK)
        			return retVal;
		}
		else if(enable == DISABLED)
		{
			tmp_value &= ~(1 << type);
			
			if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT4_ADDR,  tmp_value)) != RT_ERR_OK)
        			return retVal;
		}		 
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT5_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;

		if(enable == ENABLED)
		{
			tmp_value |=(1 << type);
			
			if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT5_ADDR,  tmp_value)) != RT_ERR_OK)
        			return retVal;
		}
		else if(enable == DISABLED)
		{
			tmp_value &= ~(1 << type);
			
			if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT5_ADDR,  tmp_value)) != RT_ERR_OK)
        			return retVal;
		}		 
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT6_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;

		if(enable == ENABLED)
		{
			tmp_value |=(1 << type);
			
			if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT6_ADDR,  tmp_value)) != RT_ERR_OK)
        			return retVal;
		}
		else if(enable == DISABLED)
		{
			tmp_value &= ~(1 << type);
			
			if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT6_ADDR,  tmp_value)) != RT_ERR_OK)
        			return retVal;
		}		 
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT7_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;

		if(enable == ENABLED)
		{
			tmp_value |=(1 << type);
			
			if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT7_ADDR,  tmp_value)) != RT_ERR_OK)
        			return retVal;
		}
		else if(enable == DISABLED)
		{
			tmp_value &= ~(1 << type);
			
			if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT7_ADDR,  tmp_value)) != RT_ERR_OK)
        			return retVal;
		}		 
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_wrapper_int_control_get
 * Description:
 *      gonfigure MACsec interrupt.
 * Input:
 *      port  -  port id
 *      type  -  interrupt type
 * Output:
 *      pEnable - enable interrupt
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get MACsec interrupt.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_TX_IPE_GLB
 *      - INT_TYPE_TX_IPESECFAIL
 *      - INT_TYPE_TX_IPELOCK
 *      - INT_TYPE_TX_IPELOCK_XG
 *      - INT_TYPE_RX_IPI_GLB
 *      - INT_TYPE_RX_IPISECFAIL
 *      - INT_TYPE_RX_IPILOCK
 *      - INT_TYPE_RX_IPILOCK_XG
 */
rtk_api_ret_t dal_rtl8373_wrapper_int_control_get(rtk_uint32 port, rtk_macsec_int_type_t type, rtk_enable_t *pEnable)
{
	rtk_api_ret_t retVal;
	rtk_uint32 tmp_value;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_IMR_PORT4_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;

        if(0 == (tmp_value & (1 << type)))
        {
            *pEnable = DISABLED;
        }
        else if(1 == (tmp_value & (1 << type)))
        {
            *pEnable = ENABLED;
        }         
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_IMR_PORT5_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;

        if(0 == (tmp_value & (1 << type)))
        {
            *pEnable = DISABLED;
        }
        else if(1 == (tmp_value & (1 << type)))
        {
            *pEnable = ENABLED;
        }     
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_IMR_PORT6_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;

        if(0 == (tmp_value & (1 << type)))
        {
            *pEnable = DISABLED;
        }
        else if(1 == (tmp_value & (1 << type)))
        {
            *pEnable = ENABLED;
        }         
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_IMR_PORT7_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;

        if(0 == (tmp_value & (1 << type)))
        {
            *pEnable = DISABLED;
        }
        else if(1 == (tmp_value & (1 << type)))
        {
            *pEnable = ENABLED;
        }         
    }
    else if(port == 4)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT4_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;

		if(0 == (tmp_value & (1 << type)))
		{
			*pEnable = DISABLED;
		}
		else if(1 == (tmp_value & (1 << type)))
		{
			*pEnable = ENABLED;
		}		 
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT5_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;

		if(0 == (tmp_value & (1 << type)))
		{
			*pEnable = DISABLED;
		}
		else if(1 == (tmp_value & (1 << type)))
		{
			*pEnable = ENABLED;
		}	 
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT6_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;

		if(0 == (tmp_value & (1 << type)))
		{
			*pEnable = DISABLED;
		}
		else if(1 == (tmp_value & (1 << type)))
		{
			*pEnable = ENABLED;
		}		 
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_IMR_PORT7_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;

		if(0 == (tmp_value & (1 << type)))
		{
			*pEnable = DISABLED;
		}
		else if(1 == (tmp_value & (1 << type)))
		{
			*pEnable = ENABLED;
		}		 
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_wrapper_int_status_set
 * Description:
 *      Configure MACsec interrupt status.
 * Input:
 *      port  -  port id
 *      statusMask  -  interrupt status mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will clean MACsec interrupt status when interrupt happened.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_TX_IPE_GLB [bit[0]]
 *      - INT_TYPE_TX_IPESECFAIL [bit[1]]
 *      - INT_TYPE_TX_IPELOCK [bit[2]]
 *      - INT_TYPE_TX_IPELOCK_XG [bit[3]]
 *      - INT_TYPE_RX_IPI_GLB [bit[8]]
 *      - INT_TYPE_RX_IPISECFAIL [bit[9]]
 *      - INT_TYPE_RX_IPILOCK [bit[10]]
 *      - INT_TYPE_RX_IPILOCK_XG [bit[11]]
 */
rtk_api_ret_t dal_rtl8373_wrapper_int_status_set(rtk_uint32 port, rtk_uint32 statusMask)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_ISR_PORT4_ADDR,  statusMask)) != RT_ERR_OK)
                    return retVal;        
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_ISR_PORT5_ADDR,  statusMask)) != RT_ERR_OK)
                    return retVal;     
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_ISR_PORT6_ADDR,  statusMask)) != RT_ERR_OK)
                    return retVal;     
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_reg_write(RTL8373_MACSEC_REG_GLB_ISR_PORT7_ADDR,  statusMask)) != RT_ERR_OK)
                    return retVal;         
    }
    else if(port == 4)
	{
		if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_ISR_PORT4_ADDR,  statusMask)) != RT_ERR_OK)
        			return retVal;		
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_ISR_PORT5_ADDR,  statusMask)) != RT_ERR_OK)
        			return retVal;	 
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_ISR_PORT6_ADDR,  statusMask)) != RT_ERR_OK)
        			return retVal;	 
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_setAsicReg(RTL8373_MACSEC_REG_GLB_ISR_PORT7_ADDR,  statusMask)) != RT_ERR_OK)
        			return retVal;		 
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_wrapper_int_status_get
 * Description:
 *      Configure MACsec interrupt status.
 * Input:
 *      port  -  port id
 
 * Output:
 *      pStatusMask  -  interrupt status mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will clean MACsec interrupt status when interrupt happened.
 *      The interrupt trigger status is shown in the following:
 *      - INT_TYPE_TX_IPE_GLB [bit[0]]
 *      - INT_TYPE_TX_IPESECFAIL [bit[1]]
 *      - INT_TYPE_TX_IPELOCK [bit[2]]
 *      - INT_TYPE_TX_IPELOCK_XG [bit[3]]
 *      - INT_TYPE_RX_IPI_GLB [bit[8]]
 *      - INT_TYPE_RX_IPISECFAIL [bit[9]]
 *      - INT_TYPE_RX_IPILOCK [bit[10]]
 *      - INT_TYPE_RX_IPILOCK_XG [bit[11]]
 */
rtk_api_ret_t dal_rtl8373_wrapper_int_status_get(rtk_uint32 port, rtk_uint32 *pStatusMask)
{
	rtk_api_ret_t retVal;
	rtk_uint32 tmp_value;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_ISR_PORT4_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;        
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_ISR_PORT5_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;     
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_ISR_PORT6_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;     
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_REG_GLB_ISR_PORT7_ADDR,  &tmp_value)) != RT_ERR_OK)
                    return retVal;         
    }
    else if(port == 4)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_ISR_PORT4_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;		
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_ISR_PORT5_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;	 
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_ISR_PORT6_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;	 
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_REG_GLB_ISR_PORT7_ADDR,  &tmp_value)) != RT_ERR_OK)
        			return retVal;		 
	}
	else
		return RT_ERR_INPUT;

	*pStatusMask = tmp_value & 0xf0f;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_wrapper_mib_reset
 * Description:
 *      Configure wrapper mib reset.
 * Input:
 *      port  - port id
 *      reset  -  reset value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will configure wrapper mib reset.
 */
rtk_api_ret_t dal_rtl8373_wrapper_mib_reset(rtk_uint32 port, rtk_uint32 reset)
{
	rtk_api_ret_t retVal;

	if(port == 0)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR,  RTL8373_MACSEC_REG_GLB_SET1_PORT4_MIBCNT_SWRST_N_OFFSET, reset?0:1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 1)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR,  RTL8373_MACSEC_REG_GLB_SET1_PORT5_MIBCNT_SWRST_N_OFFSET, reset?0:1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 2)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR,  RTL8373_MACSEC_REG_GLB_SET1_PORT6_MIBCNT_SWRST_N_OFFSET, reset?0:1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 3)
    {
        if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR,  RTL8373_MACSEC_REG_GLB_SET1_PORT7_MIBCNT_SWRST_N_OFFSET, reset?0:1)) != RT_ERR_OK)
                    return retVal;
    }
    else if(port == 4)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR,  RTL8373_MACSEC_REG_GLB_SET1_PORT4_MIBCNT_SWRST_N_OFFSET, reset?0:1)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 5)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR,  RTL8373_MACSEC_REG_GLB_SET1_PORT5_MIBCNT_SWRST_N_OFFSET, reset?0:1)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 6)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR,  RTL8373_MACSEC_REG_GLB_SET1_PORT6_MIBCNT_SWRST_N_OFFSET, reset?0:1)) != RT_ERR_OK)
        			return retVal;
	}
	else if(port == 7)
	{
		if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR,  RTL8373_MACSEC_REG_GLB_SET1_PORT7_MIBCNT_SWRST_N_OFFSET, reset?0:1)) != RT_ERR_OK)
        			return retVal;
	}
	else
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_wrapper_mib_counter
 * Description:
 *      get wrapper mib counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get wrapper mib counters.
 */
rtk_api_ret_t dal_rtl8373_wrapper_mib_counter(rtk_uint32 port, RTL8373_WRAPPER_MIBCOUNTER mibIdx, rtk_uint64* pCounter)
{
	rtk_api_ret_t retVal;
	rtk_uint32 tmp_value_L;
	rtk_uint32 tmp_value_H;
	rtk_uint64 tmp;

	if(port == 0)
    {
        if((mibIdx == TXSYS_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK))
        {
            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT4_ADDR + mibIdx * 0x4, &tmp_value_L)) != RT_ERR_OK)
                        return retVal;

            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT4_ADDR + (mibIdx + 1) * 0x4, &tmp_value_H)) != RT_ERR_OK)
                        return retVal;

            tmp = tmp_value_H;
            *pCounter = (tmp << 32) | tmp_value_L;
            
        }
        else
        {
            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT4_ADDR + mibIdx * 0x4, (rtk_uint32 *)pCounter)) != RT_ERR_OK)
                        return retVal;
        }
    }
    else if(port == 1)
    {
        if((mibIdx == TXSYS_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK))
        {
            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT5_ADDR + mibIdx * 0x4, &tmp_value_L)) != RT_ERR_OK)
                        return retVal;

            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT5_ADDR + (mibIdx + 1) * 0x4, &tmp_value_H)) != RT_ERR_OK)
                        return retVal;

            tmp = tmp_value_H;
            *pCounter = (tmp << 32) | tmp_value_L;
        }
        else
        {
            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT5_ADDR + mibIdx * 0x4, (rtk_uint32 *)pCounter)) != RT_ERR_OK)
                        return retVal;
        }
    }
    else if(port == 2)
    {
        if((mibIdx == TXSYS_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK))
        {
            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT6_ADDR + mibIdx * 0x4, &tmp_value_L)) != RT_ERR_OK)
                        return retVal;

            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT6_ADDR + (mibIdx + 1) * 0x4, &tmp_value_H)) != RT_ERR_OK)
                        return retVal;

            tmp = tmp_value_H;
            *pCounter = (tmp << 32) | tmp_value_L;
        }
        else
        {
            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT6_ADDR + mibIdx * 0x4, (rtk_uint32 *)pCounter)) != RT_ERR_OK)
                        return retVal;
        }
    }
    else if(port == 3)
    {
        if((mibIdx == TXSYS_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK))
        {
            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT7_ADDR + mibIdx * 0x4, &tmp_value_L)) != RT_ERR_OK)
                        return retVal;

            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT7_ADDR + (mibIdx + 1) * 0x4, &tmp_value_H)) != RT_ERR_OK)
                        return retVal;

            tmp = tmp_value_H;
            *pCounter = (tmp << 32) | tmp_value_L;
        }
        else
        {
            if ((retVal = dal_rtl8224_top_reg_read(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT7_ADDR + mibIdx * 0x4, (rtk_uint32 *)pCounter)) != RT_ERR_OK)
                        return retVal;
        }
    }
    else if(port == 4)
	{
		if((mibIdx == TXSYS_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK))
		{
			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT4_ADDR + mibIdx * 0x4, &tmp_value_L)) != RT_ERR_OK)
	        			return retVal;

			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT4_ADDR + (mibIdx + 1) * 0x4, &tmp_value_H)) != RT_ERR_OK)
	        			return retVal;

			tmp = tmp_value_H;
			*pCounter = (tmp << 32) | tmp_value_L;
			
		}
		else
		{
			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT4_ADDR + mibIdx * 0x4, (rtk_uint32 *)pCounter)) != RT_ERR_OK)
	        			return retVal;
		}
	}
	else if(port == 5)
	{
		if((mibIdx == TXSYS_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK))
		{
			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT5_ADDR + mibIdx * 0x4, &tmp_value_L)) != RT_ERR_OK)
	        			return retVal;

			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT5_ADDR + (mibIdx + 1) * 0x4, &tmp_value_H)) != RT_ERR_OK)
	        			return retVal;

			tmp = tmp_value_H;
			*pCounter = (tmp << 32) | tmp_value_L;
		}
		else
		{
			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT5_ADDR + mibIdx * 0x4, (rtk_uint32 *)pCounter)) != RT_ERR_OK)
	        			return retVal;
		}
	}
	else if(port == 6)
	{
		if((mibIdx == TXSYS_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK))
		{
			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT6_ADDR + mibIdx * 0x4, &tmp_value_L)) != RT_ERR_OK)
	        			return retVal;

			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT6_ADDR + (mibIdx + 1) * 0x4, &tmp_value_H)) != RT_ERR_OK)
	        			return retVal;

			tmp = tmp_value_H;
			*pCounter = (tmp << 32) | tmp_value_L;
		}
		else
		{
			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT6_ADDR + mibIdx * 0x4, (rtk_uint32 *)pCounter)) != RT_ERR_OK)
	        			return retVal;
		}
	}
	else if(port == 7)
	{
		if((mibIdx == TXSYS_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK) || (mibIdx == TXLINE_OK))
		{
			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT7_ADDR + mibIdx * 0x4, &tmp_value_L)) != RT_ERR_OK)
	        			return retVal;

			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT7_ADDR + (mibIdx + 1) * 0x4, &tmp_value_H)) != RT_ERR_OK)
	        			return retVal;

			tmp = tmp_value_H;
			*pCounter = (tmp << 32) | tmp_value_L;
		}
		else
		{
			if ((retVal = rtl8373_getAsicReg(RTL8373_MACSEC_TXSYSCRCERR_CNT_PORT7_ADDR + mibIdx * 0x4, (rtk_uint32 *)pCounter)) != RT_ERR_OK)
	        			return retVal;
		}
	}
	else 
		return RT_ERR_INPUT;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_ipg_len_set
 * Description:
 *      mac mode MACsec ipg length set.
 * Input:
 *      port   -  port number
 *      length - ipg length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will set mac mode MACsec ipg length.
 */
rtk_api_ret_t dal_rtl8373_macsec_ipg_len_set(rtk_uint32 port, rtk_uint32 length)
{
   	rtk_api_ret_t retVal;

	if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_IPG_CFG_ADDR(port), RTL8373_MAC_MACSEC_IPG_CFG_MACSEC_IPG_LENGTH_MASK, length)) != RT_ERR_OK)
        		return retVal;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_ipg_len_get
 * Description:
 *      mac mode MACsec ipg length get.
 * Input:
 *      port   -  port number
 * Output:
 *      plength - ipg length
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get mac mode MACsec ipg length.
 */
rtk_api_ret_t dal_rtl8373_macsec_ipg_len_get(rtk_uint32 port, rtk_uint32 *plength)
{
   	rtk_api_ret_t retVal;
	rtk_uint32 length = 0;

	if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_IPG_CFG_ADDR(port), RTL8373_MAC_MACSEC_IPG_CFG_MACSEC_IPG_LENGTH_MASK, &length)) != RT_ERR_OK)
        		return retVal;

	*plength = length;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_ipg_mode_set
 * Description:
 *      mac mode MACsec ipg mode set.
 * Input:
 *      port   -  port number
 *      mode -  ipg config mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will set mac mode MACsec ipg mode.
 *	  mode[1:0]:
		0: don't insert ipg for macsec
		1: insert ipg for macsec according to macsec feedback signal
		2: insert ipg for masec according to ethertype & cfg_macsec_ipg_length
		3: always insert ipg for macsec, length according to cfg_macsec_ipg_length
 */
rtk_api_ret_t dal_rtl8373_macsec_ipg_mode_set(rtk_uint32 port, rtk_uint32 mode)
{
   	rtk_api_ret_t retVal;

	if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_IPG_CFG_ADDR(port), RTL8373_MAC_MACSEC_IPG_CFG_MACSEC_IPG_MODE_MASK, mode)) != RT_ERR_OK)
        		return retVal;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_ipg_mode_get
 * Description:
 *      mac mode MACsec ipg mode get.
 * Input:
 *      port   -  port number
 * Output:
 *      pmode - ipg config mode
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get mac mode MACsec ipg mode.
 *	  mode[1:0]:
		0: don't insert ipg for macsec
		1: insert ipg for macsec according to macsec feedback signal
		2: insert ipg for masec according to ethertype & cfg_macsec_ipg_length
		3: always insert ipg for macsec, length according to cfg_macsec_ipg_length
 */
rtk_api_ret_t dal_rtl8373_macsec_ipg_mode_get(rtk_uint32 port, rtk_uint32 *pmode)
{
   	rtk_api_ret_t retVal;
	rtk_uint32 mode = 0;

	if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_IPG_CFG_ADDR(port), RTL8373_MAC_MACSEC_IPG_CFG_MACSEC_IPG_MODE_MASK, &mode)) != RT_ERR_OK)
        		return retVal;

	*pmode = mode;

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_eth_set
 * Description:
 *      mac mode MACsec eth set.
 * Input:
 *      port   -  port number
 *      entry - entry number(0-7)
 *      ethertype - ether type value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will set mac mode MACsec eth.
 */
rtk_api_ret_t dal_rtl8373_macsec_eth_set(rtk_uint32 port, rtk_uint32 entry, rtk_uint32 ethertype)
{
   	rtk_api_ret_t retVal;

	switch(entry)
	{
		case 0:
			if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_1_0_ADDR(port), RTL8373_MAC_MACSEC_ETH_1_0_MACSEC_ETH_0_MASK, ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 1:
			if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_1_0_ADDR(port), RTL8373_MAC_MACSEC_ETH_1_0_MACSEC_ETH_1_MASK, ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 2:
			if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_3_2_ADDR(port), RTL8373_MAC_MACSEC_ETH_3_2_MACSEC_ETH_2_MASK, ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 3:
			if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_3_2_ADDR(port), RTL8373_MAC_MACSEC_ETH_3_2_MACSEC_ETH_3_MASK, ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 4:
			if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_5_4_ADDR(port), RTL8373_MAC_MACSEC_ETH_5_4_MACSEC_ETH_4_MASK, ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 5:
			if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_5_4_ADDR(port), RTL8373_MAC_MACSEC_ETH_5_4_MACSEC_ETH_5_MASK, ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 6:
			if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_7_6_ADDR(port), RTL8373_MAC_MACSEC_ETH_7_6_MACSEC_ETH_6_MASK, ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 7:
			if ((retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_7_6_ADDR(port), RTL8373_MAC_MACSEC_ETH_7_6_MACSEC_ETH_7_MASK, ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		default:
			return RT_ERR_INPUT;       
	}

	return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8373_macsec_eth_get
 * Description:
 *      mac mode MACsec eth get.
 * Input:
 *      port   -  port number
 *      entry - entry number(0-7)
 * Output:
 *      pethertype - ether type value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will get mac mode MACsec eth.
 */
rtk_api_ret_t dal_rtl8373_macsec_eth_get(rtk_uint32 port, rtk_uint32 entry, rtk_uint32 *pethertype)
{
   	rtk_api_ret_t retVal;
	rtk_uint32 ethertype = 0;

	switch(entry)
	{
		case 0:
			if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_1_0_ADDR(port), RTL8373_MAC_MACSEC_ETH_1_0_MACSEC_ETH_0_MASK, &ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 1:
			if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_1_0_ADDR(port), RTL8373_MAC_MACSEC_ETH_1_0_MACSEC_ETH_1_MASK, &ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 2:
			if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_3_2_ADDR(port), RTL8373_MAC_MACSEC_ETH_3_2_MACSEC_ETH_2_MASK, &ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 3:
			if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_3_2_ADDR(port), RTL8373_MAC_MACSEC_ETH_3_2_MACSEC_ETH_3_MASK, &ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 4:
			if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_5_4_ADDR(port), RTL8373_MAC_MACSEC_ETH_5_4_MACSEC_ETH_4_MASK, &ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 5:
			if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_5_4_ADDR(port), RTL8373_MAC_MACSEC_ETH_5_4_MACSEC_ETH_5_MASK, &ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 6:
			if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_7_6_ADDR(port), RTL8373_MAC_MACSEC_ETH_7_6_MACSEC_ETH_6_MASK, &ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		case 7:
			if ((retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_7_6_ADDR(port), RTL8373_MAC_MACSEC_ETH_7_6_MACSEC_ETH_7_MASK, &ethertype)) != RT_ERR_OK)
	        			return retVal;
			break;
		default:
			return RT_ERR_INPUT;       
	}

	*pethertype = ethertype;

	return RT_ERR_OK;
}

#if 1

/* Function Name:
 *      dal_rtl8373_macsec_AIC_int_mask_set
 * Description:
 *      Set Macsec IP Advanced Interrupt Control mask bits.
 * Input:
 *      port   -  port id, 0~7
 *      dir     -  ingress / egress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_AIC_int_mask_set(rtk_uint32 port, rtk_macsec_dir_t dir, rtk_macsec_aic_int_type_t type, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regdata;


    if(dir == MACSEC_DIRECTION_INGRESS)
    {
        if ((retVal = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_AIC_ENABLE_CTL, &regdata)) != RT_ERR_OK)
            return retVal;

        if(enable == 1)
            regdata |= 1<<type;
        else
            regdata &= ~(1<<type);

        if ((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_AIC_ENABLE_CTL, regdata)) != RT_ERR_OK)
            return retVal;
        
        if ((retVal = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_AIC_ENABLE_SET, &regdata)) != RT_ERR_OK)
            return retVal;

        if(enable == 1)
            regdata |= 1<<type;
        else
            regdata &= ~(1<<type);

        if ((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_AIC_ENABLE_SET, regdata)) != RT_ERR_OK)
            return retVal;

		return RT_ERR_OK;
    }
    else if(dir == MACSEC_DIRECTION_EGRESS)
    {
        if ((retVal = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_AIC_ENABLE_CTL, &regdata)) != RT_ERR_OK)
            return retVal;

        if(enable == 1)
            regdata |= 1<<type;
        else
            regdata &= ~(1<<type);

        if ((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_AIC_ENABLE_CTL, regdata)) != RT_ERR_OK)
            return retVal;
        
        if ((retVal = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_AIC_ENABLE_SET, &regdata)) != RT_ERR_OK)
            return retVal;

        if(enable == 1)
            regdata |= 1<<type;
        else
            regdata &= ~(1<<type);

        if ((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_AIC_ENABLE_SET, regdata)) != RT_ERR_OK)
            return retVal;

		return RT_ERR_OK;
    }
    else
        return RT_ERR_INPUT;


}


/* Function Name:
 *      dal_rtl8373_macsec_AIC_int_mask_set
 * Description:
 *      Set Macsec IP Advanced Interrupt Control mask bits.
 * Input:
 *      port   -  port id, 0~7
 *      dir     -  ingress / egress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_AIC_int_mask_get(rtk_uint32 port, rtk_macsec_dir_t dir, rtk_macsec_aic_int_type_t type, rtk_enable_t* pEnable)
{
    rtk_api_ret_t retVal;
    rtk_uint32 regdata0, regdata1;


    if(dir == MACSEC_DIRECTION_INGRESS)
    {
        if ((retVal = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_AIC_ENABLE_CTL, &regdata0)) != RT_ERR_OK)
            return retVal;

        
        if ((retVal = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_AIC_ENABLE_SET, &regdata1)) != RT_ERR_OK)
            return retVal;

        if(((regdata0>>type) & 1) & ((regdata1 >> type) & 1))
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;

		return RT_ERR_OK;

    }
    else if(dir == MACSEC_DIRECTION_EGRESS)
    {
        if ((retVal = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_AIC_ENABLE_CTL, &regdata0)) != RT_ERR_OK)
            return retVal;

        
        if ((retVal = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_AIC_ENABLE_SET, &regdata1)) != RT_ERR_OK)
            return retVal;

        if(((regdata0>>type) & 1) & ((regdata1 >> type) & 1))
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;

		return RT_ERR_OK;
    }
    else
        return RT_ERR_INPUT;

}


static rtk_api_ret_t _rtl8373_macsec_reg_array_get(rtk_port_t port, rtk_macsec_dir_t dir,
                         const rtk_uint32 reg, rtk_uint32* pArray, const rtk_uint32 cnt)
{
    rtk_int32  ret = RT_ERR_OK;
    rtk_uint32 data = 0, i = 0;
    rtk_uint32 offset = reg;


    for (i = 0; i < cnt; i++ )
    {
        if (dir == MACSEC_DIRECTION_INGRESS)
        {
            if ((ret = dal_rtl8373_macsec_ingress_get(port, offset, &data))!=RT_ERR_OK)
                return ret;
            pArray[i] = data;
            offset += 4;
        }
        else
        {
            if ((ret = dal_rtl8373_macsec_egress_get(port, offset, &data))!=RT_ERR_OK)
                return ret;
            pArray[i] = data;
            offset += 4;
        }

    }

    return ret;
}

static rtk_api_ret_t _rtl8373_macsec_reg_array_set(rtk_port_t port, rtk_macsec_dir_t dir,
                         const rtk_uint32 reg, rtk_uint32* pArray, const rtk_uint32 cnt)
{
    rtk_int32  ret = RT_ERR_OK;
    rtk_uint32 data = 0, i = 0;
    rtk_uint32 offset = reg;

    for (i = 0; i < cnt; i++ )
    {
        data = pArray[i];
        if (dir == MACSEC_DIRECTION_INGRESS)
        {
            if ((ret = dal_rtl8373_macsec_ingress_set(port, offset, data))!=RT_ERR_OK)
                return ret;
            offset += 4;
        }
        else
        {
            if ((ret = dal_rtl8373_macsec_egress_set(port, offset, data))!=RT_ERR_OK)
                return ret;
            offset += 4;
        }

    }

    return ret;
}




static void _rtl8373_macsec_copy_key_to_raw(rtk_uint32 *pRaw, rtk_uint32 offset, rtk_uint8 *pKey, rtk_uint32 key_bytes)
{
    uint32_t *dst = pRaw + offset;
    const uint8_t *src = pKey;
    unsigned int i,j;
    uint32_t w;
    if (pRaw == NULL)
        return;
    for(i=0; i<(key_bytes+3)/4; i++)
    {
        w=0;
        for(j=0; j<4; j++)
            w=(w>>8)|(*src++ << 24);
        *dst++ = w;
    }
}

static void _rtl8373_macsec_copy_raw_to_key(rtk_uint32 *pRaw, rtk_uint32 offset ,rtk_uint8 *pKey, rtk_uint32 raw_words)
{
    uint32_t *src = pRaw + offset;
    uint8_t *dst = pKey;
    unsigned int i;
    if (pRaw == NULL)
        return;

    for (i = 0; i < raw_words; i++)
    {
        *dst++ = (rtk_uint8)((src[i] & 0xff));
        *dst++ = (rtk_uint8)((src[i] & 0xff00) >> 8) ;
        *dst++ = (rtk_uint8)((src[i] & 0xff0000) >> 16) ;
        *dst++ = (rtk_uint8)((src[i] & 0xff000000) >> 24) ;
    }
}

static rtk_api_ret_t _rtl8373_macsec_sa_parse(rtk_port_t port, rtk_uint32 sa_index, rtk_uint32 *pSa_raw, rtl8373_macsec_sa_params_t *pSa)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 next_offs = 0, words = 0;
    rtk_uint8 tmp8 = 0;
    
    if (pSa == NULL || pSa_raw == NULL)
        return RT_ERR_INPUT;

    memset(pSa, 0, sizeof(rtl8373_macsec_sa_params_t));

    if (pSa_raw[0] & BIT_29_IN32)
    {
        pSa->flags |= RTL8373_MACSEC_SA_FLAG_XPN;
    }

    if ((pSa_raw[0] & MASK_4_BITS) == 0b0110)
    {
        pSa->direction = MACSEC_DIRECTION_EGRESS;

        pSa->an = (rtk_uint8)((pSa_raw[0] >> 26) & 0x3);
    }
    else
    {
        pSa->direction = MACSEC_DIRECTION_INGRESS;
    }

    if (((pSa_raw[0] >> 17) & MASK_3_BITS) == 0b101)
    {
        pSa->key_bytes = 16;
    }
    else
    {
        pSa->key_bytes = 32;
    }
    next_offs = 2;

    pSa->context_id = pSa_raw[1];

    /* key */
    words = (pSa->key_bytes * 8) / 32;
    _rtl8373_macsec_copy_raw_to_key(pSa_raw, next_offs, pSa->key, words);
    next_offs += words + 4;

    /* seq */
    pSa->seq = pSa_raw[next_offs];
    words = 1;

    if (pSa->flags & RTL8373_MACSEC_SA_FLAG_XPN)
    {
        pSa->seq_h = pSa_raw[next_offs+1];
        words = 2;
    }
    next_offs += words;

    /* replay_window(ingress) */
    if (pSa->direction == MACSEC_DIRECTION_EGRESS)
    {
        words = (pSa->flags & RTL8373_MACSEC_SA_FLAG_XPN) ? 1 : 0;
    }
    else
    {
        pSa->replay_window = pSa_raw[next_offs];
        words = 1;
    }
    next_offs += words;

    /* CtxSalt */
    if (pSa->flags & RTL8373_MACSEC_SA_FLAG_XPN)
    {
        words = 3;

        pSa->ssci[0] = port_macsec_info->port[port]->sa_info[sa_index].ssci[0];
        pSa->ssci[1] = port_macsec_info->port[port]->sa_info[sa_index].ssci[1];
        pSa->ssci[2] = port_macsec_info->port[port]->sa_info[sa_index].ssci[2];
        pSa->ssci[3] = port_macsec_info->port[port]->sa_info[sa_index].ssci[3];

        tmp8 = (rtk_uint8)(pSa_raw[next_offs] & 0xFF);
        pSa->salt[0] = (tmp8 ^ (pSa->ssci[0]));
        tmp8 = (rtk_uint8)((pSa_raw[next_offs] & 0xFF00) >> 8 );
        pSa->salt[1] = (tmp8 ^ (pSa->ssci[1]));
        tmp8 = (rtk_uint8)((pSa_raw[next_offs] & 0xFF0000) >> 16 );
        pSa->salt[2] = (tmp8 ^ (pSa->ssci[2]));
        tmp8 = (rtk_uint8)((pSa_raw[next_offs] & 0xFF000000) >> 24 );
        pSa->salt[3] = (tmp8 ^ (pSa->ssci[3]));

        _rtl8373_macsec_copy_raw_to_key(pSa_raw, next_offs + 1, &pSa->salt[4], 2);

    }
    else
    {
        words = 0;
    }
    next_offs += words;

    /* IV(SCI) */
    if (pSa->flags & RTL8373_MACSEC_SA_FLAG_XPN)
    {
        if (pSa->direction == MACSEC_DIRECTION_EGRESS)
        {
            words = 3;
        }
        else
        {
            words = 0;
        }
    }
    else
    {
        words = 4;
    }

    if (words != 0)
    {
        _rtl8373_macsec_copy_raw_to_key(pSa_raw, next_offs, pSa->sci, 2);
    }
    next_offs += words;

    /* Update Control */
    if (pSa->direction == MACSEC_DIRECTION_EGRESS)
    {

        pSa->flow_index = (rtk_uint32)((pSa_raw[next_offs] >> 16) & MASK_15_BITS);
        pSa->next_sa_index = (rtk_uint32)(pSa_raw[next_offs] & MASK_14_BITS);
        pSa->update_en = (pSa_raw[next_offs] & BIT_31_IN32) ? 1 : 0;
        pSa->next_sa_valid = (pSa_raw[next_offs] & BIT_15_IN32) ? 1 : 0;
        pSa->sa_expired_irq = (pSa_raw[next_offs] & BIT_14_IN32) ? 1 : 0;
        rtlglue_printf("flow_index: %u\n", pSa->flow_index);
        rtlglue_printf("next_sa_index: %u\n", pSa->next_sa_index);
        rtlglue_printf("update_en: %u, next_sa_valid: %u, sa_expired_irq:%u\n", pSa->update_en, pSa->next_sa_valid,  pSa->sa_expired_irq);
    }

    return ret;
}



static void _rtl8373_macsec_sa_offset_parse(rtl8373_macsec_sa_params_t *pSa, rtl8373_macsec_sa_offset_t *pOffs)
{
    unsigned int long_key;
    memset(pOffs, 0, sizeof(rtl8373_macsec_sa_offset_t));

    pOffs->key_offs = 2;
    if (pSa->key_bytes == 16)
    {
        long_key = 0;
    }
    else
    {
        long_key = 4;
    }
    pOffs->hkey_offs = long_key + 6;
    pOffs->seq_offs = long_key + 10;
    if (pSa->direction == MACSEC_DIRECTION_EGRESS)
    {
        if ((pSa->flags & RTL8373_MACSEC_SA_FLAG_XPN) != 0)
        {
            pOffs->ctx_salt_offs = long_key + 13;
            pOffs->iv_offs = long_key + 16;
            if (long_key)
                pOffs->upd_ctrl_offs = 16;
            else
                pOffs->upd_ctrl_offs = 19;
        }
        else
        {
            pOffs->iv_offs = long_key + 11;
            pOffs->upd_ctrl_offs = long_key + 15;
        }
    }
    else
    {
        if ((pSa->flags & RTL8373_MACSEC_SA_FLAG_XPN) != 0)
        {
            pOffs->mask_offs = long_key + 12;
            pOffs->ctx_salt_offs = long_key + 13;
            pOffs->upd_ctrl_offs = 0;
        }
        else
        {
            pOffs->mask_offs = long_key + 11;
            pOffs->iv_offs = long_key + 12;
            pOffs->upd_ctrl_offs = 0;
        }
    }
}



rtk_api_ret_t dal_rtl8373_macsec_context_id_gen(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sa_index)
{
    rtk_uint32 context_id = 0;
    context_id = 0x10000 * (port_macsec_info->port[port]->sa_gen_seq & (0xFFFF)) + 0x1000 * (dir)+ sa_index;
    port_macsec_info->port[port]->sa_gen_seq++;
    return context_id;
}


rtk_api_ret_t dal_rtl8373_macsec_hw_sa_build(rtk_port_t port, rtk_uint32 sa_index, rtl8373_macsec_sa_params_t *pSa,  rtk_uint32 *pSa_raw)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtl8373_macsec_sa_offset_t offs;

    rtk_uint8 hkey[16] = { 0 };
    rtk_uint32 tmp = 0;
    uint32_t seq = 0; // sequence number.
    uint32_t seq_h = 0; // High part of sequence number (64-bit sequence numbers)
    rtk_uint32 gen_id = 0;

    if (pSa == NULL || pSa_raw == NULL)
        return RT_ERR_INPUT;
    if (pSa->an > 3)
        return RT_ERR_INPUT;

    if ((pSa->direction != MACSEC_DIRECTION_INGRESS) &&
        (pSa->direction != MACSEC_DIRECTION_EGRESS))
    {
        rtlglue_printf("error direction input\n");
        return RT_ERR_INPUT;
    }

    if (pSa->context_id == 0)
    {
        gen_id = dal_rtl8373_macsec_context_id_gen(port, pSa->direction, sa_index);
    }
    else
    {
        gen_id = pSa->context_id;
    }

    // Compute offsets for various fields.
    _rtl8373_macsec_sa_offset_parse(pSa, &offs);

    // Fill the entire SA record with zeros.
    memset(pSa_raw, 0, RTL8373_MACSEC_MAX_SA_SIZE * sizeof(uint32_t));

    if (pSa->direction == MACSEC_DIRECTION_EGRESS)
    {
        if((pSa->flags & RTL8373_MACSEC_SA_FLAG_XPN) != 0)
        {
            pSa_raw[0] = MACSEC_SAB_CW0_MACSEC_EG64;
        }
        else
        {
            pSa_raw[0] = MACSEC_SAB_CW0_MACSEC_EG32;
        }
        seq = pSa->seq;
        seq_h = pSa->seq_h;
        pSa_raw[0] |= (pSa->an & 0x3) << 26;
    }
    else //RTK_MACSEC_DIR_INGRESS
    {
        if((pSa->flags & RTL8373_MACSEC_SA_FLAG_XPN) != 0)
        {
            pSa_raw[0] = MACSEC_SAB_CW0_MACSEC_IG64;
        }
        else
        {
            pSa_raw[0] = MACSEC_SAB_CW0_MACSEC_IG32;
        }
        seq = (pSa->seq == 0 && pSa->seq_h == 0) ? 1 : pSa->seq;
        seq_h = pSa->seq_h;
    }

    switch (pSa->key_bytes)
    {
        case 16:
            pSa_raw[0] |= MACSEC_SAB_CW0_AES128;
            break;
        case 32:
            pSa_raw[0] |= MACSEC_SAB_CW0_AES256;
            break;
        default:
            rtlglue_printf("unsupported AES key size:%d\n", pSa->key_bytes);
            return RT_ERR_INPUT;
    }

    // Fill in ID
    pSa_raw[1] = gen_id;

    // Fill Key and HKey
    _rtl8373_macsec_copy_key_to_raw(pSa_raw, offs.key_offs, pSa->key, pSa->key_bytes);
    /* generate hkey from key, encrypt a single all-zero block */
    AES_Encrypt((uint8_t *)(pSa_raw + offs.hkey_offs),  hkey, pSa->key, pSa->key_bytes);
    _rtl8373_macsec_copy_key_to_raw(pSa_raw, offs.hkey_offs, hkey, 16);

    // Fill in sequence number/seqmask.
    pSa_raw[offs.seq_offs] = seq;
    if ((pSa->flags & RTL8373_MACSEC_SA_FLAG_XPN) != 0)
        pSa_raw[offs.seq_offs + 1] = seq_h;

    if (pSa->direction == MACSEC_DIRECTION_INGRESS)
        pSa_raw[offs.mask_offs] = pSa->replay_window;

    // Fill in CtxSalt field.
    if (offs.ctx_salt_offs > 0)
    {
        //phy_macsec_copy_key_to_raw(&phy_macsec_info[unit]->ssci[port], 0, pSa->ssci, 8);
        port_macsec_info->port[port]->sa_info[sa_index].ssci[0] = pSa->ssci[0];
        port_macsec_info->port[port]->sa_info[sa_index].ssci[1] = pSa->ssci[1];
        port_macsec_info->port[port]->sa_info[sa_index].ssci[2] = pSa->ssci[2];
        port_macsec_info->port[port]->sa_info[sa_index].ssci[3] = pSa->ssci[3];

        //[0] = most significant 32-bits Salt XOR-ed with SSCI
        tmp =  (pSa->salt[0] ^ pSa->ssci[0]) |
              ((pSa->salt[1] ^ pSa->ssci[1]) << 8)  |
              ((pSa->salt[2] ^ pSa->ssci[2]) << 16) |
              ((pSa->salt[3] ^ pSa->ssci[3]) << 24);
        pSa_raw[offs.ctx_salt_offs] = tmp;
        //[1:2] = lower 64-bits Salt
        _rtl8373_macsec_copy_key_to_raw(pSa_raw, offs.ctx_salt_offs + 1, pSa->salt + 4, 8);
    }

    // Fill in IV(SCI) fields.
    if (offs.iv_offs > 0)
    {
        _rtl8373_macsec_copy_key_to_raw(pSa_raw, offs.iv_offs, pSa->sci, 8);
    }

    // Fill in update control fields.
    if(offs.upd_ctrl_offs > 0)
    {
        tmp = (pSa->next_sa_index & MASK_14_BITS) |
            ((pSa->flow_index & MASK_15_BITS) << 16);

        if (pSa->update_en)
            tmp |= BIT_31_IN32;
        if (pSa->next_sa_valid)
            tmp |= BIT_15_IN32;
        if (pSa->sa_expired_irq)
            tmp |= BIT_14_IN32;

        pSa_raw[offs.upd_ctrl_offs] = tmp;
    }

    return ret;
}



rtk_api_ret_t dal_rtl8373_macsec_hw_sa_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sa_index)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 sa_raw[RTL8373_MACSEC_MAX_SA_SIZE] = {0};

  
    if((ret = _rtl8373_macsec_reg_array_set(port, dir, MACSEC_REG_XFORM_REC_OFFS(sa_index, dir, 0), sa_raw, MACSEC_XFORM_REC_SIZE(dir))) != RT_ERR_OK)
            return ret;

    return ret;
}



rtk_api_ret_t dal_rtl8373_macsec_hw_sa_set(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sa_index, rtl8373_macsec_sa_params_t *pSa)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 sa_raw[RTL8373_MACSEC_MAX_SA_SIZE] = {0};

    if (pSa == NULL)
        return RT_ERR_INPUT;

    if (pSa->direction == MACSEC_DIRECTION_EGRESS && dir == MACSEC_DIRECTION_INGRESS)
    {
        rtlglue_printf("direction mismatch!(set egress entry on ingress)");
        return RT_ERR_INPUT;
    }
    if (pSa->direction == MACSEC_DIRECTION_INGRESS && dir == MACSEC_DIRECTION_EGRESS)
    {
        rtlglue_printf("direction mismatch!(set egress entry on ingress)");
        return RT_ERR_INPUT;
    }

    if ((ret = dal_rtl8373_macsec_hw_sa_build(port, sa_index, pSa,  sa_raw)) != RT_ERR_OK)
        return ret;


    if((ret = _rtl8373_macsec_reg_array_set(port, dir, MACSEC_REG_XFORM_REC_OFFS(sa_index, dir, 0), sa_raw, MACSEC_XFORM_REC_SIZE(dir))) != RT_ERR_OK)
        return ret;


    return ret;
}


rtk_api_ret_t dal_rtl8373_macsec_hw_sa_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sa_index, rtl8373_macsec_sa_params_t *pSa)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 sa_raw[RTL8373_MACSEC_MAX_SA_SIZE] = {0};

    if (pSa == NULL)
        return RT_ERR_INPUT;

    if((ret = _rtl8373_macsec_reg_array_get(port, dir, MACSEC_REG_XFORM_REC_OFFS(sa_index, dir, 0), sa_raw, MACSEC_XFORM_REC_SIZE(dir))) != RT_ERR_OK)
        return ret;

    #if 0
    for (ret = 0; ret < MACSEC_XFORM_REC_SIZE(dir); ret++)
    {
        rtlglue_printf("SA[%02u] = 0x%08X\n", ret, sa_raw[ret]);
    }
    ret = RT_ERR_OK;
    #endif

    _rtl8373_macsec_sa_parse(port, sa_index, sa_raw, pSa);

    return ret;
}




/* Function Name:
 *      dal_rtl8373_macsec_match_rule_set
 * Description:
 *      setup a match-rule entry that specify how to classify a packet
 * Input:
 *      port          - port id
 *      flow_index     - table entry index
 *      data          - pointer to the struct for describe the rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_INPUT              - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_match_rule_set(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_macsec_matchrule_t *pMatch)
{
    rtk_api_ret_t  ret = RT_ERR_OK;
    rtk_uint32 reg_data = 0;
    rtk_uint32 srcPort = 0;
    rtk_uint32 tmp = 0;

    if (pMatch == NULL)
        return RT_ERR_INPUT;

    if (flow_index >= RTK_MACSEC_MATCH_RULE_MAX)
    {
        return RT_ERR_INPUT;
    }

    {
        reg_data = 0;
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_sa[3]) & 0xff) << 24);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_sa[2]) & 0xff) << 16);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_sa[1]) & 0xff) << 8);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_sa[0]) & 0xff));
        if (dir == MACSEC_DIRECTION_INGRESS)
            dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SA_LO(flow_index), reg_data);
        else
            dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_SA_LO(flow_index), reg_data);

        reg_data = 0;
        tmp = ((pMatch->etherType & 0xFF) << 8) | (pMatch->etherType >> 8);
        reg_data |= (rtk_uint32)((((rtk_uint32)tmp)  & 0xffff) << 16);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_sa[5]) & 0xff) << 8);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_sa[4]) & 0xff));
        if (dir == MACSEC_DIRECTION_INGRESS)
            dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SA_HI(flow_index), reg_data);
        else
            dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_SA_HI(flow_index), reg_data);
    }

    {
        reg_data = 0;
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_da[3]) & 0xff) << 24);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_da[2]) & 0xff) << 16);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_da[1]) & 0xff) << 8);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_da[0]) & 0xff));
        if (dir == MACSEC_DIRECTION_INGRESS)
            dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_DA_LO(flow_index), reg_data);
        else
            dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_DA_LO(flow_index), reg_data);

        reg_data = 0;
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->vlan_id)   & 0xfff) << 16);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_da[5]) & 0xff) << 8);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->mac_da[4]) & 0xff));
        if (dir == MACSEC_DIRECTION_INGRESS)
            dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_DA_HI(flow_index), reg_data);
        else
            dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_DA_HI(flow_index), reg_data);
    }

    {
        srcPort = pMatch->sourcePort;

        reg_data = 0;
        reg_data |= (pMatch->fVLANValid) ? 0x1 : 0;
        reg_data |= (pMatch->fQinQFound) ? 0x2 : 0;
        reg_data |= (pMatch->fSTagValid) ? 0x4 : 0;
        reg_data |= (pMatch->fQTagValid) ? 0x8 : 0;

        reg_data |= (pMatch->fControlPacket) ? 0x80  : 0;
        reg_data |= (pMatch->fUntagged) ?      0x100  : 0;
        reg_data |= (pMatch->fTagged) ?        0x200  : 0;
        reg_data |= (pMatch->fBadTag) ?        0x400 : 0;
        reg_data |= (pMatch->fKayTag) ?        0x800 : 0;

        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->macsec_TCI_AN)    & 0xff) << 24);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->matchPriority)    & 0xf) << 16);
        reg_data |= (rtk_uint32)((((rtk_uint32)srcPort)                & 0x3) << 12);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->vlanUserPriority) & 0x7) << 4);
        if (dir == MACSEC_DIRECTION_INGRESS)
            dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_MISC(flow_index), reg_data);
        else
            dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_MISC(flow_index), reg_data);
    }

    {
        reg_data = 0;
        reg_data |= (rtk_uint32)(((rtk_uint32)(pMatch->sci[3]) & 0xff) << 24);
        reg_data |= (rtk_uint32)(((rtk_uint32)(pMatch->sci[2]) & 0xff) << 16);
        reg_data |= (rtk_uint32)(((rtk_uint32)(pMatch->sci[1]) & 0xff) << 8);
        reg_data |= (rtk_uint32)(((rtk_uint32)(pMatch->sci[0]) & 0xff));
        if (dir == MACSEC_DIRECTION_INGRESS)
            dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SCI_LO(flow_index), reg_data);
        else
            dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_SCI_LO(flow_index), reg_data);

        reg_data = 0;
        reg_data |= (rtk_uint32)(((rtk_uint32)(pMatch->sci[7]) & 0xff) << 24);
        reg_data |= (rtk_uint32)(((rtk_uint32)(pMatch->sci[6]) & 0xff) << 16);
        reg_data |= (rtk_uint32)(((rtk_uint32)(pMatch->sci[5]) & 0xff) << 8);
        reg_data |= (rtk_uint32)(((rtk_uint32)(pMatch->sci[4]) & 0xff));
        if (dir == MACSEC_DIRECTION_INGRESS)
            dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SCI_HI(flow_index), reg_data);
        else
            dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_SCI_HI(flow_index), reg_data);
    }

    {
        reg_data = 0;
        reg_data |= pMatch->matchMask;
        if (dir == MACSEC_DIRECTION_INGRESS)
            dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_MASK(flow_index), reg_data);
        else
            dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_MASK(flow_index), reg_data);
    }

    {
        reg_data = 0;
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->flow_index) & 0xff) << 16);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->vlanUpInner) & 0x7) << 12);
        reg_data |= (rtk_uint32)((((rtk_uint32)pMatch->vlanIdInner) & 0xfff));
        if (dir == MACSEC_DIRECTION_INGRESS)
            dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_EXT(flow_index), reg_data);
        else
            dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_EXT(flow_index), reg_data);
    }

    return ret;
}


rtk_api_ret_t dal_rtl8373_macsec_match_rule_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_macsec_matchrule_t *pMatch)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 reg_data = 0;
    rtk_uint16 tmp = 0;

    if (pMatch == NULL)
        return RT_ERR_INPUT;

    memset(pMatch, 0, sizeof(rtk_macsec_matchrule_t));

    if(dir == MACSEC_DIRECTION_INGRESS)
        dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_SA_LO(flow_index), &reg_data);
    else
        dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_SA_LO(flow_index), &reg_data);

    pMatch->mac_sa[3] = (rtk_uint8)((reg_data >> 24) & 0xff);
    pMatch->mac_sa[2] = (rtk_uint8)((reg_data >> 16) & 0xff);
    pMatch->mac_sa[1] = (rtk_uint8)((reg_data >> 8)  & 0xff);
    pMatch->mac_sa[0] = (rtk_uint8)((reg_data)       & 0xff);

    if(dir == MACSEC_DIRECTION_INGRESS)
        dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_SA_HI(flow_index), &reg_data);
    else
        dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_SA_HI(flow_index), &reg_data);
    tmp = (rtk_uint16)((reg_data >> 16) & 0xffff);
    pMatch->etherType = ((tmp & 0xFF) << 8) | ((tmp >> 8) & 0xFF);
    pMatch->mac_sa[5] = (rtk_uint8)((reg_data >> 8)  & 0xff);
    pMatch->mac_sa[4] = (rtk_uint8)((reg_data)       & 0xff);

    if(dir == MACSEC_DIRECTION_INGRESS)
        dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_DA_LO(flow_index), &reg_data);
    else
        dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_DA_LO(flow_index), &reg_data);
    pMatch->mac_da[3] = (rtk_uint8)((reg_data >> 24) & 0xff);
    pMatch->mac_da[2] = (rtk_uint8)((reg_data >> 16) & 0xff);
    pMatch->mac_da[1] = (rtk_uint8)((reg_data >> 8)  & 0xff);
    pMatch->mac_da[0] = (rtk_uint8)((reg_data)       & 0xff);

    if(dir == MACSEC_DIRECTION_INGRESS)
        dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_DA_HI(flow_index), &reg_data);
    else
        dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_DA_HI(flow_index), &reg_data);
    pMatch->vlan_id   = (rtk_uint16)((reg_data >> 16) & 0xfff);
    pMatch->mac_da[5] = (rtk_uint8)((reg_data >> 8)   & 0xff);
    pMatch->mac_da[4] = (rtk_uint8)((reg_data)        & 0xff);

    if(dir == MACSEC_DIRECTION_INGRESS)
        dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_MISC(flow_index), &reg_data);
    else
        dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_MISC(flow_index), &reg_data);
    pMatch->fVLANValid = (reg_data & 0x1) ? 1 : 0;
    pMatch->fQinQFound = (reg_data & 0x2) ? 1 : 0;
    pMatch->fSTagValid = (reg_data & 0x4) ? 1 : 0;
    pMatch->fQTagValid= (reg_data & 0x8) ? 1 : 0;

    pMatch->fControlPacket = (reg_data & 0x80) ? 1 : 0;
    pMatch->fUntagged      = (reg_data & 0x100) ? 1 : 0;
    pMatch->fTagged        = (reg_data & 0x200) ? 1 : 0;
    pMatch->fBadTag        = (reg_data & 0x400) ? 1 : 0;
    pMatch->fKayTag        = (reg_data & 0x800) ? 1 : 0;

    pMatch->macsec_TCI_AN = (rtk_uint8)((reg_data >> 24) & 0xff);
    pMatch->matchPriority = (rtk_uint8)((reg_data >> 16) & 0xf);
    pMatch->sourcePort =    (rtk_uint8)((reg_data >> 12) & 0x3);

    pMatch->vlanUserPriority = (rtk_uint8)((reg_data >> 4) & 0x7);

    if(dir == MACSEC_DIRECTION_INGRESS)
        dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_SCI_LO(flow_index), &reg_data);
    else
        dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_SCI_LO(flow_index), &reg_data);
    pMatch->sci[3] = (rtk_uint8)((reg_data >> 24) & 0xff);
    pMatch->sci[2] = (rtk_uint8)((reg_data >> 16) & 0xff);
    pMatch->sci[1] = (rtk_uint8)((reg_data >> 8)  & 0xff);
    pMatch->sci[0] = (rtk_uint8)((reg_data)       & 0xff);

    if(dir == MACSEC_DIRECTION_INGRESS)
        dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_SCI_HI(flow_index), &reg_data);
    else
        dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_SCI_HI(flow_index), &reg_data);
    pMatch->sci[7] = (rtk_uint8)((reg_data >> 24) & 0xff);
    pMatch->sci[6] = (rtk_uint8)((reg_data >> 16) & 0xff);
    pMatch->sci[5] = (rtk_uint8)((reg_data >> 8)  & 0xff);
    pMatch->sci[4] = (rtk_uint8)((reg_data)       & 0xff);

    if(dir == MACSEC_DIRECTION_INGRESS)
        dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_MASK(flow_index), &reg_data);
    else
        dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_MASK(flow_index), &reg_data);
    pMatch->matchMask = reg_data;

    if(dir == MACSEC_DIRECTION_INGRESS)
        dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_EXT(flow_index), &reg_data);
    else
        dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_EXT(flow_index), &reg_data);
    pMatch->flow_index  = (rtk_uint32) ((reg_data >> 16) & 0xff);
    pMatch->vlanUpInner = (rtk_uint8)  ((reg_data >> 12) & 0x7);
    pMatch->vlanIdInner = (rtk_uint16) ((reg_data) & 0xfff);

    return ret;
}

rtk_api_ret_t dal_rtl8373_macsec_match_rule_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index)
{
    rtk_api_ret_t  ret = RT_ERR_OK;
    rtk_uint32 reg_data = 0;

    if (dir == MACSEC_DIRECTION_INGRESS)
    {
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SA_LO(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SA_HI(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_DA_LO(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_DA_LO(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_MISC(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SCI_LO(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SCI_HI(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_MASK(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_EXT(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
    }
    else
    {
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_SA_LO(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_SA_HI(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_DA_LO(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_DA_HI(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_MISC(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_SCI_LO(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_SCI_HI(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_MASK(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_EXT(flow_index), reg_data);
        if(ret != RT_ERR_OK)
            return ret;
    }
    


    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_action_rule_set
 * Description:
 *      setup a flow control entry
 * Input:
 *      port          - port id
 *      flow_index    - table entry index
 *      pAct          - pointer to the struct for describe the rule
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_INPUT              - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_action_rule_set(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_macsec_action_t *pAct)
{
    rtk_api_ret_t  ret = RT_ERR_OK;
    rtk_uint32 sa_index = pAct->sa_index;
    rtk_uint8  flow_type = 0;
    rtk_uint8  dest_port = 0;
    rtk_uint8  drop_non_reserved = 0; //do not drop
    rtk_uint8  flow_crypt_auth = 0;
    rtk_uint8  drop_action = 0; // CRC
    rtk_uint8  protect = 0;
    rtk_uint8  sa_in_use = 0;
    rtk_uint8  include_sci = 0;
    rtk_uint8  validate_frames = 0;
    rtk_uint8  tag_bypass_size = 0;
    rtk_uint8  sa_index_update = 0;
    rtk_uint8  conf_offset = 0;
    rtk_uint8  conf_protect = 0;
    rtk_uint32 data = 0;

    if (flow_index >= RTK_MACSEC_ACTION_RULE_MAX )
    {
        rtlglue_printf("flow_index out of range!\n");
        return RT_ERR_INPUT;
    }
    if (pAct->flow_type == RTK_MACSEC_FLOW_EGRESS && dir == MACSEC_DIRECTION_INGRESS)
    {
        rtlglue_printf("flow_type mismatch!(set egress entry on ingress)\n");
        return RT_ERR_INPUT;
    }
    if (pAct->flow_type == RTK_MACSEC_FLOW_INGRESS && dir == MACSEC_DIRECTION_EGRESS)
    {
        rtlglue_printf("flow_type mismatch!(set ingress entry on egress)\n");
        return RT_ERR_INPUT;
    }

    dest_port = pAct->dest_port;
    switch (pAct->flow_type)
    {
        case RTK_MACSEC_FLOW_EGRESS:
            flow_type = 0b11;
            dest_port = RTK_MACSEC_PORT_COMMON;
            flow_crypt_auth = 0b0;
            protect = pAct->params.egress.protect_frame;
            sa_in_use = pAct->params.egress.sa_in_use;
            include_sci = pAct->params.egress.include_sci;
            validate_frames = pAct->params.egress.use_es | ( pAct->params.egress.use_scb << 1);
            tag_bypass_size = pAct->params.egress.tag_bypass_size;
            conf_offset = pAct->params.egress.confidentiality_offset;
            conf_protect = pAct->params.egress.conf_protect;
            break;

        case RTK_MACSEC_FLOW_INGRESS:
            flow_type = 0b10;
            dest_port = RTK_MACSEC_PORT_CONTROLLED;
            flow_crypt_auth = 0b0;
            protect = pAct->params.ingress.replay_protect;
            sa_in_use = pAct->params.ingress.sa_in_use;

            switch (pAct->params.ingress.validate_frames)
            {
               case RTK_MACSEC_VALIDATE_DISABLE:
                   validate_frames = 0b00;
                   break;
               case RTK_MACSEC_VALIDATE_CHECK:
                   validate_frames = 0b01;
                   break;
               case RTK_MACSEC_VALIDATE_STRICT:
                   validate_frames = 0b10;
                   break;
               default:
                   rtlglue_printf("unknown type of validate_frames!\n");
                   return RT_ERR_INPUT;
            }
            conf_offset = pAct->params.ingress.confidentiality_offset;
            break;

        case RTK_MACSEC_FLOW_BYPASS:
            flow_type = 0b00;
            flow_crypt_auth = 0b0;
            //sa_in_use = 1;
            sa_in_use = pAct->params.bypass_drop.sa_in_use;
            break;

        case RTK_MACSEC_FLOW_DROP:
            flow_type = 0b01;
            flow_crypt_auth = 0b0;
            //sa_in_use = 1;
            sa_in_use = pAct->params.bypass_drop.sa_in_use;
            break;

        default:
            return RT_ERR_INPUT;
    }

    if(drop_non_reserved)
        data |= 0x10;
    else
        data &= ~0x10;

    if(flow_crypt_auth)
        data |= 0x20;
    else
        data &= ~0x20;

    if(protect)
        data |= 0x10000;
    else
        data &= ~0x10000;

    if(sa_in_use)
        data |= 0x20000;
    else
        data &= ~0x20000;

    if(include_sci)
        data |= 0x40000;
    else
        data &= ~0x40000;

    if(sa_index_update)
        data |= 0x800000;
    else
        data &= ~0x800000;

    if(conf_protect)
        data |= 0x80000000;
    else
        data &= ~0x80000000;

    data |= (rtk_uint32)((((rtk_uint32)conf_offset)     & 0x7f) << 24);
    data |= (rtk_uint32)((((rtk_uint32)tag_bypass_size)  & 0x3) << 21);
    data |= (rtk_uint32)((((rtk_uint32)validate_frames) & 0x3) << 19);
    data |= (rtk_uint32)((((rtk_uint32)sa_index)        & 0xff) << 8);
    data |= (rtk_uint32)((((rtk_uint32)drop_action)     & 0x3) << 6);
    data |= (rtk_uint32)((((rtk_uint32)dest_port)       & 0x3) << 2);
    data |= (rtk_uint32)((((rtk_uint32)flow_type)       & 0x3));

    if (dir == MACSEC_DIRECTION_INGRESS)
    {
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_FLOW_CTRL(flow_index), data);
    }
    else
    {
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_FLOW_CTRL(flow_index), data);
    }

    return ret;
}


rtk_api_ret_t dal_rtl8373_macsec_action_rule_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_macsec_action_t *pAct)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 reg_data = 0;
    rtk_uint8 flow_type = 0;

    if(pAct == NULL)
        return RT_ERR_INPUT;

    memset(pAct, 0, sizeof(rtk_macsec_action_t));

    if (dir == MACSEC_DIRECTION_INGRESS)
    {
        ret = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_FLOW_CTRL(flow_index), &reg_data);
        if (ret != RT_ERR_OK)
            return ret;
    }
    else
    {
        ret = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_FLOW_CTRL(flow_index), &reg_data);
        if (ret != RT_ERR_OK)
            return ret;
    }

    pAct->dest_port = (reg_data >> 2) & 0x3;

    pAct->sa_index  = (reg_data >> 8) & 0xff;

    flow_type = reg_data & 0x3;
    switch (flow_type)
    {
        case 0b11:
            pAct->flow_type = RTK_MACSEC_FLOW_EGRESS;
            pAct->params.egress.protect_frame          = (reg_data >> 16) & 0x1;
            pAct->params.egress.sa_in_use              = (reg_data >> 17) & 0x1;
            pAct->params.egress.include_sci            = (reg_data >> 18) & 0x1;
            pAct->params.egress.use_es                 = (reg_data >> 19) & 0x1;
            pAct->params.egress.use_scb                = (reg_data >> 20) & 0x1;
            pAct->params.egress.tag_bypass_size        = (reg_data >> 21) & 0x3;
            pAct->params.egress.sa_index_update_by_hw  = (reg_data >> 23) & 0x3;
            pAct->params.egress.confidentiality_offset = (reg_data >> 24) & 0x7f;
            pAct->params.egress.conf_protect           = (reg_data >> 31) & 0x1;
            break;
        case 0b10:
            pAct->flow_type = RTK_MACSEC_FLOW_INGRESS;
            pAct->params.ingress.replay_protect         = (reg_data >> 16) & 0x1;
            switch ((reg_data >> 19) & 0x3)
            {
                case 0b00:
                    pAct->params.ingress.validate_frames = RTK_MACSEC_VALIDATE_DISABLE;
                    break;
                case 0b01:
                    pAct->params.ingress.validate_frames = RTK_MACSEC_VALIDATE_CHECK;
                    break;
                case 0b10:
                    pAct->params.ingress.validate_frames = RTK_MACSEC_VALIDATE_STRICT;
                    break;
                default:
                    return RT_ERR_FAILED;
            }

            pAct->params.ingress.confidentiality_offset = (reg_data >> 24) & 0x7f;
            break;
        case 0b01:
        case 0b00:
        default:
            pAct->flow_type = (flow_type == 0b01) ? RTK_MACSEC_FLOW_DROP : RTK_MACSEC_FLOW_BYPASS;
            pAct->params.bypass_drop.sa_in_use = (reg_data >> 17) & 0x1;
            break;
    }

    return ret;
}


rtk_api_ret_t dal_rtl8373_macsec_action_rule_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index)
{
    rtk_api_ret_t ret = RT_ERR_OK;

    if (dir == MACSEC_DIRECTION_INGRESS)
    {
        ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_FLOW_CTRL(flow_index), 0);
    }
    else
    {
        ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_FLOW_CTRL(flow_index), 0);
    }


    return ret;
}


rtk_api_ret_t dal_rtl8373_macsec_flow_enable_set(rtk_uint32 port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_enable_t enable)
{

    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32       reg_data = 0;
    rtk_uint32 reg_offset, busycnt;
    rtk_enable_t cur_ena;

    reg_offset = (flow_index/32)*4;

    if ((ret = dal_rtl8373_macsec_flow_enable_get(port, dir, flow_index, &cur_ena)) != RT_ERR_OK)
    {
        return ret;
    }

    if (enable != cur_ena)
    {
        if(enable == ENABLED)
        {
            if (dir == MACSEC_DIRECTION_INGRESS)
            {
                ret = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_ENTRY_SET+reg_offset, &reg_data);
                if (ret != RT_ERR_OK)
                    return ret;

                reg_data |= BIT_0_IN32 << (flow_index % 32);
                ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_ENTRY_SET+reg_offset, reg_data);
            }
            else
            {
                ret = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_ENTRY_SET+reg_offset, &reg_data);
                if (ret != RT_ERR_OK)
                    return ret;

                reg_data |= BIT_0_IN32 << (flow_index % 32);
                ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_ENTRY_SET+reg_offset, reg_data);
            }

        }
        else
        {
            if (dir == MACSEC_DIRECTION_INGRESS)
            {
                ret = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_ENTRY_CLEAR+reg_offset, &reg_data);
                if (ret != RT_ERR_OK)
                    return ret;

                reg_data |= BIT_0_IN32 << (flow_index % 32);
                ret = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_ENTRY_CLEAR+reg_offset, reg_data);
            }
            else
            {
                ret = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_ENTRY_CLEAR+reg_offset, &reg_data);
                if (ret != RT_ERR_OK)
                    return ret;

                reg_data |= BIT_0_IN32 << (flow_index % 32);
                ret = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_SAM_ENTRY_CLEAR+reg_offset, reg_data);
            }

        }   

        busycnt = 1000;

        if(enable == DISABLED)
        {
            while(busycnt)
            {
                if (dir == MACSEC_DIRECTION_INGRESS)
                {
                    ret = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_IN_FLIGHT, &reg_data);
                    if (ret != RT_ERR_OK)
                        return ret;
                }
                else
                {
                    ret = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_IN_FLIGHT, &reg_data);
                    if (ret != RT_ERR_OK)
                        return ret;
                }

                if ((reg_data & 0x3F) == 0)
                    break;

                busycnt--;

            }

            if (busycnt == 0)
                rtlglue_printf("flow delete timeout/n");

            return RT_ERR_BUSYWAIT_TIMEOUT;
            
        }
    
    }

    return ret;
    
}


rtk_api_ret_t dal_rtl8373_macsec_flow_enable_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 flow_index, rtk_enable_t *pEnable)
{
    rtk_api_ret_t  ret = RT_ERR_OK;
    rtk_uint32 reg_data, reg_offset;


    reg_offset = (flow_index/32)*4;

    if (dir == MACSEC_DIRECTION_INGRESS)
    {
        ret = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_SAM_ENTRY_ENABLE+reg_offset, &reg_data);
    }
    else
    {
        ret = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_SAM_ENTRY_ENABLE+reg_offset, &reg_data);
    }

    *pEnable = (reg_data & (BIT_0_IN32 << (flow_index % 32))) ? ENABLED : DISABLED;

    return ret;
}



/* Function Name:
 *      dal_rtl8373_macsec_port_cfg_set
 * Description:
 *      Set per-port configurations for MACsec
 * Input:
 *      port     - port id
 *      pPortcfg - pointer to macsec port configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_port_cfg_set(rtk_port_t port, rtk_macsec_port_cfg_t *pPortcfg)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 data = 0;
    rtk_uint32 xpn_thr_h = 0, xpn_thr_l = 0;
    rtk_uint8 drop_kay = 0;

    if (pPortcfg == NULL)
        return RT_ERR_INPUT;

    if(pPortcfg->pn_intr_threshold > 0xFFFFFFFF)
        return RT_ERR_INPUT;

    if(pPortcfg->xpn_intr_threshold > 0xFFFFFFFFFFFFFFFF)
        return RT_ERR_INPUT;


    if (pPortcfg->flags & RTK_MACSEC_PORT_F_ENABLE)
    {
        ret = dal_rtl8373_macsec_rxbypass_set(port, 0);
        if (ret != RT_ERR_OK)
            return ret;

        ret = dal_rtl8373_macsec_txbypass_set(port, 0);
        if (ret != RT_ERR_OK)
            return ret;

    }

    ret = dal_rtl8373_macsec_ingress_get(port, 0x797C, &data);
    if (ret != RT_ERR_OK)
        return ret;
    data &= (~(BIT_11_IN32 | BIT_10_IN32));
    switch (pPortcfg->nm_validate_frames)
    {
        case RTK_MACSEC_VALIDATE_STRICT:
            data |= BIT_11_IN32;
            break;
        case RTK_MACSEC_VALIDATE_CHECK:
            data |= BIT_10_IN32;
            break;
        case RTK_MACSEC_VALIDATE_DISABLE:
        default:
            break;
    }
    ret = dal_rtl8373_macsec_ingress_set(port, 0x797C, data);
    if (ret != RT_ERR_OK)
        return ret;

    drop_kay = (pPortcfg->flags & RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY) ? (1) : (0);
    ret = dal_rtl8373_macsec_egress_get(port, 0x7944, &data);
    if (ret != RT_ERR_OK)
        return ret;
    data &= (~BIT_24_IN32);
    data |= (drop_kay) ? (BIT_24_IN32) : (0);
    ret = dal_rtl8373_macsec_egress_set(port, 0x7944, data);
    if (ret != RT_ERR_OK)
        return ret;

    ret = dal_rtl8373_macsec_ingress_get(port, 0x7944, &data);
    if (ret != RT_ERR_OK)
        return ret;
    data &= (~BIT_24_IN32);
    data |= (drop_kay) ? (BIT_24_IN32) : (0);
    ret = dal_rtl8373_macsec_ingress_set(port, 0x7944, data);
    if (ret != RT_ERR_OK)
        return ret;

    xpn_thr_h = (rtk_uint32)(pPortcfg->xpn_intr_threshold >> 32);
    xpn_thr_l = (rtk_uint32)(pPortcfg->xpn_intr_threshold & 0xFFFFFFFF);


    ret = dal_rtl8373_macsec_egress_set(port, 0xF420, pPortcfg->pn_intr_threshold);
    if (ret != RT_ERR_OK)
        return ret;

    ret = dal_rtl8373_macsec_egress_set(port, 0xF424, xpn_thr_l);
    if (ret != RT_ERR_OK)
        return ret;

    ret = dal_rtl8373_macsec_egress_set(port, 0xF428, xpn_thr_h);
    if (ret != RT_ERR_OK)
        return ret;

    return ret;
}



/* Function Name:
 *      dal_rtl8373_macsec_port_cfg_get
 * Description:
 *      Get per-port configurations for MACsec
 * Input:
 *      port     - port id
 * Output:
 *      pPortcfg - pointer to macsec port configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_port_cfg_get(rtk_port_t port,rtk_macsec_port_cfg_t *pPortcfg)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 data = 0, enablerx=0, enabletx = 0;
    rtk_uint64 xpn_thr = 0;

    if (pPortcfg == NULL)
        return RT_ERR_INPUT;
    memset(pPortcfg, 0x0, sizeof(rtk_macsec_port_cfg_t));


    ret = dal_rtl8373_macsec_rxbypass_get(port, &enablerx);
    if (ret != RT_ERR_OK)
        return ret;

    ret = dal_rtl8373_macsec_txbypass_get(port, &enabletx);
    if (ret != RT_ERR_OK)
        return ret;
    
    data = enablerx & enabletx;
    
    if (data == 0)
        pPortcfg->flags |= RTK_MACSEC_PORT_F_ENABLE;

    ret = dal_rtl8373_macsec_ingress_get(port, 0x7944, &data);
    if (ret != RT_ERR_OK)
        return ret;
    if (data & BIT_24_IN32)
        pPortcfg->flags |= RTK_MACSEC_PORT_F_DROP_NONCTRL_KAY;

    ret = dal_rtl8373_macsec_ingress_get(port, 0x797C, &data);
    if (ret != RT_ERR_OK)
        return ret;
    switch ((data & 0xC00) >> 10)
    {
        case 0b10:
            pPortcfg->nm_validate_frames = RTK_MACSEC_VALIDATE_STRICT;
            break;
        case 0b01:
            pPortcfg->nm_validate_frames = RTK_MACSEC_VALIDATE_CHECK;
            break;
        case 0b00:
        default:
            pPortcfg->nm_validate_frames = RTK_MACSEC_VALIDATE_DISABLE;
            break;
    }


    ret = dal_rtl8373_macsec_egress_get(port, 0xF420, &data);
    if (ret != RT_ERR_OK)
        return ret;
    pPortcfg->pn_intr_threshold = data;

    ret = dal_rtl8373_macsec_egress_get(port, 0xF424, &data);
    if (ret != RT_ERR_OK)
        return ret;
    xpn_thr = (rtk_uint64) data;
    ret = dal_rtl8373_macsec_egress_get(port, 0xF428, &data);
    if (ret != RT_ERR_OK)
        return ret;
    xpn_thr |= ((rtk_uint64) data) << 32;
    pPortcfg->xpn_intr_threshold =xpn_thr;

    return ret;
}





/* Function Name:
 *      dal_rtl8373_macsec_sc_create
 * Description:
 *      Create a MACsec Secure Channel
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      pSc      - pointer to macsec sc configuration structure
 * Output:
 *      pSc_id   - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_EXCEEDS_CAPACITY
 * Note:
 *      None
 */

rtk_api_ret_t dal_rtl8373_macsec_sc_create(rtk_port_t port, rtk_macsec_dir_t dir, rtk_macsec_sc_t *pSc, rtk_uint32 *pSc_id)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint8 an = 0, tci_an = 0;
    rtk_uint32 i = 0, sc_id = 0xFFFFFFFF;
    rtk_uint32 flow_base = 0;
    rtk_macsec_cipher_t cs = RTK_MACSEC_CIPHER_GCM_ASE_128;
    rtk_macsec_action_t flow;
    rtk_macsec_matchrule_t match;
    rtl8373_macsec_sa_params_t hwsa;

    if (pSc == NULL || pSc_id == NULL)
        return RT_ERR_INPUT;

    for (i = 0; i < MACSEC_SC_MAX(port); i++)
    {
        if(MACSEC_SC_IS_CLEAR(port, dir, i))
        {
            sc_id = i;
            break;
        }
    }
    if ( sc_id == 0xFFFFFFFF )
    {
        rtlglue_printf("no empty sc entry\n");
        return RT_ERR_TBL_FULL;
    }

    flow_base = RTL8373_MACSEC_HW_FLOW_ID(sc_id);

    memset(&flow, 0, sizeof(rtk_macsec_action_t));
    memset(&match, 0, sizeof(rtk_macsec_matchrule_t));
    memset(&hwsa, 0, sizeof(rtl8373_macsec_sa_params_t));

    cs = (dir == MACSEC_DIRECTION_EGRESS) ? (pSc->tx.cipher_suite) : (pSc->rx.cipher_suite);
    switch (cs)
    {
        case RTK_MACSEC_CIPHER_GCM_ASE_128:
            hwsa.flags = 0;
            hwsa.key_bytes = 16;
            if(pSc->rx.replay_window > 0xFFFFFFFF)
            {
                rtlglue_printf("PN replay_window %u out of range 0~%u", pSc->rx.replay_window, (0xFFFFFFFF));
                return RT_ERR_INPUT;
            }
            break;
        case RTK_MACSEC_CIPHER_GCM_ASE_256:
            hwsa.flags = 0;
            hwsa.key_bytes = 32;
            if(pSc->rx.replay_window > 0xFFFFFFFF)
            {
                rtlglue_printf("PN replay_window %u out of range 0~%u", pSc->rx.replay_window, (0xFFFFFFFF));
                return RT_ERR_INPUT;
            }
            break;
        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_128:
            hwsa.flags = RTL8373_MACSEC_SA_FLAG_XPN;
            hwsa.key_bytes = 16;
            if(pSc->rx.replay_window > 0x40000000)
            {
                rtlglue_printf("XPN replay_window %u out of range 0~%u", pSc->rx.replay_window, (0x40000000));
                return RT_ERR_INPUT;
            }
            break;
        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_256:
            hwsa.flags = RTL8373_MACSEC_SA_FLAG_XPN;
            hwsa.key_bytes = 32;
            if(pSc->rx.replay_window > 0x40000000)
            {
                rtlglue_printf("XPN replay_window %u out of range 0~%u", pSc->rx.replay_window, (0x40000000));
                return RT_ERR_INPUT;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    if (dir == MACSEC_DIRECTION_EGRESS)
    {
        /* match rule */
        match.fUntagged = 1;
        match.fControlPacket = 0;
        switch (pSc->tx.flow_match)
        {
            case RTK_MACSEC_MATCH_NON_CTRL:
                match.matchMask = MACSEC_SA_MATCH_MASK_CTRL_PKT;
                break;

            case RTK_MACSEC_MATCH_MAC_DA:
                memcpy(match.mac_da, pSc->tx.mac_da.octet, 6 * sizeof(rtk_uint8));
                match.matchMask = (MACSEC_SA_MATCH_MASK_MAC_DA_FULL
                    | MACSEC_SA_MATCH_MASK_CTRL_PKT);
                break;

            default:
                return RT_ERR_INPUT;
        }
        match.flow_index = flow_base;
        ret = dal_rtl8373_macsec_match_rule_set(port, dir, flow_base, &match);
        if (ret != RT_ERR_OK)
            return ret;
        MACSEC_SC_MATCH(port, dir, sc_id) = pSc->tx.flow_match;

        /* flow ctrl */
        flow.flow_type = RTK_MACSEC_FLOW_EGRESS;
        flow.dest_port = RTK_MACSEC_PORT_COMMON;
        flow.sa_index = RTL8373_MACSEC_HW_SA_ID(sc_id, 0);
        flow.params.egress.protect_frame = pSc->tx.protect_frame;
        flow.params.egress.sa_in_use = 0;
        flow.params.egress.include_sci = pSc->tx.include_sci;
        flow.params.egress.use_es = pSc->tx.use_es;
        flow.params.egress.use_scb = pSc->tx.use_scb;
        flow.params.egress.confidentiality_offset = 0;
        flow.params.egress.conf_protect = pSc->tx.conf_protect;

        ret = dal_rtl8373_macsec_action_rule_set(port, dir, flow_base, &flow);
        if (ret != RT_ERR_OK)
            return ret;

        /* TC */
        hwsa.direction = dir;
        hwsa.flow_index = flow_base;
        hwsa.sa_expired_irq = 1;
        hwsa.update_en = 1;
        hwsa.next_sa_valid = 0;
        memcpy(hwsa.sci, pSc->tx.sci, 8 * sizeof(rtk_uint8));
        for (an = 0; an < 4; an++)
        {
            hwsa.an = an;
            hwsa.next_sa_index = RTL8373_MACSEC_HW_SA_ID(sc_id, ((an + 1) % 4));
            ret = dal_rtl8373_macsec_hw_sa_set(port, dir, RTL8373_MACSEC_HW_SA_ID(sc_id, an), &hwsa);
        }
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        /* match rule */
        match.fTagged = 1;
        match.fControlPacket = 0;
        memcpy(match.sci, pSc->rx.sci, 8 * sizeof(rtk_uint8));
        switch (pSc->rx.flow_match)
        {
            case RTK_MACSEC_MATCH_SCI:
                tci_an = 0x20;
                match.matchMask = (MACSEC_SA_MATCH_MASK_MACSEC_SCI
                    | MACSEC_SA_MATCH_MASK_MACSEC_TCI_AN_SC
                    | MACSEC_SA_MATCH_MASK_CTRL_PKT);
                break;

            case RTK_MACSEC_MATCH_MAC_SA:
                tci_an = 0x0;
                memcpy(match.mac_sa, pSc->rx.mac_sa.octet, 6 * sizeof(rtk_uint8));
                match.matchMask = (MACSEC_SA_MATCH_MASK_MAC_SA_FULL
                    | MACSEC_SA_MATCH_MASK_MACSEC_TCI_AN_SC
                    | MACSEC_SA_MATCH_MASK_CTRL_PKT);
                break;

            default:
                return RT_ERR_INPUT;
        }
        for (an = 0; an < 4; an++)
        {
            match.macsec_TCI_AN = an | tci_an;
            match.flow_index = flow_base + an;
            ret = dal_rtl8373_macsec_match_rule_set(port, dir, (flow_base + an), &match);
            if (ret != RT_ERR_OK)
                return ret;
        }
        MACSEC_SC_MATCH(port, dir, sc_id) = pSc->rx.flow_match;

        /* flow ctrl */
        flow.flow_type = RTK_MACSEC_FLOW_INGRESS;
        flow.dest_port = RTK_MACSEC_PORT_CONTROLLED;

        flow.params.ingress.replay_protect = pSc->rx.replay_protect;
        flow.params.ingress.sa_in_use = 0;
        flow.params.ingress.validate_frames = pSc->rx.validate_frames;
        flow.params.ingress.confidentiality_offset = 0;

        for (an = 0; an < 4; an++)
        {
            flow.sa_index = RTL8373_MACSEC_HW_SA_ID(sc_id, an);
            ret = dal_rtl8373_macsec_action_rule_set(port, dir, (flow_base + an), &flow);
            if (ret != RT_ERR_OK)
                return ret;
        }

        /* TC */
        hwsa.direction = dir;
        hwsa.replay_window = pSc->rx.replay_window;
        memcpy(hwsa.sci, pSc->rx.sci, 8 * sizeof(rtk_uint8));
        for (an = 0; an < 4; an++)
        {
            ret = dal_rtl8373_macsec_hw_sa_set(port, dir, RTL8373_MACSEC_HW_SA_ID(sc_id, an), &hwsa);
            if (ret != RT_ERR_OK)
                return ret;
        }
    }
    MACSEC_SC_CS(port, dir, sc_id) = cs;
    MACSEC_SC_SET_USED(port, dir, sc_id);
    *pSc_id = sc_id;
    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_sc_get
 * Description:
 *      Get configuration info for a created Secure Channel
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_index - pointer to the created SC id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_NOTFOUND
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_sc_get(rtk_port_t port, rtk_macsec_dir_t dir,rtk_uint32 sc_id, rtk_macsec_sc_t *pSc)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 flow_base = 0, sa_base = 0;
    rtk_macsec_action_t flow;
    rtk_macsec_matchrule_t match;
    rtl8373_macsec_sa_params_t hwsa;

    if (pSc == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;

    if (MACSEC_SC_IS_CLEAR(port, dir, sc_id))
    {
        rtlglue_printf("%s SC %u is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }
    memset(pSc, 0, sizeof(rtk_macsec_sc_t));

    flow_base = RTL8373_MACSEC_HW_FLOW_ID(sc_id);
    sa_base = RTL8373_MACSEC_HW_SA_ID(sc_id, 0);

    ret = dal_rtl8373_macsec_action_rule_get(port, dir, flow_base, &flow);
    if (ret != RT_ERR_OK)
        return ret;
    ret = dal_rtl8373_macsec_hw_sa_get(port, dir, sa_base, &hwsa);
    if (ret != RT_ERR_OK)
        return ret;

    if (dir == MACSEC_DIRECTION_EGRESS)
    {
        pSc->tx.cipher_suite = MACSEC_SC_CS(port, dir, sc_id);
        pSc->tx.flow_match = MACSEC_SC_MATCH(port, dir, sc_id);
        if (RTK_MACSEC_MATCH_MAC_DA == pSc->tx.flow_match)
        {
            ret = dal_rtl8373_macsec_match_rule_get(port, dir, flow_base, &match);
            if (ret != RT_ERR_OK)
                return ret;
            memcpy(pSc->tx.mac_da.octet, match.mac_da, 6 * sizeof(rtk_uint8));
        }

        pSc->tx.protect_frame = flow.params.egress.protect_frame;
        pSc->tx.include_sci = flow.params.egress.include_sci;
        pSc->tx.use_es = flow.params.egress.use_es;
        pSc->tx.use_scb = flow.params.egress.use_scb;
        pSc->tx.conf_protect = flow.params.egress.conf_protect;

        memcpy(pSc->tx.sci, hwsa.sci, 8 * sizeof(rtk_uint8));
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        pSc->rx.cipher_suite = MACSEC_SC_CS(port, dir, sc_id);
        pSc->rx.flow_match = MACSEC_SC_MATCH(port, dir, sc_id);
        ret = dal_rtl8373_macsec_match_rule_get(port, dir, flow_base, &match);
        if (ret != RT_ERR_OK)
            return ret;
        if (RTK_MACSEC_MATCH_MAC_SA == pSc->rx.flow_match)
        {
            memcpy(pSc->rx.mac_sa.octet, match.mac_sa, 6 * sizeof(rtk_uint8));
        }
        memcpy(pSc->rx.sci, match.sci, 8 * sizeof(rtk_uint8));

        pSc->rx.replay_protect = flow.params.ingress.replay_protect;
        pSc->rx.validate_frames = flow.params.ingress.validate_frames;

        pSc->rx.replay_window = hwsa.replay_window;
    }
    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_sc_del
 * Description:
 *      Delete a Secure Channel
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_sc_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint8 an = 0;
    rtk_uint32 flow_base = 0, flow_id = 0;

    if (sc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;

    if (MACSEC_SC_IS_CLEAR(port, dir, sc_id))
        return RT_ERR_OK;

    flow_base = RTL8373_MACSEC_HW_FLOW_ID(sc_id);

    if (dir == MACSEC_DIRECTION_EGRESS)
    {
        flow_id = flow_base;

        ret = dal_rtl8373_macsec_flow_enable_set(port, dir, flow_id, DISABLED);
        if (ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_match_rule_del(port, dir, flow_id);
        if (ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_action_rule_del(port, dir, flow_id);
        if (ret != RT_ERR_OK)
            return ret;

        for (an = 0; an < 4; an++)
        {
            ret = dal_rtl8373_macsec_hw_sa_del(port,dir,RTL8373_MACSEC_HW_SA_ID(sc_id, an));
            MACSEC_SA_UNSET_USED(port, dir, RTL8373_MACSEC_HW_SA_ID(sc_id, an));
        }
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        for (an = 0; an < 4; an++)
        {
            flow_id = flow_base + an;
            ret = dal_rtl8373_macsec_flow_enable_set(port, dir, flow_id, DISABLED);
            ret = dal_rtl8373_macsec_match_rule_del(port, dir, flow_id);
            ret = dal_rtl8373_macsec_action_rule_del(port, dir, flow_id);
            ret = dal_rtl8373_macsec_hw_sa_del(port, dir, RTL8373_MACSEC_HW_SA_ID(sc_id, an));
            MACSEC_SA_UNSET_USED(port, dir, RTL8373_MACSEC_HW_SA_ID(sc_id, an));
        }
    }

    MACSEC_SC_UNSET_USED(port, dir, sc_id);
    return ret;
}

/* Function Name:
 *      dal_rtl8373_macsec_sc_status_get
 * Description:
 *      Get hardware status for a Secure Channel
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 * Output:
 *      pSc_status - pointer to macsec SC status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_sc_status_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_sc_status_t *pSc_status)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_macsec_action_t flow;
    rtk_uint32 flow_base = 0, flow_id = 0;
    rtk_uint32 flow_reg = 0, flow_data = 0;
    rtk_enable_t ena = DISABLED;
    rtk_macsec_an_t an;

    if (pSc_status == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(port, dir, sc_id))
    {
        rtlglue_printf("%s SC %u is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }
    memset(pSc_status, 0, sizeof(rtk_macsec_sc_status_t));

    flow_base = RTL8373_MACSEC_HW_FLOW_ID(sc_id);
    ret = dal_rtl8373_macsec_action_rule_get(port, dir, flow_base, &flow);
    if (ret != RT_ERR_OK)
        return ret;

    if (dir == MACSEC_DIRECTION_EGRESS)
    {
        ret = dal_rtl8373_macsec_action_rule_get(port, dir, flow_base, &flow);
        ret = dal_rtl8373_macsec_flow_enable_get(port, dir, flow_base, &ena);
        pSc_status->tx.hw_flow_index = flow_base;
        pSc_status->tx.hw_sa_index = flow.sa_index;
        pSc_status->tx.sa_inUse = flow.params.egress.sa_in_use;
        pSc_status->tx.hw_sc_flow_status = (ena == ENABLED) ? 1 : 0;
        pSc_status->tx.running_an = RTL8373_MACSEC_HW_SA_TO_AN(flow.sa_index);

        flow_reg = RTL8373_MACSEC_SAM_FLOW_CTRL(flow_base);
        ret = dal_rtl8373_macsec_egress_get(port, flow_reg, &flow_data);
        if (ret != RT_ERR_OK)
            return ret;
        pSc_status->tx.hw_flow_data = flow_data;
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        pSc_status->rx.hw_flow_base = flow_base;
        for (an = RTK_MACSEC_AN0; an < RTK_MACSEC_AN_MAX; an++)
        {
            flow_id = flow_base + an;
            ret = dal_rtl8373_macsec_action_rule_get(port, dir, flow_id, &flow);
            ret = dal_rtl8373_macsec_flow_enable_get(port, dir, flow_id, &ena);
            pSc_status->rx.hw_sa_index[an] = flow.sa_index;
            pSc_status->rx.sa_inUse[an] = flow.params.ingress.sa_in_use;
            pSc_status->rx.hw_sc_flow_status[an] = (ena == ENABLED) ? 1 : 0;

            flow_reg = RTL8373_MACSEC_SAM_FLOW_CTRL(flow_id);
            ret = dal_rtl8373_macsec_ingress_get(port, flow_reg, &flow_data);
            if (ret != RT_ERR_OK)
                return ret;
            pSc_status->rx.hw_flow_data[an] = flow_data;
        }
    }

    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_sa_activate
 * Description:
 *      Activate a MACsec Secure Association
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - Secure Channel id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      For egress, this function will change running SA.
 */
rtk_api_ret_t dal_rtl8373_macsec_sa_activate(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 sa_id = RTL8373_MACSEC_HW_SA_ID(sc_id, an);
    rtk_uint32 flow_id = 0;
    rtk_macsec_action_t flow;

    if (sc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(port, dir, sc_id))
    {
        rtlglue_printf("%s SC %u is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(port, dir, sa_id))
    {
        rtlglue_printf("%s SA(SC %u, AN %u) is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX", sc_id, an);
        return RT_ERR_MACSEC_SA_NOTFOUND;
    }

    if (dir == MACSEC_DIRECTION_EGRESS)
    {
        flow_id = RTL8373_MACSEC_HW_FLOW_ID(sc_id);
        ret = dal_rtl8373_macsec_action_rule_get(port, dir, flow_id, &flow);
        if (ret != RT_ERR_OK)
            return ret;
        flow.sa_index = sa_id;
        flow.params.egress.sa_in_use = 1;
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        flow_id = RTL8373_MACSEC_HW_FLOW_ID(sc_id) + an;
        ret = dal_rtl8373_macsec_action_rule_get(port, dir, flow_id, &flow);
        if (ret != RT_ERR_OK)
            return ret;
        flow.sa_index = sa_id;
        flow.params.ingress.sa_in_use = 1;
    }

    ret = dal_rtl8373_macsec_action_rule_set(port, dir, flow_id, &flow);
    if (ret != RT_ERR_OK)
        return ret;
    ret = dal_rtl8373_macsec_flow_enable_set(port, dir, flow_id, ENABLED);
    
    return ret;
}

/* Function Name:
 *      dal_rtl8373_macsec_rxsa_disable
 * Description:
 *      Disable a ingress MACsec Secure Association (inUse = 0)
 * Input:
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_rxsa_disable(rtk_port_t port, rtk_uint32 rxsc_id, rtk_macsec_an_t an)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_macsec_dir_t dir = MACSEC_DIRECTION_INGRESS;
    rtk_uint32 sa_id = RTL8373_MACSEC_HW_SA_ID(rxsc_id, an);
    rtk_uint32 flow_id = RTL8373_MACSEC_HW_FLOW_ID(rxsc_id) + an;
    rtk_macsec_action_t flow;

    if (rxsc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(port, dir, rxsc_id))
    {
        rtlglue_printf("%s SC %u is not existed!", (MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX" ,rxsc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(port, dir, rxsc_id))
    {
        rtlglue_printf("%s SA(SC %u, AN %u) is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX", rxsc_id, an);
        return RT_ERR_MACSEC_SA_NOTFOUND;
    }

    ret = dal_rtl8373_macsec_action_rule_get(port, dir, flow_id, &flow);
    if (ret != RT_ERR_OK)
        return ret;
    flow.params.ingress.sa_in_use = 0;
    ret = dal_rtl8373_macsec_action_rule_set(port, dir, flow_id, &flow);

    return ret;
}

/* Function Name:
 *      dal_rtl8373_macsec_txsa_disable
 * Description:
 *      Disable the running egress MACsec Secure Association (inUse = 0)
 * Input:
 *      port     - port id
 *      txsc_id  - egress SC id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_txsa_disable(rtk_port_t port, rtk_uint32 txsc_id)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_macsec_dir_t dir = MACSEC_DIRECTION_EGRESS;
    rtk_uint32 flow_id = RTL8373_MACSEC_HW_FLOW_ID(txsc_id);
    rtk_macsec_action_t flow;

    if (txsc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;

    if (MACSEC_SC_IS_CLEAR(port, dir, txsc_id))
    {
        rtlglue_printf("%s SC %u is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX",txsc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }

    ret = dal_rtl8373_macsec_action_rule_get(port, dir, flow_id, &flow);
    if (ret != RT_ERR_OK)
        return ret;
    flow.params.egress.sa_in_use = 0;
    ret = dal_rtl8373_macsec_action_rule_set(port, dir, flow_id, &flow);

    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_sa_create
 * Description:
 *      Create a MACsec Secure Association
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 *      pSa      - pointer to macsec SA configuration structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_sa_create(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtl8373_macsec_sa_params_t hwsa;
    rtk_macsec_cipher_t cs;
    rtk_macsec_sc_status_t sc_status;
    rtk_uint32 sa_id = RTL8373_MACSEC_HW_SA_ID(sc_id, an);
    rtk_uint32 post_sa_id = RTL8373_MACSEC_HW_SA_ID(sc_id, ((an + 3) % 4));
    rtk_uint8 flow_state_recover = 0;
    rtk_uint32 flow_id = 0;

    if (pSa == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(port))
        return RT_ERR_INPUT;

    if (MACSEC_SC_IS_CLEAR(port, dir, sc_id))
    {
        rtlglue_printf("%s SC %u is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }

    cs = MACSEC_SC_CS(port, dir, sc_id);
    switch (cs)
    {
        case RTK_MACSEC_CIPHER_GCM_ASE_128:
        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_128:
            if (pSa->key_bytes != 16)
            {
                rtlglue_printf("Bad key_bytes:%u for AES-128.", pSa->key_bytes);
                return RT_ERR_INPUT;
            }
            break;

        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_256:
        case RTK_MACSEC_CIPHER_GCM_ASE_256:
            if (pSa->key_bytes != 32)
            {
                rtlglue_printf("Bad key_bytes:%u for AES-256.", pSa->key_bytes);
                return RT_ERR_INPUT;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

    ret = dal_rtl8373_macsec_sc_status_get(port, dir, sc_id, &sc_status);
    if (ret != RT_ERR_OK)
        return ret;
    if (dir == MACSEC_DIRECTION_EGRESS)
    {
        /* disable flow for running AN */
        if ((sc_status.tx.hw_sc_flow_status == 1) && (sc_status.tx.running_an == an))
        {
            ret = dal_rtl8373_macsec_txsa_disable(port, sc_id);
            if (ret != RT_ERR_OK)
                return ret;
            flow_state_recover = 1;
        }

    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        if (sc_status.rx.hw_sc_flow_status[an] == 1)
        {
            ret = dal_rtl8373_macsec_rxsa_disable(port, sc_id, an);
            if (ret != RT_ERR_OK)
                return ret;
            flow_state_recover = 1;
        }
    }

    ret = dal_rtl8373_macsec_hw_sa_get(port, dir, sa_id, &hwsa);
    if (ret != RT_ERR_OK)
        return ret;
    switch (cs)
    {
        case RTK_MACSEC_CIPHER_GCM_ASE_128:
        case RTK_MACSEC_CIPHER_GCM_ASE_256:
            hwsa.key_bytes = pSa->key_bytes;
            hwsa.seq = pSa->pn;
            hwsa.seq_h = 0;
            memset(hwsa.salt, 0x0, sizeof(rtk_uint8) * 12);
            memset(hwsa.ssci, 0x0, sizeof(rtk_uint8) * 4);
            break;

        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_128:
        case RTK_MACSEC_CIPHER_GCM_ASE_XPN_256:
            hwsa.flags = RTL8373_MACSEC_SA_FLAG_XPN;
            hwsa.key_bytes = pSa->key_bytes;
            hwsa.seq = pSa->pn;
            hwsa.seq_h = pSa->pn_h;
            memcpy(hwsa.salt, pSa->salt, sizeof(rtk_uint8) * 12);
            memcpy(hwsa.ssci, pSa->ssci, sizeof(rtk_uint8) * 4);
            break;
        default:
            return RT_ERR_FAILED;
    }

    memcpy(hwsa.key, pSa->key, sizeof(rtk_uint8) * hwsa.key_bytes);

    ret = dal_rtl8373_macsec_hw_sa_set(port, dir, sa_id, &hwsa);
    if (ret != RT_ERR_OK)
        return ret;

    if ((dir == MACSEC_DIRECTION_EGRESS) && MACSEC_SA_IS_USED(port, dir, post_sa_id))
    {
        ret = dal_rtl8373_macsec_hw_sa_get(port, dir, post_sa_id, &hwsa);
        if (ret != RT_ERR_OK)
            return ret;
        hwsa.next_sa_valid = 1;
        ret = dal_rtl8373_macsec_hw_sa_set(port, dir, post_sa_id, &hwsa);
        if (ret != RT_ERR_OK)
            return ret;
    }

    if (flow_state_recover == 1)
    {
        ret = dal_rtl8373_macsec_flow_enable_set(port, dir, flow_id, ENABLED);
        if (ret != RT_ERR_OK)
            return ret;
    }

    MACSEC_SA_SET_USED(port, dir, sa_id);
    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_sa_get
 * Description:
 *      Get configuration info for a Secure Association
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      pSa      - pointer to macsec SA configuration structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_sa_get(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_sa_t *pSa)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 sa_id = RTL8373_MACSEC_HW_SA_ID(sc_id, an);
    rtl8373_macsec_sa_params_t hwsa;

    if (pSa == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(port, dir, sc_id))
    {
        rtlglue_printf("%s SC %u is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(port, dir, sa_id))
    {
        rtlglue_printf("%s SA(SC %u, AN %u) is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX", sc_id, an);
        return RT_ERR_MACSEC_SA_NOTFOUND;
    }

    memset(pSa, 0x0, sizeof(rtk_macsec_sa_t));

    ret = dal_rtl8373_macsec_hw_sa_get(port, dir, sa_id, &hwsa);
    if (ret != RT_ERR_OK)
        return ret;

    pSa->key_bytes = hwsa.key_bytes;
    memcpy(pSa->key, hwsa.key, sizeof(rtk_uint8) * hwsa.key_bytes);
    if (hwsa.flags & RTL8373_MACSEC_SA_FLAG_XPN)
    {
        pSa->pn = hwsa.seq;
        pSa->pn_h = hwsa.seq_h;
        memcpy(pSa->salt, hwsa.salt, sizeof(rtk_uint8) * 12);
        memcpy(pSa->ssci, hwsa.ssci, sizeof(rtk_uint8) * 4);
    }
    else /* PN */
    {
        pSa->pn = hwsa.seq;
        pSa->pn_h = hwsa.seq_h;
        memcpy(pSa->salt, hwsa.salt, sizeof(rtk_uint8) * 12);
        memcpy(pSa->ssci, hwsa.ssci, sizeof(rtk_uint8) * 4);
    }

    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_sa_del
 * Description:
 *      Delete a MACsec Secure Association
 * Input:
 *      port     - port id
 *      dir      - ingress or egress
 *      sc_id    - SC id
 *      an       - Secure Association Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_sa_del(rtk_port_t port, rtk_macsec_dir_t dir, rtk_uint32 sc_id, rtk_macsec_an_t an)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 sa_id = RTL8373_MACSEC_HW_SA_ID(sc_id, an);
    rtl8373_macsec_sa_params_t hwsa;
    rtk_macsec_sc_status_t sc_status;

    if (sc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(port, dir, sc_id))
    {
        rtlglue_printf("%s SC %u is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }

    ret = dal_rtl8373_macsec_sc_status_get(port, dir, sc_id, &sc_status);
    if (ret != RT_ERR_OK)
        return ret;
    if (dir == MACSEC_DIRECTION_EGRESS)
    {
        /* disable flow for running AN */
        if ((sc_status.tx.hw_sc_flow_status == 1) && (sc_status.tx.running_an == an))
        {
            ret = dal_rtl8373_macsec_txsa_disable(port, sc_id);
            if (ret != RT_ERR_OK)
                return ret;
        }
    }
    else /* RTK_MACSEC_DIR_INGRESS */
    {
        if (sc_status.rx.hw_sc_flow_status[an] == 1)
        {
            ret = dal_rtl8373_macsec_rxsa_disable(port, sc_id, an);
            if (ret != RT_ERR_OK)
                return ret;
        }
    }

    ret = dal_rtl8373_macsec_hw_sa_get(port, dir, sa_id, &hwsa);
    if (ret != RT_ERR_OK)
        return ret;
    memset(hwsa.key, 0, sizeof(rtk_uint8) * RTK_MACSEC_MAX_KEY_LEN);
    memset(hwsa.salt, 0x0, sizeof(rtk_uint8) * 12);
    memset(hwsa.ssci, 0x0, sizeof(rtk_uint8) * 4);
    hwsa.seq = 0;
    hwsa.seq_h = 0;
    ret = dal_rtl8373_macsec_hw_sa_set(port, dir, sa_id, &hwsa);
    if (ret != RT_ERR_OK)
        return ret;

    MACSEC_SA_UNSET_USED(port, dir, sa_id);
    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_stat_clear
 * Description:
 *      Clear all statistics counter
 * Input:
 *      port     - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_stat_clear(rtk_port_t port)
{
    rtk_api_ret_t ret = RT_ERR_OK;

    ret = dal_rtl8373_macsec_ingress_set(port,RTL8373_MACSEC_COUNT_CONTROL, 0x00000001);
    if (ret != RT_ERR_OK)
        return ret;
    ret = dal_rtl8373_macsec_egress_set(port,RTL8373_MACSEC_COUNT_CONTROL, 0x00000001);
    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_stat_port_get
 * Description:
 *      get per-port statistics counter
 * Input:
 *      port     - port id
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
rtk_api_ret_t dal_rtl8373_macsec_stat_port_get(rtk_port_t port, rtk_macsec_stat_t stat, rtk_uint64 *pCnt)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint64 cnt_h = 0, cnt_l = 0;
    rtk_uint32 data= 0;

    if (pCnt == NULL)
        return RT_ERR_INPUT;

    switch (stat)
    {
        case RTK_MACSEC_STAT_InPktsUntagged:
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC418, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC41C, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_InPktsNoTag:
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC410, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC414, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_InPktsBadTag:
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC428, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC42C, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_InPktsUnknownSCI:
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC440, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC444, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_InPktsNoSCI:
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC438, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, 0xC43C, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_STAT_OutPktsUntagged:
            ret = dal_rtl8373_macsec_egress_get(port, 0xC418, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_egress_get(port, 0xC41C, &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        default:
            return RT_ERR_INPUT;
    }
    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_stat_txsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      port     - port id
 *      txsc_id  - egress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
rtk_api_ret_t dal_rtl8373_macsec_stat_txsa_get(rtk_port_t port, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_txsa_stat_t stat, rtk_uint64 *pCnt)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint64 cnt_h = 0, cnt_l = 0;
    rtk_uint32 data= 0, base = 0;
    rtk_uint32 sa_id = RTL8373_MACSEC_HW_SA_ID(sc_id, an);
    rtk_macsec_dir_t dir = MACSEC_DIRECTION_EGRESS;

    if (pCnt == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(port, dir, sc_id))
    {
        rtlglue_printf("%s SC %u is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(port, dir, sa_id))
    {
        rtlglue_printf("%s SA(SC %u, AN %u) is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX", sc_id, an);
        return RT_ERR_MACSEC_SA_NOTFOUND;
    }

    base = (sa_id * 0x80);
    switch (stat)
    {
        case RTK_MACSEC_TXSA_STAT_OutPktsTooLong:
            ret = dal_rtl8373_macsec_egress_get(port, (base + 0x8018), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_egress_get(port, (base + 0x801C), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_TXSA_STAT_OutOctetsProtectedEncrypted:
            ret = dal_rtl8373_macsec_egress_get(port, (base + 0x8000), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_egress_get(port, (base + 0x8004), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_TXSA_STAT_OutPktsProtectedEncrypted:
            ret = dal_rtl8373_macsec_egress_get(port, (base + 0x8010), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_egress_get(port, (base + 0x8014), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_stat_rxsa_get
 * Description:
 *      get per-egress-SA statistics counter
 * Input:
 *      port     - port id
 *      rxsc_id  - ingress SC id
 *      an       - Secure Association Number
 *      stat     - statistics type
 * Output:
 *      pCnt     - pointer to counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      the counter value will read clear, customer software should collecting and accumulating the statistics.
 */
rtk_api_ret_t dal_rtl8373_macsec_stat_rxsa_get(rtk_port_t port, rtk_uint32 sc_id, rtk_macsec_an_t an, rtk_macsec_rxsa_stat_t stat, rtk_uint64 *pCnt)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint64 cnt_h = 0, cnt_l = 0;
    rtk_uint32 data= 0, base = 0;
    rtk_uint32 sa_id = RTL8373_MACSEC_HW_SA_ID(sc_id, an);
    rtk_macsec_dir_t dir = MACSEC_DIRECTION_INGRESS;

    if (pCnt == NULL)
        return RT_ERR_INPUT;
    if (sc_id >= MACSEC_SC_MAX(port))
        return RT_ERR_INPUT;
    if (sa_id >= MACSEC_SA_MAX(port))
        return RT_ERR_INPUT;
    if (MACSEC_SC_IS_CLEAR(port, dir, sc_id))
    {
        rtlglue_printf("%s SC %u is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX",sc_id);
        return RT_ERR_MACSEC_SC_NOTFOUND;
    }
    if (MACSEC_SA_IS_CLEAR(port, dir, sa_id))
    {
        rtlglue_printf("%s SA(SC %u, AN %u) is not existed!",(MACSEC_DIRECTION_EGRESS == dir) ? "TX" : "RX", sc_id, an);
        return RT_ERR_MACSEC_SA_NOTFOUND;
    }

    base = (sa_id * 0x80);
    switch (stat)
    {
        case RTK_MACSEC_RXSA_STAT_InPktsUnusedSA:
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8048), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x804C), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsNotUsingSA:
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8040), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8044), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsUnchecked:
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8010), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8014), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsDelayed:
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8018), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x801C), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsLate:
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8020), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8024), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsOK:
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8028), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x802C), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsInvalid:
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8030), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8034), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InPktsNotValid:
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8038), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x803C), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        case RTK_MACSEC_RXSA_STAT_InOctetsDecryptedValidated:
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8000), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_l = (rtk_uint64)data;
            ret = dal_rtl8373_macsec_ingress_get(port, (base + 0x8004), &data);
            if (ret != RT_ERR_OK)
                return ret;
            cnt_h = (rtk_uint64)data;
            *pCnt = (cnt_h << 32) | cnt_l;
            break;
        default:
            return RT_ERR_INPUT;
    }
    return ret;
}


/* Function Name:
 *      dal_rtl8373_macsec_intr_status_get
 * Description:
 *      Get status information for MACsec interrupt
 * Input:
 *      port     - port id
 * Output:
 *      pIntr_status - interrupt status structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
rtk_api_ret_t dal_rtl8373_macsec_intr_status_get(rtk_port_t port,rtk_macsec_intr_status_t *pIntr_status)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32 st_data = 0, exp_data = 0, thr_data = 0;
    rtk_uint32 i = 0, j = 0, sum_cnt = 0, sa_base = 0, sc_base = 0;;
    if (pIntr_status == NULL)
        return RT_ERR_INPUT;
    memset(pIntr_status, 0x0, sizeof(rtk_macsec_intr_status_t));

    ret = dal_rtl8373_macsec_egress_get(port, 0xF80C, &st_data);
    if (ret != RT_ERR_OK)
        return ret;

    if (st_data & 0x0200)
    {
        pIntr_status->status |= RTK_MACSEC_INTR_EGRESS_PN_ROLLOVER;
    }
    if (st_data & 0x0100)
    {
        pIntr_status->status |= RTK_MACSEC_INTR_EGRESS_PN_THRESHOLD;
    }

    sum_cnt = MACSEC_SA_MAX(port)/ 32;

    for (i = 0; i < sum_cnt; i++) //0,1
    {
        
        ret = dal_rtl8373_macsec_egress_get(port, (0xF000 + (4 * i)), &thr_data);
        if (ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_get(port, (0x7A00 + (4 * i)), &exp_data);
        if (ret != RT_ERR_OK)
            return ret;
        
        sa_base = i * 32; //0, 32
        sc_base = sa_base/4; //0, 8

        for (j = 0; j < 8; j++)
        {
            pIntr_status->egress_pn_thr_an_bmap[sc_base + j] = (thr_data >> (j * 4)) & 0xF;
            pIntr_status->egress_pn_exp_an_bmap[sc_base + j] = (exp_data >> (j * 4)) & 0xF;
        }

        ret = dal_rtl8373_macsec_egress_set(port, (0xF000 + (4 * i)), 0xFFFF);
        if (ret != RT_ERR_OK)
            return ret;
        ret = dal_rtl8373_macsec_egress_set(port, (0x7A00 + (4 * i)), 0xFFFF);
        if (ret != RT_ERR_OK)
            return ret;
    }

    ret = dal_rtl8373_macsec_egress_set(port, 0xF810, 0x03FF);

    return ret;
}



#endif

#if 0
/* Function Name:
 *      dal_rtl8373_macsec_init
 * Description:
 *      Initialize MACsec information.
 * Input:
 *      port_mask   -  port mask, bit[4:7]
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_init(rtk_uint32 port_mask)
{
    rtk_api_ret_t retVal;
    rtk_uint32 phy_data;
    rtk_uint32 tmp_value;
    rtk_uint32 index = 0;
    rtk_uint32 port;
    

    memset(&macsec_info, 0, sizeof(rtl8373_macsec_info_t));
    port_macsec_info = &macsec_info;


    dal_rtl8373_mdc_en(ENABLED);
   
       for(port = 0; port < RTL8373_MACSEC_PORT_NUM; port++)
       {
           if(port_mask & (1 << port))
           {
#if 1 //pending
                  /* check phy if link up*/
            if ((retVal = dal_rtl8373_phy_read(port, 7, 1, &phy_data)) != RT_ERR_OK)
                          return retVal;
            if(((phy_data >> 2) & 0x1) != 1)
                    return RT_ERR_PHY_LINK_DOWN;
#endif  

            /* enable macsec egress enable and ingress enable */
            if(retVal = dal_rtl8373_macsec_enable_set(port, (rtk_uint32)ENABLED, (rtk_uint32)ENABLED) != RT_ERR_OK)
                    return retVal;
    
            /* check egress and ingress version*/
            if(retVal = dal_rtl8373_macsec_egress_get(port, (rtk_uint32)RTL8373_MACSEC_EIP160_VERSION, &tmp_value) != RT_ERR_OK)
                    return retVal;

            if((tmp_value & 0xffff) != 0x5fa0)
                    return RT_ERR_MACSEC_EGRESS_DEVICE;
#if 1
            if(retVal = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_EIP160_VERSION, &tmp_value) != RT_ERR_OK)
                    return retVal;
            if((tmp_value & 0xffff) != 0x5fa0)
                    return RT_ERR_MACSEC_INGRESS_DEVICE;
#endif

            dal_rtl8373_macsec_rxIPbypass_set(port, DISABLED);
            dal_rtl8373_macsec_txIPbypass_set(port, DISABLED);
            dal_rtl8373_macsec_rxbypass_set(port, DISABLED);
            dal_rtl8373_macsec_txbypass_set(port, DISABLED);

            if(port == 0)
            {
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 1)
            {
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 2)
            {
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 3)
            {
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 4)
            {
                if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 5)
            {
                if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 6)
            {
                if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 7)
            {
                if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else
                return RT_ERR_PORT_ID;
            

            /* initial egress and ingress transform records*/
            for(index = 0; index < 16; index++)
            {
                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_CTRL_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_ID_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Key0_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Key1_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Key2_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Key3_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Hkey0_Key4_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Hkey1_Key5_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Hkey2_Key6_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Hkey3_Key7_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Seq0_Hkey0_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Seq1_Hkey1_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_Hkey2_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IS0_Hkey3_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IS1_Seq0_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IS2_Seq1_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IV0_Zero_E_64(index), 0) != RT_ERR_OK)
                        return retVal;
                
                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IV1_IS0_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_IS1_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_SAupd_IS2_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_IV0_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_IV1_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_SAupd_E_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_CTRL_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_ID_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Key0_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Key1_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Key2_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Key3_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Hkey0_Key4_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Hkey1_Key5_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Hkey2_Key6_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Hkey3_Key7_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Seq0_Hkey0_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Seq1_Hkey1_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Mask_Hkey2_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_IV0_Hkey3_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_IV1_Seq0_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_IV2_Seq1_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Zero_Mask_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Zero_IV0_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Zero_IV1_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Zero_IV2_I_64(index), 0) != RT_ERR_OK)
                        return retVal;

                //initial SA match rule and flow index
                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SA_LO(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SA_HI(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_DA_LO(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_DA_HI(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_MISC(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SCI_LO(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SCI_HI(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_MASK(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_EXT(index), 0) != RT_ERR_OK)
                        return retVal;

                if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_FLOW_CTRL(index), 0) != RT_ERR_OK)
                        return retVal;
            }
            
            /* configure egress and ingress context size*/
            if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_EIP62_CONTEXT_CTRL, 0xe5880218) != RT_ERR_OK)
                        return retVal;
            
            if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_EIP62_CONTEXT_CTRL, 0xe5880214) != RT_ERR_OK)
                        return retVal;
            
            /* configure context update control*/
            if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_EIP62_CONTEXT_UPD_CTRL, 0x0003) != RT_ERR_OK)
                        return retVal;
            
            if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_EIP62_CONTEXT_UPD_CTRL, 0x0003) != RT_ERR_OK)
                        return retVal;
            
            /* configure MACsec fix latency and xform size*/
            if(retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_MISC_CONTROL, 0x02000030) != RT_ERR_OK)
                        return retVal;
            
            if(retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_MISC_CONTROL, 0x01000833) != RT_ERR_OK)
                        return retVal;
           }
                
       }

    return RT_ERR_OK;

}
#endif


#if 1
/* Function Name:
 *      dal_rtl8373_macsec_init
 * Description:
 *      Initialize MACsec information.
 * Input:
 *      port_mask   -  port mask, bit[4:7]
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_QUEUE_NUM    - Invalid queue number.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_init(rtk_uint32 port_mask)
{
	rtk_api_ret_t retVal;
	rtk_uint32 phy_data;
	rtk_uint32 tmp_value;
	rtk_uint32 index = 0;
	rtk_uint32 port;

	memset(&macsec_info, 0, sizeof(rtl8373_macsec_info_t));
	memset(macsec_portinfo, 0, sizeof(macsec_portinfo));
    port_macsec_info = &macsec_info;

	for(port=0; port<RTL8373_MACSEC_PORT_NUM; port++)
	{
		macsec_info.port[port] = &macsec_portinfo[port];
		macsec_portinfo[port].max_sa_num = RTK_MAX_MACSEC_SA_PER_PORT;
	}

	dal_rtl8373_mdc_en(ENABLED);
	
       for(port = 0; port < 8; port++)
       {
       	if(port_mask & (1 << port))
	       {
	              /* check phy if link up*/
			if ((retVal = dal_rtl8373_phy_read(port, 7, 1, &phy_data)) != RT_ERR_OK)
		      			return retVal;

			if(((phy_data >> 2) & 0x1) != 1)
					return RT_ERR_PHY_LINK_DOWN;
			
			/* enable macsec egress enable and ingress enable */
			if((retVal = dal_rtl8373_macsec_enable_set(port, ENABLED, ENABLED)) != RT_ERR_OK)
					return retVal;

			if((retVal = dal_rtl8373_macsec_egress_get(port, RTL8373_MACSEC_EIP160_VERSION, &tmp_value)) != RT_ERR_OK)
					return retVal;

			dal_rtl8373_macsec_rxIPbypass_set(port, DISABLED);
			dal_rtl8373_macsec_txIPbypass_set(port, DISABLED);
			dal_rtl8373_macsec_rxbypass_set(port, DISABLED);
			dal_rtl8373_macsec_txbypass_set(port, DISABLED);


			if(port == 0)
            {
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 1)
            {
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 2)
            {
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 3)
            {
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
                if ((retVal = dal_rtl8224_top_regbit_write(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
                        return retVal;
            }
            else if(port == 4)
			{
				if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
	        			return retVal;
				if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT4_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT4_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
	        			return retVal;
			}
			else if(port == 5)
			{
				if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
	        			return retVal;
				if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT5_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT5_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
	        			return retVal;
			}
			else if(port == 6)
			{
				if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
	        			return retVal;
				if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT6_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT6_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
	        			return retVal;
			}
			else if(port == 7)
			{
				if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_TX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
	        			return retVal;
				if ((retVal = rtl8373_setAsicRegBit(RTL8373_MACSEC_REG_GLB_SET1_PORT7_ADDR, RTL8373_MACSEC_REG_GLB_SET1_PORT7_RX_SWRST_EN_OFFSET, 1)) != RT_ERR_OK)
	        			return retVal;
			}
			else
				return RT_ERR_PORT_ID;
			
			/* check egress and ingress version*/
			if((tmp_value & 0xffff) != 0x5fa0)
					return RT_ERR_MACSEC_EGRESS_DEVICE;

			if((retVal = dal_rtl8373_macsec_ingress_get(port, RTL8373_MACSEC_EIP160_VERSION, &tmp_value)) != RT_ERR_OK)
					return retVal;

			if((tmp_value & 0xffff) != 0x5fa0)
					return RT_ERR_MACSEC_INGRESS_DEVICE;

			/* initial egress and ingress transform records*/
			for(index = 0; index < 16; index++)
			{
				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_CTRL_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_ID_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Key0_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Key1_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Key2_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Key3_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Hkey0_Key4_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Hkey1_Key5_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Hkey2_Key6_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Hkey3_Key7_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Seq0_Hkey0_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Seq1_Hkey1_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_Hkey2_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IS0_Hkey3_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IS1_Seq0_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IS2_Seq1_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IV0_Zero_E_64(index), 0)) != RT_ERR_OK)
						return retVal;
				
				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_IV1_IS0_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_IS1_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_SAupd_IS2_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_IV0_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_IV1_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_TR_Zero_SAupd_E_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_CTRL_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_ID_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Key0_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Key1_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Key2_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Key3_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Hkey0_Key4_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Hkey1_Key5_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Hkey2_Key6_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Hkey3_Key7_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Seq0_Hkey0_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Seq1_Hkey1_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Mask_Hkey2_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_IV0_Hkey3_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_IV1_Seq0_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_IV2_Seq1_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Zero_Mask_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Zero_IV0_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Zero_IV1_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_TR_Zero_IV2_I_64(index), 0)) != RT_ERR_OK)
						return retVal;

				//initial SA match rule and flow index
				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SA_LO(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SA_HI(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_DA_LO(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_DA_HI(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_MISC(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SCI_LO(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_SCI_HI(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_MASK(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_EXT(index), 0)) != RT_ERR_OK)
						return retVal;

				if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_SAM_FLOW_CTRL(index), 0)) != RT_ERR_OK)
						return retVal;
			}

			/* configure egress and ingress context size*/
			if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_EIP62_CONTEXT_CTRL, 0xe5880218)) != RT_ERR_OK)
						return retVal;

			if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_EIP62_CONTEXT_CTRL, 0xe5880214)) != RT_ERR_OK)
						return retVal;

			/* configure context update control*/
			if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_EIP62_CONTEXT_UPD_CTRL, 0x0003)) != RT_ERR_OK)
						return retVal;

			if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_EIP62_CONTEXT_UPD_CTRL, 0x0003)) != RT_ERR_OK)
						return retVal;

			/* configure MACsec fix latency and xform size*/
			if((retVal = dal_rtl8373_macsec_egress_set(port, RTL8373_MACSEC_MISC_CONTROL, 0x02000030)) != RT_ERR_OK)
						return retVal;

			if((retVal = dal_rtl8373_macsec_ingress_set(port, RTL8373_MACSEC_MISC_CONTROL, 0x01000833)) != RT_ERR_OK)
						return retVal;
	       }
				
       }

	return RT_ERR_OK;

}


/* Function Name:
 *      dal_rtl8373_macsec_ra_set
 * Description:
 *      Set RA function.
 * Input:
 *      portid   -  port id
 *      mode    - 0: don't insert ipg for macsec
 *                1: insert ipg according to feedback signal
 *                2: insert ipg according to ethtype & ipg_length
 *                3: always insert ipg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_ra_set(rtk_uint32 portid, rtk_uint32 mode)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(portid);

    if (mode > 3)
        return RT_ERR_INPUT;

    retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_IPG_CFG_ADDR(portid), RTL8373_MAC_MACSEC_IPG_CFG_MACSEC_IPG_MODE_MASK, mode);

    return retVal;


}

/* Function Name:
 *      dal_rtl8373_macsec_ra_get
 * Description:
 *      Get RA function.
 * Input:
 *      portid   -  port id
 *      pMode    - 0: don't insert ipg for macsec
 *                1: insert ipg according to feedback signal
 *                2: insert ipg according to ethtype & ipg_length
 *                3: always insert ipg
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_ra_get(rtk_uint32 portid, rtk_uint32* pMode)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(portid);

    if (pMode == NULL)
        return RT_ERR_INPUT;

    retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_IPG_CFG_ADDR(portid), RTL8373_MAC_MACSEC_IPG_CFG_MACSEC_IPG_MODE_MASK, pMode);

    return retVal;


}

/* Function Name:
 *      dal_rtl8373_macsec_ipglen_set
 * Description:
 *      Set RA function.
 * Input:
 *      portid   -  port id
 *      len      - additional ipg length for macsec
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_ipglen_set(rtk_uint32 portid, rtk_uint32 len)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(portid);


    retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_IPG_CFG_ADDR(portid), RTL8373_MAC_MACSEC_IPG_CFG_MACSEC_IPG_LENGTH_MASK, len);

    return retVal;


}


/* Function Name:
 *      dal_rtl8373_macsec_ipglen_get
 * Description:
 *      Get RA function.
 * Input:
 *      portid   -  port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_ipglen_get(rtk_uint32 portid, rtk_uint32* pLen)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(portid);


    retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_IPG_CFG_ADDR(portid), RTL8373_MAC_MACSEC_IPG_CFG_MACSEC_IPG_LENGTH_MASK, pLen);

    return retVal;


}

/* Function Name:
 *      dal_rtl8373_macsec_ra_ethtype_set
 * Description:
 *      Set RA function.
 * Input:
 *      portid   -  port id
 *      id      - ether type id 0-7
 *      type    - ether type value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_ra_ethtype_set(rtk_uint32 portid, rtk_uint32 id, rtk_uint32 type)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(portid);

    if (id == 0)
        retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_1_0_ADDR(portid), RTL8373_MAC_MACSEC_ETH_1_0_MACSEC_ETH_0_MASK, type);
    else if (id == 1)
        retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_1_0_ADDR(portid), RTL8373_MAC_MACSEC_ETH_1_0_MACSEC_ETH_1_MASK, type);
    else if (id == 2)
        retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_3_2_ADDR(portid), RTL8373_MAC_MACSEC_ETH_3_2_MACSEC_ETH_2_MASK, type);
    else if (id == 3)
        retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_3_2_ADDR(portid), RTL8373_MAC_MACSEC_ETH_3_2_MACSEC_ETH_3_MASK, type);
    else if (id == 4)
        retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_5_4_ADDR(portid), RTL8373_MAC_MACSEC_ETH_5_4_MACSEC_ETH_4_MASK, type);
    else if (id == 5)
        retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_5_4_ADDR(portid), RTL8373_MAC_MACSEC_ETH_5_4_MACSEC_ETH_5_MASK, type);
    else if (id == 6)
        retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_7_6_ADDR(portid), RTL8373_MAC_MACSEC_ETH_7_6_MACSEC_ETH_6_MASK, type);
    else if (id == 7)
        retVal = rtl8373_setAsicRegBits(RTL8373_MAC_MACSEC_ETH_7_6_ADDR(portid), RTL8373_MAC_MACSEC_ETH_7_6_MACSEC_ETH_7_MASK, type);

    return retVal;


}


/* Function Name:
 *      dal_rtl8373_macsec_ra_ethtype_set
 * Description:
 *      Set RA function.
 * Input:
 *      portid   -  port id
 *      id      - ether type id 0-7
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API will initialize MACsec information.
 */
rtk_api_ret_t dal_rtl8373_macsec_ra_ethtype_get(rtk_uint32 portid, rtk_uint32 id, rtk_uint32* pType)
{
    rtk_api_ret_t retVal;

    /* Check initialization state */
    RTK_CHK_INIT_STATE();

    /* Check Port Valid */
    RTK_CHK_PORT_VALID(portid);

    if (id == 0)
        retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_1_0_ADDR(portid), RTL8373_MAC_MACSEC_ETH_1_0_MACSEC_ETH_0_MASK, pType);
    else if (id == 1)
        retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_1_0_ADDR(portid), RTL8373_MAC_MACSEC_ETH_1_0_MACSEC_ETH_1_MASK, pType);
    else if (id == 2)
        retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_3_2_ADDR(portid), RTL8373_MAC_MACSEC_ETH_3_2_MACSEC_ETH_2_MASK, pType);
    else if (id == 3)
        retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_3_2_ADDR(portid), RTL8373_MAC_MACSEC_ETH_3_2_MACSEC_ETH_3_MASK, pType);
    else if (id == 4)
        retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_5_4_ADDR(portid), RTL8373_MAC_MACSEC_ETH_5_4_MACSEC_ETH_4_MASK, pType);
    else if (id == 5)
        retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_5_4_ADDR(portid), RTL8373_MAC_MACSEC_ETH_5_4_MACSEC_ETH_5_MASK, pType);
    else if (id == 6)
        retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_7_6_ADDR(portid), RTL8373_MAC_MACSEC_ETH_7_6_MACSEC_ETH_6_MASK, pType);
    else if (id == 7)
        retVal = rtl8373_getAsicRegBits(RTL8373_MAC_MACSEC_ETH_7_6_ADDR(portid), RTL8373_MAC_MACSEC_ETH_7_6_MACSEC_ETH_7_MASK, pType);

    return retVal;


}






#endif



