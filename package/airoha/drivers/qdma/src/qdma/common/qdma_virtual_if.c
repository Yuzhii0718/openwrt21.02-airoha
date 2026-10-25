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
#include <linux/version.h>
#include <linux/etherdevice.h>
#include <linux/proc_fs.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <ecnt_hook/ecnt_hook_ether.h>

#include "qdma_bmgr.h"
#include "qdma_virtual_if.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define DPORT_GDMA1     1
#define RxMsgW1         (curRxMsg->rxMsgW1.raw)
#define BROADCAST_OFFSET 0x10
#define MULTICAST_OFFSET 0x20
#define UNKNOWN_UNICAST_OFFSET 0x30

#define BROCAST 4
#define UNICAST 2
#define UNKNOWNMUL 7
/************************************************************************
*                  M A C R O S
*************************************************************************
*/


/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/
struct qdma_softnet_data{
    struct sk_buff_head     process_queue;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	struct call_single_data  csd;
#else
    call_single_data_t csd;
#endif
    unsigned int          processed;
    unsigned int        dropped;
    unsigned int          max_queue_len;
    unsigned int        input_num;
    struct sk_buff_head     input_pkt_queue;
    struct napi_struct      napi;
};

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern int (*wan_speed_test_tso_hook)(struct sk_buff*);
extern struct net_bridge_fdb_entry *__br_fdb_get(struct net_bridge *br,const unsigned char *addr,__u16 vid);

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
static int qdma_netdev_start(struct net_device *dev);
static int qdma_netdev_open(struct net_device *dev);
static int qdma_netdev_close(struct net_device *dev);
static int qdma_netdev_set_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
/*for QDMA INT2~INT4*/
struct net_device *qdma_virtual_dev[MAX_QDMA_ITF_NUM] = {NULL};
const char* virtualDevName[MAX_QDMA_ITF_NUM] = {INT2_DEV_NAME, INT3_DEV_NAME, INT4_DEV_NAME};

uint8 int2_loopback_en = 0;
uint8 int3_loopback_en = 0;
uint8 int4_loopback_en = 0;


#if defined(QDMA_LAN)
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
extern void smp_call_function_single_ext(int cpu, struct call_single_data *csd);
#else
extern void smp_call_function_single_ext(int cpu, call_single_data_t *csd);
#endif
__IMEM int enqueue_to_cpu_queue(struct sk_buff *skb, int cpu, int queue);
int int4_rps_cpu = -1;
struct qdma_softnet_data pkt_queue[4];

int force_napi_weight = 0;
#endif
extern int (*ookla_offload_rcv_hook)(struct sk_buff *skb);
/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

static DEFINE_SPINLOCK(INT2_NAPI_LOCK);
static DEFINE_SPINLOCK(INT3_NAPI_LOCK);
static DEFINE_SPINLOCK(INT4_NAPI_LOCK);

static const struct net_device_ops int2_netdev_ops = {
	.ndo_init		   = qdma_netdev_start,
	.ndo_open		   = qdma_netdev_open,
	.ndo_stop		   = qdma_netdev_close,
	.ndo_do_ioctl	   = qdma_netdev_set_ioctl,
	.ndo_start_xmit	 = NULL,
	.ndo_change_mtu	 = NULL,
	.ndo_validate_addr  = NULL,
};

static const struct net_device_ops int3_netdev_ops = {
	.ndo_init		   = qdma_netdev_start,
	.ndo_open		   = qdma_netdev_open,
	.ndo_stop		   = qdma_netdev_close,
	.ndo_do_ioctl	   = qdma_netdev_set_ioctl,
	.ndo_start_xmit	 = NULL,
	.ndo_change_mtu	 = NULL,
	.ndo_validate_addr  = NULL,
};

static const struct net_device_ops int4_netdev_ops = {
	.ndo_init		   = qdma_netdev_start,
	.ndo_open		   = qdma_netdev_open,
	.ndo_stop		   = qdma_netdev_close,
	.ndo_do_ioctl	   = qdma_netdev_set_ioctl,
	.ndo_start_xmit	 = NULL,
	.ndo_change_mtu	 = NULL,
	.ndo_validate_addr  = NULL,
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
static void __dump_skb(struct sk_buff *skb, uint pktLen) 
{
	unchar n, *p = (unchar *)skb->data ;
	int i ;

	for(i=0 ; i<pktLen ; i++) {
		n = i & 0x0f ;
		
		if(n == 0x00) 		printk(" \n%.4x: ", i) ;
		else if(n == 0x08) 	printk(" ") ;

		printk("%.2x ", *p++) ; 
	}
	printk("\n") ;
}

int WLANOFFLOAD_FAST_HOOK(struct sk_buff *skb)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
	struct net_bridge_fdb_entry *dst = NULL;
	struct net_device *ndev = NULL;
	struct net_bridge *br = NULL;
	struct net_device *dev = NULL;

	const unsigned char *dest = skb->data;
	ndev = dev_get_by_name(&init_net, "br0");
	if (ndev)
		br = netdev_priv(ndev);
	else
		return 0;
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
	if ((dst = __br_fdb_get(br, dest, 0)) != NULL)
#else
	if ((dst = __br_fdb_get(br, dest)) != NULL)
#endif
	{
		if(IS_FDB_LOCAL(dst)){
			dev_put(ndev);
			return 0;
		}
		
		dev = dst->dst->dev;
		if(dev)
		{
			if(isWiFiInterface(dev)){
				skb->dev = dst->dst->dev;
				skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
				dev_put(ndev);

				return 1;
			}
		}
	}

	dev_put(ndev);
#endif
	return 0;
}


