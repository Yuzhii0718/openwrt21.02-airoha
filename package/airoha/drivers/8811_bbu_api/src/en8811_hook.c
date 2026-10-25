/* FILE NAME:  en8811_hook.c
 * PURPOSE:
 *    It provides en8811 ecnt hook definations.
 *
 * NOTES:
 *
 */
 #define pr_fmt(fmt) "EN8811@ " fmt
/************************************************************************
*                       I N C L U D E S                                 *
*************************************************************************
*/
#include <linux/module.h>
#include <linux/kernel.h>

#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_air_en8811.h>

#include "en8811_api.h"
#include "en8811_hook.h"
#include "en8811_reg.h"
#include "air_en8811h.h"
#include "air_en8811h_api.h"

/************************************************************************
*           D A T A   D E C L A R A T I O N S                           *
*************************************************************************
*/
bool glb_autoneg_state[SERDES_MAX_IDX] = {0};
extern Mdio_priv_data_t *en8811_priv_p ;
uint8_t en8811_get_link_info(uint8_t phy_addr, uint32_t* speed_index, uint32_t* duplex);
void get_default_ItfName(Mdio_priv_data_t *mdio_priv_data_p, char* phyItfName);

/************************************************************************
*          S T A T I C    I  L I N E    F U N C T I O N                 *
*************************************************************************
*/
static uint8_t air_en8811_power_up(uint8_t up, uint8_t phy_addr)
{
    unsigned int val = 0;

    if(phy_addr > INVALID_PHY_ADDR){
        return -1;
    }

    val = ETHER_MDIO_READ(phy_addr, MII_BMCR) & ~BIT(11);
    if(!up)
        val |= BIT(11);
    ETHER_MDIO_WRITE(phy_addr, MII_BMCR, val);

    return 0;
}

static int air_en8811_api_power_up(ECNT_AIR_EN8811_Data_s *en8811_data)
{ 
    uint8_t power_up = 0, phyAddr = INVALID_PHY_ADDR;
    
    power_up = en8811_data->air_en8811_private.power_up;

    if(!MDIO_PHY_NUM(en8811_priv_p)){
        return -1;
    }
    phyAddr = MDIO_PHY_DEFAULT(en8811_priv_p); /*take default phy address*/
    if(GET_PHY_STATUS(en8811_priv_p,phyAddr) == EN8811_PHY_OFF){
        return -1;
    }

    air_en8811_power_up(power_up,phyAddr);
    
    return 0;
}

static int air_en8811_get_link_rate(ECNT_AIR_EN8811_Data_s *en8811_data)
{ 
    uint32_t speed_index = 0, duplex = 0;
    uint8_t phyAddr = INVALID_PHY_ADDR;

    if(!MDIO_PHY_NUM(en8811_priv_p)){
        return -1;
    }
    phyAddr = MDIO_PHY_DEFAULT(en8811_priv_p); /*take default phy address*/
    if(GET_PHY_STATUS(en8811_priv_p,phyAddr) == EN8811_PHY_OFF){
        return -1;
    }
    
    if(en8811_get_link_info(phyAddr, &speed_index, &duplex)){
        return -1;
    }
    
    en8811_data->air_en8811_private.link_info.speed_index = speed_index;
    en8811_data->air_en8811_private.link_info.duplex = duplex;

    return 0;
}

static int en8811_multi_set_power_up(ECNT_AIR_EN8811_Data_s *en8811_data)
{ 
    uint8_t power_up = 0, phyAddr = INVALID_PHY_ADDR, serdesID = INVALID_SERDES_ID;

    power_up = en8811_data->air_en8811_private.power_up;
    serdesID = en8811_data->serdes_id;
    if(serdesID >= INVALID_SERDES_ID){
        return -1;
    }
    
    phyAddr = GET_SERDES_PHYADDR(serdesID);
    if(GET_PHY_STATUS(en8811_priv_p,phyAddr) == EN8811_PHY_OFF){
        return -1;
    }
    
    air_en8811_power_up(power_up,phyAddr);
    
    return 0;
}

static int en8811_multi_get_link_rate(ECNT_AIR_EN8811_Data_s *en8811_data)
{ 
    uint32_t speed_index = 0, duplex = 0;
    uint8_t phyAddr = INVALID_PHY_ADDR, serdesID = INVALID_SERDES_ID;

    serdesID = en8811_data->serdes_id;
    if(serdesID >= INVALID_SERDES_ID){
        return -1;
    }

    phyAddr = GET_SERDES_PHYADDR(serdesID);
    if(GET_PHY_STATUS(en8811_priv_p,phyAddr) == EN8811_PHY_OFF){
        return -1;
    }
    
    if(en8811_get_link_info(phyAddr, &speed_index, &duplex)){
        return -1;
    }
    
    en8811_data->air_en8811_private.link_info.speed_index = speed_index;
    en8811_data->air_en8811_private.link_info.duplex = duplex;

    return 0;
}

