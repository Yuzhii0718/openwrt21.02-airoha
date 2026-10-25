/***************************************************************
Copyright Statement:

This software/firmware and related documentation (EcoNet Software) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (EcoNet) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (ECONET SOFTWARE) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN AS IS 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/if_vlan.h>
#include <linux/if_ether.h>
#include <linux/ppp_defs.h>
#include <net/ipv6.h>
#include <net/tcp.h>
#include <asm/checksum.h>
#include <linux/in.h>


#include "lro.h"
#include "lro_glb.h"

#include <ecnt_hook/ecnt_hook.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_qdma_type.h>
#include <linux/foe_hook.h>
#if defined(TCSUPPORT_OPENWRT)
#include <qdma_bmgr.h>
#else
#include "../qdma/EN7516/qdma_bmgr.h"
#endif


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

#define INITIALIZED 1

#define LRO_NAPI_WEIGHT     (256)


/************************************************************************
*                  M A C R O S
*************************************************************************
*/    

enum lro_ring_state{
	LRO_RING_FREE = 0,
	LRO_RING_RESERVED,
};

/************************************************************************
*                  D A T A     T Y P E S
*************************************************************************
*/


/************************************************************************
*                  E X T E R N A L     D A T A   D E C L A R A T I O N S
*************************************************************************
*/

extern struct ecnt_hook_ops ecnt_driver_lro_op;
extern int (*ookla_offload_rcv_hook)(struct sk_buff *skb);
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
static int lro_enable_set(uint ctrl_idx, uint ring_idx);
static int lro_enable_get(uint ctrl_idx);
static int lro_enable_reset(uint ctrl_idx);


static int lro_packet_receive(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len, QDMA_RxInfo_T* lro_info);
static int lro_callback_EventHandler(QDMA_EventType_t qdmaEventType);
static int lro_napiPoll(struct napi_struct *napi, int budget);
static int lro_netdev_open(struct net_device *dev);
static int lro_netdev_start(struct net_device *dev);
static int lro_netdev_close(struct net_device *dev);
static int lro_netdev_set_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
static int lro_write_proc(struct file *file, const char *buffer,
    unsigned long count, void *data);
static int lro_read_proc(char *buf, char **start, off_t off, int count,
				 int *eof, void *data);


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

struct net_device *lroNapiDev = NULL;

/*
const static HWLRO_DBG_FUNC hw_lro_dbg_func[] =
{
	[0] = hwlro_agg_cnt_ctrl,
	[1] = hwlro_agg_time_ctrl,
	[2] = hwlro_age_time_ctrl,
	[3] = hwlro_pkt_int_alpha_ctrl,
	[4] = hwlro_threshold_ctrl,
	[5] = hwlro_fix_setting_switch_ctrl,
	[6] = hwlro_sdl_size_ctrl,
	[7] = hwlro_ring_enable_ctrl,
};*/
	



/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
	
static DEFINE_SPINLOCK(LRO_NAPI_LOCK);
static const struct net_device_ops lro_netdev_ops = {
	.ndo_init		   = lro_netdev_start,
	.ndo_open		   = lro_netdev_open,
	.ndo_stop		   = lro_netdev_close,
	.ndo_do_ioctl	   = lro_netdev_set_ioctl,
	.ndo_start_xmit	 = NULL,
	.ndo_change_mtu	 = NULL,
	.ndo_validate_addr  = NULL,
};


static uint8 defMacAddr[] = {0x00, 0xaa, 0xbb, 0x09, 0x76, 0xcc};

int lroDbgLevel=0;
int lroDbgStatus=0;


/************************************************************************
*				   LRO STATISTICS BEGIN
*************************************************************************
*/

/* For LRO statistics. */
unsigned int lro_agg_num_cnt[LRO_RING_NUM][LRO_MAX_AGG_CNT+1] = {{0}};
unsigned int lro_agg_size_cnt[LRO_RING_NUM][16] = {{0}};
unsigned int lro_tot_agg_cnt[LRO_RING_NUM] = {0};
unsigned int lro_tot_flush_cnt[LRO_RING_NUM] = {0};
/* For LRO debug. */
unsigned int lro_flush_cnt[LRO_RING_NUM][LRO_FLUSH_REASON_NUM] = {{0}};
char lro_flush_reason[LRO_FLUSH_REASON_NUM][50] = {
	"Total agg timeout flush",
	"Neighber age timeout flush",
	"Agg length reach flush",
	"Agg count reach flush",
	"TCP not in sequence flush",
	"Timestamp outorder flush",
	"Next packet nonrule flush",
	"This packet nonrule flush",
	"Qdma droped next packet flush"
};
	
/************************************************************************
*				   LRO STATISTICS END
*************************************************************************
*/

/************************************************************************
*				   LRO RING MANAGEMENT BEGIN
*************************************************************************
*/
	
static int lro_ring_reserve[LRO_RING_NUM][2] = {{0}};

/************************************************************************
*				   LRO RING MANAGEMENT END
*************************************************************************
*/



/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/


