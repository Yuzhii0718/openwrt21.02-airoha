/************************************************************************
 *
 *	Copyright (C) 2007 Trendchip Technologies, Corp.
 *	All Rights Reserved.
 *
 * Trendchip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Trendchip Technologeis, Co.
 *
 *************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/proc_fs.h>
#ifdef TCSUPPORT_CPU_ARMV8
#include <asm/uaccess.h>
#include <modules/npu/npuMboxAPI.h>
#else
#include <asm/addrspace.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/tc3162/kprofile_hook.h>
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/uaccess.h>
#endif
#include <asm/io.h>
#include <asm/tc3162/cmdparse.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/dma-mapping.h>
#include "tcci.h"

extern int subcmd(const cmds_t tab[], int argc, char *argv[], void *p);
extern int cmd_register(cmds_t *cmds_p);

static int doSys(int argc, char *argv[], void *p);
static int doSysMemrl(int argc, char *argv[], void *p);
static int doSysMemwl(int argc, char *argv[], void *p);
static int doSysModifyBit(int argc, char *argv[], void *p);
static int doSysCheckField(int argc, char *argv[], void *p);
static int doSysMemwlField(int argc, char *argv[], void *p);
static int doSysMemrlField(int argc, char *argv[], void *p);
static int doSysMemwl2(int argc, char *argv[], void *p);
static int doSysMemrw(int argc, char *argv[], void *p);
static int doSysMemww(int argc, char *argv[], void *p);
static int doSysMemory(int argc, char *argv[], void *p);
static int doSysMemcpy(int argc, char *argv[], void *p);
static int doSysFillMem(int argc, char *argv[], void *p);
static int doSysMac(int argc, char *argv[], void *p);
static int doSysOnuType(int argc, char *argv[], void *p);
static int doSysBBF247(int argc, char *argv[], void *p);
static int doSysComboPon(int argc, char *argv[], void *p);
static int doSysPONMode(int argc, char *argv[], void *p);
static int doSysQdmaInit(int argc, char *argv[], void *p);
#if defined(NPU_BMGR_TEST)
#if defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
static int doGetNpuValue(int argc, char *argv[], void *p);
static int doNpuHwBufferTest(int argc, char *argv[], void *p);
#endif
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_WAN_UPSTREAM_REMARK)
static int doSysRemark(int argc, char *argv[], void *p);
#endif/*TCSUPPORT_COMPILE*/
static int doSysExMdio(int argc, char *argv[], void *p);

static int doExMdioConf(int argc, char *argv[], void *p);
static int doExMdioWrite(int argc, char *argv[], void *p);
static int doExMdioRead(int argc, char *argv[], void *p);

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BOOTLOADER_MODIFY_PVNAME)
static int doSysAtsp(int argc, char *argv[], void *p);
static int doSysAtsv(int argc, char *argv[], void *p);
static int doSysAtspv(int argc, char *argv[], void *p);
#endif/*TCSUPPORT_COMPILE*/

#if defined(TCSUPPORT_WAN2LAN_EXT)
static int doLan2lan(int argc, char *argv[], void *p);
#define GSW_BASE        0xBFB58000
#define GSW_PORT_BASE     0x2000
#define GSW_MAC_FC    0x10
#define GSW_PCL(n) GSW_PORT_BASE + (n)*0x100 + 0x04
#define read_reg_word(reg) 		regRead32(reg)
#define write_reg_word(reg, wdata) 	regWrite32(reg, wdata)
extern uint32 (*gswPbusRead_hook)(uint32 pbus_addr);
extern int (*gswPbusWrite_hook)(uint32 pbus_addr, uint32 pbus_data);
extern int (*lanPortmap_hook)(int lanPort);
extern int (*lanPortmap_rev_hook)(int switchPort);
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_WAN_ETHER) && defined(TCSUPPORT_FH_ENV)
#define FH_ETHER_WAN_WAN_PORT_ID               (4)
#define FH_ETHER_WAN_WAN_PORT_MAP_SWITCH_ID    (4)
#endif/*TCSUPPORT_COMPILE*/
#endif
#ifdef WAN2LAN
static int doWan2lan(int argc, char *argv[], void *p);
int my_atoi(const char *str);
int masko_on_off = 0;
#if defined(TCSUPPORT_WAN2LAN_EXT)
int masko_ext = 0;
#endif

#if defined(TCSUPPORT_WAN2LAN_EXT)
int masko = 0;
#else
int masko = 0xF;
#endif
EXPORT_SYMBOL(masko_on_off);
#if defined(TCSUPPORT_WAN2LAN_EXT)
EXPORT_SYMBOL(masko_ext);
#endif
EXPORT_SYMBOL(masko);

#if defined(TCSUPPORT_CMDPROMPT)
int print_usage=0;
int print_state=0;
int print_portmask=0;
int print_errormsg=0;
#define WAN2LAN_INFO_USAGE "Usage: wan2lan [on||off] <number> \r\n"
#define WAN2LAN_INFO_ON "Current wan2lan feature status: on \r\n"
#define WAN2LAN_INFO_OFF "Current wan2lan feature status: off \r\n"
#define WAN2LAN_INFO_PORTMASK "Current portmask number is[%d] \r\n"
#define WAN2LAN_INFO_ERRORMSG "input portmask number is out of range(available value is 0~15) \r\n"
#endif
int wlan_masko_on_off = 0;
EXPORT_SYMBOL(wlan_masko_on_off);
int wlan_masko = 0xF;
EXPORT_SYMBOL(wlan_masko);
static int doWlan2lan(int argc, char *argv[], void *p);
#endif

#if defined(TCSUPPORT_WAN_EPON) || defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_PTM) || (defined(TCSUPPORT_CT_E8B_ADSL) && defined(TCSUPPORT_CPU_MT7505))
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7580)
#define SYS_MAC_HELP_INFO "sys mac <mac_addr> / sys mac <mac_addr> <-n> / sys mac <mac_addr> <mac_range> / sys mac <mac_addr> <mac_range> <-n>"
#else
#define SYS_MAC_HELP_INFO "sys mac <mac_addr> / sys mac <mac_addr> <-n>"
#endif
#else
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7580)
#define SYS_MAC_HELP_INFO "sys mac <mac_addr> / sys mac <mac_addr> <mac_range>"
#else
#define SYS_MAC_HELP_INFO "sys mac <mac_addr>"
#endif
#endif
#if defined(TCSUPPORT_CPU_ARMV8)
#define SYS_QDMAINIT_HELP_INFO "sys qdmainit [para1] [value1] [para2] [value2]...[paraN] [valueN]\nparameter:\nlan_dscp_mode [0-dram 1-sram]\nlan_payload_size [0-2048 1-1024 2-512 3-256]\n\
wan_dscp_mode [0-dram 1-sram]\nwan_payload_size [0-2048 1-1024 2-512 3-256]\nsupport blow parameters in arm:\nwan_fastpath [0/1 - hwnat fastpath to GDM2(ATM/PTM/PON..)]\nlan_fastpath [0/1 - hwnat fastpath to GDM1(GSW)]\n\
xsi_pcie0_fastpath [0/1 - hwnat fastpath to HSGMII PCIE0; 2 - direct to GDM; 3 - automode]\nxsi_pcie1_fastpath [0/1 - hwnat fastpath to HSGMII PCIE1; 2 - direct to GDM; 3 - automode]\n\
xsi_usb_fastpath [0/1 - hwnat fastpath to HSGMII USB; 2 - direct to GDM; 3 - automode]\nxsi_ether_fastpath [0/1 - hwnat fastpath to HSGMII/XFI ETHER; 2 - direct to GDM; 3 - automode]\n\
xsi_pon_fastpath [0/1 - hwnat fastpath to HSGMII/XFI PON; 2 - direct to GDM; 3 - automode]\nwhole_value [32bit whole value in hex]\n"
#else
#define SYS_QDMAINIT_HELP_INFO "sys qdmainit [para1] [value1] [para2] [value2]...[paraN] [valueN]\nparameter:\nlan_dscp_mode [0-dram 1-sram]\nlan_payload_size [0-2048 1-1024 2-512 3-256]\n\
wan_dscp_mode [0-dram 1-sram]\nwan_payload_size [0-2048 1-1024 2-512 3-256]\n"
#endif
#define SYS_HYBRID_HELP_INFO "sys hybrid enable/disable"
/*#define TCSUPPORT_GDMA_TEST 1*/
/*#define TCSUPPORT_TIMER_TEST 1*/

#if defined(TCSUPPORT_BUS_TEST)
#ifndef TCSUPPORT_GDMA_TEST
#define TCSUPPORT_GDMA_TEST 1
#endif
#ifdef TCSUPPORT_KPROFILE
extern int doCpuProfiling(int argc, char *argv[], void *p);
#endif
extern int doCpuCycles(int argc, char *argv[], void *p);
extern int doRdBypassWtTest(int argc, char *argv[], void *p);
extern int doSyncCmdTest(int argc, char *argv[], void *p);
extern int doSramTest(int argc, char *argv[], void *p);
extern int bustest_kthread_process(int argc, char *argv[]);
extern int gdmacopy_counting(unsigned int cnt);
extern void bustest_kthread_init(void);
extern int doBusData(int argc, char *argv[], void *p);
extern int doBusUtilCntTest(int argc, char *argv[], void *p);
#ifdef TCSUPPORT_CPU_ARMV8
extern int doCacheApiTest(int argc, char *argv[], void *p);
extern int doL2cSramTest(int argc, char *argv[], void *p);
extern int doCpuWt_dmaNpuRd(int argc, char *argv[], void *p);
extern int dohighMemTest(int argc, char *argv[], void *p);
#ifdef MBOX_API_TEST
extern int doMboxAPItest(int argc, char *argv[], void *p);
#endif
#if defined(TCSUPPORT_CPU_AN7583)
extern int doR2cRdbypassWt(int argc, char *argv[], void *p);
#endif
#endif
#endif

#if defined(TCSUPPORT_GDMA_TEST)
#define TC_GDMA_BASE     	0xBFB30000
#define	TC_GDMA_SA0    	    0xBFB30000
#define	TC_GDMA_DA0    	    0xBFB30004
#define	TC_GDMA_CT00    	0xBFB30008
#define	TC_GDMA_CT10    	0xBFB3000c
#define TC_GDMA_DONEINT  	(TC_GDMA_BASE + 0x204)
#define GDMA_CH_NUM         8
#define GDMA_MAX_BURST      4
#ifndef TCSUPPORT_CPU_ARMV8
#define TC_GDMA_TEST_SIZE	1024
#ifndef VPint
#define VPint			*(volatile unsigned long int *)
#endif
#endif
int doGdmaTest(int argc, char *argv[], void *p);
int dofesramtest(int argc, char *argv[], void *p);
#endif

#ifdef TCSUPPORT_CPU_ARMV8
#define DT_MEMORY_BASE  0x80000000 /* should be got from kernel later */
#define DT_MEMORY_SIZE  0x40000000 /* should be replaced by the result of DDR Calibration later */
static int dump(unsigned long addr, unsigned long len, unsigned long phyAddr);
#endif

#if defined(TCSUPPORT_TIMER_TEST)
#define TIMER0_IRQ_SRC              (4)
#define TIMER1_IRQ_SRC              (5)
#define TIMER2_IRQ_SRC              (6)
#define WDOG_IRQ_SRC                (9)
#define MIPS_IRQ_SRC_OFFSET         (1)

#define CONFIG_TIMER0_IRQ			(TIMER0_IRQ_SRC + MIPS_IRQ_SRC_OFFSET)
#define CONFIG_TIMER1_IRQ			(TIMER1_IRQ_SRC + MIPS_IRQ_SRC_OFFSET)
#define CONFIG_TIMER2_IRQ			(TIMER2_IRQ_SRC + MIPS_IRQ_SRC_OFFSET)
#define CONFIG_WDOG_IRQ			    (WDOG_IRQ_SRC + MIPS_IRQ_SRC_OFFSET)

int doTimerTest(int argc, char *argv[], void *p);
void timer_Configure(uint8  timer_no, uint8 timer_enable, uint8 timer_mode, uint8 timer_halt);

#define timerLdvSet(timer_no,val) *(volatile uint32 *)(CR_TIMER0_LDV+timer_no*0x08) = (val)
#define timerCtlSet(timer_no, timer_enable, timer_mode,timer_halt)	timer_Configure(timer_no, timer_enable, timer_mode, timer_halt)

uint8 timer_sel = 0;
uint32 globe_cnt = 0;
#endif
#if defined(TCSUPPORT_BUS_TEST)
#if defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
int doTimersTest(int argc, char *argv[], void *p);
extern void arht_timers_test(void);
extern void arht_wdogs_test(int wdog_no, int thsld_sec);
#endif
#endif

#if defined(NPU_BMGR_TEST)
#if defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
static int doGetNpuValue(int argc, char *argv[], void *p);
static int doNpuHwBufferTest(int argc, char *argv[], void *p);
#endif
#endif

#if defined(TCSUPPORT_AUTOBENCH_AFE)||defined(TCSUPPORT_AUTOBENCH_MT7510)
extern void tc3162wdog_kick(void);
static int waitDmtPowerOn(int argc, char *argv[], void *p);
static int checkReboot(int argc, char *argv[], void *p);
#endif
#if defined(TCSUPPORT_LED_BTN_CHECK) || defined(TCSUPPORT_TEST_LED_ALL) 
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BTN_CHECK)
#ifndef TCSUPPORT_OPENWRT
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
extern int gButtonCheck; 
#endif
#endif
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_LED_CHECK) || defined(TCSUPPORT_TEST_LED_ALL)
#ifndef TCSUPPORT_OPENWRT
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
extern int doWLanLedOn(void);
extern int doWLanLedOff(void);
extern int doWLanLedRecover(void);
extern int doLedOn(void);
extern int doLedOff(void);
#endif
extern int doLedRecover(void);
static int doWLanLedCheck(int argc, char *argv[], void *p);
static int doLedCheck(int argc, char *argv[], void *p);
#endif
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BTN_CHECK)
static int doButtonCheck(int argc, char *argv[], void *p);
#endif/*TCSUPPORT_COMPILE*/
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_NEW_GUI) || defined(TCSUPPORT_PON_TEST)
int doRomReset(int argc, char *argv[], void *p);
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
static int doWifiLedCheck(int argc, char *argv[], void *p);
extern int doWifiLedOff(void);
extern int doWifiLedRecover(void);
#endif/*TCSUPPORT_COMPILE*/

#if defined(TCSUPPORT_USBHOST)
int doUSBEYE(int argc, char *argv[], void *p);
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C2_TRUE)
int doRomCheck(int argc, char *argv[], void *p);
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_OBM) || defined(TCSUPPORT_PON_TEST)
int doSysAtsh(int argc, char *argv[], void *p);
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(USB_AUTOMOUNT) && defined(TCSUPPORT_CZ_GENERAL)
int doUsbCheck(int argc, char *argv[], void *p);
#endif/*TCSUPPORT_COMPILE*/
#ifndef TCSUPPORT_CPU_ARMV8
int doSysWatch(int argc, char *argv[], void *p);
#endif
#if defined(TCSUPPORT_NEW_SPIFLASH_DEBUG)
int do_sf_basic_test(int argc, char *argv[]);
int do_sf_task2_test(int argc, char *argv[]);
int do_sf_handler_test(int argc, char *argv[]);
#endif
#if defined(TCSUPPORT_NEW_SPIFLASH)
int do_sf_EPCheck(int argc, char *argv[]);
#endif

#if defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
static int gdmpdebug = 0;
#define GDMPDEBUGP(f, a...)	if (gdmpdebug) printk(f, ## a )

int doSysGdmpDbg(int argc, char *argv[], void *p);
int doGDmpCfg(int argc, char *argv[], void *p);
int doSysGdmpMem(int argc, char *argv[], void *p);
#endif
#ifdef TCSUPPORT_PON_TEST
int doLaserTest(int argc, char *argv[], void *p);
int doPonTempTest(int argc, char *argv[], void *p);
int doSysAtbp(int argc, char *argv[], void *p);
int doSysPonLinkStatus(int argc, char *argv[], void *p);
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_OI_C9) || defined(TCSUPPORT_OI_C7) || defined(TCSUPPORT_CDS)
int doSysAtsn(int argc, char *argv[], void *p);
#endif/*TCSUPPORT_COMPILE*/

extern void printVersionStr(void);
int doImageInfo(int argc,char *argv[],void *p);

static int doVoipTest(int argc, char *argv[], void *p);
static int doBindShowTask(int argc, char *argv[], void *p);

#ifdef TCSUPPORT_CDS
int doDead(int argc,char *argv[],void *p);
#endif

int doSysBobCarlibration(int argc, char *argv[], void *p);

#if defined(TCSUPPORT_CT_UPG_PINGPONG) || defined(TCSUPPORT_NOR_FLASH_USED) || defined(CONFIG_DUAL_IMAGE)
int doSysBootFlag(int argc, char *argv[], void *p);
#endif

int doSysSifm(int argc, char *argv[], void *p);
int doSysSerdesSel(int argc, char *argv[], void *p);
#ifdef TCSUPPORT_BOARD_SELECT
int doSysBoardInfo(int argc, char *argv[], void *p);
#endif
static int doSysHybrid(int argc, char *argv[], void *p);
#ifdef L2_AUTOPVC
static cmds_t sysCmds[] = {
#else
static const cmds_t sysCmds[] = {
#endif
	{"memrl",		doSysMemrl,		0x02,  	1,  NULL},
	{"memwl",		doSysMemwl,		0x02,  	2,  NULL},
	{"modifybit",		doSysModifyBit,		0x02,  	3,  NULL},
	{"memck",		doSysCheckField,		0x02,  	4,  "sys memck <address> <value> <MSB> <LSB> <dummy message>"},
	{"memwf",		doSysMemwlField,		0x02,  	4,  "sys memwf <address> <value> <MSB> <LSB> <dummy message>"},
	{"memrf",		doSysMemrlField,		0x02,  	3,  "sys memrf <address> <MSB> <LSB> <dummy message>"},		
	{"memwl2",		doSysMemwl2,		0x02,  	3,  "sys memwl2 <0|1 (and|or)> <reg> <hex val>"},
	{"memrw",		doSysMemrw,		0x02,  	1,  NULL},
	{"memww",		doSysMemww,		0x02,  	2,  NULL},
	{"memory",		doSysMemory,	0x02,  	2,  NULL},
	{"memcpy",		doSysMemcpy,	0x02,  	3,  NULL},
	{"fillmem",		doSysFillMem,	0x02,  	3,  NULL},
	{"mac",	    	doSysMac,	    0x02,  	1,  SYS_MAC_HELP_INFO},
	{"onutype",	    doSysOnuType,	0x02,  	1,  NULL},
	{"bbf247",	    doSysBBF247,	0x02,  	1,  NULL},
	{"combopon",	doSysComboPon,	0x02,  	1,  NULL},
	{"onumode",	    doSysPONMode,	0x02,  	1,  NULL},
	#if defined(NPU_BMGR_TEST)
	#if defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
	{"npuget",      doGetNpuValue,   0x02,   2,  NULL},
	{"npuhwbuffertest",doNpuHwBufferTest,   0x02,   2,  NULL},
	#endif
	#endif
	#if (defined(TCSUPPORT_CPU_ARMV8) && defined(TCSUPPORT_BUS_TEST))
    {"cacheApiTest",doCacheApiTest,	0x02,  	1,  NULL},
    {"l2cSramTest",doL2cSramTest,	0x02,  	0,  NULL},
    {"cpuWt_dmaNpuRd",doCpuWt_dmaNpuRd,	0x02,  	4,  NULL},
	{"highMemTest", dohighMemTest,	0x02,	0, NULL},
	#ifdef MBOX_API_TEST
	{"mboxAPItest", doMboxAPItest,	0x02,	0, NULL},
	#endif
	#if defined(TCSUPPORT_CPU_AN7583)
	{"r2cRdbypass", doR2cRdbypassWt,   0x02,  1,  "sys r2cRdbypass testCnt"},
	#endif
	#if defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
	{"timerTest",      doTimersTest,   0x02,   3,  "sys timerTest <0:tmr,1:wdog,2:wdog_thsld> <wdog_no:0~3> <thsld_sec>"},
	#endif
    #endif
	{"qdmainit",	doSysQdmaInit,	0x02,  	2,  SYS_QDMAINIT_HELP_INFO},
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_WAN_UPSTREAM_REMARK)
	{"remark",	    doSysRemark,	0x02,  	1,  NULL},
#endif/*TCSUPPORT_COMPILE*/
	{"exmdio",	    doSysExMdio,	0x02,  	0,  "sys exmdio <config|write|read>"},
#ifdef L2_AUTOPVC
	{"autopvc",		NULL,			0x12,	1,	NULL},
#endif
#ifdef WAN2LAN
	{"wan2lan",		doWan2lan,		0x02,	0,	NULL},
    {"wlan2lan",    doWlan2lan,     0x02,   0,  NULL},
#if defined(TCSUPPORT_WAN2LAN_EXT)
	{"lan2lan",		doLan2lan,		0x02,	0,	NULL},
#endif
#endif
#if defined(TCSUPPORT_AUTOBENCH_AFE) || defined(TCSUPPORT_AUTOBENCH_MT7510)
    {"waitdmt",     waitDmtPowerOn,      0x02,   0,  NULL},
    {"checkreboot",     checkReboot,      0x02,   0,  NULL},
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_NEW_GUI) || defined(TCSUPPORT_PON_TEST)
	{"romreset",		doRomReset,		0x02,	1,	NULL},
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BOOTLOADER_MODIFY_PVNAME)
	{"atsp",	doSysAtsp,			0x02,	0,	NULL},
	{"atsv",	doSysAtsv,			0x02,	0,	NULL},
	{"atpvsave",	doSysAtspv,		0x02,	0,	NULL},	
