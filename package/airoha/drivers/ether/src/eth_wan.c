/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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

#include "eth_wan.h"

#ifdef LOOPBACK_SUPPORT
#include "fe_verify.h"
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
#include <linux/ip.h>
#include "linux/ipv6.h"
#include "linux/if_ether.h"
#include "linux/if_pppox.h"
#include "linux/ppp_defs.h"
#endif/*TCSUPPORT_COMPILE*/

#include "ecnt_hook/ecnt_hook_vlan.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
struct net_device *wan_dev = NULL;
macAdapter_t *mac_wan_p = NULL;
extern int wan_port_id;
extern uint8 def_mac_addr[];
extern int masko_on_off;
extern int max_pkt_len;
extern uint8 macSTagFlag;
extern macAdapter_t *mac_p;
#ifdef LOOPBACK_SUPPORT
extern uint32 macLoopback;
#endif
extern char *wan_itf;

extern int g_force_dstq;
extern portInfo lanPortState[LIMIT_PORT_NUM];

extern unsigned long long Expand_64_Tx[GSW_MAX_TX_TYPE][GSW_MAX_PORT] ;
extern unsigned long long Expand_64_Rx[GSW_MAX_RX_TYPE][GSW_MAX_PORT] ;
extern int eth_fast_mode_flag;

#ifdef TCSUPPORT_QOS
extern int qos_flag;
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
#define     PRI_REMARKING       0xE00000
#define	PRI_REMARK_ENABLE 0x100000

/*skb->mark bit26~bit31*/
#define DSCP_MASK 0xFC000000
#define DSCP_REMARK_ENABLE 0x2000000
#endif/*TCSUPPORT_COMPILE*/

#ifdef CONFIG_TP_IMAGE
  /* fix bug 856251, wan mark is conflic with 8021.p remark, cause pbit incorrect and packet is dropped by olt */
  #ifdef CONFIG_8021P_REMARK
  #undef CONFIG_8021P_REMARK
  #endif
#else
#define CONFIG_8021P_REMARK 1
#endif /* CONFIG_TP_IMAGE */

#ifdef CONFIG_8021P_REMARK
#define QOS_8021p_MARK			0x0F00 	/* 8~11 bits used for 802.1p */
#define QOS_8021P_0_MARK		0x08	/* default mark is zero */
#define VLAN_HLEN			    4
#define VLAN_ETH_ALEN			6
#endif
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
extern int g_wan_mode;
#endif
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
static int gmac_wan_start(struct net_device *dev);
static struct net_device_stats *gmac_wan_stats(struct net_device *dev);
static struct rtnl_link_stats64 * gmac_wan_stats_64(struct net_device* dev, struct rtnl_link_stats64* stats);
static int gmac_wan_open(struct net_device *dev);
static int gmac_wan_close(struct net_device *dev);
static void gmac_wan_set_multicast_list(struct net_device *dev);
static int gmac_wan_set_macaddr(struct net_device *dev, void *p);
static int gmac_wan_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
int gmac_wan_tx(struct sk_buff *skb, struct net_device *dev);
static int get_wan_port_stat(char *buf);
extern int eth1_stats_read_proc(char *buf, char **start, 
    off_t off, int count,int *eof, void *data);
extern int eth1_stats_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data);
extern void wan_to_lan(struct sk_buff *skb, uint8 local);
/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