/************************************************************************
*				   LRO Control  BEGIN
*************************************************************************
*/
static int lro_enable_set(uint ctrl_idx, uint ring_idx)
{
	uint lro_enable_value = 0, lro_enable_mask = 0, lro_rxq_idx_value = 0, lro_rxq_idx_mask = 0;

	if(SUPPORT_RING_AGGREGATION)
	{
		lro_enable_value = (1 << ctrl_idx);
		lro_enable_mask = (1 << ctrl_idx);
		lro_rxq_idx_value = (ring_idx << ((ctrl_idx&0x3) * 8));
		lro_rxq_idx_mask = (0x1F << ((ctrl_idx&0x3) * 8));

		IO_SREG(LRO_REG_BASE(IS_WAN), ((IO_GREG(LRO_REG_BASE(IS_WAN)) & (~lro_enable_mask)) | lro_enable_value));
		IO_SREG(LRO_REG_RXQ(IS_WAN, ctrl_idx), ((IO_GREG(LRO_REG_RXQ(IS_WAN, ctrl_idx)) & ~ lro_rxq_idx_mask) | lro_rxq_idx_value));
	}
	else
	{
		lro_enable_value = ((1<<(ctrl_idx + 24))|((0xf & ring_idx) << (ctrl_idx * 0x4)));
		lro_enable_mask = ((1<<(ctrl_idx + 24))|(0xf << (ctrl_idx * 0x4)));
		IO_SREG(LRO_REG_BASE(IS_WAN), ((IO_GREG(LRO_REG_BASE(IS_WAN)) & (~lro_enable_mask)) | lro_enable_value));
	}
	return 0;
}

static int lro_enable_get(uint ctrl_idx)
{
	if(SUPPORT_RING_AGGREGATION)
	{
		return (LRO_GET_ENABLE(LRO_REG_BASE(IS_WAN), ctrl_idx) ? LRO_GET_QUEUE_IDX(LRO_REG_RXQ(IS_WAN, ctrl_idx), ctrl_idx) : 0);
	}
	else
	{
		return (LRO_GET_ENABLE(LRO_REG_BASE(IS_WAN), ctrl_idx) ? LRO_GET_QUEUE_IDX(LRO_REG_BASE(IS_WAN), ctrl_idx) : 0);
	}
}

static int lro_enable_reset(uint ctrl_idx)
{
	uint lro_enable_mask = 0, lro_rxq_idx_mask = 0;

	if(SUPPORT_RING_AGGREGATION)
	{
		lro_enable_mask = (1 << ctrl_idx);
		lro_rxq_idx_mask = (0x1F << ((ctrl_idx&0x3) * 8));
		IO_SREG(LRO_REG_BASE(IS_WAN), (IO_GREG(LRO_REG_BASE(IS_WAN)) & (~lro_enable_mask))); 
		IO_SREG(LRO_REG_RXQ(IS_WAN, ctrl_idx), (IO_GREG(LRO_REG_RXQ(IS_WAN, ctrl_idx)) & (~lro_rxq_idx_mask)));  
	}
	else
	{
		lro_enable_mask = ((1<<(ctrl_idx + 24))|(0xf << (ctrl_idx * 0x4)));
		IO_SREG(LRO_REG_BASE(IS_WAN), (IO_GREG(LRO_REG_BASE(IS_WAN)) & (~lro_enable_mask)) );
	}
	return 0;
}

static int lro_enable_set_ctrl(uint ctrl_idx, uint ring_idx)
{
	if(ctrl_idx >= LRO_RING_NUM){
		printk("lro_enable_set_ctrl: ctrl_idx error!\n");
		return -1;
	}
	lro_enable_set(ctrl_idx, ring_idx);
	
	return 0;
}

/******************************************************************************
******************************************************************************/

static int lro_enable_get_ctrl(uint ctrl_idx)
{
	if(ctrl_idx >= LRO_RING_NUM){
		printk("lro_enable_get_ctrl: ctrl_idx error!\n");
		return -1;
	}
	return lro_enable_get(ctrl_idx);
}

/******************************************************************************
******************************************************************************/

static int lro_enable_reset_ctrl(uint ctrl_idx)
{
	if(ctrl_idx >= LRO_RING_NUM){
		printk("lro_enable_reset_ctrl: ctrl_idx error!\n");
		return -1;
	}

	lro_enable_reset(ctrl_idx);
	return 0;
}

/******************************************************************************
******************************************************************************/

static int lro_agg_num_ctrl(uint value)
{
	SET_VALUE(LRO_REG_BASE(IS_WAN) + LRO_AGGNUM_OFF, LRO_AGGNUM_MASK, LRO_AGGNUM_SHIFT, value) ;
	return 0;
}

/******************************************************************************
******************************************************************************/

static int lro_agg_size_ctrl(uint value)
{
	SET_VALUE(LRO_REG_BASE(IS_WAN) + LRO_AGGSIZE_OFF, LRO_AGGSIZE_MASK, LRO_AGGSIZE_SHIFT, value) ;
	return 0;
}

/******************************************************************************
******************************************************************************/
static int lro_age_time_ctrl(uint value)
{
	SET_VALUE(LRO_REG_BASE(IS_WAN) + LRO_AGETIME_OFF, LRO_AGETIME_MASK, LRO_AGETIME_SHIFT, value) ;
	return 0;
}

/******************************************************************************
******************************************************************************/

static int lro_agg_time_ctrl(uint value)
{
	SET_VALUE(LRO_REG_BASE(IS_WAN) + LRO_AGGTIME_OFF, LRO_AGGTIME_MASK, LRO_AGGTIME_SHIFT, value) ;
	return 0;
}

/******************************************************************************
******************************************************************************/