static inline int Qdma_WANLAN_Rx_Func(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len, QDMA_RxInfo_T *rx_info)
{
	int retValue = 0;
	int (*bmRecvCallbackFun)(void *, uint, struct sk_buff *, uint) = NULL ;
	rxMsgWord_t *curRxMsg = (rxMsgWord_t*) msg_p ;
#if defined(TCSUPPORT_CPU_EN7580) 
	if((curRxMsg->rxMsgW1.raw.sport >= 0x10 && curRxMsg->rxMsgW1.raw.sport <= 0x15)|| 3 == curRxMsg->rxMsgW1.raw.sport) /* EN7580:Ether lan pkt && XFI pkt*/
#else
	if(curRxMsg->rxMsgW1.raw.sport >= 0x8 && curRxMsg->rxMsgW1.raw.sport <= 0xc) /*Other IC only Support Ether lan pkt yet*/	
#endif
	{
		retValue = ETHER_API_RECV_PKT(msg_p, msg_len, skb, rx_len);
		switch(rx_info->ring_idx)
			{
				case BROCAST:				
					gpQdmaPriv->counters.rxBroadcastCounts[rx_info->ring_idx]++;
					if(retValue < 0)
					{
						gpQdmaPriv->counters.rxPktErrs[rx_info->ring_idx]++;				
					}else{
						gpQdmaPriv->counters.rxCounts[rx_info->ring_idx]++;
					}
					break;
				case UNICAST:
					gpQdmaPriv->counters.rxUnknownUnicastCounts[rx_info->ring_idx]++;
					if(retValue < 0)
					{
						gpQdmaPriv->counters.rxPktErrs[rx_info->ring_idx]++;				
					}else{
						gpQdmaPriv->counters.rxCounts[rx_info->ring_idx]++;
					}
					break;
				case UNKNOWNMUL:
					gpQdmaPriv->counters.rxMulticastCounts[rx_info->ring_idx]++;
					if(retValue < 0)
					{
						gpQdmaPriv->counters.rxPktErrs[rx_info->ring_idx]++;				
					}else{
						gpQdmaPriv->counters.rxCounts[rx_info->ring_idx]++;
					}
					break;
			}
	}
	else if (2 == curRxMsg->rxMsgW1.raw.sport)/*XPON pkt*/
	{
		bmRecvCallbackFun = gpQdmaPriv->devCfg.bmRecvCallbackFunction;
		if(bmRecvCallbackFun)
		{
			retValue = bmRecvCallbackFun(msg_p, msg_len, skb, rx_len);
			switch(rx_info->ring_idx)
			{
				case BROCAST:
					gpQdmaPriv->counters.rxBroadcastCounts[rx_info->ring_idx]++;
					if(retValue < 0)
					{
						gpQdmaPriv->counters.rxPktErrs[rx_info->ring_idx]++;				
					}else{
						gpQdmaPriv->counters.rxCounts[rx_info->ring_idx]++;
					}
					break;
				case UNICAST:
					gpQdmaPriv->counters.rxUnknownUnicastCounts[rx_info->ring_idx]++;
					if(retValue < 0)
					{
						gpQdmaPriv->counters.rxPktErrs[rx_info->ring_idx]++;				
					}else{
						gpQdmaPriv->counters.rxCounts[rx_info->ring_idx]++;
					}
					break;
				case UNKNOWNMUL:
					gpQdmaPriv->counters.rxMulticastCounts[rx_info->ring_idx]++;
					if(retValue < 0)
					{
						gpQdmaPriv->counters.rxPktErrs[rx_info->ring_idx]++;				
					}else{
						gpQdmaPriv->counters.rxCounts[rx_info->ring_idx]++;
					}
					break;

			}
		}
		else{
			QDMA_ERR("\nRX Error: rx callback function is NULL, kfree current skb\n");
			gpQdmaPriv->counters.noRxCbErrs[rx_info->ring_idx]++ ;
			dev_kfree_skb_any(skb);
		}
	}
	else{
		dev_kfree_skb_any(skb);
	}
	return retValue;	
}
/*For future develop*/
__IMEM static int qdma_int2_rx(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len, QDMA_RxInfo_T *rx_info)
{
	int error = 0;
	txMsgWord0_t txMsgW0;
	txMsgWord1_t txMsgW1;
	rxMsgWord_t *curRxMsg = NULL;
    txMsgWord_t txMsg;

    memset(&txMsg,0,sizeof(txMsgWord_t));

	if(unlikely(skb == NULL)){
		QDMA_ERR("Error Skb NULL, fail to receive!\n");
		return 1;
	}
	skb_put(skb, rx_len);
	curRxMsg = (rxMsgWord_t*) msg_p;

	/*------------------------print debug log------------------------------*/
	QDMA_MSG(DBG_MSG, "\nEnter INT-2 receive, ring_idx=%d!\n", rx_info->ring_idx);
#ifdef CONFIG_DEBUG
	if(gpQdmaPriv->devCfg.dbgLevel >= DBG_MSG)
		__dump_skb(skb,rx_len);
#endif
	QDMA_MSG(DBG_MSG, "msg0=%.8X, msg1=%.8X, msg2=%.8X, msg3=%.8X\n",
		curRxMsg->rxMsgW0.word,curRxMsg->rxMsgW1.word,curRxMsg->rxMsgW2.word,curRxMsg->rxMsgW3.word);

    /*------------------------loopback test------------------------------*/
	if(int2_loopback_en){
		txMsgW0.word=0;
		txMsgW1.word=0;
		txMsgW0.channel = 0;
		txMsgW0.queue = 0;
		txMsgW1.fport = DPORT_GDMA1 ;
#ifdef TCSUPPORT_CPU_EN7580
		txMsgW1.no_drop = 0;
		txMsgW1.mtr_g = 0x7f;
		txMsgW1.acnt_g1 = 0x1f;
		txMsgW1.acnt_g0 = 0x1f;
		txMsgW0.gem_port_id = 0x4;	/*sp_tag, default to port-2*/
#endif
        txMsg.pTxMsgW0 = &txMsgW0;
        txMsg.pTxMsgW1 = &txMsgW1;
		error = qdma_bm_transmit_packet(skb, 0, &txMsg);
		if(unlikely(error)){
			goto rx_err;
		}
		goto rx_done;
	} 
	/*------------------------loopback test end------------------------------*/
    
	/**************** check packet len *****************/
	if(unlikely((rx_len < 52) || (rx_len > 2000)))
		goto rx_err;
    
	/**************** check cpu reason *****************/
	if((0x18 == RxMsgW1.crsn) || (0x16 == RxMsgW1.crsn)){
		/*MultiCast Offload to WIFI or WIFI Offload*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
		skb->dev = dev_get_by_name(&init_net, lan_itf);
#else
		skb->dev = dev_get_by_name(lan_itf);
#endif
		skb->protocol = eth_type_trans(skb, skb->dev);
		skb->dev->last_rx = jiffies;   
	}else{
		/*if not, can not handle the packet*/
		if(printk_ratelimit()){
			QDMA_ERR("msg0=%.8X, msg1=%.8X, msg2=%.8X, msg3=%.8X\n",
				curRxMsg->rxMsgW0.word,curRxMsg->rxMsgW1.word,curRxMsg->rxMsgW2.word,curRxMsg->rxMsgW3.word);
			QDMA_ERR("INT-2: error cpu reason, crsn=%X!\n", RxMsgW1.crsn);
		}
		goto rx_err;
	}
	
	/**************** packet receive *****************/
	if (ra_sw_nat_hook_rxinfo){
#if defined(QDMA_LAN)
		ra_sw_nat_hook_rxinfo(skb, FOE_MAGIC_GE, (char*)&curRxMsg->rxMsgW1, 
			sizeof(rxMsgWord1_t));
#else
		ra_sw_nat_hook_rxinfo(skb, FOE_MAGIC_GPON, (char*)&curRxMsg->rxMsgW1, 
			sizeof(rxMsgWord1_t)); /*wan set GPON for tmp*/
#endif
	}
    
	if (ra_sw_nat_hook_rx){
		if (ra_sw_nat_hook_rx(skb) == 0) {
			goto rx_done;
		}
		QDMA_ERR("INT-2: wrong hwnat entry!\n");
	}

rx_err:
	dev_kfree_skb_any(skb);
	return 1;

rx_done:
	return 0;
}