#if defined(TCSUPPORT_NP_CMCC) || defined(TCSUPPORT_CT_UBUS)
/************************************************************************
    Funtcion:       set_ether_wan_mac_addr_from_flash
    Description:    read mac addr from flash and set to ether wan interface
    Calls:
    Called by:      eth_wan_mac_start
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
void set_ether_wan_mac_addr_from_flash(struct net_device *dev)
{
    struct sockaddr addr;

#if defined(TCSUPPORT_MESH_ETHETWAN_PORT_ONBOARDING) && defined(TCSUPPORT_CT_UBUS)
	unsigned char virtual_itf_mac[ETH_ALEN];

	memset(virtual_itf_mac, 0, sizeof(virtual_itf_mac));
	memcpy(virtual_itf_mac, def_mac_addr, sizeof(virtual_itf_mac));
	virtual_itf_mac[0] |= 0x2;
	virtual_itf_mac[1] &= 0xF0;
	virtual_itf_mac[1] |= 0x0E;
	memcpy(addr.sa_data, virtual_itf_mac, 6);
#else
    memcpy(addr.sa_data, def_mac_addr, 6);
#endif
#if defined(TCSUPPORT_NP)
	addr.sa_data[5] += 0x3;
#endif
    if(gmac_wan_set_macaddr(dev,&addr)){
        printk(KERN_INFO "The MAC address in flash is null!\n");
    }
    printk(KERN_INFO
           "%s: MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           dev->name, 
           dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2],
           dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);
}
#endif

#ifdef TCSUPPORT_WAN_ETHER
static const struct net_device_ops gmac_wan_netdev_ops = {
    .ndo_init               = gmac_wan_start,
    .ndo_open               = gmac_wan_open,
    .ndo_stop               = gmac_wan_close,
    .ndo_start_xmit         = gmac_wan_tx,
    .ndo_get_stats          = gmac_wan_stats,
    .ndo_get_stats64        = gmac_wan_stats_64,
#if !KERNEL_3_18_21
    .ndo_set_multicast_list = gmac_wan_set_multicast_list,
#endif
    .ndo_do_ioctl           = gmac_wan_ioctl,
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
    .ndo_change_mtu         = eth_change_mtu,
#endif
    .ndo_set_mac_address    = gmac_wan_set_macaddr,
};
#endif

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

#if defined(TCSUPPORT_WAN_ETHER)

#ifdef CONFIG_8021P_REMARK
/* 
 * Fill vlanTag's PCP (Priority Code Point) by skb->mark's bit8~11.
 * If packet's byte12-13 (vlanTag's TPID) is 0x8100, just change
 * the packet's vlanTag's PCP.
 * If packet's byte12-13 (vlanTag's TPID) isn't 0x8100, 
 * insert a VLAN tag with TPID=0x8100, PCP=mark, and VID=0.
 */
static inline struct sk_buff* vlanPriRemark(struct sk_buff *skb)
{
	char * vlan_p = NULL, *ether_type_ptr = NULL;
	unsigned char ucprio = 0;
	unsigned char uc802prio = 0;
	uint16 vid=0;
	int copy_len = 0;

	if ( skb->mark & QOS_8021p_MARK ) {
		ether_type_ptr = skb->data + 12;
		ucprio = (skb->mark & QOS_8021p_MARK) >> 8;
		if (ucprio < QOS_8021P_0_MARK) { //0~7 remark
			uc802prio = ucprio;
		}
		else if ( QOS_8021P_0_MARK == ucprio ) {	//zero mark
			uc802prio = 0;
		}
		else{//pass through
			/*do nothing*/
			return skb;
		}

		if(*(unsigned short *)ether_type_ptr == htons(0x8100))
		{
			vid=(ntohs(*(unsigned short *)(ether_type_ptr+2)) & 0xfff);
			skb = skb_unshare(skb, GFP_ATOMIC);
			if (!skb)
			{
				return NULL;
			}
		}
		else{
			/*Insert a vlan tag with vid =0*/
			vid=0;
			if ( skb_headroom(skb) < VLAN_HLEN ) {
				#if 0
				struct sk_buff *sk_tmp = skb;
				skb = skb_realloc_headroom(sk_tmp, VLAN_HLEN);
				
				if ( ATM_SKB(sk_tmp)->vcc->pop ) {
					ATM_SKB(sk_tmp)->vcc->pop(ATM_SKB(sk_tmp)->vcc, sk_tmp);
				}
				else {
					dev_kfree_skb_any(sk_tmp);
				}
				#endif
				struct sk_buff *skb2 = skb_realloc_headroom(skb, VLAN_HLEN);
				dev_kfree_skb(skb);
				if (skb2 == NULL) {
					printk("Vlan:failed to realloc headroom\n");
					return NULL;
				}
				skb = skb2;
			}
			else {
				skb = skb_unshare(skb, GFP_ATOMIC);
				if ( !skb ) {
			//		printk(KERN_ERR, "Vlan: failed to unshare skbuff\n");
					return NULL;
				}
			}
		
			/*offset 4 bytes*/
			skb_push(skb, VLAN_HLEN);
		
			copy_len = 2*VLAN_ETH_ALEN;
			/*move the mac address to the beginning of new header*/
			memmove(skb->data, skb->data+VLAN_HLEN, copy_len);
			skb->network_header -= VLAN_HLEN;
			skb->mac_header -= VLAN_HLEN;
		}
	
		vlan_p = skb->data + 12;
		*(unsigned short *)vlan_p = htons(0x8100);
		
		vlan_p += 2;
		*(unsigned short *)vlan_p = 0;
		/*3 bits priority and vid vlaue*/
		*(unsigned short*)vlan_p = htons(((uc802prio & 0x7) << 13)|vid) ;
		//skb->network_header -= VLAN_HLEN;
		//skb->mac_header -= VLAN_HLEN;
	}
	return skb;
}
#endif /*CONFIG_8021P_REMARK*/