static int lro_agg_rule_set(uint agg_max_size, uint agg_max_cnt, uint agg_time, uint age_time){
	uint32_t reg_val;

	/* set register to update lro rule. */
	reg_val = ((agg_max_cnt & LRO_AGGNUM_MASK) << LRO_AGGNUM_SHIFT) | (agg_max_size & LRO_AGGSIZE_MASK);
	IO_SREG(LRO_REG_BASE(IS_WAN) + LRO_AGGNUM_OFF, reg_val) ;
	LRO_MSG_STRIP(DBG_ST, "LRO_REG_ADDR:%x reg_val:0x%x\n", LRO_REG_BASE(IS_WAN) + LRO_AGGNUM_OFF, reg_val);

	reg_val = ((age_time & LRO_AGETIME_MASK) << LRO_AGETIME_SHIFT) | (agg_time & LRO_AGGTIME_MASK);
	IO_SREG(LRO_REG_BASE(IS_WAN) + LRO_AGETIME_OFF, reg_val) ;
	LRO_MSG_STRIP(DBG_ST, "LRO_REG_ADDR:%x reg_val:0x%x\n", LRO_REG_BASE(IS_WAN) + LRO_AGETIME_OFF, reg_val);

	return 0;
}

/******************************************************************************
******************************************************************************/

static int lro_agg_rule_dump(void){
	uint32_t reg_val;

	printk("LRO Config for %s.\n", lro_path);

	/* set register to update lro rule. */
	reg_val = IO_GREG(LRO_REG_BASE(IS_WAN) + LRO_AGGNUM_OFF);
	printk("Max aggregate number: %d\n", ((reg_val >> LRO_AGGNUM_SHIFT) & LRO_AGGNUM_MASK) );
	printk("Max aggregate size: %d\n", ((reg_val >> LRO_AGGSIZE_SHIFT) & LRO_AGGSIZE_MASK) );
	
	reg_val = IO_GREG(LRO_REG_BASE(IS_WAN)+LRO_AGETIME_OFF);
	printk("Max aggregate timeout: %d\n", ((reg_val >> LRO_AGGTIME_SHIFT) & LRO_AGGTIME_MASK) );
	printk("Max age timeout: %d\n", ((reg_val >> LRO_AGETIME_SHIFT) & LRO_AGETIME_MASK) );

	return 0;
}

/************************************************************************
*				   LRO Control  END
*************************************************************************
*/



/************************************************************************
*				   LRO STATISTICS BEGIN
*************************************************************************
*/


/******************************************************************************
******************************************************************************/
static int lro_len_idx(unsigned int agg_size)
{
	int len_idx;

	if(agg_size > 65000) {
		len_idx = 13;
	}else if(agg_size > 60000) {
		len_idx = 12;
	}else if(agg_size > 55000) {
		len_idx = 11;
	}else if(agg_size > 50000) {
		len_idx = 10;
	}else if(agg_size > 45000) {
		len_idx = 9;
	}else if(agg_size > 40000) {
		len_idx = 8;
	}else if(agg_size > 35000) {
		len_idx = 7;
	}else if(agg_size > 30000) {
		len_idx = 6;
	}else if(agg_size > 25000) {
		len_idx = 5;
	}else if(agg_size > 20000) {
		len_idx = 4;
	}else if(agg_size > 15000) {
		len_idx = 3;
	}else if(agg_size > 10000) {
		len_idx = 2;
	}else if(agg_size > 5000) {
		len_idx = 1;
	}else {
		len_idx = 0;
	}

	return len_idx;
}
	

/******************************************************************************
******************************************************************************/
static void lro_status_clear(void){
	memset(lro_agg_num_cnt, 0, sizeof(lro_agg_num_cnt));
	memset(lro_agg_size_cnt, 0, sizeof(lro_agg_size_cnt));
	memset(lro_tot_agg_cnt, 0, sizeof(lro_tot_agg_cnt));
	memset(lro_tot_flush_cnt, 0, sizeof(lro_tot_flush_cnt));
}


/******************************************************************************
******************************************************************************/
static void lro_flush_clear(void){
	memset(lro_flush_cnt, 0, sizeof(lro_flush_cnt));
}

/******************************************************************************
******************************************************************************/
static int lro_status_dump(uint ring_idx)
{
	unsigned int arr_idx;
	int i=0 ;
	
	arr_idx = ring_idx - LRO_INDEX_OFF;
	
	printk("\nLRO aggregation information of Rx Ring%d:\n", ring_idx);

	if( arr_idx < LRO_RING_NUM ){
		printk("lro_tot_agg_cnt:%d\n", lro_tot_agg_cnt[arr_idx]);
		printk("lro_tot_flush_cnt:%d\n", lro_tot_flush_cnt[arr_idx]);
		for(i=0; i< LRO_MAX_AGG_CNT; i++){
			if(lro_agg_num_cnt[arr_idx][i]){
				printk("agg_num:%d, match count:%d\n", i, lro_agg_num_cnt[arr_idx][i]);
			}
		}
		for(i=0; i< 16; i++){
			if(lro_agg_size_cnt[arr_idx][i]){
				printk("lro_agg_size_index:%d, match count:%d\n", i, lro_agg_size_cnt[arr_idx][i]);
			}
		}
	}

	return 0;
}

/******************************************************************************
******************************************************************************/

static int lro_status_update(uint ring_idx, uint agg_cnt, uint agg_size)
{
	unsigned int arr_idx;
	
	arr_idx = ring_idx - LRO_INDEX_OFF;
	
	if(arr_idx < LRO_RING_NUM)
	{
		lro_agg_size_cnt[arr_idx][lro_len_idx(agg_size)]++;
		lro_agg_num_cnt[arr_idx][agg_cnt]++;
		lro_tot_flush_cnt[arr_idx]++;
		lro_tot_agg_cnt[arr_idx] += agg_cnt;
	}

	return 0;
}


/******************************************************************************
******************************************************************************/

