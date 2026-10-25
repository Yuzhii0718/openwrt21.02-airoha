/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
ECONET'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECONET 
SOFTWARE RELEASED HEREUNDER SHALL BE, AT ECONET'S SOLE OPTION, TO 
REVISE OR REPLACE THE ECONET SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO ECONET FOR SUCH 
ECONET SOFTWARE.
***************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
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
#include <asm/tc3162/cmdparse.h>
#include <asm/tc3162/tc3162.h>
#include <linux/mtd/rt_flash.h>
#include <linux/libcompileoption.h>
#include <linux/if_bridge.h>
#include "../net/bridge/br_private.h"


#ifdef TCSUPPORT_RA_HWNAT
#include <linux/foe_hook.h>
#endif
#include <ecnt_hook/ecnt_hook.h>
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_ifc.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <modules/eth_global_def.h>
#include "fe.h"
#include "fe_api.h"
#include "fe_ic_dis.h"
#include <ecnt_hook/ecnt_hook_smux.h>


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define atoi(x)  simple_strtol(x, NULL,10)

/* -----------------PSE Port Number Info ----------------- */
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
const unsigned char PSE_QUEUE_NUM[PSE_PORT_NUM] = {PSE_PORT0_QUEUE_NUM, PSE_PORT1_QUEUE_NUM, PSE_PORT2_QUEUE_NUM
                                                , PSE_PORT3_QUEUE_NUM, PSE_PORT4_QUEUE_NUM, PSE_PORT5_QUEUE_NUM
                                                , PSE_PORT6_QUEUE_NUM, PSE_PORT7_QUEUE_NUM, PSE_PORT8_QUEUE_NUM
                                                , PSE_PORT9_QUEUE_NUM};
#else
const unsigned char PSE_QUEUE_NUM[PSE_PORT_NUM] = {PSE_PORT0_QUEUE_NUM, PSE_PORT1_QUEUE_NUM, PSE_PORT2_QUEUE_NUM
                                                , PSE_PORT3_QUEUE_NUM, PSE_PORT4_QUEUE_NUM, PSE_PORT5_QUEUE_NUM
                                                , PSE_PORT6_QUEUE_NUM, PSE_PORT7_QUEUE_NUM};
#endif
/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define FE_MAJOR 226
#define FE_MAJOR_NAME "/dev/fe"

#define FE_SRAM_PD_NOT		0
#define FE_SRAM_PD_ALL		1
#define FE_SRAM_PD_HALF		2
#define FE_SRAM_PD_THFO		3

#ifdef TCSUPPORT_CPU_EN7580
int channel_retire = CHANNEL_RETIRE;
#else
int channel_retire = CHANNEL_DROP;
#endif

#define FE_TASK_QUOTA   6000            /*6000pps*/
#define FE_STOP_TIMER(timer)            { del_timer_sync(&timer) ; }
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
#define FE_START_TIMER(timer)           { mod_timer(&timer, (jiffies + ((timer.data*HZ)/1000))) ; }
#else
#define FE_START_TIMER(timer)           { mod_timer(&timer, (jiffies + ((timer.expires*HZ)/1000))) ; }
#endif

#define PSE_SHARE_THRESHOLD	0x01e001f4
static struct timer_list fe_cnt_timer ;
unsigned int *tx_ok_cnt[ANI_UNI_PORT_MAX] = {NULL};
unsigned int *tx_ok_byte_cnt[ANI_UNI_PORT_MAX] = {NULL};
unsigned int *rx_ok_cnt[ANI_UNI_PORT_MAX] = {NULL};
unsigned int *rx_ok_byte_cnt[ANI_UNI_PORT_MAX] = {NULL};
unsigned int *rx_discard_cnt[ANI_UNI_PORT_MAX] = {NULL};
unsigned int *tx_discard_cnt[ANI_UNI_PORT_MAX] = {NULL};
unsigned int *rx_error_cnt[ANI_UNI_PORT_MAX] = {NULL};
unsigned int *tx_error_cnt[ANI_UNI_PORT_MAX] = {NULL};

int fe_cnt_cur_idx = FE_CNT_SECONDS-1;

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct tasklet_struct dlfDetectTask;//task_struct
struct timer_list fe_timer ;
int fe_expire_time = 1000;  /* 1s */

int cwmp_vip_enable = 0;
uint cwmp_request_dport = 8099 ;    /* cwmp request dport fix to 8099, other customer should change to different value */
uint cwmp_connect_sport = 9090 ;    /* cwmp connect sport fix to 9090, other customer should change to different value */

Wan_itf_name wan_itf_info[WAN_ITF_ACNT_NUM];

unsigned long long gdm2_rx_len_high = 0;
unsigned long long gdm2_rx_drop_high = 0;
static unsigned int gdm2_rx_len_low_last = 0;
static unsigned int gdm2_rx_drop_low_last = 0;
unsigned long long gdm2_tx_len_high = 0;
unsigned long long gdm2_tx_drop_high = 0;
static unsigned int gdm2_tx_len_low_last = 0;
static unsigned int gdm2_tx_drop_low_last = 0;
static unsigned int stb_src_ip_mode = FE_OFF;

#if defined(TCSUPPORT_CPU_EN7521) || defined(TCSUPPORT_CPU_EN7512)
unsigned int gdm2_tx_eth_1024_to_1518_cnt_last = 0;
unsigned int gdm2_tx_eth_more_1518_cnt_last = 0;
unsigned int gdm2_tx_eth_bytes_cnt_last = 0;
unsigned int gdm2_tx_eth_len_cnt_last = 0;
unsigned int fe_dyna_set_count = 0;
#endif

int current_dev_mac_num = 0;
int wan_itf_and_dev_bandwidth_max_num = 200;    /* support max 200 dev mac */
FE_Status_Sel_t wan_itf_and_dev_bandwidth_mode = FE_ON;
EXPORT_SYMBOL(wan_itf_and_dev_bandwidth_mode);
int wan_itf_and_dev_bandwidth_timeout = 120;    /* 120s */
EXPORT_SYMBOL(wan_itf_and_dev_bandwidth_timeout);
WanItfAndDevBandwidth_t acntGrpGeneralTable[WAN_ITF_AND_DEV_NUM];
EXPORT_SYMBOL(acntGrpGeneralTable);
WanItf_t acntGrpWanItfTable[WAN_ITF_ONLY_NUM];
EXPORT_SYMBOL(acntGrpWanItfTable);
Multicast_t acntGrpMulticastTable[MULTICAST_DOWNSTREAM_NUM];
EXPORT_SYMBOL(acntGrpMulticastTable);

#if defined(TCSUPPORT_CPU_EN7521) || defined(TCSUPPORT_CPU_EN7512)
int dyn_qdma_txq_cngst_en=2;        /* 0: fixed parameter; 1: configed parameter; 2:disable */
int dyn_qdma_txq_cngst_packet_len=1466;
int dyn_qdma_txq_cngst_packet_len_reg=GDMA2_TX_ETH_1024_TO_1518_CNT;
int dyn_qdma_txq_cngst_stream_rate=200;     /* unit: mbps*/
int dyn_qdma_txq_cngst_total_min=0x3800;    /* the low 16 bit of 0xbfb550a4*/
#endif
/* monitor port2 IQ state */
unsigned int gdm2_before_rx_over_drop_cnt=0;
unsigned int gdm2_before_rx_ok=0;
unsigned int gdma2_iq_threshold=0x80;
unsigned int gdma2_drop_time=5;
unsigned int gdma2_iq_abnormal_cnt=0;
unsigned int is_increasing_flag=0;

unsigned int fe_pse_core_soft_reset_cnt=0;
/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern int (*dev_bandwidth_hook_get_cnt) (dev_bandwidth_account_t *dev_account);
extern int (*dev_bandwidth_hook_clear_cnt) (void);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
extern char* ip6_sprintf(const struct in6_addr *addr);
extern int br_mdb_fillbuf(struct net_bridge *br, void *buf,
		   unsigned long maxnum, unsigned long skip);
#endif
extern int (*multicast_get_stb_src_ip4)(unsigned int ip4);
extern int (*multicast_get_stb_src_ip6)(struct in6_addr *ip6);
extern wan_virtualdev_ifname wan_virtualdev_ifname_info[16];

#ifdef TCSUPPORT_CPU_ARMV8
extern struct device* get_frame_engine_dev(void);
#endif
extern unsigned char get_hybrid_mode(void);

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

long fe_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) ;
void get_wan_index_info(char * wan_index);
int fe_get_wan_itf_cnt(int idx, int dir, unsigned int *byteCnt_L, unsigned int *byteCnt_H, unsigned int *pktCnt);
int fe_clear_wan_itf_cnt(int idx, int dir);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
/* ADSL RTS dump function */

void TCConsole(uint8 mode)
{
}
EXPORT_SYMBOL(TCConsole);

void uartMacPutchar(int ch)
{
}
EXPORT_SYMBOL(uartMacPutchar);

uint32 GetIpAddr(void)
{
    return 0;
}
EXPORT_SYMBOL(GetIpAddr);


/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
struct file_operations fe_fops = {
	.owner			= THIS_MODULE,
	.unlocked_ioctl = fe_ioctl,
};

static struct timer_list fe_account_timer;
static wan_itf_or_dev_bandwidth_total_account_t *total_account_list = NULL;
static stb_src_ip_t *stb_src_ip_list = NULL;
static DEFINE_RWLOCK(account_lock);
static DEFINE_RWLOCK(stb_src_ip_lock);
static void *multicast_buf = NULL;
static void fe_account_timer_expires(TIMER_FUN_PAAM data);

int dbgLevel = 0;
int unlockLevel = 0;

static atomic_t protect_flag = ATOMIC_INIT(0);

/* tasklet */
struct tasklet_struct fe_pse_reset_task;

/* Timer */
static struct timer_list fe_monitor_timer;

unsigned int g_fe_iq_abnormal_cnt = 0;
unsigned int g_fe_iq_abnormal_threshold = 1;
unsigned int g_fe_iq_reset_test_flag = 0;
unsigned int g_fe_iq_reset_cnt = 0;
unsigned int g_fe_last_iq_sta1 = 0;
unsigned int g_fe_last_iq_sta2 = 0;
unsigned int g_fe_qdma_reset_cnt = 0;
unsigned int g_fe_last_qdma_dscp = 0;
unsigned int g_fe_last_gdma_chh_vld = 0;
unsigned int g_fe_all_reset_cnt = 0;
unsigned int g_fe_qdma_lan_reset_cnt = 0;
unsigned int g_fe_last_qdma_lan_dscp = 0;
unsigned int g_fe_pse_reset_test = 0;
EXPORT_SYMBOL(g_fe_pse_reset_test);

atomic_t fe_pse_reset_flag = ATOMIC_INIT(0);
EXPORT_SYMBOL(fe_pse_reset_flag);

spinlock_t fe_pse_reset_lock;

int fe_dyn_qdma_wan_txq_cngst_enable = 1;

/************************************************************************/
#if defined(TCSUPPORT_CPU_AN7552)
static unsigned char pse_cfg_get_oq_en(unsigned char pse_port, unsigned char queue_id);
#endif
/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
static int fe_reg_dump_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    int index = 0;
    off_t begin = 0;
    off_t pos = 0;
    int i = 0, j = 0;

    index += sprintf(buf+index, "FE_DMA_GLO_CFG          (0x%08x) = 0x%08lx\n", FE_DMA_GLO_CFG, read_reg_word(FE_DMA_GLO_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "FE_RST_GLO              (0x%08x) = 0x%08lx\n", FE_RST_GLO, read_reg_word(FE_RST_GLO)); 	
    CHK_BUF();
    index += sprintf(buf+index, "FE_INT_STATUS           (0x%08x) = 0x%08lx\n", FE_INT_STATUS, read_reg_word(FE_INT_STATUS)); 	
    CHK_BUF();
    index += sprintf(buf+index, "FE_INT_Enable           (0x%08x) = 0x%08lx\n", FE_INT_ENABLE, read_reg_word(FE_INT_ENABLE));
    CHK_BUF();
    index += sprintf(buf+index, "FE_FOE_TS_T             (0x%08x) = 0x%08lx\n", FE_FOE_TS, read_reg_word(FE_FOE_TS));
    CHK_BUF();
    index += sprintf(buf+index, "FE_IPv6_EXT             (0x%08x) = 0x%08lx\n", FE_IPV6_EXT, read_reg_word(FE_IPV6_EXT));
    CHK_BUF();
    index += sprintf(buf+index, "FE_CNT_CFG              (0x%08x) = 0x%08lx\n", FE_CNT_CFG, read_reg_word(FE_CNT_CFG));
    CHK_BUF();

#if defined(TCSUPPORT_CPU_EN7580)
    index += sprintf(buf+index, "FE_TAG_CFG              (0x%08x) = 0x%08lx\n", FE_TAG_CFG, read_reg_word(FE_TAG_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "FE_WAN_PORT             (0x%08x) = 0x%08lx\n", FE_WAN_PORT, read_reg_word(FE_WAN_PORT));
    CHK_BUF();
    index += sprintf(buf+index, "FE_WAN_MAC_H            (0x%08x) = 0x%08lx\n", FE_WAN_MAC_H, read_reg_word(FE_WAN_MAC_H));
    CHK_BUF();
    index += sprintf(buf+index, "FE_WAN_MAC_LMIN         (0x%08x) = 0x%08lx\n", FE_WAN_MAC_LMIN, read_reg_word(FE_WAN_MAC_LMIN));
    CHK_BUF();
    index += sprintf(buf+index, "FE_WAN_MAC_LMAX         (0x%08x) = 0x%08lx\n", FE_WAN_MAC_LMAX, read_reg_word(FE_WAN_MAC_LMAX));
    CHK_BUF();
    index += sprintf(buf+index, "FE_LAN_MAC_H            (0x%08x) = 0x%08lx\n", FE_LAN_MAC_H, read_reg_word(FE_LAN_MAC_H));
    CHK_BUF();
    index += sprintf(buf+index, "FE_LAN_MAC_LMIN         (0x%08x) = 0x%08lx\n", FE_LAN_MAC_LMIN, read_reg_word(FE_LAN_MAC_LMIN));
    CHK_BUF();
    index += sprintf(buf+index, "FE_LAN_MAC_LMAX         (0x%08x) = 0x%08lx\n", FE_LAN_MAC_LMAX, read_reg_word(FE_LAN_MAC_LMAX));
    CHK_BUF();
    index += sprintf(buf+index, "FE_CDM_OQ_MAP           (0x%08x) = 0x%08lx\n", FE_CDM_OQ_MAP, read_reg_word(FE_CDM_OQ_MAP));
    CHK_BUF();

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    for(i = 0; i < PSE_PORT_NUM; i++) {
        index += sprintf(buf+index, "P%d_RSV:   ", i);
        CHK_BUF();
        if( (i == 1) || (i == 2) ){
            /* all oqs of port-1/port-2, share one setting */
            index += sprintf(buf+index, "OQx_RSV=0x%04lx", pse_get_oq_rsv(i, 0));
            CHK_BUF();
        }else{
            for(j = 0; j < PSE_QUEUE_NUM[i]; j++) {
                index += sprintf(buf+index, "OQ%d_RSV=0x%04lx", j, pse_get_oq_rsv(i, j));
                CHK_BUF();
                if( j != (PSE_QUEUE_NUM[i]-1)){
                    index += sprintf(buf+index, ", ");
                    CHK_BUF();
                }
            }
        }
        index += sprintf(buf+index, "\n");
        CHK_BUF();
    }
    index += sprintf(buf+index, "P15_RSV:  OQ0_RSV=0x%04lx\n", pse_get_oq_rsv(15, 0));
    CHK_BUF();
    
    index += sprintf(buf+index, "PSE_TOTAL_IQ_OQ_RSV     (0x%08x) = 0x%08lx\n", PSE_OQ_TOTAL_RSV, read_reg_word(PSE_OQ_TOTAL_RSV));
    CHK_BUF();
#else
    for(i = 0; i < PSE_PORT_NUM; i++) {
        index += sprintf(buf+index, "PSE_OQ_RSV_%d            (0x%08x) = 0x%08lx\n", i, PSE_OQ_RSV(i), read_reg_word(PSE_OQ_RSV(i)));
        CHK_BUF();
    }
    index += sprintf(buf+index, "P0_RSV:   OQ0_RSV=0x%04lx, OQ1_RSV=0x%04lx, OQ2_RSV=0x%04lx\n"
        , (read_reg_word(PSE_OQ_RSV0) & 0x3fff), ((read_reg_word(PSE_OQ_RSV0)>>16) & 0x3fff), (read_reg_word(PSE_OQ_RSV1) & 0x3fff));
    CHK_BUF();
    index += sprintf(buf+index, "P1_RSV:   OQx_RSV=0x%04lx\n", ((read_reg_word(PSE_OQ_RSV1)>>16) & 0x3fff));
    CHK_BUF();
    index += sprintf(buf+index, "P2_RSV:   OQx_RSV=0x%04lx\n", (read_reg_word(PSE_OQ_RSV2) & 0x3fff));
    CHK_BUF();
    index += sprintf(buf+index, "P3_RSV:   OQ03_RSV=0x%04lx, OQ4_RSV=0x%04lx\n", ((read_reg_word(PSE_OQ_RSV2)>>16) & 0x3fff), (read_reg_word(PSE_OQ_RSV3) & 0x3fff));
    CHK_BUF();
    index += sprintf(buf+index, "P4_RSV:   OQ0_RSV=0x%04lx, OQ1_RSV=0x%04lx, OQ2_RSV=0x%04lx, OQ4_RSV=0x%04lx\n"
        , (read_reg_word(PSE_OQ_RSV4) & 0x3fff), ((read_reg_word(PSE_OQ_RSV4)>>16) & 0x3fff)
        , (read_reg_word(PSE_OQ_RSV5) & 0x3fff), ((read_reg_word(PSE_OQ_RSV5)>>16) & 0x3fff));
    CHK_BUF();
    index += sprintf(buf+index, "P5_RSV:   OQ0_RSV=0x%04lx, OQ1_RSV=0x%04lx, OQ2_RSV=0x%04lx\n"
        , (read_reg_word(PSE_OQ_RSV6) & 0x3fff), ((read_reg_word(PSE_OQ_RSV6)>>16) & 0x3fff), (read_reg_word(PSE_OQ_RSV7) & 0x3fff));
    CHK_BUF();

    index += sprintf(buf+index, "PSE_TOTAL_IQ_OQ_RSV     (0x%08x) = 0x%08lx\n", PSE_OQ_THD, ((read_reg_word(PSE_OQ_THD)>>16) & 0x3fff));
    CHK_BUF();
#endif

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    index += sprintf(buf+index, "P0_IQ_RSV=0x%04lx, P1_IQ_RSV=0x%04lx, P2_IQ_RSV=0x%04lx, P3_IQ_RSV=0x%04lx, P4_IQ_RSV=0x%04lx\n"
        , pse_get_iq_rsv(0, 0), pse_get_iq_rsv(1, 0), pse_get_iq_rsv(2, 0), pse_get_iq_rsv(3, 0), pse_get_iq_rsv(4, 0));
    CHK_BUF();
    index += sprintf(buf+index, "P5_IQ_RSV=0x%04lx, P6_IQ_RSV=0x%04lx, P7_IQ_RSV=0x%04lx, P8_IQ_RSV=0x%04lx, P9_IQ_RSV=0x%04lx\n"
        , pse_get_iq_rsv(5, 0), pse_get_iq_rsv(6, 0), pse_get_iq_rsv(7, 0), pse_get_iq_rsv(8, 0), pse_get_iq_rsv(9, 0));
    CHK_BUF();
#else
    for(i=0; i<=3; i++) {
        index += sprintf(buf+index, "PSE_IQ_RSV_%d            (0x%08x) = 0x%08lx\n", i, PSE_IQ_RSV(i), read_reg_word(PSE_IQ_RSV(i)));
        CHK_BUF();
    }
    index += sprintf(buf+index, "P0_IQ_RSV=0x%04lx, P1_IQ_RSV=0x%04lx, P2_IQ_RSV=0x%04lx\nP3_IQ_RSV=0x%04lx, P4_IQ_RSV=0x%04lx, P5_IQ_RSV=0x%04lx, P6_IQ_RSV=0x%04lx\n"
        , (read_reg_word(PSE_IQ_RSV0) & 0x3fff), ((read_reg_word(PSE_IQ_RSV0)>>16) & 0x3fff)
        , (read_reg_word(PSE_IQ_RSV1) & 0x3fff), ((read_reg_word(PSE_IQ_RSV1)>>16) & 0x3fff)
        , (read_reg_word(PSE_IQ_RSV2) & 0x3fff), ((read_reg_word(PSE_IQ_RSV2)>>16) & 0x3fff)
        , (read_reg_word(PSE_IQ_RSV3) & 0x3fff));
    CHK_BUF();
#endif

    index += sprintf(buf+index, "PSE_SHARE_USED_LTHD     (0x%08x) = 0x%04lx\n", PSE_SHARE_USED_THD, ((read_reg_word(PSE_SHARE_USED_THD)>>16) & 0x3fff));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_SHARE_USED_HTHD     (0x%08x) = 0x%04lx\n", PSE_SHARE_USED_THD, (read_reg_word(PSE_SHARE_USED_THD) & 0x3fff));
    CHK_BUF();

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    for(i = 0; i < PSE_PORT_NUM; i++) {
        index += sprintf(buf+index, "PSE_PORT%d_OQ_EN         (0x%08x) = 0x%08lx\n", i, PSE_OQUEUE_EN, pse_get_oq_en_by_port(i));
        CHK_BUF();
    }

    index += sprintf(buf+index, "PSE_FC_CFG              (0x%08x) = 0x%08lx\n", PSE_FC_CFG, read_reg_word(PSE_FC_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_MISC_CFG            (0x%08x) = 0x%08lx\n", PSE_MISC_CFG, read_reg_word(PSE_MISC_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_SHARE_BUF_STA       (0x%08x) = 0x%08lx\n", PSE_SHARE_BUF_STA, read_reg_word(PSE_SHARE_BUF_STA));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_PORT_STA            (0x%08x) = 0x%08lx\n", PSE_PORT_STA, read_reg_word(PSE_PORT_STA));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_FQFC_CFG_STA0       (0x%08x) = 0x%08lx\n", PSE_FQFC_CFG_STA0, read_reg_word(PSE_FQFC_CFG_STA0));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_FQFC_CFG_STA1       (0x%08x) = 0x%08lx\n", PSE_FQFC_CFG_STA1, read_reg_word(PSE_FQFC_CFG_STA1));
    CHK_BUF();
    index += sprintf(buf+index, "FE_MISC_CFG             (0x%08x) = 0x%08lx\n", FE_MISC_CFG, read_reg_word(FE_MISC_CFG));
    CHK_BUF();

    for(i = 0; i < PSE_PORT_NUM; i++) {
        index += sprintf(buf+index, "PSE_PORT_Q_USE_STA_%d    (0x%08x) = 0x%08lx\n", i, PSE_PORT_Q_USE_STA(i), read_reg_word(PSE_PORT_Q_USE_STA(i)));
        CHK_BUF();
        index += sprintf(buf+index, "IQ_STA = 0x%04lx, OQ_STA = 0x%04lx\n", 
            ((read_reg_word(PSE_PORT_Q_USE_STA(i))>>16) & 0x7fff), (read_reg_word(PSE_PORT_Q_USE_STA(i)) & 0x7fff));
        CHK_BUF();
    }
#else
    for(i=0; i<=6; i++) {
        index += sprintf(buf+index, "PSE_PORT%d_OQ_EN         (0x%08x) = 0x%08lx\n", i, PSE_PORT_OQ_EN(i), read_reg_word(PSE_PORT_OQ_EN(i)));
        CHK_BUF();
    }

        for(i=0; i<=5; i++) {
            index += sprintf(buf+index, "PSE_FQFC_CFG_STA_%d      (0x%08x) = 0x%08lx\n", i, PSE_FQFC_CFG_STA(i), read_reg_word(PSE_FQFC_CFG_STA(i)));
            CHK_BUF();
        }

    for(i=0; i<=5; i++) {
        index += sprintf(buf+index, "PSE_PORT_OQ_STA_%d       (0x%08x) = 0x%08lx\n", i, PSE_PORT_OQ_STA(i), read_reg_word(PSE_PORT_OQ_STA(i)));
        CHK_BUF();
    }
        
    for(i=0; i<=3; i++) {
        index += sprintf(buf+index, "PSE_PORT_IQ_STA_%d       (0x%08x) = 0x%08lx\n", i, PSE_PORT_IQ_STA(i), read_reg_word(PSE_PORT_IQ_STA(i)));
        CHK_BUF();
    }
#endif

    index += sprintf(buf+index, "PSE_FQ_CFG              (0x%08x) = 0x%08lx\n", PSE_FQ_CFG, read_reg_word(PSE_FQ_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_FQ_STA              (0x%08x) = 0x%08lx\n", PSE_FQ_STA, read_reg_word(PSE_FQ_STA));
    CHK_BUF();

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	index += sprintf(buf+index, "PSE_BUF_USE_REC         (0x%08x) = 0x%08lx\n", PSE_BUF_USE_REC, read_reg_word(PSE_BUF_USE_REC));
    CHK_BUF();
    index += sprintf(buf+index, "FREE_Q_CNT_MIN = 0x%04lx, SHARE_USED_CNT_MAX = 0x%04lx\n", 
        (read_reg_word(PSE_BUF_USE_REC)&0x7fff), ((read_reg_word(PSE_BUF_USE_REC)>>16)&0x7fff));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ_PCNT_REC         (0x%08x) = 0x%08lx\n", PSE_OQ_PCNT_REC, read_reg_word(PSE_OQ_PCNT_REC));
    CHK_BUF();
#else
    index += sprintf(buf+index, "PSE_FQ_REC              (0x%08x) = 0x%08lx\n", PSE_FQ_REC, read_reg_word(PSE_FQ_REC));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ_CNT_REC          (0x%08x) = 0x%08lx\n", PSE_OQ_CNT_REC, read_reg_word(PSE_OQ_CNT_REC));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ_REAL_CNT_REC     (0x%08x) = 0x%08lx\n", PSE_OQ_REAL_CNT_REC, read_reg_word(PSE_OQ_REAL_CNT_REC));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_SHARE_CNT_REC       (0x%08x) = 0x%08lx\n", PSE_SHARE_CNT_REC, read_reg_word(PSE_SHARE_CNT_REC));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_MISC_CFG_1          (0x%08x) = 0x%08lx\n", PSE_MISC_CFG_1, read_reg_word(PSE_MISC_CFG_1));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_MISC_CFG_2          (0x%08x) = 0x%08lx\n", PSE_MISC_CFG_2, read_reg_word(PSE_MISC_CFG_2));
    CHK_BUF();
#endif
    index += sprintf(buf+index, "PSE_PROBE_L             (0x%08x) = 0x%08lx\n", PSE_PROBE_L, read_reg_word(PSE_PROBE_L));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_PROBE_H             (0x%08x) = 0x%08lx\n", PSE_PROBE_H, read_reg_word(PSE_PROBE_H));
    CHK_BUF();

    for(i = 0; i < PSE_PORT_NUM; i++) 
    {
		index += sprintf(buf+index, "PSE_DROP_CNT_%d          (0x%08x) = 0x%08lx\n", i, PSE_DROP_CNT(i), read_reg_word(PSE_DROP_CNT(i)));
		CHK_BUF();
	}

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    for(i=0; i<=7; i++) 
#else
    for(i=0; i<=3; i++) 
#endif
    {
		index += sprintf(buf+index, "QDMA1_FC_MAPPING_%d      (0x%08x) = 0x%08lx\n", i, QDMA1_FC_MAPPING(i), read_reg_word(QDMA1_FC_MAPPING(i)));
		CHK_BUF();
	}

#else
    index += sprintf(buf+index, "PSE_FQFC_CFG            (0x%08x) = 0x%08lx\n", PSE_FQFC_CFG, read_reg_word(PSE_FQFC_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_IQ_REV_VIP          (0x%08x) = 0x%08lx\n", PSE_IQ_REV_VIP, read_reg_word(PSE_IQ_REV_VIP));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_IQ_REV1             (0x%08x) = 0x%08lx\n", PSE_IQ_REV1, read_reg_word(PSE_IQ_REV1));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_IQ_REV2             (0x%08x) = 0x%08lx\n", PSE_IQ_REV2, read_reg_word(PSE_IQ_REV2));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_IQ_STA1             (0x%08x) = 0x%08lx\n", PSE_IQ_STA1, read_reg_word(PSE_IQ_STA1));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_IQ_STA2             (0x%08x) = 0x%08lx\n", PSE_IQ_STA2, read_reg_word(PSE_IQ_STA2));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_IQ PER PORT 0:0x%02lx 1:0x%02lx 2:0x%02lx 5:0x%02lx\n", (read_reg_word(PSE_IQ_STA1) & 0xff)
        , ((read_reg_word(PSE_IQ_STA1)>>8) & 0xff), ((read_reg_word(PSE_IQ_STA1)>>16) & 0xff), ((read_reg_word(PSE_IQ_STA2)>>8) & 0xff));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ_STA1             (0x%08x) = 0x%08lx\n", PSE_OQ_STA1, read_reg_word(PSE_OQ_STA1));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ_STA2             (0x%08x) = 0x%08lx\n", PSE_OQ_STA2, read_reg_word(PSE_OQ_STA2));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ PER PORT 0:0x%02lx 1:0x%02lx 2:0x%02lx 3:0x%02lx 4:0x%02lx 5:0x%02lx 6:0x%02lx\n"
        , (read_reg_word(PSE_OQ_STA1) & 0xff), ((read_reg_word(PSE_OQ_STA1)>>8) & 0xff), ((read_reg_word(PSE_OQ_STA1)>>16) & 0xff)
        , ((read_reg_word(PSE_OQ_STA1)>>24) & 0xff), (read_reg_word(PSE_OQ_STA2) & 0xff), ((read_reg_word(PSE_OQ_STA2)>>8) & 0xff)
        , ((read_reg_word(PSE_OQ_STA2)>>16) & 0xff));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_MISC_CFG            (0x%08x) = 0x%08lx\n", PSE_MISC_CFG, read_reg_word(PSE_MISC_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "FE_PROBE_L              (0x%08x) = 0x%08lx\n", FE_PROBE_L, read_reg_word(FE_PROBE_L));
    CHK_BUF();
    index += sprintf(buf+index, "FE_PROBE_H              (0x%08x) = 0x%08lx\n", FE_PROBE_H, read_reg_word(FE_PROBE_H));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_FQ_MIN              (0x%08x) = 0x%08lx\n", PSE_FQ_MIN, read_reg_word(PSE_FQ_MIN));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_IQ_MAX1             (0x%08x) = 0x%08lx\n", PSE_IQ_MAX1, read_reg_word(PSE_IQ_MAX1));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_IQ_MAX2             (0x%08x) = 0x%08lx\n", PSE_IQ_MAX2, read_reg_word(PSE_IQ_MAX2));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_DROP_COUNT_0        (0x%08x) = 0x%08lx\n", PSE_DROP_COUNT_0, read_reg_word(PSE_DROP_COUNT_0));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_DROP_COUNT_1        (0x%08x) = 0x%08lx\n", PSE_DROP_COUNT_1, read_reg_word(PSE_DROP_COUNT_1));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_DROP_COUNT_2        (0x%08x) = 0x%08lx\n", PSE_DROP_COUNT_2, read_reg_word(PSE_DROP_COUNT_2));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_DROP_COUNT_4        (0x%08x) = 0x%08lx\n", PSE_DROP_COUNT_4, read_reg_word(PSE_DROP_COUNT_4));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_DROP_COUNT_5        (0x%08x) = 0x%08lx\n", PSE_DROP_COUNT_5, read_reg_word(PSE_DROP_COUNT_5));
    CHK_BUF();
#endif