#endif/*TCSUPPORT_COMPILE*/
#if defined(TCSUPPORT_LED_BTN_CHECK) || defined(TCSUPPORT_TEST_LED_ALL)
#if defined(TCSUPPORT_LED_CHECK) || defined(TCSUPPORT_TEST_LED_ALL)
#ifndef TCSUPPORT_OPENWRT
	{"wlanled",     doWLanLedCheck, 	0x02,	0,	NULL},
	{"led",		doLedCheck,		0x02,	0,	NULL},
#endif
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BTN_CHECK)
	{"button",	doButtonCheck,	0x02,	0,	NULL},
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
{"wifiled", 	doWifiLedCheck, 	0x02,	0,	NULL},
#endif/*TCSUPPORT_COMPILE*/
#endif
#if defined(TCSUPPORT_USBHOST)
	{"usbeye",  doUSBEYE,  0x02,   0,  NULL},
#endif	
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C2_TRUE)
	{"romcheck",	    	doRomCheck,	0x02,	0,	NULL},
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_OBM) || defined(TCSUPPORT_PON_TEST)
	{"atsh",		doSysAtsh,			0x02,	0,	NULL},
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(USB_AUTOMOUNT) && defined(TCSUPPORT_CZ_GENERAL)
	{"usbCheck",		doUsbCheck,			0x02,	0,	NULL},
#endif/*TCSUPPORT_COMPILE*/
#ifndef TCSUPPORT_CPU_ARMV8
	{"watch",		doSysWatch,			0x02,	0,	NULL},
#endif
#if defined(TCSUPPORT_NEW_SPIFLASH_DEBUG)
	{"sftest",	 do_sf_basic_test,		0x02,	0,	NULL},
	{"sftest2",	 do_sf_task2_test,		0x02,	0,	NULL},
	{"sfhandlertest",	 do_sf_handler_test,		0x02,	0,	NULL},
#endif
#if defined(TCSUPPORT_NEW_SPIFLASH)
	{"sfepcheck",	 do_sf_EPCheck,		0x02,	0,	NULL},
#endif
#if defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
	{"gdmpDebug",		doSysGdmpDbg,	0x02,  	1,  NULL},
	{"gdmpReg",		doGDmpCfg,			0x02,	0,	NULL},
	{"gdmpMemory",		doSysGdmpMem,	0x02,  	2,  NULL},	
#endif	

#ifdef TCSUPPORT_PON_TEST
	{"lasertest",        doLaserTest, 0x02, 1, "sys lasertest <on|off>"},
	{"pontest",        doPonTempTest, 0x02, 0, NULL},
	{"ponlink",        doSysPonLinkStatus, 0x02, 0, NULL},
	{"atbp",        doSysAtbp, 0x02, 0, NULL},
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_OI_C9) || defined(TCSUPPORT_OI_C7) || defined(TCSUPPORT_CDS)
	{"attsn",		doSysAtsn,			0x01,	0,	NULL},
#endif/*TCSUPPORT_COMPILE*/
	{"ver",		printVersionStr,			0x02,	0,	NULL},
	{"imginfo",		doImageInfo,			0x02,	0,	NULL},
	{"voiptest",	doVoipTest,			0x02,	0,	NULL},
	{"bindshowtask",	doBindShowTask,			0x02,	0,	NULL},
#ifdef TCSUPPORT_CDS
	{"dead",		doDead,			0x02,	0,	NULL},
#endif
	{"bobcarlibration", 	doSysBobCarlibration,			0x01,	0,	NULL},
#if defined(TCSUPPORT_CT_UPG_PINGPONG) || defined(TCSUPPORT_NOR_FLASH_USED) || defined(CONFIG_DUAL_IMAGE)
	{"bootflag", 	doSysBootFlag,			0x01,	0,	NULL},
#endif
#if defined(TCSUPPORT_GDMA_TEST)
    {"gdmatest",      doGdmaTest,   0x02,   1,  "sys gdmatest <cnt>"}, 
    {"fesramtest",    dofesramtest, 0x02,   1,  NULL},
#endif
#if defined(TCSUPPORT_BUS_TEST)
#ifdef TCSUPPORT_KPROFILE
    {"cpuProfiling", doCpuProfiling,   0x02,   1,  "sys cpuProfiling <type> //0:base, 1:cache read, 2:un-cache read"},
#endif
    {"cpuCycles",       doCpuCycles,   0x02,   1,  "sys cpuCycles [base||l2cache||dramRead||regRead] "},
    {"rdBypassWt",  doRdBypassWtTest,  0x02,  0,  "sys rdBypassWt"},
    {"syncCmdTest", doSyncCmdTest,   0x02,  2,  "sys syncCmdTest [sync0||sync6||sync7||all] testCnt"},
    {"sramTest",    doSramTest,     0x02,  0,  "sys sramTest"},
    {"busData",       doBusData,   0x02,   1,  "sys busData [data0||data1||data2||data3] hex-value"},
    {"busUtilCntTest",    doBusUtilCntTest, 0x02,  1,  "sys busUtilCntTest <testTime>"},
#endif
#if defined(TCSUPPORT_TIMER_TEST)    
    {"timer",      doTimerTest,   0x02,   1,  "sys timer <timer 0~3>"}, 
#endif         
	{"sifm", 		doSysSifm,			0x01,	0,	NULL},
	{"serdes", 	doSysSerdesSel,		0x01,	0,	NULL},
#ifdef TCSUPPORT_BOARD_SELECT
	{"boardinfo",	doSysBoardInfo, 	0x01,	0,	NULL},
#endif
	{"hybrid",	doSysHybrid, 	0x02,	1,	SYS_HYBRID_HELP_INFO},
	{NULL,			NULL,			0x10,	0,	NULL},
};

static const cmds_t subExMdioCmds[] = {
	{"config",		doExMdioConf,		0x02,  	2,  "sys exmdio config <mdc gpio> <mdio gpio>"},
	{"write",		doExMdioWrite,		0x02,  	2,  "sys exmdio write <reg> <value>"},
	{"read",		doExMdioRead,		0x02,  	1,  "sys exmdio read <reg>"},
	{NULL,			NULL,			0x10,	0,	NULL},
};


//#define BOOTLOADER_SIZE 64*1024
extern void setUserSpaceFlag(int num);

int doSys(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_CMDPROMPT)
	if(argc==1){
		setUserSpaceFlag(12);
		return -1;
	}else{
		return subcmd(sysCmds, argc, argv, p);
	}
#else
#if defined(TCSUPPORT_BUS_TEST)
	if (bustest_kthread_process(argc, argv)==0)
		return 0;
#endif
	return subcmd(sysCmds, argc, argv, p);
#endif
	
}

#if defined(TCSUPPORT_NEW_SPIFLASH_DEBUG)
int do_sf_basic_test(int argc, char *argv[])
{
	sf_basic_test_entry(argc, argv);
	return 0;
}
int do_sf_task2_test(int argc, char *argv[])
{
	sf_task2_test_entry(argc, argv);
	return 0;
}
int do_sf_handler_test(int argc, char *argv[])
{
	sf_handler_test_entry(argc, argv);
	return 0;
}
#endif
#if defined(TCSUPPORT_NEW_SPIFLASH)
int do_sf_EPCheck(int argc, char *argv[])
{
    #ifndef TCSUPPORT_CPU_ARMV8
	sf_EPCheck_entry(argc, argv);
    #endif
	return 0;
}
#endif

/* check memory address is valid or not */
static inline int address_valid_check(unsigned long addr, int byteNum)
{
	int addrMask=0;
	addrMask = (byteNum==4)?(0x3):((byteNum==2)?(0x1):(0));

    #ifdef TCSUPPORT_CPU_ARMV8

    if((addr&addrMask)!=0) {
		printk("Error: 0x%lx is not %d aligned\n", addr, byteNum);
		return -1;
	}
    
    if (!( (((448<<20)<=addr)&&(addr<(1024<<20))) || /* 0x20000000~0x40000000 for pcie dev */
           ((DT_MEMORY_BASE<=addr)&&(addr<UL(DT_MEMORY_BASE+DT_MEMORY_SIZE))) ) ) {
        printk("Error: 0x%lx is out of range\n", addr);
        return -1;
    }
    
    #else
    
	if(((addr>>28)<8) || ((addr&addrMask)!=0)) {
		printk("address error\n");
		return -1;
	}
    #endif
    
	return 0;
}

#ifdef TCSUPPORT_CPU_ARMV8
void *physAddr_to_virtAddr(unsigned int *physAddr)
{
    void *virtAddr;

    if (((unsigned int)physAddr)>=DT_MEMORY_BASE)
        virtAddr = phys_to_virt((phys_addr_t)physAddr);
    else 
        virtAddr = ioremap((phys_addr_t)physAddr, 4);

    return virtAddr;
}

unsigned int read_mem_reg(unsigned int *physAddr, unsigned int len)
{
    void *virtAddr;
    unsigned int val=0;

    if (((unsigned int)physAddr)>=DT_MEMORY_BASE) {
        virtAddr = phys_to_virt((phys_addr_t)physAddr);
        ecnt_dcache_inv((unsigned long)virtAddr, 4);
        if (len==4)
            val = readl(virtAddr);
        else if (len==2)
            val = readw(virtAddr);
        else if (len==1)
            val = readb(virtAddr);
        else
            printk("(%s)len:%d is incorrect\n", __func__, len);
    }
    else {
        virtAddr = ioremap((phys_addr_t)physAddr, 4);
        if (len==4)
            val = readl(virtAddr);
        else if (len==2)
            val = readw(virtAddr);
        else if (len==1)
            val = readb(virtAddr);
        else
            printk("(%s)len:%d is incorrect\n", __func__, len);
        
        iounmap(virtAddr);
    }

    return val;
}

void print_mem_reg(unsigned int *physAddr, unsigned int len)
{
    void *virtAddr;

    if (((unsigned int)physAddr)>=DT_MEMORY_BASE) {
        virtAddr = phys_to_virt((phys_addr_t)physAddr);
        ecnt_dcache_inv((unsigned long)virtAddr, len);
    }
    else {
        virtAddr = ioremap((phys_addr_t)physAddr, len);
    }

    if (len>4)
        dump((unsigned long)virtAddr, len, physAddr);
    else if (len==4)
        printk("0x%lx\t0x%08lx\r\n", (unsigned int)physAddr, readl(virtAddr));
    else if (len==2)
        printk("0x%lx\t0x%04lx\r\n", (unsigned int)physAddr, readw(virtAddr));
    else if (len==1)
        printk("0x%lx\t0x%02lx\r\n", (unsigned int)physAddr, readb(virtAddr));
    else
        printk("(%s)len:%d is incorrect\n", __func__, len);

    if (((unsigned int)physAddr)<DT_MEMORY_BASE)
        iounmap(virtAddr);
    return;
}

void write_mem_reg(unsigned int *physAddr, unsigned int value, unsigned int len)
{
    void *virtAddr;

    if (((unsigned int)physAddr)>=DT_MEMORY_BASE) {
        virtAddr = phys_to_virt((phys_addr_t)physAddr);
        if (len==4)
            writel(value, virtAddr);
        else if (len==2)
            writew(value, virtAddr);
        else if (len==2)
            writeb(value, virtAddr);
        else
            printk("(%s)len:%d is incorrect\n", __func__, len);
        
        ecnt_dcache_wback_inv((unsigned long)virtAddr, len);
    }
    else {
        virtAddr = ioremap((phys_addr_t)physAddr, len);
        if (len==4)
            writel(value, virtAddr);
        else if (len==2)
            writew(value, virtAddr);
        else if (len==2)
            writeb(value, virtAddr);
        else
            printk("(%s)len:%d is incorrect\n", __func__, len);
        
        iounmap(virtAddr);
    }
}
#endif

int doSysMemrl(int argc, char *argv[], void *p)
{
	unsigned long *ptr=NULL;

	ptr = (unsigned long *)simple_strtoul(argv[1], NULL, 16);
	if(address_valid_check((unsigned long)ptr, 4) != 0) {
		return -1;
	}
	printk("\r\n<Address>\t<Value>\r\n");

    #ifdef TCSUPPORT_CPU_ARMV8
    print_mem_reg(ptr, 4);    
    #else
	printk("0x%08lx\t0x%08lx\r\n", (unsigned long)ptr, (unsigned long)*ptr);
    #endif

	return 0;
}

int doSysMemwl(int argc, char *argv[], void *p)
{
	unsigned long *ptr=NULL;
	unsigned long value=0;

	ptr = (unsigned long *)simple_strtoul(argv[1], NULL, 16);
	value = (unsigned long)simple_strtoul(argv[2], NULL, 16);
	if(address_valid_check((unsigned long)ptr, 4) != 0) {
		return -1;
	}

    #ifdef TCSUPPORT_CPU_ARMV8
    write_mem_reg(ptr, value, 4);
    #else
	*ptr = value;
    #endif

	return 0;
}
int doSysModifyBit(int argc, char *argv[], void *p)
{
	unsigned long *ptr=NULL;
         unsigned long   value=0;
         unsigned long   pos=0;

	ptr = (unsigned long *) simple_strtoul(argv[1], NULL, 16);
         pos = (unsigned long)  simple_strtoul(argv[2], NULL, 10);
         value = (unsigned long) simple_strtoul(argv[3], NULL, 10);
        
	if(address_valid_check((unsigned long)ptr, 4) != 0) {
		return -1;
	}
    
    if(value == 1) {
        #ifdef TCSUPPORT_CPU_ARMV8
        write_mem_reg(ptr, (read_mem_reg(ptr,4) | (0x1 << pos)), 4);
        #else
        regWrite32(ptr, (regRead32(ptr) | (0x1 << pos)) );
        #endif
    }
    else {
        #ifdef TCSUPPORT_CPU_ARMV8
        write_mem_reg(ptr, (read_mem_reg(ptr,4) & ~(0x1 << pos)), 4);
        #else
        regWrite32(ptr, (regRead32(ptr) & ~(0x1 << pos)) );
        #endif
    }
    
    return 0;
}

int doSysCheckField(int argc, char *argv[], void *p)
{
	unsigned long *ptr=NULL;
         unsigned long   value=0;
	unsigned char	MSB = 0;
	unsigned char	LSB = 0;
	unsigned long	tmp = 0;

	if (argc > 4)
	{

		ptr = (unsigned long *) simple_strtoul(argv[1], NULL, 16);
	         value = (unsigned long) simple_strtoul(argv[2], NULL, 16);
		MSB = (unsigned long) simple_strtoul(argv[3], NULL, 10);
		LSB = (unsigned long) simple_strtoul(argv[4], NULL, 10);
	        
		if(address_valid_check((unsigned long)ptr, 4) != 0) {
			return -1;
		}
	    
		 if((LSB > MSB) ||(MSB>31) ||(LSB>31))
		{			
			printk("\r\n<MSB> <LSB> value invalid!\r\n");
			printk("usage: sys memck <address> <value> <MSB> <LSB> <dummy message>\r\n");
			printk("example: sys memck 1faf4334 0xc 4 0 'rg_bir_ltd1_mode_0=5'b01100' \r\n");
			
		}else 
		{
			#ifdef TCSUPPORT_CPU_ARMV8
		        tmp = read_mem_reg(ptr,4);
		        #else
		        tmp = regRead32(ptr);
		        #endif
			
			if((MSB == 31)&&(LSB == 0))
			{
				if(tmp != value)
					printk("0x%lx[31:00]\t0x%lx, expect 0x%x\r\n\n",(unsigned int)ptr, tmp, value);
			}else
			{					
				tmp = (tmp>>LSB) & (((unsigned long)0x1<<(MSB-LSB+1))-1);
				if(tmp != value)
					printk("0x%lx[%02d:%02d]\t0x%lx, expect 0x%x\r\n\n",(unsigned int)ptr, MSB, LSB, tmp, value);			
			}
		}
	}
	else
	{	
		printk("usage: sys memck <address> <value> <MSB> <LSB> <dummy message>\r\n");
		printk("example: sys memck 1faf4334 0xc 4 0 'rg_bir_ltd1_mode_0=5'b01100' \r\n");
	}
	
	return 0;	
}

int doSysMemwlField(int argc, char *argv[], void *p)
{
	unsigned long *ptr=NULL;
         unsigned long   value=0;
	unsigned char	MSB = 0;
	unsigned char	LSB = 0;
	unsigned long	tmp = 0;

	if (argc > 4)
	{

		ptr = (unsigned long *) simple_strtoul(argv[1], NULL, 16);
	         value = (unsigned long) simple_strtoul(argv[2], NULL, 16);
		MSB = (unsigned long) simple_strtoul(argv[3], NULL, 10);
		LSB = (unsigned long) simple_strtoul(argv[4], NULL, 10);
	        
		if(address_valid_check((unsigned long)ptr, 4) != 0) {
			return -1;
		}
	    

		 if((LSB > MSB) ||(MSB>31) ||(LSB>31))
		{			
			printk("\r\n<MSB> <LSB> value invalid!\r\n");
			printk("usage: sys memwf <address> <value> <MSB> <LSB> <dummy message>\r\n");
			printk("example: sys memwf 1faf4334 0xc 4 0 'rg_bir_ltd1_mode_0=5'b01100' \r\n");
			
		}else 
		{
			if((MSB == 31)&&(LSB == 0))
			{
				tmp = value;
			}else
			{	
				#ifdef TCSUPPORT_CPU_ARMV8
			        tmp = read_mem_reg(ptr,4);
			        #else
			        tmp = regRead32(ptr);
			        #endif
				
				tmp = (value<<LSB) | (tmp & ~((((unsigned long)0x1<<(MSB-LSB+1))-1)<<LSB));
			}

			#ifdef TCSUPPORT_CPU_ARMV8
		        write_mem_reg(ptr, tmp, 4);
		        #else
		        regWrite32(ptr, tmp);
		        #endif
		}
	}
	else
	{	
		printk("usage: sys memwf <address> <value> <MSB> <LSB> <dummy message>\r\n");
		printk("example: sys memwf 1faf4334 0xc 4 0 'rg_bir_ltd1_mode_0=5'b01100' \r\n");
	}

	
	return 0;	
}


int doSysMemrlField(int argc, char *argv[], void *p)
{

	unsigned long *ptr=NULL;
	unsigned char	MSB = 0;
	unsigned char	LSB = 0;


	if (argc > 3)
	{

		ptr = (unsigned long *)simple_strtoul(argv[1], NULL, 16);
		MSB = (unsigned long) simple_strtoul(argv[2], NULL, 10);
		LSB = (unsigned long) simple_strtoul(argv[3], NULL, 10);

		 if((LSB > MSB) ||(MSB>31) ||(LSB>31))
		{

			printk("\r\n<MSB> <LSB> value invalid!\r\n");
			printk("usage: sys memrf <address> <MSB> <LSB> <dummy message>\r\n");
			printk("example: sys memrf 1fb00834 31 28 'this field = rst_ctrl_sw[31:28]' \r\n");
			
		}else 
		{

			if(address_valid_check((unsigned long)ptr, 4) != 0) {
				return -1;
			}
			printk("<Address>\t\t<Value>\r\n");

			#ifdef TCSUPPORT_CPU_ARMV8
			printk("0x%lx[%02d:%02d]\t0x%lx\r\n\n", (unsigned int)ptr, MSB, LSB, ((read_mem_reg(ptr,4) >> LSB) & (((unsigned long)0x1<<(MSB-LSB+1))-1)));
		        #else
			printk("0x%08lx[%02d:%02d]\t0x%lx\r\n\n", (unsigned long)ptr, MSB, LSB,  ((((unsigned long)*ptr) >> LSB) & (((unsigned long)0x1<<(MSB-LSB+1))-1)));
		        #endif
		}
	}
	else
	{	
		
		printk("usage: sys memrf <address> <MSB> <LSB> <dummy message>\r\n");
		printk("example: sys memrf 1fb00834 31 28 'this field = rst_ctrl_sw[31:28]' \r\n");


	}


	return 0;

}
int doSysMemwl2(int argc, char *argv[], void *p)
{
	unsigned long orFlag;
	unsigned long ptr;
	unsigned long value;
	orFlag = (unsigned long *)simple_strtoul(argv[1], NULL, 16);
	ptr = (unsigned long)simple_strtoul(argv[2], NULL, 16);
	value = (unsigned long)simple_strtoul(argv[3], NULL, 16);
	if(address_valid_check((unsigned long)ptr, 4) != 0) {
		return -1;
	}
	if(orFlag){
        #ifdef TCSUPPORT_CPU_ARMV8
        write_mem_reg(ptr, (read_mem_reg(ptr,4)|value), 4);
        #else
		regWrite32(ptr,regRead32(ptr)|value);
        #endif
	}
	else{
        #ifdef TCSUPPORT_CPU_ARMV8
        write_mem_reg(ptr, (read_mem_reg(ptr,4)&value), 4);
        #else
		regWrite32(ptr,regRead32(ptr)&value);
        #endif
	}
	//printk("addr:0x%x val: 0x%x (%s 0x%x)\n",ptr,regRead32(ptr),orFlag?"or":"and",value);
	return 0;
}