static int get_wan_port_stat(char *buf){
	uint16 index = 0;
	uint32 reg;
	int port;
	int speed; 

	port = wan_port_id;
	
	reg = switch_reg_read(GSW_PMSR(port));

	if (!(reg & MAC_LINK_STS)) {
		index += sprintf(buf+index, "Down\n");
		return index;
	}

	speed = (reg & MAC_SPD_STS) >> MAC_SPD_STS_SHIFT;
	if (speed == PN_SPEED_1000M)
		index += sprintf(buf+index, "1000M/");
	else if (speed == PN_SPEED_100M)
		index += sprintf(buf+index, "100M/");
	else 
		index += sprintf(buf+index, "10M/");

	if (reg & MAC_DPX_STS)
		index += sprintf(buf+index, "Full Duplex");
	else 
		index += sprintf(buf+index, "Half Duplex");

	if (reg & (TX_FC_STS | RX_FC_STS)) {
		index += sprintf(buf+index, " FC:");
		if (reg & TX_FC_STS) 
			index += sprintf(buf+index, " TX");
		if (reg & RX_FC_STS) 
			index += sprintf(buf+index, " RX");
	}

	if (reg & EEE100_STS) 
		index += sprintf(buf+index, " EEE100");
	if (reg & EEE1G_STS) 
		index += sprintf(buf+index, " EEE1G");

	index += sprintf(buf+index, "\n");
	
	return index;
}

static int eth_wan_link_st_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int len = get_wan_port_stat(buf);
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


static int gmac_wan_start(struct net_device *dev)
{
#if defined(TCSUPPORT_NP_CMCC) || defined(TCSUPPORT_CT_UBUS)
	set_ether_wan_mac_addr_from_flash(dev);
#endif

#if KERNEL_2_6_36
	mac_wan_p = netdev_priv(dev);
	mac_wan_p->dev = dev;
#else
  	/* Hook up with handlers */
  	dev->get_stats		= gmac_wan_stats;
	dev->get_stats64		= gmac_wan_stats_64;
  	dev->hard_start_xmit	= gmac_wan_tx;
  	dev->open		= gmac_wan_open;
  	dev->stop		= gmac_wan_close;
  	dev->set_multicast_list = gmac_wan_set_multicast_list;
  	dev->do_ioctl		= gmac_wan_ioctl;
  	dev->set_mac_address 	= gmac_wan_set_macaddr;

#if VLAN_TAG_USED
	dev->features |= NETIF_F_HW_VLAN_TX;
#endif

#ifdef RAETH_CHECKSUM_OFFLOAD
	dev->features |= NETIF_F_IP_CSUM;
#endif

	mac_wan_p->statisticOn = MAC_STATISTIC_ON;
#endif//endof kernel_2_6_36
	return 0;
}

