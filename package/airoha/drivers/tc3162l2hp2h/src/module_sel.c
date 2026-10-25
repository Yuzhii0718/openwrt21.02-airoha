#include <linux/module.h>
#include <asm/tc3162/tc3162.h>
#include <linux/proc_fs.h>
#include <linux/version.h>
#include <linux/sched.h>
#ifdef TCSUPPORT_BOARD_SELECT
#include <linux/slab.h>
#endif
#ifdef TCSUPPORT_MIPS_1004K
#include <linux/interrupt.h>
#endif
#include <linux/mtd/rt_flash.h>
#ifndef TCSUPPORT_NP
#include "xpon_public_const.h"
#endif

#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <common/ecnt_chip_id.h>

#include "ecnt_event_global/ecnt_event_system.h"
#include <linux/kmod.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(6,6,0)
#include <linux/seq_file.h>
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(6,6,0)
static int demo_val = 0;
#endif

extern int hsgmii_lan;
extern int itf_start_idx;

#define SYS_HAS_USBHOST (1<<0)
#define SYS_HAS_2GMAC 	(1<<1)
#define SYS_HAS_WIFI 	(1<<2)
#define SYS_HAS_PTM 	(1<<3)
#define SYS_HAS_HWNAT 	(1<<4)
#define SYS_HAS_USB_DEVICE 	(1<<5)

#ifndef TCSUPPORT_NP
typedef	struct {
	unsigned char  ponMode;		
	unsigned char  scuMode;
} WanModeCmds_t;

const WanModeCmds_t scu_mode[] = {
	{XMCS_IF_WAN_DETECT_MODE_AUTO,         SYS_GPON_MODE},
	{XMCS_IF_WAN_DETECT_MODE_GPON,         SYS_GPON_MODE},
	{XMCS_IF_WAN_DETECT_MODE_EPON,         SYS_EPON_MODE},		
	{XMCS_IF_WAN_DETECT_MODE_10G_1G_EPON,  SYS_10G_1G_EPON_MODE },
	{XMCS_IF_WAN_DETECT_MODE_10G_10G_EPON, SYS_10G_10G_EPON_MODE},
	{XMCS_IF_WAN_DETECT_MODE_1G_1G_EPON,   SYS_1G_1G_EPON_MODE},
	{XMCS_IF_WAN_DETECT_MODE_XGPON,        SYS_XGPON_MODE},
	{XMCS_IF_WAN_DETECT_MODE_XGSPON,       SYS_XGSPON_MODE},
	{XMCS_IF_WAN_DETECT_MODE_NGPON2_10G_10G,	SYS_NGPON2_10G_10G_MODE},
	{XMCS_IF_WAN_DETECT_MODE_NGPON2_10G_2G, 	SYS_NGPON2_10G_2G_MODE},
	{XMCS_IF_WAN_DETECT_MODE_NGPON2_2G_2G, 		SYS_NGPON2_2G_2G_MODE},
	{XMCS_IF_WAN_DETECT_MODE_GPON_SYM, 		SYS_GPON_SYM_MODE},
	{XMCS_IF_WAN_DETECT_MODE_TURBO_EPON, 		SYS_TURBO_EPON_MODE},
};
#endif

#define SYS_WAN_SEL_MASK 0xFF
#define XPONTYPE_MASK 	(0xf0)
#define COMBOPON_MASK   (0x4)
extern unsigned long flash_base;
static unsigned char  xponMode=0;
static unsigned char  comboEnable=0;

#define  XPON_MODE_PROC	"tc3162/sys_xpon_mode"
#define  COMBO_ENABLE_PROC	"tc3162/sys_combo_enable"
#define  USBHOST_PROC	"tc3162/sys_has_usbhost"
#define  GMAC2_PROC		"tc3162/sys_has_2gmac"
#define  WIFI_PROC		"tc3162/sys_has_wifi"
#define  PTM_PROC		"tc3162/sys_has_ptm"
#define  HWNAT_PROC		"tc3162/sys_has_hwnat"
#define  USB_DEVICE_PROC	"tc3162/sys_has_usb_device"
#define  IS_FPGA_PROC	"tc3162/sys_is_fpag"
#define  CHIP_ID_PROC		"tc3162/chip_id"
#define  HARDWARE_ID_PROC		"tc3162/hid"
#define  HSGMII_ETHER_WAN_PROC "tc3162/wan_2_5"
#define  NO_PON_PROC_ENTRY "tc3162/nopon"
#define  HSGMII_ETHER_LAN_PROC "tc3162/lan_2_5"
#define  HSGMII_LAN_PROC "tc3162/hsgmii_lan"
#define  ITF_START_IDX_PROC "tc3162/itf_start_idx"
#ifdef TCSUPPORT_AUTOBENCH
#define  IS_EN7512_E1_E2_PROC	"tc3162/sys_is_7512e1e2"
#define  IS_EN7512_E1_E2_CHIP ( isEN751221 && ((regRead32(0xbfb0005c) & 0xFFFF) == 0x1))
#endif
#ifdef TCSUPPORT_MIPS_1004K
#define  IS_1004K_SUPPORT_PROC	"tc3162/sys_is_1004k_support"
extern void ecnt_register_cpu_interrupts(void);
extern void ecnt_unregister_cpu_interrupts(void);
#endif

