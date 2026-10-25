/* //BBN_Linux/DEV/main/tclinux_phoenix/modules/private/tcphy/tcetherphy_7523.c */
/************************************************************************
*                E X T E R N A L   R E F E R E N C E S  (1)
**************************************************************************/

#ifdef LINUX_OS
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
//#include <asm/tc3162/TCIfSetQuery_os.h>
#include <asm/tc3162/cmdparse.h>
#include <linux/mii.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include "tcetherphy_hook.h"
#include "tcetherphy.h"
//configure phy test command start phy and end phy
//////////////////////////////////////////////////////
#define  all_port_start 9
#define  all_port_end 12
//////////////////////////////////////////////////////

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_AUTOBENCH) 
#include <ecnt_hook/ecnt_hook.h>
#endif

#endif //LINUX_OS

#define tcMiiStationRead(phy_add, phy_reg) ETHER_MDIO_READ(phy_add, phy_reg)
#define tcMiiStationWrite(phy_add, phy_reg, phy_data) ETHER_MDIO_WRITE(phy_add, phy_reg, phy_data)
#define tc3262_gmac_tx(a,b) 0
#define tc3262_gmac_dev 
#define debug_flag	1

/************************************************************************
*                         D A T A   T Y P E S  (2)
**************************************************************************/
#ifdef LINUX_OS
#define pause(x)					mdelay(x)
#endif //LINUX_OS

#ifdef TCPHY_DEBUG
#ifdef LINUX_OS
extern void setUserSpaceFlag(int num);
#endif //LINUX_OS

#ifndef LINUX_OS 					/*Linux platform is used sendicmp user-space to send icmp packet*/
typedef struct 
{
    ping_t  *ping_p;        		/* ping pointer         */
} pingEvt_t;
typedef struct 
{
    uint32  trace:1;
    uint32  echo:1;
    uint32  active:1;
    uint32  user:1;
    uint32  checkCmd:1;
    uint32  checkRsp:1;
} pingCmdFlag_t;
#define N_PELOG         20
typedef struct 
{
    mbuf_t* mbp;
    uint16  seq1;
    uint16  seq2;
    char    cc[4];
} peLog_t;
typedef struct 
{
    uint16  index;
    uint16  cnt;            		/* data drror counter       */
    peLog_t log[N_PELOG];
} peLogDcb_t;
typedef struct 
{
    pingCmdFlag_t flags;
    int actSocket;
    uint32  pingCnt;
    uint32  echoSeq;
    uint16  option;         		/* ping data option     */
    uint16  dataSeq;        		/* data sequence number     */
    char    dataChar;       		/* data character       */
    uint8   checkInd;       		/* data check indication    */
    peLogDcb_t* pelog_p;        	/* data log dcb         */
} pingCmdCb_t;
#endif // ndef LINUX_OS
#endif

static struct timer_list ephy_timer;

struct ecnt_hook_ops ecnt_driver_ephy_op = {  // JasonG_7512
    .name = "driver_ephy_hook",
    .hookfn = ecnt_ephy_hook,
    .maintype = ECNT_ETHER_PHY,
    .is_execute = 1,
    .subtype = ECNT_DRIVER_API,
    .priority = 1
};


/************************************************************************
*                        Data for EPHY  (3)
**************************************************************************/
#define EPHY_ADDR_P0   9

static uint8	ephy_addr_base = EPHY_ADDR_P0;		// replace  "mac_p->enetPhyAddr"
static uint8 	phyaddr_common = 9,force_giga_master_counter_p9=0,force_giga_master_counter_p10=0,force_giga_master_counter_p11=0,force_giga_master_counter_p12=0;
static uint8    force_giga_master_mode_p9=0,force_giga_master_mode_p10=0,force_giga_master_mode_p11=0,force_giga_master_mode_p12=0;
static uint8    cnt_eee=0, cnt_up2down=0, cnt_up=0, down_speed_flag=0;
static uint8    inital_finish_flag = 0;
static uint8    link_state_polling = 0;
static int port9_1G_Coarse_adjustment=0,port10_1G_Coarse_adjustment=0,port11_1G_Coarse_adjustment=0,port12_1G_Coarse_adjustment=0;
static int port9_100M_Coarse_adjustment=0,port10_100M_Coarse_adjustment=0,port11_100M_Coarse_adjustment=0,port12_100M_Coarse_adjustment=0;

static int mean_square_error_counter_p9=0,mean_square_error_counter_p10=0,mean_square_error_counter_p11=0,mean_square_error_counter_p12=0;
static int sw_down_shift_p9=0,sw_down_shift_p10=0,sw_down_shift_p11=0,sw_down_shift_p12=0;
static int ephy_monitor_all_counter=0,ephy_monitor_p9_counter=0,ephy_monitor_p10_counter=0,ephy_monitor_p11_counter=0,ephy_monitor_p12_counter=0;
static int ephy_monitor_continue_p9_counter=0,ephy_monitor_continue_p10_counter=0,ephy_monitor_continue_p11_counter=0,ephy_monitor_continue_p12_counter=0;
static int ephy_monitor_p9_curren_counter=0,ephy_monitor_p10_curren_counter=0,ephy_monitor_p11_curren_counter=0,ephy_monitor_p12_curren_counter=0;
static int ephy_monitor_p9_stop_polling=0,ephy_monitor_p10_stop_polling=0,ephy_monitor_p11_stop_polling=0,ephy_monitor_p12_stop_polling=0;
static uint8    sw_down_shift_en=0,slt_excuting=0,phy_downshift_force_giga_slave_enable=1;
unsigned int slt_not_excute_loop;

#ifdef TCPHY_SUPPORT
static uint8  tcPhyFlag = 0;
static uint8  tcPhyInitFlag = 0;
static uint8  tcPhyVer = 99; 					// default: unknown PhyVer
static uint8  fgMT7530_INT = 0;
#if debug_flag
static uint8  fgMT7510Ge_INT = 0;
#endif
static uint8  mt7530_flag = 0; 
static uint8  mt7530_15R_flag = 0;		// allen_20161031
static uint8  mt7530_15R_L_flag = 0;	// allen_20161129
static uint8  mt7530_0R_L_flag = 0;		// allen_20161226
static uint8  mt7530_5R_flag = 0;		// allen_20170202
static uint8  mt751221_5R_flag = 0;		// allen_20170621

static int eco_rev = 0x00;
static uint16  default_fe_l3r17 = 0x0000;   	// set value in init setting
static uint16  default_ge_dev7r3c = 0x0000; 	// set value in init setting
#define tcPhyVer_2031     	0  	// 9400, LEH
#define tcPhyVer_2101mb   	1  	// 9401, LEM
#define tcPhyVer_2104mc   	2  	// 9402, tc2206
#define tcPhyVer_2104sd   	3  	// 9403, tc2104sd
#define tcPhyVer_2101me   	4  	// 9404, 62UE
#define tcPhyVer_2102me   	5  	// 9405, tc3182
#define tcPhyVer_2104me   	6  	// 9406, tc2206F
#define tcPhyVer_2101mf   	7  	// 9407, FPGA(MF)
#define tcPhyVer_2105sg   	8  	// 9408, TC6501
#define tcPhyVer_2101mi   	10 	// 940a, RT63260
#define tcPhyVer_2105mj   	11 	// 940b, RT63365
#define tcPhyVer_2105sk   	12 	// 940c, TC6508
#define mtPhyVer_7530     	13 	// 9412, MT7530
#define tcPhyVer_2101mm   	14 	// 940e, MT7502 
#define tcPhyVer_mt7510FE 	15 	// 940f, MT7510, FE(Fast Ethernet)
#define mtPhyVer_7510Ge   	16 	// 9421, MT7510Ge      
#define tcPhyVer_mt7505FE 	17 	// 9411, MT7505, FE(Fast Ethernet)
#define tcPhyVer_7512Fe		18	// 9412, EN7512Fe, need to check R15(0xf)[13]=0: no Giga capability
#define tcPhyVer_7512Ge		19	// 9451, EN7512Ge
#define tcPhyVer_7522Fe		20	// 9414
#define tcPhyVer_7522Ge		21	// 9471	
#define tcPhyVer_7516Ge		22	// 9481	
#define tcPhyVer_7580Ge		23	// 9491
#define tcPhyVer_7528Ge		24	// 9491
#define tcPhyVer_7523Ge		25	// 94a1


static uint8 tcPhyPortNum = 4; 		// set value in tcPhyVerLookUp


#define EPHY_ID_2031		0x9400
#define	EPHY_ID_2101mb		0x9401
#define	EPHY_ID_2104mc		0x9402
#define	EPHY_ID_2104sd		0x9403
#define	EPHY_ID_2101me		0x9404
#define	EPHY_ID_2102me		0x9405	
#define	EPHY_ID_2104me		0x9406
#define	EPHY_ID_2101mf		0x9407
#define	EPHY_ID_2105sg		0x9408
#define	EPHY_ID_2101mi		0x940a
#define	EPHY_ID_2105mj		0x940b
#define	EPHY_ID_2105sk		0x940c
#define	EPHY_ID_2101mm		0x940e
#define	EPHY_ID_7510Fe		0x940f
#define	EPHY_ID_7505Fe		0x9411
#define	EPHY_ID_7530Ge_7512Fe	0x9412
#define	EPHY_ID_7510Ge		0x9421
#define	EPHY_ID_7512Ge		0x9451
#define	EPHY_ID_7522Fe		0x9414
#define	EPHY_ID_7522Ge		0x9471
#define	EPHY_ID_7516Ge		0x9481
#define	EPHY_ID_7580Ge		0x9491
#define	EPHY_ID_7528Ge		0x9491
#define	EPHY_ID_7523Ge		0x94a1


// for tcXXXX_link_state
#define ST_LINK_DOWN 	0
#define ST_LINK_DOWN2UP 1
#define ST_LINK_UP 		2
#define ST_LINK_UP2DOWN 3

static uint8 tcphy_link_state[TCPHY_PORTNUM];
static uint8 mtphy_link_state[MTPHY_PORTNUM];

// Auto, AN, 100F, 100H, 10F,  10H,
#ifdef TCPHY_1PORT
#define tcphy_speed_Auto  		0
#define tcphy_speed_ForceAN  	1
#define tcphy_speed_Force100F  	2
#define tcphy_speed_Force100H  	3
#define tcphy_speed_Force10F  	4
#define tcphy_speed_Force10H  	5
#endif

#ifdef TCPHY_1PORT
static uint8 tcphy_speed = tcphy_speed_Auto;
#endif
#endif 			// TCPHY_SUPPORT

#if defined( TCPHY_DEBUG) || defined(MTPHY_DEBUG)
// variable for doPing()
static uint32 PingReplyCnt = 0;
static uint8 phychkval_flag = 0;			// variable for doPhyChkVal()
static uint8 phychkval_portnum = 0;
// variable for doPhyForceLink()
static uint8 force_link_flag = 0;
// variables for doErrMonitor() / tcPhyErrMonitor()
static volatile uint8 err_monitor_flag = 0;
//static uint16 runt_cnt = 0;
//static uint16 tlcc_cnt = 0;
//static uint16 crc_cnt = 0;
//static uint16 long_cnt = 0;
//static uint16 loss_cnt = 0;
//static uint16 col_cnt = 0;
// variables for doPhyLoopback()
static volatile uint8 phy_loopback_flag = 0;
static volatile uint8 recv_ok_flag = 0;
static volatile uint8 recv_err_flag = 0;
#ifndef LINUX_OS
static volatile uint8 timeout_flag = 0;
#endif
static volatile uint8 timeout_cnt = 0;
#endif
#ifdef TCPHY_FIELD_DBGTEST
#define MAX_RECORD_TC_ETHER_PHY_STR_NUM 256
#define MAX_RECORD_TC_ETHER_PHY_STR_LEN 16
static uint32 msgcnt = 0;
char tc_ether_phy[MAX_RECORD_TC_ETHER_PHY_STR_NUM][MAX_RECORD_TC_ETHER_PHY_STR_LEN];
char RASVersion[3]="v4";
#endif

extern void macSend(uint32 chanId, struct sk_buff *skb);


/************************************************************************
*            Variables for EPHY  (4)
**************************************************************************/
//*** TCPHY registers ***//
#ifdef TCPHY_SUPPORT
// type for register settings
typedef struct cfg_data_s
{
    uint32 	reg_num;
    uint32 	val;
}cfg_data_t;
typedef struct cfg_cL45data_s
{
    uint32 	dev_num;
    uint32 	reg_num;
    uint32 	val;
}cfg_cl45data_t;
typedef struct cfg_trdata_s
{
    char 	reg_typ[10];
    uint32 	reg_num;
    uint32 	val;
}cfg_trdata_t;
// variables for doPhyConfig()
static uint8 	sw_patch_flag = 1;
static uint8 	Skew_update_flag = 0;
//static uint8 	EEE_DSP_Freeze_flag = 0;
//static uint8 	VBuffer_1000M_flag = 0;
//static uint32 	Skew_port_cnt = 0;
static uint8 	current_idx = 0; 				// default 0, Do NOT change 
//static uint8 	sw_ErrOverMonitor_flag = 1; 	// default enable
#ifdef PERIOD_PRINT 							// V1.17
static uint8 	period_print_flag = 1; 			// default enable
static uint16 	period_print_cnt = 0; 			// counter
static uint16 	period_print_threshold = 240; 	// print message every 0.25s*240=60s
#endif
//static uint8 	cfg_Tx10AmpSave_flag = 1; 	// default enable
//static uint8 	cfg_LDPS_flag = 1; 			// default enable
static uint8 	sw_FixUp2DownFast_flag = 1;
#endif 			// TCPHY_SUPPORT

#ifdef TCPHY_SUPPORT
// variables for doPhyDispFlag()
#ifdef TCPHY_DEBUG_DISP_LEVEL
uint8 tcPhy_disp_level = 2; 				// default level 2
#else
uint8 	tcPhy_disp_level = 0; 				// turn all message OFF for formal release
#endif

uint 	mtSkewCal_disp_level=0;
#if debug_flag
static uint8 	power_on_finish_flag = 0;  	//dai121019 indicate boot end
#endif

#ifdef LINUX_OS
#ifdef TCPHY_SUPPORT
extern int    	subcmd(const cmds_t tab[], int argc, char *argv[], void *p);
extern int    	cmd_register(cmds_t *cmds_p);
#define	RAND_MAX		32767
#endif
#endif //LINUX_OS

// tcPhy_disp_level = 1:min. 2:typ. 3:max. message
#define TCPHYDISP1 if(tcPhy_disp_level>=1) printf
#define TCPHYDISP2 if(tcPhy_disp_level>=2) printf

#if defined(TCPHY_DEBUG) || defined(MTPHY_DEBUG)
#define TCPHYDISP3 if(tcPhy_disp_level>=3) printf
#define TCPHYDISP4 if(tcPhy_disp_level>=4) printf
#define TCPHYDISP5 if(tcPhy_disp_level>=5) printf
#define TCPHYDISP6 if(tcPhy_disp_level>=6) printf
#else
#define TCPHYDISP3 if(0) printf
#define TCPHYDISP4 if(0) printf
#define TCPHYDISP5 if(0) printf
#define TCPHYDISP6 if(0) printf
#endif
#define SkewDISP1 if(mtSkewCal_disp_level>=1) printf
#endif

#ifdef TCPHY_SUPPORT
// variables for mii registers
typedef struct 
{
    uint8 main_reset; 		// 15
    uint8 force_speed; 		// 13
    uint8 autoneg_enable; 	// 12
    uint8 powerdown; 		// 11
    uint8 force_duplex; 	// 8
} tcphy_mr0_reg_t;

typedef struct 
{
    uint16 value; 				// 15:0
    //uint8 autoneg_complete; 	// 5
    bool link_status; 			// 2
    bool link_status_prev;
} tcphy_mr1_reg_t;

typedef struct 
{
    uint8 selector_field; 	// 4:0
    uint8 able100F; 		// 8
    uint8 able100H; 		// 7
    uint8 able10F; 			// 6
    uint8 able10H; 			// 5
} tcphy_mr4_reg_t; // use for mr4 & mr5

typedef struct 
{
    uint8 selector_field; 	// 4:0
    uint8 able100F; 		// 8
    uint8 able100H; 		// 7
    uint8 able10F; 			// 6
    uint8 able10H; 			// 5
    uint8 LPNextAble;		// 15
} tcphy_mr5_reg_t; // use for mr4 & mr5

typedef struct 
{
    //uint8 parallel_detect_fault; 	// 4
    uint8 lp_np_able; 				// 3
    //uint8 np_able; 				// 2
    //uint8 lch_page_rx; 			// 1
    uint8 lp_autoneg_able; 			// 0
} tcphy_mr6_reg_t; // use for mr6

typedef struct 
{
    //uint8  slicer_err_thd; 	// 15:11
    uint16 err_over_cnt;   		// 10:0
    uint16 err_over_cnt_prev; 	// 10:0
} tcphy_l0r25_reg_t;

typedef struct 
{
    //uint8  slicer_err_thd; 	// 15:11
    uint16 err_over_cnt;   		// 10:0
    uint16 err_over_cnt_prev; 	// 10:0
} mtphy_errovcnt_reg_t;

typedef struct 
{
    uint8 lch_sig_detect;		// 15
    uint8 lch_rx_linkpulse;		// 14
    uint8 lch_linkup_100;		// 13
    uint8 lch_linkup_10;		// 12
    uint8 lch_linkup_mdi;		// 11
    uint8 lch_linkup_mdix;		// 10
    uint8 lch_descr_lock;		// 9
    uint8 mdix_status;			// 5
    uint8 tx_amp_save;			// 4:3
    uint8 final_duplex;			// 2
    uint8 final_speed;			// 1
    uint8 final_link;   		// 0
} tcphy_l0r28_reg_t;

typedef struct 
{
    uint8 lch_SignalDetect;			// 15
    uint8 lch_LinkPulse;			// 14
    uint8 lch_DescramblerLock1000;	// 13
	uint8 lch_DescramblerLock100;	// 12
    uint8 lch_LinkStatus1000_OK;	// 11
	uint8 lch_LinkStatus100_OK;		// 10
	uint8 lch_LinkStatus10_OK;		// 9 
	uint8 lch_MrPageRx;				// 8
	uint8 lch_MrAutonegComplete;	// 7
  	uint8 da_mdix;					// 6
  	uint8 FullDuplexEnable;			// 5
  	uint8 MSConfig1000;				// 4
  	uint8 final_speed_1000;			// 3 
  	uint8 final_speed_100;			// 2
  	uint8 final_speed_10;			// 1
} tcphy_1ErA2_reg_t;

typedef struct 
{
    uint8 lp_eee_10g;
    uint8 lp_eee_1000;
    uint8 lp_eee_100;  
} tcphy_l3r18_reg_t;

typedef struct 
{
    uint8 lp_eee_10g;
    uint8 lp_eee_1000;
    uint8 lp_eee_100;  
} tcphy_7r3D_reg_t;

tcphy_mr0_reg_t 	mr0;
tcphy_mr4_reg_t 	mr4; 
tcphy_mr5_reg_t 	mr5;
tcphy_mr6_reg_t 	mr6;
tcphy_l0r28_reg_t 	mr28; 		// L0R28
tcphy_l3r18_reg_t 	mrl3_18; 	// L3R17
tcphy_1ErA2_reg_t 	mr1E_A2;
tcphy_7r3D_reg_t 	mr7_3D;
// for multiple phy support
tcphy_mr1_reg_t 		Nmr1[TCPHY_PORTNUM];
tcphy_l0r25_reg_t 		Nmr25[TCPHY_PORTNUM];
mtphy_errovcnt_reg_t 	ErovcntA[MTPHY_PORTNUM];
mtphy_errovcnt_reg_t 	ErovcntB[MTPHY_PORTNUM];
mtphy_errovcnt_reg_t 	ErovcntC[MTPHY_PORTNUM];
mtphy_errovcnt_reg_t 	ErovcntD[MTPHY_PORTNUM];
// define for tc???ReadProbe()
#define ProbeZfgain  					0
#define ProbeAgccode 					1
#define ProbeBoosten 					2
#define ProbeSnr     					3
#define ProbeDcoff   					4
#define ProbeAdcoff  					5
#define ProbeAdcSign 					6
#define Probe_VgaState 					7
#define Probe_MSE 						8
#define Probe_MSE_ALL 					9
#define Probe_MSE_Sum 					10
#define Probe_Slicer_Err_Over_Sum 		11
#define Probe_Slicer_Err_Over_Sum_ALL 	12
#define Probe_MSE_Slicer_err_thres 		13
//bool tcphy_anen = ENABLE;
//bool tcphy_speed = ENABLE;
//bool tcphy_duplex = ENABLE;

// define for tc??LinkFailDetect()
#define TbtOrHbt 		0
#define HbtOnly 		1
#define TbtOnly 		2
#define TbtOrHbtOrGbt 	3
#endif


/************************************************************************
*
*            Common Functions for EPHY  (5)
*
**************************************************************************/
int    phyTxAmpcomp(int show_or_comp);									  
int 	ePhyInit(uint16 ephy_addr);
void    normalinit(void);						 

#ifdef TCPHY_SUPPORT
uint32 	tcPhyReadReg(uint8 port_num,uint8 reg_num);
void 	tcPhyWriteReg(uint8 port_num,uint8 reg_num,uint32 reg_data);
uint32 	tcPhyReadLReg(uint8 port_num,uint8 page_num,uint8 reg_num);
void 	tcPhyWriteLReg(uint8 port_num,uint8 page_num,uint8 reg_num,uint32 reg_data);
uint32 	tcPhyReadGReg(uint8 port_num,uint8 page_num,uint8 reg_num);
void 	tcPhyWriteGReg(uint8 port_num,uint8 page_num,uint8 reg_num,uint32 reg_data);  
#ifdef LINUX_OS 
uint32 	mtPhyReadGReg(uint32 port_num, uint32 dev_num, uint32 reg_num);
uint32 	mtPhyReadReg(uint8 port_num, uint8 reg_num);
int 	mtPhyMiiWrite_TrDbg(uint8 phyaddr, char *type, uint32 reg ,uint32 val, uint8 ch_num);
int32 	mtPhyMiiRead_TrDbg(uint8 phyaddr, char *type, uint32 reg , uint8 ch_num);
//void 	mtPhyChkVal (void);
void 	mtMiiRegWrite(uint32 port_num, uint32 reg_num, uint32 reg_data);
uint32 	mtMiiRegRead(uint8 port_num,uint8 reg_num);
uint32 	tcMiiExtStationRead_CL22(uint32 port_num, uint32 dev_num, uint32 reg_num);
void 	tcMiiExtStationWrite_CL22(uint32 port_num,uint32 dev_num,uint32 reg_num,uint32 reg_data);
void 	mtEMiiRegWrite(uint32 port_num, uint32 dev_num, uint32 reg_num, uint32 reg_data);
uint32 	mtEMiiRegRead(uint32 port_num, uint32 dev_num, uint32 reg_num);
unsigned int buck_pbus_read( int phy, unsigned int register_address);
void buck_pbus_write( int phy, unsigned int register_address,unsigned int value);
void toKenRingWrite(unsigned int phy, unsigned int ch, unsigned int node, unsigned int data_addr,unsigned int value);
unsigned int toKenRingRead(unsigned int phy, unsigned int ch, unsigned int node, unsigned int data_addr);
#endif
void mtMiiRegWrite(uint32 port_num, uint32 reg_num, uint32 reg_data)
{
	tcMiiStationWrite(port_num, reg_num, reg_data);
}
uint32 mtMiiRegRead(uint8 port_num,uint8 reg_num)
{
	return(tcMiiStationRead(port_num, reg_num));
}
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CT_LOOPDETECT) || !(!defined(TCSUPPORT_AUTOBENCH) || !defined(TCSUPPORT_CPU_MT7520))
EXPORT_SYMBOL(mtMiiRegWrite);
EXPORT_SYMBOL(mtMiiRegRead);
#endif/*TCSUPPORT_COMPILE*/
uint32 tcMiiExtStationRead_CL22(uint32 port_num, uint32 dev_num, uint32 reg_num)
{
    const uint16 MMD_Control_register=0xD;  	// 0xd=13
    const uint16 MMD_addr_data_register=0xE;	// 0xe=14	
  	const uint16 page_reg=31;
	uint32 value=0;
	tcMiiStationWrite(port_num, page_reg, 0x00); //switch to main page
	tcMiiStationWrite(port_num, MMD_Control_register, (0<<14)+dev_num);
	tcMiiStationWrite(port_num, MMD_addr_data_register, reg_num);
	tcMiiStationWrite(port_num, MMD_Control_register, (1<<14)+dev_num);
	value = tcMiiStationRead(port_num, MMD_addr_data_register);
	//printf("* doPhyMMDRead_CL22=>phyaddr=%d,  dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", port_num, dev_num, reg_num, value);
	return(value); 	
}
uint32 mtEMiiRegRead(uint32 port_num, uint32 dev_num, uint32 reg_num)
{
	return(tcMiiExtStationRead_CL22( port_num, dev_num, reg_num));
}
void tcMiiExtStationWrite_CL22(uint32 port_num,uint32 dev_num,uint32 reg_num,uint32 reg_data)
{
    const uint16 MMD_Control_register=0xD;
    const uint16 MMD_addr_data_register=0xE;
  	const uint16 page_reg=31;
	tcMiiStationWrite(port_num, page_reg, 0x00); //switch to main page
	tcMiiStationWrite(port_num, MMD_Control_register, (0<<14)+dev_num);
	tcMiiStationWrite(port_num, MMD_addr_data_register, reg_num);
	tcMiiStationWrite(port_num, MMD_Control_register, (1<<14)+dev_num);
	tcMiiStationWrite(port_num, MMD_addr_data_register, reg_data);
	TCPHYDISP3("* doPhyMMDWrite_CL22=> phyaddr=%d, dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", port_num, dev_num, reg_num, reg_data);
}
void mtEMiiRegWrite(uint32 port_num, uint32 dev_num, uint32 reg_num, uint32 reg_data)
{
	tcMiiExtStationWrite_CL22( port_num, dev_num, reg_num, reg_data);
}

unsigned int toKenRingRead(unsigned int phy, unsigned int ch, unsigned int node, unsigned int data_addr)
{
	unsigned int data,last_page,high_byte,low_byte;
	last_page=tcMiiStationRead(phy, 0x1f);
	tcMiiStationWrite(phy, 0x1f, 0x52b5);
	data=0xa000|(ch<<11)|(node<<7)|(data_addr<<1);
	tcMiiStationWrite(phy, 0x10, data);
	low_byte=tcMiiStationRead(phy, 0x11);
	high_byte=tcMiiStationRead(phy, 0x12);
	tcMiiStationWrite(phy, 0x1f, last_page);
	data=(high_byte*65536)+low_byte;
	return data;
}

void toKenRingWrite(unsigned int phy, unsigned int ch, unsigned int node, unsigned int data_addr,unsigned int value)
{
	unsigned int data,last_page;
	last_page=tcMiiStationRead(phy, 0x1f);
	tcMiiStationWrite(phy, 0x1f, 0x52b5);
	data=value&0xffff;
	tcMiiStationWrite(phy, 0x11, data);
	data=(value&0xffff0000)/65536;
	tcMiiStationWrite(phy, 0x12, data);
	data=0x8000|(ch<<11)|(node<<7)|(data_addr<<1);
	tcMiiStationWrite(phy, 0x10, data);
	tcMiiStationWrite(phy, 0x1f, last_page);
}

unsigned int buck_pbus_read( int phy, unsigned int register_address)
{
	unsigned int last_page,data,high_byte,low_byte;


		last_page=tcMiiStationRead(phy, 0x1f);
		tcMiiStationWrite(phy, 0x1f, 0x0004);
		tcMiiStationWrite(phy, 0x10, 0x0000);
		data=(register_address&0xffff0000)>>16;
        tcMiiStationWrite(phy, 0x15, data);
		data=register_address&0xffff;
        tcMiiStationWrite(phy, 0x16, data);
		high_byte=tcMiiStationRead(phy, 0x17);
		low_byte=tcMiiStationRead(phy, 0x18);
		tcMiiStationWrite(phy, 0x1f,last_page);
		data=(high_byte<<16)+low_byte;
		return data;
}

void buck_pbus_write( int phy, unsigned int register_address,unsigned int value)
{
	unsigned int last_page,data;


		last_page=tcMiiStationRead(phy, 0x1f);
		tcMiiStationWrite(phy, 0x1f, 0x0004);
		tcMiiStationWrite(phy, 0x10, 0x0000);
		data=(register_address&0xffff0000)>>16;
        tcMiiStationWrite(phy, 0x11, data);
		data=register_address&0xffff;
        tcMiiStationWrite(phy, 0x12, data);
		data=(value&0xffff0000)>>16;
        tcMiiStationWrite(phy, 0x13, data);
		data=value&0xffff;
        tcMiiStationWrite(phy, 0x14, data);
		tcMiiStationWrite(phy, 0x1f, last_page);
		
}


#if defined(TCSUPPORT_AUTOBENCH) && defined(TCSUPPORT_CPU_MT7520)
EXPORT_SYMBOL(mtEMiiRegWrite);
EXPORT_SYMBOL(mtEMiiRegRead);
#endif
#ifdef LINUX_OS // allen_20130926 : merge 7502 & 7510/20

uint32 mtPhyReadReg(uint8 port_num, uint8 reg_num)  // for GEPHY
{
    uint32 val, val_r31;
    uint32 phyAddr;
	
	if(ephy_addr_base == 0)
	{
		if((tcPhyVer == tcPhyVer_7523Ge))	
			ephy_addr_base += EPHY_ADDR_P0;
	}
		
	phyAddr = ephy_addr_base + port_num;
    tcMiiStationWrite(phyAddr, 31, 0);
     
    if (tcPhyVer!=tcPhyVer_2031 && (reg_num<16 || reg_num==31))
	{     
        val = tcMiiStationRead(phyAddr, reg_num); 
        //printf("mtPhyReadReg:phyAddr =%d, reg_num =0x%02lX, val =0x%04lx\r\n",phyAddr,reg_num,val);
    }
    else
	{
        val_r31 = tcMiiStationRead(phyAddr, 31); // remember last page
        // set page to L0 if necessary
        if (val_r31 != 0x8000) 
		{
            tcMiiStationWrite(phyAddr, 31, 0x8000);
        }
        // read reg
        val = tcMiiStationRead(phyAddr, reg_num); 
        // restore page if necessary
        if (val_r31 != 0x8000) 
		{
            tcMiiStationWrite(phyAddr, 31, val_r31);
        }
    }

    // update variables
    switch(reg_num){
        
    case 0:
        mr0.main_reset = (val>>15)&0x00000001;
        mr0.force_speed    = (val>>13)&0x00000001;
        mr0.autoneg_enable = (val>>12)&0x00000001;
        mr0.powerdown      = (val>>11)&0x00000001;
        mr0.force_duplex   = (val>>8)&0x00000001;
        break;
        
    case 1:
        //mr1.autoneg_complete = (val>>5)&0x00000001;       
        //mr1_link_status_reg = val;
        //mr1.value = val;
        //mr1.link_status_prev = mr1.link_status; 
        //mr1.link_status = (val>>2)&0x00000001;
        Nmr1[port_num].value = val;
        Nmr1[port_num].link_status_prev = Nmr1[port_num].link_status;
        Nmr1[port_num].link_status = (val>>2)&0x00000001;
        //if(Nmr1[port_num].link_status !=  Nmr1[port_num].link_status_prev)
		//{
	    //    printf("mtPhyReadReg case 1 :Nmr1[%d].value =0x%04lx\r\n", port_num,Nmr1[port_num].value);
    	//    printf("mtPhyReadReg case 1 :Nmr1[%d].link_status_prev =0x%04lx\r\n",port_num,Nmr1[port_num].link_status_prev );
        //	printf("mtPhyReadReg case 1 :Nmr1[%d].link_status =0x%04lx\r\n", port_num,Nmr1[port_num].link_status);
        //}
        break;

    case 4:
        mr4.able100F = (val>>8)&0x0001;     
        mr4.able100H = (val>>7)&0x0001;     
        mr4.able10F = (val>>6)&0x0001;  
        mr4.able10H = (val>>5)&0x0001;  
        mr4.selector_field = (val)&0x001f;
        break;

    case 5:
        mr5.able100F = (val>>8)&0x0001;     
        mr5.able100H = (val>>7)&0x0001;     
        mr5.able10F = (val>>6)&0x0001;  
        mr5.able10H = (val>>5)&0x0001;  
        mr5.selector_field = (val)&0x001f;
		mr5.LPNextAble = (val>>15)&0x0001;
        break;
        
    case 6:     
        mr6.lp_np_able = (val>>3)&0x0001;
        mr6.lp_autoneg_able = (val)&0x0001;
        break;
        
    default:
        break;      
    }   
    return (val);
}
#endif

uint32 tcPhyReadReg(uint8 port_num, uint8 reg_num)  // for FEPHY
{
    uint32 val, val_r31;
    uint32 phyAddr;

	if(ephy_addr_base == 0) 
	{
		if((tcPhyVer == tcPhyVer_7523Ge))	
			ephy_addr_base += EPHY_ADDR_P0;
	}
	phyAddr = ephy_addr_base + port_num;

    if (tcPhyVer!=tcPhyVer_2031 && (reg_num<16 || reg_num==31))
    {
        val = tcMiiStationRead(phyAddr, reg_num); 
    }
    else
    {
        val_r31 = tcMiiStationRead(phyAddr, 31); // remember last page
        // set page to L0 if necessary
        if (val_r31 != 0x8000) 
        {
            tcMiiStationWrite(phyAddr, 31, 0x8000);
        }
        // read reg
        val = tcMiiStationRead(phyAddr, reg_num); 
        // restore page if necessary
        if (val_r31 != 0x8000) 
        {
            tcMiiStationWrite(phyAddr, 31, val_r31);
        }
    }

    // update variables
    switch(reg_num)
    {    
    	case 0:
        	mr0.main_reset 		= (val>>15)&0x00000001;
        	mr0.force_speed    	= (val>>13)&0x00000001;
        	mr0.autoneg_enable 	= (val>>12)&0x00000001;
        	mr0.powerdown      	= (val>>11)&0x00000001;
        	mr0.force_duplex   	= (val>>8)&0x00000001;
        	break;
        
    	case 1:
        	//mr1.autoneg_complete = (val>>5)&0x00000001;       
        	//mr1_link_status_reg = val;
        	//mr1.value = val;
        	//mr1.link_status_prev = mr1.link_status; 
        	//mr1.link_status = (val>>2)&0x00000001;
        	Nmr1[port_num].value 			= val;
        	Nmr1[port_num].link_status_prev = Nmr1[port_num].link_status;
        	Nmr1[port_num].link_status 		= (val>>2)&0x00000001;
        	break;

    	case 4:
        	mr4.able100F 		= (val>>8)&0x0001;     
        	mr4.able100H 		= (val>>7)&0x0001;     
        	mr4.able10F 		= (val>>6)&0x0001;  
        	mr4.able10H 		= (val>>5)&0x0001;  
        	mr4.selector_field 	= (val)&0x001f;
        	break;

    	case 5:
        	mr5.able100F 		= (val>>8)&0x0001;     
        	mr5.able100H 		= (val>>7)&0x0001;     
        	mr5.able10F 		= (val>>6)&0x0001;  
        	mr5.able10H 		= (val>>5)&0x0001;  
        	mr5.selector_field 	= (val)&0x001f;
			mr5.LPNextAble 		= (val>>15)&0x0001;
        	break;
        
    	case 6:     
        	mr6.lp_np_able 		= (val>>3)&0x0001;
        	mr6.lp_autoneg_able = (val)&0x0001;
        	break;
        
	    case 25:           
        	Nmr25[port_num].err_over_cnt_prev 	= Nmr25[port_num].err_over_cnt;
        	Nmr25[port_num].err_over_cnt		= (val & 0x0000007ff);
        	break;
        
    	case 28:
        	mr28.lch_sig_detect  	= (val>>15)&0x0001;
        	mr28.lch_rx_linkpulse	= (val>>14)&0x0001;
        	mr28.lch_linkup_100  	= (val>>13)&0x0001;
        	mr28.lch_linkup_10   	= (val>>12)&0x0001;
        	mr28.lch_linkup_mdi  	= (val>>11)&0x0001; 	// after LEM
        	mr28.lch_linkup_mdix 	= (val>>10)&0x0001; 	// after LEM
        	mr28.lch_descr_lock  	= (val>>9)&0x0001; 		// after LEM
        	mr28.mdix_status  		= (val>>5)&0x0001; 		/* {0:mdi,1:mdix} */   
        	mr28.tx_amp_save  		= (val>>3)&0x0003; 		/* 0:100%, 1:90%, 2:80%, 3:70% */
        	mr28.final_duplex 		= (val>>2)&0x0001; 		/* {0:half-duplex, 1:full-duplex} */
        	mr28.final_speed  		= (val>>1)&0x0001; 		/* {0:10, 1:100} */
        	mr28.final_link			= (val)&0x0001; 		/* {0:linkdown, 1:linkup} */      
        	break;
        
    	default:
        	break;      
    }   
    return (val);
}

void tcPhyWriteReg(uint8 port_num,uint8 reg_num,uint32 reg_data)
{
    uint32 val_r31;
    uint32 phyAddr;

	if(ephy_addr_base == 0) // allen_20140822
	{
		if((tcPhyVer == tcPhyVer_7523Ge))	
			ephy_addr_base += EPHY_ADDR_P0;
	}
	phyAddr = ephy_addr_base + port_num;

    val_r31 = tcMiiStationRead(phyAddr, 31); // remember last page
    // set page if necessary
    if (val_r31 != 0x8000) 
    {
        tcMiiStationWrite(phyAddr, 31, 0x8000); // page to L0
    }
    tcMiiStationWrite(phyAddr, reg_num, reg_data); 
    // restore page if necessary
    if (val_r31 != 0x8000) 
    {
        tcMiiStationWrite(phyAddr, 31, val_r31);
    }
}

// read Local Reg
uint32 tcPhyReadLReg(uint8 port_num,uint8 page_num,uint8 reg_num)
{
    uint32 val, val_r31;
    uint32 phyAddr;
    uint32 pageAddr = (page_num<<12)+0x8000;

	if(ephy_addr_base == 0) // allen_20140822
	{
		if((tcPhyVer == tcPhyVer_7523Ge))	
			ephy_addr_base += EPHY_ADDR_P0;
	}
	phyAddr = ephy_addr_base + port_num;

    val_r31 = tcMiiStationRead(phyAddr, 31);  // remember last page
    // set page if necessary
    if (val_r31 != pageAddr) 
	{
        tcMiiStationWrite(phyAddr, 31, pageAddr); // switch to page Lx
    }
    val = tcMiiStationRead(phyAddr, reg_num); 
    // restore page if necessary
    if (val_r31 != pageAddr) 
	{
        tcMiiStationWrite(phyAddr, 31, val_r31);
    }
	
    if (page_num==3) 
	{
		switch(reg_num)
		{
			case 18:
               mrl3_18.lp_eee_10g = (val>>3)&0x0001;
               mrl3_18.lp_eee_1000 = (val>>2)&0x0001;
               mrl3_18.lp_eee_100 = (val>>1)&0x0001;
               break;
			default:
               break; 
		}
    }
    return val;
}

// write Local Reg
void tcPhyWriteLReg(uint8 port_num,uint8 page_num,uint8 reg_num,uint32 reg_data)
{
    uint32 val_r31;
    uint32 phyAddr;
    uint32 pageAddr = (page_num<<12)+0x8000;

	if(ephy_addr_base == 0) // allen_20140822
	{
		if((tcPhyVer == tcPhyVer_7523Ge))	
			ephy_addr_base += EPHY_ADDR_P0;
	}
	phyAddr = ephy_addr_base + port_num;

    val_r31 = tcMiiStationRead(phyAddr, 31);  // remember last page
    // set page if necessary
    if (val_r31 != pageAddr) 
	{
        tcMiiStationWrite(phyAddr, 31, pageAddr); // switch to page Lx  
    }
    tcMiiStationWrite(phyAddr, reg_num, reg_data); 
    // restore page if necessary
    if (val_r31 != pageAddr) 
	{
        tcMiiStationWrite(phyAddr, 31, val_r31);
    }
}

// read Global Reg
uint32 tcPhyReadGReg(uint8 port_num,uint8 page_num,uint8 reg_num)
{
    uint32 val, val_r31;
    uint32 phyAddr;
    uint32 pageAddr = (page_num<<12);

	if(ephy_addr_base == 0) // allen_20140822
	{
		if((tcPhyVer == tcPhyVer_7523Ge))	
			ephy_addr_base += EPHY_ADDR_P0;
	}
	phyAddr = ephy_addr_base + port_num;

    val_r31 = tcMiiStationRead(phyAddr, 31);  // remember last page
    // set page if necessary
    if (val_r31 != pageAddr) 
	{
        tcMiiStationWrite(phyAddr, 31, pageAddr); // switch to page Gx  
    }
    val = tcMiiStationRead(phyAddr, reg_num); 
    // restore page if necessary
    if (val_r31 != pageAddr) 
	{
        tcMiiStationWrite(phyAddr, 31, val_r31);
    }
    
    return val;
}

// write Global Reg
void tcPhyWriteGReg(uint8 port_num,uint8 page_num,uint8 reg_num,uint32 reg_data)
{
    uint32 val_r31;
    uint32 phyAddr;
    uint32 pageAddr = (page_num<<12);

	if(ephy_addr_base == 0) // allen_20140822
	{
		if((tcPhyVer == tcPhyVer_7523Ge))	
			ephy_addr_base += EPHY_ADDR_P0;
	}
	phyAddr = ephy_addr_base + port_num;

    val_r31 = tcMiiStationRead(phyAddr, 31);  // remember last page
    // set page if necessary
    if (val_r31 != pageAddr) 
	{
        tcMiiStationWrite(phyAddr, 31, pageAddr); // switch to page Gx
    }
    tcMiiStationWrite(phyAddr, reg_num, reg_data); 
    // restore page if necessary
    if (val_r31 != pageAddr) 
	{
        tcMiiStationWrite(phyAddr, 31, val_r31);
    }
}

#ifdef LINUX_OS // allen_20130926 : merge 7502 & 7510/20
uint32 mtPhyReadGReg(uint32 port_num, uint32 dev_num, uint32 reg_num)
{
    uint32 val;
    
    val = mtEMiiRegRead(port_num, dev_num, reg_num);

    // update variables
    switch(reg_num)
    {
	case 0xA2:
    	mr1E_A2.lch_SignalDetect  = (val>>15)&0x0001;
        mr1E_A2.lch_LinkPulse= (val>>14)&0x0001;
        mr1E_A2.lch_DescramblerLock1000  = (val>>13)&0x0001;
        mr1E_A2.lch_DescramblerLock100   = (val>>12)&0x0001;
        mr1E_A2.lch_LinkStatus1000_OK  = (val>>11)&0x0001;
        mr1E_A2.lch_LinkStatus100_OK = (val>>10)&0x0001; 
        mr1E_A2.lch_LinkStatus10_OK  = (val>>9)&0x0001; 
        mr1E_A2.lch_MrPageRx  = (val>>8)&0x0001; 
        mr1E_A2.lch_MrAutonegComplete  = (val>>7)&0x0001; 
        mr1E_A2.da_mdix  = (val>>6)&0x0001; 
        mr1E_A2.FullDuplexEnable  = (val>>5)&0x0001;    
        mr1E_A2.MSConfig1000  = (val>>4)&0x0001; 
        mr1E_A2.final_speed_1000 = (val>>3)&0x0001; 
        mr1E_A2.final_speed_100  = (val>>2)&0x0001; 
        mr1E_A2.final_speed_10   = (val>>1)&0x0001;       
        break;
        
	case 0x3d:
		mr7_3D.lp_eee_100=(val>>1)&0x0001;
		mr7_3D.lp_eee_1000=(val>>2)&0x0001;
		mr7_3D.lp_eee_10g=(val>>3)&0x0001;
		break;

		//printf("kant_7dev_3d = 0x%x \n\r", val);
		
    default:
        break;      
    }   
    return (val);
}
#endif
#endif		// TCPHY_SUPPORT


#ifdef LINUX_OS // allen_20130926 : merge 7502 & 7510/20
int32 mtPhyMiiRead_TrDbg(uint8 phyaddr, char *type, uint32 data_addr , uint8 ch_num)
{
    const uint16 page_reg=31;
    const uint32 Token_Ring_debug_reg=0x52B5;
    const uint32 Token_Ring_Control_reg=0x10;
    const uint32 Token_Ring_Low_data_reg=0x11;
    const uint32 Token_Ring_High_data_reg=0x12;

    uint16 ch_addr=0;
    uint32 node_addr=0;

    uint32 value=0;
    uint32 value_high=0;
    uint32 value_low=0;
    
  	if(stricmp(type, "DSPF") == 0)
  	{	// DSP Filter Debug Node
        ch_addr = 0x02;
		node_addr = 0x0D;
	}
	else if(stricmp(type, "PMA") == 0)
	{ 	// PMA Debug Node 
		ch_addr = 0x01;
		node_addr = 0x0F;
	}
	else if(stricmp(type, "TR") == 0)
	{ 	// Timing Recovery  Debug Node 
		ch_addr=0x01;
		node_addr=0x0D;
	}
	else if(stricmp(type, "PCS") == 0)
	{ 	// R1000PCS Debug Node 
		ch_addr=0x02;
		node_addr=0x0F;
	}
	else if(stricmp(type, "FFE") == 0)
	{ 	// FFE Debug Node 
		ch_addr=ch_num;
		node_addr=0x04;
	}
	else if(stricmp(type, "EC") == 0)
	{ 	// ECC Debug Node 
		ch_addr=ch_num;
		node_addr=0x00;
	}
	else if(stricmp(type, "ECT") == 0)
	{ 	// EC/Tail Debug Node 
		ch_addr=ch_num;
		node_addr=0x01;
	}
	else if(stricmp(type, "NC") == 0)
	{ 	// EC/NC Debug Node 
		ch_addr=ch_num;
		node_addr=0x01;
	}
	else if(stricmp(type, "DFEDC") == 0)
	{ 	// DFETail/DC Debug Node 
		ch_addr=ch_num;
		node_addr=0x05;
	}
	else if(stricmp(type, "DEC") == 0)
	{ 	// R1000DEC Debug Node 
		ch_addr=0x00; 
		node_addr=0x07;
	}
	else if(stricmp(type, "CRC") == 0)
	{ 	// R1000CRC Debug Node 
		ch_addr=ch_num;
		node_addr=0x06;
	}
	else if(stricmp(type, "AN") == 0)
	{ 	// Autoneg Debug Node 
		ch_addr=0x00; 
		node_addr=0x0F;
	}
	else if(stricmp(type, "CMI") == 0)
	{ 	// CMI Debug Node 
		ch_addr=0x03; 
		node_addr=0x0F;
	}
	else if(stricmp(type, "SUPV") == 0)
	{ 	// SUPV PHY  Debug Node 
		ch_addr=0x00; 
		node_addr=0x0D;
	}
	else
	{
    	return(0xFFFF);
		printf("Wrong TR register Type !");
	}
	data_addr=data_addr&0x3F;
      
	tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
	tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(ch_addr<<11)|(node_addr<<7)|(data_addr<<1));

	//while(!(tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)); // data ready
	{
		value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);
		value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);
		value=value_low+((value_high&0x00FF)<<16);
		TCPHYDISP4("*%s => Phyaddr=%d, ch_addr=%d, node_addr=0x%02lX, data_addr=0x%02lX , value=0x%08lX\r\n", type,phyaddr, ch_addr, node_addr, data_addr, value);
	}
	tcMiiStationWrite(phyaddr, page_reg, 0x00);//V1.11
		
	return(value);
}

int mtPhyMiiWrite_TrDbg(uint8 phyaddr, char *type, uint32 data_addr ,uint32 value, uint8 ch_num)
{
    const uint16 page_reg=31;
    const uint32 Token_Ring_debug_reg=0x52B5;
    const uint32 Token_Ring_Control_reg=0x10;
    const uint32 Token_Ring_Low_data_reg=0x11;
    const uint32 Token_Ring_High_data_reg=0x12;
	
    uint16 ch_addr=0;
    uint32 node_addr=0;
    uint32 value_high=0;
    uint32 value_low=0;
	
	if(stricmp(type, "DSPF") == 0)
	{	// DSP Filter Debug Node
		ch_addr=0x02;
		node_addr=0x0D;
	}
	else if(stricmp(type, "PMA") == 0)
	{ 	// PMA Debug Node 
		ch_addr=0x01;
		node_addr=0x0F;
	}
	else if(stricmp(type, "TR") == 0)
	{ 	// Timing Recovery	Debug Node 
		ch_addr=0x01;
		node_addr=0x0D;
	}
	else if(stricmp(type, "PCS") == 0)
	{ 	// R1000PCS Debug Node 
		ch_addr=0x02;
		node_addr=0x0F;
	}
	else if(stricmp(type, "FFE") == 0)
	{ 	// FFE Debug Node 
		ch_addr=ch_num;
		node_addr=0x04;
	}
	else if(stricmp(type, "EC") == 0)
	{ 	// ECC Debug Node 
		ch_addr=ch_num;
		node_addr=0x00;
	}
	else if(stricmp(type, "ECT") == 0)
	{ 	// EC/Tail Debug Node 
		ch_addr=ch_num;
		node_addr=0x01;
	}
	else if(stricmp(type, "NC") == 0)
	{ 	// EC/NC Debug Node 
		ch_addr=ch_num;
		node_addr=0x01;
	}
	else if(stricmp(type, "DFEDC") == 0)
	{ 	// DFETail/DC Debug Node 
		ch_addr=ch_num;
		node_addr=0x05;
	}
	else if(stricmp(type, "DEC") == 0)
	{ 	// R1000DEC Debug Node 
		ch_addr=0x00; 
		node_addr=0x07;
	}
	else if(stricmp(type, "CRC") == 0)
	{ 	// R1000CRC Debug Node 
		ch_addr=ch_num;
		node_addr=0x06;
	}
	else if(stricmp(type, "AN") == 0)
	{ 	// Autoneg Debug Node 
		ch_addr=0x00; 
		node_addr=0x0F;
	}
	else if(stricmp(type, "CMI") == 0)
	{ 	// CMI Debug Node 
		ch_addr=0x03; 
		node_addr=0x0F;
	}
	else if(stricmp(type, "SUPV") == 0)
	{ 	// SUPV PHY  Debug Node 
		ch_addr=0x00; 
		node_addr=0x0D;
	}
	   
	data_addr=data_addr&0x3F;
	value_high=(0x00FF0000&value)>>16;
	value_low=(0x0000FFFF&value);
   
	tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
		   
	tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
	tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
	tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(ch_addr<<11)|(node_addr<<7)|(data_addr<<1));
	//while(!(tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)); 			// data ready
	{
		TCPHYDISP4("*%s => Phyaddr=%d, ch_addr=%d, node_addr=0x%02lX, data_addr=0x%02lX , value=0x%08lX\r\n", type,phyaddr, ch_addr, node_addr, data_addr, value);
	}
	tcMiiStationWrite(phyaddr, page_reg, 0x00);			//V1.11
	return 0;
}
#endif


#if 1
/************************************************************************
*
*            Variables and Functions for 7523 GE PHY (ID=0x???)
*
**************************************************************************/
#ifdef LINUX_OS 
#define EN75xxGe_PORTNUM 				5		
#define EN75xxGe_PHY_INIT_LDATA_LEN 	4
#define EN75xxGe_PHY_INIT_PERPDATA_LEN 	0
#define EN75xxGe_INIT_SET_NUM 			1
#define EN75xxGe_PHY_INIT_SET_NUM 		1

#define EN75xxGe_PHY_INIT_CL45_GDATA_LEN 	14 	
#define EN75xxGe_PHY_INIT_CL45_LDATA_LEN 	47 
#define EN75xxGe_PHY_INIT_CL45_PERPDATA_LEN 0
#define EN75xxGe_PHY_INIT_CL45_SET_NUM 		1

#define EN75xxGe_PHY_INIT_TR_LDATA_LEN 		20
#define EN75xxGe_PHY_INIT_TR_PERPDATA_LEN 	1
#define EN75xxGe_PHY_INIT_TR_SET_NUM 		1

typedef struct en75xxGe_cfg_data_s
{
	char 		name[10];
	cfg_data_t 	ldata[EN75xxGe_PHY_INIT_LDATA_LEN];
	cfg_data_t 	perpdata[EN75xxGe_PHY_INIT_PERPDATA_LEN];			// per port register setting	
}en75xxGe_cfg_data_t;

typedef struct en75xxGe_cfg_cl45data_s
{
	cfg_cl45data_t gdata[EN75xxGe_PHY_INIT_CL45_GDATA_LEN];
	cfg_cl45data_t ldata[EN75xxGe_PHY_INIT_CL45_LDATA_LEN];
	cfg_cl45data_t perpdata[EN75xxGe_PHY_INIT_CL45_PERPDATA_LEN];	// per port register setting	
}en75xxGe_cfg_cl45data_t;

typedef struct en75xxGe_cfg_trdata_s
{
	cfg_trdata_t ldata[EN75xxGe_PHY_INIT_TR_LDATA_LEN];
	cfg_trdata_t perpdata[EN75xxGe_PHY_INIT_PERPDATA_LEN];			// per port register setting	
}en75xxGe_cfg_trdata_t;

const uint32 en75xxGe_page_sel_addr = 31;

en75xxGe_cfg_data_t en75xxGe_cfg[EN75xxGe_PHY_INIT_SET_NUM]=
{
	{
		{"Ge_1.0"}, 
    	//local data	
     	{ 	
       		{0x1f, 0x0003},
       		{0x1c, 0x0c92},
       		{0x1d, 0x0001},
       		{0x1f, 0x0000}
     	},
     	
    	//per-port data 
	    { 
	 	}
    },
};

en75xxGe_cfg_cl45data_t en75xxGe_cfg_cl45[EN75xxGe_PHY_INIT_CL45_SET_NUM]=
{
    {	//globle data
     	{
			{0x1f, 0x0273, 0x1000}, 				// i2mpb_tbh_ots
			{0x1f, 0x0272, 0x0c6b},					// ps_op 0x3cff, for PHYD bug, need to workaround 

			{0x1f, 0x0268, 0x07f4},					
			{0x1f, 0x0269, 0x1114},					// ps_dri  //0x4344
			{0x1f, 0x0271, 0x2ca5},					// 0x4e13, for PHYD bug, need to workaround
			{0x1f, 0x0044, 0x00a0},					// pair delay (digital)

			{0x1f, 0x027c, 0x0808},					// 10 base Tx
			{0x1f, 0x027b, 0x1177},					// disable 10 base-Te
			
			{0x1f, 0x0417, 0x7775},
			{0x1f, 0x0024, 0xc007},					// gphy led0
			{0x1f, 0x0025, 0x003f},					// gphy led0  // 0x003f
			{0x1f, 0x0026, 0xc007},					// gphy led1
			{0x1f, 0x0027, 0x003f},					// gphy led1
			{0x1f, 0x0021, 0x800a},					// gphy led


			
			//{0x1f, 0x027b, 0x1177}   				// (10baseT Tx gain for IOT, if tx no EEE, rx only)
     	},
     	
     	//local data
     	{
			{0x1e,0x0000,0x0187},					// 0->1, middle
			{0x1e,0x0001,0x01c9},					// 0->1, ov1
			{0x1e,0x0002,0x01c0},					// 0->1, ov2, z2p_ovs
			{0x1e,0x0003,0x0100},					// 1->0, middle
			{0x1e,0x0004,0x0210},					// 1->0, ov1
			{0x1e,0x0005,0x0200},					// 1->0, ov2, p2z_ovs

			{0x1e,0x0006,0x0384},					// 0->-1, middle
			{0x1e,0x0007,0x03c8},					// 0->-1, ov1
			{0x1e,0x0008,0x03c0},					// 0->-1, ov2, z2n_ovs
			{0x1e,0x0009,0x030a},					// -1->0, middle
			{0x1e,0x000a,0x0080},					// -1->0, ov1
			{0x1e,0x000b,0x0208},					// -1->0, ov2, n2z_ovs
            {0x1e,0x0011,0xff00},
			


			
			{0x1e,0x0013,0x0000},
			{0x1e,0x0014,0x0000},					// pair delay (afe)
			{0x1e,0x0044,0x0000},
			{0x1e,0x0176,0x5500},
			{0x1e,0x0177,0x0055},

			{0x1e,0x0041,0x3333},					// hvga_bias : enhance rx voltage
			{0x1e,0x0040,0x0000},					// gain down (dB); 0x00:0dB, 0x01:4dB  // allen_20180917 no rx gain down for IOT performance
			{0x1e,0x0201,0x4000},					// txvld disable, 1e_201[7:0]=0, disable voltage mode when 100_EEE 
			{0x1e,0x003d,0x0000},					// RX vbuffer bypass pwd
			{0x1e,0x0198,0x0001},					// allen_20180530, TX vld bypass pwd
			{0x1e,0x003e,0xc000},					// allen_20180530, TX vld bypass pwd (pair A/B for EEE)
			{0x1e,0x023c,0x0a20},					// allen_20180530, EEE wake up cnt from 0x14 to 0x20
			
			{0x1e,0x01a3,0x00d2},					// for 10M TP-IDL	// allen_20180815
			{0x1e,0x01a4,0x010e},					// for 10M TP-IDL	// allen_20180815
			
			{0x1e,0x0189,0x0110},
			{0x1e,0x0122,0xffff},
			{0x1e,0x0123,0xffff},
			{0x1e,0x0234,0x1180},
			{0x1e,0x0238,0x0120},
			{0x1e,0x0120,0x8014},
			{0x1e,0x0239,0x0117},
			{0x1e,0x014a,0xee20},
			{0x1e,0x019b,0x0111},
			{0x1e,0x0147,0x0000},
			{0x1e,0x02d1,0x0733},
			{0x1e,0x0236,0x0020},
			{0x1e,0x0144,0x0200},
			{0x1e,0x0323,0x0011},
			{0x1e,0x0324,0x013f},
			{0x1e,0x0326,0x0037},
			
			{0x1e,0x0190,0x0110},
			{0x1e,0x0191,0x4444},					// hvga_rsel 	3b'100
			{0x1e,0x00a6,0x0350},
			{0x07,0x003c,0x0006}	
	     },

     	//per-port data
     	{
	 	}
    }, 
};

en75xxGe_cfg_trdata_t en75xxGe_cfg_tr[EN75xxGe_PHY_INIT_TR_SET_NUM]=
{
    {	//local data
     	{ 
			{"TR", 0x26,0x444444},
			{"PMA",0x00,0x00001e},  				// enhance rx 
			{"PMA",0x01,0x6fb90a},
			{"PMA",0x17,0x060671},
			{"PMA",0x18,0x0e2f00},
			{"DEC",0x15,0x0055a0},
			{"DEC",0x17,0x07ff3f},
			{"DSPF",0x06,0x2ebaef},
			{"DSPF",0x08,0x00000b},
			{"DSPF",0x10,0x005010},
			{"DSPF",0x11,0x040001},
			{"DSPF",0x03,0x000004},
			{"DSPF",0x13,0x018670},
			{"DSPF",0x1b,0x000072},
			{"DSPF",0x1c,0x003210},
			{"DSPF",0x14,0x00024a},
			{"DSPF",0x0d,0x02314f},
			{"DSPF",0x0c,0x00504d},
			{"DSPF",0x0f,0x003028},
			{"PMA",0x03,0x082422}
     	},

     	//per-port data
     	{ 	
     		{"PMA",0x03,0x082422}	
	 	}
    }, 
};

static int32 EN75xxGePhy_erroversum_A = 0;
static int32 EN75xxGePhy_erroversum_B = 0;
static int32 EN75xxGePhy_erroversum_C = 0;
static int32 EN75xxGePhy_erroversum_D = 0;

void 		en75xxGePhyCfgLoad(uint8 idx);
void 		en75xxGePhyGRCfgCheck(void);
void 		en75xxGePhyLRCfgCheck(uint8 port_num);
void 		en75xxGePhyCfgCheck(void);

#ifdef MTPHY_DEBUG
//ESD detect
static int16 en75xxGePhy_Up2Down_cnt_gbt[5] = {0,0,0,0,0};
static int16 en75xxGePhy_Up2Down_cnt_hbt[5] = {0,0,0,0,0};
static uint8 en75xxGePhy_Up2DownFastHbt_detect[5] = {0,0,0,0,0};
static uint8 en75xxGePhy_Up2DownFastgbt_detect[5] = {0,0,0,0,0};

static bool 	en75xxGePhy_link_fail_detect_flag = 0;
//static uint8 	en75xxGePhy_reset_needed_perport[5] = {0,0,0,0,0};
//static uint8 	en75xxGePhy_reset_needed_flag = 0;
//static uint8 	en75xxGePhy_esdphy_init_flag = 1; 		// default: enable

//Disable EEE if link fail
static int16 	en75xxGePhy_Up2Down_cnt_tbt[5] = {0,0,0,0,0};
static uint8 	en75xxGePhy_Up2DownFastTbt_detect[5] = {0,0,0,0,0};
static uint8 	en75xxGePhy_linkup_check_timer[5] = {0,0,0,0,0};
static bool 	en75xxGePhyDisEEEHappened[5] = {0,0,0,0,0};
static uint8 	en75xxGePhy_linkup_check_cnt[5] = {0,0,0,0,0};
static uint8 	en75xxGePhyDisNextPage_cnt[5] = {0,0,0,0,0};
static bool 	en75xxGePhyDisNextPage_flag[5] = {0,0,0,0,0};
static uint8 	en75xxGePhyDisSlaveMode_cnt[5] = {0,0,0,0,0};  
static bool 	en75xxGePhyDisSlaveMode_flag[5] = {0,0,0,0,0};
int32 			en75xxGePhyReadProbe(uint8 port_num, uint8 mode, uint8 ch_num);
int32 			en75xxGePhyReadCoeff(uint8 port_num, uint8 mode, uint8 ch_num);
void 			en75xxGePhyDbgPorbeSet(uint8 prb_num, uint8  port_num, uint16 flg_num, uint16 clk_num );
int32 			en75xxGePhyDbgPorbeRead(uint8 prb_num,uint8  port_num);
#endif

#ifdef MTPHY_DEBUG 
int32 	en75xxGePhyReadErrOverSum(uint8 port_num);
int32 	en75xxGePhyReadAdcSum(uint8 port_num);
void 	en75xxGePhyDispProbe100(uint8 port_num, bool long_msg);
void 	en75xxGePhySwDispMessage(uint8 port_num);
uint16 	en75xxGePhyReadSnrSum(uint8 port_num, uint16 cnt);
void 	en75xxGePhyUpdateErrOverSum(uint8 port_num);
void 	en75xxGePhySwErrOverMonitor(uint8 port_num);
void 	en75xxGePhyUp2DownFastgbt(uint8 port_num);
void 	en75xxGePhyUp2DownFastHbt(uint8 port_num);
void 	en75xxGePhyUp2DownFastTbt(uint8 port_num);
bool 	en75xxGePhyLinkFailDetect (uint8 port_num, uint8 mode);		//0:10BT or 100BT, 1:100BT only, 2:10BT only,3:10BT or 100BT or 1000BT
void 	en75xxGePhyDisEEENeeded(uint8 port_num);
void 	en75xxGePhyDisNextPageNeeded(uint8 port_num);
#endif //TCPHY_DEBUG

void 	en75xxGePhySwPatch(void);
void 	doGePhyALLAnalogCal(uint8 phyaddr); 				 
uint8 	allGeAnaCalWait(uint32 delay, uint8 phyaddr);  		 
void	GECal_Rext(uint8 phyaddr, uint32 delay);			 
void	GECal_tx_offset(uint8 phyaddr, uint32 delay);				 
//void	GECal_tx_amp(uint8 phyaddr, uint32 delay);			 
static uint8 	GECal_flag = 0;
static uint8 	GECal_status = 0;
static uint8 	GECal_Rext_flag = 0;
uint8 	allGeAnaCalWait_R45(uint32 delay, uint8 phyaddr); 
uint8 	allGeAnaCalWait_TxOffset(uint32 delay, uint8 phyaddr); 
uint8 	allGeAnaCalWait_TxAmp(uint32 delay, uint8 phyaddr); 


void 	doGePhyALLAnalogCal_R45(uint8 phyaddr); 			 
void	GECal_R45(uint8 phyaddr, uint32 delay);			

void 	doEphySetSLTMode(uint8 phyaddr, uint16 speed_mode); 			 
void 	doEphySetSETMode(uint8 set_mode, uint16 parameter);   // allen_20190709


#ifdef MTPHY_DEBUG
void en75xxGePhyDbgPorbeSet(uint8 prb_num, uint8  port_num, uint16 flg_num, uint16 clk_num)
{
	uint16 	value=0;
	#define dbg_flg_en 1 
	#define dbg_DevAddr 0x1F
	#define dbg_Flg0Ctrl 0x15
	#define dbg_Flg1Ctrl 0x16
	#define dbg_ClkAddr 0x19
 
	if(prb_num == 2)
	{
		value = (port_num<<13)|(dbg_flg_en<<12)|(flg_num);
		TCPHYDISP1("Dbg Flag Register Set Value=%lx\r\n", value);
		mtEMiiRegWrite(port_num,dbg_DevAddr, dbg_Flg0Ctrl, value);
		mtEMiiRegWrite(port_num,dbg_DevAddr, dbg_Flg1Ctrl, value);
	}
	else if(prb_num == 0)
	{
		value = (port_num<<13)|(dbg_flg_en<<12)|(flg_num);
		TCPHYDISP1("Dbg Flag Register Set Value=%lx\r\n", value);
		mtEMiiRegWrite(port_num, dbg_DevAddr, dbg_Flg0Ctrl, value);
	}
	else if(prb_num == 1)
	{
		value = (port_num<<13)|(dbg_flg_en<<12)|(flg_num);
		TCPHYDISP1("Dbg Flag Register Set Value=%lx\r\n", value);
		mtEMiiRegWrite(port_num, dbg_DevAddr, dbg_Flg1Ctrl, value);
	}

    if(clk_num == 0xff)
    {	//disable clk output
		value = mtEMiiRegRead( port_num, dbg_DevAddr, dbg_ClkAddr);
		value = value & 0xEF;
		mtEMiiRegWrite(port_num, dbg_DevAddr, dbg_ClkAddr, value);
    }
	else
	{	//enable clk output
		value = (dbg_flg_en<<12)|(clk_num);
		mtEMiiRegWrite(port_num, dbg_DevAddr, dbg_ClkAddr, value);
	}	
}

int32 en75xxGePhyDbgPorbeRead(uint8 prb_num,uint8  port_num)
{
	uint16 	value=0;
	#define dbg_flg_en 1 
	#define dbg_DevAddr 0x1F
	#define dbg_Flg0Ctrl 0x15
	#define dbg_Flg1Ctrl 0x16
	#define dbg_ClkAddr 0x19
	#define dbg_monAddr 0x1A

	value = mtEMiiRegRead( port_num, dbg_DevAddr, dbg_monAddr);

	if(prb_num == 0)
		value = (value & 0x00FF);
	else if(prb_num == 1)
		value = (value>>8) & 0x00FF;
             	
	TCPHYDISP1("Dbg Flag[%d] Monitor Register Value=%lx\r\n", prb_num,value);
	return(value);	       	
}

int32 en75xxGePhyReadProbe(uint8 port_num, uint8 mode, uint8 ch_num)
{
    uint32 val=0,val_A=0,val_B=0,val_C=0,val_D=0;
    uint32 avg_val=0, avg_A=0,avg_B=0,avg_C=0,avg_D=0;
    uint32 phyaddr = port_num;
    uint16 i;
    #define avg_cnt 1000
    
    #if debug_flag
    tcMiiStationWrite(port_num, 31, 0);
    switch(mode)
    {
    	case Probe_VgaState:
    		val = mtPhyMiiRead_TrDbg(phyaddr,"PMA",0x38,0);
    		TCPHYDISP3("VgaStateA =0x%x\r\n",((val>>4) & 0x1F));
    		TCPHYDISP3("VgaStateB =0x%x\r\n",((val>>9) & 0x1F));
    		TCPHYDISP3("VgaStateC =0x%x\r\n",((val>>14) & 0x1F));
    		TCPHYDISP3("VgaStateD =0x%x\r\n",((val>>19) & 0x1F));
    		break;
    		
        case Probe_MSE:
          	switch(ch_num)
          	{
	        	case 0:		//Pair A
              		val = mtEMiiRegRead(phyaddr,0x1E,0x9B);
			  		TCPHYDISP1("XX0 0x1E,0x9B =0x%x\r\n",val);
              		val = (val>>8) & 0xFF;  	//V1.16
              		TCPHYDISP1("AA0 lch_mse_mdcA =0x%x\r\n",val);
              		break;
            	case 1:		//Pair B
              		val = mtEMiiRegRead(phyaddr,0x1E,0x9B);
			  		TCPHYDISP1("XX1 0x1E,0x9B =0x%x\r\n",val);
              		val = (val) & 0xFF;  	//V1.16
              		TCPHYDISP1("AA1 lch_mse_mdcB =0x%x\r\n",val);
              		break;
            	case 2:		//Pair C
              		val = mtEMiiRegRead(phyaddr,0x1E,0x9C);
			  		TCPHYDISP1("XX2 0x1E,0x9C =0x%x\r\n",val);
              		val = (val>>8) & 0xFF;  	//V1.16
              		TCPHYDISP1("AA2 lch_mse_mdcC =0x%x\r\n",val);
              		break;
            	case 3:		//Pair D
              		val = mtEMiiRegRead(phyaddr,0x1E,0x9C);
			  		TCPHYDISP1("XX3 0x1E,0x9C =0x%x\r\n",val);
              		val = (val) & 0xFF;  	//V1.16
              		TCPHYDISP1("AA3 lch_mse_mdcD =0x%x\r\n",val);
              		break;
            }
		    break;
		    
        case Probe_MSE_ALL:
			val=mtEMiiRegRead(phyaddr,0x1E,0x9B);
			val_A =(val>>8) & 0xFF;  //V1.16
			val_B =(val) & 0xFF;  //V1.16
			TCPHYDISP1("ALL 0x1E,0x9B =0x%x\r\n",val);		

			val=mtEMiiRegRead(phyaddr,0x1E,0x9C);

			val_C =(val>>8) & 0xFF;  //V1.16
			val_D =(val) & 0xFF;  //V1.16
			TCPHYDISP1("ALL 0x1E,0x9C =0x%x\r\n",val);		
			TCPHYDISP1("BB1 lch_mse_mdcA =0x%x\r\n",val_A);
			TCPHYDISP1("BB2 lch_mse_mdcB =0x%x\r\n",val_B);
			TCPHYDISP1("BB3 lch_mse_mdcC =0x%x\r\n",val_C);
			TCPHYDISP1("BB4 lch_mse_mdcD =0x%x\r\n",val_D);
			val=val_A+val_B+val_C+val_D;
            break;
            
        case Probe_MSE_Slicer_err_thres:
			val=mtEMiiRegRead(phyaddr,0x1E,0x9D);
			val=(val>>11) & 0x1F; 
			TCPHYDISP3("slicer_err_thres =0x%x\r\n",val);
			break;
			
        case Probe_MSE_Sum:
			for(i=0; i<avg_cnt; i++)
			{
				val = mtEMiiRegRead(phyaddr, 0x1E, 0x9B);
				val_A =(val>>8) & 0xFF;  //V1.16
				val_B =(val) & 0xFF;  //V1.16
				avg_A = avg_A + val_A;
				avg_B = avg_B + val_B;
            
				val = mtEMiiRegRead(phyaddr, 0x1E, 0x9C);
				val_C =(val>>8) & 0xFF;  //V1.16
				val_D =(val) & 0xFF;  //V1.16
				avg_C = avg_C + val_C;
				avg_D = avg_D + val_D;
             
				val = (mtEMiiRegRead(phyaddr, 0x1E, 0x9D)>>11)&0x1F;

				avg_val = avg_val + val;
			}
	        avg_A = avg_A/1000;
    	    avg_B = avg_B/1000;
          	avg_C = avg_C/1000;
          	avg_D = avg_D/1000;
          	avg_val = avg_val/1000;
          
          	val = avg_A + avg_B + avg_C + avg_D;
          
          	TCPHYDISP3("lch_mse_mdcA avg1000 =0x%x\r\n", avg_A);
          	TCPHYDISP3("lch_mse_mdcB avg1000 =0x%x\r\n", avg_B);
          	TCPHYDISP3("lch_mse_mdcC avg1000 =0x%x\r\n", avg_C);
          	TCPHYDISP3("lch_mse_mdcD avg1000 =0x%x\r\n", avg_D);
          	TCPHYDISP3("slicer_err_thres avg1000 =0x%x\r\n", avg_val);
          	break;
          	
        case Probe_Slicer_Err_Over_Sum:      
	    	switch(ch_num)
	        {
              	case 0:
              		val = mtEMiiRegRead(phyaddr, 0x1E, 0x9E);
              		ErovcntA[port_num].err_over_cnt = val;
              		TCPHYDISP3("ProbeSlicerErrOverSumA =0x%x\r\n", val);
              		break;
              		
            	case 1:
              		val = mtEMiiRegRead(phyaddr, 0x1E, 0x9F);
              		ErovcntB[port_num].err_over_cnt = val;
              		TCPHYDISP3("ProbeSlicerErrOverSumB =0x%x\r\n", val);
              		break;
              		
	            case 2:
              		val = mtEMiiRegRead(phyaddr, 0x1E, 0xA0);
              		ErovcntC[port_num].err_over_cnt = val;
              		TCPHYDISP3("ProbeSlicerErrOverSumC =0x%x\r\n", val);
              		break;
              		
            	case 3:
              		val = mtEMiiRegRead(phyaddr, 0x1E, 0xA1);
              		ErovcntD[port_num].err_over_cnt = val;
              		TCPHYDISP3("ProbeSlicerErrOverSumD =0x%x\r\n", val);
              		break;
        	}
          	break;
          		
        case Probe_Slicer_Err_Over_Sum_ALL:
              val_A = mtEMiiRegRead(phyaddr, 0x1E, 0x9E);
              TCPHYDISP3("ProbeSlicerErrOverSumA =0x%x\r\n", val_A);
              val_B = mtEMiiRegRead(phyaddr, 0x1E, 0x9F);
              TCPHYDISP3("ProbeSlicerErrOverSumB =0x%x\r\n", val_B);
              val_C = mtEMiiRegRead(phyaddr, 0x1E, 0xA0);
              TCPHYDISP3("ProbeSlicerErrOverSumC =0x%x\r\n", val_C);
              val_D = mtEMiiRegRead(phyaddr, 0x1E, 0xA1);
              TCPHYDISP3("ProbeSlicerErrOverSumD =0x%x\r\n", val_D);
              val = val_A + val_B + val_C + val_D;
              break;
  
        default:
            TCPHYDISP1("\r\ntcphy error: ReadProbe %d.\r\n",mode);
            break;
    }
    #endif
    return val;
}

void en75xxGePhySwDispMessage(uint8 port_num)
{
	#ifdef PERIOD_PRINT 	//V1.17
    uint32 value;
	#endif

    switch(mtphy_link_state[port_num]) 
    {
        case ST_LINK_DOWN: 
      		// printf("Link Satus is ST_LINK_DOWN!\r\n");
            break;
            
        case ST_LINK_DOWN2UP: 
      		//  printf("Link Satus is ST_LINK_DOWN2UP!\r\n");
			if(mr1E_A2.final_speed_1000)
			{
				TCPHYDISP1("mtphy[%d]:Link-up at 1000 %s.\r\n", port_num, (mr1E_A2.FullDuplexEnable ?"F":"H"));
			}
			else if (mr1E_A2.final_speed_100)
			{
				TCPHYDISP1("mtphy[%d]:Link-up at 100%s.\r\n", port_num, (mr1E_A2.FullDuplexEnable ?"F":"H"));
			}
			else if (mr1E_A2.final_speed_10)
			{
				TCPHYDISP1("mtphy[%d]:Link-up at 10%s.\r\n",port_num, (mr1E_A2.FullDuplexEnable ?"F":"H"));
			}
	
			if(mr1E_A2.final_speed_1000 ||mr1E_A2.final_speed_100 ||mr1E_A2.final_speed_10)
			{
				en75xxGePhyReadProbe(port_num, Probe_VgaState, 0);
				en75xxGePhyReadProbe(port_num, Probe_MSE_ALL, 0);
				en75xxGePhyReadProbe(port_num, Probe_MSE_Slicer_err_thres, 0);
				en75xxGePhyReadProbe(port_num, Probe_Slicer_Err_Over_Sum_ALL, 0);
			}
            break;
            
        case ST_LINK_UP: 
         	// printf("Link Satus is ST_LINK_UP!\r\n");
          	#ifdef PERIOD_PRINT //V1.17
            if (period_print_flag == 1)
            {
            	if (period_print_cnt<period_print_threshold)
              	{
                	period_print_cnt++;
              	}
			  	else
			  	{
                	period_print_cnt=0;
					value = mtPhyMiiRead_TrDbg(port_num, "PMA", 0x38 ,0);
					printf("port_num=0x%X,Value of PMA 0x38=0x%X\r\n",port_num,value);
					printf("VgaStateA=0x%X,VgaStateB=0x%X,VgaStateC=0x%X,VgaStateD=0x%X\r\n",(value&0x1F0)>>4,(value&0x3E00)>>9,(value&0x7C000)>>14,(value&0xF80000)>>19);
			  	}
            }
          	#endif
            break;
            
        case ST_LINK_UP2DOWN:
          	#ifdef PERIOD_PRINT //V1.17			
			period_print_cnt=0;
          	#endif
            TCPHYDISP1("mtcphy[%d]: Link-down!!!\r\n",port_num);
            break;
            
        default: printf("\r\nmtphy error: SwDispMessage error!\r\n");
    }
}

void en75xxGePhyUpdateErrOverSum(uint8 port_num)
{
    uint16 	err_over_cnt_prev_A; 			// to store previous err_over_cnt value
    uint16 	err_over_cnt_prev_B; 			// to store previous err_over_cnt value
    uint16 	err_over_cnt_prev_C; 			// to store previous err_over_cnt value
    uint16 	err_over_cnt_prev_D; 			// to store previous err_over_cnt value
    int32 	val_A, val_B, val_C, val_D;
    
    // clear Nmr25[port_num].err_over_cnt in tc2104meDispProbe100() in ST_LINK_DOWN2UP
    if (((mr1E_A2.final_speed_1000== 1) ||(mr1E_A2.final_speed_100 == 1)) 	// 1000/100BaseTX
        && (mtphy_link_state[port_num] == ST_LINK_UP)) 
    {
    	err_over_cnt_prev_A = ErovcntA[port_num].err_over_cnt;
        err_over_cnt_prev_B = ErovcntB[port_num].err_over_cnt;
        err_over_cnt_prev_C = ErovcntC[port_num].err_over_cnt;
        err_over_cnt_prev_D = ErovcntD[port_num].err_over_cnt;
        
      	// en75xxGePhyReadProbe(port_num,Probe_MSE_Sum,0);
       	en75xxGePhyReadProbe(port_num,Probe_Slicer_Err_Over_Sum,0);		//Pair A
       	en75xxGePhyReadProbe(port_num,Probe_Slicer_Err_Over_Sum,1);		//Pair B
       	en75xxGePhyReadProbe(port_num,Probe_Slicer_Err_Over_Sum,2);		//Pair C
       	en75xxGePhyReadProbe(port_num,Probe_Slicer_Err_Over_Sum,3);		//Pair D
        val_A = ErovcntA[port_num].err_over_cnt - err_over_cnt_prev_A;
        val_B = ErovcntB[port_num].err_over_cnt - err_over_cnt_prev_B;
        val_C = ErovcntC[port_num].err_over_cnt - err_over_cnt_prev_C;
        val_D = ErovcntD[port_num].err_over_cnt - err_over_cnt_prev_D;
        #if debug_flag
        TCPHYDISP4("ProbeSlicerErrOverSumA =0x%x\r\n",val_A);
        TCPHYDISP4("ProbeSlicerErrOverSumB =0x%x\r\n",val_B);
        TCPHYDISP4("ProbeSlicerErrOverSumC =0x%x\r\n",val_C);
        TCPHYDISP4("ProbeSlicerErrOverSumD =0x%x\r\n",val_D);
        #endif
        
        if( val_A < 0 )
        {
        	val_A += 2048;
        }
        if( val_B < 0 )
        {
            val_B += 2048;
        }
        if( val_C < 0 )
        {
            val_C += 2048;
        }
        if( val_D < 0 )
        {
            val_D += 2048;
        }
        EN75xxGePhy_erroversum_A = val_A;
        EN75xxGePhy_erroversum_B = val_B;
        EN75xxGePhy_erroversum_C = val_C;
        EN75xxGePhy_erroversum_D = val_D;
    }
    else 
    {
        EN75xxGePhy_erroversum_A = -1;
        EN75xxGePhy_erroversum_B = -1;
        EN75xxGePhy_erroversum_C = -1;
        EN75xxGePhy_erroversum_D = -1;
    }
}

void en75xxGePhySwErrOverMonitor(uint8 port_num)
{
    if (tcPhy_disp_level < 3) 
        return; 			// inactive
	#if debug_flag
    if (EN75xxGePhy_erroversum_A>0)
    {
        TCPHYDISP3("tcphy[%d]: ErrOver A=%ld\r\n",port_num,EN75xxGePhy_erroversum_A);          
    }
    if (EN75xxGePhy_erroversum_B>0)
    {
        TCPHYDISP3("tcphy[%d]: ErrOver B=%ld\r\n",port_num,EN75xxGePhy_erroversum_B);          
    }
    if (EN75xxGePhy_erroversum_C>0)
    {
        TCPHYDISP3("tcphy[%d]: ErrOver C =%ld\r\n",port_num,EN75xxGePhy_erroversum_C);          
    }
    if (EN75xxGePhy_erroversum_D>0)
    {
        TCPHYDISP3("tcphy[%d]: ErrOver D=%ld\r\n",port_num,EN75xxGePhy_erroversum_D);          
    }
    #endif
}

void en75xxGePhyUp2DownFastgbt(uint8 port_num)
{
	const uint8 en75xxGePhy_Up2Down_gbt_Thd = 10;
	const uint8 en75xxGePhy_Up2Down_gbt_ub = 40;

	if(mr1E_A2.final_speed_1000 & !Nmr1[port_num].link_status) 
	{
		if(en75xxGePhy_Up2Down_cnt_gbt[port_num]<en75xxGePhy_Up2Down_gbt_ub) 
		{
			en75xxGePhy_Up2Down_cnt_gbt[port_num]+=3;
		}
    }
	else if(!mr1E_A2.final_speed_1000 & !Nmr1[port_num].link_status)
	{
		if(en75xxGePhy_Up2Down_cnt_gbt[port_num]>0) 
		{
			en75xxGePhy_Up2Down_cnt_gbt[port_num]--;
		}
	}
	else if(mr1E_A2.final_speed_1000 & Nmr1[port_num].link_status) 
	{	//adjust link-up time to clear counter
		en75xxGePhy_Up2Down_cnt_gbt[port_num]=0;
	}
	
	if(en75xxGePhy_Up2Down_cnt_gbt[port_num] > en75xxGePhy_Up2Down_gbt_Thd) 
	{
	    en75xxGePhy_Up2Down_cnt_gbt[port_num]=0;
		en75xxGePhy_Up2DownFastgbt_detect[port_num]=1;
		#ifdef TCPHY_DEBUG
		TCPHYDISP4("set 7523 GePhy_Up2DownFastgbt_detect[%d] to 1.\r\n",port_num); 
		#endif
	}
	else 
	{
		en75xxGePhy_Up2DownFastgbt_detect[port_num]=0;
	}  
}

void en75xxGePhyUp2DownFastHbt(uint8 port_num)
{
	const uint8 en75xxGePhy_Up2Down_hbt_Thd = 10;
	const uint8 en75xxGePhy_Up2Down_hbt_ub = 40;

	if(mr1E_A2.final_speed_100 & !Nmr1[port_num].link_status) 
	{
		if(en75xxGePhy_Up2Down_cnt_hbt[port_num] < en75xxGePhy_Up2Down_hbt_ub) 
		{
			en75xxGePhy_Up2Down_cnt_hbt[port_num] += 3;
		}
    }
	else if(!mr1E_A2.final_speed_100 & !Nmr1[port_num].link_status) 
	{
		if(en75xxGePhy_Up2Down_cnt_hbt[port_num]>0) 
		{
			en75xxGePhy_Up2Down_cnt_hbt[port_num]--;
		}
	}
	else if(mr1E_A2.final_speed_100 & Nmr1[port_num].link_status) 
	{	//adjust link-up time to clear counter
		en75xxGePhy_Up2Down_cnt_hbt[port_num]=0;
	}
	
	if(en75xxGePhy_Up2Down_cnt_hbt[port_num]> en75xxGePhy_Up2Down_hbt_Thd) 
	{
	    en75xxGePhy_Up2Down_cnt_hbt[port_num]=0;
		en75xxGePhy_Up2DownFastHbt_detect[port_num]=1;
		#ifdef TCPHY_DEBUG
		TCPHYDISP4("set 7523 GePhy_Up2DownFastHbt_detect[%d] to 1.\r\n",port_num); 
		#endif
	}
	else 
	{
		en75xxGePhy_Up2DownFastHbt_detect[port_num]=0;
	}  
}

void en75xxGePhyUp2DownFastTbt(uint8 port_num)
{
	const uint8 en75xxGePhy_Up2Down_Tbt_Thd = 10;
	const uint8 en75xxGePhy_Up2Down_Tbt_ub = 40;
	const uint8 linkup_check_timer_done = 1;
	
    if(mr1E_A2.final_speed_10 & !Nmr1[port_num].link_status) 
	{
		en75xxGePhy_linkup_check_timer[port_num]=0;
		if(en75xxGePhy_Up2Down_cnt_tbt[port_num] < en75xxGePhy_Up2Down_Tbt_ub) 
		{
			en75xxGePhy_Up2Down_cnt_tbt[port_num]+=5;
		}
    }
	else if(!mr1E_A2.final_speed_10 & !Nmr1[port_num].link_status) 
	{
		en75xxGePhy_linkup_check_timer[port_num]=0;
		if(en75xxGePhy_Up2Down_cnt_tbt[port_num]>0) 
		{
			en75xxGePhy_Up2Down_cnt_tbt[port_num]--;
		}
	}
	else if(mr1E_A2.final_speed_10 & Nmr1[port_num].link_status ) 
	{
		if((en75xxGePhy_linkup_check_timer[port_num] == linkup_check_timer_done)) 
		{
			en75xxGePhy_Up2Down_cnt_tbt[port_num]=0;
			en75xxGePhy_linkup_check_timer[port_num]=0;
		}
		else if((en75xxGePhy_linkup_check_timer[port_num] < linkup_check_timer_done))
		{
			en75xxGePhy_linkup_check_timer[port_num]++;
		}
	}
	
	if(en75xxGePhy_Up2Down_cnt_tbt[port_num]> en75xxGePhy_Up2Down_Tbt_Thd) 
	{
	    en75xxGePhy_Up2Down_cnt_tbt[port_num]=0; 
		en75xxGePhy_Up2DownFastTbt_detect[port_num]=1;
		#ifdef TCPHY_DEBUG
		TCPHYDISP4("set 7523 GePhy_Up2DownFastTbt_detect[%d] to 1.\r\n",port_num); 
		#endif
	}
	else 
	{
		en75xxGePhy_Up2DownFastTbt_detect[port_num]=0;
	}	
}

bool en75xxGePhyLinkFailDetect(uint8 port_num, uint8 mode)
{
	switch(mode)
	{
		case TbtOrHbtOrGbt:
			if(sw_FixUp2DownFast_flag 
				&(en75xxGePhy_Up2DownFastTbt_detect[port_num]
				||en75xxGePhy_Up2DownFastHbt_detect[port_num]||en75xxGePhy_Up2DownFastgbt_detect[port_num])) 
			{
				en75xxGePhy_link_fail_detect_flag =1;
		        #ifdef TCPHY_DEBUG
		        TCPHYDISP3(" 7523 GePhy_link_fail_detect_flag(%d)=%d\r\n", port_num, en75xxGePhy_link_fail_detect_flag); 
		        #endif
			}
			else 
			{
				en75xxGePhy_link_fail_detect_flag =0;
			}	
			break;

		case HbtOnly:
			if(sw_FixUp2DownFast_flag & en75xxGePhy_Up2DownFastHbt_detect[port_num]) 
			{
				en75xxGePhy_link_fail_detect_flag =1;
		        #ifdef TCPHY_DEBUG
		        TCPHYDISP3("Hbt: 7523 GePhy_link_fail_detect_flag(%d)=%d\r\n", port_num, en75xxGePhy_link_fail_detect_flag); 
		        #endif
			}
			else 
			{
				en75xxGePhy_link_fail_detect_flag =0;
			}
			break;

		case TbtOnly:
			if(sw_FixUp2DownFast_flag & en75xxGePhy_Up2DownFastTbt_detect[port_num]) 
			{
				en75xxGePhy_link_fail_detect_flag =1;
		       	#ifdef TCPHY_DEBUG
		        TCPHYDISP3("Tbt: 7523 GePhy_link_fail_detect_flag(%d)=%d\r\n", port_num, en75xxGePhy_link_fail_detect_flag); 
		        #endif
			}
			else 
			{
				en75xxGePhy_link_fail_detect_flag =0;
			}
			break;

		default: 
			en75xxGePhy_link_fail_detect_flag =0;
	}
	return en75xxGePhy_link_fail_detect_flag;
}

void en75xxGePhyDisNextPageNeeded(uint8 port_num)
{
	const uint8 en75xxGePhyDisNextPage_cntdone = 5;
	const uint8 en75xxGePhyDisSlaveMode_cntdone = 3; 
   
   	if((mtphy_link_state[port_num] == ST_LINK_UP)||(en75xxGePhyDisEEEHappened[port_num])) 
 	{
   		en75xxGePhyDisNextPage_cnt[port_num] = 0;
   	}
   	else if((mr5.LPNextAble) && !(mr7_3D.lp_eee_100||mr7_3D.lp_eee_1000||mr7_3D.lp_eee_10g)
   		&& (en75xxGePhyDisNextPage_cnt[port_num] <= en75xxGePhyDisNextPage_cntdone)) 
   	{
   		en75xxGePhyDisNextPage_cnt[port_num]++;
   	}

   	if(en75xxGePhyDisNextPage_cnt[port_num] > en75xxGePhyDisNextPage_cntdone) 
	{
   		en75xxGePhyDisNextPage_flag[port_num] = 1;
		en75xxGePhyDisNextPage_cnt[port_num] = 0;
   	}
   	else 
	{
   		en75xxGePhyDisNextPage_flag[port_num]=0;
   	}

	// === for gphy eee iot with realtek : default slave mode
	if((mtphy_link_state[port_num] != ST_LINK_UP) && (mtphy_link_state[port_num] != ST_LINK_DOWN) &&
		(en75xxGePhyDisSlaveMode_cnt[port_num] <= en75xxGePhyDisSlaveMode_cntdone))
	{
		en75xxGePhyDisSlaveMode_cnt[port_num]++;
	}

	if(en75xxGePhyDisSlaveMode_cnt[port_num] > en75xxGePhyDisSlaveMode_cntdone)
	{
		en75xxGePhyDisSlaveMode_flag[port_num] = 1;
		en75xxGePhyDisSlaveMode_cnt[port_num] = 0;
	}
	else
	{
		en75xxGePhyDisSlaveMode_flag[port_num] = 0;
	}
}

void en75xxGePhyDisEEENeeded(uint8 port_num)
{
    const uint8 en75xxGePhy_linkup_check_done = 1;
	uint8 phy_base = 8, phyaddr;
	
	phyaddr = phy_base + port_num;
	
    if((mtphy_link_state[port_num] == ST_LINK_UP)
		&& (en75xxGePhy_linkup_check_cnt[port_num] < en75xxGePhy_linkup_check_done)) 
	{
		en75xxGePhy_linkup_check_cnt[port_num]++;
    }
	else if(mtphy_link_state[port_num] != ST_LINK_UP)
	{
		en75xxGePhy_linkup_check_cnt[port_num] = 0;
    }
	
    if(en75xxGePhyDisEEEHappened[port_num] && en75xxGePhy_linkup_check_cnt[port_num]==en75xxGePhy_linkup_check_done) 
	{	//extend link-up time
		mtEMiiRegWrite(phyaddr, 0x07, 0x3c, 0x06);
		en75xxGePhyDisEEEHappened[port_num]=0;
    }
	else if(en75xxGePhyLinkFailDetect(port_num,3) || en75xxGePhyDisNextPage_flag[port_num]) 
	{
		mtEMiiRegWrite(phyaddr, 0x07, 0x3c, 0x0000);	// disable next page
		//tcMiiStationWrite(0,0x00,0x1200);				// re-start AN
		en75xxGePhyDisEEEHappened[port_num]=1;
    }
	//TCPHYDISP3("7523 GePhy_linkup_check_cnt[%d]=%d\r\n", port_num, en75xxGePhy_linkup_check_cnt[port_num]);
	//TCPHYDISP3("7523 GePhyDisEEEHappened[%d]=%d\r\n", port_num, en75xxGePhyDisEEEHappened[port_num]);
}

void sw_downshift(uint8 phyaddr)
{
		uint32 	dev_1e_reg_9b, dev_1e_reg_9c,mean_square_error_times_limit=3,mean_square_error_value_limit=5,link_counte_limit=20,mean_squre_error_A,mean_squre_error_B,mean_squre_error_C,mean_squre_error_D,mean_squre_error_temp;

                dev_1e_reg_9b = mtEMiiRegRead(phyaddr,0x1E,0x9B);
				dev_1e_reg_9c = mtEMiiRegRead(phyaddr,0x1E,0x9C);
				mean_squre_error_A=(dev_1e_reg_9b/256)&0xff;
				mean_squre_error_B=dev_1e_reg_9b&0xff;
				mean_squre_error_C=(dev_1e_reg_9c/256)&0xff;
				mean_squre_error_D=dev_1e_reg_9c&0xff;
				/*
                printf("port%d A_mean_square_error=0x%x --" ,phyaddr,mean_squre_error_A);
                printf("port%d B_mean_square_error=0x%x --" ,phyaddr,mean_squre_error_B);
                printf("port%d C_mean_square_error=0x%x --" ,phyaddr,mean_squre_error_C);
                printf("port%d D_mean_square_error=0x%x \n\r" ,phyaddr,mean_squre_error_D);

                if(phyaddr ==9) {printf("port%d curren_counter=0x%x    --  " ,phyaddr,ephy_monitor_p9_curren_counter);}
				if(phyaddr ==10) {printf("port%d curren_counter=0x%x    --  " ,phyaddr,ephy_monitor_p10_curren_counter);}
                if(phyaddr ==11) {printf("port%d curren_counter=0x%x    --  " ,phyaddr,ephy_monitor_p11_curren_counter);}
                if(phyaddr ==12) {printf("port%d curren_counter=0x%x    --  " ,phyaddr,ephy_monitor_p12_curren_counter);}
				printf("ephy_monitor_all_counter=0x%x \n\r" ,ephy_monitor_all_counter);
				*/
				mean_squre_error_temp=ephy_monitor_all_counter-ephy_monitor_p9_curren_counter;
				if(mean_squre_error_temp ==1) { ephy_monitor_continue_p9_counter++;} else { ephy_monitor_continue_p9_counter=0;}
				mean_squre_error_temp=ephy_monitor_all_counter-ephy_monitor_p10_curren_counter;
				if(mean_squre_error_temp ==1) { ephy_monitor_continue_p10_counter++;} else { ephy_monitor_continue_p10_counter=0;}
				mean_squre_error_temp=ephy_monitor_all_counter-ephy_monitor_p11_curren_counter;
				if(mean_squre_error_temp ==1) { ephy_monitor_continue_p11_counter++;} else { ephy_monitor_continue_p11_counter=0;}
				mean_squre_error_temp=ephy_monitor_all_counter-ephy_monitor_p12_curren_counter;
				if(mean_squre_error_temp ==1) { ephy_monitor_continue_p12_counter++;} else { ephy_monitor_continue_p12_counter=0;}
		
				if(ephy_monitor_continue_p9_counter >= link_counte_limit) { ephy_monitor_continue_p9_counter=0;ephy_monitor_p9_stop_polling=1;mean_square_error_counter_p9=0;}
				if(ephy_monitor_continue_p10_counter >= link_counte_limit) { ephy_monitor_continue_p9_counter=0;ephy_monitor_p10_stop_polling=1;mean_square_error_counter_p10=0;}
				if(ephy_monitor_continue_p11_counter >= link_counte_limit) { ephy_monitor_continue_p9_counter=0;ephy_monitor_p11_stop_polling=1;mean_square_error_counter_p11=0;}
				if(ephy_monitor_continue_p12_counter >= link_counte_limit) { ephy_monitor_continue_p9_counter=0;ephy_monitor_p12_stop_polling=1;mean_square_error_counter_p12=0;}
				if(phyaddr ==9) {ephy_monitor_p9_curren_counter=ephy_monitor_all_counter;}
				if(phyaddr ==10) {ephy_monitor_p10_curren_counter=ephy_monitor_all_counter;}
				if(phyaddr ==11) {ephy_monitor_p11_curren_counter=ephy_monitor_all_counter;}
				if(phyaddr ==12) {ephy_monitor_p12_curren_counter=ephy_monitor_all_counter;}
			if((mean_squre_error_A >= mean_square_error_value_limit)||(mean_squre_error_B >= mean_square_error_value_limit)||(mean_squre_error_C >= mean_square_error_value_limit)||(mean_squre_error_D >= mean_square_error_value_limit)) 
			{
				if(phyaddr ==9) {mean_square_error_counter_p9++;}
				if(phyaddr ==10) {mean_square_error_counter_p10++;}
				if(phyaddr ==11) {mean_square_error_counter_p11++;}
				if(phyaddr ==12) {mean_square_error_counter_p12++;}
				if( mean_square_error_counter_p9 >= mean_square_error_times_limit) { tcMiiStationWrite(phyaddr, 0x9,0000);sw_down_shift_p9=1;printf("port9 sw down shift\n\r" );mean_square_error_counter_p9=0;}
				if( mean_square_error_counter_p10 >= mean_square_error_times_limit) { tcMiiStationWrite(phyaddr, 0x9,0000);sw_down_shift_p10=1;printf("port10 sw down shift\n\r" );mean_square_error_counter_p10=0;}
				if( mean_square_error_counter_p11 >= mean_square_error_times_limit) { tcMiiStationWrite(phyaddr, 0x9,0000);sw_down_shift_p11=1;printf("port11 sw down shift\n\r" );mean_square_error_counter_p11=0;}
				if( mean_square_error_counter_p12 >= mean_square_error_times_limit) { tcMiiStationWrite(phyaddr, 0x9,0000);sw_down_shift_p12=1;printf("port12 sw down shift\n\r" );mean_square_error_counter_p12=0;}
				tcMiiStationWrite(phyaddr, 0x0,0x1240);
				
			} 
			
}


void en75xxGePhySwPatch(void)
{
    uint8 	pn;
	uint32 	reg0, reg1, reg9, reg10,reg_temp,reg_9_temp,reg_10_temp,reg_temp1,master_times=2,slave_times=3,force_salve_time=2,down_shift_force_slave_time=0;
	uint32 	dev_1e_reg_a2,dev_1e_reg_a2_temp,page_1_reg_1a,page_1_reg_1a_temp;
	uint32	reg_vga=0, vga_a=0, vga_b=0, vga_c=0, vga_d=0;
	uint8 	gphy_base=9, phyaddr=9, phy_base=9;
	#if 0
    if(power_on_finish_flag == 0)
    {
    	if((tcMiiStationRead(12, 0x09)&0xE000) == 0xE000) // ? 
      	{
        	power_on_finish_flag = 1;
        	printf("\n Power On Process Finish \n");
        	//tcMiiStationWrite(12, 0x09, 0x0700);
      	}
    }
    #endif

    for(phyaddr = gphy_base; phyaddr < (gphy_base+EN75xxGe_PORTNUM); phyaddr++) 
	{
		pn = phyaddr - phy_base;
        mtPhyReadReg(pn,1);
        reg0 = tcMiiStationRead(phyaddr, 0x0);
		reg1 = tcMiiStationRead(phyaddr, 0x1);
		reg9 = tcMiiStationRead(phyaddr, 0x9);
		reg10 = tcMiiStationRead(phyaddr, 0xa);
		reg_9_temp=reg9;
		reg_10_temp=reg10;
        if( !Nmr1[pn].link_status_prev && !Nmr1[pn].link_status )
		{
            mtphy_link_state[pn] = ST_LINK_DOWN;

        }
        else if( !Nmr1[pn].link_status_prev && Nmr1[pn].link_status )
		{
            mtphy_link_state[pn] = ST_LINK_DOWN2UP;
			if((link_state_polling == 1)||(slt_excuting==1)){ printf("phy=%d link down to up\n\r",phyaddr);}
        }
        else if( Nmr1[pn].link_status_prev && !Nmr1[pn].link_status )
		{
            mtphy_link_state[pn]= ST_LINK_UP2DOWN;
			if( slt_excuting==0 )
			{
			if((ephy_monitor_p9_stop_polling==1)&&(phyaddr ==9)) { ephy_monitor_p9_stop_polling=0;}
			if((ephy_monitor_p10_stop_polling==1)&&(phyaddr ==10)) { ephy_monitor_p10_stop_polling=0;}
			if((ephy_monitor_p11_stop_polling==1)&&(phyaddr ==11)) { ephy_monitor_p11_stop_polling=0;}
			if((ephy_monitor_p12_stop_polling==1)&&(phyaddr ==12)) { ephy_monitor_p12_stop_polling=0;}		
			}		
			if((link_state_polling == 1)||(slt_excuting==1)){ printf("phy=%d link up to down\n\r",phyaddr);}
       }
        else 
		{ 	//if( Nmr1[pn].link_status_prev && Nmr1[pn].link_status ){
            mtphy_link_state[pn] = ST_LINK_UP;
			if( slt_excuting==0 ) {			
			reg_temp=reg_9_temp&0x0200;        
			if( reg_temp !=0 ) 		    
				{						
			
				reg_temp=reg_9_temp&0x1800;            
				if( reg_temp !=0x1800 ) 			
					{					
					reg_temp=(reg_9_temp&0xe7ff)|0x1800;			
					tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force master			
					}
				}
		if( phyaddr ==9 ) {	force_giga_master_counter_p9=0;	}
        if( phyaddr ==10 ) { force_giga_master_counter_p10=0;}
        if( phyaddr ==11 ) { force_giga_master_counter_p11=0;}		
        if( phyaddr ==12 ) { force_giga_master_counter_p12=0;}
		
		if( phy_downshift_force_giga_slave_enable==1 ) {	
		dev_1e_reg_a2 = mtEMiiRegRead(phyaddr,0x1E,0xa2);
		page_1_reg_1a = tcMiiStationRead(phyaddr, 0x1a);
		page_1_reg_1a_temp=page_1_reg_1a&0x0004;
		dev_1e_reg_a2_temp = dev_1e_reg_a2 &0x0004;
        if( phyaddr ==9 ) {	

		if((page_1_reg_1a_temp !=0)&&(dev_1e_reg_a2_temp !=0)&& (force_giga_master_mode_p9<=down_shift_force_slave_time)) 
		{
			tcMiiStationWrite(phyaddr, 0x0 ,0x1840);
			tcMiiStationWrite(phyaddr, 0x9 ,0x1600);
			tcMiiStationWrite(phyaddr, 0x0 ,0x1240);
			if(link_state_polling == 1){ printf("phy=%d force slave set \n\r",phyaddr);}
			force_giga_master_mode_p9++;	
		}}
        if( phyaddr ==10 ) { 
		if((page_1_reg_1a_temp !=0)&&(dev_1e_reg_a2_temp !=0)&& (force_giga_master_mode_p10<=down_shift_force_slave_time)) 
		{
			tcMiiStationWrite(phyaddr, 0x0 ,0x1840);
			tcMiiStationWrite(phyaddr, 0x9 ,0x1600);
			tcMiiStationWrite(phyaddr, 0x0 ,0x1240);
			if(link_state_polling == 1){ printf("phy=%d force slave set \n\r",phyaddr);}
			force_giga_master_mode_p10++;	
		}}
        if( phyaddr ==11 ) { 
		if((page_1_reg_1a_temp !=0)&&(dev_1e_reg_a2_temp !=0)&& (force_giga_master_mode_p11<=down_shift_force_slave_time)) 
		{
			tcMiiStationWrite(phyaddr, 0x0 ,0x1840);
			tcMiiStationWrite(phyaddr, 0x9 ,0x1600);
			tcMiiStationWrite(phyaddr, 0x0 ,0x1240);
			if(link_state_polling == 1){ printf("phy=%d force slave set \n\r",phyaddr);}
			force_giga_master_mode_p11++;	
		}}
        if( phyaddr ==12 ) { 
		if((page_1_reg_1a_temp !=0)&&(dev_1e_reg_a2_temp !=0)&& (force_giga_master_mode_p12<=down_shift_force_slave_time)) 
		{
			tcMiiStationWrite(phyaddr, 0x0 ,0x1840);
			tcMiiStationWrite(phyaddr, 0x9 ,0x1600);
			tcMiiStationWrite(phyaddr, 0x0 ,0x1240);
			if(link_state_polling == 1){ printf("phy=%d force slave set \n\r",phyaddr);}
			force_giga_master_mode_p12++;	
		}}
		}
			//check mean square error
			if( sw_down_shift_en ==1 ) { 
			if((sw_down_shift_p9==1)&&(phyaddr ==9)) { sw_down_shift_p9=0;tcMiiStationWrite(phyaddr, 0x09, 0x1e00);}
			if((sw_down_shift_p10==1)&&(phyaddr ==10)) { sw_down_shift_p10=0;tcMiiStationWrite(phyaddr, 0x09, 0x1e00);}
			if((sw_down_shift_p11==1)&&(phyaddr ==11)) { sw_down_shift_p11=0;tcMiiStationWrite(phyaddr, 0x09, 0x1e00);}
			if((sw_down_shift_p12==1)&&(phyaddr ==12)) { sw_down_shift_p12=0;tcMiiStationWrite(phyaddr, 0x09, 0x1e00);}
			dev_1e_reg_a2_temp=dev_1e_reg_a2 &0x0006;
			if( dev_1e_reg_a2_temp !=0 ) 
			{
		

 
			} else {

			dev_1e_reg_a2_temp = dev_1e_reg_a2 &0x0008;
			if(dev_1e_reg_a2_temp ==8)
			{
				 if( phyaddr ==9 ) { force_giga_master_mode_p9=0;}
				 if( phyaddr ==10 ) { force_giga_master_mode_p10=0;}
				 if( phyaddr ==11 ) { force_giga_master_mode_p11=0;}
				 if( phyaddr ==12 ) { force_giga_master_mode_p12=0;}

                //printf("port %d mean_square_error_counter_p10=%d --" ,phyaddr,mean_square_error_counter_p10);			
				if((ephy_monitor_p9_stop_polling==0)&&(phyaddr ==9)) { sw_downshift(phyaddr);}
				if((ephy_monitor_p10_stop_polling==0)&&(phyaddr ==10)) { sw_downshift(phyaddr);}
				if((ephy_monitor_p11_stop_polling==0)&&(phyaddr ==11)) { sw_downshift(phyaddr);}
				if((ephy_monitor_p12_stop_polling==0)&&(phyaddr ==12)) { sw_downshift(phyaddr);}
			}
			}			
			
					
		  }		
			}
        }
		if( slt_excuting==0 ) {	
		reg_temp=reg_9_temp&0x0200;		
		reg_temp1=reg1&0x0004;
		if((reg_temp != 0)&&(reg_temp1 == 0)) 								// check 1000Mbps capbility
		{
			//reg_temp=reg_9_temp&0x1000;
			reg_temp1=reg_10_temp&0x8000;
			if( phyaddr ==9 ) {	

			if((reg_temp1!=0)&&(force_giga_master_mode_p9==2))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x0600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force master
				force_giga_master_counter_p9=force_giga_master_counter_p9+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p9==2)&&(force_giga_master_counter_p9>=slave_times))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1800;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p9=0;
				force_giga_master_mode_p9=0;

			}		
			if((reg_temp1!=0)&&(force_giga_master_mode_p9==1))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p9=force_giga_master_counter_p9+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p9==1)&&(force_giga_master_counter_p9>=force_salve_time))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x0600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga auto master slave
				force_giga_master_counter_p9=0;
				force_giga_master_mode_p9++;

			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p9==0))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1800;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force master
				force_giga_master_counter_p9=force_giga_master_counter_p9+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p9==0)&&(force_giga_master_counter_p9>=master_times))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p9=0;
				force_giga_master_mode_p9++;

			}

			}
			
            if( phyaddr ==10 ) {
				
			if((reg_temp1!=0)&&(force_giga_master_mode_p10==2))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x0600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force master
				force_giga_master_counter_p10=force_giga_master_counter_p10+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p10==2)&&(force_giga_master_counter_p10>=slave_times))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1800;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p10=0;
				force_giga_master_mode_p10=0;

			}		
			if((reg_temp1!=0)&&(force_giga_master_mode_p10==1))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p10=force_giga_master_counter_p10+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p10==1)&&(force_giga_master_counter_p10>=force_salve_time))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x0600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga auto master slave
				force_giga_master_counter_p10=0;
				force_giga_master_mode_p10++;

			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p10==0))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1800;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force master
				force_giga_master_counter_p10=force_giga_master_counter_p10+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p10==0)&&(force_giga_master_counter_p10>=master_times))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p10=0;
				force_giga_master_mode_p10++;

			}
			}
			
            if( phyaddr ==11 ) {
				
			if((reg_temp1!=0)&&(force_giga_master_mode_p11==2))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x0600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force master
				force_giga_master_counter_p11=force_giga_master_counter_p11+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p11==2)&&(force_giga_master_counter_p11>=slave_times))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1800;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p11=0;
				force_giga_master_mode_p11=0;

			}		
			if((reg_temp1!=0)&&(force_giga_master_mode_p11==1))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p11=force_giga_master_counter_p11+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p11==1)&&(force_giga_master_counter_p11>=force_salve_time))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x0600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga auto master slave
				force_giga_master_counter_p11=0;
				force_giga_master_mode_p11++;

			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p11==0))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1800;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force master
				force_giga_master_counter_p11=force_giga_master_counter_p11+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p11==0)&&(force_giga_master_counter_p11>=master_times))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p11=0;
				force_giga_master_mode_p11++;

			}
			}
			
            if( phyaddr ==12 ) {	
			if((reg_temp1!=0)&&(force_giga_master_mode_p12==2))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x0600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force master
				force_giga_master_counter_p12=force_giga_master_counter_p12+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p12==2)&&(force_giga_master_counter_p12>=slave_times))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1800;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p12=0;
				force_giga_master_mode_p12=0;

			}		
			if((reg_temp1!=0)&&(force_giga_master_mode_p12==1))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p12=force_giga_master_counter_p12+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p12==1)&&(force_giga_master_counter_p12>=force_salve_time))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x0600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga auto master slave
				force_giga_master_counter_p12=0;
				force_giga_master_mode_p12++;

			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p12==0))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1800;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force master
				force_giga_master_counter_p12=force_giga_master_counter_p12+1;
			}
			if((reg_temp1!=0)&&(force_giga_master_mode_p12==0)&&(force_giga_master_counter_p12>=master_times))	// local phy is slave && collision
			{
				reg_temp=(reg_9_temp&0xe7ff)|0x1600;
			    tcMiiStationWrite(phyaddr, 0x09, reg_temp); 	// set giga force slave
				force_giga_master_counter_p12=0;
				force_giga_master_mode_p12++;

			}
			}

		}
		}

        //mtPhyReadReg(phyaddr,5);
        //mtPhyReadGReg(phyaddr,0x1E,0xA2);
		//mtPhyReadGReg(phyaddr,0x7,0x3d);
      
                        /*
        
		if(((reg9>>9)&0x1) == 1) 								// check 1000Mbps capbility
		{
			if((((reg10>>15)&0x1)==1)&&(((reg9>>12)&0x1)==1))	// local phy is manual mode && collision
			{
				tcMiiStationWrite(phyaddr, 0x09, 0x0600); 		// disable manual 
				//en75xxGePhyDisSlaveMode_flag[pn] = 0;
			}

            #if 0 
			if((mtphy_link_state[pn] == ST_LINK_UP) && (en75xxGePhyDisSlaveMode_flag[pn] == 0))
			{
				tcMiiStationWrite(phyaddr, 0x09, 0x1e00); // keep default master 
				//en75xxGePhyDisSlaveMode_flag[pn] = 1;
			}
			#endif
                        */
		

		#if 0
		#ifdef MTPHY_DEBUG
		// display message
		en75xxGePhySwDispMessage(pn);
		#endif

		#ifdef MTPHY_DEBUG
		if(sw_ErrOverMonitor_flag)
			en75xxGePhyUpdateErrOverSum(pn);
		if(sw_ErrOverMonitor_flag) 
			en75xxGePhySwErrOverMonitor(pn); // call after en75xxFEUpdateErrOverSum()
		#endif
		#endif
    }
	ephy_monitor_all_counter++;
	if(ephy_monitor_all_counter >=65535) { ephy_monitor_all_counter=0;ephy_monitor_p9_curren_counter=0;ephy_monitor_p10_curren_counter=0;ephy_monitor_p11_curren_counter=0;ephy_monitor_p12_curren_counter=0;}

}
#endif

void en75xxGePhyCfgLoad(uint8 idx)
{
	int pn, i;
	uint16 phyAddr, phyAddr_base = 9;
	current_idx = idx;

    // global registers
    for( pn=0; pn < EN75xxGe_PORTNUM; pn++)		
	{
		phyAddr = phyAddr_base + pn;
	
		TCPHYDISP3("\n 7523Ge, phyaddr= (%d,%d) \n", phyAddr_base, phyAddr);
		for( i=0; i<EN75xxGe_PHY_INIT_CL45_GDATA_LEN; i++ )
		{         
			mtEMiiRegWrite(phyAddr, en75xxGe_cfg_cl45[current_idx].gdata[i].dev_num, en75xxGe_cfg_cl45[current_idx].gdata[i].reg_num, en75xxGe_cfg_cl45[current_idx].gdata[i].val);
		}
		for( i=0; i<EN75xxGe_PHY_INIT_CL45_LDATA_LEN; i++ )
		{         
			mtEMiiRegWrite(phyAddr, en75xxGe_cfg_cl45[current_idx].ldata[i].dev_num, en75xxGe_cfg_cl45[current_idx].ldata[i].reg_num, en75xxGe_cfg_cl45[current_idx].ldata[i].val);
		}
		for( i=0; i<EN75xxGe_PHY_INIT_LDATA_LEN; i++ )
		{         
			tcMiiStationWrite(phyAddr, en75xxGe_cfg[current_idx].ldata[i].reg_num, en75xxGe_cfg[current_idx].ldata[i].val);
		}
		for( i=0; i<EN75xxGe_PHY_INIT_TR_LDATA_LEN; i++ )
		{         
			mtPhyMiiWrite_TrDbg( phyAddr, en75xxGe_cfg_tr[current_idx].ldata[i].reg_typ, en75xxGe_cfg_tr[current_idx].ldata[i].reg_num, en75xxGe_cfg_tr[current_idx].ldata[i].val,0);
		}
	}
	
	
    TCPHYDISP4("mtphy: CfgLoad %s\r\n",  en75xxGe_cfg[current_idx].name);
}
#endif  
#endif


#if 1
/************************************************************************
*                       API for EPHY (TCPHY_SUPPORT)
**************************************************************************/
// function declaration for TCEPHYDBG commands
#ifdef TCPHY_SUPPORT
static int doPhySwVer(int argc, char *argv[], void *p);
static int doPhyMiiRead (int argc, char *argv[], void *p);
static int doPhyMiiWrite(int argc, char *argv[], void *p);
//static int doPhyConfig (int argc, char *argv[], void *p);
static int doPhyGphyTestMode (int argc, char *argv[], void *p);
static int settingANSpeed (int argc, char *argv[], void *p);
static int settingloopback (int argc, char *argv[], void *p);
static int settingFlowControl (int argc, char *argv[], void *p);
static int getAllPortStatus (int argc, char *argv[], void *p);

#ifdef TCPHY_1PORT
static int doPhySpeed (int argc, char *argv[], void *p);
#endif
#ifdef TCPHY_DEBUG
//static int doPhyInit(int argc, char *argv[], void *p);
static int doPhySwPatch (int argc, char *argv[], void *p);
//static int doPhyRegCheck (int argc, char *argv[], void *p);
#ifndef LINUX_OS
static int doPhyLoopback (int argc, char *argv[], void *p);
#endif
//static int doMacSend(int argc, char *argv[], void *p);
//static int doMacSendRandom(int argc, char *argv[], void *p);
#ifndef PURE_BRIDGE
//static int doPing (int argc, char *argv[], void *p);
//static int doPingEchoCnt (int argc, char *argv[], void *p);
#endif
#ifdef LINUX_OS // allen_20130926 : merge 7502 & 7510/20
//static int doPhyDispFlag (int argc, char *argv[], void *p);
#endif
static int doPhyChkVal (int argc, char *argv[], void *p);
//static int doPhyForceLink (int argc, char *argv[], void *p);
//static int doErrMonitor (int argc, char *argv[], void *p);
#ifdef TC2031_DEBUG
static int doPhyErrOver (int argc, char *argv[], void *p);
#endif
#ifdef LINUX_OS // allen_20130926 : merge 7502 & 7510/20
#ifdef MTPHY_DEBUG
//static int doPhyInit(int argc, char *argv[], void *p);
static int doDbgPorbeSet(int argc, char *argv[], void *p);
//static int doReadPhycoeff(int argc, char *argv[], void *p);
static int doPhyMiiRead_TrDbg(int argc, char *argv[], void *p);
static int doPhyMiiWrite_TrDbg(int argc, char *argv[], void *p);
static int buck_pbus_read_command(int argc, char *argv[], void *p);
static int buck_pbus_write_command(int argc, char *argv[], void *p);
static int doPhyChkVal (int argc, char *argv[], void *p);
static int doPhyMMDRead(int argc, char *argv[], void *p);
static int doPhyMMDWrite(int argc, char *argv[], void *p);
//static int doPhyMMDWrite_eco(int argc, char *argv[], void *p);
#if 0
static int doPhyEMiiRead_60986(int argc, char *argv[], void *p);	// API for A60986
static int doPhyEMiiWrite_60986(int argc, char *argv[], void *p);	// API for A60986
#endif

#if debug_flag
static int doLpbk_setting(int argc, char *argv[], void *p);
static int doGbeTxShaper (int argc, char *argv[], void *p); 	//dai120828
static int doFeTxShaper (int argc, char *argv[], void *p); 		//dai120828
static int doPhyPktCnt(int argc, char *argv[], void *p);
#endif
static int doPhySwPatch (int argc, char *argv[], void *p);
#if debug_flag
static int doSkewDispFlag (int argc, char *argv[], void *p);
#endif
//static int doPhyDispFlag (int argc, char *argv[], void *p);
static int doPhyForceMode (int argc, char *argv[], void *p);
static int doPhySLTMode (int argc, char *argv[], void *p);
static int doPhyBootloaderMode (int argc, char *argv[], void *p);
static int doPhyForceEEE (int argc, char *argv[], void *p);
static int doPhyForceGE (int argc, char *argv[], void *p);
static int doPhyForceReset (int argc, char *argv[], void *p);
static int doPhyrestartAN (int argc, char *argv[], void *p);
static int doPhygetLinkRate (int argc, char *argv[], void *p);
static int doPhygetDuplex (int argc, char *argv[], void *p);
static int doPhygetAN (int argc, char *argv[], void *p);
static int doPhyforcePWD (int argc, char *argv[], void *p);
static int doPhygetLinkStatus (int argc, char *argv[], void *p);
static int doPhyforcePause (int argc, char *argv[], void *p);
static int doPhygetLoopback (int argc, char *argv[], void *p);
static int doPhyforceLoopback (int argc, char *argv[], void *p);
static int doPhygetLPCap (int argc, char *argv[], void *p);
static int doPhygetCALReg (int argc, char *argv[], void *p);
static int doPhyforceLinkDownPowerSaving (int argc, char *argv[], void *p);
static int doPhyset7530_15R (int argc, char *argv[], void *p);
static int doPhyset7530_15R_L (int argc, char *argv[], void *p); 	// 20161129 for Lecroy scope 
static int doPhyset7530_0R_L (int argc, char *argv[], void *p);  	// 20161223 for Lecroy scope 
static int doPhyset7530_5R (int argc, char *argv[], void *p);		// allen_20170202
static int doPhyset751221_5R (int argc, char *argv[], void *p);		// allen_20170621
static int doPhyForceLED (int argc, char *argv[], void *p);
static int doReadAllCalData(int argc, char *argv[], void *p);
static int doPbusRead (int argc, char *argv[], void *p);
static int doPbusWrite (int argc, char *argv[], void *p);
static int sw_ability_setting(int argc, char *argv[], void *p);
//static int doTrgmiiRxCal (void);

#if 0 //debug_flag
static int doTrgmiiPhyInit (void);
#endif

#endif
#endif
#endif // TCPHY_DEBUG
#ifdef TCPHY_FIELD_DBGTEST
static int doFieldDebugTest (int argc, char *argv[], void *p);
static int doFieldDebugPrint (int argc, char *argv[], void *p);
#endif // TCPHY_FIELD_DBGTEST
static const cmds_t ethertphyprint[] =
{
    {"ver",       		doPhySwVer,           	0x02, 0, NULL},
    {"miir",      		doPhyMiiRead,         	0x02, 0, NULL},
    {"miiw",      		doPhyMiiWrite,        	0x02, 0, NULL},
    //{"config",    	doPhyConfig,          	0x02, 0, NULL},
   	{"testmode",  		doPhyGphyTestMode,		0x02, 0, "tce testmode <speed> <speed> <mode> <pair>"},
	{"cal_all_data",  	doReadAllCalData,		0x02, 0, NULL},
	{"sw_ability_set",  sw_ability_setting,		0x02, 1, "sw_ability_set <SDS|FS> <on|ff>"},
	{"bpr",  	buck_pbus_read_command,		0x02, 0, "bpr <phyaddr> <register>"},
	{"bpw",  	buck_pbus_write_command,		0x02, 0, "bpw <phyaddr> <register> <value>"},
	#ifdef TCPHY_DEBUG
    //{"reset",     	doPhyInit,          	0x02, 0, NULL},     
    {"swpatch",   		doPhySwPatch,       	0x02, 0, NULL},
    //{"regcheck",  	doPhyRegCheck,  		0x02, 0, NULL},   
    //{"send",      		doMacSend,      		0x02, 0x3, "send <pattern> <len> <loop>"},
    //{"sendrandom", 		doMacSendRandom, 		0x02, 0, NULL},
	#if 0
	#ifndef PURE_BRIDGE     
    {"ping",      		doPing,        			0x02, 3, "ping <ip> <len> <loopnum> [delay_cnt]"},
    {"pingechocnt", 	doPingEchoCnt, 			0x02, 1, "pingechocnt <display | clear>"},
	#endif
	#endif
    //{"dispflag",    	doPhyDispFlag,  		0x02, 0, NULL},          
    //{"forcelink",   	doPhyForceLink,  		0x02, 0, NULL},      
    //{"errmonitor",  	doErrMonitor,         	0x02, 0, NULL},    
	#ifdef LINUX_OS // allen_20130926 : merge 7502 & 7510/20
	#ifdef MTPHY_DEBUG 
	{"chkval",  		doPhyChkVal,  			0x02, 1, "chkval <phyaddr>"},  
	{"emiir",			doPhyMMDRead,		  	0x02, 0, NULL},
	{"emiiw",			doPhyMMDWrite,		  	0x02, 0, NULL},
	//{"emiiwb",		doPhyMMDWrite,		  	0x02, 0, NULL},
	//{"miiwb",	  		doPhyMiiWrite,			0x02, 0, NULL},
	{"dbgprb",	  		doDbgPorbeSet,			0x02, 0, NULL},
	{"miir_trdbg",		doPhyMiiRead_TrDbg,		0x02, 0, NULL},
	{"miiw_trdbg",  	doPhyMiiWrite_TrDbg,	0x02, 0, NULL},
	{"pbr",				doPbusRead,		  	    0x02, 0, NULL},
	{"pbw",				doPbusWrite,		  	    0x02, 0, NULL},
	//{"emiiw_eco",		doPhyMMDWrite_eco,		  	0x02, 0, NULL},
	#if 0
	{"emiir_60986",		doPhyEMiiRead_60986,	0x02, 0, NULL},		// API for 60986
	{"emiiw_60986",		doPhyEMiiWrite_60986,	0x02, 0, NULL},		// API for 60986
	#endif
	//{"coeffr", 		doReadPhycoeff, 		0x02, 0, NULL},
	//{"lbpk",			doLpbk_setting,			0x02, 0, NULL},
	//{"gtxshaper", 	doGbeTxShaper, 			0x02, 0, NULL},
	//{"ftxshaper", 	doFeTxShaper, 			0x02, 0, NULL},
	//{"phycnt",		doPhyPktCnt,			0x02, 0, NULL},
	//{"swpatch",   	doPhySwPatch,         	0x02, 1, "swpatch 		<on|off>"},
	//{"dispflag",    	doPhyDispFlag,  		0x02, 0, NULL},
	//{"Skewdispflag", 	doSkewDispFlag,  		0x02, 0, NULL},
	{"setANSpeed", 		settingANSpeed,			0x02, 1, "setANSpeed <phyaddr> <speed> <dupex>"},
	{"setLoopback", 	settingloopback,	    0x02, 1, "setLoopback <phyaddr> <mode>"},
	{"flowControl", 	settingFlowControl,	    0x02, 1, "flowControl <phyaddr> <on|off>"},
	{"PortStatus", 		getAllPortStatus,	    0x02, 0, "PortStatus"},
	{"forcemode",  		doPhyForceMode,  		0x02, 1, "forcemode <phyaddr> <an|force> <10|100|1000> <full|half>"}, 
	{"sltmode",  		doPhySLTMode,  			0x02, 1, "sltmode <phyaddr> <10|100|1000>"}, 
	{"bootloadermode",  doPhyBootloaderMode,  	0x02, 1, "bootloadrmode <phyaddr> <on|off>"}, 
	{"forceEEE",  		doPhyForceEEE,  		0x02, 1, "forceEEE <phyaddr> <on|off|recover>"},
	{"forceGE",  		doPhyForceGE,  			0x02, 1, "forceGE <phyaddr> <multi|single> <master|slave>"},
	{"forceReset", 		doPhyForceReset,		0x02, 1, "forceReset <phyaddr> "},
	{"restartAN", 		doPhyrestartAN,			0x02, 1, "restartAN <phyaddr> "}, 
	{"getLinkRate", 	doPhygetLinkRate,		0x02, 1, "getLinkRate <phyaddr> "}, 
	{"getDuplex", 		doPhygetDuplex,			0x02, 1, "getDuplex <phyaddr> "},
	{"getAN", 			doPhygetAN,				0x02, 1, "getAN <phyaddr> "},
	{"forcePWD", 		doPhyforcePWD,			0x02, 1, "forcePWD <phyaddr> "},
	{"getLinkStatus", 	doPhygetLinkStatus,		0x02, 1, "getLinkStatus <phyaddr> "},
	{"forcePause", 		doPhyforcePause,		0x02, 1, "forcePause <phyaddr> <on|off> "},
	{"getLoopback", 	doPhygetLoopback,		0x02, 1, "getLoopback <phyaddr> "},
	{"forceLoopback", 	doPhyforceLoopback,		0x02, 1, "forceLoopback <phyaddr> "},
	{"getLPCap", 		doPhygetLPCap,			0x02, 1, "getLPCap <phyaddr> "},
	{"getCAL", 			doPhygetCALReg,			0x02, 1, "getCAL <phyaddr> "}, 
	{"forceLDPS", 		doPhyforceLinkDownPowerSaving,	0x02, 1, "forceLDPS <phyaddr> <on|off> "}, 
	{"set7530_15R", 	doPhyset7530_15R,		0x02, 1, "set7530_15R <on|off>"}, 
	{"set7530_15R_L", 	doPhyset7530_15R_L,		0x02, 1, "set7530_15R_L <on|off>"},		// 20161129 for Lecroy scope
	{"set7530_0R_L", 	doPhyset7530_0R_L,		0x02, 1, "set7530_0R_L <on|off>"},		// 20161226 for Lecroy scope	
	{"set7530_5R", 		doPhyset7530_5R,		0x02, 1, "set7530_5R <on|off>"},		// allen_20170202 	
	{"set751221_5R", 	doPhyset751221_5R,		0x02, 1, "set751221_5R <on|off>"},		// allen_20170621
	{"forceLED", 		doPhyForceLED,			0x02, 1, "forceLED <phyaddr> <enable|disable>"}, 
	#if 0 //debug_flag
	{"trgmii_phy_init", doTrgmiiPhyInit,		0x02, 0, NULL}, 	// YMC_20170607 
	#endif
	#endif
	#endif
	#endif // TCPHY_DEBUG
	#ifdef TCPHY_FIELD_DBGTEST
    //{"dbgtest",  	doFieldDebugTest, 		0x02, 0, NULL},      
    //{"dbgprint", 	doFieldDebugPrint, 		0x02, 0, NULL},
	#endif // TCPHY_FIELD_DBGTEST   
    {NULL,			NULL,					0,	0,	NULL},
};

/*** public, Called by tc3162l2mac.c ***/
int tcPhyPortNumGet(void)
{
	if(tcPhyVer != 99)
		return tcPhyPortNum;
	else
		return 0;
}

// ************************************************************************
// 		set tcPhyFlag & tcPhyVer
//		[in] ephy_addr
// ************************************************************************
//int tcPhyVerLookUp(macAdapter_t *mac_p)
int tcPhyVerLookUp(uint16 ephy_addr)
{
    uint32 rval;
	uint16 r15_temp;

	ephy_addr_base = 0;

	if(tcPhyVer == mtPhyVer_7530)
		mt7530_flag = 1;

	TCPHYDISP3(" [tcPhyVerLookUp] (%d)...in \r\n", ephy_addr);
	
	r15_temp = tcMiiStationRead(ephy_addr, 15);
	//printk(" [tcPhyVerLookUp] (%d)...in...in...in...in...in, r15=0x%x \r\n", ephy_addr, r15_temp);

    rval = tcMiiStationRead(ephy_addr, 3); 				// phy revision id
	if(rval == 0xffff)
	{
		#ifdef LINUX_OS 
		rval = tcMiiStationRead(ephy_addr, 3); 			// phy revision id
		#endif
	}
	else
	{
		fgMT7530_INT = 1;
	}	
    rval &= 0xffff;
	printk("%s %d %X \n", __func__, __LINE__, rval);  	// JasonG_7512
    if(rval == EPHY_ID_2031)
	{
        tcPhyVer = tcPhyVer_2031;         
        tcPhyPortNum = 1;
        TCPHYDISP1("TC2031, ");
    }
    #if debug_flag
	else if(rval == EPHY_ID_2101mb)
    {
        tcPhyVer = tcPhyVer_2101mb;
        tcPhyPortNum = 1;
        TCPHYDISP1("TC2101MB, ");
    }
	else if(rval == EPHY_ID_2104mc)
	{
        tcPhyVer = tcPhyVer_2104mc;
        tcPhyPortNum = 4;
        TCPHYDISP1("TC2104MC, ");
    }
	else if(rval == EPHY_ID_2104sd)
	{
        tcPhyVer = tcPhyVer_2104sd;
        tcPhyPortNum = 4;
        TCPHYDISP1("TC2104SD, ");
    }
	else if (rval == EPHY_ID_2101me)
	{
        tcPhyVer = tcPhyVer_2101me;
        tcPhyPortNum = 1;
        TCPHYDISP1("TC2101ME, ");
	}
	else if(rval == EPHY_ID_2102me)
	{
        tcPhyVer = tcPhyVer_2102me;
        tcPhyPortNum = 1;
        TCPHYDISP1("TC2102ME, ");	
	}
	else if (rval == EPHY_ID_2104me)
	{
        tcPhyVer = tcPhyVer_2104me;
        tcPhyPortNum = 4;
        TCPHYDISP1("TC2104ME, ");
	}
	else if(rval == EPHY_ID_2101mf)
	{
        tcPhyVer = tcPhyVer_2101mf;
        tcPhyPortNum = 1;
        TCPHYDISP1("TC2101MF, ");
    }
	else if(rval == EPHY_ID_2105sg)
	{
        tcPhyVer = tcPhyVer_2105sg;
        tcPhyPortNum = 5;
        TCPHYDISP1("TC2105SG, ");
	}
	else if(rval == EPHY_ID_2101mi)
	{
        tcPhyVer = tcPhyVer_2101mi;
        tcPhyPortNum = 1;
        TCPHYDISP1("TC2101MI, ");	
   	}
	else if(rval == EPHY_ID_2105mj)
	{
        tcPhyVer = tcPhyVer_2105mj;
        tcPhyPortNum = 5;
        TCPHYDISP1("TC2105MJ, "); 
    }
	else if(rval == EPHY_ID_2105sk)
	{
        tcPhyVer = tcPhyVer_2105sk;
        tcPhyPortNum = 5;
        TCPHYDISP1("TC2105SK, ");
    }
	else if(rval == EPHY_ID_2101mm)
	{
        tcPhyVer = tcPhyVer_2101mm;
		tcPhyPortNum = 1;
		TCPHYDISP1("TC2101MM, ");
   	}
	else if(rval == EPHY_ID_7510Fe)
	{
        tcPhyVer = tcPhyVer_mt7510FE;
        tcPhyPortNum = 4;
        TCPHYDISP1(" 7510 Fe, \n");
        tcPhyFlag = 1;
        tcPhyInitFlag = 1;
    } 
	else if(rval == EPHY_ID_7505Fe)
	{
        tcPhyVer = tcPhyVer_mt7505FE;
        tcPhyPortNum = 5;
        TCPHYDISP1(" 7505 Fe, \n");	
    } 
	else if(rval == EPHY_ID_7530Ge_7512Fe)
	{ 	
		if((r15_temp & 0x2000)== 0x2000)
		{
			//printk("r15 = 0x%x, (0x%x) \n", r15_temp, (r15_temp & 0x2000));
			
	        tcPhyVer = mtPhyVer_7530;
    	    tcPhyPortNum = 5;
        	TCPHYDISP1(" 7530 Ge, 0x%x \n", fgMT7530_INT); 
        	tcPhyFlag = 1;
        	tcPhyInitFlag = 1;
        	mt7530_flag = 1;
        }
        else  // *********************************************************
        {
        	//printk("r15 = 0x%x, (0x%x) \n", r15_temp, (r15_temp & 0x2000));

			tcPhyVer = tcPhyVer_7512Fe;
    	    tcPhyPortNum = 4;
        	TCPHYDISP1(" 7512 Fe, \n");
        	tcPhyFlag = 1;
        	tcPhyInitFlag = 1;
        }
    } 
	else if(rval == EPHY_ID_7510Ge)
	{ 
        tcPhyVer = mtPhyVer_7510Ge;  
        tcPhyPortNum = 1;
		//eco_rev = VPint(0xbfb00064);
        TCPHYDISP1(" 7510 Ge, 0x%x \n", fgMT7510Ge_INT);		// internal check flag 
        tcPhyFlag = 1;
        tcPhyInitFlag = 1;
    }
    else if(rval == EPHY_ID_7512Ge)  // ***************************
	{
        tcPhyVer = tcPhyVer_7512Ge;		
        tcPhyPortNum = 1;
        TCPHYDISP1(" 7512 Ge, \n");
        tcPhyFlag = 1;
        tcPhyInitFlag = 1;
    } 
    else if(rval == EPHY_ID_7522Fe)  
	{
        tcPhyVer = tcPhyVer_7522Fe;		
        tcPhyPortNum = 3;
        TCPHYDISP1(" 7522 Fe, \n");
        tcPhyFlag = 1;
        tcPhyInitFlag = 1;
    } 
    else if(rval == EPHY_ID_7522Ge)  
	{
        tcPhyVer = tcPhyVer_7522Ge;		
        tcPhyPortNum = 1;
        TCPHYDISP1(" 7522 Ge, \n");
        tcPhyFlag = 1;
        tcPhyInitFlag = 1;
    }
    #endif
    else if(rval == EPHY_ID_7516Ge)  
	{
        tcPhyVer = tcPhyVer_7516Ge;		
        tcPhyPortNum = 5;
        TCPHYDISP1(" 7516 Ge, \n");
        tcPhyFlag = 1;
        tcPhyInitFlag = 1;
    }
    else if(rval == EPHY_ID_7523Ge)  
	{
        tcPhyVer = tcPhyVer_7523Ge;		
        tcPhyPortNum = 4;
        TCPHYDISP1(" 7523 Ge, \n");
        tcPhyFlag = 1;
        tcPhyInitFlag = 1;
    }
    else 
	{
        TCPHYDISP3(" unknown PHYID: %lx, \n", rval);        
        tcPhyInitFlag = 0;
    }

	if(tcPhyVer == mtPhyVer_7530)
	{ 	//CML_20130226_2
		eco_rev = (mtEMiiRegRead(ephy_addr, 0x1f, 0) >> 4);
	}
	else
	{
		eco_rev = tcMiiStationRead(ephy_addr, 31);
    }
	eco_rev &= (0x0f);
	
	if(mt7530_flag == 1)
	{
		tcPhyVer = mtPhyVer_7530;
        tcPhyPortNum = 5;
        TCPHYDISP1("MCM MT7530 \n");
	}
	TCPHYDISP3(" [tcPhyVerLookUp] (%d)(%d)...out	 \r\n", ephy_addr, tcPhyVer);
	//printk(" [tcPhyVerLookUp] (%d)(%d)...out...out...out...out...out	 \r\n", ephy_addr, tcPhyVer);

	if(ephy_addr_base == 0) // allen_20140822
	{
		if(mt7530_flag == 0)
		{
			if((tcPhyVer == tcPhyVer_7523Ge))	
				ephy_addr_base += EPHY_ADDR_P0;
		}
	}
	else
	{
		if(tcPhyVer == mtPhyVer_7530)
			ephy_addr_base = 0;
	}
	
    return 0;
}



int phyTxAmpcomp(int show_or_comp)
{ 
	int	P9_tx_amp_hbt_cha_comp,P9_tx_amp_hbt_chb_comp;
	int	P10_tx_amp_hbt_cha_comp,P10_tx_amp_hbt_chb_comp;
	int	P11_tx_amp_hbt_cha_comp,P11_tx_amp_hbt_chb_comp;
	int	P12_tx_amp_hbt_cha_comp,P12_tx_amp_hbt_chb_comp;	
	int	P9_tx_amp_tbt_cha_comp,P9_tx_amp_tbt_chb_comp;
	int	P10_tx_amp_tbt_cha_comp,P10_tx_amp_tbt_chb_comp;
	int	P11_tx_amp_tbt_cha_comp,P11_tx_amp_tbt_chb_comp;
	int	P12_tx_amp_tbt_cha_comp,P12_tx_amp_tbt_chb_comp;
	int	P9_tx_amp_tst_cha_comp,P9_tx_amp_tst_chb_comp,P9_tx_amp_tst_chc_comp,P9_tx_amp_tst_chd_comp;
	int	P10_tx_amp_tst_cha_comp,P10_tx_amp_tst_chb_comp,P10_tx_amp_tst_chc_comp,P10_tx_amp_tst_chd_comp;
	int	P11_tx_amp_tst_cha_comp,P11_tx_amp_tst_chb_comp,P11_tx_amp_tst_chc_comp,P11_tx_amp_tst_chd_comp;
	int	P12_tx_amp_tst_cha_comp,P12_tx_amp_tst_chb_comp,P12_tx_amp_tst_chc_comp,P12_tx_amp_tst_chd_comp;
	int	P9_tx_amp_gbe_cha_comp,P9_tx_amp_gbe_chb_comp,P9_tx_amp_gbe_chc_comp,P9_tx_amp_gbe_chd_comp;
	int	P10_tx_amp_gbe_cha_comp,P10_tx_amp_gbe_chb_comp,P10_tx_amp_gbe_chc_comp,P10_tx_amp_gbe_chd_comp;
	int	P11_tx_amp_gbe_cha_comp,P11_tx_amp_gbe_chb_comp,P11_tx_amp_gbe_chc_comp,P11_tx_amp_gbe_chd_comp;
	int	P12_tx_amp_gbe_cha_comp,P12_tx_amp_gbe_chb_comp,P12_tx_amp_gbe_chc_comp,P12_tx_amp_gbe_chd_comp;
	unsigned int  reg_1e_12_read,reg_1e_16_read,reg_1e_17_read,reg_1e_18_read,reg_1e_19_read,reg_1e_20_read,reg_1e_21_read,reg_1e_22_read,reg_1e_174_read,reg_1e_175_read;
    unsigned int  reg_1e_12_write=0,reg_1e_16_write=0,reg_1e_17_write=0,reg_1e_18_write=0,reg_1e_19_write=0,reg_1e_20_write=0,reg_1e_21_write=0,reg_1e_22_write=0,reg_1e_174_write=0,reg_1e_175_write=0;
	int  reg_1e_12_header,reg_1e_16_header,reg_1e_17_header,reg_1e_18_header,reg_1e_19_header,reg_1e_20_header,reg_1e_21_header,reg_1e_22_header,reg_1e_174_header,reg_1e_175_header;
	int  reg_1e_12_end,reg_1e_16_end,reg_1e_17_end,reg_1e_18_end,reg_1e_19_end,reg_1e_20_end,reg_1e_21_end,reg_1e_22_end,reg_1e_174_end,reg_1e_175_end;
	uint16	start_phy=9,end_phy=12,phy_loop,status_flag=0;
	int txamp_low_limit=-3,txamp_high_limit=0x42;
    int r50_low_limit=-8,r50_high_limit=0x88; 
	int	P9_r50_cha_comp,P9_r50_chb_comp,P9_r50_chc_comp,P9_r50_chd_comp;
	int	P10_r50_cha_comp,P10_r50_chb_comp,P10_r50_chc_comp,P10_r50_chd_comp;
	int	P11_r50_cha_comp,P11_r50_chb_comp,P11_r50_chc_comp,P11_r50_chd_comp;
	int	P12_r50_cha_comp,P12_r50_chb_comp,P12_r50_chc_comp,P12_r50_chd_comp;

	// =============================================================================================
	// test mode TX amp setting
	// =============================================================================================
      P9_tx_amp_tst_cha_comp=0;P10_tx_amp_tst_cha_comp=0;P11_tx_amp_tst_cha_comp=0;P12_tx_amp_tst_cha_comp=0;
      P9_tx_amp_tst_chb_comp=0;P10_tx_amp_tst_chb_comp=0;P11_tx_amp_tst_chb_comp=0;P12_tx_amp_tst_chb_comp=0;
      P9_tx_amp_tst_chc_comp=0;P10_tx_amp_tst_chc_comp=0;P11_tx_amp_tst_chc_comp=0;P12_tx_amp_tst_chc_comp=0;
      P9_tx_amp_tst_chd_comp=0;P10_tx_amp_tst_chd_comp=0;P11_tx_amp_tst_chd_comp=0;P12_tx_amp_tst_chd_comp=0;
	// =============================================================================================
	// Giga TX amp setting
	// =============================================================================================
      P9_tx_amp_gbe_cha_comp=0;P10_tx_amp_gbe_cha_comp=0;P11_tx_amp_gbe_cha_comp=0;P12_tx_amp_gbe_cha_comp=0;
      P9_tx_amp_gbe_chb_comp=0;P10_tx_amp_gbe_chb_comp=0;P11_tx_amp_gbe_chb_comp=0;P12_tx_amp_gbe_chb_comp=0;
      P9_tx_amp_gbe_chc_comp=0;P10_tx_amp_gbe_chc_comp=0;P11_tx_amp_gbe_chc_comp=0;P12_tx_amp_gbe_chc_comp=0;
      P9_tx_amp_gbe_chd_comp=0;P10_tx_amp_gbe_chd_comp=0;P11_tx_amp_gbe_chd_comp=0;P12_tx_amp_gbe_chd_comp=0;
	// =============================================================================================
	// 100M TX amp setting
	// =============================================================================================
      P9_tx_amp_hbt_cha_comp=0;P10_tx_amp_hbt_cha_comp=0;P11_tx_amp_hbt_cha_comp=0;P12_tx_amp_hbt_cha_comp=0;
      P9_tx_amp_hbt_chb_comp=0;P10_tx_amp_hbt_chb_comp=0;P11_tx_amp_hbt_chb_comp=0;P12_tx_amp_hbt_chb_comp=0;
	// =============================================================================================
	// 10M TX amp setting
	// =============================================================================================
      P9_tx_amp_tbt_cha_comp=0;P10_tx_amp_tbt_cha_comp=0;P11_tx_amp_tbt_cha_comp=0;P12_tx_amp_tbt_cha_comp=0;
      P9_tx_amp_tbt_chb_comp=0;P10_tx_amp_tbt_chb_comp=0;P11_tx_amp_tbt_chb_comp=0;P12_tx_amp_tbt_chb_comp=0;
 

      // =============================================================================================
	// R50 setting
	// =============================================================================================
     P9_r50_cha_comp=0;P9_r50_chb_comp=0;P9_r50_chc_comp=0;P9_r50_chd_comp=0;
	 P10_r50_cha_comp=0;P10_r50_chb_comp=0;P10_r50_chc_comp=0;P10_r50_chd_comp=0;
	 P11_r50_cha_comp=0;P11_r50_chb_comp=0;P11_r50_chc_comp=0;P11_r50_chd_comp=0;
	 P12_r50_cha_comp=0;P12_r50_chb_comp=0;P12_r50_chc_comp=0;P12_r50_chd_comp=0;
	// =============================================================================================
	// test mode Coarse TX amp setting
	// =============================================================================================
      P9_tx_amp_tst_cha_comp=P9_tx_amp_tst_cha_comp+port9_1G_Coarse_adjustment;
      P9_tx_amp_tst_chb_comp=P9_tx_amp_tst_chb_comp+port9_1G_Coarse_adjustment;
      P9_tx_amp_tst_chc_comp=P9_tx_amp_tst_chc_comp+port9_1G_Coarse_adjustment;
      P9_tx_amp_tst_chd_comp=P9_tx_amp_tst_chd_comp+port9_1G_Coarse_adjustment;

      P10_tx_amp_tst_cha_comp=P10_tx_amp_tst_cha_comp+port10_1G_Coarse_adjustment;
      P10_tx_amp_tst_chb_comp=P10_tx_amp_tst_chb_comp+port10_1G_Coarse_adjustment;
      P10_tx_amp_tst_chc_comp=P10_tx_amp_tst_chc_comp+port10_1G_Coarse_adjustment;
      P10_tx_amp_tst_chd_comp=P10_tx_amp_tst_chd_comp+port10_1G_Coarse_adjustment;

      P11_tx_amp_tst_cha_comp=P11_tx_amp_tst_cha_comp+port11_1G_Coarse_adjustment;
      P11_tx_amp_tst_chb_comp=P11_tx_amp_tst_chb_comp+port11_1G_Coarse_adjustment;
      P11_tx_amp_tst_chc_comp=P11_tx_amp_tst_chc_comp+port11_1G_Coarse_adjustment;
      P11_tx_amp_tst_chd_comp=P11_tx_amp_tst_chd_comp+port11_1G_Coarse_adjustment;

      P12_tx_amp_tst_cha_comp=P12_tx_amp_tst_cha_comp+port12_1G_Coarse_adjustment;
      P12_tx_amp_tst_chb_comp=P12_tx_amp_tst_chb_comp+port12_1G_Coarse_adjustment;
      P12_tx_amp_tst_chc_comp=P12_tx_amp_tst_chc_comp+port12_1G_Coarse_adjustment;
      P12_tx_amp_tst_chd_comp=P12_tx_amp_tst_chd_comp+port12_1G_Coarse_adjustment;

	// =============================================================================================
	// Giga TX Coarse amp setting
	// =============================================================================================
      P9_tx_amp_gbe_cha_comp=P9_tx_amp_gbe_cha_comp+port9_1G_Coarse_adjustment;
      P9_tx_amp_gbe_chb_comp=P9_tx_amp_gbe_chb_comp+port9_1G_Coarse_adjustment;	  
      P9_tx_amp_gbe_chc_comp=P9_tx_amp_gbe_chc_comp+port9_1G_Coarse_adjustment;	  
	  P9_tx_amp_gbe_chd_comp=P9_tx_amp_gbe_chd_comp+port9_1G_Coarse_adjustment;  
	  
      P10_tx_amp_gbe_cha_comp=P10_tx_amp_gbe_cha_comp+port10_1G_Coarse_adjustment;
      P10_tx_amp_gbe_chb_comp=P10_tx_amp_gbe_chb_comp+port10_1G_Coarse_adjustment;	  
      P10_tx_amp_gbe_chc_comp=P10_tx_amp_gbe_chc_comp+port10_1G_Coarse_adjustment;	  
	  P10_tx_amp_gbe_chd_comp=P10_tx_amp_gbe_chd_comp+port10_1G_Coarse_adjustment;  	  
	  
      P11_tx_amp_gbe_cha_comp=P11_tx_amp_gbe_cha_comp+port11_1G_Coarse_adjustment;
      P11_tx_amp_gbe_chb_comp=P11_tx_amp_gbe_chb_comp+port11_1G_Coarse_adjustment;	  
      P11_tx_amp_gbe_chc_comp=P11_tx_amp_gbe_chc_comp+port11_1G_Coarse_adjustment;	  
	  P11_tx_amp_gbe_chd_comp=P11_tx_amp_gbe_chd_comp+port11_1G_Coarse_adjustment;

      P12_tx_amp_gbe_cha_comp=P12_tx_amp_gbe_cha_comp+port12_1G_Coarse_adjustment;
      P12_tx_amp_gbe_chb_comp=P12_tx_amp_gbe_chb_comp+port12_1G_Coarse_adjustment;	  
      P12_tx_amp_gbe_chc_comp=P12_tx_amp_gbe_chc_comp+port12_1G_Coarse_adjustment;	  
	  P12_tx_amp_gbe_chd_comp=P12_tx_amp_gbe_chd_comp+port12_1G_Coarse_adjustment;  	  
	  
	// =============================================================================================
	// 100M Coarse TX amp setting
	// =============================================================================================
      P9_tx_amp_hbt_cha_comp=P9_tx_amp_hbt_cha_comp+port9_100M_Coarse_adjustment;
      P9_tx_amp_hbt_chb_comp=P9_tx_amp_hbt_chb_comp+port9_100M_Coarse_adjustment;

      P10_tx_amp_hbt_cha_comp=P10_tx_amp_hbt_cha_comp+port10_100M_Coarse_adjustment;
      P10_tx_amp_hbt_chb_comp=P10_tx_amp_hbt_chb_comp+port10_100M_Coarse_adjustment;

      P11_tx_amp_hbt_cha_comp=P11_tx_amp_hbt_cha_comp+port11_100M_Coarse_adjustment;
      P11_tx_amp_hbt_chb_comp=P11_tx_amp_hbt_chb_comp+port11_100M_Coarse_adjustment;

      P12_tx_amp_hbt_cha_comp=P12_tx_amp_hbt_cha_comp+port12_100M_Coarse_adjustment;
      P12_tx_amp_hbt_chb_comp=P12_tx_amp_hbt_chb_comp+port12_100M_Coarse_adjustment;	  
		 
			if(show_or_comp == 0)
			{
			printf("port9 1G Coarse= %d " , port9_1G_Coarse_adjustment);
			printf("port10 1G Coarse= %d " , port10_1G_Coarse_adjustment);
			printf("port11 1G Coarse= %d " , port11_1G_Coarse_adjustment);
			printf("port12 1G Coarse= %d \r\n" , port12_1G_Coarse_adjustment);
			printf("port9 100M Coarse= %d " , port9_100M_Coarse_adjustment);
			printf("port10 100M Coarse= %d " , port10_100M_Coarse_adjustment);
			printf("port11 100M Coarse= %d " , port11_100M_Coarse_adjustment);
			printf("port12 100M Coarse= %d \r\n" , port12_100M_Coarse_adjustment);
			printf("port9A  tst= %d " , P9_tx_amp_tst_cha_comp);
			printf("port9B  tst= %d " , P9_tx_amp_tst_chb_comp);
			printf("port9C  tst= %d " , P9_tx_amp_tst_chc_comp);
			printf("port9D  tst= %d \n\r" , P9_tx_amp_tst_chd_comp);
			printf("port10A tst= %d " , P10_tx_amp_tst_cha_comp);
			printf("port10B tst= %d " , P10_tx_amp_tst_chb_comp);
			printf("port10C tst= %d " , P10_tx_amp_tst_chc_comp);
			printf("port10D tst= %d \n\r" , P10_tx_amp_tst_chd_comp);
			printf("port11A tst= %d " , P11_tx_amp_tst_cha_comp);
			printf("port11B tst= %d " , P11_tx_amp_tst_chb_comp);
			printf("port11C tst= %d " , P11_tx_amp_tst_chc_comp);
			printf("port11D tst= %d \n\r" , P11_tx_amp_tst_chd_comp);
			printf("port12A tst= %d " , P12_tx_amp_tst_cha_comp);
			printf("port12B tst= %d " , P12_tx_amp_tst_chb_comp);
			printf("port12C tst= %d " , P12_tx_amp_tst_chc_comp);
			printf("port12D tst= %d \n\r" , P12_tx_amp_tst_chd_comp);
			
			printf("port9A  gbe= %d " , P9_tx_amp_gbe_cha_comp);
			printf("port9B  gbe= %d " , P9_tx_amp_gbe_chb_comp);
			printf("port9C  gbe= %d " , P9_tx_amp_gbe_chc_comp);
			printf("port9D  gbe= %d \n\r" , P9_tx_amp_gbe_chd_comp);
			printf("port10A gbe= %d " , P10_tx_amp_gbe_cha_comp);
			printf("port10B gbe= %d " , P10_tx_amp_gbe_chb_comp);
			printf("port10C gbe= %d " , P10_tx_amp_gbe_chc_comp);
			printf("port10D gbe= %d \n\r" , P10_tx_amp_gbe_chd_comp);
			printf("port11A gbe= %d " , P11_tx_amp_gbe_cha_comp);
			printf("port11B gbe= %d " , P11_tx_amp_gbe_chb_comp);
			printf("port11C gbe= %d " , P11_tx_amp_gbe_chc_comp);
			printf("port11D gbe= %d \n\r" , P11_tx_amp_gbe_chd_comp);
			printf("port12A gbe= %d " , P12_tx_amp_gbe_cha_comp);
			printf("port12B gbe= %d " , P12_tx_amp_gbe_chb_comp);
			printf("port12C gbe= %d " , P12_tx_amp_gbe_chc_comp);
			printf("port12D gbe= %d \n\r" , P12_tx_amp_gbe_chd_comp);
			
			printf("port9A  hbt= %d " , P9_tx_amp_hbt_cha_comp);
			printf("port9B  hbt= %d " , P9_tx_amp_hbt_chb_comp);
			printf("port10A hbt= %d " , P10_tx_amp_hbt_cha_comp);
			printf("port10B hbt= %d \n\r" , P10_tx_amp_hbt_chb_comp);
			printf("port11A hbt= %d " , P11_tx_amp_hbt_cha_comp);
			printf("port11B hbt= %d " , P11_tx_amp_hbt_chb_comp);
			printf("port12A hbt= %d " , P12_tx_amp_hbt_cha_comp);
			printf("port12B hbt= %d \n\r" , P12_tx_amp_hbt_chb_comp);
			
			printf("port9A  tbt= %d " , P9_tx_amp_tbt_cha_comp);
			printf("port9B  tbt= %d " , P9_tx_amp_tbt_chb_comp);
			printf("port10A tbt= %d " , P10_tx_amp_tbt_cha_comp);
			printf("port10B tbt= %d \n\r" , P10_tx_amp_tbt_chb_comp);
			printf("port11A tbt= %d " , P11_tx_amp_tbt_cha_comp);
			printf("port11B tbt= %d " , P11_tx_amp_tbt_chb_comp);
			printf("port12A tbt= %d " , P12_tx_amp_tbt_cha_comp);
			printf("port12B tbt= %d \n\r" , P12_tx_amp_tbt_chb_comp);
			
			printf("port9A  R50= %d   " , P9_r50_cha_comp);
			printf("port9B  R50= %d   " , P9_r50_chb_comp);
			printf("port9C  R50= %d   " , P9_r50_chc_comp);
			printf("port9D  R50= %d \n\r" , P9_r50_chd_comp);
			printf("port10A R50= %d   " , P10_r50_cha_comp);
			printf("port10B R50= %d   " , P10_r50_chb_comp);
			printf("port10C R50= %d   " , P10_r50_chc_comp);
			printf("port10D R50= %d \n\r" , P10_r50_chd_comp);
			printf("port11A R50= %d   " , P11_r50_cha_comp);
			printf("port11B R50= %d   " , P11_r50_chb_comp);
			printf("port11C R50= %d   " , P11_r50_chc_comp);
			printf("port11D R50= %d \n\r" , P11_r50_chd_comp);
			printf("port12A R50= %d   " , P12_r50_cha_comp);
			printf("port12B R50= %d   " , P12_r50_chb_comp);
			printf("port12C R50= %d   " , P12_r50_chc_comp);
			printf("port12D R50= %d \n\r" , P12_r50_chd_comp);

            return 0;			
			}




	for(phy_loop = start_phy; phy_loop <= end_phy; phy_loop++)
		{
			//read out 
	        reg_1e_12_read = mtEMiiRegRead(phy_loop, 0x1e, 0x12);
	        reg_1e_16_read = mtEMiiRegRead(phy_loop, 0x1e, 0x16);
	        reg_1e_17_read = mtEMiiRegRead(phy_loop, 0x1e, 0x17);
	        reg_1e_18_read = mtEMiiRegRead(phy_loop, 0x1e, 0x18);
	        reg_1e_19_read = mtEMiiRegRead(phy_loop, 0x1e, 0x19);
	        reg_1e_20_read = mtEMiiRegRead(phy_loop, 0x1e, 0x20);
	        reg_1e_21_read = mtEMiiRegRead(phy_loop, 0x1e, 0x21);
	        reg_1e_22_read = mtEMiiRegRead(phy_loop, 0x1e, 0x22);
			reg_1e_174_read = mtEMiiRegRead(phy_loop, 0x1e, 0x174);
			reg_1e_175_read = mtEMiiRegRead(phy_loop, 0x1e, 0x175);
			printf("port = %d \r\n" , phy_loop);
			printf("1e_12 = 0x%x " , reg_1e_12_read);
			printf("1e_16 = 0x%x " , reg_1e_16_read);
			printf("1e_17 = 0x%x " , reg_1e_17_read);
			printf("1e_18 = 0x%x " , reg_1e_18_read);
			printf("1e_19 = 0x%x " , reg_1e_19_read);
			printf("1e_20 = 0x%x " , reg_1e_20_read);
			printf("1e_21 = 0x%x " , reg_1e_21_read);
			printf("1e_22 = 0x%x \r\n" , reg_1e_22_read);
			printf("1e_174 = 0x%x " , reg_1e_174_read);
			printf("1e_175 = 0x%x \r\n" , reg_1e_175_read);
			if(phy_loop == 9)
			{				
                status_flag=0;
				reg_1e_12_header= ((reg_1e_12_read&0xfc00)/1024)+P9_tx_amp_gbe_cha_comp;
				reg_1e_17_header= ((reg_1e_17_read&0x3f00)/256)+P9_tx_amp_gbe_chb_comp;			
				reg_1e_19_header= ((reg_1e_19_read&0x3f00)/256)+P9_tx_amp_gbe_chc_comp;					
				reg_1e_21_header= ((reg_1e_21_read&0x3f00)/256)+P9_tx_amp_gbe_chd_comp;
				if((reg_1e_12_header < txamp_low_limit)||(reg_1e_12_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_17_header < txamp_low_limit)||(reg_1e_17_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_19_header < txamp_low_limit)||(reg_1e_19_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_21_header < txamp_low_limit)||(reg_1e_21_header > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_12_header=0x20;reg_1e_17_header=0x20;reg_1e_19_header=0x20;reg_1e_21_header=0x20;}
				else {
				if(reg_1e_12_header < 0) {reg_1e_12_header=0;}
				if(reg_1e_12_header > 0x3f)	{reg_1e_12_header=0x3f;}
				if(reg_1e_17_header < 0) {reg_1e_17_header=0;}
				if(reg_1e_17_header > 0x3f)	{reg_1e_17_header=0x3f;}
				if(reg_1e_19_header < 0) {reg_1e_19_header=0;}
				if(reg_1e_19_header > 0x3f)	{reg_1e_19_header=0x3f;}
				if(reg_1e_21_header < 0) {reg_1e_21_header=0;}
				if(reg_1e_21_header > 0x3f)	{reg_1e_21_header=0x3f;}
				}
				status_flag=0;
				reg_1e_16_end= (reg_1e_16_read&0x003f)+P9_tx_amp_tst_cha_comp;
				reg_1e_18_end= (reg_1e_18_read&0x003f)+P9_tx_amp_tst_chb_comp;
				reg_1e_20_end= (reg_1e_20_read&0x003f)+P9_tx_amp_tst_chc_comp;
				reg_1e_22_end= (reg_1e_22_read&0x003f)+P9_tx_amp_tst_chd_comp;						
				if((reg_1e_16_end < txamp_low_limit)||(reg_1e_16_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_18_end < txamp_low_limit)||(reg_1e_18_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_20_end < txamp_low_limit)||(reg_1e_20_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_22_end < txamp_low_limit)||(reg_1e_22_end > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_16_end=0x20;reg_1e_18_end=0x20;reg_1e_20_end=0x20;reg_1e_22_end=0x20;}
				else {
				if(reg_1e_16_end < 0) {reg_1e_16_end=0;}
				if(reg_1e_16_end > 0x3f)	{reg_1e_16_end=0x3f;}
				if(reg_1e_18_end < 0) {reg_1e_18_end=0;}
				if(reg_1e_18_end > 0x3f)	{reg_1e_18_end=0x3f;}
				if(reg_1e_20_end < 0) {reg_1e_20_end=0;}
				if(reg_1e_20_end > 0x3f)	{reg_1e_20_end=0x3f;}
				if(reg_1e_22_end < 0) {reg_1e_22_end=0;}
				if(reg_1e_22_end > 0x3f)	{reg_1e_22_end=0x3f;}
				}
				status_flag=0;
				reg_1e_16_header= ((reg_1e_16_read&0xfc00)/1024)+P9_tx_amp_hbt_cha_comp;
				reg_1e_18_header= ((reg_1e_18_read&0x3f00)/256)+P9_tx_amp_hbt_chb_comp;
				if((reg_1e_16_header < txamp_low_limit)||(reg_1e_16_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_18_header < txamp_low_limit)||(reg_1e_18_header > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_16_header=0x20;reg_1e_18_header=0x20;}
				else {
				if(reg_1e_16_header < 0) {reg_1e_16_header=0;}
				if(reg_1e_16_header > 0x3f)	{reg_1e_16_header=0x3f;}
				if(reg_1e_18_header < 0) {reg_1e_18_header=0;}
				if(reg_1e_18_header > 0x3f)	{reg_1e_18_header=0x3f;}
					
				}
				status_flag=0;
				reg_1e_12_end= (reg_1e_12_read&0x003f)+P9_tx_amp_tbt_cha_comp;
				reg_1e_17_end= (reg_1e_17_read&0x003f)+P9_tx_amp_tbt_chb_comp;
				if((reg_1e_12_end < txamp_low_limit)||(reg_1e_12_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_17_end < txamp_low_limit)||(reg_1e_17_end > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_12_end=0x20;reg_1e_17_end=0x20;}
				else {
				if(reg_1e_12_end < 0) {reg_1e_12_end=0;}
				if(reg_1e_12_end > 0x3f)	{reg_1e_12_end=0x3f;}
				if(reg_1e_17_end < 0) {reg_1e_17_end=0;}
				if(reg_1e_17_end > 0x3f)	{reg_1e_17_end=0x3f;}
				}
                reg_1e_12_write=(reg_1e_12_header*1024)+reg_1e_12_end;
				reg_1e_16_write=(reg_1e_16_header*1024)+reg_1e_16_end;
				reg_1e_17_write=(reg_1e_17_header*256)+reg_1e_17_end;
				reg_1e_18_write=(reg_1e_18_header*256)+reg_1e_18_end;
				reg_1e_19_write=(reg_1e_19_read&0x00ff)+(reg_1e_19_header*256);
				reg_1e_20_write=(reg_1e_20_read&0xff00)+reg_1e_20_end;
				reg_1e_21_write=(reg_1e_21_read&0x00ff)+(reg_1e_21_header*256);
				reg_1e_22_write=(reg_1e_22_read&0xff00)+reg_1e_22_end;
				//r50
				status_flag=0;
				reg_1e_174_header= ((reg_1e_174_read&0x7f00)/256)+P9_r50_cha_comp;
				reg_1e_174_end= (reg_1e_174_read&0x007f)+P9_r50_chb_comp;
				reg_1e_175_header= ((reg_1e_175_read&0x7f00)/256)+P9_r50_chc_comp;
				reg_1e_175_end= (reg_1e_175_read&0x007f)+P9_r50_chd_comp;
				if((reg_1e_174_header < r50_low_limit)||(reg_1e_174_header >r50_high_limit)) {status_flag=1;}
				if((reg_1e_174_end < r50_low_limit)||(reg_1e_174_end >r50_high_limit)) {status_flag=1;}
				if((reg_1e_175_header < r50_low_limit)||(reg_1e_175_header >r50_high_limit)) {status_flag=1;}
				if((reg_1e_175_end < r50_low_limit)||(reg_1e_175_end >r50_high_limit)) {status_flag=1;}	
				if(status_flag == 1) {	reg_1e_174_header=0x34;reg_1e_174_end=0x34;reg_1e_175_header=0x34;reg_1e_175_end=0x34;}
                else 
				{
					if(reg_1e_174_header< 0) { reg_1e_174_header=0;}
					if(reg_1e_174_header > 0x7f) { reg_1e_174_header=0x7f;}
					if(reg_1e_174_end< 0) { reg_1e_174_end=0;}
					if(reg_1e_174_end > 0x7f) { reg_1e_174_end=0x7f;}
					if(reg_1e_175_header< 0) { reg_1e_175_header=0;}
					if(reg_1e_175_header > 0x7f) { reg_1e_175_header=0x7f;}
					if(reg_1e_175_end< 0) { reg_1e_175_end=0;}
					if(reg_1e_175_end > 0x7f) { reg_1e_175_end=0x7f;}					
				}
		
					reg_1e_175_write=0x8080+(reg_1e_175_header*256)+reg_1e_175_end;
					reg_1e_174_write=0x8080+(reg_1e_174_header*256)+reg_1e_174_end;
			}
			//calculate phy 10 
			if(phy_loop == 10)
			{
                status_flag=0;
				reg_1e_12_header= ((reg_1e_12_read&0xfc00)/1024)+P10_tx_amp_gbe_cha_comp;
				reg_1e_17_header= ((reg_1e_17_read&0x3f00)/256)+P10_tx_amp_gbe_chb_comp;
				reg_1e_19_header= ((reg_1e_19_read&0x3f00)/256)+P10_tx_amp_gbe_chc_comp;
				reg_1e_21_header= ((reg_1e_21_read&0x3f00)/256)+P10_tx_amp_gbe_chd_comp;
				if((reg_1e_12_header < txamp_low_limit)||(reg_1e_12_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_17_header < txamp_low_limit)||(reg_1e_17_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_19_header < txamp_low_limit)||(reg_1e_19_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_21_header < txamp_low_limit)||(reg_1e_21_header > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_12_header=0x20;reg_1e_17_header=0x20;reg_1e_19_header=0x20;reg_1e_21_header=0x20;}
				else {
				if(reg_1e_12_header < 0) {reg_1e_12_header=0;}
				if(reg_1e_12_header > 0x3f)	{reg_1e_12_header=0x3f;}
				if(reg_1e_17_header < 0) {reg_1e_17_header=0;}
				if(reg_1e_17_header > 0x3f)	{reg_1e_17_header=0x3f;}
				if(reg_1e_19_header < 0) {reg_1e_19_header=0;}
				if(reg_1e_19_header > 0x3f)	{reg_1e_19_header=0x3f;}
				if(reg_1e_21_header < 0) {reg_1e_21_header=0;}
				if(reg_1e_21_header > 0x3f)	{reg_1e_21_header=0x3f;}
				}
				status_flag=0;
				reg_1e_16_end= (reg_1e_16_read&0x003f)+P10_tx_amp_tst_cha_comp;
				reg_1e_18_end= (reg_1e_18_read&0x003f)+P10_tx_amp_tst_chb_comp;
				reg_1e_20_end= (reg_1e_20_read&0x003f)+P10_tx_amp_tst_chc_comp;
				reg_1e_22_end= (reg_1e_22_read&0x003f)+P10_tx_amp_tst_chd_comp;
				if((reg_1e_16_end < txamp_low_limit)||(reg_1e_16_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_18_end < txamp_low_limit)||(reg_1e_18_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_20_end < txamp_low_limit)||(reg_1e_20_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_22_end < txamp_low_limit)||(reg_1e_22_end > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_16_end=0x20;reg_1e_18_end=0x20;reg_1e_20_end=0x20;reg_1e_22_end=0x20;}
				else {
				if(reg_1e_16_end < 0) {reg_1e_16_end=0;}
				if(reg_1e_16_end > 0x3f)	{reg_1e_16_end=0x3f;}
				if(reg_1e_18_end < 0) {reg_1e_18_end=0;}
				if(reg_1e_18_end > 0x3f)	{reg_1e_18_end=0x3f;}
				if(reg_1e_20_end < 0) {reg_1e_20_end=0;}
				if(reg_1e_20_end > 0x3f)	{reg_1e_20_end=0x3f;}
				if(reg_1e_22_end < 0) {reg_1e_22_end=0;}
				if(reg_1e_22_end > 0x3f)	{reg_1e_22_end=0x3f;}
				}
				status_flag=0;
				reg_1e_16_header= ((reg_1e_16_read&0xfc00)/1024)+P10_tx_amp_hbt_cha_comp;
				reg_1e_18_header= ((reg_1e_18_read&0x3f00)/256)+P10_tx_amp_hbt_chb_comp;
				if((reg_1e_16_header < txamp_low_limit)||(reg_1e_16_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_18_header < txamp_low_limit)||(reg_1e_18_header > txamp_high_limit)) {status_flag=1;}
				if(status_flag==1) {reg_1e_16_header=0x20;reg_1e_18_header=0x20;}
				else {
				if(reg_1e_16_header < 0) {reg_1e_16_header=0;}
				if(reg_1e_16_header > 0x3f)	{reg_1e_16_header=0x3f;}
				if(reg_1e_18_header < 0) {reg_1e_18_header=0;}
				if(reg_1e_18_header > 0x3f)	{reg_1e_18_header=0x3f;}
					
				}
				status_flag=0;
				reg_1e_12_end= (reg_1e_12_read&0x003f)+P10_tx_amp_tbt_cha_comp;
				reg_1e_17_end= (reg_1e_17_read&0x003f)+P10_tx_amp_tbt_chb_comp;
				if((reg_1e_12_end < txamp_low_limit)||(reg_1e_12_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_17_end < txamp_low_limit)||(reg_1e_17_end > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_12_end=0x20;reg_1e_17_end=0x20;}
				else {
				if(reg_1e_12_end < 0) {reg_1e_12_end=0;}
				if(reg_1e_12_end > 0x3f)	{reg_1e_12_end=0x3f;}
				if(reg_1e_17_end < 0) {reg_1e_17_end=0;}
				if(reg_1e_17_end > 0x3f)	{reg_1e_17_end=0x3f;}
				}
                reg_1e_12_write=(reg_1e_12_header*1024)+reg_1e_12_end;
				reg_1e_16_write=(reg_1e_16_header*1024)+reg_1e_16_end;
				reg_1e_17_write=(reg_1e_17_header*256)+reg_1e_17_end;
				reg_1e_18_write=(reg_1e_18_header*256)+reg_1e_18_end;
				reg_1e_19_write=(reg_1e_19_read&0x00ff)+(reg_1e_19_header*256);
				reg_1e_20_write=(reg_1e_20_read&0xff00)+reg_1e_20_end;
				reg_1e_21_write=(reg_1e_21_read&0x00ff)+(reg_1e_21_header*256);
				reg_1e_22_write=(reg_1e_22_read&0xff00)+reg_1e_22_end;
				//r50
				status_flag=0;
				reg_1e_174_header= ((reg_1e_174_read&0x7f00)/256)+P10_r50_cha_comp;
				reg_1e_174_end= (reg_1e_174_read&0x007f)+P10_r50_chb_comp;
				reg_1e_175_header= ((reg_1e_175_read&0x7f00)/256)+P10_r50_chc_comp;
				reg_1e_175_end= (reg_1e_175_read&0x007f)+P10_r50_chd_comp;
				if((reg_1e_174_header < r50_low_limit)||(reg_1e_174_header >r50_high_limit)) {status_flag=1;}
				if((reg_1e_174_end < r50_low_limit)||(reg_1e_174_end >r50_high_limit)) {status_flag=1;}
				if((reg_1e_175_header < r50_low_limit)||(reg_1e_175_header >r50_high_limit)) {status_flag=1;}
				if((reg_1e_175_end < r50_low_limit)||(reg_1e_175_end >r50_high_limit)) {status_flag=1;}	
				if(status_flag == 1) {	reg_1e_174_header=0x34;reg_1e_174_end=0x34;reg_1e_175_header=0x34;reg_1e_175_end=0x34;}
                else 
				{
					if(reg_1e_174_header< 0) { reg_1e_174_header=0;}
					if(reg_1e_174_header > 0x7f) { reg_1e_174_header=0x7f;}
					if(reg_1e_174_end< 0) { reg_1e_174_end=0;}
					if(reg_1e_174_end > 0x7f) { reg_1e_174_end=0x7f;}
					if(reg_1e_175_header< 0) { reg_1e_175_header=0;}
					if(reg_1e_175_header > 0x7f) { reg_1e_175_header=0x7f;}
					if(reg_1e_175_end< 0) { reg_1e_175_end=0;}
					if(reg_1e_175_end > 0x7f) { reg_1e_175_end=0x7f;}					
				}
		
					reg_1e_175_write=0x8080+(reg_1e_175_header*256)+reg_1e_175_end;
					reg_1e_174_write=0x8080+(reg_1e_174_header*256)+reg_1e_174_end;
					}
            //calculate phy 11
			if(phy_loop == 11)
			{
                status_flag=0;
				reg_1e_12_header= ((reg_1e_12_read&0xfc00)/1024)+P11_tx_amp_gbe_cha_comp;
				reg_1e_17_header= ((reg_1e_17_read&0x3f00)/256)+P11_tx_amp_gbe_chb_comp;
				reg_1e_19_header= ((reg_1e_19_read&0x3f00)/256)+P11_tx_amp_gbe_chc_comp;
				reg_1e_21_header= ((reg_1e_21_read&0x3f00)/256)+P11_tx_amp_gbe_chd_comp;
				if((reg_1e_12_header < txamp_low_limit)||(reg_1e_12_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_17_header < txamp_low_limit)||(reg_1e_17_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_19_header < txamp_low_limit)||(reg_1e_19_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_21_header < txamp_low_limit)||(reg_1e_21_header > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_12_header=0x20;reg_1e_17_header=0x20;reg_1e_19_header=0x20;reg_1e_21_header=0x20;}
				else {
				if(reg_1e_12_header < 0) {reg_1e_12_header=0;}
				if(reg_1e_12_header > 0x3f)	{reg_1e_12_header=0x3f;}
				if(reg_1e_17_header < 0) {reg_1e_17_header=0;}
				if(reg_1e_17_header > 0x3f)	{reg_1e_17_header=0x3f;}
				if(reg_1e_19_header < 0) {reg_1e_19_header=0;}
				if(reg_1e_19_header > 0x3f)	{reg_1e_19_header=0x3f;}
				if(reg_1e_21_header < 0) {reg_1e_21_header=0;}
				if(reg_1e_21_header > 0x3f)	{reg_1e_21_header=0x3f;}
				}
				status_flag=0;
				reg_1e_16_end= (reg_1e_16_read&0x003f)+P11_tx_amp_tst_cha_comp;
				reg_1e_18_end= (reg_1e_18_read&0x003f)+P11_tx_amp_tst_chb_comp;
				reg_1e_20_end= (reg_1e_20_read&0x003f)+P11_tx_amp_tst_chc_comp;
				reg_1e_22_end= (reg_1e_22_read&0x003f)+P11_tx_amp_tst_chd_comp;
				if((reg_1e_16_end < txamp_low_limit)||(reg_1e_16_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_18_end < txamp_low_limit)||(reg_1e_18_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_20_end < txamp_low_limit)||(reg_1e_20_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_22_end < txamp_low_limit)||(reg_1e_22_end > txamp_high_limit)) {status_flag=1;}
				if(status_flag==1) {reg_1e_16_end=0x20;reg_1e_18_end=0x20;reg_1e_20_end=0x20;reg_1e_22_end=0x20;}
				else {
				if(reg_1e_16_end < 0) {reg_1e_16_end=0;}
				if(reg_1e_16_end > 0x3f)	{reg_1e_16_end=0x3f;}
				if(reg_1e_18_end < 0) {reg_1e_18_end=0;}
				if(reg_1e_18_end > 0x3f)	{reg_1e_18_end=0x3f;}
				if(reg_1e_20_end < 0) {reg_1e_20_end=0;}
				if(reg_1e_20_end > 0x3f)	{reg_1e_20_end=0x3f;}
				if(reg_1e_22_end < 0) {reg_1e_22_end=0;}
				if(reg_1e_22_end > 0x3f)	{reg_1e_22_end=0x3f;}
				}
				status_flag=0;
				reg_1e_16_header= ((reg_1e_16_read&0xfc00)/1024)+P11_tx_amp_hbt_cha_comp;
				reg_1e_18_header= ((reg_1e_18_read&0x3f00)/256)+P11_tx_amp_hbt_chb_comp;
				if((reg_1e_16_header < txamp_low_limit)||(reg_1e_16_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_18_header < txamp_low_limit)||(reg_1e_18_header > txamp_high_limit)) {status_flag=1;}
				if(status_flag==1) {reg_1e_16_header=0x20;reg_1e_18_header=0x20;}
				else {
				if(reg_1e_16_header < 0) {reg_1e_16_header=0;}
				if(reg_1e_16_header > 0x3f)	{reg_1e_16_header=0x3f;}
				if(reg_1e_18_header < 0) {reg_1e_18_header=0;}
				if(reg_1e_18_header > 0x3f)	{reg_1e_18_header=0x3f;}
					
				}
				status_flag=0;
				reg_1e_12_end= (reg_1e_12_read&0x003f)+P11_tx_amp_tbt_cha_comp;
				reg_1e_17_end= (reg_1e_17_read&0x003f)+P11_tx_amp_tbt_chb_comp;
				if((reg_1e_12_end < txamp_low_limit)||(reg_1e_12_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_17_end < txamp_low_limit)||(reg_1e_17_end > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_12_end=0x20;reg_1e_17_end=0x20;}
				else {
				if(reg_1e_12_end < 0) {reg_1e_12_end=0;}
				if(reg_1e_12_end > 0x3f)	{reg_1e_12_end=0x3f;}
				if(reg_1e_17_end < 0) {reg_1e_17_end=0;}
				if(reg_1e_17_end > 0x3f)	{reg_1e_17_end=0x3f;}
				}
                reg_1e_12_write=(reg_1e_12_header*1024)+reg_1e_12_end;
				reg_1e_16_write=(reg_1e_16_header*1024)+reg_1e_16_end;
				reg_1e_17_write=(reg_1e_17_header*256)+reg_1e_17_end;
				reg_1e_18_write=(reg_1e_18_header*256)+reg_1e_18_end;
				reg_1e_19_write=(reg_1e_19_read&0x00ff)+(reg_1e_19_header*256);
				reg_1e_20_write=(reg_1e_20_read&0xff00)+reg_1e_20_end;
				reg_1e_21_write=(reg_1e_21_read&0x00ff)+(reg_1e_21_header*256);
				reg_1e_22_write=(reg_1e_22_read&0xff00)+reg_1e_22_end;
				//r50
				status_flag=0;
				reg_1e_174_header= ((reg_1e_174_read&0x7f00)/256)+P11_r50_cha_comp;
				reg_1e_174_end= (reg_1e_174_read&0x007f)+P11_r50_chb_comp;
				reg_1e_175_header= ((reg_1e_175_read&0x7f00)/256)+P11_r50_chc_comp;
				reg_1e_175_end= (reg_1e_175_read&0x007f)+P11_r50_chd_comp;
				if((reg_1e_174_header < r50_low_limit)||(reg_1e_174_header >r50_high_limit)) {status_flag=1;}
				if((reg_1e_174_end < r50_low_limit)||(reg_1e_174_end >r50_high_limit)) {status_flag=1;}
				if((reg_1e_175_header < r50_low_limit)||(reg_1e_175_header >r50_high_limit)) {status_flag=1;}
				if((reg_1e_175_end < r50_low_limit)||(reg_1e_175_end >r50_high_limit)) {status_flag=1;}	
				if(status_flag == 1) {	reg_1e_174_header=0x34;reg_1e_174_end=0x34;reg_1e_175_header=0x34;reg_1e_175_end=0x34;}
                else 
				{
					if(reg_1e_174_header< 0) { reg_1e_174_header=0;}
					if(reg_1e_174_header > 0x7f) { reg_1e_174_header=0x7f;}
					if(reg_1e_174_end< 0) { reg_1e_174_end=0;}
					if(reg_1e_174_end > 0x7f) { reg_1e_174_end=0x7f;}
					if(reg_1e_175_header< 0) { reg_1e_175_header=0;}
					if(reg_1e_175_header > 0x7f) { reg_1e_175_header=0x7f;}
					if(reg_1e_175_end< 0) { reg_1e_175_end=0;}
					if(reg_1e_175_end > 0x7f) { reg_1e_175_end=0x7f;}					
				}
		
					reg_1e_175_write=0x8080+(reg_1e_175_header*256)+reg_1e_175_end;
					reg_1e_174_write=0x8080+(reg_1e_174_header*256)+reg_1e_174_end;
					}
            //calculate phy 12
			if(phy_loop == 12)
			{
                status_flag=0;
				reg_1e_12_header= ((reg_1e_12_read&0xfc00)/1024)+P12_tx_amp_gbe_cha_comp;
				reg_1e_17_header= ((reg_1e_17_read&0x3f00)/256)+P12_tx_amp_gbe_chb_comp;
				reg_1e_19_header= ((reg_1e_19_read&0x3f00)/256)+P12_tx_amp_gbe_chc_comp;
				reg_1e_21_header= ((reg_1e_21_read&0x3f00)/256)+P12_tx_amp_gbe_chd_comp;
				if((reg_1e_12_header < txamp_low_limit)||(reg_1e_12_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_17_header < txamp_low_limit)||(reg_1e_17_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_19_header < txamp_low_limit)||(reg_1e_19_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_21_header < txamp_low_limit)||(reg_1e_21_header > txamp_high_limit)) {status_flag=1;}
				if(status_flag==1) {reg_1e_12_header=0x20;reg_1e_17_header=0x20;reg_1e_19_header=0x20;reg_1e_21_header=0x20;}
				else {
				if(reg_1e_12_header < 0) {reg_1e_12_header=0;}
				if(reg_1e_12_header > 0x3f)	{reg_1e_12_header=0x3f;}
				if(reg_1e_17_header < 0) {reg_1e_17_header=0;}
				if(reg_1e_17_header > 0x3f)	{reg_1e_17_header=0x3f;}
				if(reg_1e_19_header < 0) {reg_1e_19_header=0;}
				if(reg_1e_19_header > 0x3f)	{reg_1e_19_header=0x3f;}
				if(reg_1e_21_header < 0) {reg_1e_21_header=0;}
				if(reg_1e_21_header > 0x3f)	{reg_1e_21_header=0x3f;}
				}
				status_flag=0;
				reg_1e_16_end= (reg_1e_16_read&0x003f)+P12_tx_amp_tst_cha_comp;
				reg_1e_18_end= (reg_1e_18_read&0x003f)+P12_tx_amp_tst_chb_comp;
				reg_1e_20_end= (reg_1e_20_read&0x003f)+P12_tx_amp_tst_chc_comp;
				reg_1e_22_end= (reg_1e_22_read&0x003f)+P12_tx_amp_tst_chd_comp;
				if((reg_1e_16_end < txamp_low_limit)||(reg_1e_16_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_18_end < txamp_low_limit)||(reg_1e_18_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_20_end < txamp_low_limit)||(reg_1e_20_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_22_end < txamp_low_limit)||(reg_1e_22_end > txamp_high_limit)) {status_flag=1;}
				if(status_flag==1) {reg_1e_16_end=0x20;reg_1e_18_end=0x20;reg_1e_20_end=0x20;reg_1e_22_end=0x20;}
				else {
				if(reg_1e_16_end < 0) {reg_1e_16_end=0;}
				if(reg_1e_16_end > 0x3f)	{reg_1e_16_end=0x3f;}
				if(reg_1e_18_end < 0) {reg_1e_18_end=0;}
				if(reg_1e_18_end > 0x3f)	{reg_1e_18_end=0x3f;}
				if(reg_1e_20_end < 0) {reg_1e_20_end=0;}
				if(reg_1e_20_end > 0x3f)	{reg_1e_20_end=0x3f;}
				if(reg_1e_22_end < 0) {reg_1e_22_end=0;}
				if(reg_1e_22_end > 0x3f)	{reg_1e_22_end=0x3f;}
				}
				status_flag=0;
				reg_1e_16_header= ((reg_1e_16_read&0xfc00)/1024)+P12_tx_amp_hbt_cha_comp;
				reg_1e_18_header= ((reg_1e_18_read&0x3f00)/256)+P12_tx_amp_hbt_chb_comp;
				if((reg_1e_16_header < txamp_low_limit)||(reg_1e_16_header > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_18_header < txamp_low_limit)||(reg_1e_18_header > txamp_high_limit)) {status_flag=1;}
				if(status_flag==1) {reg_1e_16_header=0x20;reg_1e_18_header=0x20;}
				else {
				if(reg_1e_16_header < 0) {reg_1e_16_header=0;}
				if(reg_1e_16_header > 0x3f)	{reg_1e_16_header=0x3f;}
				if(reg_1e_18_header < 0) {reg_1e_18_header=0;}
				if(reg_1e_18_header > 0x3f)	{reg_1e_18_header=0x3f;}
					
				}
				status_flag=0;
				reg_1e_12_end= (reg_1e_12_read&0x003f)+P12_tx_amp_tbt_cha_comp;
				reg_1e_17_end= (reg_1e_17_read&0x003f)+P12_tx_amp_tbt_chb_comp;
				if((reg_1e_12_end < txamp_low_limit)||(reg_1e_12_end > txamp_high_limit)) {status_flag=1;}
				if((reg_1e_17_end < txamp_low_limit)||(reg_1e_17_end > txamp_high_limit)) {status_flag=1;}
				if(status_flag == 1) {reg_1e_12_end=0x20;reg_1e_17_end=0x20;}
				else {
				if(reg_1e_12_end < 0) {reg_1e_12_end=0;}
				if(reg_1e_12_end > 0x3f)	{reg_1e_12_end=0x3f;}
				if(reg_1e_17_end < 0) {reg_1e_17_end=0;}
				if(reg_1e_17_end > 0x3f)	{reg_1e_17_end=0x3f;}
				}
                reg_1e_12_write=(reg_1e_12_header*1024)+reg_1e_12_end;
				reg_1e_16_write=(reg_1e_16_header*1024)+reg_1e_16_end;
				reg_1e_17_write=(reg_1e_17_header*256)+reg_1e_17_end;
				reg_1e_18_write=(reg_1e_18_header*256)+reg_1e_18_end;
				reg_1e_19_write=(reg_1e_19_read&0x00ff)+(reg_1e_19_header*256);
				reg_1e_20_write=(reg_1e_20_read&0xff00)+reg_1e_20_end;
				reg_1e_21_write=(reg_1e_21_read&0x00ff)+(reg_1e_21_header*256);
				reg_1e_22_write=(reg_1e_22_read&0xff00)+reg_1e_22_end;
				//r50
				status_flag=0;
				reg_1e_174_header= ((reg_1e_174_read&0x7f00)/256)+P12_r50_cha_comp;
				reg_1e_174_end= (reg_1e_174_read&0x007f)+P12_r50_chb_comp;
				reg_1e_175_header= ((reg_1e_175_read&0x7f00)/256)+P12_r50_chc_comp;
				reg_1e_175_end= (reg_1e_175_read&0x007f)+P12_r50_chd_comp;
				if((reg_1e_174_header < r50_low_limit)||(reg_1e_174_header >r50_high_limit)) {status_flag=1;}
				if((reg_1e_174_end < r50_low_limit)||(reg_1e_174_end >r50_high_limit)) {status_flag=1;}
				if((reg_1e_175_header < r50_low_limit)||(reg_1e_175_header >r50_high_limit)) {status_flag=1;}
				if((reg_1e_175_end < r50_low_limit)||(reg_1e_175_end >r50_high_limit)) {status_flag=1;}	
				if(status_flag == 1) {	reg_1e_174_header=0x34;reg_1e_174_end=0x34;reg_1e_175_header=0x34;reg_1e_175_end=0x34;}
                else 
				{
					if(reg_1e_174_header< 0) { reg_1e_174_header=0;}
					if(reg_1e_174_header > 0x7f) { reg_1e_174_header=0x7f;}
					if(reg_1e_174_end< 0) { reg_1e_174_end=0;}
					if(reg_1e_174_end > 0x7f) { reg_1e_174_end=0x7f;}
					if(reg_1e_175_header< 0) { reg_1e_175_header=0;}
					if(reg_1e_175_header > 0x7f) { reg_1e_175_header=0x7f;}
					if(reg_1e_175_end< 0) { reg_1e_175_end=0;}
					if(reg_1e_175_end > 0x7f) { reg_1e_175_end=0x7f;}					
				}
		
					reg_1e_175_write=0x8080+(reg_1e_175_header*256)+reg_1e_175_end;
					reg_1e_174_write=0x8080+(reg_1e_174_header*256)+reg_1e_174_end;
					}	
                printf("1e_12 = 0x%x " , reg_1e_12_write);
                printf("1e_16 = 0x%x " , reg_1e_16_write);
                printf("1e_17 = 0x%x " , reg_1e_17_write);
                printf("1e_18 = 0x%x " , reg_1e_18_write);
                printf("1e_19 = 0x%x " , reg_1e_19_write);
                printf("1e_20 = 0x%x " , reg_1e_20_write);
                printf("1e_21 = 0x%x " , reg_1e_21_write);
			printf("1e_22 = 0x%x \r\n" , reg_1e_22_write);
				printf("1e_174 = 0x%x " , reg_1e_174_write);
				printf("1e_175 = 0x%x \r\n" , reg_1e_175_write);

			//write back reg
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0012, reg_1e_12_write);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0016, reg_1e_16_write);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0017, reg_1e_17_write);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0018, reg_1e_18_write);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0019, reg_1e_19_write);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0020, reg_1e_20_write);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0021, reg_1e_21_write);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0022, reg_1e_22_write);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0174, reg_1e_174_write);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0175, reg_1e_175_write);
				//mtEMiiRegWrite(phy_loop, 0x1e, 0x005c, 0x5555);
				//mtEMiiRegWrite(phy_loop, 0x1e, 0x0176, 0x5500);
				//mtEMiiRegWrite(phy_loop, 0x1e, 0x0177, 0x0055);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0023, 0x885);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0024, 0x885);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0025, 0x885);
				mtEMiiRegWrite(phy_loop, 0x1e, 0x0026, 0x885);
		}
}



void my_ephy_monitor(unsigned long data)
{

	/* Schedule for the next time */
    /* add your code here */
    if(slt_excuting == 0)
	 {
		
	printf("monitor value xxxxxxx \r\n");
		for(slt_not_excute_loop = all_port_start; slt_not_excute_loop <= all_port_end; slt_not_excute_loop++)
	        {			
	                tcMiiStationWrite(slt_not_excute_loop, 0x04, 0x0de1);
	                tcMiiStationWrite(slt_not_excute_loop, 0x00, 0x1240);
					//eee set because power down can't set tokenring
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DEC", 0x15, 0x0055a0, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DEC", 0x17, 0x07ff3f, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "PMA", 0x0, 0x00001e, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "PMA", 0x1, 0x6fb90a, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "PMA", 0x18, 0x0e2f00, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "TR", 0x26, 0x444444, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0x3, 0x000004, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0x6, 0x2ebaef, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0x8, 0x00000b, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0xc, 0x00504d, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0xd, 0x02314f, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0xf, 0x003028, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0x10, 0x00000a, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0x11, 0x040001, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0x14, 0x00024a, 0);
					mtPhyMiiWrite_TrDbg(slt_not_excute_loop, "DSPF", 0x1c, 0x003210, 0);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x120, 0x8014);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x122, 0xffff);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x144, 0x0200);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x14a, 0xee20);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x19b, 0x0111);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x234, 0x1181);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x238, 0x0120);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x239, 0x0117);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x2d1, 0x0733);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x323, 0x0011);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x324, 0x013f);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1e, 0x326, 0x0037);
					mtEMiiRegWrite(slt_not_excute_loop, 0x1f, 0x268, 0x07f4);
	       }
	 }
	inital_finish_flag = 1;	
}


int ephy_timer_init(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	init_timer(&ephy_timer);
	ephy_timer.expires = jiffies + msecs_to_jiffies(10000);
    ephy_timer.function = my_ephy_monitor;
    ephy_timer.data = 0;
#else
	timer_setup(&ephy_timer, my_ephy_monitor, 0);
	ephy_timer.expires = jiffies + msecs_to_jiffies(10000);
#endif
    add_timer(&ephy_timer);
}
// ************************************************************************
// 		tcPhy initial: reset, load default register setting, restat AN
//		[in] *mac_p
// ************************************************************************
int tcPhyInit(void)
{
	uint16	ephy_addr, port_num;

	if(ecnt_register_hook(&ecnt_driver_ephy_op)) 
	{
		printk("ecnt_driver_ephy_op register fail\n");
		return -ENODEV ;
    }  // JasonG_7512

	if (isFPGA) 
	{	//xyyou add for 7505 FPGA phy addr1 and addr2 reset 			
    	tcMiiStationWrite(1, MII_BMCR, BMCR_RESET); 	
    	pause(10);			
    	tcMiiStationWrite(1, MII_BMCR, BMCR_ANRESTART | BMCR_ANENABLE); 
    	pause(10);			
    	tcMiiStationWrite(2, MII_BMCR, BMCR_RESET); 	
    	pause(10);			
    	tcMiiStationWrite(2, MII_BMCR, BMCR_ANRESTART | BMCR_ANENABLE); 
	}
	else
	{
		//ephy_addr_base = 0;
		for(ephy_addr = EPHY_ADDR_P0; ephy_addr <= 12; ephy_addr++)
		{
    		tcPhyVerLookUp(ephy_addr);
    		if(tcPhyInitFlag == 1)  // is ECNT's ephy
    		{
    			port_num = ephy_addr - ephy_addr_base;
    			TCPHYDISP1(", port_num = %d, ephy_addr = %d, ephy_addr_base = 0x%x \r\n", port_num, ephy_addr, ephy_addr_base);

    			if(port_num >= 0)
    			{
    				ePhyInit(ephy_addr);
    				GECal_flag = 0;
			    	doGePhyALLAnalogCal_R45(ephy_addr);

					toKenRingWrite(ephy_addr,1,0xf,0x12,0x5e4d2a);
	                tcMiiStationWrite(ephy_addr, 0x00, 0x0800);  					
					
    			}	
     		}	
    	}
		phyTxAmpcomp(1);
		normalinit();
	}
	ephy_timer_init();
	tcephydbgcmd();

	return 0;
}


int tcPhyDeinit(void)  // JasonG_7512
{
    ecnt_unregister_hook(&ecnt_driver_ephy_op);
    return 0;
}


int ePhyInit(uint16 ephy_addr)
{
    int i=0;
	uint32 reg_value; //CML_20130226_1

	TCPHYDISP1(" [tcPhyInit] in ");

	switch(tcPhyVer) 
	{			    
		case tcPhyVer_7523Ge: 		// EN7523 Ge
			if(ephy_addr_base == 0)
				ephy_addr_base += EPHY_ADDR_P0;
			break;
	}

              
    //tcMiiStationWrite(ephy_addr, PHY_CONTROL_REG, PHY_RESET);
    //tcMiiStationWrite(ephy_addr, 0x0, 0x8000);

    switch(tcPhyVer) 
	{
		#ifdef LINUX_OS 			// allen_20130926

		case tcPhyVer_7523Ge: 		// EN7523 Ge
			en75xxGePhyCfgLoad(0);   
			break;
		#endif
    }

	#if 0
    // always boot-up with AN-enable
    //for(i=start_addr; i<start_addr+tcPhyPortNum; i++ )
	//{            
        //tcMiiStationWrite(i, PHY_CONTROL_REG, MIIDR_AUTO_NEGOTIATE );
		// rewrite to avoid changing H/W default setting //CML_20130226_1
		#ifdef LINUX_OS // allen_20130926
		reg_value = tcMiiStationRead(ephy_addr, PHY_CONTROL_REG);
		reg_value |= MIIDR_AUTO_NEGOTIATE;
        tcMiiStationWrite(ephy_addr, PHY_CONTROL_REG, reg_value );
		#else
		tcMiiStationWrite(ephy_addr, PHY_CONTROL_REG, MIIDR_AUTO_NEGOTIATE );
		#endif
    //}
    #endif

    // tcphy_link_state init.
    for(i=0; i<tcPhyPortNum; i++)
    {
        tcphy_link_state[i] = ST_LINK_DOWN;
    }
	TCPHYDISP1(" [tcPhyInit] out ");
	TCPHYDISP1(", r31 = (%d, 0x%x) \r\n", ephy_addr, tcMiiStationRead(ephy_addr, 31));
	

	
    return 0;
}

void    normalinit(void)
{
	uint16 ephy_addr;
	
	for(ephy_addr = 9; ephy_addr <= 12; ephy_addr++)
		{
   		
			
					//Long Loop Reach setting 
					mtEMiiRegWrite(ephy_addr, 0x1e, 0x11, 0xff00);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x202, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x203, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x204, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x205, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x206, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x207, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x208, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x209, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x20a, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x20b, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x20e, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x20f, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x210, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x211, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x212, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x213, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x214, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x215, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x216, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x217, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x21a, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x21b, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x21c, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x21d, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x21e, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x21f, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x220, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x221, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x222, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x223, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x226, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x227, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x228, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x229, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x22a, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x22b, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x22c, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x22d, 0x0023);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x22e, 0x2219);
					mtEMiiRegWrite(ephy_addr, 0x1f, 0x22f, 0x0023);

					//RX setting
					mtEMiiRegWrite(ephy_addr, 0x1e, 0x3c, 0xc000);
					mtEMiiRegWrite(ephy_addr, 0x1e, 0x3d, 0x0000);
					mtEMiiRegWrite(ephy_addr, 0x1e, 0x3e, 0xc000);
					mtEMiiRegWrite(ephy_addr, 0x7, 0x3c, 0x0000);
					//0r discrete
					mtEMiiRegWrite(ephy_addr, 0x1e, 0xe6, 0x0000);
					mtEMiiRegWrite(ephy_addr, 0x1e, 0xe7, 0x4444);
					mtEMiiRegWrite(ephy_addr, 0x1e, 0xfe, 0x0002);		
					//5r discrete
					//mtEMiiRegWrite(ephy_addr, 0x1e, 0xe6, 0x1111);
					//mtEMiiRegWrite(ephy_addr, 0x1e, 0xe7, 0x0000);
					//mtEMiiRegWrite(ephy_addr, 0x1e, 0xfe, 0x0002);				
				
				    tcMiiStationWrite(ephy_addr, 0x1f, 0x1);
				    tcMiiStationWrite(ephy_addr, 0x14, 0x3a18);
				    tcMiiStationWrite(ephy_addr, 0x1f, 0x0);
					tcMiiStationWrite(ephy_addr, 0x9, 0x1e00);
				    //tcMiiStationWrite(ephy_addr, 0x00, 0x1040);
    	}


	return 0;
}

#if 0
// tcPhy initial: reset, load default register setting, restat AN
int tcPhyPortInit(uint8 port_num)
{
	uint32 reg_value; // CML_20130226_1
	
    #ifndef LINUX_OS
    //tcMiiStationWrite(port_num, PHY_CONTROL_REG, PHY_RESET);
	#endif

    switch(tcPhyVer) 
	{     
		//case tcPhyVer_7512Fe: 		// mt7512 FE
		//case tcPhyVer_7522Fe: 		// mt7512 FE
		//	mt7512FECfgLoad(0, DO_PER_PORT, port_num);
		//	break;
    }

    // always boot-up with AN-enable         
    //tcMiiStationWrite(port_num, PHY_CONTROL_REG, MIIDR_AUTO_NEGOTIATE );
    // rewrite to avoid changing H/W default setting //CML_20130226_1
	//tcMiiStationWrite(port_num, PHY_CONTROL_REG, MIIDR_AUTO_NEGOTIATE );
	reg_value = tcMiiStationRead(port_num, PHY_CONTROL_REG);
	reg_value |= MIIDR_AUTO_NEGOTIATE;
	tcMiiStationWrite(port_num, PHY_CONTROL_REG, reg_value);
	
    // tcphy_link_state init.
    tcphy_link_state[port_num]=ST_LINK_DOWN;
    return 0;
}
#endif

// return LP0Reg1 PHY Status Registers Value
uint8 getTcPhyFlag(void)
{
    return tcPhyFlag;
}
#if 0
uint32 getTcPhyStatusReg(int phy_add)
{
	// no called if (force_link_flag==1)
	/*
	#ifdef PHYPART_DEBUG
	printf("PhyPart debug: getTcPhyStatusReg() in \r\n");
	#endif
	*/
    if(!sw_patch_flag)//if sw patch off, mac get PHY status via MDIO directly.
    {
		/*    
		#ifdef PHYPART_DEBUG
		printf("PhyPart debug: getTcPhyStatusReg() out \r\n");
		#endif
		*/
		return ( tcMiiStationRead(phy_add, PHY_STATUS_REG) );
    }
    else//if sw patch on, mac get the PHY status that patch intercept. 
    {
	    /*
		#ifdef PHYPART_DEBUG
		printf("PhyPart debug: getTcPhyStatusReg() out \r\n");
		#endif
		*/
		return (Nmr1[0].value); //mr1_link_status_reg;
	}
	return 0;
}
#endif

uint32 getTcPhyRMCAPReg(int phy_add)
{
	#if 0
	#ifdef TC2101MI_SUPPORT
	if ((tcPhyVer==tcPhyVer_2101mi) && sw_patch_flag)
		return tc2101mi_reg5_val;
	else
	#endif
	#endif	
	//return ( tcMiiStationRead(phy_add, PHY_REMOTE_CAP_REG));	
	return 0;
}
#if 0
uint16 getTcPhyNStatusReg(uint8 port_num)
{
    uint16 phyAddr;

    if(!sw_patch_flag) 
	{
        phyAddr = ephy_addr_base + port_num;
        return ( tcMiiStationRead(phyAddr, PHY_STATUS_REG) ); 
    }
    else 
	{
        return (Nmr1[port_num].value);
    }
    return 0;
}
#endif

bool getTcPhyEsdDetectFlag(void)
{
	return 0;
}
	
// Software Patch for TrendChip's ethernet PHY
int tcPhySwPatch(void)
{
    if(!sw_patch_flag)
    {
       	Skew_update_flag=0;
    	return 0;
    }
	#ifdef PHYPART_DEBUG_SW_PATCH
	printf("PhyPart debug: tcPhySwPatch() in  \r\n");
	#endif
    switch (tcPhyVer) 
	{  
		case tcPhyVer_7523Ge: 
			en75xxGePhySwPatch();  				
			break;
    }
	#ifdef PHYPART_DEBUG_SW_PATCH
	printf("PhyPart debug: tcPhySwPatch() out  \r\n");
	#endif
    return 0;
}

// CI command for tcephydbg
int doEtherPhydbg(int argc, char *argv[], void *p) 
{
    return subcmd(ethertphyprint, argc, argv, p);
} 
 
#ifdef LINUX_OS
/*=========================================================================
**   tcephydbgcmd: register rootcommand of "ether ". 
**           call: call by femac.c/mainInit(), tc3262_gmac_init
**=========================================================================*/
void tcephydbgcmd(void)
{
	cmds_t tcephydbg_cmd;

	tcephydbg_cmd.name = "tce";
	tcephydbg_cmd.func = doEtherPhydbg;
	tcephydbg_cmd.flags = 0x12;
	tcephydbg_cmd.argcmin = 0;
	tcephydbg_cmd.argc_errmsg = NULL;
	cmd_register(&tcephydbg_cmd);
} /*end tcephydbgcmd*/

void tcephydbgcmd1(void)
{
	cmds_t tcephydbg_cmd;

	tcephydbg_cmd.name = "tce1";
	tcephydbg_cmd.func = doEtherPhydbg;
	tcephydbg_cmd.flags = 0x12;
	tcephydbg_cmd.argcmin = 0;
	tcephydbg_cmd.argc_errmsg = NULL;
	cmd_register(&tcephydbg_cmd);
} /*end tcephydbgcmd*/
#endif

#endif //TCPHY_SUPPORT
#ifdef TCPHY_DEBUG
uint8 getTcPhyLookbackFlag(void)
{
    return phy_loopback_flag;
}
uint8 getTcPhyForceLinkFlag(void)
{	
    return force_link_flag;
}
#endif //TCPHY_DEBUG

/************************************************************************
*         API functions body for EPHY (TCPHY_SUPPORT)
**************************************************************************/
#ifdef TCPHY_SUPPORT
static int doPhySwVer(int argc, char *argv[], void *p)
{
	printf("Ver : ");

    switch (tcPhyVer) 
	{    
		case tcPhyVer_7523Ge: 
			printf("%s, \r\n", en75xxGe_cfg[0].name);
		 	printf("tcPhyVer_7523Ge, tcPhyVer=0x%02lX \r\n", tcPhyVer);
			break;	

        default: 
	    	printf("Unknown PHY, tcPhyVer=0x%02lX \r\n", tcPhyVer);
    }

	printf(": A7523.04.04_0R setting_discrete_TXMR   \r\n");  
	return 0;
} 

uint16 getMiiPage(char *page)
{
    // g0,g1,g2,g3,g4,g5 = 0x0000~0x5000
    // l0,l1,l2,l3 = 0x8000~0xb000
    if(stricmp(page, "g0") == 0)
    {
        return (0x0000);
    }
    else if(stricmp(page, "g1") == 0)
    {
        return (0x1000);
    }
    else if(stricmp(page, "g2") == 0)
    {
        return (0x2000);
    }
    else if(stricmp(page, "g3") == 0)
    {
        return (0x3000);
    }
	else if(stricmp(page, "g4") == 0)
	{
        return (0x4000);
    }
	else if(stricmp(page, "g5") == 0)
	{
        return (0x5000);
    }
	else if(stricmp(page, "g6") == 0)
	{
        return (0x6000);
    }
    else if(stricmp(page, "g7") == 0)
	{
        return (0x7000);
    }
    else if(stricmp(page, "l0") == 0)
    {
        return (0x8000);
    }
    else if(stricmp(page, "l1") == 0)
    {
        return (0x9000);
    }
    else if(stricmp(page, "l2") == 0)
    {
        return (0xa000);
    }
    else if(stricmp(page, "l3") == 0)
    {
        return (0xb000);
    }
    else if(stricmp(page, "l4") == 0)
    {
        return (0xc000);
    }
	else if(stricmp(page, "ext") == 0)
	{	//Extended Page Registers
	    return (0x0001);
	}
	else if(stricmp(page, "misc") == 0)
	{	//Misc Page Registers
	    return (0x0002);
	}
	else if(stricmp(page, "lpi") == 0)
	{	//LPI Page Registers
	    return (0x0003);
	}
	else if( stricmp(page, "test") == 0)
	{	//Test Page Registers
	    return (0x2a30);
	}
	else if(stricmp(page, "tr") == 0)
	{	//Token Ring Debug Registers
		return (0x52b5);
	}
    else 
    {
        printf("* Wrong PageNo(%s).\r\n",page);
	    printf("* MTGPHY PageNo=><g0|ext|misc|lpi|test|tr> \r\n");
	    printf("* Tcxxx PageNo=><g0|g1|g2|g3|g4|g5|l0|l1|l2|l3|l4> \r\n");
        return (0xFFFF);
    }
}

uint16 checked_atoi(char *val)
{
	#ifdef TCPHY_DEBUG
    // only check 1st char
    if(val[0]<'0' || val[0]>'9')
        return (0xffff);
	#endif
    return (atoi(val));
}

int	doPhyMiiRead(int argc, char *argv[], void *p)
{
    uint16 	phyaddr=0;
    const 	uint16 page_reg=31;
    uint16 	page_val=0;
    uint32 	reg=0;
    uint32 	value;
    int i;

    //  argc:3
    //  tce miir all <PhyAddr> 
    //  tce miir <PhyAddr> <RegAddr>
    //  argc:4
    //  tce miir all <PhyAddr> <PageNo>
    //  tce miir <PhyAddr> <PageNo> <RegAddr>

    // get parameters
    if(argc==3)
    {
        if(stricmp(argv[1], "all") == 0)
        { 	// tce miir all <PhyAddr> 
            phyaddr = checked_atoi(argv[2]);
        }
        else
        { 	// tce miir <PhyAddr> <RegAddr>
            phyaddr = checked_atoi(argv[1]);
            reg = checked_atoi(argv[2]);
           	//sscanf(argv[2], "%lx", &reg);
		   //printf("*1: PhyAddr=%d, RegAddr=0x%02lX\r\n", phyaddr, reg);	   
        }
    }
    else if(argc==4)
    {
        if(stricmp(argv[1], "all") == 0)
        { 	// tce miir all <PhyAddr> <PageNo>
            phyaddr = checked_atoi(argv[2]);
            page_val = getMiiPage(argv[3]);
        }
        else
        { 	// tce miir <PhyAddr> <PageNo> <RegAddr>
            phyaddr = checked_atoi(argv[1]);
            page_val = getMiiPage(argv[2]);
            reg = checked_atoi(argv[3]);
            //sscanf(argv[3], "%lX", &reg);
        }
    }

    // check parameters
    if((argc==3 || argc==4) 
        && phyaddr<=31 && reg<=31 && page_val!=0xffff)
    {
        // set page
        if(!isFPGA)
		{	
        	if (argc==4)
        	{           
            	if(stricmp(argv[1], "all") == 0)
            	{ 	// multiple read
                	printf("* PageNo=%s ",argv[3]);
                	printf("\r\n");             
            	}
            	else
            	{
                	printf("* PageNo=%s ",argv[2]);
            	}
				#ifdef LINUX_OS // allen_20130926
				tcMiiStationWrite(phyaddr, page_reg, page_val);
				#else
				tcMiiStationWrite(phyaddr, page_reg, page_val);
				#endif	
        	}
		}//if(!isFPGA)
		
        // read data
        if(stricmp(argv[1], "all") == 0)
		{	// multiple read
           	for( i=0; i<32; i++ )
			{
				#ifdef LINUX_OS // allen_20130926	
				value = tcMiiStationRead(phyaddr, i);
				#else
				value = tcMiiStationRead(phyaddr, i);
				#endif
                printf("[reg=%02d val=%04lX]", i, value);
                if( (i+1) % 4 == 0 )
                    printf("\r\n");             
            }
        }
        else
		{
			#ifdef LINUX_OS // allen_20130926
			value = tcMiiStationRead(phyaddr, reg);
			#else
			value = tcMiiStationRead(phyaddr, reg);
			#endif
            printf("* PhyAddr=%d RegAddr=%02d value=%04lX\r\n", phyaddr, reg, value);
        }       
        return 0;           
    }   
    else
    { 	// error message
        printf("Usage: miir all <PhyAddr> [PageNo]\r\n");
        printf("       miir <PhyAddr> <RegAddr>\r\n");
        printf("       miir <PhyAddr> <PageNo> <RegAddr>\r\n");
        return 0;           
    }
}

int doPhyMiiWrite (int argc, char *argv[], void *p)
{
    uint16 	phyaddr=0;
    const 	uint16 page_reg=31;
    uint16 	page_val=0;
	uint32 	reg=0;
    uint32 	value=0;
	uint16 	STBit=0;
	uint16 	BFlen=0;
	uint16 	BF,BFMsk=0;
	uint32 	RValue =0;
	int 	i;
    //  tce miir <PhyAddr> <RegAddr> <Value>
    //  tce miir <PhyAddr> <PageNo> <RegAddr> <Value>
    // get parameters
    if(argc==4)
	{
        phyaddr = checked_atoi(argv[1]);
		reg = checked_atoi(argv[2]);
		//sscanf(argv[2], "%lx", &reg);
        sscanf(argv[3], "%lx", &value);
		//printf("*1: argv[2]=%s, argv[3]=%s\r\n", argv[2], argv[3]);		
		//printf("*2: phyaddr=%d, Reg=0x%02lX, value=0x%04lX\r\n", phyaddr, reg , value);		
    }
    else if (argc==5)
    {
        phyaddr = checked_atoi(argv[1]);
        page_val = getMiiPage(argv[2]);
        reg = checked_atoi(argv[3]);
        //sscanf(argv[3], "%lx", &reg);
        sscanf(argv[4], "%lx", &value);
    }
	else if (argc==6)
	{
		if(stricmp(argv[0], "miiwb") == 0)
		{
			phyaddr = checked_atoi(argv[1]);
		    reg = checked_atoi(argv[2]);
			//sscanf(argv[2], "%lx", &reg);
			STBit = checked_atoi(argv[3]);
			BFlen = checked_atoi(argv[4]);
			sscanf(argv[5], "%lx", &value);
			printf("* Phyaddr=%d, RegAddr=%02d, STBit=%0d, BFlen=%0d, value=%04lX\r\n", phyaddr, reg, STBit, BFlen, value);			
		}
	}
	else if (argc==7)
	{
		if(stricmp(argv[0], "miiwb") == 0)
		{
			phyaddr = checked_atoi(argv[1]);
			page_val = getMiiPage(argv[2]);
			reg = checked_atoi(argv[3]);
			//sscanf(argv[3], "%lx", &reg);
			STBit = checked_atoi(argv[4]);
			BFlen = checked_atoi(argv[5]);
			sscanf(argv[6], "%lx", &value);
			printf("* Phyaddr=%d, pageNo=%d, RegAddr=%02d, STBit=%0d, BFlen=%0d, value=%04lX\r\n", phyaddr, page_val, reg, STBit, BFlen, value);			
		}
	}
		
    // check parameters and write
    if ((argc==4 || argc==5) 
        && (phyaddr<=31) && (reg<=31) && (page_val!=0xffff))
	{
        // set page
        if(!isFPGA)
		{
       		if (argc==5) 
			{
        	    printf("* PageNo=%s ",argv[2]);
				#ifdef LINUX_OS // allen_20130926			
				tcMiiStationWrite(phyaddr, page_reg, page_val);
				#else
				tcMiiStationWrite(phyaddr, page_reg, page_val);
				#endif	
        	}
	   	}
        // write data
        printf("* Phyaddr=%d RegAddr=%02d value=%04lX\r\n", phyaddr, reg, value);
		#ifdef LINUX_OS // allen_20130926	
		tcMiiStationWrite(phyaddr, reg, value);
		#else
		tcMiiStationWrite(phyaddr, reg, value);
		#endif
    }
	#ifdef LINUX_OS // allen_20130926
 	else if (((argc == 6) || (argc == 7))
		&& (phyaddr<=31) && (reg<=31) && (page_val != 0xffff))
	{
		if(!isFPGA)
		{
        	if (argc==7) 
			{
            	printf("* PageNo=%s ",argv[2]);
				tcMiiStationWrite(phyaddr, page_reg, page_val);
        	}
		}
		
		if(stricmp(argv[0], "miiwb") == 0)
		{
		 	RValue = tcMiiStationRead(phyaddr, reg);
		 	for(i=0;i<BFlen;i++)
		 	{
				BF=1;
			 	BF=BF<<(STBit+i);
			 	BFMsk = BFMsk | BF;
			  	//printf("1: BF=%d, BFMsk=%04x\r\n", BF, BFMsk);
			}
			BFMsk = ~BFMsk;
			//printf("2: BF=%d, BFMsk=%04x\r\n", BF, BFMsk); 
			value = (RValue & BFMsk) | (value<<STBit);
			//printf("* Phyaddr=%d, RegAddr=0x%02lX, OrgValue=0x%04lX, Modified value=0x%04lX\r\n", phyaddr, reg,  RValue, value);

			printf("* OrgValue=%04lX,",RValue);
			tcMiiStationWrite(phyaddr, reg, value);
			RValue = tcMiiStationRead(phyaddr, reg);
			printf(" ModValue=%04lX\r\n", RValue);
		}
    }
	#endif
    else 
	{	// error message
		printf("Usage: miiw <PhyAddr> <RegAddr> <RegVal>\r\n");
		printf("       miiw <PhyAddr> <PageNo> <RegAddr> <RegVal>\r\n");
		#ifdef LINUX_OS // allen_20130926	
		printf("       miiwb <PhyAddr> <RegAddr> <STBit> <BFLen> <BFVal> \r\n");	
		printf("       miiwb <PhyAddr> <PageNo> <RegAddr> <STBit> <BFLen> <BFVal> \r\n");	
		#endif
        return 0;
    }
    return 0;
}



int	doPbusRead(int argc, char *argv[], void *p)
{
	uint32 phyaddr=0;
	uint32 pbus_addr=0;
	uint32 value=0;
	uint32 high_byte=0,low_byte=0;

	if((argc <=2)|| (argc >=4))
	{	
		printf("	   CMD Error : tce pbr <PhyAddr> <pbus_addr>\r\n");
		
	}
	else	
	{
		phyaddr = checked_atoi(argv[1]);
		sscanf(argv[2], "%x", &pbus_addr);//hex
		tcMiiStationWrite(phyaddr, 0x1f, (pbus_addr >> 6));
		low_byte = tcMiiStationRead(phyaddr, ((pbus_addr >> 2)& 0xF));
		high_byte = tcMiiStationRead(phyaddr, 0x10);
		value=(65536*high_byte)+low_byte;
		printf(" Pbus read => phyaddr=%d, pbus_addr=0x%08lX , value=0x%08lX\r\n", phyaddr, pbus_addr, value);
		return 0;	   
	}

   return 0; 

}

int doPbusWrite (int argc, char *argv[], void *p)
{

	uint32 phyaddr=0;
	uint32 pbus_addr=0;
	uint32 value=0;


	if((argc <=3)|| (argc >=5))
	{		

		printf("	   CMD Error : tce pbw <PhyAddr> <pbus_addr> <value>\r\n");

		
	}
	else	
	{
		phyaddr = checked_atoi(argv[1]);
		sscanf(argv[2], "%x", &pbus_addr);//hex
		sscanf(argv[3], "%x", &value);//hex
		tcMiiStationWrite(phyaddr, 0x1f, (pbus_addr >> 6));
		tcMiiStationWrite(phyaddr, ((pbus_addr >> 2)& 0xF), (value & 0xFFFF));
		tcMiiStationWrite(phyaddr, 0x10, (value >> 16));
		printf(" Pbus Write => phyaddr=%d, pbus_addr=0x%08lX , value=0x%08lX\r\n", phyaddr, pbus_addr, value);
		return 0;	   
	}

   return 0;

}

#ifdef TCPHY_1PORT
static int doPhySpeed (int argc, char *argv[], void *p)
{
    uint8 port_num = 0;

    if(argc != 2 ) 
    {
        printf("Usage: Speed <Auto|AN|100F|100H|10F|10H|Disp>\r\n");
    }   
    else 
    {
        if(stricmp(argv[1], "Auto") == 0 ) 
        {
            tcphy_speed = tcphy_speed_Auto;
            //tcPhyWriteReg(port_num,0,0x1200);
        }
        else if(stricmp(argv[1], "AN") == 0 )
        {
            tcphy_speed = tcphy_speed_ForceAN;
            //tcPhyWriteReg(port_num,0,0x1200);
        }
        else if(stricmp(argv[1], "100F") == 0 ) 
        {
            tcphy_speed = tcphy_speed_Force100F;
            tcPhyWriteReg(port_num,0,0x2100);
        }
        else if(stricmp(argv[1], "100H") == 0 )
        {
            tcphy_speed = tcphy_speed_Force100H;
            tcPhyWriteReg(port_num,0,0x2000);
        }
        else if(stricmp(argv[1], "10F") == 0 ) 
        {
            tcphy_speed = tcphy_speed_Force10F;
            tcPhyWriteReg(port_num,0,0x0100);
        }
        else if(stricmp(argv[1], "10H") == 0 )
        {
            tcphy_speed = tcphy_speed_Force10H;
            tcPhyWriteReg(port_num,0,0x0000);
        }
        else 
        {
            printf("Current Speed mode: %s.\r\n",
                    (tcphy_speed==tcphy_speed_Auto)?"Auto":
                    (tcphy_speed==tcphy_speed_ForceAN)?"ForceAN":
                    (tcphy_speed==tcphy_speed_Force100F)?"Force100F":
                    (tcphy_speed==tcphy_speed_Force100H)?"Force100H":
                    (tcphy_speed==tcphy_speed_Force10F)?"Force10F":
                    (tcphy_speed==tcphy_speed_Force10H)?"Force10H":
                                                        "Unknown");
        }
    }
    return 0;
}
#endif



int buck_pbus_read_command(int argc, char *argv[], void *p)
{
	unsigned int reg_addr,phyaddr,data;
	
		if((argc <=2)||(argc >=4)) 
	{
	   	printf("bpr <phyaddr> <register>\r\n");
	}
	else
	{
		phyaddr = checked_atoi(argv[1]);
		sscanf(argv[2], "%x", &reg_addr);//hex
		data=buck_pbus_read( phyaddr, reg_addr);
		printf("phy=%d register=0x%x data=0x%x \r\n",phyaddr,reg_addr,data);
		
	}
	
	
}
int buck_pbus_write_command(int argc, char *argv[], void *p)
{
	unsigned int reg_addr,phyaddr,value,data;
	
		if((argc <=3)||(argc >=5)) 
	{
	   	printf("bpw <phyaddr> <register> <value> \r\n");
	}
	else
	{
		phyaddr = checked_atoi(argv[1]);
		sscanf(argv[2], "%x", &reg_addr);//hex
		sscanf(argv[3], "%x", &value);//hex
		buck_pbus_write( phyaddr, reg_addr,value);
		data=buck_pbus_read( phyaddr, reg_addr);
		printf("phy=%d register=0x%x data=0x%x \r\n",phyaddr,reg_addr,data);
		
	}
	
	
}


int doPhypowerSave (int phyaddr,int power_save_enable)
{
		if(power_save_enable==1) 
		{
			mtEMiiRegWrite(phyaddr, 0x1f, 0x0268, 0x07f4);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0269, 0x1114);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0271, 0x2ca5);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0272, 0x0c6b);
				
				mtEMiiRegWrite(phyaddr, 0x1e, 0x0011, 0xff00);
				
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0202, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0203, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0204, 0x2019);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0205, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0206, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0207, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0208, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0209, 0x23);
				
				mtEMiiRegWrite(phyaddr, 0x1f, 0x020a, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x020b, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x020e, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x020f, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0210, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0211, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0212, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0213, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0214, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0215, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0216, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0217, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021a, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021b, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021c, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021d, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021e, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021f, 0x23);
				
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0220, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0221, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0222, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0223, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0226, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0227, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0228, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0229, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022a, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022b, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022c, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022d, 0x23);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022e, 0x2219);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022f, 0x23);

		}
		if(power_save_enable==0) 
		{
			mtEMiiRegWrite(phyaddr, 0x1f, 0x0268, 0x07f4);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0269, 0x111f);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0271, 0x7ca5);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0272, 0x0c7f);
				
				mtEMiiRegWrite(phyaddr, 0x1e, 0x0011, 0x0f00);
				
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0202, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0203, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0204, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0205, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0206, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0207, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0208, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0209, 0x20);
				
				mtEMiiRegWrite(phyaddr, 0x1f, 0x020a, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x020b, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x020e, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x020f, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0210, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0211, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0212, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0213, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0214, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0215, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0216, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0217, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021a, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021b, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021c, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021d, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021e, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x021f, 0x20);
				
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0220, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0221, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0222, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0223, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0226, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0227, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0228, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x0229, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022a, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022b, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022c, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022d, 0x20);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022e, 0x2020);
				mtEMiiRegWrite(phyaddr, 0x1f, 0x022f, 0x20);

		}
}


int doPhyGphyTestMode (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0, port = 0, mode=0, pair=0, speed=0;
	uint32 reg3 = 0, r15_temp = 0,temp1,temp2;

	if((argc <4)||(argc >=6)) 
	{
	   	printf("Usage: testmode <speed> <phyaddr> <mode or channel> <pair>\r\n");
	}
	else	
	{
		speed=checked_atoi(argv[1]);
    	phyaddr = checked_atoi(argv[2]);
		mode=checked_atoi(argv[3]);
    	if(argc ==5) {pair=checked_atoi(argv[4]);}
			if(speed == 2500 )	
		  	{
			    buck_pbus_write(phyaddr,0x10204,0x0);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x800c, 0x0008);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x800d, 0x0000);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x800e, 0x1100);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x800f, 0x0001);

			 if(mode == 1 )
			  {
				  temp1=buck_pbus_read(phyaddr,0x30008);
				  temp2=temp1|0x07;
				  buck_pbus_write(phyaddr,0x30008,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=temp1|0x07000000;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xfff0ffff)|0x00010000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  printf("port[%d] into 2.5G testmode 1\r\n",phyaddr);					  
			  }
			 if(mode == 2)
			  {
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=(temp1&0xfffeffff)|0x00010000;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xfff0ffff)|0x00020000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=(temp1&0xfffeffff)|0x00000000;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  printf("port[%d] into 2.5G testmode 2\r\n",phyaddr);	
				  
			  }		
			 if(mode == 3)
			  {
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=(temp1&0xf8feffff)|0x01010000;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xfff0ffff)|0x00030000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  temp1=buck_pbus_read(phyaddr,0x85024);
				  temp2=temp1&0xfeffffff;
				  buck_pbus_write(phyaddr,0x85024,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=temp1&0xfffeffff;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  delay1ms(1000);
				  temp1=buck_pbus_read(phyaddr,0x10608);
				  temp2=(temp1&0xfffff000)|0x808;
				  buck_pbus_write(phyaddr,0x10608,temp2);
				  printf("port[%d] into 2.5G testmode 3\r\n",phyaddr);	
				  
			  }
			 if(mode == 4)
			  {
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=(temp1&0xfffeffff)|0x00010000;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  if(pair == 1 ) 
				  {
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xff8fffff)|0x00100000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  printf("port[%d] into 2.5G testmode 4 tone 1\r\n",phyaddr);
				  }
				  if(pair == 2 ) 
				  {
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xff8fffff)|0x00200000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  printf("port[%d] into 2.5G testmode 4 tone 2\r\n",phyaddr);
				  }
				  if(pair == 3 ) 
				  {
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xff8fffff)|0x00300000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  printf("port[%d] into 2.5G testmode 4 tone 3\r\n",phyaddr);
				  }
				  if(pair == 4 ) 
				  {
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xff8fffff)|0x00400000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  printf("port[%d] into 2.5G testmode 4 tone 4\r\n",phyaddr);
				  }
				  if(pair == 5 ) 
				  {
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xff8fffff)|0x00500000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  printf("port[%d] into 2.5G testmode 4 tone 5\r\n",phyaddr);
				  }
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xfff0ffff)|0x00040000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=(temp1&0xfffeffff)|0x00000000;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  temp1=buck_pbus_read(phyaddr,0x3089c);
				  temp2=(temp1&0xfffff800)|0x000001ff;
				  buck_pbus_write(phyaddr,0x3089c,temp2);
					  
			  }		
			 if(mode == 5)
			  {
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=temp1|0x00010000;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xfff0ffff)|0x00050000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=temp1&0xfffeffff;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30080);
				  temp2=(temp1&0xfffffff8)|0x00000006;
				  buck_pbus_write(phyaddr,0x30080,temp2);
				  printf("port[%d] into 2.5G testmode 5\r\n",phyaddr);					  
			  }	
			 if(mode == 6)
			  {
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=temp1|0x00010000;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30004);
				  temp2=(temp1&0xfff0ffff)|0x00060000;
				  buck_pbus_write(phyaddr,0x30004,temp2);
				  temp1=buck_pbus_read(phyaddr,0x30200);
				  temp2=temp1&0xfffeffff;
				  buck_pbus_write(phyaddr,0x30200,temp2);
				  printf("port[%d] into 2.5G testmode 6\r\n",phyaddr);				  
			  }	
			  
			}
		  if(speed == 1000 )	{
			if(mode == 0 )
			{	//default
		        doPhypowerSave (phyaddr,1);
				//disable packet gen
	            tcMiiStationWrite(phyaddr, 0x1f, 0x0001);
	            tcMiiStationWrite(phyaddr, 0x1d, 0x0000);
				tcMiiStationWrite(phyaddr, 9, 0x1e00);
				tcMiiStationWrite(phyaddr, 0x0, 0x1040);
            	mtEMiiRegWrite(phyaddr, 0x1e, 0x145, 0x1000);	
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x1a3, 0x00d2);
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x1a4, 0x010e);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xc000);				
				printf("Usage: default\r\n");
			}
			if(mode == 1 )
			{	//Gphy test mode1
				
				doPhypowerSave (phyaddr,0);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xc000);
				tcMiiStationWrite(phyaddr, 9, 0x2700);
				printf("Usage:port[%d] into testmode1\r\n",phyaddr);
			}
			if(mode == 2 )
			{	//Gphy test mode2
                doPhypowerSave (phyaddr,0);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xc000);
				tcMiiStationWrite(phyaddr, 9, 0x4700);
				printf("Usage:port[%d] into testmode2\r\n",phyaddr);
			}
			if(mode == 3 )
			{	//Gphy test mode3
                doPhypowerSave (phyaddr,0);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xc000);
				tcMiiStationWrite(phyaddr, 9, 0x6700);
				printf("Usage:port[%d] into testmode3\r\n",phyaddr);
			}
			if(mode == 4 )
			{	//Gphy test mode4
				
			if(pair == 1 )
			{	//Gphy test mode4 pairA
				mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0x7070);
				printf("Usage:port[%d] into testmode4_pairA\r\n",phyaddr);
			}
			if(pair == 2)
			{	//Gphy test mode4 pairB
				mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xb0b0);
    			printf("Usage:port[%d] into testmode4_pairB\r\n",phyaddr);
		    }
			if(pair == 3 )
			{	//Gphy test mode4 pairC
			   	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xd0d0);
				printf("Usage:port[%d] into testmode4_pairC\r\n",phyaddr);
		    }
			if(pair == 4 )
			{	//Gphy test mode4 pairD
				mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xe0e0);
				printf("Usage:port[%d] into testmode4_pairD\r\n",phyaddr);
	        }
                doPhypowerSave (phyaddr,0);

            tcMiiStationWrite(phyaddr, 9, 0x8700);
				//tcMiiStationWrite(phyaddr, 9, 0x8700);
				//printf("Usage:port[%d] into testmode4\r\n",phyaddr);
}
		  	}
		if(speed == 100 )	{
			//send pair A 100M MLT3
			if(mode == 1 ) 
			{
			    //disable packet gen
	            tcMiiStationWrite(phyaddr, 0x1f, 0x0001);
	            tcMiiStationWrite(phyaddr, 0x1d, 0x0000);
            	tcMiiStationWrite(phyaddr, 0x1f, 0x0000);
	            tcMiiStationWrite(phyaddr, 0x0, 0x8000);
	            udelay(100);

	            tcMiiStationWrite(phyaddr, 0x9, 0x0600);
             	tcMiiStationWrite(phyaddr, 0x0, 0x2100);
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x145, 0x5010);	
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x1a3, 0x00d2);
            	mtEMiiRegWrite(phyaddr, 0x1e, 0x1a4, 0x010e);
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x3e, 0xc000);  //disable power save mode for pair A &B	
	            printf("set p[%d] 100M pairA \r\n", phyaddr);	
			}
			//send pair B MLT3
			if(mode == 2 ) 
			{
				//disable packet gen
	            tcMiiStationWrite(phyaddr, 0x1f, 0x0001);
	            tcMiiStationWrite(phyaddr, 0x1d, 0x0000);
	            tcMiiStationWrite(phyaddr, 0x1f, 0x0000);
	            tcMiiStationWrite(phyaddr, 0x0, 0x8000);
	            udelay(100);
	            tcMiiStationWrite(phyaddr, 0x9, 0x0600);
	            tcMiiStationWrite(phyaddr, 0x0, 0x2100);
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x145, 0x5018);	
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x1a3, 0x00d2);
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x1a4, 0x010e);
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x3e, 0xc000);  //disable power save mode for pair A &B					
	            printf("set p[%d] 100M pairB \r\n", phyaddr);					
			}
			
		 }
		if(speed == 10 )	{ 
			//send pair A NLP
			if(mode == 1 ) 
			{
	           tcMiiStationWrite(phyaddr, 0x1f, 0x0001);
	           tcMiiStationWrite(phyaddr, 0x1d, 0x0000);
	
	           tcMiiStationWrite(phyaddr, 0x1f, 0x0000);
	           tcMiiStationWrite(phyaddr, 0x0, 0x8000);

	           udelay(100);
	
	           //set force link 10M
	           tcMiiStationWrite(phyaddr, 0x0, 0x0100);
	           //set giga ability and switch normal mode
               tcMiiStationWrite(phyaddr, 0x9, 0x0600);
	           //desable 10 base TE
	           mtEMiiRegWrite(phyaddr, 0x1f, 0x27b, 0x1177);  
	           //disable auto MDIX and force MDI
	           mtEMiiRegWrite(phyaddr, 0x1e, 0x145, 0x5010);
               //return to normal
	           mtEMiiRegWrite(phyaddr, 0x1e, 0x1a3, 0x00d2);
	           mtEMiiRegWrite(phyaddr, 0x1e, 0x1a4, 0x010e);
	           mtEMiiRegWrite(phyaddr, 0x1e, 0x3e, 0xc000);  //disable power save mode for pair A &B

	           printf("set p[%d] 10M link pulse \r\n", phyaddr);				
			}		
			//send pair A random
			if(mode == 2 ) 
			{
	           //disable packet gen
	           tcMiiStationWrite(phyaddr, 0x1f, 0x0001);
	           tcMiiStationWrite(phyaddr, 0x1d, 0x0000);
	           //do phy software reset
	           tcMiiStationWrite(phyaddr, 0x1f, 0x0000);
	           tcMiiStationWrite(phyaddr, 0x0, 0x8000);

	           udelay(100);
	           //set force link 10M
	           tcMiiStationWrite(phyaddr, 0x0, 0x0100);
	           //set giga ability and switch normal mode
               tcMiiStationWrite(phyaddr, 0x9, 0x0600);
	
	           //desable 10 base TE
	           mtEMiiRegWrite(phyaddr, 0x1f, 0x27b, 0x1177);  
	           //disable auto MDIX and force MDI
	           mtEMiiRegWrite(phyaddr, 0x1e, 0x145, 0x5010);
	           //return to normal
	           mtEMiiRegWrite(phyaddr, 0x1e, 0x1a3, 0x00d2);
	           mtEMiiRegWrite(phyaddr, 0x1e, 0x1a4, 0x010e);
               //set random packet
	           tcMiiStationWrite(phyaddr, 0x1f, 0x0001);
	           tcMiiStationWrite(phyaddr, 0x1d, 0xf842);
               mtEMiiRegWrite(phyaddr, 0x1e, 0x3e, 0xc000);  //disable power save mode for pair A &B

	           printf("set p[%d] 10M random \r\n", phyaddr);
			}		
			//send pair A fixed
			if(mode == 3 ) 
			{
                //disable packet gen
	            tcMiiStationWrite(phyaddr, 0x1f, 0x0001);
	            tcMiiStationWrite(phyaddr, 0x1d, 0x0000);
	            //do phy software reset
	            tcMiiStationWrite(phyaddr, 0x1f, 0x0000);
	            tcMiiStationWrite(phyaddr, 0x0, 0x8000);
             	udelay(100);
	
	            //set force link 10M
	            tcMiiStationWrite(phyaddr, 0x0, 0x0100);
	            //set giga ability and switch normal mode
                tcMiiStationWrite(phyaddr, 0x9, 0x0200);
	            //desable 10 base TE
	            mtEMiiRegWrite(phyaddr, 0x1f, 0x27b, 0x1177);  
	            //disable auto MDIX and force MDI
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x145, 0x5010);
                //for harmonic
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x1a3, 0x0000);
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x1a4, 0x0000);

                //set fixed data packet  0x00
	            tcMiiStationWrite(phyaddr, 0x1f, 0x0001);
                tcMiiStationWrite(phyaddr, 0x1e, 0x0000);
	            tcMiiStationWrite(phyaddr, 0x1d, 0xf840);
	            mtEMiiRegWrite(phyaddr, 0x1e, 0x3e, 0xc000);  //disable power save mode for pair A &B
            	printf("set p[%d] 10M fix \r\n", phyaddr);
			}					
		
		  
		}	
		}
}

int sw_ability_setting(int argc, char *argv[], void *p)
{
	unsigned int sw_ability,switch_status;
	if((argc <=2)||(argc >=4)) 
	{
	   	printf("sw_ability_set <SDS|FS|SLT> <on|ff>\r\n");
	}
	else	
	{
		sw_ability=checked_atoi(argv[1]);
    	switch_status = checked_atoi(argv[2]);
	if(sw_ability==1) 
	{
		if(switch_status==1) { sw_down_shift_en=1;printf("software downshift enable\r\n");} else { sw_down_shift_en=0;printf("software downshift disable\r\n");}
	}
		if(sw_ability==2) 
	{
		if(switch_status==1) { phy_downshift_force_giga_slave_enable=1;printf("giga downshift force slave enable\r\n");} else { phy_downshift_force_giga_slave_enable=0;printf("giga downshift force slave disable\r\n");}
	}
		if(sw_ability==3) 
	{
		if(switch_status==1) { slt_excuting=1;printf("slt excuting\r\n");} else { slt_excuting=0;printf("slt not excuting\r\n");}
	}	
	}

}

int doReadAllCalData(int argc, char *argv[], void *p)
{
	uint16 do_cal_loop,mode,show_temp;
	unsigned int  reg_1e_12_read,reg_1e_16_read,reg_1e_17_read,reg_1e_18_read,reg_1e_19_read,reg_1e_20_read,reg_1e_21_read,reg_1e_22_read,reg_1e_174_read,reg_1e_175_read;
	


phyTxAmpcomp(0);


			for(do_cal_loop = 9; do_cal_loop<=12; do_cal_loop ++)
		{

	reg_1e_12_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x12);
	reg_1e_16_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x16);
	reg_1e_17_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x17);
	reg_1e_18_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x18);
	reg_1e_19_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x19);
	reg_1e_20_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x20);
	reg_1e_21_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x21);
	reg_1e_22_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x22);
	reg_1e_174_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x174);
	reg_1e_175_read = mtEMiiRegRead(do_cal_loop, 0x1e, 0x175);
	
	printf("Port %d \r\n",do_cal_loop);
	show_temp=reg_1e_16_read&0x003f;
	printf("tx_amp_tst_cha = 0x%x " ,show_temp);
	show_temp=reg_1e_18_read&0x003f;		
	printf("tx_amp_tst_chb = 0x%x " ,show_temp);
	show_temp=reg_1e_20_read&0x003f;	
	printf("tx_amp_tst_chc = 0x%x " ,show_temp);
	show_temp=reg_1e_22_read&0x003f;
	printf("tx_amp_tst_chd = 0x%x \n\r" ,show_temp);
	
	show_temp=(reg_1e_12_read&0xfc00)/1024;
	printf("tx_amp_gbe_cha = 0x%x " ,show_temp);
	show_temp=(reg_1e_17_read&0x3f00)/256;
	printf("tx_amp_gbe_chb = 0x%x " ,show_temp);
	show_temp=(reg_1e_19_read&0x3f00)/256;
	printf("tx_amp_gbe_chc = 0x%x " ,show_temp);
	show_temp=(reg_1e_21_read&0x3f00)/256;
	printf("tx_amp_gbe_chd = 0x%x \n\r" ,show_temp);
	
	show_temp=(reg_1e_16_read&0xfc00)/1024;	
	printf("tx_amp_hbt_cha = 0x%x " ,show_temp);
	show_temp=(reg_1e_18_read&0x3f00)/256;
	printf("tx_amp_hbt_chb = 0x%x " ,show_temp);
	
	show_temp=reg_1e_12_read&0x003f;	
	printf("tx_amp_tbt_cha = 0x%x " ,show_temp);
	show_temp=reg_1e_17_read&0x003f;
	printf("tx_amp_tbt_chb = 0x%x \n\r" ,show_temp);
	
	show_temp=(reg_1e_174_read&0x7f00)/256;	
	printf("R50_cha = 0x%x " ,show_temp);
	show_temp=reg_1e_174_read&0x007f;
	printf("R50_chb = 0x%x " ,show_temp);
	show_temp=(reg_1e_175_read&0x7f00)/256;	
	printf("R50_chc = 0x%x " ,show_temp);
	show_temp=reg_1e_175_read&0x007f;
	printf("R50_chd = 0x%x \n\r" ,show_temp);

	printf("  1f_200 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x200));
	printf("  1f_201 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x201));
    printf("  1f_202 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x202));
	printf("  1f_203 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x203));
	printf("  1f_204 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x204));
	printf("  1f_205 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x205));
    printf("  1f_206 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x206));
	printf("  1f_207 = 0x%x\n\r" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x207));			
	printf("  1f_208 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x208));
	printf("  1f_209 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x209));
    printf("  1f_20a = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x20a));
	printf("  1f_20b = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x20b));
	printf("  1f_20c = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x20c));
	printf("  1f_20d = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x20d));
    printf("  1f_20e = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x20e));
	printf("  1f_20f = 0x%x\n\r" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x20f));	

	printf("  1f_210 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x210));
	printf("  1f_211 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x211));
    printf("  1f_212 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x212));
	printf("  1f_213 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x213));
	printf("  1f_214 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x214));
	printf("  1f_215 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x215));
    printf("  1f_216 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x216));
	printf("  1f_217 = 0x%x\n\r" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x217));			
	printf("  1f_218 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x218));
	printf("  1f_219 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x219));
    printf("  1f_21a = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x21a));
	printf("  1f_21b = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x21b));
	printf("  1f_21c = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x21c));
	printf("  1f_21d = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x21d));
    printf("  1f_21e = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x21e));
	printf("  1f_21f = 0x%x\n\r" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x21f));

	printf("  1f_220 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x220));
	printf("  1f_221 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x221));
    printf("  1f_222 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x222));
	printf("  1f_223 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x223));
	printf("  1f_224 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x224));
	printf("  1f_225 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x225));
    printf("  1f_226 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x226));
	printf("  1f_227 = 0x%x\n\r" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x227));			
	printf("  1f_228 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x228));
	printf("  1f_229 = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x229));
    printf("  1f_22a = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x22a));
	printf("  1f_22b = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x22b));
	printf("  1f_22c = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x22c));
	printf("  1f_22d = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x22d));
    printf("  1f_22e = 0x%x" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x22e));
	printf("  1f_22f = 0x%x\n\r" , mtEMiiRegRead(do_cal_loop, 0x1f, 0x22f));

	// printf downshift status
	tcMiiStationWrite(do_cal_loop, 0x1f, 0x1);
	printf("  Ext_14 = 0x%x\n\r" , tcMiiStationRead(do_cal_loop, 0x14));
	tcMiiStationWrite(do_cal_loop, 0x1f, 0x0);
	
	}
	    if(sw_down_shift_en == 1 ) { printf("software downshift enable\r\n");} else { printf("software downshift disable\r\n");}
		if(slt_excuting == 1 ) { printf("slt_excuting\r\n");} else { printf("not slt_excuting\r\n");}
		if(phy_downshift_force_giga_slave_enable == 1 ) { printf("giga downshift force slave enable\r\n");} else { printf("giga downshift force slave disable\r\n");}

        printf("\r\n");

	return 0; //Coverity #139525
}

#endif // TCPHY_SUPPORT

/************************************************************************
*                       API functions body for EPHY
**************************************************************************/
#if defined(TCPHY_DEBUG) || defined(MTPHY_DEBUG)
#if debug_flag

static int doPhyInit(int argc, char *argv[], void *p)
{
    tcPhyInit();

    return 0;
}
#endif
#endif

#if defined(TCPHY_DEBUG)|| defined(MTPHY_DEBUG)
static int doPhySwPatch (int argc, char *argv[], void *p)
{
	uint8 pn;
	
    if(argc != 2 ) 
	{
        printf("Sw patch status: %s.\r\n", (sw_patch_flag? "on": "off"));
    }
    else if(stricmp(argv[1], "on") == 0 )  
	{
        sw_patch_flag = 1;      
        printf("Sw patch status: ON.\r\n");
    }
    else if(stricmp(argv[1], "off") == 0 ) 
	{
        sw_patch_flag = 0;      
        printf("Sw patch status: OFF.\r\n");
    }           
	else if(stricmp(argv[1], "dbg") == 0) // allen_20140822
	{
		printf("\n tcPhyVer = %d \n", tcPhyVer);

		pn = 4;
		printf("mtPhyReg case 1 :Nmr1[%d].value =0x%04lx\r\n", pn, Nmr1[pn].value);
		printf("mtPhyReg case 1 :Nmr1[%d].link_status_prev =0x%04lx\r\n", pn, Nmr1[pn].link_status_prev );
		printf("mtPhyReg case 1 :Nmr1[%d].link_status =0x%04lx\r\n", pn, Nmr1[pn].link_status);
	}	
    else 
	{
        printf("Sw patch status: %s.\r\n", (sw_patch_flag? "on": "off"));
    }
    return 0;
}
#endif

#if defined(TCPHY_DEBUG)
#if debug_flag
static int doPhyRegCheck (int argc, char *argv[], void *p)
{
    uint32 val, i;
    uint32 phyaddr;
    uint32 reg;
    uint32 loop;
    uint32 val_inc;
    volatile uint32 data;

    if((argc != 5) && (argc != 6) )
    {
        printf("Usage: regcheck <PhyAddr> <PhyReg> <Value> <Loop> [ValueIncStep]\r\n");
        printf("Note:  turn off periodic mii access before test.\r\n");
        return 0;
    }
    
    phyaddr = atoi(argv[1]);
    reg = atoi(argv[2]);
	//  val = atoi(argv[3]);
    sscanf(argv[3], "%lx", &val);

    loop = atoi(argv[4]);
	sscanf(argv[5], "%lx", &val_inc);

    for(i=0; i<loop; i++ )
    {
        if(argc==6)
        {
            val = (val+val_inc)&0xffff;
            tcMiiStationWrite(phyaddr,reg,val);
        }

        data = tcMiiStationRead(phyaddr, reg);
        if(data != val )
        {
            printf("\r\nError!! Phyaddr=%ld reg=%ld loop=%ld Expected=0x%lx Read=0x%lx\r\n", 
                phyaddr, reg, i, val, data);
            return 0;
        }
        
        if((i%10000) == 0)
        {
            printf(".");
        }
    }
    printf("\r\nLoop=%ld Done!!\r\n", i);
    return 0;
}
#endif
// called by macRxRingProc() for doPhyLoopback()
#ifdef LINUX_OS
uint8 phyMbpChk(struct sk_buff* oldMbp)
{
#else
uint8 phyMbpChk(mbuf_t*oldMbp)
{
#endif
    if(phy_loopback_flag)
    {
        recv_ok_flag = 1;
        return 1;
    }
    
    if((oldMbp->data[12] == 8) && (oldMbp->data[13] == 0) )
    {	// ip packet
        if(oldMbp->data[34] == 0 && oldMbp->data[23] == 1)//Echo Ping Reply
        {
            if( oldMbp->data[50] == 'B' && oldMbp->data[51] == 'E' && 
                 oldMbp->data[52] == 'A' && oldMbp->data[53] == 'F' )
            {
                PingReplyCnt++;
                return 1;
            }
        }
    }   
    return 0;
}

// called by macRxRingProc() for doPhyLoopback()
#ifdef LINUX_OS
uint8 phy_recv_err_check(struct sk_buff** oldMbp, uint32 *frameSize)
{
	#ifdef TC2031_DEBUG
 	#if  !(defined(CONFIG_MIPS_TC3182) ||  defined(CONFIG_MIPS_TC3262)) ||  defined(TCSUPPORT_CPU_ARMV8)
	int i = 0;
	if(phy_loopback_flag)
	{
	#if 0  /*jason.gong allen*/
		if(rxDescrp->rdes0.word & (1<<30))
		{
			*frameSize = rxDescrp->rdes0.bits.rfl;
			*frameSize = (*frameSize > 60) ? *frameSize : 60;
		}
		else
		{
			*frameSize = rxDescrp->rdes0.bits.rfl;
		}
		*oldMbp = (struct sk_buff *)(rxDescrp->skb);
		skb_put(*oldMbp, *frameSize);
		dbg_plinel_1("Received packet have CRC error! length=", *frameSize);
		dbg_pline_1("\r\n");
		for(i=0; i<*frameSize; i++ )
			dbg_plineb_1(" ", (*oldMbp)->data[i]);
		dbg_pline_1("\r\n");
		recv_err_flag = 1;	
		#endif
	}
	#endif
	#endif
	return 0;
}
#else
uint8 phy_recv_err_check(mbuf_t **oldMbp, macRxDescr_t *rxDescrp, uint32 *frameSize)
{
    #ifdef TC2031_DEBUG
    int i = 0;
    if(phy_loopback_flag)
    {
    	if(rxDescrp->rdes0.word & (1<<30))
        {
        	*frameSize = rxDescrp->rdes0.bits.rfl;
            *frameSize = (*frameSize > 60) ? *frameSize : 60;
        }
        else
            *frameSize = rxDescrp->rdes0.bits.rfl;                      
        (volatile mbuf_t *)(*oldMbp) = (volatile mbuf_t *)(rxDescrp->mbuf);
        (*oldMbp)->cnt = *frameSize;
        dbg_plinel_1("Received packet have CRC error! length=", *frameSize);
        dbg_pline_1("\r\n");
        for( i=0; i<*frameSize; i++ )
            dbg_plineb_1(" ", (*oldMbp)->data[i]);
        dbg_pline_1("\r\n");
        recv_err_flag = 1;                      
    }
    #endif
    return 0;
}
#endif

#ifdef LINUX_OS
/*======================================================================================
**      function name: reverseLong
**      descriptions: Reverse the bytes ordering of the input value.
**      parameters:
**            ul: Specify the 4 bytes value that you want to reverse the ordering.
**      revision: Here 2008/08/24
========================================================================================*/
static uint32 reverseLong(uint32 ul)
{
	uint8 z[4];
	z[3] = *((uint8 *)&ul + 0);
	z[2] = *((uint8 *)&ul + 1);
	z[1] = *((uint8 *)&ul + 2);
	z[0] = *((uint8 *)&ul + 3);
	return *((uint32 *)z);
}/*end reverseLong*/
/*======================================================================================
**      function name: scramble
**      descriptions: Scramble the input 32bits value.            
**      return:  After Scramble the value
**      call: reverseLong
**      revision: Here 2008/08/24
=========================================================================================*/
static uint32 scramble(uint32 checkCode)
{
	uint32 a[6];
	a[1] = (checkCode & 0x0000001F) << 0x0C;
	a[2] = (checkCode & 0x03E003E0) << 0x01;
	a[3] = (checkCode & 0xF8000400) >> 0x0A;
	a[4] = (checkCode & 0x0000F800) << 0x10;
	a[5] = (checkCode & 0x041F0000) >> 0x0F;
	checkCode = a[1] + a[2] + a[3] + a[4] + a[5];
	/* ICQ's check code is little-endian. Change the endian style */
	checkCode = reverseLong(checkCode);
	return checkCode;
}/*end scramble*/
/*==============================================================================
**      function name: rand
**      descriptions: Random value generation. 
**      return: Random value
**      call: timerVlrGet
**      revision: Here 2008/08/24
================================================================================*/
static uint32 rand(void)
{
	uint32 vlr;
	timerVlrGet(1, vlr);
	scramble(vlr);
	return (vlr & RAND_MAX);
}
#if 0
/*================================================================================
**      function name: doMacSend
**      descriptions: send packet  with the  data pattern we specific
**      parameters:
**            argc : argument number
**            argv : argument point 
**		 		p  : no use
**      return:
**            Success:        0
**            Otherwise:     -1
=================================================================================*/
static int doMacSend(int argc, char *argv[], void *p)
{
	uint8 pattern;
	uint32 pattern32;
	uint32 len;
	uint32 loop;
	uint32 delay_cnt = 0;	
	struct sk_buff *mbp = NULL;
	uint32 i,j;
	
	sscanf(argv[1], "%lx", &pattern32);
	len = simple_strtoul(argv[2],NULL,10);
	loop = simple_strtoul(argv[3],NULL,10);
	if(argv[4] != NULL)
	{
		delay_cnt  = simple_strtoul(argv[4],NULL,10);
	}
	pattern = (uint8) pattern32;
	dbg_plineb_1("\n\rFill pattern  =",pattern);
	dbg_plinel_1("\n\rPacket length =",len);
	dbg_plinel_1("\n\rLoop cnt      =",loop);
	dbg_plinel_1("\n\rdelay_cnt      =",delay_cnt);
	j = 0;
	while (j < loop) 
	{
		mbp = (struct sk_buff *)skbmgr_dev_alloc_skb2k();
		if(mbp == NULL)
		{
			continue;
		}			
		skb_reserve(mbp, 2);
		skb_put(mbp, len);
		for(i = 0; i < len; i++)
		{	/* fill pattern */
			mbp->data[i] = pattern;
		}
		//macSend(0, mbp);
		ETHER_MAC_SEND(mbp);
		j++;

		
		if( delay_cnt != 0 )
		{
			if( (j%delay_cnt)==0)
				delay1ms(1);
		}
	}
	dbg_pline_1("\n\rDone!\n\r");
	return 0;
}
/*===============================================================
**      function name: doMacSendRandom
**      descriptions: send packet with random dataa pattern
**      parameters:
**            argc : argument number
**            argv : argument point 
**		 		p  : no use      
**      return:
**            Success:        0
**            Otherwise:     -1
=================================================================*/
static int doMacSendRandom(int argc, char *argv[], void *p)
{
	uint32 len=0;
	uint32 loop=0;
	struct sk_buff *mbp = NULL;
	uint32 i, j;
	
	if(argc == 2)
	{
		loop = simple_strtoul(argv[1],NULL,10);
		dbg_plinel_1("\n\rLoop cnt      =",loop);
	} 
	else if(argc==3)
	{
		len = simple_strtoul(argv[1],NULL,10);
		loop = simple_strtoul(argv[2],NULL,10);
		dbg_plinel_1("\n\rLoop cnt      =",loop);		
		dbg_plinel_1("\n\rPacket length =",len);		
	}
	else
	{
		printf("Usage: sendrandom <len> <loop>\r\n");
		printf("       sendrandom <loop>\r\n");
		return 0;
	}
	j = 0;
	while(j < loop) 
	{
		mbp = (struct sk_buff *)skbmgr_dev_alloc_skb2k();
		if(mbp == NULL)
		{
			continue;
		}
		/* da */  // broadcast
		mbp->data[0] = 0xff;
		mbp->data[1] = 0xff;
		mbp->data[2] = 0xff;
		mbp->data[3] = 0xff;
		mbp->data[4] = 0xff;
		mbp->data[5] = 0xff;
		/* sa */
		mbp->data[6] = 0x00;
		mbp->data[7] = 0x00;
		mbp->data[8] = 0x00;
		mbp->data[9] = 0x00;
		mbp->data[10] = 0x00;
		mbp->data[11] = 0x01;
		/* ether type */
		mbp->data[12] = 0xff;
		mbp->data[13] = 0xff;
		if(argc == 2)		
			len = rand();
		if(len < 60)
			len = 60;
		if(len > 1514)
			len = 1514;
		for(i = 14; i < len; i++)
		{	/* fill pattern */
			mbp->data[i] = (uint8) (rand() + rand());
		}
		skb_reserve(mbp, 2);
		skb_put(mbp, len);
		
		//macSend(0, mbp);
		ETHER_MAC_SEND(mbp);
		j++;
		
		// print "." every 1024 packets
		if((j % 1024) == 0) 
		{
			dbg_pline_1(".");
			//dbg_plinel_1("\r\n loop=", j);
		}
	}
	dbg_plinel_1("\r\n loop=", j);
	dbg_pline_1("\n\rDone!\n\r");
	return 0;
}
#endif
#endif
#ifndef PURE_BRIDGE
#ifndef LINUX_OS
// called by doPing()
static int pingExec(cbuf_t  *cbp,           /* cbuf pointer         */
    uint16  signature,      /* event function signature */
    void    *data_p,        /* data pointer         */
    uint16  data_size       /* data size            */)
{
    pingEvt_t *ping_evt_p = (pingEvt_t *)data_p;
    pingtx( ping_evt_p->ping_p );
    return 0;
} /* pintExec */
// called by doPing()
static int pingKeyProc(int c)
{
    struct usock *up;
    extern pingCmdCb_t pingCmdCb;
    /* Ignore all but ^C */
    if(c != CTLC)
        return 0;
    if (pingCmdCb.actSocket == -1 || (up=itop(pingCmdCb.actSocket)) == NULL)
        return 0;
    printf("^C\n");
    malert(up->eventid,1);  /* zc 5/18/95 - change to mb */
    pingCmdCb.actSocket = -1;
    pingCmdCb.flags.active = 0;
    return 0;
} /* pingKeyProc */
#endif
#if debug_flag
static int doPing(int argc, char *argv[], void *p)
{       
#if 0
	#ifndef LINUX_OS	
    pingEvt_t ping_evt;
    mbuf_t  *bp;
    struct sockaddr_in from;
    icmp_t  icmp;
    ping_t  *ping_p;
    uint32  timestamp;
    uint32  rtt;
    int32   abserr;
    ip4a    target, dnsQueryResult;
    int ip_sock;
    int fromlen;
    int max_size;
    int retvalue = 0;
    extern pingCmdCb_t pingCmdCb;
    int tmp;
	//  ping_test_flag = 1;

    // clear ReplyCnt
    PingReplyCnt = 0;
    if((ping_p=calloc(1, sizeof(ping_t))) == NULL)
        sysreset();
    if((ip_sock = socket(AF_INET,SOCK_RAW,ICMP_PTCL)) == -1)
    {
        printf("Can't create socket\n");
        retvalue = 1;
        goto ping_exit1;
    }
    ping_p->s = ip_sock;

    printf("Resolving %s... ",argv[1]);
    dnsQueryResult = resolveName(argv[1]);
    if(!dnsQueryResult)
    {
        printf("unknown\n");
        retvalue = 1;
        goto ping_exit;
    }
    if(resolve( inet_ntoa(dnsQueryResult), &ping_p->target, NULL) != 0)
    {
        printf("unknown\n");
        retvalue = 1;
        goto ping_exit;
    }
    printf("%s\n",inet_ntoa(ping_p->target));
    
    pingCmdCb.flags.user = 1;
    pingCmdCb.option = 1;
    
    tmp = atoi(argv[2]) - 42;
    if(tmp < 18 || tmp > 1472)
    {
        printf("Valid packet length is: 60 < len < 1514\r\n");
        goto ping_exit;
    }
    else
        ping_p->len = tmp;
    tmp = atoi(argv[3]);
    if(tmp <= 0)
    {
        printf("Valid loopnum is: loopnum > 0\r\n");
        goto ping_exit;
    }
    else
        pingCmdCb.pingCnt = tmp;
    tmp = atoi(argv[4]);
    if(tmp < 0)
        ping_p->interval = 0;
    else
        ping_p->interval = tmp;

	ping_start:
    TRCLOG3216( LOG_IP, 5, "doping %lx %x %x", ping_p->target,ping_p->len,ping_p->interval);

    /* user has specified interval */
    pingCmdCb.flags.active = 1;
    pingCmdCb.actSocket = ip_sock;
    if(olStart( NULL, OL_SIG_PING ) != 0)
    {
        goto ping_exit;
    }
    ping_p->flag.from = 1;
    ping_evt.ping_p = ping_p;
    olDataCmdSend(NULL, EVT_SYS_OL_PING, pingExec, OL_SIG_PING, &ping_evt, sizeof(pingEvt_t));
    /* CTLC is the delimeter */
    ControlStdio(0,0,CTLC,pingKeyProc); 
    retvalue = 1;
    pause(1000);
    ResetStdio();
    olStop(NULL, OL_SIG_PING);
    pingCmdCb.option = 0;

	ping_exit:
    close(ip_sock);
    pause(1000); // waiting PingReply Completed
    printf("Ping Reply Count: %d\r\n", PingReplyCnt);

	ping_exit1:
    free(ping_p);
	//  ping_test_flag = 1;
	//  if( retvalue )
	//      printf("\r\nping failed!!");
	//  return retvalue;
	#else
	setUserSpaceFlag(4);
	return 0;
	#endif
#endif
	return 0;
}
#endif

#if debug_flag
static int doPingEchoCnt(int argc, char *argv[], void *p)
{
    if(!strcmp(argv[1], "clear"))
        PingReplyCnt = 0;
    printf("Ping Reply Count: %ld\r\n", PingReplyCnt);
    return 0;
}
#endif
#endif

// called by macPeriodCheck() in tc3162l2mac.c
void tcPhyChkVal(void)
{ 	// for 7512 (linux)
	uint32 port_num;

	if((tcPhyVer == tcPhyVer_7523Ge))
	{
		ephy_addr_base = EPHY_ADDR_P0;
	}
	port_num = phychkval_portnum - ephy_addr_base;  		// 0~4

   	if(phychkval_flag)
	{
	    uint32 	val;
		uint32 	phyAddr = phychkval_portnum;
        uint32 	mr02,mr03;
        uint8 	mr_anen, mr_dplx, mr_speed;
        uint8 	mr_an_capable;
        uint8 	mr_lp_an_capable;
        uint8 	mr_lp_anen;
		uint8 	mr01_link;	
        uint32 	reg3, r15_temp;
        uint8 	lr_linkup, lr_speed, lr_dplx, lr_mdix;
		// show phy_id
	
		tcMiiStationWrite(phyAddr, 31, 0);   				// g0r31 = 0x0
		val = tcMiiStationRead(phyAddr, 2);
		mr02 = val&(0xffff);
		val = tcMiiStationRead(phyAddr, 3); 
		mr03 = val&(0xffff);
		reg3 = tcMiiStationRead(phyAddr, 3); 
		printf(" tcPhy ID: %lx %lx\r\n", mr02, mr03); 
	
		val = tcMiiStationRead(phyAddr, 0);
		mr_anen = (val>>12)&0x01;
		mr_dplx = (val>>8)&0x01;
		mr_speed = ((val>>13)|(val>>5))&0x03;				// 00:10,01:100,10:1000
		
		// reg4h
		val = tcMiiStationRead(phyAddr, 0x04);
		mr_an_capable = (val>>5)&0x0F; 						// 100F,100H,10F,10H

		// reg9h
		val = tcMiiStationRead(phyAddr, 0x09);
		mr_an_capable = mr_an_capable | ((val>>4)&0x30);	// 1000F,1000H

		// reg5h
		val = tcMiiStationRead(phyAddr, 0x05);
		mr_lp_an_capable = (val>>5)&0x0F; 					// 100F,100H,10F,10H
		
		// regAh
		val = tcMiiStationRead(phyAddr, 0x0A);
		mr_lp_an_capable = mr_lp_an_capable | ((val>>6)&0x30); 		// 1000F,1000H

		// reg6h
		val = tcMiiStationRead(phyAddr, 0x06);
		mr_lp_anen = (val)&0x01;
   
		// reg1h 											// allen_20130926 from biker
		val = tcMiiStationRead(phyAddr, 0x01);
		mr01_link = (val>>2)&0x01;
		//printf(" [1] reg1 = 0x%x, mr01_link = %d \n", val, mr01_link);
		
		val = tcMiiStationRead(phyAddr, 0x01);
		mr01_link = (val>>2)&0x01;
		//printf(" [2] reg1 = 0x%x, mr01_link = %d \n", val, mr01_link);
	
		val = tcMiiStationRead(phyAddr, 0x01);
		mr01_link = (val>>2)&0x01;
		printf(" [3] reg1 = 0x%x, mr01_link = %d, tcPhyVer = 0x%x \r\n", val, mr01_link, tcPhyVer);
    	//printf(" [3] phychkval_portnum = %d, phyAddr = %d, port_num = %d \r\n", phychkval_portnum, phyAddr, port_num);

		r15_temp = tcMiiStationRead(phyAddr, 0xf);
		if((reg3 == EPHY_ID_7523Ge))
		{									
			val = mtPhyReadGReg(phyAddr,0x1E,0xA2);
    
    		//printf(" tcPhyChkVal= 0x%04lx",val);
  
			lr_linkup = (val>>9)&0x07;		//dev1e_A2[9]:LinkStatus10_OK,dev1e_A2[10]:LinkStatus100_OK,dev1e_A2[11]:LinkStatus1000_OK
			lr_speed = (val>>1)&0x07; 		//dev1e_A2[1]:final_speed_10,,dev1e_A2[2]:dev1e_A2[2]:final_speed_100,dev1e_A2[3]:final_speed_1000
			lr_dplx = (val>>5)&0x01;
			lr_mdix = (val>>6)&0x01;
		}    
		else
		{ 
			// l0reg28
			val = tcPhyReadLReg(port_num,0, 28);
			//printf(" tcPhyChkVal= 0x%04lx",val);
			
			lr_linkup = (val)&0x01;
			lr_speed = (val>>1)&0x01;
			lr_dplx = (val>>2)&0x01;
			lr_mdix = (val>>5)&0x01;
		}
			
		printf(" TcPhy mode:");	
		if(mr_anen)
		{ 	// Auto-neg
	    	printf(" AN-(");
	    	printf(" %s,",((mr_an_capable>>5)&0x01)?"1000F":"");
	    	printf(" %s,",((mr_an_capable>>4)&0x01)?"1000H":"");
	    	printf(" %s,",((mr_an_capable>>3)&0x01)?"100F":"");
	    	printf(" %s,",((mr_an_capable>>2)&0x01)?"100H":"");     
	    	printf(" %s,",((mr_an_capable>>1)&0x01)?"10F":"");      
	    	printf(" %s)\r\n",((mr_an_capable>>0)&0x01)?"10H":"");  
		}
		else if((reg3 == EPHY_ID_7523Ge))
		{ 	// Force-speed
			if(mr_speed==0x00)
	 		{
		 		printf(" Force-%d%s\r\n",10,(mr_dplx?"F":"H"));
			}
			else if(mr_speed==0x01)
			{
				printf(" Force-%d%s\r\n",100,(mr_dplx?"F":"H"));
	 		}
			else if(mr_speed==0x02)
			{
				printf(" Force-%d%s\r\n",1000,(mr_dplx?"F":"H"));
	 		}
    	}
	    else 
	 	{
	 		printf(" Force-%d%s\r\n",(mr_speed?100:10),(mr_dplx?"F":"H")); 	
        }
		 
		if(mr01_link == 0)  // link down
		{ 	// allen_20130926
			printf(" *** Link is down!\r\n");

            // dev1E_RA2 message
            if(mr1E_A2.lch_SignalDetect || mr1E_A2.lch_LinkPulse
                || mr1E_A2.final_speed_1000 || mr1E_A2.final_speed_100|| mr1E_A2.final_speed_10)
            {
				TCPHYDISP4(" tcphy[%ld]: ",phyAddr);

				if(mr1E_A2.lch_SignalDetect) 
					TCPHYDISP4("SigDet ");
				if(mr1E_A2.lch_LinkPulse)
					TCPHYDISP4("RxLkp ");
				if(mr1E_A2.final_speed_1000)
				{					
					if(mr1E_A2.MSConfig1000)
					{
						TCPHYDISP4("Up1000 Master ");
					}
					else
					{
						TCPHYDISP4("Up1000 Slave ");
					}
				}

				if(mr1E_A2.final_speed_100)
					TCPHYDISP4("Up100 ");
				if(mr1E_A2.final_speed_10)
					TCPHYDISP4("Up10 ");
				if(mr1E_A2.da_mdix)
				{
					TCPHYDISP4("UpMdix ");
				}
				else
				{
					TCPHYDISP4("UpMdi ");
				}
				TCPHYDISP4("\r\n");
            }

        }
		else 
		{ 	// link-up
			if((reg3==EPHY_ID_7523Ge)) 
			{
				if(lr_speed == 0x04)
				{
					printf(" TCphy is link-up at 1000 %s ",(mr1E_A2.MSConfig1000?"Master":"Slave"));
				}
				else if(lr_speed == 0x02)
				{
					printf(" TCphy is link-up at 100 %s ",(lr_dplx?"F":"H"));
				}
				else if(lr_speed == 0x01)
				{	
					printf(" TCphy is link-up at 10 %s ",(lr_dplx?"F":"H"));
				}
	      	}
			else
			{
				printf(" TCphy is link-up at %d%s ",(lr_speed?100:10),(lr_dplx?"F":"H"));	
			}
           
	   		if(mr_lp_anen)
			{
                printf(", Link-partner supports AN-(");
                printf(" %s,",((mr_lp_an_capable>>5)&0x01)?"1000F":"");
                printf(" %s,",((mr_lp_an_capable>>4)&0x01)?"1000H":"");
                printf(" %s,",((mr_lp_an_capable>>3)&0x01)?"100F":"");
                printf(" %s,",((mr_lp_an_capable>>2)&0x01)?"100H":"");  
                printf(" %s,",((mr_lp_an_capable>>1)&0x01)?"10F":"");       
                printf(" %s),  ",((mr_lp_an_capable>>0)&0x01)?"10H":"");   
            }
	    	else 
			{
                printf(" Link-partner operates in Force mode.\r\n");
            }

            printf(" %s,",(lr_mdix?"mdix":"mdi")); 
            
			if(lr_speed >= 0x02)
			{	// 100Mbps/1000Mbps

				if((reg3 == EPHY_ID_7523Ge))
				{
					#if debug_flag
					val=en75xxGePhyReadProbe(phyAddr, Probe_VgaState, 0);
					printf("7523 VgaStateA, B, C, D = 0x%x, 0x%x, 0x%x, 0x%x\r\n",((val>>4) & 0x1F),((val>>9) & 0x1F),((val>>14) & 0x1F),((val>>19) & 0x1F));
				
					val=en75xxGePhyReadProbe(phyAddr,Probe_MSE,0);
					printf("7523 lch_mse_mdcA =0x%x\r\n",val);
					val=en75xxGePhyReadProbe(phyAddr,Probe_MSE,1);
					printf("7523 lch_mse_mdcB =0x%x\r\n",val);
					val=en75xxGePhyReadProbe(phyAddr,Probe_MSE,2);
					printf("7523 lch_mse_mdcC =0x%x\r\n",val);
					val=en75xxGePhyReadProbe(phyAddr,Probe_MSE,3);
					printf("7523 lch_mse_mdcD =0x%x\r\n",val);
			
					val=en75xxGePhyReadProbe(phyAddr,Probe_MSE_Slicer_err_thres,0);
					printf("slicer_err_thres =0x%x\r\n",val);
					val=en75xxGePhyReadProbe(phyAddr,Probe_Slicer_Err_Over_Sum,0);
					printf("ProbeSlicerErrOverSumA =0x%x\r\n",val);
					val=en75xxGePhyReadProbe(phyAddr,Probe_Slicer_Err_Over_Sum,1);
					printf("ProbeSlicerErrOverSumB =0x%x\r\n",val);
					val=en75xxGePhyReadProbe(phyAddr,Probe_Slicer_Err_Over_Sum,2);
					printf("ProbeSlicerErrOverSumC =0x%x\r\n",val);
					val=en75xxGePhyReadProbe(phyAddr,Probe_Slicer_Err_Over_Sum,3);
					printf("ProbeSlicerErrOverSumD =0x%x\r\n",val);
					#endif
				}
			}
		}   
		phychkval_flag = 0;
	}
}

#if debug_flag
static int doPhyForceLink(int argc, char *argv[], void *p)
{
    if(!tcPhyFlag)
        return 0;

    if(argc != 2) 
    {
        printf("forcelink status: %s.\r\n", (force_link_flag?"on":"off"));
    }
    else if(stricmp(argv[1], "on") == 0)  
    {
        force_link_flag = 1;        
        printf("Force link status: ON.\r\n");
    }
    else if(stricmp(argv[1], "off") == 0) 
    {
        force_link_flag = 0;        
        printf("Force link status: OFF.\r\n");
    }           
    else 
    {
        printf("Usage: forcelink status [on|off] \r\n");
    }
    return 0;
}
#endif

void EphyMonitor(void)
{
    if(inital_finish_flag == 1)
	{
    /* add your monitor code here*/
    tcPhyChkVal();
    tcPhySwPatch();
    //printf(" ephy monitor \r\n");
	}
}

#if debug_flag
// set flag for tcPhyErrMonitor
static int doErrMonitor(int argc, char *argv[], void *p)
{
    if(!tcPhyFlag)
        return 0;

    if(argc != 2) 
    {
        printf("Error monitor status: %s!\r\n", (err_monitor_flag?"on":"off"));
    }
    else if(stricmp(argv[1], "on") == 0)  
    {
        err_monitor_flag = 1;       
        printf("Error monitor ON!\r\n");
    }
    else if(stricmp(argv[1], "off") == 0) 
    {
        err_monitor_flag = 0;       
        printf("Error monitor OFF!\r\n");
    }           
    else 
    {
        printf("Usage: errmonitor [on|off] \r\n");
    }
    return 0;
}
#endif

#endif // TCPHY_DEBUG

/************************************************************************
*            debug functions body for EPHY (TCPHY_TESTDBG CI commands)
**************************************************************************/
#ifdef TCPHY_FIELD_DBGTEST
static int doFieldDebugTest(int argc, char *argv[], void *p)
{
    if(!tcPhyFlag) 
    {
        printf("NOT TC PHY!!\r\n");
        return 0;
    }
    if(argc==2 || argc==3) 
    {	// yhchen: need to re-write to support both 2031 & 2101mb !!
        if(tcPhyVer == tcPhyVer_2031) 
        {
            #ifdef TC2031_DBGTEST
            doPhyDebugTest(argc, argv, p);
            #endif
        }
        else 
        {
            printf("Not support!!\r\n");
        }
        return 0;
    }
    printf("Usage: dbgtest <1|2|3> [wait_time]\r\n");
    return 0;
}

static int doFieldDebugPrint(int argc, char *argv[], void *p)
{
    uint32 	i = 0;
    for(i=0; i<msgcnt; i++) 
    {
        printf("%3d: %s \n", i, tc_ether_phy[i]);
    }
    return 0;
}
#endif // TCPHY_FIELD_DBGTEST
#ifdef TCSUPPORT_DYNAMIC_VLAN
extern dyVLAN_PORT_t dyVLAN_PORT[];
void tcPortLinkChk(void)
{
	int port;
	for(port = 0; port < TCPHY_PORTNUM; port++)
	{
		if(tcphy_link_state[port] == ST_LINK_DOWN2UP)
		{
			printk("Port %d link up\n", port);
		}
		else if(tcphy_link_state[port] == ST_LINK_UP2DOWN)
		{
			printk("Port %d link down\n", port);
			dyVLAN_PORT[port].state = 0;
		}
	}
}
#endif

#ifdef LINUX_OS 		// allen_20130926
int doDbgPorbeSet(int argc, char *argv[], void *p)
{
	uint16 PortSel=0;
	uint32 value=0;
	uint32 DbgflgSel=0;
	uint32 DbgClk=0;
	#define dbg_flg_en 1 
	#define dbg_DevAddr 0x1F
 	#define dbg_Flg0Ctrl 0x15
 	#define dbg_Flg1Ctrl 0x16
 	#define dbg_ClkAddr 0x19
	
   	if(argc==4)
	{
		PortSel = checked_atoi(argv[2]);
		sscanf(argv[3], "%lx", &DbgflgSel);
		//DbgClk = checked_atoi(argv[3]);
		printf("* dbg_port_Sel=%d, dbg_flag_sel=0x%04lX\r\n", PortSel, DbgflgSel);		
	}
   	else if(argc==5)
   	{
		PortSel = checked_atoi(argv[2]);
		sscanf(argv[3], "%lx", &DbgflgSel);
		//DbgClk = checked_atoi(argv[3]);
		sscanf(argv[4], "%lx", &DbgClk);
		printf("* dbg_port_Sel=%d, dbg_flag_sel=0x%04lX, dbg_clk_sel=0x%02lX\r\n", PortSel, DbgflgSel, DbgClk);		
	}

	if(((argc == 4) ||(argc == 5)) && (PortSel <= 31))
	{
		if(argc == 4)
		{
			if(stricmp(argv[1], "all") == 0)
			{
	           value = (PortSel<<13)|(dbg_flg_en<<12)|(DbgflgSel);
			   printf("Dbg Flag Register Set Value=%lx\r\n", value);
			   mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_Flg0Ctrl, value);
			   mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_Flg1Ctrl, value);
			}
			else if(stricmp(argv[1], "0") == 0)
			{
				value = (PortSel<<13)|(dbg_flg_en<<12)|(DbgflgSel);
				printf("Dbg Flag Register Set Value=%lx\r\n", value);
				mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_Flg0Ctrl, value);
			}
			else if(stricmp(argv[1], "1") == 0)
			{
				value = (PortSel<<13)|(dbg_flg_en<<12)|(DbgflgSel);
				printf("Dbg Flag Register Set Value=%lx\r\n", value);
				mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_Flg1Ctrl, value);
			}
			DbgClk = mtEMiiRegRead(PortSel, dbg_DevAddr, dbg_ClkAddr);
			value = DbgClk & 0xEF;
			mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_ClkAddr, value);
		}
		else if((argc == 5) && (stricmp(argv[1], "1") == 0))
		{
	    	value = (PortSel<<13)|(dbg_flg_en<<12)|(DbgflgSel);
			mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_Flg1Ctrl, value);
			value = (dbg_flg_en<<12)|(DbgClk);
			mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_ClkAddr, value);    
	 	   	printf("Dbg Flag Register Set Value=%lx\r\n", value);
		}
		else if((argc == 5) && (stricmp(argv[1], "all") == 0))
		{
	    	value = (PortSel<<13)|(dbg_flg_en<<12)|(DbgflgSel);
	    	printf("Dbg Flag Register Set Value=%lx\r\n", value);
    		mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_Flg0Ctrl, value);
			mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_Flg1Ctrl, value);
			value = (dbg_flg_en<<12)|(DbgClk);
			mtEMiiRegWrite(PortSel, dbg_DevAddr, dbg_ClkAddr, value);
		}
	}
	else 
	{ 	// error message
		printf("Usage: dbgprb 1 <PortNo> <DbgflgSel> <Clk>\r\n");
		printf("       dbgprb 1 <PortNo> <DbgflgSel> \r\n");
		printf("       dbgprb 0 <PortNo> <DbgflgSel> \r\n");
		printf("       dbgprb all <PortNo> <DbgflgSel> <Clk>\r\n");
		return 0;
	}
	return 0;
}

int doPhyMiiRead_TrDbg(int argc, char *argv[], void *p)
{
    const uint16 page_reg=31;
    const uint32 Token_Ring_debug_reg=0x52B5;
    const uint32 Token_Ring_Control_reg=0x10;
    const uint32 Token_Ring_Low_data_reg=0x11;
    const uint32 Token_Ring_High_data_reg=0x12;
    uint16 phyaddr=0;
    uint16 ch_addr=0;
    uint32 node_addr=0;
    uint32 data_addr=0;
    uint32 value=0;
    uint32 value_high=0;
    uint32 value_low=0;
	int i=0;
	uint32 start_addr=0;
	uint32 end_addr=0;

    if(argc ==4) 
    {	// tce miir_trdgb <PhyAddr> <TrRegType> <DataAddr>
		phyaddr = checked_atoi(argv[1]);		// 0~31
		sscanf(argv[3], "%lx", &data_addr);		// 6 bit HEX
        if(strcasecmp(argv[2], "DSPF") == 0)
        {	// DSP Filter Debug Node
            ch_addr = 0x02;
			node_addr = 0x0D;
	    }
		else if(strcasecmp(argv[2], "PMA") == 0)
		{ 	// PMA Debug Node 
			ch_addr = 0x01;
			node_addr = 0x0F;
		}
		else if(strcasecmp(argv[2], "TR") == 0)
		{ 	// Timing Recovery  Debug Node 
			ch_addr = 0x01;
			node_addr = 0x0D;
		}
		else if(strcasecmp(argv[2], "PCS") == 0)
		{ 	// R1000PCS Debug Node 
			ch_addr = 0x02;
			node_addr = 0x0F;
		}
		else if(strcasecmp(argv[2], "FFE_A") == 0)
		{ 	// FFE Debug Node 
			ch_addr = 0x00;
			node_addr = 0x04;
		}
		else if(strcasecmp(argv[2], "FFE_B") == 0)
		{ 	// FFE Debug Node 
			ch_addr = 0x01;
			node_addr = 0x04;
		}
		else if(strcasecmp(argv[2], "FFE_C") == 0)
		{ 	// FFE Debug Node 
			ch_addr = 0x02;
			node_addr = 0x04;
		}
		else if(strcasecmp(argv[2], "FFE_D") == 0)
		{ 	// FFE Debug Node 
			ch_addr = 0x03;
			node_addr = 0x04;
		}
		else if(strcasecmp(argv[2], "FFE_INDEP") == 0)
		{ 	// FFE Debug Node 
			ch_addr = 0x00;
			node_addr = 0x04;
		}
 		else if(strcasecmp(argv[2], "ECNC_A") == 0)
 		{ 	// EC/NC Debug Node 
			ch_addr = 0x00;
			node_addr = (data_addr&0xC0)>>6; 	// V1.17 for ECNC_0xE0 is per port
			#if debug_flag
			printf("* ECNC_A => Phyaddr=%d,TrRegTyp =%s,node_addr=0x%02lX,data_addr=0x%04lX ,ch_addr=0x%02lX\r\n",phyaddr, argv[2],node_addr,data_addr, ch_addr);
			#endif
		}
 		else if(strcasecmp(argv[2], "ECNC_B") == 0)
 		{ 	// EC/NC Debug Node 
			ch_addr = 0x01;
			node_addr = (data_addr&0xC0)>>6; 	// V1.17 for ECNC_0xE0 is per port
			#if debug_flag
			printf("* ECNC_B => Phyaddr=%d,TrRegTyp =%s,node_addr=0x%02lX,data_addr=0x%04lX ,ch_addr=0x%02lX\r\n",phyaddr, argv[2],node_addr,data_addr, ch_addr);
			#endif	
		}
 		else if(strcasecmp(argv[2], "ECNC_C") == 0)
 		{ 	// EC/NC Debug Node 
			ch_addr = 0x02;
			node_addr = (data_addr&0xC0)>>6; 	// V1.17 for ECNC_0xE0 is per port
			#if debug_flag
			printf("* ECNC_C => Phyaddr=%d,TrRegTyp =%s,node_addr=0x%02lX,data_addr=0x%04lX ,ch_addr=0x%02lX\r\n",phyaddr, argv[2],node_addr,data_addr, ch_addr);
			#endif
		}
 		else if(strcasecmp(argv[2], "ECNC_D") == 0)
 		{ 	// EC/NC Debug Node 
			ch_addr = 0x03;
			node_addr = (data_addr&0xC0)>>6; 	// V1.17 for ECNC_0xE0 is per port
			#if debug_flag
			printf("* ECNC_D => Phyaddr=%d,TrRegTyp =%s,node_addr=0x%02lX,data_addr=0x%04lX ,ch_addr=0x%02lX\r\n",phyaddr, argv[2],node_addr,data_addr, ch_addr);
			#endif
		}
 		else if(strcasecmp(argv[2], "ECNC_INDEP") == 0)
 		{ 	// EC/NC Debug Node 
			ch_addr=0x00;
			node_addr=(data_addr&0xC0)>>6;
			#if debug_flag
			printf("* ECNC_INDEP => Phyaddr=%d,TrRegTyp =%s,node_addr=0x%02lX,data_addr=0x%04lX ,ch_addr=0x%02lX\r\n",phyaddr, argv[2],node_addr,data_addr, ch_addr);
			#endif
		}
		else if(strcasecmp(argv[2], "DFEDC_A") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x00;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DFEDC_B") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x01;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DFEDC_C") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x02;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DFEDC_D") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x03;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DFEDC_INDEP") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x00;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DEC") == 0)
		{ 	// R1000DEC Debug Node 
			ch_addr=0x00; 
			node_addr=0x07;
		}
		else if(strcasecmp(argv[2], "CRC_A") == 0)
		{ 	// R1000CRC Debug Node 
			ch_addr=0x00;
			node_addr=0x06;
		}
		else if(strcasecmp(argv[2], "CRC_B") == 0)
		{ 	// R1000CRC Debug Node 
			ch_addr=0x01;
			node_addr=0x06;
		}
		else if(strcasecmp(argv[2], "CRC_C") == 0)
		{ 	// R1000CRC Debug Node 
			ch_addr=0x02;
			node_addr=0x06;
		}
		else if(strcasecmp(argv[2], "CRC_D") == 0)
		{ 	// R1000CRC Debug Node 
			ch_addr=0x03;
			node_addr=0x06;
		}
		else if(strcasecmp(argv[2], "AN") == 0)
		{ 	// Autoneg Debug Node 
			ch_addr=0x00; 
			node_addr=0x0F;
		}
		else if(strcasecmp(argv[2], "CMI") == 0)
		{ 	// CMI Debug Node 
			ch_addr=0x03; 
			node_addr=0x0F;
		}
		else if(strcasecmp(argv[2], "SUPV") == 0)
		{ 	// SUPV PHY  Debug Node 
			ch_addr=0x00; 
			node_addr=0x0D;
		}
		else
		{
			printf("	miir_trdgb <PhyAddr> <TrRegTyp>(DSPF|PMA|TR|PCS|FFE_x|ECNC_x|DFEDC_x|DEC|CRC_x|AN|CMI|SUPV) <DataAddr>\r\n");
			return 0;
		}
		data_addr = data_addr&0x3F;
		tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
		tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(ch_addr<<11)|(node_addr<<7)|(data_addr<<1));
		//while(!(tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)); 			// data ready
		{
			value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);
			value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);
			value=value_low+((value_high&0x00FF)<<16);
			#if debug_flag
			printf("* %s => Phyaddr=%d,TrRegTyp =%s,node_addr=0x%02lX,data_addr=0x%04lX ,value=0x%06lX\r\n", argv[2], phyaddr, argv[2],node_addr,data_addr, value);
			#endif
		}
		tcMiiStationWrite(phyaddr, page_reg, 0x00);			// V1.11
		return 0;		
	}
	else if (argc==5)
	{	// tce miir_trdgb <PhyAddr> <CHAddr> <NodeAddr> <DataAddr>
        phyaddr = checked_atoi(argv[1]);		// 0~31
   	    ch_addr = checked_atoi(argv[2]);		// 0:A pair  /1:B pair  /2:C pair  /3:D pair 
		sscanf(argv[3], "%lx", &node_addr);		// 4 bit HEX
		sscanf(argv[4], "%lx", &data_addr);		// 6 bit HEX
    
		tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
		tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(ch_addr<<11)|(node_addr<<7)|(data_addr<<1));
		//while(!(tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)); 			// data ready
		{
			value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);		
			value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);			
			value = value_low + ((value_high&0x00FF)<<16);
			#if debug_flag
			printf("* miir_trdgb => Phyaddr=%d, ch_addr=%d,node_addr=0x%02lX,data_addr=0x%04lX ,value=0x%06lX\r\n", phyaddr, ch_addr, node_addr, data_addr, value);
			#endif
		}
		tcMiiStationWrite(phyaddr, page_reg, 0x00);		// V1.11
		return 0;
    }
	else if(argc ==6) 
	{	//miir_trdgb all <PhyAddr> <TrRegTyp> <s_addr> <e_addr>
		phyaddr = checked_atoi(argv[2]);		// 0~31
		sscanf(argv[4], "%lx", &start_addr);	// hex
		sscanf(argv[5], "%lx", &end_addr);		// hex
		for(i=start_addr; i<=end_addr; i++)
		{
		    data_addr=i;
	        if(stricmp(argv[3], "DSPF") == 0)
	        {	// DSP Filter Debug Node
        	    ch_addr=0x02;
				node_addr=0x0D;
		    }
			else if(stricmp(argv[3], "PMA") == 0)
			{ 	// PMA Debug Node 
					ch_addr=0x01;
					node_addr=0x0F;
			}
			else if(stricmp(argv[3], "TR") == 0)
			{ 	// Timing Recovery  Debug Node 
				ch_addr=0x01;
				node_addr=0x0D;
			}
			else if(stricmp(argv[3], "PCS") == 0)
			{ 	// R1000PCS Debug Node 
				ch_addr=0x02;
				node_addr=0x0F;
			}
			else if(stricmp(argv[3], "FFE_A") == 0)
			{ 	// FFE Debug Node 
				ch_addr=0x00;
				node_addr=0x04;
			}
			else if(stricmp(argv[3], "FFE_B") == 0)
			{ 	// FFE Debug Node 
				ch_addr=0x01;
				node_addr=0x04;
			}
			else if(stricmp(argv[3], "FFE_C") == 0)
			{ 	// FFE Debug Node 
				ch_addr=0x02;
				node_addr=0x04;
			}
			else if(stricmp(argv[3], "FFE_D") == 0)
			{ 	// FFE Debug Node 
				ch_addr=0x03;
				node_addr=0x04;
			}
			else if(stricmp(argv[3], "FFE_INDEP") == 0)
			{ 	// FFE Debug Node 
				ch_addr=0x00;
				node_addr=0x04;
			}
 			else if(stricmp(argv[3], "ECNC_A") == 0)
 			{ 	// EC/NC Debug Node 
				ch_addr=0x00;
				node_addr=(data_addr&0x40)>>6;
			}
 			else if(stricmp(argv[3], "ECNC_B") == 0)
 			{ 	// EC/NC Debug Node 
				ch_addr=0x01;
				node_addr=(data_addr&0x40)>>6;
			}
 			else if(stricmp(argv[3], "ECNC_C") == 0)
 			{ 	// EC/NC Debug Node 
				ch_addr=0x02;
				node_addr=(data_addr&0x40)>>6;
			}
 			else if(stricmp(argv[3], "ECNC_D") == 0)
 			{ 	// EC/NC Debug Node 
				ch_addr=0x03;
				node_addr=(data_addr&0x40)>>6;
			}
 			else if(stricmp(argv[3], "ECNC_INDEP") == 0)
 			{ 	// EC/NC Debug Node 
				ch_addr=0x00;
				node_addr=(data_addr&0xC0)>>6;
			}
			else if(stricmp(argv[3], "DFEDC_A") == 0)
			{ 	// DFETail/DC Debug Node 
				ch_addr=0x00;
				node_addr=0x05;
			}
			else if(stricmp(argv[3], "DFEDC_B") == 0)
			{ 	// DFETail/DC Debug Node 
				ch_addr=0x01;
				node_addr=0x05;
			}
			else if(stricmp(argv[3], "DFEDC_C") == 0)
			{ 	// DFETail/DC Debug Node 
				ch_addr=0x02;
				node_addr=0x05;
			}
			else if(stricmp(argv[3], "DFEDC_D") == 0)
			{ 	// DFETail/DC Debug Node 
				ch_addr=0x03;
				node_addr=0x05;
			}
			else if(stricmp(argv[3], "DFEDC_INDEP") == 0)
			{ 	// DFETail/DC Debug Node 
				ch_addr=0x00;
				node_addr=0x05;
			}
			else if(stricmp(argv[3], "DEC") == 0)
			{ 	// R1000DEC Debug Node 
				ch_addr=0x00; 
				node_addr=0x07;
			}
			else if(stricmp(argv[3], "CRC_A") == 0)
			{ 	// R1000CRC Debug Node 
				ch_addr=0x00;
				node_addr=0x06;
			}
			else if(stricmp(argv[3], "CRC_B") == 0)
			{ 	// R1000CRC Debug Node 
				ch_addr=0x01;
				node_addr=0x06;
			}
			else if(stricmp(argv[3], "CRC_C") == 0)
			{ 	// R1000CRC Debug Node 
				ch_addr=0x02;
				node_addr=0x06;
			}
			else if(stricmp(argv[3], "CRC_D") == 0)
			{ 	// R1000CRC Debug Node 
				ch_addr=0x03;
				node_addr=0x06;
			}
			else if(stricmp(argv[3], "AN") == 0)
			{ 	// Autoneg Debug Node 
				ch_addr=0x00; 
				node_addr=0x0F;
			}
			else if(stricmp(argv[3], "CMI") == 0)
			{ 	// CMI Debug Node 
				ch_addr=0x03; 
				node_addr=0x0F;
			}
			else if(stricmp(argv[3], "SUPV") == 0)
			{ 	// SUPV PHY  Debug Node 
				ch_addr=0x00; 
				node_addr=0x0D;
			}
			else 
			{
				printf("	miir_trdgb all <PhyAddr> <TrRegTyp>(DSPF|PMA|TR|PCS|FFE_x|ECNC_x|DFEDC_x|DEC|CRC_x|AN|CMI|SUPV) <s_addr> <e_addr>\r\n");
				return 0;
			}
			data_addr=data_addr&0x3F;
			tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
			tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(ch_addr<<11)|(node_addr<<7)|(data_addr<<1));
			//while(!(tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)); // data ready
			{
				value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);
				value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);
				value = value_low + ((value_high&0x00FF)<<16);
				#if debug_flag
				printf("* %s => Phyaddr=%d,TrRegTyp =%s,node_addr=0x%02lX,data_addr=0x%04lX,value=0x%06lX\r\n", argv[2], phyaddr, argv[2],node_addr, i, value);
				#endif
			}
		}
		tcMiiStationWrite(phyaddr, page_reg, 0x00);		// V1.11
		return 0;
    }
	else
	{
		#if debug_flag
        printf("       CMD Error : miir_trdgb <PhyAddr> <CHAddr> <NodeAddr> <DataAddr>\r\n");
        printf("                   miir_trdgb <PhyAddr> <TrRegTyp>(DSPF|PMA|TR|PCS|FFE_x|ECNC_x|DFEDC_x|DEC|CRC_x|AN|CMI|SUPV) <DataAddr>\r\n");
        printf("                   miir_trdgb all <PhyAddr> <TrRegTyp>(DSPF|PMA|TR|PCS|FFE_x|ECNC_x|DFEDC_x|DEC|CRC_x|AN|CMI|SUPV) <s_addr> <e_addr>\r\n");
        printf("                   <where x=A/B/C/D/INDEP>\r\n");
		#endif
        return 0;      
	}
}

int doPhyMiiWrite_TrDbg(int argc, char *argv[], void *p)
{
    const uint16 page_reg=31;
    const uint32 Token_Ring_debug_reg=0x52B5;
    const uint32 Token_Ring_Control_reg=0x10;
    const uint32 Token_Ring_Low_data_reg=0x11;
    const uint32 Token_Ring_High_data_reg=0x12;
    uint16 phyaddr=0;
    uint16 ch_addr=0;
    uint32 node_addr=0;
    uint32 data_addr=0;
    uint32 value=0;
    uint32 value_high=0;
    uint32 value_low=0;
	
	if(argc ==5)
	{	// tce miiw_trdgb <PhyAddr> <TrRegType> <DataAddr> <Value>	
		phyaddr = checked_atoi(argv[1]);		// 0~31
		sscanf(argv[3], "%lx", &data_addr);		// 6 bit HEX
		if(strcasecmp(argv[2], "DSPF") == 0)
		{	// DSP Filter Debug Node
			ch_addr=0x02;
		   	node_addr=0x0D;
		}
		else if(strcasecmp(argv[2], "PMA") == 0)
		{ 	// PMA Debug Node 
			ch_addr=0x01;
			node_addr=0x0F;
		}
		else if(strcasecmp(argv[2], "TR") == 0)
		{ 	// Timing Recovery  Debug Node 
			ch_addr=0x01;
			node_addr=0x0D;
		}
		else if(strcasecmp(argv[2], "PCS") == 0)
		{ 	// R1000PCS Debug Node 
			ch_addr=0x02;
			node_addr=0x0F;
		}
		else if(strcasecmp(argv[2], "FFE_A") == 0)
		{ 	// FFE Debug Node 
			ch_addr=0x00;
			node_addr=0x04;
		}
		else if(strcasecmp(argv[2], "FFE_B") == 0)
		{ 	// FFE Debug Node 
			ch_addr=0x01;
			node_addr=0x04;
		}
		else if(strcasecmp(argv[2], "FFE_C") == 0)
		{ 	// FFE Debug Node 
			ch_addr=0x02;
			node_addr=0x04;
		}
		else if(strcasecmp(argv[2], "FFE_D") == 0)
		{ 	// FFE Debug Node 
			ch_addr=0x03;
			node_addr=0x04;
		}
		else if(strcasecmp(argv[2], "FFE_INDEP") == 0)
		{ 	// FFE Debug Node 
			ch_addr=0x00;
			node_addr=0x04;
		}
		else if(strcasecmp(argv[2], "ECNC_A") == 0)
		{ 	// EC/NC Debug Node 
			ch_addr=0x00;
			node_addr=(data_addr&0x40)>>6;
		}
		else if(strcasecmp(argv[2], "ECNC_B") == 0)
		{ 	// EC/NC Debug Node 
			ch_addr=0x01;
			node_addr=(data_addr&0x40)>>6;
		}
		else if(strcasecmp(argv[2], "ECNC_C") == 0)
		{ 	// EC/NC Debug Node 
			ch_addr=0x02;
			node_addr=(data_addr&0x40)>>6;
		}
		else if(strcasecmp(argv[2], "ECNC_D") == 0)
		{ 	// EC/NC Debug Node 
			ch_addr=0x03;
			node_addr=(data_addr&0x40)>>6;
		}
		else if(strcasecmp(argv[2], "ECNC_INDEP") == 0)
		{ 	// EC/NC Debug Node 
			ch_addr=0x00;
			node_addr=(data_addr&0xC0)>>6;
		}
		else if(strcasecmp(argv[2], "DFEDC_A") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x00;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DFEDC_B") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x01;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DFEDC_C") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x02;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DFEDC_D") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x03;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DFEDC_INDEP") == 0)
		{ 	// DFETail/DC Debug Node 
			ch_addr=0x00;
			node_addr=0x05;
		}
		else if(strcasecmp(argv[2], "DEC") == 0)
		{ 	// R1000DEC Debug Node 
			ch_addr=0x00; 
			node_addr=0x07;
		}
		else if(strcasecmp(argv[2], "CRC_A") == 0)
		{ 	// R1000CRC Debug Node 
			ch_addr=0x00;
			node_addr=0x06;
		}
		else if(strcasecmp(argv[2], "CRC_B") == 0)
		{ 	// R1000CRC Debug Node 
			ch_addr=0x01;
			node_addr=0x06;
		}
		else if(strcasecmp(argv[2], "CRC_C") == 0)
		{ 	// R1000CRC Debug Node 
			ch_addr=0x02;
			node_addr=0x06;
		}
		else if(strcasecmp(argv[2], "CRC_D") == 0)
		{ 	// R1000CRC Debug Node 
			ch_addr=0x03;
			node_addr=0x06;
		}
		else if(strcasecmp(argv[2], "AN") == 0)
		{ 	// Autoneg Debug Node 
			ch_addr=0x00; 
			node_addr=0x0F;
		}
		else if(strcasecmp(argv[2], "CMI") == 0)
		{ 	// CMI Debug Node 
			ch_addr=0x03; 
			node_addr=0x0F;
		}
		else if(strcasecmp(argv[2], "SUPV") == 0)
		{ 	// SUPV PHY  Debug Node 
			ch_addr=0x00; 
			node_addr=0x0D;
		}
		else 
		{
			printf("	miiw_trdgb <PhyAddr> <TrRegTyp>(DSPF|PMA|TR|PCS|FFE_x|ECNC_x|DFEDC_x|DEC|CRC_x|AN|CMI|SUPV) <DataAddr> <Value>\r\n");
			return 0;
		}
        data_addr=data_addr&0x3F;
		sscanf(argv[4], "%lx", &value);			// 24 bit //HEX
		value_high=(0x00FF0000&value)>>16;
		value_low=(0x0000FFFF&value);

		tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);	   
		tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
		tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
		tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(ch_addr<<11)|(node_addr<<7)|(data_addr<<1));
		//while(!(tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)); // data ready
	   	{
			printf("* %s => Phyaddr=%d, ch_addr=%d, node_addr=0x%02lX, data_addr=0x%04lX , value=0x%06lX\r\n",argv[2], phyaddr, ch_addr, node_addr, data_addr, value);
		}
		tcMiiStationWrite(phyaddr, page_reg, 0x00);		// V1.11
		return 0;
	}
	else if (argc==6)
	{
    	// tce miiw_trdgb <PhyAddr> <CHAddr> <NodeAddr> <DataAddr> <Value>
    	phyaddr = checked_atoi(argv[1]);		// 0~31
    	ch_addr = checked_atoi(argv[2]);		// 0:A pair  /1:B pair  /2:C pair  /3:D pair 
    	sscanf(argv[3], "%lx", &node_addr);		// 4 bit //HEX
    	sscanf(argv[4], "%lx", &data_addr);		// 6 bit //HEX
    	sscanf(argv[5], "%lx", &value);			// 24 bit //HEX
        value_high=(0x00FF0000&value)>>16;
        value_low=(0x0000FFFF&value);
        tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);	
        tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
        tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
        tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(ch_addr<<11)|(node_addr<<7)|(data_addr<<1));
        //while(!(tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)); // data ready
		{
			printf("* miiw_trdgb ready => Phyaddr=%d, ch_addr=%d, node_addr=0x%02lX, data_addr=0x%04lX , value=0x%06lX\r\n", phyaddr, ch_addr, node_addr, data_addr, value);
		}
		tcMiiStationWrite(phyaddr, page_reg, 0x00);		// V1.11
		return 0;
	}
	else
	{
        printf("       CMD Error : miiw_trdgb <PhyAddr> <CHAddr> <NodeAddr> <DataAddr> <Value>\r\n");
        printf("                   miiw_trdgb <PhyAddr> <TrRegTyp>(DSPF|PMA|TR|PCS|FFE_x|ECNC_x|DFEDC_x|DEC|CRC_x|AN|CMI|SUPV) <DataAddr> <Value>\r\n");
        printf("                   <where x=A/B/C/D/INDEP>\r\n");
        return 0;      
	}
}
#if debug_flag
//uint32 mtPhyMiiRead_TrDbg(uint8 phyaddr, char *type, uint32 reg, uint8 ch_num);
int doReadPhycoeff(int argc, char *argv[], void *p)		// coeffr <PhyAddr> <FFE/DEC/EC/NC/TR/DFE>
{ 
	uint16 	phyaddr=0;
 	uint32 	value;
 	uint8 	i, j;
 	uint8 	sw_patch_flag_bak;				// V1.11
 	char 	*ch_tbl[4]={"A","B","C","D"};

	sw_patch_flag_bak = sw_patch_flag;		// V1.11
  	if(argc==3)
  	{
    	sw_patch_flag = 0;
        mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 4, 0x60000, 0);		//V1.11  force freeze DSP
        phyaddr = checked_atoi(argv[1]);
        printf("* PhyAddr=%d\r\n", phyaddr);
        if((stricmp(argv[2], "ffe") == 0) ||(stricmp(argv[2], "FFE") == 0))
        {
        	for(j=0; j<4; j++)
        	{	//chA~chD
              	for(i=0; i<16; i++)
              	{
                	value = mtPhyMiiRead_TrDbg(phyaddr, "FFE", i,j);
                  	printf("* FFE_%02xh CH%s value=0x%04lX\r\n", i,ch_tbl[j], value);
                }
            }
        }
        else if((stricmp(argv[2], "dec") == 0) ||(stricmp(argv[2], "DEC") == 0))
        {    
        	for(j=0; j<4; j++)
        	{
            	for(i=0; i<3; i++)
            	{
                	value = mtPhyMiiRead_TrDbg(phyaddr, "DEC", (4*j+i),0); 
                    printf("* DEC_%02xh CH%s, value=0x%04lX\r\n",(4*j+i),ch_tbl[j],value);
                }
            }
        } 
        else if((stricmp(argv[2], "ec") == 0) ||(stricmp(argv[2], "EC") == 0))
        {
        	for(j=0; j<4; j++)
        	{
            	for(i=0; i<64; i++)
            	{
                	value = mtPhyMiiRead_TrDbg(phyaddr, "EC", i, j); 
                    printf("* EC[%02d], CH%s value=0x%04lX\r\n", i,ch_tbl[j],value);
                }
              	for(i=0; i<16; i++)
              	{
                	value = mtPhyMiiRead_TrDbg(phyaddr, "ECT", i, j); 
                  	//printf("* PhyAddr=%d, ECTail[%02d], CH%s value=0x%04lX\r\n", i,ch_tbl[j],value);
                  	printf("* PhyAddr=%d, ECTail[%02d], CH%s value=0x%04lX\r\n", phyaddr,i,ch_tbl[j],value);
                }
            }
        } 
        else if((stricmp(argv[2], "nc") == 0) ||(stricmp(argv[2], "NC") == 0))
        {
        	for(j=0; j<4; j++)
            {
            	for(i=16; i<32; i++)
                {
                	value = mtPhyMiiRead_TrDbg(phyaddr, "NC", i, j); 
                    printf("* NC1[%02d], CH%s value=0x%04lX\r\n", i, ch_tbl[j], value);
                }
                for(i=32; i<48; i++)
                {
                	value = mtPhyMiiRead_TrDbg(phyaddr, "NC", i, j); 
                    printf("* NC2[%02d], CH%s value=0x%04lX\r\n", i, ch_tbl[j], value);
                }
                for(i=48; i<64; i++)
                {
                	value = mtPhyMiiRead_TrDbg(phyaddr, "NC", i, j); 
                    printf("* NC3[%02d], CH%s value=0x%04lX\r\n", i, ch_tbl[j], value);
                }  
            }
        }
        else if((stricmp(argv[2], "tr") == 0) ||(stricmp(argv[2], "TR") == 0))
        {
        	for(i=0; i<16; i++)
        	{
            	value = mtPhyMiiRead_TrDbg(phyaddr, "NC", i, 0); 
                printf("* NC[%02d], CH%s value=0x%04lX\r\n", i, ch_tbl[j], value);
            }
        }
        else if((stricmp(argv[2], "dfe") == 0) ||(stricmp(argv[2], "DFE") == 0))
        {
        	for(j=0; j<4; j++)
        	{
				for(i=0; i<3; i++)
				{	//DFE tap0/1/2
					value = mtPhyMiiRead_TrDbg(phyaddr, "DEC", (4*j+i), j); 
					printf("* DFETap[%02d], CH%s value=0x%06lX\r\n", i, ch_tbl[j], value);
				}  
			}
            for(j=0; j<4; j++)
            {
                for(i=0; i<16; i++)
                {	//DFE Tail
                    value = mtPhyMiiRead_TrDbg(phyaddr, "DFEDC", i, j); 
                    printf("* DFEDC[%02d], CH%s value=0x%04lX\r\n", i, ch_tbl[j], value);
                }
            }
        }
		mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 4, 0x40000, 0);		// V1.11  un-force freeze DSP
		sw_patch_flag = sw_patch_flag_bak;						// V1.11
 	}  
  	else 
  	{ 	// error message
      	printf("Usage: coeffr <PhyAddr> <ffe/dec/ec/nc/tr/dfe>\r\n");
      	return 0;           
  	}
  	// check parameters
  	return 0;           
}
#endif

#if debug_flag
static int doPhyDispFlag(int argc, char *argv[], void *p)
{
    int i;
    if(argc >= 2 && argc <= 9) 
	{
        tcPhy_disp_level = 0;
        for(i=1; i<argc; i++) 
		{
            if(stricmp(argv[i], "on") == 0)
			{
                tcPhy_disp_level++;
            }
        }
    }   
    printf("tcPhy display level: %d.\r\n", tcPhy_disp_level);
    return 0;
}
#endif
#if debug_flag
static int doSkewDispFlag(int argc, char *argv[], void *p)
{
	if(argc >= 2 && argc <= 9) 
	{
		mtSkewCal_disp_level = 0;
		if(stricmp(argv[1], "on") == 0)
		{
			mtSkewCal_disp_level++;
		}
	}   
    printf("1000M Skew cal SW patch display level: %d.\r\n", mtSkewCal_disp_level);
    return 0;
}
#endif

int doPhyForceMode(int argc, char *argv[], void *p) 	
{
	uint16 port_num = 0, phyaddr = 0;
	uint32 reg3 = 0, r15_temp = 0;
	
	phyaddr = checked_atoi(argv[1]);
	reg3 = tcMiiStationRead(phyaddr, 3);
	r15_temp = tcMiiStationRead(phyaddr, 15);

	tcMiiStationWrite(phyaddr, 0x0, 0x1040);  // [12]:AN, [6]:MSB_0x10=giga	

	if(argc == 5)  // tce forcemode <phyaddr> <an|force> <10|100|1000> <full|half>
	{
		if(stricmp(argv[2], "an") == 0)
		{
			if ((stricmp(argv[3], "10") == 0)&&(stricmp(argv[4], "full") == 0))
	{
				tcMiiStationWrite(phyaddr, 0x04, 0x0041);
				tcMiiStationWrite(phyaddr, 0x09, 0x0000);
				printf("an 10 full  \r\n");
	}
			else if((stricmp(argv[3], "10") == 0)&&(stricmp(argv[4], "half") == 0))
	{
				tcMiiStationWrite(phyaddr, 0x04, 0x0021);
				tcMiiStationWrite(phyaddr, 0x09, 0x0000);
				printf("an 10 half  \r\n");
	}
			else if((stricmp(argv[3], "100") == 0)&&(stricmp(argv[4], "full") == 0))
	{
				tcMiiStationWrite(phyaddr, 0x04, 0x0101);
				tcMiiStationWrite(phyaddr, 0x09, 0x0000);
				printf("an 100 full  \r\n");
	}
			else if((stricmp(argv[3], "100") == 0)&&(stricmp(argv[4], "half") == 0))
	{
				tcMiiStationWrite(phyaddr, 0x04, 0x0081);
				tcMiiStationWrite(phyaddr, 0x09, 0x0000);
				printf("an 100 half  \r\n");
	}
			else if(stricmp(argv[3], "1000") == 0)
	{
				if(reg3==EPHY_ID_7523Ge)
				{
					if(stricmp(argv[4], "full") == 0)
	{
						tcMiiStationWrite(phyaddr, 0x04, 0x0001);
						tcMiiStationWrite(phyaddr, 0x09, 0x0600);
						printf("an 1000 full  \r\n");
	}
					else if(stricmp(argv[4], "half") == 0)
	{    
						printf("not support 1000 half \r\n");
				 	}		 	
			 	}
				else
					printf("port[%d] not support 1000 mode. \r\n",phyaddr);	
	}

			//tcMiiStationWrite(phyaddr, 0x00, 0x1200);
			printf("back to AN \r\n");
		}
		else if(stricmp(argv[2], "force") == 0)
		{
			if ((stricmp(argv[3], "10") == 0)&&(stricmp(argv[4], "full") == 0))
			{
				tcMiiStationWrite(phyaddr, 0x04, 0x0041);
				tcMiiStationWrite(phyaddr, 0x09, 0x0000);
				printf("force(an) 10 full  \r\n");
			}
			else if((stricmp(argv[3], "10") == 0)&&(stricmp(argv[4], "half") == 0))
			{
				tcMiiStationWrite(phyaddr, 0x00, 0x000);
				printf("force 10 half  \r\n");
			}
			else if((stricmp(argv[3], "100") == 0)&&(stricmp(argv[4], "full") == 0))
			{
				tcMiiStationWrite(phyaddr, 0x04, 0x0101);
				tcMiiStationWrite(phyaddr, 0x09, 0x0000);
				printf("force(an) 100 full  \r\n");
			}
			else if((stricmp(argv[3], "100") == 0)&&(stricmp(argv[4], "half") == 0))
			{
				tcMiiStationWrite(phyaddr, 0x00, 0x2000);
				printf("force 100 half  \r\n");
			}
			else if(stricmp(argv[3], "1000") == 0)
			{
				if(reg3==EPHY_ID_7523Ge)
				{
					if(stricmp(argv[4], "full") == 0)
					{
						tcMiiStationWrite(phyaddr, 0x04, 0x0001);
						tcMiiStationWrite(phyaddr, 0x09, 0x0600);
						printf("force(an) 1000 full  \r\n");
				 	}
					else if(stricmp(argv[4], "half") == 0)
					{
				 		printf("not support 1000 half \r\n");
				 	}		 	
			 	}
				else
					printf("port[%d] not support 1000 mode. \r\n",phyaddr);	
			}
		}
		else
		{
			phyaddr = 0xFFFF;
		}
	}
	else if(argc == 4) // tce forcemode <phyaddr> <an|force> <10|100|1000>
	{
		if(stricmp(argv[2], "an") == 0)
		{
			if(stricmp(argv[3], "10") == 0)
			{
				tcMiiStationWrite(phyaddr, 0x04, 0x0061);
				tcMiiStationWrite(phyaddr, 0x09, 0x0000);
			}
			else if(stricmp(argv[3], "100") == 0)
			{
				tcMiiStationWrite(phyaddr, 0x04, 0x01e1);
				tcMiiStationWrite(phyaddr, 0x09, 0x0000);
			}
			else if(stricmp(argv[3], "1000") == 0)
			{
				tcMiiStationWrite(phyaddr, 0x04, 0x01e1);
				tcMiiStationWrite(phyaddr, 0x09, 0x0600);
			}
			else
			{
				phyaddr = 0xFFFF;
			}
			//tcMiiStationWrite(phyaddr, 0x00, 0x1200);
			printf("back to AN \r\n");
		}
		else if(stricmp(argv[2], "force") == 0)
		{
			if(stricmp(argv[3], "10") == 0)
			{
				tcMiiStationWrite(phyaddr, 0x00, 0x100);
				printf("force 10 half  \r\n");
			}
			else if(stricmp(argv[3], "100") == 0)
			{
				tcMiiStationWrite(phyaddr, 0x00, 0x2100);
				printf("force 100 half  \r\n");
			}
			else if((stricmp(argv[3], "1000") == 0)&&(reg3==EPHY_ID_7523Ge))
			{
				//tcMiiStationWrite(phyaddr, 0x00, 0x40);
				printf("not support 1000 half  \r\n");
			}
			else
			{
				phyaddr= 0xFFFF;
			}
		}
		else
		{
			phyaddr= 0xFFFF;
		}
	}
	else if(argc == 3) // tce forcemode <phyaddr> <an|force> 
	{
		if(stricmp(argv[2], "an") == 0)
		{
			if((reg3==EPHY_ID_7523Ge))
			{
				tcMiiStationWrite(phyaddr, 0x04, 0x01e1);
				tcMiiStationWrite(phyaddr, 0x09, 0x0600);
			}

			//tcMiiStationWrite(phyaddr, 0x00, 0x1200);
			printf("back to AN  \r\n");
		}
			else
			{
			phyaddr= 0xFFFF;
			}
		}
			else
			{
			phyaddr= 0xFFFF;
		}
	if(phyaddr == 0xFFFF)
		printf("usage: tce forcemode <phyaddr> <an|force> <10|100|1000> <full|half> \r\n");
		
	return 0;
}

int doPhySLTMode(int argc, char *argv[], void *p) 	// allen_20171109
{
	uint16 phyaddr = 0;
	uint32 reg3 = 0, r15_temp = 0;
	
	phyaddr = checked_atoi(argv[1]);
	reg3 = tcMiiStationRead(phyaddr, 3);
	r15_temp = tcMiiStationRead(phyaddr, 15);

	tcMiiStationWrite(phyaddr, 0x0, 0x1040);  // [12]:AN, [6]:MSB_0x10=giga	

	if(argc == 3) // tce sltmode <phyaddr> <10|100|1000> 
	{
		if(stricmp(argv[2], "10") == 0)
		{
			doEphySetSLTMode(phyaddr, 10);
			printf("force 10 half  \r\n");
			}
		else if(stricmp(argv[2], "100") == 0)
		{
			doEphySetSLTMode(phyaddr, 100);
			printf("force 100 half  \r\n");
			}
		else if((stricmp(argv[2], "1000") == 0)&&(reg3==EPHY_ID_7523Ge))
		{
			doEphySetSLTMode(phyaddr, 1000);
			printf("force 1000 loopback  \r\n");
		}
		else
		{
			phyaddr= 0xFFFF;
		}
	}
	else
	{
		phyaddr= 0xFFFF;
	}
	if(phyaddr == 0xFFFF)
		printf("usage: tce sltmode <phyaddr> <10|100|1000>  \r\n");
		
	return 0;
}

int doPhyBootloaderMode(int argc, char *argv[], void *p) 	
{
	uint16 phyaddr = 0;
	//uint32 reg3 = 0, r15_temp = 0;
	
	phyaddr = checked_atoi(argv[1]);

	if(argc == 3) // tce bootloadermode <phyaddr> <on|off> 
	{
		if(stricmp(argv[2], "on") == 0)
		{
			tcMiiStationWrite(phyaddr, 0x9, 0x0000);        // disable Giga
	        mtEMiiRegWrite(phyaddr, 0x7, 0x003c, 0x0000);   // disable EEE
			printf("force bootloader mode on  \r\n");
		}
		else if(stricmp(argv[2], "off") == 0)
		{
			tcMiiStationWrite(phyaddr, 0x9, 0x0600);        // enable Giga
	        mtEMiiRegWrite(phyaddr, 0x7, 0x003c, 0x0006);   // enable EEE
			printf("force bootloader mode off  \r\n");
		}
		else
		{
			phyaddr= 0xFFFF;
		}
	}
	else
	{
		phyaddr= 0xFFFF;
	}
	if(phyaddr == 0xFFFF)
	{
		printf("usage: tce bootloadermode <phyaddr> <on|off>  \r\n");
	}	
	return 0;
}



void doEphySetSLTMode(uint8 phyaddr, uint16 speed_mode)
{
	//uint32 reg3 = 0, r15_temp = 0;
	uint16 ch_addr = 0;
    uint32 node_addr = 0;
    const uint16 page_reg = 31;
    const uint32 Token_Ring_debug_reg = 0x52B5;
    const uint32 Token_Ring_Control_reg = 0x10;
    const uint32 Token_Ring_Low_data_reg = 0x11;
    const uint32 Token_Ring_High_data_reg = 0x12;
    uint32 data_addr = 0;
    uint32 value = 0;
    uint32 value_high = 0;
    uint32 value_low = 0;
	sw_down_shift_en=0;slt_excuting=1;inital_finish_flag = 1;
	
    if(speed_mode == 10)
	{
		tcMiiStationWrite(phyaddr, 0x00, 0x100);
	}
	else if(speed_mode == 100)
	{
		tcMiiStationWrite(phyaddr, 0x00, 0x2100);
	}
	else if(speed_mode == 1000)
	{
		tcMiiStationWrite(phyaddr, 0x1f, 0x8000);
		tcMiiStationWrite(phyaddr, 0x18, 0x0001);
		//printf(" reg24 = 0x%x \n", tcMiiStationRead(phyaddr, 0x18));
		
		ch_addr = 0x02;
		node_addr = 0x0F;
		data_addr = 1; 					// PCS_1
		//data_addr = data_addr&0x3F;
		// sscanf(argv[4], "%lx", &value);			// 24 bit //HEX
		value = 0x28001;
		value_high = (0x00FF0000&value)>>16;
		value_low = (0x0000FFFF&value);
		tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);	   
		tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
		tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
		tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(ch_addr<<11)|(node_addr<<7)|(data_addr<<1));

		tcMiiStationWrite(phyaddr, 0x1f, 0x8000);
		tcMiiStationWrite(phyaddr, 0x09, 0x1f00);
		//tcMiiStationWrite(phyaddr, 0x00, 0x1200);
	}
}


		

void doEphySetSETMode(uint8 set_mode, uint16 parameter)    // allen_20190709
{
	uint16	phyaddr=0;
	
	if(set_mode == 0)				// set LED 
	{
		mtEMiiRegWrite(12, 0x1f, 0x21, 0x800a);
		mtEMiiRegWrite(12, 0x1f, 0x25, 0x003f);
		mtEMiiRegWrite(12, 0x1f, 0x27, 0x003f);
	
		if(parameter == 0) 			// set for LED0 on/active, LED1 off
 		{
 			mtEMiiRegWrite(12, 0x1f, 0x24, 0xc007);
 			mtEMiiRegWrite(12, 0x1f, 0x26, 0x4007);
		}
		else if(parameter == 1)		// set for LED0 off, LED1 on/active
		{
			mtEMiiRegWrite(12, 0x1f, 0x24, 0x4007);
 			mtEMiiRegWrite(12, 0x1f, 0x26, 0xc007);
		}
		else if(parameter == 2)		// set for default, LED0/1 on/active
		{
			mtEMiiRegWrite(12, 0x1f, 0x24, 0xc007);
 			mtEMiiRegWrite(12, 0x1f, 0x26, 0xc007);
		}
	}
	else if(set_mode == 1)	// set Power Saving
	{
		
		// global register
		if(parameter == 0)	// default
		{
			mtEMiiRegWrite(9, 0x1f, 0x271, 0x7fff);
			mtEMiiRegWrite(9, 0x1f, 0x269, 0x444f);
		}
		else if(parameter == 1)	// for 110m
		{
			mtEMiiRegWrite(9, 0x1f, 0x271, 0x7e0e);
			//mtEMiiRegWrite(9, 0x1f, 0x272, 0x14ff);
			mtEMiiRegWrite(9, 0x1f, 0x269, 0x111f);
		}
		else if(parameter == 2)	
		{
			mtEMiiRegWrite(9, 0x1f, 0x271, 0x7c63);
			//mtEMiiRegWrite(9, 0x1f, 0x272, 0x14ff);
			mtEMiiRegWrite(9, 0x1f, 0x269, 0x000f);
		}
		else if(parameter == 3)	
		{
			mtEMiiRegWrite(9, 0x1f, 0x271, 0x7c42);
			//mtEMiiRegWrite(9, 0x1f, 0x272, 0x14ff);
			mtEMiiRegWrite(9, 0x1f, 0x269, 0x000f);
		}
	
		// local register
		for(phyaddr = 9; phyaddr<=12; phyaddr++)
		{
			tcMiiStationWrite(phyaddr, 0x0, 0x0800);
			tcMiiStationWrite(phyaddr, 0x9, 0x0600);
		
			if(parameter == 0)	// default
			{
				tcMiiStationWrite(phyaddr, 0x17, 0x00a0);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x3e, 0xc000);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x5c, 0x7777);
			}
			else if(parameter == 3)	
			{
				tcMiiStationWrite(phyaddr, 0x17, 0x00b0);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x3e, 0x0000);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x5c, 0x4444);
			}
			else						// for 110m
			{
				tcMiiStationWrite(phyaddr, 0x17, 0x00b0);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x3e, 0x0000);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x5c, 0x7777);
			}
			udelay(100);
			//tcMiiStationWrite(phyaddr, 0x0, 0x1200);
		}
	}
}


uint32 getPhyPortMask(void)
{
	if(isMT7520S)
		return 0x1000;		// port12
	else if( isMT7520 || isMT7525)
		return 0x1e00;		// port9-12
	else if(isMT7525G || isMT7520G)
		return 0xf;			// port0-3
	else
	{
		printf("not found phy port\n");
		return 0;
	}
}

#define PHY_EEE_ON 					1
#define PHY_EEE_OFF 				2
#define PHY_EEE_RECOVER 			3
void phyForceEEEAllOnOffRecover(int mode)
{
	int port_num;
	uint32 reg3;
	uint32 phy_port_mask = getPhyPortMask();
	for(port_num= 0 ; port_num<32; port_num++)
	{
		if(!(phy_port_mask & (1<<port_num)))
			continue;		
		reg3 = tcMiiStationRead(port_num, 3); 
		switch(mode)
		{
			case PHY_EEE_ON:
				if((reg3 == 0x9421)||(reg3 == 0x9412))
				{
					mtEMiiRegWrite(port_num, 0x7, 0x003c, 0x0006);
				}
				else
				{
					tcMiiStationWrite(port_num, 31, 0xb000); //  l3
					tcMiiStationWrite(port_num, 17, 0x0002); //  17 2 : enable FE EEE
				}
				printf("force port[%d] EEE on \r\n", port_num);					
				break;
			case PHY_EEE_OFF:
				if((reg3 == 0x9421)||(reg3 == 0x9412))
				{
					mtEMiiRegWrite(port_num, 0x7, 0x003c, 0x0000);
				}
				else
				{
					tcMiiStationWrite(port_num, 31, 0xb000); //  l3
					tcMiiStationWrite(port_num, 17, 0x0000); //  17 2 : enable FE EEE
				}
				printf("force port[%d] EEE off \r\n", port_num);
				break;
			case PHY_EEE_RECOVER:
				if((reg3 == 0x9421)||(reg3 == 0x9412))
				{
					mtEMiiRegWrite(port_num, 0x7, 0x003c, default_ge_dev7r3c);
				}
				else
				{
					tcMiiStationWrite(port_num, 31, 0xb000); //  l3
					tcMiiStationWrite(port_num, 17, default_fe_l3r17); //  17 2 : enable FE EEE
				}
				printf("recover port[%d] EEE to default \r\n", port_num);
				break;
			default:
				printf("not support mode %d",mode);
				break;
		}
	}
}

int doPhyForceEEE (int argc, char *argv[], void *p) 
{
	uint16 phyaddr = 0;
	uint32 reg3 = 0, r15_temp = 0;
	if(!strcmp(argv[1],"all"))
	{
		phyaddr = 0xffff;
	}
	else
	{
		phyaddr = checked_atoi(argv[1]);
		reg3 = tcMiiStationRead(phyaddr, 3); 
	}
	r15_temp = tcMiiStationRead(phyaddr, 0xf);
	if(argc == 3) 
	{
		if((stricmp(argv[2], "on") == 0))
		{
			if(phyaddr == 0xffff)
			{
				phyForceEEEAllOnOffRecover(PHY_EEE_ON);
				return 0;
			}
			
			if((reg3 == EPHY_ID_7523Ge))
			{
				mtEMiiRegWrite(phyaddr, 0x7, 0x003c, 0x0006);
			}
			else
			{
				tcMiiStationWrite(phyaddr, 31, 0xb000); //  l3
				tcMiiStationWrite(phyaddr, 17, 0x0002); //  17 2 : enable FE EEE
			}
			printf("force port[%d] EEE on \r\n", phyaddr);
		}
		else if((stricmp(argv[2], "off") == 0))
		{
		
			if(phyaddr == 0xffff)
			{
				phyForceEEEAllOnOffRecover(PHY_EEE_OFF);
				return 0;
			}

			if((reg3 == EPHY_ID_7523Ge))
			{
				mtEMiiRegWrite(phyaddr, 0x7, 0x003c, 0x0000);
			}
			else
			{
				tcMiiStationWrite(phyaddr, 31, 0xb000); //  l3
				tcMiiStationWrite(phyaddr, 17, 0x0000); //  17 2 : enable FE EEE
			}
			printf("force port[%d] EEE off \r\n", phyaddr);
		}
		else if((stricmp(argv[2], "recover") == 0))
		{
		
			if(phyaddr == 0xffff){
				phyForceEEEAllOnOffRecover(PHY_EEE_RECOVER);
				return 0;
			}

			if((reg3==EPHY_ID_7523Ge))
			{
				mtEMiiRegWrite(phyaddr, 0x7, 0x003c, default_ge_dev7r3c);
			}
			else
			{
				tcMiiStationWrite(phyaddr, 31, 0xb000); //  l3
				tcMiiStationWrite(phyaddr, 17, default_fe_l3r17); //  17 2 : enable FE EEE
			}
			printf("recover port[%d] EEE to default \r\n", phyaddr);
		}
		else
		{
			phyaddr = 0xFFFF;
		}
	}
	else
	{
		phyaddr = 0xFFFF;
	}
	if(phyaddr == 0xFFFF)
		printf("usage: tce forceEEE <phyaddr> <on|off|recover> \r\n");

	return 0;
}

int doPhyForceGE (int argc, char *argv[], void *p) // allen_20131120
{
	uint16 phyaddr = 0;
	uint32 reg3 = 0, r15_temp = 0;
	
	phyaddr = checked_atoi(argv[1]);
	reg3 = tcMiiStationRead(phyaddr, 3); 
	r15_temp = tcMiiStationRead(phyaddr, 0xf);
	
	if((reg3 == EPHY_ID_7523Ge))
	{
		if (argc == 4) 
		{
			if ((stricmp(argv[2], "multi") == 0))
			{
				if ((stricmp(argv[3], "master") == 0))
				{
					tcMiiStationWrite(phyaddr, 0x9, 0x1e00);
					printf("force port[%d] multi master on \r\n", phyaddr);
				}
				else if((stricmp(argv[3], "slave") == 0))
				{
					tcMiiStationWrite(phyaddr, 0x9, 0x1600);
					printf("force port[%d] multi slave on \r\n", phyaddr);
				}
			}
			else if((stricmp(argv[2], "single") == 0))
			{
				if ((stricmp(argv[3], "master") == 0))
				{
					tcMiiStationWrite(phyaddr, 0x9, 0x1a00);
					printf("force port[%d] single master on \r\n", phyaddr);
				}
				else if((stricmp(argv[3], "slave") == 0))
				{
					tcMiiStationWrite(phyaddr, 0x9, 0x1400);
					printf("force port[%d] single slave on \r\n", phyaddr);
				}
			}
			else
			{
				phyaddr = 0xFFFF;
			}			
		}
		else if(argc == 3) 
		{
			if((stricmp(argv[2], "multi") == 0))
			{
				tcMiiStationWrite(phyaddr, 0x9, 0x0600);
				printf("force port[%d] multi on \r\n", phyaddr);
			}
			else if((stricmp(argv[2], "single") == 0))
			{
				tcMiiStationWrite(phyaddr, 0x9, 0x0400);
				printf("force port[%d] single on \r\n", phyaddr);
			}
			else
			{
				phyaddr = 0xFFFF;
			}
		}
		else
		{
			phyaddr = 0xFFFF;
		}
	}
	else
	{
		phyaddr = 0xFFFF;
	}
	if(phyaddr == 0xFFFF)
		printf("usage: tce forceGE <phyaddr> <multi|single> <master|slave>\r\n");

	return 0;
}

int doPhyForceReset(int argc, char *argv[], void *p) 	// allen_20131120
{
	uint16 phyaddr = 0;
	uint32 reg3, r15_temp;

	phyaddr = checked_atoi(argv[1]);
	reg3 = tcMiiStationRead(phyaddr, 3); 
	r15_temp = tcMiiStationRead(phyaddr, 15); 
	
	if(reg3==EPHY_ID_7512Ge) // 7512GE
	{
		tcPhyVer = tcPhyVer_7512Ge;
	}
	else if((reg3 == EPHY_ID_7530Ge_7512Fe)&&(((r15_temp>>13)&0x1) == 1)) // 7530GE
	{
		tcPhyVer = mtPhyVer_7530;
	}
	//else if(reg3==EPHY_ID_7522Ge)
	//{
	//	tcPhyVer = tcPhyVer_7522Ge;
	//}
	else if(reg3==EPHY_ID_7522Fe)
	{
		tcPhyVer = tcPhyVer_7522Fe;
	}
	else if(reg3==EPHY_ID_7523Ge)
	{
		tcPhyVer = tcPhyVer_7523Ge;
	}
	else
	{
		tcPhyVer = tcPhyVer_7512Fe;
	}
	
	if (argc == 2) 
	{
		tcMiiStationWrite(phyaddr, 0x0, 0x0800);
		pause(1000);
		tcMiiStationWrite(phyaddr, 0x0, 0x1040);
		printf("port[%d] reset \r\n", phyaddr);

		switch (tcPhyVer) 
		{  
			case tcPhyVer_7523Ge:
				en75xxGePhyCfgLoad(0);			
			break;
		}
	}
	else
	{
		phyaddr = 0xFFFF;
	}
	if(phyaddr == 0xFFFF)
		printf("usage: tce forceReset <phyaddr> \r\n");
	return 0;
}

int doPhyrestartAN(int argc, char *argv[], void *p) 	// allen_20131213
{
	uint16 phyaddr = 0;
	uint32 reg3, reg0;

	phyaddr = checked_atoi(argv[1]);
	reg3 = tcMiiStationRead(phyaddr, 3); 
	reg0 = tcMiiStationRead(phyaddr, 0);
	reg0 = reg0 | 0x1200;
	if(argc == 2) 
	{
		tcMiiStationWrite(phyaddr, 0x0, reg0);
		printf("phyaddr[%d] restart AN \r\n", phyaddr);		
	}
	else
	{
		phyaddr = 0xFFFF;
	}

	if(phyaddr == 0xFFFF)
		printf("usage: tce restartAN <phyaddr> \r\n");
	return 0;
}

int doPhyChkVal(int argc, char *argv[], void *p)
{	// for tcPhyChkVal					
	phychkval_flag = 1;

	//printf(" [debug 0] (%d) \r\n", tcPhyVer);
	
   	if(argc == 2) 
	{ 	// set port_num for tcPhyChkVal()
		switch (tcPhyVer) 
	   	{
			case mtPhyVer_7530:	
				phychkval_portnum = checked_atoi(argv[1]);
				//printf(" [debug 1-0] (%d) \r\n", tcPhyVer);
				break;
				
			case tcPhyVer_7512Fe:	
			case tcPhyVer_7522Fe:
				phychkval_portnum = checked_atoi(argv[1]);
				//printf(" [debug 1-1] (%d) \r\n", tcPhyVer);
				break;

			case tcPhyVer_7512Ge:	
			case tcPhyVer_7522Ge:
				phychkval_portnum = checked_atoi(argv[1]);
				//printf(" [debug 1-2] (%d) \r\n", tcPhyVer);
				break;	
				
			case tcPhyVer_7523Ge:
				phychkval_portnum = checked_atoi(argv[1]);
				break;		

			default:
			//printf(" [debug 1-3] (%d) \r\n", tcPhyVer);
				break;
		}
		//printf(" [debug 2] (%d) \r\n", tcPhyVer);
    }
    return 0;
}


int getAllPortStatus (int argc, char *argv[], void *p)
{
	uint16 loop,reg_02,reg_04,reg_0a,reg_1d,dev7_reg3c,reg_01,status_temp;


        //set all port 
        for(loop=all_port_start;loop <= all_port_end;loop++)
	    {	
		   reg_01=tcMiiStationRead(loop, 1);
		   reg_04=tcMiiStationRead(loop, 4);
		   reg_0a=tcMiiStationRead(loop, 0x0a);		
		   reg_1d=tcMiiStationRead(loop, 0x1d);	
		   dev7_reg3c=mtEMiiRegRead(loop, 0x7, 0x3c);
		   printf("port%d  \n\r",loop);
		   reg_01=reg_01&0x0004;
		   if(reg_01 !=0)
		    {
		      printf("link status=link up");
		      status_temp=reg_1d&0x0018;
		      reg_0a=reg_0a&0x4000;
	          if(status_temp ==0x0000) { printf("  link speed=10M");}	
		      if(status_temp ==0x0008) { printf("  link speed=100M");}	
		      if(status_temp ==0x0010) { if(reg_0a ==0x4000) {printf("  link speed=1G Master");} else {printf("  link speed=1G Salve");}}
		      status_temp=reg_1d&0x0020;
		      if(status_temp ==0x0000) { printf("  Half_duplex");} else {printf("  Full_duplex");}
		      reg_04=reg_04&0x0c00;
		      if(reg_04 ==0x0000) {printf("  flowcontrol=disable");}
		      if(reg_04 ==0x0c00) {printf("  flowcontrol=enable");}		
		      dev7_reg3c=dev7_reg3c&0x0006;
		      if(dev7_reg3c ==0x0006) {printf("  EEE ability=100M and 1G\n\r");}
		      if(dev7_reg3c ==0x0002) {printf("  EEE ability=only 100M\n\r");}
		      if(dev7_reg3c ==0x0004) {printf("  EEE ability=only 1G\n\r");}
		      if(dev7_reg3c ==0x0000) {printf("  EEE ability=disable\n\r");}		
		    }	
		   else
		    {
		      printf("link down \n\r");			
		    }
	    }
        printf("\n\r");	
	
}
		
int settingFlowControl (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0,loop,reg_04,control;
	if(argc !=3) 
	{  
          printf(" flowControl <phyaddr> <on|off>\r\n");
		  printf("phy=32 is all port\r\n");
	}
	else 
	{
	phyaddr = checked_atoi(argv[1]);
        control = checked_atoi(argv[2]);	
        if(phyaddr==32)
	   {
             //set all port 
        for(loop=all_port_start;loop <= all_port_end;loop++)
	      {	
		    reg_04=tcMiiStationRead(loop, 4);
		    if(control==1) {reg_04=reg_04|0x0c00;} 
		    if(control==0) {reg_04=reg_04&0xf3ff;} 
		    tcMiiStationWrite(loop, 0x04, reg_04);
	      }
		if(control==1) { printf("all port flowControl on\r\n");}
		if(control==0) { printf("all port flowControl off\r\n");}
	   }
	else
	 { 
		reg_04=tcMiiStationRead(phyaddr, 4);
		if(control==1) {reg_04=reg_04|0x0c00;}
		if(control==0) {reg_04=reg_04&0xf3ff;}
		tcMiiStationWrite(phyaddr, 0x04, reg_04); 
		if(control==1) { printf("prot%d flowControl on\r\n",phyaddr);}
		if(control==0) { printf("port%d flowControl off\r\n",phyaddr);}		
	 }
		 printf("\n\r");  
		
	}
	
}



int settingloopback (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0,loop,reg_00,mode,page1_reg1d,control;
	if(argc !=4) 
	{  
          printf("tce setLoopback <phyaddr> <mode> <on|off>\r\n");
		  printf("phy=32 is all port\r\n");
	}
	else 
	{
	    phyaddr = checked_atoi(argv[1]);
        mode = checked_atoi(argv[2]);
        control = checked_atoi(argv[3]);
	if(phyaddr==32)	   
	{          //set all port         
	for(loop=all_port_start;loop <= all_port_end;loop++)	      
	{            
		reg_00=tcMiiStationRead(loop, 0);	        
		tcMiiStationWrite(loop, 0x1f, 0x0001);	        
		page1_reg1d=tcMiiStationRead(loop, 0x1a);	        
		if(mode==1) 
			{ 		    
				if(control==1) {page1_reg1d=page1_reg1d|0x8000;printf("enable port %d MDI Loopback \r\n",loop);} 		    
				if(control==0) {page1_reg1d=page1_reg1d&0x7fff;printf("disable port %d MDI Loopback \r\n",loop);}		        
				tcMiiStationWrite(loop, 0x1a, page1_reg1d);
		        					
			}
		
		tcMiiStationWrite(loop, 0x1f, 0x0000);		    
		if(mode==0)		
			{ 		
				if(control==1) {reg_00=reg_00|0x4000;printf("enable port %d Loopback \r\n",loop);}		
				if(control==0) {reg_00=reg_00&0xbfff;printf("disable port %d Loopback \r\n",loop);}        
						
			}		

		tcMiiStationWrite(loop, 0, reg_00);         
		      

	}	   
	}
	   else	
	   {
         reg_00=tcMiiStationRead(phyaddr, 0);
	     tcMiiStationWrite(phyaddr, 0x1f, 0x0001);
         page1_reg1d=tcMiiStationRead(phyaddr, 0x1d);
	     if(mode==1) 
		 { 
  	  	 if(control==1) {page1_reg1d=page1_reg1d|0x8000;printf("enable port %d MDI Loopback \r\n",loop);} 
		 if(control==0) {page1_reg1d=page1_reg1d&0x7fff;printf("disable port %d MDI Loopback \r\n",loop);}	
	     tcMiiStationWrite(phyaddr, 0x1d, page1_reg1d);	
         			
		 }		
		 tcMiiStationWrite(phyaddr, 0x1f, 0x0000);
		
	    if(mode==0)
		 { 
		  if(control==1) {reg_00=reg_00|0x4000;printf("enable port %d Loopback \r\n",loop);}
		  if(control==0) {reg_00=reg_00&0xbfff;printf("disable port %d Loopback \r\n",loop);}
		  
		 }
		  tcMiiStationWrite(phyaddr, 0, reg_00);          
	   }
        printf("\n\r");		   
	}
	return 0;
}

int settingANSpeed (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0,loop,speed,duplex,reg_09,reg_04;
	if(argc !=4) 
	{  
          printf("tce setANSpeed <phyaddr> <speed> <duplex>\r\n");
		  printf("phy=32 is all port      1G half is set all speed\r\n");
	}
	else 
	{
	    phyaddr = checked_atoi(argv[1]);
        speed = checked_atoi(argv[2]);
        duplex = checked_atoi(argv[3]);	
	if(phyaddr==32)
	{
          //set all port 
          for(loop=all_port_start;loop <= all_port_end;loop++)
	   {
             reg_09=tcMiiStationRead(loop, 9);
             reg_04=tcMiiStationRead(loop, 4);	     
             if(speed == 1000 )	
	      { 
        if(duplex == 1 )
		{			
          reg_09=reg_09|0x0200;
		  tcMiiStationWrite(loop, 9, reg_09);
		  reg_04=reg_04&0xfe1f;
		  tcMiiStationWrite(loop, 4, reg_04);
          tcMiiStationWrite(loop, 0, 0x1340);
		  printf("port%d 1G full duplex set\r\n",loop);
		}
		else 
		{
          reg_09=reg_09|0x0200;
		  tcMiiStationWrite(loop, 9, reg_09);
		  reg_04=reg_04|0x01e0;
		  tcMiiStationWrite(loop, 4, reg_04);
          tcMiiStationWrite(loop, 0, 0x1340);
		  printf("port%d all speed set \r\n",loop);
		}
		 
	      }
	    if(speed == 100 )	
	    { 
           reg_09=reg_09&0xfdff;
		   tcMiiStationWrite(loop, 9, reg_09);
		if(duplex == 1 ) { reg_04=reg_04&0xff1f;reg_04=reg_04|0x0100;printf("port%d 100M full duplex set\r\n",loop);} else { reg_04=reg_04&0xfe9f;reg_04=reg_04|0x0080;printf("port%d 100M Half duplex set\r\n",loop);}
		   tcMiiStationWrite(loop, 4, reg_04);
		   tcMiiStationWrite(loop, 0, 0x1340);	
	      }
	      if(speed == 10 )	
	      { 
           reg_09=reg_09&0xfdff;
		   tcMiiStationWrite(loop, 9, reg_09);
		if(duplex == 1 ) { reg_04=reg_04&0xfe5f;reg_04=reg_04|0x0040;printf("port%d 10M full duplex set\r\n",loop);} else { reg_04=reg_04&0xfe3f;reg_04=reg_04|0x0020;printf("port%d 10M Half duplex set\r\n",loop);}
		   tcMiiStationWrite(loop, 4, reg_04);
		   tcMiiStationWrite(loop, 0, 0x1340);	
	      }		 
	   }

	}	

	else
	{
             reg_09=tcMiiStationRead(phyaddr, 9);
             reg_04=tcMiiStationRead(phyaddr, 4);	
         switch (speed) 	
	     {	      
           case 1000:  
		    if(duplex == 1 ) 
		     {
               reg_09=reg_09|0x0200;
		       tcMiiStationWrite(phyaddr, 9, reg_09);
		       reg_04=reg_04&0xfe1f;
		       tcMiiStationWrite(phyaddr, 4, reg_04);
		       tcMiiStationWrite(phyaddr, 0, 0x1340);
               printf("port%d 1G full duplex set\r\n",phyaddr);			   
		     }
		    else
		     {
               reg_09=reg_09|0x0200;
		       tcMiiStationWrite(phyaddr, 9, reg_09);
		       reg_04=reg_04|0x01e0;
		       tcMiiStationWrite(phyaddr, 4, reg_04);
		       tcMiiStationWrite(phyaddr, 0, 0x1340);
               printf("port%d 1G all speed set\r\n",phyaddr);			   
		     }
                break;
		
             case 100: 
               reg_09=reg_09&0xfdff;
		       tcMiiStationWrite(phyaddr, 9, reg_09);
		       if(duplex == 1 ) { reg_04=reg_04&0xff1f;reg_04=reg_04|0x0100;printf("port%d 100M full duplex set\r\n",phyaddr);} else { reg_04=reg_04&0xfe9f;reg_04=reg_04|0x0080;printf("port%d 100M half duplex set\r\n",phyaddr);}
		       tcMiiStationWrite(phyaddr, 4, reg_04);
		       tcMiiStationWrite(phyaddr, 0, 0x1340);		
                break;
		
             case 10: 
               reg_09=reg_09&0xfdff;
		       tcMiiStationWrite(phyaddr, 9, reg_09);
		       if(duplex == 1 ) { reg_04=reg_04&0xfe5f;reg_04=reg_04|0x0040;printf("port%d 10M full duplex set\r\n",phyaddr);} else { reg_04=reg_04&0xfe3f;reg_04=reg_04|0x0020;printf("port%d 10M Half duplex set\r\n",phyaddr);}
		       tcMiiStationWrite(phyaddr, 4, reg_04);	
		       tcMiiStationWrite(phyaddr, 0, 0x1340);		
               break;
		
		     default:
		       printf("tce setANSpeed <phyaddr> <speed> <duplex>\r\n");
		       printf("phy=32 is all port      1G half is set all speed\r\n");
	           break;	
	      }		
	}
	}
	return 0;	
}

int doPhygetLinkRate (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0;
	uint32 	val;
    uint8 	mr_speed;

	phyaddr = checked_atoi(argv[1]);
	val = tcMiiStationRead(phyaddr, 0);
	mr_speed = ((val>>13)|(val>>5))&0x03;	// 00:10,01:100,10:1000
	
	if(mr_speed == 0x0)
		printf(" [%d] 10M \r\n", phyaddr);	
	else if(mr_speed == 0x1)
		printf(" [%d] 100M \r\n", phyaddr);
	else if(mr_speed == 0x2)
		printf(" [%d] 1000M \r\n", phyaddr);

	return mr_speed;	
}

int doPhygetDuplex (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0, r10_temp, r5_temp, r1_temp;
	uint32 	val;
    uint8 	mr_dplx;

	phyaddr = checked_atoi(argv[1]);
	val = tcMiiStationRead(phyaddr, 0);
	mr_dplx = (val>>8)&0x01;
	r10_temp = tcMiiStationRead(phyaddr, 10);
	r5_temp = tcMiiStationRead(phyaddr, 5);
	r1_temp = tcMiiStationRead(phyaddr, 1);
	#if debug_flag
	if((r1_temp>>5)&0x1 == 1)
	{
		if(((r10_temp>>11)&0x1 == 1)||((r5_temp>>8)&0x1 == 1)||((r5_temp>>6)&0x1 == 1))
		{
			mr_dplx = 1;
		}
	}
	#endif
	if(mr_dplx == 0x0)
		printf(" [%d] half \r\n", phyaddr);	
	else if(mr_dplx == 0x1)
		printf(" [%d] full \r\n", phyaddr);
		
	return mr_dplx;	
}

int doPhygetAN (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0;
	uint32 	val;
    uint8 	mr_anen;

	phyaddr = checked_atoi(argv[1]);
	val = tcMiiStationRead(phyaddr, 0);
    #if debug_flag
	mr_anen = (val>>12)&0x01;
	#endif
	if(mr_anen == 0x0)
		printf(" [%d] AN disable \r\n", phyaddr);	
	else if(mr_anen == 0x1)
		printf(" [%d] AN enable \r\n", phyaddr);

	return mr_anen;	
}

int doPhyforcePWD (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0;

	phyaddr = checked_atoi(argv[1]);
	printf(" force [%d] power down \r\n", phyaddr);
	tcMiiStationWrite(phyaddr, 0, 0x0800);

	return 0;	
}

int doPhygetLinkStatus (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0;
	uint32 	val;
    uint8 	r1_link_status;

	phyaddr = checked_atoi(argv[1]);
	val = tcMiiStationRead(phyaddr, 1);
	#if debug_flag
	r1_link_status = (val>>2)&0x01;
	#endif
	if(r1_link_status == 0x0)
		printf(" [%d] link down \r\n", phyaddr);	
	else if(r1_link_status == 0x1)
		printf(" [%d] link up \r\n", phyaddr);

	return r1_link_status;	
}

int doPhyforcePause (int argc, char *argv[], void *p)
{	// pause: r4[11:10]
	uint16 phyaddr = 0, r4_temp = 0;
	
	phyaddr = checked_atoi(argv[1]);
	if((stricmp(argv[2], "on") == 0))
	{
		r4_temp = tcMiiStationRead(phyaddr, 4);
		tcMiiStationWrite(phyaddr, 4, (r4_temp|0x0c00));
		printf(" force [%d] pause on \r\n", phyaddr);
	}
	else if((stricmp(argv[2], "off") == 0))
	{
		r4_temp = tcMiiStationRead(phyaddr, 4);
		tcMiiStationWrite(phyaddr, 4, (r4_temp&0xf3ff));
		printf(" force [%d] pause off \r\n", phyaddr);
	}

	return 0;	
}

int doPhygetLoopback (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0;
	uint32 	val;
    uint8 	r0_loopback;

	phyaddr = checked_atoi(argv[1]);
	val = tcMiiStationRead(phyaddr, 0);
	#if debug_flag
	r0_loopback = (val>>14)&0x01;
	#endif
	if(r0_loopback == 0x0)
		printf(" [%d] loopback off \r\n", phyaddr);	
	else if(r0_loopback == 0x1)
		printf(" [%d] loopback on \r\n", phyaddr);

	return r0_loopback;	
}

int doPhyforceLoopback (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0, r0_temp = 0;

	phyaddr = checked_atoi(argv[1]);
	r0_temp = tcMiiStationRead(phyaddr, 0);
	printf(" force [%d] loopback \r\n", phyaddr);
	tcMiiStationWrite(phyaddr, 0, (r0_temp|0x4000));

	return 0;
}

int doPhygetLPCap (int argc, char *argv[], void *p)
{
	uint16 phyaddr = 0, r5_temp = 0, r10_temp = 0;

	phyaddr = checked_atoi(argv[1]);
	r5_temp = tcMiiStationRead(phyaddr, 5);
	r10_temp = tcMiiStationRead(phyaddr, 10);

	if(((r10_temp>>11)&0x1) == 1)
	{
		printf(" [%d] LPCap support 1000M \r\n", phyaddr);	
		return 2;    // 1000
	}	
	else
	{
		if(((r5_temp>>7)&0x3) != 0)
		{
			printf(" [%d] LPCap support 100M \r\n", phyaddr);	
			return 1;    // 100
		}
		else
		{
			return 0;	// 10 or not an up
		}
	}
}

int doPhygetCALReg (int argc, char *argv[], void *p)
{
	uint16 	phyaddr = 0;
	uint16	l2_30=0, l0_0=0, l0_26=0, g2_25=0, l3_25=0;
	uint16	l0_30=0, l4_21=0, g4_21=0, l2_23=0;
	uint16	g1_26=0, l4_23=0;
	uint32  delay=20, cal_case=0;

	phyaddr = checked_atoi(argv[1]);

	if(argc == 4)
	{
		delay = checked_atoi(argv[2]);
		cal_case =  checked_atoi(argv[3]);
	}
	
#if 1

	if(cal_case == 1) // tx offset
	{
		GECal_flag = 0;
		while(GECal_flag == 0)
			GECal_tx_offset(phyaddr, delay);
	}
	else if(cal_case == 5) // GE tx amp
	{
		GECal_flag = 0;
		//while(GECal_flag == 0)
			//GECal_R50(phyaddr, delay);
	}
	else if(cal_case == 6) // GE tx amp
	{
		GECal_flag = 0;
		//while(GECal_flag == 0)
			//GECal_tx_amp(phyaddr, delay);
	}
	
	printf("l2_30=0x%x, l0_0 =0x%x, l0_26=0x%x, g2_25=0x%x, l3_25=0x%x, \r\n", l2_30, l0_0 , l0_26, g2_25, l3_25);
	printf("l0_30=0x%x, l4_21=0x%x, g4_21=0x%x, l2_23=0x%x,             \r\n", l0_30, l4_21, g4_21, l2_23);
	printf("g1_26=0x%x, l4_23=0x%x,                                     \r\n", g1_26, l4_23);
#endif
	
	return 0;
}

int doPhyforceLinkDownPowerSaving (int argc, char *argv[], void *p)  // allen_20160415
{
	uint16 	phyaddr = 0, port_num = 0;
	uint16	l0_30=0;

	phyaddr = checked_atoi(argv[1]);
	port_num = phyaddr - ephy_addr_base;

	if((stricmp(argv[2], "on") == 0))
	{
		if((phyaddr>=8)&&(phyaddr<=11))
		{
			tcMiiStationWrite(phyaddr, 31, 0x8000); // l0
			l0_30 = tcMiiStationRead(phyaddr, 30);
			tcMiiStationWrite(phyaddr, 30, 0xa000);
			printf("l0_30 = 0x%x \r\n",  tcMiiStationRead(phyaddr, 30));
		}else
			printf(" phyaddr = 8~11 only \r\n");
	}
	else if((stricmp(argv[2], "off") == 0))
	{
		if((phyaddr>=8)&&(phyaddr<=11))
		{
			tcMiiStationWrite(phyaddr, 31, 0x8000); // l0
			l0_30 = tcMiiStationRead(phyaddr, 30);
			tcMiiStationWrite(phyaddr, 30, 0x0000);
			printf("l0_30 = 0x%x \r\n",  tcMiiStationRead(phyaddr, 30));
		}else
			printf(" phyaddr = 8~11 only \r\n");
	}
	else
	{
		printf("Usage: forceLDPS <phyaddr> <on|off> \r\n");
	}
	tcMiiStationWrite(phyaddr, 31, 0x0000);
	return 0;
}

int doPhyset7530_15R(int argc, char *argv[], void *p)	// allen_20160428
{				
	uint16 	phyaddr = 0;

	if((stricmp(argv[1], "on") == 0))
	{
	   	for(phyaddr = 0; phyaddr<=4; phyaddr ++)
   		{
			//mtEMiiRegWrite(phyaddr, 0x1f, 0x108, 0xd010);		// 15R match
			//mtEMiiRegWrite(phyaddr, 0x1e,  0x45, 0x0706);		// R offset
			//mtEMiiRegWrite(phyaddr, 0x1e,  0x46, 0x0707);		// R offset
			//mtEMiiRegWrite(phyaddr, 0x1f,  0x44, 0x0000);		// for 1000 diff A&B

			// === TX === (IOT with test center)
			mtEMiiRegWrite(phyaddr, 0x1e,  0x5c, 0x5555);
			// === RX === (IOT with test center)	
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA",  0x0, 0x2b, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x16, 0x16801c, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x18, 0xe2e00, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "DSPF", 0x3, 0x8, 0);
			tcMiiStationWrite(phyaddr, 0x1f, 0x2a30); // test page
			tcMiiStationWrite(phyaddr, 0x1c, 0x14e5);
			tcMiiStationWrite(phyaddr, 0x1f, 0x0002); // misc page
			tcMiiStationWrite(phyaddr, 0x17, 0x11ff); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x0001); // ext page
			tcMiiStationWrite(phyaddr, 0x14, 0x3a45); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x8000);
			
   		}
   		printf(" 7530 set 15R \r\n");
   		mt7530_15R_flag = 1; 
	}
	else
	{
		for(phyaddr = 0; phyaddr<=4; phyaddr ++)
   		{
			mtEMiiRegWrite(phyaddr, 0x1f, 0x108, 0x1010);		
			mtEMiiRegWrite(phyaddr, 0x1e,  0x45, 0x0404);	
			mtEMiiRegWrite(phyaddr, 0x1e,  0x46, 0x0404);	
			mtEMiiRegWrite(phyaddr, 0x1f,  0x44, 0x00a0);	

			// === TX ===
			mtEMiiRegWrite(phyaddr, 0x1e,  0x5c, 0x7777);
			// === RX === 	
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA",  0x0, 0x2a, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x16, 0x0f801c, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x18, 0xe2f00, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "DSPF", 0x3, 0x0, 0);
			tcMiiStationWrite(phyaddr, 0x1f, 0x2a30); // test page
			tcMiiStationWrite(phyaddr, 0x1c, 0x14d5);
			tcMiiStationWrite(phyaddr, 0x1f, 0x0002); // misc page
			tcMiiStationWrite(phyaddr, 0x17, 0x01ff); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x0001); // ext page
			tcMiiStationWrite(phyaddr, 0x14, 0x3a55); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x8000);

   		}
   		printf(" 7530 set 0R \r\n");
   		mt7530_15R_flag = 0; 
	}
    return 0;
}

int doPhyset7530_5R(int argc, char *argv[], void *p)	// allen_20170202
{				
	uint16 	phyaddr = 0;

	if((stricmp(argv[1], "on") == 0))
	{
	   	for(phyaddr = 0; phyaddr<=4; phyaddr ++)
   		{
			mtEMiiRegWrite(phyaddr, 0x1f, 0x108, 0x5010);		// 5R match
			mtEMiiRegWrite(phyaddr, 0x1f,  0x44, 0x0000);		// for 1000 diff A&B
			mtEMiiRegWrite(phyaddr, 0x1e,  0x16, 0x0012);		// tx i2mpb_tst	// allen_20170331 for FH OEM/ODM // allen_20170405	
			mtEMiiRegWrite(phyaddr, 0x1e,  0x45, 0x0706);		// R offset
			mtEMiiRegWrite(phyaddr, 0x1e,  0x46, 0x0707);		// R offset

			mtEMiiRegWrite(phyaddr, 0x1e,  0x12, 0x71d0);		// for 100 baseTX 
			// === TX === (IOT with test center)
			mtEMiiRegWrite(phyaddr, 0x1e,  0x5c, 0x7777);
			// === RX === (IOT with test center)	
			//mtPhyMiiWrite_TrDbg(phyaddr, "PMA",  0x0, 0x2b, 0);
			//mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x16, 0x16801c, 0);
			//mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x18, 0xe2e00, 0);
			//mtPhyMiiWrite_TrDbg(phyaddr, "DSPF", 0x3, 0x8, 0);
			//tcMiiStationWrite(phyaddr, 0x1f, 0x2a30); // test page
			//tcMiiStationWrite(phyaddr, 0x1c, 0x14e5);
			//tcMiiStationWrite(phyaddr, 0x1f, 0x0002); // misc page
			//tcMiiStationWrite(phyaddr, 0x17, 0x11ff); 
			//tcMiiStationWrite(phyaddr, 0x1f, 0x0001); // ext page
			//tcMiiStationWrite(phyaddr, 0x14, 0x3a45); 
			//tcMiiStationWrite(phyaddr, 0x1f, 0x8000);
			
   		}
   		printf(" 7530 set 5R \r\n");
   		mt7530_5R_flag = 1; 
	}
	else if((stricmp(argv[1], "off") == 0))
	{
		for(phyaddr = 0; phyaddr<=4; phyaddr ++)
   		{
			mtEMiiRegWrite(phyaddr, 0x1f, 0x108, 0x1010);	
			mtEMiiRegWrite(phyaddr, 0x1f,  0x44, 0x00a0);
			mtEMiiRegWrite(phyaddr, 0x1e,  0x45, 0x0404);	
			mtEMiiRegWrite(phyaddr, 0x1e,  0x46, 0x0404);		

			mtEMiiRegWrite(phyaddr, 0x1e,  0x12, 0x7210);		// for 100 baseTX
			// === TX ===
			mtEMiiRegWrite(phyaddr, 0x1e,  0x5c, 0x7777);
			// === RX === 	
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA",  0x0, 0x2a, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x16, 0x0f801c, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x18, 0xe2f00, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "DSPF", 0x3, 0x0, 0);
			tcMiiStationWrite(phyaddr, 0x1f, 0x2a30); // test page
			tcMiiStationWrite(phyaddr, 0x1c, 0x14d5);
			tcMiiStationWrite(phyaddr, 0x1f, 0x0002); // misc page
			tcMiiStationWrite(phyaddr, 0x17, 0x01ff); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x0001); // ext page
			tcMiiStationWrite(phyaddr, 0x14, 0x3a55); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x8000);

   		}
   		printf(" 7530 set 0R \r\n");
   		mt7530_5R_flag = 0; 
	}
	else
	{
		#if debug_flag
		printf(" 1f_108 = 0x%x \r\n", mtEMiiRegRead(0, 0x1f, 0x108));
		#endif
	}
    return 0;
}


int doPhyset751221_5R(int argc, char *argv[], void *p)	// allen_20170621
{				
	uint16 	phyaddr = 0, port_num = 0;
	uint16 	reg0_temp = 0, dev1e_145_temp = 0;

	#if 1
	//tcMiiStationWrite(12, 0x0, 0x0800);
	pause(1000);
	//tcMiiStationWrite(12, 0x0, 0x3100);
	
	tcPhyVerLookUp(12);
	ePhyInit(12);
	GECal_flag = 0;

	tcMiiStationWrite(phyaddr, 0x1f, 0x0000);		// g0 
	reg0_temp = tcMiiStationRead(phyaddr, 0x0);		// keep the default value
	tcMiiStationWrite(phyaddr, 0x0,  0x0140);		// set [12]AN disable, [8]full duplex, [13/6]1000Mbps

	mtEMiiRegWrite(phyaddr, 0x1f, 0x0100, 0xc000);			// BG voltage output
	dev1e_145_temp = mtEMiiRegRead(phyaddr, 0x1e, 0x0145);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0145, 0x1010);			// fix mdi
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0185, 0x0000);			// disable tx slew control
	#endif
	
	if((stricmp(argv[1], "on") == 0))
	{
		//doGePhyALLAnalogCal_R45(12);

   		//if(GECal_status == 0)
   		//	doGePhyALLAnalogCal_R45(12);
   			
	   	for(port_num = 0; port_num<=3; port_num ++)
   		{
			//FECal_R45(port_num, 20); 	
   		}
   		
		//tcMiiStationWrite(12, 0x0, 0x8000);
		pause(1000);
		//tcMiiStationWrite(12, 0x0, 0x1200);
	
		tcPhyVerLookUp(12);
		ePhyInit(12);	
   		
   		printf(" 751221 set 5R \r\n");
   		mt751221_5R_flag = 1; 
	}
	else if((stricmp(argv[1], "off") == 0))
	{
		doGePhyALLAnalogCal(12);

   		if(GECal_status == 0)
   			doGePhyALLAnalogCal(12);

		for(port_num = 0; port_num<=3; port_num ++)
   		{
			//FECal_R50(port_num, 20);
   		}
   		
   		//tcMiiStationWrite(12, 0x0, 0x8000);
		pause(1000);
		//tcMiiStationWrite(12, 0x0, 0x1200);
	
		tcPhyVerLookUp(12);
		ePhyInit(12);	
   		
   		printf(" 751221 set 0R \r\n");
   		mt751221_5R_flag = 0; 
	}
	else
	{
		#if debug_flag
		printf(" 1f_108 = 0x%x \r\n", mtEMiiRegRead(12, 0x1f, 0x108));
		#endif
	}
    return 0;
}


int doPhyset7530_15R_L(int argc, char *argv[], void *p)	// 20161129 for Lecroy scope
{				
	uint16 	phyaddr = 0;

	if((stricmp(argv[1], "on") == 0))
	{
	   	for(phyaddr = 0; phyaddr<=4; phyaddr ++)
   		{
			mtEMiiRegWrite(phyaddr, 0x1f, 0x108, 0xd010);		// 15R match
			mtEMiiRegWrite(phyaddr, 0x1e,  0x45, 0x0605);		// R offset
			mtEMiiRegWrite(phyaddr, 0x1e,  0x46, 0x0605);		// R offset
			//mtEMiiRegWrite(phyaddr, 0x1f,  0x44, 0x0080);		// for 1000 diff A&B

			// === TX === (IOT with test center)
			mtEMiiRegWrite(phyaddr, 0x1e,  0x5c, 0x5555);
			// === RX === (IOT with test center)	
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA",  0x0, 0x2b, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x16, 0x16801c, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x18, 0xe2e00, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "DSPF", 0x3, 0x8, 0);
			tcMiiStationWrite(phyaddr, 0x1f, 0x2a30); // test page
			tcMiiStationWrite(phyaddr, 0x1c, 0x14e5);
			tcMiiStationWrite(phyaddr, 0x1f, 0x0002); // misc page
			tcMiiStationWrite(phyaddr, 0x17, 0x11ff); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x0001); // ext page
			tcMiiStationWrite(phyaddr, 0x14, 0x3a45); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x8000);
   		}
   		printf(" 7530 set 15R_L \r\n");
   		mt7530_15R_L_flag = 1;
	}
	else
	{
		for(phyaddr = 0; phyaddr<=4; phyaddr ++)
   		{
			mtEMiiRegWrite(phyaddr, 0x1f, 0x108, 0x1010);		
			mtEMiiRegWrite(phyaddr, 0x1e,  0x45, 0x0404);	
			mtEMiiRegWrite(phyaddr, 0x1e,  0x46, 0x0404);	
			mtEMiiRegWrite(phyaddr, 0x1f,  0x44, 0x00a0);	

			// === TX ===
			mtEMiiRegWrite(phyaddr, 0x1e,  0x5c, 0x7777);
			// === RX === 	
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA",  0x0, 0x2a, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x16, 0x0f801c, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "PMA", 0x18, 0xe2f00, 0);
			mtPhyMiiWrite_TrDbg(phyaddr, "DSPF", 0x3, 0x0, 0);
			tcMiiStationWrite(phyaddr, 0x1f, 0x2a30); // test page
			tcMiiStationWrite(phyaddr, 0x1c, 0x14d5);
			tcMiiStationWrite(phyaddr, 0x1f, 0x0002); // misc page
			tcMiiStationWrite(phyaddr, 0x17, 0x01ff); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x0001); // ext page
			tcMiiStationWrite(phyaddr, 0x14, 0x3a55); 
			tcMiiStationWrite(phyaddr, 0x1f, 0x8000);
   		}
   		printf(" 7530 set 0R \r\n");
   		mt7530_15R_L_flag = 0;
	}
    return 0;
}

int doPhyset7530_0R_L(int argc, char *argv[], void *p)	// 20161226 for Lecroy scope
{				
	uint16 	phyaddr = 0;

	if((stricmp(argv[1], "on") == 0))
	{
	   	//for(phyaddr = 0; phyaddr<=4; phyaddr ++)
   		//{

   		//}
   		printf(" 7530 set 0R_L \r\n");
   		mt7530_0R_L_flag = 1;
	}
	else
	{
		//for(phyaddr = 0; phyaddr<=4; phyaddr ++)
   		//{

   		//}
   		printf(" 7530 set 0R \r\n");
   		mt7530_0R_L_flag = 0;
	}
    return 0;
}


int doPhyForceLED(int argc, char *argv[], void *p) 	// allen_20160630 
{
	uint16 port_num = 0, phyaddr = 0;
	uint32 reg3 = 0, r15_temp = 0, dev_1f_24_temp = 0, dev_1f_24 = 0;
	
	phyaddr = checked_atoi(argv[1]);
	reg3 = tcMiiStationRead(phyaddr, 3);
	r15_temp = tcMiiStationRead(phyaddr, 15);

	if(reg3==EPHY_ID_7512Ge) // 7512GE
	{
		tcPhyVer = tcPhyVer_7512Ge;
	}
	else if((reg3 == EPHY_ID_7530Ge_7512Fe)&&(((r15_temp>>13)&0x1) == 1)) // 7530GE
	{
		tcPhyVer = mtPhyVer_7530;
	}
	else if(reg3==EPHY_ID_7522Ge)
	{
		tcPhyVer = tcPhyVer_7522Ge;
	}
	else if(reg3==EPHY_ID_7522Fe)
	{
		tcPhyVer = tcPhyVer_7522Fe;
	}
	else if(reg3==EPHY_ID_7523Ge)
	{
		tcPhyVer = tcPhyVer_7523Ge;
	}
	else
	{
		tcPhyVer = tcPhyVer_7512Fe;
	}

	//printf("(%d) tcPhyVer = %x \r\n", phyaddr, tcPhyVer);

	if(tcPhyVer == mtPhyVer_7530)
	{
		dev_1f_24_temp = mtEMiiRegRead(phyaddr, 0x1f, 0x24);
		//printf("1f_24 = 0x%x \r\n", dev_1f_24_temp);
		
		if((stricmp(argv[2], "enable") == 0))
		{
			dev_1f_24 = (dev_1f_24_temp | (0x1<<15));		
			mtEMiiRegWrite(phyaddr, 0x1f, 0x24, dev_1f_24);
			//printf("1f_24 = 0x%x \r\n", dev_1f_24);
		}
		else if((stricmp(argv[2], "disable") == 0))
		{
			dev_1f_24 = (dev_1f_24_temp & 0x7fff);		
			mtEMiiRegWrite(phyaddr, 0x1f, 0x24, dev_1f_24);
			//printf("1f_24 = 0x%x \r\n", dev_1f_24);
		}
		else
		{
			printf("Usage: forceLED <phyaddr> <enable|disable> \r\n");
		}
	}
	return 0;
}	




/************************************************************************
    Funtcion:       mdio_cl22_write
    Description:    
    Calls:
    Called by:      trgmii setting
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void mdio_cl22_write(uint32 port_num,uint32 dev_num,uint32 reg_num,uint32 reg_data)
{
    const uint16 MMD_Control_register=0xD;
    const uint16 MMD_addr_data_register=0xE;
  	const uint16 page_reg=31;


	tcMiiStationWrite(port_num, page_reg, 0x00); //switch to main page
	tcMiiStationWrite(port_num, MMD_Control_register, (0<<14)+dev_num);
	tcMiiStationWrite(port_num, MMD_addr_data_register, reg_num);
	tcMiiStationWrite(port_num, MMD_Control_register, (1<<14)+dev_num);
	tcMiiStationWrite(port_num, MMD_addr_data_register, reg_data);
//	printf("* doPhyMMDWrite_CL22=> phyaddr=%d, dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", port_num, dev_num, reg_num, reg_data);
}
//** MTK120625 start,
static spinlock_t pbus_lock;
/* frank modify for rt62806 */
uint32
gswPbusRead(uint32 pbus_addr)
{
	uint32 pbus_data;

	uint32 phyaddr;
	uint32 reg;
	uint32 value;

	spin_lock_bh(&pbus_lock);

	phyaddr = 31;
	// 1. write high-bit page address
	reg = 31;
	value = (pbus_addr >> 6);
  	tcMiiStationWrite(phyaddr, reg, value);
	//mdelay(5);
	//DBG_PRINTF("1. miiw phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);
	//printk("1. miiw phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);

	// 2. read low DWord
	reg = (pbus_addr>>2) & 0x000f;
	value = tcMiiStationRead(phyaddr, reg);
	//mdelay(5);
	//DBG_PRINTF("2. miir phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);
	//printk("2. miir phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);
	pbus_data = value;

	// 3. read high DWord
	reg = 16;
		value = tcMiiStationRead(phyaddr, reg);
	//mdelay(5);
	//DBG_PRINTF("3. miir phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);
	//printk("3. miir phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);

	pbus_data = (pbus_data) | (value<<16);

	//DBG_PRINTF("# pbus read addr=0x%04x data=0x%04x_%04x\r\n", (pbus_addr&0xfffc), (pbus_data>>16), (pbus_data&0xffff));
	//printk("# pbus read addr=0x%04x data=0x%04x_%04x\r\n", (pbus_addr&0xfffc), (pbus_data>>16), (pbus_data&0xffff));
//	printk("gswPbusRead read data:\n");

//	printk("pbus_data: %x\n", pbus_data);

	spin_unlock_bh(&pbus_lock);
	return pbus_data;
} /* end frank modify for rt62806 */

/* frank modify for rt62806 */
int
gswPbusWrite(uint32 pbus_addr, uint32 pbus_data)
{
	uint32 phyaddr;
	uint32 reg;
	uint32 value;

	spin_lock_bh(&pbus_lock);

	phyaddr = 31;

	// 1. write high-bit page address
	reg = 31;
	value = (pbus_addr >> 6);
	tcMiiStationWrite(phyaddr, reg, value);
	//mdelay(5);

	//printk("1. miiw phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);
	//DBG_PRINTF("1. miiw phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);

	// 2. write low DWord
	reg = (pbus_addr>>2) & 0x000f;
	value = pbus_data & 0xffff;
	tcMiiStationWrite(phyaddr, reg, value);
	//mdelay(5);
	//printk("2. miiw phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);
	//DBG_PRINTF("2. miiw phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);

	// 3. write high DWord
	reg = 16;
	value = (pbus_data>>16) & 0xffff;
	tcMiiStationWrite(phyaddr, reg, value);
	//mdelay(5);
	//printk("3. miiw phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);
	//DBG_PRINTF("3. miiw phyaddr=%2d reg=%2d value=%04x\r\n", phyaddr, reg, value);

	//DBG_PRINTF("# pbus write addr=0x%04x data=0x%04x_%04x\r\n", (pbus_addr&0xfffc), (pbus_data>>16),(pbus_data&0xffff));
	spin_unlock_bh(&pbus_lock);
  	return 0;
} /* end frank modify for rt62806 */
/*___________________________________________________________________
**      function name: doTrgmiiRxCal
**      descriptions:
**       Let TRGMII interface in training mode. Calibrate optimal Rx delay for each        
**       Rx pin. After calibration, leave training mode.
**     
**      parameters:
**         
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. YMC 2017/06/07
**___________________________________________________________________
*/
#define EXTERNAL 1
#define INTERNAL 0
#define SOC_TO_EXT 0
#define EXT_TO_SOC 1
#define HEAD_55    1
#define TAIL_55    2
#define GSW_BASE		0xBFB58000
//F U N C T I O N   D E F I N I T I O N S
//#define read_reg_word(reg) 		VPint(reg)
//#if debug_flag
//#define write_reg_word(reg, wdata) 	VPint(reg)=wdata
//#endif

#if 0
uint32 macMT7530ReadReg2(uint32 gsw_reg, uint8 external)
{
    if(external){
        return gswPbusRead(gsw_reg);
    }else{
        return read_reg_word(GSW_BASE + gsw_reg);
    }
}

int macMT7530WriteReg2(uint32 gsw_reg, uint32 gsw_data, uint8 external)
{
    if(external){
        return gswPbusWrite(gsw_reg, gsw_data);
    }else{
    #if debug_flag
        write_reg_word((GSW_BASE + gsw_reg), gsw_data);
    #endif    
        return 0;
    }
}

int ErrChk(u32 RegAddr, u8 RxSwitch){
	uint32 ErrCnt, RegVal;

	ErrCnt = 0;

	RegVal = macMT7530ReadReg2(RegAddr, RxSwitch);
	
	RegVal = RegVal | 0x40000000;
	macMT7530WriteReg2(RegAddr, RegVal, RxSwitch);

	RegVal = RegVal & 0xBFFFFFFF;
	macMT7530WriteReg2(RegAddr, RegVal, RxSwitch);

	ErrCnt = macMT7530ReadReg2(RegAddr, RxSwitch);
	ErrCnt = (ErrCnt & 0x00000F00) >> 8;

	return ErrCnt;
}

typedef struct _RX_CH_Window
{
    int head_cur;
    int	tail_cur;
    int head_best;
    int tail_best;
} RX_CH_Window;

void TrgmiiRxCal_RxRst(void)
{
	uint32 RxChannel, RxRegAddr, Regvalue;
	uint8 RxSwitch=0;
	for(RxSwitch=INTERNAL;RxSwitch <= EXTERNAL;RxSwitch++){
		//set clk delay to 0.
		Regvalue = macMT7530ReadReg2(0x7A00, RxSwitch);
    	Regvalue = Regvalue & 0xFFFFFF80;
		macMT7530WriteReg2(0x7A00, Regvalue, RxSwitch);
		for(RxChannel = 0 ; RxChannel <= 4 ; RxChannel++){
			switch(RxChannel){
    	        case 0:
    	            RxRegAddr = 0x7A10;
    	            break;
    	        case 1:
    	            RxRegAddr = 0x7A18;
    	            break;	            
    	        case 2:
    	            RxRegAddr = 0x7A20;
    	            break;
    	        case 3:
    	            RxRegAddr = 0x7A28;
    	            break;
    	        default:
    	            RxRegAddr = 0x7A30;
    	            break;
    	    }
			//set RxD delay to 0.
			Regvalue = macMT7530ReadReg2(RxRegAddr, RxSwitch);
		    Regvalue = Regvalue & 0xFFFFFF80;
		    macMT7530WriteReg2(RxRegAddr, Regvalue, RxSwitch);
		}
	}
}
#endif

#if 0 //debug_flag
static int doTrgmiiRxCal(void)
{
	uint32 RxChannel, DAC, RxRegAddr, TxRegAddrl, TxRegAddr2, Regvalue, ErrCnt, Val;
    uint8 TxSwitch=0, RxSwitch=0, direction=0, state;
    uint16 delay_tap=0;
	RX_CH_Window	RX_CH_Window_55[5];
	RxChannel = 0;
	DAC = 0;

	TrgmiiRxCal_RxRst();
	for(TxSwitch=INTERNAL;TxSwitch <= EXTERNAL;TxSwitch++)
	{
		//Tx training mode enable
		TxRegAddrl = 0x7A40;
		Regvalue = macMT7530ReadReg2(TxRegAddrl, TxSwitch);
		Regvalue = Regvalue | 0x80000000;
		macMT7530WriteReg2(TxRegAddrl, Regvalue, TxSwitch);
		for(RxChannel = 0 ; RxChannel <= 4 ; RxChannel++)
		{
			TxRegAddr2 = 0x7A50 + 0x8*RxChannel;
			//All Channel transmits 0xAA
			Regvalue = macMT7530ReadReg2(TxRegAddr2,TxSwitch);
			Regvalue = (Regvalue & 0xFFFFFF00) | 0xAA;
			macMT7530WriteReg2(TxRegAddr2, Regvalue, TxSwitch);
		}
	}
    for (direction=0;direction < 2;direction++){
        printk("Direction %s \n",direction?"EXT_TO_SOC":"SOC_TO_EXT");     
        if(direction == SOC_TO_EXT){
            TxSwitch = INTERNAL;
            RxSwitch = EXTERNAL;
        }else{
            TxSwitch = EXTERNAL;
            RxSwitch = INTERNAL;
        }
    	//Scan
    	for (RxChannel = 0 ; RxChannel <= 4 ; RxChannel++) {
    	    switch(RxChannel){
    	        case 0:
    	            RxRegAddr = 0x7A10;
    				TxRegAddr2 = 0x7A50;
    	            break;
    	        case 1:
    	            RxRegAddr = 0x7A18;
    				TxRegAddr2 = 0x7A58;
    	            break;	            
    	        case 2:
    	            RxRegAddr = 0x7A20;
    				TxRegAddr2 = 0x7A60;
    	            break;
    	        case 3:
    	            RxRegAddr = 0x7A28;
    				TxRegAddr2 = 0x7A68;
    	            break;
    	        default:
    	            RxRegAddr = 0x7A30;
    				TxRegAddr2 = 0x7A70;
    	            break;
    	    }
			//Test pattern=0x55 on test channel.
			Regvalue = macMT7530ReadReg2(TxRegAddr2,TxSwitch);
    		Regvalue = (Regvalue & 0xFFFFFF00) | 0x55;
    		macMT7530WriteReg2(TxRegAddr2, Regvalue, TxSwitch);
			
			//clear variable
			RX_CH_Window_55[RxChannel].head_cur=0;
			RX_CH_Window_55[RxChannel].head_best=0;
			RX_CH_Window_55[RxChannel].tail_cur=0;
			RX_CH_Window_55[RxChannel].tail_best=0;
			//delay RxD to find pass window
            state = HEAD_55;
            for (DAC = 0 ; DAC <= 63 ; DAC++) {
                ErrCnt = 0;
    			Regvalue = macMT7530ReadReg2(RxRegAddr, RxSwitch);
    			Regvalue = Regvalue & 0xFFFFFF80;
    			Regvalue = Regvalue | DAC;
    			macMT7530WriteReg2(RxRegAddr, Regvalue, RxSwitch);
    			ErrCnt = ErrChk(RxRegAddr, RxSwitch);
				Regvalue = macMT7530ReadReg2(RxRegAddr, RxSwitch);
    			Val = (Regvalue & 0x00FF0000) >> 16;
    			//printk("RxChannel=%2d, DAC=0x%2x, Val=0x%2x, ErrCnt=0x%2x \n", 
    			    //RxChannel, DAC, Val, ErrCnt);
			    switch(state){
			        case HEAD_55:
			            if((Val == 0x55) && (ErrCnt == 0)){
			                RX_CH_Window_55[RxChannel].head_cur=DAC;
			                state = TAIL_55;
			            }
			            break;
			        case TAIL_55:
			            if((Val == 0x55) && (ErrCnt == 0)){
			                RX_CH_Window_55[RxChannel].tail_cur=DAC;			                
			            }else{
			                state = HEAD_55;
			            }
			            break;
			        default:
			            break;
                }
				if ((RX_CH_Window_55[RxChannel].tail_cur - RX_CH_Window_55[RxChannel].head_cur) > 
					(RX_CH_Window_55[RxChannel].tail_best - RX_CH_Window_55[RxChannel].head_best))
				{
					RX_CH_Window_55[RxChannel].head_best = RX_CH_Window_55[RxChannel].head_cur;
					RX_CH_Window_55[RxChannel].tail_best = RX_CH_Window_55[RxChannel].tail_cur;
				}
				if(RX_CH_Window_55[RxChannel].head_best != 0 && RX_CH_Window_55[RxChannel].tail_best != 0){
					if((Val == 0xAA) && (ErrCnt == 0))
						break;
				}
    		}
			//revert Tx pattern to 0xAA
			Regvalue = macMT7530ReadReg2(TxRegAddr2,TxSwitch);
			Regvalue = (Regvalue & 0xFFFFFF00) | 0xAA;
			macMT7530WriteReg2(TxRegAddr2, Regvalue, TxSwitch);
    	}
		
		//find optimal delay per RxD
		for(RxChannel = 0 ; RxChannel <= 4 ; RxChannel++){
			switch(RxChannel){
    	        case 0:
    	            RxRegAddr = 0x7A10;
    	            break;
    	        case 1:
    	            RxRegAddr = 0x7A18;
    	            break;	            
    	        case 2:
    	            RxRegAddr = 0x7A20;
    	            break;
    	        case 3:
    	            RxRegAddr = 0x7A28;
    	            break;
    	        default:
    	            RxRegAddr = 0x7A30;
    	            break;
    	    }
			Regvalue = macMT7530ReadReg2(RxRegAddr, RxSwitch);
        	Regvalue = Regvalue & 0xFFFFFF80;
	    	if((RX_CH_Window_55[RxChannel].head_best < RX_CH_Window_55[RxChannel].tail_best)){	
				delay_tap = ((RX_CH_Window_55[RxChannel].head_best + RX_CH_Window_55[RxChannel].tail_best)>>1);
				#if debug_flag
				printk("RxChannel=%2d,delay windows 55: 0x%2X ~ 0x%2X, optimal delay: 0x%2X\n",
                    RxChannel, RX_CH_Window_55[RxChannel].head_best, RX_CH_Window_55[RxChannel].tail_best, delay_tap);
				#endif
	    	}else{
	    		   delay_tap = 0;
	    		   #if debug_flag
	    		   printk("RxChannel=%2d, delay windows fail, set to default \n",RxChannel);
				   #endif
	    	}
           Regvalue = Regvalue | delay_tap;
           macMT7530WriteReg2(RxRegAddr, Regvalue, RxSwitch);
    	}
    }
	for(TxSwitch=INTERNAL;TxSwitch <= EXTERNAL;TxSwitch++)
	{
	    //Tx training mode disable
	    TxRegAddrl = 0x7A40;
	    Regvalue = macMT7530ReadReg2(TxRegAddrl, TxSwitch);
	    Regvalue = Regvalue & ~0x80000000;
	    macMT7530WriteReg2(TxRegAddrl, Regvalue, TxSwitch);
	}
	
	return 0;
}
#endif

#if 0 //debug_flag
/*___________________________________________________________________
**      function name: doTrgmiiPhyInit
**      descriptions:
**       TRGMII phy parameter init. and Rx delay calibration.      
**            
**      parameters:
**         
**      global:
**
**      return:
**         -1:fail
**         0:succuces
**
**      call:
**
**      revision:
**      1. YMC 2017/06/07
**___________________________________________________________________
*/
static int doTrgmiiPhyInit(void)
{
	uint32 reg;
	//Deduce MT7530 P6 Tx Driving Strength	
	gswPbusWrite(0x7a54, 0xbb);
	gswPbusWrite(0x7a5c, 0xbb);
	gswPbusWrite(0x7a64, 0xbb);
	gswPbusWrite(0x7a6c, 0xbb);
	gswPbusWrite(0x7a74, 0xbb);
	gswPbusWrite(0x7a7c, 0xff);
	#if debug_flag
	write_reg_word(GSW_BASE + 0x7a54, 0x88);
	write_reg_word(GSW_BASE + 0x7a5c, 0x88);
	write_reg_word(GSW_BASE + 0x7a64, 0x88);
	write_reg_word(GSW_BASE + 0x7a6c, 0x88);
	write_reg_word(GSW_BASE + 0x7a74, 0x88);
	write_reg_word(GSW_BASE + 0x7a7c, 0x77);
    #endif
	//Set MT7530 P6 TxD delay to 0	
	gswPbusWrite(0x7a50, 0x55);
	gswPbusWrite(0x7a58, 0x55);
	gswPbusWrite(0x7a60, 0x55);
	gswPbusWrite(0x7a68, 0x55);
	gswPbusWrite(0x7a70, 0x55);
	
	//SOC ODT
    reg = read_reg_word(GSW_BASE + 0x7a04);
    #if debug_flag
    write_reg_word(GSW_BASE + 0x7a04, reg | 0x3020000);

    write_reg_word(GSW_BASE + 0x7a14, 0x3227700);
	write_reg_word(GSW_BASE + 0x7a1C, 0x3227700);
	write_reg_word(GSW_BASE + 0x7a24, 0x3227700);
	write_reg_word(GSW_BASE + 0x7a2C, 0x3227700);
	write_reg_word(GSW_BASE + 0x7a34, 0x3227700);
    #endif
    // SSC off
    mdio_cl22_write(0,0x1f,0x407,0x0301);

	//doTrgmiiRxCal();
	
	//FW version
	//printk("EN7512 TRGMII V2.0\n"); 
	return 0;
}
#endif


int doPhyMMDRead(int argc, char *argv[], void *p)
{
#if debug_flag

    #ifndef CL45_CMD_SUPPORT
    const uint16 MMD_Control_register=0xD;
    const uint16 MMD_addr_data_register=0xE;
  	const uint16 page_reg=31;
    #endif
	uint32 dev_addr=0;
	uint32 phyaddr=0;
	uint32 reg_addr=0;
	uint32 start_addr=0;
	uint32 end_addr=0;
	uint32 value=0;
	int i=0;

	if (argc!=4 && argc!=6)
	{
		printf("Usage: emiir all <phyaddr> <devaddr> <s_addr> <e_addr>\r\n");
		printf("       emiir <phyaddr> <devaddr> <reg>\r\n");
		return 0;			
	}
	
	if (strcasecmp(argv[1], "all") == 0)
	{ 	/*Post Read*/
		phyaddr = checked_atoi(argv[2]);//0~31
		sscanf(argv[3], "%lx", &dev_addr);//hex
		sscanf(argv[4], "%lx", &start_addr);//hex
		sscanf(argv[5], "%lx", &end_addr);//hex
		if((start_addr>65535)  ||(end_addr>65535))
		{
			printk("s_addr or e_addr must be less than 65536\r\n");
			return 0;
		}
		for(i=start_addr; i<=end_addr; i++)
		{
			#ifdef CL45_CMD_SUPPORT
			value = mtEMiiRegRead(phyaddr, dev_addr, i);
			printf("* doPhyMMDRead =>phyaddr=%d,  dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", phyaddr, dev_addr,  i, value);
			#else
			tcMiiStationWrite(phyaddr, page_reg, 0x00); 					//switch to main page
			tcMiiStationWrite(phyaddr, MMD_Control_register, (0<<14)+dev_addr);
			tcMiiStationWrite(phyaddr, MMD_addr_data_register, i);
			tcMiiStationWrite(phyaddr, MMD_Control_register, (1<<14)+dev_addr);
			value = tcMiiStationRead(phyaddr, MMD_addr_data_register);
			printf("* doPhyMMDRead_CL22 =>phyaddr=%d,  dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", phyaddr, dev_addr,  i, value);
			#endif
		}
		printk("\r\n");
	}
    else
	{
		// tce emiir <DevAddr> <PhyAddr> <regAddr>
		phyaddr = checked_atoi(argv[1]);//0~31
		//dev_addr = checked_atoi(argv[2]);//0~31
		sscanf(argv[2], "%lx", &dev_addr);//hex
		sscanf(argv[3], "%lx", &reg_addr);//hex
		#ifdef CL45_CMD_SUPPORT
		value = mtEMiiRegRead(phyaddr, dev_addr, reg_addr);
		printf("* doPhyMMDRead =>phyaddr=%d,  dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", phyaddr, dev_addr, reg_addr, value);
		#else
		tcMiiStationWrite(phyaddr, page_reg, 0x00); 						//switch to main page
		tcMiiStationWrite(phyaddr, MMD_Control_register, (0<<14)+dev_addr);
		tcMiiStationWrite(phyaddr, MMD_addr_data_register, reg_addr);
		tcMiiStationWrite(phyaddr, MMD_Control_register, (1<<14)+dev_addr);
		value = tcMiiStationRead(phyaddr, MMD_addr_data_register);
		printf("* doPhyMMDRead_CL22=>phyaddr=%d,  dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", phyaddr, dev_addr, reg_addr, value);
		#endif
		return 0;
    }
    #endif
    
    return 0;           
}

int doPhyMMDWrite(int argc, char *argv[], void *p) 	// cl45
{
#if debug_flag
    #ifndef CL45_CMD_SUPPORT
    const uint16 MMD_Control_register=0xD;
    const uint16 MMD_addr_data_register=0xE;
    const uint16 page_reg=31;
    #endif
	uint32 dev_addr=0;
	uint32 phyaddr=0;
	uint32 reg_addr=0;
	uint32 value=0;
	uint16 STBit=0;
	uint16 BFlen=0;
	uint16 BF,BFMsk=0;
	uint32 RValue =0;
	uint8 i=0;

	if(argc==5)
	{	// tce emiiw <DevAddr> <PhyAddr> <regAddr> <data>
		phyaddr = checked_atoi(argv[1]);//0~31
		//dev_addr = checked_atoi(argv[2]);//0~31
		#if debug_flag
		sscanf(argv[2], "%lx", &dev_addr);//hex
		sscanf(argv[3], "%lx", &reg_addr);//hex
		sscanf(argv[4], "%lx", &value);//hex
		#endif
    }
	else if(argc==7)
	{
		if(stricmp(argv[0], "emiiwb") == 0)
		{
			phyaddr = checked_atoi(argv[1]);
			//reg = checked_atoi(argv[2]);
			#if debug_flag
			sscanf(argv[2], "%lx", &dev_addr);
			sscanf(argv[3], "%lx", &reg_addr);
			STBit = checked_atoi(argv[4]);
			BFlen = checked_atoi(argv[5]);
			sscanf(argv[6], "%lx", &value);
			#endif
			//printf("* Phyaddr=%d, DevAddr=0x%02lX,RegAddr=0x%02lX, STBit=%0d, BFlen=%0d, value=0x%04lX\r\n", phyaddr,dev_addr, reg_addr, STBit, BFlen, value);		
		}
	}
#if debug_flag

 	if(((argc == 5) || (argc == 7)) && (phyaddr<=0x1f))
 	{
 		if(argc == 5)
 		{     	
        	#ifdef CL45_CMD_SUPPORT
			mtEMiiRegWrite(phyaddr, dev_addr,reg_addr,value);
			printf("* doPhyMMDWrite => phyaddr=%d, dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", phyaddr, dev_addr, reg_addr, value);
        	#else
			tcMiiStationWrite(phyaddr, page_reg, 0x00); 						//switch to main page
			tcMiiStationWrite(phyaddr, MMD_Control_register, (0<<14)+dev_addr);
			tcMiiStationWrite(phyaddr, MMD_addr_data_register, reg_addr);
			tcMiiStationWrite(phyaddr, MMD_Control_register, (1<<14)+dev_addr);
			tcMiiStationWrite(phyaddr, MMD_addr_data_register, value);
			printf("* doPhyMMDWrite_CL22=> phyaddr=%d, dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", phyaddr, dev_addr, reg_addr, value);
        	#endif
		}
 		else if(((argc == 7) && (stricmp(argv[0], "emiiwb")) == 0))
 		{
			RValue = mtEMiiRegRead(phyaddr,dev_addr, reg_addr);
			for(i=0; i<BFlen; i++)
			{
				BF=1;
				BF=BF<<(STBit+i);
				BFMsk = BFMsk | BF;
				//printf("1: BF=%d, BFMsk=%04x\r\n", BF, BFMsk);
			}
			BFMsk = ~BFMsk;
			//printf("2: BF=%d, BFMsk=%04x\r\n", BF, BFMsk); 
			value = (RValue & BFMsk) | (value<<STBit);
			//printf("* Phyaddr=%d, RegAddr=0x%02lX, OrgValue=0x%04lX, Modified value=0x%04lX\r\n", phyaddr, reg,  RValue, value);
		  	printf("* Phyaddr=%d, DevAddr=0x%02lX,RegAddr=0x%02lX, STBit=%0d, BFlen=%0d, value=0x%04lX\r\n", phyaddr,dev_addr, reg_addr, STBit, BFlen, value);		
			printf("* OrgValue=0x%04lX,",RValue);
			mtEMiiRegWrite(phyaddr, dev_addr,reg_addr, value);
			RValue = mtEMiiRegRead(phyaddr,dev_addr, reg_addr);
			printf(" ModValue=0x%04lX\r\n", RValue);
 		}
		return 0;
	}
	else	
	{
        printf("       CMD Error : emiiw <PhyAddr> <DevAddr> <regAddr> <data>\r\n");
		printf("                         emiiwb <PhyAddr> <DevAddr> <RegAddr> <STBit> <BFLen> <BFVal> \r\n");	
        return 0;      
	}
#endif	
#endif
   return 0;
}

#if 0
int doPhyMMDWrite_eco(int argc, char *argv[], void *p) 	// cl45
{
	uint32 dev_addr=0;
	uint32 phyaddr=0;
	uint32 reg_addr=0;
	uint32 value=0;
	uint8 i=0;

	uint8	tx_offset_reg_shift=0, tx_amp_reg_shift=0, tx_amp_reg_shift_eco_lsb=0, tx_amp_reg_shift_eco_msb=0; 
	uint16	tx_offset_reg=0, reg_temp=0, reg_temp_eco=0;
	uint8	tx_amp_temp=0, tx_amp_temp_eco_lsb=0, tx_amp_temp_eco_msb=0, tx_amp_reg=0, cnt=0, tx_amp_reg_100=0;
	uint8	tx_amp_offset_hbt=0, tx_amp_offset_hbt_p5=0, tx_amp_offset_tst=0, tx_amp_offset_tbt=0, tx_amp_temp_hbt=0, tx_amp_temp_tst=0, tx_amp_temp_tbt=0;

	if(argc == 5)
	{	// tce emiiw <DevAddr> <PhyAddr> <regAddr> <data>
		phyaddr = checked_atoi(argv[1]);//0~31
		#if debug_flag
		sscanf(argv[2], "%lx", &dev_addr);//hex
		sscanf(argv[3], "%lx", &reg_addr);//hex
		sscanf(argv[4], "%lx", &value);//hex
		#endif
    }

 	if(((argc == 5)) && (phyaddr<=0x1f))
 	{
 		if(argc == 5)
 		{     	
			// ======================================================================================
			// 		7516 E1 EPHY Digital bug workaround  (5) (hbt/tst)
			//		da_p0[5:0] = reg_p0[5:0]
			//		da_p1[5:1] = reg_p1[4:0]
			//		da_p2[5:1] = reg_p2[3:0], reg_p1[5]
			//		da_p3[5:1] = reg_p3[2:0], reg_p2[5:4]
			//		da_p4[5:1] = reg_p4[1:0], reg_p3[5:3]		
			// ======================================================================================

			if(phyaddr == 8) 			// p0
			{
				tx_amp_reg_shift_eco_lsb = 0;
					
				tx_amp_reg_shift_eco_msb = 0;
				tx_amp_temp_eco_msb = ((tx_amp_temp_tst>>0))<<tx_amp_reg_shift_eco_msb;
			}
			else if(phyaddr == 9) 		// p1
			{
				tx_amp_reg_shift_eco_lsb = 0;
					
				tx_amp_reg_shift_eco_msb = 0;	// p1[4:0]
				tx_amp_temp_eco_msb = ((tx_amp_temp_tst>>1))<<tx_amp_reg_shift_eco_msb;
			}
			else if(phyaddr == 10)		// p2
			{
				tx_amp_reg_shift_eco_lsb = 5;	// p1[5]
				tx_amp_temp_eco_lsb = (tx_amp_temp_tst>>1) & 0x1;
				reg_temp_eco = (mtEMiiRegRead((phyaddr-1), 0x1e, tx_amp_reg_100) & (~0x0020));
				mtEMiiRegWrite((phyaddr-1), 0x1e, tx_amp_reg_100, (reg_temp_eco|((tx_amp_temp_eco_lsb<<tx_amp_reg_shift_eco_lsb)&(~0x0010))));	// 1e_16, 18, 20, 22

				tx_amp_reg_shift_eco_msb = 0;	// p2[3:0]
				tx_amp_temp_eco_msb = ((tx_amp_temp_tst>>2))<<tx_amp_reg_shift_eco_msb;
			}
			else if(phyaddr == 11)		// p3
			{
				tx_amp_reg_shift_eco_lsb = 4;	// p2[5:4]
				tx_amp_temp_eco_lsb = (tx_amp_temp_tst>>1) & 0x3;
				reg_temp_eco = (mtEMiiRegRead((phyaddr-1), 0x1e, tx_amp_reg_100) & (~0x0030));
				mtEMiiRegWrite((phyaddr-1), 0x1e, tx_amp_reg_100, (reg_temp_eco|((tx_amp_temp_eco_lsb<<tx_amp_reg_shift_eco_lsb)&(~0x0008))));	// 1e_16, 18, 20, 22

				tx_amp_reg_shift_eco_msb = 0;	// p3[2:0]
				tx_amp_temp_eco_msb = ((tx_amp_temp_tst>>3))<<tx_amp_reg_shift_eco_msb;
			}
			else if(phyaddr == 12)		// p4
			{
				tx_amp_reg_shift_eco_lsb = 3;	// p3[5:3]
				tx_amp_temp_eco_lsb = (tx_amp_temp_tst>>1) & 0x7;
				reg_temp_eco = (mtEMiiRegRead((phyaddr-1), 0x1e, tx_amp_reg_100) & (~0x0038));
				mtEMiiRegWrite((phyaddr-1), 0x1e, tx_amp_reg_100, (reg_temp_eco|((tx_amp_temp_eco_lsb<<tx_amp_reg_shift_eco_lsb)&(~0x0004))));	// 1e_16, 18, 20, 22

				tx_amp_reg_shift_eco_msb = 0;	// p4[1:0]
				tx_amp_temp_eco_msb = ((tx_amp_temp_tst>>4))<<tx_amp_reg_shift_eco_msb;
			}
			mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|(tx_amp_temp_eco_msb)));
 		
			mtEMiiRegWrite(phyaddr, dev_addr, reg_addr,value);
			#if debug_flag
			printf("* doPhyMMDWrite => phyaddr=%d, dev_addr=%d, data_addr=0x%04lX , value=0x%04lX\r\n", phyaddr, dev_addr, reg_addr, value);
			#endif
		}
 		
		return 0;
	}
	else	
	{
        printf("       CMD Error : emiiw_eco <PhyAddr> <DevAddr> <regAddr> <data>\r\n");
        return 0;      
	}
}
#endif

#if 0
int doPhyEMiiWrite_60986(int argc, char *argv[], void *p) 	// API for 60986
{
	uint32 phyaddr=0;
	uint32 reg_addr=0;
	uint32 value=0;
	uint16	reg_addr_31_16=0, reg_addr_15_0=0;
	uint16	value_31_16=0, value_15_0=0;

	if(argc == 4)
	{	// tce emiiw_60986 <PhyAddr> <regAddr> <data>
		phyaddr = checked_atoi(argv[1]);		//0~31
		sscanf(argv[2], "%lx", &reg_addr);		//hex
		sscanf(argv[3], "%lx", &value);			//hex
    }
    reg_addr_31_16 = (reg_addr>>16) & 0xffff;
    reg_addr_15_0 = (reg_addr>>0) & 0xffff;
    value_31_16 = (value>>16) & 0xffff;
    value_15_0 = (value>>0) & 0xffff;

    if(((argc == 4)) && (phyaddr<=0x1f))
 	{
 		if(argc == 4)
 		{
 			mtEMiiRegWrite(phyaddr, 0x1f, 0x800, reg_addr_31_16);
 			mtEMiiRegWrite(phyaddr, 0x1f, 0x801, reg_addr_15_0);
 			mtEMiiRegWrite(phyaddr, 0x1f, 0x802, value_31_16);
 			mtEMiiRegWrite(phyaddr, 0x1f, 0x803, value_15_0);
 			mtEMiiRegWrite(phyaddr, 0x1f, 0x804, 0x1);			// write
 		}
 		printf("* doPhyEMiiWrite_60986 => phyaddr=%d, data_addr=0x%08lX , value=0x%08lX\r\n", phyaddr, reg_addr, value);
	}
	else
	{
		printf("       CMD Error : emiiw_60986 <PhyAddr> <regAddr> <data> \r\n");
        return 0;   
	}
}

int doPhyEMiiRead_60986(int argc, char *argv[], void *p) 	// API for 60986
{
	uint32 phyaddr=0;
	uint32 reg_addr=0;
	uint32 value=0;
	uint16	reg_addr_31_16=0, reg_addr_15_0=0;
	uint16	value_31_16=0, value_15_0=0;

	if(argc == 3)
	{	// tce emiir_60986 <PhyAddr> <regAddr>
		phyaddr = checked_atoi(argv[1]);	//0~31
		sscanf(argv[2], "%lx", &reg_addr);	//hex
    }
    reg_addr_31_16 = (reg_addr>>16) & 0xffff;
    reg_addr_15_0 = (reg_addr>>0) & 0xffff;

	if(((argc == 3)) && (phyaddr<=0x1f))
 	{
 		if(argc == 3)
 		{
 			mtEMiiRegWrite(phyaddr, 0x1f, 0x800, reg_addr_31_16);
 			mtEMiiRegWrite(phyaddr, 0x1f, 0x801, reg_addr_15_0);
 			mtEMiiRegWrite(phyaddr, 0x1f, 0x804, 0x2);			// read
 			value_31_16 = mtEMiiRegRead(phyaddr, 0x1f, 0x805);
 			value_15_0 = mtEMiiRegRead(phyaddr, 0x1f, 0x806);
 			value = value_31_16<<16 | value_15_0;
 		}
 		printf("* doPhyMMDRead =>phyaddr=%d, data_addr=0x%08lX , value=0x%08lX\r\n", phyaddr, reg_addr, value);
 	}	
	else
	{
		printf("       CMD Error : emiir_60986 <PhyAddr> <regAddr>  \r\n");
        return 0; 
	}
}
#endif
#if debug_flag
int doLpbk_setting(int argc, char *argv[], void *p)
{
    //tce lpbk <10M> <phyaddr> <farend/di/digital/analog> <on/off>
    //tce lpbk <100M> <phyaddr> <farend/digital/analog> <on/off>
    //tce lpbk <1000M> <phyaddr> <farend/digital/analog> <on/off>
	const uint16 page_reg=31;
	const uint16 TestPage=0x2A30;
	const uint16 ExtendPage=0x0001;
	const uint32 Token_Ring_debug_reg=0x52B5;
	const uint32 Token_Ring_Control_reg=0x10;
	const uint32 Token_Ring_Low_data_reg=0x11;
	const uint32 Token_Ring_High_data_reg=0x12;
	const uint16 Token_Ring_CH_addr_PCS=0x2;
	const uint16 Token_Ring_Node_addr_PCS=0xF;
	//const uint16 MainPage=0x00;
	uint32 value;
	uint32 value_high=0;
	uint32 value_low=0;
	uint16 phyaddr=0;

	if(argc==5)
	{
		phyaddr = checked_atoi(argv[2]);//0~31
		if((strcasecmp(argv[1], "10M") == 0)&&((strcasecmp(argv[4], "on") == 0)||(strcasecmp(argv[4], "off") == 0)))
		{
			if(strcasecmp(argv[3], "farend") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{
					tcMiiStationWrite(phyaddr, page_reg, ExtendPage);
					value = tcMiiStationRead(phyaddr, 0x1A);
					tcMiiStationWrite(phyaddr, 0x1A, value|0x8000);		//set Media_Loopback(ExtReg1Ah[15])='1'
					tcMiiStationWrite(phyaddr, page_reg, 0);			//recover page number to main page
					tcMiiStationWrite(phyaddr, 0x00, 0x0100);			//set Smi_Reg00 as Force-10F _mode , set reg00=0x0100
				}
				else
				{
					tcMiiStationWrite(phyaddr, page_reg, ExtendPage);
					value = tcMiiStationRead(phyaddr, 0x1A);
					tcMiiStationWrite(phyaddr, 0x1A, value&0x7FFF);		//set Media_Loopback(ExtReg1Ah[15])='0'
					tcMiiStationWrite(phyaddr, page_reg, 0);			//recover page number to main page
					tcMiiStationWrite(phyaddr, 0x00, 0x1040);			//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
				}
			}
			else if(strcasecmp(argv[3], "di_part1") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{
					tcMiiStationWrite(phyaddr, page_reg, 0);			//force page number to main page
					tcMiiStationWrite(phyaddr, 0x00, 0x0000);			//set Smi_Reg00 as Force-10H _mode ,no Loopback mode, no MrCollisionTestEn , set reg00=0x0000
				}
				else
				{
					tcMiiStationWrite(phyaddr, page_reg, 0);			//force page number to main page 
					tcMiiStationWrite(phyaddr, 0x00, 0x1040);			//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
					value = tcMiiStationRead(phyaddr, 0x16);
					tcMiiStationWrite(phyaddr, 0x16, value|0x2000);		//recover EchoMode10Dis(Reg16h[13]) ='1'
				}
			}
			else if(strcasecmp(argv[3], "di_part2") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{
					tcMiiStationWrite(phyaddr, page_reg, 0);			//force page number to main page
					value = tcMiiStationRead(phyaddr, 0x16);
					tcMiiStationWrite(phyaddr, 0x16, value&0xDFFF);		//set EchoMode10Dis(Reg16h[13]) ='0'
				}
				else
				{
					tcMiiStationWrite(phyaddr, page_reg, 0);			//force page number to main page
					tcMiiStationWrite(phyaddr, 0x00, 0x1040);			//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
					value = tcMiiStationRead(phyaddr, 0x16);
					tcMiiStationWrite(phyaddr, 0x16, value|0x2000);		//recover EchoMode10Dis(Reg16h[13]) ='1'
				}
			}
			else if(strcasecmp(argv[3], "digital") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{
					tcMiiStationWrite(phyaddr, page_reg, 0);			//force page number to main page
					tcMiiStationWrite(phyaddr, 0x00, 0x4100);			//set Smi_Reg00 as Force-10F _mode + Loopback mode , set reg00=0x4100
				}
				else
				{
					tcMiiStationWrite(phyaddr, page_reg, 0);			//force page number to main page 
					tcMiiStationWrite(phyaddr, 0x00, 0x1040);			//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
				}
			}
			else if(strcasecmp(argv[3], "analog") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{ 
                    value=mtEMiiRegRead(phyaddr, 0x1E, 0x14D);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x14D, value|0x0080);	//set AnaLoopBack_pre(dev1eh_reg14Dh[7])='1' //CML_20130221
					value=mtEMiiRegRead(phyaddr, 0x1E, 0x33);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x33, value&0xFFF8);	//set cr_pd_*_pair_sw_en(dev1eh_reg33h[2:0]) ='000'
					tcMiiStationWrite(phyaddr, 0x00, 0x0100);			//set Smi_Reg00 as Force-10F _mode , set reg00=0x0100
				}
				else
				{
                    value=mtEMiiRegRead(phyaddr, 0x1E, 0x14D);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x14D, value&0xFF7F);	//set AnaLoopBack_pre(dev1eh_reg14Dh[7])='0'				  
					value=mtEMiiRegRead(phyaddr, 0x1E, 0x33);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x33, value|0x0007);	//set cr_pd_*_pair_sw_en(dev1eh_reg33h[2:0]) ='111'		   
					tcMiiStationWrite(phyaddr, 0x00, 0x1040);			//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
				}
			}
			else
			{
				printf("	 CMD Error : tce lpbk <10M> <phyaddr> <farend/di/digital/analog> <on/off>\r\n");
			}
			return 0;
		}
		else if((strcasecmp(argv[1], "100M") == 0)&&((strcasecmp(argv[4], "on") == 0)||(strcasecmp(argv[4], "off") == 0)))
		{
			if(strcasecmp(argv[3], "farend") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{
					tcMiiStationWrite(phyaddr, page_reg, ExtendPage);
					value = tcMiiStationRead(phyaddr, 0x1A);
					tcMiiStationWrite(phyaddr, 0x1A, value|0x8000);		//set Media_Loopback(ExtReg1Ah[15])='1'
					tcMiiStationWrite(phyaddr, page_reg, 0);			//recover page number to main page
					tcMiiStationWrite(phyaddr, 0x00, 0x1040);			//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
					value = tcMiiStationRead(phyaddr, 0x09);					
					tcMiiStationWrite(phyaddr, 0x09, value&0xFCFF);		//set AdvCap1000FDX(Reg09[9])=0 &AdvCap1000HDX(Reg09[8])=0 to disable 1000M ability , use AN to get 100M Full ability to remote
				}
				else
				{
					tcMiiStationWrite(phyaddr, page_reg, ExtendPage);
					value = tcMiiStationRead(phyaddr, 0x1A);					
					tcMiiStationWrite(phyaddr, 0x1A, value&0x7FFF);		//set Media_Loopback(ExtReg1Ah[15])='0'
					tcMiiStationWrite(phyaddr, page_reg, 0);			//recover page number to main page
					tcMiiStationWrite(phyaddr, 0x00, 0x1040);			//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
					value = tcMiiStationRead(phyaddr, 0x04);					
					tcMiiStationWrite(phyaddr, 0x04, value|0x0300);		//recovery to default value,set AdvCap1000FDX(Reg09[9])=1 &AdvCap1000HDX(Reg09[8])=1
				}
			}
			else if(strcasecmp(argv[3], "digital") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{
					tcMiiStationWrite(phyaddr,page_reg,0);			//force page number to main page
					tcMiiStationWrite(phyaddr,0x00,0x6100);			//set Smi_Reg00 as Force-100F _mode + Loopback mode , set reg00=0x6100
				}
				else
				{
					tcMiiStationWrite(phyaddr,page_reg,0);			//force page number to main page 
					tcMiiStationWrite(phyaddr,0x00,0x1040);			//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
				}
			}
			else if(strcasecmp(argv[3], "analog") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{ 
                    value=mtEMiiRegRead(phyaddr, 0x1E,0x14D);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x14D, value|0x0080);	//set AnaLoopBack_pre(dev1eh_reg14Dh[7])='1' //CML_20130221
					value=mtEMiiRegRead(phyaddr, 0x1E,0x33);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x33, value&0xFFF8);	//set cr_pd_*_pair_sw_en(dev1eh_reg33h[2:0]) ='000'
                    //token ring register PCS_01 setting
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(Token_Ring_CH_addr_PCS<<11)|(Token_Ring_Node_addr_PCS<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); 	// data ready					
					value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);
					value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);
					value=value_low+((value_high&0x00FF)<<15);
					value=value|0x0000C000; 							//set RxABPairSwapForce(PCS_01[15])=1 && set RxABPairSwapForceVal(PCS_01[14])=1
					value_high=(0x00FF0000&value);
					value_low=(0x0000FFFF&value);
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
					tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(Token_Ring_CH_addr_PCS<<11)|(Token_Ring_Node_addr_PCS<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); 	// data ready
					//token ring register PCS_01 setting end
					tcMiiStationWrite(phyaddr, page_reg, 0x00);			//V1.11
					tcMiiStationWrite(phyaddr,0x00,0x2100);				//set Smi_Reg00 as Force-100F _mode , set reg00=0x0100
				}
				else
				{
                    value = mtEMiiRegRead(phyaddr, 0x1E, 0x14D);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x14D, value&0xFF7F);	//set AnaLoopBack_pre(dev1eh_reg14Dh[7])='0'				  
					value = mtEMiiRegRead(phyaddr, 0x1E, 0x33);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x33, value|0x0007);	//set cr_pd_*_pair_sw_en(dev1eh_reg33h[2:0]) ='111'
					//token ring register PCS_01 setting
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(Token_Ring_CH_addr_PCS<<11)|(Token_Ring_Node_addr_PCS<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); 	// data ready
					value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);
					value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);
					value=value_low+((value_high&0x00FF)<<15);
					value=value&0x00FF3FFF; 							//set RxABPairSwapForce(PCS_01[15])=0 && set RxABPairSwapForceVal(PCS_01[14])=0
					value_high=(0x00FF0000&value);
					value_low=(0x0000FFFF&value);
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
					tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(Token_Ring_CH_addr_PCS<<11)|(Token_Ring_Node_addr_PCS<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); 	// data ready
					//token ring register PCS_01 setting end
					tcMiiStationWrite(phyaddr, page_reg, 0x00);			//V1.11
					tcMiiStationWrite(phyaddr,0x00,0x1040);				//set Smi_Reg00 to initial value , set reg00=0x1040(initial value)
				}
			}
			else
			{
				printf("     CMD Error : tce lpbk <100M> <phyaddr> <farend/digital/analog> <on/off>\r\n");
			}
			return 0;
		}
		else if((strcasecmp(argv[1], "1000M") == 0)&&((strcasecmp(argv[4], "on") == 0)||(strcasecmp(argv[4], "off") == 0)))
		{
			if(strcasecmp(argv[3], "farend") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{
					tcMiiStationWrite(phyaddr,page_reg,ExtendPage);
					value=tcMiiStationRead(phyaddr,0x1A);
					tcMiiStationWrite(phyaddr,0x1A,value|0x8000);		//set Media_Loopback(ExtReg1Ah[15])='1'
					tcMiiStationWrite(phyaddr,page_reg,0);				//recover page number to main page 
					tcMiiStationWrite(phyaddr,0x00,0x1040);				//set Smi_Reg00 to initial value , set reg00=0x1040(initial value)  
					tcMiiStationWrite(phyaddr,0x09,0x0700);				//set smi_Reg09 to initial value , set reg09=0x0700(initial value) 
				}
				else
				{
					tcMiiStationWrite(phyaddr,page_reg,ExtendPage);
					value=tcMiiStationRead(phyaddr,0x1A);
					tcMiiStationWrite(phyaddr,0x1A,value&0x7FFF);		//set Media_Loopback(ExtReg1Ah[15])='0'
					tcMiiStationWrite(phyaddr,page_reg,0);				//recover page number to main page
					tcMiiStationWrite(phyaddr,0x00,0x1040);				//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
					tcMiiStationWrite(phyaddr,0x09,0x0700);				//set smi_Reg09 to initial value , set reg09=0x0700(initial value)
				}
			}
			else if(strcasecmp(argv[3], "digital_lpbk") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{
					tcMiiStationWrite(phyaddr,page_reg,0);				//force page number to main page
					tcMiiStationWrite(phyaddr,0x00,0x4140);				//set Smi_Reg00 as Force-1000F _mode + Loopback mode , set reg00=0x4140
				}
				else
				{
					tcMiiStationWrite(phyaddr,page_reg,0);				//force page number to main page
					tcMiiStationWrite(phyaddr,0x00,0x1040);				//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
				}
			}
			else if(strcasecmp(argv[3], "analog") == 0)
			{
				if(strcasecmp(argv[4], "on") == 0)
				{
                    value=mtEMiiRegRead(phyaddr, 0x1E,0x14D);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x14D, value|0x0080);	//set AnaLoopBack_pre(dev1eh_reg14Dh[7])='1' //CML_20130221
					value=tcMiiStationRead(phyaddr,0x18);
					tcMiiStationWrite(phyaddr,0x18,value|0x0001);		//set CableLoopback_pre(Reg18h)=1 to enable 1000M cable loop back
					value=mtEMiiRegRead(phyaddr, 0x1E,0x33);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x33, value&0xFFF8);	//set cr_pd_*_pair_sw_en(dev1eh_reg33h[2:0]) ='000'
					//token ring register PCS_01 setting
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(Token_Ring_CH_addr_PCS<<11)|(Token_Ring_Node_addr_PCS<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); // data ready
					value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);
					value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);
					value=value_low+((value_high&0x00FF)<<15);
					value=value|0x00003C00; 						//set TxCDPairSwapForce(PCS_01[13])=1 && set TxCDPairSwapForceVal(PCS_01[12])=1 && set TxABPairSwapForce(PCS_01[11])=1 && set TxABPairSwapForceVal(PCS_01[10])=1
					value_high=(0x00FF0000&value);
					value_low=(0x0000FFFF&value);
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
					tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(Token_Ring_CH_addr_PCS<<11)|(Token_Ring_Node_addr_PCS<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); // data ready
					//token ring register PCS_01 setting end
					//token ring register ECNC_C1 setting
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(0x0<<11)|(0x3<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); // data ready
					value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);
					value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);
					value=value_low+((value_high&0x00FF)<<15);
					value=value|0x00000041; 						//set EcAForceGainswapdisab(ECNC_C1[6])=1 && set ECForceUpdateDis(ECNC_C1[0])=1
					value_high=(0x00FF0000&value);
					value_low=(0x0000FFFF&value);
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
					tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(0x0<<11)|(0x3<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); // data ready
					//token ring register PCS_01 setting end
					tcMiiStationWrite(phyaddr, page_reg, 0x00);		//V1.11
					tcMiiStationWrite(phyaddr,0x00,0x0140);			//set Smi_Reg00 as Force-1000F _mode , set reg00=0x0140
				}
				else
				{
                    value=mtEMiiRegRead(phyaddr, 0x1E,0x14D);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x14D, value&0xFF7F);		//set AnaLoopBack_pre(dev1eh_reg14Dh[7])='0'
					value=tcMiiStationRead(phyaddr,0x18);			
					tcMiiStationWrite(phyaddr,0x18,value&0xFFFE);			//recovery to default,set CableLoopback_pre(Reg18h)=0 to disable 1000M cable loop back		  
					value=mtEMiiRegRead(phyaddr, 0x1E,0x33);
					mtEMiiRegWrite(phyaddr, 0x1E, 0x33, value|0x0007);		//recovery to default,set cr_pd_*_pair_sw_en(dev1eh_reg33h[2:0]) ='111'
					//token ring register PCS_01 setting
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(Token_Ring_CH_addr_PCS<<11)|(Token_Ring_Node_addr_PCS<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); // data ready
					value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);
					value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);
					value=value_low+((value_high&0x00FF)<<15);
					value=value&0x00FFC3FF; 					//recovery to default,set TxCDPairSwapForce(PCS_01[13])=0 && set TxCDPairSwapForceVal(PCS_01[12])=0 && set TxABPairSwapForce(PCS_01[11])=0 && set TxABPairSwapForceVal(PCS_01[10])=0
					value_high=(0x00FF0000&value);
					value_low=(0x0000FFFF&value);
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
					tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(Token_Ring_CH_addr_PCS<<11)|(Token_Ring_Node_addr_PCS<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); // data ready
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(1<<13)|(0x0<<11)|(0x3<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); // data ready
					value_low = tcMiiStationRead(phyaddr, Token_Ring_Low_data_reg);
					value_high = tcMiiStationRead(phyaddr, Token_Ring_High_data_reg);
					value=value_low+((value_high&0x00FF)<<15);
					value=value&0xFFFFFFBE; 					//recovery to default,set EcAForceGainswapdisab(ECNC_C1[6])=0 && set ECForceUpdateDis(ECNC_C1[0])=0
					value_high=(0x00FF0000&value);
					value_low=(0x0000FFFF&value);
					tcMiiStationWrite(phyaddr, page_reg, Token_Ring_debug_reg);
					tcMiiStationWrite(phyaddr, Token_Ring_Low_data_reg, value_low);
					tcMiiStationWrite(phyaddr, Token_Ring_High_data_reg, value_high);
					tcMiiStationWrite(phyaddr, Token_Ring_Control_reg, (1<<15)|(0<<13)|(0x0<<11)|(0x3<<7)|(0x01<<1));
					while(!((tcMiiStationRead(phyaddr, Token_Ring_Control_reg)&0x8000)==0x8000)); // data ready
					tcMiiStationWrite(phyaddr, page_reg, 0x00);		//V1.11		  
					tcMiiStationWrite(phyaddr,0x00,0x1040);				//set Smi_Reg00 to initial value , set reg00=0x1040(initial value) 
				}
			}
			else
			{
				printf("	 CMD Error : tce lpbk <1000M> <phyaddr> <farend/digital/analog> <on/off>\r\n");
			}
			return 0;
		}
	}
	printf("     CMD Error : tce lpbk <10M> <phyaddr> <farend/di/digital/analog> <on/off>\r\n");
	printf("     CMD Error : tce lpbk <100M> <phyaddr> <farend/digital/analog> <on/off>\r\n");
	printf("     CMD Error : tce lpbk <1000M> <phyaddr> <farend/digital/analog> <on/off>\r\n");
	return 0;	   
}

int doGbeTxShaper(int argc, char *argv[], void *p)
{
  	uint32 BaseAddr = 0x200;
  	uint32 TuneAddr1 = 0;
  	uint32 TuneAddr2 = 0;
  	uint32 TarAddr = 0;
  	uint32 wData = 0;
  	uint32 wTmp1 = 0, wTmp2 = 0, wTmp3 = 0;

  	//if ((argc != 7) || ((strcmpi("fe",argv[1]) != 0) && (strcmpi("gbe",argv[1]) != 0)))
  	if((argc != 7) && (argc != 4))
  	{
    	printf("Usage1: tce gtxshaper <norm/test> <PreSec> <NextSec> <Mid_Value1> <Ovs_Value2> <OvP_Value3>\n");
    	printf("<norm/test> = select normal or test mode\n");
    	printf("<PreSec>  = {P2|P1|Z0|N1|N2} for GbE\n");
    	printf("<NextSec> = {P2|P1|Z0|N1|N2} for GbE\n");
    	printf("<Mid_Value1> = 1st transition point (HEX format) - middle level\n");
    	printf("<Ovs_Value2> = 2nd transition point (HEX format) - overshoot level\n");
    	printf("<OvP_Value3> = 3rd transition point (HEX format) - overshoot-post level\n");
    	printf("Usage2: tce gtxshaper <norm/test> <PreSec> <NextSec>\n");
    	printf("        Get current combination setting\n");
  	}
  	else //if ((strcmpi("norm",argv[1]) == 0) || (strcmpi("test",argv[1]) == 0)) //GbE TX Shaper
  	{
    	if((strcasecmp("norm",argv[1]) == 0)) 		//GbE Normal Mode
    	{
      		BaseAddr = 0x200;
    	}
    	else if((strcasecmp("test",argv[1]) == 0))	//GbE Test Mode
    	{
      		BaseAddr = 0x200;  						///V2.00 change 0x232 to 0x200, E2 merge normal and test mode
    	}
    	else
    	{
      		printf("Arg[1] Error: {norm|test}\n");
    	}

    	if(strcasecmp("P2",argv[2]) == 0)
    	{
      		TuneAddr1 = 0;
    	}
    	else if(strcasecmp("P1",argv[2]) == 0)
    	{
      		TuneAddr1 = 10;
    	}
    	else if(strcasecmp("Z0",argv[2]) == 0)
    	{
      		TuneAddr1 = 20;
    	}
    	else if(strcasecmp("N1",argv[2]) == 0)
    	{
      		TuneAddr1 = 30;
    	}
    	else if(strcasecmp("N2",argv[2]) == 0)
    	{
      		TuneAddr1 = 40;
    	}
    	else
    	{
      		printf("Arg[2] Error: {P2|P1|Z0|N1|N2}\n");
    	}

    	if(strcasecmp("P2",argv[3]) == 0)
    	{
      		TuneAddr2 = 0;
    	}
    	else if(strcasecmp("P1",argv[3]) == 0)
    	{
      		TuneAddr2 = 2;
    	}
    	else if(strcasecmp("Z0",argv[3]) == 0)
    	{
      		TuneAddr2 = 4;
    	}
    	else if(strcasecmp("N1",argv[3]) == 0)
    	{
      		TuneAddr2 = 6;
    	}
    	else if(strcasecmp("N2",argv[3]) == 0)
    	{
      		TuneAddr2 = 8;
    	}
    	else
    	{
      		printf("Arg[3] Error: {P2|P1|Z0|N1|N2}\n");
    	}

		#if debug_flag
    	if(argc == 7) //Set corresponding register
    	{
      		sscanf(argv[4], "%lx", &wTmp1);
      		sscanf(argv[5], "%lx", &wTmp2);
      		sscanf(argv[6], "%lx", &wTmp3);
      		if((wTmp1 > 0x3F) || (wTmp2 > 0x3F) || (wTmp2 > 0x3F))
      		{
        		printf("Arg[4~6] Error: value should be 6bit digit\n");
      		}
      		else
      		{
        		wData = (wTmp2 << 8) + wTmp1;
        
        		TarAddr = BaseAddr + TuneAddr1 + TuneAddr2;
        		mtEMiiRegWrite(0x00, 0x1F, TarAddr, wData);
        		mtEMiiRegWrite(0x00, 0x1F, TarAddr+1, wTmp3);
        		printf("Write(dev1Fh_reg%Xh) = %X\n", TarAddr, wData); 		//debug
        		printf("Write(dev1Fh_reg%Xh) = %X\n", TarAddr+1, wTmp3); 	//debug
      		}
    	}
    	else  //Get corresponding setting
    	{
        	TarAddr = BaseAddr + TuneAddr1 + TuneAddr2;
        	wData = mtEMiiRegRead(0x00, 0x1F, TarAddr);
        	wTmp1 = wData & 0x003F;
        	wTmp2 = (wData & 0x3F00) >> 8;
        	printf("Read(dev1Fh_reg%Xh) = %X\n", TarAddr, wData);
        	wData = mtEMiiRegRead(0x00, 0x1F, TarAddr+1);
        	wTmp3 = wData & 0x003F;
        	printf("Read(dev1Fh_reg%Xh) = %X\n", TarAddr+1, wData);
        	printf("[Mid, Ovs, Ovp] = %X %X %X\n", wTmp1, wTmp2, wTmp3);
    	}
    	#endif
  	}
  	return 0;
}

int doFeTxShaper(int argc, char *argv[], void *p)
{
  	uint32 wData = 0;
  	uint32 wTmp1 = 0, wTmp2 = 0, wTmp3 = 0;
  	uint32 iPort = 0;
  
  	#if debug_flag
  	if((argc != 7) && (argc != 4))
  	{
    	printf("Usage1: tce ftxshaper <PortNum> <PreSec> <NextSec> <Mid_Value1> <Ovs_Value2> <OvP_Value3>\n");
    	printf("where\n");
    	printf("<PortNum> = 0 ~ 4\n");
    	printf("<PreSec> = {1,0,-1} for FE\n");
    	printf("<NextSec> = {1,0,-1} for FE\n");
    	printf("<Mid_Value1> = 1st transition point (HEX format) - middle level\n");
    	printf("<Ovs_Value2> = 2nd transition point (HEX format) - overshoot level\n");
    	printf("<OvP_Value3> = 3rd transition point (HEX format) - overshoot-post level\n");
    	printf("Usage2: tce ftxshaper <PortNum> <PreSec> <NextSec>\n");
    	printf("        Get current combination setting\n");
  	}
  	else if(argc == 7)  //Set Mid, Ovs, Ovp
  	{
    	sscanf(argv[1], "%lx", &iPort);
    	sscanf(argv[4], "%lx", &wTmp1);
    	sscanf(argv[5], "%lx", &wTmp2);
    	sscanf(argv[6], "%lx", &wTmp3);
    	if((iPort > 4) || (iPort < 0))
      		printf("Port Number Error: {0~4}");
    	else if((wTmp1 > 0x3FF) || (wTmp2 > 0x3FF) || (wTmp2 > 0x3FF))
      		printf("Arg[4~6] Error: value should be 10bit digit\n");
    	else
    	{
      		if((strcasecmp("0",argv[2])==0) && (strcasecmp("1",argv[3])==0))
      		{
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x00);
        		wData = (wData & 0x1000) | wTmp1;
        		mtEMiiRegWrite(iPort, 0x1E, 0x00, wData);
        		mtEMiiRegWrite(iPort, 0x1E, 0x01, wTmp2);
        		mtEMiiRegWrite(iPort, 0x1E, 0x02, wTmp3);
        		printf("Set Port%d 0 to 1: %X %X %X\n", iPort, wData, wTmp2, wTmp3);
      		}
      		else if((strcasecmp("1",argv[2])==0) && (strcasecmp("0",argv[3])==0))
      		{
        		mtEMiiRegWrite(iPort, 0x1E, 0x03, wTmp1);
        		mtEMiiRegWrite(iPort, 0x1E, 0x04, wTmp2);
        		mtEMiiRegWrite(iPort, 0x1E, 0x05, wTmp3);
        		printf("Set Port%d 1 to 0: %X %X %X\n", iPort, wTmp1, wTmp2, wTmp3);
      		}
      		else if((strcasecmp("0",argv[2])==0) && (strcasecmp("-1",argv[3])==0))
      		{
        		mtEMiiRegWrite(iPort, 0x1E, 0x06, wTmp1);
        		mtEMiiRegWrite(iPort, 0x1E, 0x07, wTmp2);
        		mtEMiiRegWrite(iPort, 0x1E, 0x08, wTmp3);
        		printf("Set Port%d 0 to -1: %X %X %X\n", iPort, wTmp1, wTmp2, wTmp3);
      		}
      		else if((strcasecmp("-1",argv[2])==0) && (strcasecmp("0",argv[3])==0))
      		{
        		mtEMiiRegWrite(iPort, 0x1E, 0x09, wTmp1);
        		mtEMiiRegWrite(iPort, 0x1E, 0x0A, wTmp2);
        		mtEMiiRegWrite(iPort, 0x1E, 0x0B, wTmp3);
        		printf("Set Port%d -1 to 0: %X %X %X\n", iPort, wTmp1, wTmp2, wTmp3);
      		}
      		else
      		{
        		printf("Arg[2][3] Error: should be[0 1],[1 0], [0 -1], [-1 0]\n");
      		}
    	}
  	}
  	else  //Get current setting
  	{
    	sscanf(argv[1], "%lx", &iPort);
    	if((iPort > 4) || (iPort < 0))
      		printf("Port Number Error: {0~4}");
    	else
    	{
      		if((strcasecmp("0",argv[2])==0) && (strcasecmp("1",argv[3])==0))
      		{
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x00);
        		wData = wData & 0x3FF;
        		printf("Mid (dev1Eh_reg000h[9:0]) = %X\n", wData);
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x01);
        		wData = wData & 0x3FF;
        		printf("Ovs (dev1Eh_reg001h[9:0]) = %X\n", wData);
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x02);
        		wData = wData & 0x3FF;
        		printf("Ovp (dev1Eh_reg002h[9:0]) = %X\n", wData);
      		}
      		else if((strcasecmp("1",argv[2])==0) && (strcasecmp("0",argv[3])==0))
      		{
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x03);
        		wData = wData & 0x3FF;
        		printf("Mid (dev1Eh_reg003h[9:0]) = %X\n", wData);
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x04);
        		wData = wData & 0x3FF;
        		printf("Ovs (dev1Eh_reg004h[9:0]) = %X\n", wData);
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x05);
        		wData = wData & 0x3FF;
        		printf("Ovp (dev1Eh_reg005h[9:0]) = %X\n", wData);
      		}
      		else if((strcasecmp("0",argv[2])==0) && (strcasecmp("-1",argv[3])==0))
      		{
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x06);
        		wData = wData & 0x3FF;
        		printf("Mid (dev1Eh_reg006h[9:0]) = %X\n", wData);
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x07);
        		wData = wData & 0x3FF;
        		printf("Ovs (dev1Eh_reg007h[9:0]) = %X\n", wData);
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x08);
        		wData = wData & 0x3FF;
        		printf("Ovp (dev1Eh_reg008h[9:0]) = %X\n", wData);
      		}
      		else if((strcasecmp("-1",argv[2])==0) && (strcasecmp("0",argv[3])==0))
      		{
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x09);
        		wData = wData & 0x3FF;
        		printf("Mid (dev1Eh_reg009h[9:0]) = %X\n", wData);
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x0A);
        		wData = wData & 0x3FF;
        		printf("Ovs (dev1Eh_reg00Ah[9:0]) = %X\n", wData);
        		wData = mtEMiiRegRead(iPort, 0x1E, 0x0B);
        		wData = wData & 0x3FF;
        		printf("Ovp (dev1Eh_reg00Bh[9:0]) = %X\n", wData);
      		}
      		else
      		{
        		printf("Arg[2][3] Error: should be[0 1],[1 0], [0 -1], [-1 0]\n");
      		}
    	}
  	}
  	#endif
  	return 0;
}

int doPhyPktCnt(int argc, char *argv[], void *p)
{       
	const uint16 page_reg=31;
	uint32 PktCnt, CrcErrCnt=0;
    uint32 phyaddr=0;
	#define PktCnt_Addr 0x12
	#define CrcErrcnt_Addr 0x17
	#define page_Addr 0x0001
	
    if(argc==2)
	{
    	// tce phycnt <PhyAddr> 
    	phyaddr = checked_atoi(argv[1]);		//0~31
    	tcMiiStationWrite(phyaddr, page_reg, page_Addr);
    	PktCnt = tcMiiStationRead(phyaddr, PktCnt_Addr);
    	if(PktCnt & 0x8000)
		{
    		PktCnt = PktCnt & 0x7FFF;
    	}
    	else
		{
			#if debug_flag
    	    printf("Invalid Pkcnt !, value=%d",PktCnt);
			#endif	
    	    return 0;
    	}
        CrcErrCnt = tcMiiStationRead(phyaddr, CrcErrcnt_Addr) & 0x00FF;
		#if debug_flag	
        printf(" PktCnt= %d, CrcErrCnt = %d \r\n", PktCnt, CrcErrCnt); 
		#endif
    }
    else
	{
		#if debug_flag
        printf("       CMD Error : phycnt <PhyAddr>\r\n");
		#endif	
        return 0;      
    }
    return 0;
}
#endif
#else
// called by macPeriodCheck() in tc3162l2mac.c
#endif
#endif

/************************************************************************
*                       END
*************************************************************************/
#define ANACAL_INIT			0x01
#define ANACAL_ERROR		0xFD
#define ANACAL_SATURATION	0xFE
#define	ANACAL_FINISH		0xFF

#define ANACAL_PAIR_A		0
#define ANACAL_PAIR_B		1
#define ANACAL_PAIR_C		2
#define ANACAL_PAIR_D		3

#define DAC_IN_0V					0x000
#define DAC_IN_2V					0x0f0	// +/-1V
#define TX_AMP_OFFSET_0mV			0
#define TX_AMP_OFFSET_VALID_BITS	6

#define FE_CAL_P0			0

const uint8	ZCAL_TO_R50ohm_TBL[64] =
{
	127,	127,	124,	120,	117,	114,	112,	109,	106,	104,	102,	99,		97,		79,		77,		75,
	73,		72,		70,		68,		66,		65,		47,		46,		44,		43,		42,		40,		39,		38,		37,		36,
	//34,		33,		32,		15,		14,		13,		12,		11,		10,		10,		9,		8,		7,		6,		5,		5,
	//34,		33,		32,		20,		20,		20,		20,		20,		10,		10,		9,		8,		7,		6,		5,		5,
	34,		33,		32,		26,		26,		20,		20,		20,		10,		10,		9,		8,		7,		6,		5,		5, // BGA
	4,		3,		2,		2,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0
#if 0 // mog_review
	127,	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 
  	123, 	122, 	117, 	115, 	112, 	103, 	100,  	98,  	87,  	85,  	83,  	81,  	72,  	70,  	68,  	66, 
   	64,  	55,  	53,  	52,  	50,  	49,  	48,  	38,  	36,  	35,  	34,  	33,  	32,  	22,  	21,  	20,
   	19,  	18,  	17,  	16,   	7,   	6,   	5,   	4,   	3,   	2,   	1,   	0,   	0,   	0,   	0,   	0
#endif 
};

const uint8	ZCAL_TO_R50ohm_TBL_100[64] =
{
	127,	127,	127,	127,	127,	127,	126,	123,	120,	117,	114,	112,	110,	107,	105,	103, 
	101,	99,		97,		79,		77,		75,		74,		72,		70,		69,		67,		66,		65,		47,		46,		45,
	43,		42,		41,		40,		39,		38,		37,		36,		34,		34,		33,		32,		15,		14,		13,		12,
	11,		10,		10,		9,		8,		7,		7,		6,		5,		4,		4,		3,		2,		2,		1,		1		
#if 0 // 2nd	
	127,	127,	124,	120,	117,	114,	112,	109,	106,	104,	102,	99,		97,		79,		77,		75,
	//73,		72,		70,		68,		66,		65,		47,		46,		44,		43,		42,		40,		39,		38,		37,		36,
	73,		72,		70,		68,		66,		65,		47,		44,		44,		44,		44,		44,		44,		38,		37,		36,
	34,		33,		32,		15,		14,		13,		12,		11,		10,		10,		9,		8,		7,		6,		5,		5,
	4,		3,		2,		2,		1,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0
#endif
#if 0 // mog_review // 1st
	127,	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127, 
  	123, 	122, 	117, 	115, 	112, 	103, 	100,  	98,  	87,  	85,  	83,  	81,  	72,  	70,  	68,  	66, 
   	64,  	55,  	53,  	52,  	50,  	49,  	48,  	38,  	36,  	35,  	34,  	33,  	32,  	22,  	21,  	20,
   	19,  	18,  	17,  	16,   	7,   	6,   	5,   	4,   	3,   	2,   	1,   	0,   	0,   	0,   	0,   	0
#endif 
};

const uint8	ZCAL_TO_R45ohm_TBL[64] =
{
#if 0
	127,	127,	127,	127,	127,	127,	126,	123,	120,	117,	114,	112,	110,	107,	105,	103,   // 0  - 15
	101,	99,		97,		79,		77,		75,		74,		72,		70,		69,		67,		66,		65,		47,		46,		45,    // 16 - 31, 30:mid
	43,		42,		41,		40,		39,		38,		37,		36,		34,		34,		33,		32,		15,		14,		13,		12,
	11,		10,		10,		9,		8,		7,		7,		6,		5,		4,		4,		3,		2,		2,		1,		1		
#endif

#if 0

   127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
   127,127,127,124,121,118,114,111,108,105,102, 99, 96, 94, 91, 88,
   86, 83, 81, 78, 76, 74, 71, 69, 67, 65, 63, 61, 59, 57, 55, 53,
   51, 49, 47, 46, 44, 42, 41, 39, 37, 36, 34, 33, 31, 30, 29, 27
#endif  
   
   //5R to 0R
   127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
   127,127,127,127,127,123,118,114,110,106,102, 98, 96, 92, 88, 85,
   82, 80, 76, 72, 70, 67, 64, 62, 60, 56, 54, 52, 49, 48, 45, 43,
   40, 39, 36, 34, 32, 32, 30, 28, 25, 24, 22, 20, 18, 16, 16, 14



};


const uint8	ZCAL_TO_R44ohm_TBL[64] =
{
	127,	127,	127,	127,	127,	127,	127,	127,	127,	127,	127,	127,	127,	127,	127,	127,   // 0  - 15
	127,	127,	124,	121,	119,	116,	114,	104,	102,	99,		97,		88,		85,		83,		81,		72,    // 16 - 31, 30:mid
	70,		68,		66,		64,		55,		53,		52,		50,		49,		48,		38,		37,		36,		34,		33,		32,
	23,		22,		20,		19,		18,		17,		16,		7,		6,		5,		4,		3,		2,		1,		0,		0		
};


// ************************************************************************
// 		Tx calibration use the Software flow : Rext, R50, Tx offset, Tx amp
//		Rx calibration use the Hardware flow
// ************************************************************************
#define   regWriteWord(addr,val)      ( *(volatile unsigned int *)(addr)=val )
#define   regReadWord(addr,val) 	  ( val=*(volatile unsigned int *)(addr) )

void doGePhyALLAnalogCal(uint8 phyaddr) // for EN7512 GBE 
{
	uint8	rg_zcal_ctrl, all_ana_cal_status;
	uint16	ad_cal_comp_out_init;
	uint16	dev1e_e0_ana_cal_r5, reg0_temp, dev1e_145_temp; 
	int 	calibration_polarity, tx_offset_temp;
	uint16	calibration_pair, cal_temp;
	uint8	tx_offset_reg_shift, tx_amp_reg_shift; 
	uint16	tx_offset_reg, reg_temp;
	uint8	tx_amp_temp, tx_amp_reg, cnt=0, tx_amp_reg_100;

	tcMiiStationWrite(phyaddr, 0x1f, 0x0000);		// g0 
	reg0_temp = tcMiiStationRead(phyaddr, 0x0);		// keep the default value
	tcMiiStationWrite(phyaddr, 0x0,  0x0140);		// set [12]AN disable, [8]full duplex, [13/6]1000Mbps

	mtEMiiRegWrite(phyaddr, 0x1f, 0x0100, 0xc000);			// BG voltage output
	dev1e_145_temp = mtEMiiRegRead(phyaddr, 0x1e, 0x0145);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0145, 0x1010);			// fix mdi
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0185, 0x0000);			// disable tx slew control
	
	// calibration start ======================================================================================
	while(GECal_flag == 0)		
		GECal_Rext(phyaddr, 100);

	// *** R50 Cal start *******************************************************************************
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);	// 1e_dc[0]:rg_txvos_calen
	for(calibration_pair = ANACAL_PAIR_A; calibration_pair <= ANACAL_PAIR_D; calibration_pair ++)
	{
		rg_zcal_ctrl = 0x20;  						// start with 0 dB
		dev1e_e0_ana_cal_r5 = (mtEMiiRegRead(phyaddr, 0x1e, 0x00e0) & (~0x003f));
		mtEMiiRegWrite(phyaddr, 0x1e, 0x00e0, (dev1e_e0_ana_cal_r5 | rg_zcal_ctrl));	// 1e_e0[5:0]:rg_zcal_ctrl
		if(calibration_pair == ANACAL_PAIR_A)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1101);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);	
		}
		else if(calibration_pair == ANACAL_PAIR_B)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x1000);	// 1e_dc[12]:rg_zcalen_b
		}
		else if(calibration_pair == ANACAL_PAIR_C)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0100);	// 1e_dc[8]:rg_zcalen_c
		}
		else // if(calibration_pair == ANACAL_PAIR_D)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0010);	// 1e_dc[4]:rg_zcalen_d
		}

		all_ana_cal_status = allGeAnaCalWait(20, phyaddr); // delay 20 usec
		if(all_ana_cal_status == 0)
		{
			all_ana_cal_status = ANACAL_ERROR;	
			printf("  GE R50 AnaCal ERROR!   \r\n");
		}
	
		ad_cal_comp_out_init = (mtEMiiRegRead(phyaddr, 0x1e, 0x017a)>>8) & 0x1;		// 1e_17a[8]:ad_cal_comp_out	
		if(ad_cal_comp_out_init == 1)
		{
			calibration_polarity = -1;
		}
		else
		{
			calibration_polarity = 1;
		}

		cnt = 0;
		while(all_ana_cal_status < ANACAL_ERROR)
		{
			cnt ++;
			rg_zcal_ctrl += calibration_polarity;
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00e0, (dev1e_e0_ana_cal_r5 | rg_zcal_ctrl));
			all_ana_cal_status = allGeAnaCalWait(20, phyaddr); // delay 20 usec

			if(all_ana_cal_status == 0)
			{
				all_ana_cal_status = ANACAL_ERROR;	
				printf("  GE R50 AnaCal ERROR!   \r\n");
			}
			else if(((mtEMiiRegRead(phyaddr, 0x1e, 0x017a)>>8)&0x1) != ad_cal_comp_out_init) 
			{
				all_ana_cal_status = ANACAL_FINISH;	
			}
			else
			{
				if((rg_zcal_ctrl == 0x3F)||(rg_zcal_ctrl == 0x00))	
				{
					all_ana_cal_status = ANACAL_SATURATION;  // need to FT
					printf(" GE R50 AnaCal Saturation!  \r\n");
				}
			}
		}
		
		if(all_ana_cal_status == ANACAL_ERROR)
		{	
			rg_zcal_ctrl = 0x20;  // 0 dB
			//mtEMiiRegWrite(phyaddr, 0x1e, 0x00e0, (dev1e_e0_ana_cal_r5 | rg_zcal_ctrl));
		}
		else
		{
			rg_zcal_ctrl = ZCAL_TO_R50ohm_TBL_100[rg_zcal_ctrl]-2;	// wait Mog zcal/r50 mapping table
			printf("  GE R50 AnaCal Done! (%d) (0x%x)(0x%x) \r\n", cnt, rg_zcal_ctrl, (rg_zcal_ctrl|0x80));
		}

		if(calibration_pair == ANACAL_PAIR_A)  
		{
			ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0174) & (~0x7f00);
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0174, (ad_cal_comp_out_init | (((rg_zcal_ctrl<<8)&0xff00) | 0x8000)));	// 1e_174[15:8]
	
			if(isEN7526c)
			{
				mtEMiiRegWrite(phyaddr, 0x1e, 0x0174, (ad_cal_comp_out_init | ((((rg_zcal_ctrl-3)<<8)&0xff00) | 0x8000)));	// 1e_174[15:8]
			}		
		}
		else if(calibration_pair == ANACAL_PAIR_B)
		{
			ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0174) & (~0x007f);
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0174, (ad_cal_comp_out_init | (((rg_zcal_ctrl<<0)&0x00ff) | 0x0080)));	// 1e_174[7:0]
			
			if(isEN7526c)
			{
				mtEMiiRegWrite(phyaddr, 0x1e, 0x0174, (ad_cal_comp_out_init | ((((rg_zcal_ctrl-3)<<0)&0x00ff) | 0x0080)));	// 1e_174[7:0]
			}
		}
		else if(calibration_pair == ANACAL_PAIR_C)
		{
			ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0175) & (~0x7f00);
			//ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0175);
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0175, (ad_cal_comp_out_init | (((rg_zcal_ctrl<<8)&0xff00) | 0x8000)));	// 1e_175[15:8]
			//printf(" GE-c 1e_174(0x%x), 1e_175(0x%x)  \r\n", mtEMiiRegRead(phyaddr, 0x1e, 0x0174), mtEMiiRegRead(phyaddr, 0x1e, 0x0175));
			if(isEN7526c)
			{
				printf(" GE-c zcal(%d) \r\n", rg_zcal_ctrl);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x0175, (ad_cal_comp_out_init | ((((rg_zcal_ctrl-3)<<8)&0xff00) | 0x8000)));		
			}
		}
		else // if(calibration_pair == ANACAL_PAIR_D)
		{
			ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0175) & (~0x007f);
			//ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0175);
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0175, (ad_cal_comp_out_init | (((rg_zcal_ctrl<<0)&0x00ff) | 0x0080)));	// 1e_175[7:0]
			//printf(" GE-d 1e_174(0x%x), 1e_175(0x%x)  \r\n", mtEMiiRegRead(phyaddr, 0x1e, 0x0174), mtEMiiRegRead(phyaddr, 0x1e, 0x0175));
			if(isEN7526c)
			{
				printf(" GE-d zcal(%d) \r\n", rg_zcal_ctrl);
				mtEMiiRegWrite(phyaddr, 0x1e, 0x0175, (ad_cal_comp_out_init | ((((rg_zcal_ctrl-3)<<0)&0x00ff) | 0x0080)));		
			}
		}
		//mtEMiiRegWrite(phyaddr, 0x1e, 0x00e0, ((rg_zcal_ctrl<<8)|rg_zcal_ctrl));
	}
	

	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0000);
	// *** R50 Cal end ***

	// *** Tx offset Cal start *************************************************************************
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0001);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0096, 0x8000);	// 1e_96[15]:bypass_tx_offset_cal, Hw bypass, Fw cal
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xf808);	// 1e_3e
	
	for(calibration_pair = ANACAL_PAIR_A; calibration_pair <= ANACAL_PAIR_D; calibration_pair ++)
	{
		tx_offset_temp = TX_AMP_OFFSET_0mV;
		
		if(calibration_pair == ANACAL_PAIR_A)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x1000);				// 1e_dd[12]:rg_txg_calen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, (0x8000|DAC_IN_0V));	// 1e_17d:dac_in0_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, (0x8000|DAC_IN_0V));	// 1e_181:dac_in1_a
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0172) & (~0x3f00));
			tx_offset_reg_shift = 8;									// 1e_172[13:8]
			tx_offset_reg = 0x0172;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else if(calibration_pair == ANACAL_PAIR_B)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0100);				// 1e_dd[8]:rg_txg_calen_b
			mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, (0x8000|DAC_IN_0V));	// 1e_17e:dac_in0_b
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, (0x8000|DAC_IN_0V));	// 1e_182:dac_in1_b
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0172) & (~0x003f));
			tx_offset_reg_shift = 0;									// 1e_172[5:0]
			tx_offset_reg = 0x0172;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else if(calibration_pair == ANACAL_PAIR_C)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0010);				// 1e_dd[4]:rg_txg_calen_c
			mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, (0x8000|DAC_IN_0V));	// 1e_17f:dac_in0_c
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, (0x8000|DAC_IN_0V));	// 1e_183:dac_in1_c
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0173) & (~0x3f00));
			tx_offset_reg_shift = 8;									// 1e_173[13:8]
			tx_offset_reg = 0x0173;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else // if(calibration_pair == ANACAL_PAIR_D)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0001);				// 1e_dd[0]:rg_txg_calen_d
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, (0x8000|DAC_IN_0V));	// 1e_180:dac_in0_d
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, (0x8000|DAC_IN_0V));	// 1e_184:dac_in1_d
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0173) & (~0x003f));
			tx_offset_reg_shift = 0;									// 1e_173[5:0]
			tx_offset_reg = 0x0173;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));	// 1e_172, 1e_173
		
		all_ana_cal_status = allGeAnaCalWait(20, phyaddr); // delay 20 usec
		if(all_ana_cal_status == 0)
		{
			all_ana_cal_status = ANACAL_ERROR;	
			printf(" GE Tx offset AnaCal ERROR!   \r\n");
		}
	
		ad_cal_comp_out_init = (mtEMiiRegRead(phyaddr, 0x1e, 0x017a)>>8) & 0x1;		// 1e_17a[8]:ad_cal_comp_out	
		if(ad_cal_comp_out_init == 1)
		{
			calibration_polarity = -1;
		}
		else
		{
			calibration_polarity = 1;
		}

		cnt = 0;
		while(all_ana_cal_status < ANACAL_ERROR)
		{
			cnt ++;
			tx_offset_temp += calibration_polarity;
			if(tx_offset_temp >= 0)
			{
				cal_temp = tx_offset_temp;
			}
			else
			{
				cal_temp = (1<<(TX_AMP_OFFSET_VALID_BITS-1)) | abs(tx_offset_temp);
			}
			mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(cal_temp<<tx_offset_reg_shift)));

			all_ana_cal_status = allGeAnaCalWait(20, phyaddr); // delay 20 usec
			if(all_ana_cal_status == 0)
			{
				all_ana_cal_status = ANACAL_ERROR;	
				printf(" GE Tx offset AnaCal ERROR!   \r\n");
			}
			else if(((mtEMiiRegRead(phyaddr, 0x1e, 0x017a)>>8)&0x1) != ad_cal_comp_out_init) 
			{
				all_ana_cal_status = ANACAL_FINISH;	
			}
			else
			{
				if((tx_offset_temp == -31)||(tx_offset_temp == 31))	
				{
					all_ana_cal_status = ANACAL_SATURATION;  // need to FT
					printf(" GE Tx offset AnaCal Saturation!  \r\n");
				}
			}
		}
		
		if(all_ana_cal_status == ANACAL_ERROR)
		{	
			tx_offset_temp = TX_AMP_OFFSET_0mV;
			mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else
		{
			printf(" GE Tx offset AnaCal Done! (%d)(0x%x)  \r\n", cnt, cal_temp);
		}
	}
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, 0x0000);
	
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0000);	// disable analog calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);	// disable Tx offset calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0x0000);	// disable Tx VLD force mode
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0000);	// disable Tx offset/amplitude calibration circuit
	// *** Tx offset Cal end ***

	// *** Tx Amp Cal start *******************************************************************************
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0001);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00e1, 0x0010);	// 1e_e1[4]:select 1V
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xf808);	// 1e_3e:enable Tx VLD
	
	for(calibration_pair = ANACAL_PAIR_A; calibration_pair <= ANACAL_PAIR_D; calibration_pair ++)
	{
		tx_amp_temp = 0x20;	// start with 0 dB

		if(calibration_pair == ANACAL_PAIR_A)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x1000);				// 1e_dd[12]:tx_a amp calibration enable
			mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, (0x8000|DAC_IN_2V));	// 1e_17d:dac_in0_a	
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, (0x8000|DAC_IN_2V));	// 1e_181:dac_in1_a
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x012) & (~0xfc00));
			tx_amp_reg_shift = 10;										// 1e_12[15:10]
			tx_amp_reg = 0x12;
			tx_amp_reg_100 = 0x16;
		}
		else if(calibration_pair == ANACAL_PAIR_B)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0100);				// 1e_dd[8]:tx_b amp calibration enable
			mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, (0x8000|DAC_IN_2V));	// 1e_17e:dac_in0_b
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, (0x8000|DAC_IN_2V));	// 1e_182:dac_in1_b
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x017) & (~0x3f00));
			tx_amp_reg_shift = 8;										// 1e_17[13:8]
			tx_amp_reg = 0x17;
			tx_amp_reg_100 = 0x18;
		}
		else if(calibration_pair == ANACAL_PAIR_C)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0010);				// 1e_dd[4]:tx_c amp calibration enable
			mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, (0x8000|DAC_IN_2V));	// 1e_17f:dac_in0_c
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, (0x8000|DAC_IN_2V));	// 1e_183:dac_in1_c
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x019) & (~0x3f00));
			tx_amp_reg_shift = 8;										// 1e_19[13:8]
			tx_amp_reg = 0x19;
			tx_amp_reg_100 = 0x20;
		}
		else //if(calibration_pair == ANACAL_PAIR_D)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0001);				// 1e_dd[0]:tx_d amp calibration enable
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, (0x8000|DAC_IN_2V));	// 1e_180:dac_in0_d
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, (0x8000|DAC_IN_2V));	// 1e_184:dac_in1_d
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x021) & (~0x3f00));
			tx_amp_reg_shift = 8;										// 1e_21[13:8]
			tx_amp_reg = 0x21;
			tx_amp_reg_100 = 0x22;
		}
		mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(tx_amp_temp<<tx_amp_reg_shift)));	// 1e_12, 1e_17, 1e_19, 1e_21

		all_ana_cal_status = allGeAnaCalWait(20, phyaddr); 	// delay 20 usec
		if(all_ana_cal_status == 0)
		{
			all_ana_cal_status = ANACAL_ERROR;	
			printf(" GE Tx amp AnaCal ERROR!   \r\n");
		}
	
		ad_cal_comp_out_init = (mtEMiiRegRead(phyaddr, 0x1e, 0x017a)>>8) & 0x1;		// 1e_17a[8]:ad_cal_comp_out
		if(ad_cal_comp_out_init == 1)
		{
			calibration_polarity = -1;
		}
		else
		{
			calibration_polarity = 1;
		}

		cnt =0;
		while(all_ana_cal_status < ANACAL_ERROR)
		{
			cnt ++;
			tx_amp_temp += calibration_polarity;
			mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(tx_amp_temp<<tx_amp_reg_shift)));
			all_ana_cal_status = allGeAnaCalWait(100, phyaddr); // delay 20 usec
			if(all_ana_cal_status == 0)
			{
				all_ana_cal_status = ANACAL_ERROR;	
				printf(" GE Tx amp AnaCal ERROR!   \r\n");
			}
			else if(((mtEMiiRegRead(phyaddr, 0x1e, 0x017a)>>8)&0x1) != ad_cal_comp_out_init) 
			{
				all_ana_cal_status = ANACAL_FINISH;	
				reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg)&(~0xff00);
				if(calibration_pair == ANACAL_PAIR_A)
				{
					//mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|((tx_amp_temp+0x9)<<tx_amp_reg_shift)));	// for gbe(DAC), 0xc
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|((tx_amp_temp+0x2)<<tx_amp_reg_shift)));

					if(isEN7526c)
					{
						printf(" GE-a tx_amp(%d) \r\n", tx_amp_temp);
						//mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|((tx_amp_temp+12)<<tx_amp_reg_shift)));
						mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|((tx_amp_temp+5)<<tx_amp_reg_shift)));
					}
				}
				else //if(calibration_pair == ANACAL_PAIR_B)
				{
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|((tx_amp_temp+0x2)<<tx_amp_reg_shift)));	// for gbe(DAC), 0xc
					if(isEN7526c)
					{
						if(calibration_pair == ANACAL_PAIR_B)
						{
							printf(" GE-b tx_amp(%d) \r\n", tx_amp_temp);
							//mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|((tx_amp_temp+13)<<tx_amp_reg_shift)));
							mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|((tx_amp_temp+6)<<tx_amp_reg_shift)));
						}
						else
						{
							printf(" GE-c,d tx_amp(%d) \r\n", tx_amp_temp);
							mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|((tx_amp_temp+9)<<tx_amp_reg_shift)));
						}
					}
				}

				printf(" GE Tx amp AnaCal Done! (pair-%d)(0x%x)(0x%x)\r\n", calibration_pair, tx_amp_reg, mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg));
				
			}
			else
			{
				if((tx_amp_temp == 0x3f)||(tx_amp_temp == 0x00))	
				{
					all_ana_cal_status = ANACAL_SATURATION;  // need to FT
					printf(" GE Tx amp AnaCal Saturation!  \r\n");
				}
			}
		}
		if(all_ana_cal_status == ANACAL_ERROR)
		{	
			tx_amp_temp = 0x20;
			mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(tx_amp_temp<<tx_amp_reg_shift)));
			GECal_status = 0;
			break;
		}
		else
		{
			printf(" GE Tx amp AnaCal Done! (%d) (0x%x)\r\n", cnt, tx_amp_temp);
			GECal_status = 1;
			
			reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg_100)& (~0x003f);
			if((all_ana_cal_status == ANACAL_SATURATION)||(tx_amp_temp==0))
				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|(0x0010)));  // for tst	
			else
				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|(tx_amp_temp+14)));  // for tst

			reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg_100)&(~0xff00);
			
			if((all_ana_cal_status == ANACAL_SATURATION)||(tx_amp_temp==0))
				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|((0x4)<<tx_amp_reg_shift)));	// for 100
			else
			{
				//if(calibration_pair == ANACAL_PAIR_A)
				#if defined(TCSUPPORT_CPU_EN7521)
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|((tx_amp_temp+0x4)<<tx_amp_reg_shift)));	// for 100, 0xc
				#else //if(calibration_pair == ANACAL_PAIR_B)
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|((tx_amp_temp+0x7)<<tx_amp_reg_shift)));	// for 100, 0xc, 0x9 // for 7512
				#endif	
			}

			if(isEN7526c)
			{
				if((calibration_pair == ANACAL_PAIR_A)||(calibration_pair == ANACAL_PAIR_B))
				{
					reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg_100);
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp));

					
					reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg);
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, ((reg_temp & 0xff00)|0x1b));	// for 10
				}		
			}
		}
	}
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, 0x0000);
	
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0000);	// disable analog calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);	// disable Tx offset calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0x0000);	// disable Tx VLD force mode
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0000);	// disable Tx offset/amplitude calibration circuit
	// *** Tx Amp Cal end ***

	// *** Rx offset Cal start ******************************************************************************
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0096, 0x8000);					// 1e_96[15]:bypass_tx_offset_cal, Hw bypass, Fw cal
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0037, 0x0033);					// tx/rx_cal_criteria_value
	reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0039) & (~0x4800));	// [14]: bypass all calibration, [11]: bypass adc offset cal analog
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0039, reg_temp);				// rx offset cal by Hw setup
	reg_temp = (mtEMiiRegRead(phyaddr, 0x1f, 0x0107) & (~0x1000));	// [12]: enable rtune calibration
	mtEMiiRegWrite(phyaddr, 0x1f, 0x0107, reg_temp);				// disable rtune calibration
	reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0171) & (~0x0180));	// 1e_171[8:7]: bypass tx/rx dc offset cancellation process
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0171, (reg_temp | 0x0180));
	reg_temp = mtEMiiRegRead(phyaddr, 0x1e, 0x0039);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0039, (reg_temp | 0x2000));		// rx offset calibration start
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0039, (reg_temp & (~0x2000)));	// rx offset calibration end
	pause(10);														// mdelay for Hw calibration finish
	reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0171) & (~0x0180));
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0171, reg_temp);

	tcMiiStationWrite(phyaddr, 0x0,  reg0_temp);
	mtEMiiRegWrite(phyaddr, 0x1f, 0x0100, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0145, dev1e_145_temp);
	// *** Rx offset Cal end ***

	//tcMiiStationWrite(phyaddr, 0x0,  0x1200);
}

const uint8 EN75xx_TX_OFS_TBL[64] =
{
  0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
  0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
};

#if debug_flag
void doGePhyALLAnalogCal_R45(uint8 phyaddr) 
{
	uint8	rg_zcal_ctrl, all_ana_cal_status, phyaddr_p0, phyaddr_init, tbl_idx=0;
	uint16	ad_cal_comp_out_init;
	uint16	dev1e_e0_ana_cal_r5, reg0_temp, dev1e_145_temp; 
	int 	calibration_polarity, tx_offset_temp;
	uint16	calibration_pair, cal_temp;
	uint16	tx_offset_reg_shift, tx_amp_reg_shift, tx_amp_reg_shift_eco_lsb, tx_amp_reg_shift_eco_msb; 
	uint16	tx_offset_reg, reg_temp, reg_temp_eco;
	uint16	tx_amp_temp, tx_amp_temp_eco_lsb, tx_amp_temp_eco_msb, tx_amp_reg, cnt=0, tx_amp_reg_100;
	uint16	tx_amp_ofs_hbt_a, tx_amp_ofs_hbt_b, tx_amp_offset_hbt_p2, tx_amp_offset_hbt_p3, tx_amp_offset_hbt_p4, tx_amp_offset_tbt, tx_amp_offset_tbt_p4, tx_amp_temp_hbt, tx_amp_temp_tst, tx_amp_temp_tbt;
	uint16	tx_amp_offset_hbt_p0_a, tx_amp_offset_hbt_p0_b, tx_amp_offset_hbt_p1_a, tx_amp_offset_hbt_p1_b, tx_amp_offset_hbt_p2_a, tx_amp_offset_hbt_p2_b, tx_amp_offset_hbt_p3_a, tx_amp_offset_hbt_p3_b, tx_amp_offset_hbt_p4_a, tx_amp_offset_hbt_p4_b; 
	uint16	tx_amp_offset_tst, tx_amp_offset_tst_p9, tx_amp_offset_tst_p10, tx_amp_offset_tst_p11;

	uint16	r50_ofs = 0, tx_amp_ofs_tst = 0, tx_amp_ofs_tst_a = 0, tx_amp_ofs_tst_b = 0, tx_amp_ofs_tst_c = 0, tx_amp_ofs_tst_d = 0;

	phyaddr_p0 = 9;

	tcMiiStationWrite(phyaddr, 0x1f, 0x0000);		// g0 
	reg0_temp = tcMiiStationRead(phyaddr, 0x0);		// keep the default value
	tcMiiStationWrite(phyaddr, 0x0,  0x0140);		// set [12]AN disable, [8]full duplex, [13/6]1000Mbps

	mtEMiiRegWrite(phyaddr, 0x1f, 0x0100, 0xc000);			// BG voltage output
	dev1e_145_temp = mtEMiiRegRead(phyaddr, 0x1e, 0x0145);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0145, 0x1010);			// fix mdi
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0185, 0x0000);			// disable tx slew control
	
	for(phyaddr_init = phyaddr_p0; phyaddr_init<=12; phyaddr_init++)
	{
		//mtEMiiRegWrite(phyaddr_init, 0x1e, 0x0015, 0x0004);	// gating, cutoff others pair 
		mtEMiiRegWrite(phyaddr_init, 0x1e, 0x00e7, 0x0000);	// optimize the echo response
		mtEMiiRegWrite(phyaddr_init, 0x1e, 0x00e6, 0x1111);	// rg_hvga_surge_echo=1@external surge R
		mtEMiiRegWrite(phyaddr_init, 0x1e, 0x00fe, 0x0002);	// hvga tx input slew rate adjustment
	}
	
	// calibration start ======================================================================================
	while(GECal_Rext_flag == 0)			// move to bootloader
		GECal_Rext(phyaddr, 100);

	// *** R45 Cal start *******************************************************************************
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00e1, 0x0000);	// 1e_e1[4]:select 1V
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00dc, 0x0000);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00e1, 0x0000);	// 1e_e1[4]:select 1V

	//mtEMiiRegWrite(phyaddr, 0x1e, 0x0015, 0x0004);	// gating, cutoff thers pair 

	for(calibration_pair = ANACAL_PAIR_A; calibration_pair <= ANACAL_PAIR_D; calibration_pair ++)
	{
		rg_zcal_ctrl = 0x20;  						// start with 0 dB
		dev1e_e0_ana_cal_r5 = (mtEMiiRegRead( phyaddr_p0, 0x1e, 0x00e0) & (~0x003f));
		mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00e0, (dev1e_e0_ana_cal_r5 | rg_zcal_ctrl));	// 1e_e0[5:0]:rg_zcal_ctrl
		
		if(calibration_pair == ANACAL_PAIR_A)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1101);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);
		}
		else if(calibration_pair == ANACAL_PAIR_B)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x1000);	// 1e_dc[12]:rg_zcalen_b
		}
		else if(calibration_pair == ANACAL_PAIR_C)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0100);	// 1e_dc[8]:rg_zcalen_c
		}
		else // if(calibration_pair == ANACAL_PAIR_D)
		{
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0010);	// 1e_dc[4]:rg_zcalen_d
		}

		all_ana_cal_status = allGeAnaCalWait_R45(20, phyaddr); // delay 20 usec
		if(all_ana_cal_status == 0)
		{
			all_ana_cal_status = ANACAL_ERROR;	
			printf("  GE R45 AnaCal ERROR!   \r\n");
		}
	
		ad_cal_comp_out_init = (mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)>>8) & 0x1;		// 1e_17a[8]:ad_cal_comp_out	
		if(ad_cal_comp_out_init == 1)
		{
			calibration_polarity = -1;
		}
		else
		{
			calibration_polarity = 1;
		}

		cnt = 0;
		while(all_ana_cal_status < ANACAL_ERROR)
		{
			cnt ++;
			rg_zcal_ctrl += calibration_polarity;
			mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00e0, (dev1e_e0_ana_cal_r5 | rg_zcal_ctrl));
			all_ana_cal_status = allGeAnaCalWait_R45(20, phyaddr); // delay 20 usec

			if(all_ana_cal_status == 0)
			{
				all_ana_cal_status = ANACAL_ERROR;	
				printf("  GE R45 AnaCal ERROR!   \r\n");
			}
			else if(((mtEMiiRegRead( phyaddr_p0, 0x1e, 0x017a)>>8)&0x1) != ad_cal_comp_out_init) 
			{
				all_ana_cal_status = ANACAL_FINISH;	
			}
			else
			{
				if((rg_zcal_ctrl == 0x3F)||(rg_zcal_ctrl == 0x00))	
				{
					all_ana_cal_status = ANACAL_SATURATION;  // need to FT
					printf(" GE R45 AnaCal Saturation!  \r\n");
				}
			}
		}
		
		r50_ofs = 11;
			
		if(all_ana_cal_status == ANACAL_ERROR)
		{	
			rg_zcal_ctrl = 0x20;  // 0 dB
		}
		else
		{
			printf("  GE R45 AnaCal zcal_idx(before table) (dec: %d) (hex: 0x%x) \r\n",  rg_zcal_ctrl, (rg_zcal_ctrl));

			if((rg_zcal_ctrl-r50_ofs) > 0)
				rg_zcal_ctrl = ZCAL_TO_R45ohm_TBL[(rg_zcal_ctrl-r50_ofs)];

			printf("  GE R45 AnaCal Done! (cnt:%d)(after table) ([7][6:0]:0x%x) ([6:0]:0x%x) \r\n", cnt, rg_zcal_ctrl, (rg_zcal_ctrl|0x80));
		}

		if(calibration_pair == ANACAL_PAIR_A)  
		{
			ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0174) & (~0x7f00);
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0174, (ad_cal_comp_out_init | (((rg_zcal_ctrl<<8)&0xff00) | 0x8000)));	// 1e_174[15:8]
		}
		else if(calibration_pair == ANACAL_PAIR_B)
		{
			ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0174) & (~0x007f);
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0174, (ad_cal_comp_out_init | (((rg_zcal_ctrl<<0)&0x00ff) | 0x0080)));	// 1e_174[7:0]
		}
		else if(calibration_pair == ANACAL_PAIR_C)
		{
			ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0175) & (~0x7f00);
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0175, (ad_cal_comp_out_init | (((rg_zcal_ctrl<<8)&0xff00) | 0x8000)));	// 1e_175[15:8]
		}
		else // if(calibration_pair == ANACAL_PAIR_D)
		{
			ad_cal_comp_out_init = mtEMiiRegRead(phyaddr, 0x1e, 0x0175) & (~0x007f);
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0175, (ad_cal_comp_out_init | (((rg_zcal_ctrl<<0)&0x00ff) | 0x0080)));	// 1e_175[7:0]
		}
	}
	
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0000);
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00db, 0x0000);
	// *** R50 Cal end ***

	#if 1
	// *** Tx offset Cal start *************************************************************************
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0001);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00dc, 0x0001);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0096, 0x8000);	// 1e_96[15]:bypass_tx_offset_cal, Hw bypass, Fw cal
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xf808);	// 1e_3e
	//mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x1000);	// 

	//mtEMiiRegWrite(phyaddr, 0x1e, 0x0015, 0x0004);	// gating, cutoff thers pair 
	
	for(calibration_pair = ANACAL_PAIR_A; calibration_pair <= ANACAL_PAIR_D; calibration_pair ++)
	{
		tbl_idx = 31; //TX_AMP_OFFSET_0mV;
		tx_offset_temp = EN75xx_TX_OFS_TBL[tbl_idx];
		
		if(calibration_pair == ANACAL_PAIR_A)
		{
			// for 75.., from ACD/BH's suggest
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x1000);				// 1e_dd[12]:rg_txg_calen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0101);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0001);
			
			mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, (0x8000|DAC_IN_0V));	// 1e_17d:dac_in0_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, (0x8000|DAC_IN_0V));	// 1e_181:dac_in1_a
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0172) & (~0x3f00));
			tx_offset_reg_shift = 8;									// 1e_172[13:8]
			tx_offset_reg = 0x0172;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else if(calibration_pair == ANACAL_PAIR_B)
		{
			// for 75.., from ACD/BH's suggest
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0100);				// 1e_dd[8]:rg_txg_calen_b
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x1001);	// 1e_dc[12]:rg_zcalen_b

			mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, (0x8000|DAC_IN_0V));	// 1e_17e:dac_in0_b
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, (0x8000|DAC_IN_0V));	// 1e_182:dac_in1_b
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0172) & (~0x003f));
			tx_offset_reg_shift = 0;									// 1e_172[5:0]
			tx_offset_reg = 0x0172;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else if(calibration_pair == ANACAL_PAIR_C)
		{
			// for 75.., from ACD/BH's suggest
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0010);				// 1e_dd[4]:rg_txg_calen_c
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0101);	// 1e_dc[8]:rg_zcalen_c

			mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, (0x8000|DAC_IN_0V));	// 1e_17f:dac_in0_c
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, (0x8000|DAC_IN_0V));	// 1e_183:dac_in1_c
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0173) & (~0x3f00));
			tx_offset_reg_shift = 8;									// 1e_173[13:8]
			tx_offset_reg = 0x0173;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else // if(calibration_pair == ANACAL_PAIR_D)
		{
			// for 75.., from ACD/BH's suggest
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0001);				// 1e_dd[0]:rg_txg_calen_d
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0011);	// 1e_dc[4]:rg_zcalen_d

			mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, (0x8000|DAC_IN_0V));	// 1e_180:dac_in0_d
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, (0x8000|DAC_IN_0V));	// 1e_184:dac_in1_d
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0173) & (~0x003f));
			tx_offset_reg_shift = 0;									// 1e_173[5:0]
			tx_offset_reg = 0x0173;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));	// 1e_172, 1e_173
		
		all_ana_cal_status = allGeAnaCalWait_TxOffset(20, phyaddr); // delay 20 usec
		if(all_ana_cal_status == 0)
		{
			all_ana_cal_status = ANACAL_ERROR;	
			printf(" GE Tx offset AnaCal ERROR!   \r\n");
		}

		//printf(" GE Tx offset check \r\n");
		//printf(" reg0=0x%x, 1f_100=0x%x, 1e_145=0x%x, 1e_185=0x%x  \r\n", mtMiiRegRead(phyaddr, 0), (mtEMiiRegRead(phyaddr, 0x1f, 0x100)), (mtEMiiRegRead(phyaddr, 0x1e, 0x145)), (mtEMiiRegRead(phyaddr, 0x1e, 0x185)));
		//printf(" 1e_db=0x%x, 1e_dc=0x%x, 1e_96=0x%x, 1e_3e=0x%x  \r\n", (mtEMiiRegRead(phyaddr, 0x1e, 0xdb)), (mtEMiiRegRead(phyaddr, 0x1e, 0xdc)), (mtEMiiRegRead(phyaddr, 0x1e, 0x96)), (mtEMiiRegRead(phyaddr, 0x1e, 0x3e)));
		//printf(" 1e_17d=0x%x, 1e_181=0x%x, 1e_17e=0x%x, 1e_182=0x%x  \r\n", (mtEMiiRegRead(phyaddr, 0x1e, 0x17d)), (mtEMiiRegRead(phyaddr, 0x1e, 0x181)), (mtEMiiRegRead(phyaddr, 0x1e, 0x17e)), (mtEMiiRegRead(phyaddr, 0x1e, 0x182)));
		//printf(" 1e_e0=0x%x, 1f_115=0x%x, 1f_426=0x%x, 1e_15=0x%x  \r\n", (mtEMiiRegRead(phyaddr, 0x1e, 0xe0)), (mtEMiiRegRead(phyaddr, 0x1f, 0x115)), (mtEMiiRegRead(phyaddr, 0x1f, 0x426)), (mtEMiiRegRead(phyaddr, 0x1e, 0x15)));
					
		ad_cal_comp_out_init = (mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)>>8) & 0x1;		// 1e_17a[8]:ad_cal_comp_out
		
		if(ad_cal_comp_out_init == 1)
		{
			calibration_polarity = 1;
		}
		else
		{
			calibration_polarity = -1;
		}

		cnt = 0;
		while(all_ana_cal_status < ANACAL_ERROR)
		{
			cnt ++;
			tbl_idx += calibration_polarity;
			tx_offset_temp = EN75xx_TX_OFS_TBL[tbl_idx];

			//printf(" GE Tx offset AnaCal cnt=%d,  comp_out = 0x%x, comp_init = 0x%x, tx_offset_temp=0x%x, cal_temp=0x%x, 1e_172=0x%x  \r\n", cnt, (mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)), ad_cal_comp_out_init, tx_offset_temp, cal_temp, (mtEMiiRegRead(phyaddr, 0x1e, 0x0172)));
			
			if(tx_offset_temp >= 0)
			{
				cal_temp = tx_offset_temp;
			}
			else
			{
				tx_offset_temp = 0x3f;
				cal_temp = 0x3f; //(1<<(TX_AMP_OFFSET_VALID_BITS-1)) | abs(tx_offset_temp);
			}
			mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(cal_temp<<tx_offset_reg_shift)));

			all_ana_cal_status = allGeAnaCalWait_TxOffset(20, phyaddr); // delay 20 usec
			if(all_ana_cal_status == 0)
			{
				all_ana_cal_status = ANACAL_ERROR;	
				printf(" GE Tx offset AnaCal ERROR!   \r\n");
			}
			else if(((mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)>>8)&0x1) != ad_cal_comp_out_init) 
			{
				all_ana_cal_status = ANACAL_FINISH;	
			}
			else
			{
				//printf(" GE Tx offset AnaCal 1e_17a=0x%x, cal_temp=0x%x  \r\n", mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a), cal_temp);
				
				if((tx_offset_temp == 0x1f)||(tx_offset_temp == 0x3f))	
				{
					all_ana_cal_status = ANACAL_SATURATION;  // need to FT
					printf(" GE Tx offset AnaCal Saturation!  \r\n");
				}
			}
		}
		
		if(all_ana_cal_status == ANACAL_ERROR)
		{	
			tx_offset_temp = TX_AMP_OFFSET_0mV;
			mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else
		{
			printf(" GE Tx offset AnaCal Done! (%d)(0x%x)  \r\n", cnt, cal_temp);
		}
	}
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, 0x0000);
	
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0000);	// disable analog calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);	// disable Tx offset calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0x0000);	// disable Tx VLD force mode
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0000);	// disable Tx offset/amplitude calibration circuit
	// *** Tx offset Cal end ***

	// *** Tx Amp Cal start *******************************************************************************
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0001);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00e1, 0x0000);	// 1e_e1[4]:select 1V
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00dc, 0x0001);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00e1, 0x0000);	// 1e_e1[4]:select 1V
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xf808);	// 1e_3e:enable Tx VLD

	//mtEMiiRegWrite(phyaddr, 0x1e, 0x0015, 0x0004);	// gating, cutoff thers pair 
	
	for(calibration_pair = ANACAL_PAIR_A; calibration_pair <= ANACAL_PAIR_D; calibration_pair ++)
	{
		tx_amp_temp = 0x20;	// start with 0 dB

		if(calibration_pair == ANACAL_PAIR_A)
		{
			// for 75.., from ACD/BH's suggest
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x1000);				// 1e_dd[12]:tx_a amp calibration enable
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1101);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0001);
			
			mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, (0x8000|DAC_IN_2V));	// 1e_17d:dac_in0_a	
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, (0x8000|DAC_IN_2V));	// 1e_181:dac_in1_a
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x012) & (~0xfc00));
			tx_amp_reg_shift = 10;										// 1e_12[15:10]
			tx_amp_reg = 0x12;
			tx_amp_reg_100 = 0x16;
		}
		else if(calibration_pair == ANACAL_PAIR_B)
		{
			// for 75.., from ACD/BH's suggest
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0100);				// 1e_dd[8]:tx_b amp calibration enable
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x1001);	// 1e_dc[12]:rg_zcalen_b

			mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, (0x8000|DAC_IN_2V));	// 1e_17e:dac_in0_b
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, (0x8000|DAC_IN_2V));	// 1e_182:dac_in1_b
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x017) & (~0x3f00));
			tx_amp_reg_shift = 8;										// 1e_17[13:8]
			tx_amp_reg = 0x17;
			tx_amp_reg_100 = 0x18;
		}
		else if(calibration_pair == ANACAL_PAIR_C)
		{
			// for 75.., from ACD/BH's suggest
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0010);				// 1e_dd[4]:tx_c amp calibration enable
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0101);	// 1e_dc[8]:rg_zcalen_c

			mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, (0x8000|DAC_IN_2V));	// 1e_17f:dac_in0_c
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, (0x8000|DAC_IN_2V));	// 1e_183:dac_in1_c
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x019) & (~0x3f00));
			tx_amp_reg_shift = 8;										// 1e_19[13:8]
			tx_amp_reg = 0x19;
			tx_amp_reg_100 = 0x20;
		}
		else //if(calibration_pair == ANACAL_PAIR_D)
		{
			// for 75.., from ACD/BH's suggest
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0001);				// 1e_dd[0]:tx_d amp calibration enable
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0011);	// 1e_dc[4]:rg_zcalen_d
		
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, (0x8000|DAC_IN_2V));	// 1e_180:dac_in0_d
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, (0x8000|DAC_IN_2V));	// 1e_184:dac_in1_d
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x021) & (~0x3f00));
			tx_amp_reg_shift = 8;										// 1e_21[13:8]
			tx_amp_reg = 0x21;
			tx_amp_reg_100 = 0x22;
		}
		
		all_ana_cal_status = allGeAnaCalWait_TxAmp(20, phyaddr); 	// delay 20 usec
		if(all_ana_cal_status == 0)
		{
			all_ana_cal_status = ANACAL_ERROR;	
			printf(" GE Tx amp AnaCal ERROR!   \r\n");
			GECal_status = 0;
		}
	
		ad_cal_comp_out_init = (mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)>>8) & 0x1;		// 1e_17a[8]:ad_cal_comp_out
		if(ad_cal_comp_out_init == 1)
		{
			calibration_polarity = -1;
		}
		else
		{
			calibration_polarity = 1;
		}

		cnt =0;
		while(all_ana_cal_status < ANACAL_ERROR)
		{
			cnt ++;
			tx_amp_temp += calibration_polarity;

			mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(tx_amp_temp<<tx_amp_reg_shift)));		// 1e_12, 1e_17, 1e_19, 1e_21
			
			all_ana_cal_status = allGeAnaCalWait_TxAmp(20, phyaddr); // delay 20 usec

			//printf(" GE Tx amp AnaCal cnt=%d,  comp_out = 0x%x, comp_init = 0x%x, tx_amp_temp=0x%x, 1e_12=0x%x  \r\n", cnt, (mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)), ad_cal_comp_out_init, tx_amp_temp, (mtEMiiRegRead(phyaddr, 0x1e, 0x012)));
			
			if(all_ana_cal_status == 0)
			{
				all_ana_cal_status = ANACAL_ERROR;	
				printf(" GE Tx amp AnaCal ERROR!   \r\n");
				GECal_status = 0;
			}
			else if(((mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)>>8)&0x1) != ad_cal_comp_out_init) 
			{
				all_ana_cal_status = ANACAL_FINISH;	
				reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg)&(~0xff00);

				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(tx_amp_temp<<tx_amp_reg_shift)));		// 1e_12, 1e_17, 1e_19, 1e_21
			}
			else
			{
				//mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(tx_amp_temp<<tx_amp_reg_shift)));		// 1e_12, 1e_17, 1e_19, 1e_21
				
				if((tx_amp_temp == 0x3f)||(tx_amp_temp == 0x00))	
				{
					all_ana_cal_status = ANACAL_SATURATION;  // need to FT
					printf(" GE Tx amp AnaCal Saturation!  \r\n");
					GECal_status = 0;
				}
			}
		}
		
		if(all_ana_cal_status == ANACAL_ERROR)
		{	
			tx_amp_temp = 0x20;
			mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(tx_amp_temp<<tx_amp_reg_shift)));
			GECal_status = 0;
			break;
		}
		else
		{
			printf(" GE Tx amp AnaCal Done! (%d) (0x%x)\r\n", cnt, tx_amp_temp);
			GECal_status = 1;

			// =============================================================================================
			// tst, gbe
			// =============================================================================================
			reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg_100)& (~0x003f);		// for tst

			tx_amp_ofs_tst = 10;
			//normal setting by orign TXMR
			//tx_amp_ofs_tst_a = 8;
			
			//CAP TRANSEORMER COMP
			tx_amp_ofs_tst_a = 14, tx_amp_ofs_tst_b = 15, tx_amp_ofs_tst_c = 16, tx_amp_ofs_tst_d = 16; 
			
			if((calibration_pair == ANACAL_PAIR_A))
			{
				tx_amp_temp_tst = tx_amp_temp - tx_amp_ofs_tst_a;
			}
			else if((calibration_pair == ANACAL_PAIR_B))
			{
				tx_amp_temp_tst = tx_amp_temp - tx_amp_ofs_tst_b;
			}
			else if((calibration_pair == ANACAL_PAIR_C))
			{
				tx_amp_temp_tst = tx_amp_temp - tx_amp_ofs_tst_c;
			}
			else if((calibration_pair == ANACAL_PAIR_D))
			{
				tx_amp_temp_tst = tx_amp_temp - tx_amp_ofs_tst_d;
			}
			else
			{
				tx_amp_temp_tst = tx_amp_temp - tx_amp_ofs_tst;
			}
			
			if((all_ana_cal_status == ANACAL_SATURATION)||(tx_amp_temp_tst==0))
			{
				if((calibration_pair == ANACAL_PAIR_A))
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|(0x8020)));  // for gbe
				else
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|(0x2020)));  // for gbe	
			}
			else
			{
				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|(tx_amp_temp_tst)));	// for tst	// 1e_16, 1e_18, 1e_19, 1e_22[5:0]
			}

			reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg)& (~0xfc00);		// for gbe

			if((all_ana_cal_status == ANACAL_SATURATION))
			{
				if((calibration_pair == ANACAL_PAIR_A))
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(0x8020)));  // for gbe
				else
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(0x2020)));  // for gbe
			}
			else
			{
				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(tx_amp_temp<<tx_amp_reg_shift)));	// for gbe	// 1e_12, 1e_17, 1e_18, 1e_21[13:8]
			}

			// =============================================================================================
			// hbt
			// =============================================================================================
			
			reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg_100)&(~0xff00); // hbt

			// E3 ECNT0057_061
			tx_amp_ofs_hbt_a = 6;  // 4->3
			tx_amp_ofs_hbt_b = 6;
				
			if((calibration_pair == ANACAL_PAIR_A))
			{
				tx_amp_temp_hbt = tx_amp_temp - tx_amp_ofs_hbt_a;
			}
			else if((calibration_pair == ANACAL_PAIR_B))
			{
				tx_amp_temp_hbt = tx_amp_temp - tx_amp_ofs_hbt_b;
			}
			else
			{
				tx_amp_temp_hbt = tx_amp_temp;
			}
			
			if((all_ana_cal_status == ANACAL_SATURATION)||(tx_amp_temp_hbt==0))
			{
				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|((0x4)<<tx_amp_reg_shift)));	// for 100

				if((calibration_pair == ANACAL_PAIR_A))
				{
					mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, 0x6010);	// for 100
				}
				else if((calibration_pair == ANACAL_PAIR_B))
				{
					if(phyaddr == 9)
						mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, 0x2420);	// for 100
					else
						mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, 0x2820);	// for 100
				}
			}else
			{
				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg_100, (reg_temp|(tx_amp_temp_hbt<<tx_amp_reg_shift)));	// for 100	// 1e_16, 1e_18, 1e_19, 1e_22[13:8]
			}

			// =============================================================================================
			// tbt
			// =============================================================================================

			reg_temp = mtEMiiRegRead(phyaddr, 0x1e, tx_amp_reg)& (~0x003f);		// for tbt
			tx_amp_offset_tbt = 0;
			tx_amp_offset_tbt_p4 = 0;

			tx_amp_temp_tbt = tx_amp_temp + tx_amp_offset_tbt;
			
			if((all_ana_cal_status == ANACAL_SATURATION)||(tx_amp_temp_tbt==0))
			{
				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(0x0010)));  // for tbt	
			}
			else
			{
				mtEMiiRegWrite(phyaddr, 0x1e, tx_amp_reg, (reg_temp|(tx_amp_temp_tbt)));	// for tbt	// 1e_12, 1e_17, 1e_18, 1e_21[5:0]
			}
		}
	}
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, 0x0000);
	
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0000);	// disable analog calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);	// disable Tx offset calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0x0000);	// disable Tx VLD force mode
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0000);	// disable Tx offset/amplitude calibration circuit
	// *** Tx Amp Cal end ***

	// *** Rx offset Cal start ******************************************************************************
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0096, 0x8000);					// 1e_96[15]:bypass_tx_offset_cal, Hw bypass, Fw cal
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0037, 0x0033);					// tx/rx_cal_criteria_value
	reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0039) & (~0x4800));	// [14]: bypass all calibration, [11]: bypass adc offset cal analog
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0039, reg_temp);				// rx offset cal by Hw setup
	reg_temp = (mtEMiiRegRead(phyaddr, 0x1f, 0x0107) & (~0x1000));	// [12]: enable rtune calibration
	mtEMiiRegWrite(phyaddr, 0x1f, 0x0107, reg_temp);				// disable rtune calibration
	reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0171) & (~0x0180));	// 1e_171[8:7]: bypass tx/rx dc offset cancellation process
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0171, (reg_temp | 0x0180));
	reg_temp = mtEMiiRegRead(phyaddr, 0x1e, 0x0039);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0039, (reg_temp | 0x2000));		// rx offset calibration start
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0039, (reg_temp & (~0x2000)));	// rx offset calibration end
	pause(10);														// mdelay for Hw calibration finish
	reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0171) & (~0x0180));
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0171, reg_temp);

	tcMiiStationWrite(phyaddr, 0x0,  reg0_temp);
	mtEMiiRegWrite(phyaddr, 0x1f, 0x0100, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0145, dev1e_145_temp);
	// *** Rx offset Cal end ***

	mtEMiiRegWrite(9, 0x1e, 0x0015, 0x0000);	// gating, short with others pair 
	mtEMiiRegWrite(10, 0x1e, 0x0015, 0x0000);	// gating, short with others pair
	mtEMiiRegWrite(11, 0x1e, 0x0015, 0x0000);	// gating, short with others pair
	mtEMiiRegWrite(12, 0x1e, 0x0015, 0x0000);	// gating, short with others pair

	//tcMiiStationWrite(phyaddr, 0x0,  0x1200);
	#endif
}
#endif



void	GECal_Rext(uint8 phyaddr, uint32 delay)									
{
	uint8	rg_zcal_ctrl, all_ana_cal_status;
	uint16	ad_cal_comp_out_init;
	uint16	dev1e_e0_ana_cal_r5; 
	int 	calibration_polarity;
	uint16	reg_temp;
	uint8	cnt=0, phyaddr_p0=9;

	// *** Iext/Rext Cal start ********************************************************************************
	all_ana_cal_status = ANACAL_INIT;
	// analog calibration enable, Rext calibration enable
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x1110);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00e1, 0x0000);	// 1e_e1[4]:rg_cal_refsel(0:1.2V)

	rg_zcal_ctrl = 0x20;  								// start with 0 dB
	dev1e_e0_ana_cal_r5 = mtEMiiRegRead(phyaddr, 0x1e, 0x00e0);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00e0, (rg_zcal_ctrl));	// 1e_e0[5:0]:rg_zcal_ctrl

	all_ana_cal_status = allGeAnaCalWait(delay, phyaddr); 	// delay 20 usec
	if(all_ana_cal_status == 0)
	{
		all_ana_cal_status = ANACAL_ERROR;	
		printf(" GE Rext AnaCal ERROR!   \r\n");
	}
	
	ad_cal_comp_out_init = (mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)>>8) & 0x1;		// 1e_17a[8]:ad_cal_comp_out	
	if(ad_cal_comp_out_init == 1)
	{
		calibration_polarity = -1;
	}
	else // ad_cal_comp_out_init == 0
	{
		calibration_polarity = 1;
	}
	
	cnt = 0;
	while(all_ana_cal_status < ANACAL_ERROR)
	{
		cnt ++;
		rg_zcal_ctrl += calibration_polarity;
		mtEMiiRegWrite(phyaddr, 0x1e, 0x00e0, (rg_zcal_ctrl));
		all_ana_cal_status = allGeAnaCalWait(delay, phyaddr); // delay 20 usec

		if(all_ana_cal_status == 0)
		{
			all_ana_cal_status = ANACAL_ERROR;	
			printf("  GE Rext AnaCal ERROR!   \r\n");
		}
		else if(((mtEMiiRegRead(9, 0x1e, 0x017a)>>8)&0x1) != ad_cal_comp_out_init) 
		{
			all_ana_cal_status = ANACAL_FINISH;	
			printf("  GE Rext AnaCal Done! (%d)(0x%x)  \r\n", cnt, rg_zcal_ctrl);
		}
		else
		{
			if((rg_zcal_ctrl == 0x3F)||(rg_zcal_ctrl == 0x00))	
			{
				all_ana_cal_status = ANACAL_SATURATION;  // need to FT(IC fail?)
				printf(" GE Rext AnaCal Saturation!  \r\n");
				rg_zcal_ctrl = 0x20;  // 0 dB
			}
			//else
			//	printf(" GE Rxet cal (%d)(%d)(%d)(0x%x)  \r\n", cnt, ad_cal_comp_out_init, ((mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)>>8)&0x1), mtEMiiRegRead(phyaddr, 0x1e, 0xe0));
		}
	}

	if(all_ana_cal_status == ANACAL_ERROR)
	{	
		rg_zcal_ctrl = 0x20;  // 0 dB
		mtEMiiRegWrite(phyaddr, 0x1e, 0x00e0, (dev1e_e0_ana_cal_r5 | rg_zcal_ctrl));
		printf("  GE Rext AnaCal Fail! (0x%x)  \r\n", rg_zcal_ctrl);
		GECal_Rext_flag = 1;
	}
	else
	{
		mtEMiiRegWrite(phyaddr, 0x1e, 0x00e0, (dev1e_e0_ana_cal_r5 | rg_zcal_ctrl));
		mtEMiiRegWrite(phyaddr, 0x1e, 0x00e0, ((rg_zcal_ctrl<<8)|rg_zcal_ctrl));
		//****  1f_115[2:0] = rg_zcal_ctrl[5:3]  // Mog review
		mtEMiiRegWrite(phyaddr, 0x1f, 0x0115, ((rg_zcal_ctrl & 0x3f)>>3));
		printf("  GE Rext AnaCal Done! (%d)(0x%x)  \r\n", cnt, rg_zcal_ctrl);
		GECal_Rext_flag = 1;
		
		#if 0
		regReadWord(0xbfa2016c, reg_temp);
		//printf("RG_BG_RASEL = 0x%x (x%x)\r\n", reg_temp, rg_zcal_ctrl);
		reg_temp = (reg_temp & 0x1fff); 
		reg_temp = ((((rg_zcal_ctrl>>3)&0x7)<<13) | reg_temp);
		//printf("RG_BG_RASEL = 0x%x (x%x)\r\n", reg_temp, rg_zcal_ctrl);
		regWriteWord(0xbfa2016c, reg_temp);   // for ACD/steven simldo
		#endif
	}
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0000);
	// *** Iext/Rext Cal end ***
}


void	GECal_tx_offset(uint8 phyaddr, uint32 delay)
{
	uint8	all_ana_cal_status=0, phyaddr_p0=0;
	uint16	ad_cal_comp_out_init=0;
	uint16	reg0_temp=0, dev1e_145_temp=0; 
	int 	calibration_polarity=0, tx_offset_temp=0;
	uint16	cnt=0,calibration_pair=0, cal_temp=0;
	uint16	tx_offset_reg_shift=0; 
	uint16	tx_offset_reg=0, reg_temp=0;
	//uint16	tx_amp_temp=0, tx_amp_temp_eco_lsb=0, tx_amp_temp_eco_msb=0, tx_amp_reg=0, cnt=0, tx_amp_reg_100=0;
	//uint16	tx_amp_offset_hbt=0, tx_amp_offset_hbt_p2=0, tx_amp_offset_hbt_p3=0, tx_amp_offset_hbt_p4=0, tx_amp_offset_tbt=0, tx_amp_offset_tbt_p4=0, tx_amp_temp_hbt=0, tx_amp_temp_tst=0, tx_amp_temp_tbt=0;
	//uint16	tx_amp_offset_hbt_p0_a=0, tx_amp_offset_hbt_p0_b=0, tx_amp_offset_hbt_p1_a=0, tx_amp_offset_hbt_p1_b=0, tx_amp_offset_hbt_p2_a=0, tx_amp_offset_hbt_p2_b=0, tx_amp_offset_hbt_p3_a=0, tx_amp_offset_hbt_p3_b=0, tx_amp_offset_hbt_p4_a=0, tx_amp_offset_hbt_p4_b=0; 
	//uint16	tx_amp_offset_tst=0, tx_amp_offset_tst_p8_p9=0, tx_amp_offset_tst_p10=0, tx_amp_offset_tst_p11=0;
	
	phyaddr_p0 = 9;
	
	tcMiiStationWrite(phyaddr, 0x1f, 0x0000);		// g0 
	reg0_temp = tcMiiStationRead(phyaddr, 0x0); 	// keep the default value
	tcMiiStationWrite(phyaddr, 0x0,  0x0140);		// set [12]AN disable, [8]full duplex, [13/6]1000Mbps
	
	mtEMiiRegWrite(phyaddr, 0x1f, 0x0100, 0xc000);			// BG voltage output
	dev1e_145_temp = mtEMiiRegRead(phyaddr, 0x1e, 0x0145);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0145, 0x1010);			// fix mdi
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0185, 0x0000);			// disable tx slew control
		
	//eco_rev = VPint(0xbfb0005c);
	
	//printf(" 7523 GbE eco_rev (0x%x) &0xff(0x%x)\r\n", eco_rev, (eco_rev&0xff));
	mtEMiiRegWrite(9, 0x1e, 0x0015, 0x0004);	// gating, cutoff thers pair 
	mtEMiiRegWrite(10, 0x1e, 0x0015, 0x0004);	// gating, cutoff thers pair
	mtEMiiRegWrite(11, 0x1e, 0x0015, 0x0004);	// gating, cutoff thers pair
	mtEMiiRegWrite(12, 0x1e, 0x0015, 0x0004);	// gating, cutoff thers pair
		
	// *** Tx offset Cal start *************************************************************************
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0001);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x00dc, 0x0001);	// 1e_dc[0]:rg_txvos_calen
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0096, 0x8000);	// 1e_96[15]:bypass_tx_offset_cal, Hw bypass, Fw cal
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0xf808);	// 1e_3e

	mtEMiiRegWrite(phyaddr, 0x1e, 0x0015, 0x0004);	// gating, cutoff thers pair 
	
	for(calibration_pair = ANACAL_PAIR_A; calibration_pair <= ANACAL_PAIR_D; calibration_pair ++)
	{
		tx_offset_temp = TX_AMP_OFFSET_0mV;
		
		if(calibration_pair == ANACAL_PAIR_A)
		{
			// for 75.., from ACD/BH's suggest
			//mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x1000);				// 1e_dd[12]:rg_txg_calen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0101);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0001);
			
			mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, (0x8000|DAC_IN_0V));	// 1e_17d:dac_in0_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, (0x8000|DAC_IN_0V));	// 1e_181:dac_in1_a
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0172) & (~0x3f00));
			tx_offset_reg_shift = 8;									// 1e_172[13:8]
			tx_offset_reg = 0x0172;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else if(calibration_pair == ANACAL_PAIR_B)
		{
			// for 75.., from ACD/BH's suggest
			//mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0100);				// 1e_dd[8]:rg_txg_calen_b
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x1001);	// 1e_dc[12]:rg_zcalen_b

			mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, (0x8000|DAC_IN_0V));	// 1e_17e:dac_in0_b
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, (0x8000|DAC_IN_0V));	// 1e_182:dac_in1_b
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0172) & (~0x003f));
			tx_offset_reg_shift = 0;									// 1e_172[5:0]
			tx_offset_reg = 0x0172;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else if(calibration_pair == ANACAL_PAIR_C)
		{
			// for 75.., from ACD/BH's suggest
			//mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0010);				// 1e_dd[4]:rg_txg_calen_c
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0101);	// 1e_dc[8]:rg_zcalen_c

			mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, (0x8000|DAC_IN_0V));	// 1e_17f:dac_in0_c
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, (0x8000|DAC_IN_0V));	// 1e_183:dac_in1_c
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0173) & (~0x3f00));
			tx_offset_reg_shift = 8;									// 1e_173[13:8]
			tx_offset_reg = 0x0173;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else // if(calibration_pair == ANACAL_PAIR_D)
		{
			// for 75.., from ACD/BH's suggest
			//mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0001);				// 1e_dd[0]:rg_txg_calen_d
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0100);	// 1e_db[12]:rg_cal_ckinv, [8]:rg_ana_calen, [4]:rg_rext_calen, [0]:rg_zcalen_a
			mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0011);	// 1e_dc[4]:rg_zcalen_d

			mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, (0x8000|DAC_IN_0V));	// 1e_180:dac_in0_d
			mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, (0x8000|DAC_IN_0V));	// 1e_184:dac_in1_d
			reg_temp = (mtEMiiRegRead(phyaddr, 0x1e, 0x0173) & (~0x003f));
			tx_offset_reg_shift = 0;									// 1e_173[5:0]
			tx_offset_reg = 0x0173;
			//mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));	// 1e_172, 1e_173
		
		all_ana_cal_status = allGeAnaCalWait_TxOffset(delay, phyaddr); // delay 20 usec
		if(all_ana_cal_status == 0)
		{
			all_ana_cal_status = ANACAL_ERROR;	
			printf(" GE Tx offset AnaCal ERROR!   \r\n");
		}
		
		printf(" GE Tx offset check \r\n");
		printf(" reg0=0x%x, 1f_100=0x%x, 1e_145=0x%x, 1e_185=0x%x  \r\n", mtMiiRegRead(phyaddr, 0), (mtEMiiRegRead(phyaddr, 0x1f, 0x100)), (mtEMiiRegRead(phyaddr, 0x1e, 0x145)), (mtEMiiRegRead(phyaddr, 0x1e, 0x185)));
		printf(" 1e_db=0x%x, 1e_dc=0x%x, 1e_96=0x%x, 1e_3e=0x%x  \r\n", (mtEMiiRegRead(phyaddr, 0x1e, 0xdb)), (mtEMiiRegRead(phyaddr, 0x1e, 0xdc)), (mtEMiiRegRead(phyaddr, 0x1e, 0x96)), (mtEMiiRegRead(phyaddr, 0x1e, 0x3e)));
		printf(" 1e_17d=0x%x, 1e_181=0x%x, 1e_17e=0x%x, 1e_182=0x%x  \r\n", (mtEMiiRegRead(phyaddr, 0x1e, 0x17d)), (mtEMiiRegRead(phyaddr, 0x1e, 0x181)), (mtEMiiRegRead(phyaddr, 0x1e, 0x17e)), (mtEMiiRegRead(phyaddr, 0x1e, 0x182)));
		printf(" 1e_e0=0x%x, 1f_115=0x%x, 1f_426=0x%x, 1e_15=0x%x  \r\n", (mtEMiiRegRead(phyaddr, 0x1e, 0xe0)), (mtEMiiRegRead(phyaddr, 0x1f, 0x115)), (mtEMiiRegRead(phyaddr, 0x1f, 0x426)), (mtEMiiRegRead(phyaddr, 0x1e, 0x15)));
				
		ad_cal_comp_out_init = (mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)>>8) & 0x1;		// 1e_17a[8]:ad_cal_comp_out
		
		if(ad_cal_comp_out_init == 1)
		{
			calibration_polarity = -1;
		}
		else
		{
			calibration_polarity = 1;
		}

		cnt = 0;
		while(all_ana_cal_status < ANACAL_ERROR)
		{
			cnt ++;
			tx_offset_temp += calibration_polarity;

			//printf(" GE Tx offset AnaCal cnt=%d,  comp_out = 0x%x, comp_init = 0x%x, tx_offset_temp=0x%x, cal_temp=0x%x  \r\n", cnt, (mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)), ad_cal_comp_out_init, tx_offset_temp, cal_temp);
			//printf(" GE Tx offset AnaCal cnt=%d,  comp_out = 0x%x, comp_init = 0x%x, tx_offset_temp=0x%x, cal_temp=0x%x, 1e_172=0x%x  \r\n", cnt, (mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)), ad_cal_comp_out_init, tx_offset_temp, cal_temp, (mtEMiiRegRead(phyaddr, 0x1e, 0x0172)));
		
			if(tx_offset_temp >= 0)
			{
				cal_temp = tx_offset_temp;
			}
			else
			{
				//cal_temp = (1<<(TX_AMP_OFFSET_VALID_BITS-1)) | abs(tx_offset_temp);
				tx_offset_temp = 0x3f;
				cal_temp = 0x3f; 
			}
			mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(cal_temp<<tx_offset_reg_shift)));

			all_ana_cal_status = allGeAnaCalWait_TxOffset(delay, phyaddr); // delay 20 usec
			if(all_ana_cal_status == 0)
			{
				all_ana_cal_status = ANACAL_ERROR;	
				printf(" GE Tx offset AnaCal ERROR!   \r\n");
			}
			else if(((mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a)>>8)&0x1) != ad_cal_comp_out_init) 
			{
				all_ana_cal_status = ANACAL_FINISH;	
			}
			else
			{
				//printf(" GE Tx offset AnaCal 1e_17a=0x%x, cal_temp=0x%x  \r\n", mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017a), cal_temp);
				
				if((tx_offset_temp == -31)||(tx_offset_temp == 0x3f))	
				{
					all_ana_cal_status = ANACAL_SATURATION;  // need to FT
					printf(" GE Tx offset AnaCal Saturation!  \r\n");
				}
			}
		}
		
		if(all_ana_cal_status == ANACAL_ERROR)
		{	
			tx_offset_temp = TX_AMP_OFFSET_0mV;
			mtEMiiRegWrite(phyaddr, 0x1e, tx_offset_reg, (reg_temp|(tx_offset_temp<<tx_offset_reg_shift)));
		}
		else
		{
			printf(" GE Tx offset AnaCal Done! (%d)(0x%x)  \r\n", cnt, cal_temp);
		}
	}
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017d, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017e, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x017f, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0180, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0181, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0182, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0183, 0x0000);
	mtEMiiRegWrite(phyaddr, 0x1e, 0x0184, 0x0000);
	
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00db, 0x0000);	// disable analog calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dc, 0x0000);	// disable Tx offset calibration circuit
	mtEMiiRegWrite(phyaddr, 0x1e, 0x003e, 0x0000);	// disable Tx VLD force mode
	mtEMiiRegWrite(phyaddr, 0x1e, 0x00dd, 0x0000);	// disable Tx offset/amplitude calibration circuit
	// *** Tx offset Cal end ***
	GECal_flag = 1;
}



uint8 allGeAnaCalWait(uint32 delay, uint8 phyaddr) 
{
	uint8 all_ana_cal_status, phyaddr_p0=9;	

	mtEMiiRegWrite(phyaddr_p0, 0x1e, 0x017c, 0x0001);	// da_calin_flag pull high
	
	udelay(delay);

	all_ana_cal_status = mtEMiiRegRead(phyaddr_p0, 0x1e, 0x017b) & 0x1;
	mtEMiiRegWrite(phyaddr_p0, 0x1e, 0x017c, 0x0000);	// da_calin_flag pull low

	return all_ana_cal_status;
}


uint8 allGeAnaCalWait_R45(uint32 delay, uint8 phyaddr)  
{
	uint8 all_ana_cal_status, phyaddr_p0=9;	

	//phyaddr = 8;
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x017c, 0x0001);	// da_calin_flag pull high
	
	udelay(delay);

	//printf(" 1e_e0(0x%x), 1e_17a(0x%x) \r\n", mtEMiiRegRead( 8, 0x1e, 0xe0), mtEMiiRegRead( 8, 0x1e, 0x17a));
	
	all_ana_cal_status = mtEMiiRegRead( phyaddr_p0, 0x1e, 0x017b) & 0x1;
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x017c, 0x0000);	// da_calin_flag pull low

	return all_ana_cal_status;
}

uint8 allGeAnaCalWait_TxOffset(uint32 delay, uint8 phyaddr)
{
	uint8 all_ana_cal_status, phyaddr_p0=9;	

	//phyaddr = 8;
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x017c, 0x0001);	// da_calin_flag pull high
	
	udelay(delay);

	//printf(" 1e_172(0x%x), 1e_173(0x%x), 1e_17a(0x%x) \r\n", mtEMiiRegRead(phyaddr, 0x1e, 0x172), mtEMiiRegRead(phyaddr, 0x1e, 0x173), mtEMiiRegRead( 8, 0x1e, 0x17a));
	
	all_ana_cal_status = mtEMiiRegRead( phyaddr_p0, 0x1e, 0x017b) & 0x1;
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x017c, 0x0000);	// da_calin_flag pull low

	return all_ana_cal_status;
}

uint8 allGeAnaCalWait_TxAmp(uint32 delay, uint8 phyaddr)
{
	uint8 all_ana_cal_status, phyaddr_p0=9;	

	//phyaddr = 8;
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x017c, 0x0001);	// da_calin_flag pull high
	
	udelay(delay);

	//printf(" 1e_12(0x%x), 1e_17(0x%x), 1e_19(0x%x), 1e_21(0x%x), 1e_17a(0x%x) \r\n", mtEMiiRegRead(phyaddr, 0x1e, 0x12), mtEMiiRegRead(phyaddr, 0x1e, 0x17), mtEMiiRegRead(phyaddr, 0x1e, 0x19), mtEMiiRegRead(phyaddr, 0x1e, 0x21), mtEMiiRegRead( 8, 0x1e, 0x17a));
	
	all_ana_cal_status = mtEMiiRegRead( phyaddr_p0, 0x1e, 0x017b) & 0x1;

	//printf(" 1e_12(0x%x), 1e_17(0x%x), 1e_19(0x%x), 1e_21(0x%x), 1e_17b(0x%x), 1e_17a(0x%x) \r\n", mtEMiiRegRead(phyaddr, 0x1e, 0x12), mtEMiiRegRead(phyaddr, 0x1e, 0x17), mtEMiiRegRead(phyaddr, 0x1e, 0x19), mtEMiiRegRead(phyaddr, 0x1e, 0x21), mtEMiiRegRead( 8, 0x1e, 0x17b), mtEMiiRegRead( 8, 0x1e, 0x17a));
	
	mtEMiiRegWrite( phyaddr_p0, 0x1e, 0x017c, 0x0000);	// da_calin_flag pull low

	return all_ana_cal_status;
}


/* Register startup/shutdown routines */
module_init(tcPhyInit);
module_exit(tcPhyDeinit);  // JasonG_7512