/*For WIFI-2.4G: default Ring-5*/
__IMEM static int qdma_int3_rx(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len, QDMA_RxInfo_T *rx_info)
{
	int error = 0 ;
	txMsgWord0_t txMsgW0;
	txMsgWord1_t txMsgW1;
	rxMsgWord_t *curRxMsg = NULL;
    txMsgWord_t txMsg;

    memset(&txMsg,0,sizeof(txMsgWord_t));

	if(unlikely(skb == NULL)){
		QDMA_ERR("Error Skb NULL, fail to receive!\n");
		return 1;
	}
	curRxMsg = (rxMsgWord_t*) msg_p;
#if !defined (QDMA_LAN)
	if(4 == rx_info->ring_idx || 2 == rx_info->ring_idx ||7 == rx_info->ring_idx)
	{
		Qdma_WANLAN_Rx_Func(msg_p, msg_len, skb, rx_len, rx_info);
		goto rx_done;
	}
#endif
	skb_put(skb, rx_len);
        
	if(ookla_offload_rcv_hook && 
		curRxMsg->rxMsgW0.raw.sp_tag == 0xAAAA){
		skb->dev = dev_get_by_name(&init_net, lan_itf);
		skb->protocol = eth_type_trans(skb, skb->dev);
		skb->ppe_magic = 0xAAAA;
		ookla_offload_rcv_hook(skb);
		goto rx_done;
	}
	/*------------------------print debug log------------------------------*/
	QDMA_MSG(DBG_MSG, "\nEnter INT-3 receive, ring_idx=%d!\n", rx_info->ring_idx);
#ifdef CONFIG_DEBUG
	if(gpQdmaPriv->devCfg.dbgLevel >= DBG_MSG)
		__dump_skb(skb,rx_len);
#endif
	QDMA_MSG(DBG_MSG, "msg0=%.8X, msg1=%.8X, msg2=%.8X, msg3=%.8X\n",
		curRxMsg->rxMsgW0.word,curRxMsg->rxMsgW1.word,curRxMsg->rxMsgW2.word,curRxMsg->rxMsgW3.word);

	/*------------------------loopback test start------------------------------*/
	if(int3_loopback_en){
		txMsgW0.word=0;
		txMsgW1.word=0;
		txMsgW0.channel = 0;
		txMsgW0.queue = 0;
		txMsgW1.fport = DPORT_GDMA1 ;
#ifdef TCSUPPORT_CPU_EN7580
		txMsgW1.no_drop = 0;
		txMsgW1.mtr_g = 0x7f;
		txMsgW1.acnt_g1 = 0x1f;
		txMsgW1.acnt_g0 = 0x1f;
		txMsgW0.gem_port_id = 0x4;	/*sp_tag, default to port-2*/
#endif
        txMsg.pTxMsgW0 = &txMsgW0;
        txMsg.pTxMsgW1 = &txMsgW1;
		error = qdma_bm_transmit_packet(skb, 0, &txMsg);
		if(unlikely(error)){
			goto rx_err;
		}
		goto rx_done;
	}
	/*------------------------loopback test end------------------------------*/
    
	/**************** check packet len *****************/
	if(unlikely((rx_len < 52) || (rx_len > 2000)))
		goto rx_err;
    
	/**************** check cpu reason *****************/
	if (ra_sw_nat_hook_rxinfo){
#if defined(QDMA_LAN)
		ra_sw_nat_hook_rxinfo(skb, FOE_MAGIC_GE, (char*)&curRxMsg->rxMsgW1, 
			sizeof(rxMsgWord1_t));
#else
		ra_sw_nat_hook_rxinfo(skb, FOE_MAGIC_GPON, (char*)&curRxMsg->rxMsgW1, 
			sizeof(rxMsgWord1_t)); /*wan set GPON for tmp*/
#endif
	}
#if defined(QDMA_LAN)
	if(0x16 == RxMsgW1.crsn){
		if (ra_sw_nat_to_wifi_fast_tx){
#ifdef TCSUPPORT_WLAN_MT76_MAC80211
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
			skb->dev = dev_get_by_name(&init_net, lan_itf);
#else
			skb->dev = dev_get_by_name(lan_itf);

#endif
			skb->protocol = eth_type_trans(skb, skb->dev);
			skb->dev->last_rx = jiffies;
#endif
			if (ra_sw_nat_to_wifi_fast_tx(skb) == 0) {
				goto rx_done;
			}
		}
        if(ra_sw_nat_hit_ifc_fast_handler)
        {
            if(0 == ra_sw_nat_hit_ifc_fast_handler(skb))
            {
                goto rx_done;
            }
        }
	}
	if(0x18 == RxMsgW1.crsn){
		if (ra_sw_nat_to_xsi_fast_tx){
			if (ra_sw_nat_to_xsi_fast_tx(skb) == 0) {
				goto rx_done;
			}
		}
	}
#endif
    if(ra_sw_nat_vxlan_fast_handler)
    {
        if(0 == ra_sw_nat_vxlan_fast_handler(skb))
            goto rx_done;
    }
	if((0x18 == RxMsgW1.crsn) || (0x16 == RxMsgW1.crsn)){
		/*MultiCast Offload to WIFI or WIFI Offload*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
		skb->dev = dev_get_by_name(&init_net, lan_itf);
#else
		skb->dev = dev_get_by_name(lan_itf);
#endif
		skb->protocol = eth_type_trans(skb, skb->dev);
		skb->dev->last_rx = jiffies;   
	}else{
#ifdef TCSUPPORT_CPU_EN7580
        /*packet from port 0 and cpu reason is 0x 1E,then direct transmit */
        if((0 == RxMsgW1.sport)&&(0x1E == RxMsgW1.crsn)){
            if(WLANOFFLOAD_FAST_HOOK(skb))
                goto rx_done;
        }
		if(wan_speed_test_tso_hook)
		{
			if(wan_speed_test_tso_hook(skb) == 0){
				return 0;
			}	
		}		
#endif
		/*if not, can not handle the packet*/
		if(printk_ratelimit()){
			QDMA_ERR("msg0=%.8X, msg1=%.8X, msg2=%.8X, msg3=%.8X\n",
				curRxMsg->rxMsgW0.word,curRxMsg->rxMsgW1.word,curRxMsg->rxMsgW2.word,curRxMsg->rxMsgW3.word);
			QDMA_ERR("INT-3: error cpu reason, crsn=%X!\n", RxMsgW1.crsn);
		}
		goto rx_err;
	}
	
	/**************** packet receive *****************/
	if (ra_sw_nat_hook_rx){
		if (ra_sw_nat_hook_rx(skb) == 0) {
			goto rx_done;
		}
		QDMA_ERR("INT-3: wrong hwnat entry!\n");
	}

rx_err:
	dev_kfree_skb_any(skb);
	return 1;

rx_done:
	return 0;
}