int doSysMemrw(int argc, char *argv[], void *p)
{
	unsigned short *ptr;

	ptr = (unsigned short *)simple_strtoul(argv[1], NULL, 16);
	if(address_valid_check((unsigned long)ptr, 2) != 0) {
		return -1;
	}
	printk("\r\n<Address>\t<Value>\r\n");

    #ifdef TCSUPPORT_CPU_ARMV8
    print_mem_reg(ptr, 2);
    #else
	printk("0x%08lx\t0x%04x\r\n", (unsigned long) ptr, (unsigned int) (*ptr));
    #endif

	return 0;
}

int doSysMemww(int argc, char *argv[], void *p)
{
	unsigned short *ptr;
	unsigned short value;

	ptr = (unsigned short *)simple_strtoul(argv[1], NULL, 16);
	value = (unsigned short)simple_strtoul(argv[2], NULL, 16);
	if(address_valid_check((unsigned long)ptr, 2) != 0) {
		return -1;
	}

    #ifdef TCSUPPORT_CPU_ARMV8
    write_mem_reg(ptr, value, 2);
    #else
	*ptr = value;
    #endif

	return 0;
}

void * memcpy_endian(void * dest,const void *src,size_t count)
{
    char *tmp = (char *) dest, *s = (char *) src;
#ifdef __BIG_ENDIAN
    unsigned int offset=0;
#else
    unsigned int offset=3;
#endif

    while (count--) {
        *tmp++ = *(char *)(((unsigned long)s) ^ offset);
        s++;
    }

    return dest;
}

static int dump(unsigned long addr, unsigned long len, unsigned long phyAddr)
{
	register int n, m, c, r;
	unsigned char temp[16];

    #ifdef TCSUPPORT_CPU_ARMV8
    if (phyAddr==NULL) {
        printk("phyAddr==NULL\n");
        return -1;
    }
    #endif

	for( n = len; n > 0; ){
            #ifdef TCSUPPORT_CPU_ARMV8
            printk("%lx ", phyAddr);
            #else
			printk("%.8lx ", addr);
            #endif
			r = n < 16? n: 16;
			memcpy_endian((void *) temp, (void *) addr, r);
			addr += r;
            #ifdef TCSUPPORT_CPU_ARMV8
            phyAddr += r;
            #endif
			for( m = 0; m < r; ++m ){
					printk("%c", (m & 3) == 0 && m > 0? '.': ' ');
					printk("%.2x", temp[m]);
			}
			for(; m < 16; ++m )
					printk("   ");
			printk("  |");
			for( m = 0; m < r; ++m ){
				c = temp[m];
				printk("%c", ' ' <= c && c <= '~'? c: '.');
			}
			n -= r;
			for(; m < 16; ++m )
					printk(" ");
			printk("|\n");
	}

	printk("#\n");
	return 0;
}

int doSysMemory(int argc, char *argv[], void *p)
{
	unsigned long addr, len;
    #ifdef TCSUPPORT_CPU_ARMV8
    void *virtAddr;
    #endif

	addr = simple_strtoul(argv[1], NULL, 16);
	len = simple_strtoul(argv[2], NULL, 16);

    #ifdef TCSUPPORT_CPU_ARMV8
    if(address_valid_check((unsigned long)addr, 4) != 0) {
        return -1;
    }

    print_mem_reg(addr, len);
    #else
	dump(addr, len,NULL);
    #endif

	return 0;
}

int doSysMemcpy(int argc, char *argv[], void *p)
{
	unsigned long src, dst, len;
    #ifdef TCSUPPORT_CPU_ARMV8
    void *virtSrcAddr, *virtDstAddr;
    #endif

	src = simple_strtoul(argv[1], NULL, 16);
	dst = simple_strtoul(argv[2], NULL, 16);
	len = simple_strtoul(argv[3], NULL, 16);
    
    #ifdef TCSUPPORT_CPU_ARMV8
    if((address_valid_check((unsigned long)src, 4) != 0) ||
       (address_valid_check((unsigned long)dst, 4) != 0)) {
        return -1;
    }

    virtSrcAddr = physAddr_to_virtAddr(src);
    virtDstAddr = physAddr_to_virtAddr(dst);
    
    memcpy((void *)virtDstAddr, (void *)virtSrcAddr, len);

    if (((unsigned long)dst)>=DT_MEMORY_BASE)
        ecnt_dcache_wback_inv((unsigned long)virtDstAddr, len);
    else
        iounmap(virtDstAddr);
    
    if (((unsigned long)src)<DT_MEMORY_BASE)
        iounmap(virtSrcAddr);
        

    #else
	memcpy((void *)dst, (void *)src, len);
    #endif
    
	return 0;
}

int doSysFillMem(int argc, char *argv[], void *p)
{
	unsigned long addr, len, pattern;
    #ifdef TCSUPPORT_CPU_ARMV8
    void *virtAddr;
    #endif

	addr = simple_strtoul(argv[1], NULL, 16);
	len = simple_strtoul(argv[2], NULL, 16);
	pattern = simple_strtoul(argv[3], NULL, 16);

    #ifdef TCSUPPORT_CPU_ARMV8
    if(address_valid_check((unsigned long)addr, 4) != 0) {
        return -1;
    }

    virtAddr = physAddr_to_virtAddr(addr);

    if (((unsigned long)addr)>=DT_MEMORY_BASE) {
        memset((void *)virtAddr, pattern, len);
        ecnt_dcache_wback_inv((unsigned long)virtAddr, len);
    }
    else {
        memset((void *)virtAddr, pattern, len);
        iounmap(virtAddr);
    }

    #else
	memset((void *)addr, pattern, len);
    #endif
    	
	return 0;
}


int doSysMac(int argc, char *argv[], void *p)
{
    setUserSpaceFlag(1);
	return 0;
}
int doSysOnuType(int argc, char *argv[], void *p)
{
    setUserSpaceFlag(16);
	return 0;
}

int doSysBBF247(int argc, char *argv[], void *p)
{
    setUserSpaceFlag(34);
	return 0;
}

int doSysComboPon(int argc, char *argv[], void *p)
{
    setUserSpaceFlag(35);
	return 0;
}

int doSysPONMode(int argc, char *argv[], void *p)
{
    if(isEN7580 || isEN7581 || isAN7583)
        setUserSpaceFlag(28);
    else
        printk("Do not support to set the pon mode\n");
    return 0;
}

int doSysQdmaInit(int argc, char *argv[], void *p)
{
    if(SUPPORT_QDMAINIT)
        setUserSpaceFlag(29);
    else
        printk("Do not support to set qdma init parameter\n");
    return 0;
}

#if defined(TCSUPPORT_CPU_MT7505) || defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
int doSysGdmpDbg(int argc, char *argv[], void *p)
{
	if (argc == 2)
	{
		if ((strcmp(argv[1], "on")) == 0)
		{
			gdmpdebug = 1;
			printk("Now the gdmpDebug is on\r\n");
		}	
		else if	((strcmp(argv[1], "off")) == 0)
		{	
			printk("Now the gdmpDebug is off\r\n");
			gdmpdebug = 0;
		}	
	}
	else
	{	
		printk("usage: sys gdmpDebug <on | off>\r\n");
		printk("Now the gdmpDebug is %s\r\n",gdmpdebug ? "on":"off");
	}
	
	return 0;
}

int doSysGdmpMem(int argc, char *argv[], void *p)
{
	unsigned long length;
	unsigned char buf[18];
	int i,j = 0;
	struct file *fp;

	unsigned long *ptr;
	mm_segment_t fs;
	
#ifdef TCSUPPORT_CPU_ARMV8
	/* GDUMP SRAM phy addr*/
	unsigned long base = 0x1FA40000;
	unsigned int gdumpdata = 0;
	unsigned int gdumpdata2 = 0;
	int ret = 0;
#endif
	ptr = (unsigned long *)simple_strtoul(argv[1], NULL, 16);
	length = (unsigned long)simple_strtoul(argv[2], NULL, 16);
	if ((length % 8) != 0)
	{
		printk("the length must be n times 8!!!\n");
		return 0;
	}

#ifdef TCSUPPORT_CPU_ARMV8
	base = (unsigned long)ptr - base;

#endif
	memset(buf,0,sizeof(buf));
	fp=filp_open("/tmp/gdmp.raw",O_WRONLY | O_CREAT |O_TRUNC,0);
	if(IS_ERR(fp)) {
	    printk("file open error!\n");
	    return PTR_ERR(fp);
	} 
	
	fs=get_fs();
	set_fs(KERNEL_DS);
	for( i = 0; i < length/4; i = i + 2)
	{
#ifdef TCSUPPORT_CPU_ARMV8
		ret = get_gdmpSram_data((unsigned long)(base + ((i+1)<<2)), &gdumpdata);
		if (ret != 0)
		{
			printk("get gdump sram data fail !\n");
			goto end;			
		}
		ret = get_gdmpSram_data((unsigned long)(base + (i<<2)), &gdumpdata2);
		if (ret != 0)
		{
			printk("get gdump sram data fail !\n");
			goto end;			
		}

		GDMPDEBUGP("%.8lx", gdumpdata);
		GDMPDEBUGP("%.8lx", gdumpdata2);
		GDMPDEBUGP( "\r\n");

		sprintf(buf,"%.8lx%.8lx\r\n",gdumpdata,gdumpdata2);

#else
		GDMPDEBUGP("%.8x", (unsigned long) (*(ptr + i + 1)));
		GDMPDEBUGP("%.8x", (unsigned long) (*(ptr + i)));
		GDMPDEBUGP( "\r\n");

		sprintf(buf,"%.8x%.8x\r\n",(unsigned long) (*(ptr + i + 1)),(unsigned long) (*(ptr + i)));
#endif
		ecnt_kernel_fs_write(fp, buf, sizeof(buf),&fp->f_pos);
		memset(buf,0,sizeof(buf));
	}

	printk("GDMP Memory dump Finished\n");
end:
	filp_close(fp,NULL);	
	set_fs(fs);

	return 0;
}

int doGDmpCfg(int argc, char *argv[], void *p)
{
#define GDMP_REG_BASE	0xBFBF0000
#define GDMP_GLO_CFG	(GDMP_REG_BASE + 0x00009000)
#define GDMP_CAP_RLT	(GDMP_REG_BASE + 0x00009004)
#define GDMP_TRG_RLT	(GDMP_REG_BASE + 0x00009008)
#define GDMP_INT_STS	(GDMP_REG_BASE + 0x00009010)
#define GDMP_INT_MSK	(GDMP_REG_BASE + 0x00009014)
#define GDMP_PRB_SEL	(GDMP_REG_BASE + 0x00009020)
#define	GDMP_TRG_PATN0_L		(GDMP_REG_BASE + 0x00009030)
#define	GDMP_TRG_PATN0_MSK_L	(GDMP_REG_BASE + 0x00009034)
#define	GDMP_TRG_PATN0_H		(GDMP_REG_BASE + 0x00009038)
#define	GDMP_TRG_PATN0_MSK_H	(GDMP_REG_BASE + 0x0000903C)
#define	GDMP_TRG_PATN1_L		(GDMP_REG_BASE + 0x00009040)
#define	GDMP_TRG_PATN1_MSK_L	(GDMP_REG_BASE + 0x00009044)
#define	GDMP_TRG_PATN1_H		(GDMP_REG_BASE + 0x00009048)
#define	GDMP_TRG_PATN1_MSK_H	(GDMP_REG_BASE + 0x0000904C)
#define	GDMP_TRG_MODE			(GDMP_REG_BASE + 0x00009050)
#define	GDMP_CAP_CFG			(GDMP_REG_BASE + 0x00009060)
#define	GDMP_CLK_CFG			(GDMP_REG_BASE + 0x00009070)

	unsigned char buf[1024];
	struct file *fp;
	mm_segment_t fs;
	int len = 0;

	fp=filp_open("/tmp/gdmp.csr",O_WRONLY | O_CREAT |O_TRUNC,0);
	if(IS_ERR(fp)) {
	    printk("file open error!\n");
	    return PTR_ERR(fp);
	} 
	fs=get_fs();
	set_fs(KERNEL_DS);
	memset(buf,0,sizeof(buf));
#ifdef TCSUPPORT_CPU_ARMV8
		len = gdump_cfg_dump(buf,gdmpdebug);
#else
	GDMPDEBUGP("GDMP_GLO_CFG\t%08lx\r\n",regRead32(GDMP_GLO_CFG));	
	len += sprintf(buf + len,"GDMP_GLO_CFG\t%08lx\r\n",regRead32(GDMP_GLO_CFG));
	
	GDMPDEBUGP("GDMP_CAP_RLT\t%08lx\r\n",regRead32(GDMP_CAP_RLT));
	len += sprintf(buf + len,"GDMP_CAP_RLT\t%08lx\r\n",regRead32(GDMP_CAP_RLT));
	
	GDMPDEBUGP("GDMP_TRG_RLT\t%08lx\r\n",regRead32(GDMP_TRG_RLT));	
	len += sprintf(buf + len,"GDMP_TRG_RLT\t%08lx\r\n",regRead32(GDMP_TRG_RLT));
	
	GDMPDEBUGP("GDMP_INT_STS\t%08lx\r\n",regRead32(GDMP_INT_STS));
	len += sprintf(buf + len,"GDMP_INT_STS\t%08lx\r\n",regRead32(GDMP_INT_STS));
	
	GDMPDEBUGP("GDMP_INT_MSK\t%08lx\r\n",regRead32(GDMP_INT_MSK));	
	len += sprintf(buf + len,"GDMP_INT_MSK\t%08lx\r\n",regRead32(GDMP_INT_MSK));
	
	GDMPDEBUGP("GDMP_PRB_SEL\t%08lx\r\n",regRead32(GDMP_PRB_SEL));
	len += sprintf(buf + len,"GDMP_PRB_SEL\t%08lx\r\n",regRead32(GDMP_PRB_SEL));

	GDMPDEBUGP("GDMP_TRG_PATN0_L\t%08lx\r\n",regRead32(GDMP_TRG_PATN0_L));	
	len += sprintf(buf + len,"GDMP_TRG_PATN0_L\t%08lx\r\n",regRead32(GDMP_TRG_PATN0_L));
	
	GDMPDEBUGP("GDMP_TRG_PATN0_MSK_L\t%08lx\r\n",regRead32(GDMP_TRG_PATN0_MSK_L));
	len +=sprintf(buf + len,"GDMP_TRG_PATN0_MSK_L\t%08lx\r\n",regRead32(GDMP_TRG_PATN0_MSK_L));

	GDMPDEBUGP("GDMP_TRG_PATN0_H\t%08lx\r\n",regRead32(GDMP_TRG_PATN0_H));	
	len +=sprintf(buf + len,"GDMP_TRG_PATN0_H\t%08lx\r\n",regRead32(GDMP_TRG_PATN0_H));
	
	GDMPDEBUGP("GDMP_TRG_PATN0_MSK_H\t%08lx\r\n",regRead32(GDMP_TRG_PATN0_MSK_H));
	len +=sprintf(buf + len,"GDMP_TRG_PATN0_MSK_H\t%08lx\r\n",regRead32(GDMP_TRG_PATN0_MSK_H));
	
	GDMPDEBUGP("GDMP_TRG_PATN1_L\t%08lx\r\n",regRead32(GDMP_TRG_PATN1_L));	
	len +=sprintf(buf + len,"GDMP_TRG_PATN1_L\t%08lx\r\n",regRead32(GDMP_TRG_PATN1_L));
	
	GDMPDEBUGP("GDMP_TRG_PATN1_MSK_L\t%08lx\r\n",regRead32(GDMP_TRG_PATN1_MSK_L));
	len +=sprintf(buf + len,"GDMP_TRG_PATN1_MSK_L\t%08lx\r\n",regRead32(GDMP_TRG_PATN1_MSK_L));
	
	GDMPDEBUGP("GDMP_TRG_PATN1_H\t%08lx\r\n",regRead32(GDMP_TRG_PATN1_H));	
	len +=sprintf(buf + len,"GDMP_TRG_PATN1_H\t%08lx\r\n",regRead32(GDMP_TRG_PATN1_H));
	
	GDMPDEBUGP("GDMP_TRG_PATN1_MSK_H\t%08lx\r\n",regRead32(GDMP_TRG_PATN1_MSK_H));
	len +=sprintf(buf + len,"GDMP_TRG_PATN1_MSK_H\t%08lx\r\n",regRead32(GDMP_TRG_PATN1_MSK_H));
	
	GDMPDEBUGP("GDMP_TRG_MODE\t%08lx\r\n",regRead32(GDMP_TRG_MODE));	
	len +=sprintf(buf + len,"GDMP_TRG_MODE\t%08lx\r\n",regRead32(GDMP_TRG_MODE));
	
	GDMPDEBUGP("GDMP_CAP_CFG\t%08lx\r\n",regRead32(GDMP_CAP_CFG));
	len +=sprintf(buf + len,"GDMP_CAP_CFG\t%08lx\r\n",regRead32(GDMP_CAP_CFG));

	GDMPDEBUGP("GDMP_CLK_CFG\t%08lx\r\n",regRead32(GDMP_CLK_CFG));
	len +=sprintf(buf + len,"GDMP_CLK_CFG\t%08lx\r\n",regRead32(GDMP_CLK_CFG));	
#endif
	ecnt_kernel_fs_write(fp, buf, strlen(buf),&fp->f_pos);
	filp_close(fp,NULL);
	set_fs(fs);
	printk("GDMP Register dump Finished\n");
	return 0;
}
#endif

#if defined(TCSUPPORT_TIMER_TEST)
uint32 getTimerCnt(void)
{
    return globe_cnt;
}

void pauseTimerCnt(uint32 second)
{
    volatile uint32 timebase;
    uint32 timepassed;

    timebase = getTimerCnt();
    printk("timebase %d\r\n", timebase);
    do
    {
        timepassed = getTimerCnt();       
        printk("timepassed %d\r\n", timepassed);      
        if(timepassed < timebase)
        {
           timepassed += (0xffffffff - timebase + 1); 
        }
        else
        {
            timepassed -= timebase;
        }
    }
    while(timepassed < second);
}

static irqreturn_t timer_isr(int irq, void *dev_id)
{
    uint32 word;
    /*1.close interrupt*/
    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19))
    disable_interrupt_by_intSrc(TIMER0_IRQ_SRC+timer_sel);
    #else
    VPint(CR_INTC_IMR) &=~(1<<(TIMER0_IRQ_SRC + timer_sel));
    #endif
    
    /*2.get interrupt status bit,and write 1 to clear*/
    word = VPint(CR_TIMER_CTL);
	word &= 0xffc0ffff;
	switch(timer_sel){
		case 0:
			word |= 0x00010000;
			break;
        case 1:
			word |= 0x00020000;
			break;
		case 2:
			word |= 0x00040000;
			break;
		case 3:
			word |= 0x00200000;
			break;
		default:
			word |= 0x00250000;
			break;
	}
	VPint(CR_TIMER_CTL) = word;
    
    /*3.interrupt process*/
	globe_cnt++;
    
    /*4.open interrupt*/
    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19))
    enable_interrupt_by_intSrc(TIMER0_IRQ_SRC+timer_sel);
    #else
    VPint(CR_INTC_IMR) |=(1<<(TIMER0_IRQ_SRC + timer_sel));
    #endif
    
	return IRQ_HANDLED ;
}

void intMaskSet(uint32 int_src, uint8 enable)
{
    if(enable)
    {   
        #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19))
        enable_interrupt_by_intSrc(int_src);
        #else
        VPint(CR_INTC_IMR)|= (0x01 << int_src);
        #endif
    }
    else
    {
        #if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19))
        disable_interrupt_by_intSrc(int_src);
        #else
        VPint(CR_INTC_IMR)&= ~((0x01) << int_src);
        #endif
    }
}

void timer_Configure(uint8  timer_no, uint8 timer_enable, uint8 timer_mode, uint8 timer_halt)
{
    uint32 word,word1;
    word = VPint(CR_TIMER_CTL);
    word1 = (timer_enable << timer_no)|(timer_mode << (timer_no + 8))|(timer_halt << (timer_no + 26));
    word |= word1;
    VPint (CR_TIMER_CTL)=word;    
} 

void timerSet(uint32 timer_no,uint32 timerTime, uint32 enable,uint32 mode, uint32 halt)
{   
    uint32 word;
	
    word = 1 * SYS_HCLK * 500;	/* 1000 /2; */
	word = word * timerTime;
    timerLdvSet(timer_no,word);
    timerCtlSet(timer_no,enable,mode,halt);
}

