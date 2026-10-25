/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/io.h>
//#include <assert.h>
#include "./mtk-phy_hook.h"


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


/************************************************************************
*                  M A C R O S
*************************************************************************
*/



/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/


/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
static const struct mtk_usb_phy_ops *ops;

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void ecnt_u3h_phy_init(void)
{
	if(ops->init != NULL)
	{
		ops->init();
	}
	else
	{
		printk("mtk_usb_phy_init ops is NULL!\n");
	}
}
EXPORT_SYMBOL(ecnt_u3h_phy_init);

void ecnt_u3h_phy_PowerDown(unsigned int port_num)
{
	if(ops->PowerDown != NULL)
	{
		ops->PowerDown(port_num);
	}
	else
	{
		printk("ecnt_u3h_phy_PowerDown ops is NULL!\n");
	}
}
EXPORT_SYMBOL(ecnt_u3h_phy_PowerDown);

void ecnt_u3h_phy_PowerUp(unsigned int port_num)
{
	if(ops->PowerUp != NULL)
	{
		ops->PowerUp(port_num);
	}
	else
	{
		printk("ecnt_u3h_phy_PowerUp ops is NULL!\n");
	}
}
EXPORT_SYMBOL(ecnt_u3h_phy_PowerUp);

int mtk_usb_phy_ops_init(const struct mtk_usb_phy_ops *ops_ptr)
{

	if((ops_ptr != NULL) && (ops_ptr->init != NULL) && 
		(ops_ptr->PowerDown != NULL) && (ops_ptr->PowerUp != NULL))
	{
	
		ops = ops_ptr;
	}
	else
	{
		printk("mtk_usb_phy_ops_init fail!\n");
		return -1;
	}

	return 0;
}