static int lro_flush_dump(uint ring_idx)
{
	unsigned int arr_idx;
	int i;
	
	arr_idx = ring_idx - LRO_INDEX_OFF;
	
	printk("Flush times for each reason of Rx Ring%d:\n", ring_idx);
	if(arr_idx < LRO_RING_NUM)
	{
		for(i=0; i < LRO_FLUSH_REASON_NUM; i++){
			printk("%08d ,%s\n", lro_flush_cnt[arr_idx][i], lro_flush_reason[i]);
		}
	}

	return 0;
}


/******************************************************************************
******************************************************************************/

static int lro_flush_update(uint ring_idx, uint flush_reason)
{
	unsigned int arr_idx;
	
	arr_idx = ring_idx - LRO_INDEX_OFF;
	
	if( arr_idx < LRO_RING_NUM )
	{
		lro_flush_cnt[arr_idx][flush_reason]++;
	}

	return 0;
}

/************************************************************************
*				   LRO STATISTICS END
*************************************************************************
*/


/************************************************************************
*				   LRO RING MANAGEMENT BEGIN
*************************************************************************
*/
int get_free_ring(int ppe_entry_idx){
	int i = 0;
	
	LRO_MSG(DBG_MSG ,"get_free_ring in, ppe_entry_idx:%d.\n", ppe_entry_idx);

	/* check if the entry has been binded. */
	for(i = 0; i < LRO_RING_NUM; i++){
		if(lro_ring_reserve[i][0]==LRO_RING_RESERVED){
			if(ra_sw_nat_hook_is_entry_valid){
				if(ra_sw_nat_hook_is_entry_valid(lro_ring_reserve[i][1], (LRO_RING_START+i))){/* current ppe entry has aged out. */
					lro_ring_reserve[i][1] = ppe_entry_idx;
					LRO_MSG(DBG_ST ,"Return a Aged LRO ring:%d, ppe_entry_idx:%d.\n", i, ppe_entry_idx);
					return (LRO_RING_START+i);
				}
			}
		}
		if((lro_ring_reserve[i][0] == LRO_RING_RESERVED) && (lro_ring_reserve[i][1] == ppe_entry_idx)){
			LRO_MSG(DBG_ST ,"Return a Resrved LRO ring:%d, ppe_entry_idx:%d.\n", i, ppe_entry_idx);
			return (LRO_RING_START+i);
		}
	}

	/* scan for free Rx ring. */
	for(i = 0; i < LRO_RING_NUM; i++){
		if(lro_ring_reserve[i][0] == LRO_RING_FREE){
			lro_ring_reserve[i][1] = ppe_entry_idx;
			lro_ring_reserve[i][0] = LRO_RING_RESERVED;
			LRO_MSG(DBG_ST ,"Return a Free LRO ring:%d, ppe_entry_idx:%d.\n", i, ppe_entry_idx);
			return (LRO_RING_START+i);
		}
	}

	LRO_MSG(DBG_ST ,"No Free LRO ring.\n" );

	return -1;
}

/************************************************************************
*				   LRO RING MANAGEMENT END
*************************************************************************
*/


static int lro_init(void)
{
	int error = 0;
	struct net_device *dev = (struct net_device *)NULL;
	LRO_Adapter_T *lroApt = (LRO_Adapter_T *)NULL;
	QDMA_InitCfg_t qdmaApiInitCfg;
	struct proc_dir_entry *lro_proc = NULL ;
	uint lro_ring_idx = 0;
	
	memset(&qdmaApiInitCfg, 0, sizeof(QDMA_InitCfg_t));

	/* Init qdma recieve and event handler callbacks. */
	qdmaApiInitCfg.cbInt2RecvPkts = (qdma_callback_int_handler_t)lro_packet_receive;
	qdmaApiInitCfg.cbInt2EventHandler = (qdma_callback_int_handler_t)lro_callback_EventHandler;
	qdmaApiInitCfg.macType = MAC_TYPE_LRO;
	qdmaApiInitCfg.txRecycleThrh = 4;

	QDMA_API_INIT(QDMA_TYPE, &qdmaApiInitCfg);
	
	if(QDMA_RX_MODE == QDMA_RX_NAPI) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19)
		dev = alloc_netdev(sizeof(LRO_Adapter_T), LRO_DEV_NAME, NET_NAME_USER, ether_setup);
#else
		dev = alloc_netdev(sizeof(LRO_Adapter_T), LRO_DEV_NAME, ether_setup);
#endif
		if (!dev)
		{
			printk("\n\nFAILED: lro_dev net_device allocation\n\n");
			return -ENOMEM;
		}
		
		lroNapiDev = dev;
		lroApt = netdev_priv(dev);
		memset(lroApt, 0, sizeof(LRO_Adapter_T));
		
		lroNapiDev->netdev_ops = &lro_netdev_ops;
		lroApt->napi.weight = LRO_NAPI_WEIGHT;
		netif_napi_add(dev, &lroApt->napi, lro_napiPoll, lroApt->napi.weight);
		
		error = register_netdev(dev);
		if (error)
		{
			printk("\n\nFAILED: lro_dev net_device register fail\n\n");
			free_netdev(dev);
			return error;
		}
		
		set_bit(__LINK_STATE_START, &lroNapiDev->state);
		
		if (lroNapiDev->netdev_ops->ndo_open){
			error = lroNapiDev->netdev_ops->ndo_open(lroNapiDev);
			if (error)
				clear_bit(__LINK_STATE_START, &lroNapiDev->state);
		}
	}

	/* enable LRO for ring 11 ~ 14, set agg_size as 40000, set agg_count as 64, set agg_time 10ms, set age_time as 50ms. */
	for(lro_ring_idx = 0; lro_ring_idx < LRO_RING_NUM; lro_ring_idx++){
		lro_enable_set_ctrl(lro_ring_idx, LRO_RING_START + lro_ring_idx);
	}

	lro_agg_rule_set(LRO_MAX_AGG_SIZE, LRO_MAX_AGG_CNT,  LRO_SAMBA_AGG_TIME, LRO_MAX_AGE_TIME);
	
	/* create proc node */
	lro_proc = create_proc_entry(lro_path, 0, NULL);
	if(lro_proc) {
		lro_proc->read_proc = lro_read_proc;
		lro_proc->write_proc = lro_write_proc;
	}
	
	return 0;
}