/*For WIFI-5G: default Ring-6*/
__IMEM static int qdma_int4_rx(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len, QDMA_RxInfo_T *rx_info)
{
	int error = 0 ;
	txMsgWord0_t txMsgW0;
	txMsgWord1_t txMsgW1;
	rxMsgWord_t *curRxMsg = NULL;
    txMsgWord_t txMsg;

    memset(&txMsg,0,sizeof(txMsgWord_t));

	if(unlikely(skb == NULL)){
		QDMA_ERR("Error Skb NULL, fail to receive!\n");
		return 1;
	}
	skb_put(skb, rx_len);
	curRxMsg = (rxMsgWord_t*) msg_p;

	/*------------------------print debug log------------------------------*/
	QDMA_MSG(DBG_MSG, "\nEnter INT-4 receive, ring_idx=%d!\n", rx_info->ring_idx);
#ifdef CONFIG_DEBUG
	if(gpQdmaPriv->devCfg.dbgLevel >= DBG_MSG)
		__dump_skb(skb,rx_len);
#endif
	QDMA_MSG(DBG_MSG, "msg0=%.8X, msg1=%.8X, msg2=%.8X, msg3=%.8X\n",
		curRxMsg->rxMsgW0.word,curRxMsg->rxMsgW1.word,curRxMsg->rxMsgW2.word,curRxMsg->rxMsgW3.word);

	/*------------------------loopback test start------------------------------*/
	if(int4_loopback_en){
		txMsgW0.word=0;
		txMsgW1.word=0;
		txMsgW0.channel = 0;
		txMsgW0.queue = 0;
		txMsgW1.fport = DPORT_GDMA1 ;
#ifdef TCSUPPORT_CPU_EN7580
		txMsgW1.no_drop = 0;
		txMsgW1.mtr_g = 0x7f;
		txMsgW1.acnt_g1 = 0x1f;
		txMsgW1.acnt_g0 = 0x1f;
		txMsgW0.gem_port_id = 0x4;	/*sp_tag, default to port-2*/
#endif
        txMsg.pTxMsgW0 = &txMsgW0;
        txMsg.pTxMsgW1 = &txMsgW1;
		error = qdma_bm_transmit_packet(skb, 0, &txMsg);
		if(unlikely(error)){
			goto rx_err;
		}
		goto rx_done;
	}
	/*------------------------loopback test end------------------------------*/
    
	/**************** check packet len *****************/
	if(unlikely((rx_len < 52) || (rx_len > 2000)))
		goto rx_err;
	/**************** check cpu reason *****************/
	if (ra_sw_nat_hook_rxinfo){
#if defined(QDMA_LAN)
		ra_sw_nat_hook_rxinfo(skb, FOE_MAGIC_GE, (char*)&curRxMsg->rxMsgW1, 
			sizeof(rxMsgWord1_t));
#else
		ra_sw_nat_hook_rxinfo(skb, FOE_MAGIC_GPON, (char*)&curRxMsg->rxMsgW1, 
			sizeof(rxMsgWord1_t)); /*wan set GPON for tmp*/
#endif
	}
#if defined(QDMA_LAN)
	if(0x16 == RxMsgW1.crsn){
		if (ra_sw_nat_to_wifi_fast_tx){
#ifdef TCSUPPORT_WLAN_MT76_MAC80211
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
			skb->dev = dev_get_by_name(&init_net, lan_itf);
#else
			skb->dev = dev_get_by_name(lan_itf);

#endif
			skb->protocol = eth_type_trans(skb, skb->dev);
			skb->dev->last_rx = jiffies;
#endif
			if (ra_sw_nat_to_wifi_fast_tx(skb) == 0) {
				goto rx_done;
			}
		}
		if(ra_sw_nat_natv6_fast_handler)
		{
			if(0 == ra_sw_nat_natv6_fast_handler(skb))
				goto rx_done;
		}
	}
	if(0x18 == RxMsgW1.crsn){
		if (ra_sw_nat_to_xsi_fast_tx){
			if (ra_sw_nat_to_xsi_fast_tx(skb) == 0) {
				goto rx_done;
			}
		}
	}
#endif

	if((0x18 == RxMsgW1.crsn) || (0x16 == RxMsgW1.crsn)){
		/*MultiCast Offload to WIFI or WIFI Offload*/
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
		skb->dev = dev_get_by_name(&init_net, lan_itf);
#else
		skb->dev = dev_get_by_name(lan_itf);
#endif
		skb->protocol = eth_type_trans(skb, skb->dev);
		skb->dev->last_rx = jiffies;   
	}else{
#ifdef TCSUPPORT_CPU_EN7580
        /*packet from port 0 and cpu reason is 0x 1E,then direct transmit */
        if((0 == RxMsgW1.sport)&&(0x1E == RxMsgW1.crsn)){
            if(WLANOFFLOAD_FAST_HOOK(skb))
                goto rx_done;
        }
		if(wan_speed_test_tso_hook)
		{
			if(wan_speed_test_tso_hook(skb) == 0){
				return 0;
			}
		}		
#endif
#if 0
		/*if not, can not handle the packet*/
		if(printk_ratelimit()){
			QDMA_MSG(DBG_ST, "msg0=%.8X, msg1=%.8X, msg2=%.8X, msg3=%.8X\n",
				curRxMsg->rxMsgW0.word,curRxMsg->rxMsgW1.word,curRxMsg->rxMsgW2.word,curRxMsg->rxMsgW3.word);
			QDMA_MSG(DBG_ST, "INT-4: error cpu reason, crsn=%X!\n", RxMsgW1.crsn);
		}
#endif
		goto rx_err;
	}
    
	/**************** packet receive *****************/

#if defined(QDMA_LAN)
    if ((int4_rps_cpu >= 0) && (int4_rps_cpu < 4))
    {
        enqueue_to_cpu_queue(skb, int4_rps_cpu, int4_rps_cpu);
        goto rx_done;
    }
#endif
    
	if (ra_sw_nat_hook_rx){
		if (ra_sw_nat_hook_rx(skb) == 0) {
			goto rx_done;
		}
		QDMA_ERR("INT-4: wrong hwnat entry!\n");
	}

rx_err:
	dev_kfree_skb_any(skb);
	return 1;

rx_done:
	return 0;
}