int gmac_wan_tx(struct sk_buff *skb, struct net_device *dev)
{
    ethTxMsg_t ethTxMsg;
    int error,ringIdx;
	int length;
	uint32 txq = 0;
#if defined(TCSUPPORT_RA_HWNAT)	
	struct port_info eth_info;	
    memset(&eth_info, 0, sizeof(struct port_info));

	int i, channel=5;
	unsigned long int port_mask;
#endif
#if defined(TCSUPPORT_CPU_EN7580)
	int tsid = 0x7f;
	int acnt1 = 0x1f;
	int acnt0 = 0x1f;
#endif
	if((wan_port_id<0) || (wan_port_id>=LIMIT_PORT_NUM)){
		dev_kfree_skb_any(skb);
		mac_wan_p->macStat.MIB_II.outDiscards++;
		return NETDEV_TX_OK;
	}

	setComeFromWan(skb,1);
	
#ifdef CONFIG_8021P_REMARK
	skb=vlanPriRemark(skb);
	if(skb==NULL){
		printk("802.1p remark failure\r\n");
		return NETDEV_TX_OK;
	}
#endif

#ifdef WAN2LAN
    wan_to_lan(skb,0);
#endif

#if defined(TCSUPPORT_CF_ENV) && defined(TCSUPPORT_CPU_EN7521) && defined(TCSUPPORT_WAN_ETHER)
	FH_EWAN_VLAN_HANDLE(skb, FHEWAN_TX_HOOK);
#endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
	skb = FH_VLAN_OPERATION(skb);
	if (NULL == skb)
	{
		printk("skb_vlan_operation failure\n");
		return NETDEV_TX_OK;
	}
	
	skb = FH_DSCP_OPERATION(skb);
	if (NULL == skb)
	{
		printk("skb_dscp_operation failure\n");
		return NETDEV_TX_OK;
	}
#endif/*TCSUPPORT_COMPILE*/

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CF_ENV) && defined(TCSUPPORT_WAN_ETHER)
    /* padding for the packet that length less than 64 byte */
    if(unlikely(skb->len < (ETH_ZLEN + 4)))
    {
        if(skb_padto(skb, (ETH_ZLEN + 4))) 
        {
            mac_wan_p->macStat.MIB_II.outDiscards++;
            return NETDEV_TX_OK;
        }
        skb_put(skb, (ETH_ZLEN + 4 - skb->len)) ;
    }
#endif/*TCSUPPORT_COMPILE*/

    memset(&ethTxMsg, 0, sizeof(ethTxMsg_t));
    if(macSTagFlag){
        skb = macEN7512STagInsert(&ethTxMsg, skb);
        if(skb == NULL) {
            mac_wan_p->macStat.MIB_II.outDiscards++;
            return NETDEV_TX_OK;
        }
    }

	/***************** Count the MIB-II *****************/
	update_sw_mib(mac_wan_p, skb, TX_MIB);
	
    /**************** prepare Tx Msg *****************/ 
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
	if(g_wan_mode == USE_QDMA_WAN_ETHER) {
		/* GDMA2 */
		ethTxMsg.raw.fPort = DPORT_GDMA2;
	}else
#endif
	{	
		/* GDMA1 */
		ethTxMsg.raw.fPort = DPORT_GDMA1;
	}
    /**************** prepare port_info *****************/ 
#ifdef TCSUPPORT_RA_HWNAT
	if (ra_sw_nat_hook_tx != NULL) {
		if(SUPPORT_TSID_AS_METER) {
			/* in EN7580, tsid=0x7F means no sharping, tsid=0~126 measn sharping */
			eth_info.tsid = 0x7F;
		}
		eth_info.txq = (txq & 0xf);
		eth_info.eth_is_wan = 1;
		if(!(SPTAG_INSERT_SUPPORT)) {
			eth_info.eth_macSTagEn = macSTagFlag&0x1;
		}
        eth_info.magic = FOE_MAGIC_GE;
        
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)	
		eth_info.txq_is_valid = 1;
#endif
	}
#endif

#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
	if((g_wan_mode == USE_QDMA_WAN_ETHER)){
		if(SUPPORT_GDM_SPTAG_ACTION)
			eth_info.channel = 0;
		else
			eth_info.channel = wan_port_id;
	}else
#endif
	{
		eth_info.channel = wan_port_id;
	}

    if(g_force_dstq == 1)       /*To Gdma*/
        eth_info.ds_to_qdma =0;
    else if(g_force_dstq == 2)  /*To Qdma*/
        eth_info.ds_to_qdma =1;

#ifdef LOOPBACK_SUPPORT
	if (macLoopback & LOOPBACK_PKT) {
		printk("Wan TX: ");
		dump_skb(skb);
	}
#endif
    ringIdx = 0;

    ethTxMsg.raw.channel = eth_info.channel;
