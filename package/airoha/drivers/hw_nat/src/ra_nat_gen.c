 /************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/spinlock.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/libcompileoption.h>
#include <linux/if_ether.h>
#include <linux/if_pppox.h>
#include <linux/jhash.h>
#include <linux/inetdevice.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>

#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_multicast_general.h>
#include <ecnt_hook/ecnt_hook_ifc.h>

#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>

#include "ra_nat_gen.h"
#include "ra_nat_cust.h"
#include "ra_nat_offload.h"
#include "ra_nat_dev.h"
#include "ra_nat_ety.h"
#include "ra_nat_ic_dis.h"

#include "util.h"
#include "foe_fdb.h"
#include "ppe_api.h"

#include "../net/bridge/br_private.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
static struct list_head BlackIpTable[1024];
static struct list_head BlackIpTableRange;
static struct list_head BlackIpPortProtocolTable;

#define BLACKIP_UP	0
#define BLACKIP_DOWN	1
#define BLACKIP_UP_DOWN	2

unsigned char snoopingSpeed_en = 0;

static struct timer_list gdm2_update_timer;

ppeAcntGrp_t ppe_acnt_grp[ACNT_GRP_WAN_NUM];

PPE_METER_ACNT_T ppe_meter_acnt_res;

u32 lanTcpBlackListBit = 0;

struct FoeEntryExt*  foe_ext = NULL;
#ifdef CONFIG_TP_IMAGE
struct FoeEntryIntf *foe_intf = NULL;
struct hwnat_entries_args *foe_args = NULL;
unsigned int foe_args_size = 0;
#endif /* CONFIG_TP_IMAGE */

uint8 ptm_etherwan_coexist = 0;
int etherwan_use_qdma = 0;
int use_etherwan = 0;

int gMeterGroupSupport = 1;

int udp_bypass_enable = 0;

unsigned char natv6_2544_check_flag = 0;
unsigned char natv6_2544_ongoing_flag = 0;
unsigned int natv6_2544_pkt_cnt_last = 0;
unsigned int natv6_2544_pkt_cnt_new = 0;
unsigned int natv6_2544_cur_rate = 0;
unsigned int natv6_2544_trigger_rate = 8000; /* default: 8000 pps */
unsigned int natv6_2544_timer_cnt = 0;
unsigned int natv6_2544_hold_time = 90*60;  /* default: 1.5 h */

DEFINE_SPINLOCK(black_ip_lock);
DEFINE_SPINLOCK(black_ip_port_protocol_lock);

extern int wifiqueue_start_idx;

unsigned int wifi_rate[SINGLE_WLAN_SSID_NUM*4];

/* hwnat_stag_7512autotest is for EN7512 PPE AutoTest testing purpose!
 * EN7512 PPE AutoTest was developed baesd on MT7510 PPE AutoTest.
 * Because EN7512 and MT7510 have different GSW stag mechanism,
 * we need to do modifications, so that EN7512 PPE AutoTest can
 * run transparently without any Script modifcation by QA. */
unsigned int hwnat_stag_7512autotest = 0;

u8 sw_port_offload_mask = 0;

#define MULTOCPU 1
#define FORCETOCPU 0

int32_t isSpecialTag(uint16_t eth_type);

struct offload_info *offload_info_entry = NULL;
struct ppeTlsResource tlsRsr;
struct ppeAccountResource accountRsr;

#define atoi(x)         simple_strtol(x, NULL,10)

unsigned char test_flag= 0;
unsigned char swnat_foe_clean_flag = 0;
unsigned int tun_offset = 0;

static L2LU_key_table_s hwnat_key_table[15];
static int l2lu_delete_period = 20;       //20s
int is_l2lu_pkt_exist = 0;
static uint16 ppe_l2lu_key_mask = ( 0 );
DEFINE_SPINLOCK(l2lu_lock);

HWNAT_RESOURCE_ALLOC_t hwnat_resource_poll[HWNAT_RESOURCE_TYPE_MAX] =
{
	{0,HWNAT_RESOURCE_METER_NUM},
	{0,HWNAT_RESOURCE_ACCOUNT1},
	{0,HWNAT_RESOURCE_ACCOUNT2},
};

int xpon_is_multicast_entry(struct FoeEntry *foe_entry);
void PpeGetFlowAcntInfo(struct sk_buff *skb, uint8_t wan_index, uint8_t dir);

void ppeMeterBindInfoHandler(struct sk_buff * skb,struct port_info * pinfo, int magic,u_int16_t *mGrpId);

int PpeL2luKeyUpdateBindTime(struct sk_buff * skb);
int PpeL2luKeySetStatusBind(int PpeIndex);
void PpeL2LUKeyHwTblSet(uint8_t index,L2LU_key_table_s key_entry);

static DEFINE_RWLOCK(fast_rule_lock);
static int hwnat_fast_rule_enable = 0;
static int hwnat_fast_rule_mark_enable = 1;
static int hwnat_fast_rule_reverse_enable = 1;
static int hwnat_fast_rule_mark_bit = -1;

#define PPE_FAST_RULE_AGE_TIME 10*HZ

struct hwnat_fast_rule_list_t
{
	struct list_head list;
	unsigned int foe_index;
	unsigned char ip_proto;
	unsigned int sip;
	unsigned int dip;
	unsigned short sport;
	unsigned short dport;	
	struct timer_list age_timer;
};

static struct list_head fast_rule_index_list;

STATIC_RULE_EXT_t static_rule_ext;

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern struct spinlock hw_nat_lock;
extern int g_wan_mode;

extern uint hwnat_fast_bind;
extern uint hwnat_fast_bind_hash_conflict;

extern int internet_hwnat_timer_switch;

extern portInfo lanPortState[5];
extern uint32 dsoffload_presetting;

extern int dualband_interrupt_enable;
extern unsigned int hwnat_dram_idx_base;

#ifdef TR068_LED
extern int internet_led_on;
extern int internet_trying_led_on;
#endif

extern int PpeGetInfoBlk2(struct FoeEntry * foe_entry, _info_blk2 * iblk2);

extern int use_soc_lan;
extern int isolation_enable;

extern int internet_hwnat_pktnum;

extern int (*soft_ratelimit_enqueue_hook) (struct sk_buff * skb,unsigned int queue_idx);

extern int (*soft_ratelimit_set_queue_hook) (unsigned int queue,unsigned int rate,void* func);
extern int (*soft_ratelimit_allocatequeue_hook)( unsigned int num);
extern int (*soft_ratelimit_recyclequeue_hook)( unsigned int start,unsigned num);
extern int (*fe_resource_mark_wan_idx_hook)( struct sk_buff *skb, u8 wan_index, u8 dir);

extern int (*wifi_eth_fast_tx_hook)(struct sk_buff *skb);

extern int (*hwnat_multicast_data_hook)(struct sk_buff *skb);

extern int ppe_is_multicast_data_pack(struct sk_buff* skb);

extern int ppe_trans_tpid_to_vpm(uint16_t eth_type);

extern int (*hwnat_multicast_set_hwnat_info_hook)(int index, unsigned long mask, short int bindvid);
extern int  (*hwnat_set_rule_according_to_state_hook)(int index, int state,unsigned long mask);

extern int (*multicast_speed_learn_flow_hook)(struct sk_buff* skb);
extern int (*hwnat_multicast_set_info_for_sfu_hook)(int index, int tag);

extern multicast_flood_hwentry_t*  (*multicast_flood_find_entry_hook)(int index);
extern IGMP_HWNATEntry_t* (*multicast_speed_find_entry_hook)(int index);

extern int DebugLevel;

extern int timeOutVal;	//30s

extern unsigned char inter_switch_mask;
extern unsigned char extend_switch_mask;
extern char lan_port_map[];
extern char extend_switch_port;

extern int is_hwnat_led_enable;

extern struct net_device	*DstPort[MAX_IF_NUM];

extern PktParseResult		PpeParseResult;

extern unsigned char ratelimitByItf_en;
extern unsigned char ratelimitByIP_en;

extern unsigned char natv6_411_test_flag;
extern unsigned int natv6_411_timer;
extern int l2tp_session_hash_enable;

extern unsigned char ppe_clean_table_flag;
extern unsigned int ppe_clean_table_timer;
extern void set_foe_SMA_mode(unsigned int mode);
//static struct timer_list test_timer;
extern unsigned char (*get_swnat_clean_flag_hook)(void);
extern void (*set_swnat_clean_flag_hook)(unsigned char val);
extern int (*sw_threshold_get_hook)(int *min, int *max);

extern void (*get_wan_index_info_hook)(char* wan_index);

extern void PpeSetInfoBlk2(struct FoeEntry * foe_entry, uint32_t fpidx, uint32_t port_mg, uint32_t port_ag, uint32_t ds_to_qdma);
extern void PpeSetPortInfo(struct FoeEntry * foe_entry, struct port_info * pinfo, int magic);

extern int ecnt_multicast_hwnat_mul_to_cpu_handler(struct sk_buff *skb, struct FoeEntry *foe_entry);
extern int multicast_speed_is_multicast_entry(struct FoeEntry *foe_entry);
extern u32 macMT7530LanPortMap2Switch(u32 lanPortId);
extern int32_t PpeCleanTableHandler(void);
extern u_int16_t PpeGetMacMatchAcntGrpIndex(struct sk_buff * skb);
extern u_int16_t PpeGetMacMatchMeterGrpIndex(struct sk_buff * skb);
extern int getPpeMeterBindInfo(struct sk_buff * skb, u_int16_t *meterGrpId);
extern void PpeGetAcntMeterIdByFlowId(struct sk_buff * skb, int magic, struct port_info * pinfo, u_int16_t *meterGrpId, u_int16_t *acntGrpIndex);
extern u_int16_t PpeGetMeterIdByMapping(struct sk_buff * skb, int magic,  struct port_info * pinfo);
extern int mats_test(unsigned int round, unsigned int startAddr, unsigned int size);

extern uint8 macSTagFlag;
extern int (*ra_sw_nat_hook_rx_set_l2lu) (struct sk_buff * skb, unsigned int direction, int PpeIndex);

extern int hwnat_delete_foe_entry(int index);
extern int hwnat_is_alive_index(int index);

extern void PpeSetRxForceRingIdx(struct FoeEntry * foe_entry, uint8_t ring_index);
extern int isUpStreamFlow(int magic, struct port_info* pinfo);

extern int ppe_get_ipv4_5t(struct sk_buff* skb,uint8_t* proto,uint32_t* sip,uint32_t* dip,uint16_t* sport,uint16_t* dport);
extern int ppe_trans_int_to_ip_format(unsigned int ip,unsigned char* out_ip);
extern void PpeGetWanMeterId(struct sk_buff * skb, u_int16_t *meterGrpId, int magic, struct port_info * pinfo);

extern int ecnt_multicast_get_channel_by_stag(unsigned int stag_dp);
extern int hwnat_multicast_set_info_for_sfu(int index, int mask);

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/***************sw nat start***********************************/
unsigned char get_swnat_clean_flag(void){
	return swnat_foe_clean_flag;
}

void set_swnat_clean_flag(unsigned char val){
	swnat_foe_clean_flag = val;
}
/****************sw nat end************************************/

/****************L2 VID start***********************************/

void PpeL2LUKeyTblInit(void)
{
	int i=0;

    if(TCSUPPORT_XPON_HAL_API_NG_VAL)
        ppe_l2lu_key_mask = ( C_PCP_DTC_MASK | C_VID_DTC_MASK);
    
	for(i=0; i<L2LU_KEY_ENTRY_NUM; i++)
	{
        memset(&(hwnat_key_table[i]), 0, sizeof(L2LU_key_table_s));	
		PpeL2LUKeyHwTblSet(i, (hwnat_key_table[i]));
	}
	is_l2lu_pkt_exist = 0;
    spin_lock_init(&l2lu_lock);
    
	return;
}

void setL2LUKeyEntry(L2LU_key_s key_data,L2LU_key_table_s *key_entry)
{
    uint16_t cvid = 0;
    uint16_t svid = 0;

    uint8_t cpcp = 0;	
    uint8_t spcp = 0;
    uint16_t key_mask = 0;

    if (key_data.ctci > 0)
    {
        cvid = (key_data.ctci & 0xfff);
        cpcp = (key_data.ctci >> 13);
        key_mask = (C_VID_DTC_MASK | C_PCP_DTC_MASK);
    }

    if (key_data.stci > 0)
    {
        svid = (key_data.stci & 0xfff);
        spcp = (key_data.stci >> 13);
        key_mask = (S_VID_DTC_MASK | S_PCP_DTC_MASK);
    }

    /* ip dscp */
    if (key_data.dscp > 0)
    {
        key_mask |= DSCP_MASK;
    }

    /* set key entry */    
    key_entry->svid = svid;
    key_entry->spcp = spcp;
    key_entry->cvid = cvid;
    key_entry->cpcp = cpcp;
    key_entry->dscp = key_data.dscp;
    key_entry->mask = key_mask;
    key_entry->priority = key_data.priority;
}

int getL2LUKeyMatchEntry(L2LU_key_table_s key_entry)
{
    int i=0;

    for(i=0; i<L2LU_KEY_ENTRY_NUM; i++)
    {
        /* mask not match, continue */
        if (hwnat_key_table[i].mask != key_entry.mask)
        {
            continue;
        }

        /* vid not match, continue */
        if (key_entry.mask & C_VID_DTC_MASK)
        {
            if (hwnat_key_table[i].cvid != key_entry.cvid)
            {
                continue;
            }
        }

        if (key_entry.mask & S_VID_DTC_MASK)
        {
            if (hwnat_key_table[i].svid != key_entry.svid)
            {
                continue;
            }
        }

        /* pcp not match, continue */
        if (key_entry.mask & C_PCP_DTC_MASK)
        {
            if (hwnat_key_table[i].cpcp != key_entry.cpcp)
            {
                continue;
            }
        }

        if (key_entry.mask & S_PCP_DTC_MASK)
        {
            if (hwnat_key_table[i].spcp != key_entry.spcp)
            {
                continue;
            }
        }

        /* dscp not match, continue */
        if (key_entry.mask & DSCP_MASK)
        {
            if (hwnat_key_table[i].dscp != key_entry.dscp)
            {
                continue;
            }
            
            /* ip_version not match, continue */
            if (hwnat_key_table[i].ip_ver != key_entry.ip_ver)
            {
                continue;
            }
        }

        return i; 
    }

    return -1;
}

void PpeIfcRuleSet(uint8_t index,L2LU_key_table_s key_entry)
{
	int      ifc_lut1_idx = 0;
    unsigned int    field = 0;
    IFC_Mode_t   enable   = IFC_ENABLE;
    unsigned int end_flag = 1;
    unsigned int mask     = 0; 
    unsigned int key_low  = 0;
    unsigned int key_high = 0;
    ifc_lut1_idx = (IFC_VLAN_KEY_BASE + index * 2);
    
    /*
         mask = 0, disable PBIT0 and IPv4_DSCP ifc rule
         mask & S_PCP_DTC_MASK, config PBIT0  ifc rule
         mask & DSCP_MASK: config ifc rule:
                 dscp (v4)
                 TRAFFIC_CLASS (v6)

         Notice: S_PCP_DTC_MASK && DSCP_MASK, one ifc  
    */
    if((0 == key_entry.mask) || (key_entry.mask & S_PCP_DTC_MASK))
    {
        if(0 == key_entry.mask)
        {
            enable   = IFC_DISABLE;
            mask     = 0;
            key_low  = 0;
            key_high = 0;
        }
        else 
        {
            mask     = 0xF;
            key_low  = ((key_entry.spcp<<1) & 0xe);
            key_high = 0;
        }
        
        if(key_entry.mask & DSCP_MASK)
        {
            end_flag = 0;
        }
        
        IFC_API_SET_LUT1_RULE(ifc_lut1_idx, enable, end_flag, PBIT0, 0, mask, key_low, key_high);
        ifc_lut1_idx += 1;
    }
    
    if((0 == key_entry.mask) ||  (key_entry.mask & DSCP_MASK))
    {
        if ((0 == key_entry.ip_ver) || (0 == key_entry.mask))
        {
            field = IPv4_DSCP;
        }
        else
        {
            field = IPv6_TRAFFIC_CLASS;
        }

        if(0 == key_entry.mask)
        {
            enable   = IFC_DISABLE;
            mask     = 0;
            key_low  = 0;
            key_high = 0;
        }
        else
        {
            mask     = 0xFF;
            key_low  = key_entry.dscp;
            key_high = 0;
        }
        
        IFC_API_SET_LUT1_RULE(ifc_lut1_idx, enable, 1, field, 0, mask, key_low, key_high);
    }

	return;
}

void PpeL2LUKeyTblSet(uint8_t index,L2LU_key_table_s key_entry)
{
    uint32_t regAddr=0;
    uint32_t regValue=0;

    /* set key table */
    hwnat_key_table[index].cvid = key_entry.cvid;
    hwnat_key_table[index].cpcp = key_entry.cpcp;
    hwnat_key_table[index].dscp = key_entry.dscp;
    hwnat_key_table[index].svid = key_entry.svid;
    hwnat_key_table[index].spcp = key_entry.spcp;
    hwnat_key_table[index].mask = key_entry.mask;
    hwnat_key_table[index].ip_ver = key_entry.ip_ver;
    hwnat_key_table[index].priority = key_entry.priority;
    hwnat_key_table[index].ppe_index = key_entry.ppe_index;
    hwnat_key_table[index].bind_status= key_entry.bind_status;
    hwnat_key_table[index].start_time= key_entry.start_time;

    if(isMT751020)
    {
        /* set key0 register, refer to programm guide */
        regAddr = (L2LU_KEY0_BASE + index*8);
        regValue = ((key_entry.cvid << 20) | (key_entry.cpcp << 16) | (key_entry.dscp << 8) | ((key_entry.mask & (C_VID_DTC_MASK | C_PCP_DTC_MASK)) >> 2) | (key_entry.mask & 0x1f));

        RegWrite(regAddr, regValue);

        regAddr = (L2LU_KEY1_BASE + index*8);
        regValue = ((key_entry.svid << 20) | (key_entry.spcp << 16) | ((key_entry.mask & (S_VID_DTC_MASK | S_PCP_DTC_MASK))>>6));
        RegWrite(regAddr, regValue);
    }
    else if((isEN751221) || (isEN751627))
    {
        /* set key0 register, refer to programm guide */
        regAddr = (L2LU_KEY0_BASE + index*8);
        regValue = key_entry.dscp << 16 | key_entry.mask;
        RegWrite(regAddr, regValue);

        /* set key1 register, refer to programm guide */
        regAddr = (L2LU_KEY1_BASE + index*8);
        regValue = key_entry.spcp << 29 | key_entry.svid << 16 | key_entry.cpcp << 13 | key_entry.cvid;
        RegWrite(regAddr, regValue);
    }
    else if(isEN7580)
        PpeIfcRuleSet(index,key_entry);
}


void PpeL2LUKeySoftTblSet(uint8_t index,L2LU_key_table_s key_entry)
{
    /* set key table */
    hwnat_key_table[index].cvid = key_entry.cvid;
    hwnat_key_table[index].cpcp = key_entry.cpcp;
    hwnat_key_table[index].dscp = key_entry.dscp;
    hwnat_key_table[index].svid = key_entry.svid;
    hwnat_key_table[index].spcp = key_entry.spcp;
    hwnat_key_table[index].mask = key_entry.mask;
    hwnat_key_table[index].ip_ver = key_entry.ip_ver;
    hwnat_key_table[index].priority = key_entry.priority;
    hwnat_key_table[index].ppe_index = key_entry.ppe_index;
    hwnat_key_table[index].bind_status= key_entry.bind_status;
    hwnat_key_table[index].start_time= key_entry.start_time;
}

void PpeL2LUKeyHwTblSet(uint8_t index,L2LU_key_table_s key_entry)
{
    uint32_t regAddr=0;
    uint32_t regValue=0;

    if(isMT751020)
    {
        /* set key0 register, refer to programm guide */
        regAddr = (L2LU_KEY0_BASE + index*8);
        regValue = ((key_entry.cvid << 20) | (key_entry.cpcp << 16) | (key_entry.dscp << 8) | ((key_entry.mask & (C_VID_DTC_MASK | C_PCP_DTC_MASK)) >> 2) | (key_entry.mask & 0x1f));

        RegWrite(regAddr, regValue);

        regAddr = (L2LU_KEY1_BASE + index*8);
        regValue = ((key_entry.svid << 20) | (key_entry.spcp << 16) | ((key_entry.mask & (S_VID_DTC_MASK | S_PCP_DTC_MASK))>>6));

        /* set key1 register, refer to programm guide */
        RegWrite(regAddr, regValue);
    }
    else if((isEN751221) || (isEN751627))
    {
        /* set key0 register, refer to programm guide */
        regAddr = (L2LU_KEY0_BASE + index*8);
        regValue = key_entry.dscp << 16 | key_entry.mask;
        RegWrite(regAddr, regValue);

        /* set key1 register, refer to programm guide */
        regAddr = (L2LU_KEY1_BASE + index*8);
        regValue = key_entry.spcp << 29 | key_entry.svid << 16 | key_entry.cpcp << 13 | key_entry.cvid;
        RegWrite(regAddr, regValue);
    }
    
    else if(isEN7580)
        PpeIfcRuleSet(index,key_entry);
}

void PpeL2LUKeyTblClear(void)
{
    int i=0;

    L2LU_key_table_s key_entry;

    /* init key entry */
    memset(&key_entry, 0, sizeof(L2LU_key_table_s));

    for (i=0; i<L2LU_KEY_ENTRY_NUM; i++)
    {
        PpeL2LUKeyTblSet(i,key_entry);
    }
}

int getL2LUKeyFreeIdx(uint8_t pkt_type)
{
    int i=0;

    for(i=0; i<L2LU_KEY_ENTRY_NUM ; i++)
    {
        if (!hwnat_key_table[i].mask)
        {
            return i;
        }
    }

    /* unicast return when table is full */
    if (L2LU_UNICAST == pkt_type)
    {
        return -1;
    }

    /* multicast can re-write unicast when table is full */
    for(i=0; i<L2LU_KEY_ENTRY_NUM ; i++)
    {
        if (L2LU_UNICAST == hwnat_key_table[i].priority)
        {
            return i;
        }
    }

    return -1;
}

int getL2LUKeyDeleteIdx(uint8_t pkt_type)
{
    int i=0;
	unsigned long starttime_bofore = 0;
	unsigned long starttime_now = 0;
	unsigned long update_time = 0;

	starttime_now = jiffies;
	
    for(i=0; i<L2LU_KEY_ENTRY_NUM ; i++)
    {
    	/*find first bind_status equal to L2LU_DELETE*/
        if((L2LU_DELETE==hwnat_key_table[i].bind_status)&&(pkt_type == hwnat_key_table[i].priority))
        {
            return i;
        }
		/*find first time interval >l2lu_delete_period(unit:s) */
		starttime_bofore = hwnat_key_table[i].start_time;
		if(starttime_bofore <= starttime_now)
			update_time = (starttime_now - starttime_bofore)/100;
		else
			update_time = ((0xFFFFFFFF) - starttime_bofore + starttime_now)/100;

		if((update_time > l2lu_delete_period)&&(pkt_type == hwnat_key_table[i].priority)&& (L2LU_BIND==hwnat_key_table[i].bind_status)){	
			return i;
		}
    }
	
    return -1;
}

int PPeL2LUKeyEntryExist(uint8_t pkt_type)
{
    int i=0;
	if(0 == ppe_l2lu_key_mask)
		return 0;

    for(i=0; i<L2LU_KEY_ENTRY_NUM ; i++)
    {
        if((0!=hwnat_key_table[i].mask)&&(pkt_type == hwnat_key_table[i].priority))
        {
            return 1;
        }
    }

    return 0;
}


int PpeL2luKeyUpdateBindTime(struct sk_buff * skb)
{
	uint8 *pdata=NULL;
	unsigned short cvid = 0;
	unsigned short svid = 0;	
    uint8 cpcp = 0;	
    uint8 spcp = 0;		
    uint8 dscp = 0;	
	uint8 vlanNum=0;
    uint8 ip_ver = 0;
	unsigned short vlanId[2] = {0};
    uint8 pbit[2] = {0};
	unsigned short tpid[2] = {0};
    uint16 key_mask = 0;
    uint8 i = 2;
    int index=0;
	unsigned long starttime_bofore = 0;
	unsigned long starttime_now = 0;
	unsigned long update_time = 0;
	L2LU_key_table_s key_entry;

	pdata=skb->data - 2;
	while((tpid[vlanNum] =((*pdata << 8) + *(pdata + 1))) == 0x8100 ||(tpid[vlanNum] == 0x88a8) ||(tpid[vlanNum] == 0x9100))
	{			
        if(vlanNum < 2)
        {
		    vlanId[vlanNum] = ((*(pdata+2) << 8) + *(pdata + 3)) & 0x0fff;
            pbit[vlanNum] = ((*(pdata+2) >> 5)) & 0x07;
        }
		vlanNum++;
        pdata += 4;
        i--;
        if(i == 0)
			break;
	}

    if((FOE_MAGIC_GE == FOE_MAGIC_TAG(skb))){
        if((TCSUPPORT_MULTI_SWITCH_EXT_VAL && (skb->dlf_resv0 & 0x7) == extend_switch_port)){
            vlanNum++;
            vlanId[1] = vlanId[0];
            vlanId[0] = skb->dlf_resv1;
        }
    }


    if(vlanNum ==  0)
    {
        ;
    }
    else if(vlanNum ==  1)
	{
		if(tpid[0] ==0x8100)
		{
			cvid = vlanId[0];
            cpcp = pbit[0];
            key_mask = (C_VID_DTC_MASK | C_PCP_DTC_MASK);
		}
		else
		{
			svid = vlanId[0];
            spcp = pbit[0];
            key_mask = (S_VID_DTC_MASK | S_PCP_DTC_MASK);		
		}
	}
    else if(vlanNum >= 2)
	{
		svid = vlanId[0];
		cvid = vlanId[1];
        spcp = pbit[0];
        cpcp = pbit[1];
        key_mask = (C_VID_DTC_MASK | C_PCP_DTC_MASK | S_VID_DTC_MASK | S_PCP_DTC_MASK);
    }
    
    if(((*pdata) == 0x08) && (*(pdata+1) == 0x00) && ((*(pdata+2) & 0xf0) == 0x40))
    {
        //IPv4 packets, get dscp
        pdata += 3; //TOS field, the first 6 bit is DSCP
        if((isEN751221) 
            || ((!isEN7580) && ((RegRead(FE_MISC_CFG_7528) & 0x1) == 0))
            || ((isEN7580) && ((RegRead(FE_MISC_CFG_7580) & 0x1) == 0)))
            dscp = ((*pdata) >> 2) & 0x3f;
        else
            dscp = (*pdata);
        if(dscp>0)
            key_mask |= DSCP_MASK;
        ip_ver = 0;
	} else if(((*pdata) == 0x86) && (*(pdata+1) == 0xDD) && ((*(pdata+2) & 0xf0) == 0x60))
    {
        //IPv6 packets, get traffic class
        pdata += 2; //Traffic field
        dscp = ((*pdata & 0x0F)<<4) | ((*(pdata+1) & 0xF0)>>4);
        if(dscp>0)
            key_mask |= DSCP_MASK;
        ip_ver = 1;
	}

    key_mask &= ppe_l2lu_key_mask;

    if(key_mask != 0)
	{
		memset(&key_entry, 0, sizeof(L2LU_key_table_s));
        spin_lock(&l2lu_lock);

        key_entry.cvid = cvid;
        key_entry.cpcp = cpcp;
        key_entry.dscp = dscp;
        key_entry.spcp = spcp;
        key_entry.svid = svid;
        key_entry.ip_ver = ip_ver;
        key_entry.mask = key_mask;
        key_entry.priority = L2LU_UNICAST;
		
        index = getL2LUKeyMatchEntry(key_entry);
        if(index != -1)
		{
			//keeplive will find delete index in l2lykeyhandler. and int could be dound in this function.
			if(hwnat_key_table[index].bind_status == L2LU_UNBIND){
				key_entry.bind_status = L2LU_BIND;
				key_entry.ppe_index = FOE_ENTRY_NUM(skb);
				PpeL2LUKeyTblSet(index,key_entry);
				
			}
			hwnat_key_table[index].start_time = jiffies;
        }

		for(index=0; index<L2LU_KEY_ENTRY_NUM; index++)
		{
			if ((hwnat_key_table[index].mask)&&(hwnat_key_table[index].priority == L2LU_UNICAST))
			{
				starttime_now = jiffies;
				starttime_bofore = hwnat_key_table[index].start_time;
				if(starttime_bofore <= starttime_now)
					update_time = (starttime_now - starttime_bofore)/100;
				else
					update_time = ((0xFFFFFFFF) - starttime_bofore + starttime_now)/100;
							
				if(update_time > l2lu_delete_period){	
					hwnat_key_table[index].bind_status = L2LU_DELETE;
				}
			}
		}

        spin_unlock(&l2lu_lock);
	}
    
	
	return 0 ;
}

