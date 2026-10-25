/* FILE NAME:  en8811_proc.c
 * PURPOSE:
 *    It provides en8811 phy proc definations.
 *
 * NOTES:
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
#include <linux/string.h>

#include "en8811.h"
#include "en8811_api.h"
#include "en8811_proc.h"
#include "en8811_reg.h"
#include "air_en8811h.h"
#include "air_en8811h_api.h"
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_air_en8811.h>

/************************************************************************
*            E X T E R N A L   D A T A   D E C L A R A T I O N S               *
*************************************************************************
*/
extern Mdio_priv_data_t *en8811_priv_p ;
extern int debuglevel;

uint8_t en8811_get_link_info(uint8_t phy_addr, uint32_t* speed_index, uint32_t* duplex);
uint8_t check_phy_itfname_valid(Mdio_priv_data_t *mdio_priv_data_p, char *itfname);
char* get_ItfName_by_phyAddr(Mdio_priv_data_t *mdio_priv_data_p, uint8_t phyAddr);
struct phy_device* get_phyDev_by_ItfName(Mdio_priv_data_t *mdio_priv_data_p, char *iftname);
void get_default_ItfName(Mdio_priv_data_t *mdio_priv_data_p, char* phyItfName);

/************************************************************************
*           D A T A   D E C L A R A T I O N S                           *
*************************************************************************
*/
#define EN8811_JUDGE_INPUT_ITFNAME(str)  (((strstr((str), "itf="))) ? EN8811_TRUE: EN8811_FALSE)

typedef struct _parse_data{
    struct phy_device* phyDev;
    struct mii_bus *mbus;
    uint8_t phyAddr;
    char cmd[CMD_LEN];
    char param[CMD_LEN];
    char subparam[CMD_LEN];
    char phyItfName[CMD_LEN];
}PARSE_DATA_T;

typedef enum{
    EN8811_PBUS = 0,
    EN8811_BUCKPBUS = 1
}Bus_type_t;

typedef enum{
    EN8811_DEBUG_COUNT = 0,
    EN8811_DEBUG_COUNT_PLUS,
    EN8811_DEBUG_REGS,
    EN8811_DEBUG_TEMP
}Debug_type_t;

/************************************************************************
*      S T A T I C    I  L I N E    F U N C T I O N                     *
*************************************************************************
*/
static int EN8811_cl22_op(struct phy_device *phydev, uint32_t reg, uint32_t reg_bit, uint8_t op)
{
    int ret = 0;
    uint32_t reg_data = 0 ;
    struct mii_bus *mbus = phydev_mdio_bus(phydev);
    int addr = phydev_addr(phydev);
    
    reg_data = air_mii_cl22_read(mbus, addr, reg);

    if(EN8811_WRITE_1 == op){
        reg_data |= reg_bit;
    }else if(EN8811_WRITE_0 == op){
        reg_data &= ~reg_bit;
    }else{
        EN8811_LOG(EN8811_LOG_WARNING,"input error: %d \n", op);
           return EN8811_ERROR;
    }

    ret = air_mii_cl22_write(mbus, addr, reg, reg_data);
    if(ret == EN8811_ERROR){
        EN8811_LOG(EN8811_LOG_ERROR,"write error, reg:%d, reg_data:%d \n", reg, reg_data);
        return EN8811_ERROR;
    }
    return EN8811_CONTINUE;
}

static int EN8811_get_LinkInfo(char *buf,uint8_t index,struct phy_device *phydev,char *phyItfName)
{
    int speed = 0;
    int duplex = 0;

    if(!CHECK_PHY_DEV(phydev)){
        EN8811_LOG(EN8811_LOG_ERROR,"Invalid PHY device\n");
        return index;
    }
    index += sprintf(buf+index, "PHY[%s]: ",phyItfName);

    speed = phydev->speed;
    duplex = phydev->duplex;   
    //link status
    if (speed == SPEED_UNKNOWN) {
        index += sprintf(buf+index, "Down \n\n");
        return index;
    }else if (speed == SPEED_2500){
        index += sprintf(buf+index, "2.5Gbps/");
    }else if (speed == SPEED_1000){
        index += sprintf(buf+index, "1000Mbps/");
    }else if (speed == SPEED_100){
        index += sprintf(buf+index, "100Mbps/");
    }
        
    //full or half duplex
    if (duplex == DUPLEX_FULL)
        index += sprintf(buf+index, "Full");
    else if (DUPLEX_HALF)
        index += sprintf(buf+index, "Half");

    index += sprintf(buf+index, "\n\n");
    return index;
}