int multi_en8811_get_link_status(ECNT_AIR_EN8811_Data_s *en8811_data)
{
	uint8_t phyAddr = INVALID_PHY_ADDR, serdesID = INVALID_SERDES_ID;
	int link_state = 0;
	uint32_t speed_index = 0, duplex = 0;
	serdesID = en8811_data->serdes_id;
    if(serdesID >= INVALID_SERDES_ID){
        return -1;
    }
    phyAddr = GET_SERDES_PHYADDR(serdesID);
    if(GET_PHY_STATUS(en8811_priv_p,phyAddr) == EN8811_PHY_OFF){
        return -1;
    }
    if(en8811_get_link_info(phyAddr, &speed_index, &duplex)){
        return -1;
    }
	if ( speed_index != EN8811_LINK_DOWN)
	{
		link_state = 1;
	}
	else 
		link_state = 0;
	EN8811_LOG(EN8811_LOG_DBG,"serdes_id: %d, link_state: %d\n", en8811_data->serdes_id, link_state);
	en8811_data->air_en8811_private.link_info.link_state = link_state;
	en8811_data->air_en8811_private.link_info.autoneg_state = glb_autoneg_state[en8811_data->serdes_id];
	return 0;
}
/************************************************************************
*                  G L O B A L   F U N C T I O N S                      *
*************************************************************************
*/
uint8_t en8811_get_link_info(uint8_t phy_addr, uint32_t* speed_index, uint32_t* duplex)
{
    int ret = 0;
    uint32_t speed_index_tmp = 0, duplex_tmp = 0;
    struct phy_device* en8811_phy_dev = NULL;

    en8811_phy_dev = GET_PHY_DEV(en8811_priv_p, phy_addr);
    if(en8811_phy_dev == NULL){
        EN8811_LOG(EN8811_LOG_DUMP_DATA,"en8811_phy_dev is error! \n");
        return -1;
    }
    ret = en8811h_read_status(en8811_phy_dev);
    if(ret < 0){
        EN8811_LOG(EN8811_LOG_DUMP_DATA,"en8811h_read_status ret is error! \n");
        return -1;
    }
    
    duplex_tmp = en8811_phy_dev->duplex;
    switch(en8811_phy_dev->speed){
        case SPEED_2500:
        {
            speed_index_tmp = EN8811_2P5G;
            break;
        }

        case SPEED_1000:
        {
            speed_index_tmp = EN8811_1G;
            break;
        }

        case SPEED_100:
        {
            speed_index_tmp= EN8811_100M;
            break;
        }
    }

    *speed_index = speed_index_tmp;
    *duplex = duplex_tmp;

    return 0;
}

/************************************************************************
*         E C N T  H O O K  D E C L A R A T I O N S                     *
*************************************************************************
*/
air_en8811_api_op_t
air_en8811_operation[]=
{
    air_en8811_api_power_up,
    air_en8811_get_link_rate,
    en8811_multi_set_power_up,
    en8811_multi_get_link_rate,
	multi_en8811_get_link_status,
};

ecnt_ret_val ecnt_air_en8811_hook(struct ecnt_data *indata)
{
    ECNT_AIR_EN8811_Data_s *en8811_data = (ECNT_AIR_EN8811_Data_s *)indata ;

    if(en8811_data->function_id >= AIR_EN8811_FUNCTION_MAX_NUM) {
        return ECNT_HOOK_ERROR;
    }

    en8811_data->retValue = air_en8811_operation[en8811_data->function_id](en8811_data) ;
    
    return ECNT_CONTINUE;
}

struct ecnt_hook_ops ecnt_driver_air_en8811_op = {
    .name = "driver_AIR_EN8811_hook",
    .hookfn = ecnt_air_en8811_hook,
    .maintype = ECNT_AIR_EN8811,
    .is_execute = 1,
    .subtype = ECNT_DRIVER_API,
    .priority = 1
};

/************************************************************************
*                     HOOK INIT AND EXIT FUNCTIONS                      *
*************************************************************************
*/
uint8_t en8811_hook_init(void)
{
	memset(glb_autoneg_state,1, sizeof(glb_autoneg_state));
    if(ecnt_register_hook(&ecnt_driver_air_en8811_op)) {
        pr_err("en8811_hook init failed!\n");
        return -ENODEV;
    }

    pr_info("en8811_hook init ok!\n");
    return 0;
}

uint8_t en8811_hook_exit(void)
{
    ecnt_unregister_hook(&ecnt_driver_air_en8811_op);
    
    pr_info("en8811_hook exit ok!\n");
    return 0;
}