#define  HW_CONF_REG 	0xbfb0008c

#ifndef VPint
#define VPint			*(volatile unsigned long int *)
#endif

extern void (*cpu_wait)(void);
unsigned char module_sel;

char isRT63365E1 = 0;
char check_rt63365ver1(void){
	return isRT63365E1;
}
EXPORT_SYMBOL(check_rt63365ver1);

//define global variables for unrelease-source-code module (such as tcvlantag) to use as compile options.
int wanAtmSupported = 0;
int wanPtmSupported = 0;
int wanXponSupported = 0;
int wanMultiServiceSupported = 0;

#ifdef TCSUPPORT_BOARD_SELECT
uint8_t *etherWanSupported;
uint8_t *etherLanSupported;
#else
int etherWanSupported[SERDES_PORT_CNT] = {0};
int etherLanSupported[SERDES_PORT_CNT] = {0};
#endif

static int isCreateEtherWanProc = 0;
static int isCreateNoPonProc = 0;
static int isCreateEtherLanProc = 0;
int wanEtherSupported = 0;

EXPORT_SYMBOL(wanAtmSupported);
EXPORT_SYMBOL(wanPtmSupported);
EXPORT_SYMBOL(wanEtherSupported);
EXPORT_SYMBOL(wanXponSupported);
EXPORT_SYMBOL(wanMultiServiceSupported);

typedef enum {
	ETHER_WAN = 0,
	ETHER_LAN,
} ETHER_TYPE_T;


#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
static int hsgmii_ether_wan_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	uint8_t serdes_if = 0;
	int serdes_port = 0;
	char *serdes_if_name;
	char *serdes_port_name;

	for(serdes_port = 0; serdes_port < get_serdes_port_count(); serdes_port++) {
		serdes_if = get_serdes_interface_sel((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
		serdes_port_name = get_serdes_port_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
		if(serdes_if == UNKNOW_SERDES_IF) {
			index += sprintf(buf + index, "get %s error.\n", serdes_port_name);
			return -1;
		} else {
			serdes_if_name = get_serdes_interface_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port, serdes_if);
			if(etherWanSupported[serdes_port] == 1) {
				index += sprintf(buf + index, "%s=%s\n", serdes_port_name, serdes_if_name);
			}
		}
	}

	index -= off;
	*start = buf + off;
	if (index > count) {
		index = count;
	} else {
		*eof = 1;
	}
	
	if (index < 0) {
		index = 0;
	}
	return index;
}
#endif

#if defined(TCSUPPORT_CPU_EN7581)
static int read_proc_no_pon(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	uint8_t serdes_if = 0; 
	int serdes_port = 0;
	char *serdes_if_name;
	char *serdes_port_name;

	serdes_if = get_serdes_interface_sel((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
	serdes_port_name = get_serdes_port_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
	if(serdes_if == UNKNOW_SERDES_IF) {
		index += sprintf(buf + index, "get %s error.\n", serdes_port_name);
		return -1;
	} else {
		serdes_if_name = get_serdes_interface_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port, serdes_if);
		index += sprintf(buf + index, "%s=%s\n", serdes_port_name, serdes_if_name);
		
	}
	index -= off;
	*start = buf + off;
	if (index > count) {
		index = count;
	} else {
		*eof = 1;
	}
	
	if (index < 0) {
		index = 0;
	}
	return index;
}
#endif

#if defined(TCSUPPORT_HSGMII_LAN)
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581) 
static int hsgmii_ether_lan_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	uint8_t serdes_if = 0;
	int serdes_port = 0;
	char *serdes_if_name;
	char *serdes_port_name;

	for(serdes_port = 0; serdes_port < get_serdes_port_count(); serdes_port++) {
		serdes_if = get_serdes_interface_sel((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
		serdes_port_name = get_serdes_port_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port);
		if(serdes_if == UNKNOW_SERDES_IF) {
			index += sprintf(buf + index, "get %s error.\n", serdes_port_name);
			return -1;
		} else {
			serdes_if_name = get_serdes_interface_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)serdes_port, serdes_if);
			if(etherLanSupported[serdes_port] == 1) {
				index += sprintf(buf + index, "%s=%s\n", serdes_port_name, serdes_if_name);
			}
		}
	}

	index -= off;
	*start = buf + off;
	if (index > count) {
		index = count;
	} else {
		*eof = 1;
	}
	
	if (index < 0) {
		index = 0;
	}
	return index;
}
#endif
static int hsgmii_lan_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)	
{	
	int index = 0;
	index = sprintf(buf, "%d\n", hsgmii_lan);
	index -= off;
	*start = buf + off;
	if (index > count)
	{
		index = count;
	}
	else
	{
		*eof = 1;
	}
	if (index < 0)
	{
		index = 0;
	}
	return index;
}

static int itf_start_idx_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)	
{	
	int index = 0;
	index = sprintf(buf, "%d\n", itf_start_idx);
	index -= off;
	*start = buf + off;
	if (index > count)
	{
		index = count;
	}
	else
	{
		*eof = 1;
	}
	if (index < 0)
	{
		index = 0;
	}
	return index;
}
#endif