int doTimerTest(int argc, char *argv[], void *p)
{   
    uint8 src_no=0;
    uint8 wdog_test_flag = 0;
    timer_sel = simple_strtoul(argv[1], NULL, 10); 
    printk("timer %d test\r\n",timer_sel);

    if((timer_sel < 0) || (timer_sel > 3))
    {
        printk("Error timer number input ,should be <0~3>");
    }

    /*set irq src number,or wdog test flag*/
    switch(timer_sel)
    {
        case 0:
           src_no = CONFIG_TIMER0_IRQ; 
           break;
        case 1:
           src_no = CONFIG_TIMER1_IRQ; 
           break;
        case 2:
           src_no = CONFIG_TIMER2_IRQ; 
           break;
        case 3: 
           wdog_test_flag = 1; 
           break;
        default:
            break;
    }
    globe_cnt = 0;
    if(wdog_test_flag)
    {
        /*system reboot,use "watchdog_reset" in tcwdog.c,using timer3*/
        while(1);
    }
    else
    {
        /*enable timer irq and set irq processor function*/
        timerSet(timer_sel, TIMERTICKS_1S, ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);
        intMaskSet(TIMER0_IRQ_SRC + timer_sel, ENABLE);
        if(request_irq(src_no, timer_isr, 0, NULL, NULL) != 0) 
        {
            printk("Request the interrupt service routine fail, irq:%d.\n", src_no) ;
            return -ENODEV ;
        }
        
        /*test,pause 10s using our enabled timer*/
        printk("Timer %d test start,counter 10 second\r\n",timer_sel);
        pauseTimerCnt(10);
        printk("Timer %d test finish\r\n",timer_sel);
        
        /*disable timer irq src*/ 
        free_irq(src_no, NULL);
        intMaskSet(TIMER0_IRQ_SRC + timer_sel, DISABLE);
    }  
    return 0;
}
#endif

#if defined(TCSUPPORT_BUS_TEST)
#if defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
int doTimersTest(int argc, char *argv[], void *p)
{
    int test_case, wdog_no, thsld_sec;

    test_case = simple_strtoul(argv[1], NULL, 10);
    wdog_no = simple_strtoul(argv[2], NULL, 10);
    thsld_sec = simple_strtoul(argv[3], NULL, 10);

    switch (test_case) {
    
        case 0:
            arht_timers_test();
            break;
        case 1:
            arht_wdogs_test(wdog_no, 0);
            break;
        case 2:
            arht_wdogs_test(wdog_no, thsld_sec);
            break;
        default:
            printk("(%s) wrong test case:%d\n", __func__, test_case);
    }

    return 0;
}
#endif
#endif

#if defined(TCSUPPORT_GDMA_TEST)
uint32 reverse32(uint32 ori_value)
{
    return (((ori_value&0xff000000) >> 24) | ((ori_value&0x00ff0000) >> 8) | ((ori_value&0x0000ff00) << 8)| ((ori_value&0x000000ff) << 24));
}

static int swap_flag = 0;
static int dword_align_flag = 0;

int gdmatest(uint8 *d_addr, uint8 *s_addr, uint32 test_size, uint32 src_phy_base, uint32 dst_phy_base) 
{
    uint32 i = 0;
    uint8 addr_offset = 0;
    uint8 *src = s_addr;   
    uint8 *dst = d_addr;
    uint8 channel = 0;
    uint8 burst_size = 0;
    /*copy 1k data*/
    uint32 len = test_size;
	uint32 count=0, count2=0;
    #ifdef TCSUPPORT_CPU_ARMV8
    u32 src_phy, dst_phy;
    #endif
    #ifndef TCSUPPORT_BUS_TEST
	printk("### GDMA Single-channel testing ###\r\n");
    #endif
    for(channel = 0; channel < GDMA_CH_NUM; channel++)
	{
	    #ifndef TCSUPPORT_BUS_TEST
       	printk("Channel:%d, size=%d\r\n", channel, len);
        #endif
		for(burst_size = 0; burst_size <= GDMA_MAX_BURST; burst_size++)
		{              
 			for(addr_offset = 0; addr_offset < 4; addr_offset++)
			{
               	if( dword_align_flag && (addr_offset != 0) )
					continue;
					
                src = (uint8 *)(s_addr + addr_offset);
                dst = (uint8 *)(d_addr + addr_offset);
                #ifdef TCSUPPORT_CPU_ARMV8
                src_phy = src_phy_base + addr_offset;
                dst_phy = dst_phy_base + addr_offset;
                #endif
                #ifndef TCSUPPORT_BUS_TEST
                #ifdef TCSUPPORT_CPU_ARMV8
                printk("  GDMA src:0x%lx dst:0x%lx src_phy:0x%lx dst_phy:0x%lx burst=%d\r\n", src, dst, src_phy, dst_phy, burst_size); 
                #else
                printk("  GDMA src:0x%08lx dst:0x%x burst=%d\r\n", src, dst, burst_size); 
                #endif
                #endif
                for(i = 0; i < len; i++)
                {
                    src[i] = (i+count)&0xff;
                    /* when DRAM is busy, lots of cpu data-writes may be ququed in write-buff, so it's possible
                     * that some dst data-writes arrive at DRAM after GDMA has finished moved data to DRAM, so
                     * prevent from doing dst data-writes!
                     * src data-writes don't have this issue because HW's Bolcking mechanism will block GDMA's data-read. */
                    //dst[i] = 0;
                }

                #ifdef TCSUPPORT_CPU_ARMV8
                if(swap_flag)
                    SET_GDMA_CONFIG(channel, src_phy, dst_phy, ((len&0xffff)<<16)|(burst_size<<3)|(1<<1)|(1<<0), 0x10000004);
                else
                    SET_GDMA_CONFIG(channel, src_phy, dst_phy, ((len&0xffff)<<16)|(burst_size<<3)|(1<<1)|(1<<0), 0x4);

                
                /* wait until GDMA is done */
                while(!IS_GDMA_DONE(channel));
                CLEAR_GDMA_DONE(channel); /* clear done bit */
                
                #else /*TCSUPPORT_CPU_ARMV8*/
                
                VPint(TC_GDMA_SA0 + 16*channel) = ((unsigned long)src)&0x1fffffff; /* convert uncache to phy addr */
                VPint(TC_GDMA_DA0 + 16*channel) = ((unsigned long)dst)&0x1fffffff;
					
				if( swap_flag )
	            	VPint(TC_GDMA_CT10 + 16*channel) = 0x10000004;
				else
					VPint(TC_GDMA_CT10 + 16*channel) = 0x4;
					
                VPint(TC_GDMA_CT00 + 16*channel) = ((len&0xffff)<<16) | (burst_size<<3) | (1<<1) | (1<<0);
                
                /* wait until GDMA is done */
                while (((VPint(TC_GDMA_CT00 + 16*channel)) & 0x2) != 0);

                #endif /*TCSUPPORT_CPU_ARMV8*/
                
                for(i = 0; i < len; i++)
                {
                    if(src[i] != dst[i])
                    {
                        #ifdef TCSUPPORT_CPU_ARMV8
                        printk("\r\nCompare failed:src[%d](0x%lx)=0x%02x dst[%d](0x%0x)=0x%02x\r\n",i,src_phy+i, src[i],i, dst_phy+i, dst[i]);
							
						printk("TC_GDMA_SA0[%d]=%08lx\r\n", channel, GET_GDMA_SA(channel));
						printk("TC_GDMA_DA0[%d]=%08lx\r\n", channel, GET_GDMA_DA(channel));
						printk("TC_GDMA_CT10[%d]=%08lx\r\n", channel, GET_GDMA_CT1(channel));
						printk("TC_GDMA_CT00[%d]=%08lx\r\n", channel, GET_GDMA_CT0(channel));
						printk("GET_SHARE_UNZIPMEM_SEL=%08lx\r\n", GET_SHARE_UNZIPMEM_SEL());
                        printk("channel:%d, burst_size:%d, addr_offset==%d, count==%d\r\n", channel, burst_size, addr_offset, count);
                        
                        dump(s_addr, len+0x10, src_phy_base);														
						dump(d_addr, len+0x10, dst_phy_base);
                        
                        #else
						printk("\r\nCompare failed:src[%d](%08lx)=%02x dst[%d](%08lx)=%02x\r\n",i,&src[i], src[i],i, &dst[i], dst[i]);
							
						printk("TC_GDMA_SA0[%d]=%08lx\r\n", channel, VPint(TC_GDMA_SA0 + 16*channel));
						printk("TC_GDMA_DA0[%d]=%08lx\r\n", channel, VPint(TC_GDMA_DA0 + 16*channel));
						printk("TC_GDMA_CT10[%d]=%08lx\r\n", channel, VPint(TC_GDMA_CT10 + 16*channel));
						printk("TC_GDMA_CT00[%d]=%08lx\r\n", channel, VPint(TC_GDMA_CT00 + 16*channel));
						printk("0xbfb00954=%08lx\r\n", VPint(0xbfb00954));	
                        
						dump(src, len, NULL);														
						dump(dst, len, NULL);	
                        #endif
                        return -1;
                    }
                }
                count++;
                if(i == len)
                {
                    /* printk("Compare completely success\r\n"); */
                }    
            }    
        }
	}


	/* Multi-Channel testing */
    #ifndef TCSUPPORT_BUS_TEST
	printk("### GDMA Multi-channel testing ###\r\n");
    #endif
	for(burst_size = 0; burst_size <= GDMA_MAX_BURST; burst_size++)
	{
	    #ifndef TCSUPPORT_BUS_TEST
		printk("GDMA burst_size:%d size=%d\r\n",burst_size, len);	
        #endif
		for(addr_offset = 0; addr_offset < 4; addr_offset++)
		{
			if( dword_align_flag && (addr_offset != 0) )
				continue;
		
			src = (uint8 *)(s_addr + addr_offset);
			dst = (uint8 *)(d_addr + addr_offset);
            #ifdef TCSUPPORT_CPU_ARMV8
            src_phy = src_phy_base + addr_offset;
            dst_phy = dst_phy_base + addr_offset;
            #endif
            
			for(i = 0; i < len; i++) {
				src[i] = (i+count2)&0xff;
            }
            count2++;
			
			for(channel = 0; channel < GDMA_CH_NUM; channel++)
			{
			    #if 0
			    /* when DRAM is busy, lots of cpu data-writes may be ququed in write-buff, so it's possible
                 * that some dst data-writes arrive at DRAM after GDMA has finished moved data to DRAM, so
                 * prevent from doing dst data-writes!
                 * src data-writes don't have this issue because HW's Bolcking mechanism will block GDMA's data-read. */
				for(i = 0; i < len; i++){
					dst[i] = 0;					
		    	}
                #endif

                #ifdef TCSUPPORT_CPU_ARMV8
                if(swap_flag)
                    SET_GDMA_CONFIG(channel, src_phy, dst_phy, 0, 0x10000004);
                else
                    SET_GDMA_CONFIG(channel, src_phy, dst_phy, 0, 0x4);

                #ifndef TCSUPPORT_BUS_TEST
                printk("  [CH%d] GDMA src addr:0x%lx dst addr:0x%lx src_phy:0x%lx dst_phy:0x%lx\r\n",channel, src, dst, src_phy, dst_phy);
                #endif
                dst_phy += (TC_GDMA_TEST_SIZE+32);
                
                #else /*TCSUPPORT_CPU_ARMV8*/
                
				VPint(TC_GDMA_SA0 + 16*channel) = ((unsigned long)src)&0x1fffffff; /* convert uncache to phy addr */
				VPint(TC_GDMA_DA0 + 16*channel) = ((unsigned long)dst)&0x1fffffff;
				
				if( swap_flag )
					VPint(TC_GDMA_CT10 + 16*channel) = 0x10000004;
				else
					VPint(TC_GDMA_CT10 + 16*channel) = 0x4;

                #ifndef TCSUPPORT_BUS_TEST
				printk("  [CH%d] GDMA src addr:0x%08lx dst addr:0x%x\r\n",channel, src, dst); 
                #endif

                #endif /*TCSUPPORT_CPU_ARMV8*/
                
				dst += (TC_GDMA_TEST_SIZE+32);
			}

			for(channel = 0; channel < GDMA_CH_NUM; channel++) {
                #ifdef TCSUPPORT_CPU_ARMV8
                SET_GDMA_CT0(channel, ((len&0xffff)<<16)|(burst_size<<3)|(1<<1)|(1<<0));
                #else
				VPint(TC_GDMA_CT00 + 16*channel) = ((len&0xffff)<<16) | (burst_size<<3) | (1<<1) | (1<<0);	
                #endif
            }

			count=0;
			/* wait until GDMA is done */
			while(1){
				for(channel = 0; channel < GDMA_CH_NUM; channel++){
                    #ifdef TCSUPPORT_CPU_ARMV8
                    if(!IS_GDMA_DONE(channel)){
                    #else
					if(((VPint(TC_GDMA_CT00 + 16*channel)) & 0x2) != 0 ){
                    #endif
						break;
					}
				}
				if( channel == GDMA_CH_NUM ) {
                    #ifdef TCSUPPORT_CPU_ARMV8
                    for(channel = 0; channel < GDMA_CH_NUM; channel++)
                        CLEAR_GDMA_DONE(channel); /* clear done bit */
                    #endif
					break;
                }
				count++;		
			}
            #ifndef TCSUPPORT_BUS_TEST
			printk("  final count=%d\r\n", count);
            #endif
			dst = (uint8 *)(d_addr + addr_offset);
			for(channel = 0; channel < GDMA_CH_NUM; channel++){					
				for(i = 0; i < len; i++)
				{
					if(src[i] != dst[i])
					{
						printk("\r\nERROR:(CH%d) Compare failed:s_addr[%d](%08lx)=%x  d_addr[%d](%08lx)=%x",channel, i,&src[i], src[i],i, &dst[i], dst[i]);
						return -1;
					}
				}
				if(i == len)
				{
					/* printk("Compare completely success\r\n"); */
				}	 
				dst += (TC_GDMA_TEST_SIZE+32);				
			}
	
		}

	}	
	return 0;
}

int gdmacopy(uint8 *dram_d_addr,uint8 *dram_s_addr,uint32 *sram_d_addr, uint32 *sram_s_addr, struct physAddr *physAddrP, unsigned long test_cnt) {
	unsigned long i;

    uint32 src_phy_base=0, dst_phy_base=0;
    
#ifndef TCSUPPORT_BUS_TEST
	printk("GDMA copy LEN:%d Dword\r\n",TC_GDMA_TEST_SIZE);
#endif
    #ifdef TCSUPPORT_CPU_ARMV8
    if (physAddrP==NULL) {
        printk("Error: physAddrP==NULL\n");
        return -1;
    }
    #endif

	for( i=0; i<test_cnt; i++ ){
        #ifndef TCSUPPORT_BUS_TEST
		printk("GDMA Test round: %d\r\n", i+1);

		printk("\r\n#####################GDMA DRAM Copy to DRAM Test#####################\r\n");
        #endif
		swap_flag = 0;
		dword_align_flag=0;
        #ifdef TCSUPPORT_CPU_ARMV8
        src_phy_base = physAddrP->dram_s_phys;
        dst_phy_base = physAddrP->dram_d_phys;
        #endif
		if( gdmatest(dram_d_addr, dram_s_addr, TC_GDMA_TEST_SIZE, src_phy_base, dst_phy_base) == 0 ) {
            #ifndef TCSUPPORT_BUS_TEST
			printk("\r\nGDMA DRAM Copy to DRAM Test Pass!!\r\n");
            #endif
        }
		else{
			printk("\r\nGDMA DRAM Copy to DRAM Test Fail!!\r\n");	
			return -1;
		}
	    #ifndef TCSUPPORT_BUS_TEST
		printk("\r\n#####################GDMA DRAM Copy to SRAM Test#####################\r\n");
        #endif

#ifdef __BIG_ENDIAN	
		swap_flag = 1;
#else
		swap_flag = 0;
#endif
		dword_align_flag=0;
        #ifdef TCSUPPORT_CPU_ARMV8
        src_phy_base = physAddrP->dram_s_phys;
        dst_phy_base = physAddrP->sram_d_phys;
        #endif
		if( gdmatest(sram_d_addr, dram_s_addr, TC_GDMA_TEST_SIZE, src_phy_base, dst_phy_base) == 0 ) {
            #ifndef TCSUPPORT_BUS_TEST
			printk("\r\nGDMA DRAM Copy to SRAM Test Pass!!\r\n");
            #endif
        }
		else{		
			printk("\r\nGDMA DRAM Copy to SRAM Test Fail!!\r\n");	
			return -1;
		}
		#ifndef TCSUPPORT_BUS_TEST
		printk("\r\n#####################GDMA SRAM Copy to DRAM Test#####################\r\n");
        #endif
#ifdef __BIG_ENDIAN	
		swap_flag = 1;
		dword_align_flag=1;
#else
		swap_flag = 0;
		dword_align_flag=0;
#endif
        #ifdef TCSUPPORT_CPU_ARMV8
        src_phy_base = physAddrP->sram_s_phys;
        dst_phy_base = physAddrP->dram_d_phys;
        #endif
		if( gdmatest(dram_d_addr, sram_s_addr, TC_GDMA_TEST_SIZE, src_phy_base, dst_phy_base) == 0 ) {
            #ifndef TCSUPPORT_BUS_TEST
			printk("\r\nGDMA SRAM Copy to DRAM Test Pass!!\r\n");	
            #endif
        }
		else{		
			printk("\r\nGDMA SRAM Copy to DRAM Test Fail!!\r\n");
			return -1;
		}
	    #ifndef TCSUPPORT_BUS_TEST
		printk("\r\n#####################GDMA SRAM Copy to SRAM Test#####################\r\n");
        #endif
		swap_flag = 0;		
#ifdef __BIG_ENDIAN			
		dword_align_flag = 1;
#else
		dword_align_flag = 0;
#endif
        #ifdef TCSUPPORT_CPU_ARMV8
        src_phy_base = physAddrP->sram_s_phys;
        dst_phy_base = physAddrP->sram_d_phys;
        #endif
		if( gdmatest(sram_d_addr, sram_s_addr, TC_GDMA_TEST_SIZE, src_phy_base, dst_phy_base) == 0 ){
            #ifndef TCSUPPORT_BUS_TEST
			printk("\r\nGDMA SRAM Copy to SRAM Test Pass!!\r\n");	
            #endif
        }
		else{		
			printk("\r\nGDMA SRAM Copy to SRAM Test Fail!!\r\n");
			return -1;
		}
	#if defined(TCSUPPORT_BUS_TEST)
		if (gdmacopy_counting(i)==0)
			return -1;
	#endif	
	}

    #ifndef TCSUPPORT_BUS_TEST
    printk("\r\nGDMA test(%d times) PASS!!\r\n", test_cnt);
    #endif
    
    return 0;
}

int doGdmaTest(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
    uint8 *dram_s_addr = NULL;
    uint8 *dram_d_addr = NULL; 
    /* sram uncache,en7512 gdump sram:0xbfa30000~bfa37fff */
    uint32 *sram_s_addr = (uint8 *)0xbfa40000;
    uint32 *sram_d_addr = (uint8 *)0xbfa48000;

    /* get param from console */
    uint32 test_cnt = simple_strtoul(argv[1], NULL, 10);   
    
    /* dram cache,1024+64(16 dword)  */
    uint8 *src_array = (uint8 *)kmalloc(TC_GDMA_TEST_SIZE+32, GFP_KERNEL);
    if(src_array == NULL){
		printk("src_array memory allocate fail!!\r\n");
		return 0;
	}
    uint8 *dst_array = (uint8 *)kmalloc((TC_GDMA_TEST_SIZE+32)*GDMA_CH_NUM, GFP_KERNEL);
	if(dst_array == NULL){
        kfree(src_array);
		printk("dst_array memory allocate fail!!\r\n");
		return 0;
	}
	
    /* cache covert  to uncache */
    dram_s_addr = (uint8 *)((uint32)src_array | 0x20000000);   
    dram_d_addr = (uint8 *)((uint32)dst_array | 0x20000000);
    #ifndef TCSUPPORT_CPU_ARMV8
    dma_cache_inv((unsigned long)(src_array), TC_GDMA_TEST_SIZE+32);
    dma_cache_inv((unsigned long)(dst_array), (TC_GDMA_TEST_SIZE+32)*GDMA_CH_NUM);

    dma_cache_inv((unsigned long)(sram_s_addr), TC_GDMA_TEST_SIZE+32);
    dma_cache_inv((unsigned long)(sram_d_addr), (TC_GDMA_TEST_SIZE+32)*GDMA_CH_NUM);
    #endif
	gdmacopy(dram_d_addr, dram_s_addr,sram_d_addr, sram_s_addr, NULL, test_cnt);	
	
    kfree(src_array);
    kfree(dst_array);

#endif
    return 0;
}

int dofesramtest(int argc, char *argv[], void *p)
{
        int i,j;
        unsigned long addr = 0;
        unsigned long size = 0;
        unsigned char defPat[4] = {0x5a, 0xa5, 0x00, 0xff};
        unsigned long fe_addr = 0xbfa30000;
        unsigned long fe_addr2 = 0xbfb50000;
        int fail = 0;

        unsigned char *addr1, *addr2;
        size = (unsigned long)simple_strtoul(argv[1], NULL, 16);
        printk("size=%d  ",size);
        #ifndef TCSUPPORT_CPU_ARMV8
        /* Enable pbus access FE memory */
        VPint(0xbfb00958) |= 0x01;
        #endif
        /* FE SRAM 2M test */
        addr = 0xbf000000;
        addr1 = (unsigned char*)addr;
        printk("Test start address 0x%X len 0x%x\n", addr1,size);
        for(j=0;j<4;j++)
        {
            for(i=0;i<size ;i++)
            {
                *(addr1+i) = defPat[j];
            }

            printk("cmp pattern 0x%x \n ",defPat[j]);

            for(i=0;i<size ;i++)
            {
                if(*(addr1+i) != defPat[j])
                {
                    printk("read 0x%x: %X value error\n", addr1+i, *addr1);
                    fail = 1;
                    break;
                }
            }
            if(fail)
            {
                break;
            }
        }
        if(fail)
        {
            printk("fe sram test fail\n");
            return -1;
        }
        else
        {
            printk("fe sram test pass\n");
        }
        /*0xbfa30000 mapping test */
        addr = 0xbf030000;
        addr1 = (unsigned char*)addr;
        addr2 = (unsigned char*)fe_addr;

        printk("Test start address 0x%X len: 0x10000\n", addr2);
        for(j=0;j<4;j++)
        {
            for(i=0;i<0x10000 ;i++)
            {
                *(addr2+i) = defPat[j];
            }

            printk("cmp pattern 0x%x \n ",defPat[j]);
            //cmp
            for(i=0;i<0x10000 ;i++)
            {
                if(*(addr1+i) != *(addr2+i))
                {
                    printk("read 0x%x: %X value error\n", (addr1+i), *(addr1+i));
                    printk("read 0x%x: %X value error\n", (addr2+i), *(addr2+i));
                    fail = 1;
                    break;
                }
            }
            if(fail)
            {
                break;
            }
        }
        if(fail)
        {
            printk("0x%x test fail\n",addr2);
            return -1;
        }
        else
        {
            printk("0x%x test Pass\n",addr2);
        }

        /*0xbfb50000 mapping test */
        addr = 0xbf150000;
        addr1 = (unsigned char*)addr;
        addr2 = (unsigned char*)fe_addr2;

        printk("Test start address 0x%X len: 0x8000\n", addr2);
        for(j=3;j>=0;j--)
        {
            for(i=0;i<0x8000 ;i++)
            {
                *(addr2+i) = defPat[j];
            }

            printk("cmp pattern 0x%x \n ",defPat[j]);
            //cmp
            for(i=0;i<0x8000 ;i++)
            {
                if(*(addr1+i) != *(addr2+i))
                {
                    printk("read 0x%x: %X value error\n", (addr1+i), *(addr1+i));
                    printk("read 0x%x: %X value error\n", (addr2+i), *(addr2+i));
                    fail = 1;
                    break;
                }
            }
            if(fail)
            {
                break;
            }
        }
        if(fail)
        {
            printk("0x%x test fail\n",addr2);
            return -1;
        }
        else
        {
            printk("0x%x test Pass\n",addr2);
        }

        return 0;
}

#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_WAN_UPSTREAM_REMARK)
int doSysRemark(int argc, char *argv[], void *p)
{
    setUserSpaceFlag(26);
	if(argc < 2 || argc > 4)
	{	
		printk("Usage: remark wantr69 pbit_value\r\n");
		return -1;
	}
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/

int doSysExMdio(int argc, char *argv[], void *p)
{
	return subcmd(subExMdioCmds, argc, argv, p);
}
int doExMdioConf(int argc, char *argv[], void *p)
{
	unsigned char tmpMdc;
	unsigned char tmpMdio;
	tmpMdc = (unsigned char)simple_strtoul(argv[1], NULL, 10);
	tmpMdio = (unsigned char)simple_strtoul(argv[2], NULL, 10);
    #ifndef TCSUPPORT_CPU_ARMV8
	if(exModeMDIOGpioConf(tmpMdc,tmpMdio)){
		tmpMdc = tmpMdio = 0;
		exModeMDIOGpioQuery(&tmpMdc,&tmpMdio);
		printk("ex mdio config:mdc gpio:%d,mdio gpio:%d\n",tmpMdc,tmpMdio);
		return 0;
	}
	else
		return (-1);
    #endif

	return 0;
}

int doExMdioWrite(int argc, char *argv[], void *p)
{
	unsigned int tmpReg = (unsigned int)simple_strtoul(argv[1], NULL, 16);
	unsigned int tmpVal = (unsigned int)simple_strtoul(argv[2], NULL, 16);
    #ifndef TCSUPPORT_CPU_ARMV8
	exModeMDIOWrite(tmpReg,tmpVal);
	printk("write mdio reg 0x%x val:0x%x \n",tmpReg,exModeMDIORead(tmpReg));
    #endif
	return 0;
}
int doExMdioRead(int argc, char *argv[], void *p)
{
	unsigned int tmpReg = (unsigned int)simple_strtoul(argv[1], NULL, 16);
    #ifndef TCSUPPORT_CPU_ARMV8
	printk("mdio reg 0x%x val:0x%x \n",tmpReg,exModeMDIORead(tmpReg));
    #endif
	return 0;
}

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BOOTLOADER_MODIFY_PVNAME)
static int doSysAtsp(int argc, char *argv[], void *p){
    setUserSpaceFlag(6);
	return 0;	
}

static int doSysAtsv(int argc, char *argv[], void *p){
    setUserSpaceFlag(7);
	return 0;
	
}
  
static int doSysAtspv(int argc, char *argv[], void *p){
    setUserSpaceFlag(8);
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/

#ifdef L2_AUTOPVC
/*_____________________________________________________________________________
**      function name: autopvc_cmd_register_to_sys
**      descriptions:
**         Register autopvc ci-cmd into sys ci-cmd tables.
**
**      parameters:
**             cmds_p: Specify you want to register ci-cmd
**      global:
**             Cmds
**      return:
**              Success:        0
**              Otherwise:     -1
**      call:
**   	None
**      revision:
**      1. krammer 2008/8/13
**____________________________________________________________________________
*/
int
autopvc_cmd_register_to_sys(cmds_t *cmds_p)
{
	int i=0;
	printk("register autopvc cmd to sys\n");
	for(i=0; sysCmds[i].name; i++){
		if(memcmp(sysCmds[i].name,"autopvc",7) == 0){
			sysCmds[i].func = (*cmds_p).func;
			return 0;
		}
	}

	printk("Can't find autopvc-cmd\n");
	return -1;
}
int
autopvc_cmd_unregister_to_sys(void)
{
	int i=0;
    printk("unregister autopvc cmd from sys\n");
	for(i=0; sysCmds[i].name; i++){      
		if(memcmp(sysCmds[i].name,"autopvc",7) == 0){
            sysCmds[i].func = NULL;
			return 0;
		}
	}

	printk("Can't find autopvc-cmd\n");
	return -1;
}
EXPORT_SYMBOL(autopvc_cmd_register_to_sys);
EXPORT_SYMBOL(autopvc_cmd_unregister_to_sys);
#endif

#if defined(TCSUPPORT_WAN2LAN_EXT)
 u32 gsw_read(uint32 reg)
 {
 	if(isMT7520G || isMT7525G || isEN7526G){
		if(gswPbusRead_hook != NULL)
			return gswPbusRead_hook(reg);
		else
			return 0;
 	}
	else{
		return read_reg_word(GSW_BASE + reg);
	}
 }
 void gsw_write(uint32 reg, u32 regVal)
 {
 	if(isMT7520G || isMT7525G || isEN7526G){
		if(gswPbusWrite_hook != NULL)
			gswPbusWrite_hook(reg, regVal);
 	}
	else{
		write_reg_word(GSW_BASE + reg, regVal);
	}
	return;
 }
 int getPort(int lanPort)
 {
 	int portId = -1;
 	if(lanPortmap_hook)
		portId = lanPortmap_hook(lanPort);
	return portId;
 }
/*get lan port ,ex: LAN1 ,LAN2 ,LAN3 */
 int getLanPort(int switchPort)
 {
 	int portId = -1;
 	if(lanPortmap_rev_hook)
		portId = lanPortmap_rev_hook(switchPort);
	return portId;
 }
 
 static int doLan2lan(int argc, char *argv[], void *p)
 {
 	int srcLan = 0;	
	int portId = -1;
	int dstLan = 0;
	u32 regVal = 0;

	if(isMT7520G || isMT7525G || isEN7526G){
		if(gswPbusWrite_hook == NULL){
			printk("Ext_switch, but not insmod raeth, error\n");
			return -1;
		}
	}
		
 	if(argc != 5 && argc != 2){
 		printk("Usage: lan2lan <src LanPort> [rx||tx||rxtx] [on] <dst LanPort>  or\r\n lan2lan off\r\n");
		return -1;
	}
	if(5 == argc){
		if((my_atoi(argv[4]) > 4) || (my_atoi(argv[4]) < 1)){
			printk("dst Lan port out of range(available value is 1~4) \r\n");
			return -1;
		}
		if (strcmp(argv[3], "on") == 0){
			srcLan = my_atoi(argv[1]);
			if(srcLan < 1 || srcLan >4){
				printk("Src Lan port must in 1~4.");
					return -1;
			}
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_WAN_ETHER) && defined(TCSUPPORT_FH_ENV)
                if(FH_ETHER_WAN_WAN_PORT_ID == srcLan)
                {
                    portId = FH_ETHER_WAN_WAN_PORT_MAP_SWITCH_ID;
                }
                else
#endif/*TCSUPPORT_COMPILE*/  
                {
                    portId = getPort(srcLan);
                    if(-1 == portId)
                    {
                        printk("Invalid src LanPort!\n");
                        return -1;
                    }
                }
        	    if(strcmp(argv[2], "rx") == 0){
				regVal = gsw_read(GSW_PCL(portId));
				regVal |= 1<<8;		 /* Enable TX */
				gsw_write(GSW_PCL(portId), regVal);
				printk("lan2lan src Lan port %d rx on\r\n", srcLan);
			}
			else if(strcmp(argv[2], "tx") == 0){
				regVal = gsw_read(GSW_PCL(portId));
				regVal |= 1<<9;		 /* Enable RX */
				gsw_write(GSW_PCL(portId), regVal);
				printk("lan2lan src Lan port %d tx on\r\n", srcLan);
			}
			else if(strcmp(argv[2], "rxtx") == 0){
				regVal = gsw_read(GSW_PCL(portId));
				regVal |= 1<<8;
				regVal |= 1<<9;
				gsw_write(GSW_PCL(portId), regVal);
				printk("lan2lan src Lan port %d rx and tx on\r\n", srcLan);
			}
			else{
				printk("Usage: lan2lan <src LanPort> [rx||tx||rxtx] [on] <dst LanPort>  or\r\n      lan2lan off\r\n");
				return -1;
			}
			dstLan = my_atoi(argv[4]);
			portId = getPort(dstLan);
			if(-1 == portId){
				printk("Invalid dst LanPort!\n");
				return -1;
			}
			if(dstLan == srcLan){
				printk("Src port is the same with the dst port, error!\n");
				return -1;
			}
			regVal = gsw_read(GSW_MAC_FC);
			regVal |= 1<<3;   /* Enable mirror function */
			regVal &= 0xFFFFFFF8;
			regVal |= portId;
			gsw_write(GSW_MAC_FC, regVal);
			printk("lan2lan dst Lan port %d\r\n", dstLan);
			
			return 0;
		}
		else{
			printk("Usage: lan2lan <src LanPort> [rx||tx||rxtx] [on] <dst LanPort>  or\r\n      lan2lan off\r\n");
			return -1;
		}

	}	
	else if(2 == argc){
		if(strcmp(argv[1], "off") == 0){
			regVal = gsw_read(GSW_MAC_FC);
			regVal &= ~(1<<3);
			regVal &= 0xFFFFFFF8;
			gsw_write(GSW_MAC_FC, regVal);
			for(srcLan = 1; srcLan <= 4; srcLan++){
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_WAN_ETHER) && defined(TCSUPPORT_FH_ENV)
                if(FH_ETHER_WAN_WAN_PORT_ID == srcLan)
                {
                    portId = FH_ETHER_WAN_WAN_PORT_MAP_SWITCH_ID;
                }
                else
#endif/*TCSUPPORT_COMPILE*/  
                {
                    portId = getPort(srcLan);
                    if(-1 == portId)
                    {
                        printk("Invalid src LanPort!\n");
                        continue;
                    }
                }  
                
				regVal = gsw_read(GSW_PCL(portId));
				regVal &= ~(1<<8);
				regVal &= ~(1<<9);
				gsw_write(GSW_PCL(portId), regVal);
			}
			printk("lan2lan now is off\r\n");
			return 0;
		}
		else{
			printk("Usage: lan2lan <src LanPort> [rx||tx||rxtx] [on] <dst LanPort>  or\r\n      lan2lan off\r\n");
			return -1;
		}
	}
	else{
		printk("Usage: lan2lan <src LanPort> [rx||tx||rxtx] [on] <dst LanPort>  or\r\n      lan2lan off\r\n");
		return -1;
	}
	return -1;

}
#endif
#ifdef WAN2LAN
int my_atoi(const char *str)
{
	int result = 0;
	int signal = 1;
	if( (*str >= '0' && *str <= '9') || *str == '-' || *str == '+' ){
		if( *str == '-' || *str == '+' ){
			if( *str == '-' )
				signal = -1;
			str++;
		}
	}
	else 
		return 0;
	while( *str >= '0' && *str <= '9' )
		result = result*10 + (*str++ - '0');
	return signal*result;
}

