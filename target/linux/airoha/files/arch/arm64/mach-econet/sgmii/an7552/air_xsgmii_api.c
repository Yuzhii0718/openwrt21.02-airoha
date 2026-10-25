/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/

#include <linux/kernel.h>   
#include <linux/types.h>
#include "../sgmii_globaldef.h"
#include "H/air_xpon_xsgmii.h"

/************************************************************************
*                  E X T E R N A L   V A R I A B L E   D E C L A R A T I O N S
*************************************************************************
*/

u8 dbg_print = 0;
EXPORT_SYMBOL(dbg_print);


/************************************************************************
*                  A P I s
*************************************************************************
*/

/*************************************************************************************************************************
*  proc command:	echo 'p1 p2 p3 p4 p5'>proc/xsgmii
*
*  proc parameter:
* 	p1:serdes:	0=version,					1=ETH_API(not supported),	2=XPON_API, 					3=PCIE0_API(not supported), 
*				4=PCIE1_API(not supported),	5=USB_API(not supported),	6=XPON_RW_API(not supported),	7=XPON_Test(not supported)
*	
*	p2:xsgmii:	0=USXGMII(not supported), 		1=HSGMII, 				2=SGMII
*	p3:mod:		0=force mode, 				1=an mode, 				2=Link status,					3=fpga verfity
*	p4:an:		0=off,						1=on
*	p5:rate:		0=1G, 						1=100M, 					2=10M
*
**************************************************************************************************************************
*/
 
u32 xsgmii_api(u8 serdes,u8 xsgmii,u8 mod,u8 rate,u8 an){
	u32 status = 0x00;
	if(dbg_print) printk("xsgmii_api: serdes %x,xsgmii %x,mod %x,rate %x,an %x\n",serdes, xsgmii, mod, rate, an);
	switch(serdes){
		case 0 :
			switch(xsgmii){
				case 1:
					printk("\nNot supported.(ETH_xSGMII_Ver)\n");
					//ETH_xSGMII_Ver();					
					break;
				case 2:
					XPON_xSGMII_Ver(); 
					break;
				case 3:
				case 4:
					printk("\nNot supported.(PCIE_xSGMII_Ver)\n");
					//PCIE0_xSGMII_Ver();
					break;
				case 5:
					XPON_xSGMII_Ver();
					break;
				case 6:
					dbg_print = mod;
					break;
				default:
					status = 0xff;
					//ETH_xSGMII_Ver();
					break;
			}	
			break;
		case 1 :
			printk("\nNot supported.(ETH_xSGMII)\n");
			status = 0xff;
			//status = ETH_xSGMII_API(xsgmii,mod,an,rate); 
			break;
		case 2 :
			status = XPON_xSGMII_API(xsgmii,mod,an,rate);
			break;
		case 3 :
			printk("\nNot supported.(PCIE0_xSGMII)\n");
			status = 0xff;
			//status = PCIE0_xSGMII_API(xsgmii,mod,an,rate);
			break;
		case 4 :
			printk("\nNot supported.(PCIE1_xSGMII)\n");
			status = 0xff;
			//status = PCIE1_xSGMII_API(xsgmii,mod,an,rate);
			break;
		case 5 :
			printk("\nNot supported.(USB_xSGMII)\n");
			status = 0xff;
			//status = USB_xSGMII_API(xsgmii,mod,an,rate);
			break;
		case 6 :
			printk("\nNot supported.(XPON_RG_RW_API)\n");
			//XPON_RG_RW_API(1,(RgAddr)(an<<8)+rate,0);
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
					printk("\nNot supported.(ETH_Test)\n");
					status = 0xff;
					//ETH_Test(mod,an,rate);
					break;
				case 1:
					printk("\nNot supported.(XPON_Test)\n");
					//XPON_Test(mod,an,rate);
					status = 0xff;
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