#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_HSGMII_LAN)
#ifdef TCSUPPORT_BOARD_SELECT
static int init_ether_LAN_WAN(uint8_t srdsPortCnt)
{
	int ret = 0;

	etherWanSupported = (uint8_t *)kmalloc(sizeof(uint8_t) * srdsPortCnt, GFP_KERNEL);
	etherLanSupported = (uint8_t *)kmalloc(sizeof(uint8_t) * srdsPortCnt, GFP_KERNEL);

	if(NULL == etherWanSupported || NULL == etherLanSupported) {
		printk("error, alloc etherWanSupported or etherLanSupported fail.\n");
		ret = -1;
	}

	return ret;
}
#endif

static void initEtherLanWan(void)
{
	int port_idx = 0;
	uint8_t serdes_if = 0;
	ECNT_EVENT_SYSTEM_SERDES_ETHER_TYPE_SEL_t serdesEtherType = ECNT_EVENT_SERDES_ETHER_TYPE_NONE;

#ifdef TCSUPPORT_BOARD_SELECT
	init_ether_LAN_WAN(get_serdes_port_count());
#endif

#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581) 
		const int port_cnt = get_serdes_port_count();

		for(port_idx = 0; port_idx < port_cnt; port_idx++) {
			if(isEN7523) {
				serdes_if = get_serdes_interface_sel(port_idx);
				
				if(serdes_if == UNKNOW_SERDES_IF) {
					printk("Unknow serdes interface:%02x\n", serdes_if);
					return;
				}
				/* This field at EN7523 is only valid when the serdes interface is selected to HSGMII. */
#ifdef TCSUPPORT_BOARD_SELECT
				/*ECNT_EVENT_SERDES_PON_PON or ECNT_EVENT_SERDES_WIFI1_PCIE or
				 * ECNT_EVENT_SERDES_WIFI2_PCIE or ECNT_EVENT_SERDES_USB1_USB30
				 */
				if(serdes_if == 0) 
#else
				if(serdes_if == ECNT_EVENT_SERDES_PON_PON ||
					serdes_if == ECNT_EVENT_SERDES_WIFI1_PCIE ||
					serdes_if == ECNT_EVENT_SERDES_WIFI2_PCIE ||
					serdes_if == ECNT_EVENT_SERDES_USB1_USB30) 
#endif
				{
					continue;
				}
			}
			serdesEtherType = get_serdes_ether_type_sel((ECNT_EVENT_SYSTEM_SERDES_SEL_t)port_idx);
			serdes_if = get_serdes_interface_sel((ECNT_EVENT_SYSTEM_SERDES_SEL_t)port_idx);			
			
#if defined(TCSUPPORT_CPU_EN7581)
			if((port_idx == 0) && (serdes_if != ECNT_EVENT_SERDES_PON_PON)){
				if(isCreateNoPonProc == 0) {
					create_proc_read_entry(NO_PON_PROC_ENTRY, 0, NULL, read_proc_no_pon, NULL);
					isCreateNoPonProc = 1;
				}
			}
#endif
		
#if defined(TCSUPPORT_WAN_ETHER)
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581) 
			if(serdesEtherType == ECNT_EVENT_SERDES_ETHER_TYPE_WAN) {
				wanEtherSupported = 1;
				if(isCreateEtherWanProc == 0) {
					create_proc_read_entry(HSGMII_ETHER_WAN_PROC, 0, NULL, hsgmii_ether_wan_read_proc, NULL);
					isCreateEtherWanProc = 1;
				}
				etherWanSupported[port_idx] = 1;
			} 
#endif
#endif
#if defined(TCSUPPORT_HSGMII_LAN)
			if(serdesEtherType == ECNT_EVENT_SERDES_ETHER_TYPE_LAN) {
				if(isCreateEtherLanProc == 0) {
					create_proc_read_entry(HSGMII_ETHER_LAN_PROC, 0, NULL, hsgmii_ether_lan_read_proc, NULL);
					isCreateEtherLanProc = 1;
				}
				etherLanSupported[port_idx] = 1;
			}
#endif
		}
#else 
		etherWanSupported[0] = 1;
#if defined(TCSUPPORT_HSGMII_LAN)
		etherLanSupported[0] = 1;
#endif
#endif
}
#endif

static void initXponWan(void)
{
	wanXponSupported = 0;
	
#if defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON)
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	wanXponSupported = (get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_PON) == ECNT_EVENT_SERDES_PON_PON) ? 1 : 0;
#else
	wanXponSupported = 1;
#endif
#endif

#if defined(TCSUPPORT_NP)
	wanXponSupported = 0;
#endif
}

#if defined(TCSUPPORT_WAN_ATM) || defined(TCSUPPORT_WAN_PTM)
static void initXdsl(void)
{
#if defined(TCSUPPORT_WAN_ATM)
    wanAtmSupported = 1;
#endif

#if defined(TCSUPPORT_WAN_PTM)
    wanPtmSupported = 1;
#endif
}
#endif

