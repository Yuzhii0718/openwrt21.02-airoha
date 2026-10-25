/* FILE NAME:  en8811_api.c
 * PURPOSE:
 *        This file contains the API functions to set or get en8811_priv_p structure used by the driver.
 * NOTES:
 *
 */
#define pr_fmt(fmt) "EN8811@ " fmt
/************************************************************************
*                  I N C L U D E S                                      *
*************************************************************************
*/    
#include <linux/string.h>
#include <linux/kernel.h>

#if defined(TCSUPPORT_CPU_EN7523)
#include "ecnt_event_global/ecnt_event_system.h"
#endif

#include "en8811_api.h"
#include "en8811_ic_dis.h"

/************************************************************************
*                  D A T A   D E C L A R A T I O N S                    *
*************************************************************************
*/
extern Mdio_priv_data_t *en8811_priv_p ;

extern char *hsgmii_pcie0_itf;
extern char *hsgmii_pcie1_itf;
extern char *hsgmii_usb_itf;
extern char *hsgmii_eth_itf;
extern char *hsgmii_pon_itf;

extern int get_8811phy_addr(unsigned char *phyaddr, int len);

#if SUPPORT_BOARD_SELECT
extern ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t get_serdes_phy_sel(uint8_t port_idx);
#else
extern ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t get_serdes_phy_sel(ECNT_EVENT_SYSTEM_SERDES_SEL_t port_idx);
#endif

char *serdes_info[] ={
    [SERDES_PCIE0_IDX] = "SERDES_PCIE0",
    [SERDES_PCIE1_IDX] = "SERDES_PCIE1",
    [SERDES_USB_IDX] = "SERDES_USB",
    [SERDES_AE_IDX] = "SERDES_PON",
    [SERDES_ETH_IDX] = "SERDES_ETH",
};

PhySerdes_t serdes_map[SERDES_MAX_IDX] ={
    [SERDES_PCIE0_IDX] = {.phy_addr = 8, .serdes_id = SERDES_PCIE0_IDX},
    [SERDES_PCIE1_IDX] = {.phy_addr = 0, .serdes_id = SERDES_PCIE1_IDX},  /*Useless, it is reserved in order to be consistent with the serdes index configuration of hsgmii*/
    [SERDES_USB_IDX] = {.phy_addr = 15, .serdes_id = SERDES_USB_IDX},
    [SERDES_AE_IDX] = {.phy_addr = 14, .serdes_id = SERDES_AE_IDX},
    [SERDES_ETH_IDX] = {.phy_addr = 13, .serdes_id = SERDES_ETH_IDX},
};

char *itfname_info[MAX_SERDES_NUM];

uint8_t serdes_map_by_ordes[MAX_SERDES_NUM] = {3,4,0,1,2}; /*pon-eth-wifi1-wifi2-usb1*/
uint8_t serdes_map_by_ordes_num =  sizeof(serdes_map_by_ordes)/sizeof(serdes_map_by_ordes[0]);

/************************************************************************
*            S T A T I C    I  L I N E    F U N C T I O N               *
*************************************************************************
*/
static inline void set_itfname_info(void)
{
    itfname_info[SERDES_PCIE0_IDX] = hsgmii_pcie0_itf;
    itfname_info[SERDES_PCIE1_IDX] = hsgmii_pcie1_itf;
    itfname_info[SERDES_USB_IDX] = hsgmii_usb_itf;
    itfname_info[SERDES_AE_IDX] = hsgmii_pon_itf;
    itfname_info[SERDES_ETH_IDX] = hsgmii_eth_itf;
}

static inline char* GET_PHY_ITFNAME(Serdes_id_t serdesID)
{
    if ( serdesID >= 0 && serdesID < SERDES_MAX_IDX){
        return itfname_info[serdesID];
    }
    else{
        return INVALID_ITF_NAME;
    }
} 