static int EN8811_check_readproc_para(char *buf, int len, char **start, off_t off, int count, int *eof)
{
    len -= off;
    *start = buf + off;

    if (len > count)
    {
        len = count;
    }
    else
    {
        *eof = 1;
    }

    if (len < 0)
    {
        len = 0;
    }
    return len;
}

inline static void get_itfname_remove_suffix(char *str, char *itfname, uint8_t iftname_size)
{
    const char *itf_prefix = "itf=";
    const char *start_of_value;

    start_of_value = strstr(str, itf_prefix);
    if(start_of_value){
        start_of_value += strlen(itf_prefix);
        strscpy(itfname,start_of_value,iftname_size);
        *str = '\0';
    }
}

inline static void EN8811_remove_itfname_prefix(char *phyItfName)
{
    const char *prefix = "itf=";
    char *match = 0;

    match = strstr(phyItfName,prefix);
    if(match){
        match += strlen(prefix);
        memmove(phyItfName, match , strlen(match) + 1); /*remove prefix  'itf='*/
    }
}
static int EN8811_check_writeproc_para(const char *buffer,unsigned long count, PARSE_DATA_T *input_para, Mdio_priv_data_t *mdio_priv_data_p)
{
    char val_string[STRING_LEN] = {'\0'}, cmd[CMD_LEN] = {'\0'}, param[CMD_LEN] = {'\0'}, subparam[CMD_LEN] = {'\0'};
    struct phy_device* phy_dev = NULL;
    struct mii_bus *mbus = NULL;
    int ret = 0;
    char phyItfName[CMD_LEN] = {'\0'};

    if (count > sizeof(val_string) - 1){
        return -EINVAL;
    }

    if (copy_from_user(val_string, buffer, count)){
        return -EINVAL;
    }

    /*start to parse input infomation*/
    ret = sscanf(val_string, "%s %s %s %s",cmd, param, subparam, phyItfName);
    EN8811_LOG(EN8811_LOG_DBG,"[before prase: %d] cmd:%s, param:%s, subparam:%s, phyItfName:%s\n",ret,cmd, param, subparam, phyItfName);
    if(ret == 4){
        if(!EN8811_JUDGE_INPUT_ITFNAME(phyItfName)){
            get_default_ItfName(mdio_priv_data_p,phyItfName);
        }else{
            EN8811_remove_itfname_prefix(phyItfName);
        }
    }
    else if(ret == 3){
        if(!EN8811_JUDGE_INPUT_ITFNAME(subparam)){
            get_default_ItfName(mdio_priv_data_p,phyItfName);
        }else{
            get_itfname_remove_suffix(subparam, phyItfName,STRING_LEN);
        }
    }
    else if(ret == 2){
        if(!EN8811_JUDGE_INPUT_ITFNAME(param)){
            get_default_ItfName(mdio_priv_data_p,phyItfName);
        }else{
            get_itfname_remove_suffix(param, phyItfName,STRING_LEN);
        }
    }
    else if(ret == 1){
        if(strstr(cmd, "bps") || strstr(cmd, "restartAN") || strstr(cmd, "Auto")){
            get_default_ItfName(mdio_priv_data_p,phyItfName);
        }
        else{
            if(!EN8811_JUDGE_INPUT_ITFNAME(cmd)){
                pr_err("Please check your input parameter: echo itf=[itfname] > /proc/tc3163/en8811_\n");
                return -EINVAL;
            }
            else{
                get_itfname_remove_suffix(cmd, phyItfName,STRING_LEN);
            }
        }
    }
    else{
        pr_err("Please check your input parameter\n");
        return -EINVAL;
    }
    EN8811_LOG(EN8811_LOG_DBG,"[after prase] cmd:%s, param:%s, subparam:%s, phyItfName:%s\n",cmd, param, subparam, phyItfName);

    /*start to check valid*/
    if(!check_phy_itfname_valid(mdio_priv_data_p, phyItfName)){
        return -EINVAL;
    }
    
    /*save  info to input_para*/
    phy_dev = get_phyDev_by_ItfName(mdio_priv_data_p, phyItfName);    
    if(!CHECK_PHY_DEV(phy_dev)){
        return -EINVAL;
    }
    input_para->phyDev = phy_dev;

    mbus = phydev_mdio_bus(phy_dev);
    if(!CHECK_POINTER(mbus)){
        return -EINVAL;
    }
    input_para->mbus = mbus;

    strncpy(input_para->cmd,cmd,CMD_LEN);
    strncpy(input_para->param,param,CMD_LEN);
    strncpy(input_para->subparam,subparam,CMD_LEN);
    strncpy(input_para->phyItfName,phyItfName,CMD_LEN);

    return 0;
}