int PpeL2luKeySetStatusBind(int PpeIndex)
{
	int i = 0;

	if (PpeIndex <0 || PpeIndex >= FOE_4TB_SIZ_TOTAL)
		return 0;
	
	spin_lock(&l2lu_lock);

	for(i = 0 ; i < L2LU_KEY_ENTRY_NUM; i ++){

		if((hwnat_key_table[i].ppe_index != PpeIndex)||(hwnat_key_table[i].bind_status == L2LU_BIND)){
			continue;
		}

		PpeL2LUKeyHwTblSet(i, hwnat_key_table[i]);	
		hwnat_key_table[i].bind_status = L2LU_BIND;
		hwnat_key_table[i].start_time = jiffies;
		if(is_l2lu_pkt_exist == 0)
			is_l2lu_pkt_exist = 1;
	}
	spin_unlock(&l2lu_lock);
	
	return 0 ;

}

int PpeL2luKeyHandler(struct sk_buff * skb, unsigned int direction, int PpeIndex)
{
    uint8 *pdata=skb->data;
	unsigned short cvid = 0;
	unsigned short svid = 0;	
    uint8 cpcp = 0;	
    uint8 spcp = 0;		
    uint8 dscp = 0;	
	uint8 vlanNum=0;
    uint8 ip_ver = 0;
	unsigned short vlanId[2] = {0};
    uint8 pbit[2] = {0};
	unsigned short tpid[2] = {0};
	int entryIndex = 0;
    uint16 key_mask = 0;

	L2LU_key_table_s key_entry;
	memset(&key_entry, 0, sizeof(L2LU_key_table_s));

	if (PpeIndex <0 || PpeIndex >= FOE_4TB_SIZ_TOTAL)
		return 0;

	if(0x01 == (0xFF & *pdata)){ //multicast not do L2LU
		return 0;
	}

	pdata = pdata + 12;
	if(isMT7525 || isMT7520)
	{
	    if((direction == 1) && (macSTagFlag)) 
	    {
	        //direction: 0 means from wan, 1 means from gmac
	        pdata += 4;//skip S_TAG
	    }
	}

	while((tpid[vlanNum] =((*pdata << 8) + *(pdata + 1))) == 0x8100 ||(tpid[vlanNum] == 0x88a8) ||(tpid[vlanNum] == 0x9100))
	{			
	    vlanId[vlanNum] = ((*(pdata+2) << 8) + *(pdata + 3)) & 0x0fff;
        pbit[vlanNum] = ((*(pdata+2) >> 5)) & 0x07;

		vlanNum++;
        pdata += 4;
        if(vlanNum == 2)
			break;
	}	

    if(direction == 1){
        if((TCSUPPORT_MULTI_SWITCH_EXT_VAL && (skb->dlf_resv0 & 0x7) == extend_switch_port)){
            vlanNum++;
            vlanId[1] = vlanId[0];
            vlanId[0] = skb->dlf_resv1;
        }
    }
    if(vlanNum ==  0)
    {
        ;
    }
    else if(vlanNum ==  1)
	{
		if(tpid[0] ==0x8100)
		{
			cvid = vlanId[0];
            cpcp = pbit[0];
            key_mask = (C_VID_DTC_MASK | C_PCP_DTC_MASK);
		}
		else
		{
			svid = vlanId[0];
            spcp = pbit[0];
            key_mask = (S_VID_DTC_MASK | S_PCP_DTC_MASK);		
		}
	}
    else if(vlanNum >= 2)
	{
		svid = vlanId[0];
		cvid = vlanId[1];
        spcp = pbit[0];
        cpcp = pbit[1];
        key_mask = (C_VID_DTC_MASK | C_PCP_DTC_MASK | S_VID_DTC_MASK | S_PCP_DTC_MASK);
    }	
    if(((*pdata) == 0x08) && (*(pdata+1) == 0x00) && ((*(pdata+2) & 0xf0) == 0x40))
    {
        //IPv4 packets, get dscp
        pdata += 3; //TOS field, the first 6 bit is DSCP
        if((isEN751221) 
            || ((!isEN7580) && ((RegRead(FE_MISC_CFG_7528) & 0x1) == 0))
            || ((isEN7580) && ((RegRead(FE_MISC_CFG_7580) & 0x1) == 0)))
            dscp = ((*pdata) >> 2) & 0x3f;
        else
            dscp = (*pdata);
        if(dscp>0)
            key_mask |= DSCP_MASK;
        ip_ver = 0;
	} else if(((*pdata) == 0x86) && (*(pdata+1) == 0xDD) && ((*(pdata+2) & 0xf0) == 0x60))
    {
        //IPv6 packets, get traffic class
        pdata += 2; //Traffic field
        dscp = ((*pdata & 0x0F)<<4) | ((*(pdata+1) & 0xF0)>>4);
        if(dscp>0)
            key_mask |= DSCP_MASK;
        ip_ver = 1;
	}
	
    key_mask &= ppe_l2lu_key_mask;

    if(key_mask != 0)	
	{
        spin_lock(&l2lu_lock);
        memset(&key_entry, 0, sizeof(L2LU_key_table_s));

        key_entry.cvid = cvid;
        key_entry.cpcp = cpcp;
        key_entry.dscp = dscp;
        key_entry.spcp = spcp;
        key_entry.svid = svid;
        key_entry.mask = key_mask;
        key_entry.ip_ver = ip_ver;
        key_entry.ppe_index= PpeIndex;
        key_entry.priority = L2LU_UNICAST;
        
        entryIndex = getL2LUKeyMatchEntry(key_entry);
		
        if(-1 == entryIndex)
        {
            entryIndex = getL2LUKeyFreeIdx(L2LU_UNICAST);
            if(-1 != entryIndex) {
                PpeL2LUKeySoftTblSet(entryIndex, key_entry);
            } else {
                entryIndex = getL2LUKeyDeleteIdx(L2LU_UNICAST);
                if(entryIndex != -1)
                {	
                    PpeL2LUKeySoftTblSet(entryIndex,key_entry);
                }	
            }
        }
		
        spin_unlock(&l2lu_lock);
    }
    
    return 0;
}

void PpeProcL2LUMulticast(uint32_t ctci,uint32_t stci,uint8_t dscp)
{
    int key_index = -1;

    L2LU_key_s key_data;
    L2LU_key_table_s key_entry;

    /* init key entry */
    memset(&key_data, 0, sizeof(L2LU_key_s));
    memset(&key_entry, 0, sizeof(L2LU_key_table_s));

    key_data.ctci = ctci;
    key_data.stci = stci;
    key_data.dscp = dscp;
    key_data.priority = L2LU_MULCAST;

    setL2LUKeyEntry(key_data,&key_entry);

    /* find the s-vlan or c-vlan entry */
    key_index = getL2LUKeyMatchEntry(key_entry);
    if (key_index >= 0)
    {
        printk("the S-VLAN or C-VLAN is existed,return\n");
        return;
    }

    /* get free key index */
    key_index = getL2LUKeyFreeIdx(L2LU_MULCAST);
    if (key_index < 0)
    {
        printk("the L2LU Key is already full,return\n");
        return;
    }

    /* set L2LU key register */
    PpeL2LUKeyTblSet(key_index,key_entry);
}

static int hwnat_l2lu_mask_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
    int len;
    char buf[64]={0};
    int i=0;
    unsigned long current_time = jiffies;
	int buf_len = 0;
    
    if(ppe_l2lu_key_mask != 0)
    {
        if(ppe_l2lu_key_mask & S_VID_DTC_MASK)
            buf_len += sprintf(buf + buf_len, "svid ");

        if(ppe_l2lu_key_mask & S_PCP_DTC_MASK)
			buf_len += sprintf(buf + buf_len, "spcp ");

        if(ppe_l2lu_key_mask & C_VID_DTC_MASK)
			buf_len += sprintf(buf + buf_len, "cvid ");

        if(ppe_l2lu_key_mask & C_PCP_DTC_MASK)
			buf_len += sprintf(buf + buf_len, "cpcp ");

        if(ppe_l2lu_key_mask & DSCP_MASK)
			buf_len += sprintf(buf + buf_len, "dscp ");

        len = sprintf(page, "ppe_l2lu_key_mask: %s\n", buf);
    }
    else
    {
        len = sprintf(page, "ppe_l2lu_key_mask: NULL\n");	
    }
    for(i=0; i<L2LU_KEY_ENTRY_NUM; i++){
        len += sprintf(page+len, "%d mask:%02X,spcp:%d,svid=%d,cpcp=%d,cvid:%d,dscp:%X,ip_ver:%d,priority:%d,ppe_index=%d bindstat=%d start_time=%lu current_time=%lu.\n",
        i,
        hwnat_key_table[i].mask,
        hwnat_key_table[i].spcp,
        hwnat_key_table[i].svid,
        hwnat_key_table[i].cpcp,
        hwnat_key_table[i].cvid,
        hwnat_key_table[i].dscp,
        hwnat_key_table[i].ip_ver,
        hwnat_key_table[i].priority,
        hwnat_key_table[i].ppe_index,
        hwnat_key_table[i].bind_status,
        hwnat_key_table[i].start_time,
        current_time); 
    }

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

static int hwnat_l2lu_mask_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
    char val_string[64];
    char buf_string[64] = {0};
    int cvid = 0, cpcp = 0, svid = 0, spcp = 0, dscp = 0;
	int buf_len = 0;


    if (count > sizeof(val_string) - 1)
        return -EINVAL;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT;

    if(!sscanf(val_string, "cvid_%d_cpcp_%d_svid_%d_spcp_%d_dscp_%d", &cvid, &cpcp, &svid, &spcp, &dscp))
    {
        printk("usage: echo cvid_<0|1>_cpcp_<0|1>_svid_<0|1>_spcp_<0|1>_dscp_<0|1> > /proc/tc3162/hwnat_l2lu_mask");
        return -EFAULT;
    }

    ppe_l2lu_key_mask = 0;

    if(cvid) 
    {
        ppe_l2lu_key_mask |= C_VID_DTC_MASK;
    }
    if(cpcp) 
    {
        ppe_l2lu_key_mask |= C_PCP_DTC_MASK;
    }
    if(svid) 
    {
        ppe_l2lu_key_mask |= S_VID_DTC_MASK;
    }  
    if(spcp) 
    {
        ppe_l2lu_key_mask |= S_PCP_DTC_MASK;
    }
    if(dscp) 
    {
        ppe_l2lu_key_mask |= DSCP_MASK;
    }

    if(ppe_l2lu_key_mask == 0)
    {
        printk("ppe_l2lu_key_mask: NULL\n") ; 	
    }
    else
    {
        if(ppe_l2lu_key_mask & C_VID_DTC_MASK)
            buf_len += sprintf(buf_string + buf_len, "cvid ");

        if(ppe_l2lu_key_mask & C_PCP_DTC_MASK)
            buf_len += sprintf(buf_string + buf_len, "cpcp ");

        if(ppe_l2lu_key_mask & S_VID_DTC_MASK)
			buf_len += sprintf(buf_string + buf_len, "svid ");

        if(ppe_l2lu_key_mask & S_PCP_DTC_MASK)
            buf_len += sprintf(buf_string + buf_len, "spcp ");

        if(ppe_l2lu_key_mask & DSCP_MASK)
            buf_len += sprintf(buf_string + buf_len, "dscp ");

        printk("ppe_l2lu_key_mask: %s\n", buf_string);	
    }

    PpeL2LUKeyTblClear();

    return count;
}

static int hwnat_l2lu_update_period_read_proc(char *page, char **start, off_t off,int count, int *eof, void *data)
{
    printk("l2lu_delete_period:%ds\n", l2lu_delete_period);

    return 0;   
}
static int hwnat_l2lu_update_period_write_proc(struct file *file, const char *buffer,unsigned long count, void *data)
{
    char get_buf[128];

    if(count > (sizeof(get_buf) - 1))
        return -EINVAL ;

    memset(get_buf, 0, sizeof(get_buf));

    if(copy_from_user(get_buf, buffer, count))
        return -EFAULT;

    get_buf[count] = '\0';

    sscanf(get_buf, "%d",&l2lu_delete_period);

    printk("l2lu_delete_period:%ds\n",l2lu_delete_period);

    return count;
}