/* L2u, L2Br, VIP do not dump */


	/* CDMA1 Reg */
    index += sprintf(buf+index, "CDMA_VLAN_CTRL            (0x%08x) = 0x%08lx\n", CDMA1_VLAN_CTRL, read_reg_word(CDMA1_VLAN_CTRL));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA_PPP_GEN              (0x%08x) = 0x%08lx\n", CDMA1_PPP_GEN, read_reg_word(CDMA1_PPP_GEN));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA_FWD_CFG              (0x%08x) = 0x%08lx\n", CDMA1_FWD_CFG, read_reg_word(CDMA1_FWD_CFG));
    CHK_BUF();
    if((isEN751627) || (CRSN_QSEL_SUPPORT)) {   
        for(i=0; i<4; i++) {
            index += sprintf(buf+index, "CDMA1_CRSN_QSEL%d          (0x%08x) = 0x%08lx\n", i, CDMA1_CRSN_QSEL(i), read_reg_word(CDMA1_CRSN_QSEL(i)));
            CHK_BUF();
        }
    } else {
        index += sprintf(buf+index, "CDMA_CRSN_QSEL0            (0x%08x) = 0x%08lx\n", CDMA1_CRSN_QSEL0, read_reg_word(CDMA1_CRSN_QSEL0));
        CHK_BUF();
        index += sprintf(buf+index, "CDMA_CRSN_QSEL1            (0x%08x) = 0x%08lx\n", CDMA1_CRSN_QSEL1, read_reg_word(CDMA1_CRSN_QSEL1));
        CHK_BUF();
    }
	index += sprintf(buf+index, "CDMA_HWF_CHN_EN           (0x%08x) = 0x%08lx\n", CDMA1_HWF_CHN_EN, read_reg_word(CDMA1_HWF_CHN_EN));
    CHK_BUF();

    /* GDMA1 Reg */
    index += sprintf(buf+index, "GDMA1_FWD_CFG             (0x%08x) = 0x%08lx\n", GDMA1_FWD_CFG, read_reg_word(GDMA1_FWD_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_SHRP_CFG            (0x%08x) = 0x%08lx\n", GDMA1_SHRP_CFG, read_reg_word(GDMA1_SHRP_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_MAC_ADRL            (0x%08x) = 0x%08lx\n", GDMA1_MAC_ADRL, read_reg_word(GDMA1_MAC_ADRL));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_MAC_ADRH            (0x%08x) = 0x%08lx\n", GDMA1_MAC_ADRH, read_reg_word(GDMA1_MAC_ADRH));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_VLAN_CHECK          (0x%08x) = 0x%08lx\n", GDMA1_VLAN_CHECK, read_reg_word(GDMA1_VLAN_CHECK));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_LEN_CFG             (0x%08x) = 0x%08lx\n", GDMA1_LEN_CFG, read_reg_word(GDMA1_LEN_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_LAN_PCP             (0x%08x) = 0x%08lx\n", GDMA1_LAN_PCP, read_reg_word(GDMA1_LAN_PCP));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_LPBK_CFG            (0x%08x) = 0x%08lx\n", GDMA1_LPBK_CFG, read_reg_word(GDMA1_LPBK_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_CHN_RLS             (0x%08x) = 0x%08lx\n", GDMA1_CHN_RLS, read_reg_word(GDMA1_CHN_RLS));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_TXCHN_EN            (0x%08x) = 0x%08lx\n", GDMA1_TXCHN_EN, read_reg_word(GDMA1_TXCHN_EN));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_RXCHN_EN            (0x%08x) = 0x%08lx\n", GDMA1_RXCHN_EN, read_reg_word(GDMA1_RXCHN_EN));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_RXCHN_FP            (0x%08x) = 0x%08lx\n", GDMA1_RXCHN_FP, read_reg_word(GDMA1_RXCHN_FP));
    CHK_BUF();
    index += sprintf(buf+index, "FE_CPORT_CFG              (0x%08x) = 0x%08lx\n", FE_CPORT_CFG, read_reg_word(FE_CPORT_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "FE_CPORT_CHN_MAP          (0x%08x) = 0x%08lx\n", FE_CPORT_CHN_MAP, read_reg_word(FE_CPORT_CHN_MAP));
    CHK_BUF();
    index += sprintf(buf+index, "FE_CPORT_SHRP_CFG         (0x%08x) = 0x%08lx\n", FE_CPORT_SHRP_CFG, read_reg_word(FE_CPORT_SHRP_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDM1_TX_CHN_VLD           (0x%08x) = 0x%08lx\n", GDMA1_TX_CHN_VLD, read_reg_word(GDMA1_TX_CHN_VLD));
    CHK_BUF();
    index += sprintf(buf+index, "GDM1_RX_CHN_VLD           (0x%08x) = 0x%08lx\n", GDMA1_RX_CHN_VLD, read_reg_word(GDMA1_RX_CHN_VLD));
    CHK_BUF();

    	
    index += sprintf(buf+index, "CDMA1_TX_OK_CNT           (0x%08x) = 0x%08lx\n", CDMA1_TX_OK_CNT, read_reg_word(CDMA1_TX_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXCPU_OK_CNT        (0x%08x) = 0x%08lx\n", CDMA1_RXCPU_OK_CNT, read_reg_word(CDMA1_RXCPU_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXHWF_OK_CNT        (0x%08x) = 0x%08lx\n", CDMA1_RXHWF_OK_CNT, read_reg_word(CDMA1_RXHWF_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXCPU_KA_CNT        (0x%08x) = 0x%08lx\n", CDMA1_RXCPU_KA_CNT, read_reg_word(CDMA1_RXCPU_KA_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXCPU_DROP_CNT      (0x%08x) = 0x%08lx\n", CDMA1_RXCPU_DROP_CNT, read_reg_word(CDMA1_RXCPU_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXHWF_DROP_CNT      (0x%08x) = 0x%08lx\n", CDMA1_RXHWF_DROP_CNT, read_reg_word(CDMA1_RXHWF_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXCPU0_OK_CNT       (0x%08x) = 0x%08lx\n", CDMA1_RXCPU0_OK_CNT, read_reg_word(CDMA1_RXCPU0_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXCPU1_OK_CNT       (0x%08x) = 0x%08lx\n", CDMA1_RXCPU1_OK_CNT, read_reg_word(CDMA1_RXCPU1_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXHWF_FAST_ALL_CNT  (0x%08x) = 0x%08lx\n", CDMA1_RXHWF_FAST_ALL_CNT, read_reg_word(CDMA1_RXHWF_FAST_ALL_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXCPU0_DROP_CNT     (0x%08x) = 0x%08lx\n", CDMA1_RXCPU0_DROP_CNT, read_reg_word(CDMA1_RXCPU0_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXCPU1_DROP_CNT     (0x%08x) = 0x%08lx\n", CDMA1_RXCPU1_DROP_CNT, read_reg_word(CDMA1_RXCPU1_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA1_RXHWF_FAST_DROP_CNT (0x%08x) = 0x%08lx\n", CDMA1_RXHWF_FAST_DROP_CNT, read_reg_word(CDMA1_RXHWF_FAST_DROP_CNT));
    CHK_BUF();

    index += sprintf(buf+index, "GDMA1_MIB_CLER            (0x%08x) = Write Only\n", GDMA1_MIB_CLER);
    CHK_BUF();
    /* GDMA1 count display */
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	index += sprintf(buf+index, "GDMA1_TX_GET_CNT          (0x%08x) = 0x%08lx\n", GDMA1_TX_GET_CNT , read_reg_word(GDMA1_TX_GET_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_TX_OK_CNT_L         (0x%08x) = 0x%08lx\n", GDMA1_TX_OK_CNT , read_reg_word(GDMA1_TX_OK_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_TX_OK_CNT_H         (0x%08x) = 0x%08lx\n", GDMA1_TX_OK_CNT_H, read_reg_word(GDMA1_TX_OK_CNT_H));
	CHK_BUF();
#else
    index += sprintf(buf+index, "GDMA1_TX_GET_CNT          (0x%08x) = 0x%08lx\n", GDMA1_TX_GET_CNT , read_reg_word(GDMA1_TX_GET_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_TX_OK_CNT           (0x%08x) = 0x%08lx\n", GDMA1_TX_OK_CNT, read_reg_word(GDMA1_TX_OK_CNT));
    CHK_BUF();
#endif
    index += sprintf(buf+index, "GDMA1_TX_DROP_CNT         (0x%08x) = 0x%08lx\n", GDMA1_TX_DROP_CNT, read_reg_word(GDMA1_TX_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_TX_OK_BYTE_CNT      (0x%08x) = 0x%08lx\n", GDMA1_TX_OK_BYTE_CNT, read_reg_word(GDMA1_TX_OK_BYTE_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_RX_OK_CNT           (0x%08x) = 0x%08lx\n", GDMA1_RX_OK_CNT , read_reg_word(GDMA1_RX_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_RX_FC_DROP_CNT      (0x%08x) = 0x%08lx\n", GDMA1_RX_FC_DROP_CNT, read_reg_word(GDMA1_RX_FC_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_RX_RC_DROP_CNT      (0x%08x) = 0x%08lx\n", GDMA1_RX_RC_DROP_CNT, read_reg_word(GDMA1_RX_RC_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_RX_OVER_DROP_CNT    (0x%08x) = 0x%08lx\n", GDMA1_RX_OVER_DROP_CNT, read_reg_word(GDMA1_RX_OVER_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_RX_ERROR_DROP_CNT   (0x%08x) = 0x%08lx\n", GDMA1_RX_ERROR_DROP_CNT, read_reg_word(GDMA1_RX_ERROR_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA1_RX_BYTECNT          (0x%08x) = 0x%08lx\n", GDMA1_RX_BYTECNT, read_reg_word(GDMA1_RX_BYTECNT));
    CHK_BUF();


    /* CDMA2 */ 
    index += sprintf(buf+index, "CDMA2_VLAN_CTRL           (0x%08x) = 0x%08lx\n", CDMA2_VLAN_CTRL, read_reg_word(CDMA2_VLAN_CTRL));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_PPP_GEN             (0x%08x) = 0x%08lx\n", CDMA2_PPP_GEN, read_reg_word(CDMA2_PPP_GEN));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_FWD_CFG             (0x%08x) = 0x%08lx\n", CDMA2_FWD_CFG , read_reg_word(CDMA2_FWD_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_HWF_CHN_EN          (0x%08x) = 0x%08lx\n", CDMA2_HWF_CHN_EN, read_reg_word(CDMA2_HWF_CHN_EN));
    CHK_BUF();
    if(CRSN_QSEL_SUPPORT) {        
    	for(i=0; i<4; i++) {
    		index += sprintf(buf+index, "CDMA2_CRSN_QSEL%d          (0x%08x) = 0x%08lx\n", i, CDMA2_CRSN_QSEL(i), read_reg_word(CDMA2_CRSN_QSEL(i)));
    		CHK_BUF();
    	}
    } else {
        index += sprintf(buf+index, "CDMA2_CRSN_QSEL0          (0x%08x) = 0x%08lx\n", CDMA2_CRSN_QSEL0, read_reg_word(CDMA2_CRSN_QSEL0));
        CHK_BUF();
        index += sprintf(buf+index, "CDMA2_CRSN_QSEL1          (0x%08x) = 0x%08lx\n", CDMA2_CRSN_QSEL1, read_reg_word(CDMA2_CRSN_QSEL1));
        CHK_BUF();
    }

    /* GDMA2 */
    index += sprintf(buf+index, "GDMA2_FWD_CFG             (0x%08x) = 0x%08lx\n", GDMA2_FWD_CFG, read_reg_word(GDMA2_FWD_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_SHRP_CFG            (0x%08x) = 0x%08lx\n", GDMA2_SHRP_CFG, read_reg_word(GDMA2_SHRP_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_MAC_ADRL            (0x%08x) = 0x%08lx\n", GDMA2_MAC_ADRL, read_reg_word(GDMA2_MAC_ADRL));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_MAC_ADRH            (0x%08x) = 0x%08lx\n", GDMA2_MAC_ADRH, read_reg_word(GDMA2_MAC_ADRH));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_VLAN_CHECK          (0x%08x) = 0x%08lx\n", GDMA2_VLAN_CHECK, read_reg_word(GDMA2_VLAN_CHECK));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_LEN_CFG             (0x%08x) = 0x%08lx\n", GDMA2_LEN_CFG, read_reg_word(GDMA2_LEN_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "FE_WAN_PCP                (0x%08x) = 0x%08lx\n", GDMA2_WAN_PCP, read_reg_word(GDMA2_WAN_PCP));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_LPBP_CFG            (0x%08x) = 0x%08lx\n", GDMA2_LPBP_CFG, read_reg_word(GDMA2_LPBP_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_CHN_RLS             (0x%08x) = 0x%08lx\n", GDMA2_CHN_RLS, read_reg_word(GDMA2_CHN_RLS));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_TXCHN_EN            (0x%08x) = 0x%08lx\n", GDMA2_TXCHN_EN, read_reg_word(GDMA2_TXCHN_EN));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_RXCHN_EN            (0x%08x) = 0x%08lx\n", GDMA2_RXCHN_EN, read_reg_word(GDMA2_RXCHN_EN));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_RXCHN_FP            (0x%08x) = 0x%08lx\n", GDMA2_RXCHN_FP, read_reg_word(GDMA2_RXCHN_FP));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_RXUC_SHPR_CFG       (0x%08x) = 0x%08lx\n", GDMA2_RXUC_SHPR_CFG, read_reg_word(GDMA2_RXUC_SHPR_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_RXBC_SHPR_CFG       (0x%08x) = 0x%08lx\n", GDMA2_RXBC_SHPR_CFG, read_reg_word(GDMA2_RXBC_SHPR_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_RXMC_SHPR_CFG       (0x%08x) = 0x%08lx\n", GDMA2_RXMC_SHPR_CFG, read_reg_word(GDMA2_RXMC_SHPR_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_RXOC_SHPR_CFG       (0x%08x) = 0x%08lx\n", GDMA2_RXOC_SHPR_CFG, read_reg_word(GDMA2_RXOC_SHPR_CFG));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_TX_CHN_VLD          (0x%08x) = 0x%08lx\n", GDMA2_TX_CHN_VLD, read_reg_word(GDMA2_TX_CHN_VLD));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_RX_CHN_VLD          (0x%08x) = 0x%08lx\n", GDMA2_RX_CHN_VLD, read_reg_word(GDMA2_RX_CHN_VLD));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_TX_OK_CNT           (0x%08x) = 0x%08lx\n", CDMA2_TX_OK_CNT, read_reg_word(CDMA2_TX_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXCPU_OK_CNT        (0x%08x) = 0x%08lx\n", CDMA2_RXCPU_OK_CNT, read_reg_word(CDMA2_RXCPU_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXHWF_OK_CNT        (0x%08x) = 0x%08lx\n", CDMA2_RXHWF_OK_CNT, read_reg_word(CDMA2_RXHWF_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXCPU_KA_CNT        (0x%08x) = 0x%08lx\n", CDMA2_RXCPU_KA_CNT, read_reg_word(CDMA2_RXCPU_KA_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXCPU_DROP_CNT      (0x%08x) = 0x%08lx\n", CDMA2_RXCPU_DROP_CNT, read_reg_word(CDMA2_RXCPU_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXHWF_DROP_CNT      (0x%08x) = 0x%08lx\n", CDMA2_RXHWF_DROP_CNT, read_reg_word(CDMA2_RXHWF_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXCPU0_OK_CNT       (0x%08x) = 0x%08lx\n", CDMA2_RXCPU0_OK_CNT, read_reg_word(CDMA2_RXCPU0_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXCPU1_OK_CNT       (0x%08x) = 0x%08lx\n", CDMA2_RXCPU1_OK_CNT, read_reg_word(CDMA2_RXCPU1_OK_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXHWF_FAST_ALL_CNT  (0x%08x) = 0x%08lx\n", CDMA2_RXHWF_FAST_ALL_CNT, read_reg_word(CDMA2_RXHWF_FAST_ALL_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXCPU0_DROP_CNT     (0x%08x) = 0x%08lx\n", CDMA2_RXCPU0_DROP_CNT, read_reg_word(CDMA2_RXCPU0_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXCPU1_DROP_CNT     (0x%08x) = 0x%08lx\n", CDMA2_RXCPU1_DROP_CNT, read_reg_word(CDMA2_RXCPU1_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "CDMA2_RXHWF_FAST_DROP_CNT (0x%08x) = 0x%08lx\n", CDMA2_RXHWF_FAST_DROP_CNT, read_reg_word(CDMA2_RXHWF_FAST_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA2_MIB_CLER            (0x%08x) = Write Only\n", GDMA2_MIB_CLER);
    CHK_BUF();


    /* GDMA2 count display */
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	index += sprintf(buf+index, "GDMA2_TX_GETCNT           (0x%08x) = 0x%08lx\n", GDMA2_TX_GETCNT , read_reg_word(GDMA2_TX_GETCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_TX_OKCNT_L          (0x%08x) = 0x%08lx\n", GDMA2_TX_OKCNT , read_reg_word(GDMA2_TX_OKCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_TX_OKCNT_H          (0x%08x) = 0x%08lx\n", GDMA2_TX_OKCNT_H, read_reg_word(GDMA2_TX_OKCNT_H));
	CHK_BUF();
#else
    /* GDMA2_TX_GETCNT */
    index += sprintf(buf+index, "GDMA2_TX_GETCNT           (0x%08x) = 0x%08lx\n", GDMA2_TX_GETCNT, read_reg_word(GDMA2_TX_GETCNT));
    CHK_BUF();
    /* GDMA2_TX_GOKCNT */
    index += sprintf(buf+index, "GDMA2_TX_GOKCNT           (0x%08x) = 0x%08lx\n", GDMA2_TX_OKCNT, read_reg_word(GDMA2_TX_OKCNT));
    CHK_BUF();
#endif
    /* GDMA2_TX_DROPCNT */	
    index += sprintf(buf+index, "GDMA2_TX_DROPCNT          (0x%08x) = 0x%08lx\n", GDMA2_TX_DROPCNT, read_reg_word(GDMA2_TX_DROPCNT));
    CHK_BUF();
    /* GDMA2_TX_OKBYTESCNT */
    /* wait polling mod */
    index += sprintf(buf+index, "GDMA2_TX_OKBYTE_CNT       (0x%08x) = 0x%08lx\n", GDMA2_TX_OKBYTE_CNT, read_reg_word(GDMA2_TX_OKBYTE_CNT));
    CHK_BUF();
    /* GDMA2_RX_OKCNT */
    index += sprintf(buf+index, "GDMA2_RX_OKCNT            (0x%08x) = 0x%08lx\n", GDMA2_RX_OKCNT, read_reg_word(GDMA2_RX_OKCNT));
    CHK_BUF();

    /* GDMA2_RX_FLOWCTRLDROPCNT */
    index += sprintf(buf+index, "GDMA2_RX_FCDROPCNT        (0x%08x) = 0x%08lx\n", GDMA2_RX_FCDROPCNT, read_reg_word(GDMA2_RX_FCDROPCNT));
    CHK_BUF();
    /* GDMA2_RX_RATECTRLDROPCNT */
    index += sprintf(buf+index, "GDMA2_RX_RCDROPCNT        (0x%08x) = 0x%08lx\n", GDMA2_RX_RCDROPCNT, read_reg_word(GDMA2_RX_RCDROPCNT));
    CHK_BUF();

    /* GDMA2_RX_OVDROPCNT */		
    index += sprintf(buf+index, "GDMA2_RX_OVDROPCNT        (0x%08x) = 0x%08lx\n", GDMA2_RX_OVDROPCNT, read_reg_word(GDMA2_RX_OVDROPCNT));
    CHK_BUF();
    /* GDMA2_RX_ERRDROPCNT */
    	
    index += sprintf(buf+index, "GDMA2_RX_ERRDROPCNT       (0x%08x) = 0x%08lx\n", GDMA2_RX_ERRDROPCNT, read_reg_word(GDMA2_RX_ERRDROPCNT));
    CHK_BUF();
    /* GDMA2_RX_RXOKBYTECNT */	
    index += sprintf(buf+index, "GDMA2_RX_OKBYTECNT        (0x%08x) = 0x%08lx\n", GDMA2_RX_OKBYTECNT, read_reg_word(GDMA2_RX_OKBYTECNT));
    CHK_BUF();


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


static int fe_debug_reg_dump_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    int index = 0;
    off_t begin = 0;
    off_t pos = 0;
    int i = 0;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
    index += sprintf(buf+index, "PSE_DROP_CNT:\n");
    CHK_BUF();
    for(i = 0; i < PSE_PORT_NUM; i++) 
    {
        index += sprintf(buf+index, "P%i:0x%08lx  ", i, read_reg_word(PSE_DROP_CNT(i)));
        CHK_BUF();
        if( (i%4) == 3 ){
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }

    index += sprintf(buf+index, "\nPSE_IQ_CNT:    P0:0x%02lx  P1:0x%02lx  P2:0x%02lx  P3:0x%02lx  P4:0x%02lx  P5:0x%02lx\n"
        , ((read_reg_word(PSE_PORT_Q_USE_STA0)>>16) & 0x7fff), ((read_reg_word(PSE_PORT_Q_USE_STA1)>>16) & 0x7fff)
        , ((read_reg_word(PSE_PORT_Q_USE_STA2)>>16) & 0x7fff), ((read_reg_word(PSE_PORT_Q_USE_STA3)>>16) & 0x7fff)
        , ((read_reg_word(PSE_PORT_Q_USE_STA4)>>16) & 0x7fff), ((read_reg_word(PSE_PORT_Q_USE_STA5)>>16) & 0x7fff));
    CHK_BUF();
	index += sprintf(buf+index, "               P6:0x%02lx  P7:0x%02lx  P8:0x%02lx  P9:0x%02lx\n"
        , ((read_reg_word(PSE_PORT_Q_USE_STA6)>>16) & 0x7fff), ((read_reg_word(PSE_PORT_Q_USE_STA7)>>16) & 0x7fff)
        , ((read_reg_word(PSE_PORT_Q_USE_STA8)>>16) & 0x7fff), ((read_reg_word(PSE_PORT_Q_USE_STA9)>>16) & 0x7fff));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ_CNT:    P0:0x%02lx  P1:0x%02lx  P2:0x%02lx  P3:0x%02lx  P4:0x%02lx  P5:0x%02lx\n"
        , (read_reg_word(PSE_PORT_Q_USE_STA0) & 0x7fff), (read_reg_word(PSE_PORT_Q_USE_STA1) & 0x7fff)
        , (read_reg_word(PSE_PORT_Q_USE_STA2) & 0x7fff), (read_reg_word(PSE_PORT_Q_USE_STA3) & 0x7fff)
        , (read_reg_word(PSE_PORT_Q_USE_STA4) & 0x7fff), (read_reg_word(PSE_PORT_Q_USE_STA5) & 0x7fff));
    CHK_BUF(); 
	index += sprintf(buf+index, "               P6:0x%02lx  P7:0x%02lx  P8:0x%02lx  P9:0x%02lx\n"
        , (read_reg_word(PSE_PORT_Q_USE_STA6) & 0x7fff), (read_reg_word(PSE_PORT_Q_USE_STA7) & 0x7fff)
        , (read_reg_word(PSE_PORT_Q_USE_STA8) & 0x7fff), (read_reg_word(PSE_PORT_Q_USE_STA9) & 0x7fff));
    CHK_BUF(); 
    index += sprintf(buf+index, "PSE_SHARE_BUF:  SHARED_USED_CNT:0x%04lx  SHARED_FREE_CNT:0x%04lx\n"
        , ((read_reg_word(PSE_SHARE_BUF_STA)>>16) & 0x7fff), (read_reg_word(PSE_SHARE_BUF_STA) & 0x7fff));
    CHK_BUF();
#elif defined(TCSUPPORT_CPU_EN7580)
    index += sprintf(buf+index, "PSE_DROP_CNT:\nP0:0x%08lx  P1:0x%08lx  P2:0x%08lx  P3:0x%08lx\n", read_reg_word(PSE_DROP_CNT(0))
        , read_reg_word(PSE_DROP_CNT(1)), read_reg_word(PSE_DROP_CNT(2)), read_reg_word(PSE_DROP_CNT(3)));
    CHK_BUF();
    index += sprintf(buf+index, "P4:0x%08lx  P5:0x%08lx  P6:0x%08lx\n", read_reg_word(PSE_DROP_CNT(4))
        , read_reg_word(PSE_DROP_CNT(5)), read_reg_word(PSE_DROP_CNT(6)));
    CHK_BUF();
    
    index += sprintf(buf+index, "\nPSE_IQ_CNT:    P0:0x%02lx  P1:0x%02lx  P2:0x%02lx  P3:0x%02lx  P4:0x%02lx  P5:0x%02lx\n"
        , (read_reg_word(PSE_PORT_IQ_STA0) & 0x3fff), ((read_reg_word(PSE_PORT_IQ_STA0)>>16) & 0x3fff)
        , (read_reg_word(PSE_PORT_IQ_STA1) & 0x3fff), ((read_reg_word(PSE_PORT_IQ_STA1)>>16) & 0x3fff)
        , (read_reg_word(PSE_PORT_IQ_STA2) & 0x3fff), ((read_reg_word(PSE_PORT_IQ_STA2)>>16) & 0x3fff));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ_CNT:    P0:0x%02lx  P1:0x%02lx  P2:0x%02lx  P3:0x%02lx  P4:0x%02lx  P5:0x%02lx\n"
        , (read_reg_word(PSE_PORT_OQ_STA0) & 0x3fff), ((read_reg_word(PSE_PORT_OQ_STA0)>>16) & 0x3fff)
        , (read_reg_word(PSE_PORT_OQ_STA1) & 0x3fff), ((read_reg_word(PSE_PORT_OQ_STA1)>>16) & 0x3fff)
        , (read_reg_word(PSE_PORT_OQ_STA2) & 0x3fff), ((read_reg_word(PSE_PORT_OQ_STA2)>>16) & 0x3fff));
    CHK_BUF(); 
    index += sprintf(buf+index, "PSE_SHARE_BUF:  SHARED_USED_CNT:0x%04lx  SHARED_FREE_CNT:0x%04lx\n"
        , ((read_reg_word(PSE_FQFC_CFG_STA2)>>16) & 0x7fff), (read_reg_word(PSE_FQFC_CFG_STA2) & 0x7fff));
    CHK_BUF();
#else
    index += sprintf(buf+index, "PSE_IQ_STA1               (0x%08x) = 0x%08lx\n", PSE_IQ_STA1, read_reg_word(PSE_IQ_STA1));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_IQ_STA2               (0x%08x) = 0x%08lx\n", PSE_IQ_STA2, read_reg_word(PSE_IQ_STA2));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ_STA1               (0x%08x) = 0x%08lx\n", PSE_OQ_STA1, read_reg_word(PSE_OQ_STA1));
    CHK_BUF();
    index += sprintf(buf+index, "PSE_OQ_STA2               (0x%08x) = 0x%08lx\n", PSE_OQ_STA2, read_reg_word(PSE_OQ_STA2));
    CHK_BUF();
#endif
        
	index += sprintf(buf+index, "\nCDMA1_TX_OK_CNT           (0x%08x) = 0x%08lx\n", CDMA1_TX_OK_CNT, read_reg_word(CDMA1_TX_OK_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "CDMA1_RXCPU_OK_CNT        (0x%08x) = 0x%08lx\n", CDMA1_RXCPU_OK_CNT, read_reg_word(CDMA1_RXCPU_OK_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "CDMA1_RXHWF_OK_CNT        (0x%08x) = 0x%08lx\n", CDMA1_RXHWF_OK_CNT, read_reg_word(CDMA1_RXHWF_OK_CNT));
	CHK_BUF();
#if defined(TCSUPPORT_CPU_EN7580)
	index += sprintf(buf+index, "CDMA1_RXHWF_FAST_OK_CNT   (0x%08x) = 0x%08lx\n", CDMA1_RXHWF_FAST_ALL_CNT, read_reg_word(CDMA1_RXHWF_FAST_ALL_CNT));
	CHK_BUF();
#endif
	index += sprintf(buf+index, "CDMA1_RXCPU_DROP_CNT      (0x%08x) = 0x%08lx\n", CDMA1_RXCPU_DROP_CNT, read_reg_word(CDMA1_RXCPU_DROP_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "CDMA1_RXHWF_DROP_CNT      (0x%08x) = 0x%08lx\n", CDMA1_RXHWF_DROP_CNT, read_reg_word(CDMA1_RXHWF_DROP_CNT));
	CHK_BUF();
#if defined(TCSUPPORT_CPU_EN7580)
	index += sprintf(buf+index, "CDMA1_RXHWF_FAST_DROP_CNT (0x%08x) = 0x%08lx\n", CDMA1_RXHWF_FAST_DROP_CNT, read_reg_word(CDMA1_RXHWF_FAST_DROP_CNT));
	CHK_BUF();
#endif

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	index += sprintf(buf+index, "\nGDMA1_TX_GET_CNT          (0x%08x) = 0x%08lx\n", GDMA1_TX_GET_CNT , read_reg_word(GDMA1_TX_GET_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_TX_OK_CNT_L         (0x%08x) = 0x%08lx\n", GDMA1_TX_OK_CNT , read_reg_word(GDMA1_TX_OK_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_TX_OK_CNT_H         (0x%08x) = 0x%08lx\n", GDMA1_TX_OK_CNT_H, read_reg_word(GDMA1_TX_OK_CNT_H));
	CHK_BUF();
#else
	index += sprintf(buf+index, "\nGDMA1_TX_GET_CNT          (0x%08x) = 0x%08lx\n", GDMA1_TX_GET_CNT , read_reg_word(GDMA1_TX_GET_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_TX_OK_CNT           (0x%08x) = 0x%08lx\n", GDMA1_TX_OK_CNT, read_reg_word(GDMA1_TX_OK_CNT));
	CHK_BUF();
#endif
	index += sprintf(buf+index, "GDMA1_TX_DROP_CNT         (0x%08x) = 0x%08lx\n", GDMA1_TX_DROP_CNT, read_reg_word(GDMA1_TX_DROP_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_RX_OK_CNT           (0x%08x) = 0x%08lx\n", GDMA1_RX_OK_CNT , read_reg_word(GDMA1_RX_OK_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_RX_FC_DROP_CNT      (0x%08x) = 0x%08lx\n", GDMA1_RX_FC_DROP_CNT, read_reg_word(GDMA1_RX_FC_DROP_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_RX_RC_DROP_CNT      (0x%08x) = 0x%08lx\n", GDMA1_RX_RC_DROP_CNT, read_reg_word(GDMA1_RX_RC_DROP_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_RX_OVER_DROP_CNT    (0x%08x) = 0x%08lx\n", GDMA1_RX_OVER_DROP_CNT, read_reg_word(GDMA1_RX_OVER_DROP_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA1_RX_ERROR_DROP_CNT   (0x%08x) = 0x%08lx\n", GDMA1_RX_ERROR_DROP_CNT, read_reg_word(GDMA1_RX_ERROR_DROP_CNT));
	CHK_BUF();

    
	index += sprintf(buf+index, "\nCDMA2_TX_OK_CNT           (0x%08x) = 0x%08lx\n", CDMA2_TX_OK_CNT, read_reg_word(CDMA2_TX_OK_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "CDMA2_RXCPU_OK_CNT        (0x%08x) = 0x%08lx\n", CDMA2_RXCPU_OK_CNT, read_reg_word(CDMA2_RXCPU_OK_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "CDMA2_RXHWF_OK_CNT        (0x%08x) = 0x%08lx\n", CDMA2_RXHWF_OK_CNT, read_reg_word(CDMA2_RXHWF_OK_CNT));
	CHK_BUF();
#if defined(TCSUPPORT_CPU_EN7580)
	index += sprintf(buf+index, "CDMA2_RXHWF_FAST_OK_CNT   (0x%08x) = 0x%08lx\n", CDMA2_RXHWF_FAST_ALL_CNT, read_reg_word(CDMA2_RXHWF_FAST_ALL_CNT));
	CHK_BUF();
#endif
	index += sprintf(buf+index, "CDMA2_RXCPU_DROP_CNT      (0x%08x) = 0x%08lx\n", CDMA2_RXCPU_DROP_CNT, read_reg_word(CDMA2_RXCPU_DROP_CNT));
	CHK_BUF();
	index += sprintf(buf+index, "CDMA2_RXHWF_DROP_CNT      (0x%08x) = 0x%08lx\n", CDMA2_RXHWF_DROP_CNT, read_reg_word(CDMA2_RXHWF_DROP_CNT));
	CHK_BUF();
#if defined(TCSUPPORT_CPU_EN7580)
	index += sprintf(buf+index, "CDMA2_RXHWF_FAST_DROP_CNT (0x%08x) = 0x%08lx\n", CDMA2_RXHWF_FAST_DROP_CNT, read_reg_word(CDMA2_RXHWF_FAST_DROP_CNT));
	CHK_BUF();
#endif

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	index += sprintf(buf+index, "\nGDMA2_TX_GETCNT           (0x%08x) = 0x%08lx\n", GDMA2_TX_GETCNT , read_reg_word(GDMA2_TX_GETCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_TX_OKCNT_L          (0x%08x) = 0x%08lx\n", GDMA2_TX_OKCNT , read_reg_word(GDMA2_TX_OKCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_TX_OKCNT_H          (0x%08x) = 0x%08lx\n", GDMA2_TX_OKCNT_H, read_reg_word(GDMA2_TX_OKCNT_H));
	CHK_BUF();
#else
	index += sprintf(buf+index, "\nGDMA2_TX_GETCNT           (0x%08x) = 0x%08lx\n", GDMA2_TX_GETCNT, read_reg_word(GDMA2_TX_GETCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_TX_GOKCNT           (0x%08x) = 0x%08lx\n", GDMA2_TX_OKCNT, read_reg_word(GDMA2_TX_OKCNT));
	CHK_BUF();
#endif
	index += sprintf(buf+index, "GDMA2_TX_DROPCNT          (0x%08x) = 0x%08lx\n", GDMA2_TX_DROPCNT, read_reg_word(GDMA2_TX_DROPCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_RX_OKCNT            (0x%08x) = 0x%08lx\n", GDMA2_RX_OKCNT, read_reg_word(GDMA2_RX_OKCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_RX_FCDROPCNT        (0x%08x) = 0x%08lx\n", GDMA2_RX_FCDROPCNT, read_reg_word(GDMA2_RX_FCDROPCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_RX_RCDROPCNT        (0x%08x) = 0x%08lx\n", GDMA2_RX_RCDROPCNT, read_reg_word(GDMA2_RX_RCDROPCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_RX_OVDROPCNT        (0x%08x) = 0x%08lx\n", GDMA2_RX_OVDROPCNT, read_reg_word(GDMA2_RX_OVDROPCNT));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA2_RX_ERRDROPCNT       (0x%08x) = 0x%08lx\n", GDMA2_RX_ERRDROPCNT, read_reg_word(GDMA2_RX_ERRDROPCNT));
	CHK_BUF();

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_EN7523)
	if( dbgLevel>=1 ) {
		index += sprintf(buf+index, "\nGDMA3_TX_GETCNT           (0x%08x) = 0x%08lx\n", GDMA3_TX_GETCNT, read_reg_word(GDMA3_TX_GETCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA3_TX_GOKCNT           (0x%08x) = 0x%08lx\n", GDMA3_TX_OKCNT_L, read_reg_word(GDMA3_TX_OKCNT_L));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA3_TX_DROPCNT          (0x%08x) = 0x%08lx\n", GDMA3_TX_DROPCNT, read_reg_word(GDMA3_TX_DROPCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA3_RX_OKCNT            (0x%08x) = 0x%08lx\n", GDMA3_RX_OKCNT, read_reg_word(GDMA3_RX_OKCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA3_RX_FCDROPCNT        (0x%08x) = 0x%08lx\n", GDMA3_RX_FCDROPCNT, read_reg_word(GDMA3_RX_FCDROPCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA3_RX_RCDROPCNT        (0x%08x) = 0x%08lx\n", GDMA3_RX_RCDROPCNT, read_reg_word(GDMA3_RX_RCDROPCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA3_RX_OVDROPCNT        (0x%08x) = 0x%08lx\n", GDMA3_RX_OVDROPCNT, read_reg_word(GDMA3_RX_OVDROPCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA3_RX_ERRDROPCNT       (0x%08x) = 0x%08lx\n", GDMA3_RX_ERRDROPCNT, read_reg_word(GDMA3_RX_ERRDROPCNT));
		CHK_BUF();
	}
#endif
	if(GDM4_SUPPORT)
	{
		index += sprintf(buf+index, "\nGDMA4_TX_GETCNT           (0x%08x) = 0x%08lx\n", GDMA4_TX_GETCNT, read_reg_word(GDMA4_TX_GETCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA4_TX_OKCNT_L          (0x%08x) = 0x%08lx\n", GDMA4_TX_OKCNT_L, read_reg_word(GDMA4_TX_OKCNT_L));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA4_TX_OKCNT_H          (0x%08x) = 0x%08lx\n", GDMA4_TX_OKCNT_H, read_reg_word(GDMA4_TX_OKCNT_H));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA4_TX_DROPCNT          (0x%08x) = 0x%08lx\n", GDMA4_TX_DROPCNT, read_reg_word(GDMA4_TX_DROPCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA4_RX_OKCNT_L          (0x%08x) = 0x%08lx\n", GDMA4_RX_OKCNT_L, read_reg_word(GDMA4_RX_OKCNT_L));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA4_RX_OKCNT_H          (0x%08x) = 0x%08lx\n", GDMA4_RX_OKCNT_H, read_reg_word(GDMA4_RX_OKCNT_H));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA4_RX_FCDROPCNT        (0x%08x) = 0x%08lx\n", GDMA4_RX_FCDROPCNT, read_reg_word(GDMA4_RX_FCDROPCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA4_RX_RCDROPCNT        (0x%08x) = 0x%08lx\n", GDMA4_RX_RCDROPCNT, read_reg_word(GDMA4_RX_RCDROPCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA4_RX_OVDROPCNT        (0x%08x) = 0x%08lx\n", GDMA4_RX_OVDROPCNT, read_reg_word(GDMA4_RX_OVDROPCNT));
		CHK_BUF();
		index += sprintf(buf+index, "GDMA4_RX_ERRDROPCNT       (0x%08x) = 0x%08lx\n", GDMA4_RX_ERRDROPCNT, read_reg_word(GDMA4_RX_ERRDROPCNT));
		CHK_BUF();
	}

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

#ifdef TCSUPPORT_CPU_EN7580
static int mbi_unlock_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    int index = 0;
    off_t begin = 0;
    off_t pos = 0;
    unsigned int reg=0, value=0;
    
    reg = GDMA2_CHN_RLS;
    value = read_reg_word(reg);
    
    index += sprintf(buf+index, "mbi_tx_busy: %d\n", (value&(1<<MBI_TX_BUSY_OFFSET))?1:0);
    CHK_BUF();
    index += sprintf(buf+index, "mbi_tx_Aging: %d\n", (value&MBI_TX_AGE_SEL_MASK)>>MBI_TX_AGE_SEL_OFFSET);
    CHK_BUF();
    index += sprintf(buf+index, "mbi_rx_busy: %d\n", (value&(1<<MBI_RX_BUSY_OFFSET))?1:0);
    CHK_BUF();
    index += sprintf(buf+index, "mbi_rx_Aging: %d\n", (value&MBI_RX_AGE_SEL_MASK)>>MBI_RX_AGE_SEL_OFFSET);
    CHK_BUF();
    index += sprintf(buf+index, "dbgLevel: %d\n", dbgLevel);
    CHK_BUF();
    index += sprintf(buf+index, "unlockLevel: %d\n", unlockLevel);
    CHK_BUF();
    
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

static int mbi_unlock_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    uint8_t val_string[96], cmd[32], param1[16] ;
    struct ecnt_fe_data fe_data;

    if (count > sizeof(val_string) - 1)
        return -EINVAL ;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT ;

    sscanf(val_string, "%s %s", cmd, param1) ;

    if(!strcmp(cmd, "aging")) {
        mbi_hang_unlock_by_aging(FE_GDM_SEL_GDMA2, atoi(param1));
    } else if(!strcmp(cmd, "terminate")) {
        mbi_hang_unlock_by_terminate(FE_GDM_SEL_GDMA2);
    } else if(!strcmp(cmd, "level")) {
        dbgLevel = atoi(param1);
    } else if(!strcmp(cmd, "retire")) {
        fe_data.gdm_sel = FE_GDM_SEL_GDMA2;
        fe_data.channel = atoi(param1);
        if(fe_data.channel<32) {
            fe_api_set_channel_retire_one(&fe_data);
            printk("channel-%d retire done\n", fe_data.channel);
        } else if(fe_data.channel==32) {
            fe_api_set_channel_retire_all(&fe_data);
            printk("all channel retire done\n");
        }
    } else if(!strcmp(cmd, "help")) {
        printk("echo terminate > /proc/tc3162/mbi_unlock\n");
        printk("echo aging [0~3] > /proc/tc3162/mbi_unlock\n");
        printk("aging [0~3] means: 0:Not Age; 1:0x8000; 2:0x10000; 3:0x18000\n");
        printk("echo retire [0~32] > /proc/tc3162/mbi_unlock\n");
    } else if(!strcmp(cmd, "unlock")) {
        unlockLevel = atoi(param1);
    }
    return count ;
}

#if defined(TCSUPPORT_CPU_EN7523)
static int fe_oq_cnt_dump_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    int index = 0;
    off_t begin = 0;
    off_t pos = 0;
    int port_num = 0;
    int queue_num = 0;
	unsigned int reg_val = 0;
    uint16 share_use = 0,share_free = 0;
    unsigned short port_deq_cnt[PSE_PORT_NUM_MAX][PSE_PORT_QUEUE_MAX];
    unsigned short port_rls_cnt[PSE_PORT_NUM_MAX][PSE_PORT_QUEUE_MAX];
    unsigned char port_queue_num[PSE_PORT_NUM_MAX] = PSE_PORT_QUEUE_NUM_ARRAY;

    share_use = (read_reg_word(PSE_SHARE_BUF_STA)>>PSE_SHARE_BUF_USED_CNT_SHIFT)&PSE_SHARE_BUF_USED_CNT_MASK;
    share_free = read_reg_word(PSE_SHARE_BUF_STA)&PSE_SHARE_BUF_FREE_CNT_MASK;

	index += sprintf(buf+index, "share use 0x%x,share free 0x%x\n",share_use,share_free);
        CHK_BUF();
		
    for(port_num = 0;port_num < PSE_PORT_NUM_MAX;port_num++)
    {
        if(0 == port_queue_num[port_num])
            continue;

        index += sprintf(buf+index, "\nP%d_OQ_CNT:\n",port_num);
        CHK_BUF();

        for(queue_num = 0;queue_num < port_queue_num[port_num];queue_num++)
        {
            write_reg_word(PSE_QUEUE_CFG_WR, (port_num<<PSE_CFG_PORT_ID_SHIFT)|(queue_num<<PSE_CFG_QUEUE_ID_SHIFT));
            reg_val = read_reg_word(PSE_OQ_PCNT);
            port_deq_cnt[port_num][queue_num] = ((reg_val>>16) & 0x3fff);
            port_rls_cnt[port_num][queue_num] = (reg_val & 0x3fff);

            if((0 == (queue_num+1)%4)||(queue_num == (port_queue_num[port_num]-1)))
            {
                index += sprintf(buf+index, "OQ%d_CNT=(0x%04x)(0x%04x)\n", queue_num,port_deq_cnt[port_num][queue_num], port_rls_cnt[port_num][queue_num]);
                CHK_BUF();
            } 
            else
            {
                index += sprintf(buf+index, "OQ%d_CNT=(0x%04x)(0x%04x), ", queue_num,port_deq_cnt[port_num][queue_num], port_rls_cnt[port_num][queue_num]);
                CHK_BUF();
            }
        }
    }

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
#else
static int fe_oq_cnt_dump_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    int index = 0;
    off_t begin = 0;
    off_t pos = 0;
    int i = 0;
	unsigned int reg_val = 0;
    unsigned short P0_deq_cnt[PSE_PORT0_QUEUE_NUM], P0_rls_cnt[PSE_PORT0_QUEUE_NUM]; /* CDM1->QDMA1 */
    unsigned short P1_deq_cnt[PSE_PORT1_QUEUE_NUM], P1_rls_cnt[PSE_PORT1_QUEUE_NUM]; /* GDM1->GSW */
    unsigned short P2_deq_cnt[PSE_PORT2_QUEUE_NUM], P2_rls_cnt[PSE_PORT2_QUEUE_NUM]; /* GDM2->PON*/
    unsigned short P3_deq_cnt[PSE_PORT3_QUEUE_NUM], P3_rls_cnt[PSE_PORT3_QUEUE_NUM]; /* GDM3->XFI */
    unsigned short P4_deq_cnt[PSE_PORT4_QUEUE_NUM], P4_rls_cnt[PSE_PORT4_QUEUE_NUM]; /* PPE */
    unsigned short P5_deq_cnt[PSE_PORT5_QUEUE_NUM], P5_rls_cnt[PSE_PORT5_QUEUE_NUM]; /* CDM2->QDMA2 */


    for(i=0; i<PSE_PORT0_QUEUE_NUM; i++) {
        write_reg_word(PSE_PORT_OQ_STA4, (i<<16));
        P0_deq_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA4) & 0x3fff);
        P0_rls_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA5) & 0x3fff);
    }
    for(i=0; i<PSE_PORT1_QUEUE_NUM; i++) {
        write_reg_word(PSE_PORT_OQ_STA4, (((1<<5)|i)<<16));
        P1_deq_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA4) & 0x3fff);
        P1_rls_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA5) & 0x3fff);
    }
    for(i=0; i<PSE_PORT2_QUEUE_NUM; i++) {
        write_reg_word(PSE_PORT_OQ_STA4, (((2<<5)|i)<<16));
        P2_deq_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA4) & 0x3fff);
        P2_rls_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA5) & 0x3fff);
    }
    for(i=0; i<PSE_PORT3_QUEUE_NUM; i++) {
        write_reg_word(PSE_PORT_OQ_STA4, (((3<<5)|i)<<16));
        P3_deq_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA4) & 0x3fff);
        P3_rls_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA5) & 0x3fff);
    }
    for(i=0; i<PSE_PORT4_QUEUE_NUM; i++) {
        write_reg_word(PSE_PORT_OQ_STA4, (((4<<5)|i)<<16));
        P4_deq_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA4) & 0x3fff);
        P4_rls_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA5) & 0x3fff);
    }
    for(i=0; i<PSE_PORT5_QUEUE_NUM; i++) {
        write_reg_word(PSE_PORT_OQ_STA4, (((5<<5)|i)<<16));
        P5_deq_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA4) & 0x3fff);
        P5_rls_cnt[i] = (read_reg_word(PSE_PORT_OQ_STA5) & 0x3fff);
    }


    index += sprintf(buf+index, "\nP0_OQ_CNT:\n");
    CHK_BUF();
    index += sprintf(buf+index, "OQ0_CNT=(0x%04x)(0x%04x), OQ1_CNT=(0x%04x)(0x%04x), OQ2_CNT=(0x%04x)(0x%04x)\n"
        , P0_deq_cnt[0], P0_rls_cnt[0], P0_deq_cnt[1], P0_rls_cnt[1], P0_deq_cnt[2], P0_rls_cnt[2]);
    CHK_BUF();

    index += sprintf(buf+index, "\nP1_OQ_CNT:\n");
    CHK_BUF();
    index += sprintf(buf+index, "OQ0_CNT=(0x%04x)(0x%04x), OQ1_CNT=(0x%04x)(0x%04x), OQ2_CNT=(0x%04x)(0x%04x), OQ3_CNT=(0x%04x)(0x%04x)\n"
        , P1_deq_cnt[0], P1_rls_cnt[0], P1_deq_cnt[1], P1_rls_cnt[1], P1_deq_cnt[2], P1_rls_cnt[2], P1_deq_cnt[3], P1_rls_cnt[3]);
    CHK_BUF();
    index += sprintf(buf+index, "OQ4_CNT=(0x%04x)(0x%04x), OQ5_CNT=(0x%04x)(0x%04x)\n"
        , P1_deq_cnt[4], P1_rls_cnt[4], P1_deq_cnt[5], P1_rls_cnt[5]);
    CHK_BUF();

    index += sprintf(buf+index, "\nP2_OQ_CNT:\n");
    CHK_BUF();
    for(i=0; i<8; i++) {
        index += sprintf(buf+index, "OQ%d_CNT=(0x%04x)(0x%04x), OQ%d_CNT=(0x%04x)(0x%04x), OQ%d_CNT=(0x%04x)(0x%04x), OQ%d_CNT=(0x%04x)(0x%04x)\n"
            , (i<<2), P2_deq_cnt[i<<2], P2_rls_cnt[i<<2], (i<<2)+1, P2_deq_cnt[(i<<2)+1], P2_rls_cnt[(i<<2)+1]
            , (i<<2)+2, P2_deq_cnt[(i<<2)+2], P2_rls_cnt[(i<<2)+2], (i<<2)+3, P2_deq_cnt[(i<<2)+3], P2_rls_cnt[(i<<2)+3]);
        CHK_BUF();
    }

    index += sprintf(buf+index, "\nP3_OQ_CNT:\n");
    CHK_BUF();
    index += sprintf(buf+index, "OQ0_CNT=(0x%04x)(0x%04x), OQ1_CNT=(0x%04x)(0x%04x), OQ2_CNT=(0x%04x)(0x%04x), OQ3_CNT=(0x%04x)(0x%04x)\n"
        , P3_deq_cnt[0], P3_rls_cnt[0], P3_deq_cnt[1], P3_rls_cnt[1], P3_deq_cnt[2], P3_rls_cnt[2], P3_deq_cnt[3], P3_rls_cnt[3]);
    CHK_BUF();

    index += sprintf(buf+index, "\nP4_OQ_CNT:\n");
    CHK_BUF();
    index += sprintf(buf+index, "OQ0_CNT=(0x%04x)(0x%04x), OQ1_CNT=(0x%04x)(0x%04x), OQ2_CNT=(0x%04x)(0x%04x), OQ3_CNT=(0x%04x)(0x%04x)\n"
        , P4_deq_cnt[0], P4_rls_cnt[0], P4_deq_cnt[1], P4_rls_cnt[1], P4_deq_cnt[2], P4_rls_cnt[2], P4_deq_cnt[3], P4_rls_cnt[3]);
    CHK_BUF();

    index += sprintf(buf+index, "\nP5_OQ_CNT:\n");
    CHK_BUF();
    index += sprintf(buf+index, "OQ0_CNT=(0x%04x)(0x%04x), OQ1_CNT=(0x%04x)(0x%04x), OQ2_CNT=(0x%04x)(0x%04x)\n"
        , P5_deq_cnt[0], P5_rls_cnt[0], P5_deq_cnt[1], P5_rls_cnt[1], P5_deq_cnt[2], P5_rls_cnt[2]);
    CHK_BUF();

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
#endif


#ifdef TCSUPPORT_CPU_EN7523
static int fe_gdm3_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    int index = 0;
    off_t begin = 0;
    off_t pos = 0;

    index += sprintf(buf+index, "\n GDMA3_TX_GET_CNT_L      (0x%08x) = 0x%08lx\n", GDMA3_TX_GET_CNT_L , read_reg_word(GDMA3_TX_GET_CNT_L));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA3_TX_OK_CNT_L        	(0x%08x) = 0x%08lx\n", GDMA3_TX_OK_CNT_L , read_reg_word(GDMA3_TX_OK_CNT_L));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_OK_CNT_H          (0x%08x) = 0x%08lx\n", GDMA3_TX_OK_CNT_H , read_reg_word(GDMA3_TX_OK_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_DROP_CNT          (0x%08x) = 0x%08lx\n", GDMA3_TX_DROP_CNT , read_reg_word(GDMA3_TX_DROP_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_OK_BYTE_CNT_L     (0x%08x) = 0x%08lx\n", GDMA3_TX_OK_BYTE_CNT , read_reg_word(GDMA3_TX_OK_BYTE_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_OK_BYTE_CNT_H     (0x%08x) = 0x%08lx\n", GDMA3_TX_OK_BYTE_CNT_H , read_reg_word(GDMA3_TX_OK_BYTE_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_PKT_CNT_L     (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_PKT_CNT , read_reg_word(GDMA3_TX_ETH_PKT_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_PKT_CNT_H     (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_PKT_CNT_H , read_reg_word(GDMA3_TX_ETH_PKT_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_BYTE_CNT_L    (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_BYTE_CNT , read_reg_word(GDMA3_TX_ETH_BYTE_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_BYTE_CNT_H    (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_BYTE_CNT_H , read_reg_word(GDMA3_TX_ETH_BYTE_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_DROP_CNT      (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_DROP_CNT , read_reg_word(GDMA3_TX_ETH_DROP_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_RUNT_CNT      (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_RUNT_CNT , read_reg_word(GDMA3_TX_ETH_RUNT_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_LONG_CNT      (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_LONG_CNT , read_reg_word(GDMA3_TX_ETH_LONG_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_E64_CNT_L     (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_E64_CNT , read_reg_word(GDMA3_TX_ETH_E64_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_E64_CNT_H     (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_E64_CNT_H , read_reg_word(GDMA3_TX_ETH_E64_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L64_CNT_L     (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L64_CNT , read_reg_word(GDMA3_TX_ETH_L64_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L64_CNT_H     (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L64_CNT_H , read_reg_word(GDMA3_TX_ETH_L64_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L127_CNT_L    (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L127_CNT , read_reg_word(GDMA3_TX_ETH_L127_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L127_CNT_H    (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L127_CNT_H , read_reg_word(GDMA3_TX_ETH_L127_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L255_CNT_L    (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L255_CNT , read_reg_word(GDMA3_TX_ETH_L255_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L255_CNT_H    (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L255_CNT_H , read_reg_word(GDMA3_TX_ETH_L255_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L511_CNT_L    (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L511_CNT , read_reg_word(GDMA3_TX_ETH_L511_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L511_CNT_H    (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L511_CNT_H , read_reg_word(GDMA3_TX_ETH_L511_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L1023_CNT_L   (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L1023_CNT , read_reg_word(GDMA3_TX_ETH_L1023_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_ETH_L1023_CNT_H   (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_L1023_CNT_H , read_reg_word(GDMA3_TX_ETH_L1023_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_OK_BC_BYTE_CNT_L  (0x%08x) = 0x%08lx\n", GDMA3_TX_OK_BC_BYTE_CNT_L , read_reg_word(GDMA3_TX_OK_BC_BYTE_CNT_L));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_OK_BC_BYTE_CNT_H  (0x%08x) = 0x%08lx\n", GDMA3_TX_OK_BC_BYTE_CNT_H , read_reg_word(GDMA3_TX_OK_BC_BYTE_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_OK_MC_BYTE_CNT_L  (0x%08x) = 0x%08lx\n", GDMA3_TX_OK_MC_BYTE_CNT_L , read_reg_word(GDMA3_TX_OK_MC_BYTE_CNT_L));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_OK_MC_BYTE_CNT_H  (0x%08x) = 0x%08lx\n", GDMA3_TX_OK_MC_BYTE_CNT_H , read_reg_word(GDMA3_TX_OK_MC_BYTE_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_BC_PKT_CNT_L      (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_BC_CNT , read_reg_word(GDMA3_TX_ETH_BC_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_BC_PKT_CNT_H      (0x%08x) = 0x%08lx\n", GDMA3_TX_BC_PKT_CNT_H, read_reg_word(GDMA3_TX_BC_PKT_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_MC_PKT_CNT_L      (0x%08x) = 0x%08lx\n", GDMA3_TX_ETH_MC_CNT , read_reg_word(GDMA3_TX_ETH_MC_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_TX_MC_PKT_CNT_H      (0x%08x) = 0x%08lx\n", GDMA3_TX_MC_PKT_CNT_H, read_reg_word(GDMA3_TX_MC_PKT_CNT_H));
    CHK_BUF();

	index += sprintf(buf+index, "\n GDMA3_RX_MAC_DROP_CNT   (0x%08x) = 0x%08lx\n", GDMA3_RX_MAC_DROP_CNT , read_reg_word(GDMA3_RX_MAC_DROP_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_OK_CNT_L      	(0x%08x) = 0x%08lx\n", GDMA3_RX_OK_CNT , read_reg_word(GDMA3_RX_OK_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_OK_CNT_H        	(0x%08x) = 0x%08lx\n", GDMA3_RX_OK_CNT_H , read_reg_word(GDMA3_RX_OK_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_FC_DROP_CNT       (0x%08x) = 0x%08lx\n", GDMA3_RX_FC_DROP_CNT , read_reg_word(GDMA3_RX_FC_DROP_CNT));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA3_RX_RC_DROP_CNT       (0x%08x) = 0x%08lx\n", GDMA3_RX_RC_DROP_CNT , read_reg_word(GDMA3_RX_RC_DROP_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_OVER_DROP_CNT     (0x%08x) = 0x%08lx\n", GDMA3_RX_OVER_DROP_CNT , read_reg_word(GDMA3_RX_OVER_DROP_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ERROR_DROP_CNT    (0x%08x) = 0x%08lx\n", GDMA3_RX_ERROR_DROP_CNT , read_reg_word(GDMA3_RX_ERROR_DROP_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_OK_BYTE_CNT_L     (0x%08x) = 0x%08lx\n", GDMA3_RX_OK_BYTE_CNT , read_reg_word(GDMA3_RX_OK_BYTE_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_OK_BYTE_CNT_H     (0x%08x) = 0x%08lx\n", GDMA3_RX_OK_BYTE_CNT_H , read_reg_word(GDMA3_RX_OK_BYTE_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_PKT_CNT_L     (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_PKT_CNT , read_reg_word(GDMA3_RX_ETH_PKT_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_PKT_CNT_H     (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_PKT_CNT_H , read_reg_word(GDMA3_RX_ETH_PKT_CNT_H));
    CHK_BUF();
	
	index += sprintf(buf+index, "GDMA3_RX_ETH_BYTE_CNT_L    (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_BYTE_CNT , read_reg_word(GDMA3_RX_ETH_BYTE_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_BYTE_CNT_H    (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_BYTE_CNT_H , read_reg_word(GDMA3_RX_ETH_BYTE_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_DROP_CNT      (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_DROP_CNT , read_reg_word(GDMA3_RX_ETH_DROP_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_CRCE_CNT      (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_CRCE_CNT , read_reg_word(GDMA3_RX_ETH_CRCE_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_FRAG_CNT      (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_FRAG_CNT , read_reg_word(GDMA3_RX_ETH_FRAG_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_JABBER_CNT    (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_JABBER_CNT , read_reg_word(GDMA3_RX_ETH_JABBER_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_RUNT_CNT      (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_RUNT_CNT , read_reg_word(GDMA3_RX_ETH_RUNT_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_LONG_CNT      (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_LONG_CNT , read_reg_word(GDMA3_RX_ETH_LONG_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_E64_CNT_L     (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_E64_CNT , read_reg_word(GDMA3_RX_ETH_E64_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_E64_CNT_H     (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_E64_CNT_H , read_reg_word(GDMA3_RX_ETH_E64_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_L64_CNT_L     (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L64_CNT , read_reg_word(GDMA3_RX_ETH_L64_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_L64_CNT_H     (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L64_CNT_H , read_reg_word(GDMA3_RX_ETH_L64_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_L127_CNT_L    (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L127_CNT , read_reg_word(GDMA3_RX_ETH_L127_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_L127_CNT_H	(0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L127_CNT_H , read_reg_word(GDMA3_RX_ETH_L127_CNT_H));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_L255_CNT_L    (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L255_CNT , read_reg_word(GDMA3_RX_ETH_L255_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_L255_CNT_H    (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L255_CNT_H , read_reg_word(GDMA3_RX_ETH_L255_CNT_H));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA3_RX_ETH_L511_CNT_L    (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L511_CNT , read_reg_word(GDMA3_RX_ETH_L511_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_L511_CNT_H    (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L511_CNT_H , read_reg_word(GDMA3_RX_ETH_L511_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_L1023_CNT_L   (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L1023_CNT , read_reg_word(GDMA3_RX_ETH_L1023_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_L1023_CNT_H   (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_L1023_CNT_H , read_reg_word(GDMA3_RX_ETH_L1023_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_ETH_OK_CNT        (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_OK_CNT , read_reg_word(GDMA3_RX_ETH_OK_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_OAM_CNT        	(0x%08x) = 0x%08lx\n", GDMA3_RX_OAM_CNT , read_reg_word(GDMA3_RX_OAM_CNT));
    CHK_BUF();

	index += sprintf(buf+index, "GDMA3_RX_OK_BC_BYTE_CNT_L        (0x%08x) = 0x%08lx\n", GDMA3_RX_OK_BC_BYTE_CNT_L , read_reg_word(GDMA3_RX_OK_BC_BYTE_CNT_L));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_OK_BC_BYTE_CNT_H        (0x%08x) = 0x%08lx\n", GDMA3_RX_OK_BC_BYTE_CNT_H , read_reg_word(GDMA3_RX_OK_BC_BYTE_CNT_H));
    CHK_BUF();
    index += sprintf(buf+index, "GDMA3_RX_OK_MC_BYTE_CNT_L        (0x%08x) = 0x%08lx\n", GDMA3_RX_OK_MC_BYTE_CNT_L , read_reg_word(GDMA3_RX_OK_MC_BYTE_CNT_L));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_OK_MC_BYTE_CNT_H        (0x%08x) = 0x%08lx\n", GDMA3_RX_OK_MC_BYTE_CNT_H , read_reg_word(GDMA3_RX_OK_MC_BYTE_CNT_H));
	CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_BC_PKT_CNT_L        (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_BC_CNT , read_reg_word(GDMA3_RX_ETH_BC_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_BC_PKT_CNT_H        (0x%08x) = 0x%08lx\n", GDMA3_RX_BC_PKT_CNT_H , read_reg_word(GDMA3_RX_BC_PKT_CNT_H));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_MC_PKT_CNT_L        (0x%08x) = 0x%08lx\n", GDMA3_RX_ETH_MC_CNT , read_reg_word(GDMA3_RX_ETH_MC_CNT));
    CHK_BUF();
	index += sprintf(buf+index, "GDMA3_RX_MC_PKT_CNT_H        (0x%08x) = 0x%08lx\n", GDMA3_RX_MC_PKT_CNT_H , read_reg_word(GDMA3_RX_MC_PKT_CNT_H));
    CHK_BUF();
	
    
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


static int fe_gdm3_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    uint8_t val_string[96], cmd[32], param1[16] ;
    
	unsigned int reg=0;

    if (count > sizeof(val_string) - 1)
        return -EINVAL ;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT ;

    sscanf(val_string, "%s %s", cmd, param1) ;

    if(!strcmp(cmd, "dump")) {
		if(!strcmp(param1, "2xhsgmii0")) {
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (4 << GDM3_TX_MIB_ID_OFFSET) + (0 << GDM3_RX_MIB_ID_OFFSET);
			printk("2xhsgmii0 %08x\n", reg);
			write_reg_word(GDMA3_MIB_CFG, reg );

		}else if(!strcmp(param1, "2xhsgmii1")){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (5 << GDM3_TX_MIB_ID_OFFSET) + (1 << GDM3_RX_MIB_ID_OFFSET);
			printk("2xhsgmii1 %08x\n", reg);
			write_reg_word(GDMA3_MIB_CFG, reg );

		}else if(!strcmp(param1, "1xhsgmii0")){
			reg = (1 << GDM3_RX_MIB_SPLIT_EN_OFFSET) + (1 << GDM3_TX_MIB_SPLIT_EN_OFFSET) + (6 << GDM3_TX_MIB_ID_OFFSET) + (2 << GDM3_RX_MIB_ID_OFFSET);
			printk("1xhsgmii0 %08x\n", reg);
			write_reg_word(GDMA3_MIB_CFG, reg );

		}else if(!strcmp(param1, "all")){ 
			printk("all %08x\n", reg);
			write_reg_word(GDMA3_MIB_CFG, reg );
		}

    } 
    return count ;
}




unsigned int fe_get_reg_from_chnl(int gdm_idx, int chnl,int set_idx)
{

	unsigned int reg=0;
	if(set_idx == 0){
		if(gdm_idx == 1 && chnl == 0)
			reg = GDMA1_CHN0_SPTAG_SET0;
		else if(gdm_idx == 1 && chnl == 1)
			reg = GDMA1_CHN1_SPTAG_SET0;
		else if(gdm_idx == 1 && chnl == 2)
			reg = GDMA1_CHN2_SPTAG_SET0;
		else if(gdm_idx == 2 && chnl == 0)
			reg = GDMA2_CHN0_SPTAG_SET0;
		else if(gdm_idx == 2 && chnl == 1)
			reg = GDMA2_CHN1_SPTAG_SET0;
		else if(gdm_idx == 2 && chnl == 2)
			reg = GDMA2_CHN2_SPTAG_SET0;
		else if(gdm_idx == 3 && chnl == 0)
			reg = GDMA3_CHN0_SPTAG_SET0;
		else if(gdm_idx == 3 && chnl == 1)
			reg = GDMA3_CHN1_SPTAG_SET0;
		else if(gdm_idx == 3 && chnl == 2)
			reg = GDMA3_CHN2_SPTAG_SET0;
		else if((4 == gdm_idx) && (0 == chnl))
		{
			reg = GDMA4_CHN0_SPTAG_SET0;
		}
		else if((4 == gdm_idx) && (1 == chnl))
		{
			reg = GDMA4_CHN1_SPTAG_SET0;
		}
		else if((4 == gdm_idx) && (2 == chnl))
		{
			reg = GDMA4_CHN2_SPTAG_SET0;
		}
	}else if(set_idx == 1){
		if(gdm_idx == 1 && chnl == 0)
			reg = GDMA1_CHN0_SPTAG_SET1;
		else if(gdm_idx == 1 && chnl == 1)
			reg = GDMA1_CHN1_SPTAG_SET1;
		else if(gdm_idx == 1 && chnl == 2)
			reg = GDMA1_CHN2_SPTAG_SET1;
		else if(gdm_idx == 2 && chnl == 0)
			reg = GDMA2_CHN0_SPTAG_SET1;
		else if(gdm_idx == 2 && chnl == 1)
			reg = GDMA2_CHN1_SPTAG_SET1;
		else if(gdm_idx == 2 && chnl == 2)
			reg = GDMA2_CHN2_SPTAG_SET1;
		else if(gdm_idx == 3 && chnl == 0)
			reg = GDMA3_CHN0_SPTAG_SET1;
		else if(gdm_idx == 3 && chnl == 1)
			reg = GDMA3_CHN1_SPTAG_SET1;
		else if(gdm_idx == 3 && chnl == 2)
			reg = GDMA3_CHN2_SPTAG_SET1;
		else if((4 == gdm_idx) && (0 == chnl))
		{
			reg = GDMA4_CHN0_SPTAG_SET1;
		}
		else if((4 == gdm_idx) && (1 == chnl))
		{
			reg = GDMA4_CHN1_SPTAG_SET1;
		}
		else if((4 == gdm_idx) && (2 == chnl))
		{
			reg = GDMA4_CHN2_SPTAG_SET1;
		}
	}else if(set_idx == 2){
		if(gdm_idx == 1 && chnl == 0)
			reg = GDMA1_CHN0_SPTAG_SET2;
		else if(gdm_idx == 1 && chnl == 1)
			reg = GDMA1_CHN1_SPTAG_SET2;
		else if(gdm_idx == 1 && chnl == 2)
			reg = GDMA1_CHN2_SPTAG_SET2;
		else if(gdm_idx == 2 && chnl == 0)
			reg = GDMA2_CHN0_SPTAG_SET2;
		else if(gdm_idx == 2 && chnl == 1)
			reg = GDMA2_CHN1_SPTAG_SET2;
		else if(gdm_idx == 2 && chnl == 2)
			reg = GDMA2_CHN2_SPTAG_SET2;
		else if(gdm_idx == 3 && chnl == 0)
			reg = GDMA3_CHN0_SPTAG_SET2;
		else if(gdm_idx == 3 && chnl == 1)
			reg = GDMA3_CHN1_SPTAG_SET2;
		else if(gdm_idx == 3 && chnl == 2)
			reg = GDMA3_CHN2_SPTAG_SET2;
		else if((4 == gdm_idx) && (0 == chnl))
		{
			reg = GDMA4_CHN0_SPTAG_SET2;
		}
		else if((4 == gdm_idx) && (1 == chnl))
		{
			reg = GDMA4_CHN1_SPTAG_SET2;
		}
		else if((4 == gdm_idx) && (2 == chnl))
		{
			reg = GDMA4_CHN2_SPTAG_SET2;
		}
	}else if(set_idx == 3){
		if(gdm_idx == 1 && chnl == 0)
			reg = GDMA1_CHN0_SPTAG_SET3;
		else if(gdm_idx == 1 && chnl == 1)
			reg = GDMA1_CHN1_SPTAG_SET3;
		else if(gdm_idx == 1 && chnl == 2)
			reg = GDMA1_CHN2_SPTAG_SET3;
		else if(gdm_idx == 2 && chnl == 0)
			reg = GDMA2_CHN0_SPTAG_SET3;
		else if(gdm_idx == 2 && chnl == 1)
			reg = GDMA2_CHN1_SPTAG_SET3;
		else if(gdm_idx == 2 && chnl == 2)
			reg = GDMA2_CHN2_SPTAG_SET3;
		else if(gdm_idx == 3 && chnl == 0)
			reg = GDMA3_CHN0_SPTAG_SET3;
		else if(gdm_idx == 3 && chnl == 1)
			reg = GDMA3_CHN1_SPTAG_SET3;
		else if(gdm_idx == 3 && chnl == 2)
			reg = GDMA3_CHN2_SPTAG_SET3;
		else if((4 == gdm_idx) && (0 == chnl))
		{
			reg = GDMA4_CHN0_SPTAG_SET3;
		}
		else if((4 == gdm_idx) && (1 == chnl))
		{
			reg = GDMA4_CHN1_SPTAG_SET3;
		}
		else if((4 == gdm_idx) && (2 == chnl))
		{
			reg = GDMA4_CHN2_SPTAG_SET3;
		}
	}
	
	printk("reg addr  is %08x ", reg);
	return reg;

}





static int fe_set_gdm_sptag_bitmapping_info(int gdm_idx, int chnl, int idx, int bitmask)
{
	unsigned int reg_val = 0;
	unsigned int reg=0, offset = 0,mask = 0;

	if(idx >= 0 && idx < 6){

		reg = fe_get_reg_from_chnl(gdm_idx,chnl, 0);
		
		offset = 5*idx;
	}else if(idx < 12){
		reg = fe_get_reg_from_chnl(gdm_idx,chnl, 1);

		offset = 5*idx -30;
	}else if(idx < 16){
		reg = fe_get_reg_from_chnl(gdm_idx,chnl, 2);

		offset = 5*idx -60;
	}else
		return 0;

	if(0 == reg)
		return 0;
	mask = 0x1f << offset;

	reg_val = read_reg_word(reg);

	IO_SMASK(reg, mask, offset, bitmask);

	return 0;
}

static int fe_set_gdm_sptag_srctagoffset(int gdm_idx, int chnl,unsigned char bit1_offset, unsigned char bit0_offset)
{
	unsigned int reg_val = 0, reg = 0;

	reg = fe_get_reg_from_chnl(gdm_idx,chnl, 2);
	if(0 == reg)
		return 0;

	reg_val = read_reg_word(reg);
	
	reg_val &=0xC00fffff;

	reg_val |= (bit1_offset & 0x1f)<<25 | ((bit0_offset & 0x1f)<<20) ;
	printk("reg_val%08x \n",reg_val);

	write_reg_word(reg, reg_val);

	return 0;
}

static int fe_set_gdm_sptag_tx_bypass(int gdm_idx, int chnl,unsigned char enable )
{
	unsigned int reg_val = 0, reg = 0;

	reg = fe_get_reg_from_chnl(gdm_idx,chnl, 2);
	if(0 == reg)
		return 0;

	reg_val = read_reg_word(reg);
	reg_val &=0x7FFFFFFF;
	if(enable)
		reg_val |= 1<<31  ;
	else
		reg_val &= 0x7FFFFFFF;
	printk("reg_val%08x \n",reg_val);

	write_reg_word(reg, reg_val);

	return 0;
}

static int fe_set_gdm_sptag_tpid(int gdm_idx, int chnl,unsigned short tpid )
{
	unsigned int reg_val = 0, reg = 0;

	reg = fe_get_reg_from_chnl(gdm_idx,chnl, 3);
	if(0 == reg)
		return 0;


	reg_val = read_reg_word(reg);
	reg_val &=0xFFFF;
	reg_val |= (tpid&0xFFFF)<<16  ;


	write_reg_word(reg, reg_val);

	return 0;
}
/*
gdm idx range 1-3 mean gdm1 - gdm3
chnl idx range 0-2 mean chnl 0-2
is_tx 0 mean rx mode 1 mean tx mode
mode range 0-3 
mode 0 sptag no operation
mode 1 insert/remove 
mode 2 replace mode
mode 3 replace when tag,insert when untag

*/
static int fe_set_gdm_sptag_mode(int gdm_idx, int chnl, int is_tx, unsigned char mode )
{
	unsigned int reg_val = 0, reg = 0;

	reg = fe_get_reg_from_chnl(gdm_idx,chnl, 3);
	if(0 == reg)
		return 0;
	reg_val = read_reg_word(reg);



	if(is_tx == 0){
		reg_val &=0xFFFFFFFc;
		reg_val |=(mode&0x3);
	}else if(is_tx ==1){
		reg_val &=0xFFFFFFF3;
		reg_val |=((mode&0x3)<<2);
	}

	write_reg_word(reg, reg_val);

	return 0;
}

static int fe_set_gdm_vpm_mode(int gdm_idx, int chnl, unsigned char vpm0, unsigned char vpm1,unsigned char vpm2,unsigned char vpm3 )
{
	unsigned int reg_val = 0, reg = 0;

	reg = fe_get_reg_from_chnl(gdm_idx,chnl, 3);
	if(0 == reg)
		return 0;
	reg_val = read_reg_word(reg);

	reg_val &=0xFFFF00FF;
	reg_val |= (vpm0&0x3)<<8;
	reg_val |= (vpm1&0x3)<<10;
	reg_val |= (vpm2&0x3)<<12;
	reg_val |= (vpm3&0x3)<<14;

	write_reg_word(reg, reg_val);

	return 0;
}
/*
gdm idx range 1-3 mean gdm1 - gdm3
chnl idx range 0-2 mean chnl 0-2

mode range 0-3 
mode 0 non specific mode
mode 1 ecnt switch mode
mode 2 DSA tag mode,hw move cfi to location
mode 3 rsv
*/

static int fe_set_gdm_spectag_mode(int gdm_idx, int chnl, unsigned char mode )
{
	unsigned int reg_val = 0, reg = 0;

	reg = fe_get_reg_from_chnl(gdm_idx,chnl, 3);
	if(0 == reg)
		return 0;
	reg_val = read_reg_word(reg);

	reg_val &=0xFFFFFFCF;
	reg_val |= (mode&0x3)<<4;

	write_reg_word(reg, reg_val);

	return 0;
}

static int fe_set_gdm_srctaginfo_mode(int gdm_idx, int chnl, unsigned char mode )
{
	unsigned int reg_val = 0, reg = 0;

	reg = fe_get_reg_from_chnl(gdm_idx,chnl, 3);
	if(0 == reg)
		return 0;
	reg_val = read_reg_word(reg);

	reg_val &=0xFFFFFFBF;
	reg_val |= (mode&0x1)<<6;

	write_reg_word(reg, reg_val);

	return 0;
}

static int fe_show_gdm_sptag_info(int gdm_idx, int chnl)
{
	int i = 0, flow_idx = 0, offset = 0;
	unsigned int reg = 0,reg0 = 0,reg1 = 0,reg2 = 0,reg3 = 0;

	reg0 = fe_get_reg_from_chnl(gdm_idx,chnl, 0);
	reg1 = fe_get_reg_from_chnl(gdm_idx,chnl, 1);
	reg2 = fe_get_reg_from_chnl(gdm_idx,chnl, 2);
	reg3 = fe_get_reg_from_chnl(gdm_idx,chnl, 3);

	if(reg0 == 0 || reg1 == 0 || reg2 == 0 || reg3 == 0 )
		return 0;

	printk("dump gdm%d chnl%d sptag info\n", gdm_idx, chnl);
	printk("sptag index \tpkt flow idx\n");
	for(i=0; i < 16; i ++){
		if(i<6){
			reg = reg0;
			offset = 5*i;
		}else if(i<12){
			reg = reg1;
			offset = 5*i -30;
		}else{
			reg = reg2;
			offset = 5*i -60;
		}
		flow_idx = (read_reg_word(reg) >> offset)&0x1f ;
		
		printk("%d\t%d\n",i, flow_idx);
	}
	if(gdm_idx == 1){
		reg = GDMA1_SRC_PORT_SET;
	}else if (gdm_idx == 2){
		reg = GDMA2_SRC_PORT_SET;
	}else if (gdm_idx == 3){
		reg = GDMA3_SRC_PORT_SET;
	}
	else if(4 == gdm_idx)
	{
		reg = GDMA4_SRC_PORT_SET;
	}
	
	if (chnl ==0)
		printk(" src portmode %d (0: chnl + offset, 1: sptag + offset)offset %d \n", (read_reg_word(reg) >> 0)&0x1, (read_reg_word(reg) >> 8)&0xf);
	else if (chnl ==1)
		printk(" src portmode %d (0: chnl + offset, 1: sptag + offset)offset %d \n", (read_reg_word(reg) >> 1)&0x1, (read_reg_word(reg) >> 12)&0xf);
	else if (chnl ==2)
		printk(" src portmode %d (0: chnl + offset, 1: sptag + offset)offset %d \n", (read_reg_word(reg) >> 2)&0x1, (read_reg_word(reg) >> 16)&0xf);

	printk("srctag1 in 32 bit sptag %d\n", (read_reg_word(reg2) >> 25)&0x1f);
	printk("srctag0 in 32 bit sptag %d\n", (read_reg_word(reg2) >> 20)&0x1f);
	printk("GDM sptag_tx_bypass %d\n", (read_reg_word(reg2) >> 31)&0x1);

	printk("GDM sptag_tpid %04x\n", (read_reg_word(reg3) >> 16)&0xffff);

	printk("GDM VPM0 %d VPM1 %d VPM2 %d VPM3 %d, (0:untag, 1:0X8100 2:0x88a8 3:sptag_tpid)\n", (read_reg_word(reg3) >> 8)&0x3, (read_reg_word(reg3) >> 10)&0x3, (read_reg_word(reg3) >> 12)&0x3,(read_reg_word(reg3) >> 14)&0x3);

	printk("GDM tx srctag info %d (0:from pes info,1:from sptag)\n", (read_reg_word(reg3) >> 6)&0x1);
	printk("GDM spec tag mode %d (0:none spec ,1:ecnt switch,2:DSA TAG,3:RSV )\n", (read_reg_word(reg3) >> 4)&0x3);

	printk("GDM chnl mode  0: no operation 1:insert/remove mode 2: replace mode 3:replace when src tagged,insert/remove when src untagged \n");
	
	printk("sptag rx mode  %d\n", (read_reg_word(reg3))&0x3);
	printk("sptag tx mode  %d\n", (read_reg_word(reg3) >> 2)&0x3);

	return 0;
}

int fe_set_gdm2_sptag_for_loopback(void)
{
	int gdm_idx = 2, chnl = 0, i = 0;
	
	/*set gdm2 tx insert mode; gdm2 rx remove mode*/
	fe_set_gdm_sptag_mode(gdm_idx, chnl, 1, 1);
	fe_set_gdm_sptag_mode(gdm_idx, chnl, 0, 1);

	fe_set_gdm_sptag_tx_bypass(gdm_idx, chnl, 0);

	/*adjust bit mapping*/
	for(i=0; i<16; i++)
	{
		fe_set_gdm_sptag_bitmapping_info(gdm_idx, chnl, i, 16+i);
	}
	
	return 0;
}

int fe_set_gdm2_sptag_for_loopback_recover(void)
{
	int gdm_idx = 2, chnl = 0;

	/*set gdm2 sptag to no operation*/
	fe_set_gdm_sptag_mode(gdm_idx, chnl, 1, 0);
	fe_set_gdm_sptag_mode(gdm_idx, chnl, 0, 0);

	return 0;
}

int fe_set_gdm_sptag_for_extswitch(int gdm_idx, int chnl)
{
	int i = 0;
	
	if(SUPPORT_EN8851)
	{
		/*set gdm tx no operation; gdm rx remove mode*/
		fe_set_gdm_sptag_mode(gdm_idx, chnl, 1, 0);
		fe_set_gdm_sptag_mode(gdm_idx, chnl, 0, 1);
	}
	else
	{
		/*set gdm tx insert mode; gdm rx remove mode*/
		fe_set_gdm_sptag_mode(gdm_idx, chnl, 1, 1);
		fe_set_gdm_sptag_mode(gdm_idx, chnl, 0, 1);
	}

	fe_set_gdm_sptag_tx_bypass(gdm_idx, 0, 0);

	/*adjust bit mapping*/
	for(i=0; i<16; i++)
	{
		fe_set_gdm_sptag_bitmapping_info(gdm_idx, chnl, i, 16+i);
	}
	
	return 0;
}

int fe_set_gdm_sptag_for_extswitch_recover(int gdm_idx, int chnl)
{
	/*set gdm sptag to no operation*/
	fe_set_gdm_sptag_mode(gdm_idx, chnl, 1, 0);
	fe_set_gdm_sptag_mode(gdm_idx, chnl, 0, 0);

	return 0;
}

static int fe_gdm_sptag_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	

	return 0;
}


static int fe_gdm_sptag_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    uint8_t val_string[96] = {0}, cmd[32]= {0}, subcmd[32] = {0};

	int sptag_idx = 0,bitmask = 0, para1 = 0, para2 = 0, enable = 0, para3 = 0, para4 = 0;

	int gdm_idx = 0,chnl = 0;

    if (count > sizeof(val_string) - 1)
        return -EINVAL ;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT ;

    sscanf(val_string, "%s %s", cmd, subcmd) ;

	if(!strcmp(cmd, "set")){
		if(!strcmp(subcmd, "mapping")){
			sscanf(val_string, "%s %s %d %d %d %d", cmd, subcmd, &gdm_idx, &chnl, &sptag_idx, &bitmask);
			fe_set_gdm_sptag_bitmapping_info(gdm_idx, chnl,sptag_idx, bitmask);
		}else if (!strcmp(subcmd, "srctagoffset")){
			sscanf(val_string, "%s %s %d %d %d %d", cmd, subcmd,&gdm_idx, &chnl, &para1, &para2);
			fe_set_gdm_sptag_srctagoffset(gdm_idx, chnl,para1, para2);
		}else if (!strcmp(subcmd, "sptagtxbypass")){
			sscanf(val_string, "%s %s %d %d %d", cmd, subcmd, &gdm_idx, &chnl,&enable );
			if(enable)
				fe_set_gdm_sptag_tx_bypass(gdm_idx, chnl,1);
			else
				fe_set_gdm_sptag_tx_bypass(gdm_idx, chnl,0);
		}else if (!strcmp(subcmd, "sptagtpid")){
			sscanf(val_string, "%s %s %d %d %x", cmd, subcmd, &gdm_idx, &chnl,&para1 );
			fe_set_gdm_sptag_tpid(gdm_idx, chnl,para1);
		}else if(!strcmp(subcmd, "sptagmode")){
			sscanf(val_string, "%s %s %d %d %d %d", cmd, subcmd, &gdm_idx, &chnl,&para1, &para2);
			fe_set_gdm_sptag_mode(gdm_idx, chnl,para1, para2);
		}else if(!strcmp(subcmd, "vpm")){
			sscanf(val_string, "%s %s %d %d %d %d %d %d", cmd, subcmd, &gdm_idx, &chnl,&para1, &para2,&para3, &para4);
			fe_set_gdm_vpm_mode(gdm_idx, chnl,para1, para2, para3, para4);
		}else if(!strcmp(subcmd, "specmode")){
			sscanf(val_string, "%s %s %d %d %d ", cmd, subcmd, &gdm_idx, &chnl,&para1);
			fe_set_gdm_spectag_mode(gdm_idx, chnl,para1);
		}else if(!strcmp(subcmd, "srctaginfo")){
			sscanf(val_string, "%s %s %d %d %d ", cmd, subcmd, &gdm_idx, &chnl,&para1);
			fe_set_gdm_srctaginfo_mode(gdm_idx, chnl,para1);
		}
	}else if(!strcmp(cmd, "showinfo")){
	
		sscanf(val_string, "%s %d %d", cmd, &para1, &para2);
		fe_show_gdm_sptag_info(para1, para2);
			
	}else if(!strcmp(cmd, "help")){
		printk("echo set mapping [sptag_idx(0-15)] [flow_idx(0-31)] >/proc/tc3162/gdm3_sptag \n");
		printk("echo set srctagoffset [flow_offset(0-15)] [sptagoffset(0-31)] >/proc/tc3162/gdm3_sptag \n");
		printk("echo set sptagtxbypass [enable(0-1)]  >/proc/tc3162/gdm3_sptag \n");
		printk("echo set sptagtpid [tpid(hex)]  >/proc/tc3162/gdm3_sptag \n");
		printk("echo set sptagmode [chnl(hex0-2)] [mode(hex0-3)] >/proc/tc3162/gdm3_sptag \n");
		printk("echo set vpm [vpm0(hex0-3)] [vpm1(hex0-3)][vpm2(hex0-3)] [vpm3(hex0-3)] >/proc/tc3162/gdm3_sptag \n");
		printk("GDM3 chnl mode  0: no operation 1:insert/remove mode 2: replace mode 3:replace when src tagged,insert/remove when src untagged \n");

	}

    return count ;
}
#if defined(TCSUPPORT_CPU_AN7552)
static void get_pse_port_cfg_oq_en(unsigned char pse_port, unsigned char max_oq_num, 
    unsigned char array[])
{
    unsigned char oq_id = 0;
    
    for(oq_id=0; oq_id<max_oq_num; oq_id++)
    {
        array[oq_id] = pse_cfg_get_oq_en(pse_port, oq_id);
    }
    
    return ;
}

static int fe_pse_oq_en_dump_proc(char *buf, char **start, off_t off, int count,
    int *eof, void *data)
{
    int index = 0;
    off_t begin = 0;
    off_t pos = 0;
    int i = 0;
    int j = 0;
	unsigned int reg_val = 0;
    unsigned char p0_oq_en[PSE_PORT0_QUEUE_NUM];      /* CDM1->QDMA1 */
    unsigned char p1_oq_en[PSE_PORT1_QUEUE_NUM];      /* GDM1->GSW */
    unsigned char p2_oq_en[PSE_PORT2_QUEUE_NUM];      /* GDM2->PON*/
    unsigned char p3_oq_en[PSE_PORT3_QUEUE_NUM];      /* GDM3->XFI */
    unsigned char p4_oq_en[PSE_PORT4_QUEUE_NUM];      /* PPE */
    unsigned char p5_oq_en[PSE_PORT5_QUEUE_NUM];      /* CDM2->QDMA2 */
	unsigned char p6_oq_en[PSE_PORT6_QUEUE_NUM];      /* GDM4->WOE */
	unsigned char p7_oq_en[PSE_PORT7_QUEUE_NUM];      /* PPE2 */
	unsigned char p8_oq_en[PSE_PORT8_QUEUE_NUM];      /* CDM3->NPU */
	unsigned char p9_oq_en[PSE_PORT9_QUEUE_NUM];      /* CMD4->TDMA */
    unsigned char p10_oq_en[PSE_PORT10_QUEUE_NUM];    /* Copy Engine */
	unsigned char p15_oq_en[PSE_PORT15_QUEUE_NUM];    /* Free Port */

    get_pse_port_cfg_oq_en(0,  PSE_PORT0_QUEUE_NUM, p0_oq_en);
    get_pse_port_cfg_oq_en(1,  PSE_PORT1_QUEUE_NUM, p1_oq_en);
    get_pse_port_cfg_oq_en(2,  PSE_PORT2_QUEUE_NUM, p2_oq_en);
    get_pse_port_cfg_oq_en(3,  PSE_PORT3_QUEUE_NUM, p3_oq_en);
    get_pse_port_cfg_oq_en(4,  PSE_PORT4_QUEUE_NUM, p4_oq_en);
    get_pse_port_cfg_oq_en(5,  PSE_PORT5_QUEUE_NUM, p5_oq_en);
    
    get_pse_port_cfg_oq_en(6,  PSE_PORT6_QUEUE_NUM,  p6_oq_en);
    get_pse_port_cfg_oq_en(7,  PSE_PORT7_QUEUE_NUM,  p7_oq_en);
    get_pse_port_cfg_oq_en(8,  PSE_PORT8_QUEUE_NUM,  p8_oq_en);
    get_pse_port_cfg_oq_en(9,  PSE_PORT9_QUEUE_NUM,  p9_oq_en);
    get_pse_port_cfg_oq_en(10, PSE_PORT10_QUEUE_NUM, p10_oq_en);
    get_pse_port_cfg_oq_en(15, PSE_PORT15_QUEUE_NUM, p15_oq_en);

    for(i=0; i < PSE_PORT0_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P0_OQ%d_EN: %d    ", i, p0_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT1_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P1_OQ%d_EN: %d    ", i, p1_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT2_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P2_OQ%d_EN: %d    ", i, p2_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT3_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P3_OQ%d_EN: %d    ", i, p3_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT4_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P4_OQ%d_EN: %d    ", i, p4_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT5_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P5_OQ%d_EN: %d    ", i, p5_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT6_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P6_OQ%d_EN: %d    ", i, p6_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT7_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P7_OQ%d_EN: %d    ", i, p7_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
        
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT8_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P8_OQ%d_EN: %d    ", i, p8_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT9_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P9_OQ%d_EN: %d    ", i, p9_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT10_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P10_OQ%d_EN: %d    ", i, p10_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    for(i=0; i < PSE_PORT15_QUEUE_NUM; i++)
    {
        index += sprintf(buf+index, "P15_OQ%d_EN: %d    ", i, p15_oq_en[i]);
        CHK_BUF();
        if((i) && (0 == (i%6)))
        {
            index += sprintf(buf+index, "\n");
            CHK_BUF();
        }
    }
    index += sprintf(buf+index, "\n");
    CHK_BUF();
    
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
static int fe_oq_cnt_max_dump_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    int index = 0;
    off_t begin = 0;
    off_t pos = 0;
    int port_num = 0;
    int queue_num = 0;
	unsigned int reg_val = 0;
    uint16 share_use_max = 0;
    unsigned short port_deq_cnt[PSE_PORT_NUM_MAX][PSE_PORT_QUEUE_MAX];
    unsigned short port_rls_cnt[PSE_PORT_NUM_MAX][PSE_PORT_QUEUE_MAX];
    unsigned char port_queue_num[PSE_PORT_NUM_MAX] = PSE_PORT_QUEUE_NUM_ARRAY;

    share_use_max = (read_reg_word(PSE_BUF_USE_REC)>>PSE_SHARE_BUF_USED_MAX_SHIFT)&PSE_SHARE_BUF_USED_MAX_MASK;
	index += sprintf(buf+index, "share use max 0x%x\n",share_use_max);
        CHK_BUF();

    for(port_num = 0;port_num < PSE_PORT_NUM_MAX;port_num++)
    {
        if((0 == port_queue_num[port_num]))
            continue;

        index += sprintf(buf+index, "\nP%d_OQ_CNT_MAX:\n",port_num);
        CHK_BUF();

        for(queue_num = 0;queue_num < port_queue_num[port_num];queue_num++)
        {
            write_reg_word(PSE_QUEUE_CFG_WR, (port_num<<PSE_CFG_PORT_ID_SHIFT)|(queue_num<<PSE_CFG_QUEUE_ID_SHIFT));
            reg_val = read_reg_word(PSE_OQ_PCNT);
            port_deq_cnt[port_num][queue_num] = ((reg_val>>16) & 0x3fff);
            port_rls_cnt[port_num][queue_num] = (reg_val & 0x3fff);

            if((0 == (queue_num+1)%4)||(queue_num == (port_queue_num[port_num]-1)))
            {
                index += sprintf(buf+index, "OQ%d_MAX=(0x%04x)(0x%04x)\n", queue_num,port_deq_cnt[port_num][queue_num], port_rls_cnt[port_num][queue_num]);
                CHK_BUF();
            } 
            else
            {
                index += sprintf(buf+index, "OQ%d_MAX=(0x%04x)(0x%04x), ", queue_num,port_deq_cnt[port_num][queue_num], port_rls_cnt[port_num][queue_num]);
                CHK_BUF();
            }
        }
    }

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

int feSetFragmentForcePort(unsigned char port, unsigned char nbq)
{
	IO_SMASK(IP_ASSEM_FRAG_FP_CFG, IP_FRAG_PORT_MASK, IP_FRAG_PORT_SHIFT, port);
	IO_SMASK(IP_ASSEM_FRAG_FP_CFG, IP_FRAG_NBQ_MASK, IP_FRAG_NBQ_SHIFT, nbq);

	return 0 ;
}

int feSetAssembleForcePort(unsigned char port, unsigned char nbq)
{
	IO_SMASK(IP_ASSEM_FRAG_FP_CFG, IP_ASSEM_PORT_MASK, IP_ASSEM_PORT_SHIFT, port);
	IO_SMASK(IP_ASSEM_FRAG_FP_CFG, IP_ASSEM_NBQ_MASK, IP_ASSEM_NBQ_SHIFT, nbq);

	return 0 ;
}

int feSetGdma2PadEnable(void)
{
	if(SUPPORT_GDMA2)
		IO_SBITS(GDMA2_FWD_CFG, GDMA_PAD_EN_BIT);

	return 0 ;
}

int feSetGdma3Gdma4PadEnable(void)
{
	if(SUPPORT_GDMA3)
		IO_SBITS(GDMA3_FWD_CFG, GDMA_PAD_EN_BIT);

	if(SUPPORT_GDMA4)
		IO_SBITS(GDMA4_FWD_CFG, GDMA_PAD_EN_BIT);

	return 0 ;
}

int feSetTunTblCfg(unsigned int index, unsigned int offset, unsigned int type)
{

	uint tunTblCfg = 0 ;

	tunTblCfg = (TUN_TBL_CFG_REQ | 
					(((index&TUN_TBL_CFG_TABLE_ID_MASK)<<TUN_TBL_CFG_TABLE_ID_SHIFT)) |
					(((offset&TUN_TBL_CFG_OFFSET_ID_MASK)<<TUN_TBL_CFG_OFFSET_ID_SHIFT)) |
					((type<<TUN_TBL_CFG_RWCMD_SHIFT))) ;


	IO_SREG(TUN_TBL_CFG, tunTblCfg) ;

	if(feChecConfigDone(TUN_TBL_CFG, TUN_TBL_CFG_RWCMD_DONE) < 0) {
		printk("Timeout for set tunnel table configuration.\n") ;
		return -ETIME ;
	}

	return 0 ;


}

int feSetWanIP6Cfg(unsigned int index, unsigned int offset, unsigned int type)
{

	uint wanip6Cfg = 0 ;

	wanip6Cfg = (type | 
					(((index&WAN_IP6_CFG_IP_ID_MASK)<<WAN_IP6_CFG_IP_ID_SHIFT)) |
					(((offset&WAN_IP6_CFG_IP_OFST_MASK)<<WAN_IP6_CFG_IP_OFST_SHIFT)) ) ;


	IO_SREG(WAN_IP6_CFG, wanip6Cfg) ;

	
	return 0 ;

}



int fe_dump_tunnel_table(int tableIndex)
{
	tunnelTable_t tunnel_table = {0};
	
	feSetTunTblCfg(tableIndex, FE_TUN_SEL_CFG, FE_TUN_CMD_READ);	
	tunnel_table.config.word= IO_GREG(TUN_TBL_DATA0) ;


	feSetTunTblCfg(tableIndex, FE_TUN_SEL_BANK0, FE_TUN_CMD_READ);	
	tunnel_table.mac_header0.word= IO_GREG(TUN_TBL_DATA3) ;
	tunnel_table.mac_header1.word= IO_GREG(TUN_TBL_DATA2) ;
	tunnel_table.mac_header2.word= IO_GREG(TUN_TBL_DATA1) ;
	tunnel_table.mac_header3.word= IO_GREG(TUN_TBL_DATA0) ;


	feSetTunTblCfg(tableIndex, FE_TUN_SEL_BANK1, FE_TUN_CMD_READ);	
	tunnel_table.ip_header0.word= IO_GREG(TUN_TBL_DATA3) ;
	tunnel_table.ip_header1.word= IO_GREG(TUN_TBL_DATA2) ;
	tunnel_table.ip_header2.word= IO_GREG(TUN_TBL_DATA1) ;
	tunnel_table.ip_header3.word= IO_GREG(TUN_TBL_DATA0) ;


	feSetTunTblCfg(tableIndex, FE_TUN_SEL_BANK2, FE_TUN_CMD_READ);	
	tunnel_table.ip_header4.word= IO_GREG(TUN_TBL_DATA3) ;
	tunnel_table.ip_header5.word= IO_GREG(TUN_TBL_DATA2) ;
	tunnel_table.ip_header6.word= IO_GREG(TUN_TBL_DATA1) ;
	tunnel_table.ip_header7.word= IO_GREG(TUN_TBL_DATA0) ;


	feSetTunTblCfg(tableIndex, FE_TUN_SEL_BANK3, FE_TUN_CMD_READ);	
	tunnel_table.l2tp_header.word= IO_GREG(TUN_TBL_DATA3) ;
	tunnel_table.gre_header.word= IO_GREG(TUN_TBL_DATA2) ;
	tunnel_table.vxlan_header.word= IO_GREG(TUN_TBL_DATA1) ;
	tunnel_table.udp_header.word= IO_GREG(TUN_TBL_DATA0) ;

	printk("=============<TUNNEL TABLE %d>=============\n", tableIndex);

	printk("config format:is_layer2_tunnel(1)|use_Inner_TTL(1)|use_Inner_DSCP(1)|GRE_KEY_FLAG(1) |L2TP_OVER_IP(1)|L2TPV3(1)|RSV(6)| PPPOE_EN(1)|vlan_en(1)|VPM(2) |TUN0(4)|TUN1(4)|TUN2(4)|TUN3(4) \n");

	printk("\n%08x ", tunnel_table.config.word);

	printk("\n%08x \t",	tunnel_table.mac_header0.word);
	printk("/		 DMAC[47:16]		  /");

	printk("\n%08x \t",	tunnel_table.mac_header1.word);
	printk("/	DMAC[15:0]|	DMAC[47:32]	  /");

	printk("\n%08x \t",	tunnel_table.mac_header2.word);
	printk("/		 SMAC[31:0]		  	  /");

	printk("\n%08x \t",	tunnel_table.mac_header3.word);
	printk("/	VLAN ID	  |	PPPOE ID	  /");


	printk("\n%08x \t",	tunnel_table.ip_header0.word);
	printk("/TTL(8)|DSCP(8)|RSV(16)	for ipv4 \t/");
	printk("/HOP_LIMIT(8)|TRAFFIC_CLASS(8)|RSV(16)	for ipv6 \t/");

	printk("\n%08x \t",	tunnel_table.ip_header1.word);
	printk("/	Identification(16)|	ipv4 pre CKS(16) for ipv4 \t /");
	printk("/	flow label(24)|RSV(5)|SIP_ID(3) for ipv6  /");

	printk("\n%08x \t",	tunnel_table.ip_header2.word);
	printk("/		 SIP for ipv4	  	\t  /");
	printk("/		 DIP[127:96] for ipv6\t  /");

	printk("\n%08x \t",	tunnel_table.ip_header3.word);
	printk("/		 DIP for ipv4	  	\t  /");
	printk("/		 DIP[95:64] for ipv6\t  /");


	printk("\n%08x \t",	tunnel_table.ip_header4.word);
	printk("/		 RSV for ipv4	  	\t  /");
	printk("/		 DIP[63:32] for ipv6\t  /");

	printk("\n%08x \t",	tunnel_table.ip_header5.word);
	printk("/		 RSV for ipv4	  	\t  /");
	printk("/		 DIP[31:0] for ipv6\t  /");

	
	printk("\n%08x \t",	tunnel_table.ip_header6.word);
	printk("/		 RSV for ipv4	  	\t  /");
	printk("/		 RSV for ipv6\t  /");

	printk("\n%08x \t",	tunnel_table.ip_header7.word);
	printk("/		 RSV for ipv4	  	\t  /");
	printk("/		 RSV for ipv6\t  		/");


	printk("\n%08x \t",	tunnel_table.l2tp_header.word);
	printk("/		TUNNEL id + session id for L2TP Header	  	\t  /");

	printk("\n%08x \t",	tunnel_table.gre_header.word);
	printk("/		Virtual subnet id(24)|Flow id(8) for GRE Header	  	\t  /");


	printk("\n%08x \t",	tunnel_table.vxlan_header.word);
	printk("/		VNI(24)|RSV(8)	  for VXLAN Header	\t  /");

	printk("\n%08x \t",	tunnel_table.udp_header.word);
	printk("/		udp src(16)|udp dst(16))	 for USP Header 	\t  /");
	
	printk("\n");

	return 0;
}



int fe_set_tunnel_info(unsigned int tableIndex, unsigned int offset, unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3)
{

	
	IO_SREG(TUN_TBL_DATA3, value0) ;
	IO_SREG(TUN_TBL_DATA2, value1) ;
	IO_SREG(TUN_TBL_DATA1, value2) ;
	IO_SREG(TUN_TBL_DATA0, value3) ;

	feSetTunTblCfg(tableIndex, offset, FE_TUN_CMD_WRITE);	

	return 0;
}

/*value 0 ipv6[127:96]  value 1 ipv6[95:64]   value 0 ipv6[63:32]   value 0 ipv6[31:0]  */
int fe_set_wanipv6_info(unsigned int ipIndex,unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3)
{

	
	IO_SREG(WAN_IP6_WDATA, value0) ;
	feSetWanIP6Cfg(ipIndex, 3, FE_TUN_CMD_WRITE);
	
	IO_SREG(WAN_IP6_WDATA, value1) ;
	feSetWanIP6Cfg(ipIndex, 2, FE_TUN_CMD_WRITE);
	IO_SREG(WAN_IP6_WDATA, value2) ;
	feSetWanIP6Cfg(ipIndex, 1, FE_TUN_CMD_WRITE);
	IO_SREG(WAN_IP6_WDATA, value3) ;
	feSetWanIP6Cfg(ipIndex, 0, FE_TUN_CMD_WRITE);

	return 0;
}



int fe_dump_wanip6_table(int ipIndex)
{

	unsigned int ip0 = 0, ip1 = 0, ip2 = 0 ,ip3 = 0 ;

	feSetWanIP6Cfg(ipIndex, 3, FE_TUN_CMD_READ);
	ip0 = IO_GREG(WAN_IP6_RDATA);

	feSetWanIP6Cfg(ipIndex, 2, FE_TUN_CMD_READ);
	ip1 = IO_GREG(WAN_IP6_RDATA);

	feSetWanIP6Cfg(ipIndex, 1, FE_TUN_CMD_READ);
	ip2 = IO_GREG(WAN_IP6_RDATA);

	feSetWanIP6Cfg(ipIndex, 0, FE_TUN_CMD_READ);
	ip3 = IO_GREG(WAN_IP6_RDATA);

	printk("ip_id %d ipv6 ip[127:96] %08x  ipv6 ip[96:64] %08x ipv6 ip[63:32] %08x ipv6 ip[31:0] %08x \n", ipIndex, ip0, ip1, ip2, ip3);
	
	return 0;		
}



static int fe_tunnel_table_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	
	return 0;			
}

static int fe_tunnel_table_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char val_string[64], cmd[16] ;
	int value0, value1, value2, value3, table_index, table_offset = 0; 

	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;
    
	sscanf(val_string, "%s %d %d %x %x %x %x", cmd, &table_index, &table_offset, &value0, &value1,&value2,&value3) ;

	if(!strcmp(cmd, "dump")) {
		printk("dump table %d info \n",table_index );
		fe_dump_tunnel_table(table_index);
	}else if(!strcmp(cmd, "set")){
		printk("set table %d bank %d info \n",table_index, table_offset);
		printk("info0 %08x info1 %08x info2 %08x info3 %08x\n",value0, value1, value2, value3 );
		fe_set_tunnel_info(table_index, table_offset, value0, value1, value2, value3);
	}else if(!strcmp(cmd, "setip")){
		printk("set ipv6 %d rsv %d \n",table_index, table_offset);
		printk("info0 %08x info1 %08x info2 %08x info3 %08x\n",value0, value1, value2, value3 );
		fe_set_wanipv6_info(table_index, value0, value1, value2, value3);
	}else if(!strcmp(cmd, "dumpipv6")) {
		printk("dump ipv6 table %d info \n",table_index );
		fe_dump_wanip6_table(table_index);
	}

	return count;
}

static int channel_retire_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	if(CHANNEL_RETIRE==channel_retire){
		printk("do channel retire when pon up to down\n");
	}
	else{
		printk("do chnnel drop when pon up to down\n");
	}
	return 0;			
}

static int channel_retire_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];
	int i, len = 0; 

	if (count >= (sizeof(get_buf) - 1))
		len = sizeof(get_buf) - 1;
	else 
		len = count;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;

	get_buf[len] = '\0';
	
	sscanf(get_buf, "%d",&channel_retire);
	
	if(channel_retire!=CHANNEL_RETIRE)
		channel_retire = CHANNEL_DROP;

	return len;
}


static int cwmp_port_config_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("cwmp_vip_enable=%d, cwmp_request_dport=%d, cwmp_connect_sport=%d\n", cwmp_vip_enable, cwmp_request_dport, cwmp_connect_sport);
	return 0;
}

static int cwmp_port_config_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64] ;

	#if defined(TCSUPPORT_CPU_EN7580)
	IFC_API_DEL_VIP_BY_PORT(cwmp_connect_sport,cwmp_request_dport);
	#endif
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%d %d %d", &cwmp_vip_enable, &cwmp_request_dport, &cwmp_connect_sport) ;

	#if defined(TCSUPPORT_CPU_EN7580)
    if(cwmp_vip_enable)
	    IFC_API_SET_VIP_BY_PORT(cwmp_connect_sport,cwmp_request_dport);
	#endif
	
	return count ;
}

static int wan_itf_and_dev_bandwidth_mode_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("wan_itf_and_dev_bandwidth_mode=%d\n", wan_itf_and_dev_bandwidth_mode);
	return 0;
}

static int wan_itf_and_dev_bandwidth_mode_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64] = {0};
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	if(!strncasecmp(val_string, "on", strlen("on"))) {
		wan_itf_and_dev_bandwidth_mode = FE_ON ;
	} else if(!strncasecmp(val_string, "off", strlen("off"))) {
		wan_itf_and_dev_bandwidth_mode = FE_OFF ;
	} else {
		printk("usage: echo [on/off] > /proc/tc3162/wan_itf_and_dev_bandwidth_mode");
		return -EFAULT;
	}
    
	return count ;
}

static int wan_itf_and_dev_bandwidth_timeout_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("wan_itf_and_dev_bandwidth_timeout=%ds\n", wan_itf_and_dev_bandwidth_timeout);
	return 0;
}

static int wan_itf_and_dev_bandwidth_timeout_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64] ;
	int num=0;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	if(!sscanf(val_string, "%d", &num))
	{
		printk("usage: echo 0~600 > /proc/tc3162/wan_itf_and_dev_bandwidth_timeout");
		return -EFAULT;
	}    

	wan_itf_and_dev_bandwidth_timeout = num;
    
	return count ;
}

static int wan_itf_and_dev_bandwidth_max_num_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("wan_itf_and_dev_bandwidth_max_num=%d\n", wan_itf_and_dev_bandwidth_max_num);
	return 0;
}

static int wan_itf_and_dev_bandwidth_max_num_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64] ;
	int num=0;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	if(!sscanf(val_string, "%d", &num))
	{
		printk("usage: echo [num] > /proc/tc3162/wan_itf_and_dev_bandwidth_max_num\n");
		return -EFAULT;
	}    

	wan_itf_and_dev_bandwidth_max_num = num;
    
	return count ;
}

static int wan_itf_and_dev_bandwidth_total_account_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int index=0, i=0;
    off_t pos=0, begin=0 ;
    uint softUpPkts[ACNT_GRP_WAN_NUM]={0}, softUpByte_L[ACNT_GRP_WAN_NUM]={0}, softUpByte_H[ACNT_GRP_WAN_NUM]={0};
    uint softDownPkts[ACNT_GRP_WAN_NUM]={0}, softDownByte_L[ACNT_GRP_WAN_NUM]={0}, softDownByte_H[ACNT_GRP_WAN_NUM]={0};
    int wan_itf_index[ACNT_GRP_WAN_NUM] = {0} ;
    struct list_head *pos2 = NULL;
    struct list_head *tmp = NULL;
    struct list_head *head = &total_account_list->total_account_node;
    wan_itf_or_dev_bandwidth_total_account_t *current_node = NULL;
    wan_itf_or_dev_bandwidth_total_account_t *new_node = NULL;
    dev_bandwidth_account_t dev_account;
    
    if(wan_itf_and_dev_bandwidth_mode == FE_OFF) {
        index += sprintf(buf+index, "wan interface and dev bandwidth mode is off now.\n");
        CHK_BUF();
        goto done;
    }

    index += sprintf(buf+index, "wan interface and dev bandwidth mode is on, current timeout threshold is %ds\n", wan_itf_and_dev_bandwidth_timeout);
    CHK_BUF();
    
    index += sprintf(buf+index, "\nwan interface account summury:\n");
    CHK_BUF();
    index += sprintf(buf+index, "WAN_ITF_ID\tUpstram:(byte)(packet)\t\tDownstream:(byte)(packet)\n");
    CHK_BUF();
    read_lock_bh(&account_lock);
    list_for_each(pos2, head) {
        current_node = list_entry(pos2, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
        if(current_node->account_type == ACCOUNT_TYPE_WAN_INTERFACE) { 
            if(wan_itf_info[current_node->wan_itf_index].wanIndex != -1) {
                wan_itf_index[current_node->wan_itf_index] = 1;
            }
        }
    }
    read_unlock_bh(&account_lock);
    
    for(i=0; i<ACNT_GRP_WAN_NUM; i++) {
        if(wan_itf_index[i] == 1) {
            fe_get_wan_itf_cnt(i, UPSTREAM_ONLY, &softUpByte_L[i], &softUpByte_H[i], &softUpPkts[i]);
            fe_get_wan_itf_cnt(i, DOWNSTREAM_ONLY, &softDownByte_L[i], &softDownByte_H[i], &softDownPkts[i]);
        }
    }
    
    write_lock_irq(&account_lock);
    list_for_each(pos2, head) {
        current_node = list_entry(pos2, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
        if(current_node->account_type == ACCOUNT_TYPE_WAN_INTERFACE) { 
            if(wan_itf_info[current_node->wan_itf_index].wanIndex == -1) {
                tmp = pos2->prev;
                kfree(current_node);
                list_del(pos2);
                pos2 = tmp;
            } else {
                wan_itf_index[current_node->wan_itf_index] = 1;
                i = current_node->wan_itf_index;
                index += sprintf(buf+index, "nas%d_%d\t\t(%03llu)(%02d)\t\t\t(%03llu)(%02d)\n"
                    , wan_itf_info[current_node->wan_itf_index].wanIndex / ACNT_GRP_WAN_NUM
                    , wan_itf_info[current_node->wan_itf_index].wanIndex % ACNT_GRP_WAN_NUM
                    , ((unsigned long long)(current_node->upstream_total_byte_hi + softUpByte_H[i]) << 32)
                    + current_node->upstream_total_byte_lo + softUpByte_L[i]
                    , current_node->upstream_total_packet + softUpPkts[i]
                    , ((unsigned long long)(current_node->downstream_total_byte_hi + softDownByte_H[i]) << 32)
                    + current_node->downstream_total_byte_lo + softDownByte_L[i]
                    , current_node->downstream_total_packet + softDownPkts[i]);
                CHK_BUF_WRITE_UNLOCK(&account_lock);
            }
        }
    }
    write_unlock_irq(&account_lock);

    for(i=0; i<ACNT_GRP_WAN_NUM; i++) {
        if((wan_itf_info[i].valid == 1) && (wan_itf_index[i] == 0)) {
        	printk("fe: allocate new_node: type=WAN_INTERFACE, wan_itf_index[%d]=%d(%s)\n"
                , i, wan_itf_info[i].wanIndex, wan_itf_info[i].wan_dev->name);
        	new_node = kzalloc(sizeof(wan_itf_or_dev_bandwidth_total_account_t), GFP_KERNEL);
        	if(new_node == NULL){	
        		printk(KERN_WARNING "fe: allocate memory for total_account_list fail\n");
                goto done;
            }
            new_node->account_type = ACCOUNT_TYPE_WAN_INTERFACE;
            new_node->wan_itf_index = i;
            write_lock_irq(&account_lock);
            list_add_tail(&new_node->total_account_node, head);
            write_unlock_irq(&account_lock);
            fe_get_wan_itf_cnt(i, UPSTREAM_ONLY, &softUpByte_L[i], &softUpByte_H[i], &softUpPkts[i]);
            fe_get_wan_itf_cnt(i, DOWNSTREAM_ONLY, &softDownByte_L[i], &softDownByte_H[i], &softDownPkts[i]);
            index += sprintf(buf+index, "nas%d_%d\t\t(%03llu)(%02d)\t\t\t(%03llu)(%02d)\n"
                , wan_itf_info[i].wanIndex / ACNT_GRP_WAN_NUM, wan_itf_info[i].wanIndex % ACNT_GRP_WAN_NUM
                , ((unsigned long long)softUpByte_H[i] << 32) + softUpByte_L[i], softUpPkts[i]
                , ((unsigned long long)softDownByte_H[i] << 32) + softDownByte_L[i], softDownPkts[i]);
            CHK_BUF();
        }
    }
    
    index += sprintf(buf+index, "\ndev bandwidth account summury:\n");
    CHK_BUF();
    index += sprintf(buf+index, "MAC Address\t\tUpstram:(byte)(packet)\t\tDownstream:(byte)(packet)\n");
    CHK_BUF();
    read_lock_bh(&account_lock);
    list_for_each(pos2, head) {
        current_node = list_entry(pos2, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
        if(current_node->account_type == ACCOUNT_TYPE_DEV_BANDWIDTH) {
            memset(&dev_account, 0, sizeof(dev_bandwidth_account_t));
            memcpy(dev_account.mac, current_node->mac, ETH_ALEN);
            if(dev_bandwidth_hook_get_cnt) {
        		dev_bandwidth_hook_get_cnt(&dev_account);
            }
            softUpByte_L[0] = dev_account.upstream_byte_lo;
            softUpByte_H[0] = dev_account.upstream_byte_hi;
            softUpPkts[0] = dev_account.upstream_packet;
            softDownByte_L[0] = dev_account.downstream_byte_lo;
            softDownByte_H[0] = dev_account.downstream_byte_hi;
            softDownPkts[0] = dev_account.downstream_packet;
            
            index += sprintf(buf+index, "%pM\t(%03llu)(%02d)\t\t\t(%03llu)(%02d)\n", current_node->mac
                , ((unsigned long long)(current_node->upstream_total_byte_hi + softUpByte_H[0]) << 32) 
                + current_node->upstream_total_byte_lo + softUpByte_L[0]
                , current_node->upstream_total_packet + softUpPkts[0]
                , ((unsigned long long)(current_node->downstream_total_byte_hi + softDownByte_H[0]) << 32) 
                + current_node->downstream_total_byte_lo + softDownByte_L[0]
                , current_node->downstream_total_packet + softDownPkts[0]);
            CHK_BUF_READ_UNLOCK(&account_lock);
        }
    }
    read_unlock_bh(&account_lock);
    
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

static int wan_itf_and_dev_bandwidth_total_account_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    char val_string[64] ;
    int num=0;
    struct list_head *pos = NULL;
    struct list_head *head = &total_account_list->total_account_node;
    wan_itf_or_dev_bandwidth_total_account_t *current_node = NULL;
	
    if (count > sizeof(val_string) - 1)
        return -EINVAL ;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT ;

    if(wan_itf_and_dev_bandwidth_mode == FE_OFF) {
        printk("wan interface and dev bandwidth mode is off now.\n");
        return -EFAULT;
    }

    if(!sscanf(val_string, "%d", &num))
    {
        printk("usage: echo 0 > /proc/tc3162/wan_itf_and_dev_bandwidth_total_account");
        return -EFAULT;
    }

    if(num == 0) {
        /* clear all hw path total account */
        write_lock_irq(&account_lock);
        list_for_each(pos, head) {
            current_node = list_entry(pos, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
            if((current_node->account_type == ACCOUNT_TYPE_WAN_INTERFACE) 
                || (current_node->account_type == ACCOUNT_TYPE_DEV_BANDWIDTH)) {
                current_node->upstream_total_byte_hi = 0;
                current_node->upstream_total_byte_lo = 0;
                current_node->upstream_total_packet = 0;
                current_node->downstream_total_byte_hi = 0;
                current_node->downstream_total_byte_lo = 0;
                current_node->downstream_total_packet = 0;
            } else {
                printk("Error: current_node->account_type is %d\n", current_node->account_type);
            }
        }
        write_unlock_irq(&account_lock);
        
        /* clear all wan interface soft account */
        read_lock_bh(&account_lock);
        list_for_each(pos, head) {
            current_node = list_entry(pos, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
            if((current_node->account_type == ACCOUNT_TYPE_WAN_INTERFACE)) {
                printk("clear wan itf soft cnt: (%d)(%d)\n", current_node->wan_itf_index
                    , wan_itf_info[current_node->wan_itf_index].wanIndex);
                fe_clear_wan_itf_cnt(current_node->wan_itf_index, UPSTREAM_ONLY);
                fe_clear_wan_itf_cnt(current_node->wan_itf_index, DOWNSTREAM_ONLY);
            }
        }
        read_unlock_bh(&account_lock);

        /* clear all dev bandwidth soft account */
        if(dev_bandwidth_hook_clear_cnt) {
    		dev_bandwidth_hook_clear_cnt();
        }
    }
    
    return count ;
}

int fe_add_dev_to_total_account(unsigned char *mac)
{
    int foundDevFlag=0;
    struct list_head *pos = NULL;
    struct list_head *head = &total_account_list->total_account_node;
    wan_itf_or_dev_bandwidth_total_account_t *current_node = NULL;
    wan_itf_or_dev_bandwidth_total_account_t *new_node = NULL;
    const unsigned char empty[ETH_ALEN] = {0};
    static int printed_flag = 0;	//only print once

    if(memcmp(mac, empty, ETH_ALEN) == 0)
        return 0;
    
    if(current_dev_mac_num >= wan_itf_and_dev_bandwidth_max_num) {
        if(!printed_flag)
        {
            printed_flag = 1;
            printk("current_dev_mac_num:%d, max_num:%d\n", current_dev_mac_num, wan_itf_and_dev_bandwidth_max_num);
        }
        return 0;
    }
    
    read_lock_bh(&account_lock);
    list_for_each(pos, head) {
        current_node = list_entry(pos, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
        if((current_node->account_type == ACCOUNT_TYPE_DEV_BANDWIDTH)
        && (memcmp(mac, current_node->mac, ETH_ALEN) == 0)) {
            foundDevFlag = 1;
        }
    }
    read_unlock_bh(&account_lock);
    
    if(foundDevFlag == 0) {
    	printk("fe: allocate new_node: type=DEV_BANDWIDTH, mac=%pM\n", mac);
    	new_node = kzalloc(sizeof(wan_itf_or_dev_bandwidth_total_account_t), GFP_ATOMIC);
    	if(new_node == NULL){	
    		printk(KERN_WARNING "fe: allocate memory for total_account_list fail\n");
    		return 0;
    	}
        current_dev_mac_num++;
        new_node->account_type = ACCOUNT_TYPE_DEV_BANDWIDTH;
        memcpy(new_node->mac, mac, ETH_ALEN);
        write_lock_irq(&account_lock);
        list_add_tail(&new_node->total_account_node, head);
        write_unlock_irq(&account_lock);
    }

    return 0;
}

static int stb_src_ip_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int index=0, i=0;
    off_t pos=0, begin=0 ;
    struct list_head *pos2 = NULL;
    struct list_head *head = &stb_src_ip_list->stb_src_ip_node;
    stb_src_ip_t *current_node = NULL;
    
    index += sprintf(buf+index, "mode:%s\n", (stb_src_ip_mode==FE_ON)?("on"):("off"));
    CHK_BUF();
    
    read_lock_bh(&stb_src_ip_lock);
    list_for_each(pos2, head) {
        current_node = list_entry(pos2, stb_src_ip_t, stb_src_ip_node);
        if(current_node->ip_type == 0)
            index += sprintf(buf+index, "%d: %pI4\n", ++i, &current_node->ip4);
        else
            index += sprintf(buf+index, "%d: %pI6c\n", ++i, &current_node->ip6);
        CHK_BUF_READ_UNLOCK(&stb_src_ip_lock);
    }
    read_unlock_bh(&stb_src_ip_lock);

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

static int stb_src_ip_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char val_string[64] ;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	if(!strncasecmp(val_string, "on", strlen("on"))) {
		stb_src_ip_mode = FE_ON ;
	} else if(!strncasecmp(val_string, "off", strlen("off"))) {
		stb_src_ip_mode = FE_OFF ;
	} else {
		printk("usage: echo [on/off] > /proc/tc3162/stb_src_ip");
		return -EFAULT;
	}
    
	return count ;
}


int fe_add_stb_src_ip(unsigned int ip_type, unsigned int ip4, struct in6_addr *ip6)
{
    int foundDevFlag=0;
    struct list_head *pos = NULL;
    struct list_head *head = &stb_src_ip_list->stb_src_ip_node;
    stb_src_ip_t *current_node = NULL;
    stb_src_ip_t *new_node = NULL;

    read_lock_bh(&stb_src_ip_lock);
    list_for_each(pos, head) {
        current_node = list_entry(pos, stb_src_ip_t, stb_src_ip_node);
        if((current_node->ip_type == ip_type)
        && (((ip_type==0) && (current_node->ip4==ip4)) 
        || ((ip_type==1) && (memcmp(ip6->s6_addr, current_node->ip6.s6_addr, 16) == 0)))) {
            foundDevFlag = 1;
            break;
        }
    }
    read_unlock_bh(&stb_src_ip_lock);
    
    if(foundDevFlag == 0) {
    	new_node = kzalloc(sizeof(stb_src_ip_t), GFP_KERNEL);
    	if(new_node == NULL){	
    		printk(KERN_WARNING "fe: allocate memory for stb_src_ip_list fail\n");
    		return 0;
    	}
        
        new_node->ip_type = ip_type;
        if(ip_type == 0) {
            new_node->ip4 = ip4;
    	    printk("fe: add stb src ipv4: %pI4\n", &ip4);
        } else {
            memcpy(new_node->ip6.s6_addr, ip6->s6_addr, 16);
    	    printk("fe: add stb src ipv6: %pI6c\n", ip6);
        }
        write_lock_irq(&stb_src_ip_lock);
        list_add_tail(&new_node->stb_src_ip_node, head);
        write_unlock_irq(&stb_src_ip_lock);
    }

    return 0;
}

int fe_del_stb_src_ip(unsigned int ip_type, unsigned int ip4, struct in6_addr *ip6)
{
    int foundDevFlag=0;
    struct list_head *pos = NULL;
    struct list_head *head = &stb_src_ip_list->stb_src_ip_node;
    stb_src_ip_t *current_node = NULL;

    write_lock_irq(&stb_src_ip_lock);
    list_for_each(pos, head) {
        current_node = list_entry(pos, stb_src_ip_t, stb_src_ip_node);
        if((current_node->ip_type == ip_type)
        && (((ip_type==0) && (current_node->ip4==ip4)) 
        || ((ip_type==1) && (memcmp(ip6->s6_addr, current_node->ip6.s6_addr, 16) == 0)))) {
            foundDevFlag = 1;
            kfree(current_node);
            list_del(pos);
            break;
        }
    }
    
    if(foundDevFlag == 0) {
    	printk("fe: delete stb src ip fail: this ip address can't be found\n");
    }

    write_unlock_irq(&stb_src_ip_lock);
    return 0;
}

int get_stb_src_ip4(unsigned int ip4)
{
    struct list_head *pos = NULL;
    struct list_head *head = &stb_src_ip_list->stb_src_ip_node;
    stb_src_ip_t *current_node = NULL;

    if(stb_src_ip_mode == FE_OFF)
        return 1;
    
    read_lock_bh(&stb_src_ip_lock);
    if(list_empty(head)) {
        read_unlock_bh(&stb_src_ip_lock);
        return 0;
    }
    
    list_for_each(pos, head) {
        current_node = list_entry(pos, stb_src_ip_t, stb_src_ip_node);
        if((current_node->ip_type==0) && (current_node->ip4==ip4)) {
            read_unlock_bh(&stb_src_ip_lock);
            return 1;
        }
    }

    read_unlock_bh(&stb_src_ip_lock);
    return 0;
}

int get_stb_src_ip6(struct in6_addr *ip6)
{
    struct list_head *pos = NULL;
    struct list_head *head = &stb_src_ip_list->stb_src_ip_node;
    stb_src_ip_t *current_node = NULL;

    if(stb_src_ip_mode == FE_OFF)
        return 1;
    
    read_lock_bh(&stb_src_ip_lock);
    if(list_empty(head)) {
        read_unlock_bh(&stb_src_ip_lock);
        return 0;
    }
    
    list_for_each(pos, head) {
        current_node = list_entry(pos, stb_src_ip_t, stb_src_ip_node);
        if((current_node->ip_type==1) && (memcmp(ip6->s6_addr, current_node->ip6.s6_addr, 16) == 0)) {
            read_unlock_bh(&stb_src_ip_lock);
            return 1;
        }
    }

    read_unlock_bh(&stb_src_ip_lock);
    return 0;
}

int fe_set_oq_rsv(int rsv_value)
{
	int i = 0;
	
	for(i = 0; i < PSE_PORT0_OQ_NUM; i++)
	{
		pse_set_oq_rsv(0, i, rsv_value);
	}

	for(i = 0; i < PSE_PORT1_OQ_NUM; i++)
	{
		pse_set_oq_rsv(1, i, rsv_value);
	}

	for(i = 0; i < PSE_PORT2_OQ_NUM; i++)
	{
		pse_set_oq_rsv(2, i, rsv_value);
	}

	for(i = 0; i < PSE_PORT3_OQ_NUM; i++)
	{
		pse_set_oq_rsv(3, i, rsv_value);
	}

	for(i = 0; i < PSE_PORT4_OQ_NUM; i++)
	{
		pse_set_oq_rsv(4, i, rsv_value);
	}

	for(i = 0; i < PSE_PORT5_OQ_NUM; i++)
	{
		pse_set_oq_rsv(5, i, rsv_value);
	}

	for(i = 0; i < PSE_PORT6_OQ_NUM; i++)
	{
		pse_set_oq_rsv(6, i, rsv_value);
	}

	for(i = 0; i < PSE_PORT7_OQ_NUM; i++)
	{
		pse_set_oq_rsv(7, i, rsv_value);
	}
	
	for(i = 0; i < PSE_PORT9_OQ_NUM; i++)
	{
		pse_set_oq_rsv(9, i, rsv_value);
	}

	for(i = 0; i < PSE_PORT10_OQ_NUM; i++)
	{
		pse_set_oq_rsv(10, i, rsv_value);
	}
	
	return 0;
}

static int fe_ratelimit_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return 0;			
}

static int fe_ratelimit_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int gdm_rc_cfg_id = -1;
	uint8_t byte_mode = 0;
	uint8_t val_string[64] = {0};
	unsigned int ratelimit_value = 0;
	struct ecnt_fe_data fe_data;
	
	if (count > (sizeof(val_string) - 1))
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	memset(&fe_data, 0x0, sizeof(struct ecnt_fe_data));
	
	sscanf(val_string, "%d %d %d %d %d", &gdm_rc_cfg_id, &fe_data.api_data.meter_cfg.meter_id, &fe_data.api_data.meter_cfg.rate, 
	&fe_data.api_data.meter_cfg.pkt_mode, &fe_data.index);
	
	if (FE_RX_RATE_LIMIT == gdm_rc_cfg_id)
	{
		printk("set meterID=%d, rate=%d, mode=%d, portID=0x%x\n", fe_data.api_data.meter_cfg.meter_id, fe_data.api_data.meter_cfg.rate, 
			fe_data.api_data.meter_cfg.pkt_mode, fe_data.index);
	}
	else
	{
		fe_data.gdm_sel = fe_data.index;
		fe_data.api_data.rate_cfg.rate = fe_data.api_data.meter_cfg.rate;
		fe_data.api_data.rate_cfg.mode = fe_data.api_data.meter_cfg.pkt_mode;
		printk("set gdm_sel= %d, gdm_rc_cfg_id=%d, rateLimit_value=%d, byte_mode=%d\n", fe_data.gdm_sel, gdm_rc_cfg_id, 
			fe_data.api_data.rate_cfg.rate, fe_data.api_data.rate_cfg.mode);
	}
	
	if (FE_WAN_RX_UC_RATE == gdm_rc_cfg_id)
	{
		fe_api_set_rxuc_rate(&fe_data);
	}
	else if (FE_WAN_RX_BC_RATE == gdm_rc_cfg_id)
	{
		fe_api_set_rxbc_rate(&fe_data);
	}
	else if (FE_WAN_RX_MC_RATE == gdm_rc_cfg_id)
	{
		fe_api_set_rxmc_rate(&fe_data);
	}
	else if (FE_WAN_RX_OC_RATE == gdm_rc_cfg_id)
	{
		fe_api_set_rxoc_rate(&fe_data);
	}
	else if (FE_WAN_TX_RATE_LIMIT == gdm_rc_cfg_id)
	{
		fe_api_set_tx_rate(&fe_data);
	}
	else if (FE_RX_RATE_LIMIT == gdm_rc_cfg_id)
	{
		fe_api_set_rx_rate(&fe_data);
	}
	else if (FE_WAN_RX_MAC_FILTER_RATE_LIMIT == gdm_rc_cfg_id)
	{
		fe_api_set_rx_mac_filter_rate(&fe_data);
	}
	else
	{
		printk("error gdm_rc_cfg_id, return. \n");
		return -EINVAL ;
	}
	
	return count;
}

static int  fe_power_down_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("usage: echo 1 2 > /proc/tc3162/fe_sram_pd\n");
	printk("para1: PSE SRAM PowerDown, 0: none, 1: All, 2: half\n");
	printk("para2: PPE SRAM PowerDown. 0: none, 1: All, 2: 1/4, 3: 3/4\n");
	
	return 0;
}

static int fe_power_down_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int pse_sram_pd = 0;
	int ppe_sram_pd = 0;
	uint32_t reg = 0;
	uint8_t val_string[64] = {0};
	
	if (count > (sizeof(val_string) - 1))
	{
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count))
	{
		return -EFAULT;
	}

	sscanf(val_string, "%d %d", &pse_sram_pd, &ppe_sram_pd);
	printk("PSE_SRAM_PD = %d, PPE_SRAM_PD = %d\n", pse_sram_pd, ppe_sram_pd);
	
	if (FE_SRAM_PD_ALL == pse_sram_pd)
	{
		/* power down the PSE ALL SRAM */
		IO_SMASK(FE_SRAM_PD0, PSE_PKT_SRAM_PD_MASK, PSE_PKT_SRAM_PD_SHIFT, 0xffff);
		
		/* enable half of PPE SRAM for PSE used */
		IO_SBITS(PSE_ALL_PD_CFG, PSE_ALL_PD_BIT);
		
		/* set PSE buffer to 0x1000 */
		write_reg_word(PSE_FQ_CFG, 0x1000);

		/* set PSE all rsv to 0x2AE0/4 */
		IO_SMASK(PSE_BUFF_SET, PSE_ALL_RSV_MASK, PSE_ALL_RSV_SHIFT, 0xab8);

		/* set share buff HTHD and LTHD: 0x1000 - 0xab8 - 0x300 */
		write_reg_word(PSE_SHARE_USED_THD, 0x1240248);
		
		/* fe core reset */
		write_reg_word(FE_RST_GLO, 0x1);
		mdelay(1);

		/* set PSE Port OQ RSV to 0x20 */
		fe_set_oq_rsv(0x20);
	}
	else if(FE_SRAM_PD_HALF == pse_sram_pd)
	{
		/* power down the PSE SRAM 1/2 */
		IO_SMASK(FE_SRAM_PD0, PSE_PKT_SRAM_PD_HALF_MASK, PSE_PKT_SRAM_PD_HALF_SHIFT, 0xff);
		
		/* set PSE buffer to half 0x4000/2 */
		write_reg_word(PSE_FQ_CFG, 0x2000);

		/* set PSE all rsv to half 0x2AE0/2 */
		IO_SMASK(PSE_BUFF_SET, PSE_ALL_RSV_MASK, PSE_ALL_RSV_SHIFT, 0x1570);

		/* set share buff HTHD and LTHD: 0x2000 - 0x1570 - 0x300 */
		write_reg_word(PSE_SHARE_USED_THD, 0x3c80790);

		/* fe core reset */
		write_reg_word(FE_RST_GLO, 0x1);
		mdelay(1);
		
		/* set PSE Port OQ RSV to 0x40 */
		fe_set_oq_rsv(0x40);
	}
	else if(FE_SRAM_PD_NOT == pse_sram_pd)
	{
		/* power up the PSE ALL SRAM */
		IO_SMASK(FE_SRAM_PD0, PSE_PKT_SRAM_PD_MASK, PSE_PKT_SRAM_PD_SHIFT, 0x0);
		
		/* disable half of PPE SRAM for PSE used */
		IO_CBITS(PSE_ALL_PD_CFG, PSE_ALL_PD_BIT);
		
		/* set PSE buffer to normal 0x4000 */
		write_reg_word(PSE_FQ_CFG, 0x4000);

		/* set PSE all rsv to HW default: 0x2AE0 */
		IO_SMASK(PSE_BUFF_SET, PSE_ALL_RSV_MASK, PSE_ALL_RSV_SHIFT, 0x2ae0);

		/* set share buff HTHD and LTHD: 0x4000 - 0x2ae0 - 0x300 */
		write_reg_word(PSE_SHARE_USED_THD, 0x9101220);
		
		/* fe core reset */
		write_reg_word(FE_RST_GLO, 0x1);
		mdelay(1);
		
		printk("PSE SRAM PD ignore\n");
	}
	else
	{
		printk("error PSE_PD_ID, return. \n");
		return -EINVAL;
	}

	if (FE_SRAM_PD_ALL == ppe_sram_pd)
	{
		/* power down the PPE ALL SRAM */
		write_reg_word(FE_SRAM_PD1, 0xffffffff);
		
		/* disable PPE SRAM Table */
		IO_CBITS(PPE_HASH_CFG, PPE_SRAM_EN_BIT);
	}
	else if(FE_SRAM_PD_HALF == ppe_sram_pd)
	{
		/* power down the PPE SRAM 1/4 */
		write_reg_word(FE_SRAM_PD1, 0xff00);

		/* enable PPE SRAM Table */
		IO_SBITS(PPE_HASH_CFG, PPE_SRAM_EN_BIT);
	}
	else if(FE_SRAM_PD_THFO == ppe_sram_pd)
	{
		/* power down the PPE SRAM 3/4 */
		write_reg_word(FE_SRAM_PD1, 0xffffff00);

		/* enable PPE SRAM Table */
		IO_SBITS(PPE_HASH_CFG, PPE_SRAM_EN_BIT);
	}
	else if(FE_SRAM_PD_NOT == ppe_sram_pd)
	{
		/* power up the PPE ALL SRAM */
		write_reg_word(FE_SRAM_PD1, 0x0);
		
		/* enable PPE SRAM Table */
		IO_SBITS(PPE_HASH_CFG, PPE_SRAM_EN_BIT);
		
		printk("PPE SRAM PD ignore\n");
	}
	else
	{
		printk("error PPE_PD_ID, return. \n");
		return -EINVAL;
	}
	
	return count;
}

void fe_reset(void)
{
	unsigned int regVal=0;

#ifdef TCSUPPORT_CPU_ARMV8
	regVal = GET_SCU_RST_CTRL_1();
	regVal |= (QDMA1_RST | QDMA2_RST | FE_RST);
	SET_SCU_RST_CTRL_1(regVal);
	mdelay(1);	
	regVal &= ~(QDMA1_RST | QDMA2_RST | FE_RST);
	mdelay(1);
	SET_SCU_RST_CTRL_1(regVal);
#else
	regVal = IO_GREG(CR_RSTCTRL2);
	regVal |= (QDMA1_RST | QDMA2_RST | FE_RST);
	IO_SREG(CR_RSTCTRL2, regVal);
	mdelay(1);	
	regVal &= ~(QDMA1_RST | QDMA2_RST | FE_RST);
	mdelay(1);
	IO_SREG(CR_RSTCTRL2, regVal);
#endif
}

static int fe_reset_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return 0;
}

static int fe_reset_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int reset_type = -1;
	uint8_t val_string[64] = {0};
	
	if (count > (sizeof(val_string) - 1))
	{
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count))
	{
		return -EFAULT;
	}
	
	sscanf(val_string, "%d", &reset_type);

	if (FE_SCU_RESET == reset_type)
		fe_reset();
	
	return count;
}

void pse_page_set_to_128Byte(void)
{
    int i=0, j=0;
    unsigned int reg=0, switch_val=0;
    unsigned int qdmaChnlEn[2][8];
    
    // disable switch port6, and qdma1 & qdma2 queue enable.
    for(i=0; i<2; i++) {
        if(i==0)
            reg = QDMA1_CHN_EN_BASE;
        else
            reg = QDMA2_CHN_EN_BASE;
        for(j=0; j<8; j++) {
            qdmaChnlEn[i][j] = read_reg_word(reg+(j<<2));
            write_reg_word(reg+(j<<2), 0);
        }
    }
    switch_val = read_reg_word(0xbfb5b600);
    write_reg_word(0xbfb5b600, 0x56330);
    
	// set PSE Page as 128Byte, then fe rest.
	reg = read_reg_word(FE_DMA_GLO_CFG);
	reg &= (~(L2_SPACE_MASK)) & (~(PSE_PAGE_SIZE));
	reg |= ((L2SPACE_VAL << L2_SPACE_OFFSET) | (PSE_PAGE_SIZE));
	write_reg_word(FE_DMA_GLO_CFG, reg);

	//fe core reset
	if(ARBITER_SUPPORT)
	{
		reg = (1<<FE_CORE_RESET_OFFSET) | (FE_GDM3_MBI_ARB_RST_OFFSET) | (FE_GDM4_MBI_ARB_RST_OFFSET);
		write_reg_word(FE_RST_GLO, reg);
	}
	else
	{
    	write_reg_word(FE_RST_GLO, 1<<FE_CORE_RESET_OFFSET);
	}
    
    mdelay(1);
    
    // enable switch port6, and qdma1 & qdma2 queue enable.
    write_reg_word(0xbfb5b600, switch_val);
    for(i=0; i<2; i++) {
        if(i==0)
            reg = QDMA1_CHN_EN_BASE;
        else
            reg = QDMA2_CHN_EN_BASE;
        for(j=0; j<8; j++) {
            write_reg_word(reg+(j<<2), qdmaChnlEn[i][j]);
        }
    }   
}

#if defined(TCSUPPORT_CPU_EN7521) || defined(TCSUPPORT_CPU_EN7512)
static int dyn_qdma_txq_cngst_threshold_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	printk("dyn_qdma_txq_cngst_en is %d.\n", dyn_qdma_txq_cngst_en);
	printk("dyn_qdma_txq_cngst_packet_len is %d.\n", dyn_qdma_txq_cngst_packet_len);
	printk("dyn_qdma_txq_cngst_stream_rate is %d.\n", dyn_qdma_txq_cngst_stream_rate);
	printk("dyn_qdma_txq_cngst_total_min is 0x%x.\n", dyn_qdma_txq_cngst_total_min);
	return 0;
}

static int dyn_qdma_txq_cngst_threshold_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char val_string[64], cmd[32];
    uint value=0;
	QDMA_TxQDynCngstTotalThrh_T total_threshold;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;
    
	sscanf(val_string, "%s %d", cmd, &value) ;
    
	if(!strcmp(cmd, "enable")) {
		dyn_qdma_txq_cngst_en = value;
		printk("dyn_qdma_txq_cngst_en is %d.\n", dyn_qdma_txq_cngst_en);
	} else if(!strcmp(cmd, "packet_len")) {
		dyn_qdma_txq_cngst_packet_len = value;
		if(dyn_qdma_txq_cngst_packet_len < 64) {
			dyn_qdma_txq_cngst_packet_len_reg = GDMA2_TX_ETH_LESS64_CNT;
		} else if(dyn_qdma_txq_cngst_packet_len == 64) {
			dyn_qdma_txq_cngst_packet_len_reg = GDMA2_TX_ETH_64_CNT;
		} else if(dyn_qdma_txq_cngst_packet_len < 128) {
			dyn_qdma_txq_cngst_packet_len_reg = GDMA2_TX_ETH_65_TO_127_CNT;
		} else if(dyn_qdma_txq_cngst_packet_len < 256) {
			dyn_qdma_txq_cngst_packet_len_reg = GDMA2_TX_ETH_128_TO_255_CNT;
		} else if(dyn_qdma_txq_cngst_packet_len < 512) {
			dyn_qdma_txq_cngst_packet_len_reg = GDMA2_TX_ETH_256_TO_511_CNT;
		} else if(dyn_qdma_txq_cngst_packet_len < 1024) {
			dyn_qdma_txq_cngst_packet_len_reg = GDMA2_TX_ETH_512_TO_1023_CNT;
		} else if(dyn_qdma_txq_cngst_packet_len < 1518) {
			dyn_qdma_txq_cngst_packet_len_reg = GDMA2_TX_ETH_1024_TO_1518_CNT;
		} else {
			dyn_qdma_txq_cngst_packet_len_reg = GDMA2_TX_ETH_MORE1518_CNT;
		} 
		printk("dyn_qdma_txq_cngst_packet_len is %d.\n", dyn_qdma_txq_cngst_packet_len);
		printk("dyn_qdma_txq_cngst_packet_len_reg is 0x%x.\n", dyn_qdma_txq_cngst_packet_len_reg);
	} else if(!strcmp(cmd, "stream_rate")) {
		dyn_qdma_txq_cngst_stream_rate = value;
		printk("dyn_qdma_txq_cngst_stream_rate is %d.\n", dyn_qdma_txq_cngst_stream_rate);
	} else if(!strcmp(cmd, "total_min")) {
		dyn_qdma_txq_cngst_total_min = value;
		printk("dyn_qdma_txq_cngst_total_min is 0x%x.\n", dyn_qdma_txq_cngst_total_min);
	} else {
		printk("echo enable [0/1] > /proc/tc3162/dyn_qdma_txq_cngst_threshold_write_proc \n");
		printk("echo packet_len [eg:1518 (unit is byte)] > /proc/tc3162/dyn_qdma_txq_cngst_threshold_write_proc \n");
		printk("echo stream_rate [eg:200(unit is mbps)] > /proc/tc3162/dyn_qdma_txq_cngst_threshold_write_proc \n");
		printk("echo total_min [eg:14336] > /proc/tc3162/dyn_qdma_txq_cngst_threshold_write_proc \n");
	}
	
	return count;
}
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_ENV)
extern int fhdrv_kdrv_get_cpu_mac(unsigned char *mac, unsigned int len);
#endif/*TCSUPPORT_COMPILE*/

void setup_mac_reg(void){
	unsigned char mac[6];
	unsigned char mac_range = 8;
	int i=0;
	unsigned int val=0;

#ifdef TCSUPPORT_CPU_ARMV8 
	if(get_ethaddr(mac, sizeof(mac)) != 0){
		printk("func:%s get ethaddr error!!!\n",__func__);
	}
#else
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_ENV)
	fhdrv_kdrv_get_cpu_mac((unsigned char *)(mac), 6);
#else/*TCSUPPORT_COMPILE*/
	for (i=0; i<6; i++) {
		mac[i] = READ_FLASH_BYTE(flash_base + 0xff48 + i);
	}
	mac_range = READ_FLASH_BYTE(flash_base + 0xffb5);
	/* if use old boot version, has no mac_range info, should use default range=8 */
	if(mac_range == 0){
		printk("setup_mac_reg: boot has no mac range info!!!\n");
		mac_range = 8;
	}
#endif/*TCSUPPORT_COMPILE*/
#endif
	if((isEN751627) || (FE_MAC_RANGE_SUPPORT))
	{
		val = (mac[3]<<16) | (mac[4]<<8) | mac[5];
		
		write_reg_word(FE_LAN_MAC_H, (mac[0]<<16) | (mac[1]<<8) | mac[2]);
		write_reg_word(FE_LAN_MAC_LMIN, val);
		write_reg_word(FE_LAN_MAC_LMAX, val);

		write_reg_word(FE_WAN_MAC_H, (mac[0]<<16) | (mac[1]<<8) | mac[2]);
		write_reg_word(FE_WAN_MAC_LMIN, val);
		if( (val+mac_range-1) > 0xffffff ){
			printk("setup_mac_reg: error mac range set!!!\n");
			write_reg_word(FE_WAN_MAC_LMAX, 0xffffff);
		}else{
			write_reg_word(FE_WAN_MAC_LMAX, val+mac_range-1);
		}
	}
	else
	{
		/***GDMA1**/
		write_reg_word(GDMA1_MAC_ADRL, (mac[2]<<24) | (mac[3]<<16) | (mac[4]<<8) | (mac[5]<<0));
		write_reg_word(GDMA1_MAC_ADRH, (0xf8<<16) | (mac[0]<<8) | (mac[1]<<0)); /* need to mask 3 bits for ethWan */

		/***GDMA2, default mask 3 bit**/
		write_reg_word(GDMA2_MAC_ADRL, (mac[2]<<24) | (mac[3]<<16) | (mac[4]<<8) | (mac[5]<<0));
		write_reg_word(GDMA2_MAC_ADRH, (0xf8<<16) | (mac[0]<<8) | (mac[1]<<0));
	}

}

void setup_pse_IQ_reservation(void)
{
    unsigned int reg=0;

    if(isEN7580) {
        reg = read_reg_word(PSE_FQFC_CFG_STA0);
        reg |= (0x1<<1);	// enable pse port1 flow control
        write_reg_word(PSE_FQFC_CFG_STA0, reg);
    } else {
        /* decrease PSE p2 and increase PSE p5 IQ reservation
         * in order to prevent from upstream packet drop at
         * GPON multi-tcom test */
        reg = read_reg_word(PSE_IQ_REV1);
        reg &= ~(0xff<<PSE_IQ_RES1_P2_OFFSET);
        reg |= (0x10<<PSE_IQ_RES1_P2_OFFSET);
        write_reg_word(PSE_IQ_REV1, reg);

        reg = read_reg_word(PSE_IQ_REV2);
        reg &= ~(0xff<<PSE_IQ_RES2_P5_OFFSET);
	    /* EN7528 use GDMP SRAM to support multi tcont */
	    if(isEN7528)
	        reg |= (0x80<<PSE_IQ_RES2_P5_OFFSET);
	    else
	        reg |= (0x40<<PSE_IQ_RES2_P5_OFFSET);
	    reg &= ~(0xff<<PSE_IQ_RES2_P4_OFFSET);
        reg |= (0x34<<PSE_IQ_RES2_P4_OFFSET);
	
        write_reg_word(PSE_IQ_REV2, reg);
    }

    return;
}

void fe_use_gdmp_sram(void)
{
    unsigned int reg=0, val = 0;
    //GDMP SRAM for fe using
    write_reg_word(SHARED_UNZIPMENT_SEL, 0x3);

    //set FQ_MAX_PCNT to 0xE0
    val = read_reg_word(PSE_FQFC_CFG);
    val &= ~PSE_FQFC_CFG_FQMAX_MASK;
    val |= (0xe0 << PSE_FQFC_CFG_FQMAX_OFFSET);
    write_reg_word(PSE_FQFC_CFG, val);

    //do pse reset to make configuration effective
    fe_do_core_reset_without_qdma();

    //set gdm2 tx buf enq, deq
    write_reg_word(GDMA2_TX_CHN_BUF, 0x3);
}

#if defined(TCSUPPORT_CPU_AN7552)
static void pse_cfg_set_oq_en(unsigned char pse_port, 
                        unsigned char queue_id, unsigned char enable)
{
    unsigned int val = 0;

    if(enable)
    {
        write_reg_word(PSE_QUEUE_CFG_VAL, 1);
    }
    else
    {
        write_reg_word(PSE_QUEUE_CFG_VAL, 0);
    }
    val  = ( (pse_port << PSE_CFG_PORT_ID_SHIFT)  & PSE_CFG_PORT_ID_MASK );
    val |= ( (queue_id << PSE_CFG_QUEUE_ID_SHIFT) & PSE_CFG_QUEUE_ID_MASK );
    val |= (PSE_CFG_WR_EN | PSE_CFG_OQEN_SEL);
    write_reg_word(PSE_QUEUE_CFG_WR, val);
    
    return ;
}

static unsigned char pse_cfg_get_oq_en(unsigned char pse_port, unsigned char queue_id)
{
    unsigned int val = 0;

    val  = ( (pse_port << PSE_CFG_PORT_ID_SHIFT)  & PSE_CFG_PORT_ID_MASK );
    val |= ( (queue_id << PSE_CFG_QUEUE_ID_SHIFT) & PSE_CFG_QUEUE_ID_MASK );
    write_reg_word(PSE_QUEUE_CFG_WR, val);
    
    return IO_GMASK(PSE_QUEUE_CFG_VAL, PSE_CFG_OQ_EN_MASK, PSE_CFG_OQ_EN_SHIFT);
}

static void disable_unuse_pse_oq( void )
{
    unsigned char oq_id = 0;
    
    /*disable p0 oq 4 used by fast path*/
    if(SUPPORT_CDM_HW_OQ_CONF)
    {
        pse_cfg_set_oq_en(0, 4, 0);
    }
    
    /*
        disable p1 oq5
        oq0 ~ oq4 used to switch
    */
    pse_cfg_set_oq_en(1, 5, 0);

#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
    /*disable p2 */
    for(oq_id=1; oq_id < PSE_PORT2_QUEUE_NUM; oq_id++)
    {
        pse_cfg_set_oq_en(2, oq_id, 0);
    }
#endif

    /*disable p5 oq 4 used by fast path*/
    if(SUPPORT_CDM_HW_OQ_CONF)
    {
        pse_cfg_set_oq_en(5, 4, 0);
    }
    
    /*disable p6 oq 0/1  not use npu bridge now*/
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
    pse_cfg_set_oq_en(6, 0, 0);
    pse_cfg_set_oq_en(6, 1, 0);
#endif

    /*disable p7 oq 0~9,  oq10-11 used for wifi*/
    for(oq_id=0; oq_id < 8; oq_id++)
    {
        pse_cfg_set_oq_en(7, oq_id, 0);
    }
#if defined(TCSUPPORT_ACTIVE_ETHERNET_WAN)
    pse_cfg_set_oq_en(7, 8, 0);
    pse_cfg_set_oq_en(7, 9, 0);
#endif

    return ;
}
#endif

void fe_reg_setup(void)
{
    unsigned int val = 0;
    if(isEN751627)
        cdm1SetRxqRedCfg(RATE_OR_THRL_DROP);

    if(isEN7528) {
        write_reg_word(FE_MISC_CFG, 0x2);
		
        //set oam/omci go to rx ring15
        val = read_reg_word(CDMA2_FWD_CFG);
        val &= ~PSE_OAM_QSEL_MASK;
        val |= (0xf << PSE_OAM_QSEL_OFFSET);
        write_reg_word(CDMA2_FWD_CFG, val);
    }
	
	setup_mac_reg();
    setup_pse_IQ_reservation();

	if(COPY_ENGINE_SUPPORT)
	{
		/* enable FE copy engine for MC/KA/DPI */
		write_reg_word(FE_PCE_CFG, 0x7);

		/* set VIP_QSEL to RxRing1 same as other IC */
		IO_SMASK(CDMA1_FWD_CFG, VIP_QSEL_MASK , VIP_QSEL_SHIFT , 4);
		IO_SMASK(CDMA2_FWD_CFG, VIP_QSEL_MASK , VIP_QSEL_SHIFT , 4);
	}

	if(GDM4_SUPPORT)
	{
		/* set GDM4 source interface offset to 8 */
		IO_SMASK(GDMA4_SRC_PORT_SET, SPORT_OFFSET0_MASK, SPORT_OFFSET0_SHIFT, 8);
		IO_SMASK(GDMA4_SRC_PORT_SET, SPORT_OFFSET1_MASK, SPORT_OFFSET1_SHIFT, 8);
		IO_SMASK(GDMA4_SRC_PORT_SET, SPORT_OFFSET2_MASK, SPORT_OFFSET2_SHIFT, 8);
	}

	if(RECONFIG_WOE_FC)
	{
		val = (((WOE_PORT<<5)|WOE_OQ2)<<16)|((WOE_PORT<<5)|WOE_OQ1);
		write_reg_word(QDMA1_FC_MAPPING3,val);
	}

	/*fix gdm3 src port to 0x16 for fttr*/
	if(SUPPORT_FTTR_ON_GDM3)
	{
		GDMA3DisableSTAG();
		write_reg_word(GDMA3_SRC_PORT_SET,0x33300);
	}
	
	if(SUPPORT_FE_OQ_CNT_ENHANCE)
	{
		IO_SBITS(FE_DMA_GLO_CFG, FE_RLS_CNT_TYPE_BIT);
		write_reg_word(PSE_BUFFER_TYPE, 0xffff);
	}

	if(SUPPORT_CDM3_MBI_ENHANCE)
	{
		write_reg_word(CDM3_TMBI_FRAG, 0xffffffff);
		write_reg_word(CDM3_RMBI_FRAG, 0xffffffff);
	}
	
    return;
}

#if defined(TCSUPPORT_SISM_HOST)
void fe_vip_tc_console_setup(uint8_t enable)
{
	if (enable)
	{
		unsigned int value=0;

		write_reg_word(FE_VIP_PATN(22),0xaaaa);/* 0xaaaa for en7516/en7517 tc_console */
		value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
		write_reg_word(FE_VIP_EN(22),value);
	}
	else
	{
		write_reg_word(FE_VIP_EN(22), 0);
	}

	return;
}
EXPORT_SYMBOL (fe_vip_tc_console_setup);
#endif /*defined(TCSUPPORT_SISM_HOST)*/

void fe_vip_setup(void)
{
	unsigned int value=0;
    
	/* 0~14: general config;   15~29:alloc config;  30~31: CWMP config*/
#if !defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_CPU_EN7523)   /*OSBNB00088112:JOYME3 Turnkey arp attack test case.*/
#if !defined(TCSUPPORT_CPU_EN7527) && !defined(TCSUPPORT_CPU_EN7516) && !defined(TCSUPPORT_CPU_EN7523)
    	write_reg_word(FE_VIP_PATN(0),0x01);/* IP ->ICMP(0x01) */
    	value = (0x01<<VIP_EN_CPU_OFFSET)|(0x02<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    	write_reg_word(FE_VIP_EN(0),value);
#endif

#if !defined(TCSUPPORT_CPU_EN7523)	/* 7523 move to IFC */
	write_reg_word(FE_VIP_PATN(1),0x0806);/* Ether ->ARP(0x0806) */
	value = (0x01<<VIP_EN_CPU_OFFSET)|(0x0<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
	write_reg_word(FE_VIP_EN(1),value);
#endif
#endif
#if !defined(TCSUPPORT_CPU_EN7523) && !defined(TCSUPPORT_CPU_EN7527) && !defined(TCSUPPORT_CPU_EN7516)	/* 7516/7527/7523 move to IFC */
	write_reg_word(FE_VIP_PATN(2),0x02);/* IP ->IGMP(0x02) */
	value = (0x01<<VIP_EN_CPU_OFFSET)|(0x02<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
	write_reg_word(FE_VIP_EN(2),value);
#endif
	write_reg_word(FE_VIP_PATN(3),0x8863);/* Ether ->PPP(0x8863) */
	value = (0x01<<VIP_EN_CPU_OFFSET)|(0x0<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
	write_reg_word(FE_VIP_EN(3),value);

	write_reg_word(FE_VIP_PATN(4),0xc021);/* PPP ->LCP(0xc021) */
	value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
	write_reg_word(FE_VIP_EN(4),value);

#if !defined(TCSUPPORT_CPU_EN7527) && !defined(TCSUPPORT_CPU_EN7516) && !defined(TCSUPPORT_CPU_EN7523)
	write_reg_word(FE_VIP_PATN(5),0x3a);/* IPv6 ->ICMPv6(0x3a) */
	value = (0x01<<VIP_EN_CPU_OFFSET)|(0x02<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
	write_reg_word(FE_VIP_EN(5),value);
#endif

    write_reg_word(FE_VIP_PATN(6),0x8021);/* PPP ->IPCP (0x8021) */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<< VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(6),value);

    write_reg_word(FE_VIP_PATN(7),0xc223);/* PPP ->CHAP (0xc223) */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<< VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(7),value);

    if (!isEN7528)
    {
        write_reg_word(FE_VIP_PATN(8),0x43);/* bootp  (0x43) */
        value = (0x01<<VIP_EN_CPU_OFFSET) | (0x01<< VIP_EN_SPEN_OFFSET) | (0x04<< VIP_EN_TYPE_OFFSET) | (0x01<<VIP_EN_ENABLE_OFFSET);
        write_reg_word(FE_VIP_EN(8),value);

        write_reg_word(FE_VIP_PATN(9),0x44);/* bootp (0x44) */
        value = (0x01<<VIP_EN_CPU_OFFSET) | (0x01<< VIP_EN_SPEN_OFFSET) | (0x04<< VIP_EN_TYPE_OFFSET) | (0x01<<VIP_EN_ENABLE_OFFSET);
        write_reg_word(FE_VIP_EN(9),value);
    }

    write_reg_word(FE_VIP_PATN(10),0x1f401f4);/* ISAKMP (udpSrcPort:500, udpDstPort:500) */
    value = (0x01<<VIP_EN_CPU_OFFSET) | (0x01<< VIP_EN_DPEN_OFFSET) | (0x01<< VIP_EN_SPEN_OFFSET) | (0x04<< VIP_EN_TYPE_OFFSET) | (0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(10),value);

    write_reg_word(FE_VIP_PATN(11),0xc057);/* PPP ->IPv6CP (0xc057) */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(11),value);

    write_reg_word(FE_VIP_PATN(12),0x2220223);/* DHCPv6 (udpSrcPort:546, udpDstPort:547) (udpSrcPort:547, udpDstPort:546) */
    value = (0x01<<VIP_EN_CPU_OFFSET) | (0x01<<VIP_EN_DPEN_OFFSET) 
        | (0x01<<VIP_EN_SPEN_OFFSET) | (0x04<<VIP_EN_TYPE_OFFSET) | (0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(12),value);
    
	#if !defined(TCSUPPORT_CPU_EN7523)
    write_reg_word(FE_VIP_PATN(13), 5060<<VIP_PATN_SP_OFFSET);/* VoIP SIP (SPORT = 5060) */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_SPEN_OFFSET)|(0x04<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(13), value);
    
    write_reg_word(FE_VIP_PATN(14), 5060<<VIP_PATN_DP_OFFSET);/* VoIP SIP (DPORT = 5060) */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_DPEN_OFFSET)|(0x04<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(14), value);

    write_reg_word(FE_VIP_PATN(15), 20<<VIP_PATN_SP_OFFSET);/* FTP TCP (SPORT = 20) */
    value = (0x01<<VIP_EN_SPEN_OFFSET)|(0x03<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(15), value);

    write_reg_word(FE_VIP_PATN(16), 21<<VIP_PATN_SP_OFFSET);/* FTP TCP (SPORT = 21) */
    value = (0x01<<VIP_EN_SPEN_OFFSET)|(0x03<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(16), value);
	#endif

	write_reg_word(FE_VIP_PATN(17), 6880<<VIP_PATN_SP_OFFSET);/*intelligent plateform (SPORT = 6880) */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_SPEN_OFFSET)|(0x03<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(17), value);
    
    write_reg_word(FE_VIP_PATN(18), 6880<<VIP_PATN_DP_OFFSET);/*intelligent plateform  (DPORT = 6880) */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_DPEN_OFFSET)|(0x03<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(18), value);

    write_reg_word(FE_VIP_PATN(19),0xc023);/* PPP ->PAP(0xc023), to avoid the issue of OSBNB00109234 */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(19),value);

    write_reg_word(FE_VIP_PATN(20),0x893a);/* Ether ->ETH_P_1905 (0x893a) */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x0<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(20),value);

    write_reg_word(FE_VIP_PATN(21),0x88cc);/* Ether ->ETH_P_LLDP (0x88cc) */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x0<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(21),value);