static PHY_STATUS_t get_phystatus_from_sredes(Serdes_id_t sel_index)
{
    ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t phy_sel = ECNT_EVENT_SERDES_PHY_NONE;
#if SUPPORT_BOARD_SELECT
	uint8_t serdes_port = get_serdes_port_count();
#else
	ECNT_EVENT_SYSTEM_SERDES_SEL_t serdes_port = ECNT_EVENT_SERDES_SEL_MAX;
#endif

    switch (sel_index){
#if SUPPORT_SERDES_PCIE0
        case SERDES_PCIE0_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_WIFI1;
            break;
#endif
#if SUPPORT_SERDES_PCIE1
        case SERDES_PCIE1_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_WIFI2;
            break;
#endif
#if SUPPORT_SERDES_USB
        case SERDES_USB_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_USB1;
            break;
#endif
#if SUPPORT_SERDES_PON
        case SERDES_AE_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_PON;
            break;
#endif
#if SUPPORT_SERDES_ETHER
        case SERDES_ETH_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_ETHER;
            break;
#endif
        default: 
            pr_info("index input error, sel_index = %d\n",sel_index);
            break;
    }

    phy_sel = get_serdes_phy_sel(serdes_port);
    if (phy_sel == ECNT_EVENT_SERDES_PHY_AN8811){
        return EN8811_PHY_ON;
    }
    else if (phy_sel == ECNT_EVENT_SERDES_PHY_DSL)
    {
    	pr_info("current serdes type is DSL, but still act the 8811 phy, for backup!");
    	return EN8811_PHY_ON;	//for DSL mode, we should also using en8811phy for detect and switch.
    }
    else{
        return EN8811_PHY_OFF;
    }
}

static void get_serdesInfo_from_map(uint8_t phyAddr, uint8_t* serdesID, char** serdesName)
{
    uint8_t i = 0;

    for(i = 0; i< PHYSERDES_MAP_SIZE; i++){
        if(GET_SERDES_PHYADDR(i) == phyAddr){
            *serdesID = GET_SERDES_ID(i);
            *serdesName = GET_SERDES_NAME(*serdesID);
            return;
        }
    }
    *serdesID = INVALID_SERDES_ID;
    *serdesName = INVALID_ITF_NAME;
}

static void set_en8811_phydev(Mdio_priv_data_t *mdio_priv_data_p, struct phy_device *phyDev)
{
    uint8_t phyAddr = INVALID_PHY_ADDR, serdesID = INVALID_SERDES_ID, phyNum = 0;
    char *serdesName = NULL;
    uint32_t phy_id = 0;
    PHY_STATUS_t phy_status = EN8811_PHY_OFF;

    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return;
    }
    phyNum = MDIO_PHY_NUM(mdio_priv_data_p);
    if(!CHECK_PHY_DEV(phyDev)){
        return;
    }
    phyAddr = phyDev->mdio.addr;
    phy_id = phyDev->phy_id;
    if(!CHECK_PHY_ADDR(phyAddr)){
        return;
    }
    phy_status = GET_PHY_STATUS(mdio_priv_data_p,phyAddr);
    if(phy_status == EN8811_PHY_OFF){
        EN8811_LOG(EN8811_LOG_WARNING,"Failed to add PHY[%hhu]!",phyAddr);
        return;
    }

    if(phy_id == 0){
        EN8811_LOG(EN8811_LOG_WARNING,"PHY[%hhu]:INVALID phy_id\n",phyAddr);
        return;
    }
    get_serdesInfo_from_map(phyAddr,&serdesID,&serdesName);
    
    PHY_DEV_PTR(mdio_priv_data_p, phyAddr) = phyDev;
    PHY_DEV_ADDR(mdio_priv_data_p, phyAddr) = phyAddr;
    PHY_DEV_ID(mdio_priv_data_p, phyAddr) = phy_id;
    PHY_STATUS(mdio_priv_data_p, phyAddr) = phy_status;
    MDIO_REG_ADDR(mdio_priv_data_p, phyNum) = phyAddr;
    SET_SERDES_ID_SYNC(mdio_priv_data_p, phyAddr, serdesID);
    (MDIO_PHY_NUM(mdio_priv_data_p))++;
    if(MDIO_PHY_DEFAULT(mdio_priv_data_p) == INVALID_PHY_ADDR){
        MDIO_PHY_DEFAULT(mdio_priv_data_p) = phyAddr;
    }

    pr_info("Adding[%hhu]: PHY[%hhu], %s\n",MDIO_PHY_NUM(mdio_priv_data_p),phyAddr,serdesName);
}