static int hwnat_l2lu_multicast_read_proc(char *page, char **start, off_t off,int count, int *eof, void *data)
{
    int i = 0;
    int len = 0;

    for (i=0; i<L2LU_KEY_ENTRY_NUM; i++)
    { 
        len += sprintf(page+len, "%d mask:%02X,cvid:%d,cpbit:%X,svid:%d,spbit:%X,dscp:%X,pkt_type:%d\n",
        i,
        hwnat_key_table[i].mask,
        hwnat_key_table[i].cvid,
        hwnat_key_table[i].cpcp,
        hwnat_key_table[i].svid,
        hwnat_key_table[i].spcp,
        hwnat_key_table[i].dscp,
        hwnat_key_table[i].priority);
    }

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

static int hwnat_l2lu_multicast_write_proc(struct file *file, const char *buffer,unsigned long count, void *data)
{
    uint32_t dscp = 0;
    uint32_t ctci = 0;
    uint32_t stci = 0;

    char cmd[16] = {0};
    char val_string[64] = {0};

    if (count > (sizeof(val_string) - 1))
    {
        return -EINVAL;
    }

    if (copy_from_user(val_string, buffer, count))
    {
        return -EFAULT;
    }

    sscanf(val_string, "%15s %d %d %d",cmd,&ctci,&stci,&dscp);

    if (!strcmp(cmd, L2LU_ADD))
    {
        PpeProcL2LUMulticast(ctci,stci,dscp);
    }
    else if(!strcmp(cmd, L2LU_DEL))
    {
        PpeL2LUKeyTblClear();
    }
    else
    {
        printk("usage: echo add|del ctci_value stci_value dscp_value > /proc/tc3162/hwnat_L2LU_multicast \n");
        printk("example: echo add 100 0 0 > /proc/tc3162/hwnat_L2LU_multicast \n");
    }

    return count;
}
/***************L2 VID END************************************/

/***************black ip start**********************************/
int PpeBlack_ip_add(unsigned int ip, unsigned int mask, unsigned int type, unsigned int sign)
{
	struct BlackHwnatIp *entry = NULL;
	struct BlackHwnatIp *tmp = NULL;
	unsigned int hash;

	if (mask>32)
	{
		printk("Invalid mask!\n");
		return -1;
	}

	if ((type != BLACKIP_UP) && (type != BLACKIP_DOWN) && (type != BLACKIP_UP_DOWN))
	{
		printk("Invalid type!\n");
		return -1;
	}

	if (sign > 31)
	{
		printk("Invalid sign!\n");
		return -1;
	}

	hash = jhash_3words(ip,0,0,0) % 1024;	
	//spin_lock_bh
	spin_lock_bh(&black_ip_lock);

	if(32 == mask)
	{
		list_for_each_entry_safe(entry, tmp,&BlackIpTable[hash], list)
		{
			if(ip == entry->ipv4)
			{
				if (type == BLACKIP_UP || type == BLACKIP_DOWN)
					entry->type |= 1<<type;
				else
					entry->type = 3;
				entry->sign |= (1<<sign);
				spin_unlock_bh(&black_ip_lock);
				return 0;
			}
		}
	}
	else
	{
		list_for_each_entry_safe(entry, tmp, &BlackIpTableRange, list)
		{
			if((ip == entry->ipv4) && (mask == entry->mask))
			{
				if (type == BLACKIP_UP || type == BLACKIP_DOWN)
					entry->type |= 1<<type;
				else
					entry->type = 3;
				entry->sign |= (1<<sign);
				spin_unlock_bh(&black_ip_lock);
				return 0;
			}
		}
	}
	spin_unlock_bh(&black_ip_lock);

	entry = (struct BlackHwnatIp *)kmalloc(sizeof(struct BlackHwnatIp), GFP_KERNEL);
	
	if (entry) 
	{
		memset(entry, 0, sizeof(struct BlackHwnatIp));
		entry->ipv4 = ip;
		entry->mask = mask;
		entry->ipv6_mode = 0;
		if (type == BLACKIP_UP || type == BLACKIP_DOWN)
			entry->type |= 1<<type;
		else
			entry->type = 3;
		entry->sign |= (1<<sign);
	}
	else
	{
		printk("PpeBlack_ip_add:---kmalloc fail!\n");	
		return -1;
	}

	spin_lock_bh(&black_ip_lock);
	if(32 == mask)
		list_add_rcu(&(entry->list),&BlackIpTable[hash]);
	else
		list_add_rcu(&(entry->list),&BlackIpTableRange);	
	spin_unlock_bh(&black_ip_lock);

	return 0;
}


int PpeBlack_ip_delete(unsigned int ip,unsigned int mask,unsigned int type,unsigned int sign)
{
	struct BlackHwnatIp *entry = NULL;
	struct BlackHwnatIp *tmp = NULL;
	unsigned int hash;

	if (mask > 32)
	{
		printk("Invalid mask!\n");
		return -1;
	}

	if ((type != BLACKIP_UP) && (type != BLACKIP_DOWN) && (type != BLACKIP_UP_DOWN))
	{
		printk("Invalid type!\n");
		return -1;
	}

	if (sign > 31)
	{
		printk("Invalid sign!\n");
		return -1;
	}

	hash = jhash_3words(ip,0,0,0) % 1024;	

	spin_lock_bh(&black_ip_lock);

	if(32 == mask)
	{
		list_for_each_entry_safe(entry, tmp,&BlackIpTable[hash], list)
		{
			if(ip == entry->ipv4)
			{
				entry->sign &= ~(1<<sign);
				if(0 == entry->sign)
				{
					list_del(&entry->list);
					kfree(entry);
					entry = NULL;
				}
				spin_unlock_bh(&black_ip_lock);
				return 0;
			}
		}
	}
	else
	{
		list_for_each_entry_safe(entry, tmp, &BlackIpTableRange, list)
		{
			if( (ip == entry->ipv4) && (mask == entry->mask))
			{
				entry->sign &= ~(1<<sign);
				if(0 == entry->sign)
				{
					list_del(&entry->list);
					kfree(entry);
					entry = NULL;
				}
				spin_unlock_bh(&black_ip_lock);
				return 0;
			}
		}
	}
	spin_unlock_bh(&black_ip_lock);
	
	return 0;
}

int PpeBlack_ip_delete_same_sign(unsigned int sign)
{
	struct BlackHwnatIp *entry = NULL;
	struct BlackHwnatIp *tmp = NULL;
	unsigned int range = 0;

	if (sign > 31)
	{
		printk("Invalid sign!\n");
		return -1;
	}

	spin_lock_bh(&black_ip_lock);
	
	for(range=0;range<1024;range++)
	{
		list_for_each_entry_safe(entry, tmp,&BlackIpTable[range], list)
		{
			if(0 != (entry->sign & (1<<sign)))
			{
				entry->sign &= ~(1<<sign);
				if(0 == entry->sign)
				{
					list_del(&entry->list);
					kfree(entry);
					entry = NULL;
				}
			}
		}
	}
	list_for_each_entry_safe(entry, tmp, &BlackIpTableRange, list)
	{
		if(0 != (entry->sign & (1<<sign)))
		{
			entry->sign &= ~(1<<sign);
			if(0 == entry->sign)
			{
				list_del(&entry->list);
				kfree(entry);
				entry = NULL;
			}
		}
	}
	spin_unlock_bh(&black_ip_lock);
	
	return 0;
}

int PpeBlack_ipv6_add(unsigned char *u6_addr8, unsigned int mask, unsigned int type, unsigned int sign)
{
	struct BlackHwnatIp *entry = NULL;
	struct BlackHwnatIp *tmp = NULL;
	unsigned int hash = 0, ip = 0;

	if (mask>128)
	{
		printk("Invalid mask!\n");
		return -1;
	}

	if ((type != BLACKIP_UP) && (type != BLACKIP_DOWN) && (type != BLACKIP_UP_DOWN))
	{
		printk("Invalid type!\n");
		return -1;
	}

	if (sign > 31)
	{
		printk("Invalid sign!\n");
		return -1;
	}

	ip = (((unsigned int)u6_addr8[0])<<24) | (((unsigned int)u6_addr8[1])<<16) |(((unsigned int)u6_addr8[2])<<8) |((unsigned int)u6_addr8[3]);
	
	hash = jhash_3words(ip,0,0,0) % 1024;	
	//spin_lock_bh
	spin_lock_bh(&black_ip_lock);

	if(128 == mask)
	{
		list_for_each_entry_safe(entry, tmp,&BlackIpTable[hash], list)
		{
			if(0 == memcmp(u6_addr8, entry->u6_addr8, 16))
			{
				if (type == BLACKIP_UP || type == BLACKIP_DOWN)
					entry->type |= 1<<type;
				else
					entry->type = 3;
				entry->sign |= (1<<sign);
				spin_unlock_bh(&black_ip_lock);
				return 0;
			}
		}
	}
	else
	{
		list_for_each_entry_safe(entry, tmp, &BlackIpTableRange, list)
		{
			if((0 == memcmp(u6_addr8, entry->u6_addr8, 16)) && (mask == entry->mask))
			{
				if (type == BLACKIP_UP || type == BLACKIP_DOWN)
					entry->type |= 1<<type;
				else
					entry->type = 3;
				entry->sign |= (1<<sign);
				spin_unlock_bh(&black_ip_lock);
				return 0;
			}
		}
	}
	spin_unlock_bh(&black_ip_lock);

	entry = (struct BlackHwnatIp *)kmalloc(sizeof(struct BlackHwnatIp), GFP_KERNEL);
	
	if (entry) 
	{
		memset(entry, 0, sizeof(struct BlackHwnatIp));
		memcpy(entry->u6_addr8, u6_addr8,
						sizeof(entry->u6_addr8));
		
		entry->mask = mask;
		entry->ipv6_mode = 1;
		if (type == BLACKIP_UP || type == BLACKIP_DOWN)
			entry->type |= 1<<type;
		else
			entry->type = 3;
		entry->sign |= (1<<sign);
	}
	else
	{
		printk("PpeBlack_ipv6_add:---kmalloc fail!\n");	
		return -1;
	}

	spin_lock_bh(&black_ip_lock);
	if(128 == mask)
		list_add_rcu(&(entry->list),&BlackIpTable[hash]);
	else
		list_add_rcu(&(entry->list),&BlackIpTableRange);	
	spin_unlock_bh(&black_ip_lock);

	return 0;
}


int PpeBlack_ipv6_delete(unsigned char *u6_addr8,unsigned int mask,unsigned int type,unsigned int sign)
{
	struct BlackHwnatIp *entry = NULL;
	struct BlackHwnatIp *tmp = NULL;
	unsigned int hash = 0, ip = 0;

	if (mask > 128)
	{
		printk("Invalid mask!\n");
		return -1;
	}

	if ((type != BLACKIP_UP) && (type != BLACKIP_DOWN) && (type != BLACKIP_UP_DOWN))
	{
		printk("Invalid type!\n");
		return -1;
	}

	if (sign > 31)
	{
		printk("Invalid sign!\n");
		return -1;
	}

	ip = (((unsigned int)u6_addr8[0])<<24) | (((unsigned int)u6_addr8[1])<<16) |(((unsigned int)u6_addr8[2])<<8) |((unsigned int)u6_addr8[3]);

	hash = jhash_3words(ip,0,0,0) % 1024;	

	spin_lock_bh(&black_ip_lock);

	if(128 == mask)
	{
		list_for_each_entry_safe(entry, tmp,&BlackIpTable[hash], list)
		{
			if(0 == memcmp(u6_addr8, entry->u6_addr8, 16))
			{
				entry->sign &= ~(1<<sign);
				if(0 == entry->sign)
				{
					list_del(&entry->list);
					kfree(entry);
					entry = NULL;
				}
				spin_unlock_bh(&black_ip_lock);
				return 0;
			}
		}
	}
	else
	{
		list_for_each_entry_safe(entry, tmp, &BlackIpTableRange, list)
		{
			if( (0 == memcmp(u6_addr8, entry->u6_addr8, 16)) && (mask == entry->mask))
			{
				entry->sign &= ~(1<<sign);
				if(0 == entry->sign)
				{
					list_del(&entry->list);
					kfree(entry);
					entry = NULL;
				}
				spin_unlock_bh(&black_ip_lock);
				return 0;
			}
		}
	}
	spin_unlock_bh(&black_ip_lock);
	
	return 0;
}

int PpeBlack_ip_port_protocol_add(unsigned int ip, unsigned int port, unsigned int protocol)
{
	struct BlackHwnatIpPortProtocol *entry = NULL;
	struct BlackHwnatIpPortProtocol *tmp = NULL;

	if(port > 65536)
	{
		printk("Invalid port! port should between 0 ~ 65536\n");
		return -1;
	}

	if(protocol < 1 || protocol > 4)
	{
		printk("Invalid protocol! protocol should between 1 ~ 4");
		return -1;
	}
	
	spin_lock_bh(&black_ip_port_protocol_lock);

	list_for_each_entry_safe(entry, tmp,&BlackIpPortProtocolTable, list)
	{
		if((ip == entry->src_ip_from_wan) && (port == entry->src_port_from_wan) && (protocol == entry->protocol))
		{
			spin_unlock_bh(&black_ip_port_protocol_lock);
			return 0;
		}
	}
	spin_unlock_bh(&black_ip_port_protocol_lock);
	
	entry = (struct BlackHwnatIpPortProtocol *)kmalloc(sizeof(struct BlackHwnatIpPortProtocol), GFP_KERNEL);

	if(entry)
	{
		memset(entry, 0, sizeof(struct BlackHwnatIpPortProtocol));
		entry->protocol = protocol;
		entry->src_ip_from_wan = ip;
		entry->src_port_from_wan = port;
	}
	else
	{
		printk("PpeBlack_ip_port_protocol_add:---kmalloc fail!\n");	
		return -1;
	}

	spin_lock_bh(&black_ip_port_protocol_lock);
		list_add_rcu(&(entry->list),&BlackIpPortProtocolTable);	
	spin_unlock_bh(&black_ip_port_protocol_lock);

	return 0;
}

int PpeBlack_ip_port_protocol_delete(unsigned int ip, unsigned int port, unsigned int protocol)
{
	struct BlackHwnatIpPortProtocol *entry = NULL;
	struct BlackHwnatIpPortProtocol *tmp = NULL;

	if(port > 65536)
	{
		printk("Invalid port! port should between 0 ~ 65536\n");
		return -1;
	}

	if(protocol < 1 || protocol > 4)
	{
		printk("Invalid protocol! protocol should between 1 ~ 4");
		return -1;
	}
	
	spin_lock_bh(&black_ip_port_protocol_lock);
	list_for_each_entry_safe(entry, tmp,&BlackIpPortProtocolTable, list)
	{
		if((ip == entry->src_ip_from_wan) && (port == entry->src_port_from_wan) && (protocol == entry->protocol))
		{
			list_del(&entry->list);
			kfree(entry);
			entry = NULL;
			spin_unlock_bh(&black_ip_port_protocol_lock);
			return 0;
		}
	}
	spin_unlock_bh(&black_ip_port_protocol_lock);

	return 0;
}

int PpeBlack_ip_port_protocol_show(void)
{
	struct BlackHwnatIpPortProtocol *entry = NULL;
	char *proto_name[4] = {"TCP","UDP","ICMP","ALL"};

	spin_lock_bh(&black_ip_port_protocol_lock);
	list_for_each_entry(entry, &BlackIpPortProtocolTable, list)	 
	{
		printk("ipv4 addr: %u.%u.%u.%u\t port: %u\t protocol: %d(%s)\t\n",
			((entry->src_ip_from_wan)&(0xFF000000))>>24,((entry->src_ip_from_wan)&(0x00FF0000))>>16, 
			((entry->src_ip_from_wan)&(0x0000FF00))>>8, ((entry->src_ip_from_wan)&(0x000000FF)), 
			entry->src_port_from_wan, entry->protocol,proto_name[entry->protocol - 1]);
	}
	spin_unlock_bh(&black_ip_port_protocol_lock);

	return 0;
}

int PpeBlack_ip_port_protocol_delete_all(void)
{
	struct BlackHwnatIpPortProtocol *entry = NULL;
	struct BlackHwnatIpPortProtocol *tmp = NULL;
	
	spin_lock_bh(&black_ip_port_protocol_lock);
	list_for_each_entry_safe(entry, tmp,&BlackIpPortProtocolTable, list)
	{
		list_del(&entry->list);
		kfree(entry);
		entry = NULL;
	}
	spin_unlock_bh(&black_ip_port_protocol_lock);

	return 0;
}

int ClaculateMask(unsigned int mask)
{
	int mask_bit = 0;
	int i = 31;
	for(i=31;i>(31-mask);i--)
		mask_bit += 1<<i;
	return mask_bit;
}

int compare_ipv6addr(unsigned char *skb_ipv6_addr, unsigned char *rule_ipv6_addr,unsigned int mask)
{
	unsigned int offset = 0, cmp_byte = 0, i = 7;
	unsigned int cmp_result1 = 0, cmp_result2 = 0;
	unsigned char byte_mask = 0;
	cmp_byte = mask/8;
	offset = mask%8;
	if((memcmp(skb_ipv6_addr, rule_ipv6_addr, cmp_byte)==0))
		cmp_result1 = 1;

	for(i=7;i>(7-offset);i--)
		byte_mask += 1<<i;

	if((skb_ipv6_addr[cmp_byte] & byte_mask) == (rule_ipv6_addr[cmp_byte] & byte_mask))
		cmp_result2 = 1;
	
	if(cmp_result1 && cmp_result2)
		return 1;
	else
		return 0;
}

int PpeBlack_ip_port_protocol_foe_handle(struct sk_buff *skb)
{
	struct BlackHwnatIpPortProtocol *entry = NULL;
	struct BlackHwnatIpPortProtocol *tmp = NULL;
	struct iphdr *iph = NULL;
	unsigned char *cp = NULL;
	ushort etherType = 0;
	struct pppoe_hdr *ppph = NULL;
	unsigned int check_addr = 0;
	unsigned int check_protocol = 0;
	ushort check_port = 0;
	ushort ppp_proto = 0;

#define PPP_IP		0x21	/* Internet Protocol */

	cp = skb_mac_header(skb) ;
	/* skip DA and SA mac address */
	cp += 12 ;
	/* get ether type */
	etherType = *(ushort *) cp ;
	/* skip ether type */
	cp += 2 ;
	
	/*parse if vlan exists*/
	if( (etherType == htons(ETH_P_8021Q)) || (etherType == htons(ETH_P_QinQ_88a8)) || (etherType == htons(ETH_P_QinQ_9100)) ) 
	{
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}

	/*parse if second vlan exists*/
	if( (etherType == htons(ETH_P_8021Q)) || (etherType == htons(ETH_P_QinQ_88a8)) || (etherType == htons(ETH_P_QinQ_9100)) ) 
	{
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}

	if( etherType == htons(ETH_P_IP) )		/*IPv4*/ /*0x0800*/
	{
		iph = (struct iphdr*)(cp) ;
		check_addr = ntohl(iph->saddr) ;
		check_protocol = iph->protocol ;

		if((check_protocol == IPPROTO_TCP) || (check_protocol == IPPROTO_UDP))
			check_port = ntohs(*(ushort *)(cp + 20));
	}	
	else if( etherType == htons(ETH_P_PPP_SES) )		/*PPPoe session*/
	{
		ppph = (struct pppoe_hdr*)(cp) ;
		cp = (unsigned char *)(ppph + 1) ;		/*skip pppoe head*/
		ppp_proto = *(ushort *)cp;
		cp += 2;						/*skip ppp protocol*/

		if( ppp_proto == htons(PPP_IP) )
		{
			iph = (struct iphdr* )(cp);
			check_addr = ntohl(iph->saddr) ;
			check_protocol = iph->protocol ;

			if((check_protocol == IPPROTO_TCP) || (check_protocol == IPPROTO_UDP))
				check_port = ntohs(*(ushort *)(cp + 20));
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	spin_lock(&black_ip_port_protocol_lock);
	list_for_each_entry_safe(entry, tmp, &BlackIpPortProtocolTable, list)
	{
		if((entry->src_ip_from_wan==0xFFFFFFFF) || (entry->src_ip_from_wan==check_addr))
			if((entry->protocol==4) || ((entry->protocol==1)&&(check_protocol==IPPROTO_TCP)) || \
				((entry->protocol==2)&&(check_protocol==IPPROTO_UDP)) || ((entry->protocol==3)&&(check_protocol==IPPROTO_ICMP)))
		{
			if((check_protocol==IPPROTO_ICMP) || (entry->src_port_from_wan==65536) || (entry->src_port_from_wan==check_port))
			{
				memset(FOE_INFO_START_ADDR((struct sk_buff *)skb), 0, FOE_INFO_LEN);
				spin_unlock(&black_ip_port_protocol_lock);
				return 1;
			}
		}
	}
	spin_unlock(&black_ip_port_protocol_lock);

	return 0;
}

/*dir =1 mean downstream match src ip, dir = 0 means upstream match dst ip*/
void PpeBlack_ip_foe_handle(struct sk_buff *skb, int direction)
{
	struct BlackHwnatIp *entry = NULL;
	struct BlackHwnatIp *tmp = NULL;
	unsigned int hash_ip = 0;

	ushort etherType = 0;
	ushort ppp_proto = 0;
	struct iphdr *iph = NULL;
	struct ipv6hdr *ip6h = NULL;
	struct pppoe_hdr *ppph = NULL;
	unsigned char *cp = NULL;
	unsigned int check_addr = 0;
	unsigned char check_u6_addr8[16] = {0};
	int iptype = -1; /*0:IPv4  1:IPv6*/
	unsigned int type_offset = 0;

#define PPP_IP		0x21	/* Internet Protocol */
#define PPP_IPV6	0x57	/* Internet Protocol Version 6 */
#define IPTYPE_IPV4	0
#define IPTYPE_IPV6	1

	if (skb == NULL )
	{
		return ;
	}

	cp = skb_mac_header(skb) ;
	/* skip DA and SA mac address */
	cp += 12 ;
	/* get ether type */
	etherType = *(ushort *) cp ;
	/* skip ether type */
	cp += 2 ;
	
	/*parse if vlan exists*/
	if( (etherType == htons(ETH_P_8021Q)) || (etherType == htons(ETH_P_QinQ_88a8)) || (etherType == htons(ETH_P_QinQ_9100)) ) 
	{
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}

	/*parse if second vlan exists*/
	if( (etherType == htons(ETH_P_8021Q)) || (etherType == htons(ETH_P_QinQ_88a8)) || (etherType == htons(ETH_P_QinQ_9100)) ) 
	{
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}

	if( etherType == htons(ETH_P_IP) )		/*IPv4*/
	{
		iph = (struct iphdr*)(cp) ;
		if(direction == 1)
			check_addr = ntohl(iph->saddr) ;
		else
			check_addr = ntohl(iph->daddr) ;
		iptype = IPTYPE_IPV4 ;
	}
	else if( etherType == htons(ETH_P_IPV6) )	/*IPv6*/
	{
		ip6h = (struct ipv6hdr* )(cp);
		if(direction == 1)
			memcpy(check_u6_addr8, ip6h->saddr.s6_addr,sizeof(check_u6_addr8));
		else
			memcpy(check_u6_addr8, ip6h->daddr.s6_addr,sizeof(check_u6_addr8));
		iptype = IPTYPE_IPV6 ;
	}
	else if( etherType == htons(ETH_P_PPP_SES) )		/*PPPoe session*/
	{
		ppph = (struct pppoe_hdr*)(cp) ;
		cp = (unsigned char *)(ppph + 1) ;		/*skip pppoe head*/
		ppp_proto = *(ushort *)cp;
		cp += 2;						/*skip ppp protocol*/

		if( ppp_proto == htons(PPP_IP) )
		{
			iph = (struct iphdr* )(cp);
			if(direction == 1)
				check_addr = ntohl(iph->saddr) ;
			else
				check_addr = ntohl(iph->daddr) ;
			iptype = IPTYPE_IPV4 ;
		}
		else if( ppp_proto == htons(PPP_IPV6) )
		{
			ip6h = (struct ipv6hdr* )(cp);
			if(direction == 1)
				memcpy(check_u6_addr8, ip6h->saddr.s6_addr,sizeof(check_u6_addr8));
			else
				memcpy(check_u6_addr8, ip6h->daddr.s6_addr,sizeof(check_u6_addr8));
			iptype = IPTYPE_IPV6 ;
		}
		else
		{
			return ;
		}
	}
	else
	{
		return ;
	}

	if(direction == 1)
		type_offset = BLACKIP_DOWN;
	else
		type_offset = BLACKIP_UP;


	if(iptype == IPTYPE_IPV4){
		hash_ip = jhash_3words(check_addr,0,0,0) % 1024;
	
		spin_lock(&black_ip_lock);
		list_for_each_entry_safe(entry, tmp,&BlackIpTable[hash_ip], list)
		{
			if((entry->ipv6_mode == 0) &&(check_addr == entry->ipv4))
			{

				if(0 == (entry->type & (1<<type_offset)))
				{
					spin_unlock(&black_ip_lock);
					return;
				}
				else
				{
					memset(FOE_INFO_START_ADDR((struct sk_buff *)skb), 0, FOE_INFO_LEN);
					spin_unlock(&black_ip_lock);
					return;
				}	
			}
		}					

		list_for_each_entry_safe(entry, tmp, &BlackIpTableRange, list)
		{
			if((entry->ipv6_mode == 0)&&((check_addr & ClaculateMask(entry->mask)) == (entry->ipv4 & ClaculateMask(entry->mask))))
			{
				if(0 == (entry->type & (1<<type_offset)))
				{
					spin_unlock(&black_ip_lock);
					return;
				}
				else
				{
					memset(FOE_INFO_START_ADDR((struct sk_buff *)skb), 0, FOE_INFO_LEN);
					spin_unlock(&black_ip_lock);
					return;
				}	
			}
		}

		spin_unlock(&black_ip_lock);

	}else if (iptype == IPTYPE_IPV6){
		check_addr = (((unsigned int)check_u6_addr8[0])<<24) | (((unsigned int)check_u6_addr8[1])<<16) |(((unsigned int)check_u6_addr8[2])<<8) |((unsigned int)check_u6_addr8[3]);
		hash_ip = jhash_3words(check_addr,0,0,0) % 1024;	
		
		spin_lock(&black_ip_lock);
		list_for_each_entry_safe(entry, tmp,&BlackIpTable[hash_ip], list)
		{
			if((entry->ipv6_mode == 1) &&(memcmp(check_u6_addr8, entry->u6_addr8, 16)==0))
			{
				if(0 == (entry->type & (1<<type_offset)))
				{
					spin_unlock(&black_ip_lock);
					return;
				}
				else
				{
					memset(FOE_INFO_START_ADDR((struct sk_buff *)skb), 0, FOE_INFO_LEN);
					spin_unlock(&black_ip_lock);
					return;
				}	
			}
		}		
		
		list_for_each_entry_safe(entry, tmp, &BlackIpTableRange, list)
		{
			if((entry->ipv6_mode == 1)&& (compare_ipv6addr(check_u6_addr8, entry->u6_addr8, entry->mask)))
			{
				if(0 == (entry->type & (1<<type_offset)))
				{
					spin_unlock(&black_ip_lock);
					return;
				}
				else
				{
					memset(FOE_INFO_START_ADDR((struct sk_buff *)skb), 0, FOE_INFO_LEN);
					spin_unlock(&black_ip_lock);
					return;
				}	
			}
		}

		spin_unlock(&black_ip_lock);

	}else 
		return;

	if(1 == direction)
		PpeBlack_ip_port_protocol_foe_handle(skb);

	return;
}

int32_t PpeSetLanTcpBlackList(uint16_t lanPort, uint16_t enable)
{
	if(lanPort>3)
		return -1;

	lanTcpBlackListBit &= ~(1<<lanPort);
	lanTcpBlackListBit |= (enable<<lanPort);

	printk("lanTcpBlackListBit:%d\n", lanTcpBlackListBit);
	return 0;
}

int32_t isLanTcpBlackList(struct sk_buff * skb, int magic, struct port_info * pinfo)
{
	if(0 == lanTcpBlackListBit)
		return 0;

	if((magic == FOE_MAGIC_EPON)||(magic == FOE_MAGIC_GPON)||((magic == FOE_MAGIC_GE)&&pinfo->eth_is_wan))
	{
		if(isLANInterface(skb->original_dev))
			return 0;
		
		if(lanTcpBlackListBit&(1<<getLogicLANIndex(skb->original_dev)))
			return 1;
	}

	return 0;
}
/***************black ip end***********************************/

/**************soft ratelimit start*******************************/
 static int skb_ratelimit_tx(struct sk_buff*skb)
  {
	 if(skb==NULL || skb->dev==NULL)
		 return 0;

	 skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);

	  return 0;
  }

 int getWifiQueueIndex(struct net_device *dev,unsigned char direction)
{
	unsigned int index = 0;

	if ( is5GWiFiInterface(dev) )
		index = get5GLogicWifiIndex(dev) + SINGLE_WLAN_SSID_NUM;/*5g wifi index:8~15*/
	else if (is24GWiFiInterface(dev))
		index = get24GLogicWifiIndex(dev);  /*2.4g wifi index:0~7*/	
	else 
		return -1;

	if(WIFI_RATELIMIT_RX == direction)
		return wifiqueue_start_idx+index*2;
	else
		return wifiqueue_start_idx+index*2+1;
}

static int wlan_ssid_ratelimit_read_proc(
                char *page, char **start, off_t off,
                int count, int *eof, void *data)
{
        int i = 0;

	if(wifiqueue_start_idx==-1)
	{
		printk("do not set any ssid ratelimit\n");
		return 0;
	}
	
        printk("%4s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s\n",
				"","ra0","ra1","ra2","ra3","ra4","ra5","ra6","ra7","rai0","rai1","rai2","rai3","rai4","rai5","rai6","rai7");


        printk("%4s","rx:");

        for(i = 0;i < 16;i++)
                printk("%10d",wifi_rate[2*i]);

        printk("\n");


        printk("%4s","tx:");

        for(i = 0;i < 16;i++)
                printk("%10d",wifi_rate[2*i+1]);

        printk("\n");

        printk("\nNote:unit is Kbps,0 means no ratelimit\n");
        return 0;
}

extern int (*getWifiQueueIndex_hook)(struct net_device *dev,unsigned char direction);
static int wlan_ssid_ratelimit_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[32];
	unsigned char ssid[8];
	unsigned char dir_tmp[8];
	unsigned int ratelimit = 0;
	int queue_idx = -1;
	unsigned char direction;
	struct net_device dev;

	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	memset(ssid,0,8);
	memset(dir_tmp,0,8);
	
	sscanf(get_buf, "%7s %7s %d",ssid,dir_tmp, &ratelimit);
	memcpy(dev.name,ssid,8);
	if(-1 == wifiqueue_start_idx  && soft_ratelimit_allocatequeue_hook)
		wifiqueue_start_idx = soft_ratelimit_allocatequeue_hook(SINGLE_WLAN_SSID_NUM*4);
	if(-1 == wifiqueue_start_idx )
		goto help;
	if(!strcmp(dir_tmp, "rx"))
		direction = WIFI_RATELIMIT_RX;
	else if(!strcmp(dir_tmp, "tx"))
		direction = WIFI_RATELIMIT_TX;
	else
		goto help;
	queue_idx = getWifiQueueIndex(&dev,direction);
	getWifiQueueIndex_hook = getWifiQueueIndex;
	if(queue_idx >= (SINGLE_WLAN_SSID_NUM*4)+wifiqueue_start_idx)
	{
		printk("error the queue_idx:%d is out of range\n",queue_idx);
		goto help;
	}
	wifi_rate[queue_idx-wifiqueue_start_idx] = ratelimit;	
	
	if(soft_ratelimit_set_queue_hook)
	{
		if(queue_idx%2)
			soft_ratelimit_set_queue_hook(queue_idx,ratelimit,skb_ratelimit_tx);
		else
			soft_ratelimit_set_queue_hook(queue_idx,ratelimit,wifi_eth_fast_tx_hook);
	}
	return count;

	help:
	printk("echo ssid(raX/raiX) dir(rx/tx) ratelimit(kbps) > /proc/tc3162/wlan_ssid_ratelimit\n");
	printk("Example:echo ra0 rx 100 > /proc/tc3162/wlan_ssid_ratelimit\n");
	return count;
}

/*____________________________________________________________________*
**	function name: restore_offload_info
**
**	description:
*     restore offload info for gpon or epon, initialize restore_offload_info_hook()
*
**	parameters:
*     	struct sk_buff *skb
*     	struct port_info *pinfo
*	int magic
*
**	return:
*     none
* ___________________________________________________________________*/

void restore_offload_info(struct sk_buff *skb, struct port_info *pinfo, int magic)
{
	struct offload_info *offload_entry;
    if (offload_info_entry)
        offload_entry = &offload_info_entry[FOE_ENTRY_NUM(skb)];
    else 
        return;

	if(magic == FOE_MAGIC_GPON){
		pinfo->stag = offload_entry->qgpon.gemid;
		pinfo->channel = offload_entry->qgpon.tcon;

		pinfo->tsid = (offload_entry->qgpon.tsid<<1) | (offload_entry->qgpon.tse&0x1);
		pinfo->txq = offload_entry->qgpon.txq;
	}
	else if(magic == FOE_MAGIC_EPON){
		pinfo->channel = offload_entry->qepon.llid;

		pinfo->tsid = (offload_entry->qepon.tsid<<1) | (offload_entry->qepon.tse&0x1);
		pinfo->txq = offload_entry->qepon.txq;
	}
}

/*____________________________________________________________________*
**	function name: store_offload_info
**
**	description:
*     store offload info for gpon or epon, called by PpeSetForcePortInfo()
*
**	parameters:
*     	struct sk_buff *skb
*     	struct port_info *pinfo
*	int magic
*
**	return:
*     none
* ___________________________________________________________________*/

void store_offload_info(struct sk_buff *skb,struct port_info * pinfo, int magic)
{
	struct offload_info *offload_entry;
    if (offload_info_entry)
        offload_entry = &offload_info_entry[FOE_ENTRY_NUM(skb)];
    else 
        return;

	if(magic == FOE_MAGIC_GPON){
		offload_entry->qgpon.gemid = pinfo->stag;
		offload_entry->qgpon.tcon = pinfo->channel;

		offload_entry->qgpon.tse = pinfo->tsid & 0x1;
		offload_entry->qgpon.tsid = (pinfo->tsid >> 1) & 0x1f;
		offload_entry->qgpon.txq = pinfo->txq;
	}
	else if(magic == FOE_MAGIC_EPON){
    		offload_entry->qepon.llid = pinfo->channel;

		offload_entry->qepon.tse = pinfo->tsid & 0x1;
		offload_entry->qepon.tsid = (pinfo->tsid >> 1) & 0x1f;
		offload_entry->qepon.txq = pinfo->txq;
	}
}


/**************soft ratelimit end********************************/

/**************old multicast start*******************************/
int PpeMulticastDataHandler(struct sk_buff *skb)
{
	return ppe_is_multicast_data_pack(skb);
}

int is_multi_need_ds_offload(unsigned long mask)
{
	uint16_t etype;
	int i;
	
	etype = (0x1f&mask);
	for(i=0;i<5;i++)
	{
		if(etype&(1<<i)){
			if(2 != lanPortState[i].linkSpeed)
				return 1;
		}
	}
	
	return 0;
}

void multi_set_ds_offload(struct FoeEntry *foe_entry,unsigned int foe_index)
{
	foe_ext[foe_index].ds_offload = 4;/* PDMA TX Ring 3 */

    /* for en7512 soft qdma */
    PpeSetInfoBlk2(foe_entry, FP_QDMA1_SW, 0x3F, 0x3F, 1);
    
    if(isEN751627)
    {
        PpeSetRxForceRingIdx(foe_entry, RING_IDX_1);
    }

    return;
}

static int get_wlan_act_dp(unsigned long ra_mask,unsigned long rai_mask,unsigned long rax_mask)
{
	int act_dp = 0;
	int ra_index = 0;
	int rai_index = 0;
	int rax_index = 0;
	
	/* break when find one */
	while (ra_mask)
	{
		if (ra_mask & (1 << ra_index))
		{
			break;
		}
		
		ra_index++;
	}
	
	while (rai_mask)
	{
		if (rai_mask & (1 << rai_index))
		{
			break;
		}
		
		rai_index++;
	}

	while (rax_mask)
	{
		if (rax_mask & (1 << rax_index))
		{
			break;
		}
		
		rax_index++;
	}

	if (ra_mask)
	{
		act_dp = DP_RA0 + ra_index;
	}

	if (rai_mask)
	{
		act_dp = DP_RAI0 + rai_index;
	}

	if (rax_mask)
	{
		act_dp = DP_RAX0 + rax_mask;
	}

	return act_dp;
}

void set_act_dp_bits(struct FoeEntry *foe_entry,int value)
{
	if (IS_IPV4_GRP(foe_entry))
	{
		foe_entry->ipv4_hnapt.act_dp = 0;
		foe_entry->ipv4_hnapt.act_dp = value;
	}
	else if (IS_IPV6_GRP(foe_entry))
	{
		foe_entry->ipv6_5t_route.act_dp = 0;
		foe_entry->ipv6_5t_route.act_dp = value;
	}
}

void set_mcast_bit(struct FoeEntry *foe_entry,uint8_t value)
{
	if (IS_IPV4_GRP(foe_entry))
	{
		foe_entry->ipv4_hnapt.mcast = value;
	}
	else if (IS_IPV6_GRP(foe_entry))
	{
		foe_entry->ipv6_5t_route.mcast = value;
	}
}

static void PpeSetInfoBlk2_Acnt0(struct FoeEntry * foe_entry, uint32_t port_ag)
{
	_info_blk2 *iblk2;
    if (IS_IPV4_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
    else if (IS_IPV6_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
    else if (IS_L2_RRIDGE(foe_entry))
	iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
    else
        return;

    if(isEN7580)
		SetInfo2Mgrp(iblk2,port_ag);
	    //iblk2->port_mg = port_ag;	
    else
        iblk2->port_ag = port_ag;

	return;
}

extern int ecnt_multicast_hwnat_state_handler(unsigned int foe_index,unsigned int port_mask,unsigned int local);
static int hwnat_multicast_qdma_1toN_update(int foe_index, unsigned long mask)
{
	return ecnt_multicast_hwnat_state_handler(foe_index, mask, 0);
}

/*_____________________________________________________________________________
**      function name: hwnat_multicast_set_info
**      descriptions:
**          According to multicast entry info to set hw nat info
**
**      parameters:
**			int index
**			unsigned long mask
**
**      global:
**             None
**
**      return:
**            Success:        0
**            Otherwise:     -1
**
**      call:**   
**
**      revision:
**____________________________________________________________________________
*/
int hwnat_multicast_set_info(int index, unsigned long mask, short int bindvid)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	struct port_info multicast_info;
	unsigned int interfaceflag;
	unsigned long  tag = mask & HWNAT_LAN_IF_MASK;
	unsigned long  rainfo = (mask >> HWNAT_WLAN_IF_BASE) & HWNAT_RA_IF_MASK;
	unsigned long  raiinfo = (mask >> (HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_NUM)) & HWNAT_RAI_IF_MASK;
#if defined(TCSUPPORT_WLAN_MT7990)
	unsigned long  raxinfo = (mask >> (HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_NUM+HWNAT_WLAN_IF_I_NUM)) & HWNAT_RAI_IF_MASK;
#else
	unsigned long  raxinfo = 0;
#endif
	int raindex = 0,raiindex = 0,raxindex = 0,need_ds_offload = 0;
	int wlan_act_dp = 0;
    unsigned int blk2_result = 0;
    _info_blk2 blk2;
    int force_dstq = 0;
	int channel = 1;
    
    memset(&blk2, 0, sizeof(blk2));
	if (index <0 || index >= FOE_4TB_SIZ_TOTAL)
		return 0;

	if(SUPPORT_QDMA_MULTICAST_1toN)
		return hwnat_multicast_qdma_1toN_update(index, mask);

	foe_entry = getPpeEntryLocal(index, foe_entry);
	if(foe_entry == NULL)
		return -1;

    ETHER_API_GET_FORCE_DSTQ(&force_dstq);

    if (TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && (dsoffload_presetting & DS_MULTICAST))
    {
            /* for en7512 soft qdma, need ds offload even for 1G port */
            need_ds_offload = 1;
    }

	interfaceflag = ((mask & HWNAT_LAN_IF_MASK) > 0?1:0) | ((((mask >> HWNAT_WLAN_IF_BASE) & HWNAT_WLAN_IF_MASK) > 0?1:0)<<1);
    blk2_result = PpeGetInfoBlk2(foe_entry,&blk2);

#if 0 //PpeParseResult.vlan_layer 
	if(SUPPORT_L2_INFO_CFG)
	{
		foe_entry->bfib1.vlan_layer = PpeParseResult.vlan_layer;
		if (IS_IPV4_GRP(foe_entry))
		{
	        foe_entry->ipv4_hnapt.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->ipv4_hnapt.vlan2 = ntohs(PpeParseResult.vlan2);
		}
		else if (IS_IPV6_GRP(foe_entry))
		{
			foe_entry->ipv6_5t_route.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->ipv6_5t_route.vlan2 = ntohs(PpeParseResult.vlan2);
		}
		else if (IS_L2_RRIDGE(foe_entry))
		{
			foe_entry->l2_bridge32.vlan1 = ntohs(PpeParseResult.vlan1);
			foe_entry->l2_bridge32.vlan2 = ntohs(PpeParseResult.vlan2);
		}
	}
#endif
	
	if(DebugLevel >= 1)
		printk("%s %d interfaceflag=%d,mask=0x%lx,blk2_result=%d,blk2.fpidx=%d,vlan_tag %x etype %X, vlan_layer %d\n",__FUNCTION__,__LINE__,
		interfaceflag,mask,blk2_result,blk2.fpidx,PpeParseResult.vlan_tag, foe_entry->ipv4_hnapt.etype, foe_entry->ipv4_hnapt.bfib1.vlan_layer);
    if ((TCSUPPORT_FWC_MCST_VAL || TCSUPPORT_XPON_HAL_API_MCST_VAL)
		&& ((MULTICAST_SPEED_STATE_II != interfaceflag) || (blk2_result && (FP_QDMA1_SW == blk2.fpidx))))
    {
	    //not care struct port_info,so just clear
	    memset(&multicast_info, 0, sizeof(struct port_info));
	    /* for EN7580 info */ 
	    multicast_info.tsid = HWNAT_DEFAULT_MTRGRP;
	    multicast_info.nbq= 1;
	    PpeSetPortInfo(foe_entry, &multicast_info, FOE_MAGIC_OFFLOAD);
		if(SUPPORT_VPM_FROM_PARSER)
		{
			foe_entry->bfib1.vpm = 1;
		}

		if(SUPPORT_SRAM_ENTRY && (index<hwnat_dram_idx_base)) {
			setPpeEntry(index, foe_entry);
		}
	    return 0;
    }

	//3 & 1 mean include lan speed
	if(MULTICAST_SPEED_STATE_I == interfaceflag || MULTICAST_SPEED_STATE_II == interfaceflag)
	{
		//set stag
			channel = ecnt_multicast_get_channel_by_stag(0xff&tag);
			if (foe_entry->bfib1.pkt_type < 2)
			{
				if(SUPPORT_VPM_FROM_PARSER)
				{
					foe_entry->ipv4_hnapt.etype &= 0xfe00;
        			foe_entry->bfib1.vpm = 1;
				}
				else	
				{
					foe_entry->ipv4_hnapt.etype &= 0xff00;
					foe_entry->bfib1.vpm = ppe_trans_tpid_to_vpm(htons(foe_entry->ipv4_hnapt.etype));
				}
				foe_entry->ipv4_hnapt.etype |= (0xff&tag);
				foe_entry->ipv4_hnapt.channel = channel;
			}
			else
			{
				if(SUPPORT_VPM_FROM_PARSER)
				{
					foe_entry->ipv6_5t_route.etype &= 0xfe00;
        			foe_entry->bfib1.vpm = 1;
				}
				else
				{
					foe_entry->ipv6_5t_route.etype &= 0xff00;
					foe_entry->bfib1.vpm = ppe_trans_tpid_to_vpm(htons(foe_entry->ipv6_5t_route.etype));
				}
				foe_entry->ipv6_5t_route.etype |= (0xff&tag);
				foe_entry->ipv6_5t_route.channel = channel;
			}
	}

    if(MULTICAST_SPEED_STATE_II == interfaceflag)
    {
        if (!(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && (dsoffload_presetting & DS_MULTICAST)))
        {
            if (1 != force_dstq)
                PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);

        }    
        else
        {
            if(need_ds_offload)
            {
                multi_set_ds_offload(foe_entry,index);
            }
        }
    }
	//both lan and wifi speed
	if(MULTICAST_SPEED_STATE_I == interfaceflag)
	{
		//break when find one 
		while(rainfo)
		{
			if(rainfo&(1<<raindex))
			{
				break;
			}
			raindex++;
		}
		while(raiinfo)
		{
			if(raiinfo&(1<<raiindex))
			{
				break;
			}
			raiindex++;
		}

		while(raxinfo)
		{
			if(raxinfo&(1<<raxindex))
			{
				break;
			}
			raxindex++;
		}
		
		/* reset vlan layer */
		if ( (0 == foe_entry->bfib1.vlan_layer) && (!SUPPORT_L2_INFO_CFG))
		{
			foe_entry->bfib1.vlan_layer = 1;
		}
		
		if(foe_entry->bfib1.pkt_type < 2)
		{
			//set act_dp
			foe_entry->ipv4_hnapt.act_dp = 0;
			if(rainfo)
				foe_entry->ipv4_hnapt.act_dp = DP_RA0 + raindex;
			if(raiinfo)
				foe_entry->ipv4_hnapt.act_dp = DP_RAI0 + raiindex;
			if(raxinfo)
				foe_entry->ipv4_hnapt.act_dp = DP_RAX0 + raxindex;
			
			//set mcast and force port
			foe_entry->ipv4_hnapt.mcast = 1;
            if (!(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && (dsoffload_presetting & DS_MULTICAST)))
            {
                if (1 != force_dstq)
                    PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);
  
            }
            else
            {
                if(need_ds_offload)
                {
			multi_set_ds_offload(foe_entry,index);
                }
            }
		}
		else
		{
			//set act_dp
				foe_entry->ipv6_5t_route.act_dp = 0;
			if(rainfo)
				foe_entry->ipv6_5t_route.act_dp = DP_RA0 + raindex;
			if(raiinfo)
				foe_entry->ipv6_5t_route.act_dp = DP_RAI0 + raiindex;
			if(raxinfo)
				foe_entry->ipv6_5t_route.act_dp = DP_RAX0 + raxindex;
			
			//set mcast and act_dp
			foe_entry->ipv6_5t_route.mcast = 1;
            if (!(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && (dsoffload_presetting & DS_MULTICAST)))
            {
                if (1 != force_dstq)
                    PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);
       
            }
            else
            {
                if(need_ds_offload)
                {
                    multi_set_ds_offload(foe_entry,index);
                }
            }
        }
	}
	else if ((TCSUPPORT_FWC_MCST_VAL) && (MULTICAST_SPEED_STATE_II == interfaceflag))
	{
		/* enable mcast for Askey TLF */
		set_mcast_bit(foe_entry,1);

		/* reset vlan layer */
		if ( (0 == foe_entry->bfib1.vlan_layer) && (!SUPPORT_L2_INFO_CFG))
		{
			foe_entry->bfib1.vlan_layer = 1;
		}

		/* clean act_dp */
		set_act_dp_bits(foe_entry,0);
				
		/* force port to GDMA1 */
		PpeSetInfoBlk2(foe_entry, FP_GDMA1, 0x3F, 0x3F, 0);
	}
	else if ((TCSUPPORT_FWC_MCST_VAL) && (MULTICAST_SPEED_STATE_III == interfaceflag))
	{
		/* disable mcast */
		set_mcast_bit(foe_entry,0);

		/* clean vlan layer */
		foe_entry->bfib1.vlan_layer = 0;

		/* get wlan act_dp */
		wlan_act_dp = get_wlan_act_dp(rainfo,raiinfo,raxinfo);
		
		/* set act_dp */
		set_act_dp_bits(foe_entry,wlan_act_dp);
		
		/* force port to PDMA */
		PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);
	}
	else if(MULTICAST_SPEED_STATE_IV == interfaceflag)
	{
		PpeSetInfoBlk2(foe_entry, FP_DROP, 0x3F, 0x3F, 1);
	}
	else if(MULTICAST_SPEED_STATE_III == interfaceflag)
	{
		/* disable mcast */
		set_mcast_bit(foe_entry,0);

		/* set vpm as 0x8100 if vlan_layer exist */
		if(foe_entry->bfib1.vlan_layer)
		    foe_entry->bfib1.vpm = 1;

		/* get wlan act_dp */
		wlan_act_dp = get_wlan_act_dp(rainfo,raiinfo,raxinfo);
		
		/* set act_dp */
		set_act_dp_bits(foe_entry,wlan_act_dp);
        
		/* force port to PDMA */
		PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);
		
		PpeSetRxForceRingIdx(foe_entry, RING_IDX_1);
	}
	