#if !defined(TCSUPPORT_SISM_HOST)
    write_reg_word(FE_VIP_PATN(22),0xaaaa);/* 0xaaaa for en7516/en7517 tc_console */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(22),value);
#endif /*!defined(TCSUPPORT_SISM_HOST)*/

    write_reg_word(FE_VIP_PATN(23),0xbeea);/* 0xbeea for en7516/en7517 */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(23),value);

    write_reg_word(FE_VIP_PATN(24),0xbeeb);/* 0xbeeb for en7516/en7517 */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(24),value);

    write_reg_word(FE_VIP_PATN(25),0xbeec);/* 0xbeec for en7516/en7517 */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(25),value);

    write_reg_word(FE_VIP_PATN(26),0xbeed);/* 0xbeed for en7516/en7517 */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(26),value);

    write_reg_word(FE_VIP_PATN(27),0xbeee);/* 0xbeee for en7516/en7517 */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(27),value);

    write_reg_word(FE_VIP_PATN(28),0xbeef);/* 0xbeef for en7516/en7517 */
    value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
    write_reg_word(FE_VIP_EN(28),value);

    return ;

}

static irqreturn_t fe_isr(int irq, void *dev_id)
{
    unsigned int i=0;
    unsigned int intStatus=0 ;
    
    intStatus = FeGetIntStatus() & FeGetIntMask() ;
    
    if(dbgLevel >= 1) {
        printk("FE_ISR Come In: intStatus:%08x\n", intStatus) ;
    }
    if(intStatus & INT_STATUS_PSE_FC) {
        if(dbgLevel >= 2) {
            for(i=0; i<6; i++) {
                if(intStatus & (1<<i))
                    printk("Interrupt: PSE_P%d_FC alert\n", i) ;
            }
        }
        FeSetIntStatus(INT_STATUS_PSE_FC) ;
    }
    
    if(intStatus & INT_STATUS_TSO) {
        printk("Interrupt(%08x): TSO Interrupt alert\n", intStatus) ;
        FeSetIntStatus(INT_STATUS_TSO) ;
    }
    
    if(intStatus & INT_STATUS_PSE_FQ_EMPTY) {
        printk("Interrupt(%08x): PSE_FQ_EMPTY alert\n", intStatus) ;
        FeSetIntStatus(INT_STATUS_PSE_FQ_EMPTY) ;
    }
    if(intStatus & INT_STATUS_PSE_FC_DROP) {
        printk("Interrupt(%08x): PSE_FC_DROP alert\n", intStatus) ;
        FeSetIntStatus(INT_STATUS_PSE_FC_DROP) ;
    }
    
    if(intStatus & INT_GDM2_RX_2_GEMPORT_ISSUE) {
        //printk("Interrupt(%08x): INT_GDM2_RX_2_GEMPORT_ISSUE alert\n", intStatus) ;
        //dump_stack();
        FeSetIntStatus(INT_GDM2_RX_2_GEMPORT_ISSUE) ;
#ifdef TCSUPPORT_CPU_EN7580
        if(unlockLevel == 1) {
            mbi_hang_unlock_by_aging(FE_GDM_SEL_GDMA2, 3);
        } else if(unlockLevel == 2) {
            mbi_hang_unlock_by_terminate(FE_GDM_SEL_GDMA2);
        }
#endif
    }
    if(intStatus & INT_GDM2_RX_FIFO_OVERFLOW) {
        printk("Interrupt(%08x): INT_GDM2_RX_FIFO_OVERFLOW alert\n", intStatus) ;
        dump_stack();
        FeSetIntStatus(INT_GDM2_RX_FIFO_OVERFLOW) ;
    }
    if(intStatus & INT_GDM2_RX_MBI_BY_AUTOAGING) {
        //printk("Interrupt(%08x): INT_GDM2_RX_MBI_BY_AUTOAGING alert\n", intStatus) ;
        FeSetIntStatus(INT_GDM2_RX_MBI_BY_AUTOAGING) ;
    }
    if(intStatus & INT_GDM2_RX_MBI_BY_CSR) {
        printk("Interrupt(%08x): INT_GDM2_RX_MBI_BY_CSR alert\n", intStatus) ;
        FeSetIntStatus(INT_GDM2_RX_MBI_BY_CSR) ;
    }
    if(intStatus & INT_GDM2_TX_MBI_BY_AUTOAGING) {
        //printk("Interrupt(%08x): INT_GDM2_TX_MBI_BY_AUTOAGING alert\n", intStatus) ;
        FeSetIntStatus(INT_GDM2_TX_MBI_BY_AUTOAGING) ;
    }
    if(intStatus & INT_GDM2_TX_MBI_BY_CSR) {
        printk("Interrupt(%08x): INT_GDM2_TX_MBI_BY_CSR alert\n", intStatus) ;
        FeSetIntStatus(INT_GDM2_TX_MBI_BY_CSR) ;
    }
    if(intStatus & INT_GDM2_TX_MBI_BY_MAC) {
        printk("Interrupt(%08x): INT_GDM2_TX_MBI_BY_MAC alert\n", intStatus) ;
        FeSetIntStatus(INT_GDM2_TX_MBI_BY_MAC) ;
    }
    if(SUPPORT_DETECT_PSE_EXCEPTION){
        if(intStatus & INT_STATUS_PSE_EXCEPTION) {
            printk("Interrupt(%08x): INT_STATUS_PSE_EXCEPTION alert\n", intStatus) ;
            //do action
            fe_pse_core_soft_reset_cnt++;
            fe_core_soft_reset();
            FeSetIntStatus(INT_STATUS_PSE_EXCEPTION) ;
        }
    }
	return IRQ_HANDLED ;
}

