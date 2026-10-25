/***************************************************************
Copyright Statement:

This software/firmware and related documentation (??EcoNet Software??) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (??EcoNet??) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (??ECONET SOFTWARE??) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ??AS IS?? 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER??S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER??S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/kernel.h>
#include <asm/io.h>
#include "hub.h"
#include "ecnt_drivers_usb_core.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
/******************
usb 2.0 port status definition which is the same with ehci.h.
	shnwind add 20101012.
***************/
#define PORT_RESET	(1<<8)		/* reset port */
#define PORT_SUSPEND	(1<<7)		/* suspend port */
#define PORT_RESUME	(1<<6)		/* resume it */
#define PORT_OCC	(1<<5)		/* over current change */
#define PORT_OC		(1<<4)		/* over current active */
#define PORT_PEC	(1<<3)		/* port enable change */
#define PORT_PE		(1<<2)		/* port enable */
#define PORT_CSC	(1<<1)		/* connect status change */
#define PORT_CONNECT	(1<<0)		/* device connected */
/***********************************/
#define USB_PORT0_STAT_20_ADDR 0xbfba1064
#define USB_PORT1_STAT_20_ADDR 0xbfba1068

#define USB_PORT0_STAT_11_ADDR 0xbfba0054
#define USB_PORT1_STAT_11_ADDR 0xbfba0058
#define OHCI_USB_CONTROL_ADDR 0xbfba0004

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

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
int power_saving_mode(int mode);

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
int power_saving_mode(int mode)
{
	unsigned long x;

	if(mode == 0){
		/* clear bit7,6 of 0xc0000000 (after USB flash is unpluged) 
		 * to reset rootHub and SIE of usb1.1, so that power saving
		 * mode can work properly  --Trey */
		 if(((readl((void *)USB_PORT0_STAT_20_ADDR) & PORT_CONNECT) == 0) 
			 && ((readl((void *)USB_PORT1_STAT_20_ADDR) & PORT_CONNECT) == 0)
			 && ((readl((void *)USB_PORT0_STAT_11_ADDR) & PORT_CONNECT) == 0)
			 && ((readl((void *)USB_PORT1_STAT_11_ADDR) & PORT_CONNECT) == 0)){
			 	/* printk("SET USB 11 RESET\n"); */
				x = readl((void *)OHCI_USB_CONTROL_ADDR);
				x &= ~((1<<7) | (1<<6));
				writel(x, (void *)OHCI_USB_CONTROL_ADDR);
		 }
	}else if(mode == 1){
		/* set bit7 and clear bit6 of 0xc0000000 (when USB flash is pluged) 
		 * to set to normal mode for rootHub and SIE of usb1.1, so that power 
		 * saving mode can work properly  --Trey */
		x = readl((void *)OHCI_USB_CONTROL_ADDR);
		x |= (1<<7);
		x &= ~(1<<6);
		writel(x, (void *)OHCI_USB_CONTROL_ADDR);
		/* printk("SET USB 11 OPERATION\n"); */
	}
	return 0;
}

int ecnt_hub_port_connect_inline_hook
(struct usb_device *udev)
{
#ifdef TCSUPPORT_USB_HOST_LED
	struct usb_hub	*hub = NULL;
	unsigned char maxchild = 0;
	
	if(TCSUPPORT_XPON_HAL_API_VAL)
	{
		hub = usb_hub_to_struct_hub(udev);
		if(hub){
			maxchild = hub->descriptor->bNbrPorts;
			if(maxchild == 4)
				hubflag = 1;
		}
		if(!strcmp(dev_name(&udev->dev), "1-1.1"))
			hubflag = 2;
	}
	if ( udev && udev->phyportnum )
	{
		if(TCSUPPORT_XPON_HAL_API_VAL)
		{
			if(udev->phyportnum ==USBPHYPORT1 && (hubflag == 1 || !strcmp(dev_name(&udev->dev), "1-1.2")))
				return ECNT_CONTINUE;
			
			if ( udev->phyportnum >= USBPHYPORT1
				&& udev->phyportnum <= USBPHYPORT2 )
				pre_usb_state[udev->phyportnum - 1] = USB_CONNECT;

			if ( Usb_Led_Flash_Op_hook )
				Usb_Led_Flash_Op_hook(USB_CONNECT, udev->phyportnum);	
		}
		else{
		if ( udev->phyportnum >= USBPHYPORT1
			&& udev->phyportnum <= USBPHYPORT2 )
			pre_usb_state[udev->phyportnum - 1] = USB_CONNECT;

		if ( Usb_Led_Flash_Op_hook )
			Usb_Led_Flash_Op_hook(USB_CONNECT, udev->phyportnum);	
		}
	}
#endif

	return ECNT_CONTINUE;
}