#if 0//because before updating hw nat rule,clear all hw nat first,so should not use this code ~o~  maybe
	if(MULTICAST_SPEED_STATE_II == interfaceflag)
	{
		//set mcast and act_dp
		foe_entry->ipv4_hnapt.mcast = 0;
		foe_entry->ipv4_hnapt.act_dp = 0;//not useful for pure lan
		PpeSetInfoBlk2(foe_entry, FP_GDMA1, 0x3F, 0x3F, 0);
	}

	if(MULTICAST_SPEED_STATE_III == interfaceflag)
	{
		//set mcast and act_dp
		foe_entry->ipv4_hnapt.mcast = 0;
		foe_entry->ipv4_hnapt.act_dp = 0;//not useful for pure lan
		PpeSetInfoBlk2(foe_entry, FP_GDMA1, 0x3F, 0x3F, 0);
	}
#endif

    PpeSetInfoBlk2_Acnt0(foe_entry, HWNAT_MULTI_ACNTGRP);

	if(SUPPORT_SRAM_ENTRY && (index<hwnat_dram_idx_base)) {
		setPpeEntry(index, foe_entry);
	}
	return 0;
}

/*_____________________________________________________________________________
**      function name: judgeinterface
**      descriptions:
**          judge if one multicast entry including wifi interface and lan interface
**
**      parameters:
**			struct sk_buff *skb
**
**      global:
**             None
**
**      return:
**            wifi&lan interfaces:      1
**            other case:     0
**
**      call:**   
**
**      revision:
**____________________________________________________________________________
*/
int judgeinterface(struct sk_buff *skb)
{
	uint32 laninterfaceflag = 0,wifiinterfaceflag = 0;
	IGMP_HWNATEntry_t* ms_stats_p = NULL;
	if (multicast_speed_find_entry_hook) 
		ms_stats_p = (IGMP_HWNATEntry_t *)multicast_speed_find_entry_hook(FOE_ENTRY_NUM(skb));
	
	if(ms_stats_p)
	{
		laninterfaceflag = (ms_stats_p->mask & HWNAT_LAN_IF_MASK)>0?1:0;
		wifiinterfaceflag = ((ms_stats_p->mask >> HWNAT_WLAN_IF_BASE) & HWNAT_WLAN_IF_MASK)>0?1:0;
	}
	
	if(laninterfaceflag && wifiinterfaceflag)
		return 1;
	else
		return 0;
}

static void multicast_speed_set_dev(unsigned long maskindex,struct sk_buff *skb)
{
	if(maskindex >= HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_NUM+HWNAT_WLAN_IF_I_NUM)
	{
		skb->dev = DstPort[DP_RAX0+(maskindex-HWNAT_WLAN_IF_BASE-HWNAT_WLAN_IF_NUM-HWNAT_WLAN_IF_I_NUM)];
	}
	else if(maskindex >= HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_NUM)
	{
		skb->dev = DstPort[DP_RAI0+(maskindex-HWNAT_WLAN_IF_BASE-HWNAT_WLAN_IF_NUM)];
	}
	else
	{
		skb->dev = DstPort[DP_RA0+(maskindex-HWNAT_WLAN_IF_BASE)];
	}
	
	return;
}

#define CONTINUE_TO_HANDLE_WIFI 0
#define NOWIFI_JUST_RETURN_OUT 1

int OffloadMLCHandler(struct sk_buff *skb, IGMP_HWNATEntry_t *ms_stats_p, multicast_flood_hwentry_t *hw_flood_entry, int wifi_flag){
    struct sk_buff *skb2;

    int masktemp = 0, maskindex;
    u8 sw_mul_port_mask = 0;
	if(TCSUPPORT_MULTI_SWITCH_EXT_VAL){
		if(ms_stats_p != NULL)
			sw_mul_port_mask = ms_stats_p->mask & extend_switch_mask ;
		else if(hw_flood_entry !=NULL)
			sw_mul_port_mask = hw_flood_entry->port_mask & extend_switch_mask;
		else
			sw_mul_port_mask =0;
	}else
		sw_mul_port_mask = sw_port_offload_mask;
	
	for(maskindex = 0; maskindex < 8; maskindex++)
	{	
		if((sw_mul_port_mask & (1<<maskindex)) == 0)
			continue;

		if(masktemp)
		{
			skb2 = skb_clone(skb, GFP_ATOMIC);
			if(skb2)
			{
			    if (DstPort[DP_GMAC1+maskindex] != NULL){
			        skb2->dev = DstPort[DP_GMAC1+maskindex];	
			    skb2->dev->netdev_ops->ndo_start_xmit(skb2,skb2->dev);
			    }else{
			        dev_kfree_skb_any(skb2);
			    }
			}
		}
		else
		{
			if(((NULL != ms_stats_p) && (ms_stats_p->wifinum > 0)) || ((NULL != hw_flood_entry) && (1 == wifi_flag))){
                skb2 = skb_clone(skb, GFP_ATOMIC);
                if(skb2)
                {
                    if (DstPort[DP_GMAC1+maskindex] != NULL){
                        skb2->dev = DstPort[DP_GMAC1+maskindex];
                    skb2->dev->netdev_ops->ndo_start_xmit(skb2,skb2->dev);
                    }else{
                        dev_kfree_skb_any(skb2);
                    }
                }
            }
            else if(((NULL != ms_stats_p) && (0 == ms_stats_p->wifinum)) || ((NULL != hw_flood_entry) && (0 == wifi_flag))){
                skb->dev = DstPort[DP_GMAC1+maskindex];
            }
		}
	
		masktemp = sw_mul_port_mask & (1<<maskindex);
    }
    if(((NULL != ms_stats_p) && (ms_stats_p->wifinum > 0)) || ((NULL != hw_flood_entry) && (1 == wifi_flag)))
        return CONTINUE_TO_HANDLE_WIFI;
    else if(((NULL != ms_stats_p) && (0 == ms_stats_p->wifinum)) || ((NULL != hw_flood_entry) && (0 == wifi_flag)))
        return NOWIFI_JUST_RETURN_OUT;
    return CONTINUE_TO_HANDLE_WIFI;
}

int multicast_stag_handle(struct sk_buff *skb)
{
	uint8 *cp=NULL;
	uint16 sp_tag = 0;
	uint8 vpm = 0;

	cp = skb->data;
	sp_tag = ntohs(*(uint16 *)(cp+12));

	vpm = (sp_tag & 0x300) >> 8;

	switch(vpm)
	{
		case 0:/*untag*/
			memmove(skb->data+4, skb->data, 12);
			skb_pull(skb, 4);
			break;
			
		case 1:/*8100*/
			*(uint16 *)(cp+12) = htons(0x8100);
			break;

		case 2:/*88a8*/
		case 3:
			*(uint16 *)(cp+12) = htons(0x88a8);
			break;
			
		default:
			return -1;
	}

	return 0;
}

/*
case3:
rgmii0 connects QTN, rgmii1 connects HPNA: eth0.5<->10    eth0.6<->10
rgmii1 connexts QTN, rgmii0 connects HPNA: eth0.5<->5     eth0.6<->5   eth0.1<->0 
case2:
rgmii0 connects HPNA: eth0.1<->0
case1:
rgmii1 connects QTN:  eth0.5<->5  eth0.6<->5
*/
int get_lan_state(unsigned long mask, int *pt_enable, u8 *offloadClearMask, int *vlan_id, int *vpm, int *mcast, u8 *sw_port_offload_mask)
{
    int offloadflag = 0;
    offloadflag = (isolation_enable | (use_soc_lan << 1));
    switch(offloadflag){
        case 3:
            if(lan_port_map[5] == 0){    //rgmii0 connects QTN, rgmii1 connects HPNA: eth0.5<->10    eth0.6<->10
                if((mask & 0xfe) != 0){  //with external lan
                    *pt_enable = 1;
                    *offloadClearMask |= 0x1; //switch port 0 need clear
                    if((mask & 0x200000) != 0){
                        *mcast = 1;
                        *sw_port_offload_mask |= (1 << 5);      //eth0.6 need go offload path
                    }
                    if((mask & 0x100000) != 0){
                        *mcast = 1;
                        *sw_port_offload_mask |= (1 << 4);      //eth0.5 need go offload path
                    }
                }
                else{
                    *pt_enable = 0;
                    if((mask & 0x1) != 0){      //only soc lan
                        if(((mask & 0x200000) != 0) && ((mask & 0x100000) == 0)){   //only eth0.6
                            *vlan_id = 40;
                            *vpm = 1;
                        }
                        else if(((mask & 0x200000) == 0) && ((mask & 0x100000) != 0)){  //only eth0.5
                            *vlan_id = 1;
                            *vpm = 0;
                        }
                        else if(((mask & 0x200000) != 0) && ((mask & 0x100000) != 0)){  //eth0.6+eth0.5
                            *vlan_id = 1;
                            *vpm = 0;
                            *mcast = 1;
                            *sw_port_offload_mask |= (1 << 5);   //eth0.6 need go offload path
                        }
                    }
                    else{
                        *vlan_id = 1;
                        *vpm = 0;
                    }
                }                
            }
            else{               //rgmii1 connexts QTN, rgmii0 connects HPNA: eth0.5<->5     eth0.6<->5   eth0.1<->10 
                if((mask & 0xfe) != 0){  //with external lan
                    *pt_enable = 1;
                    *offloadClearMask |= 0x1;   //switch port0 need clear
                    if((mask & 0x1) !=0){   //soc lan + external lan
                        *mcast = 1;
                        *sw_port_offload_mask |= 1;      //eth0.1 need go offload path
                    }
                    if((mask & 0x200000) != 0){   //with eth0.6
                        if((mask & 0xde0000) == 0){    //only with eth0.6(do not care eth0.1)
                            *vlan_id= 40;
                            *vpm = 1;
                        }
                        else{
                            if((mask & 0x100000) == 0)  //not with eth0.5
                                *offloadClearMask |= (1 << lan_port_map[5]);  //switch port an_port_map[5] need clear
                            *vlan_id = 1;
                            *vpm = 0;
                            *mcast = 1;
                            *sw_port_offload_mask |= (1<<5); //eth0.6 need go offload path
                        }
                    }
                }
                else{       //without external lan
                    *pt_enable = 0;     
                    *vpm = 0;
                    *vlan_id = 1;
                }
            }
            break;
            
        case 2:
            if((mask & 0xfe) != 0){  //with external lan
                *pt_enable = 1;
                *offloadClearMask |= 0x1;   //switch port0 need clear
                if((mask & 0x1) !=0){   //soc lan + external lan
                    *mcast = 1;
                    *sw_port_offload_mask |= 1;      //eth0.1 need go offload path
                }
            }
            else{ 
                *pt_enable = 0;     //without external lan
            }
            *vpm = 0;
            *vlan_id = 1;
            break;
            
        case 1:                                 //case1:rgmii1 connects QTN:  eth0.5<->5  eth0.6<->5
            *pt_enable = 0;
            if((mask & 0x200000) != 0){   //with eth0.6
                if((mask & 0xdf0000) == 0){    //only with eth0.6
                    *vlan_id = 40;
                    *vpm = 1;
                }
                else{
                    if((mask & 0x100000) == 0)  //not with eth0.5
                        *offloadClearMask |= (1 << lan_port_map[5]);  //switch port lan_port_map[5] need clear
                    *vlan_id = 1;
                    *vpm = 0;
                    *mcast = 1;
                    *sw_port_offload_mask |= (1<<5); //eth0.6 need go offload path
                }
            }
            else{
                *vlan_id = 1;
                *vpm = 0;
            }
            break;
            
        case 0:
            *pt_enable = 0;
            *offloadClearMask = 0;
            *vlan_id = 1;
            *vpm = 0;
            break;
            
        default:
            break;
    }

    return 0;
}

int MulticastHandler(struct sk_buff *skb, struct FoeEntry *foe_entry,int typeflag,int handlerfrom)
{
	int loop;
	int masktemp = 1;//default is set to 1(must)
	unsigned long maskindex;
	struct sk_buff *skb2;
	int act_dp;
	IGMP_HWNATEntry_t* ms_stats_p;
    int mask = 0;
    int wifi_flag = 0;
    int wifi_num = 0;
    int wifi_mask = 0;
    int wifi_index = 0;
    int wifi_send_flag = 0;
    int  i = 0;
    multicast_flood_hwentry_t* hw_flood_entry = NULL;
	int ms_wifinum = 0;
	int ms_wifimask = 0;
	
	skb_set_network_header(skb, 0);

	skb_push(skb, ETH_HLEN);	//pointer to layer2 header

	//if multicast to cpu,the packet is modify by hw nat rule,wifi not need to stag,so need to to remove it
	if(!SUPPORT_STAG_FROM_INFO)
	{
		if(MULTOCPU == handlerfrom)
			multicast_stag_handle(skb);
	}

	rcu_read_lock();
	if (multicast_speed_find_entry_hook) 
		ms_stats_p = (IGMP_HWNATEntry_t *)multicast_speed_find_entry_hook(FOE_ENTRY_NUM(skb));
	else
	{
		ms_stats_p = NULL;
	}

    if(multicast_flood_find_entry_hook)
    {
        hw_flood_entry = (multicast_flood_hwentry_t *)multicast_flood_find_entry_hook(FOE_ENTRY_NUM(skb));
        if(NULL == hw_flood_entry)
        {
            wifi_flag = 0;
            wifi_num  = 0;
        }
        else
        {
            mask = hw_flood_entry->port_mask;
            wifi_flag = mask >> HWNAT_WLAN_IF_BASE ? 1 : 0;
            if(wifi_flag)
            {
                wifi_mask = (mask >> HWNAT_WLAN_IF_BASE) & HWNAT_WLAN_IF_MASK;
                for(i = 0;i < HWNAT_WLAN_IF_MAXNUM; i++)
                {
                    if(wifi_mask & (1 << i))
                    {
                        wifi_num++;
                    }
                }
            }
        }
    }
    else
    {
        wifi_flag = 0;
        wifi_num  = 0;
    }
		
    if(((use_soc_lan || isolation_enable) && (MULTOCPU == handlerfrom)) || 
        (TCSUPPORT_MULTI_SWITCH_EXT_VAL && ((MULTOCPU == handlerfrom)||(FORCETOCPU == handlerfrom)))){
        if(OffloadMLCHandler(skb, ms_stats_p, hw_flood_entry, wifi_flag)){
            rcu_read_unlock();
            return 1;
        }
    }

	if(NULL != ms_stats_p)
	{
		ms_wifinum = ms_stats_p->wifinum;
		ms_wifimask = ms_stats_p->mask;
	}
	else if(ECNT_HOOK_MULTICAST_GET_WIFI_NUM_AND_MASK(FOE_ENTRY_NUM(skb),&ms_wifinum,&ms_wifimask))
		wifi_flag = 1;
	else
		wifi_flag = 0;
	
	if((NULL != ms_stats_p) || (1 == wifi_flag))
	{
		if((1 == ms_wifinum) || (1 == wifi_num))//when wifinum == 1,not find ra interface according to multicast group information and get info according to act_dp directly
		{
			if(1 == typeflag)
			{
				act_dp = foe_entry->ipv4_hnapt.act_dp;
				skb->dev = DstPort[foe_entry->ipv4_hnapt.act_dp];
			}
			else
			{
				act_dp = foe_entry->ipv6_5t_route.act_dp;
				skb->dev = DstPort[foe_entry->ipv6_5t_route.act_dp];
			}
			
			if (skb->dev == NULL)
			{
				rcu_read_unlock();
				printk("\r\n%s:Error: act_dp= %d!",__FUNCTION__,act_dp);
				kfree_skb(skb);
				return 0;
			}
		}
		else if(ms_wifinum > 1)
		{
			masktemp = 0;
			for(maskindex = HWNAT_WLAN_IF_BASE,loop = 0; loop < ms_wifinum; maskindex++)
			{	
				if((ms_wifimask & (1<<maskindex)) == 0)
					continue;

				loop++;
				if(masktemp)
				{
					skb2 = skb_clone(skb, GFP_ATOMIC);
					if(skb2)
					{
						multicast_speed_set_dev(maskindex,skb2);
						if (skb2->dev != NULL && (skb2->dev->flags & IFF_UP))
						{
						    skb2->dev->netdev_ops->ndo_start_xmit(skb2,skb2->dev);
						}
						else
                            kfree_skb(skb2);
					}
				}
				else
				{
					multicast_speed_set_dev(maskindex,skb);
				}
			
				masktemp = ms_wifimask & (1<<maskindex);	
			}	
		}
		else if ((NULL != ms_stats_p) && (0 == ms_wifinum))
		{
			rcu_read_unlock();
			kfree_skb(skb);
			return 0;
		}
        else if(1 < wifi_num)
        {
            wifi_send_flag = 0;
           
            for(i = 0,loop = 0; loop < wifi_num; i++)
            {
                if(0 == (wifi_mask & (1 << i)))
                {
                    continue ;
                }
                loop++;
                wifi_index = HWNAT_WLAN_IF_BASE + i;
                if(wifi_send_flag)
                {
                    skb2 = skb_clone(skb, GFP_ATOMIC);
                    if(skb2)
                    {
                        multicast_speed_set_dev(wifi_index,skb2);
                    	if (skb2->dev != NULL && (skb2->dev->flags & IFF_UP))
                    	{
	                        skb2->dev->netdev_ops->ndo_start_xmit(skb2,skb2->dev);
                    	}
                        else{
                            kfree_skb(skb2);
                        }
                    }
                    
                }
                else
                {
                    multicast_speed_set_dev(wifi_index,skb);
                }
                wifi_send_flag = wifi_mask & (1 << i);
            }
        }
	}	

	rcu_read_unlock();
	return 1;
}

int PpeHitBindMULToCpuHanler(struct sk_buff *skb, struct FoeEntry *foe_entry)
{
	int masktemp = 1;//default is set to 1(must)
	int typeflag;
	uint32 dest_port = 0;
	
	if (TCSUPPORT_VLAN_ACCESS_TRUNK_VAL)
	{
		switch(foe_entry->bfib1.pkt_type)
		{
			case L2_BRIDGE:
				dest_port = foe_entry->l2_bridge32.act_dp;
				break;
			case IPV4_HNAPT:
			case IPV4_HNAT:
				dest_port = foe_entry->ipv4_hnapt.act_dp;
				break;
			case IPV4_DSLITE:
				dest_port = foe_entry->ipv4_dslite.act_dp;
				break;
			case IPV6_3T_ROUTE:
				dest_port = foe_entry->ipv6_3t_route.act_dp;
				break;
			case IPV6_5T_ROUTE:
				dest_port = foe_entry->ipv6_5t_route.act_dp;
				break;
			case IPV6_6RD:
				dest_port = foe_entry->ipv6_6rd.act_dp;
				break;
			default:
				break;
		}
	}

	if (TCSUPPORT_VLAN_ACCESS_TRUNK_VAL && (dest_port >= DP_MCAST_NAS0) && (dest_port <= DP_MCAST_PPP7))
	{
		multicast_vlan_packet_handle(skb, dest_port);
	}
	else
	{
		if(!multicast_speed_find_entry_hook&&!multicast_flood_find_entry_hook)
		{
			ecnt_multicast_hwnat_mul_to_cpu_handler(skb,foe_entry);
			masktemp = 0;
		}
		else
		{
			typeflag = multicast_speed_is_multicast_entry(foe_entry);
			if(typeflag)
				masktemp = MulticastHandler(skb,foe_entry,typeflag,MULTOCPU);		
		}
		
		if(masktemp)
		{
			if (skb->dev == NULL || (!(skb->dev->flags & IFF_UP)))
			{
				kfree_skb(skb);
				return 0;
			}

			skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
		}
	}
	
	return 0;
}

/*
    get mask of sptag from software lan port id mask
    sw_port_offload_mask: 8 bit map mapping to 
    mask 0~7 bit mapping lan port 1~8 normall, eth0.1~0.4 ~0.8(0.8 for furture develop) 
*/
int hwnat_update_eth_sptag(unsigned long *mask, u8 *sw_port_offload_mask, int *mcast)
{
	unsigned long sptag_mask = 0 ; 
	unsigned long tag = (*mask) & 0xff ;
	unsigned int i = 0;
	unsigned int offset = 0;
	
	if(tag == 0){
		return 0;
	}
	
	for(i = 0; i < 8; i ++)
	{
		if((tag)&(1<<i))
		{
			offset = macMT7530LanPortMap2Switch(i);
			if( offset < 6)
				sptag_mask |= (1 << offset);
		}
	}

	if(TCSUPPORT_MULTI_SWITCH_EXT_VAL)
	{
		if((tag & inter_switch_mask)!=0){ //with inter switch port
			if((tag & extend_switch_mask)!=0){//inter and entend switch
				*mcast = 1;
				*sw_port_offload_mask = tag & extend_switch_mask;
			}else{//only with inter switch
				*mcast = 0;
				*sw_port_offload_mask = 0;
			}
		}else{ //only with extend switch port
			*mcast = 0;
			*sw_port_offload_mask = tag & extend_switch_mask;
		}
	}
	
	(*mask) = (*mask) & (~HWNAT_LAN_IF_MASK);
	(*mask) = (*mask) | (sptag_mask & HWNAT_LAN_IF_MASK);
	return 0;
}