static int fe_tls_enable_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len = 0;
	len = sprintf(page, "%d\n", tls_enable);
	
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

static int fe_tls_enable_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[32];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	if (sscanf(val_string, "%d", &tls_enable) != 1) {
		printk("usage: <tls_enable>\n");
	}
	
	return count;
}

static int fe_tls_debug_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len = 0;
	len = sprintf(page, "%d\n", tls_debug_on);
	
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

static int fe_tls_debug_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[32];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	if (sscanf(val_string, "%d", &tls_debug_on) != 1) {
		printk("usage: <tls_debug_on>\n");
	}
	
	return count;
}

void fe_check_counter_overflow(void)
{
	unsigned int gdm2_rx_len_low = read_reg_word(GDMA2_RX_ETHERPLEN);
	unsigned int gdm2_tx_len_low = read_reg_word(GDMA2_TX_ETHLENCNT);

	unsigned int gdm2_rx_drop_low = read_reg_word(GDMA2_RX_ETHDROPCNT);
	unsigned int gdm2_tx_drop_low = read_reg_word(GDMA2_TX_ETHDROPCNT);

	if(gdm2_rx_len_low < gdm2_rx_len_low_last)
		gdm2_rx_len_high++;

	if(gdm2_tx_len_low < gdm2_tx_len_low_last)
		gdm2_tx_len_high++;

	if(gdm2_rx_drop_low < gdm2_rx_drop_low_last)
		gdm2_rx_drop_high++;

	if(gdm2_tx_drop_low < gdm2_tx_drop_low_last)
		gdm2_tx_drop_high++;

	gdm2_rx_len_low_last = gdm2_rx_len_low;	
	gdm2_tx_len_low_last = gdm2_tx_len_low;
  
	gdm2_rx_drop_low_last = gdm2_rx_drop_low;	
	gdm2_tx_drop_low_last = gdm2_tx_drop_low;
}

