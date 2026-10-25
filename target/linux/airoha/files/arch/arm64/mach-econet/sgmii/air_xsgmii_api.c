#include <linux/kernel.h>   
#include <linux/types.h>
#include "sgmii_globaldef.h"
#include "H/air_eth_xsgmii.h"
#include "H/air_xpon_xsgmii.h"
#include "H/air_usb_xsgmii.h"
#include "H/air_pcie2l_xsgmii.h"
u8 dbg_print = 0;
EXPORT_SYMBOL(dbg_print);
RGDATA_t api_aux ={(0b11111111<<24)|(0b11111111<<16)|(0b11111111<<8)|(0b11111100<<0)};
EXPORT_SYMBOL(api_aux);
 
u32 xsgmii_api(u8 serdes,u8 xsgmii,u8 mod,u8 rate,u8 an){
	u32 status = 0x00;
	if(dbg_print && (mod!=3) && (mod!=4)) printk("xsgmii_api: serdes %x,xsgmii %x,mod %x,rate %x,an %x\n",serdes, xsgmii, mod, rate, an);
	switch(serdes){
		case 0 :
			switch(xsgmii){
				case 1:
					ETH_xSGMII_Ver();					
					break;
				case 2:
					XPON_xSGMII_Ver(); 
					break;
				case 3:
				case 4:
					PCIE0_xSGMII_Ver();
					break;
				case 5:
					USB_xSGMII_Ver();
					break;
				case 6:
					dbg_print = mod;
					break;
				case 7:					
					if(rate) api_aux.value |= 1<<an;
					else api_aux.value &= ~(1<<an);
					printk("api_aux = %x\n",api_aux.value);
					break;
				default:
					status = 0xff;
					ETH_xSGMII_Ver();
					break;
			}	
			break;
		case 1 :
			status = ETH_xSGMII_API(xsgmii,mod,an,rate); 
			break;
		case 2 :
			status = XPON_xSGMII_API(xsgmii,mod,an,rate);
			break;
		case 3 :
			status = PCIE0_xSGMII_API(xsgmii,mod,an,rate);
			break;
		case 4 :
			status = PCIE1_xSGMII_API(xsgmii,mod,an,rate);
			break;
		case 5 :
			status = USB_xSGMII_API(xsgmii,mod,an,rate);
			break;
		case 6 :
			XPON_RG_RW_API(1,(RgAddr)(an<<8)+rate,0);
			break;
		case 7 :
			/*
			proc parameter:
				p1 serdes
				p2 xsgmii: 0 eth,1 xpon...
				p3 mod: function test
				p4 an : 0 jcpll 
				p5 rate : 0
			*/
			switch(xsgmii){
				case 0:
					ETH_Test(mod,an,rate);
					break;
				case 1:
					XPON_Test(mod,an,rate);
					break;
			}		
			break;
			
		default :
			status = 0xff;
			printk("xsgmii_api !!!\n");
	}	
	
	return status;
}
EXPORT_SYMBOL(xsgmii_api);