#if defined(TCSUPPORT_MULTISERVICE_ON_WAN)
static void initWanMultiService(void)
{
	wanMultiServiceSupported = 1;
}
#endif

static void init_wan_mode_support(void)
{
#if defined(TCSUPPORT_WAN_ATM) || defined(TCSUPPORT_WAN_PTM)
	initXdsl();
#endif

#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_HSGMII_LAN)
	initEtherLanWan();
#endif

	initXponWan();

#if defined(TCSUPPORT_MULTISERVICE_ON_WAN)
	initWanMultiService();
#endif
}
static int need_wifi_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{	
	return 0;
}
static int need_usbhost_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{	
	return 0;
}
static int need_2gmac_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{	
	return 0;
}

static int need_ptm_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{	
	return 0;
}
static int need_hwnat_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{	
	return 0;
}

static int need_usbdev_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{	
	return 0;
}

#ifdef TCSUPPORT_AUTOBENCH
static int need_7512e1e2_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{	
	return 0;
}
#endif

#ifdef TCSUPPORT_MIPS_1004K
static int is_1004k_support_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{	
	return 0;
}
#endif

static int need_isFPGA_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{	
	return 0;
}

#ifdef TCSUPPORT_MIPS_1004K
static irqreturn_t cpu_cm_err_isr(int irq, void *dev_id)
{
	printk("\nEnter %s\n", __func__);

    /* clear CM2 error cause */
    VPint((GCMP_BASE_ADDR|0xa0000000)+0x48)=0;

	return IRQ_HANDLED ;
}

static irqreturn_t cpu_cm_pcint_isr(int irq, void *dev_id)
{
	printk("\nEnter %s\n", __func__);

    /* clear CM2 Performance Counter Overflow Status Register */
    VPint((GCMP_BASE_ADDR|0xa0000000)+0x6000+0x120)=0x7;

	return IRQ_HANDLED ;
}

void cpu_wait_for_en7512(void)
{
		volatile unsigned int ram_access = 0;
		int cnt;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,18,21)
		local_irq_disable();
#endif
		if(!need_resched()){
			for (cnt = 0; cnt < 100; cnt++)
				ram_access = VPint(0xA0000000); /* DRAM access */
		}	
		local_irq_enable();
}
#endif

uint get_wan_conf(void)
{
    #ifdef TCSUPPORT_CPU_ARMV8
    return GET_WAN_CONF();
    #else
    return regRead32(0xBFB00070);
    #endif
}

void set_wan_conf(uint reg)
{
    #ifdef TCSUPPORT_CPU_ARMV8
    SET_WAN_CONF(reg);
    #else
    regWrite32(0xBFB00070,reg); 
    #endif
    return;
}

#ifndef TCSUPPORT_NP
void wan_xpon_mode_set(void)
{
	uint reg=0;
    uint type=0;
    uint num =0;
    uint i=0;

#if defined(TCSUPPORT_CPU_EN7581)
	type = get_onutype(); 
#else
    type = READ_FLASH_BYTE(flash_base + XPON_MODE_FLASH_ADDR);
#endif
    xponMode =(type & XPONTYPE_MASK)>>4;
	comboEnable=(type & COMBOPON_MASK)>>2;

    num= sizeof(scu_mode)/sizeof(WanModeCmds_t);
    reg = get_wan_conf();

    for(i=0;i<num;i++){
        if(scu_mode[i].ponMode == xponMode){
            reg &= ~(SYS_WAN_SEL_MASK);
            reg |= scu_mode[i].scuMode;
            break;
        }
    }
    if(i >= num)
        printk("XPON WAN Mode Configuration Error. xponMode:%d\n", xponMode);
    else {
        set_wan_conf(reg);
    }
    
}

static int xpon_mode_read_proc(char *page, char **start, off_t off,int count, int *eof, void *data)    
{
	int len;
    
	len = sprintf(page, "%d\n", xponMode) ;
    len -= off;
    *start = page + off;

    if (len > count)
        len = count;
    else
        *eof = 1;

    if (len < 0)
        len = 0;

	return len;
}

static int combo_enable_read_proc(char *page, char **start, off_t off,int count, int *eof, void *data)    
{
	int len;
    
	len = sprintf(page, "%d\n", comboEnable) ;
    len -= off;
    *start = page + off;

    if (len > count)
        len = count;
    else
        *eof = 1;

    if (len < 0)
        len = 0;

	return len;
}

#endif


#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
void check_low_power(void){
	unsigned int val;
	struct file *fp;

	if(GET_POWER_SAVING){
		fp = filp_open("tmp/low_power", O_RDWR|O_CREAT, 0644);
		if(IS_ERR(fp)){
			printk("create file error\r\n");
		}
		filp_close(fp, NULL);
	}
}

static int hid_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;

#ifdef TCSUPPORT_CPU_ARMV8
	len += sprintf(buf + len, "%x\n", GET_HIR());
#else
	len += sprintf(buf + len, "%x\n", ((regRead32(0xBFB00064)>>16) & (0xFFFF)));
#endif
	len -= off;
	*start = buf + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}