//merge from linos,but it dosen't support SW3052
static int doWan2lan(int argc, char *argv[], void *p)
{
#if defined(TCSUPPORT_CMDPROMPT)
    	setUserSpaceFlag(13);
#endif

#if defined(TCSUPPORT_WAN2LAN_EXT)
		if(argc < 2 || argc > 5){
#else
		if(argc < 2 || argc > 3){
#endif
#if !defined(TCSUPPORT_CMDPROMPT)
#if defined(TCSUPPORT_WAN2LAN_EXT)
			printk("Usage: wan2lan [on||off] <number> , or\r\n wan2lan on [up||down] [out||in||raeth] <number>\r\n");
#else
			printk("Usage: wan2lan [on||off] <number> \r\n");
#endif
#else
		print_usage=1;
#endif
		if(masko_on_off == 1){
#if !defined(TCSUPPORT_CMDPROMPT)
			printk("Current wan2lan feature status: on \r\n");
			printk("Current portmask number is[%d] \r\n",masko);
#else
			print_state=1;
			print_portmask=1;
#endif
		}
		else if(masko_on_off == 0){
#if !defined(TCSUPPORT_CMDPROMPT)
			printk("Current wan2lan feature status: off \r\n");
#else
			print_state=1;
#endif
		}
		return -1;
	}
	if(argc == 3){
			if(((my_atoi(argv[2]) > 255) || (my_atoi(argv[2]) < 0))
				&& (strcmp(argv[2], "xsi") != 0)){
#if !defined(TCSUPPORT_CMDPROMPT)
			printk("input portmask number is out of range(available value is 0~255) \r\n");
#else
			print_errormsg=1;
#endif
			return -1;
		}
	}
#if defined(TCSUPPORT_WAN2LAN_EXT)
		else if(argc == 5){
				if((my_atoi(argv[4]) > 255) || (my_atoi(argv[4]) < 0)){
#if !defined(TCSUPPORT_CMDPROMPT)
				printk("input portmask number is out of range(available value is 0~255) \r\n");
#else
				print_errormsg=1;
#endif
				return -1;
			}
		}
#endif
#if !defined(TCSUPPORT_WAN2LAN_EXT)
	if (strcmp(argv[1], "on") == 0){
		masko_on_off = 1;
		masko = 0xF;
#if !defined(TCSUPPORT_CMDPROMPT)
		printk("Current wan2lan feature status: on \r\n");
#else
		print_state=1;
#endif
	}	
#else
	if (strcmp(argv[1], "on") == 0){
		if(argc == 3 || argc == 2)
			masko_on_off = 1;
		masko = 0xF;
#if !defined(TCSUPPORT_CMDPROMPT)
		printk("Current wan2lan feature status: on \r\n");
#else
		print_state=1;
#endif
	}	

#endif
	else if(strcmp(argv[1], "off") == 0){
		masko_on_off = 0;
#if defined(TCSUPPORT_WAN2LAN_EXT)
		masko_ext = 0;
		masko = 0x0;
#endif
#if !defined(TCSUPPORT_CMDPROMPT)
		printk("Current wan2lan feature status: off \r\n");
#else
		print_state=1;
#endif
		return 0;
	}
#if defined(TCSUPPORT_WAN2LAN_EXT)
	else{
		printk("Usage: wan2lan [on||off] <number> , or\r\n wan2lan on [up||down] [out||in||raeth] <number>\r\n");
		return -1;
	}
	if(argc >= 4){
		if (strcmp(argv[2], "up") == 0){
			if(strcmp(argv[3], "out") == 0)
				masko_ext |= 0x01;
			else if(strcmp(argv[3], "in") == 0)
				masko_ext |= 0x04;
			else if(strcmp(argv[3], "raeth") == 0)
				masko_ext |= 0x10;
			else{
				printk("Usage: wan2lan [on||off] <number> , or\r\n wan2lan on [up||down] [out||in||raeth] <number>\r\n");
				return -1;
			}
		}	
		else if(strcmp(argv[2], "down") == 0){
			if(strcmp(argv[3], "out") == 0)
				masko_ext |= 0x02;
			else if(strcmp(argv[3], "in") == 0)
				masko_ext |= 0x08;
			else if(strcmp(argv[3], "raeth") == 0)
				masko_ext |= 0x20;
			else{
				printk("Usage: wan2lan [on||off] <number> , or\r\n wan2lan on [up||down] [out||in||raeth] <number>\r\n");
				return -1;
			}
		}
		else if(strcmp(argv[2], "ext") == 0){
			if(my_atoi(argv[3]) > 0x3f || my_atoi(argv[3]) < 1){
				printk("mask error, must in 0x01~0x3f\n");
				return -1;
			}
			else{
				masko_ext |= my_atoi(argv[3]);
			}
		}
		
#if 0
		else if(strcmp(argv[2], "updown") == 0){
			if(strcmp(argv[3], "out") == 0)
				masko_ext |= 0x03;
			else if(strcmp(argv[3], "in") == 0)
				masko_ext |= 0x0c;
			else{
				printk("Usage: wan2lan [on||off] <number> , or\r\n wan2lan on [up||down] [out||in||raeth] <number>\r\n");
				return -1;
			}
		}
#endif
		else{
			printk("Usage: wan2lan [on||off] <number> , or\r\n wan2lan on [up||down] [out||in||raeth] <number>\r\n");
			return -1;
		}
	}
	printk("Current masko_ext number is[%d] \r\n",masko_ext);
#endif
	if(argc == 3){
		if(strcmp(argv[2], "xsi") == 0)
			masko = 0x10;
		else
			masko = simple_strtoul(argv[2],NULL, 10);
#if !defined(TCSUPPORT_CMDPROMPT)
		printk("Current portmask number is[%d] \r\n",masko);
#else
		print_portmask=1;
#endif
		return 0;
	}
#if defined(TCSUPPORT_WAN2LAN_EXT)
	else if(argc == 5){
		masko = simple_strtoul(argv[4],NULL, 10);
		printk("Current portmask number is[%d] \r\n",masko);
		return 0;
	}
	else{
		printk("Current portmask number is[%d] \r\n",masko);
	}
#endif
	return -1;
}

static int doWlan2lan(int argc, char *argv[], void *p)
{
    if(argc < 2 || argc > 3){
        printk("Usage: wlan2lan [on||off] <number> \r\n");

        if(wlan_masko_on_off == 1){
            printk("Current wlan2lan feature status: on \r\n");
            printk("Current portmask number is[%d] \r\n",masko);
        }
        else if(wlan_masko_on_off == 0){
            printk("Current wlan2lan feature status: off \r\n");
        }
        return -1;
    }
    if(argc == 3){
        if((my_atoi(argv[2]) > 15) || (my_atoi(argv[2]) < 0)){
            printk("input portmask number is out of range(available value is 0~15) \r\n");
            return -1;
        }
    }

    if (strcmp(argv[1], "on") == 0){
        wlan_masko_on_off = 1;
        wlan_masko = 0xF;
        printk("Current wlan2lan feature status: on \r\n");
    }	
    else if(strcmp(argv[1], "off") == 0){
        wlan_masko_on_off = 0;
        printk("Current wlan2lan feature status: off \r\n");
        return 0;
    }

    if(argc == 3){
        wlan_masko = simple_strtoul(argv[2],NULL, 10);
        printk("Current portmask number is[%d] \r\n",wlan_masko);
        return 0;
    }
    return -1;   
}

#if defined(TCSUPPORT_WAN2LAN_EXT)
int sys_proc_wan2lan_ext_point_mask(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
		u32 index = 0;
		off_t pos = 0;
		off_t begin = 0;
	
		index += sprintf(buf+index, "%x\n", masko_ext );
	
		*eof = 1;
	
	done:
		*start = buf + (off - begin);
		index -= (off - begin);
		if (index<0) 
			index = 0;
		if (index>count) 	
			index = count;
		return index;

}

int sys_proc_wan2lan_ext_port_mask(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
		u32 index = 0;
		off_t pos = 0;
		off_t begin = 0;

		index += sprintf(buf+index, "%x\n", masko);
	
		*eof = 1;
	
	done:
		*start = buf + (off - begin);
		index -= (off - begin);
		if (index<0) 
			index = 0;
		if (index>count) 
			index = count;
		return index;

}

int sys_proc_lan2lan_src_ports(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
		u32 index = 0;
		u32 regVal = 0;
		u32 Vlaue = 0;
		int portId = -1;
		u32 LAN_port = 0;  /*LAN_port from 1 ~4 .LAN1 LAN2 LAN3 LAN4*/
		u32 LAN_port_rx_tx_Value = 0;
		off_t pos = 0;
		off_t begin = 0;
		
		for(portId = 0;portId < 6;portId++ )
		{
			regVal = gsw_read(GSW_PCL(portId));

			/*bit8 ,bit9 rx tx enbale */
			Vlaue = ((regVal & 0x300)>>8);
			if(Vlaue)
			{
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_WAN_ETHER) && defined(TCSUPPORT_FH_ENV)
                if(FH_ETHER_WAN_WAN_PORT_MAP_SWITCH_ID == portId)
                {
                    LAN_port = FH_ETHER_WAN_WAN_PORT_ID;
                }
                else
#endif/*TCSUPPORT_COMPILE*/
                {
                    LAN_port = getLanPort(portId);
                }
				LAN_port_rx_tx_Value |= (Vlaue<<(2*(LAN_port-1)));
			}
		}
		/*LAN_port_rx_tx_Value. bit0,1 :LAN1 rx,tx enable ; bit2,3 :LAN2 rx,tx enable ;*/
		index += sprintf(buf+index, "%x\n", LAN_port_rx_tx_Value);
	
		*eof = 1;
	
	done:
		*start = buf + (off - begin);
		index -= (off - begin);
		if (index<0) 
			index = 0;
		if (index>count) 
			index = count;
		return index;

}