#if defined(TCSUPPORT_CPU_EN7521) || defined(TCSUPPORT_CPU_EN7512)
void fe_dyn_set_qdma_wan_txq_cngst_totalthr(void)
{
	QDMA_TxQDynCngstTotalThrh_T total_threshold;
    
	unsigned int gdm2_tx_eth_1024_to_1518_cnt = read_reg_word(GDMA2_TX_ETH_1024_TO_1518_CNT);
	unsigned int gdm2_tx_eth_more_1518_cnt = read_reg_word(GDMA2_TX_ETH_MORE1518_CNT);
	unsigned int gdm2_tx_eth_bytes_cnt = read_reg_word(dyn_qdma_txq_cngst_packet_len_reg);
	unsigned int gdm2_tx_eth_len_cnt = read_reg_word(GDMA2_TX_ETHLENCNT);

    unsigned int gdm2_tx_eth_1024_to_1518_cnt_per_sec = 0;
    unsigned int gdm2_tx_eth_more_1518_cnt_per_sec = 0;
    unsigned int gdm2_tx_eth_bytes_cnt_per_sec = 0;
    unsigned int gdm2_tx_eth_len_cnt_per_sec = 0;
	
    if(0 == fe_dyn_qdma_wan_txq_cngst_enable)
        return;
	
	if(!isEN751221)
		return;
    
    if(gdm2_tx_eth_1024_to_1518_cnt >= gdm2_tx_eth_1024_to_1518_cnt_last){
        gdm2_tx_eth_1024_to_1518_cnt_per_sec = (gdm2_tx_eth_1024_to_1518_cnt - gdm2_tx_eth_1024_to_1518_cnt_last);
    }
    else{
        gdm2_tx_eth_1024_to_1518_cnt_per_sec = (((unsigned int) -1) - gdm2_tx_eth_1024_to_1518_cnt_last + gdm2_tx_eth_1024_to_1518_cnt);
    }
    
    if(gdm2_tx_eth_more_1518_cnt >= gdm2_tx_eth_more_1518_cnt_last){
        gdm2_tx_eth_more_1518_cnt_per_sec = (gdm2_tx_eth_more_1518_cnt - gdm2_tx_eth_more_1518_cnt_last);
    }
    else{
        gdm2_tx_eth_more_1518_cnt_per_sec = (((unsigned int) -1) - gdm2_tx_eth_more_1518_cnt_last + gdm2_tx_eth_more_1518_cnt);
    }
    
    if(gdm2_tx_eth_bytes_cnt >= gdm2_tx_eth_bytes_cnt_last){
        gdm2_tx_eth_bytes_cnt_per_sec = (gdm2_tx_eth_bytes_cnt - gdm2_tx_eth_bytes_cnt_last);
    }
    else{
        gdm2_tx_eth_bytes_cnt_per_sec = (((unsigned int) -1) - gdm2_tx_eth_bytes_cnt_last + gdm2_tx_eth_bytes_cnt);
    }
    
    if(gdm2_tx_eth_len_cnt >= gdm2_tx_eth_len_cnt_last){
        gdm2_tx_eth_len_cnt_per_sec = (gdm2_tx_eth_len_cnt - gdm2_tx_eth_len_cnt_last);
    }
    else{
        gdm2_tx_eth_len_cnt_per_sec = (((unsigned int) -1) - gdm2_tx_eth_len_cnt_last + gdm2_tx_eth_len_cnt);
    }
    
    gdm2_tx_eth_1024_to_1518_cnt_last = gdm2_tx_eth_1024_to_1518_cnt;
    gdm2_tx_eth_more_1518_cnt_last = gdm2_tx_eth_more_1518_cnt;
    gdm2_tx_eth_bytes_cnt_last = gdm2_tx_eth_bytes_cnt;
    gdm2_tx_eth_len_cnt_last = gdm2_tx_eth_len_cnt;

    if(dyn_qdma_txq_cngst_en == 2)
        return ;

    memset(&total_threshold, 0, sizeof(total_threshold));
	QDMA_API_GET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_WAN, &total_threshold);
    
    if(dyn_qdma_txq_cngst_en == 0) {
        //gdm2_tx_eth_more_1518_cnt_per_sec*1518*8bit/1s >=750Mbps, gdm2_tx_eth_len_cnt_per_sec*8bit/1s >= 750Mbps
        if(gdm2_tx_eth_more_1518_cnt_per_sec >= 61758 && gdm2_tx_eth_len_cnt_per_sec >= 93750000){
            if(total_threshold.dynCngstTotalMinThrh != 0x5C0){
                printk("%s [%d]: qdma_wan dynCngstTotalMinThrh set to 0x1700.\n", strrchr(__FILE__, '/')+1, __LINE__);
                total_threshold.dynCngstTotalMinThrh = 0x5C0;
                QDMA_API_SET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_WAN, &total_threshold);
            }
            fe_dyna_set_count = 0;
        }
        //gdm2_tx_eth_1024_to_1518_cnt_per_sec*1518*8bit/1s >=200Mbps, gdm2_tx_eth_len_cnt_per_sec*8bit/1s >= 200Mbps
        else  if(gdm2_tx_eth_1024_to_1518_cnt_per_sec >= 16469 && gdm2_tx_eth_len_cnt_per_sec >= 25000000){
            if(total_threshold.dynCngstTotalMinThrh != 0xE00){
                printk("%s [%d]: qdma_wan dynCngstTotalMinThrh set to 0x3800.\n", strrchr(__FILE__, '/')+1, __LINE__);
                total_threshold.dynCngstTotalMinThrh = 0xE00;
                QDMA_API_SET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_WAN, &total_threshold);
            }
            fe_dyna_set_count = 0;
        }
    } else {
        //gdm2_tx_eth_bytes_cnt_per_sec*1518*8bit/1s >=[stream_rate]Mbps, gdm2_tx_eth_len_cnt_per_sec*8bit/1s >= [stream_rate]Mbps
        if(gdm2_tx_eth_bytes_cnt_per_sec >= (dyn_qdma_txq_cngst_stream_rate*1000*1000/8/dyn_qdma_txq_cngst_packet_len) 
            &&  gdm2_tx_eth_len_cnt_per_sec >= (dyn_qdma_txq_cngst_stream_rate*1000*1000/8)) {
            if(total_threshold.dynCngstTotalMinThrh != (dyn_qdma_txq_cngst_total_min>>2)){
                printk("%s [%d]: qdma_wan dynCngstTotalMinThrh set to 0x%x.\n", strrchr(__FILE__, '/')+1, __LINE__, dyn_qdma_txq_cngst_total_min);
                total_threshold.dynCngstTotalMinThrh = (dyn_qdma_txq_cngst_total_min>>2);
                QDMA_API_SET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_WAN, &total_threshold);
            }
            fe_dyna_set_count = 0;
        }
    }
    if(fe_dyna_set_count == 300){
        if(total_threshold.dynCngstTotalMinThrh != 0xCD){
            printk("%s [%d]: qdma_wan dynCngstTotalMinThrh set to default.\n", strrchr(__FILE__, '/')+1, __LINE__);
            total_threshold.dynCngstTotalMinThrh = 0xCD;
            QDMA_API_SET_TXQ_CNGST_TOTAL_THRESHOLD(ECNT_QDMA_WAN, &total_threshold);
        }
        fe_dyna_set_count = 0;
    }      
    fe_dyna_set_count ++;
}
#endif
void fe_timer_expires(TIMER_FUN_PAAM arg)
{
    tasklet_schedule(&dlfDetectTask) ;
	fe_check_counter_overflow();
#if defined(TCSUPPORT_CPU_EN7521) || defined(TCSUPPORT_CPU_EN7512)
	if(isEN751221)
		fe_dyn_set_qdma_wan_txq_cngst_totalthr();
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    fe_timer.data = fe_expire_time ; /* 1 s */
#else
	fe_timer.expires = fe_expire_time ; /* 1 s */
#endif
    FE_START_TIMER(fe_timer);

    return ;
}