static int EN8811_bus_op(Bus_type_t type,const char *buffer, unsigned long count)
{
    int ret = 0;
    unsigned long bus_addr = 0, r_bus_data = 0, w_bus_data = 0;
    char mode[CMD_LEN] = {'\0'}, bus_addr_rcv[STRING_LEN] = {'\0'}, bus_data_rcv[STRING_LEN] = {'\0'};
    struct phy_device* phyDev = NULL;
    PARSE_DATA_T parse_data;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;
	char phyItfName_rcv[STRING_LEN] = {'\0'};

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return count;
    }

    memset(&parse_data, 0, sizeof(PARSE_DATA_T));
    ret = EN8811_check_writeproc_para(buffer,count,&parse_data,mdio_priv_data_p);
    if(ret){
        return ret;
    }
    else{
        phyDev = parse_data.phyDev;
        strcpy(mode,parse_data.cmd);
        strcpy(bus_addr_rcv,parse_data.param);
        ret = kstrtoul(bus_addr_rcv,CMD_LEN,&bus_addr);
        strcpy(bus_data_rcv,parse_data.subparam);
        strcpy(phyItfName_rcv,parse_data.phyItfName);
    }

    if(!strcmp(mode, "r")){
        if(type == EN8811_PBUS){
            r_bus_data = air_pbus_reg_read(phyDev, bus_addr);
        }
        else if(type == EN8811_BUCKPBUS){
            r_bus_data = air_buckpbus_reg_read(phyDev, bus_addr);
        }
        else{
            printk("Invalid bus type\n\n");
        }
        printk("[phyItfName:%s] bus_addr=0x%05lx r_bus_data=0x%08lx \n\n",phyItfName_rcv, bus_addr, r_bus_data);
    }
    else if(!strcmp(mode, "w")){
        if(bus_data_rcv[0] == '\0'){
            return -EINVAL;
        }
        ret = kstrtoul(bus_data_rcv,CMD_LEN,&w_bus_data);
        if(type == EN8811_PBUS){
            ret = air_pbus_reg_write(phyDev, bus_addr, w_bus_data);
            r_bus_data = air_pbus_reg_read(phyDev, bus_addr);
        }
        else if(type == EN8811_BUCKPBUS){
            ret = air_buckpbus_reg_write(phyDev, bus_addr, w_bus_data);
            r_bus_data = air_buckpbus_reg_read(phyDev, bus_addr);
        }
        printk("[phyItfName:%s] bus_addr=0x%05lx w_bus_data=0x%08lx r_bus_data=0x%08lx \n\n",phyItfName_rcv, bus_addr, w_bus_data, r_bus_data);
    }
    else{
        EN8811_LOG(EN8811_LOG_ERROR,"please check your input parameter\n");
    }

    return count;
}

static uint32_t EN8811_print_debug_info(char *phyItfName, struct phy_device* phyDev, char **buff, Debug_type_t type)
{
    uint32_t len = 0;
    int ret = 0;

    if(type == EN8811_DEBUG_COUNT){
        ret =  en8811h_read_status(phyDev);
        if(ret < 0){
            pr_err("read status error! \n");
        }
        len += sprintf(*buff + len,"==========PHY[%s] COUNTER==========\n",phyItfName);
        if (phyDev->link) {
            ret |= airphy_ss_counter_show(phyDev, buff, &len);
        }
        ret |= airphy_fcm_counter_show(phyDev, buff, &len);
    }
    else if(type == EN8811_DEBUG_COUNT_PLUS){
        if (phyDev->link) {
            ret |= airphy_mac_counter_show(phyDev,buff, &len);
            ret |= airphy_ls_counter_show(phyDev, buff, &len);
        }
    }
    else if(type == EN8811_DEBUG_REGS){
        len += sprintf(*buff + len,"==========PHY[%s] AIR DBG REG==========\n",phyItfName);
        ret = airphy_dbg_regs_show(phyDev, buff, &len);
    }
    else if(type == EN8811_DEBUG_TEMP){
        len += sprintf(*buff + len,"==========PHY[%s] AIR TEMP==========\n",phyItfName);
        ret = airphy_temp_show(phyDev, buff, &len);
    }
    else{
        EN8811_LOG(EN8811_LOG_ERROR,"Invalid type %d\n", type);
    }

    if (ret < 0){
        EN8811_LOG(EN8811_LOG_ERROR,"airphy_fcm_counter_show return error! \n");
        return 0;
    }
    return len;
}