int hwnat_set_rule_according_to_state(int index,int state,unsigned long mask)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	int interfaceindex = 0;
	unsigned long masktemp = 0;
	unsigned char isCTVer = (TCSUPPORT_CT_VAL ? 1 : 0);
	int need_ds_offload = 0;
    int force_dstq = 0;
    int pt_enable = 0;
    u8  offloadClearMask = 0;
    int vpm = 0;
    int vlan_id = 0;
    int mcast = 0;
    sw_port_offload_mask = 0;

    if(use_soc_lan || isolation_enable)
        get_lan_state(mask, &pt_enable, &offloadClearMask, &vlan_id, &vpm, &mcast, &sw_port_offload_mask);

	hwnat_update_eth_sptag(&mask, &sw_port_offload_mask, &mcast);
    masktemp = mask;
	//spin_lock_irqsave(&hw_nat_multicastspeed_lock, lock_flag);	
	if (index <0 || index >= FOE_4TB_SIZ_TOTAL)
	{
		//spin_unlock_irqrestore(&hw_nat_multicastspeed_lock, lock_flag);
		printk("\r\n[%s]the index is beyond the limit!",__FUNCTION__);
		return 0;
	}


	foe_entry = getPpeEntryLocal(index, foe_entry);
	if(foe_entry == NULL)
		return -1;

    ETHER_API_GET_FORCE_DSTQ(&force_dstq);

    if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&(dsoffload_presetting&DS_MULTICAST))
    {
            /* for en7512 soft qdma, need ds offload even for 1G port */
            need_ds_offload = 1;
    }
	switch(state)
	{
		case MULTICAST_SPEED_STATE_I:
		case MULTICAST_SPEED_STATE_III:
			//whenever there are wlan interfaces which are including in multicast group,set one to act_dp(when state change(hw nat rule change),cpe will not crash)
			masktemp = (masktemp >> HWNAT_WLAN_IF_BASE) & HWNAT_WLAN_IF_MASK;
			while(masktemp)
			{
				if(masktemp&(1<<interfaceindex))
				{
					break;
				}
				interfaceindex++;
			}
			if(foe_entry->bfib1.pkt_type < 2)
			{
				if(!masktemp)
				{
					foe_entry->ipv4_hnapt.act_dp = 0;
				}
				else if(interfaceindex >= HWNAT_WLAN_IF_NUM + HWNAT_WLAN_IF_I_NUM)
				{
					foe_entry->ipv4_hnapt.act_dp = DP_RAX0 + interfaceindex - HWNAT_WLAN_IF_NUM - HWNAT_WLAN_IF_I_NUM;
				}
				else if(interfaceindex >= HWNAT_WLAN_IF_NUM)
				{
					foe_entry->ipv4_hnapt.act_dp = DP_RAI0 + interfaceindex - HWNAT_WLAN_IF_NUM;
				}
				else
				{
					foe_entry->ipv4_hnapt.act_dp = DP_RA0 + interfaceindex;
				}
				
				if(MULTICAST_SPEED_STATE_I == state)
				{
					//set mcast and act_dp
					foe_entry->ipv4_hnapt.mcast = 1;

					//set stag for lan	
					foe_entry->ipv4_hnapt.etype &= 0xff80;//clear low 7 bits
					foe_entry->ipv4_hnapt.etype |= 0x8000/*1<<15*/;//bit 15 always is 1(stag)
					foe_entry->ipv4_hnapt.etype |= (0xff&mask);
                    if(use_soc_lan || isolation_enable){
                        foe_entry->ipv4_hnapt.etype &= ~offloadClearMask;//update offload port
                        foe_entry->ipv4_hnapt.etype |= ((pt_enable << 7));//update PT

                        foe_entry->ipv4_hnapt.etype &= 0xf0ff; //clear vpm;
                        foe_entry->ipv4_hnapt.etype |= (vpm << 8); //update vpm;                   
                    }
					if(TCSUPPORT_MULTI_SWITCH_EXT_VAL){
                        foe_entry->ipv4_hnapt.mcast = mcast;  
                        foe_entry->ipv4_hnapt.etype &= 0xf0ff; //clear vpm
                    }
					
		if(!TCSUPPORT_CG_DS_ADD_TAG_VAL)
		{
			if ( 0 == isCTVer ){
				if(isolation_enable)
					foe_entry->ipv4_hnapt.vlan1 = vlan_id;
			}
			
			if(!SUPPORT_L2_INFO_CFG)
				foe_entry->ipv4_hnapt.bfib1.vlan_layer = 1;
		}
				
                    if(!(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && (dsoffload_presetting & DS_MULTICAST)))
                    {
                        if  (1 != force_dstq){
							if( TCSUPPORT_MULTI_SWITCH_EXT_VAL && ((foe_entry->ipv4_hnapt.etype & 0x3f)== 0) ){
								 foe_entry->ipv4_hnapt.mcast = 0;
								 foe_entry->ipv4_hnapt.vlan1 = 0x0;
								 foe_entry->ipv4_hnapt.etype &= ~0x8000/*1<<15*/;//bit 15 should be 0
								 foe_entry->ipv4_hnapt.bfib1.vlan_layer = 0x0;
								 PpeSetInfoBlk2(foe_entry, FP_QDMA1_SW, 0x3F, 0x3F, 1);
							 }else
								 PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);
						}
                    }
                    else
                    {
                        if(need_ds_offload)
                        {
                            multi_set_ds_offload(foe_entry,index);
                        }
                    }
				}
				else
				{
					//set mcast and act_dp
					foe_entry->ipv4_hnapt.mcast = 0;
					
					//clear stag for lan
					foe_entry->ipv4_hnapt.etype &= 0xff80;
					foe_entry->ipv4_hnapt.vlan1 = 0x0;
					foe_entry->ipv4_hnapt.etype &= ~0x8000/*1<<15*/;//bit 15 should be 0
					foe_entry->ipv4_hnapt.bfib1.vlan_layer = 0x0;
					foe_entry->ipv4_hnapt.fqos = 0;

                        			PpeSetInfoBlk2(foe_entry, FP_QDMA1_SW, 0x3F, 0x3F, 1);
    
				}		
			}
			else
			{
				if(!masktemp)
				{
					foe_entry->ipv6_5t_route.act_dp = 0;
				}
				else if(interfaceindex >= HWNAT_WLAN_IF_NUM + HWNAT_WLAN_IF_I_NUM)
				{
					foe_entry->ipv6_5t_route.act_dp = DP_RAX0 + interfaceindex - HWNAT_WLAN_IF_NUM - HWNAT_WLAN_IF_I_NUM;
				}
				else if(interfaceindex >= HWNAT_WLAN_IF_NUM)
				{
					foe_entry->ipv6_5t_route.act_dp = DP_RAI0 + interfaceindex - HWNAT_WLAN_IF_NUM;
				}
				else
				{
					foe_entry->ipv6_5t_route.act_dp = DP_RA0 + interfaceindex;
				}

				if(MULTICAST_SPEED_STATE_I == state)
				{
					//set mcast and act_dp
					foe_entry->ipv6_5t_route.mcast = 1;
					
					//set stag for lan	
					foe_entry->ipv6_5t_route.etype &= 0xff80;//clear low 7 bits
					foe_entry->ipv6_5t_route.etype |= 0x8000/*1<<15*/;//bit 15 always is 1(stag)
					foe_entry->ipv6_5t_route.etype |= (0xff&mask);
                    if(use_soc_lan || isolation_enable){    
                        foe_entry->ipv6_5t_route.etype &= ~offloadClearMask;//update oddload port 
                        foe_entry->ipv6_5t_route.etype &= 0xff7f; //clear PT;
                        foe_entry->ipv6_5t_route.etype |= ((pt_enable << 7));//update PT

                        foe_entry->ipv6_5t_route.etype &= 0xf0ff; //clear vpm
                        foe_entry->ipv6_5t_route.etype |= (vpm << 8); //update vpm                   
                    }
					if(TCSUPPORT_MULTI_SWITCH_EXT_VAL){
                        foe_entry->ipv6_5t_route.mcast = mcast;  
                        foe_entry->ipv6_5t_route.etype &= 0xf0ff; //clear vpm
                    }
		if(!TCSUPPORT_CG_DS_ADD_TAG_VAL)
		{
			if ( 0 == isCTVer ){
                        		if(isolation_enable)
                            		foe_entry->ipv6_5t_route.vlan1 = vlan_id;
                    	}
			
                    	if(!SUPPORT_L2_INFO_CFG)
                        		foe_entry->ipv6_5t_route.bfib1.vlan_layer = 1;
		}
                    if (!(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && (dsoffload_presetting & DS_MULTICAST)))
                    {
                        if (1 != force_dstq){
                            if(TCSUPPORT_MULTI_SWITCH_EXT_VAL && ((foe_entry->ipv6_5t_route.etype & 0x3f)== 0)){
            					foe_entry->ipv6_5t_route.mcast = 0;
                                foe_entry->ipv6_5t_route.vlan1 = 0x0;
            					foe_entry->ipv6_5t_route.etype &= ~0x8000/*1<<15*/;//bit 15 should be 0
            					foe_entry->ipv6_5t_route.bfib1.vlan_layer = 0x0;
                                PpeSetInfoBlk2(foe_entry, FP_QDMA1_SW, 0x3F, 0x3F, 1);
                            }else
                                PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);
						}
                            
                    }
                    else
                    {
                        if(need_ds_offload)
                        {
                            multi_set_ds_offload(foe_entry,index);
                        }
                    }
				}
				else
				{	
					//set mcast and act_dp
					foe_entry->ipv6_5t_route.mcast = 0;
					//set stag for lan
					foe_entry->ipv6_5t_route.etype &= 0xff80;
					foe_entry->ipv6_5t_route.vlan1 = 0x0;
					foe_entry->ipv6_5t_route.etype &= ~0x8000/*1<<15*/;//bit 15 should be 0
					foe_entry->ipv6_5t_route.bfib1.vlan_layer = 0x0;
					foe_entry->ipv6_5t_route.fqos = 0;
					
                        			PpeSetInfoBlk2(foe_entry, FP_QDMA1_SW, 0x3F, 0x3F, 1);

				}
			}
			if(FOE_NBQ_CFG_SUPPORT) {
				if(((masktemp>>HWNAT_WLAN_IF_BASE)>>HWNAT_WLAN_IF_NUM) & HWNAT_RAI_IF_MASK){/*5G send to ring-6*/
					if(dualband_interrupt_enable)
						PpeSetRxForceRingIdx(foe_entry, index & 4 ? RING_IDX_6 : RING_IDX_5);
					else
						PpeSetRxForceRingIdx(foe_entry, RING_IDX_6);
				} else {/*2.4G send to ring-5*/
					PpeSetRxForceRingIdx(foe_entry,RING_IDX_5);
				}
			}
			break;

		case MULTICAST_SPEED_STATE_II:
			if(foe_entry->bfib1.pkt_type < 2)
			{
				//set mcast and act_dp
				foe_entry->ipv4_hnapt.mcast = 0;
				foe_entry->ipv4_hnapt.act_dp = 0;//not useful for pure lan
				//set stag for lan	
				foe_entry->ipv4_hnapt.etype &= 0xff80;//clear low 7 bits
				foe_entry->ipv4_hnapt.etype |= 0x8000/*1<<15*/;//bit 15 always is 1(stag)
				if( TCSUPPORT_CT_SIMCARD_SEPARATION_VAL && TCSUPPORT_CT_2PORTS_VAL && !snoopingSpeed_en){
					mask = 0x18;//flooding to all port
				}
				foe_entry->ipv4_hnapt.etype |= (0xff&mask);
                if(use_soc_lan || isolation_enable){
                    foe_entry->ipv4_hnapt.etype &= ~offloadClearMask;//update offload port                  
					foe_entry->ipv4_hnapt.mcast = mcast;//set mcast
                    foe_entry->ipv4_hnapt.etype &= 0xff7f; //clear PT;
                    foe_entry->ipv4_hnapt.etype |= ((pt_enable << 7));//update PT

                    foe_entry->ipv4_hnapt.etype &= 0xf0ff; //clear vpm
                    foe_entry->ipv4_hnapt.etype |= (vpm << 8); //update vpm;     
                }
				if(TCSUPPORT_MULTI_SWITCH_EXT_VAL){
                    foe_entry->ipv4_hnapt.mcast = mcast;  
                    foe_entry->ipv4_hnapt.etype &= 0xf0ff; //clear vpm
                }
				
		if(!TCSUPPORT_CG_DS_ADD_TAG_VAL)
		{
			if ( 0 == isCTVer ){
				if(isolation_enable)
				foe_entry->ipv4_hnapt.vlan1 = vlan_id;
			}
			
			if(!SUPPORT_L2_INFO_CFG)
				foe_entry->ipv4_hnapt.bfib1.vlan_layer = 1;
		}
		
                if(!(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && (dsoffload_presetting & DS_MULTICAST)))
                {         
                    if (1 != force_dstq){
                        if(TCSUPPORT_MULTI_SWITCH_EXT_VAL &&((foe_entry->ipv4_hnapt.etype & 0x3f)== 0)){
                            foe_entry->ipv4_hnapt.vlan1 = 0x0;
        					foe_entry->ipv4_hnapt.etype &= ~0x8000/*1<<15*/;//bit 15 should be 0
        					foe_entry->ipv4_hnapt.bfib1.vlan_layer = 0x0;
                            PpeSetInfoBlk2(foe_entry, FP_QDMA1_SW, 0x3F, 0x3F, 1);
                        }else
                            PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);
					}

                }
                else
                {
                    if(need_ds_offload)
                    {
                        multi_set_ds_offload(foe_entry,index);
                    }
                }
                if(FOE_NBQ_CFG_SUPPORT) {
                    set_info2_nbq(foe_entry, IPV4_HNAPT, foe_entry->ipv4_hnapt.channel);
                }
			}
			else
			{
				//set mcast and act_dp
				foe_entry->ipv6_5t_route.mcast = 0;
				foe_entry->ipv6_5t_route.act_dp = 0;//not useful for pure lan
				
				//set stag for lan	
				foe_entry->ipv6_5t_route.etype &= 0xff80;//clear low 7 bits
				foe_entry->ipv6_5t_route.etype |= 0x8000/*1<<15*/;//bit 15 always is 1(stag)
				if( TCSUPPORT_CT_SIMCARD_SEPARATION_VAL && TCSUPPORT_CT_2PORTS_VAL && !snoopingSpeed_en){
					mask = 0x18;//flooding to all port
				}
				foe_entry->ipv6_5t_route.etype |= (0xff&mask);
                if(use_soc_lan || isolation_enable){
                    foe_entry->ipv6_5t_route.etype &= ~offloadClearMask;//update offload port
					foe_entry->ipv6_5t_route.mcast = mcast;//set mcast
    					
                    foe_entry->ipv6_5t_route.etype &= 0xff7f; //clear PT;
                    foe_entry->ipv6_5t_route.etype |= ((pt_enable << 7));//update PT

                    foe_entry->ipv6_5t_route.etype &= 0xf0ff; //clear vpm
                    foe_entry->ipv6_5t_route.etype |= (vpm << 8); //update vpm;   
                }
				if(TCSUPPORT_MULTI_SWITCH_EXT_VAL){
                    foe_entry->ipv6_5t_route.mcast = mcast;  
                    foe_entry->ipv6_5t_route.etype &= 0xf0ff; //clear vpm
                }

		if(!TCSUPPORT_CG_DS_ADD_TAG_VAL)
		{
			if ( 0 == isCTVer ){
				if(isolation_enable)
					foe_entry->ipv6_5t_route.vlan1 = vlan_id;
			}
			
			if(!SUPPORT_L2_INFO_CFG)
				foe_entry->ipv6_5t_route.bfib1.vlan_layer = 1;
		}

                if(!(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL && (dsoffload_presetting & DS_MULTICAST)))
                {
                    if (1 != force_dstq){
                        if(TCSUPPORT_MULTI_SWITCH_EXT_VAL && ((foe_entry->ipv6_5t_route.etype & 0x3f)== 0)){
        					foe_entry->ipv6_5t_route.vlan1 = 0x0;
        					foe_entry->ipv6_5t_route.etype &= ~0x8000/*1<<15*/;//bit 15 should be 0
        					foe_entry->ipv6_5t_route.bfib1.vlan_layer = 0x0;
                            PpeSetInfoBlk2(foe_entry, FP_QDMA1_SW, 0x3F, 0x3F, 1);
                        }else
                            PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);
					}

                }
                else
                {
                    if(need_ds_offload)
                    {
                        multi_set_ds_offload(foe_entry,index);
                    }
                }
                if(FOE_NBQ_CFG_SUPPORT) {
                    set_info2_nbq(foe_entry, IPV6_5T_ROUTE, foe_entry->ipv6_5t_route.channel);
                }
            }
			break;	

		case MULTICAST_SPEED_STATE_IV:	
			memset(foe_entry, 0, SIZE_OF_FOE_ENTRY);
			break;

		//do nothing
		default:
			printk("\r\n[%s]Not support such state:%d",__FUNCTION__,state);
			break;
			
	}

    PpeSetInfoBlk2_Acnt0(foe_entry, HWNAT_MULTI_ACNTGRP);

	if(SUPPORT_SRAM_ENTRY && (index<hwnat_dram_idx_base)) {
		setPpeEntry(index, foe_entry);
	}

	//spin_unlock_irqrestore(&hw_nat_multicastspeed_lock, lock_flag);

	return 1;
}


/**************old mulitcast end********************************/


/**************wan account start*******************************/
void PpeSetWanAcntId(struct sk_buff *skb, uint8_t wan_index, uint8_t dir)
{
    struct FoeEntry foe_entry_local;
    struct FoeEntry *foe_entry=&foe_entry_local;
    uint32_t foe_entry_idx=0;
    unsigned long lock_flag;
    _info_blk2 *iblk2;
    uint8_t is_multicast_entry = 0;
    uint8_t acnt_mode = 0;
    uint8_t acnt_idx = 0;

    if(wan_index > 7)
        return;

	if(isEN7528)
		return;

    if((UPSTREAM_ONLY != dir)&&(DOWNSTREAM_ONLY != dir))
        return;

	if((!SUPPORT_FLOW_ACNT) && (FOE_AI(skb) != HIT_UNBIND_RATE_REACH))
		return;
    
    if(isEN751627){
        PpeGetFlowAcntInfo(skb, wan_index, dir);
        return;
    }
        
	foe_entry_idx = FOE_ENTRY_NUM(skb);
	foe_entry = getPpeEntryLocal(foe_entry_idx, foe_entry);
	if(foe_entry == NULL)
		return;

    spin_lock_irqsave(&hw_nat_lock, lock_flag);

    if (IS_IPV4_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv4_hnapt.info_blk2);
    else if (IS_IPV6_GRP(foe_entry))
        iblk2 = (_info_blk2 *)(&foe_entry->ipv6_5t_route.info_blk2);
    else if (IS_L2_RRIDGE(foe_entry))
            iblk2 = (_info_blk2 *)(&foe_entry->l2_bridge32.info_blk2);
    else{
        spin_unlock_irqrestore(&hw_nat_lock, lock_flag);
        return;
    }

    is_multicast_entry = xpon_is_multicast_entry(foe_entry);

	if(TCSUPPORT_ACCOUNT_METER_V2_VAL && fe_resource_mark_wan_idx_hook){
		if(is_multicast_entry) // bit7 use for multicast flag, bit0~6 use for wan_index
			wan_index |= (0x1<<7);
		if(DebugLevel >=1 )
			printk("%s %d multicast flag is %d, wan_index is %d\n",__FUNCTION__,__LINE__, is_multicast_entry, wan_index);
		fe_resource_mark_wan_idx_hook(skb, wan_index, dir);
	}else{
	    if(SUPPORT_FLOW_ACNT){
	        if(UPSTREAM_ONLY == dir)
	            FE_API_GET_ACNT1_MODE(UP_STREAM, &acnt_mode);
	        else
	            FE_API_GET_ACNT1_MODE(DOWN_STREAM, &acnt_mode);

	        if(WAN_ITF_MODE == acnt_mode){
	            acnt_idx = wan_index*2 + is_multicast_entry + 1;

	            skb->fe_resource_mark &= ~(ACNT1_MASK) ;
	            skb->fe_resource_mark |= ((acnt_idx << ACNT1_OFFSET) & ACNT1_MASK);

	            if(is_multicast_entry)
				{
	                acnt_idx = wan_index*2 + is_multicast_entry;
	                iblk2->port_ag &= ~(HWNAT_DEFAULT_ACNT1GRP << ACNT0_MASK_LEN);
	                iblk2->port_ag |= ((acnt_idx & HWNAT_DEFAULT_ACNT1GRP) << ACNT0_MASK_LEN);
	            }
	        } else if(WAN_ITF_AND_TRAFFIC_FLOW_MODE == acnt_mode) {
	            acnt_idx = wan_index + 1;

	            skb->fe_resource_mark &= ~(ACNT1_MASK) ;
	            skb->fe_resource_mark |= ((acnt_idx << ACNT1_OFFSET) & ACNT1_MASK);
	        }
	    }else{
	        iblk2->port_ag = ACNT_GRP_SMUX_CNT(wan_index, dir, is_multicast_entry);
	    }
	}

    if(SUPPORT_SRAM_ENTRY && (foe_entry_idx<hwnat_dram_idx_base)) {
        setPpeEntry(foe_entry_idx, foe_entry);
    }

    spin_unlock_irqrestore(&hw_nat_lock, lock_flag);

    return;
}

void PpeGetFlowAcntInfo(struct sk_buff *skb, uint8_t wan_index, uint8_t dir)
{
    FOE_FLOW_ACNT_EN(skb) = PPE_ENABLE;
    FOE_FLOW_ACNT_IDX(skb) = wan_index*2 + dir;

    return;
}

void PpeClearWanAcntId(uint8_t wan_index)
{
	uint8_t acnt_mode = 0;
    
	if(wan_index < 8)
	{
	    if(SUPPORT_FLOW_ACNT){
            FE_API_GET_ACNT1_MODE(UP_STREAM, &acnt_mode);
            if(acnt_mode == WAN_ITF_MODE) {
                FE_API_CLEAR_FlOW_CNT((wan_index*2), UP_STREAM, acnt_mode);
			    FE_API_CLEAR_FlOW_CNT((wan_index*2+1), UP_STREAM, acnt_mode);
            } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){
                FE_API_CLEAR_FlOW_CNT(wan_index, UP_STREAM, acnt_mode);
            }
            
            FE_API_GET_ACNT1_MODE(DOWN_STREAM, &acnt_mode);
            if(acnt_mode == WAN_ITF_MODE) {
    			FE_API_CLEAR_FlOW_CNT((wan_index*2), DOWN_STREAM, acnt_mode);
    			FE_API_CLEAR_FlOW_CNT((wan_index*2+1), DOWN_STREAM, acnt_mode);
            } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){
                FE_API_CLEAR_FlOW_CNT(wan_index, DOWN_STREAM, acnt_mode);
            }
        }else{
		//read clear
    		IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wan_index, 1, 0));
    		IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wan_index, 0, 0));
    		IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wan_index, 1, 1));
    		IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wan_index, 0, 1));
    		IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wan_index, 1, 0));
    		IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wan_index, 1, 0));
    		IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wan_index, 0, 0));
    		IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wan_index, 0, 0));
    		IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wan_index, 1, 1));
    		IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wan_index, 1, 1));
    		IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wan_index, 0, 1));
    		IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wan_index, 0, 1));
        }
		memset(ppe_acnt_grp+wan_index,0,sizeof(ppeAcntGrp_t));
	}	
}

void PpeGetWanAcntIdCounter(unsigned char wan_index, struct net_device_stats *storage)
{
	uint8_t acnt_mode = 0;
    
    if((wan_index >= 8) || (NULL == storage))
    {
        return ;
    }
    if(SUPPORT_FLOW_ACNT){

        unsigned int uniUpxCnt = 0, uniUpxCnt_hi = 0;
        unsigned int uniDownCnt = 0, uniDownCnt_hi = 0;    
        uint uni_Bytes_lo = 0, uni_Bytes_hi = 0;
        uint uni_UP_byte = 0, mul_UP_byte = 0;
        uint uni_DOWN_byte = 0, mul_DOWN_byte = 0;
        unsigned int mulUpxCnt = 0, mulUpxCnt_hi = 0;
        unsigned int mulDownCnt = 0, mulDownCnt_hi = 0;    
        uint mul_Bytes_lo = 0, mul_Bytes_hi = 0;
        
        FE_API_GET_ACNT1_MODE(UP_STREAM, &acnt_mode);
        if(acnt_mode == WAN_ITF_MODE) {
            FE_API_GET_FlOW_PKT_CNT(wan_index*2, UP_STREAM, acnt_mode,&uniUpxCnt, &uniUpxCnt_hi);
            FE_API_GET_FlOW_CNT(wan_index*2, UP_STREAM, acnt_mode, &uni_Bytes_lo, &uni_Bytes_hi);
            uni_UP_byte = uni_Bytes_lo;
            
            FE_API_GET_FlOW_PKT_CNT(wan_index*2+1 , UP_STREAM, acnt_mode, &mulUpxCnt, &mulUpxCnt_hi);
            FE_API_GET_FlOW_CNT(wan_index*2+1 , UP_STREAM, acnt_mode, &mul_Bytes_lo, &mul_Bytes_hi);
            mul_UP_byte = mul_Bytes_lo;
        } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){
            FE_API_GET_FlOW_PKT_CNT(wan_index, UP_STREAM, acnt_mode, &uniUpxCnt, &uniUpxCnt_hi);
            FE_API_GET_FlOW_CNT(wan_index, UP_STREAM, acnt_mode, &uni_Bytes_lo, &uni_Bytes_hi);
            uni_UP_byte = uni_Bytes_lo;
            mul_UP_byte = mulUpxCnt = 0;
        }
        
        FE_API_GET_ACNT1_MODE(DOWN_STREAM, &acnt_mode);
        if(acnt_mode == WAN_ITF_MODE) {
            FE_API_GET_FlOW_PKT_CNT(wan_index*2, DOWN_STREAM, acnt_mode, &uniDownCnt, &uniDownCnt_hi);
            FE_API_GET_FlOW_CNT(wan_index*2, DOWN_STREAM, acnt_mode, &uni_Bytes_lo, &uni_Bytes_hi);
            uni_DOWN_byte = uni_Bytes_lo;        
            
            FE_API_GET_FlOW_PKT_CNT(wan_index*2+1, DOWN_STREAM, acnt_mode, &mulDownCnt, &mulDownCnt_hi);
            FE_API_GET_FlOW_CNT(wan_index*2+1, DOWN_STREAM, acnt_mode, &mul_Bytes_lo, &mul_Bytes_hi);
            mul_DOWN_byte = mul_Bytes_lo;
        } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){
            FE_API_GET_FlOW_PKT_CNT(wan_index, DOWN_STREAM, acnt_mode, &uniDownCnt, &uniDownCnt_hi);
            FE_API_GET_FlOW_CNT(wan_index, DOWN_STREAM, acnt_mode, &uni_Bytes_lo, &uni_Bytes_hi);
            uni_DOWN_byte = uni_Bytes_lo;
            mul_DOWN_byte = mulDownCnt = 0;
        }

        storage->rx_packets = uniDownCnt + mulDownCnt;
        storage->tx_packets = uniUpxCnt + mulUpxCnt;
        storage->rx_bytes = uni_DOWN_byte + mul_DOWN_byte;
        storage->tx_bytes = uni_UP_byte + mul_UP_byte;

    }else{
        storage->rx_packets +=  (IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wan_index, 1, 0)) + IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wan_index, 1, 1)));
        storage->tx_packets +=  (IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wan_index, 0, 0)) + IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wan_index, 0, 1)));
        storage->rx_bytes   +=  (((((unsigned long long)(IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wan_index, 1, 0))))<<32)+IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wan_index, 1, 0))) \
                                    + ((((unsigned long long)(IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wan_index, 1, 1))))<<32)+IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wan_index, 1, 1))));
        storage->tx_bytes   +=  (((((unsigned long long)(IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wan_index, 0, 0))))<<32)+IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wan_index, 0, 0))) \
                                    + ((((unsigned long long)(IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wan_index, 0, 1))))<<32)+IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wan_index, 0, 1))));
    } 
    return ;
}