int sys_proc_lan2lan_dst_port(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
		u32 index = 0;
		u32 regVal= 0;
		int switch_port = -1 ;
		int LAN_port = 0;  /*LAN_port_port from 1 ~4 .LAN1 LAN2 LAN3 LAN4*/
		off_t pos = 0;
		off_t begin = 0;
		
		/* 0xbfb58010,  . bit 3: Enable mirror function ,bit 2~0: port num ,only support one port */
		regVal = gsw_read(GSW_MAC_FC);
		if(regVal & 0x8)
		{
			switch_port = (regVal & 0x7) ;
			LAN_port = getLanPort(switch_port);
		}
		index += sprintf(buf+index, "%d\n", LAN_port);

		*eof = 1;
	
	done:
		*start = buf + (off - begin);
		index -= (off - begin);
		if (index<0) 
			index = 0;
		if (index>count) 
			index = count;
		return index;

}
				 
#endif

#if defined(TCSUPPORT_CMDPROMPT)
int
getWan2lanInfo(char *wan2lan_print)
{
	uint16	index=0;

	if(print_usage)
		index += sprintf( wan2lan_print+index, WAN2LAN_INFO_USAGE);
	if(print_errormsg)
		index += sprintf( wan2lan_print+index, WAN2LAN_INFO_ERRORMSG);
	if(print_state){
		if(masko_on_off == 1)
			index += sprintf( wan2lan_print+index, WAN2LAN_INFO_ON);
		else if(masko_on_off == 0)
			index += sprintf( wan2lan_print+index, WAN2LAN_INFO_OFF);
	}
	if(print_portmask)
			index += sprintf( wan2lan_print+index, WAN2LAN_INFO_PORTMASK,masko);

	print_usage=0;
	print_errormsg=0;
	print_state=0;
	print_portmask=0;
	
	return index;
}
int sys_proc_wan2lan(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int len = getWan2lanInfo(buf);
	if (len <= off+count)
		*eof = 1;
	*start = buf + off;
	len -= off;
	if (len>count)
		len = count;
	if (len<0)
		len = 0;
	return len;
}

#endif
#endif

#if defined(TCSUPPORT_CMDPROMPT)
int
getSysSubcmdsInfo(char *sys_subcmds)
{
	uint16	index=0;
	register const cmds_t *cmdp=NULL;
	char buf[66]={0};
	int i=0;

	index += sprintf( sys_subcmds+index, "valid subcommands:\n");

	memset(buf,' ',sizeof(buf));
	buf[64] = '\n';
	buf[65] = '\0';

	for(i=0,cmdp = sysCmds;cmdp->name != NULL;cmdp++){
			strncpy(&buf[i*16],cmdp->name,strlen(cmdp->name));
			if(i == 3){
				index += sprintf( sys_subcmds+index, buf);
				memset(buf,' ',sizeof(buf));
				buf[64] = '\n';
				buf[65] = '\0';
			}
			i = (i+1)%4;
	}
	if(i != 0)
		index += sprintf( sys_subcmds+index, buf);

	return index;
}
int sys_proc_subcmds(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int len = getSysSubcmdsInfo(buf);
	if (len <= off+count)
		*eof = 1;
	*start = buf + off;
	len -= off;
	if (len>count)
		len = count;
	if (len<0)
		len = 0;
	return len;
}
#endif


#if defined(TCSUPPORT_AUTOBENCH_AFE) || defined(TCSUPPORT_AUTOBENCH_MT7510)
static int waitDmtPowerOn(int argc, char *argv[], void *p)
{
	unsigned long int value;
	/* wait until dmt3095 is power on */       
        while(1){
		if(isRT63365){
			//printk("GPIO %x\n",(VPint(CR_GPIO_DATA)));
			value = (VPint(CR_GPIO_DATA) & (1<<0));
		}else{
			value = (VPint(CR_GPIO_DATA) & 0x2000000);
		}	
		if(value){
			return 0;
		}else{
			msleep(1000);
		}
	
		tc3162wdog_kick();
        }
}

static int checkReboot(int argc, char *argv[], void *p)
{
    if(VPint(0x8001fffc)==0x1234){
        VPint(0x8001fffc)=0;
        return 0; /* return back to get a console for setting */
    }
    else{
        printk("wait for system to reboot\n");
        while(1);
    }
}
#endif

#if defined(TCSUPPORT_LED_CHECK) || defined(TCSUPPORT_TEST_LED_ALL) 

static int doWLanLedCheck(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(30);
	if((argc == 2) && ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "off") == 0) || (strcmp(argv[1], "recover") == 0))){
		if(strcmp(argv[1], "off") == 0){
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
			doWLanLedOff();
#endif
			printk("Wlan led is turned off! \r\n");	
		 }	
		else if(strcmp(argv[1], "on") == 0){
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		doWLanLedOn();
#endif
		printk("Wlan led is turned on! \r\n");
	}
		else
		{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
			doWLanLedRecover();
#endif
			printk("Recover wlanled status! \r\n");
		}
	}
	else{
		printk("Usage: sys wlanled [on|off|recover] \r\n");
		return -1;
	}

	return 0;
}	
		
#endif

#if defined(TCSUPPORT_LED_BTN_CHECK) || defined(TCSUPPORT_TEST_LED_ALL)
#if defined(TCSUPPORT_LED_CHECK) || defined(TCSUPPORT_TEST_LED_ALL)
#ifndef TCSUPPORT_OPENWRT
static int doLedCheck(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(9);
	if((argc == 2) && ((strcmp(argv[1], "on") == 0) || (strcmp(argv[1], "off") == 0) || (strcmp(argv[1], "recover") == 0))){
		if(strcmp(argv[1], "off") == 0){
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
			doLedOff();
#endif
			printk("All led is turned off! \r\n");
		 }
		else if(strcmp(argv[1], "on") == 0){
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
			doLedOn();
#endif
			printk("All led is turned on! \r\n");
		}
		else
		{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
			doLedRecover();
#endif
			printk("Recover led status! \r\n");
		}
	}
	else{
		printk("Usage: sys led [on||off|recover] \r\n");
		return -1;
	}

	return 0;
}
#endif
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_BTN_CHECK)
static int doButtonCheck(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(10);
	if(argc != 2 || (strcmp(argv[1], "enable") && strcmp(argv[1], "disable"))){
		printk("Usage: sys button [enable||disable]\r\n");
		return -1;
	}
#ifndef TCSUPPORT_OPENWRT
	if (strcmp(argv[1], "disable") == 0){
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		gButtonCheck = 1;
#endif
		printk("All buttons are disabled! \r\n");
	}
	else{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		gButtonCheck = 0;
#endif
		printk("All buttons are enabled! \r\n");
	 }
#endif
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
static int doWifiLedCheck(int argc, char *argv[], void *p)
{
	if((argc == 2) && ((strcmp(argv[1], "off") == 0) || (strcmp(argv[1], "recover") == 0))){
		if(strcmp(argv[1], "off") == 0){
			doWifiLedOff();
			printk("Wifi led is turned off! \r\n");	
		 }	
		else
		{
			doWifiLedRecover();
			printk("Wifi led status! \r\n");
		}
	}
	else{
		printk("Usage: sys wifiled [off|recover] \r\n");
		return -1;
	}

	return 0;
}	
#endif/*TCSUPPORT_COMPILE*/

#if defined(TCSUPPORT_USBHOST)
int doUSBEYE(int argc, char *argv[], void *p)
{
	printk("USB EYE TEST\n");
    #ifndef TCSUPPORT_CPU_ARMV8
	if(isRT63365 || isMT751020 || isMT7505){
		VPint(0xbfbb0054) = 0x00041000;
		VPint(0xbfbb0058) = 0x00041000;
	}
	else if(isEN751221){
		VPint(0xbfb90434) = 0x40000000;
		VPint(0xbfb90444) = 0x40000000;
	}
	else{
		VPint(0xbfb80cf8) = 0x80001b40;
		VPint(0xbfb80cfc) = 0x800000e0;
		VPint(0xbfba1060) = 0x1000000;
		VPint(0xbfba1064) = 0x4000500;
		VPint(0xbfba1068) = 0x4000500;
		VPint(0xbfba1064) = 0x4000400;
		VPint(0xbfba1068) = 0x4000400;
	}
    #endif
	return 0;
}
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_NEW_GUI) || defined(TCSUPPORT_PON_TEST)
int doRomReset(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(5);
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C2_TRUE)
int doRomCheck(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(11);
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_OBM) || defined(TCSUPPORT_PON_TEST)
int doSysAtsh(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(14);
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/

#if/*TCSUPPORT_COMPILE*/ defined(USB_AUTOMOUNT) && defined(TCSUPPORT_CZ_GENERAL)
int doUsbCheck(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(15);
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/
#ifndef TCSUPPORT_CPU_ARMV8
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,31))
extern int watchFlag;
#endif
int doSysWatch(int argc, char *argv[], void *p)
{
	uint32 mask =0;
	uint32 val_Lo = 0x0;
	uint32 val_Hi = 0x40000003;
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
	int watchFlag=0;
	#endif
	if(argc<=3){
		printk("usgae: sys watch <flag0~3> <addr> <mask> \n");
		printk("flag 1: write only\t 2: read only\t 3:read/write\n");
		printk("addr and mask MUST be Hex, mask value:<0~0x1ff>\n");
		printk("addr value must be double word\n");
		return -1;
	}
	watchFlag = simple_strtoul(argv[1], NULL, 10);
	if(argc==4){
		val_Lo = simple_strtoul(argv[2], NULL, 16);
		val_Lo &=(~0x7); //bit0:2 used for w/r/i
		mask = simple_strtoul(argv[3], NULL, 16);
	}

	if(mask>0x1ff){
		printk("mask value error, mask value:<0~0x1ff> return\n");
		watchFlag=0;
		return -1;
	}
	printk("watch addr= %08lx, mask=%08lx\n",val_Lo,mask);
	switch(watchFlag){
		case 1://write only
			val_Lo |= (0x1<<0);
			break;
		case 2://read only
			val_Lo |= (0x1<<1);
			break;
		case 3://r/w
			val_Lo |= (0x3<<0);
			break;
		default:
			printk("do not monitor any addr,return\n");
			watchFlag=0;
			break;
	}
	if(watchFlag==0)
		return -1;
		
	val_Hi |= (mask<<3);
	
	printk("val_Lo=%08lx,val_Hi=%08lx\n",val_Lo,val_Hi);
	write_c0_watchlo2(val_Lo);
	write_c0_watchhi2(val_Hi);

	return 0;
}
#endif
#ifdef TCSUPPORT_PON_TEST
int doLaserTest(int argc, char *argv[], void *p)
{
	unsigned int write_data = 0;
	unsigned int read_data = 0;
	if(argc != 2 || (strcmp(argv[1], "on") && strcmp(argv[1], "off"))){
		printk("Usage: sys lasertest [on||off]\r\n");
		return -1;
	}

	if (strcmp(argv[1], "on") == 0){
		read_data = VPint(0xbfaf0108);
		write_data = (read_data & 0xffffff5f) | (1 * 0xa0);
		VPint(0xbfaf0108) = write_data;
		printk("Enable laser force up! \r\n");
	}	
	else{
		read_data = VPint(0xbfaf0108);
		write_data = (read_data & 0xffffff5f) | (0 * 0xa0);
		VPint(0xbfaf0108) = write_data;
		printk("Disable laser force up! \r\n");	
	 }

	return 0;
}

int doPonTempTest(int argc, char *argv[], void *p){
	setUserSpaceFlag(17);
	return 0;
}

int doSysAtbp(int argc, char *argv[], void *p){
	setUserSpaceFlag(18);
	return 0;
}
int doSysPonLinkStatus(int argc, char *argv[], void *p){
	setUserSpaceFlag(19);
	return 0;
}
#endif
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_OI_C9) || defined(TCSUPPORT_OI_C7) || defined(TCSUPPORT_CDS)
int doSysAtsn(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(20);
	return 0;
}
#endif/*TCSUPPORT_COMPILE*/

int doImageInfo(int argc,char *argv[],void *p)
{
	setUserSpaceFlag(21);
	return 0;
}
int doVoipTest(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(22);
	return 0;
}

int doSysBobCarlibration(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(23);
	return 0;
}

#if defined(TCSUPPORT_CT_UPG_PINGPONG) || defined(TCSUPPORT_NOR_FLASH_USED) || defined(CONFIG_DUAL_IMAGE)
int doSysBootFlag(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(24);
	return 0;
}
#endif

int doBindShowTask(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(25);
	return 0;
}

int doSysSifm(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(27);
	return 0;
}

int doSysSerdesSel(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(32);
	return 0;
}

#ifdef TCSUPPORT_BOARD_SELECT
int doSysBoardInfo(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(36);
	return 0;
}
#endif

int doSysHybrid(int argc, char *argv[], void *p)
{
	setUserSpaceFlag(37);
	return 0;
}

#ifdef TCSUPPORT_CDS
int doDead(int argc,char *argv[],void *p)
{	
	while(1)	
	{
	}	
	return 0;
}

#endif

#if defined(NPU_BMGR_TEST) && (defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552))
#define RUN_FLAG_VALUE				1
#define BMGR_BUF_ID_IS_EMPTY			0xffffffff

#define BMGR0_BASE				0x1EC08800   // first  BMGR
#define BMGR1_BASE				0x1EC0A000   // second BMGR

#if defined(TCSUPPORT_CPU_AN7583)
#define BMGR2_BASE				0x1EC07000   // third  BMGR
#endif

#if defined(TCSUPPORT_CPU_AN7552)   //AN7552 , index must be 0 or 1
#define BMGR_BASE(index)                       ((index==0)?(BMGR0_BASE):(BMGR1_BASE))
#elif defined(TCSUPPORT_CPU_AN7583) //AN7583 , index must be 0 or 1 or 2
#define BMGR_BASE(index)                       ((index==0)?(BMGR0_BASE):( ((index==1)?(BMGR1_BASE):(BMGR2_BASE)) ))
#endif

#define BUF_ID_START                             0x0004
#define BMGR_BUF_ID_START(index)                (BMGR_BASE(index)+BUF_ID_START)

#if defined(TCSUPPORT_CPU_AN7552)
#define BMGR_INTR_ON							0x0040
#define BMGR_M0_CNT                              0x0050
#define BMGR_M_CNT(port)                        (BMGR_M0_CNT+(0x4*port))             // port  must be 0 or 1 or 2
//#define BMGR_M1_CNT                              0x0054
//#define BMGR_M2_CNT                              0x0058
#define BMGR_M_CNT_ADDR(index,port)             (BMGR_BASE(index)+BMGR_M_CNT(port))
#define BITS_MASK                               0x0000ffff
#define HIGH_16BITS_VALUE(value)                ((value >> 16) & BITS_MASK)
#define LOW_16BITS_VALUE(value)                 (value & BITS_MASK)
#endif


#if defined(TCSUPPORT_CPU_AN7583)
#define BMGR_RAM_WR_CNT                          0X004C
#define BMGR_RAM_RD_CNT                          0X0050
#define BMGR_RAM_WR_CNT_ADDR(index)              ((BMGR_BASE(index)+BMGR_RAM_WR_CNT))
#define BMGR_RAM_RD_CNT_ADDR(index)              ((BMGR_BASE(index)+BMGR_RAM_RD_CNT))
#endif


#define BMGR_PBUS(index,port)                  ((BMGR_BASE(index)+0x800*(port)))     // port  must be 0 or 1 or 2
#define GET_BUF_ID_ADDR_0                        0x0100
#define GET_BUF_ID_ADDR(port)                   (GET_BUF_ID_ADDR_0+(0x4*port))        // port  must be 0 or 1 or 2
//#define GET_BUF_ID_ADDR_1                        0x0104
//#define GET_BUF_ID_ADDR_2                        0x0108

#define RELEASE_BUF_ID_ADDR_0                    0x010C
#define RELEASE_BUF_ID_ADDR(port)               (RELEASE_BUF_ID_ADDR_0+(0x4*port))    // port  must be 0 or 1 or 2
//#define RELEASE_BUF_ID_ADDR_1                    0x0110
//#define RELEASE_BUF_ID_ADDR_2                    0x0114

#define BMGR_PBUS_ALLOC_BUFID_ADDR(index,port)  ((BMGR_PBUS(index,port))+(GET_BUF_ID_ADDR(port)))
#define BMGR_PBUS_FREE_BUFID_ADDR(index,port)   ((BMGR_PBUS(index,port))+(RELEASE_BUF_ID_ADDR(port)))

static struct task_struct *task_cpu0_npubuffer_Test;
static struct task_struct *task_cpu1_npubuffer_Test;
unsigned int alloc_buffer_id(int, int);
void free_buffer_id(int, int, int);
int get_buffer_addr_index(int, int);
unsigned int read_npu_flag(int, int);
void set_npu_flag_value(int, unsigned long);
int check_cpu_flag_status(void);

#if defined(TCSUPPORT_CPU_AN7552)
unsigned int alloc_buffer_addr_array[6] ={0};
unsigned int  free_buffer_addr_array[6] ={0};
unsigned int get_buffer_id_cnt_array[6] ={0};

int  get_buffer_addr_index(index, port)
{
	if ((index == 0) && (port == 0)){
		return 0;
	}else if ((index == 0) && (port == 1)){
		return 1;
	}else if ((index == 0) && (port == 2)){
		return 2;
	}else if ((index == 1) && (port == 0)){
		return 3;
	}else if ((index == 1) && (port == 1)){
		return 4;
	}else if ((index == 1) && (port == 2)){
		return 5;
	}
}
#endif
#if  defined(TCSUPPORT_CPU_AN7583)
// This is done to reduce calculation time from BMGR[index]'s [port] to real addr
unsigned long alloc_buffer_addr_array[9] = {0}; //store BMGR alloc addr to array , e.g. 0x1EC_8900  0x1EC_9104 0x1EC_9908
unsigned long  free_buffer_addr_array[9] = {0}; //store BMGR free  addr to array , e.g. 0x1EC_890C  0x1EC_9110 0x1EC_9914
unsigned long get_buffer_id_cnt_array[9] = {0};
int  get_buffer_addr_index(index, port)
{
	if ((index == 0) && (port == 0)){
		return 0;
	}else if ((index == 0) && (port == 1)){
		return 1;
	}else if ((index == 0) && (port == 2)){
		return 2;
	}else if ((index == 1) && (port == 0)){
		return 3;
	}else if ((index == 1) && (port == 1)){
		return 4;
	}else if ((index == 1) && (port == 2)){
		return 5;
	}else if ((index == 2) && (port == 0)){
		return 6;
	}else if ((index == 2) && (port == 1)){
		return 7;
	}else if ((index == 2) && (port == 2)){
		return 8;
	}
}

#endif