static void fe_detect_task(unsigned long data)
{
    unsigned int value=0, interval_cnt=0;
    static unsigned int pre_gdm2_rx_cnt=0;
    static unsigned int cur_gdm2_rx_cnt=0;

    pre_gdm2_rx_cnt = cur_gdm2_rx_cnt;
    cur_gdm2_rx_cnt = read_reg_word(GDMA2_RX_OKCNT);

    if(cur_gdm2_rx_cnt >= pre_gdm2_rx_cnt) {
        interval_cnt = cur_gdm2_rx_cnt - pre_gdm2_rx_cnt;
    } else {
        interval_cnt = (uint)(-1) - pre_gdm2_rx_cnt + cur_gdm2_rx_cnt;
    }

    if(!cwmp_vip_enable)
        return;
    
    /* unprotect -> protect*/
    if(atomic_read(&protect_flag) == 0) {
        if(interval_cnt > FE_TASK_QUOTA) {
            /* open protect*/
            write_reg_word(FE_VIP_PATN(30), cwmp_request_dport<<VIP_PATN_DP_OFFSET);/* CWMP Manager (DPORT = 8099) */
            value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_DPEN_OFFSET)|(0x03<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
            write_reg_word(FE_VIP_EN(30), value);
            write_reg_word(FE_VIP_PATN(31), cwmp_connect_sport<<VIP_PATN_SP_OFFSET);/* CWMP Port (SPORT = 9090) */
            value = (0x01<<VIP_EN_CPU_OFFSET)|(0x01<<VIP_EN_SPEN_OFFSET)|(0x03<<VIP_EN_TYPE_OFFSET)|(0x01<<VIP_EN_ENABLE_OFFSET);
            write_reg_word(FE_VIP_EN(31), value);
            atomic_set(&protect_flag, 1);
#ifdef TCSUPPORT_FE_VIP_ENHANCEMENT
            QDMA_API_READ_VIP_INFO(ECNT_QDMA_WAN);
#endif
        }
    /* protect -> unprotect*/
    } else {
        if(interval_cnt < FE_TASK_QUOTA) {
            /* close protect*/
            write_reg_word(FE_VIP_PATN(30), 0);
            write_reg_word(FE_VIP_EN(30), 0);
            atomic_set(&protect_flag, 0);
#ifdef TCSUPPORT_FE_VIP_ENHANCEMENT
            QDMA_API_READ_VIP_INFO(ECNT_QDMA_WAN);
#endif
        }
    }

    return ;
}

static void fe_reset_all(void)
{
    uint32 val = 0, i;
    uint32 *fe_reg = NULL;
    uint32 addr, reg_len = (((0xbfb5169c - 0xbfb50000) >> 2) + 1);

    fe_reg = (uint32 *)kmalloc((reg_len << 2), GFP_KERNEL);
    if(fe_reg == NULL)
    {
        printk("Error: not enough memory.");
        return;
    }

    /* Clean PPE table */      
#ifdef TCSUPPORT_RA_HWNAT_ENHANCE_HOOK
    if(ra_sw_nat_hook_clean_table) 
    {
        ra_sw_nat_hook_clean_table();       
    }
#endif

    /* PPE disable */       
    val = read_reg_word(0xbfb50e00);    
    val &= ~(0x01);     
    write_reg_word(0xbfb50e00, val); 
    
    /* backup registers */
    for(i = 0; i < reg_len; i++) 
    {
        /* skip qdma lan */
        addr = 0xbfb50000 + (i << 2);
        if ((addr >= 0xbfb50800) && (addr <= 0xbfb50b1c))
        {
            continue;
        }
        fe_reg[i] = read_reg_word(0xbfb50000 + (i << 2));
    }

    /* reset fe */
    val = read_reg_word(0xbfb00834) ;
    val |= (1 << 21);
    write_reg_word(0xbfb00834, val);
    val &= ~(1 << 21);
    write_reg_word(0xbfb00834, val);

    /* recovery registers */
    for(i = 0; i < reg_len; i++) 
    {
        /* skip qdma lan */
        addr = 0xbfb50000 + (i << 2);
        if ((addr >= 0xbfb50800) && (addr <= 0xbfb50b1c))
        {
            continue;
        }
        write_reg_word(0xbfb50000 + (i << 2), fe_reg[i]);
    }
    
    /* PPE enable */        
    val = read_reg_word(0xbfb50e00);     
    val |= 0x01;     
    write_reg_word(0xbfb50e00, val); 

    kfree(fe_reg);
}

static void fe_pse_reset(unsigned long data)
{
    uint8 gpon_flag = 1;
    uint32 val = 0, flags=0;
    uint32 gdm1_txchn_en = 0, gdm1_rxchn_en = 0, gdm1_hwfchn_en = 0;
    uint32 gdm2_txchn_en = 0, gdm2_rxchn_en = 0, gdm2_hwfchn_en = 0;
    uint32 qdma_wan_free_dscp = 0, qdma_wan_free_dscp1 = 0;
    uint32 gdma2_tx_chn_vld = 0, gdma2_tx_chn_vld1 = 0;
    uint32 qdma_lan_free_dscp = 0, qdma_lan_free_dscp1 = 0;
    uint32 qdma_lan_buf_usage = 0;

    atomic_set(&fe_pse_reset_flag, 1);
    spin_lock_irqsave(&fe_pse_reset_lock, flags);

    #ifdef TCSUPPORT_CPU_ARMV8
    if ((GET_WAN_CONF() & 0x7) == 0)
    {
        gpon_flag = 1;
    }
    else if ((GET_WAN_CONF() & 0x7) == 1)
    {
        gpon_flag = 0;
    }
    #else
    if ((read_reg_word(0xbfb00070) & 0x7) == 0)
    {
        gpon_flag = 1;
    }
    else if ((read_reg_word(0xbfb00070) & 0x7) == 1)
    {
        gpon_flag = 0;
    }
    #endif
    else
    {
        spin_unlock_irqrestore(&fe_pse_reset_lock, flags);
        atomic_set(&fe_pse_reset_flag, 0);
        return;
    }

    //printk("Warning: PSE resetting!\n");

    gdm2_hwfchn_en = read_reg_word(0xbfb5140c);
    gdm2_txchn_en  = read_reg_word(0xbfb51524);
    gdm2_rxchn_en  = read_reg_word(0xbfb51528);

    gdm1_hwfchn_en = read_reg_word(0xbfb5040c);
    gdm1_txchn_en  = read_reg_word(0xbfb50524);
    gdm1_rxchn_en  = read_reg_word(0xbfb50528);

    /* GSW port6 force link down */
    val = read_reg_word(GSW_PMCR(6));
    val &= ~(0x01);
    write_reg_word(GSW_PMCR(6), val);

    /* stop PON MAC rx */
    if (gpon_flag)
    {
        val = read_reg_word(0xbfb64160);
        val |= 0x00000001;              //rx:bit0, tx:bit8
        write_reg_word(0xbfb64160, val); 
    }
    else
    {
        val = read_reg_word(0xbfb66000);
        val |= 0x00000200;              //rx:bit9, tx:bit8
        write_reg_word(0xbfb66000, val); 
    }

    /* drop pkts from GDMA1 & GDMA2 */
    val = read_reg_word(GDMA1_FWD_CFG);
    val &= ~(0xFFFF);
    val |= 0x7777;
    write_reg_word(GDMA1_FWD_CFG, val);

    val = read_reg_word(GDMA2_FWD_CFG);
    val &= ~(0xFFFF);
    val |= 0x7777;
    write_reg_word(GDMA2_FWD_CFG, val);

    /* disable CDM2/CMD1_HWF_CHN_EN */
    write_reg_word(0xbfb5140c, 0); 
    write_reg_word(0xbfb5040c, 0); 
    
    /* disable GDM_RXCHN_EN */
    write_reg_word(0xbfb51528, 0); 
    write_reg_word(0xbfb50528, 0); 

    /*Wait for the descriptor free up, then start to reset PSE*/
    mdelay(600);

    /* disable QDMA LAN/WAN TX/RX_DMA */
    val = read_reg_word(0xbfb54004);
    val &= ~(0x04 | 0x01);
    write_reg_word(0xbfb54004, val);
    
    val = read_reg_word(0xbfb55004);
    val &= ~(0x04 | 0x01);
    write_reg_word(0xbfb55004, val);

    /* stop PON MAC tx */
    if (gpon_flag)
    {
        val = read_reg_word(0xbfb64160);
        val |= 0x00000100;              //rx:bit0, tx:bit8
        write_reg_word(0xbfb64160, val); 
    }
    else
    {
        val = read_reg_word(0xbfb66000);
        val |= 0x00000100;              //rx:bit9, tx:bit8
        write_reg_word(0xbfb66000, val); 
    }
    
    /* disable GDM_TXCHN_EN */
    write_reg_word(0xbfb51524, 0); 
    write_reg_word(0xbfb50524, 0); 

    /* reset PSE */
    mdelay(200);
    
    val = read_reg_word(FE_RST_GLO);
    val |= 1;
    write_reg_word(FE_RST_GLO,val);
    g_fe_iq_reset_cnt++;

    /* need to reset qdma wan in case of dscp leaking */
    val = read_reg_word(0xbfb550f0);
    qdma_wan_free_dscp = val & 0xFFFF;
    //if ((qdma_wan_free_dscp != 0x1000) || g_fe_pse_reset_test)
    {
        QDMA_API_RESET(ECNT_QDMA_WAN);
        g_fe_qdma_reset_cnt++;
        g_fe_last_qdma_dscp = qdma_wan_free_dscp;
    }

    val = read_reg_word(0xbfb540f0);
    qdma_lan_free_dscp = val & 0xFFFF;
    //if ((qdma_lan_free_dscp != 0x400) || g_fe_pse_reset_test)
    {
        QDMA_API_RESET(ECNT_QDMA_LAN);
        g_fe_qdma_lan_reset_cnt++;
        g_fe_last_qdma_lan_dscp = qdma_lan_free_dscp;
    }

    /* need to reset whole FE in case halt packet jammed in GDM2 */
    gdma2_tx_chn_vld = read_reg_word(0xbfb51570);
    //if (gdma2_tx_chn_vld || g_fe_pse_reset_test)
    {
        fe_reset_all();
        g_fe_all_reset_cnt++;
        g_fe_last_gdma_chh_vld = gdma2_tx_chn_vld;
    }

    mdelay(100);

    qdma_wan_free_dscp1 = read_reg_word(0xbfb550f0);
    qdma_lan_free_dscp1 = read_reg_word(0xbfb540f0);
    gdma2_tx_chn_vld1 = read_reg_word(0xbfb51570);
    
    /* enable GDM_TXCHN_EN */
    write_reg_word(0xbfb51524, gdm2_txchn_en); 
    write_reg_word(0xbfb50524, gdm1_txchn_en); 

    /* start PON MAC tx */
    if (gpon_flag)
    {
        val = read_reg_word(0xbfb64160);
        val &= ~(0x00000100);       //rx:bit0, tx:bit8
        write_reg_word(0xbfb64160, val); 
    }
    else
    {
        val = read_reg_word(0xbfb66000);
        val &= ~0x00000100;             //rx:bit9, tx:bit8
        write_reg_word(0xbfb66000, val); 
    }

    /* enable QDMA LAN/WAN TX/RX_DMA  */
    val = read_reg_word(0xbfb54004);
    val |= (0x04 | 0x01);
    write_reg_word(0xbfb54004, val);
    
    val = read_reg_word(0xbfb55004);
    val |= (0x04 | 0x01);
    write_reg_word(0xbfb55004, val);

    /* enable GDM_RXCHN_EN */
    write_reg_word(0xbfb51528, gdm2_rxchn_en); 
    write_reg_word(0xbfb50528, gdm1_rxchn_en); 

    /* enable CDM2_HWF_CHN_EN */
    write_reg_word(0xbfb5140c, gdm2_hwfchn_en); 
    write_reg_word(0xbfb5040c, gdm1_hwfchn_en); 

    /* recover GDMA1 & GDMA2 cfg */
    val = read_reg_word(GDMA1_FWD_CFG);
    val &= ~(0xFFFF);
    val |= 0x4444;
    write_reg_word(GDMA1_FWD_CFG, val);

    val = read_reg_word(GDMA2_FWD_CFG);
    val &= ~(0xFFFF);
    val |= 0x4444;
    write_reg_word(GDMA2_FWD_CFG, val);
    
    /* start PON MAC rx */
    if (gpon_flag)
    {
        val = read_reg_word(0xbfb64160);
        val &= ~(0x00000001);       //rx:bit0, tx:bit8
        write_reg_word(0xbfb64160, val); 
    }
    else
    {
        val = read_reg_word(0xbfb66000);
        val &= ~0x00000200;             //rx:bit9, tx:bit8
        write_reg_word(0xbfb66000, val); 
    }

    /* GSW port6 force link up */
    val = read_reg_word(GSW_PMCR(6));
    val |= 0x01;
    write_reg_word(GSW_PMCR(6), val);

    printk("Info: Datapath recovery finished(Debug info: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x).\n", 
        qdma_wan_free_dscp, qdma_wan_free_dscp1, gdma2_tx_chn_vld, gdma2_tx_chn_vld1, qdma_lan_free_dscp, qdma_lan_free_dscp1);

    spin_unlock_irqrestore(&fe_pse_reset_lock, flags);
    atomic_set(&fe_pse_reset_flag, 0);
}


static void fe_gdm2_iq_stat_monitor(void)
{
    uint32 pse_iq_cnt = 0;
	uint32 tmp = 0;
    if (atomic_read(&fe_pse_reset_flag))
    {
        return;
    }
	
    /* Monitor port2  */
    pse_iq_cnt = ((read_reg_word(PSE_PORT_Q_USE_STA2)>>16) & 0x7fff);

    /* only  monitor IQ_port2 >= threshold*/
	if(pse_iq_cnt < gdma2_iq_threshold){
		return;
	}
	
    /* If issue occurs,gdm2 rx will be blocked. */
	tmp = read_reg_word(GDMA2_RX_OKCNT);
	if(tmp != gdm2_before_rx_ok){
		gdm2_before_rx_ok = tmp;
		return;
	}
	
	/* Monitor GDMA2_RX_OVDROPCNT */
	tmp = read_reg_word(GDMA2_RX_OVDROPCNT);
	
	if(tmp > gdm2_before_rx_over_drop_cnt){
		is_increasing_flag ++ ;
	}else {
		is_increasing_flag = 0;
	}
	gdm2_before_rx_over_drop_cnt = tmp;

	
	if (is_increasing_flag > gdma2_drop_time )
    {
        gdma2_iq_abnormal_cnt++;
        is_increasing_flag = 0;
        tasklet_schedule(&fe_pse_reset_task);
    }
}

static void fe_iq_stat_monitor(void)
{
    uint32 pse_iq_cnt = 0;
    uint32 pse_iq_stat1 = 0, pse_iq_stat2 = 0;
    uint32 pse_oq_stat1 = 0, pse_oq_stat2 = 0;

    if (atomic_read(&fe_pse_reset_flag))
    {
        return;
    }
    
    pse_iq_stat1 = read_reg_word(0xbfb50110);
    pse_iq_stat2 = read_reg_word(0xbfb50114);
    pse_oq_stat1 = read_reg_word(0xbfb50118);
    pse_oq_stat2 = read_reg_word(0xbfb5011c);
    pse_iq_cnt = (pse_iq_stat1 & 0xff) + ((pse_iq_stat1 >> 8) & 0xff) + ((pse_iq_stat1 >> 16) & 0xff) 
        + ((pse_iq_stat1 >> 24) & 0xff) + (pse_iq_stat2 & 0xff) + ((pse_iq_stat2 >> 8) & 0xff) + ((pse_iq_stat2 >> 16) & 0xff);

#if defined(TCSUPPORT_CPU_EN7528)
	if (((pse_iq_cnt > 0xE0) || g_fe_iq_reset_test_flag) && (g_fe_iq_abnormal_threshold > 0))
#else
	if (((pse_iq_cnt > 0xA0) || g_fe_iq_reset_test_flag) && (g_fe_iq_abnormal_threshold > 0))
#endif
    {
        g_fe_iq_abnormal_cnt++;
        printk("Warning: Datapath resource exceeds threshold, start recovering, IQ_STA1 0x%x, IQ_STA2 0x%x, OQ_STA1 0x%x, OQ_STA2 0x%x\n", pse_iq_stat1, pse_iq_stat2, pse_oq_stat1, pse_oq_stat2);
        
        if (g_fe_iq_abnormal_cnt >= g_fe_iq_abnormal_threshold)
        {
            g_fe_iq_abnormal_cnt = 0;
            g_fe_iq_reset_test_flag = 0;
            g_fe_last_iq_sta1 = pse_iq_stat1;
            g_fe_last_iq_sta2 = pse_iq_stat2;
            tasklet_schedule(&fe_pse_reset_task);
        }
    }
    else
    {
        g_fe_iq_abnormal_cnt = 0;
    }
}

static void fe_monitor(unsigned long data)
{

	if (SUPPORT_GDM2_IQ_MONITOR)
		fe_gdm2_iq_stat_monitor();
	else
    	fe_iq_stat_monitor();
    
    /* Schedule for the next time */
    fe_monitor_timer.expires = jiffies + msecs_to_jiffies(1000);
    add_timer(&fe_monitor_timer);
}

static int fe_timer_init(void)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    /* Schedule timer for monitoring link status */
    init_timer(&fe_monitor_timer);
    fe_monitor_timer.expires = jiffies + msecs_to_jiffies(1000);
    fe_monitor_timer.function = fe_monitor;
    fe_monitor_timer.data = 0;
#else
	timer_setup(&fe_monitor_timer,fe_monitor,0);
	fe_monitor_timer.expires = jiffies + msecs_to_jiffies(1000);
#endif
    add_timer(&fe_monitor_timer);

    return 0;
}

static int fe_timer_deinit(void)
{
    del_timer_sync(&fe_monitor_timer);

    return 0;
}

void fe_cnt_timer_expires(TIMER_FUN_PAAM arg)
{
    int index = 0;
    mt7530_switch_api_MibCntType MibCntType;
    unsigned int cnt = 0;
	unsigned int cnt_discard_rx = 0, cnt_discard_tx = 0;
	unsigned int cnt_error_rx = 0, cnt_error_tx = 0;
    int pre_idx = fe_cnt_cur_idx;
    fe_cnt_cur_idx = (fe_cnt_cur_idx + 1) % FE_CNT_SECONDS;

    for(index=0; index<ANI_UNI_PORT_MAX-1; index++) {
        ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_UCAST_CNT, index, &cnt);
        tx_ok_cnt[index][fe_cnt_cur_idx] = cnt;
        ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_MCAST_CNT, index, &cnt);
        tx_ok_cnt[index][fe_cnt_cur_idx] += cnt;
        ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_BCAST_CNT, index, &cnt);
        tx_ok_cnt[index][fe_cnt_cur_idx] += cnt;
        
        ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_RX_UCAST_CNT, index, &cnt);
        rx_ok_cnt[index][fe_cnt_cur_idx] = cnt;
        ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_RX_MCAST_CNT, index, &cnt);
        rx_ok_cnt[index][fe_cnt_cur_idx] += cnt;
        ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_RX_BCAST_CNT, index, &cnt);
        rx_ok_cnt[index][fe_cnt_cur_idx] += cnt;
        
        ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_OCT_CNT_L, index, &cnt);
        tx_ok_byte_cnt[index][fe_cnt_cur_idx] = cnt;
        
        ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_RX_OCT_CNT_L, index, &cnt);
        rx_ok_byte_cnt[index][fe_cnt_cur_idx] = cnt;

		ETHER_API_GET_DROP_CRC_COUNTER(index, &cnt_discard_rx, &cnt_discard_tx, &cnt_error_rx, &cnt_error_tx);
		rx_discard_cnt[index][fe_cnt_cur_idx] = cnt_discard_rx;
		tx_discard_cnt[index][fe_cnt_cur_idx] = cnt_discard_tx;
		rx_error_cnt[index][fe_cnt_cur_idx] = cnt_error_rx;
		tx_error_cnt[index][fe_cnt_cur_idx] = cnt_error_tx;
    }
    
    tx_ok_cnt[ANI_UNI_PORT_MAX-1][fe_cnt_cur_idx] = read_reg_word(GDMA2_TX_OKCNT);
    tx_ok_byte_cnt[ANI_UNI_PORT_MAX-1][fe_cnt_cur_idx] = read_reg_word(GDMA2_TX_OKBYTE_CNT);
    rx_ok_cnt[ANI_UNI_PORT_MAX-1][fe_cnt_cur_idx] = read_reg_word(GDMA2_RX_OKCNT);
    rx_ok_byte_cnt[ANI_UNI_PORT_MAX-1][fe_cnt_cur_idx] = read_reg_word(GDMA2_RX_OKBYTECNT);

	rx_discard_cnt[ANI_UNI_PORT_MAX-1][fe_cnt_cur_idx] = read_reg_word(GDMA2_RX_ETHDROPCNT) + read_reg_word(GDMA2_RX_FCDROPCNT)
		+ read_reg_word(GDMA2_RX_RCDROPCNT) + read_reg_word(GDMA2_RX_OVDROPCNT) + read_reg_word(GDMA2_RX_ERRDROPCNT);
	tx_discard_cnt[ANI_UNI_PORT_MAX-1][fe_cnt_cur_idx] = read_reg_word(GDMA2_TX_DROPCNT);
	rx_error_cnt[ANI_UNI_PORT_MAX-1][fe_cnt_cur_idx] = read_reg_word(GDMA2_RX_ETHCRCCNT);
	tx_error_cnt[ANI_UNI_PORT_MAX-1][fe_cnt_cur_idx] = 0;
    
    for(index=0; index<ANI_UNI_PORT_MAX; index++) {
        if(tx_ok_cnt[index][fe_cnt_cur_idx] < tx_ok_cnt[index][pre_idx]) {
            /* if tx/rx ok cnt less than previous value, and previous value is bigger than FE_CNT_THR, means spillover, no need to add prevous value */
            /* if prefious value is less than FE_CNT_THR, means clear by register clear, need to add previous value */
            if(tx_ok_cnt[index][pre_idx] < FE_CNT_THR)
                tx_ok_cnt[index][fe_cnt_cur_idx] += tx_ok_cnt[index][pre_idx];
        }
        if(tx_ok_byte_cnt[index][fe_cnt_cur_idx] < tx_ok_byte_cnt[index][pre_idx]) {
            if(tx_ok_byte_cnt[index][pre_idx] < FE_BYTE_THR)
                tx_ok_byte_cnt[index][fe_cnt_cur_idx] += tx_ok_byte_cnt[index][pre_idx];
        }
        if(rx_ok_cnt[index][fe_cnt_cur_idx] < rx_ok_cnt[index][pre_idx]) {
            if(rx_ok_cnt[index][pre_idx] < FE_CNT_THR)
                rx_ok_cnt[index][fe_cnt_cur_idx] += rx_ok_cnt[index][pre_idx];
        }
        if(rx_ok_byte_cnt[index][fe_cnt_cur_idx] < rx_ok_byte_cnt[index][pre_idx]) {
            if(rx_ok_byte_cnt[index][pre_idx] < FE_BYTE_THR)
                rx_ok_byte_cnt[index][fe_cnt_cur_idx] += rx_ok_byte_cnt[index][pre_idx];
        }

		if(rx_discard_cnt[index][fe_cnt_cur_idx] < rx_discard_cnt[index][pre_idx]) {
            if(rx_discard_cnt[index][pre_idx] < FE_CNT_THR)
                rx_discard_cnt[index][fe_cnt_cur_idx] += rx_discard_cnt[index][pre_idx];
        }
		if(tx_discard_cnt[index][fe_cnt_cur_idx] < tx_discard_cnt[index][pre_idx]) {
            if(tx_discard_cnt[index][pre_idx] < FE_CNT_THR)
                tx_discard_cnt[index][fe_cnt_cur_idx] += tx_discard_cnt[index][pre_idx];
        }
		if(rx_error_cnt[index][fe_cnt_cur_idx] < rx_error_cnt[index][pre_idx]) {
            if(rx_error_cnt[index][pre_idx] < rx_error_cnt)
                rx_error_cnt[index][fe_cnt_cur_idx] += rx_error_cnt[index][pre_idx];
        }
		if(tx_error_cnt[index][fe_cnt_cur_idx] < tx_error_cnt[index][pre_idx]) {
            if(tx_error_cnt[index][pre_idx] < tx_error_cnt)
                tx_error_cnt[index][fe_cnt_cur_idx] += tx_error_cnt[index][pre_idx];
        }
    }
    
	/* Schedule for the next time */
    fe_cnt_timer.expires = jiffies + msecs_to_jiffies(1000);
    add_timer(&fe_cnt_timer);
}

struct ecnt_hook_ops ecnt_fe_api_op = {
	.name = "fe_api_hook",
	.is_execute = 1,
	.hookfn = ecnt_fe_api_hook,
	.maintype = ECNT_FE,
	.subtype = ECNT_FE_API,
	.priority = 1
};

int fe_pse_reset_info_read(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    printk("PSE reset cnt        : %u\n", g_fe_iq_reset_cnt);
    printk("Last PSE_IQ_STA1     : 0x%x\n", g_fe_last_iq_sta1);
    printk("Last PSE_IQ_STA2     : 0x%x\n\n", g_fe_last_iq_sta2);

    printk("QDMA WAN reset cnt   : %u\n", g_fe_qdma_reset_cnt);
    printk("Last DBG_LMGR_STATUS : 0x%x\n\n", g_fe_last_qdma_dscp);

    printk("FE reset cnt         : %u\n", g_fe_all_reset_cnt);
    printk("Last GDM2_TX_CHN_VLD : 0x%x\n\n", g_fe_last_gdma_chh_vld);

    printk("QDMA LAN reset cnt   : %u\n", g_fe_qdma_lan_reset_cnt);
    printk("Last DBG_LMGR_STATUS : 0x%x\n\n", g_fe_last_qdma_lan_dscp);
    return 0;
}

int fe_pse_reset_threshold_read(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    printk("PSE reset threshold: %u\n", g_fe_iq_abnormal_threshold);
    return 0;
}

