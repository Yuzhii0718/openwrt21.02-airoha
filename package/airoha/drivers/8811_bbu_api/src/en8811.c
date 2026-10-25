/* FILE NAME:  en8811.c
 *
 * PURPOSE:
 *      This driver is for Airoha MDIO bus controller.
 *      It can handle multiple PHYs of AN8811.
 *
 * NOTES:
 *   Version 2.0 is compatibel with 1.x.
 *   It need to use the global varible from mdio_arht.
 *
 */
#define pr_fmt(fmt) "EN8811@ " fmt
/************************************************************************
*                       I N C L U D E S                                 *
*************************************************************************
*/
#include <linux/delay.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/phy.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <asm/tc3162/tc3162.h>
#include <linux/module.h>
#ifdef CONFIG_TP_IMAGE
#include <linux/netdevice.h>
#endif /* CONFIG_TP_IMAGE */

#include "air_en8811h.h"
#include "air_en8811h_api.h"
#include "en8811.h"
#include "en8811_api.h"
#include "en8811_reg.h"
#include "en8811_proc.h"
#include "en8811_hook.h"

#include "ecnt_event_global/ecnt_event_system.h"
/************************************************************************
*           D A T A   D E C L A R A T I O N S                           *
*************************************************************************
*/
struct arht_mdio_priv {
	struct mii_bus *mii_bus;
};
extern struct arht_mdio_priv *mdio_priv_p;

Mdio_priv_data_t *en8811_priv_p = NULL;
int debuglevel = 0;
module_param(debuglevel, int,S_IRUGO);

/************************************************************************
*          S T A T I C    I  L I N E    F U N C T I O N                 *
*************************************************************************
*/
static int en8811_set_parameter(Mdio_priv_data_t *mdio_priv_data_p)
{
    struct phy_device* phy_dev = NULL;
    uint8_t all_phyAddrs_num = 0, i = 0, phyAddr = INVALID_PHY_ADDR;

    all_phyAddrs_num = MDIO_PHY_NUM(mdio_priv_data_p);
    for(i = 0 ; i < all_phyAddrs_num; i++){
        phyAddr = MDIO_REG_ADDR(mdio_priv_data_p, i);
        EN8811_LOG(EN8811_LOG_DBG,"phy[%d] addr= %d\n",i,phyAddr);

        phy_dev = GET_PHY_DEV(mdio_priv_data_p, phyAddr);
        if(!CHECK_PHY_DEV(phy_dev)){
            return -1;
        }

        if (isEN7580){
            printk("7580 set minIPG to 9\n");
            air_buckpbus_reg_write(phy_dev, 0xe002c, 0x09090505);
	    }
        else{
            EN8811_LOG(EN8811_LOG_INFO,"set minIPG to 5\n");
            air_buckpbus_reg_write(phy_dev, 0xe002c, 0x05050505);
        }
    }

    return 0;
}


/************************************************************************
*                  MODULES INIT AND EXIT FUNCTIONS                      *
*************************************************************************
*/
int __init en8811_bbu_api_module_init(void)
{
    int ret = 0;
    struct mii_bus *mdio_mii_bus = NULL;

    /*Create and save bus pointer to global variable */
    en8811_priv_p = (Mdio_priv_data_t*)mem_alloc(sizeof(Mdio_priv_data_t));
    if (en8811_priv_p == NULL) {
        pr_err(" Failed to allocate memory for en8811_priv_p\n");
        return -ENOMEM;
    }

    if(mdio_priv_p){
        mdio_mii_bus = mdio_priv_p->mii_bus;
    }
    else{
        mem_free(en8811_priv_p);
        return -EFAULT;
    }

    if(mdio_mii_bus){
        MDIO_BUS_PRT(en8811_priv_p) = mdio_mii_bus;
    }
    else{
        mem_free(en8811_priv_p);
        return -EFAULT;
    }

    ret = init_en8811_priv(en8811_priv_p);
    if(ret == EN8811_FALSE){
        mem_free(en8811_priv_p);
        pr_err("Failed to initialize data structure!\n");
        return ret;
    }

    ret = en8811_set_parameter(en8811_priv_p);
    if(ret){
        pr_warn("Failed to set parameter!\n");
    }

    en8811_proc_init();
    en8811_hook_init();

    pr_info("en8811_bbu_api(%s) module init ok!\n",EN8811_BBU_API_VERSION);
    return 0;
}

void __exit en8811_bbu_api_module_exit(void)
{
    en8811_hook_exit();
    en8811_proc_exit();
    mem_free(en8811_priv_p);

    
    pr_info("en8811_bbu_api module exit ok!\n");
}

module_init(en8811_bbu_api_module_init);
module_exit(en8811_bbu_api_module_exit);