/************************************************************************
*                  G L O B A L   F U N C T I O N S                      *
*************************************************************************
*/
int en8811_debug_level_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(buf + len, "echo 0-5 > proc/tc3162/en8811_debug\n");
    len += sprintf(buf + len, "[0]: disable all message.\n");
    len += sprintf(buf + len, "[1]: enable error message.\n");
    len += sprintf(buf + len, "[2]: enable warning message.\n");
    len += sprintf(buf + len, "[3]: enable info message.\n");
    len += sprintf(buf + len, "[4]: enable debug message.\n");
    len += sprintf(buf + len, "[5]: enable data dump message.\n");
    len += sprintf(buf + len, "[6]: Dump all en8811 private data.\n");

    len += sprintf(buf + len, "\n debuglevel:%hhu\n\n", debuglevel);
    
    len = EN8811_check_readproc_para(buf, len, start, off, count, eof);
    return len;
}

int en8811_debug_level_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int ret = 0;
    char val_string[STRING_LEN];
    uint8_t debug = 0, debug_tmp = 0;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return count;
    }

    if (count > sizeof(val_string) - 1)
    {
        return -EINVAL;
    }

    if (copy_from_user(val_string, buffer, count))
    {
        return -EFAULT;
    }

    ret = sscanf(val_string, "%hhu ", &debug);
    if(!ret){
        return -EINVAL;
    }
    if( debug < EN8811_LOG_MAX_LEVEL){
        debuglevel = debug; 
        return count;
    }
    else{
        debug_tmp = debuglevel;
    }

    if (debug == EN8811_LOG_MAX_LEVEL){
        debuglevel = EN8811_LOG_OFF;
        print_phy_privdataInfo(mdio_priv_data_p);
    }
    else{
        printk("echo 0-5 > proc/tc3162/en8811_debug\n");
        EN8811_LOG(EN8811_LOG_ERROR,"Please check the input parameter!\n");
    }

    debuglevel = debug_tmp; 
    return count;
}

int en8811_link_state_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int ret = 0, len = 0;
    unsigned int speed_index = 0, duplex = 0;
    uint8_t i = 0;
    struct phy_device* phyDev = NULL;
    uint8_t phyAddr = INVALID_PHY_ADDR;
    uint8_t all_phyAddrs_num = 0;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;
    char *phyItfName=NULL;

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return count;
    }
    all_phyAddrs_num = MDIO_PHY_NUM(mdio_priv_data_p);
    for(i = 0 ; i < all_phyAddrs_num; i++){
        phyAddr = MDIO_REG_ADDR(mdio_priv_data_p, i);
        EN8811_LOG(EN8811_LOG_DBG,"phy[%d] addr= %d\n",i,phyAddr);
        ret = en8811_get_link_info(phyAddr,&speed_index, &duplex);
        if(ret)
            return -EINVAL;
        
        phyDev = GET_PHY_DEV(mdio_priv_data_p, phyAddr);
        if(!CHECK_PHY_DEV(phyDev)){
            EN8811_LOG(EN8811_LOG_ERROR,"Invalid PHY device\n");
            return  -EINVAL;
        }

        phyItfName = get_ItfName_by_phyAddr(mdio_priv_data_p,phyAddr);
        len += EN8811_get_LinkInfo(buf + len,0,phyDev,phyItfName);
    }

    len = EN8811_check_readproc_para(buf, len, start, off, count, eof);
    return len;
}

int en8811_speed_mode_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(buf,"Restart AN  : echo restartAN [itf=phy_itfname]> /proc/tc3162/en8811_speed_mode \n\n");

    len += sprintf(buf + len,"Open flow control : echo fc open [itf=phy_itfname] > /proc/tc3162/en8811_speed_mode \n");
    len += sprintf(buf + len,"Close flow control: echo fc close [itf=phy_itfname] > /proc/tc3162/en8811_speed_mode \n\n");

    len += sprintf(buf + len,"Enable An  : echo Auto [itf=phy_itfname] > /proc/tc3162/en8811_speed_mode \n");
    len += sprintf(buf + len,"Set 2.5G FD: echo 2.5Gbps [itf=phy_itfname] > /proc/tc3162/en8811_speed_mode \n");
    len += sprintf(buf + len,"Set 1G FD  : echo 1Gbps [itf=phy_itfname]   > /proc/tc3162/en8811_speed_mode \n");
    len += sprintf(buf + len,"Set 100M FD: echo 100Mbps [itf=phy_itfname] > /proc/tc3162/en8811_speed_mode \n\n");

    len = EN8811_check_readproc_para(buf, len, start, off, count, eof);
    return len;
}