#ifdef CONFIG_TP_IMAGE
void PpeGetWanAcntIdCounterTp(unsigned char wan_index, struct net_device_stats *storage)
{
	uint8_t acnt_mode = 0;
	struct hwnat_wan_acnt_args opt;
	memset(&opt,0,sizeof(struct hwnat_wan_acnt_args));

	if((wan_index >= 8) || (NULL == storage))
	{
		return ;
	}

	opt.wanid = wan_index;
	opt.action = PPE_METHOD_READ;

	PpeWanACnt(&opt);

	storage->rx_packets = opt.wan_acnt.mulRxCnt + opt.wan_acnt.uniRxCnt;
	storage->tx_packets = opt.wan_acnt.mulTxCnt + opt.wan_acnt.uniTxCnt;
	storage->rx_bytes = opt.wan_acnt.mulRxBytes + opt.wan_acnt.uniRxBytes;
	storage->tx_bytes = opt.wan_acnt.mulTxBytes + opt.wan_acnt.uniTxBytes;

#ifdef CONFIG_TP_DEV_EXTSTATS
	storage->rx_multicast_packets = opt.wan_acnt.mulRxCnt;
	storage->tx_multicast_packets = opt.wan_acnt.mulTxCnt;
	storage->rx_multicast_bytes = opt.wan_acnt.mulRxBytes;
	storage->tx_multicast_bytes = opt.wan_acnt.mulTxBytes;
	storage->rx_unicast_packets = opt.wan_acnt.uniRxCnt;
	storage->tx_unicast_packets = opt.wan_acnt.uniTxCnt;
	storage->rx_unicast_bytes = opt.wan_acnt.uniRxBytes;
	storage->tx_unicast_bytes = opt.wan_acnt.uniTxBytes;
#endif /* CONFIG_TP_DEV_EXTSTATS */

	return ;
}

void PpeGetWanAcntIdCounterTp64(unsigned char wan_index, struct rtnl_link_stats64* stats)
{
	uint8_t acnt_mode = 0;
	struct hwnat_wan_acnt_args opt;
	memset(&opt,0,sizeof(struct hwnat_wan_acnt_args));

	if((wan_index >= 8) || (NULL == stats))
	{
		return ;
	}

	opt.wanid = wan_index;
	opt.action = PPE_METHOD_READ;

	PpeWanACnt(&opt);
	
	stats->rx_packets = opt.wan_acnt.mulRxCnt + opt.wan_acnt.uniRxCnt;
	stats->tx_packets = opt.wan_acnt.mulTxCnt + opt.wan_acnt.uniTxCnt;
	stats->rx_bytes = opt.wan_acnt.mulRxBytes + opt.wan_acnt.uniRxBytes;
	stats->tx_bytes = opt.wan_acnt.mulTxBytes + opt.wan_acnt.uniTxBytes;

	#ifdef CONFIG_TP_DEV_EXTSTATS
	stats->rx_multicast_packets = opt.wan_acnt.mulRxCnt;
	stats->tx_multicast_packets = opt.wan_acnt.mulTxCnt;
	stats->rx_multicast_bytes = opt.wan_acnt.mulRxBytes;
	stats->tx_multicast_bytes = opt.wan_acnt.mulTxBytes;
	stats->rx_unicast_packets = opt.wan_acnt.uniRxCnt;
	stats->tx_unicast_packets = opt.wan_acnt.uniTxCnt;
	stats->rx_unicast_bytes = opt.wan_acnt.uniRxBytes;
	stats->tx_unicast_bytes = opt.wan_acnt.uniTxBytes;
	#endif /* CONFIG_TP_DEV_EXTSTATS */

	return ;
}

void PpeClearWanAcntIdCounter(unsigned char wan_index)
{
	uint8_t acnt_mode = 0;
	struct hwnat_wan_acnt_args opt;
	memset(&opt,0,sizeof(struct hwnat_wan_acnt_args));

	if((wan_index >= 8))
	{
		return ;
	}

	opt.wanid = wan_index;
	opt.action = PPE_METHOD_CLEAR;

	PpeWanACnt(&opt);
	
	return ;
}

#endif /* CONFIG_TP_IMAGE */
void wan_account_read_en7580(int i)
{
    uint8_t acnt_mode = WAN_ITF_MODE;
    unsigned int uniUpxCnt = 0, uniUpxCnt_hi = 0;
    unsigned int uniDownCnt = 0, uniDownCnt_hi = 0;    
    uint uni_Bytes_lo = 0, uni_Bytes_hi = 0;
    unsigned int mulUpxCnt= 0, mulUpxCnt_hi = 0;
    unsigned int mulDownCnt = 0, mulDownCnt_hi = 0;    
    uint mul_Bytes_lo = 0, mul_Bytes_hi = 0;

    if(!TCSUPPORT_ACCOUNT_METER_V2_VAL)
    	FE_API_GET_ACNT1_MODE(UP_STREAM, &acnt_mode);

    if(acnt_mode == WAN_ITF_MODE) {
        FE_API_GET_FlOW_PKT_CNT(i*2, UP_STREAM, acnt_mode, &uniUpxCnt, &uniUpxCnt_hi);
        FE_API_GET_FlOW_CNT(i*2, UP_STREAM, acnt_mode, &uni_Bytes_lo, &uni_Bytes_hi);
        ppe_acnt_grp[i].uniTxCnt = ((unsigned long long)uniUpxCnt_hi << 32) + uniUpxCnt;
        ppe_acnt_grp[i].uniTxBytes = ((unsigned long long)uni_Bytes_hi << 32) + uni_Bytes_lo;

        FE_API_GET_FlOW_PKT_CNT(i*2+1 , UP_STREAM, acnt_mode, &mulUpxCnt, &mulUpxCnt_hi);
        FE_API_GET_FlOW_CNT(i*2+1 , UP_STREAM, acnt_mode, &mul_Bytes_lo, &mul_Bytes_hi);
        ppe_acnt_grp[i].mulTxCnt = ((unsigned long long)mulUpxCnt_hi << 32) + mulUpxCnt;
        ppe_acnt_grp[i].mulTxBytes = ((unsigned long long)mul_Bytes_hi << 32) + mul_Bytes_lo;
    } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){            
        FE_API_GET_FlOW_PKT_CNT(i, UP_STREAM, acnt_mode, &uniUpxCnt, &uniUpxCnt_hi);
        FE_API_GET_FlOW_CNT(i, UP_STREAM, acnt_mode, &uni_Bytes_lo, &uni_Bytes_hi);
        ppe_acnt_grp[i].uniTxCnt = ((unsigned long long)uniUpxCnt_hi << 32) + uniUpxCnt;
        ppe_acnt_grp[i].uniTxBytes = ((unsigned long long)uni_Bytes_hi << 32) + uni_Bytes_lo;
        ppe_acnt_grp[i].mulTxCnt = ppe_acnt_grp[i].mulTxBytes = 0;
    }

    if(!TCSUPPORT_ACCOUNT_METER_V2_VAL)
    	FE_API_GET_ACNT1_MODE(DOWN_STREAM, &acnt_mode);

    if(acnt_mode == WAN_ITF_MODE) {
        FE_API_GET_FlOW_PKT_CNT(i*2, DOWN_STREAM, acnt_mode, &uniDownCnt, &uniDownCnt_hi);
        FE_API_GET_FlOW_CNT(i*2, DOWN_STREAM, acnt_mode, &uni_Bytes_lo, &uni_Bytes_hi);
        ppe_acnt_grp[i].uniRxCnt = ((unsigned long long)uniDownCnt_hi << 32) + uniDownCnt;
        ppe_acnt_grp[i].uniRxBytes = ((unsigned long long)uni_Bytes_hi << 32) + uni_Bytes_lo;

        FE_API_GET_FlOW_PKT_CNT(i*2+1, DOWN_STREAM, acnt_mode, &mulDownCnt, &mulDownCnt_hi);
        FE_API_GET_FlOW_CNT(i*2+1, DOWN_STREAM, acnt_mode, &mul_Bytes_lo, &mul_Bytes_hi);
        ppe_acnt_grp[i].mulRxCnt = ((unsigned long long)mulDownCnt_hi << 32) + mulDownCnt;
        ppe_acnt_grp[i].mulRxBytes = ((unsigned long long)mul_Bytes_hi << 32) + mul_Bytes_lo;
    } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE){
        FE_API_GET_FlOW_PKT_CNT(i, DOWN_STREAM, acnt_mode, &uniDownCnt, &uniDownCnt_hi);
        FE_API_GET_FlOW_CNT(i, DOWN_STREAM, acnt_mode, &uni_Bytes_lo, &uni_Bytes_hi);
        ppe_acnt_grp[i].uniRxCnt = ((unsigned long long)uniDownCnt_hi << 32) + uniDownCnt;
        ppe_acnt_grp[i].uniRxBytes = ((unsigned long long)uni_Bytes_hi << 32) + uni_Bytes_lo;
        ppe_acnt_grp[i].mulRxCnt = ppe_acnt_grp[i].mulRxBytes = 0;
    }
    return;
}

void wan_account_read_en751627(int i)
{
    unsigned int uniUpxCnt = 0, uniUpxCnt_hi = 0;
    unsigned int uniDownCnt = 0, uniDownCnt_hi = 0;    
    uint uni_Bytes_lo = 0, uni_Bytes_hi = 0;
    
    FE_API_GET_FlOW_PKT_CNT(i, UP_STREAM, WAN_ITF_MODE, &uniUpxCnt, &uniUpxCnt_hi);
    FE_API_GET_FlOW_CNT(i, UP_STREAM, WAN_ITF_MODE, &uni_Bytes_lo, &uni_Bytes_hi);
    ppe_acnt_grp[i].uniTxCnt = ((unsigned long long)uniUpxCnt_hi << 32) + uniUpxCnt;
    ppe_acnt_grp[i].uniTxBytes = ((unsigned long long)uni_Bytes_hi << 32) + uni_Bytes_lo;
    ppe_acnt_grp[i].mulTxCnt = ppe_acnt_grp[i].mulTxBytes = 0;

    FE_API_GET_FlOW_PKT_CNT(i, DOWN_STREAM, WAN_ITF_MODE, &uniDownCnt, &uniDownCnt_hi);
    FE_API_GET_FlOW_CNT(i, DOWN_STREAM, WAN_ITF_MODE, &uni_Bytes_lo, &uni_Bytes_hi);
    ppe_acnt_grp[i].uniRxCnt = ((unsigned long long)uniDownCnt_hi << 32) + uniDownCnt;
    ppe_acnt_grp[i].uniRxBytes = ((unsigned long long)uni_Bytes_hi << 32) + uni_Bytes_lo;
    ppe_acnt_grp[i].mulRxCnt = ppe_acnt_grp[i].mulRxBytes = 0;

    return;
}

void wan_account_read(int i)
{
	unsigned int cnt32_lo=0, cnt32_hi=0;
        
	if(SUPPORT_FLOW_ACNT){
    	wan_account_read_en7580(i);
	}else if(isEN751627){
	    wan_account_read_en751627(i);
    }   
	else{
		ppe_acnt_grp[i].uniRxCnt += IO_GREG(ACNT_GRP_SMUX_Frame_CNT(i, 1, 0));
		ppe_acnt_grp[i].uniTxCnt += IO_GREG(ACNT_GRP_SMUX_Frame_CNT(i, 0, 0));
		ppe_acnt_grp[i].mulRxCnt += IO_GREG(ACNT_GRP_SMUX_Frame_CNT(i, 1, 1));
		ppe_acnt_grp[i].mulTxCnt += IO_GREG(ACNT_GRP_SMUX_Frame_CNT(i, 0, 1));
		if (isEN751221) {
			FE_API_GET_AC_GROUP_BYTE_CNT((ACNT_GRP_SMUX_CNT(i, 1, 0)), &cnt32_lo, &cnt32_hi);
			ppe_acnt_grp[i].uniRxBytes = ((((unsigned long long)cnt32_hi)<<32)+cnt32_lo);
			FE_API_GET_AC_GROUP_BYTE_CNT((ACNT_GRP_SMUX_CNT(i, 0, 0)), &cnt32_lo, &cnt32_hi);
			ppe_acnt_grp[i].uniTxBytes = ((((unsigned long long)cnt32_hi)<<32)+cnt32_lo);
			FE_API_GET_AC_GROUP_BYTE_CNT((ACNT_GRP_SMUX_CNT(i, 1, 1)), &cnt32_lo, &cnt32_hi);
			ppe_acnt_grp[i].mulRxBytes = ((((unsigned long long)cnt32_hi)<<32)+cnt32_lo);
			FE_API_GET_AC_GROUP_BYTE_CNT((ACNT_GRP_SMUX_CNT(i, 0, 1)), &cnt32_lo, &cnt32_hi);
			ppe_acnt_grp[i].mulTxBytes = ((((unsigned long long)cnt32_hi)<<32)+cnt32_lo);
		} else {
			ppe_acnt_grp[i].uniRxBytes += ((((unsigned long long)(IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(i, 1, 0))))<<32)+IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(i, 1, 0)));
			ppe_acnt_grp[i].uniTxBytes += ((((unsigned long long)(IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(i, 0, 0))))<<32)+IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(i, 0, 0)));
			ppe_acnt_grp[i].mulRxBytes += ((((unsigned long long)(IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(i, 1, 1))))<<32)+IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(i, 1, 1)));
			ppe_acnt_grp[i].mulTxBytes += ((((unsigned long long)(IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(i, 0, 1))))<<32)+IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(i, 0, 1)));
		}
	}
	return;
}

void wan_account_clear_one(int wanIndex)
{
    uint8_t acnt_mode = WAN_ITF_MODE;
    
    if(SUPPORT_FLOW_ACNT){
        if(!TCSUPPORT_ACCOUNT_METER_V2_VAL)
            FE_API_GET_ACNT1_MODE(UP_STREAM, &acnt_mode);

        if(acnt_mode == WAN_ITF_MODE) {
            FE_API_CLEAR_FlOW_CNT((wanIndex*2), UP_STREAM, acnt_mode);
            FE_API_CLEAR_FlOW_CNT((wanIndex*2+1), UP_STREAM, acnt_mode);
        } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE) {
            FE_API_CLEAR_FlOW_CNT(wanIndex, UP_STREAM, acnt_mode);
        }
        
        if(!TCSUPPORT_ACCOUNT_METER_V2_VAL)
            FE_API_GET_ACNT1_MODE(DOWN_STREAM, &acnt_mode);

        if(acnt_mode == WAN_ITF_MODE) {
            FE_API_CLEAR_FlOW_CNT((wanIndex*2), DOWN_STREAM, acnt_mode);
            FE_API_CLEAR_FlOW_CNT((wanIndex*2+1), DOWN_STREAM, acnt_mode);
        } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE) {
            FE_API_CLEAR_FlOW_CNT(wanIndex, DOWN_STREAM, acnt_mode);
        }
    }else if(isEN751627){
        FE_API_CLEAR_FlOW_CNT(wanIndex, UP_STREAM, WAN_ITF_MODE);
        FE_API_CLEAR_FlOW_CNT(wanIndex, DOWN_STREAM, WAN_ITF_MODE);
    }else{
        IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wanIndex, 1, 0));
        IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wanIndex, 0, 0));
        IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wanIndex, 1, 1));
        IO_GREG(ACNT_GRP_SMUX_Frame_CNT(wanIndex, 0, 1));
        IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wanIndex, 1, 0));
        IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wanIndex, 1, 0));
        IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wanIndex, 0, 0));
        IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wanIndex, 0, 0));
        IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wanIndex, 1, 1));
        IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wanIndex, 1, 1));
        IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(wanIndex, 0, 1));
        IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(wanIndex, 0, 1));
    }
    memset(ppe_acnt_grp+wanIndex,0,sizeof(ppeAcntGrp_t));

    return;
}

void wan_account_clear_all(void)
{
    int i = 0;
    uint8_t acnt_mode = WAN_ITF_MODE;

    for(i = 0;i < ACNT_GRP_WAN_NUM;i++)
    {   
        if(SUPPORT_FLOW_ACNT){
            if(!TCSUPPORT_ACCOUNT_METER_V2_VAL)
                FE_API_GET_ACNT1_MODE(UP_STREAM, &acnt_mode);

            if(acnt_mode == WAN_ITF_MODE) {
                FE_API_CLEAR_FlOW_CNT((i*2), UP_STREAM, acnt_mode);
                FE_API_CLEAR_FlOW_CNT((i*2+1), UP_STREAM, acnt_mode);
            } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE) {
                FE_API_CLEAR_FlOW_CNT(i, UP_STREAM, acnt_mode);
            }
            
            if(!TCSUPPORT_ACCOUNT_METER_V2_VAL)
                FE_API_GET_ACNT1_MODE(DOWN_STREAM, &acnt_mode);

            if(acnt_mode == WAN_ITF_MODE) {
                FE_API_CLEAR_FlOW_CNT((i*2), DOWN_STREAM, acnt_mode);
                FE_API_CLEAR_FlOW_CNT((i*2+1), DOWN_STREAM, acnt_mode);
            } else if(acnt_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE) {
                FE_API_CLEAR_FlOW_CNT(i, DOWN_STREAM, acnt_mode);
            }
        }else if(isEN751627){
            FE_API_CLEAR_FlOW_CNT(i, UP_STREAM, WAN_ITF_MODE);
            FE_API_CLEAR_FlOW_CNT(i, DOWN_STREAM, WAN_ITF_MODE);
        }else{
            IO_GREG(ACNT_GRP_SMUX_Frame_CNT(i, 1, 0));
            IO_GREG(ACNT_GRP_SMUX_Frame_CNT(i, 0, 0));
            IO_GREG(ACNT_GRP_SMUX_Frame_CNT(i, 1, 1));
            IO_GREG(ACNT_GRP_SMUX_Frame_CNT(i, 0, 1));
            IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(i, 1, 0));
            IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(i, 1, 0));
            IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(i, 0, 0));
            IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(i, 0, 0));
            IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(i, 1, 1));
            IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(i, 1, 1));
            IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_HIGH(i, 0, 1));
            IO_GREG(ACNT_GRP_SMUX_BYTE_CNT_LOW(i, 0, 1));       
        }
        
    }
    memset(ppe_acnt_grp,0,ACNT_GRP_WAN_NUM*sizeof(ppeAcntGrp_t));

    return;
}

int wan_account_clear(int wanIndex)
{
    int ret = 0;

    if((wanIndex >= 0)&&(wanIndex < 8))
    {	
    	wan_account_clear_one(wanIndex);
    }
    else if(99 == wanIndex)     /* clear all */
    {
        wan_account_clear_all();
    }
    else
    {
        printk("wanIndex %d error\n", wanIndex);
        ret = -EFAULT;
    }

    return ret;
}

static int hwnat_wan_account_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int i = 0,index = 0;
	off_t pos=0, begin=0 ;
	uint8_t acnt1_mode=0, acnt2_mode=0;
	unsigned int uniUpxCnt = 0, uniUpxCnt_hi = 0, uniDownCnt = 0, uniDownCnt_hi = 0;
	uint uni_Bytes_lo = 0, uni_Bytes_hi = 0;
	char wan_index_info[8]={-1,-1,-1,-1,-1,-1,-1,-1};
    
	if(get_wan_index_info_hook){
		get_wan_index_info_hook(wan_index_info);
	}
	
	index += sprintf(buf+index, "interface uniTxCnt uniRxCnt uniTxBytes uniRxBytes mulTxCnt mulRxCnt mulTxBytes mulRxBytes\n");
	for (i = 0; i < ACNT_GRP_WAN_NUM; i++) 
	{
		wan_account_read(i);
        
		if(TCSUPPORT_CT_VAL){
			if(wan_index_info[i] != -1){
				index += sprintf(buf+index, "nas%d_%d:  0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx\n", wan_index_info[i]/8, wan_index_info[i]%8, ppe_acnt_grp[i].uniTxCnt,\
					ppe_acnt_grp[i].uniRxCnt,ppe_acnt_grp[i].uniTxBytes,ppe_acnt_grp[i].uniRxBytes,ppe_acnt_grp[i].mulTxCnt,ppe_acnt_grp[i].mulRxCnt,\
					ppe_acnt_grp[i].mulTxBytes,ppe_acnt_grp[i].mulRxBytes);
			}
			else{
				index += sprintf(buf+index, "NA:  0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx\n", ppe_acnt_grp[i].uniTxCnt,\
					ppe_acnt_grp[i].uniRxCnt,ppe_acnt_grp[i].uniTxBytes,ppe_acnt_grp[i].uniRxBytes,ppe_acnt_grp[i].mulTxCnt,ppe_acnt_grp[i].mulRxCnt,\
					ppe_acnt_grp[i].mulTxBytes,ppe_acnt_grp[i].mulRxBytes);
			}
		}
		else{
			index += sprintf(buf+index, "w%d:  0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx\n", i,ppe_acnt_grp[i].uniTxCnt,\
				ppe_acnt_grp[i].uniRxCnt,ppe_acnt_grp[i].uniTxBytes,ppe_acnt_grp[i].uniRxBytes,ppe_acnt_grp[i].mulTxCnt,ppe_acnt_grp[i].mulRxCnt,\
				ppe_acnt_grp[i].mulTxBytes,ppe_acnt_grp[i].mulRxBytes);
		}
		CHK_BUF();
	}

    FE_API_GET_ACNT1_MODE(UP_STREAM, &acnt1_mode);
    FE_API_GET_ACNT1_MODE(DOWN_STREAM, &acnt2_mode);
    if((acnt1_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE) && (acnt2_mode == WAN_ITF_AND_TRAFFIC_FLOW_MODE))
    {
    	for (i = 0; i < TRAFFIC_FLOW_MAX_NUM; i++) 
    	{      
    		FE_API_GET_FlOW_PKT_CNT(i+ACNT_GRP_WAN_NUM, UP_STREAM, acnt1_mode, &uniUpxCnt , &uniUpxCnt_hi);
    		FE_API_GET_FlOW_CNT(i+ACNT_GRP_WAN_NUM, UP_STREAM, acnt1_mode, &uni_Bytes_lo, &uni_Bytes_hi);
    		ppe_acnt_grp[i].uniTxCnt = ((unsigned long long)uniUpxCnt_hi << 32) + uniUpxCnt;
    		ppe_acnt_grp[i].uniTxBytes = ((unsigned long long)uni_Bytes_hi << 32) + uni_Bytes_lo;
    		ppe_acnt_grp[i].mulTxCnt = ppe_acnt_grp[i].mulTxBytes = 0;
            
    		FE_API_GET_FlOW_PKT_CNT(i+ACNT_GRP_WAN_NUM, DOWN_STREAM, acnt2_mode, &uniDownCnt, &uniDownCnt_hi);
    		FE_API_GET_FlOW_CNT(i+ACNT_GRP_WAN_NUM, DOWN_STREAM, acnt2_mode, &uni_Bytes_lo, &uni_Bytes_hi);
    		ppe_acnt_grp[i].uniRxCnt = ((unsigned long long)uniDownCnt_hi << 32) + uniDownCnt;
    		ppe_acnt_grp[i].uniRxBytes = ((unsigned long long)uni_Bytes_hi << 32) + uni_Bytes_lo;
    		ppe_acnt_grp[i].mulRxCnt = ppe_acnt_grp[i].mulRxBytes = 0;
            
			index += sprintf(buf+index, "flow_%d:  0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx 0x%llx\n", i, ppe_acnt_grp[i].uniTxCnt,\
				ppe_acnt_grp[i].uniRxCnt,ppe_acnt_grp[i].uniTxBytes,ppe_acnt_grp[i].uniRxBytes,ppe_acnt_grp[i].mulTxCnt,ppe_acnt_grp[i].mulRxCnt,\
				ppe_acnt_grp[i].mulTxBytes,ppe_acnt_grp[i].mulRxBytes);
    		CHK_BUF();
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

static int hwnat_wan_account_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[64];
	char* endpo; 
	int ret = 0;
	int wanIndex = -1;
		
	if (count > sizeof(val_string) - 1)
	{
		ret = -EFAULT;
		goto end;
	}
	
	memset(val_string,0,64);
	if (copy_from_user(val_string, buffer, count))
	{
		ret = -EFAULT;
		goto end;
	}

	wanIndex = simple_strtol(val_string,&endpo,10);

	if(-EFAULT == wan_account_clear(wanIndex))
		goto end;

	ret = count;
	end:
	return ret;
}

/**************wan account end********************************/

void SetEtherWANQdma(int enable)
{
	QDMA_TxBufCtrl_T txBufUsage;
	QDMA_txCngstCfg_t qdmaTxCngstCfg;
	
	if(enable){
		if((TCSUPPORT_CPU_EN7512_VAL || TCSUPPORT_CPU_EN758X_VAL) && (g_wan_mode != USE_QDMA_WAN_ETHER))
		{  
			qdmaTxCngstCfg.txCngstMode = QDMA_TXCNGST_DYNAMIC_NORMAL;
			QDMA_API_SET_TXQ_CNGST_AUTO_CONFIG(ECNT_QDMA_WAN, &qdmaTxCngstCfg);
			memset(&txBufUsage, 0, sizeof(QDMA_TxBufCtrl_T));
			txBufUsage.mode = QDMA_ENABLE;
			txBufUsage.chnThreshold = 0x10; /* PSE blocks */
			txBufUsage.totalThreshold = 0xc0;
			if (QDMA_API_SET_TXBUF_THRESHOLD(ECNT_QDMA_WAN, &txBufUsage))
			{
				return;
			}
			QDMA_API_SET_MAC_LIMIT_THRESHOLD(ECNT_QDMA_WAN, 200, 50);
			QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN, QDMA_ENABLE);
			QDMA_API_RX_DMA_MODE(ECNT_QDMA_WAN, QDMA_ENABLE);

			ETHER_WAN_USE_QDMA_WAN(USE_QDMA_WAN_ETHER);
			PpeCleanTableHandler();
			FE_API_SET_CHANNEL_RETIRE_ALL(FE_GDM_SEL_GDMA2, 8);			
		}
	}else{
		if((TCSUPPORT_CPU_EN7512_VAL || TCSUPPORT_CPU_EN758X_VAL) && (g_wan_mode != USE_QDMA_WAN_PON_XDSL))
		{
			ETHER_WAN_USE_QDMA_WAN(USE_QDMA_WAN_PON_XDSL);
			PpeCleanTableHandler();
			FE_API_SET_CHANNEL_RETIRE_ALL(FE_GDM_SEL_GDMA2, 8);			
		}	   
	}

	return;
}

static int blackip_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	struct BlackHwnatIp *entry = NULL;
	struct BlackHwnatIp *tmp = NULL;
	int range = 0;

	spin_lock_bh(&black_ip_lock);

	printk("black ip table, type=1 means match up stream dst ip, type=2 means match down stream src ip, type=3 means both type=1&type=2\n");
	for(range = 0;range<1024;range++)
	{
		list_for_each_entry_safe(entry, tmp,&BlackIpTable[range], list)
		{
			if(0 != (entry->sign ))
			{
				if(entry->ipv6_mode){
					printk("ipv6 addr: %02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\t mask %u\t type %u\t sign %u\n",
						entry->u6_addr8[0],entry->u6_addr8[1],entry->u6_addr8[2],entry->u6_addr8[3],
						entry->u6_addr8[4],entry->u6_addr8[5],entry->u6_addr8[6],entry->u6_addr8[7],
						entry->u6_addr8[8],entry->u6_addr8[9],entry->u6_addr8[10],entry->u6_addr8[11],
						entry->u6_addr8[12],entry->u6_addr8[13],entry->u6_addr8[14],entry->u6_addr8[15], entry->mask, entry->type, entry->sign);
				}else{
					printk("ipv4 addr: %u.%u.%u.%u\t mask %u\t type %u\t sign %u\n",
						((entry->ipv4)&(0xFF000000))>>24,((entry->ipv4)&(0x00FF0000))>>16, 
						((entry->ipv4)&(0x0000FF00))>>8, ((entry->ipv4)&(0x000000FF)), 
						entry->mask, entry->type, entry->sign);

				}
				
			}
		}
	}
	printk("black ip range table \n");
	list_for_each_entry_safe(entry, tmp, &BlackIpTableRange, list)
	{
		if(0 != (entry->sign))
		{
			if(entry->ipv6_mode){
				printk("ipv6 addr: %02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\t mask %u\t type %u\t sign %u\n",
					entry->u6_addr8[0],entry->u6_addr8[1],entry->u6_addr8[2],entry->u6_addr8[3],
					entry->u6_addr8[4],entry->u6_addr8[5],entry->u6_addr8[6],entry->u6_addr8[7],
					entry->u6_addr8[8],entry->u6_addr8[9],entry->u6_addr8[10],entry->u6_addr8[11],
					entry->u6_addr8[12],entry->u6_addr8[13],entry->u6_addr8[14],entry->u6_addr8[15], entry->mask, entry->type, entry->sign);
			}
			else{
				printk("ipv4 addr: %u.%u.%u.%u\t mask %u\t type %u\t sign %u\n",
					((entry->ipv4)&(0xFF000000))>>24,((entry->ipv4)&(0x00FF0000))>>16, 
					((entry->ipv4)&(0x0000FF00))>>8, ((entry->ipv4)&(0x000000FF)), 
					entry->mask, entry->type, entry->sign);
			}
				
		}
	}
	spin_unlock_bh(&black_ip_lock);	
	
	return 0;	
}

