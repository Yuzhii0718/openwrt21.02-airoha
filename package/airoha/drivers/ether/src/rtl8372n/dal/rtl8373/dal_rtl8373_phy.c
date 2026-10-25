/* MDIO Manageable Device(MDD) address*/
#include <dal_rtl8373_phy.h>

#include <dal_rtl8373_drv.h>
//#include <stdlib.h>
#include <linux/string.h>
#include <rtk_error.h>
#include <dal_rtl8373_port.h>
//#include "port.h"



/* Function Name:
*      dal_rlt8373_phy_common_c45_an_restart
* Description:
*      After set auto NegoAbility,restart the phy
* Input:
*      port - port id
* Output:
*      None
* Return:
*      RT_ERR_OK     - OK
*      RT_ERR_FAILED - invalid parameter
* Note:
*      1000 is vendor specific in C45.
*/
rtk_api_ret_t dal_rlt8373_phy_common_c45_an_restart(rtk_port_t port)
{
    rtk_int32   ret;
    rtk_uint32 phyData = 0;

    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;

    if (phyData & BIT_12) /*AN is enabled*/
    {
        phyData |= BIT_9; /*AN restart*/
        if ((ret = dal_rtl8373_phy_write( 1UL<<port, PHY_MMD_AN, 0, phyData)) != RT_ERR_OK)
            return ret;
    }
    
    return ret;
}

/* Function Name:
 *      dal_rlt8373_phy_common_c45_autoNegoEnable_get
 * Description:
 *      Get the auto-negotiation enable or not
 * Input:
 *      port - port id
 * Output:
 *      pEnable - pointer to output the auto-negotiation state
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_common_c45_autoNegoEnable_get( rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_api_ret_t   ret;
    rtk_uint32 phyData = 0;

    if ((ret = dal_rtl8373_phy_read( port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;

    *pEnable = (phyData & BIT_12) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      dal_rlt8373_phy_common_c45_autoNegoEnable_set
 * Description:
 *      Set the auto-negotiation state of the specific port enable or not.
 * Input:
 *      port - port id
 *      enable - auto-negotiation state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_common_c45_autoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtk_api_ret_t   ret;
    rtk_uint32  phyData = 0;

    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_AN, 0, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= (~(BIT_12 | BIT_9));
    phyData |= (enable == ENABLED) ? (BIT_12 | BIT_9) : (0);

    ret = dal_rtl8373_phy_write(1UL<<port, PHY_MMD_AN, 0, phyData);
    return ret;
}

/* Function Name:
 *      dal_rlt8373_phy_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto-negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto-negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_autoNegoAbility_set(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtk_api_ret_t   ret = RT_ERR_OK;
    rtk_uint32  phyData = 0;

    if ((ret = dal_rtl8373_phy_read( port, PHY_MMD_AN, 16, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_5 | BIT_6 | BIT_7 | BIT_8 | BIT_10 | BIT_11));

    phyData |= (pAbility->Half_10) ? (BIT_5) : (0);
    phyData |= (pAbility->Full_10) ? (BIT_6) : (0);
    phyData |= (pAbility->Half_100) ? (BIT_7) : (0);
    phyData |= (pAbility->Full_100) ? (BIT_8) : (0);
    phyData |= (pAbility->FC) ? (BIT_10) : (0);
    phyData |= (pAbility->AsyFC) ? (BIT_11) : (0);

    if ((ret = dal_rtl8373_phy_write(1UL<<port, PHY_MMD_AN, 16, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_7 | BIT_8 | BIT_12));
    phyData |= (pAbility->adv_2_5G) ? (BIT_7) : (0);

    phyData |= (pAbility->adv_5G) ? (BIT_8) : (0);
    phyData |= (pAbility->adv_10GBase_T) ? (BIT_12) : (0);

    if ((ret = dal_rtl8373_phy_write(1UL<<port, PHY_MMD_AN, 32, phyData)) != RT_ERR_OK)

        return ret;

    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_VEND2, 0xA412, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_9));
    phyData |= (pAbility->Full_1000) ? (BIT_9) : (0);

    if ((ret = dal_rtl8373_phy_write( 1UL<<port, PHY_MMD_VEND2, 0xA412, phyData)) != RT_ERR_OK)
        return ret;
    if ((ret = dal_rlt8373_phy_common_c45_an_restart(port) != RT_ERR_OK))
    {
    //printf("line %d\n",(uint16)__LINE__);
        return ret;
    }

    return ret;
}

/* Function Name:
 *      dal_rlt8373_phy_autoNegoAbility_get
 * Description:
 *      get ability advertisement for auto-negotiation of the specific port
 * Input:
 *      port - port id 
 * Output:
*      pAbility  - auto-negotiation ability of the specific port
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_autoNegoAbility_get(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtk_api_ret_t ret = RT_ERR_OK;
    rtk_uint32  phyData = 0;
    rtk_enable_t AutoNegotiationEn;

    if ((ret = dal_rlt8373_phy_common_c45_autoNegoEnable_get(port, &AutoNegotiationEn)) != RT_ERR_OK)
	{
		return ret;
	}

    if(AutoNegotiationEn == 0)
    {
        return RT_ERR_FAILED;
    } 

    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_AN, 16, &phyData)) != RT_ERR_OK)
        return ret;

    pAbility->Half_10 = (phyData >> 5) & 1;
    pAbility->Full_10 = (phyData >> 6) & 1;  
    pAbility->Half_100 = (phyData >> 7) & 1;
    pAbility->Full_100 = (phyData >> 8) & 1;
    pAbility->FC  = (phyData >> 10) & 1;
    pAbility->AsyFC = (phyData >> 11) & 1;

    phyData = 0;
    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_AN, 32, &phyData)) != RT_ERR_OK)
        return ret;

    pAbility->adv_2_5G = (phyData >> 7) & 1;
    pAbility->adv_5G = (phyData >> 8) & 1;
    pAbility->adv_10GBase_T = (phyData >> 12) & 1;

    phyData = 0;
    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_VEND2, 0xA412, &phyData)) != RT_ERR_OK)
        return ret;

    pAbility->Full_1000 = (phyData >> 9) & 1;

    return ret;
}
/* Function Name:
 *      dal_rlt8373_phy_common_c45_autoSpeed_set
 * Description:
 *      Set phy speed for the specific port
 * Input:
 *      port - port id
 *      pAbility  - phy ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */

rtk_api_ret_t dal_rlt8373_phy_common_c45_autoSpeed_set(rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
	rtk_uint32 ret = RT_ERR_FAILED;
	rtk_enable_t AutoNegotiationEn = 1;
	rtk_port_phy_ability_t ability;
	
	memset(&ability,0,sizeof(rtk_port_phy_ability_t));
		
	ability.Half_10 		= pAbility->Half_10;
	ability.Full_10 		= pAbility->Full_10;
	ability.Half_100		= pAbility->Half_100;
	ability.Full_100		= pAbility->Full_100;
	ability.Full_1000		= pAbility->Full_1000;
	ability.adv_2_5G   = pAbility->adv_2_5G;
	ability.adv_5G   = pAbility->adv_5G;
	ability.adv_10GBase_T	=  pAbility->adv_10GBase_T;
	ability.FC				= pAbility->FC;
	ability.AsyFC			= pAbility->AsyFC;

	if ((ret = dal_rlt8373_phy_common_c45_autoNegoEnable_set(port, AutoNegotiationEn)) != RT_ERR_OK)
	{
		return ret;
	}
	
	if ((ret = dal_rlt8373_phy_autoNegoAbility_set(port, &ability)) != RT_ERR_OK)
	{
		return ret;
	}

	return RT_ERR_OK;

}

