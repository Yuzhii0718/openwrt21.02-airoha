
#ifndef _ECNT_USXGMII_H_
#define _ECNT_USXGMII_H_

//#include "sgmii_reg.h"
//#include "sgmii_hal_top.h"
#include "../sgmii_globaldef.h"

/************************************************************************
*                  define
*************************************************************************
*/

//#define SGMII_VERSION "A7581.00.00"
/************************************************************************
*                  extern function
*************************************************************************
*/
//int get_serdes_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t sel);
//void SET_SSR3(u32 val);
//u32 GET_SSR3(void);
//u32 get_chip_scu_data(u32 reg);
//void set_chip_scu_data(u32 reg, u32 val);


/************************************************************************
*                  M A C R O S
*************************************************************************
*/	
#define rg_type_t(type)\
		union type##_t {\
		type hal;\
		RGDATA_t dat;\
		}
/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
typedef struct{
	struct device *dev;
	void __iomem *pcs1_base;
	void __iomem *pcs2_base;
	void __iomem *an_base;
	void __iomem *ra_base;
	void __iomem *multi_sgmii_base;
	void __iomem *phya_base;	
	void __iomem *pma_base;	
	int irq;
}xsgmii_base;

typedef struct{
	struct device *dev;
	void __iomem *pcs1_base[2]; //xfi
	void __iomem *pcs2_base[2]; //sgmii
	void __iomem *an_base[2]; //sgmii
	void __iomem *ra_base[2]; //sgmii
	void __iomem *multi_sgmii_base[2]; //sgmii
	void __iomem *phya_base;//pxp	
	void __iomem *pma_base[2];	
	int irq;
}p2l_xsgmii_base;


typedef struct {
	//xsgmii_base pcie_L0;
	xsgmii_base eth; 
	//xsgmii_base xfi; 
}usxgmii_port;


typedef enum {
	USXGMII_PORT_ETH,
	//SGMII_PORT_PCIE1,
	//SGMII_PORT_USB0,
	//SGMII_PORT_PON0,
	USXGMII_PORT_UNKNOW
}usxgmii_port_type;


typedef enum{
	USXGMII_REG_PCS1,
	USXGMII_REG_PCS2,
	USXGMII_REG_AN,
	USXGMII_REG_RATEADAPT,
	USXGMII_REG_PHYA,
	USXGMII_REG_UNKNOW
}usxgmii_reg_type;


typedef enum{
	USXGMII_API_METHOD_GET,
	USXGMII_API_METHOD_SET,
	USXGMII_API_METHOD_UNKNOW
}usxgmii_api_method_type;

typedef enum{
	USXGMII_API_MODE,
	USXGMII_API_INFO,
	USXGMII_API_TEST,
	USXGMII_API_VERSION,
	USXGMII_API_UNKNOW
}usxgmii_api_type;

typedef enum{
	ENUM_USXGII_10G,
	ENUM_USXGII_5G,
	ENUM_USXGII_2p5G,
	ENUM_USXGII_1G,
	ENUM_USXGII_100M,
	ENUM_USXGII_UNKNOW,
}USXGMII_RATE_t;


typedef enum{
	USXGMII,
	HSGMII,
	SGMII,
}xSGMII_t;

typedef enum{
	ENUM_SGMII_1000M,
	ENUM_SGMII_100M,
	ENUM_SGMII_10M,
	ENUM_SGMII_UNKNOW,
}SGMII_RATE_t;	

enum RST{
	eALLPCS,
	eRXPCS,
	eTXPCS,
	eTXFIFO,
	eREF,
	ePMA,
	eRX,
	eTX,
	eM1,
	eM2,
};	

enum {OFF,ON};
enum {B_ON,B_OFF};

/************************************************************************
*                   
*************************************************************************
*/

#endif //_ECNT_SGMII_H_



