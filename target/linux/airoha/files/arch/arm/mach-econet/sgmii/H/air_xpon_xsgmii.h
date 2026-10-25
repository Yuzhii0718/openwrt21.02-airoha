#ifndef _AIR_XPON_XSGMII_
#define _AIR_XPON_XSGMII_
#include "../sgmii_globaldef.h"
u32 XPON_xSGMII_API(u8 xsgmii,u8 mod,u8 an, u8 rate);
void XPON_xSGMII_Ver(void);
void XPON_RG_RW_API(u8 R,RgAddr rg,u32 data);
void XPON_Test(u8 mod,u8 an, u8 rate);
#endif