#if defined(TCSUPPORT_CPU_EN7580)
	/* in EN7580, tsid=0x7F means no sharping, tsid=0~126 measn sharping */
	FE_API_GET_METER_IDX(skb, UP_STREAM, &tsid, 0);
	eth_info.tsid = tsid;
	ethTxMsg.raw.mtr_g = tsid;
	
	FE_API_GET_ACNT1_IDX(skb, UP_STREAM, &acnt1);
	FE_API_GET_ACNT0_IDX(skb, UP_STREAM, &acnt0);
	ethTxMsg.raw.acnt_g1 = acnt1;
	ethTxMsg.raw.acnt_g0 = acnt0;
	eth_info.stag = ethTxMsg.raw.sp_tag;
	eth_info.nbq = eth_info.channel;
#endif

	if (eth_fast_mode_flag) {
		eth_info.fast = 1;
	} else {
		eth_info.fast = 0;
	}

#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
	if(g_wan_mode == USE_QDMA_WAN_ETHER) {
		error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_WAN, skb, 
			ethTxMsg.msg[0], ethTxMsg.msg[1], &eth_info);
	}else
#endif
	{
		error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_LAN, skb, 
			ethTxMsg.msg[0], ethTxMsg.msg[1], &eth_info);
	}

    if(error){
		dev_kfree_skb_any(skb);
		mac_wan_p->macStat.MIB_II.outDiscards++;
		return NETDEV_TX_OK;
    }
    mac_wan_p->macStat.inSilicon.txEnQueueNum++;
	return NETDEV_TX_OK;
}




static struct net_device_stats *gmac_wan_stats(struct net_device *dev)
{
	struct net_device_stats *stats;

	stats = &mac_wan_p->stats;
	stats->rx_packets = read_reg_word(GSW_RX_UNIC(wan_port_id)) + 
	    read_reg_word(GSW_RX_MULC(wan_port_id));
	stats->tx_packets = read_reg_word(GSW_TX_UNIC(wan_port_id)) + 
	    read_reg_word(GSW_TX_MULC(wan_port_id));
	stats->rx_bytes = read_reg_word(GSW_RX_OCL(wan_port_id));
	stats->tx_bytes = read_reg_word(GSW_TX_OCL(wan_port_id));
	stats->rx_dropped = read_reg_word(GSW_RX_DROC(wan_port_id));
	stats->tx_dropped = read_reg_word(GSW_TX_DROC(wan_port_id));
	stats->multicast = read_reg_word(GSW_RX_MULC(wan_port_id));
	stats->rx_errors = read_reg_word(GSW_RX_ALIGE(wan_port_id)) + 
	    read_reg_word(GSW_RX_CRC(wan_port_id)) + 
	    read_reg_word(GSW_RX_RUNT(wan_port_id)) + 
	    read_reg_word(GSW_RX_FRGE(wan_port_id)) + 
	    read_reg_word(GSW_RX_LONG(wan_port_id));
	stats->tx_errors = read_reg_word(GSW_TX_COLC(wan_port_id));
	stats->collisions = read_reg_word(GSW_TX_COLC(wan_port_id));

	return stats;
}

struct rtnl_link_stats64* gmac_wan_stats_64(struct net_device* dev, struct rtnl_link_stats64* stats)
{
	int port_id = wan_port_id;