static int etherwan_use_qdma_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("use_etherwan :%d, etherwan_use_qdma: %d g_wan_mode:%d\n",use_etherwan, etherwan_use_qdma, g_wan_mode);
	
	return 0;	
}

static int etherwan_use_qdma_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];
	unsigned int hwnat_off = 0;
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d",&hwnat_off);

	etherwan_use_qdma = hwnat_off;
	
	if(etherwan_use_qdma)
	{
		if(isEN751627)
			ptm_etherwan_coexist = 0;
		SetEtherWANQdma(1);
	}
	else
	{
		if(isEN751627)
			ptm_etherwan_coexist = 1;
		SetEtherWANQdma(0);
	}

	return count;
}

enum meterDirEnum ppeMeterDir_get(struct port_info * pinfo, int magic)
{
    if (magic == FOE_MAGIC_ATM || magic == FOE_MAGIC_PTM ||
        magic == FOE_MAGIC_EPON || magic == FOE_MAGIC_GPON ||
        (magic == FOE_MAGIC_GE && pinfo->eth_is_wan))
        return METER_DIR_UP;
    else
        return METER_DIR_DOWN;

}

int qdma_qos_mark(struct sk_buff *skb)
{
	int priority = 0;

	priority = (skb->mark & QOS_FILTER_MARK)>> 4;

	return priority;
}

int getGrpIdxByQueue(struct sk_buff * skb,enum meterDirEnum dir){	
	unsigned int queue = 0xf; 
    struct list_head *vHead, *plist;	
    struct meterVlan *mVlan; 
	u_int16_t mGrpId = HWNAT_DEFAULT_GRP_ID;
	
	queue = qdma_qos_mark(skb);
	
	/* for Meter Group ID */
	spin_lock(&meterRsr.meterLock);
   
	vHead = &meterRsr.mVid[0];
	if (!list_empty(vHead))
	{
		list_for_each(plist, vHead) {
		mVlan = list_entry(plist, struct meterVlan, list);
		   
		if (mVlan->dir == dir && mVlan->queue == (queue&0xf)) {
		/* In TRTCM, Meters "n" and "n+1" are used together to color flows.
		 * but flow only need to bind to the "n" Meter */
			if (mVlan->isTrtcm)
				mGrpId = mVlan->group & (~(0x1));
			else
				mGrpId = mVlan->group;

				mVlan->foeIdx = FOE_ENTRY_NUM(skb);
				mVlan->isBind = 1;
				break;
		   }
	   }
	}

	spin_unlock(&meterRsr.meterLock);


	return mGrpId;
}

void ppeMeterBindInfoHandler(struct sk_buff * skb,
						struct port_info * pinfo, 
						int magic,
						u_int16_t *mGrpId)
{
	enum meterDirEnum dir = ppeMeterDir_get(pinfo, magic);

	if(gMeterGroupSupport == 0){
		return;
	}

	*mGrpId = getGrpIdxByQueue(skb,dir);

	if(isEN7580 && (pinfo->tsid == 0x7f || pinfo->tsid == 0x3f))
		pinfo->tsid = *mGrpId;

	if (DebugLevel >= 1) {
		printk("(%s)mGrpId:%d,skb->mark=0x%x\n", __func__,*mGrpId,skb->mark);
	}

	return;
}

static void 
ppeMeterAccounterBindInfoHandler (
    struct sk_buff * skb, struct FoeEntry * foe_entry, 
    struct port_info * pinfo, int magic, 
    u_int16_t *mGrpId, u_int16_t *aGrpId)
{
    unsigned int vlan = 0;
    struct list_head *vHead, *plist;
    enum meterDirEnum dir = ppeMeterDir_get(pinfo, magic);
    struct meterVlan *mVlan;

	if (DebugLevel >= 3) {
		printk("(%s) vlan_layer:%d\t", __func__, PpeParseResult.vlan_layer);
	}

    if (PpeParseResult.vlan_layer == 0)
        return;

    if (IS_IPV4_GRP(foe_entry))
        vlan = foe_entry->ipv4_hnapt.vlan1;
    else if (IS_IPV6_GRP(foe_entry))
        vlan = foe_entry->ipv6_5t_route.vlan1;
    else if (IS_L2_RRIDGE(foe_entry))
	vlan = foe_entry->l2_bridge32.vlan1;

	if (DebugLevel >= 3) {
		printk("vlan:%x\t", vlan);
	}


    /* for Meter Group ID */

    spin_lock(&meterRsr.meterLock);
    
    vHead = &meterRsr.mVid[vlan&0xfff];

    if (!list_empty(vHead))
    {
        list_for_each(plist, vHead) {
            mVlan = list_entry(plist, struct meterVlan, list);
            if (mVlan->dir == dir && mVlan->pbits == ((vlan>>12)&0xf)) {
                /* In TRTCM, Meters "n" and "n+1" are used together to color flows.
                 * but flow only need to bind to the "n" Meter */
                if (mVlan->isTrtcm)
                    *mGrpId = mVlan->group & (~(0x1));
                else
                    *mGrpId = mVlan->group;
                mVlan->foeIdx = FOE_ENTRY_NUM(skb);
                mVlan->isBind = 1;
                break;
            }
        }
    }

    spin_unlock(&meterRsr.meterLock);


    /* for Accounter Group ID */

    spin_lock(&accountRsr.accountLock);

    vHead = &accountRsr.aVid[vlan&0xfff];

    if (!list_empty(vHead))
    {
        list_for_each(plist, vHead) {
            mVlan = list_entry(plist, struct meterVlan, list);
            if (mVlan->dir == dir && mVlan->pbits == ((vlan>>12)&0xf)) {
                *aGrpId = mVlan->group;
                mVlan->foeIdx = FOE_ENTRY_NUM(skb);
                mVlan->isBind = 1;
                break;
            }
        }
    }

    spin_unlock(&accountRsr.accountLock);

	if (DebugLevel >= 3) {
		printk("mGrpId:%x\taGrpId:%x\n", *mGrpId, *aGrpId);
	}
    
    return;
}

void ppeMeterResource_init(void)
{
    int i;

    meterRsr.meterLock = __SPIN_LOCK_UNLOCKED(meterRsr.meterLock);

    for (i = 0; i < METER_GROUP_MAX; i++) {
        INIT_LIST_HEAD(&meterRsr.mGroup[i]);
        memset(&meterRsr.meterCfg[i], 0, sizeof(struct ppe_meter_cfg));
    }

    for (i = 0; i < VID_MAX; i++)
        INIT_LIST_HEAD(&meterRsr.mVid[i]);

    return;
}

static void ppeTlsResource_init(void)
{
    int i;
    unsigned int reg;

    memset(&tlsRsr, 0, sizeof(struct ppeTlsResource));

    tlsRsr.tlsLock = __SPIN_LOCK_UNLOCKED(tlsRsr.tlsLock);

    for (i = 0; i < TLS_CFG_REG_NUM; i++)
    {
        reg = RegRead(PPE_TLS_CFG0+(i<<2)); /* i<<2 means i*4 */

        if (reg & 0x1) /* enable bit */
        {
            tlsRsr.tlsCfg[i] = 1;
            tlsRsr.tlsVid[(reg>>TLS_REG_OFF_TAG0)&0xf] = 1;
            tlsRsr.tlsVid[(reg>>TLS_REG_OFF_TAG1)&0xf] = 1;
            if (((reg>>TLS_REG_OFF_RNG)&0x1) == 0)
            {
                tlsRsr.tlsVid[(reg>>TLS_REG_OFF_TAG2)&0xf] = 1;
                tlsRsr.tlsVid[(reg>>TLS_REG_OFF_TAG3)&0xf] = 1;
                tlsRsr.tlsVid[(reg>>TLS_REG_OFF_TAG4)&0xf] = 1;
            }
        }
    }

    return;
}

static void ppeAccountResource_init(void)
{
    int i;

    accountRsr.accountLock = __SPIN_LOCK_UNLOCKED(accountRsr.accountLock);

    for (i = 0; i < METER_GROUP_MAX; i++) {
        INIT_LIST_HEAD(&accountRsr.aGroup[i]);
    }

    for (i = 0; i < VID_MAX; i++)
        INIT_LIST_HEAD(&accountRsr.aVid[i]);

    return;
}
static void ppeResources_init(void)
{
    ppeTlsResource_init();
    ppeMeterResource_init();
    ppeAccountResource_init();

    return;
}


unsigned int ppeParseMeterIdx(unsigned int foe_index, int magic, struct port_info * pinfo)
{
	unsigned char idx_tmp = 0 ;

	if((!ratelimitByItf_en) && (!ratelimitByIP_en))
		return HWNAT_DEFAULT_GRP_ID ;

	if(!isUpStreamFlow(magic, pinfo))
		return HWNAT_DEFAULT_GRP_ID ;
    
	idx_tmp = foe_ext[foe_index].classify_index&0x1f;

	if(idx_tmp & 0x10)/*enable traffic flow ratelimit ,and matched*/
		return ((idx_tmp & 0xf) + METER_BEGIN_INDEX_FOR_RATELIMIT) ;/*meter index : 30 ~ 43*/

	return HWNAT_DEFAULT_GRP_ID;
}


int  ecnt_ppe_get_meter_and_account_id_hook(struct FoeEntry * foe_entry,struct sk_buff * skb,
	struct port_info * pinfo, int magic,unsigned short *p_meter_id,unsigned short *p_account_id)
{
	unsigned short meter_id = HWNAT_DEFAULT_GRP_ID;
	unsigned short account_id = HWNAT_DEFAULT_GRP_ID;
	unsigned short meter_id_tmp = HWNAT_DEFAULT_GRP_ID;

	if(TCSUPPORT_FWC_QOS_VAL)
		getPpeMeterBindInfo(skb, &meter_id);
	else if(TCSUPPORT_CT_VAL||TCSUPPORT_XPON_HAL_API_VAL||TCSUPPORT_XPON_HAL_API_EXT_VAL)
	{
		ppeMeterBindInfoHandler(skb, pinfo, magic, &meter_id);
		if((HWNAT_DEFAULT_GRP_ID == meter_id)&&(TCSUPPORT_XPON_HAL_API_VAL||TCSUPPORT_XPON_HAL_API_EXT_VAL))
			meter_id = ppeParseMeterIdx(FOE_ENTRY_NUM(skb), magic, pinfo);
	}
	else
		ppeMeterAccounterBindInfoHandler(skb, foe_entry, pinfo, magic, &meter_id, &meter_id);

	if(TCSUPPORT_CT_JOYME_BANDWIDTH_VAL || TCSUPPORT_CF_JOYMEV2_PON_VAL || TCSUPPORT_XPON_HAL_API_EXT_VAL
		|| TCSUPPORT_CRJO_VAL){
		account_id = PpeGetMacMatchAcntGrpIndex(skb);
		meter_id_tmp = PpeGetMacMatchMeterGrpIndex(skb);
		if(meter_id_tmp != HWNAT_DEFAULT_GRP_ID)
			meter_id = meter_id_tmp;
	}

	if(TCSUPPORT_CF_JOYMEV2_PON_VAL)
	{
	    if(ppe_is_multicast_entry(foe_entry))
	        account_id = HWNAT_MULTI_ACNTGRP;
	}
	
	PpeGetAcntMeterIdByFlowId(skb, magic, pinfo, &meter_id, &account_id);

	PpeGetWanMeterId(skb, &meter_id, magic, pinfo);
	
	if(TCSUPPORT_CDS_VAL)
		meter_id_tmp = PpeGetMeterIdByMapping(skb, magic, pinfo);
	if(meter_id_tmp != HWNAT_DEFAULT_GRP_ID)
		meter_id = meter_id_tmp;

	*p_meter_id = meter_id;
	*p_account_id = account_id;
		
	return 0;
}

static int hwnat_meter_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{

	printk("%d\n",gMeterGroupSupport);

	return 0;	
}

static int hwnat_meter_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];
	unsigned int flag = 0;
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d",&flag);

	gMeterGroupSupport = flag;
	
	printk("gMeterGroupSupport: %d \n",gMeterGroupSupport);
	
	return count;
}

static int hwnat_stag_7512autotest_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char get_buf[9];
    
    if(count > (sizeof(get_buf) - 1))
        return -EINVAL ;

    memset(get_buf, 0, sizeof(get_buf));
    
    if(copy_from_user(get_buf, buffer, count))
        return -EFAULT;

    get_buf[count] = '\0';

    sscanf(get_buf, "%d",&hwnat_stag_7512autotest);

    if (hwnat_stag_7512autotest)
        printk("\nhwnat_stag_7512autotest enable\n");
    else
        printk("\nhwnat_stag_7512autotest disable\n");
    
    return count;
}

static int shrink_table_dump_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	//printk("use_etherwan :%d, etherwan_use_qdma: %d\n",use_etherwan, etherwan_use_qdma);
	
	return 0;	
}

static int shrink_table_dump_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	struct hwnat_shrink_field shrinkField;
	struct sockaddr_in6 ip6;
	uint8_t val_string[128], cmd[16];
	uint8_t param1[16], param2[48], param3[48] ;
	int ret = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%15s %15s %47s %47s", cmd, param1, param2, param3) ;
	printk("cmd:%s, param1:%s, param2:%s, param3:%s\n", cmd, param1, param2, param3);

	if(!strcmp(cmd, "dump")) {
		ppe_dump_shrink_table();
	} else if(!strcmp(cmd, "timeout")) {
		timeOutVal = atoi(param1) * 100;
	} else if(!strcmp(cmd, "clear")) {
		ppeInitShrinkTable();
	}
	else if(!strcmp(cmd, "set")) {
		// echo set smac [0~15]  00:aa:bb:01:23:01 > /proc/tc3162/shrink_table_debug
		// echo set ipv4 [0~15]  192.85.20.1 192.168.30.1 > /proc/tc3162/shrink_table_debug
		// echo set ipv6 [0~15]  2000::1  2000::2 > /proc/tc3162/shrink_table_debug
		if(!strcmp(param1, "smac")) {
			str_to_mac(shrinkField.smac, (char *)param2);		 
			ret = find_and_update_shrink_table(PPE_UPDMEM_SEL_SMAC, &shrinkField);
		} else if(!strcmp(param1, "ipv4")) {
			str_to_ip((unsigned long *)&shrinkField.eg_ipv4[1], (char *)param2);
			printk("shrinkField.eg_ipv4:%X\n", shrinkField.eg_ipv4[1]);
			str_to_ip((unsigned long *)&shrinkField.eg_ipv4[0], (char *)param3);
			printk("shrinkField.eg_ipv4:%X\n", shrinkField.eg_ipv4[0]);
			find_and_update_shrink_table(PPE_UPDMEM_SEL_IPv4, &shrinkField);
		} else if(!strcmp(param1, "ipv6")) {
			ret = inet_pton6((char *)param2, (unsigned char *)&ip6.sin6_addr.s6_addr[0]);
			shrinkField.eg_ipv6[4] = ntohl(*((unsigned int *)ip6.sin6_addr.s6_addr));
			shrinkField.eg_ipv6[5] = ntohl(*((unsigned int *)(ip6.sin6_addr.s6_addr + 4)));
			shrinkField.eg_ipv6[6] = ntohl(*((unsigned int *)(ip6.sin6_addr.s6_addr + 8)));
			shrinkField.eg_ipv6[7] = ntohl(*((unsigned int *)(ip6.sin6_addr.s6_addr + 12)));
			printk("eg_sipv6:%X:%X:%X:%X\n", shrinkField.eg_ipv6[4], shrinkField.eg_ipv6[5], shrinkField.eg_ipv6[6], shrinkField.eg_ipv6[7]);
			ret = inet_pton6((char *)param3, (unsigned char *)&ip6.sin6_addr.s6_addr[0]);
			shrinkField.eg_ipv6[0] = ntohl(*((unsigned int *)ip6.sin6_addr.s6_addr));
			shrinkField.eg_ipv6[1] = ntohl(*((unsigned int *)(ip6.sin6_addr.s6_addr + 4)));
			shrinkField.eg_ipv6[2] = ntohl(*((unsigned int *)(ip6.sin6_addr.s6_addr + 8)));
			shrinkField.eg_ipv6[3] = ntohl(*((unsigned int *)(ip6.sin6_addr.s6_addr + 12)));
			printk("eg_dipv6:%X:%X:%X:%X\n", shrinkField.eg_ipv6[0], shrinkField.eg_ipv6[1], shrinkField.eg_ipv6[2], shrinkField.eg_ipv6[3]);
			find_and_update_shrink_table(PPE_UPDMEM_SEL_IPv6, &shrinkField);
		}
	}

	return count ;
}

/******************************************************************************
******************************************************************************/
static int hwnat_test_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	off_t begin=0 ;
	
	*eof = 1;

	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0)
		index = 0;
	if (index>count)
		index = count;
	return index;
}

/******************************************************************************
******************************************************************************/
static int hwnat_test_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    uint8_t val_string[96], cmd[32] ;
    uint8_t param1[16], param2[16], param3[16], param4[16], param5[16] ;
    uint startAddr=0, size=0;

    if (count > sizeof(val_string) - 1)
        return -EINVAL ;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT ;

    sscanf(val_string, "%31s %15s %15s %15s %15s %15s", cmd, param1, param2, param3, param4, param5) ;

    if(!strcmp(cmd, "mats_test")) {
        startAddr = strtol(param2, NULL, 16);
        size = strtol(param3, NULL, 16);
        mats_test(atoi(param1), startAddr, size);
    }
    return count ;
}
static int udp_bypass_enable_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[32] ;
	uint cmd ;
	
	if (count > sizeof(val_string) - 1)
		return -EFAULT ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%31d", &cmd) ;

	if(cmd == 1) 
	{
		udp_bypass_enable = 1;
	}
	else if(cmd ==2)
        udp_bypass_enable = 2;
    else
	{
		udp_bypass_enable = 0;
	}
	
	return count ;
}


static int udp_bypass_enable_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("%d\n",udp_bypass_enable);
	
	return 0;
}

static void foe_convert_mac(unsigned char* dst_mac, struct FoeEntry *foe_entry)
{
	if((NULL == dst_mac) || (NULL == foe_entry))
	{
		return ;
	}
	
	if (foe_entry->bfib1.pkt_type < 2 )
	{
		FoeGetMacInfo(dst_mac, foe_entry->ipv4_hnapt.dmac_hi);
	}
    else if(SUPPORT_DSLITE_HSK_16B && IS_IPV4_DSLITE(foe_entry))
    {
        FoeGetMacInfo(dst_mac, foe_entry->ipv4_dslite.dmac_hi);
    }
	else
	{
		FoeGetMacInfo(dst_mac, foe_entry->ipv6_5t_route.dmac_hi);
	}

	return ;
}

int xpon_is_multicast_entry(struct FoeEntry *foe_entry)
{
	unsigned char dst_mac[ETH_ALEN];
	
	memset(dst_mac, 0, sizeof(dst_mac));
	foe_convert_mac(dst_mac ,foe_entry);
	
	if (foe_entry->bfib1.pkt_type < 2 )
	{
		if (dst_mac[0]&0x01)
			return 1;
		if (foe_entry->ipv4_hnapt.dip > 0xe0000000)
			return 1;
	}
    else if(SUPPORT_DSLITE_HSK_16B && IS_IPV4_DSLITE(foe_entry)) 
    {
        if (dst_mac[0]&0x01)
            return 1;
        if (foe_entry->ipv4_dslite.dip > 0xe0000000)
            return 1;
    }
	else
	{
		if (dst_mac[0]&0x01)
			return 1;
		
		if (foe_entry->ipv6_3t_route.ipv6_dip0 > 0xff000000)
			return 1;
	}
	return 0;
}

/*Using return value to judge
0:is not multicast entry
1:is multicast entry and is for ipv4
2:is multicast entry and is for ipv6
*/
inline int multicast_speed_is_multicast_entry(struct FoeEntry *foe_entry)
{
	unsigned char dst_mac[ETH_ALEN];

	memset(dst_mac, 0, sizeof(dst_mac));
	foe_convert_mac(dst_mac ,foe_entry);
	
	if (foe_entry->bfib1.pkt_type < 2 )
	{
		if ((dst_mac[0]&0x01) && (foe_entry->ipv4_hnapt.dip > 0xe0000000))
			return 1;
	}
	else
	{
		if (dst_mac[0]&0x01)
			return 2;
		
		if (foe_entry->ipv6_5t_route.ipv6_dip0 > 0xff000000)
			return 2;
	}

	return 0;
}

udpSessionTest_t udpSessionTest = {0};
int udpSessionTestRestoreTime = 120;
static void udp_session_test_timer_check(void)
{
	static int time_1_s = 0;

	if(!udpSessionTest.enable)
		return;

	if((time_1_s++)%4)
		return;

	udpSessionTest.count = 0;

	if(!udpSessionTest.start_flag)
		return;

	udpSessionTest.restore_time--;
	
	if(0 == udpSessionTest.restore_time)
	{
		printk("udp session test restore\n");
		RegModifyBits(PPE_IP_PROT_0, IPPROTO_UDP, 8, 8);
		udpSessionTest.start_flag = 0;
	}
	
	return;
}

static int udp_session_test_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[32] = {0} ;
	int value = 0;
	char cmd[16] = {0};

	if (count > (sizeof(val_string) - 1))
	{
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count))
	{
		return -EFAULT;
	}

	sscanf(val_string, "%15s %d",cmd,&value);

	if (!strcmp(cmd, "enable"))
	{
		udpSessionTest.enable = value;
	}
	else if(!strcmp(cmd, "restore_time"))
	{
		udpSessionTestRestoreTime = value;
	}

	return count ;
}


static int udp_session_test_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int i = 0;
	printk("enable:%d\n",udpSessionTest.enable);
	printk("start_flag %d\n", udpSessionTest.start_flag);
	printk("count %d\n", udpSessionTest.count);
	printk("restore_time %d\n", udpSessionTest.restore_time);
	printk("default restore_time %d\n", udpSessionTestRestoreTime);

	for(i=0; i<2 ; i++)
	{
		printk("info index %d sip 0x%x, dip 0x%x\n\tsport %d, dport %d\n",i,udpSessionTest.info[i].sip,\
			udpSessionTest.info[i].dip,udpSessionTest.info[i].sport,udpSessionTest.info[i].dport);
	}
	
	return 0;
}

#define GDMA2_RX_ETH_65_TO_127_CNT (0xbfb51600 + 0x8C)
static void natv6_2544_test_timer_check(void)
{
	if(natv6_2544_check_flag)
	{
		natv6_2544_pkt_cnt_last = natv6_2544_pkt_cnt_new;
		natv6_2544_pkt_cnt_new = IO_GREG(GDMA2_RX_ETH_65_TO_127_CNT);
		if(natv6_2544_pkt_cnt_last && natv6_2544_pkt_cnt_new)
		{
			if(natv6_2544_pkt_cnt_new >= natv6_2544_pkt_cnt_last)
				natv6_2544_cur_rate = (natv6_2544_pkt_cnt_new - natv6_2544_pkt_cnt_last) << 2;
			else
				natv6_2544_cur_rate = (0xFFFFFFFF - natv6_2544_pkt_cnt_last + natv6_2544_pkt_cnt_new) << 2;
			if(natv6_2544_cur_rate >= natv6_2544_trigger_rate)
			{
				natv6_2544_ongoing_flag = 1;
				natv6_2544_check_flag = 0;
				natv6_2544_timer_cnt = 0;
				natv6_2544_cur_rate = 0;
				natv6_2544_pkt_cnt_last = 0;
				natv6_2544_pkt_cnt_new = 0;
			}
			else
			{
				natv6_2544_check_flag = 0;
				natv6_2544_cur_rate = 0;
				natv6_2544_pkt_cnt_last = 0;
				natv6_2544_pkt_cnt_new = 0;
			}
		}
	}

	if(natv6_2544_ongoing_flag)
	{
		natv6_2544_timer_cnt ++;
		if((natv6_2544_timer_cnt >> 2) >= natv6_2544_hold_time)
		{
			natv6_2544_ongoing_flag = 0;
			natv6_2544_timer_cnt = 0;
		}
	}
	
	return;
}

static int natv6_2544_test_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[32] = {0} ;
	int value = 0;
	char cmd[16] = {0};

	if (count > (sizeof(val_string) - 1))
	{
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count))
	{
		return -EFAULT;
	}

	sscanf(val_string, "%15s %d", cmd, &value);

	if (!strcmp(cmd, "rate"))
	{
		natv6_2544_trigger_rate = value;
	}
	else if (!strcmp(cmd, "time"))
	{
		natv6_2544_hold_time = value;
	}
	else if (!strcmp(cmd, "dump"))
	{
		printk("natv6_2544_check_flag: %d \n", natv6_2544_check_flag);
		printk("natv6_2544_ongoing_flag: %d \n", natv6_2544_ongoing_flag);
		printk("natv6_2544_pkt_cnt_last: %d \n", natv6_2544_pkt_cnt_last);
		printk("natv6_2544_pkt_cnt_new: %d \n", natv6_2544_pkt_cnt_new);
		printk("natv6_2544_cur_rate: %d \n", natv6_2544_cur_rate);
		printk("natv6_2544_trigger_rate: %d pps \n", natv6_2544_trigger_rate);
		printk("natv6_2544_timer_cnt: %d \n", natv6_2544_timer_cnt);
		printk("natv6_2544_hold_time: %d s \n", natv6_2544_hold_time);
	}
	else
	{
		printk("echo rate XX > /proc/tc3162/natv6_2544_test \n");
		printk("echo time XX > /proc/tc3162/natv6_2544_test \n");
		printk("echo dump > /proc/tc3162/natv6_2544_test \n");
	}

	return count ;
}

static int natv6_2544_test_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	off_t pos = 0, begin = 0;

	index += sprintf(buf+index, "%d \n", natv6_2544_ongoing_flag);

	CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0) 	index = 0 ;
	if(index>count) 	index = count ;
	return index ;

}

static void natv6_411_test_timer_check(void)
{
	int chnl_idx;
	QDMA_TxRateLimitSet_T txRateLimitSet;

	if(0 == natv6_411_test_flag)
		return;

	if(0 == natv6_411_timer)
		return;

	natv6_411_timer--;

	if(natv6_411_timer != 0)
		return;

	for(chnl_idx = 1; chnl_idx < 5; chnl_idx++) 
	{
		txRateLimitSet.chnlIdx = chnl_idx;
		txRateLimitSet.chnlRateLimitEn = 1;
		txRateLimitSet.rateLimitValue = (1000 * 1000 );		/* unit: kbps */
		QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_LAN, &txRateLimitSet);
	}
	
	return;
}

static void ppe_clean_table_timer_check(void)
{
	if((1 == ppe_clean_table_flag) && (0 == ppe_clean_table_timer))
	{
		return;
	}

	if(0 == ppe_clean_table_timer)
	{
		return;
	}

	--ppe_clean_table_timer;

	if(ppe_clean_table_timer > 0)
	{
		return;
	}

	if(ppe_clean_table_flag == 1)
	{
		set_foe_SMA_mode(MODE_BUILD_FOE);
	}
	
	return;
}

static int hwnat_static_rule_ext_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int i = 0;
    unsigned char* para_name[EXT_PARA_MAX_NUM] = 
        {"ACNT_GRP3","MTR_GRP2","MTR_GRP3","DPI","UDF","SNPT","DNPT","NPTV6_MODE0_FOE_NUM",
         "NPTV6_MODE1_PREFIX_ID","NPTV6_MODE1_PREFIX_LEN","L2B_DSCP_REMARK_ENABLE"};

	for(i=0; i<EXT_PARA_MAX_NUM ; i++)
	{
		printk("para %s,%s,value %d\n",para_name[i],static_rule_ext.enable[i]?"ENABLE":"DISABLE",static_rule_ext.value[i]);
	}
	
	return 0;
}