static int chip_id_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	unsigned int chidIdx = END_PACKAGE_ID;    
	int i;
	chipInformation chip_id_table[] = 
	{
		/* Add newest package id of IC at here */
		/* AN7583 */
		{AN7583GT,	isAN7583GT},
		{AN7583GIT,	isAN7583GIT},
		{AN7583CT,	isAN7583CT},
		{AN7583DT,	isAN7583DT},
		{AN7583ST,	isAN7583ST},
		{AN9510GT,	isAN9510GT},
		{AN7553GT,	isAN7553GT},
		{AN7553CT,	isAN7553CT},
		{AN7567GT,	isAN7567GT},
		{AN7567CT,	isAN7567CT},
		{AN7583ET,	isAN7583ET},
		{AN7583EIT,	isAN7583EIT},
		{AN7583FG,	isAN7583FG},
		{AN7583FP,	isAN7583FP},
		{AN7583FD,	isAN7583FD},
		{AN7583FS,	isAN7583FS},
		{AN7583FF,	isAN7583FF},
		
		/* AN7552 */
		{AN7552CT,	isAN7552CT},
		{AN7552ST,	isAN7552ST},
		{AN7552FT,	isAN7552FT},
		{AN7563CT,	isAN7563CT},
		{AN7563PT,	isAN7563PT},
		/* EN7581 */
		{AN7581GT,	isAN7581GT},
		{AN7566GT,	isAN7566GT},
		{AN7581PT,	isAN7581PT},
		{AN7581ST,	isAN7581ST},
		{AN7551PT,	isAN7551PT},
		{AN7581CT,	isAN7581CT},
		{AN7581DT,	isAN7581DT},
		{AN7581FG,	isAN7581FG},
		{AN7581FP,	isAN7581FP},
		{AN7581FD,	isAN7581FD},
		{AN7551GT,	isAN7551GT},
		{AN7566PT,	isAN7566PT},
		{AN7581IT,	isAN7581IT},
		{AN7581SIT,	isAN7581SIT},

		/* EN7523 */
		{EN7529DU,	isEN7529DU},
		{EN7529DT,	isEN7529DT},
		{EN7529CU,	isEN7529CU},
		{EN7562DU,	isEN7562DU},
		{EN7562DT,	isEN7562DT},
		{EN7562CU,	isEN7562CU},
		{EN7523GU,	isEN7523GU},
		{EN7523DU,	isEN7523DU},
		{EN7529GTH,	isEN7529GTH},
		{EN7562GTH,	isEN7562GTH},
		{EN7523SU,	isEN7523SU},
		{EN7529GTS,	isEN7529GTS},
		{EN7562GTS,	isEN7562GTS},
		{EN7529IT,	isEN7529IT},
		{EN7529CT,	isEN7529CT},
		{EN7562CT,	isEN7562CT},

		{EN7523DT,	isEN7523DT},
		{EN7529DTM,	isEN7529DTM},
		{EN7562DTM,	isEN7562DTM},
		{EN7529ITM,	isEN7529ITM},
		{EN7529CTM,	isEN7529CTM},
		{EN7562CTM,	isEN7562CTM},
		{EN7523DTM,	isEN7523DTM},
		
		/* EN7528 */
		{EN7528HU, isEN7528HU},
		{EN7528DU, isEN7528DU},
		{EN7561DU, isEN7561DU},
		{EN7526FH_EN7528DU, isEN7526FH_EN7528DU},
		{EN7521G_EN7528DU, isEN7521G_EN7528DU},

		/* EN7580 */
		{EN7580GT, isEN7580GT},
		{EN7580ST, isEN7580ST},
		{EN7580GAT, isEN7580GAT},
		{EN7565, isEN7565},

		/* EN7516 */
		{EN7516G, isEN7516G},

		/* EN7527 */
		{EN7527G, isEN7527G},
		{EN7561G, isEN7561G},

		/* EN7512 */
		{EN7512, isEN7512},
		{EN7513, isEN7513},
		{EN7513G, isEN7513G},

		/* EN7521, EN7521FC */
		{EN7521FCUD, isEN7521FCUD},
		{EN7521F, isEN7521F},
		{EN7521S, isEN7521S},
		{EN7526D, isEN7526D},
		{EN7526F, isEN7526F},
		{EN7526G, isEN7526G},
		{EN7526FT, isEN7526FT},
		{EN7526FP, isEN7526FP},
		{EN7526FT_C, isEN7526FT_C},
		{END_PACKAGE_ID, END_PACKAGE_ID}
	};
	
	for (i = 0; chip_id_table[i].chidIdx != END_PACKAGE_ID; i++) {
		if(chip_id_table[i].chipId) {
			chidIdx = chip_id_table[i].chidIdx;
			break;
		}
	}

	index = sprintf(buf, "%d\n", chidIdx);

	index -= off;
	*start = buf + off;

	if (index > count)
		index = count;
	else
		*eof = 1;

	if (index < 0)
		index = 0;

	return index;
}
#endif

#if defined(TCSUPPORT_HSGMII_LAN)	
static void int_itf_start_idx(void)
{
#if 0
	itf_start_idx = 2;
#else
	itf_start_idx = 1;
#endif
	create_proc_read_entry(ITF_START_IDX_PROC, 0, NULL, itf_start_idx_read_proc, NULL);
}