#if defined(TCSUPPORT_CPU_AN7552)
/*
BMGR_M0_CNT  0x0050
BMGR_M1_CNT  0x0054
BMGR_M2_CNT  0x0058
*/
unsigned int get_bufer_id_cnt(int index, int port)
{
	unsigned int temp = 0;
	unsigned int high = 0;
	unsigned int low  = 0;


	int array_index=0;
	array_index = get_buffer_addr_index(index, port);
	unsigned int temp_value =  get_buffer_id_cnt_array[array_index];

	void *virtAddr;
	unsigned int *physAddr = BMGR_M_CNT_ADDR(index,port);

	if (temp_value == 0){
        virtAddr = physAddr_to_virtAddr((phys_addr_t)physAddr);
		get_buffer_id_cnt_array[array_index] = (unsigned int *)virtAddr;
	}else{
	    virtAddr = temp_value;
	}

	temp = regRead32((unsigned int *)virtAddr);
	high = HIGH_16BITS_VALUE(temp);
	low  = LOW_16BITS_VALUE(temp);

	//printk("The address of  BMGR_M%d_CNT_ADDR(%d,%d) is 0x%x  ===> virtAddr : 0x%x  and value=0x%x, freed  buffer id counter  = 0x%x , Used buffer id count = 0x%x \n", index, index, port, (unsigned int)physAddr, (unsigned int *)virtAddr, temp, high, low);
	return regRead32((unsigned int *)virtAddr);

}
#endif
/*
The following memory addresses are shared memory
npu can directly access these address;
but cpu wants to use these address, it needs to translate address before it can be used.

e.g.
//AN7552
CPU view             NPU view
0x9a1f6004   <--->   0x5a1f6004
0x9a1f600C   <--->   0x5a1f600C

//AN7583
CPU view             NPU view
0x9c600004   <--->   0x5c600004
0x9c60000C   <--->   0x5c60000C
 */
#ifdef TCSUPPORT_CPU_AN7552
	#define NPU_RUN_FLAG_ADDR        0x9a1f6004
	#define CPU_RUN_FLAG_ADDR        0x9a1f600C

	#define NPU_OVERFLOW_COUNT_ADDR  0x9a1f6010

	#define NPU_CORE0_COUNT_ADDR     0x9a1f6024
	#define NPU_CORE1_COUNT_ADDR     0x9a1f602C
	#define CPU_CORE0_COUNT_ADDR     0x9a1f6034
	#define CPU_CORE1_COUNT_ADDR     0x9a1f603C
#endif

#ifdef TCSUPPORT_CPU_AN7583
	#define NPU_RUN_FLAG_ADDR                   0x9c600004
	#define CPU_RUN_FLAG_ADDR                   0x9c60000C

	#define NPU_OVERFLOW_COUNT_ADDR             0x9c600014
	#define NPU_EMPTY_BUFFERID_COUNT_ADDR       0x9c60001C

	#define CPU_CORE0_COUNT_ADDR                0x9c600024
	#define CPU_CORE1_COUNT_ADDR                0x9c60002C

	#define NPU_CORE0_COUNT_ADDR                0x9c600034
	#define NPU_CORE1_COUNT_ADDR                0x9c60003C
	#define NPU_CORE2_COUNT_ADDR                0x9c600044
	#define NPU_CORE3_COUNT_ADDR                0x9c60004C
	#define NPU_CORE4_COUNT_ADDR                0x9c600054
	#define NPU_CORE5_COUNT_ADDR                0x9c60005C

	#define NPU_CORE0_INNER_CSR_READ_COUNT_ADDR     0x9c600064
	#define NPU_CORE1_INNER_CSR_READ_COUNT_ADDR     0x9c60006C
	#define NPU_CORE2_INNER_CSR_READ_COUNT_ADDR     0x9c600074
	#define NPU_CORE3_INNER_CSR_READ_COUNT_ADDR     0x9c60007C
	#define NPU_CORE4_INNER_CSR_READ_COUNT_ADDR     0x9c600084
	#define NPU_CORE5_INNER_CSR_READ_COUNT_ADDR     0x9c60008C

	#define NPU_CORE0_INNER_CSR_WRITE_COUNT_ADDR     0x9c600094
	#define NPU_CORE1_INNER_CSR_WRITE_COUNT_ADDR     0x9c60009C
	#define NPU_CORE2_INNER_CSR_WRITE_COUNT_ADDR     0x9c6000A4
	#define NPU_CORE3_INNER_CSR_WRITE_COUNT_ADDR     0x9c6000AC
	#define NPU_CORE4_INNER_CSR_WRITE_COUNT_ADDR     0x9c6000B4
	#define NPU_CORE5_INNER_CSR_WRITE_COUNT_ADDR     0x9c6000BC
#endif

#if defined(TCSUPPORT_CPU_AN7552)
unsigned int  npu_flag_addr_array[7] ={NPU_RUN_FLAG_ADDR, CPU_RUN_FLAG_ADDR, NPU_OVERFLOW_COUNT_ADDR, CPU_CORE0_COUNT_ADDR, CPU_CORE1_COUNT_ADDR, NPU_CORE0_COUNT_ADDR, NPU_CORE1_COUNT_ADDR};
#endif

#if defined(TCSUPPORT_CPU_AN7583)
unsigned int  npu_flag_addr_array[24] ={NPU_RUN_FLAG_ADDR, CPU_RUN_FLAG_ADDR, NPU_OVERFLOW_COUNT_ADDR, NPU_EMPTY_BUFFERID_COUNT_ADDR, \
                                        CPU_CORE0_COUNT_ADDR, CPU_CORE1_COUNT_ADDR, \
										NPU_CORE0_COUNT_ADDR, NPU_CORE1_COUNT_ADDR, NPU_CORE2_COUNT_ADDR, NPU_CORE3_COUNT_ADDR, NPU_CORE4_COUNT_ADDR, NPU_CORE5_COUNT_ADDR, \
										NPU_CORE0_INNER_CSR_READ_COUNT_ADDR, NPU_CORE1_INNER_CSR_READ_COUNT_ADDR, NPU_CORE2_INNER_CSR_READ_COUNT_ADDR, \
										NPU_CORE3_INNER_CSR_READ_COUNT_ADDR, NPU_CORE4_INNER_CSR_READ_COUNT_ADDR, NPU_CORE5_INNER_CSR_READ_COUNT_ADDR, \
										NPU_CORE0_INNER_CSR_WRITE_COUNT_ADDR, NPU_CORE1_INNER_CSR_WRITE_COUNT_ADDR, NPU_CORE2_INNER_CSR_WRITE_COUNT_ADDR, \
										NPU_CORE3_INNER_CSR_WRITE_COUNT_ADDR, NPU_CORE4_INNER_CSR_WRITE_COUNT_ADDR, NPU_CORE5_INNER_CSR_WRITE_COUNT_ADDR
										};
#endif

#if defined(TCSUPPORT_CPU_AN7583)
unsigned int get_BMGR_RAM_WR_CNT_RG(int index){ //AN7583
	unsigned int temp_value=0;
	temp_value = regRead32(BMGR_RAM_WR_CNT_ADDR(index));
	//printk("BMGR_RAM_WR_CNT_ADDR(%d) is 0x%x and value= 0x%x\n", index, BMGR_RAM_WR_CNT_ADDR(index), temp_value);
    return temp_value;
}

unsigned int get_BMGR_RAM_RD_CNT_RG(int index){ //AN7583
	unsigned int temp_value=0;
	temp_value = regRead32(BMGR_RAM_RD_CNT_ADDR(index));
	//printk("BMGR_RAM_RD_CNT_ADDR(%d) is 0x%x and value= 0x%x\n", index, BMGR_RAM_RD_CNT_ADDR(index), temp_value);
    return temp_value;
}
#endif

void set_npu_flag_value(int npu_index, unsigned long value)
{
	write_mem_reg(npu_flag_addr_array[npu_index], value, 4);
}

unsigned int read_npu_flag(int npu_index, int bPrint)
{
	unsigned int val=0;
	unsigned long *ptr=NULL;

	ptr = npu_flag_addr_array[npu_index];
	if(address_valid_check((unsigned long)ptr, 4) != 0)
	{
		return -1;
	}
    if(bPrint)
	    printk("\r\n<Address>\t<Value>\r\n");
	#ifdef TCSUPPORT_CPU_ARMV8
		//print_mem_reg(ptr, 4);
		{
			int len = 4;
			void *virtAddr;

			if (((unsigned int)ptr)>=DT_MEMORY_BASE) {
				virtAddr = phys_to_virt((phys_addr_t)ptr);
				ecnt_dcache_inv((unsigned long)virtAddr, len);
			}
			else {
				virtAddr = ioremap((phys_addr_t)ptr, len);
			}

			if (len>4)
				dump((unsigned long)virtAddr, len, ptr);
			else if (len==4){
				if(bPrint)
				    printk("0x%lx\t0x%08lx\r\n\n\n", (unsigned int)ptr, readl(virtAddr));
				return readl(virtAddr);
			}
			else if (len==2)
				printk("0x%lx\t0x%04lx\r\n", (unsigned int)ptr, readw(virtAddr));
			else if (len==1)
				printk("0x%lx\t0x%02lx\r\n", (unsigned int)ptr, readb(virtAddr));
			else
				printk("(%s)len:%d is incorrect\n", __func__, len);

			if (((unsigned int)ptr)<DT_MEMORY_BASE)
				iounmap(virtAddr);
			return;
		}
	#else
		printk("0x%08lx\t0x%08lx\r\n", (unsigned long)ptr, (unsigned long)*ptr);
	#endif


	return val;
}


#define  DEBUG   0
unsigned int alloc_buffer_id(int index, int port)
{
    #if DEBUG
		//These code can't run at stress test. It will call trace
		unsigned int temp= 2147483647;
		void * virtAddr;
		unsigned int *physAddr = BMGR_PBUS_ALLOC_BUFID_ADDR(index,port);

		virtAddr = ioremap((phys_addr_t)physAddr, 4/*len*/);
		temp     = regRead32((unsigned long *)virtAddr);
		iounmap(virtAddr);

		printk(" [%s] get buffer_id=0x%x from BMGR_PBUS_FREE_BUFID_ADDR(%d,%d) physAddr : 0x%x ===> virtAddr : 0x%llx  \r\n", \
					__func__, temp, index, port, (unsigned int)physAddr, virtAddr);
		return temp;
	#else

		int array_index = 0;
		array_index = get_buffer_addr_index(index, port);
		unsigned long temp_value =  alloc_buffer_addr_array[array_index];
		void * virtAddr;

		unsigned int val = 0;
		unsigned int *physAddr = BMGR_PBUS_ALLOC_BUFID_ADDR(index,port);

		if (0 == temp_value){
			virtAddr = ioremap((phys_addr_t)physAddr, 4/*len*/);
			//iounmap(virtAddr); //We don't need to use this virtAddr until stress test is ended

			alloc_buffer_addr_array[array_index] = (unsigned long *)virtAddr;
            /*
			printk(" get_buffer_addr_index(%d, %d)=>array_index:%d , alloc_buffer_addr_array[%d]= 0x%llx \n",\
		         index, port, array_index, array_index, alloc_buffer_addr_array[array_index]);

			printk(" [%s] get buffer_id=0x%x from BMGR_PBUS_FREE_BUFID_ADDR(%d,%d) physAddr : 0x%x ===> virtAddr : 0x%llx  \r\n", \
					__func__, val, index, port, (unsigned int)physAddr, virtAddr);
            */
		}else{
			virtAddr = temp_value;
			/*
			printk(" [%s] get buffer_id=0x%x from BMGR_PBUS_FREE_BUFID_ADDR(%d,%d), the virtAddr: 0x%llx  is from array \r\n", \
					__func__, val, index, port, virtAddr);
			*/
		}
		return regRead32((unsigned long *)virtAddr);

	#endif
}

void free_buffer_id(int index, int port, int buffer_id)
{
	#if DEBUG
		//These code can't run at stress test. It will call trace
		void * virtAddr;
		unsigned int *physAddr = BMGR_PBUS_FREE_BUFID_ADDR(index,port);

		virtAddr = ioremap((phys_addr_t)physAddr, 4/*len*/);
		regWrite32((unsigned long *)virtAddr, buffer_id);
		iounmap(virtAddr);

		printk(" [%s] return buffer_id=0x%x to BMGR_PBUS_FREE_BUFID_ADDR(%d,%d) physAddr : 0x%x ===> virtAddr : 0x%llx  \r\n",\
					__func__, buffer_id, index, port, (unsigned int)physAddr, virtAddr);
	#else
		int array_index = 0;
		array_index = get_buffer_addr_index(index, port);
		unsigned long temp_value =  free_buffer_addr_array[array_index];

		void *virtAddr;
		unsigned int value = buffer_id;

		if (0 == temp_value){

			unsigned int* physAddr = BMGR_PBUS_FREE_BUFID_ADDR(index,port);

			virtAddr = ioremap((phys_addr_t)physAddr, 4/*len*/);	// the default settings is 4

			free_buffer_addr_array[array_index] = virtAddr;
			//iounmap(virtAddr);  //We don't need to use this virtAddr until stress test is ended

			/*
			printk(" get_buffer_addr_index(%d, %d)=>array_index:%d , free_buffer_addr_array[%d]= 0x%llx \n",\
					 index, port, array_index, array_index, free_buffer_addr_array[array_index]);
			printk(" [%s] return buffer_id=0x%x to BMGR_PBUS_FREE_BUFID_ADDR(%d,%d) physAddr : 0x%x ===> virtAddr : 0x%llx  \r\n",\
					__func__, value, index, port, (unsigned int)physAddr, virtAddr);
			*/

		}else{
			virtAddr = temp_value;
			/*
			printk(" [%s] return buffer_id=0x%x from BMGR_PBUS_FREE_BUFID_ADDR(%d,%d), the virtAddr: 0x%llx  is from array \r\n", \
						__func__, value, index, port, virtAddr);
			*/
		}
		regWrite32((unsigned long *)virtAddr, value);
	#endif
}


#define DBG  0 //default: disabled
void dynamic_alloc_free_buffer_cpu0(int index, int port, int hw_init_buffer_id_num, int stress_test_buf_id_num, int bPrint)
{
	int i;
	unsigned int buffer_id;
	unsigned int temp_cnt;
	unsigned int overflow_count = 0;
	unsigned int empty_bufferid_count = 0;

	unsigned int  buffer_id_check0[stress_test_buf_id_num];
	memset(buffer_id_check0, 0 , sizeof(buffer_id_check0));
	#if 0//DBG
	for (i=0; i<stress_test_buf_id_num; i++){
	     printk(" %d ", buffer_id_check0[i]);
	}
	printk("\n\n");
	#endif

	if(bPrint){
	    printk("StressTest target is:  [%s]\n", __func__);
	    printk("The address of  BMGR_PBUS_ALLOC_BUFID_ADDR(%d,%d) is 0x%x  [%s]\n", index, port, BMGR_PBUS_ALLOC_BUFID_ADDR(index,port), __func__);
	    printk("BUFFER_ID_NUM is %d [%s]\n", hw_init_buffer_id_num, __func__);
        printk("stress_test_buf_id_num is %d [%s]\n", stress_test_buf_id_num, __func__);
	}

	for(i=0; i<stress_test_buf_id_num; i++){
		buffer_id = alloc_buffer_id(index, port);
		if (0==buffer_id)
			printk("buffer_id must be >=1, becasuse the settings of BMGR_BUF_ID_START(%d)=%d  \n", index,regRead32(BMGR_BUF_ID_START(index)));

		#if defined(TCSUPPORT_CPU_AN7552)
		temp_cnt  = get_bufer_id_cnt(index, port);
		if(bPrint)
		    printk("  [hw_buffer_id(%d,%d)]  get  buffer_id = 0x%x , freed  buffer id counter  = 0x%x , Used buffer id count = 0x%x  [%s] \n", index, port, buffer_id, HIGH_16BITS_VALUE(temp_cnt), LOW_16BITS_VALUE(temp_cnt), __func__);
		#endif

		if ((buffer_id & BMGR_BUF_ID_IS_EMPTY) == BMGR_BUF_ID_IS_EMPTY){
			#if DBG
                printk("Warning !!!!  buffer_id=0x%x for port[%d] of BMGR[%d]  ,  [%s]\n", BMGR_BUF_ID_IS_EMPTY, port, index, __func__);
			#endif
				empty_bufferid_count = read_npu_flag(3, 0);
				empty_bufferid_count += 1;
                set_npu_flag_value( 3, empty_bufferid_count);
		}else{
            if(buffer_id > hw_init_buffer_id_num){
				#if DBG
					printk("Error !!!!  buffer_id=0x%x  >  0x%x  for port[%d] of BMGR[%d]  ,  [%s]\n", buffer_id, hw_init_buffer_id_num, port, index, __func__);
					#if defined(TCSUPPORT_CPU_AN7583)
					temp_cnt = get_BMGR_RAM_WR_CNT_RG(index);
				    printk("BMGR_RAM_WR_CNT_ADDR(%d) is 0x%x and value= 0x%x\n", index, BMGR_RAM_WR_CNT_ADDR(index), temp_cnt);
					temp_cnt = get_BMGR_RAM_RD_CNT_RG(index);
	                printk("BMGR_RAM_RD_CNT_ADDR(%d) is 0x%x and value= 0x%x\n", index, BMGR_RAM_RD_CNT_ADDR(index), temp_cnt);
					#endif
				#endif

				overflow_count = read_npu_flag(2, 0);
				overflow_count += 1;
				set_npu_flag_value( 2, overflow_count);

			}else{
				buffer_id_check0[i] = buffer_id;
			}
		}
	}

	if(bPrint)
	    printk("    =======================[hw_buffer_id(%d,%d)],   Get BUFFER ID is finished. [%s]======================= \n\n",index, port, __func__);

	int k;
	unsigned int temp_value;
	for (k=0; k<stress_test_buf_id_num; k++)
	{
			temp_value = buffer_id_check0[k];
			if (0==temp_value)
				break;
		    free_buffer_id(index, port, temp_value);

			#if defined(TCSUPPORT_CPU_AN7552)
		    temp_cnt  = get_bufer_id_cnt(index, port);
			if(bPrint)
		        printk("  [hw_buffer_id(%d,%d)]  free  buffer_id = 0x%x  , freed  buffer id counter  = 0x%x , Used buffer id count = 0x%x  [%s] \n", index, port, temp_value, HIGH_16BITS_VALUE(temp_cnt), LOW_16BITS_VALUE(temp_cnt), __func__);
			#endif
	}

	if (bPrint && empty_bufferid_count >=1 )
		printk("Warning !!!!  empty_bufferid_count=%d \n", empty_bufferid_count);

	if (bPrint && overflow_count >=1 )
		printk("Error !!!!  overflow_count=%d \n", overflow_count);


	if(bPrint)
	    printk("    =======================[hw_buffer_id(%d,%d)],   Free BUFFER ID is finished. [%s]======================= \n\n",index, port, __func__);
}


void dynamic_alloc_free_buffer_cpu1(int index, int port, int hw_init_buffer_id_num, int stress_test_buf_id_num, int bPrint)
{
	int i;
	unsigned int buffer_id;
	unsigned int temp_cnt;
	unsigned int overflow_count = 0;
	unsigned int empty_bufferid_count = 0;

	unsigned int  buffer_id_check1[stress_test_buf_id_num];
	memset(buffer_id_check1, 0 , sizeof(buffer_id_check1));
	#if 0//DBG
	for (i=0; i<stress_test_buf_id_num; i++){
	     printk(" %d ", buffer_id_check1[i]);
	}
	printk("\n\n");
	#endif

	if(bPrint){
	    printk("StressTest target is:  [%s]\n", __func__);
	    printk("The address of  BMGR_PBUS_ALLOC_BUFID_ADDR(%d,%d) is 0x%x  [%s]\n", index, port, BMGR_PBUS_ALLOC_BUFID_ADDR(index,port), __func__);
	    printk("BUFFER_ID_NUM is %d [%s]\n", hw_init_buffer_id_num, __func__);
        printk("stress_test_buf_id_num is %d [%s]\n", stress_test_buf_id_num, __func__);
	}

	for(i=0; i<stress_test_buf_id_num; i++){
		buffer_id = alloc_buffer_id(index, port);
		if (0==buffer_id)
			printk("buffer_id must be >=1, becasuse the settings of BMGR_BUF_ID_START(%d)=%d  \n", index,regRead32(BMGR_BUF_ID_START(index)));

		#if defined(TCSUPPORT_CPU_AN7552)
		temp_cnt  = get_bufer_id_cnt(index, port);
		if(bPrint)
		    printk("  [hw_buffer_id(%d,%d)]  get  buffer_id = 0x%x , freed  buffer id counter  = 0x%x , Used buffer id count = 0x%x  [%s] \n", index, port, buffer_id, HIGH_16BITS_VALUE(temp_cnt), LOW_16BITS_VALUE(temp_cnt), __func__);
		#endif

		if ((buffer_id & BMGR_BUF_ID_IS_EMPTY) == BMGR_BUF_ID_IS_EMPTY){
			#if DBG
                printk("Warning !!!!  buffer_id=0x%x for port[%d] of BMGR[%d]  ,  [%s]\n", BMGR_BUF_ID_IS_EMPTY, port, index, __func__);
			#endif
				empty_bufferid_count = read_npu_flag(3, 0);
				empty_bufferid_count += 1;
                set_npu_flag_value( 3, empty_bufferid_count);
		}else{
            if(buffer_id > hw_init_buffer_id_num){
				#if DBG
					printk("Error !!!!  buffer_id=0x%x  >  0x%x  for port[%d] of BMGR[%d]  ,  [%s]\n", buffer_id, hw_init_buffer_id_num, port, index, __func__);
					#if defined(TCSUPPORT_CPU_AN7583)
					temp_cnt = get_BMGR_RAM_WR_CNT_RG(index);
				    printk("BMGR_RAM_WR_CNT_ADDR(%d) is 0x%x and value= 0x%x\n", index, BMGR_RAM_WR_CNT_ADDR(index), temp_cnt);
					temp_cnt = get_BMGR_RAM_RD_CNT_RG(index);
	                printk("BMGR_RAM_RD_CNT_ADDR(%d) is 0x%x and value= 0x%x\n", index, BMGR_RAM_RD_CNT_ADDR(index), temp_cnt);
					#endif
				#endif

				overflow_count = read_npu_flag(2, 0);
				overflow_count += 1;
				set_npu_flag_value( 2, overflow_count);

			}else{
				buffer_id_check1[i] = buffer_id;
			}
		}
	}
	if(bPrint)
	    printk("    =======================[hw_buffer_id(%d,%d)],   Get BUFFER ID is finished. [%s]======================= \n\n",index, port, __func__);

	int k;
	unsigned int temp_value;
	for (k=0; k<stress_test_buf_id_num; k++)
	{
			temp_value = buffer_id_check1[k];
			if (0==temp_value)
				break;
		    free_buffer_id(index, port, temp_value);
			#if defined(TCSUPPORT_CPU_AN7552)
		    temp_cnt  = get_bufer_id_cnt(index, port);
			if(bPrint)
		        printk("  [hw_buffer_id(%d,%d)]  free  buffer_id = 0x%x  , freed  buffer id counter  = 0x%x , Used buffer id count = 0x%x  [%s] \n", index, port, temp_value, HIGH_16BITS_VALUE(temp_cnt), LOW_16BITS_VALUE(temp_cnt), __func__);
			#endif
	}

	if (bPrint && empty_bufferid_count >=1 )
		printk("Warning !!!!  empty_bufferid_count=%d \n", empty_bufferid_count);

	if (bPrint && overflow_count >=1 )
		printk("Error !!!!  overflow_count=%d \n", overflow_count);


	if(bPrint)
	    printk("    =======================[hw_buffer_id(%d,%d)],   Free BUFFER ID is finished. [%s]======================= \n\n",index, port, __func__);
}