/* Function Name:
 *      dal_rlt8373_phy_common_c45_speed_get
 * Description:
 *      Get speed of the specific port
 * Input:
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_common_c45_speed_get(rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    rtk_api_ret_t   ret;
    rtk_uint32  phyData = 0;
    rtk_uint8   speed_l = 0;
    rtk_uint8   speed_h = 0;

    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    speed_l = (((phyData & BIT_6) ? 1 : 0) << 1) | ((phyData & BIT_13) ? 1 : 0);
    speed_h = (phyData & (BIT_5 | BIT_4 | BIT_3 | BIT_2)) >> 2;

    switch (speed_l)
    {
        case 0:
            *pSpeed = PORT_SPEED_10M;
            break;
        case 1:
            *pSpeed = PORT_SPEED_100M;
            break;
        case 2:
            *pSpeed = PORT_SPEED_1000M;
            break;
        case 3:
            switch (speed_h)
            {
                case 0:
                    *pSpeed = PORT_SPEED_10G;
                    break;
                case 7:
                    *pSpeed = PORT_SPEED_5G;
                    break;
                case 6:
                    *pSpeed = PORT_SPEED_2500M;
                    break;
                default:
                    *pSpeed = PORT_SPEED_10M;
                    break;
            }
            break;
        default:
            *pSpeed = PORT_SPEED_10M;
            break;
    }

    return ret;
}

/* Function Name:
 *      dal_rlt8373_phy_common_c45_speed_set
 * Description:
 *      Set speed of the specific port
 * Input:
 *      port          - port id
 *      speed         - link speed rtk_port_speed_t
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_common_c45_speed_set(rtk_port_t port, rtk_port_speed_t speed)
{
    rtk_api_ret_t   ret;
    rtk_uint32  phyData = 0;

    if ((ret = dal_rtl8373_phy_read( port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~(BIT_13 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2));

    switch (speed)
    {
        case PORT_SPEED_10M:
            break;
        case PORT_SPEED_100M:
            phyData |= BIT_13;
            break;
        case PORT_SPEED_1000M:
            phyData |= BIT_6;
            break;
        case PORT_SPEED_10G:
            phyData |= (BIT_13 | BIT_6);
            break;
        case PORT_SPEED_2500M:
            phyData |= (BIT_13 | BIT_6 | BIT_4 | BIT_3);
            break;
        case PORT_SPEED_5G:
            phyData |= (BIT_13 | BIT_6 | BIT_4 | BIT_3 | BIT_2);
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    ret = dal_rtl8373_phy_write(1UL<<port, PHY_MMD_PMAPMD, 0, phyData);
    return ret;
}

/* Function Name:
 *      dal_rlt8373_phy_common_c45_enable_set
 * Description:
 *      Set PHY interface state enable/disable of the specific port
 * Input:
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_common_c45_enable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtk_api_ret_t   ret;
    rtk_uint32  phyData = 0;

    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    phyData &= (~BIT_11);
    phyData |= (enable == ENABLED) ? (0) : (BIT_11);

    ret = dal_rtl8373_phy_write(1UL<<port, PHY_MMD_PMAPMD, 0, phyData);
    return ret;
}

/* Function Name:
 *      dal_rlt8373_phy_common_c45_enable_get
 * Description:
 *      Get PHY interface state enable/disable of the specific port
 * Input:
 *      port          - port id
 * Output:
 *      pEnable       - pointer to admin configuration of PHY interface
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_common_c45_enable_get( rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_api_ret_t   ret;
    rtk_uint32  phyData = 0;

    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_PMAPMD, 0, &phyData)) != RT_ERR_OK)
        return ret;

    *pEnable = (phyData & BIT_11) ? (DISABLED) : (ENABLED);
    return ret;
}
/* Function Name:
 *      dal_rlt8373_phy_common_c45_duplex_get
 * Description:
 *      Get duplex of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_common_c45_duplex_get(rtk_port_t port, rtk_port_duplex_t *pDuplex)
{
    rtk_api_ret_t   ret = RT_ERR_OK;
    rtk_uint32  phyData = 0;

    if ((ret = dal_rtl8373_phy_read((rtk_uint16)port, PHY_MMD_VEND2, 0xA400, &phyData)) != RT_ERR_OK)
        return ret;

    *pDuplex = (phyData & BIT_8) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX;

    return ret;
}

/* Function Name:
 *      dal_rlt8373_phy_common_c45_duplex_set
 * Description:
 *      Set duplex of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
rtk_api_ret_t dal_rlt8373_phy_common_c45_duplex_set(rtk_port_t port, rtk_port_duplex_t duplex)
{
    rtk_api_ret_t   ret = RT_ERR_OK;
    rtk_uint32  phyData = 0;

    if ((ret = dal_rtl8373_phy_read((rtk_uint16)port, PHY_MMD_VEND2, 0xA400, &phyData)) != RT_ERR_OK)
        return ret;

    phyData = (duplex == PORT_FULL_DUPLEX) ? (phyData | BIT_8) : (phyData & ~BIT_8);

    ret = dal_rtl8373_phy_write(1UL<< port, PHY_MMD_VEND2, 0xA400, phyData);
    return ret;
}


/* Function Name:
 *      dal_rlt8373_phy_common_c45_speedDuplexStatusResReg_get
 * Description:
 *      Get operational link speed-duplex status from proprietary register
 * Input:
 *      port    - port id
 * Output:
 *      pSpeed - pointer to PHY operational link speed
 *      pDuplex - pointer to PHY operational Duplex
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      Only PHY that supports proprietary resolution register can use this driver!
 */
rtk_int32 dal_rlt8373_phy_common_c45_speedDuplexStatusResReg_get(rtk_port_t port, rtk_port_speed_t *pSpeed, rtk_port_duplex_t *pDuplex)
{
    rtk_int32   ret = RT_ERR_FAILED;
    rtk_uint32  phyData, spd;

    *pSpeed = PORT_SPEED_10M;
    *pDuplex = PORT_HALF_DUPLEX;
    if ((ret = dal_rtl8373_phy_read(port, PHY_MMD_VEND2, 0xA434, &phyData)) != RT_ERR_OK)
        return ret;

    /* [10:9,5:4] */
    spd = ((phyData & (0x3UL << 9)) >> (9 - 2)) | ((phyData & (0x3UL << 4)) >> 4);
    switch (spd)
    {
        case 0x0:
          *pSpeed = PORT_SPEED_10M;
          break;
        case 0x1:
          *pSpeed = PORT_SPEED_100M;
          break;
        case 0x2:
          *pSpeed = PORT_SPEED_1000M;
          break;
        case 0x3:
          *pSpeed = PORT_SPEED_500M;
          break;
        case 0x4:
          *pSpeed = PORT_SPEED_10G;
          break;
        case 0x5:
          *pSpeed = PORT_SPEED_2500M;
          break;
        case 0x6:
          *pSpeed = PORT_SPEED_5G;
          break;        
        default:
          *pSpeed = PORT_SPEED_10M;
          break;
    }

    *pDuplex = (phyData & 0x8)? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX;
    return RT_ERR_OK;
}

