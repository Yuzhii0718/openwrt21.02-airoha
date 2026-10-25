#ifndef _LINUX_ECNT_DRIVERS_USB_CORE_H
#define _LINUX_ECNT_DRIVERS_USB_CORE_H
#include <linux/kernel.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#include <linux/kmemcheck.h>
#endif
#include <linux/compiler.h>
#include <linux/time.h>
#include <linux/bug.h>
#include <linux/cache.h>
#include <linux/atomic.h>
#include <asm/types.h>
#include <linux/usb.h>
#include <ecnt_hook/ecnt_hook.h>
#ifdef TCSUPPORT_USB_HOST_LED
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#endif
#include <linux/libcompileoption.h>

extern int ecnt_hub_port_connect_inline_hook(struct usb_device *udev);
#ifdef TCSUPPORT_USB_HOST_LED
extern void (*Usb_Led_Flash_Op_hook)(unsigned int opmode ,unsigned int phyport);
extern int pre_usb_state[2];
static int hubflag;
#endif

static inline int ecnt_usb_disconnect_inline_hook
(struct usb_device *udev)
{

#ifdef TCSUPPORT_USB_HOST_LED
	if ( udev && udev->phyportnum )
	{
		if ( udev->phyportnum >= USBPHYPORT1
			&& udev->phyportnum <= USBPHYPORT2 )
			pre_usb_state[udev->phyportnum - 1] = USB_DISCONNECT;

		if ( Usb_Led_Flash_Op_hook )
				Usb_Led_Flash_Op_hook(USB_DISCONNECT, udev->phyportnum); 
	}
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_usb_submit_urb_inline_hook
(struct usb_device *udev)
{
#ifdef TCSUPPORT_USB_HOST_LED
	if ( udev && udev->phyportnum)
	{
		if(TCSUPPORT_XPON_HAL_API_VAL)
		{ 
			if(udev->phyportnum ==USBPHYPORT1 && (hubflag == 1 || !strcmp(dev_name(&udev->dev), "1-1.2")))
				return ECNT_CONTINUE;

			if ( Usb_Led_Flash_Op_hook )
				Usb_Led_Flash_Op_hook(USB_BLINK, udev->phyportnum);	
		}
		else{
		if ( udev->phyportnum >= USBPHYPORT1
			&& udev->phyportnum <= USBPHYPORT2 )
			pre_usb_state[udev->phyportnum - 1] = USB_BLINK;

		if ( Usb_Led_Flash_Op_hook )
			Usb_Led_Flash_Op_hook(USB_BLINK, udev->phyportnum);	
		}
	}
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_usb_alloc_dev_inline_hook
(struct usb_device *parent, struct usb_device *dev, unsigned port1)
{
	if ( !dev )
		return ECNT_CONTINUE;

#ifdef TCSUPPORT_USB_HOST_LED
	if ( unlikely(!parent) )
	{
		dev->phyportnum = port1;
	}
	else
	{
		if(isEN7580 || isEN7581){
			if((dev->bus->busnum == 1) || (dev->bus->busnum == 2))
				dev->phyportnum = USBPHYPORT1;
			else
				dev->phyportnum = USBPHYPORT2;
		}else{
			dev->phyportnum = dev->devpath[0] - 48;
		}
	}
#endif

	return ECNT_CONTINUE;
}

static inline int ecnt_usb_proc_clearhalt_dev_inline_hook(struct usb_device *dev)
{
	struct usb_hcd *hcd = NULL;	

	/* USB3 Host will ignore the usb_clear_halt requested by up-layer */
	hcd = bus_to_hcd(dev->bus);
	if (hcd->driver->flags & HCD_USB3)
	{
		return ECNT_RETURN_DROP;
	}

	return ECNT_CONTINUE;
}

#endif