int en8811_speed_mode_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int ret = 0;
    char cmd[CMD_LEN] = {0}, param[CMD_LEN] = {0};
    uint8_t phyAddr = INVALID_PHY_ADDR;
    struct phy_device* phyDev = NULL;
    struct mii_bus *mbus = NULL;
    PARSE_DATA_T parse_data;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;
    char* phyItfName = NULL;

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return count;
    }
    
    memset(&parse_data, 0, sizeof(PARSE_DATA_T));
    ret = EN8811_check_writeproc_para(buffer,count,&parse_data,mdio_priv_data_p);
    if(ret){
        return ret;
    }
    else{
        phyItfName = parse_data.phyItfName;
        phyAddr = get_phyAddr_by_ItfName(phyItfName);
        phyDev = parse_data.phyDev;
        mbus = parse_data.mbus;
        strcpy(cmd,parse_data.cmd);
        strcpy(param,parse_data.param);
    }

    if(!strcmp(cmd, "restartAN")){
        ret = EN8811_cl22_op(phyDev, EN8811_RestartAN_REG, EN8811_RestartAN_BIT, EN8811_WRITE_1);
        if(ret == EN8811_ERROR){
            goto CHECK_INPUT;
        }
        do{
            mdelay(100);
            ret = air_mii_cl22_read(mbus, phyAddr, EN8811_AN_COMPLETE_REG);
            ret &= EN8811_AN_COMPLETE_BIT;
        }while(ret);
        printk("EN8811[%s] restartAN successly!\n\n",phyItfName);
    }
    else if(!strcmp(cmd, "fc")){
        if(!strcmp(param, "open")){
            ret = EN8811_cl22_op(phyDev, EN8811_FC_REG, EN8811_FC_BIT, EN8811_WRITE_1);
            if(ret == EN8811_ERROR){
                goto CHECK_INPUT;
            }
            printk("EN8811[%s] fc opened successly!\n\n",phyItfName);
        }
        else if(!strcmp(param, "close")){
            ret = EN8811_cl22_op(phyDev, EN8811_FC_REG, EN8811_FC_BIT, EN8811_WRITE_0);
            if(ret == EN8811_ERROR){
                goto CHECK_INPUT;
            }
            printk("EN8811[%s] fc closed successly!\n\n",phyItfName);
        }
        else
            goto CHECK_INPUT;
    }
    else if(!strcmp(cmd, "2.5Gbps")){
        ret = airphy_set_mode(phyDev, AIR_PORT_MODE_FORCE_2500);
        if(ret == EN8811_ERROR){
            goto CHECK_INPUT;
        }
        printk("EN8811[%s] set 2.5Gbps successly!\n\n",phyItfName);
    }
    else if(!strcmp(cmd, "1Gbps")){
        ret = airphy_set_mode(phyDev, AIR_PORT_MODE_FORCE_1000);
        if(ret == EN8811_ERROR){
            goto CHECK_INPUT;
        }
        printk("EN8811[%s] set 1Gbps successly!\n\n",phyItfName);
    }
    else if(!strcmp(cmd, "100Mbps")){
        ret = airphy_set_mode(phyDev, AIR_PORT_MODE_FORCE_100);
        if(ret == EN8811_ERROR){
            goto CHECK_INPUT;
        }
        printk("EN8811[%s] set 100Mbps successly!\n\n",phyItfName);
    }
    else if(!strcmp(cmd, "Auto")){
        ret = airphy_set_mode(phyDev, AIR_PORT_MODE_AUTONEGO);;
        if(ret == EN8811_ERROR){
            goto CHECK_INPUT;
        }
        printk("EN8811[%s] set auto successly!\n\n",phyItfName);
    }
    else
        goto CHECK_INPUT;

    return count;
CHECK_INPUT:
        pr_err("Please check the input parameter!\n\n");
        return -EINVAL;
}

int en8811_power_read_proc(char *buf, char **start, off_t off, int count,int *eof, void *data)
{
    int len = 0;

    len += sprintf(buf + len,"Set Power on : echo power on  [itf=phy_itfname] > /proc/tc3162/en8811_power \n");
    len += sprintf(buf + len,"Set Power off: echo power off [itf=phy_itfname] > /proc/tc3162/en8811_power \n\n");

    len = EN8811_check_readproc_para(buf, len, start, off, count, eof);
    return len;
}

int en8811_power_write_proc(struct file *file, const char *buffer,unsigned long count, void *data)
{
    int ret = 0;
    char cmd[CMD_LEN] = {0}, param[CMD_LEN] = {0};
    struct phy_device* phyDev = NULL;
    PARSE_DATA_T parse_data;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;
    char *phyItfName = NULL;

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return count;
    }
    
    memset(&parse_data, 0, sizeof(PARSE_DATA_T));
    ret = EN8811_check_writeproc_para(buffer,count,&parse_data,mdio_priv_data_p);
    if(ret){
        return ret;
    }
    else{
        phyItfName = parse_data.phyItfName;
        phyDev = parse_data.phyDev;
        strcpy(cmd,parse_data.cmd);
        strcpy(param,parse_data.param);
    }

    if(!strcmp(cmd, "power")){
        if(!strcmp(param, "on")){
            ret = airphy_set_mode(phyDev, AIR_PORT_MODE_POWER_UP);
            if(ret == EN8811_ERROR){
                goto CHECK_INPUT;
            }
            printk("EN8811[%s] power on successly!\n\n",phyItfName);
        }
        else if(!strcmp(param, "off")){
            ret = airphy_set_mode(phyDev, AIR_PORT_MODE_POWER_DOWN);
            if(ret == EN8811_ERROR){
                goto CHECK_INPUT;
            }
            printk("EN8811[%s] power off successly!\n\n",phyItfName);
        }
        else
            goto CHECK_INPUT;
    }
    else
        goto CHECK_INPUT;

    return count;