/******************************************************************************
******************************************************************************/
static int lro_netdev_open(struct net_device *dev)
{
	LRO_Adapter_T *lroApt = netdev_priv(dev);	

	napi_enable(&lroApt->napi);

	netif_start_queue(dev);

	return 0;
}

/******************************************************************************
******************************************************************************/


static int lro_netdev_close(struct net_device *dev)
{
	LRO_Adapter_T *lroApt = netdev_priv(dev);
	
	printk("lro_netdev_close\n");

	printk("%s: stoping interface.\n", dev->name);

	netif_stop_queue(dev);

	napi_disable(&lroApt->napi);

	return 0;
}


static int lro_netdev_set_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	/* Not implemented yet */
	  return 0;
}


static int lro_netdev_start(struct net_device *dev)
{
	uint8 flashMacAddr[] = {0x00, 0xAA, 0xBB, 0x01, 0x23, 0x45};
	
	if (flashMacAddr[0] == 0 && flashMacAddr[1] == 0 && flashMacAddr[2] == 0 &&
		flashMacAddr[3] == 0 && flashMacAddr[4] == 0 && flashMacAddr[5] == 0 )
		printk(KERN_INFO "\nThe MAC address in flash is null!\n");		
	else	
		memcpy(defMacAddr, flashMacAddr, 6);	  

	memcpy(dev->dev_addr, defMacAddr, 6);
	dev->addr_len = 6;

	return 0;
}

/******************************************************************************
******************************************************************************/
	
static int lro_napiPoll(struct napi_struct *napi, int budget)
{

	int n;
	unsigned long flags;

	/* call eth_rx to receive a packet 
	 * until all packets are received */
	n = QDMA_API_RECEIVE_PACKETS_INT2(QDMA_TYPE, budget);

	if (n < budget)
	{
		spin_lock_irqsave(&LRO_NAPI_LOCK, flags);
	
		__napi_complete(napi);
		QDMA_API_ENABLE_RXPKT_INT2(QDMA_TYPE);

		spin_unlock_irqrestore(&LRO_NAPI_LOCK, flags);
	}
	
	return n;

}

/******************************************************************************
******************************************************************************/
static int lro_callback_EventHandler(QDMA_EventType_t qdmaEventType)
{
	LRO_Adapter_T *lroApt = netdev_priv(lroNapiDev);

	if ((qdmaEventType == QDMA_EVENT_RECV_PKTS) ||
		(qdmaEventType == QDMA_EVENT_NO_RX_BUFFER))
	{
		if (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER)
			LRO_MSG(DBG_WARN, "QDMA_EVENT_NO_RX_BUFFER\n");
	
		LRO_MSG(DBG_MSG, "lro_callback_EventHandler come in, qdmaEventType is %s\n"
			, ((qdmaEventType == QDMA_EVENT_RECV_PKTS)?"QDMA_EVENT_RECV_PKTS":"QDMA_EVENT_NO_RX_BUFFER"));

		/* because no dev info available, we always
		 * use nas8's poll function. */
		if (napi_schedule_prep(&lroApt->napi))
		{
			LRO_MSG(DBG_MSG, "napi_schedule_prep come in\n");

			QDMA_API_DISABLE_RXPKT_INT2(QDMA_TYPE); /* disable RX interrupt. */
			__napi_schedule(&lroApt->napi);
		}
	}
	else
		LRO_MSG(DBG_WARN, "Wrong QDMA Event Type: %d\n", qdmaEventType);

	return 0;
}

/******************************************************************************
******************************************************************************/

void dump_skb(struct sk_buff *skb)
{
	char tmp[80];
	char *p = (char *)K0_TO_K1(skb->data);
	char *t = (char *)tmp;
	int i, n = 0, line_len;
	

	printk("skb=%08lx skb->head=%08lx skb->end=%08lx \n data=%08lx len=%d data_len=%d mark=%08lX\n", 
		(uint32) skb, (uint32) skb->head, (uint32) skb->end, (uint32) skb->data, skb->len, skb->data_len, (uint32)skb->mark);
	line_len = skb->len - skb->data_len;
	for (i = 0; i < line_len; i++) {
		t += sprintf(t, "%02x ", *p++ & 0xff);
		if ((i & 0x0f) == 0x0f) {
			printk("%04x: %s\n", n, tmp);
			n += 16;
			t = tmp;
		}
	}
	if (i & 0x0f)
		printk("		%04x: %s\n", n, tmp);

}

#if !defined(LRO_LAN)

extern int (*wan_speed_test_hook)(struct sk_buff*);