int fe_pse_reset_threshold_write(struct file *file, const char *buffer,
    unsigned long count, void *data)
{
    char val_string[64];

    if (count >= sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

    val_string[count] = '\0';
    g_fe_iq_abnormal_threshold = simple_strtoul(val_string, NULL, 10);  

    return count;
}

int fe_pse_reset_action_read(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    printk("PSE reset action: %u\n", g_fe_iq_reset_test_flag);
    return 0;
}

int fe_pse_reset_action_write(struct file *file, const char *buffer,
    unsigned long count, void *data)
{
    uint32 action = 0;
    char val_string[64];

    if (count >= sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

    val_string[count] = '\0';
    g_fe_iq_reset_test_flag = simple_strtoul(val_string, NULL, 10);

    return count;
}

int fe_pse_reset_test_read(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    printk("PSE reset cfg: %u\n", g_fe_pse_reset_test);
    return 0;
}

int fe_pse_reset_test_write(struct file *file, const char *buffer,
    unsigned long count, void *data)
{
    uint32 action = 0;
    char val_string[64];

    if (count >= sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

    val_string[count] = '\0';
    g_fe_pse_reset_test = simple_strtoul(val_string, NULL, 10);

    return count;
}

int fe_pse_soft_core_reset_write(struct file *file, const char *buffer,
    unsigned long count, void *data)
{
	char val_string[64], cmd[16] ;
    unsigned int value = 0;
	
    if (count >= sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

	sscanf(val_string, "%s ", cmd) ;

	if(!strcmp(cmd, "reset")) {
		fe_core_soft_reset();
		fe_pse_core_soft_reset_cnt++;
	}else if(!strcmp(cmd, "clear")) {
		fe_pse_core_soft_reset_cnt=0;
	}

    return count;
}
int fe_pse_soft_core_reset_read(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	printk("fe_pse_soft_core_reset=%u\n",fe_pse_core_soft_reset_cnt);
	return 0;
}
int fe_pse_gdma2_iq_state_read(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    printk("gdma2 iq threshold: %u\n", gdma2_iq_threshold);
    printk("gdma2 countinous drop time: %u\n", gdma2_drop_time);
	printk("Number of abnormal events: %d\n",gdma2_iq_abnormal_cnt);
    return 0;
}

int fe_pse_gdma2_iq_state_write(struct file *file, const char *buffer,
    unsigned long count, void *data)
{
	char val_string[64], cmd[16] ;
    unsigned int value = 0;
	
    if (count >= sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

	sscanf(val_string, "%s %d", cmd, &value) ;

	if(!strcmp(cmd, "threshold")) {
		gdma2_iq_threshold = value;
	}else if(!strcmp(cmd, "time")){
		gdma2_drop_time =value;
	}

    return count;
}

static int fe_dyn_qdma_wan_txq_cngst_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len = 0;
	len = sprintf(page, "%d\n", fe_dyn_qdma_wan_txq_cngst_enable);
	
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

static int fe_dyn_qdma_wan_txq_cngst_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[32];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	if (sscanf(val_string, "%d", &fe_dyn_qdma_wan_txq_cngst_enable) != 1) {
		printk("usage: <fe_dyn_qdma_wan_txq_cngst_enable>\n");
	}
	
	return count;
}

int fe_set_vr_wan_read_proc(char *buf, char **start, off_t off, int count,
				 int *eof, void *data)
{
	int i = 0;
	
	for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
		if(wan_itf_info[i].valid == 0)
			continue;
		
		if(wan_itf_info[i].is_vr_wan){
			printk("vr wan: %s\n", wan_itf_info[i].wan_dev->name);
			break;
		}
	}

	return 0;
}

int fe_set_vr_wan_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	int i = 0;
	char val_string[16];
	char wan_name[16];
	struct net_device *vr_dev = NULL;

	if (count >= sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	val_string[count] = '\0';

	memset(wan_name,0,16);
	sscanf(val_string, "%s", wan_name);

	for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
		if(wan_itf_info[i].valid == 0)
			continue;

		wan_itf_info[i].is_vr_wan = 0;	// clear all first
		if(strcmp(wan_itf_info[i].wan_dev->name, wan_name) == 0){
			wan_itf_info[i].is_vr_wan = 1;
#if defined(TCSUPPORT_CT_JOYME4) 
#if KERNEL_2_6_36
			vr_dev = dev_get_by_name(&init_net, wan_name);
#else
			vr_dev = dev_get_by_name(wan_name);
#endif		
			if ( vr_dev ) {
				IFC_API_SET_LUT0_RULE(15, IFC_ENABLE, 18, 0, 0xFFF, 0, vr_dev->tci, 0) ;
				dev_put(vr_dev);
			}
#endif
		}
	}

	return count;
}


/* FAQ Set Config*/

int fe_faq_cfg_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[64] = {0}, cmd[64] = {0}, subcmd[64] = {0};
	uint num=0,cnt=0;
	
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%s %s %d", cmd, subcmd, &num) ;
	
	if(!strcmp(cmd, "faq1")) { 
		if(!strcmp(subcmd, "enable")) { 
			cdm1SetFaqEnable;
		}
		else if(!strcmp(subcmd, "disable")){
			cdm1SetFaqDisable;
		}
		else if(!strcmp(subcmd, "sel")){
			cdm1SetFaqDisable;
			cdm1SetFlowSel(num);
			cdm1SetFaqEnable;
		}
		else if(!strcmp(subcmd, "mode")){
			cdm1SetFaqDisable;
			cdm1SetFlowMode(num);
			cdm1SetFaqEnable;
		}
		else if(!strcmp(subcmd, "timeout")){
			cdm1SetFaqDisable;
			cdm1SetTimeout(num);
			cdm1SetFaqEnable;
 		}
		else if(!strcmp(subcmd, "thr")){
			cdm1SetFaqDisable;
			cdm1SetLthr(num);
			cdm1SetFaqEnable;
 		}
	}else if(!strcmp(cmd, "faq2")) { 
		if(!strcmp(subcmd, "enable")) { 
			cdm2SetFaqEnable;
		}
		else if(!strcmp(subcmd, "disable")){
			cdm2SetFaqDisable;
		}
		else if(!strcmp(subcmd, "sel")){
			cdm2SetFaqDisable;
			cdm2SetFlowSel(num);
			cdm2SetFaqEnable;
		}
		else if(!strcmp(subcmd, "mode")){
			cdm2SetFaqDisable;
			cdm2SetFlowMode(num);
			cdm2SetFaqEnable;
		}
		else if(!strcmp(subcmd, "timeout")){
			cdm2SetFaqDisable;
			cdm2SetTimeout(num);
			cdm2SetFaqEnable;
 		}
		else if(!strcmp(subcmd, "thr")){
			cdm2SetFaqDisable;
			cdm2SetLthr(num);
			cdm2SetFaqEnable;
 		}
	}else if(!strcmp(cmd, "dump")) { 
		if(!strcmp(subcmd, "nfaqcnt")) { 
			IO_SBITS(CDM1_FAQDBG_CFG,1<<FAQDBG_CNTSEL_SHIFT);
			cnt=cdm1GetEnqCnt;
			printk("nfaq1cnt=%d\n",cnt);
			IO_SBITS(CDM2_FAQDBG_CFG,1<<FAQDBG_CNTSEL_SHIFT);
			cnt=cdm2GetEnqCnt;
			printk("nfaq2cnt=%d\n",cnt);
		}
	}
	return count;
}

int isEtherWanVirInterface(char *name)
{
	int i = 0;

	for(i = 0; i < 16; i++){
		if(wan_virtualdev_ifname_info[i].valid == 0)
			continue;
		if(strncmp(name, wan_virtualdev_ifname_info[i].virtual_name, 16) == 0)
			return 1;
	}

	return 0;
}

int is_wan_interface(struct net_device *dev)
{
	if((!isEtherWanVirInterface(dev->name)) 
		&& (memcmp(dev->name, "pon0.", strlen("pon0.")) != 0) 
		&& (memcmp(dev->name, "nas", strlen("nas")) != 0))
    {
    	return 0;
    }

#if defined(TCSUPPORT_XPON_HAL_API_EXT) || defined(TCSUPPORT_XPON_HAL_API)
    return 1;
#endif
    if((dev->name[WAN_ITF_NAME_PRE_LEN] < '0') ||
        (dev->name[WAN_ITF_NAME_PRE_LEN] >= '8'))
        return 0;

	if(memcmp(dev->name, "nas10", 5) == 0)
		return 0;
#ifdef CONFIG_TP_IMAGE
	/* do not use eth0.8 */
	if(memcmp(dev->name, "eth0.8", 6) == 0)
		return 0;
#endif /* CONFIG_TP_IMAGE */
	return 1;
}

static int netdev_down_update_acntgroup(int wan_itf)
{
    int i=0;
    
    for(i=0; i<WAN_ITF_AND_DEV_NUM; i++) {
        if((acntGrpGeneralTable[i].valid == 1) && (acntGrpGeneralTable[i].wan_itf == wan_itf)) {
            acntGrpGeneralTable[i].valid = 0;
        }
    }
    
    for(i=0; i<WAN_ITF_ONLY_NUM; i++) {
        if((acntGrpWanItfTable[i].valid == 1) && (acntGrpWanItfTable[i].wan_itf == wan_itf)) {
            acntGrpWanItfTable[i].valid = 0;
        }
    }
    
    for(i=0; i<MULTICAST_DOWNSTREAM_NUM; i++) {
        if((acntGrpMulticastTable[i].valid == 1) && (acntGrpMulticastTable[i].wan_itf == wan_itf)) {
            acntGrpMulticastTable[i].valid = 0;
        }
    }

    return 0;
}

#ifdef CONFIG_TP_IMAGE
static int smux_clear_stats64_by_dev(struct net_device *dev)
{
	struct smux_api_data_s smux_data = {0};
	SMUX_Stats64_By_dev indate = {0};
	
	if(dev == NULL)
	{
		return -1;
	}
	
	indate.dev = dev;
	
	smux_data.api_type = SMUX_API_TYPE_SET;
	smux_data.cmd_id = SMUX_CLEAR_STATS64_BY_DEV;
	smux_data.smux_stats64_by_dev = &indate ;

	if(ECNT_HOOK_ERROR == __ECNT_HOOK(ECNT_SMUX, ECNT_SMUX_API, (struct ecnt_data * )&smux_data) ){
		printk("ECNT_HOOK_ERROR occur with cmd_id:0x%x\n", smux_data.cmd_id);
		return -1;
	}	

	if(smux_data.ret != SMUX_ECNT_HOOK_PROCESS_SUCESS)
	{
		printk("smux_get_stats64_by_dev_tp Fail \n");
		return -1;
	}
	return 0;
}


#endif /* CONFIG_TP_IMAGE */

static int fe_res_device_event(struct notifier_block *unused, unsigned long event, void *ptr)
{
    int i;
    int idx=0;
    char wan_index_num = -1;
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
    struct net_device *dev = netdev_notifier_info_to_dev(ptr);
#else
    struct net_device *dev = ptr;
#endif

    switch (event) {
    case NETDEV_UP:
        if(!is_wan_interface(dev))
            return NOTIFY_DONE;
        
        for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
            if(wan_itf_info[i].valid == 0){
#ifndef CONFIG_TP_IMAGE
                if(TCSUPPORT_CT_VAL || TCSUPPORT_OPENWRT_MULTI_WAN_VAL)
                    wan_index_num = (dev->name[3] - '0') * 8 + (dev->name[5] - '0');
                else
#endif /* CONFIG_TP_IMAGE */
                    wan_index_num = (dev->name[3] - '0') * 8;
                wan_itf_info[i].wan_dev = dev;
                wan_itf_info[i].valid = 1;
                wan_itf_info[i].wanIndex = wan_index_num;
                for(idx=0; idx<WAN_ITF_ACNT_NUM; idx++)
                    printk("NETDEV_UP: wan_itf_info[%d].valid=%d, wan_itf_info[%d].wan_dev=%s,wanIndex=%d\n"
                    , idx, wan_itf_info[idx].valid, idx, (wan_itf_info[idx].valid==0)?("NULL"):wan_itf_info[idx].wan_dev->name,wan_itf_info[idx].wanIndex);
                return NOTIFY_DONE;
            }
        }
        break;

    case NETDEV_DOWN:
#ifdef CONFIG_TP_IMAGE
        /* we clear ppp intf nas stats here because when ppp down , nas's count can be cleared */
        if(is_wan_interface(dev) || 0 == memcmp(dev->name,"ppp",3))
        {
            smux_clear_stats64_by_dev(dev);
        }
#endif /* CONFIG_TP_IMAGE */

        if(!is_wan_interface(dev))
            return NOTIFY_DONE;
        
        for(i = 0; i < WAN_ITF_ACNT_NUM; i++){
            if(wan_itf_info[i].wan_dev == dev){
                wan_itf_info[i].wan_dev = NULL;
                wan_itf_info[i].valid = 0;
                wan_itf_info[i].wanIndex = -1;
                netdev_down_update_acntgroup(i);
                for(idx=0; idx<WAN_ITF_ACNT_NUM; idx++)
                    printk("NETDEV_DOWN: wan_itf_info[%d].valid=%d, wan_itf_info[%d].wan_dev=%s\n"
                    , idx, wan_itf_info[idx].valid, idx, (wan_itf_info[idx].valid==0)?("NULL"):wan_itf_info[idx].wan_dev->name);
                return NOTIFY_DONE;
            }
        }
        break;
    default :      
        break;
    }

    return NOTIFY_DONE;
} 

static struct notifier_block fe_res_notifier_block = {
    .notifier_call = fe_res_device_event,
};

int fe_get_wan_itf_cnt(int idx, int dir, unsigned int *byteCnt_L, unsigned int *byteCnt_H, unsigned int *pktCnt){

    /* retrieve cpu path statistics for !7580 wan interface mode */
    if (wan_itf_info[idx].wan_dev)
    {
        if (wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats64)
        {
            struct rtnl_link_stats64 *p_stats = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
            struct rtnl_link_stats64 tmp = {0};
            p_stats = wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats64(wan_itf_info[idx].wan_dev, &tmp);
#else
			wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats64(wan_itf_info[idx].wan_dev, p_stats);
#endif
            if (p_stats)
            {
                if(dir == DOWNSTREAM_ONLY)
                {
                    *byteCnt_H = (uint)((p_stats->rx_bytes >> 32) & 0xFFFFFFFFUL) ;
                    *byteCnt_L = (uint)(p_stats->rx_bytes & 0xFFFFFFFFUL) ;
                    *pktCnt = (uint)(p_stats->rx_packets) ;
                }
                else
                {
                    *byteCnt_H = (uint)((p_stats->tx_bytes >> 32) & 0xFFFFFFFFUL) ;
                    *byteCnt_L = (uint)(p_stats->tx_bytes & 0xFFFFFFFFUL) ;
                    *pktCnt = (uint)(p_stats->tx_packets) ;
                }
            }
        }
        else if (wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats)
        {
            struct net_device_stats *p_stats = NULL;
            
            p_stats = wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats(wan_itf_info[idx].wan_dev);
            if (p_stats)
            {
                if(dir == DOWNSTREAM_ONLY)
                {
                    *byteCnt_L = p_stats->rx_bytes ;
                    *pktCnt = p_stats->rx_packets ;
                }
                else
                {
                    *byteCnt_L = p_stats->tx_bytes ;
                    *pktCnt = p_stats->tx_packets ;
                }
            }
        }
    }

    return 0;
}

int fe_clear_wan_itf_cnt(int idx, int dir){

    /* retrieve cpu path statistics for !7580 wan interface mode */
    if (wan_itf_info[idx].wan_dev)
    {
        if (wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats64)
        {
            struct rtnl_link_stats64 *p_stats = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
            struct rtnl_link_stats64 tmp = {0};
            p_stats = wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats64(wan_itf_info[idx].wan_dev, &tmp);
#else
			wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats64(wan_itf_info[idx].wan_dev, p_stats);
#endif
            if (p_stats)
            {
                if(dir == DOWNSTREAM_ONLY)
                {
                    p_stats->rx_bytes = 0 ;
                    p_stats->rx_packets = 0 ;
                }
                else
                {
                    p_stats->tx_bytes = 0 ;
                    p_stats->tx_packets = 0 ;
                }
            }
        }
        else if (wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats)
        {
            struct net_device_stats *p_stats = NULL;
            
            p_stats = wan_itf_info[idx].wan_dev->netdev_ops->ndo_get_stats(wan_itf_info[idx].wan_dev);
            if (p_stats)
            {
                if(dir == DOWNSTREAM_ONLY)
                {
                    p_stats->rx_bytes = 0 ;
                    p_stats->rx_packets = 0;
                }
                else
                {
                    p_stats->tx_bytes = 0 ;
                    p_stats->tx_packets = 0 ;
                }
            }
        }
    }

    return 0;
}

void get_wan_index_info(char * wan_index){    
    int i = 0;
    for(i = 0; i < 8; i ++){
        wan_index[i] = wan_itf_info[i].wanIndex ;
    }

    return ;
}