static void init_hsmii_lan(void)
{
#if 0
/*
 * hsgmii LAN  port is inserted in front of the switch LAN port.
 * hsgmii LAN |            switch Lan 
 *   eht0.1   | eth0.2 | eth0.3 | eth0.4 | eth0.5
 */
	hsgmii_lan = 1;
#else
/*
 * hsgmii LAN  port is inserted behind the switch LAN port.
 *             switch Lan               | hsgmii LAN
 *   eth0.1 | eth0.2 | eth0.3 | eth0.4  |    eth0.5 
 */
	hsgmii_lan = 0;
#endif
	create_proc_read_entry(HSGMII_LAN_PROC, 0, NULL, hsgmii_lan_read_proc, NULL);
}
#endif

#ifdef TCSUPPORT_WLAN_INODE
extern int inode_npu_en;
static int ecnt_inode_npu_en_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	printk("inode_npu_en = %d\n",inode_npu_en);

	return 0;
}

static int ecnt_inode_npu_en_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[32] = {0};
	char name[32] = {0};
	int flag = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	if (sscanf(val_string, "%d", &flag) != 1) {
		return 	-EFAULT;
	}

	inode_npu_en = flag;
	printk("inode_npu_en = %d\n",inode_npu_en);

	return count;
}
#endif

#ifdef TCSUPPORT_BOARD_SELECT
static void free_ether_LAN_WAN(void)
{
	if(etherWanSupported) {
		kfree(etherWanSupported);
	}
	
	if(etherLanSupported) {
		kfree(etherLanSupported);
	}
}
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(6,6,0)
static ssize_t demo_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	char user_buf[32] = {0};
	size_t len = count, buf_length = 0;
	loff_t pos = *ppos;
	unsigned long ret = 0;
	ssize_t retval = 0;
	
	/* use memory size for data stream or use length of string buffer */
	buf_length = snprintf(user_buf, sizeof(user_buf), "%d", demo_val);
	
	/* valid check */
	if ( pos >= buf_length )
		return 0;
	if ( len >= buf_length )
		len = buf_length;
	if ( pos + len > buf_length )
		len = buf_length - pos;
	
	/* ret contains the amount of chars wasn't successfully written to buf */
	ret = copy_to_user(buf, user_buf + pos, len);
	
	*ppos += len;
	retval = len - ret;

	return retval;
}

static ssize_t demo_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char val_string[32];
	int val = 0;
	
	/* valid check */
	if ( count > sizeof(val_string) - 1 )
	{
		printk("Input buf is out of memory!\n");
		return -EINVAL;
	}
	copy_from_user(val_string, buf, count);
	val_string[count] = '\0';
	
	sscanf(val_string, "%d", &val);
	demo_val = val;
	
	return count;
}

/* demo_show could be called by single_open(file, demo_show, NULL), and if in this case the proc_read is not required */
static int demo_show(struct seq_file *file, void *v)
{
	seq_printf(file, "demo_val = [%d]\n", demo_val);
	return 0;
}

static int demo_open(struct inode *inode, struct file *file)
{
	return single_open(file, NULL, NULL);
}

static const struct proc_ops proc_demo_ops = {
	.proc_open	= demo_open,
	.proc_read	= demo_read,
	.proc_write	= demo_write,
	.proc_lseek	= seq_lseek,
	.proc_release	= single_release,
};
#endif

