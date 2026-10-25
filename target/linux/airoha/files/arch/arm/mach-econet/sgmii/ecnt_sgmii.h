
#ifndef _ECNT_SGMII_H_
#define _ECNT_SGMII_H_

#include "sgmii_reg.h"
#include "sgmii_hal_top.h"
#include "sgmii_globaldef.h"



/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define PCS1_BASE_OFFSET       0x100
#define PCS2_BASE_OFFSET       0xA00
#define AN_BASE_OFFSET         0x000
#define RATEADAPT_BASE_OFFSET  0x000 
#define PHYA_BASE_OFFSET       0x000 

#define SGMII_VERSION "A7523.00.04" //EN7523_20231013
#define DEBUG	0

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
	void __iomem *phya_base;
	int irq;
	int speed;
	int an_state;
	int link_status;
}sgmii_base;

typedef struct {
	sgmii_base pcie0;
	sgmii_base pcie1; 
	sgmii_base usb0; 
}sgmii_port;


typedef enum {
	SGMII_PORT_PCIE0,
	SGMII_PORT_PCIE1,
	SGMII_PORT_USB0,
	SGMII_PORT_PON0,
	SGMII_PORT_UNKNOW
}sgmii_port_type;


typedef enum{
	SGMII_REG_PCS1,
	SGMII_REG_PCS2,
	SGMII_REG_AN,
	SGMII_REG_RATEADAPT,
	SGMII_REG_PHYA,
	SGMII_REG_UNKNOW
}sgmii_reg_type;

typedef enum{
	SGMII_SPEED_2500M,
	SGMII_SPEED_1000M,
	SGMII_SPEED_100M,
	SGMII_SPEED_10M,
	SGMII_SPEED_AN,
	SGMII_SPEED_UNKNOW
}sgmii_speed_type;


typedef enum{
	SGMII_API_METHOD_GET,
	SGMII_API_METHOD_SET,
	SGMII_API_METHOD_UNKNOW
}sgmii_api_method_type;

typedef enum{
	SGMII_API_MODE,
	SGMII_API_INFO,
	SGMII_API_TEST,
	SGMII_API_VERSION,
	SGMII_API_UNKNOW
}sgmii_api_type;
/************************************************************************
*                  P U B L I C    F U N C T I O N
*************************************************************************
*/

#endif //_ECNT_SGMII_H_