static uint8_t scan_register_phydev(Mdio_priv_data_t *mdio_priv_data_p)
{
    struct mii_bus *bus;
    struct phy_device *phydev;
    uint8_t phyAddr = INVALID_PHY_ADDR;

    bus = MDIO_BUS_PRT(mdio_priv_data_p);
    if(bus == NULL){
        EN8811_LOG(EN8811_LOG_WARNING,"bus is NULL\n");
        return EN8811_FALSE;
    }
    
    for(phyAddr = MIN_PHY_ADDR; phyAddr < INVALID_PHY_ADDR; phyAddr++){
        /*judge phy status*/
        if(PHY_STATUS(mdio_priv_data_p,phyAddr) == EN8811_PHY_OFF){
            continue;
        }
        
        phydev = mdiobus_get_phy(bus, phyAddr);
        EN8811_LOG(EN8811_LOG_DUMP_DATA,"phydev[%hhu] = %p\n", phyAddr, phydev);
        if(phydev){
            set_en8811_phydev(mdio_priv_data_p,phydev);          
        }
    }

    if(phyAddr == INVALID_PHY_ADDR && MDIO_PHY_DEFAULT(mdio_priv_data_p) == INVALID_PHY_ADDR){
        pr_warn("not find any phy device \n");
        return EN8811_FALSE;
    }
    return EN8811_TRUE;
}

static void print_serdes_mapinfo(Mdio_priv_data_t *mdio_priv_data_p)
{
    uint8_t new_serdesID = INVALID_SERDES_ID,  serdesID = INVALID_SERDES_ID;
    uint8_t phyAddr = INVALID_PHY_ADDR, phy_status = 0;
    char* serdesName = "";
    char* phy_itf_name = "";
    char addr_str[5];

    set_itfname_info();

    printk("-----------Serdes Map Info:--------\n");
    printk("%-5s   %-4s   %-3s     %-10s\n","name","addr","status","serdes");
    for(serdesID = 0; serdesID < serdes_map_by_ordes_num; serdesID++){
        new_serdesID = serdes_map_by_ordes[serdesID];

        phyAddr = GET_SERDES_PHYADDR(new_serdesID);
        if(phyAddr == 0){
            strncpy(addr_str, " -- ", sizeof(addr_str));
            addr_str[sizeof(addr_str)-1] = '\0';
        }
        else{
            snprintf(addr_str, sizeof(addr_str), "%02d", phyAddr);
        }

        serdesName = GET_SERDES_NAME(new_serdesID);
        phy_itf_name = GET_PHY_ITFNAME(new_serdesID);
        phy_status = GET_PHY_STATUS(mdio_priv_data_p,phyAddr);
        printk("%-6s  %-5s  %-7s  %-1s\n",
                        (strncmp(phy_itf_name,INVALID_ITF_NAME,4) == 0) ? " --- ":phy_itf_name,
                        addr_str,
                        phy_status ? "ON":"OFF",
                        serdesName);
    }
    printk("----------------------------------\n\n");
}

/*Attention: The fucntion was called after reset_serdes_mapInfo*/
static void set_phy_status_info(Mdio_priv_data_t *mdio_priv_data_p)
{
    uint8_t phyAddr = INVALID_PHY_ADDR;
    Serdes_id_t serdesID;

    for(serdesID = 0; serdesID < INVALID_SERDES_ID; serdesID++){
        phyAddr = GET_SERDES_PHYADDR(serdesID);
        if(phyAddr == 0 || phyAddr == INVALID_PHY_ADDR){
            EN8811_LOG(EN8811_LOG_DBG,"%s, phyAddr[%hhu] is off\n",GET_SERDES_NAME(serdesID),phyAddr);
            PHY_STATUS(mdio_priv_data_p,phyAddr) = EN8811_PHY_OFF;
        }
        else{
            EN8811_LOG(EN8811_LOG_DBG,"%s, phyAddr[%hhu] is on\n",GET_SERDES_NAME(serdesID),phyAddr);
            PHY_STATUS(mdio_priv_data_p,phyAddr) = get_phystatus_from_sredes(serdesID);
        }
    }

}