int IFC_RULE_GOTO_SPEED_TEST( struct sk_buff *skb,uint rx_len)
{
	if(unlikely(skb == NULL)){
		printk("Error Skb NULL, fail to receive!\n");
		return 0;
	}

	if (unlikely((rx_len < 52) || (rx_len > SKBMGR_LRO_RX_BUF_LEN)))
	{
		dev_kfree_skb_any(skb);
		return 1;
	}
	skb_put(skb, rx_len);  
	skb->protocol = eth_type_trans(skb,lroNapiDev);


	if(wan_speed_test_hook)
	{
		if(wan_speed_test_hook(skb)!=0){
			dev_kfree_skb_any(skb);
			return 0;
		}
	}
	return 1;

}
#endif


struct net_device	*rx_general_dev = NULL;
static int lro_packet_ookla_receive(lro_rx_msg_t *rx_msg, 
	struct sk_buff *skb, QDMA_RxInfo_T* lro_info, uint rx_len)
{
	struct tcphdr *th = NULL;
	struct iphdr *iph = NULL;
	
	uint16_t agg_cnt = 0;
	uint16_t stag = 0;
	
	stag = rx_msg->raw.sp_tag;
	if(stag != 0xAAAA)
		return -1;
	
	agg_cnt = (rx_msg->msg[2] >> LRO_RXMSG_AGGCOUNT_SHIFT) & LRO_RXMSG_AGGCOUNT_MASK;

	iph = (struct iphdr *)(skb->data + 14);

	if(agg_cnt > 1){
		/* Update IPv4 head. */
		iph->tot_len = htons(rx_msg->raw.agg_ipv4_total_len);
	
		th = (struct tcphdr *)(skb->data + 14 + (iph->ihl << 2));
		th->ack_seq = htonl(lro_info->tcp_ack_seq);
		th->window = htons(rx_msg->raw.tcp_win);

		if (th->doff == TCP_HEAD_LEN_TIMESTAMP) {
			__be32 *topt = (__be32 *)(th + 1);

			if (*topt == htonl((TCPOPT_NOP << 24) | (TCPOPT_NOP << 16)
					   | (TCPOPT_TIMESTAMP << 8)
					   | TCPOLEN_TIMESTAMP)){
				topt+=2;
				*topt = htonl(lro_info->tcp_ts_rply);
			}
		}
	}

	skb_put(skb, rx_len);  
	skb->dev = rx_general_dev;
	skb->protocol = eth_type_trans(skb,skb->dev);
	skb->ppe_magic = 0xAAAA;
	
	if(ookla_offload_rcv_hook && skb->dev){
		ookla_offload_rcv_hook(skb);
	}
	else
		dev_kfree_skb_any(skb);

	return 0;
}