static int __init ra_modsel_init(void)
{
	unsigned int x;
#if LINUX_VERSION_CODE > KERNEL_VERSION(6,6,0)
	struct proc_dir_entry *proc_demo;
#endif

	init_wan_mode_support();
#if defined(TCSUPPORT_HSGMII_LAN)	
	int_itf_start_idx();
	init_hsmii_lan();
#endif

#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
	check_low_power();
#endif
    if(isEN7523) {
		module_sel = SYS_HAS_USBHOST | SYS_HAS_WIFI | SYS_HAS_HWNAT;
	}else if(isTC3182){
		x = VPint(HW_CONF_REG);
		printk("3182 config value %x\n",x);
		if((x&(1<<8)) == 0){ //tc3182 ldv
			printk("System has only 2gmac \n");
			module_sel = SYS_HAS_2GMAC;
		}else{
			if(((x&(1<<10)) == 0) &&
				(x&((1<<20)|(1<<21)|(1<<22)|(1<<23))) == ((1<<21)|(1<<22)|(1<<23))){
				printk("System has wifi, usb and 1gmac \n");
				module_sel = SYS_HAS_USBHOST | SYS_HAS_WIFI ;
			}else{
				printk("System has wifi,usb and 2gmac \n");
				module_sel = SYS_HAS_USBHOST | SYS_HAS_2GMAC | SYS_HAS_WIFI ;
			}
		}
		module_sel |= SYS_HAS_USB_DEVICE;
	}else if (isRT65168) {
		module_sel = SYS_HAS_USBHOST | SYS_HAS_2GMAC | SYS_HAS_WIFI | SYS_HAS_PTM | SYS_HAS_HWNAT;
	}else if (isRT63365) {
		module_sel = SYS_HAS_USBHOST | SYS_HAS_WIFI | SYS_HAS_HWNAT;
		if ( (VPint(0xbfb00064) & (0xffff)) == 0x0 )
			isRT63365E1 = 1;
	}else if (isRT63260) {
		if(VPint(HW_CONF_REG) & (1<<9)){
			module_sel = SYS_HAS_USB_DEVICE;	
		}else{
			module_sel = 0;	
		}
#if defined(TCSUPPORT_CPU_MT7510) && !defined(TCSUPPORT_BONDING)
	//use SCU_RST to hold PTM MAC reset when vdsl is link down.
	}else if (isMT751020) {
		x = VPint(0xbfb00834);
		x |= (1<<5);
		VPint(0xbfb00834) = x;

		module_sel = SYS_HAS_USBHOST | SYS_HAS_WIFI | SYS_HAS_HWNAT;
#endif
	}else if(isEN7528) {
		module_sel = SYS_HAS_USBHOST | SYS_HAS_WIFI | SYS_HAS_HWNAT;
	}else if(isEN751627) {
		module_sel = SYS_HAS_USBHOST | SYS_HAS_WIFI | SYS_HAS_HWNAT;
		if (wanXponSupported){
			 /* Power down DMTC, and keep VD PL still on. */
			VPint(0xBFA20178) = 0x3;
			VPint(0xBFB00084) = 0x3;
		}else{
			 /* Power down PON PHY since xPON is not supported */
			 /* disable PON interface in ecnt_global_chip_init.h */
			//VPint(0xBFB00830) |= 1;
		}
	}else if(isEN7580) {
		module_sel = SYS_HAS_USBHOST | SYS_HAS_WIFI | SYS_HAS_HWNAT;
	}else if(isEN751221 || isMT751020) {
		module_sel = SYS_HAS_USBHOST | SYS_HAS_WIFI | SYS_HAS_HWNAT;
		if (isEN751221){// only for EN751221 for power saving
			/* Power down SIMLDO */
			VPint(0xBFA20168) |= (1<<8);
			if (wanXponSupported){
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
				if(isEN7526c){
					/* Power down DMT SRAM & FNLL since xDSL is not supported */
					VPint(0xBFA2011C) = 0x2;
					VPint(0xBFA20120) = 0x100020;
				}
				else{
					/* Power down DMTC, and keep VD PL still on. */
					VPint(0xBFA2011C) = 0x3;
					VPint(0xBFA20148) = 0xFEFF001C;
					VPint(0xBFB00084) = 0x3;
				}
#endif
			}
			else{
				/* Power down PON PHY since xPON is not supported */
				VPint(0xBFB00830) |= 1;
                #ifdef TCSUPPORT_MIPS_1004K
				if(isEN7512){
					printk("change wait function for cpu_wait_en7512\n");
					cpu_wait = cpu_wait_for_en7512;
				}
                #endif
			}
		}
	}else
	{//3162u now support 1 gmac , usb host
		module_sel = SYS_HAS_USBHOST | SYS_HAS_WIFI | SYS_HAS_USB_DEVICE;
	}

	if(module_sel & SYS_HAS_WIFI)
	 	create_proc_read_entry(WIFI_PROC, 0, NULL, need_wifi_read_proc, NULL);
	if(module_sel & SYS_HAS_2GMAC) 
	 	create_proc_read_entry(GMAC2_PROC, 0, NULL, need_2gmac_read_proc, NULL);
	if(module_sel & SYS_HAS_USBHOST) 
	 	create_proc_read_entry(USBHOST_PROC, 0, NULL, need_usbhost_read_proc, NULL);
	if(module_sel & SYS_HAS_PTM)
	 	create_proc_read_entry(PTM_PROC, 0, NULL, need_ptm_read_proc, NULL);
	if(module_sel & SYS_HAS_HWNAT)
	 	create_proc_read_entry(HWNAT_PROC, 0, NULL, need_hwnat_read_proc, NULL);
	if(module_sel & SYS_HAS_USB_DEVICE)
	 	create_proc_read_entry(USB_DEVICE_PROC, 0, NULL, need_usbdev_read_proc, NULL);
	if(isFPGA)
	 	create_proc_read_entry(IS_FPGA_PROC, 0, NULL, need_isFPGA_read_proc, NULL);	
#ifdef TCSUPPORT_AUTOBENCH
	if(IS_EN7512_E1_E2_CHIP)
	 	create_proc_read_entry(IS_EN7512_E1_E2_PROC, 0, NULL, need_7512e1e2_read_proc, NULL);
#endif

#if defined(TCSUPPORT_MIPS_1004K) || defined(TCSUPPORT_CPU_ARMV8)
	ecnt_register_cpu_interrupts();
#endif

#ifdef TCSUPPORT_MIPS_1004K
	create_proc_read_entry(IS_1004K_SUPPORT_PROC, 0, NULL, is_1004k_support_read_proc, NULL);

    //ecnt_register_cpu_interrupts();

	if(request_irq(CPU_CM_ERR, cpu_cm_err_isr, 0, "cpu_cm_err", NULL) != 0)
		printk("\nrequest_irq for CPU_CM_ERR failed\n") ;
	if(request_irq(CPU_CM_PCINT, cpu_cm_pcint_isr, 0, "cpu_cm_pcint", NULL) != 0)
		printk("\nrequest_irq for CPU_CM_PCINT failed\n") ;	
#endif
#ifndef TCSUPPORT_NP
    if(isEN7580 || isEN7581 || isAN7583){   
        wan_xpon_mode_set();
        create_proc_read_entry(XPON_MODE_PROC, 0, NULL, xpon_mode_read_proc, NULL);
		create_proc_read_entry(COMBO_ENABLE_PROC, 0, NULL, combo_enable_read_proc, NULL);
    }
#endif
#ifdef TCSUPPORT_LITTLE_ENDIAN
/*lan sel set as arbitor mode, so that packet can be sent to wdma*/
	if(isEN7528){
		x = VPint(0xBFB00070); 
		x &= (~0x30);
		x |= 0x20;    
		VPint(0xBFB00070) = x;   
	}
#endif

#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
	create_proc_read_entry(CHIP_ID_PROC, 0, NULL, chip_id_read_proc, NULL);	
	create_proc_read_entry(HARDWARE_ID_PROC, 0, NULL, hid_read_proc, NULL);
#endif

#ifdef TCSUPPORT_WLAN_INODE
	struct proc_dir_entry *inode_npu_en_proc = NULL;

	inode_npu_en_proc = create_proc_entry("tc3162/npu_en_inode", 0, NULL);
	if(inode_npu_en_proc) {
		inode_npu_en_proc->read_proc = ecnt_inode_npu_en_read_proc;
		inode_npu_en_proc->write_proc = ecnt_inode_npu_en_write_proc;
	}
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(6,6,0)
	proc_demo = proc_create("tc3162/demo", 0, NULL, &proc_demo_ops);
	if ( !proc_demo )
	{
		printk("create proc tc3162/demo failed!\n");
		return -1;
	}
#endif

	return 0;
}
 