/******************************************************************************
******************************************************************************/
static int qdma_int2_napiPoll(struct napi_struct *napi, int budget)
{
	int n;
	unsigned long flags;
    struct ECNT_QDMA_Data qdma_data;

	/* call qdma_int2_rx to receive a packet 
	 * until all packets are received */
    qdma_data.qdma_private.cnt = budget;
	n = qdma_receive_packets_int2(&qdma_data);

	if (n < budget)
	{
		spin_lock_irqsave(&INT2_NAPI_LOCK, flags);
	
		__napi_complete(napi);
		qdma_enable_rxpkt_int2(&qdma_data);;

		spin_unlock_irqrestore(&INT2_NAPI_LOCK, flags);
	}
	
	return n;
}

static int qdma_int3_napiPoll(struct napi_struct *napi, int budget)
{
	int n;
	unsigned long flags;
    struct ECNT_QDMA_Data qdma_data;

	/* call qdma_int3_rx to receive a packet 
	 * until all packets are received */
    qdma_data.qdma_private.cnt = budget;
	n = qdma_receive_packets_int3(&qdma_data);

	if (n < budget)
	{
		spin_lock_irqsave(&INT3_NAPI_LOCK, flags);
	
		__napi_complete(napi);
		qdma_enable_rxpkt_int3(&qdma_data);;

		spin_unlock_irqrestore(&INT3_NAPI_LOCK, flags);
	}
	
	return n;
}

static int qdma_int4_napiPoll(struct napi_struct *napi, int budget)
{
	int n;
	unsigned long flags;
    struct ECNT_QDMA_Data qdma_data;
    int budget_tmp = budget;
        
#if defined(QDMA_LAN)
    if (force_napi_weight > 0)
        budget_tmp = force_napi_weight;
#endif

	/* call qdma_int4_rx to receive a packet 
	 * until all packets are received */
    qdma_data.qdma_private.cnt = budget_tmp;
	n = qdma_receive_packets_int4(&qdma_data);

	if (n < budget_tmp)
	{
		spin_lock_irqsave(&INT4_NAPI_LOCK, flags);
	
		__napi_complete(napi);
		qdma_enable_rxpkt_int4(&qdma_data);;

		spin_unlock_irqrestore(&INT4_NAPI_LOCK, flags);
	}
    
#if defined(QDMA_LAN)
    if (n > budget)
        return budget;
#endif
	return n;
}

/******************************************************************************
******************************************************************************/
__IMEM static int qdma_int2_EventHandler(QDMA_EventType_t qdmaEventType)
{   
    unsigned long flags;
    QDMA_Virtual_Adapter_T *virtualApt = netdev_priv(qdma_virtual_dev[INT2_ITF_INDEX]);
    struct ECNT_QDMA_Data qdma_data ;
    
    if ((qdmaEventType == QDMA_EVENT_RECV_PKTS) ||
        (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER))
	{
        if (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER)
            printk("\nINT2: QDMA_EVENT_NO_RX_BUFFER\n");
    
		spin_lock_irqsave(&INT2_NAPI_LOCK, flags);

		if (napi_schedule_prep(&virtualApt->napi))
		{
			qdma_disable_rxpkt_int2(&qdma_data);  /*disable INT-2 RX interrupt.*/
			__napi_schedule(&virtualApt->napi);
		}
		spin_unlock_irqrestore(&INT2_NAPI_LOCK, flags);
	}
	else
		printk("\nWrong QDMA Event Type: %d\n", qdmaEventType);

	return 0;
}