static int doGetNpuValue(int argc, char *argv[], void *p)
{
	int npu_index = 0;
	unsigned int value = 0;

	npu_index  = simple_strtoul(argv[1], NULL, 10);
	value = (unsigned long)simple_strtoul(argv[2], NULL, 16);

	if(value == 0xffffffff)
	{
		unsigned int temp_npu_value;
		temp_npu_value = read_npu_flag(npu_index, 0); //read  value, 1:print, 0:non-print

		if(npu_index==4 || npu_index==5)      // cpu
			printk(" [cpu %d] execution count=0x%x  for alloc/free buffer id  \n", (npu_index-4), temp_npu_value );

		else if(npu_index>=6 && npu_index<=11) //npu
			printk(" [npu %d] execution count=0x%x  for alloc/free buffer id  \n", (npu_index-6), temp_npu_value );

		else if(npu_index==0)
			printk(" npu_status_flag= 0x%x \n", temp_npu_value );

		else if(npu_index==1)
			printk(" cpu_status_flag= 0x%x \n", temp_npu_value );

		else if(npu_index==2)
			printk(" overflow counter:0x%x  (It's illegal to read counter of buffer_id )  \n", temp_npu_value );  // overflow

		else if(npu_index==3)
			printk(" empty buffer id counter:0x%x  (It's counter of empty buffer id to read buffer_id )  \n", temp_npu_value );  // empty buffer id

		return 0;
	}

	if(value != 0xffffffff)
	{
		set_npu_flag_value(npu_index, value);      //write value
		return 0;
	}
}

int check_cpu_flag_status()
{
	if (RUN_FLAG_VALUE==read_npu_flag(1, 0)) //check cpu status
		return 1;     //run
	else
		return 0;     //stop
}

/* cmd                 index   value
   sys npuhwbuffertest     0     0x1    start hardware buffer id test for all npu
   sys npuhwbuffertest     0     0x0    stop  hardware buffer id test for all npu

   sys npuhwbuffertest     1     0x1    start hardware buffer id test for all cpu and npu
   sys npuhwbuffertest     1     0x0    stop  hardware buffer id test for all cpu and npu

Notice:
   cpu can read/write for Outer RG but can't access inner RG(inner csr)
   npu can read/write for Outer RG or inner RG(inner csr)
*/
static int doNpuHwBufferTest(int argc, char *argv[], void *p)
{
	int index;
	unsigned int value;
	unsigned int temp_npu_value;

	index = simple_strtoul(argv[1], NULL, 10);
	value = (unsigned long)simple_strtoul(argv[2], NULL, 16);


	if (index==0 && value == 0x1){
		printk("NPU start test \n");
		set_npu_flag_value(index, value);         //write 1 to start test for npu

		temp_npu_value = read_npu_flag(index, 0); //read, second parameter is 0 which means don't show message

	}else if(index==1 && value == 0x1){
		printk("CPU + NPU start test \n");

		set_npu_flag_value(0, value);             //write 1 to start test for npu
		set_npu_flag_value(index, value);         //write 1 to start test for cpu

		temp_npu_value = read_npu_flag(0, 0);     //read npu status
		printk("npu_status_flag= 0x%x  \n"  , temp_npu_value);

		temp_npu_value = read_npu_flag(index, 0); //read cpu status
		printk("cpu_status_flag= 0x%x \n\n", temp_npu_value);

		wake_up_process(task_cpu0_npubuffer_Test);
		wake_up_process(task_cpu1_npubuffer_Test);

	}else if (index==0 && value == 0x0){
		printk("NPU Stop test \n");
		set_npu_flag_value(index, value);         //write 0 to stop test for npu

		temp_npu_value = read_npu_flag(index, 0); //read npu status
		printk("npu_status_flag= 0x%x  \n"  , temp_npu_value);

	}else if (index==1 && value == 0x0){
		printk("CPU + NPU Stop test \n");

		set_npu_flag_value(0, value);             //write 0 to stop test for npu
		set_npu_flag_value(index, value);         //write 0 to stop test for cpu

		temp_npu_value = read_npu_flag(0, 0);     //read npu status
		printk("npu_status_flag= 0x%x  \n"  , temp_npu_value);

		temp_npu_value = read_npu_flag(index, 0); //read cpu status
		printk("cpu_status_flag= 0x%x \n\n", temp_npu_value);
	}


	return 0;
}

#define DEFAULT_BUFFER_ID_NUM			4512

#define ENABLE_READ_EXCESS_BUFFERID             1
#if ENABLE_READ_EXCESS_BUFFERID
#define READ_EXCESS_BUFFERID_COUNT              1000
#endif

/* AN7552: 2 cpu core + 2 npu core
   AN7583: 2 cpu core + 6 npu core

   TEST_CORE_NUM: Every BMGR can be accessed by up to core number at the same time

   -The TEST_CORE_NUM for every BMGR using outer RG
   * cpu + npu
		AN7552: Max value of core is 4  (cpu0 cpu1 npu0 npu1) for every BMGR of AN7552								=> TEST_CORE_NUM = 4
		AN7583: Max value of core is 8  (cpu0 cpu1 npu0 npu1 npu2 npu3 npu4 npu5) for every BMGR of AN7583			=> TEST_CORE_NUM = 8

   -The TEST_CORE_NUM for every BMGR using [inner csr(npu) + outer RG(cpu)]
   AN7583: Max value of core is 5 [(select 3 npu core from total npu core) + 2 cpu core] for every BMGR of AN7583	=> TEST_CORE_NUM = 5
 */

#if defined(TCSUPPORT_CPU_AN7552) //AN7552
#define TEST_CORE_NUM				4
#define BMGR_PREFETCH_COUNT			0
#endif

#if defined(TCSUPPORT_CPU_AN7583) //AN7583
/*Either outer RG  or  inner RG (npu) + outer RG(cpu) */
#define OUTERRG_DYNAMIC_ALLOC_FREE_TEST         0	// outer RG
#define MIXED_DYNAMIC_ALLOC_FREE_TEST           1	// inner RG (npu) + outer RG (cpu)

#define BMGR_PREFETCH_COUNT			12

#if MIXED_DYNAMIC_ALLOC_FREE_TEST   // inner RG (npu) + outer RG(cpu)
#define TEST_CORE_NUM                           5
#endif
#if OUTERRG_DYNAMIC_ALLOC_FREE_TEST
#define TEST_CORE_NUM                           8
#endif
#endif


void doCpu0NpuHwBufferTest(void *arg)
{
	printk("%s \n",__func__);

	int cnt=0;
	int buffer_id_num = DEFAULT_BUFFER_ID_NUM;
	int bPrint = 0;             // 1: print message
	int bPrint_Outside = 0;     // 1: print message
	int hw_init_buffer_id_num ;
	int stress_test_buf_id_num;

	int test_core_num          =  TEST_CORE_NUM;
	hw_init_buffer_id_num      =  buffer_id_num;

	#if (OUTERRG_DYNAMIC_ALLOC_FREE_TEST) && (ENABLE_READ_EXCESS_BUFFERID)   //AN7552 or AN7583
	printk("cpu0 : outer RG  test and read excess buffer id test \n");
	stress_test_buf_id_num     =  buffer_id_num / test_core_num + READ_EXCESS_BUFFERID_COUNT/2;
	#endif

	#if (OUTERRG_DYNAMIC_ALLOC_FREE_TEST) && (!ENABLE_READ_EXCESS_BUFFERID)   //AN7552 or AN7583
	printk("cpu0 : outer RG  test \n");
	stress_test_buf_id_num     =  (buffer_id_num - BMGR_PREFETCH_COUNT) / test_core_num ;
	#endif

#if defined(TCSUPPORT_CPU_AN7583)	//AN7583
	#if (MIXED_DYNAMIC_ALLOC_FREE_TEST) && (ENABLE_READ_EXCESS_BUFFERID)
	printk("cpu0 : inner csr + outer RG  test and read excess buffer id test \n");
	stress_test_buf_id_num = buffer_id_num / test_core_num + READ_EXCESS_BUFFERID_COUNT/2;
	#endif

	#if (MIXED_DYNAMIC_ALLOC_FREE_TEST) && (!ENABLE_READ_EXCESS_BUFFERID)
	printk("cpu0 : inner csr + outer RG  test \n");
	stress_test_buf_id_num = (buffer_id_num - BMGR_PREFETCH_COUNT) / test_core_num ;
	#endif
#endif

	int cpu0 = 4;

	printk("Cpu0 start test \n");
	printk("Cpu0: buffer_id_num          is %d \n"  , buffer_id_num);
	printk("Cpu0: test core num          is %d \n"  , test_core_num);
	printk("Cpu0: stress_test_buf_id_num is %d \n", stress_test_buf_id_num);

	while(1)
	{
		//case 1
		dynamic_alloc_free_buffer_cpu0(0, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0,  the count of execution stores to npu_flag_addr_array[4]
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 2
		dynamic_alloc_free_buffer_cpu0(0, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 3
		dynamic_alloc_free_buffer_cpu0(0, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 4
		dynamic_alloc_free_buffer_cpu0(0, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 5
		dynamic_alloc_free_buffer_cpu0(0, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 6
		dynamic_alloc_free_buffer_cpu0(0, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 7
		dynamic_alloc_free_buffer_cpu0(0, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 8
		dynamic_alloc_free_buffer_cpu0(0, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 9
		dynamic_alloc_free_buffer_cpu0(0, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 10
		dynamic_alloc_free_buffer_cpu0(1, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 11
		dynamic_alloc_free_buffer_cpu0(1, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 12
		dynamic_alloc_free_buffer_cpu0(1, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 13
		dynamic_alloc_free_buffer_cpu0(1, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 14
		dynamic_alloc_free_buffer_cpu0(1, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 15
		dynamic_alloc_free_buffer_cpu0(1, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 16
		dynamic_alloc_free_buffer_cpu0(1, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 17
		dynamic_alloc_free_buffer_cpu0(1, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 18
		dynamic_alloc_free_buffer_cpu0(1, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

#if TCSUPPORT_CPU_AN7583
		//case 19
		dynamic_alloc_free_buffer_cpu0(2, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 20
		dynamic_alloc_free_buffer_cpu0(2, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 21
		dynamic_alloc_free_buffer_cpu0(2, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 22
		dynamic_alloc_free_buffer_cpu0(2, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 23
		dynamic_alloc_free_buffer_cpu0(2, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 24
		dynamic_alloc_free_buffer_cpu0(2, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 25
		dynamic_alloc_free_buffer_cpu0(2, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 26
		dynamic_alloc_free_buffer_cpu0(2, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;

		//case 27
		dynamic_alloc_free_buffer_cpu0(2, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" ################################# cpu0 cnt:%d   ################################# [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu0, cnt);  //4: cpu0
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU0_TEST;
#endif
	}

	EXIT_CPU0_TEST:
	    printk("Stop hardware buffer id management test , [%s] \n",__func__);

}

void doCpu1NpuHwBufferTest(void *arg)
{
	printk("%s \n",__func__);

	int cnt=0;
	int buffer_id_num = DEFAULT_BUFFER_ID_NUM;
	int bPrint = 0;           // 1: print message
	int bPrint_Outside = 0;	  // 1: print message
	int hw_init_buffer_id_num ;
	int stress_test_buf_id_num;

	int test_core_num          =  TEST_CORE_NUM;
	hw_init_buffer_id_num      =  buffer_id_num;

	#if (OUTERRG_DYNAMIC_ALLOC_FREE_TEST) && (ENABLE_READ_EXCESS_BUFFERID)   //AN7552 or AN7583
	printk("cpu1 : outer RG  test and read excess buffer id test \n");
	stress_test_buf_id_num     =  buffer_id_num / test_core_num + READ_EXCESS_BUFFERID_COUNT/2;
	#endif

	#if (OUTERRG_DYNAMIC_ALLOC_FREE_TEST) && (!ENABLE_READ_EXCESS_BUFFERID)   //AN7552 or AN7583
	printk("cpu1 : outer RG  test \n");
	stress_test_buf_id_num     =  (buffer_id_num - BMGR_PREFETCH_COUNT) / test_core_num ;
	#endif

#if defined(TCSUPPORT_CPU_AN7583)	//AN7583
	#if (MIXED_DYNAMIC_ALLOC_FREE_TEST) && (ENABLE_READ_EXCESS_BUFFERID)
	printk("cpu1 : inner csr + outer RG  test and read excess buffer id test \n");
	stress_test_buf_id_num = buffer_id_num / test_core_num + READ_EXCESS_BUFFERID_COUNT/2;
	#endif

	#if (MIXED_DYNAMIC_ALLOC_FREE_TEST) && (!ENABLE_READ_EXCESS_BUFFERID)
	printk("cpu1 : inner csr + outer RG  test \n");
	stress_test_buf_id_num = (buffer_id_num - BMGR_PREFETCH_COUNT) / test_core_num ;
	#endif
#endif

	int cpu1 = 5;

	printk("Cpu1 start test \n");
	printk("Cpu1: buffer_id_num          is %d \n"  , buffer_id_num);
	printk("Cpu1: test core num          is %d \n"  , test_core_num);
	printk("Cpu1: stress_test_buf_id_num is %d \n", stress_test_buf_id_num);

    while(1)
	{
		//case 1:
		dynamic_alloc_free_buffer_cpu1(0, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1, the count of execution stores to npu_flag_addr_array[5]
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 2:
		dynamic_alloc_free_buffer_cpu1(0, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 3:
		dynamic_alloc_free_buffer_cpu1(0, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 4:
		dynamic_alloc_free_buffer_cpu1(0, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 5:
		dynamic_alloc_free_buffer_cpu1(0, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 6:
		dynamic_alloc_free_buffer_cpu1(0, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 7:
		dynamic_alloc_free_buffer_cpu1(0, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 8:
		dynamic_alloc_free_buffer_cpu1(0, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 9:
		dynamic_alloc_free_buffer_cpu1(0, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 10:
		dynamic_alloc_free_buffer_cpu1(1, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 11:
		dynamic_alloc_free_buffer_cpu1(1, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 12:
		dynamic_alloc_free_buffer_cpu1(1, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 13:
		dynamic_alloc_free_buffer_cpu1(1, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 14:
		dynamic_alloc_free_buffer_cpu1(1, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 15:
		dynamic_alloc_free_buffer_cpu1(1, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 16:
		dynamic_alloc_free_buffer_cpu1(1, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 17:
		dynamic_alloc_free_buffer_cpu1(1, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 18:
		dynamic_alloc_free_buffer_cpu1(1, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

#if TCSUPPORT_CPU_AN7583
		//case 19:
		dynamic_alloc_free_buffer_cpu1(2, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 20:
		dynamic_alloc_free_buffer_cpu1(2, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 21:
		dynamic_alloc_free_buffer_cpu1(2, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 22:
		dynamic_alloc_free_buffer_cpu1(2, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 23:
		dynamic_alloc_free_buffer_cpu1(2, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 24:
		dynamic_alloc_free_buffer_cpu1(2, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 25:
		dynamic_alloc_free_buffer_cpu1(2, 0, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 26:
		dynamic_alloc_free_buffer_cpu1(2, 1, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
			goto EXIT_CPU1_TEST;

		//case 27:
		dynamic_alloc_free_buffer_cpu1(2, 2, hw_init_buffer_id_num, stress_test_buf_id_num, bPrint);
		cnt += 1;
		if (bPrint_Outside)
			printk(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ cpu1 cnt:%d   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ [%s]\n", cnt, __func__);
		set_npu_flag_value(cpu1, cnt);  //5: cpu1
		msleep(20);
		if (!check_cpu_flag_status())
		goto EXIT_CPU1_TEST;
#endif
	}

	EXIT_CPU1_TEST:
	    printk("Stop hardware buffer id management test , [%s] \n",__func__);

}
#endif


int tcsyscmd_init(void)
{
	cmds_t sys_cmd;
#if defined(TCSUPPORT_POWERSAVE_ENABLE) 
	uint32 value=0;
#endif
#if defined(TCSUPPORT_CMDPROMPT)
	/* sys subcommands  info */
	create_proc_read_entry("tc3162/sys_subcmds", 0, NULL, sys_proc_subcmds, NULL);
#ifdef WAN2LAN
	/* sys wan2lan related info */
	create_proc_read_entry("tc3162/sys_wan2lan", 0, NULL, sys_proc_wan2lan, NULL);
#endif
#endif

#if defined(TCSUPPORT_WAN2LAN_EXT)
			/* sys wan2lan lan2lan  related info */
	create_proc_read_entry("tc3162/sys_wan2lan_ext_src_point_mask", 0, NULL, sys_proc_wan2lan_ext_point_mask, NULL);
	create_proc_read_entry("tc3162/sys_wan2lan_ext_dst_port_mask", 0, NULL, sys_proc_wan2lan_ext_port_mask, NULL);
	create_proc_read_entry("tc3162/sys_lan2lan_src_ports", 0, NULL, sys_proc_lan2lan_src_ports, NULL);
	create_proc_read_entry("tc3162/sys_lan2lan_dst_port", 0, NULL, sys_proc_lan2lan_dst_port, NULL);

#endif


	/*Init sys root ci-cmd*/
	sys_cmd.name= "sys";
	sys_cmd.func=doSys;
	sys_cmd.flags=0x12;
	sys_cmd.argcmin=0;
	sys_cmd.argc_errmsg=NULL;

#if defined(TCSUPPORT_POWERSAVE_ENABLE)
	if(!isMT751020){
#if !defined(TCSUPPORT_USBHOST)
	VPint(0xbfb000a8)=0;
#endif
	}
#endif
	/*Register sys ci-cmd*/
	cmd_register(&sys_cmd);

#if defined(TCSUPPORT_BUS_TEST)
	bustest_kthread_init();
#endif

#if defined(NPU_BMGR_TEST) && (defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552))
	task_cpu0_npubuffer_Test = kthread_create(doCpu0NpuHwBufferTest, NULL, "cpu0_npubuffer_Test");
	task_cpu1_npubuffer_Test = kthread_create(doCpu1NpuHwBufferTest, NULL, "cpu1_npubuffer_Test");
	kthread_bind(task_cpu0_npubuffer_Test, 0);     /* bind to CPU0 */
	kthread_bind(task_cpu1_npubuffer_Test, 1);     /* bind to CPU1 */
#endif
	return 0;
}

//static void __exit tcsyscmd_exit(void)
void tcsyscmd_exit(void)
{
#if defined(TCSUPPORT_CMDPROMPT)
	remove_proc_entry("tc3162/sys_subcmds", NULL);
#ifdef WAN2LAN
	remove_proc_entry("tc3162/sys_wan2lan", NULL);
#endif
#endif
#if defined(TCSUPPORT_WAN2LAN_EXT)
			/* sys wan2lan lan2lan  related info */
	remove_proc_entry("tc3162/sys_wan2lan_ext_src_point_mask", NULL);
	remove_proc_entry("tc3162/sys_wan2lan_ext_dst_port_mask",  NULL);
	remove_proc_entry("tc3162/sys_lan2lan_src_ports", NULL);
	remove_proc_entry("tc3162/sys_lan2lan_dst_port",  NULL);

#endif
}
//module_init (tcsyscmd_init);
//module_exit (tcsyscmd_exit);