static void __exit ra_modsel_exit(void){
#if defined(TCSUPPORT_HSGMII_LAN)
	remove_proc_entry(ITF_START_IDX_PROC, 0);
	remove_proc_entry(HSGMII_LAN_PROC, 0);
#endif

#if defined(TCSUPPORT_CPU_EN7581)
	if(isCreateNoPonProc) {
		remove_proc_entry(NO_PON_PROC_ENTRY, 0);
	}
#endif

	if(isCreateEtherWanProc) {
		remove_proc_entry(HSGMII_ETHER_WAN_PROC, 0);
	}

#ifdef TCSUPPORT_WLAN_INODE
	remove_proc_entry("tc3162/npu_en_inode", 0);
#endif

#if defined(TCSUPPORT_HSGMII_LAN)	
	if(isCreateEtherLanProc)	{		
		remove_proc_entry(HSGMII_ETHER_LAN_PROC, 0);
	}
#endif
	if(module_sel & SYS_HAS_WIFI)
		remove_proc_entry(WIFI_PROC, 0);
	if(module_sel & SYS_HAS_2GMAC) 
		remove_proc_entry(GMAC2_PROC, 0);
	if(module_sel & SYS_HAS_USBHOST) 
		remove_proc_entry(USBHOST_PROC, 0);
	if(module_sel & SYS_HAS_PTM)
		remove_proc_entry(PTM_PROC, 0);
	if(module_sel & SYS_HAS_HWNAT)
		remove_proc_entry(HWNAT_PROC, 0);
	if(module_sel & SYS_HAS_USB_DEVICE)
		remove_proc_entry(USB_DEVICE_PROC, 0);
	if(isFPGA)
	 	remove_proc_entry(IS_FPGA_PROC, 0);
#ifdef TCSUPPORT_AUTOBENCH
	if(IS_EN7512_E1_E2_CHIP)
	 	remove_proc_entry(IS_EN7512_E1_E2_PROC, 0);
#endif
#if defined(TCSUPPORT_MIPS_1004K) || defined(TCSUPPORT_CPU_ARMV8)
	ecnt_unregister_cpu_interrupts();
#endif
#ifdef TCSUPPORT_MIPS_1004K
	remove_proc_entry(IS_1004K_SUPPORT_PROC, 0);

    free_irq(CPU_CM_ERR, NULL);
    free_irq(CPU_CM_PCINT, NULL);
#endif
#ifndef TCSUPPORT_NP
    if(isEN7580 || isEN7581 || isAN7583){
        remove_proc_entry(XPON_MODE_PROC, 0);
		remove_proc_entry(COMBO_ENABLE_PROC, 0);
	}
#endif



#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
	remove_proc_entry(CHIP_ID_PROC, 0);
	remove_proc_entry(HARDWARE_ID_PROC, 0);
#endif

#ifdef TCSUPPORT_BOARD_SELECT
#if defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_HSGMII_LAN)
	free_ether_LAN_WAN();
#endif
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(6,6,0)
	remove_proc_entry("tc3162/demo", NULL);
#endif

	return;
}
module_init(ra_modsel_init);
module_exit(ra_modsel_exit);