static void reset_serdes_mapInfo(void)
{
    uint8_t i;
    uint8_t phyAddrs[] = {0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t phyNums = sizeof(phyAddrs);
    uint8_t is_allzero_flag = 0;

    if(get_8811phy_addr(phyAddrs, phyNums) != 0){
        pr_err("[func:%s] get phy addr from flash error!!! use default information!\n",__func__);
        return ;        
    }

    for (i=0; i<phyNums; i++) {
        if(phyAddrs[i] != 0){
            is_allzero_flag += 1;
        }
    }

    if(is_allzero_flag == 0){
        pr_info("use default information! \n");
        return ;
    }
    else{
        /*use  mi config information!*/
        for(i=0; i < serdes_map_by_ordes_num; i++){
            serdes_map[serdes_map_by_ordes[i]].phy_addr = phyAddrs[i];
        }
        pr_info("use mi config information! \n");
        return ;
    }
}    

static void init_en8811_priv_data(Mdio_priv_data_t *mdio_priv_data_p)
{
    uint8_t i = 0;
    
    for(i = 0; i < MAX_PHY_DEVICES; i++){
        PHY_DEV_PTR(mdio_priv_data_p,i) = NULL;
        PHY_DEV_ADDR(mdio_priv_data_p,i) = INVALID_PHY_ADDR;
        PHY_SERDES_ID(mdio_priv_data_p,i) = INVALID_SERDES_ID;
        PHY_DEV_ID(mdio_priv_data_p,i) = 0;
        PHY_STATUS(mdio_priv_data_p,i) = 0;
        MDIO_REG_ADDR(mdio_priv_data_p,i) = INVALID_PHY_ADDR;
        MDIO_REG_SDID(mdio_priv_data_p,i) = INVALID_SERDES_ID;
    }
    MDIO_PHY_NUM(mdio_priv_data_p) = 0;
    MDIO_PHY_DEFAULT(mdio_priv_data_p) = INVALID_PHY_ADDR;
}
/************************************************************************
*                  G L O B A L   F U N C T I O N S                      *
*************************************************************************
*/
void* mem_alloc(int size)
{
    void* ptr = NULL;
    
    if (size > 0)
    {
        ptr = kmalloc(size, GFP_KERNEL);
        if (ptr != NULL)
        {
            memset(ptr,0,size);
            EN8811_LOG(EN8811_LOG_DBG,"Memory allocation successful, size = %d\n",size);
        }
        else{
            EN8811_LOG(EN8811_LOG_ERROR,"Memory allocation failed, size = %d\n",size);
        }
    }
    
    return ptr;
} 

void mem_free(void* ptr)
{
    if (ptr)
    {
        kfree(ptr);
        ptr = NULL;
        EN8811_LOG(EN8811_LOG_DBG,"Memory freed successfully\n");
    } 
}

uint8_t init_en8811_priv(Mdio_priv_data_t *mdio_priv_data_p)
{
    uint8_t ret = 0;
    EN8811_LOG(EN8811_LOG_DBG,"Enter \n");

    init_en8811_priv_data(mdio_priv_data_p);
    
    /*Set the phy interface name*/
    set_itfname_info();
    
    /*update serdes-map table if mi configed*/
    reset_serdes_mapInfo();
    
    /*Set phy status form serdes config on mi.conf*/
    set_phy_status_info(mdio_priv_data_p);

    /*Save has registered phy device*/
    ret = scan_register_phydev(mdio_priv_data_p);
    if(ret == EN8811_FALSE) {
        EN8811_LOG(EN8811_LOG_ERROR,"scan_register_phydev failed!\n");
    }
    else {
        pr_info("Default PHY address is set to %hhu\n",MDIO_PHY_DEFAULT(mdio_priv_data_p));
        EN8811_LOG(EN8811_LOG_DBG,"Initializing mdio_priv_data_p ok!\n");
    }
    return EN8811_TRUE;
}

void print_phy_registeredInfo(Mdio_priv_data_t *mdio_priv_data_p)
{
    uint8_t default_serdesID = INVALID_SERDES_ID;
    uint8_t default_phyAddr = 0;
    uint8_t registered_phy_num = 0;

    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return;
    }

    print_serdes_mapinfo(mdio_priv_data_p);

    registered_phy_num = MDIO_PHY_NUM(mdio_priv_data_p);
    if(registered_phy_num == 0){
        printk("nothing of registerd phy devices\n");
        return;
    }

    default_phyAddr = MDIO_PHY_DEFAULT(mdio_priv_data_p);
    default_serdesID = MDIO_REG_SDID(mdio_priv_data_p,default_phyAddr);
    printk("Default phyDev address: %hhu, Serdes name:%s ,Interface name:%s\n\n",
                    default_phyAddr,GET_SERDES_NAME(default_serdesID),GET_PHY_ITFNAME(default_serdesID));
}