__IMEM static int qdma_int3_EventHandler(QDMA_EventType_t qdmaEventType)
{   
    unsigned long flags;
    QDMA_Virtual_Adapter_T *virtualApt = netdev_priv(qdma_virtual_dev[INT3_ITF_INDEX]);
    struct ECNT_QDMA_Data qdma_data ;
    
    if ((qdmaEventType == QDMA_EVENT_RECV_PKTS) ||
        (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER))
	{
        if (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER)
            printk("\nINT3: QDMA_EVENT_NO_RX_BUFFER\n");
    
		spin_lock_irqsave(&INT3_NAPI_LOCK, flags);

		if (napi_schedule_prep(&virtualApt->napi))
		{
			qdma_disable_rxpkt_int3(&qdma_data);  /*disable INT-3 RX interrupt.*/
			__napi_schedule(&virtualApt->napi);
		}
		spin_unlock_irqrestore(&INT3_NAPI_LOCK, flags);
	}
	else
		printk("\nWrong QDMA Event Type: %d\n", qdmaEventType);

	return 0;
}

__IMEM static int qdma_int4_EventHandler(QDMA_EventType_t qdmaEventType)
{   
    unsigned long flags;
    QDMA_Virtual_Adapter_T *virtualApt = netdev_priv(qdma_virtual_dev[INT4_ITF_INDEX]);
    struct ECNT_QDMA_Data qdma_data ;
    
    if ((qdmaEventType == QDMA_EVENT_RECV_PKTS) ||
        (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER))
	{
        if (qdmaEventType == QDMA_EVENT_NO_RX_BUFFER)
            printk("\nINT4: QDMA_EVENT_NO_RX_BUFFER\n");
    
		spin_lock_irqsave(&INT4_NAPI_LOCK, flags);

		if (napi_schedule_prep(&virtualApt->napi))
		{
			qdma_disable_rxpkt_int4(&qdma_data);  /*disable INT-4 RX interrupt.*/
			__napi_schedule(&virtualApt->napi);
		}
		spin_unlock_irqrestore(&INT4_NAPI_LOCK, flags);
	}
	else
		printk("\nWrong QDMA Event Type: %d\n", qdmaEventType);

	return 0;
}

/******************************************************************************
******************************************************************************/
static int qdma_netdev_open(struct net_device *dev)
{
	QDMA_Virtual_Adapter_T *virtualApt = netdev_priv(dev);	

	napi_enable(&virtualApt->napi);

	netif_start_queue(dev);

	return 0;
}

/******************************************************************************
******************************************************************************/
static int qdma_netdev_close(struct net_device *dev)
{
	QDMA_Virtual_Adapter_T *virtualApt = netdev_priv(dev);
	
	printk("qdma_netdev_close\n");

	printk("%s: stoping interface.\n", dev->name);

	netif_stop_queue(dev);

	napi_disable(&virtualApt->napi);

	return 0;
}

/******************************************************************************
******************************************************************************/
static int qdma_netdev_set_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	/* Not need implement */
    return 0;
}

/******************************************************************************
******************************************************************************/
static int qdma_netdev_start(struct net_device *dev)
{
    /* Not need implement */
	return 0;
}

/******************************************************************************
******************************************************************************/
static int qdma_virtual_if_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+index, "echo loopback int2/int3/int4 on/off ") ;
	index += sprintf(buf+index, " > /proc/%s\n", qdma_virtual_if_path) ;  	

	index += sprintf(buf+index, "INT-2 loopback: %s\n", int2_loopback_en?"on":"off");
	index += sprintf(buf+index, "INT-3 loopback: %s\n", int3_loopback_en?"on":"off");
    index += sprintf(buf+index, "INT-4 loopback: %s\n", int4_loopback_en?"on":"off");

	CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0) 	index = 0 ;
	if(index>count) 	index = count ;
	return index ;
}

static int qdma_virtual_if_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[128];
    char int_name[32];
    char cmd[32];
    char action[32];
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

 	sscanf(val_string, "%s %s %s", cmd, int_name, action) ;

    if(!strcmp(cmd, "loopback")){
        if(!strcmp(int_name, "int2")){
            if(!strcmp(action, "on")){
                int2_loopback_en = 1;
            }else if(!strcmp(action, "off")){
                int2_loopback_en = 0;
            }else{
                QDMA_ERR("Error action!\n");
            }
        }else if(!strcmp(int_name, "int3")){
            if(!strcmp(action, "on")){
                int3_loopback_en = 1;
            }else if(!strcmp(action, "off")){
                int3_loopback_en = 0;
            }else{
                QDMA_ERR("Error action!\n");
            }
        }else if(!strcmp(int_name, "int4")){
            if(!strcmp(action, "on")){
                int4_loopback_en = 1;
            }else if(!strcmp(action, "off")){
                int4_loopback_en = 0;
            }else{
                QDMA_ERR("Error action!\n");
            }
        }else{
             QDMA_ERR("Error INT name!\n");
        }
    }else{
         QDMA_ERR("Error CMD!\n");
    }
    
	return count ;
}

#if defined(QDMA_LAN)
__IMEM int enqueue_to_cpu_queue(struct sk_buff *skb, int cpu, int queue)
{
    struct qdma_softnet_data *sd;
    unsigned long flags;
    unsigned int queue_len;

    sd = &pkt_queue[queue];

    local_irq_save(flags);
    spin_lock(&sd->input_pkt_queue.lock);
    queue_len = skb_queue_len(&sd->input_pkt_queue);
    if(queue_len > sd->max_queue_len)
        sd->max_queue_len = queue_len;
    if (queue_len <= 10000) {
        if (queue_len){
enqueue:
            __skb_queue_tail(&sd->input_pkt_queue, skb);
	        sd->input_num++;
            spin_unlock(&sd->input_pkt_queue.lock);
            local_irq_restore(flags);
            return NET_RX_SUCCESS;
        }
        if((!__test_and_set_bit(NAPI_STATE_SCHED, &sd->napi.state)))
        {
            if (cpu != get_cpu()){
                if(cpu_online(cpu)){
                    smp_call_function_single_ext(cpu, &sd->csd);
                }
            }
            else{
                __napi_schedule(&sd->napi);
            }
            put_cpu();
        }
        goto enqueue;
    }

    sd->dropped++;
    spin_unlock(&sd->input_pkt_queue.lock);

    local_irq_restore(flags);

    kfree_skb(skb);
    return NET_RX_DROP;
}