CHECK_INPUT:
        pr_err("Please check the input parameter!\n");
        return -EINVAL;
}

int en8811_version_read_proc(char *buf, char **start, off_t off, int count,int *eof, void *data)
{
    int len = 0;
    uint32_t pbus_value = 0;
    struct phy_device* phyDev = NULL;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return count;
    }

    if(MDIO_PHY_NUM(mdio_priv_data_p)){
        phyDev = GET_PHY_DEV(mdio_priv_data_p, MDIO_PHY_DEFAULT(mdio_priv_data_p));
        if(!CHECK_PHY_DEV(phyDev)){
            return -EINVAL;
        }
        pbus_value = air_buckpbus_reg_read(phyDev, 0x3b3c);

        len += sprintf(buf + len,"---------Version Infomation---------\n");
        len += sprintf(buf + len,"EN8811 MD32 FW Version(0x3b3c) : %08x\n", pbus_value);
        len += sprintf(buf + len,"EN8811 driver version: %s\n", EN8811H_DRIVER_VERSION);
        len += sprintf(buf + len,"8811_bbu_api driver version: %s\n\n", EN8811_BBU_API_VERSION);
    }

    len = EN8811_check_readproc_para(buf, len, start, off, count, eof);
    return len;
}

int en8811_pbus_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(buf + len,"echo r [register] [itf=phy_itfname] > /proc/tc3162/en8811_pbus \n");
    len += sprintf(buf + len,"echo w [register] [data] [itf=phy_itfname] > /proc/tc3162/en8811_pbus\n\n");
    len += sprintf(buf + len,"example: echo r 0xcf998 itf=eth0.8 > /proc/tc3162/en8811_pbus \n");
    len += sprintf(buf + len,"         echo w 0xcf998 0x18100f itf=eth0.8 > /proc/tc3162/en8811_pbus \n\n");

    len = EN8811_check_readproc_para(buf, len, start, off, count, eof);
    return len;

}

int en8811_pbus_write_proc(struct file *file, const char *buffer,unsigned long count, void *data)
{
    int ret = 0;

    ret = EN8811_bus_op(EN8811_PBUS, buffer, count);

    return ret;
}

int en8811_buckpbus_read_proc(char *buf, char **start, off_t off, int count,int *eof, void *data)
{
    int len = 0;

    len += sprintf(buf + len,"echo r [register] [itf=phy_itfname] > /proc/tc3162/en8811_buckpbus \n");
    len += sprintf(buf + len,"echo w [register] [data] [itf=phy_itfname] > /proc/tc3162/en8811_buckpbus \n\n");
    len += sprintf(buf + len,"example: echo r 0xca0f8 itf=eth0.8 > /proc/tc3162/en8811_buckpbus\n");
    len += sprintf(buf + len,"         echo w 0xca0f8 0x1a01503 itf=eth0.8 > /proc/tc3162/en8811_buckpbus\n\n");
    
    len = EN8811_check_readproc_para(buf, len, start, off, count, eof);
    return len;
}

int en8811_buckpbus_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int ret = 0;

    ret = EN8811_bus_op(EN8811_BUCKPBUS, buffer, count);

    return ret;
}

int en8811_serdes_info_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return len;
    }
    
    print_phy_registeredInfo(mdio_priv_data_p);

    return len;
}

static int en8811_print_dbginfo_read(Debug_type_t type)
{
    uint32_t len = 0;
    uint8_t phyAddr = INVALID_PHY_ADDR;
    struct phy_device* phyDev = NULL;
    char *buff = NULL;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;
    char *phyItfName = NULL;

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return -EINVAL;
    }
    /*use default phy device if have*/
    if(!MDIO_PHY_NUM(mdio_priv_data_p)){
        pr_err("not found EN8811\n");
        return -EINVAL;
    }
    phyAddr = MDIO_PHY_DEFAULT(mdio_priv_data_p);
    phyDev = PHY_DEV_PTR(mdio_priv_data_p,phyAddr);

    buff = mem_alloc(BUFFER_LENGTH);
    if(!CHECK_POINTER(buff)){
        return -ENOMEM;
    }

    phyItfName = get_ItfName_by_phyAddr(mdio_priv_data_p,phyAddr);
    len = EN8811_print_debug_info(phyItfName,phyDev,&buff,type);
    if (len){
        printk("%s \n",buff ? buff : "NULL");
    }
    mem_free(buff);

    return 0;
}