/******************************************************************************
******************************************************************************/
/* call back function for packets recieve. */
static int lro_packet_receive(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len, QDMA_RxInfo_T* lro_info)
{
	uint offset, iph_off = 0, th_off = 0, ip6h_off = 0;
	uint iph_en = 0, ip6h_en = 0, th_en = 0;
	struct tcphdr *th = NULL;
	struct iphdr *iph = NULL;
	struct ipv6hdr *ip6h = NULL;
	struct sk_buff *newskb = NULL;
	
	lro_rx_msg_t *rx_msg; 
	uint16_t ip_id = 0;
	uint16_t l2_len;
	uint16_t agg_cnt = 0, flush_reason;

	/* record for ip id check. */
	static uint16_t last_agg_cnt = 0;
	static uint16_t last_id = 0;
	static uint32_t last_rxmsg2 = 0;
	
	rx_msg = (lro_rx_msg_t*)msg_p;
	LRO_MSG(DBG_ST ,"msg0:%x, msg1:%x, msg2:%x, msg3:%x\n", (unsigned int)rx_msg->msg[0], (unsigned int)rx_msg->msg[1], (unsigned int)rx_msg->msg[2], (unsigned int)rx_msg->msg[3]);
	if(0 == lro_packet_ookla_receive(rx_msg, skb, lro_info,rx_len))
		return 0;
	
	/* update lro statistics.  */
	l2_len = rx_msg->raw.l2_len;
	agg_cnt = (rx_msg->msg[2] >> LRO_RXMSG_AGGCOUNT_SHIFT) & LRO_RXMSG_AGGCOUNT_MASK;
	flush_reason = (rx_msg->msg[2] >> LRO_RXMSG_FLUSHRESON_SHIFT) & LRO_RXMSG_FLUSHRESON_MASK;

	/* Record statistics. */
	if(lroDbgStatus){
		lro_status_update(lro_info->ring_idx, agg_cnt, rx_len);
		lro_flush_update(lro_info->ring_idx, flush_reason);
	}

	/* if skb->data less than 1500B, then copy to small skb. */

	if(rx_len <= LRO_SHORT_LEN){
		/* copy the packet buffer to new skb for short packets */
		newskb = skbmgr_dev_alloc_skb2k();
		if(newskb) {
			skb_reserve(newskb, NET_IP_ALIGN) ;
			memcpy(newskb->data, skb->data, rx_len) ;

			dev_kfree_skb_any(skb);
			skb = newskb;
		}
	}

	if(rx_msg->raw.ip4){
		iph_en = 1;
		iph_off = l2_len;
		iph = (struct iphdr *)(skb->data + iph_off);

		if(rx_msg->raw.ip6){
			LRO_ERR("ipv4 & ipv6 are both detected!\n");
		}
		
		if(iph->protocol == IPPROTO_TCP){				/* IPv4+TCP */
			th_en = 1;
			th_off = iph_off+ (iph->ihl << 2);
		}else if (iph->protocol == IPPROTO_IPV6){		/* 6RD: IPv4+IPv6 */
			ip6h_en = 1;
			ip6h_off = iph_off+ (iph->ihl << 2);
			ip6h = (struct ipv6hdr *)(skb->data + ip6h_off);

			if (ip6h->nexthdr == NEXTHDR_TCP) {			/* 6RD+TCP */
				th_en = 1;
				offset = ((uint8_t *) ip6h + sizeof(struct ipv6hdr)) - skb->data;
			}
			else{
				LRO_ERR("Non TCP packet detected!\n");
				dev_kfree_skb_any(skb);
				return -1;
			}
		}
		else{
			LRO_ERR("Non TCP packet detected!\n");
			dev_kfree_skb_any(skb);
			return -1;
		}
	}
	else if(rx_msg->raw.ip6){
		ip6h_en = 1;
		ip6h_off = l2_len;
		ip6h = (struct ipv6hdr *)(skb->data + l2_len);
		
		if (ip6h->nexthdr == NEXTHDR_TCP){				/* IPv6+TCP */
			th_en = 1;
			th_off = (ip6h_off + sizeof(struct ipv6hdr));

		}else if (ip6h->nexthdr == IPPROTO_IPIP) {		/* DS-Lite: IPv6+IPv4 error. */
			iph_en = 1;
			iph_off = ip6h_off + sizeof(struct ipv6hdr);
			iph = (struct iphdr *)(skb->data + iph_off);
			
			if (iph->protocol == IPPROTO_TCP) {			/* DS-Lite+TCP */
				th_en = 1;
				th_off = iph_off + (iph->ihl << 2);
			}
			else{
				LRO_ERR("Non TCP packet detected!\n");
				dev_kfree_skb_any(skb);
				return -1;
			}
		}else{
			LRO_ERR("Non TCP packet detected!\n");
			dev_kfree_skb_any(skb);
			return -1;
		}

	}
	else{
		LRO_ERR("Non IP packet detected!\n");
		dev_kfree_skb_any(skb);
		return -1;
	}

	/* check id in ipv4. */
	if(rx_msg->raw.ip4 && (lroDbgLevel >= DBG_ST)){
		ip_id = ntohs(iph->id);
		if(ip_id != (last_id + last_agg_cnt)){
			printk("ip_id:%d, last_id:%d, last_agg_cnt:%d, agg_cnt:%d, last_msg2:%x,msg2:%x\n", ip_id, last_id, last_agg_cnt, agg_cnt, last_rxmsg2, (unsigned int)rx_msg->msg[2]);
		}
		last_id = ip_id;
		last_agg_cnt = agg_cnt;
		last_rxmsg2 = rx_msg->msg[2];		
	}

	/* if not aggregated packet pass through. */
	if(agg_cnt == (uint32_t)1){
#ifdef LRO_LAN
		ETHER_API_RECV_PKT(msg_p, msg_len, skb, rx_len);
#else
		IFC_RULE_GOTO_SPEED_TEST(skb,rx_len);
#endif
		return 0;
	}

	/* Update IPv4 head. */
	if(iph_en){
		iph->tot_len = htons(rx_msg->raw.agg_ipv4_total_len);
		iph->check = 0;
		iph->check = ip_fast_csum((void*)iph, iph->ihl); /* caculate ip head checksum. */
	}
	
	if(ip6h_en && th_en){
		ip6h->payload_len = htons(rx_len - ip6h_off - 40);
	}
	
	/* Update TCP head. */
#if 0//for th_en can never equal to 0
	if(!th_en){
		printk("Non TCP packet in LRO\n");

		if(lroDbgLevel >= DBG_ST){
			skb_put(skb, rx_len);
			dump_skb(skb);
			skb->len -= rx_len;
			skb->tail -= rx_len;
		}
	}
	else
#endif
	{
		th = (struct tcphdr *)(skb->data + th_off);
		/* update ACK Sequence & window size in TCP head. */
		th->ack_seq = htonl(lro_info->tcp_ack_seq);
		th->window = htons(rx_msg->raw.tcp_win);

		/* check tcp options (update timestamp reply) */
		if (th->doff == TCP_HEAD_LEN_TIMESTAMP) {
			__be32 *topt = (__be32 *)(th + 1);

			if (*topt == htonl((TCPOPT_NOP << 24) | (TCPOPT_NOP << 16)
					   | (TCPOPT_TIMESTAMP << 8)
					   | TCPOLEN_TIMESTAMP)){
				topt+=2;
				/* copy timestamp reply. */
				*topt = htonl(lro_info->tcp_ts_rply);
			}
		}

		skb->ip_summed = CHECKSUM_UNNECESSARY;	/* TCP Checksum is not caculated. */
	}
	
#ifdef LRO_LAN
	ETHER_API_RECV_PKT(msg_p, msg_len, skb, rx_len);
#else
	IFC_RULE_GOTO_SPEED_TEST(skb,rx_len);
#endif	
	return 0;
}