__IMEM int qdma_int4_rps_process(struct sk_buff *skb)
{
	if (ra_sw_nat_hook_rx)
		ra_sw_nat_hook_rx(skb);

    return 0;
}

int qdma_rps_poll(struct napi_struct *napi, int quota, struct qdma_softnet_data *sd, 
    int (*qdma_rps_pkt_process)(struct sk_buff * skb))
{
    int work = 0;
	
    napi->weight = quota;
    local_irq_disable();
    while (1) {
        struct sk_buff *skb;
        while ((skb = __skb_dequeue(&sd->process_queue))) {
            local_irq_enable();
            qdma_rps_pkt_process(skb);
            local_irq_disable();
            if (++work >= quota) {
                local_irq_enable();
                return work;
            }
        }

        spin_lock(&sd->input_pkt_queue.lock);
        if (skb_queue_empty(&sd->input_pkt_queue)) {
		/*
		 * Inline a custom version of __napi_complete().
		 * only current cpu owns and manipulates this napi,
		 * and NAPI_STATE_SCHED is the only possible flag set
		 * on backlog.
		 * We can use a plain write instead of clear_bit(),
		 * and we dont need an smp_mb() memory barrier.
		 */
		list_del(&napi->poll_list);
		napi->state = 0;
		spin_unlock(&sd->input_pkt_queue.lock);
		break;
	}

	skb_queue_splice_tail_init(&sd->input_pkt_queue,&sd->process_queue);
        spin_unlock(&sd->input_pkt_queue.lock);
    }
    local_irq_enable();
    return work;
}

int qdma_int4_poll_cpu0(struct napi_struct *napi, int quota)
{
	struct qdma_softnet_data *sd = &pkt_queue[0];
	return qdma_rps_poll(napi, quota, sd, qdma_int4_rps_process);
}

int qdma_int4_poll_cpu1(struct napi_struct *napi, int quota)
{
	struct qdma_softnet_data *sd = &pkt_queue[1];
	return qdma_rps_poll(napi, quota, sd, qdma_int4_rps_process);
}

int qdma_int4_poll_cpu2(struct napi_struct *napi, int quota)
{
	struct qdma_softnet_data *sd = &pkt_queue[2];
	return qdma_rps_poll(napi, quota, sd, qdma_int4_rps_process);
}

int qdma_int4_poll_cpu3(struct napi_struct *napi, int quota)
{
	struct qdma_softnet_data *sd = &pkt_queue[3];
	return qdma_rps_poll(napi, quota, sd, qdma_int4_rps_process);
}

void qdma_rps_napi_init(void)
{
    int i;

    for(i=0;i<4;i++)
    {
        memset(&pkt_queue[i], 0, sizeof(pkt_queue[i]));
        skb_queue_head_init(&pkt_queue[i].input_pkt_queue);
        skb_queue_head_init(&pkt_queue[i].process_queue);

        pkt_queue[i].csd.func = __napi_schedule;
        pkt_queue[i].csd.info = &pkt_queue[i].napi;
        pkt_queue[i].csd.flags = 0;

        pkt_queue[i].napi.weight = 1024;
        pkt_queue[i].napi.gro_list = NULL;
        pkt_queue[i].napi.gro_count = 0;
    }

    pkt_queue[0].napi.poll = qdma_int4_poll_cpu0;
    pkt_queue[1].napi.poll = qdma_int4_poll_cpu1;
    pkt_queue[2].napi.poll = qdma_int4_poll_cpu2;
    pkt_queue[3].napi.poll = qdma_int4_poll_cpu3;
    return;
}

static int qdma_int4_rps_cpu_read_proc(char *buf, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("%d\n", int4_rps_cpu);	

	printk("----------- QDMA INT4 RPS info dump -----------\n");
    printk("               CPU0       CPU1       CPU2       CPU3\n");
    printk("enqueue: %10d %10d %10d %10d\n", 
        pkt_queue[0].input_num, pkt_queue[1].input_num, pkt_queue[2].input_num, pkt_queue[3].input_num);
    printk("cur_len: %10d %10d %10d %10d\n", 
        skb_queue_len(&pkt_queue[0].input_pkt_queue), skb_queue_len(&pkt_queue[1].input_pkt_queue), 
        skb_queue_len(&pkt_queue[2].input_pkt_queue), skb_queue_len(&pkt_queue[3].input_pkt_queue));
    printk("dropped: %10d %10d %10d %10d\n", 
        pkt_queue[0].dropped, pkt_queue[1].dropped, pkt_queue[2].dropped, pkt_queue[3].dropped);
    printk("max_len: %10d %10d %10d %10d\n", 
        pkt_queue[0].max_queue_len, pkt_queue[1].max_queue_len, pkt_queue[2].max_queue_len, pkt_queue[3].max_queue_len);
	return 0;
}

static int qdma_int4_rps_cpu_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[64];
	int len = 0;
    int i;

	if ( count > sizeof(val_string) - 1 )
		len = sizeof(val_string) - 1;
	else
		len = count;

	memset(val_string, 0, sizeof(val_string));
	if ( copy_from_user(val_string, buffer, len) )
		return -EFAULT;

	sscanf(val_string, "%d", &int4_rps_cpu);

    for (i = 0; i < 4; i++)
    {
        pkt_queue[i].input_num = 0;
        pkt_queue[i].dropped = 0;
        pkt_queue[i].max_queue_len = 0;
    }
    
	return len;
}

static int qdma_force_napi_weight_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+index, "%d\n", force_napi_weight) ;

	CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0) 	index = 0 ;
	if(index>count) 	index = count ;
	return index ;
}

static int qdma_force_napi_weight_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[128];
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

 	sscanf(val_string, "%d", &force_napi_weight) ;    
	return count ;
}
#endif

