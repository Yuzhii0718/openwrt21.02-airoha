#ifndef _EN8811_API_H_
#define _EN8811_API_H_

#include <linux/phy.h>
#include <linux/mii.h>
#include <linux/platform_device.h>

/************************************************************************
*                  Debugging Macros and Defines                         *
*************************************************************************
*/
typedef enum
{
    EN8811_LOG_OFF = 0,
    EN8811_LOG_ERROR,
    EN8811_LOG_WARNING,
    EN8811_LOG_INFO,
    EN8811_LOG_DBG,
    EN8811_LOG_DUMP_DATA, 
    EN8811_LOG_MAX_LEVEL
} EN8811_LOG_LEVEL;

extern int debuglevel;
#define EN8811_LOG(Level,fmt,args...)          \
        {                                   \
            if (Level <= debuglevel)        \
            {                               \
                printk("EN8811@ [%s:%u]==="fmt,__func__,__LINE__,##args);\
            }                               \
        }

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

/************************************************************************
*                  D A T A   D E C L A R A T I O N S                    *
*************************************************************************
*/
#define PHY_SUPPORT_CL45 1
#define MIN_PHY_ADDR 8     /*en8811 only suooprt phy address from 8 to 15*/
#define MAX_PHY_ADDR 15
#define INVALID_PHY_ADDR (MAX_PHY_ADDR+1)
#define MAX_PHY_DEVICES  (MAX_PHY_ADDR+1) /*for convenience of search: index of array equal phy address*/
#define INVALID_ITF_NAME "None"
#define STRING_LEN 32
#define CMD_LEN 16

typedef enum{
    EN8811_FALSE = 0,
    EN8811_TRUE =  1
}EN8811_BOOL;

typedef enum{
    EN8811_PHY_OFF = 0,
    EN8811_PHY_ON = 1
}PHY_STATUS_t;


/*start of defines for serdes*/
typedef enum{
    SERDES_PCIE0_IDX = 0,
    SERDES_PCIE1_IDX = 1,
    SERDES_USB_IDX = 2,
    SERDES_AE_IDX = 3,
    SERDES_ETH_IDX = 4,
    INVALID_SERDES_ID
}Serdes_id_t;
#define SERDES_MAX_IDX INVALID_SERDES_ID
#define MAX_SERDES_NUM INVALID_SERDES_ID

typedef struct{
    uint8_t phy_addr;
    uint8_t serdes_id;
}PhySerdes_t;

extern char *serdes_info[];
#define GET_SERDES_NAME(serdesID) (serdes_info[(serdesID)])

extern PhySerdes_t serdes_map[];
#define PHYSERDES_MAP_SIZE (sizeof(serdes_map) / sizeof(serdes_map[0]))
#define GET_SERDES_ID(idx) (serdes_map[(idx)].serdes_id)
#define GET_SERDES_PHYADDR(idx) (serdes_map[(idx)].phy_addr)

/*end of defines for serdes*/

/*start of defines for structures type to manage phy*/
typedef struct _phy_priv_data{
    struct phy_device *phy_dev;
    uint8_t phy_addr;
    uint8_t serdes_id;
    uint32_t  phy_id;
    uint8_t phy_status;
}Phy_priv_data_t;

typedef struct _mdio_priv_data{
    struct mii_bus *mii_bus;
    Phy_priv_data_t phy_info[MAX_PHY_DEVICES];  /*save every phy device's detailed information*/
    uint8_t num_phy_register;                   /*number of registered phy devices*/
    uint8_t phy_addr_list[MAX_PHY_DEVICES];     /*save phy address of registered phy devices*/
    uint8_t phy_registers[MAX_PHY_DEVICES];     /*save serdes_id of registered phy devices, index is phy address*/
    uint8_t phy_addr_default;                   /*default value  for first phy address*/
}Mdio_priv_data_t; /*copy form dmio_arht.c*/

#define MDIO_BUS_PRT(priv)       ((priv)->mii_bus)
#define MDIO_PHY_NUM(priv)       ((priv)->num_phy_register)
#define MDIO_REG_ADDR(priv,idx)   ((priv)->phy_addr_list[(idx)]) /*save registered phy address*/
#define MDIO_REG_SDID(priv,idx)   ((priv)->phy_registers[(idx)]) /*save registered serdes id*/
#define MDIO_PHY_DEFAULT(priv)   ((priv)->phy_addr_default)
#define PHY_DEV_PTR(priv,idx)    ((priv)->phy_info[(idx)].phy_dev)
#define PHY_DEV_ADDR(priv,idx)   ((priv)->phy_info[(idx)].phy_addr)
#define PHY_SERDES_ID(priv,idx)  ((priv)->phy_info[(idx)].serdes_id)
#define PHY_DEV_ID(priv,idx)     ((priv)->phy_info[(idx)].phy_id)
#define PHY_STATUS(priv,idx)     ((priv)->phy_info[(idx)].phy_status)

#define SET_SERDES_ID_SYNC(priv,idx,id) \
    do {\
        ((priv)->phy_info[(idx)].serdes_id = (id));\
        ((priv)->phy_registers[(idx)] = (id));\
    }while(0)

#define GET_PHY_DEV(priv,phyAddr) PHY_DEV_PTR(priv,phyAddr)
#define GET_PHY_STATUS(priv,phyAddr) PHY_STATUS(priv,phyAddr)
#define GET_DEFAULT_PHYADDR(priv)      MDIO_PHY_DEFAULT(priv)

/*end of defines for structures type to manage phy*/

#define CHECK_BASE(ptr,msg) \
    (((ptr) != NULL) ? EN8811_TRUE : \
    (printk("EN8811@ [%s:%d] %s\n",__func__,__LINE__,msg), EN8811_FALSE))
#define CHECK_POINTER(ptr)      CHECK_BASE(ptr,"ptr is NULL")
#define CHECK_PHY_PRIV(ptr)     CHECK_BASE(ptr,"en8811_priv_p is NULL")
#define CHECK_PHY_DEV(ptr)      CHECK_BASE(ptr,"PHY device is NULL")

#define CHECK_PHY_ADDR(addr) \
    (((addr) >= MIN_PHY_ADDR) && ((addr) <= MAX_PHY_ADDR) ? EN8811_TRUE : \
    (printk("EN8811@ [%s:%d] PHY address[%d] is invalid\n",__func__,__LINE__,(addr)), EN8811_FALSE))

#define CHECK_SERDES_ID(id) \
    (((id) < INVALID_SERDES_ID) ? EN8811_TRUE : \
    (printk("EN8811@ [%s:%d] Serdes id[%d] is invalid\n",__func__,__LINE__,(id)), EN8811_FALSE))

/************************************************************************
*                   Function Prototypes                                 *
*************************************************************************
*/
void* mem_alloc(int size);
void mem_free(void* ptr);
uint8_t init_en8811_priv(Mdio_priv_data_t *mdio_priv_data_p);
void print_phy_registeredInfo(Mdio_priv_data_t *mdio_priv_data_p);
void print_phy_privdataInfo(Mdio_priv_data_t *mdio_priv_data_p);

uint8_t check_phy_itfname_valid(Mdio_priv_data_t *mdio_priv_data_p, char *itfname);
uint8_t get_phyAddr_by_ItfName(char *iftname);
struct phy_device* get_phyDev_by_ItfName(Mdio_priv_data_t *mdio_priv_data_p, char *iftname);
char* get_ItfName_by_phyAddr(Mdio_priv_data_t *mdio_priv_data_p, uint8_t phyAddr);
void get_default_ItfName(Mdio_priv_data_t *mdio_priv_data_p, char* phyItfName);

#endif /* _EN8811_API_H_ */