int en8811_count_read_proc(char *buf, char **start, off_t off, int count,int *eof, void *data)
{
    int ret = 0;
    
    ret = en8811_print_dbginfo_read(EN8811_DEBUG_COUNT);
    if(ret){
        pr_err("print count error!\n");
        return ret;
    }
    
    ret = en8811_print_dbginfo_read(EN8811_DEBUG_COUNT_PLUS);
    if(ret){
        pr_err("print count error!\n");
        return ret;
    }
    return 0;
}

int en8811_dbg_regs_read_proc(char *buf, char **start, off_t off, int count,int *eof, void *data)
{
    int ret = 0;

    ret = en8811_print_dbginfo_read(EN8811_DEBUG_REGS);
    if(ret){
        pr_err("print regs error!\n");
        return ret;
    }
    return 0;
}

int en8811_temp_read_proc(char *buf, char **start, off_t off, int count,int *eof, void *data)
{
    int ret = 0;

    ret = en8811_print_dbginfo_read(EN8811_DEBUG_TEMP);
    if(ret){
        pr_err("print temp error!\n");
        return ret;
    }

    return 0;
}

static int en8811_print_dbginfo_write(const char *buffer, unsigned long count, Debug_type_t type)
{
    int ret = 0;
    uint32_t len = 0;
    uint8_t phyAddr = INVALID_PHY_ADDR;
    char *phyItfName = NULL;
    struct phy_device* phyDev = NULL;
    PARSE_DATA_T parse_data;
    char* buff = NULL;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return count;
    }
    
    memset(&parse_data, 0, sizeof(PARSE_DATA_T));
    ret = EN8811_check_writeproc_para(buffer,count,&parse_data,mdio_priv_data_p);
    if(ret){
        return ret;
    }
    else{
        phyAddr = parse_data.phyAddr;
        phyDev = parse_data.phyDev;
        phyItfName = parse_data.phyItfName;
    }

    buff = mem_alloc(BUFFER_LENGTH);
    if(!CHECK_POINTER(buff)){
        return -ENOMEM;
    }

    len = EN8811_print_debug_info(phyItfName,phyDev,&buff,type);
    if (len){
        printk("%s \n",buff ? buff : "NULL");
    }
    mem_free(buff);

    return 0;
}

int en8811_count_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int ret = 0;

    ret = en8811_print_dbginfo_write(buffer,count,EN8811_DEBUG_COUNT);
    if(ret){
        pr_err("print count error!\n");
        return ret;
    }

    ret = en8811_print_dbginfo_write(buffer,count,EN8811_DEBUG_COUNT_PLUS);
    if(ret){
        pr_err("print count error!\n");
        return ret;
    }
    return count;
}

int en8811_dbg_regs_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int ret = 0;

    ret = en8811_print_dbginfo_write(buffer,count,EN8811_DEBUG_REGS);
    if(ret){
        pr_err("print regs error!\n");
        return ret;
    }

    return count;
}

int en8811_temp_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int ret = 0;

    ret = en8811_print_dbginfo_write(buffer,count,EN8811_DEBUG_TEMP);
    if(ret){
        pr_err("print temp error!\n");
        return ret;
    }

    return count;
}

int en8811_cable_diag_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int ret = 0;
    char cmd[CMD_LEN] = {0};
    struct phy_device* phyDev = NULL;
	uint32_t pbus_value = 0;
    PARSE_DATA_T parse_data;
    Mdio_priv_data_t *mdio_priv_data_p = NULL;
    char *phyItfName = NULL;

    mdio_priv_data_p = en8811_priv_p;
    if(!CHECK_PHY_PRIV(mdio_priv_data_p)){
        return count;
    }
    
    memset(&parse_data, 0, sizeof(PARSE_DATA_T));
    ret = EN8811_check_writeproc_para(buffer,count,&parse_data,mdio_priv_data_p);
    if(ret){
        return ret;
    }
    else{
        phyItfName = parse_data.phyItfName;
        phyDev = parse_data.phyDev;
        strcpy(cmd,parse_data.cmd);
    }

    if(!strcmp(cmd, "start")){
		pbus_value = air_buckpbus_reg_read(phyDev,0x3b3c);
		printk("MD32 FW Version: %08x\n", pbus_value);
		ret = airphy_cable_step1(phyDev);
		
	} else {
            printk("echo start itf=[itfname] > /proc/tc3162/en8811_cable_diag\n");
			goto CHECK_INPUT;
    }
    return count;