/******************************************************************************
******************************************************************************/
static int lro_read_proc(char *buf, char **start, off_t off, int count,
				 int *eof, void *data)
{
	int index = 0;
	off_t begin = 0;

	printk("lroDbgLevel:%d\n", lroDbgLevel);

	*eof = 1;
	
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

/*****************************************************************************
******************************************************************************/

static int lro_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[64];
	
	char cmd[64];
	uint8_t param1[16], param2[16], param3[16], param4[16], param5[16] ;
	uint dbgLevel=0, ringidx, value1, value2, value3, value4;
	int i=0;

	if (count > sizeof(val_string) - 1){
		printk("command too long\n");
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	sscanf(val_string, "%s %s %s %s %s %s", cmd, param1, param2, param3, param4, param5) ;

	
	if(!strcmp(cmd, "level")) {
		sscanf(val_string, "level %d", &dbgLevel);
		if( dbgLevel<=DBG_LEVEL_MAX){
			lroDbgLevel = dbgLevel;
			printk("Debug Level: %d\n", lroDbgLevel) ;
		}
		else{
			printk("CMD Format: echo level <level> > /proc/%s  level->[0,3]\n", lro_path);
		}		
	}
	else if(!strcmp(cmd, "dbg") ){
		sscanf(val_string, "dbg %d", &dbgLevel);
		lroDbgStatus= dbgLevel;
		printk("Debug Level: %d\n", lroDbgStatus) ;
	}
	else if(!strcmp(cmd, "dump") && !strcmp(param1, "flush")){
		sscanf(param2, "%u", &ringidx);
		if((ringidx >= LRO_INDEX_OFF) && (ringidx < (LRO_INDEX_OFF + LRO_RING_NUM))){
			lro_flush_dump(ringidx);
		}
		else{
			for(i = LRO_INDEX_OFF; i < LRO_INDEX_OFF + LRO_RING_NUM; i++){
				lro_flush_dump(i);
			}
		}
	}
	else if(!strcmp(cmd, "dump") && !strcmp(param1, "status")){
		sscanf(param2, "%u", &ringidx);
		if((ringidx >= LRO_INDEX_OFF) && (ringidx < (LRO_INDEX_OFF + LRO_RING_NUM))){
			lro_status_dump(ringidx);
		}
		else{
			for(i = LRO_INDEX_OFF; i < LRO_INDEX_OFF + LRO_RING_NUM; i++){
				lro_status_dump(i);
			}
		}
	}
	else if(!strcmp(cmd, "clear") && !strcmp(param1, "flush")){
		lro_flush_clear();
	}
	else if(!strcmp(cmd, "clear") && !strcmp(param1, "status")){
		lro_status_clear();
	}
	else if(!strcmp(cmd, "ctrl") ){
		if(!strcmp(param1, "agg_num")){
			sscanf(param2, "%u", &value1);
			lro_agg_num_ctrl(value1);
		}
		else if(!strcmp(param1, "agg_size")){
			sscanf(param2, "%u", &value1);
			lro_agg_size_ctrl(value1);
		}
		else if(!strcmp(param1, "agg_time")){
			sscanf(param2, "%u", &value1);
			lro_agg_time_ctrl(value1);
		}
		else if(!strcmp(param1, "age_time")){
			sscanf(param2, "%u", &value1);
			lro_age_time_ctrl(value1);
		}
		else if(!strcmp(param1, "rule")){
			sscanf(val_string, "ctrl rule %u %u %u %u", &value1, &value2, &value3, &value4);
			lro_agg_rule_set(value1, value2, value3, value4);
		}
		else if(!strcmp(param1, "dump_rule")){
			lro_agg_rule_dump();
		}
		else if(!strcmp(param1, "enable")){
			sscanf(param2, "%u", &value1);
			sscanf(param3, "%u", &value2);
			lro_enable_set_ctrl(value1, value2);
		}
		else if(!strcmp(param1, "enable_dump")){
			for(i=0; i<LRO_RING_NUM; i++){
				printk("Lro ctrl_idx:%d, queue_idx:%d\n", i, lro_enable_get_ctrl(i));
			}
		}
		else if(!strcmp(param1, "enable_reset")){
			for(i=0; i<LRO_RING_NUM; i++){
				lro_enable_reset_ctrl(i);
			}
		}
	}
	else{
		printk("CMD Format: echo level <level> > /proc/%s  level->[0,3]\n", lro_path);
		printk("CMD Format: echo clear flush > /proc/%s\n", lro_path);
		printk("CMD Format: echo dump flush > /proc/%s\n", lro_path);
		printk("CMD Format: echo clear status > /proc/%s\n", lro_path);
		printk("CMD Format: echo dump status > /proc/%s\n", lro_path);
		printk("CMD Format: echo ctrl agg_num <value> > /proc/%s\n", lro_path);
		printk("CMD Format: echo ctrl agg_size <value> > /proc/%s\n", lro_path);
		printk("CMD Format: echo ctrl agg_time <value> > /proc/%s\n", lro_path);
		printk("CMD Format: echo ctrl age_time <value> > /proc/%s\n", lro_path);
		printk("CMD Format: echo ctrl rule <agg_num> <agg_size> <agg_time> <age_time> > /proc/%s\n", lro_path);
		printk("CMD Format: echo ctrl dump_rule > /proc/%s\n", lro_path);
		printk("CMD Format: echo ctrl enable <ctrl_idx> <queue_idx> > /proc/%s\n", lro_path);
	}
	
	return count;
}

/******************************************************************************
******************************************************************************/

static void __exit lro_module_cleanup(void) 
{   
	
	ecnt_unregister_hook(&ecnt_driver_lro_op);

	return;
}


/******************************************************************************
******************************************************************************/

static int __init lro_module_init(void)
{
	printk("lro_module_init\n");
	
	lro_init();

	if(ecnt_register_hook(&ecnt_driver_lro_op)) {
		QDMA_ERR("ecnt_dev_lro_op register fail\n");
		return -ENODEV ;
	}
        
	rx_general_dev = dev_get_by_name(&init_net, "eth0");
	if(rx_general_dev)
		dev_put(rx_general_dev);

	return 0 ;
}


module_init(lro_module_init);
module_exit(lro_module_cleanup);