static int hwnat_static_rule_ext_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[32] = {0} ;
	unsigned int value = 0;
	unsigned int para;

	if (count > (sizeof(val_string) - 1))
	{
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count))
	{
		return -EFAULT;
	}

	sscanf(val_string, "%d %d",&para,&value);

    if(para >= EXT_PARA_MAX_NUM)
        goto help;

    static_rule_ext.enable[para] = 1;
    static_rule_ext.value[para] = value;

    printk("para = %d,value = %d\n",para,value);

	return count ;

    help:
        printk("para:\n");
        printk("0 -- acnt_grp3\n");
        printk("1 -- mtr_grp2\n");
        printk("2 -- mtr_grp3\n");
        printk("3 -- dpi\n");
        printk("4 -- udf\n");
        printk("5 -- snpt\n");
        printk("6 -- dnpt\n");
        printk("7 -- nptv6_mode0_foe_num\n");
        printk("8 -- nptv6_mode1_prefix_id\n");
        printk("9 -- nptv6_mode1_prefix_len\n");
        printk("10 -- l2b_dscp_remark_enable\n");
		printk("11 -- l2tp_sessionId\n");
        
    return count;
}

static int hwnat_tunnel_offset_in_hash_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("tunnel_offset is %x\n",tun_offset);
	return 0;
}

/******************************************************************************
******************************************************************************/
static int hwnat_tunnel_offset_in_hash_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    uint8_t val_string[96], cmd[32] ;

    if (count > sizeof(val_string) - 1)
        return -EINVAL ;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT ;

    sscanf(val_string, "%s", cmd) ;

    if(!strcmp(cmd, "gre")) {
        tun_offset = 0x200;
    }else if(!strcmp(cmd, "esp"))
	{
		tun_offset = 0x400;
	}
	else if(!strcmp(cmd, "l2tp"))
	{
		tun_offset = 0x600;
	}else if(!strcmp(cmd, "vxlan"))
	{
		tun_offset = 0x400;
	}else
	{
		tun_offset = 0;
		printk("tunnel offset in hash disable!");
	}
    return count ;
}


void auto_GDM2_update(TIMER_FUN_PAAM time)
{
	unsigned long curr_gdm2_tx_ok_cnt;
	unsigned long curr_gdm2_rx_ok_cnt;
	unsigned long tx_ok_cnt_diff;
	unsigned long rx_ok_cnt_diff;
	static unsigned long last_gdm2_tx_ok_cnt;
	static unsigned long last_gdm2_rx_ok_cnt;

	if (internet_hwnat_timer_switch) {
		/* To retrieve TX and RX and relevant MIB counters */
		FE_API_GET_OK_CNT(FE_GDM_SEL_GDMA2, FE_GDM_SEL_TX, (uint *)&curr_gdm2_tx_ok_cnt);
		FE_API_GET_OK_CNT(FE_GDM_SEL_GDMA2, FE_GDM_SEL_RX, (uint *)&curr_gdm2_rx_ok_cnt);

		tx_ok_cnt_diff = (curr_gdm2_tx_ok_cnt > last_gdm2_tx_ok_cnt) ? 
						 (curr_gdm2_tx_ok_cnt - last_gdm2_tx_ok_cnt) : 
						 (0xFFFFFFFF - (last_gdm2_tx_ok_cnt - curr_gdm2_tx_ok_cnt) + 1);
		rx_ok_cnt_diff = (curr_gdm2_rx_ok_cnt > last_gdm2_rx_ok_cnt) ? 
						 (curr_gdm2_rx_ok_cnt - last_gdm2_rx_ok_cnt) : 
						 (0xFFFFFFFF - (last_gdm2_rx_ok_cnt - curr_gdm2_rx_ok_cnt) + 1);

        last_gdm2_tx_ok_cnt = curr_gdm2_tx_ok_cnt;
        last_gdm2_rx_ok_cnt = curr_gdm2_rx_ok_cnt;
						 
		/* To determine wheter blinking LED or not  */
		if ((tx_ok_cnt_diff > internet_hwnat_pktnum) ||
			(rx_ok_cnt_diff > internet_hwnat_pktnum)) {

#ifdef TCSUPPORT_KERNEL_API
		ledBlinkOn(LED_DSL_STATUS, 1);
#else
		ledTurnOn(LED_DSL_ACT_STATUS);
#endif
			/*for interner traffic led*/
			#ifdef TR068_LED
			if(TCSUPPORT_C9_ROST_LED_VAL == 0)
			{
			if(internet_led_on) {//IP connected and IP traffic is passing
#ifdef TCSUPPORT_KERNEL_API
				ledBlinkOn(LED_INTERNET_STATUS, 1);
#else
				ledTurnOn(LED_INTERNET_ACT_STATUS);
#endif
			} 
			else {
				if(!internet_trying_led_on) {
#ifdef TCSUPPORT_KERNEL_API
					ledTurnOff(LED_INTERNET_STATUS);
#else
					ledTurnOff(LED_INTERNET_STATUS);
					ledTurnOff(LED_INTERNET_TRYING_STATUS);
#endif					
				}
			}
			}
			#endif
		}
	}

	udp_session_test_timer_check();

	if(TCSUPPORT_CT_JOYME4_VAL) {
		natv6_2544_test_timer_check();
		
		natv6_411_test_timer_check();
	}
	
	ppe_clean_table_timer_check();
	/* Schedule for the next time */
	gdm2_update_timer.expires = jiffies + msecs_to_jiffies(250);
  	add_timer(&gdm2_update_timer);
}

int hwnat_resource_alloc(HWNAT_RESOURCE_TYPE alloc_type,unsigned int alloc_num,unsigned char* desc)
{
	unsigned int start_id = 0;
	
	if(alloc_type >= HWNAT_RESOURCE_TYPE_MAX)
	{
		printk("%s allocate hwnat resource fail,alloc_type = %d\n",desc,alloc_type);
		return -1;
	}

	if(alloc_num > (hwnat_resource_poll[alloc_type].total_num-hwnat_resource_poll[alloc_type].used_num))
	{
		printk("%s allocate hwnat resource fail,alloc_num = %d,free num = %d\n",desc,alloc_num,hwnat_resource_poll[alloc_type].total_num-hwnat_resource_poll[alloc_type].used_num);
		return -1;
	}

	start_id = hwnat_resource_poll[alloc_type].used_num;
	
	hwnat_resource_poll[alloc_type].used_num+=alloc_num;
	
	return start_id;
}

int ppeFastRuleEnable(void)
{
	hwnat_fast_rule_enable = 1;
	return 0;
}

int ppeFastRuleDisable(void)
{
	hwnat_fast_rule_enable = 0;
	return 0;
}

int ppeFastRuleSetMarkBit(unsigned char bitValue)
{
	hwnat_fast_rule_mark_bit = bitValue;
	return 0;
}

int ppeFastRuleSetMarkEnable(unsigned char value)
{
	hwnat_fast_rule_mark_enable = value;
	return 0;
}

int ppeFastRuleSetReverseEnable(unsigned char value)
{
	hwnat_fast_rule_reverse_enable = value;
	return 0;
}

static void ppeFastRuleListInit(void)
{
	INIT_LIST_HEAD(&fast_rule_index_list);
}

static  void* ppeFastRuleListAlloc(int size)
{
	void* ptr = NULL;
	
	if (size>0)
	{
		ptr = kmalloc(size, GFP_ATOMIC);
		if(ptr == NULL) {
			printk("Alloc memory for ppeLocalInlist failed\n") ;
			return NULL ;
		}
		memset(ptr,0,size);
	}
	
	return ptr;
} 

static void ppeFastRuleListFree(void* ptr)
{
	if (ptr)
	{
		kfree(ptr);
		ptr = NULL;
	} 
}

int ppeShowFastRuleList(void)
{
	struct hwnat_fast_rule_list_t *tmp_entry = NULL;
	unsigned char sip_str[32],dip_str[32];

	printk("ppe fast rule state:%s\n",hwnat_fast_rule_enable?"Enable":"Disable");
	printk("ppe fast rule mark bit:%d\n",hwnat_fast_rule_mark_bit);
	printk("ppe fast rule marked_enable:%d,reverse_enable:%d\n",hwnat_fast_rule_mark_enable,hwnat_fast_rule_reverse_enable);
	
	read_lock_bh(&fast_rule_lock);
	list_for_each_entry(tmp_entry, &fast_rule_index_list, list)
	{
		memset(sip_str,0,32);
		memset(dip_str,0,32);
		ppe_trans_int_to_ip_format(tmp_entry->sip,sip_str);		
		ppe_trans_int_to_ip_format(tmp_entry->dip,dip_str);
		printk("hash index %d:%s,sip %s,dip %s,sport %d,dport %d\n",tmp_entry->foe_index,(tmp_entry->ip_proto==IPPROTO_UDP)?"UDP":"TCP",sip_str,dip_str,tmp_entry->sport,tmp_entry->dport);
	}	
	read_unlock_bh(&fast_rule_lock);

	return 0;
}

static int ppeClearFastRuleList(void)
{
	struct hwnat_fast_rule_list_t * fast_rule_entry = NULL;
	struct hwnat_fast_rule_list_t * tmp = NULL;
	
	write_lock(&fast_rule_lock);
	list_for_each_entry_safe(fast_rule_entry, tmp,&fast_rule_index_list, list)	 
	{
		del_timer_sync(&fast_rule_entry->age_timer);
		list_del(&fast_rule_entry->list);
		ppeFastRuleListFree(fast_rule_entry);
	}	
	write_unlock(&fast_rule_lock);

	return 0;
}

static void ppeFastRuleListTimeOut(TIMER_FUN_PAAM arg)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	struct hwnat_fast_rule_list_t * entry = (struct hwnat_fast_rule_list_t * )arg;
#else
	struct hwnat_fast_rule_list_t * entry = from_timer(entry, arg, age_timer);
#endif

	if(!entry)
		return;

	del_timer(&entry->age_timer);	
	write_lock(&fast_rule_lock);
	list_del(&entry->list);	
	write_unlock(&fast_rule_lock);
	ppeFastRuleListFree(entry);

	return;
}

static int ppeAddNewFastRuleIndex(struct sk_buff* skb,unsigned int foe_index)
{
	struct hwnat_fast_rule_list_t *tmp_entry = NULL;
	unsigned char ip_proto = 0;
	unsigned int sip,dip;
	unsigned short sport,dport;

	read_lock(&fast_rule_lock);
	list_for_each_entry(tmp_entry, &fast_rule_index_list, list)	 
	{
		if(tmp_entry && tmp_entry->foe_index == foe_index)
		{
			read_unlock(&fast_rule_lock);
			return 0;
		}
	}
	read_unlock(&fast_rule_lock);
	
	if(-1 == ppe_get_ipv4_5t(skb,&ip_proto,&sip,&dip,&sport,&dport))
		return 0;

	tmp_entry = (struct hwnat_fast_rule_list_t*)ppeFastRuleListAlloc(sizeof(struct hwnat_fast_rule_list_t));
	if(!tmp_entry)
	{
		return 0;
	}
	memset(tmp_entry,0,sizeof(struct hwnat_fast_rule_list_t));
	if(tmp_entry)
	{
		tmp_entry->foe_index = foe_index;
		tmp_entry->ip_proto = ip_proto;
		tmp_entry->sip = sip;
		tmp_entry->dip = dip;
		tmp_entry->sport = sport;
		tmp_entry->dport = dport;
		write_lock(&fast_rule_lock);
		list_add_tail(&tmp_entry->list,&fast_rule_index_list);		
		write_unlock(&fast_rule_lock);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		setup_timer(&tmp_entry->age_timer, ppeFastRuleListTimeOut, (unsigned long)tmp_entry);
#else
		timer_setup(&tmp_entry->age_timer, ppeFastRuleListTimeOut, 0);
#endif
		mod_timer(&tmp_entry->age_timer,jiffies + PPE_FAST_RULE_AGE_TIME);
	}
	
	return 0;
}

static int ppeCheckFastRuleIndex(struct sk_buff* skb,struct FoeEntry *foe_entry)
{	
	struct hwnat_fast_rule_list_t *tmp_entry = NULL;
	
	read_lock(&fast_rule_lock);
	list_for_each_entry(tmp_entry, &fast_rule_index_list, list)	 
	{
		if((tmp_entry->ip_proto == IPPROTO_UDP)&&(foe_entry->bfib1.udp != 1))
			continue;
		else if((tmp_entry->ip_proto == IPPROTO_TCP)&&(foe_entry->bfib1.udp != 0))
			continue;

		if(tmp_entry->sip != foe_entry->ipv4_hnapt.dip)
			continue;

		if(tmp_entry->dip != foe_entry->ipv4_hnapt.sip)
			continue;

		if(tmp_entry->sport != foe_entry->ipv4_hnapt.dport)
			continue;

		if(tmp_entry->dport != foe_entry->ipv4_hnapt.sport)
			continue;
		
		read_unlock(&fast_rule_lock);
		return 1;
	}	
	read_unlock(&fast_rule_lock);

	return 0;
}

int ppeCheckFastRuleBind(struct sk_buff* skb,struct FoeEntry *foe_entry)
{
	unsigned int foe_index = FOE_ENTRY_NUM(skb);
	
	if(!hwnat_fast_rule_enable)
		return 0;

	if((FOE_AI(skb)  != UN_HIT) &&(FOE_AI(skb) != HIT_UNBIND))
		return 0;

	if(!IS_IPV4_HNAPT(foe_entry))
		return 0;

	if((-1 != hwnat_fast_rule_mark_bit)&&(skb->mark&(1<<hwnat_fast_rule_mark_bit)))
	{
		ppeAddNewFastRuleIndex(skb,foe_index);
		if(hwnat_fast_rule_mark_enable)
			return 1;
		else
			return 0;
	}

	if(hwnat_fast_rule_reverse_enable&&ppeCheckFastRuleIndex(skb,foe_entry))
		return 1;
	
	return 0;
}

static int hwnat_fast_bind_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{

	printk("%d\n",hwnat_fast_bind);
    printk("Hash Conflict: %d\n", hwnat_fast_bind_hash_conflict);
	return 0;	
}

static int hwnat_fast_bind_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];
	
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';
	sscanf(get_buf, "%d", &hwnat_fast_bind);
    hwnat_fast_bind_hash_conflict = 0;

	if (3 == hwnat_fast_bind)
	{
		IO_SBITS(PPE_FLOW_CFG, (1<<29));
	}
	else
	{
		IO_CBITS(PPE_FLOW_CFG, (1<<29));
	}

	return count;
}

#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
void ppeSetMulBrVid(struct sk_buff *skb, unsigned short vid)
{
    foe_ext[FOE_ENTRY_NUM(skb)].mul_br_vid = vid;
    return;
}

unsigned short ppeGetMulBrVid(struct sk_buff *skb)
{
    return foe_ext[FOE_ENTRY_NUM(skb)].mul_br_vid;
}
#endif
void ecnt_hwnat_gen_init(void)
{	
	struct proc_dir_entry *hwnat_proc;	
	unsigned int range=0;
	

	if(TCSUPPORT_HWNAT_L2VID_VAL)
	{	
		// init hwnat l2lu key table
		PpeL2LUKeyTblInit();

		ra_sw_nat_hook_rx_set_l2lu = PpeL2luKeyHandler;

		hwnat_proc = create_proc_entry("tc3162/hwnat_l2lu_mask", 0, NULL);
		if (hwnat_proc){
			hwnat_proc->read_proc = hwnat_l2lu_mask_read_proc;
			hwnat_proc->write_proc = hwnat_l2lu_mask_write_proc;
		} else {
			printk("proc_entry tc3162/hwnat_l2lu_mask alloc fail\n");
		}   

		/* add to use L2LU for downstream multicast VLAN */
		hwnat_proc = create_proc_entry("tc3162/hwnat_l2lu_multicast", 0, NULL);
		if (hwnat_proc)
		{
			hwnat_proc->read_proc = hwnat_l2lu_multicast_read_proc;
			hwnat_proc->write_proc = hwnat_l2lu_multicast_write_proc;
		}else {
			printk("proc_entry tc3162/hwnat_l2lu_multicast alloc fail\n");
		} 

		hwnat_proc = create_proc_entry("tc3162/hwnat_l2lu_update_period", 0, NULL);
		if (hwnat_proc){
			hwnat_proc->read_proc = hwnat_l2lu_update_period_read_proc;
			hwnat_proc->write_proc = hwnat_l2lu_update_period_write_proc;
		}else {
			printk("proc_entry tc3162/hwnat_l2lu_update_period alloc fail\n");
		} 
	}
	
	for(range=0;range<1024;range++)
		INIT_LIST_HEAD(&BlackIpTable[range]);
	
	INIT_LIST_HEAD(&BlackIpTableRange);

	INIT_LIST_HEAD(&BlackIpPortProtocolTable);

	spin_lock_init(&black_ip_lock);
	spin_lock_init(&black_ip_port_protocol_lock);
	
	restore_offload_info_hook = restore_offload_info;

	if(TCSUPPORT_SWNAT_VAL)
	{
		get_swnat_clean_flag_hook = get_swnat_clean_flag;
		set_swnat_clean_flag_hook = set_swnat_clean_flag;
	}

	hwnat_proc = create_proc_entry("tc3162/wlan_ssid_ratelimit", 0, NULL);
	hwnat_proc->read_proc = wlan_ssid_ratelimit_read_proc;
	hwnat_proc->write_proc =  wlan_ssid_ratelimit_write_proc;
	
	hwnat_multicast_data_hook = PpeMulticastDataHandler;

	if(TCSUPPORT_HWNAT_WAN_ACCOUNT_VAL)
	{
		ra_sw_nat_set_wan_acntid_hook = PpeSetWanAcntId;
		ra_sw_nat_clear_wan_acntid_hook = PpeClearWanAcntId;
	
		if(TCSUPPORT_SMUX_HW_STAT_VAL)
		{
		#ifdef CONFIG_TP_IMAGE
			ra_sw_nat_get_wan_acntid_counter_hook = PpeGetWanAcntIdCounterTp;
			ra_sw_nat_get_wan_acntid_counter_hook64 = PpeGetWanAcntIdCounterTp64;
			ra_sw_nat_clear_wan_acntid_counter_hook = PpeClearWanAcntIdCounter;
		#else
			ra_sw_nat_get_wan_acntid_counter_hook = PpeGetWanAcntIdCounter;
		#endif /* CONFIG_TP_IMAGE */
 		}
		else if(!isEN751627)
		{
			hwnat_proc = create_proc_entry("tc3162/hwnat_wan_account", 0, NULL);
			if(hwnat_proc)
			{
				hwnat_proc->read_proc = hwnat_wan_account_read_proc;
				hwnat_proc->write_proc = hwnat_wan_account_write_proc;
			}
		}
		memset(ppe_acnt_grp,0,ACNT_GRP_WAN_NUM*sizeof(ppeAcntGrp_t));
	}
	
	hwnat_multicast_set_hwnat_info_hook = hwnat_multicast_set_info;	
	hwnat_set_rule_according_to_state_hook = hwnat_set_rule_according_to_state;
	hwnat_multicast_set_info_for_sfu_hook = hwnat_multicast_set_info_for_sfu;
	
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	ra_sw_nat_set_mul_br_vid_hook = ppeSetMulBrVid;
	ra_sw_nat_get_mul_br_vid_hook = ppeGetMulBrVid;
#endif

	hwnat_proc = create_proc_entry("tc3162/blackip_list", 0, NULL);
	hwnat_proc->read_proc = blackip_read_proc;

	hwnat_proc = create_proc_entry("tc3162/etherwan_use_qdma", 0, NULL);
	hwnat_proc->read_proc = etherwan_use_qdma_read_proc;
	hwnat_proc->write_proc =  etherwan_use_qdma_write_proc;
	
	ppeResources_init();
	


	hwnat_proc = create_proc_entry("tc3162/hwnat_meter", 0, NULL);
	hwnat_proc->read_proc = hwnat_meter_read_proc;
	hwnat_proc->write_proc =  hwnat_meter_write_proc;	

	hwnat_proc = create_proc_entry("tc3162/hwnat_stag_7512autotest", 0, NULL);
	hwnat_proc->write_proc =  hwnat_stag_7512autotest_write_proc;
	
	if (PPE_SHNK_TABLE_SUPPORT) {
		hwnat_proc = create_proc_entry("tc3162/shrink_table_debug", 0, NULL);
		if(hwnat_proc)
		{
			hwnat_proc->read_proc = shrink_table_dump_read_proc;
			hwnat_proc->write_proc =  shrink_table_dump_write_proc;	
		}
        
		hwnat_proc = create_proc_entry("tc3162/hwnat_test", 0, NULL);
		if(hwnat_proc)
		{
			hwnat_proc->read_proc = hwnat_test_read_proc;
			hwnat_proc->write_proc =  hwnat_test_write_proc;	
		}
	}

    /* To create udp_bypass proc  */
	hwnat_proc = create_proc_entry("tc3162/udp_bypass", 0, NULL);
	if (hwnat_proc)
	{
		hwnat_proc->read_proc = udp_bypass_enable_read_proc ;
		hwnat_proc->write_proc = udp_bypass_enable_write_proc ;
	} 
	else 
	{
		printk("proc_entry tc3162/udp_bypass alloc fail\n");
	}

	/* Schedule timer */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	init_timer(&gdm2_update_timer);
	gdm2_update_timer.expires = jiffies + msecs_to_jiffies(250);
	gdm2_update_timer.function = auto_GDM2_update;
#else
	timer_setup(&gdm2_update_timer, auto_GDM2_update, 0);
	gdm2_update_timer.expires = jiffies + msecs_to_jiffies(250);
#endif
	add_timer(&gdm2_update_timer);

	memset(&ppe_meter_acnt_res, 0x0, sizeof(ppe_meter_acnt_res));
	
	
	ppeFastRuleListInit();

	hwnat_proc = create_proc_entry("tc3162/hwnat_fast_bind", 0, NULL);
	hwnat_proc->read_proc = hwnat_fast_bind_read_proc;
	hwnat_proc->write_proc =  hwnat_fast_bind_write_proc;
	hwnat_proc = create_proc_entry("tc3162/udp_session_test", 0, NULL);
	hwnat_proc->read_proc = udp_session_test_read_proc;
	hwnat_proc->write_proc = udp_session_test_write_proc;
	if(TCSUPPORT_CT_JOYME4_VAL)
		udpSessionTest.enable = 1;

	if(TCSUPPORT_CT_JOYME4_VAL)
	{
		hwnat_proc = create_proc_entry("tc3162/natv6_2544_test", 0, NULL);
		hwnat_proc->read_proc = natv6_2544_test_read_proc;
		hwnat_proc->write_proc = natv6_2544_test_write_proc;
	}

    memset(&static_rule_ext,0,sizeof(STATIC_RULE_EXT_t));
    hwnat_proc = create_proc_entry("tc3162/hwnat_static_rule_ext", 0, NULL);
    hwnat_proc->read_proc = hwnat_static_rule_ext_read_proc;
	hwnat_proc->write_proc = hwnat_static_rule_ext_write_proc;

	hwnat_proc = create_proc_entry("tc3162/hwnat_tunnel_offset_in_hash", 0, NULL);
	hwnat_proc->read_proc = hwnat_tunnel_offset_in_hash_read_proc;
	hwnat_proc->write_proc = hwnat_tunnel_offset_in_hash_write_proc;

#ifdef CONFIG_TP_IMAGE
	foe_intf = (struct FoeEntryIntf *)vmalloc(sizeof(struct FoeEntryIntf) * FOE_4TB_SIZ_TOTAL);

	if (foe_intf)
	{
		/* FOE_4TB_SIZ_TOTAL: 32768 */
		memset(foe_intf, 0, sizeof(struct FoeEntryIntf) * FOE_4TB_SIZ_TOTAL);
	}

	foe_args_size = sizeof(struct hwnat_entries_args) + sizeof(struct hwnat_tuple_ext) * FOE_ARGS_MAX_ENTRIES_NUM;
	foe_args = (struct hwnat_entries_args *)vmalloc(foe_args_size);

	if (foe_args)
	{
		/* sizeof(struct hwnat_entries_args): 64 Bytes
		 * sizeof(struct hwnat_tuple_ext): 224 Bytes
		 * FOE_ARGS_MAX_ENTRIES_NUM: 2048
		 */
		memset(foe_args, 0, foe_args_size);
	}
#endif /* CONFIG_TP_IMAGE */

	return;
}

void ecnt_hwnat_gen_deinit(void)
{	

	if(TCSUPPORT_HWNAT_L2VID_VAL)
	{
		ra_sw_nat_hook_rx_set_l2lu = NULL;
		remove_proc_entry("tc3162/hwnat_l2lu_update_period", NULL);
		remove_proc_entry("tc3162/hwnat_l2lu_multicast", NULL);
		remove_proc_entry("tc3162/hwnat_l2lu_mask", NULL);
	}
	
	remove_proc_entry("tc3162/wlan_ssid_ratelimit", NULL);
	if(soft_ratelimit_recyclequeue_hook)
		soft_ratelimit_recyclequeue_hook(wifiqueue_start_idx,SINGLE_WLAN_SSID_NUM*4);

	if (offload_info_entry)
		kfree(offload_info_entry);
	
	restore_offload_info_hook = NULL;

	get_swnat_clean_flag_hook = NULL;
	set_swnat_clean_flag_hook = NULL;
	
	hwnat_multicast_data_hook = NULL;

	if (foe_ext != NULL){
		if (SUPPORT_ALLOC_DMA_BUF_IN_KERNEL){
			vfree(foe_ext);
		}
		else{
			kfree(foe_ext);
		}
	}
	foe_ext = NULL;

	hwnat_multicast_set_hwnat_info_hook = NULL;	
	hwnat_set_rule_according_to_state_hook = NULL;
	hwnat_multicast_set_info_for_sfu_hook = NULL;
	
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	ra_sw_nat_set_mul_br_vid_hook = NULL;
	ra_sw_nat_get_mul_br_vid_hook = NULL;
#endif
	remove_proc_entry("tc3162/blackip_list", NULL);
	remove_proc_entry("tc3162/etherwan_use_qdma", NULL);

	remove_proc_entry("tc3162/hwnat_meter", NULL);	
	
	remove_proc_entry("tc3162/hwnat_stag_7512autotest", NULL);

	if (PPE_SHNK_TABLE_SUPPORT){
		remove_proc_entry("tc3162/shrink_table_debug", NULL);
		remove_proc_entry("tc3162/hwnat_test", NULL);
	}
	
	if(TCSUPPORT_HWNAT_WAN_ACCOUNT_VAL){
		ra_sw_nat_set_wan_acntid_hook = NULL;
		ra_sw_nat_clear_wan_acntid_hook = NULL;
		if(TCSUPPORT_SMUX_HW_STAT_VAL)
		{
			ra_sw_nat_get_wan_acntid_counter_hook = NULL;
			#ifdef CONFIG_TP_IMAGE
			ra_sw_nat_get_wan_acntid_counter_hook64 = NULL;
			ra_sw_nat_clear_wan_acntid_counter_hook = NULL;
			#endif /* CONFIG_TP_IMAGE */
		}
		else if(!isEN751627)
		{
			remove_proc_entry("tc3162/hwnat_wan_account", NULL);
		}
	}
	remove_proc_entry("tc3162/udp_bypass",NULL);

    remove_proc_entry("tc3162/hwnat_static_rule_ext",NULL);
	remove_proc_entry("tc3162/hwnat_tunnel_offset_in_hash", NULL);

	FE_API_DEINIT_RESOURCE_MANAGE();	
    
	/* To delete timer */
	del_timer_sync(&gdm2_update_timer);

	ppeClearFastRuleList();

	remove_proc_entry("tc3162/hwnat_fast_bind", NULL);

	if(TCSUPPORT_CT_JOYME4_VAL)
	{
		remove_proc_entry("tc3162/natv6_2544_test", NULL);
	}

	remove_proc_entry("tc3162/udp_session_test", NULL);

#ifdef CONFIG_TP_IMAGE
	if (foe_intf)
	{
		vfree(foe_intf);
		foe_intf = NULL;
	}
	if (foe_args)
	{
		vfree(foe_args);
		foe_args = NULL;
		foe_args_size = 0;
	}
#endif /* CONFIG_TP_IMAGE */

	return;
}