	stats->rx_packets = (*(*(Expand_64_Rx + 2)+port_id)<<32) +switch_reg_read(GSW_RX_UNIC(port_id)) + (*(*(Expand_64_Rx + 3)+port_id)<<32) +switch_reg_read(GSW_RX_MULC(port_id));
	stats->tx_packets = (*(*(Expand_64_Tx + 2)+port_id)<<32 )+ switch_reg_read(GSW_TX_UNIC(port_id)) + (*(*(Expand_64_Rx + 3) + 6)<<32) + switch_reg_read(GSW_RX_MULC(6));
	stats->rx_bytes = (*(*(Expand_64_Rx + 12)+port_id)<<32) + switch_reg_read(GSW_RX_OCL(port_id));
	stats->tx_bytes = (*(*(Expand_64_Tx + 12) +port_id)<<32) + switch_reg_read(GSW_TX_OCL(port_id));
	stats->rx_dropped = (*(*Expand_64_Rx + port_id)<< 32) + switch_reg_read(GSW_RX_DROC(port_id));
	stats->tx_dropped = (*(*Expand_64_Tx + port_id)<<32) + switch_reg_read(GSW_TX_DROC(port_id));
	stats->multicast = (*(*(Expand_64_Rx + 3)+port_id)<<32 )+ switch_reg_read(GSW_RX_MULC(port_id));
	stats->rx_errors = (*(*(Expand_64_Rx + 5) +port_id)<<32) + switch_reg_read(GSW_RX_ALIGE(port_id)) + (*(*(Expand_64_Rx + 6)+port_id)<<32) + switch_reg_read(GSW_RX_CRC(port_id)) + 
						(*(*(Expand_64_Rx + 7)+port_id)<<32) + switch_reg_read(GSW_RX_RUNT(port_id)) + (*(*(Expand_64_Rx + 8)+port_id)<<32 )+ switch_reg_read(GSW_RX_FRGE(port_id)) + 
						(*(*(Expand_64_Rx + 9)+port_id)<<32) + switch_reg_read(GSW_RX_LONG(port_id));
	stats->tx_errors = (*(*(Expand_64_Tx + 5) +port_id)<<32) + switch_reg_read(GSW_TX_COLC(port_id));
	stats->collisions = (*(*(Expand_64_Tx+ 5)+ port_id)<<32 )+ switch_reg_read(GSW_TX_COLC(port_id));
	return stats;
}

/* Starting up the ethernet device */
static int gmac_wan_open(struct net_device *dev)
{
// 	printk("tc3262_gmac_wan_open\n");
	#if KERNEL_2_6_36
	napi_enable(&mac_wan_p->napi);
	#endif
	netif_start_queue(dev);
  	return 0; 
}
static int gmac_wan_close(struct net_device *dev)
{
// 	printk(KERN_INFO "tc3262_gmac_wan_close\n");
	#if KERNEL_2_6_36
	napi_disable(&mac_wan_p->napi);
	#endif

  	netif_stop_queue(dev);

  	return 0;
}
/* Setup multicast list */
static void gmac_wan_set_multicast_list(struct net_device *dev)
{
	return; /* Do nothing */
}

/* Setting customized mac address */
static int gmac_wan_set_macaddr(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;

	/* Check if given address is valid ethernet MAC address */
  	if (!is_valid_ether_addr(addr->sa_data))
    		return(-EIO);

	/* Save the customize mac address */
  	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	return 0; /* Do nothing */
}
/* Handling ioctl call */
static int gmac_wan_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	return generic_mii_ioctl(&mac_p->mii_if, if_mii(ifr), cmd, NULL);
}


int eth_wan_dev_register(void)
{
	struct net_device *dev;

#if KERNEL_3_18_21
	dev = alloc_netdev(sizeof(macAdapter_t), wan_itf, NET_NAME_USER, ether_setup);
#else
    dev = alloc_netdev(sizeof(macAdapter_t), wan_itf, ether_setup);
#endif

    if (!dev)
    	return -ENOMEM;

	wan_dev = dev;
        
	mac_wan_p = netdev_priv(dev);
#if KERNEL_2_6_36
	/* Hook up with handlers */
	dev->netdev_ops = &gmac_wan_netdev_ops;

	mac_wan_p->napi.weight = MAC_NAPI_WEIGHT;
        
	netif_napi_add(dev, &mac_wan_p->napi, NULL/*dev->poll function*/, 
	    MAC_NAPI_WEIGHT);
#else
	dev->init = tc3262_gmac_wan_start;
#endif	
	return register_netdev(dev);
}
        
int eth_wan_init(void){
    struct proc_dir_entry *eth_proc;
	int err;
	
    err = eth_wan_dev_register();
	if (err){
		printk("register netdev fail\n");
		return err;
	}	
	create_proc_read_entry("tc3162/eth1_link_st", 0, NULL, eth_wan_link_st_proc, NULL);
	eth_proc = create_proc_entry("tc3162/eth1_stats", 0, NULL);
	eth_proc->read_proc = eth1_stats_read_proc;
	eth_proc->write_proc = eth1_stats_write_proc;

	return 0;
}

int eth_wan_exit(void)
{
	remove_proc_entry("tc3162/eth1_stats", 0);
	remove_proc_entry("tc3162/eth1_link_st", 0);
	unregister_netdev(wan_dev);
	free_netdev(wan_dev);
	
	return 0;
}
#endif