void print_phy_privdataInfo(Mdio_priv_data_t *mdio_priv_data_p)
{
    uint8_t i = 0;
    
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return;
    }

    pr_info("-------Dump EN8811 Private Data-------\n");
    pr_info("mii_bus: %s\n", MDIO_BUS_PRT(mdio_priv_data_p) ? "Full" : "None");
    for(i = MIN_PHY_ADDR; i < MAX_PHY_DEVICES; i++){
        pr_info("---phy_devices[%hhu]: %s---\n", i, PHY_DEV_PTR(mdio_priv_data_p,i) ? "Full" : "None");
        if(PHY_STATUS(mdio_priv_data_p,i) == EN8811_PHY_ON){
            pr_info("phy status: ON\n");
        }
        else{
            pr_info("phy status: OFF\n");
        }

        if(PHY_DEV_ADDR(mdio_priv_data_p,i) == INVALID_PHY_ADDR){
            pr_info("phy_addr : None\n");
        }
        else{
            pr_info("phy_addr : %hhu\n", PHY_DEV_ADDR(mdio_priv_data_p,i));
        }

        if(PHY_SERDES_ID(mdio_priv_data_p,i) == INVALID_SERDES_ID){
            pr_info("serdes_id: None\n");
        }
        else{
            pr_info("serdes_id: %hhu\n", PHY_SERDES_ID(mdio_priv_data_p,i));
        }
        pr_info("phy_id   : 0x%04x\n", PHY_DEV_ID(mdio_priv_data_p,i));
    }
    print_phy_registeredInfo(mdio_priv_data_p);
}

uint8_t check_phy_itfname_valid(Mdio_priv_data_t *mdio_priv_data_p, char *itfname)
{
    int i, serdesID = INVALID_SERDES_ID;

    set_itfname_info();
    for(i = MIN_PHY_ADDR; i <= MAX_PHY_ADDR; i++)
    {
        serdesID = MDIO_REG_SDID(mdio_priv_data_p,i);
        if(serdesID == INVALID_SERDES_ID){
            continue;
        }
        if(strncmp(itfname,itfname_info[serdesID],STRING_LEN) == 0) {
            return EN8811_TRUE;
        }
    }
    pr_err("PhyItfname[%s] is invalid\n",itfname);
    return EN8811_FALSE;
}

uint8_t get_phyAddr_by_ItfName(char *iftname)
{
    uint8_t i = 0;

    for(i=0; i< MAX_SERDES_NUM; i++){
        if(strcmp(iftname, GET_PHY_ITFNAME(i)) == 0){
            return GET_SERDES_PHYADDR(i);
        }
    }

    return INVALID_PHY_ADDR;
}

struct phy_device* get_phyDev_by_ItfName(Mdio_priv_data_t *mdio_priv_data_p, char *iftname)
{
    uint8_t i = 0, phyAddr = 0;

    for(i=0; i< MAX_SERDES_NUM; i++){
        if(strcmp(iftname, GET_PHY_ITFNAME(i)) == 0){
            phyAddr = GET_SERDES_PHYADDR(i);
            return  GET_PHY_DEV(mdio_priv_data_p, phyAddr);    
        }
    }

    return NULL;
}

char* get_ItfName_by_phyAddr(Mdio_priv_data_t *mdio_priv_data_p, uint8_t phyAddr)
{
    uint8_t i = 0, serdesID = INVALID_SERDES_ID;

    set_itfname_info();
    if(CHECK_PHY_ADDR(phyAddr) == EN8811_TRUE){
        for(i = MIN_PHY_ADDR;  i <= MAX_PHY_ADDR; i++){
            if(phyAddr == i){
                serdesID = MDIO_REG_SDID(mdio_priv_data_p,phyAddr);
                return itfname_info[serdesID];
            }
        }
    }

    return INVALID_ITF_NAME;
}

void get_default_ItfName(Mdio_priv_data_t *mdio_priv_data_p, char* phyItfName)
{
    const char *itfname_str;
    uint8_t default_phy_addr = 0;

    EN8811_LOG(EN8811_LOG_INFO,"use default phy device!\n");
    if(!MDIO_PHY_NUM(mdio_priv_data_p)){
        strscpy(phyItfName , INVALID_ITF_NAME,CMD_LEN);
        return;
    }

    default_phy_addr = MDIO_PHY_DEFAULT(mdio_priv_data_p);
    itfname_str = get_ItfName_by_phyAddr(mdio_priv_data_p,default_phy_addr);
    strscpy(phyItfName , itfname_str,CMD_LEN);
    return;
}