/******************************************************************************
******************************************************************************/
int qdma_virtual_interface_init(void)
{
    int itf = 0;
    int error = 0;
    struct proc_dir_entry *qdma_proc=NULL;
    struct net_device *dev = (struct net_device *)NULL;
    QDMA_Virtual_Adapter_T *virtualApt = (QDMA_Virtual_Adapter_T *)NULL;
    struct ECNT_QDMA_Data qdma_data;
    QDMA_InitCfg_t qdmaApiInitCfg;

    //******** 1. int the rx&event callback function ********//
    memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
    memset(&qdmaApiInitCfg, 0, sizeof(QDMA_InitCfg_t));
    
    qdmaApiInitCfg.cbInt2RecvPkts = (qdma_callback_recv_packet_with_info_t)qdma_int2_rx;
    qdmaApiInitCfg.cbInt2EventHandler = (qdma_callback_event_handler_t)qdma_int2_EventHandler;
    qdmaApiInitCfg.cbInt3RecvPkts = (qdma_callback_recv_packet_with_info_t)qdma_int3_rx;
    qdmaApiInitCfg.cbInt3EventHandler = (qdma_callback_event_handler_t)qdma_int3_EventHandler;
    qdmaApiInitCfg.cbInt4RecvPkts = (qdma_callback_recv_packet_with_info_t)qdma_int4_rx;
    qdmaApiInitCfg.cbInt4EventHandler = (qdma_callback_event_handler_t)qdma_int4_EventHandler;
#if defined(QDMA_LAN)/*QDMA_LAN & QDMA_WAN macType no need change*/
    qdmaApiInitCfg.macType = MAC_TYPE_ETHER;
#else
    qdmaApiInitCfg.macType = MAC_TYPE_XPON;
#endif
    qdmaApiInitCfg.txRecycleThrh = 4;

    qdma_data.qdma_private.pInitCfg = &qdmaApiInitCfg;
    qdma_init(&qdma_data);

    //******** 2. register device & NAPI operation ********//
    if(QDMA_RX_MODE == QDMA_RX_NAPI) {
        for(itf = 0; itf < MAX_QDMA_ITF_NUM; itf++)
        {
            /*1) alloc net device*/
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19)
            dev = alloc_netdev(sizeof(QDMA_Virtual_Adapter_T), virtualDevName[itf], NET_NAME_USER, ether_setup);
#else
            dev = alloc_netdev(sizeof(QDMA_Virtual_Adapter_T), virtualDevName[itf], ether_setup);
#endif
            if (!dev)
            {
                printk("\n\nFAILED: net_device allocation fail\n\n");
                return -ENOMEM;
            }
            /*2) clear net device private space*/
            qdma_virtual_dev[itf] = dev;
            virtualApt = netdev_priv(dev);
            memset(virtualApt, 0, sizeof(QDMA_Virtual_Adapter_T));
            /*3) add dev operation and NAPI to device*/
            if( itf == INT2_ITF_INDEX ){
                qdma_virtual_dev[itf]->netdev_ops = &int2_netdev_ops;
                virtualApt->napi.weight = INT2_NAPI_WEIGHT;
                netif_napi_add(dev, &virtualApt->napi, qdma_int2_napiPoll, virtualApt->napi.weight);
            }else if( itf == INT3_ITF_INDEX ){
                qdma_virtual_dev[itf]->netdev_ops = &int3_netdev_ops;
                virtualApt->napi.weight = INT3_NAPI_WEIGHT;
                netif_napi_add(dev, &virtualApt->napi, qdma_int3_napiPoll, virtualApt->napi.weight);
            }else{ /* INT4_ITF_INDEX */
                qdma_virtual_dev[itf]->netdev_ops = &int4_netdev_ops;
                virtualApt->napi.weight = INT4_NAPI_WEIGHT;
                netif_napi_add(dev, &virtualApt->napi, qdma_int4_napiPoll, virtualApt->napi.weight);
            }
            /*4) register the net device*/
            error = register_netdev(dev);
            if (error)
            {
                printk("\n\nFAILED: net_device register fail\n\n");
                free_netdev(dev);
                return error;
            }
            
            set_bit(__LINK_STATE_START, &qdma_virtual_dev[itf]->state);
            
            if (qdma_virtual_dev[itf]->netdev_ops->ndo_open){
                error = qdma_virtual_dev[itf]->netdev_ops->ndo_open(qdma_virtual_dev[itf]);
                if (error)
                    clear_bit(__LINK_STATE_START, &qdma_virtual_dev[itf]->state);
            }
        }
    }else{
        printk("\n\nFAILED: QDMA RX is not NAPI mode!\n\n");
    }
    
    //******** 3. register proc node ********//
    qdma_proc = create_proc_entry(qdma_virtual_if_path, 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_virtual_if_read_proc ;
		qdma_proc->write_proc = qdma_virtual_if_write_proc ;
	}

#if defined(QDMA_LAN)
    qdma_rps_napi_init();

    qdma_proc = create_proc_entry("qdma_lan/int4_rps_cpu", 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_int4_rps_cpu_read_proc ;
		qdma_proc->write_proc = qdma_int4_rps_cpu_write_proc ;
	}

    qdma_proc = create_proc_entry("qdma_lan/force_napi_weight", 0, NULL) ;
	if(qdma_proc) {
		qdma_proc->read_proc = qdma_force_napi_weight_read_proc ;
		qdma_proc->write_proc = qdma_force_napi_weight_write_proc ;
	}
#endif
    return 0;

}

void qdma_virtual_interface_deinit(void)
{
	int itf = 0;

	for(itf = 0; itf < MAX_QDMA_ITF_NUM; itf++)
	{
		unregister_netdev(qdma_virtual_dev[itf]);
	}

    //******** unregister proc node ********//
    remove_proc_entry(qdma_virtual_if_path, NULL) ;
#if defined(QDMA_LAN)
    remove_proc_entry("qdma_lan/int4_rps_cpu", NULL) ;
    remove_proc_entry("qdma_lan/force_napi_weight", NULL) ;
#endif
}