static void fe_account_timer_expires(TIMER_FUN_PAAM data)
{
    int i=0, j=0, multicast_num=0, foundWanItfFlag=0, foundDevFlag=0;
    unsigned long preBytes=0;
#if defined(CONFIG_BRIDGE_IGMP_SNOOPING) && defined(TCSUPPORT_IGMP_SNOOPING)
    struct __mc_fdb_entry *fdbs = NULL;
#endif
	struct net_device *ndev = NULL;
	struct net_bridge *br = NULL;
    struct list_head *pos = NULL;
    struct list_head *head = &total_account_list->total_account_node;
    wan_itf_or_dev_bandwidth_total_account_t *current_node = NULL;
    wan_itf_or_dev_bandwidth_total_account_t *new_node = NULL;
    const unsigned char empty[ETH_ALEN] = {0};
    unsigned int upstream_byte_lo=0;
    unsigned int upstream_byte_hi=0;
    unsigned int upstream_packet=0;
    unsigned int downstream_byte_lo=0;
    unsigned int downstream_byte_hi=0;
    unsigned int downstream_packet=0;
	unsigned char group_addr[40];
    
    for(i=0; i<WAN_ITF_AND_DEV_NUM; i++) {
        if(acntGrpGeneralTable[i].valid == 1) {
            foundWanItfFlag = foundDevFlag = 0;
            upstream_byte_hi = read_reg_word(PPE_AC_BCNT_H(2*i));
            upstream_byte_lo = read_reg_word(PPE_AC_BCNT_L(2*i));
            upstream_packet = read_reg_word(PPE_AC_PCNT(2*i));
            downstream_byte_hi = read_reg_word(PPE_AC_BCNT_H(2*i+1));
            downstream_byte_lo = read_reg_word(PPE_AC_BCNT_L(2*i+1));
            downstream_packet = read_reg_word(PPE_AC_PCNT(2*i+1));
            read_lock_bh(&account_lock);
            list_for_each(pos, head) {
                current_node = list_entry(pos, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
                if((current_node->account_type == ACCOUNT_TYPE_WAN_INTERFACE) 
                    && (acntGrpGeneralTable[i].wan_itf == current_node->wan_itf_index)) {
                    foundWanItfFlag = 1;
                    current_node->upstream_total_packet += upstream_packet;
                    current_node->upstream_total_byte_hi += upstream_byte_hi;
                    preBytes = current_node->upstream_total_byte_lo;
                    current_node->upstream_total_byte_lo += upstream_byte_lo;
                    if(preBytes > current_node->upstream_total_byte_lo)
                        current_node->upstream_total_byte_hi ++;
                    
                    current_node->downstream_total_packet += downstream_packet;
                    current_node->downstream_total_byte_hi += downstream_byte_hi;
                    preBytes = current_node->downstream_total_byte_lo;
                    current_node->downstream_total_byte_lo += downstream_byte_lo;
                    if(preBytes > current_node->downstream_total_byte_lo)
                        current_node->downstream_total_byte_hi ++;
                } else if((current_node->account_type == ACCOUNT_TYPE_DEV_BANDWIDTH)
                && (memcmp(acntGrpGeneralTable[i].mac, current_node->mac, ETH_ALEN) == 0)) {
                    foundDevFlag = 1;
                    current_node->upstream_total_packet += upstream_packet;
                    current_node->upstream_total_byte_hi += upstream_byte_hi;
                    preBytes = current_node->upstream_total_byte_lo;
                    current_node->upstream_total_byte_lo += upstream_byte_lo;
                    if(preBytes > current_node->upstream_total_byte_lo)
                        current_node->upstream_total_byte_hi ++;
                    
                    current_node->downstream_total_packet += downstream_packet;
                    current_node->downstream_total_byte_hi += downstream_byte_hi;
                    preBytes = current_node->downstream_total_byte_lo;
                    current_node->downstream_total_byte_lo += downstream_byte_lo;
                    if(preBytes > current_node->downstream_total_byte_lo)
                        current_node->downstream_total_byte_hi ++;
                }
            }
            read_unlock_bh(&account_lock);
            
            if(foundWanItfFlag == 0) {
                printk("fe: allocate new_node: type=WAN_INTERFACE, wan_itf_index[%d]=%d(%s)\n"
                    , acntGrpGeneralTable[i].wan_itf, wan_itf_info[acntGrpGeneralTable[i].wan_itf].wanIndex
                    , wan_itf_info[acntGrpGeneralTable[i].wan_itf].wan_dev->name);
            	new_node = kzalloc(sizeof(wan_itf_or_dev_bandwidth_total_account_t), GFP_ATOMIC);
            	if(new_node == NULL){	
            		printk(KERN_WARNING "fe: allocate memory for total_account_list fail\n");
            		return ;
            	}
                new_node->account_type = ACCOUNT_TYPE_WAN_INTERFACE;
                new_node->wan_itf_index = acntGrpGeneralTable[i].wan_itf;
                new_node->upstream_total_byte_hi = upstream_byte_hi;
                new_node->upstream_total_byte_lo = upstream_byte_lo;
                new_node->upstream_total_packet = upstream_packet;
                new_node->downstream_total_byte_hi = downstream_byte_hi;
                new_node->downstream_total_byte_lo = downstream_byte_lo;
                new_node->downstream_total_packet = downstream_packet;
                write_lock_irq(&account_lock);
                list_add_tail(&new_node->total_account_node, head);
                write_unlock_irq(&account_lock);
            }
            
            if((foundDevFlag == 0) && (memcmp(acntGrpGeneralTable[i].mac, empty, ETH_ALEN) != 0)) {
                printk("fe: allocate new_node: type=DEV_BANDWIDTH, mac=%pM\n", acntGrpGeneralTable[i].mac);
            	new_node = kzalloc(sizeof(wan_itf_or_dev_bandwidth_total_account_t), GFP_ATOMIC);
            	if(new_node == NULL){	
            		printk(KERN_WARNING "fe: allocate memory for total_account_list fail\n");
            		return ;
            	}
                new_node->account_type = ACCOUNT_TYPE_DEV_BANDWIDTH;
                memcpy(new_node->mac, acntGrpGeneralTable[i].mac, ETH_ALEN);
                new_node->upstream_total_byte_hi = upstream_byte_hi;
                new_node->upstream_total_byte_lo = upstream_byte_lo;
                new_node->upstream_total_packet = upstream_packet;
                new_node->downstream_total_byte_hi = downstream_byte_hi;
                new_node->downstream_total_byte_lo = downstream_byte_lo;
                new_node->downstream_total_packet = downstream_packet;
                write_lock_irq(&account_lock);
                list_add_tail(&new_node->total_account_node, head);
                write_unlock_irq(&account_lock);
            }
        }
    }

    for(i=0; i<WAN_ITF_ONLY_NUM; i++) {
        if(acntGrpWanItfTable[i].valid == 1) {
            foundWanItfFlag = 0;
            upstream_byte_hi = read_reg_word(PPE_AC_BCNT_H(WAN_ITF_ONLY_BASE+2*i));
            upstream_byte_lo = read_reg_word(PPE_AC_BCNT_L(WAN_ITF_ONLY_BASE+2*i));
            upstream_packet = read_reg_word(PPE_AC_PCNT(WAN_ITF_ONLY_BASE+2*i));
            downstream_byte_hi = read_reg_word(PPE_AC_BCNT_H(WAN_ITF_ONLY_BASE+2*i+1));
            downstream_byte_lo = read_reg_word(PPE_AC_BCNT_L(WAN_ITF_ONLY_BASE+2*i+1));
            downstream_packet = read_reg_word(PPE_AC_PCNT(WAN_ITF_ONLY_BASE+2*i+1));
            read_lock_bh(&account_lock);
            list_for_each(pos, head) {
                current_node = list_entry(pos, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
                if((current_node->account_type == ACCOUNT_TYPE_WAN_INTERFACE) 
                    && (acntGrpWanItfTable[i].wan_itf == current_node->wan_itf_index)) {
                    foundWanItfFlag = 1;
                    current_node->upstream_total_packet += upstream_packet;
                    current_node->upstream_total_byte_hi += upstream_byte_hi;
                    preBytes = current_node->upstream_total_byte_lo;
                    current_node->upstream_total_byte_lo += upstream_byte_lo;
                    if(preBytes > current_node->upstream_total_byte_lo)
                        current_node->upstream_total_byte_hi ++;
                    
                    current_node->downstream_total_packet += downstream_packet;
                    current_node->downstream_total_byte_hi += downstream_byte_hi;
                    preBytes = current_node->downstream_total_byte_lo;
                    current_node->downstream_total_byte_lo += downstream_byte_lo;
                    if(preBytes > current_node->downstream_total_byte_lo)
                        current_node->downstream_total_byte_hi ++;
                    break;
                }
            }
            read_unlock_bh(&account_lock);
            
            if(foundWanItfFlag == 0) {
                printk("fe: allocate new_node: type=WAN_INTERFACE, wan_itf_index[%d]=%d(%s)\n"
                    , acntGrpGeneralTable[i].wan_itf, wan_itf_info[acntGrpGeneralTable[i].wan_itf].wanIndex
                    , wan_itf_info[acntGrpGeneralTable[i].wan_itf].wan_dev->name);
            	new_node = kzalloc(sizeof(wan_itf_or_dev_bandwidth_total_account_t), GFP_ATOMIC);
            	if(new_node == NULL){	
            		printk(KERN_WARNING "fe: allocate memory for total_account_list fail\n");
            		return ;
            	}
                new_node->account_type = ACCOUNT_TYPE_WAN_INTERFACE;
                new_node->wan_itf_index = acntGrpWanItfTable[i].wan_itf;
                new_node->upstream_total_byte_hi = upstream_byte_hi;
                new_node->upstream_total_byte_lo = upstream_byte_lo;
                new_node->upstream_total_packet = upstream_packet;
                new_node->downstream_total_byte_hi = downstream_byte_hi;
                new_node->downstream_total_byte_lo = downstream_byte_lo;
                new_node->downstream_total_packet = downstream_packet;
                write_lock_irq(&account_lock);
                list_add_tail(&new_node->total_account_node, head);
                write_unlock_irq(&account_lock);
            }
        }
    }

    multicast_num = 0;
#if defined(CONFIG_BRIDGE_IGMP_SNOOPING) && defined(TCSUPPORT_IGMP_SNOOPING)
#ifdef TCSUPPORT_IGMPSNOOPING_ENHANCE
    if(multicast_buf) {
        #if KERNEL_2_6_36
        ndev = dev_get_by_name(&init_net, "br0");
        #else
        ndev = dev_get_by_name("br0");
        #endif
        if ( ndev ) {
            br = netdev_priv(ndev);
            dev_put(ndev);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
            multicast_num = br_mdb_fillbuf(br, multicast_buf, 128, 0);
#else
			multicast_num = 0;
#endif
            fdbs = (struct __mc_fdb_entry *)multicast_buf;
        }
    }
#endif

    if(multicast_num != 0) {
        for(i=0; i<MULTICAST_DOWNSTREAM_NUM; i++) {
            if(acntGrpMulticastTable[i].valid == 1) {
                foundWanItfFlag = 0;
                downstream_byte_hi = read_reg_word(PPE_AC_BCNT_H(MULTICAST_DOWNSTREAM_BASE+i));
                downstream_byte_lo = read_reg_word(PPE_AC_BCNT_L(MULTICAST_DOWNSTREAM_BASE+i));
                downstream_packet = read_reg_word(PPE_AC_PCNT(MULTICAST_DOWNSTREAM_BASE+i));
                read_lock_bh(&account_lock);
                list_for_each(pos, head) {
                    current_node = list_entry(pos, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
                    if((current_node->account_type == ACCOUNT_TYPE_WAN_INTERFACE) 
                        && (acntGrpMulticastTable[i].wan_itf == current_node->wan_itf_index)) {
                        foundWanItfFlag = 1;
                        current_node->downstream_total_packet += downstream_packet;
                        current_node->downstream_total_byte_hi += downstream_byte_hi;
                        preBytes = current_node->downstream_total_byte_lo;
                        current_node->downstream_total_byte_lo += downstream_byte_lo;
                        if(preBytes > current_node->downstream_total_byte_lo)
                            current_node->downstream_total_byte_hi ++;
                        break;
                    }
                }
                read_unlock_bh(&account_lock);
                
                if(foundWanItfFlag == 0) {
                    printk("fe: allocate new_node: type=WAN_INTERFACE, wan_itf_index[%d]=%d(%s)\n"
                        , acntGrpMulticastTable[i].wan_itf, wan_itf_info[acntGrpMulticastTable[i].wan_itf].wanIndex
                        , wan_itf_info[acntGrpMulticastTable[i].wan_itf].wan_dev->name);
                	new_node = kzalloc(sizeof(wan_itf_or_dev_bandwidth_total_account_t), GFP_ATOMIC);
                	if(new_node == NULL){	
                		printk(KERN_WARNING "fe: allocate memory for total_account_list fail\n");
                		return ;
                	}
                    new_node->account_type = ACCOUNT_TYPE_WAN_INTERFACE;
                    new_node->wan_itf_index = acntGrpMulticastTable[i].wan_itf;
                    new_node->downstream_total_byte_hi = downstream_byte_hi;
                    new_node->downstream_total_byte_lo = downstream_byte_lo;
                    new_node->downstream_total_packet = downstream_packet;
                    write_lock_irq(&account_lock);
                    list_add_tail(&new_node->total_account_node, head);
                    write_unlock_irq(&account_lock);
                }
                
                if(acntGrpMulticastTable[i].group_ip_type == 0) {
                    sprintf(group_addr, NIPQUAD_FMT, NIPQUAD(acntGrpMulticastTable[i].group_ip.ip4));
                } else if(acntGrpMulticastTable[i].group_ip_type == 1) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
                    strncpy(group_addr, ip6_sprintf(&acntGrpMulticastTable[i].group_ip.ip6), sizeof(group_addr)-1);
#endif
                }
                for(j=0; j<multicast_num; j++) {
                    if(memcmp(group_addr, fdbs[j].group_addr, strlen(group_addr)) == 0) {
                        foundDevFlag = 0;
                        read_lock_bh(&account_lock);
                        list_for_each(pos, head) {
                            current_node = list_entry(pos, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
                            if((current_node->account_type == ACCOUNT_TYPE_DEV_BANDWIDTH)
                                && (memcmp(fdbs[j].host_addr, current_node->mac, ETH_ALEN) == 0)) {
                                foundDevFlag = 1;
                                current_node->downstream_total_packet += downstream_packet;
                                current_node->downstream_total_byte_hi += downstream_byte_hi;
                                preBytes = current_node->downstream_total_byte_lo;
                                current_node->downstream_total_byte_lo += downstream_byte_lo;
                                if(preBytes > current_node->downstream_total_byte_lo)
                                    current_node->downstream_total_byte_hi ++;
                                break;
                            }
                        }
                        read_unlock_bh(&account_lock);
                        
                        if((foundDevFlag == 0) && (memcmp(fdbs[j].host_addr, empty, ETH_ALEN) != 0)) {
                            printk("fe: allocate new_node: type=DEV_BANDWIDTH, mac=%pM\n", fdbs[j].host_addr);
                        	new_node = kzalloc(sizeof(wan_itf_or_dev_bandwidth_total_account_t), GFP_ATOMIC);
                        	if(new_node == NULL){	
                        		printk(KERN_WARNING "fe: allocate memory for total_account_list fail\n");
                        		return ;
                        	}
                            new_node->account_type = ACCOUNT_TYPE_DEV_BANDWIDTH;
                            memcpy(new_node->mac, fdbs[j].host_addr, ETH_ALEN);
                            new_node->downstream_total_byte_hi = downstream_byte_hi;
                            new_node->downstream_total_byte_lo = downstream_byte_lo;
                            new_node->downstream_total_packet = downstream_packet;
                            write_lock_irq(&account_lock);
                            list_add_tail(&new_node->total_account_node, head);
                            write_unlock_irq(&account_lock);
                        }
                    }
                }
                
            }
        }
    }
#endif

    /* Schedule for the next time */
    fe_account_timer.expires = jiffies + msecs_to_jiffies(1000*5);  /* update every 5 seconds. */
    add_timer(&fe_account_timer);

    return ;
}

int free_all_account_nodes(void) {
	struct list_head *head = &total_account_list->total_account_node;
    struct list_head *pos = head->next;
    struct list_head *tmp = NULL;
    wan_itf_or_dev_bandwidth_total_account_t *current_node = NULL;
    
    write_lock_irq(&account_lock);
    while(pos != head) {
        tmp = pos->next;
        current_node = list_entry(pos, wan_itf_or_dev_bandwidth_total_account_t, total_account_node);
        kfree(current_node);
        list_del(pos);
        pos = tmp;
    }
    write_unlock_irq(&account_lock);
    
    return 0;
}

int free_all_stb_src_ip_nodes(void) {
	struct list_head *head = &stb_src_ip_list->stb_src_ip_node;
    struct list_head *pos = head->next;
    struct list_head *tmp = NULL;
    stb_src_ip_t *current_node = NULL;
    
    write_lock_irq(&stb_src_ip_lock);
    while(pos != head) {
        tmp = pos->next;
        current_node = list_entry(pos, stb_src_ip_t, stb_src_ip_node);
        kfree(current_node);
        list_del(pos);
        pos = tmp;
    }
    write_unlock_irq(&stb_src_ip_lock);
    
    return 0;
}

void disable_woe_mode(void){
	unsigned int Raw = 0;

	#ifdef TCSUPPORT_CPU_ARMV8
	Raw = GET_WAN_CONF();
	Raw = (Raw & 0xffffffcf);     /* set bit4~bit5 to 0, as gsw mode */
	SET_WAN_CONF(Raw);
	#else
	Raw = read_reg_word(0xbfb00070);
	Raw = Raw & 0xffffffcf;     /* set bit4~bit5 to 0, as gsw mode */
	write_reg_word(0xbfb00070, Raw);
	#endif

	return;
}

void fe_set_cdm_oq_map(void)
{
	/*set rx ring-1 & ring-15 come from PSE Port0 OQ-1, other ring come from PSE Port0 OQ-0; */
	write_reg_word(FE_CDM_OQ_MAP, 0x10); 
	write_reg_word(FE_CDM_OQ_MAP1, 0x10000000);
	write_reg_word(FE_CDM2_OQ_MAP, 0x10); 
	write_reg_word(FE_CDM2_OQ_MAP1, 0x10000000);
}

void fe_set_qdma_fc_map(void)
{
	/*set QDMA2 flowcontrol channel10~11 mapping; */
	write_reg_word(QDMA2_FC_MAPPING5, 0x004b004a); 
}

void pse_set_share_threshold(void)
{
	write_reg_word(PSE_SHARE_USED_THD,PSE_SHARE_THRESHOLD);
}

void fe_set_per_oq_rsv(uint port,uint oq,uint val)
{
#if defined(TCSUPPORT_CPU_EN7581)
    uint ori_rsv = pse_get_oq_rsv(port,oq);
    uint tmp = 0;
    uint pse_port_oq_num[PSE_PORT_NUM] = {PSE_PORT0_QUEUE_NUM,PSE_PORT1_QUEUE_NUM,\
        PSE_PORT2_QUEUE_NUM,PSE_PORT3_QUEUE_NUM,PSE_PORT4_QUEUE_NUM,PSE_PORT5_QUEUE_NUM,\
        PSE_PORT6_QUEUE_NUM,PSE_PORT7_QUEUE_NUM,PSE_PORT8_QUEUE_NUM,PSE_PORT9_QUEUE_NUM,\
        PSE_PORT10_QUEUE_NUM};

    if(port >= PSE_PORT_NUM)
        return;

    if(oq >= pse_port_oq_num[port])
        return;
    
    pse_set_oq_rsv(port,oq,val);

    /*modify all rsv*/
    tmp = GET_PSE_ALL_RSV()-ori_rsv+val;
    SET_PSE_ALL_RSV(tmp);

#if defined(TCSUPPORT_CPU_AN7583)
	/*modify hthd*/
    tmp = GET_PSE_FQ_LITMI()-GET_PSE_ALL_RSV()-0x100;
    SET_PSE_SHARED_USED_HTHD(tmp);

    /*modify mthd&lthd,mthd = hthd - 0x100,lthd = mthd*0.75*/
    tmp = tmp-0x100;
    SET_PSE_SHARED_USED_MTHD(tmp);
    tmp = tmp*3/4;
    SET_PSE_SHARED_USED_LTHD(tmp);

#else
    /*modify hthd*/
    tmp = GET_PSE_FQ_LITMI()-GET_PSE_ALL_RSV()-0x20;
    SET_PSE_SHARED_USED_HTHD(tmp);

    /*modify mthd&lthd,mthd = all_share - 0x20,lthd = mthd*0.75*/
    tmp = GET_PSE_FQ_LITMI()-GET_PSE_ALL_RSV()-0x100;
    SET_PSE_SHARED_USED_MTHD(tmp);
    tmp = tmp*3/4;
    SET_PSE_SHARED_USED_LTHD(tmp);
#endif
#endif
    
    return;
}

#if defined(TCSUPPORT_CPU_EN7581)
static int fe_oq_rsv_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	off_t begin = 0;
	off_t pos = 0;
	int port_num = 0;
	int queue_num = 0;
	unsigned short port_oq_rsv[PSE_PORT_NUM_MAX][PSE_PORT_QUEUE_MAX];
	unsigned char port_queue_num[PSE_PORT_NUM_MAX] = PSE_PORT_QUEUE_NUM_ARRAY;

	for(port_num = 0;port_num < PSE_PORT_NUM_MAX;port_num++)
	{
		if(0 == port_queue_num[port_num])
			continue;

	        index += sprintf(buf+index, "\nP%d_OQ_RSV:\n",port_num);
	        CHK_BUF();

		for(queue_num = 0;queue_num < port_queue_num[port_num];queue_num++)
		{
			port_oq_rsv[port_num][queue_num] = pse_get_oq_rsv(port_num,queue_num);

			 if((0 == (queue_num+1)%4)||(queue_num == (port_queue_num[port_num]-1)))
		            {
		                index += sprintf(buf+index, "OQ%d_RSV=(0x%04x)\n", queue_num,port_oq_rsv[port_num][queue_num]);
		                CHK_BUF();
		            } 
		            else
		            {
		                index += sprintf(buf+index, "OQ%d_RSV=(0x%04x), ", queue_num,port_oq_rsv[port_num][queue_num]);
		                CHK_BUF();
		            }
		}

	}

        index += sprintf(buf+index, "\nPSE_ALL_RSV:\n");      
        CHK_BUF();      
        index += sprintf(buf+index, "PSE_ALL_RSV=(0x%04x)\n", GET_PSE_ALL_RSV());      
        CHK_BUF();

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

static int fe_oq_rsv_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	uint8_t val_string[64] = {0};
    uint port,oq,val;
	
	if (count > (sizeof(val_string) - 1))
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	
	sscanf(val_string, "%d %d 0x%x", &port, &oq, &val);
    printk("set port %d oq %d rsv 0x%x\n",port,oq,val);

    fe_set_per_oq_rsv(port,oq,val);
	
	return count;
}
#endif

void disable_xsi_mac(void)
{
	uint32 default_val = 0;

	default_val = GET_SCU_RST_RG();
	SET_SCU_RST_RG(default_val | XFI_MAC_PCIE0_RST | XFI_MAC_PCIE1_RST | XFI_MAC_USB_RST | XFI_MAC_ETH_RST);

	return 0;
}
void disable_gsw_port6(void)
{
	uint32 value = 0;

	value = read_reg_word(GSW_PMCR(6));
	value &= ~FORCE_LNK_PN;
	write_reg_word(GSW_PMCR(6), value);
	return 0;
}
static void fe_oq_rsv_init(void)
{
#if defined(TCSUPPORT_CPU_EN7581)
    uint pse_all_rsv = GET_PSE_ALL_RSV();
    int i = 0;

#if !defined(TCSUPPORT_CPU_AN7583)
    /*hw miss ppe2 oq rsv in all rsv in 7581,fix it*/
    pse_all_rsv += PSE_PORT8_QUEUE_NUM*PSE_RSV_PAGE_DEFAULT;
    SET_PSE_ALL_RSV(pse_all_rsv);
#endif
    /*reduce P0 oq to 0x40*/
    for(i = 0;i < PSE_PORT0_QUEUE_NUM;i++)
        fe_set_per_oq_rsv(0,i,0x40);
	
#if defined(TCSUPPORT_CPU_AN7583)
	/*7583 wifi to wan fastpath need to adjust port 0 oq4 reserver buffer to 0*/
		fe_set_per_oq_rsv(0,4,0);
	/*7583 P0 OQ5 need to change to 0xe0 because of the qdma faq function*/
		fe_set_per_oq_rsv(0,5,0xE0);
#endif

    /*reduce P1 oq to 0x40*/
    for(i = 0;i < PSE_PORT1_QUEUE_NUM;i++)
        fe_set_per_oq_rsv(1,i,0x40);

#if defined(TCSUPPORT_CPU_AN7583)
/*7583 P2 OQ HW default rsv change from 0x80 to 0x20,modify rsv to 0x80*/
	for(i = 0;i < 5;i++)
		fe_set_per_oq_rsv(2,i,0x80);
#endif
    /*rsv P2 OQ0~OQ5 page for pon,other oq rsv can be adjusted by API*/
    for(i = 6;i < PSE_PORT2_QUEUE_NUM;i++)
        fe_set_per_oq_rsv(2,i,0);

    /*reduce P3 oq to 0x40*/
    for(i = 0;i < PSE_PORT3_QUEUE_NUM;i++)
        fe_set_per_oq_rsv(3,i,0x40);

    /*rsv 0x40 for PPE OQ0&1,release OQ2&3*/
    fe_set_per_oq_rsv(4,0,0x40);
    fe_set_per_oq_rsv(4,1,0x40);
#if !defined(TCSUPPORT_CPU_AN7583)	
    fe_set_per_oq_rsv(4,2,0);
    fe_set_per_oq_rsv(4,3,0);
    fe_set_per_oq_rsv(8,0,0x40);
    fe_set_per_oq_rsv(8,1,0x40);
    fe_set_per_oq_rsv(8,2,0);
    fe_set_per_oq_rsv(8,3,0);
#endif

    /*reduce P5 oq to 0x40*/
    for(i = 0;i < PSE_PORT5_QUEUE_NUM;i++)
        fe_set_per_oq_rsv(5,i,0x40);

#if defined(TCSUPPORT_CPU_AN7583)
	/*7583 wifi to wan fastpath need to adjust port 5 oq4 reserver buffer to 0*/
		fe_set_per_oq_rsv(5,4,0);
	/*7583 P5 OQ5 need to change to 0x60 because of the qdma faq function*/
		fe_set_per_oq_rsv(5,5,0x60);
#endif

#if defined(TCSUPPORT_CPU_AN7583)
	/*rsv p6 oq3 for 7583*/
	for(i = 0;i < PSE_PORT6_QUEUE_NUM;i++)
	{
		if(3 == i)
			continue;
		fe_set_per_oq_rsv(6,i,0);
	}
#else
    /*rsv P6 OQ7 for npu bridge channel7,release other OQ*/
    for(i = 0;i < PSE_PORT6_QUEUE_NUM-1;i++)
        fe_set_per_oq_rsv(6,i,0);
#endif

#if !defined(TCSUPPORT_CPU_AN7583)
    /*rsv P7 OQ0~3 for npu wifi tx offload,reduce OQ4~9 to 0x40 */
    for(i = 4;i < PSE_PORT7_QUEUE_NUM;i++)
        fe_set_per_oq_rsv(7,i,0x40);
#endif

    /*reduce P9 to 0x40*/
    fe_set_per_oq_rsv(9,0,0x40);
    fe_set_per_oq_rsv(9,1,0x40);

#if !defined(TCSUPPORT_CPU_AN7583)
    /*reduce P10 to 0x40*/
    fe_set_per_oq_rsv(10,0,0x40);
    fe_set_per_oq_rsv(10,1,0x40);
#endif
#endif
    return;
}

static void fe_gdm_misc_init(void)
{
    /*7581 for pon idle setting*/
    #if defined(TCSUPPORT_CPU_EN7581)
    IO_SBITS(GDMA_MISC_CFG,GDMA2_CHN_VLD_MODE_BIT);
    IO_SBITS(GDMA_MISC_CFG,GDMA2_RDM_ACK_WAIT_PREF_BIT);
    #endif

    return;
}

static void fe_crsn_ipv6_fragment_init(void)
{
#if SUPPORT_FE_IPV6_EXT
	IO_SMASK(FE_IPV6_EXT, FE_IPV6_EXT_MASK, IP6_EXT0_SHIFT, 0X2C);
#endif
    return;
}

/* fe init setting */
/* [1:0], 0x0--qdma_lan payload size 2k, 0x1--qdma_lan 1k, 0x2--qdma_lan 512, 0x3--qdma_lan 256
** [5:4], 0x0--qdma_wan payload size 2k, 0x1--qdma_wan 1k, 0x2--qdma_wan 512, 0x3--qdma_wan 256
*/
static void fe_init_faq_aggregation(void)
{
	unchar flashQdmaInit = 0; 
	unsigned int qdmaLanPayloadMode=0;
	unsigned int qdmaWanPayloadMode=0;
	
#ifdef TCSUPPORT_CPU_ARMV8
	flashQdmaInit = get_qdmainit();
#else
	flashQdmaInit = READ_FLASH_BYTE(flash_base + 0xffb4);
#endif
	
	qdmaLanPayloadMode = (flashQdmaInit & 0x3);
	qdmaWanPayloadMode = (flashQdmaInit >> 4) & 0x3;
	/* CDM1 FAQ CFG*/
	switch (qdmaLanPayloadMode){
		case 0 :
			write_reg_word(CDM1_FAQ_CFG,0x07c6);
			write_reg_word(CDM1_FAQTHR_CFG,0xc40007f0);
			cdm1SetFaqEnable;
			break;
		case 1 :
			write_reg_word(CDM1_FAQ_CFG,0x07e6);
			write_reg_word(CDM1_FAQTHR_CFG,0xc40003f0);
			cdm1SetFaqEnable;
			break;
		case 2 :
			write_reg_word(CDM1_FAQ_CFG,0x07f6);
			write_reg_word(CDM1_FAQTHR_CFG,0xc40001f0);
			cdm1SetFaqEnable;
			break;
		case 3 :
			write_reg_word(CDM1_FAQ_CFG,0x07f6);
			write_reg_word(CDM1_FAQTHR_CFG,0xc40000f0);
			cdm1SetFaqEnable;
			break;
		default :
			break;
	}
	/* CDM2 FAQ CFG*/
	switch (qdmaWanPayloadMode){
		case 0 :
			write_reg_word(CDM2_FAQ_CFG,0x07c6);
			write_reg_word(CDM2_FAQTHR_CFG,0xc40007f0);
			cdm2SetFaqEnable;
			break;
		case 1 :
			write_reg_word(CDM2_FAQ_CFG,0x07e6);
			write_reg_word(CDM2_FAQTHR_CFG,0xc40003f0);
			cdm2SetFaqEnable;
			break;
		case 2 :
			write_reg_word(CDM2_FAQ_CFG,0x07f6);
			write_reg_word(CDM2_FAQTHR_CFG,0xc40001f0);
			cdm2SetFaqEnable;
			break;
		case 3 :
			write_reg_word(CDM2_FAQ_CFG,0x07f6);
			write_reg_word(CDM2_FAQTHR_CFG,0xc40000f0);
			cdm2SetFaqEnable;
			break;
		default :
			break;
	}
	return ;
}

static void enable_force_to_slow(void)
{
    unsigned int val_Hthd = 0, val_Lthd = 0;

    FeForceSlowEnable();
	FeReferOqBufEnable();

	FeSetFastDuty(1);
	FeSetSlowDuty(1);
	FeSetBufferCheckGap(32);
	
	val_Lthd = (read_reg_word(PSE_SHARE_BUF_STA) & 0x7fff)/3;
	val_Hthd = (read_reg_word(PSE_SHARE_BUF_STA) & 0x7fff)/3*2;
	FeSetShareUsedLthd(val_Lthd);
	FeSetShareUsedHthd(val_Hthd);

    return ;
}

static int __init fe_init(void)
{
    struct proc_dir_entry *fe_proc=NULL;
    unsigned int reg=0;
    unsigned int regVal=0;
    unsigned int mask=0;
    int ret = 0, idx = 0, index=0;
	struct list_head *head = NULL;
	size_t size = 0;
	struct ecnt_fe_data fe_data;
#ifdef TCSUPPORT_CPU_ARMV8
	struct device *dev = NULL;
#endif
    unsigned int intStatus=0 ;

	if(isEN7528)
	{
		disable_woe_mode();
		fe_use_gdmp_sram();
	}

	if(ecnt_register_hook(&ecnt_fe_api_op))
	{
		printk("ecnt_dev_fe_api_op register fail\n");
		return 0;
	}

	if(ARBITER_SUPPORT)
	{
		disable_xsi_mac();
	}

	//disable gsw port6
	disable_gsw_port6();
	
    fe_reset();
    fe_reg_setup();

	if(SUPPORT_PSE_PAGE_128)
	{
		pse_page_set_to_128Byte();
	}

	if(SUPPORT_PSE_USE_GDMP)
	{
		pse_set_total_page();
	}

	if(SUPPORT_PSE_SHARE_THRESHOLD)
	{
		pse_set_share_threshold();
	}

	if(SUPPORT_CDM_OQ_MAP)
	{
		fe_set_cdm_oq_map();
	}

	if(SUPPORT_VIP)
	{
		fe_vip_setup();
	}
	if(SUPPORT_PSE_FC_CFG)
	{
		/* enable tdma fc use pse share buffer */
		unsigned int val;
		val = read_reg_word(PSE_FC_CFG);
		val &= ~(0x1<<23);
		write_reg_word(PSE_FC_CFG, val);
	}

	if(SUPPORT_OQ_RSV_ADJUST)
	{		
		fe_oq_rsv_init();
	}

	if(SUPPORT_QDMA_FC_MAP)
	{
		fe_set_qdma_fc_map();
	}

	if(isEN7581)
	{
		fe_gdm_misc_init();
	}
	else if(isEN7580)
	{
		// set rx ring-1 & ring-15 come from PSE Port OQ-1, other ring come from PSE Port OQ-0;
		write_reg_word(FE_CDM_OQ_MAP, 0x80028002);
		FE_API_SET_DROP_UDP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
		FE_API_SET_DROP_UDP_CHKSUM_ERR_ENABLE(FE_GDM_SEL_GDMA1, FE_DISABLE);
    } 

#if defined(TCSUPPORT_CPU_AN7552)
    if(SUPPORT_CDM_HW_OQ_CONF)
    {
        /*CDM_HWF_OQ USE OQ5*/
        regVal = read_reg_word(FE_CDM_HWF_OQ);
        regVal &= (~CDM_HWF_OQ_MASK);
        regVal |= 5;
        write_reg_word(FE_CDM_HWF_OQ, regVal);
    }
    if(SUPPORT_DISABLE_UNUSE_OQ)
    {
        disable_unuse_pse_oq();
    }
#endif

	if(SUPPORT_SLOWPATH_OQ_ADJUST){
		/*7583 wifi to wan fastpath need to adjust slowpath to oq5, CDM_HWF_OQ USE OQ5*/
        regVal = read_reg_word(FE_CDM_HWF_OQ);
        regVal &= (~CDM_HWF_OQ_MASK);
        regVal |= 5;
        write_reg_word(FE_CDM_HWF_OQ, regVal);
	}

	//set rxbc 1000pps
#if defined(TCSUPPORT_CT_JOYME4) && defined(TCSUPPORT_CPU_EN7521)		
	write_reg_word(GDMA2_RXBC_SHPR_CFG, 0x800001ff);
#endif 

	tasklet_init(&dlfDetectTask, fe_detect_task, 0);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	init_timer(&fe_timer) ;
	fe_timer.data = fe_expire_time; /* 1 s */
  	fe_timer.expires = jiffies + ((fe_expire_time*HZ)/1000);
	fe_timer.function = fe_timer_expires ;
#else
	timer_setup(&fe_timer, fe_timer_expires, 0);
	fe_timer.expires = jiffies + ((fe_expire_time*HZ)/1000);
#endif
  	add_timer(&fe_timer);
    
#if defined(TCSUPPORT_CPU_EN7580)
    /*Set oam rx ring 15*/
    regVal = read_reg_word(CDMA2_FWD_CFG);
    regVal &= ~(OAM_QSEL_MASK << OAM_QSEL_OFFSET);
    regVal |= ((RING_IDX_15 & OAM_QSEL_MASK) << OAM_QSEL_OFFSET);
    write_reg_word(CDMA2_FWD_CFG, regVal);
#endif

    if(FRAG_PKT_CKS_ERROR_WORKROUND)
    {
        regVal = read_reg_word(GDMA1_FWD_CFG);
        regVal &= ~(1<<GDMA1_FWD_CFG_UDP_OFFSET);
        regVal &= ~(1<<GDMA1_FWD_CFG_TCP_OFFSET);
        write_reg_word(GDMA1_FWD_CFG, regVal);

        regVal = read_reg_word(GDMA2_FWD_CFG);
        regVal &= ~(1<<GDMA1_FWD_CFG_UDP_OFFSET);
        regVal &= ~(1<<GDMA1_FWD_CFG_TCP_OFFSET);
        write_reg_word(GDMA2_FWD_CFG, regVal);

        regVal = read_reg_word(GDMA3_FWD_CFG);
        regVal &= ~(1<<GDMA1_FWD_CFG_UDP_OFFSET);
        regVal &= ~(1<<GDMA1_FWD_CFG_TCP_OFFSET);
        write_reg_word(GDMA3_FWD_CFG, regVal);
    }

	if(SUPPORT_FRAG_ASSEM)
	{
		/* Init Fragment & Assemble Force Port */
		feSetFragmentForcePort(6, FRAG_FP_NBQ); /* NPU Core-3, NPU Bridge Channel-3 */
		feSetAssembleForcePort(0, ASSEM_FP_NBQ); /* QDMA LAN, RX Ring-22 */
	}

	feSetGdma2PadEnable();
	feSetGdma3Gdma4PadEnable(); /* Enable padding before sending to HSGMII, otherwise it may cause delay */

#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7523)
	/* set rx queue for lan->wifi traffic to Q1 */
	/* CDMA1_CRSN_QSEL */
	cdm1SetSpecCrsnToCpuRing(CRSN_22, CDMA_CRSN_QSEL_Q1);

	cdm1SetSpecCrsnToCpuRing(CRSN_08, CDMA_CRSN_QSEL_Q1);
	cdm1SetSpecCrsnToCpuRing(CRSN_21, CDMA_CRSN_QSEL_Q1);
	#if defined(TCSUPPORT_NPU_WIFI_OFFLOAD)
	/*once defined qdma lan int4 will be stop,and ring6 is bind to int4*/
	cdm1SetSpecCrsnToCpuRing(CRSN_24, CDMA_CRSN_QSEL_Q5);
	#else
	cdm1SetSpecCrsnToCpuRing(CRSN_24, CDMA_CRSN_QSEL_Q6);
	#endif
	cdm1SetSpecCrsnToCpuRing(CRSN_25, CDMA_CRSN_QSEL_Q1);

	cdm2SetSpecCrsnToCpuRing(CRSN_08, CDMA_CRSN_QSEL_Q1);
	cdm2SetSpecCrsnToCpuRing(CRSN_21, CDMA_CRSN_QSEL_Q1);
	cdm2SetSpecCrsnToCpuRing(CRSN_22, CDMA_CRSN_QSEL_Q1);
	cdm2SetSpecCrsnToCpuRing(CRSN_24, CDMA_CRSN_QSEL_Q6);
	cdm2SetSpecCrsnToCpuRing(CRSN_25, CDMA_CRSN_QSEL_Q1);
#else
	/* set rx queue for lan->wifi traffic to Q1L */
	/* CDMA1_CRSN_QSEL */
	regVal = read_reg_word(CDMA1_CRSN_QSEL1);
	regVal &= ~(CDMA1_CRSN_QSEL_CPU_REASON_22_MASK); //crsn:0x16
	regVal |= (CDMA1_CRSN_QSEL_Q1L<<CDMA1_CRSN_QSEL_CPU_REASON_22_OFFSET);
	write_reg_word(CDMA1_CRSN_QSEL1, regVal);
#endif

#ifdef TCSUPPORT_FTTDP_V2
/* let FE CPORT add 4-byte CRC for packets from GSW to FE,
 * so that, packets from PTM_LAN or GSW can both have CRC,
 * which will be stripped at GDM1. */
    reg = read_reg_word(FE_CPORT_CFG);
    reg &= ~(1<<30);
    write_reg_word(FE_CPORT_CFG, reg);

/* disable STAG indication for FE, because packets from GSW
 * and PTM_LAN should both don't have STAG */
    reg = read_reg_word(GDMA1_VLAN_CHECK);
    reg &= ~(1<<0);
    write_reg_word(GDMA1_VLAN_CHECK, reg);

/* set LAN as Arbitor mode, so that PTM and GSW can work together, 
 * set WAN as GPON mode */
    #ifdef TCSUPPORT_CPU_ARMV8
    reg = GET_WAN_CONF();
    reg &= ~((0x7<<0)|(0x3<<4));
    reg |= ((0<<0)|(0x3<<4));
    SET_WAN_CONF(reg);
    #else
    reg = read_reg_word(0xbfb00070);
    reg &= ~((0x7<<0)|(0x3<<4));
    reg |= (0<<0)|(0x3<<4);
    write_reg_word(0xbfb00070, reg);
    #endif
#endif
    reg = read_reg_word(FE_CPORT_CFG);
    reg = (reg & (~(0x3<<24))) | (0x2 <<24);
    write_reg_word(FE_CPORT_CFG, reg);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    init_timer(&fe_cnt_timer) ;
    fe_cnt_timer.expires = jiffies + msecs_to_jiffies(1000) ; /* 1000 ms */
    fe_cnt_timer.function = fe_cnt_timer_expires ;
#else
	timer_setup(&fe_cnt_timer,fe_cnt_timer_expires,0);
	fe_cnt_timer.expires = jiffies + msecs_to_jiffies(1000) ; /* 1000 ms */
#endif
    add_timer(&fe_cnt_timer);

    for(index=0; index<ANI_UNI_PORT_MAX; index++) {
        tx_ok_cnt[index] = (unsigned int *)kzalloc(sizeof(unsigned int)*FE_CNT_SECONDS, GFP_KERNEL);
        tx_ok_byte_cnt[index] = (unsigned int *)kzalloc(sizeof(unsigned int)*FE_CNT_SECONDS, GFP_KERNEL);
        rx_ok_cnt[index] = (unsigned int *)kzalloc(sizeof(unsigned int)*FE_CNT_SECONDS, GFP_KERNEL);
        rx_ok_byte_cnt[index] = (unsigned int *)kzalloc(sizeof(unsigned int)*FE_CNT_SECONDS, GFP_KERNEL);

		rx_discard_cnt[index] = (unsigned int *)kzalloc(sizeof(unsigned int)*FE_CNT_SECONDS, GFP_KERNEL);
		tx_discard_cnt[index] = (unsigned int *)kzalloc(sizeof(unsigned int)*FE_CNT_SECONDS, GFP_KERNEL);
		rx_error_cnt[index] = (unsigned int *)kzalloc(sizeof(unsigned int)*FE_CNT_SECONDS, GFP_KERNEL);
		tx_error_cnt[index] = (unsigned int *)kzalloc(sizeof(unsigned int)*FE_CNT_SECONDS, GFP_KERNEL);
    }

    ret = register_chrdev(FE_MAJOR, FE_MAJOR_NAME, &fe_fops);	
    if (ret < 0) {
        printk(KERN_WARNING "pcie: can't get major %d\n", FE_MAJOR);
        return ret;
    }

    /* Register fe interrupt */
#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7523)
    /* default aging mode for mbi unlock issue */
    mbi_hang_unlock_by_aging(FE_GDM_SEL_GDMA2, 3);

    /* init FE Interrupt Mask */
    mask |= INT_STATUS_PSE_FQ_EMPTY ;
    mask |= INT_STATUS_TX_MBI | INT_STATUS_RX_MBI ;
    mask |= INT_GDM2_RX_2_GEMPORT_ISSUE | INT_GDM2_RX_FIFO_OVERFLOW ;
    if(SUPPORT_DETECT_PSE_EXCEPTION){
        mask |= INT_STATUS_PSE_EXCEPTION ;
    }
    FeSetIntMask(mask);
	
    if(SUPPORT_DETECT_PSE_EXCEPTION){
        intStatus = FeGetIntStatus() & FeGetIntMask() ;
        /* clear int */
        if(intStatus & INT_STATUS_PSE_EXCEPTION) 
            FeSetIntStatus(INT_STATUS_PSE_EXCEPTION) ;
    }
		
#ifdef TCSUPPORT_CPU_ARMV8
	if( (dev=get_frame_engine_dev()) == NULL ) {
		printk("Get device failed.\n") ; 
		return 0 ;
	}
	if(request_irq(get_fe_irq(), fe_isr, 0, fe_isr_name, dev) != 0) {
		printk("Request the FE interrupt service routine fail, irq:%d.\n", get_fe_irq()) ;
		return -ENODEV ;
	}
#else
    if(request_irq(FE_ERR_INTR, fe_isr, 0, fe_isr_name, NULL) != 0) {
        printk("Request the FE interrupt service routine fail, irq:%d.\n", FE_ERR_INTR) ;
        return -ENODEV ;
    }
#endif
    
    fe_proc = create_proc_entry("tc3162/mbi_unlock", 0, NULL) ;
    if(fe_proc) {
        fe_proc->read_proc = mbi_unlock_read_proc ;
        fe_proc->write_proc = mbi_unlock_write_proc ;
    }
    create_proc_read_entry("tc3162/fe_oq_cnt", 0, NULL, fe_oq_cnt_dump_proc, NULL);

#if defined(TCSUPPORT_CPU_EN7523)
    create_proc_read_entry("tc3162/fe_oq_cnt_max", 0, NULL, fe_oq_cnt_max_dump_proc, NULL);
#if defined(TCSUPPORT_CPU_AN7552)
    if(SUPPORT_DISABLE_UNUSE_OQ)
    {
        create_proc_read_entry("tc3162/fe_oq_use", 0, NULL, fe_pse_oq_en_dump_proc, NULL);
    }
#endif
#endif
#endif

	#ifdef TCSUPPORT_CPU_EN7523
	fe_proc = create_proc_entry("tc3162/gdm3_debug_reg", 0, NULL) ;
    if(fe_proc) {
        fe_proc->read_proc = fe_gdm3_read_proc ;
        fe_proc->write_proc = fe_gdm3_write_proc ;
    }

	
	fe_proc = create_proc_entry("tc3162/gdm_sptag", 0, NULL) ;
    if(fe_proc) {
        fe_proc->read_proc = fe_gdm_sptag_read_proc ;
        fe_proc->write_proc = fe_gdm_sptag_write_proc ;
    }
	#endif

	create_proc_read_entry("tc3162/fe_reg", 0, NULL, fe_reg_dump_proc, NULL);
    create_proc_read_entry("tc3162/fe_debug_reg", 0, NULL, fe_debug_reg_dump_proc, NULL);
	fe_proc = create_proc_entry("tc3162/channel_retire", 0, NULL);
	fe_proc->read_proc = channel_retire_read_proc;
	fe_proc->write_proc = channel_retire_write_proc;

	fe_proc = create_proc_entry("tc3162/cwmp_port_config", 0, NULL) ;
	if(fe_proc) {
		fe_proc->read_proc = cwmp_port_config_read_proc ;
		fe_proc->write_proc = cwmp_port_config_write_proc ;
	}

	#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
	fe_proc = create_proc_entry("tc3162/fe_ratelimit", 0, NULL);
	if(fe_proc) 
	{
		fe_proc->read_proc = fe_ratelimit_read_proc;
		fe_proc->write_proc = fe_ratelimit_write_proc;
	}
	#endif

	if(SUPPORT_TUNNEL_OFFLOAD)
	{
		fe_proc = create_proc_entry("tc3162/fe_tunneltable", 0, NULL);
		if(fe_proc)
		{
			fe_proc->read_proc = fe_tunnel_table_read_proc;
			fe_proc->write_proc = fe_tunnel_table_write_proc;
		}
	}

	if(SUPPORT_FE_SRAM_POWER_DOWN)
	{
		fe_proc = create_proc_entry("tc3162/fe_sram_pd", 0, NULL);
		if(fe_proc)
		{
			fe_proc->read_proc = fe_power_down_read_proc;
			fe_proc->write_proc = fe_power_down_write_proc;
		}
	}

	fe_proc = create_proc_entry("tc3162/fe_reset", 0, NULL);
	if(fe_proc) 
	{
		fe_proc->read_proc = fe_reset_read_proc;
		fe_proc->write_proc = fe_reset_write_proc;
	}

	#if defined(TCSUPPORT_CPU_EN7581)
	fe_proc = create_proc_entry("tc3162/fe_oq_rsv", 0, NULL);
	if(fe_proc)
	{
		fe_proc->read_proc = fe_oq_rsv_read_proc;
		fe_proc->write_proc = fe_oq_rsv_write_proc;
	} 
	#endif
	
	#ifdef TCSUPPORT_CSC_EEUROPE
    fe_proc = create_proc_entry("tc3162/tls_enable", 0, NULL);
    if(fe_proc)
    {
        fe_proc->read_proc = fe_tls_enable_read_proc;
        fe_proc->write_proc = fe_tls_enable_write_proc;
    }

    fe_proc = create_proc_entry("tc3162/tls_debug_on", 0, NULL);
    if(fe_proc)
    {
        fe_proc->read_proc = fe_tls_debug_read_proc;
        fe_proc->write_proc = fe_tls_debug_write_proc;
    }

    fe_tls_entry_list_init();
	#endif

	fe_tls_tag_vlan_list_init();

#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7523)
	/* Disable IFC first. If IFC ko is insmod, IFC will be enabled */
	IFCDisable;
#endif

	/* Enable 1:N vlan action, init vlan table */
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_EN7581)
	feEnableMcVlan;

	memset(&fe_data, 0, sizeof(struct ecnt_fe_data));	
	fe_data.function_id = FE_SET_MC_VLAN_CLEAR_ALL;
	fe_data.api_data.fe_mcvlan_table_cfg.mc_table_id = FE_MC_VLAN_TABLE_MAX;
	fe_api_set_mc_vlan_clear_all(&fe_data);
#endif

    spin_lock_init(&fe_pse_reset_lock);

    if(isEN751221)
        tasklet_init(&fe_pse_reset_task, fe_pse_reset, 0);
    else if(isEN7528)
        tasklet_init(&fe_pse_reset_task, fe_do_core_reset_with_qdma, 0);
	else if(SUPPORT_GDM2_IQ_MONITOR)
        tasklet_init(&fe_pse_reset_task, fe_core_soft_reset, 0);

    if (isEN751221 || isEN7528 || SUPPORT_GDM2_IQ_MONITOR)
    {
        fe_timer_init();

        fe_proc = create_proc_entry("tc3162/fe_pse_reset_info", 0, NULL);
        if (fe_proc)
        {
            fe_proc->read_proc = fe_pse_reset_info_read;
        }


        fe_proc = create_proc_entry("tc3162/fe_pse_reset_threshold", 0, NULL);
        if (fe_proc)
        {
            fe_proc->read_proc = fe_pse_reset_threshold_read;
            fe_proc->write_proc = fe_pse_reset_threshold_write;
        }

        fe_proc = create_proc_entry("tc3162/fe_pse_reset_action", 0, NULL);
        if (fe_proc)
        {
            fe_proc->read_proc = fe_pse_reset_action_read;
            fe_proc->write_proc = fe_pse_reset_action_write;
        }

        fe_proc = create_proc_entry("tc3162/fe_pse_reset_test", 0, NULL);
        if (fe_proc)
        {
            fe_proc->read_proc = fe_pse_reset_test_read;
            fe_proc->write_proc = fe_pse_reset_test_write;
        }
    }
	if (SUPPORT_GDM2_IQ_MONITOR){	
        fe_proc = create_proc_entry("tc3162/fe_pse_gdma2_iq_state", 0, NULL);
        if (fe_proc)
        {
            fe_proc->read_proc = fe_pse_gdma2_iq_state_read;
            fe_proc->write_proc = fe_pse_gdma2_iq_state_write;
        }
    }
	
    if(SUPPORT_DETECT_PSE_EXCEPTION){
        fe_proc = create_proc_entry("tc3162/fe_pse_soft_core_reset", 0, NULL);
        if (fe_proc)
        {
            fe_proc->read_proc = fe_pse_soft_core_reset_read;
            fe_proc->write_proc = fe_pse_soft_core_reset_write;
        }
    }
    
#if defined(TCSUPPORT_CPU_EN7521) || defined(TCSUPPORT_CPU_EN7512)
	fe_proc = create_proc_entry("tc3162/dyn_qdma_txq_cngst_threshold", 0, NULL);
	fe_proc->read_proc = dyn_qdma_txq_cngst_threshold_read_proc;
	fe_proc->write_proc = dyn_qdma_txq_cngst_threshold_write_proc;
#endif
    
	fe_proc = create_proc_entry("tc3162/wan_itf_and_dev_bandwidth_mode", 0, NULL) ;
	if(fe_proc) {
		fe_proc->read_proc = wan_itf_and_dev_bandwidth_mode_read_proc ;
		fe_proc->write_proc = wan_itf_and_dev_bandwidth_mode_write_proc ;
	}
    
	fe_proc = create_proc_entry("tc3162/wan_itf_and_dev_bandwidth_timeout", 0, NULL) ;
	if(fe_proc) {
		fe_proc->read_proc = wan_itf_and_dev_bandwidth_timeout_read_proc ;
		fe_proc->write_proc = wan_itf_and_dev_bandwidth_timeout_write_proc ;
	}
    
	fe_proc = create_proc_entry("tc3162/wan_itf_and_dev_bandwidth_max_num", 0, NULL) ;
	if(fe_proc) {
		fe_proc->read_proc = wan_itf_and_dev_bandwidth_max_num_read_proc ;
		fe_proc->write_proc = wan_itf_and_dev_bandwidth_max_num_write_proc ;
	}
    
	fe_proc = create_proc_entry("tc3162/wan_itf_and_dev_bandwidth_total_account", 0, NULL) ;
	if(fe_proc) {
		fe_proc->read_proc = wan_itf_and_dev_bandwidth_total_account_read_proc ;
		fe_proc->write_proc = wan_itf_and_dev_bandwidth_total_account_write_proc ;
	}
    
	fe_proc = create_proc_entry("tc3162/stb_src_ip", 0, NULL) ;
	if(fe_proc) {
		fe_proc->read_proc = stb_src_ip_read_proc ;
		fe_proc->write_proc = stb_src_ip_write_proc ;
	}
	fe_proc = create_proc_entry("tc3162/fe_dyn_qdma_wan_txq_cngst", 0, NULL) ;
	if(fe_proc) {
		fe_proc->read_proc = fe_dyn_qdma_wan_txq_cngst_read_proc ;
		fe_proc->write_proc = fe_dyn_qdma_wan_txq_cngst_write_proc ;
	}

	fe_proc = create_proc_entry("tc3162/fe_set_vr_wan", 0, NULL) ;
	if(fe_proc) {
		fe_proc->read_proc = fe_set_vr_wan_read_proc ;
		fe_proc->write_proc = fe_set_vr_wan_write_proc ;
	}
	if(SUPPORT_FAQ_AGGRE) {
		fe_proc = create_proc_entry("tc3162/fe_faq_cfg", 0, NULL) ;
		if(fe_proc) {
			fe_proc->write_proc = fe_faq_cfg_write_proc ;
		}
	}  
	    
	for(idx = 0; idx < WAN_ITF_ACNT_NUM; idx ++){
		memset(&wan_itf_info[idx], 0x0, sizeof(Wan_itf_name));
		wan_itf_info[idx].wanIndex = -1;
	}

	for(idx = 0; idx < 16; idx ++){
        memset(&wan_virtualdev_ifname_info[idx], 0x0, sizeof(wan_virtualdev_ifname));
    }
    
	memset(acntGrpGeneralTable, 0x0, sizeof(acntGrpGeneralTable));
	memset(acntGrpWanItfTable, 0x0, sizeof(acntGrpWanItfTable));
	memset(acntGrpMulticastTable, 0x0, sizeof(acntGrpMulticastTable));
    
#if defined(CONFIG_BRIDGE_IGMP_SNOOPING) && defined(TCSUPPORT_IGMP_SNOOPING)
	size = 128 * sizeof(struct __mc_fdb_entry);
	multicast_buf = kmalloc(size, GFP_KERNEL);
	if (!multicast_buf)
		return -ENOMEM;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	init_timer(&fe_account_timer);
	fe_account_timer.expires = jiffies + msecs_to_jiffies(1000);
	fe_account_timer.function = fe_account_timer_expires;
	fe_account_timer.data = 0;
#else
	timer_setup(&fe_account_timer,fe_account_timer_expires,0);
	fe_account_timer.expires = jiffies + msecs_to_jiffies(1000);
#endif
	add_timer(&fe_account_timer);
    
	total_account_list = kzalloc(sizeof(wan_itf_or_dev_bandwidth_total_account_t), GFP_KERNEL);
	if(total_account_list == NULL){	
		printk(KERN_WARNING "fe: allocate memory for total_account_list fail\n");
		return -ENOMEM;
	}
	head = &total_account_list->total_account_node;
	INIT_LIST_HEAD(head);
    
	stb_src_ip_list = kzalloc(sizeof(stb_src_ip_t), GFP_KERNEL);
	if(stb_src_ip_list == NULL){	
		printk(KERN_WARNING "fe: allocate memory for stb_src_ip_list fail\n");
		return -ENOMEM;
	}
	head = &stb_src_ip_list->stb_src_ip_node;
	INIT_LIST_HEAD(head);

	register_netdevice_notifier(&fe_res_notifier_block);
  	rcu_assign_pointer(get_wan_index_info_hook, get_wan_index_info);
  	rcu_assign_pointer(multicast_get_stb_src_ip4, get_stb_src_ip4);
  	rcu_assign_pointer(multicast_get_stb_src_ip6, get_stb_src_ip6);


	if(SUPPORT_QDMA_3S_ACCOUNT_METER)
		fe_account_meter_proc_init();
	

#if SUPPORT_FE_IPV6_EXT
	fe_crsn_ipv6_fragment_init();
#endif

	if(SUPPORT_FAQ_AGGRE){		
		fe_init_faq_aggregation();	
	}
	
	if (get_hybrid_mode() == 1)
	{
		fe_data.api_data.enable = 1;
		fe_api_set_force_slow_enable(&fe_data);
	}
	
	return 0;
}

static void __exit fe_exit(void) 
{
	int index=0;
	#ifdef TCSUPPORT_CPU_ARMV8
	struct device *dev = NULL;
	#endif

	del_timer_sync(&fe_cnt_timer) ;
    
	for(index=0; index<ANI_UNI_PORT_MAX; index++) {
		kfree(tx_ok_cnt[index]);
		kfree(tx_ok_byte_cnt[index]);
		kfree(rx_ok_cnt[index]);
		kfree(rx_ok_byte_cnt[index]);

		kfree(rx_discard_cnt[index]);
		kfree(tx_discard_cnt[index]);
		kfree(rx_error_cnt[index]);
		kfree(tx_error_cnt[index]);
	}
	
	FE_STOP_TIMER(fe_timer) ;
	tasklet_kill(&dlfDetectTask);
	remove_proc_entry("tc3162/fe_reg", 0);
	remove_proc_entry("tc3162/fe_debug_reg",0);
	remove_proc_entry("tc3162/channel_retire",0);
	remove_proc_entry("tc3162/cwmp_port_config", 0);
	remove_proc_entry("tc3162/wan_itf_and_dev_bandwidth_mode", 0);
	remove_proc_entry("tc3162/wan_itf_and_dev_bandwidth_timeout", 0);
	remove_proc_entry("tc3162/wan_itf_and_dev_bandwidth_max_num", 0);
	remove_proc_entry("tc3162/wan_itf_and_dev_bandwidth_total_account", 0);
	remove_proc_entry("tc3162/stb_src_ip", 0);
	remove_proc_entry("tc3162/fe_reset", 0);
	if(SUPPORT_TUNNEL_OFFLOAD)
	{
		remove_proc_entry("tc3162/fe_tunneltable", 0);
	}

	if(SUPPORT_FE_SRAM_POWER_DOWN)
	{
		remove_proc_entry("tc3162/fe_sram_pd", 0);
	}

	#if defined(TCSUPPORT_CPU_EN7581)
	remove_proc_entry("tc3162/fe_oq_rsv", 0);
	#endif

	#ifdef TCSUPPORT_CSC_EEUROPE
	remove_proc_entry("tc3162/tls_enable",0);
	remove_proc_entry("tc3162/tls_debug_on", 0);
	#endif
	
#if defined(TCSUPPORT_CPU_EN7580)
	remove_proc_entry("tc3162/mbi_unlock", NULL); 
	remove_proc_entry("tc3162/fe_oq_cnt", NULL); 

#ifdef TCSUPPORT_CPU_EN7523
#if defined(TCSUPPORT_CPU_AN7552)
    if(SUPPORT_DISABLE_UNUSE_OQ)
    {
        remove_proc_entry("tc3162/fe_oq_use", NULL); 
    }
#endif    
    remove_proc_entry("tc3162/fe_oq_cnt_max", NULL); 
#endif
	
#ifdef TCSUPPORT_CPU_ARMV8
	if( (dev=get_frame_engine_dev()) == NULL ) {
		printk("Get device failed.\n") ; 
		return ;
	}
	free_irq(get_fe_irq(), dev) ;
#else
	free_irq(FE_ERR_INTR, NULL) ;
#endif
#endif

#ifdef TCSUPPORT_CPU_EN7523
	remove_proc_entry("tc3162/gdm3_debug_reg", NULL); 
	remove_proc_entry("tc3162/gdm_sptag", NULL); 

#endif


#if defined(TCSUPPORT_CPU_EN7521) || defined(TCSUPPORT_CPU_EN7512)
	remove_proc_entry("tc3162/dyn_qdma_txq_cngst_threshold", 0);
#endif

	unregister_chrdev(FE_MAJOR, FE_MAJOR_NAME);
	ecnt_unregister_hook(&ecnt_fe_api_op);

    if (isEN751221 || isEN7528 || SUPPORT_GDM2_IQ_MONITOR)
    {
        fe_timer_deinit();

        remove_proc_entry("tc3162/fe_pse_reset_info", 0);
        remove_proc_entry("tc3162/fe_pse_reset_threshold", 0);
        remove_proc_entry("tc3162/fe_pse_reset_action", 0);
        remove_proc_entry("tc3162/fe_pse_reset_test", 0);
    }
	if(SUPPORT_GDM2_IQ_MONITOR){
    	remove_proc_entry("tc3162/fe_pse_gdma2_iq_state", 0);
	}
	if(SUPPORT_DETECT_PSE_EXCEPTION){
		remove_proc_entry("tc3162/fe_pse_soft_core_reset", 0);
	}
	remove_proc_entry("tc3162/fe_dyn_qdma_wan_txq_cngst", 0);  
	remove_proc_entry("tc3162/fe_set_vr_wan", 0);

	if(SUPPORT_QDMA_3S_ACCOUNT_METER)
		fe_account_meter_proc_exit();
	
	if(SUPPORT_FAQ_AGGRE) {		
		remove_proc_entry("tc3162/fe_faq_cfg", 0);	
	}  
		
#if defined(CONFIG_BRIDGE_IGMP_SNOOPING) && defined(TCSUPPORT_IGMP_SNOOPING)
	kfree(multicast_buf);
#endif

	del_timer(&fe_account_timer);
	free_all_account_nodes();
	kfree(total_account_list);
	free_all_stb_src_ip_nodes();
	kfree(stb_src_ip_list);
    
	unregister_netdevice_notifier(&fe_res_notifier_block);
  	rcu_assign_pointer(get_wan_index_info_hook, NULL);
  	rcu_assign_pointer(multicast_get_stb_src_ip4, NULL);
  	rcu_assign_pointer(multicast_get_stb_src_ip6, NULL);
	return;
}


/* Register startup/shutdown routines */
module_init(fe_init);
module_exit(fe_exit);
MODULE_LICENSE("Proprietary");