CHECK_INPUT:
        pr_err("Please check the input parameter!\n");
        return -EINVAL;
}
/************************************************************************
*                  PROC INIT AND EXIT FUNCTIONS                         *
*************************************************************************
*/
int en8811_proc_init(void)
{
    struct proc_dir_entry *en8811_proc;

    /* set debug level */
    en8811_proc = create_proc_entry("tc3162/en8811_debug", 0, NULL);
    if(en8811_proc) 
    {
        en8811_proc->read_proc = en8811_debug_level_read_proc;
        en8811_proc->write_proc = en8811_debug_level_write_proc;
    }
    
    en8811_proc = create_proc_entry("tc3162/en8811_serdes", 0, NULL);
    if(en8811_proc) 
    {
        en8811_proc->read_proc = en8811_serdes_info_read_proc;
        en8811_proc->write_proc = NULL;
    }

    en8811_proc = create_proc_entry("tc3162/en8811_link_st", 0, NULL);
    if(en8811_proc) 
    {
        en8811_proc->read_proc = en8811_link_state_read_proc;
        en8811_proc->write_proc = NULL;
    }

    en8811_proc = create_proc_entry("tc3162/en8811_speed_mode", 0, NULL);
    if(en8811_proc) 
    {
        en8811_proc->read_proc = en8811_speed_mode_read_proc;
        en8811_proc->write_proc = en8811_speed_mode_write_proc;
    }

    en8811_proc = create_proc_entry("tc3162/en8811_power", 0, NULL);
    if(en8811_proc) 
    {
        en8811_proc->read_proc = en8811_power_read_proc;
        en8811_proc->write_proc = en8811_power_write_proc;
    }

    en8811_proc = create_proc_entry("tc3162/en8811_version", 0, NULL);
    if(en8811_proc) 
    {
        en8811_proc->read_proc = en8811_version_read_proc;
        en8811_proc->write_proc = NULL;
    }

    en8811_proc = create_proc_entry("tc3162/en8811_pbus", 0, NULL);
    if(en8811_proc) 
    {
        en8811_proc->read_proc = en8811_pbus_read_proc;
        en8811_proc->write_proc = en8811_pbus_write_proc;
    }

    en8811_proc = create_proc_entry("tc3162/en8811_buckpbus", 0, NULL);
    if(en8811_proc) 
    {
        en8811_proc->read_proc = en8811_buckpbus_read_proc;
        en8811_proc->write_proc = en8811_buckpbus_write_proc;
    }

    en8811_proc = create_proc_entry("tc3162/en8811_count", 0, NULL);
    if(en8811_proc) 
    {
        en8811_proc->read_proc = en8811_count_read_proc;
        en8811_proc->write_proc = en8811_count_write_proc;
    }

	en8811_proc = create_proc_entry("tc3162/en8811_dbg_regs", 0, NULL);
	if(en8811_proc)
	{
		en8811_proc->read_proc = en8811_dbg_regs_read_proc;
		en8811_proc->write_proc = en8811_dbg_regs_write_proc;
	}
	en8811_proc = create_proc_entry("tc3162/en8811_temp", 0, NULL);
	if(en8811_proc)
	{
		en8811_proc->read_proc = en8811_temp_read_proc;
		en8811_proc->write_proc = en8811_temp_write_proc;
	}

	en8811_proc = create_proc_entry("tc3162/en8811_cable_diag", 0, NULL);
	if(en8811_proc)
	{
		en8811_proc->write_proc = en8811_cable_diag_write_proc;
	}

    pr_info("en8811_proc init ok!\n");
    return 0;
}

int en8811_proc_exit(void)
{
    remove_proc_entry("tc3162/en8811_debug", 0);
    remove_proc_entry("tc3162/en8811_link_st", 0);
    remove_proc_entry("tc3162/en8811_speed_mode", 0);
    remove_proc_entry("tc3162/en8811_power", 0);
    remove_proc_entry("tc3162/en8811_version", 0);
    remove_proc_entry("tc3162/en8811_pbus", 0);
    remove_proc_entry("tc3162/en8811_buckpbus", 0);
    remove_proc_entry("tc3162/en8811_count", 0);
    remove_proc_entry("tc3162/en8811_serdes", 0);
    remove_proc_entry("tc3162/en8811_dbg_regs", 0);
    remove_proc_entry("tc3162/en8811_temp", 0);
    remove_proc_entry("tc3162/en8811_cable_diag", 0);
    pr_info("en8811_proc exit ok!\n");
    return 0;
}
