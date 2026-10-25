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
THAT IT IS RECEIVER??S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER??S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/net_tstamp.h>
#include <ecnt_hook/ecnt_hook_1588.h>
#include "eth_lan.h"
#include "eth_virtual_if.h"
#include "./tcphy/phy_api.h"
#include "ethtool_support.h"

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
extern macAdapter_t *mac_p;
extern char lan_port_map[];
extern uint8 def_mac_addr[];
extern struct net_device *lan_dev;
extern uint8 use_ext_switch;
extern char *sep_itf;
extern int itf_num;
extern int vport_enable;

extern unsigned long long Expand_64_Tx[GSW_MAX_TX_TYPE][GSW_MAX_PORT] ;
extern unsigned long long Expand_64_Rx[GSW_MAX_RX_TYPE][GSW_MAX_PORT] ;

extern unsigned int g_Tx_Last[GSW_MAX_TX_TYPE][GSW_MAX_PORT] ;
extern unsigned int g_Rx_Last[GSW_MAX_RX_TYPE][GSW_MAX_PORT] ;

#ifdef TCSUPPORT_QTN_ISOLATION
extern int isolation_enable;
#endif

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
extern char lan_port_map_ext[];
extern char extend_switch_port;
#endif

extern int link_aggr_en;
extern int (*hwnat_skb_to_foe_hook)(struct sk_buff* skb);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,55)
extern int dev_change_flags(struct net_device *dev, unsigned int flags,struct netlink_ext_ack *extack);
#else
extern int dev_change_flags(struct net_device *dev, unsigned int flags);
#endif
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
#if 0
static int virtual_lan_dev_start(struct net_device *dev);
static int virtual_lan_dev_close(struct net_device *dev);
static int virtual_lan_dev_open(struct net_device *dev);
static struct net_device_stats *virtual_lan_dev_stats(struct net_device *dev);
#endif
int virtual_lan_dev_start(struct net_device *dev);
int virtual_lan_dev_close(struct net_device *dev);
int virtual_lan_dev_open(struct net_device *dev);
struct net_device_stats *virtual_lan_dev_stats(struct net_device *dev);
struct rtnl_link_stats64* virtual_lan_dev_stats_64(struct net_device* dev, struct rtnl_link_stats64* stats);


static int virtual_dev_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
static int virtual_dev_set_macaddr(struct net_device *dev, void *p);
#ifdef TCSUPPORT_ECNT_1588v2
extern int ecnt_config_hwtstamp(struct net_device *netdev, struct ifreq *ifr);
#endif

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
struct net_device *lan_virtual_dev[MAX_ETH_ITF_NUM] = {NULL};
macAdapter_t *mac_virtual_p[MAX_ETH_ITF_NUM] = {NULL};

#ifdef TCSUPPORT_AUTOBENCH
extern int eth_mac_tx_autobench(struct sk_buff *skb, struct net_device *dev);
#endif

#if KERNEL_2_6_36
static const struct net_device_ops virtual_dev_ops = {
    .ndo_init               = virtual_lan_dev_start,
    .ndo_open               = virtual_lan_dev_open,
    .ndo_stop               = virtual_lan_dev_close,
    .ndo_start_xmit         = eth_lan_mac_tx,
    .ndo_get_stats          = virtual_lan_dev_stats,
    .ndo_get_stats64        = virtual_lan_dev_stats_64,
/*    .ndo_set_multicast_list = tc3262_gmac_set_multicast_list,*/
    .ndo_do_ioctl           = virtual_dev_ioctl,
    .ndo_set_mac_address    = virtual_dev_set_macaddr,
    .ndo_validate_addr      = eth_validate_addr,
#ifdef CONFIG_NET_POLL_CONTROLLER
    .ndo_poll_controller    = tc3262_gmac_poll_controller,
#endif
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

int virtual_lan_dev_start(struct net_device *dev)
{
    int index=0;
    int i;
	unsigned char virtual_itf_mac[ETH_ALEN];
    for( ; index < itf_num ; index++)
    {
        if(dev == lan_virtual_dev[index])
            break;
    }
    if(index == itf_num)
	{
		printk(KERN_INFO "%s(): find no stored dev!\n", __FUNCTION__);
		return -ENETDOWN;
	}

#ifdef CONFIG_TP_IMAGE
	netif_carrier_off(dev);
#endif /* CONFIG_TP_IMAGE */

#if defined(TCSUPPORT_ECNT_MAP)
	memset(virtual_itf_mac, 0, sizeof(virtual_itf_mac));
	memcpy(virtual_itf_mac, def_mac_addr, sizeof(virtual_itf_mac));
	virtual_itf_mac[0] |= 0x2;
	virtual_itf_mac[1] &= 0xF0;
	virtual_itf_mac[1] |= index;
	if(virtual_itf_mac[1] == def_mac_addr[1])
	{
		if(0xf > itf_num)
		{
			virtual_itf_mac[1] &= 0xF0;
			virtual_itf_mac[1] |= itf_num;
		}
		else
		{
			printk(KERN_INFO "%s(): mac conflict!\n", __FUNCTION__);
			return -ENETDOWN;
		}
	}
	memcpy(dev->dev_addr, virtual_itf_mac, dev->addr_len);
#else
    for (i = 0; i < 6; i++) {
		dev->dev_addr[i] = def_mac_addr[i];
	}
#endif
#if KERNEL_2_6_36
		mac_virtual_p[index]->dev = dev;
#else
		dev->get_stats			= virtual_lan_dev_stats;
		dev->hard_start_xmit	= eth_lan_mac_tx;
		dev->open				= virtual_lan_dev_open;
		dev->stop				= virtual_lan_dev_close;
		dev->set_multicast_list = tc3262_gmac_set_multicast_list;
		dev->do_ioctl			= virtual_dev_ioctl;
		dev->set_mac_address	= virtual_dev_set_macaddr;
#ifdef TC3262_GMAC_NAPI
		dev->poll				= tc3262_gmac_poll;
#endif 
		dev->weight 			= MAC_NAPI_WEIGHT;	
#if VLAN_TAG_USED
		dev->features |= NETIF_F_HW_VLAN_TX;
#endif
#ifdef RAETH_CHECKSUM_OFFLOAD
		dev->features |= NETIF_F_IP_CSUM;
#endif		
#endif //KERNEL_2_6_36
		return 0;
}

int virtual_lan_dev_open(struct net_device *dev)
{
    netif_start_queue(dev);
    return 0;
}



int virtual_lan_dev_close(struct net_device *dev)
{
    netif_stop_queue(dev);
    return 0;
}

unsigned int find_port_by_name(struct net_device *dev)
{
    unsigned int eth_intf = 0;
    uint8 itf_name_length = strlen(sep_itf);
    if ( memcmp(dev->name, sep_itf, itf_name_length) == 0 ){
        switch(itf_name_length){
            case 3:
                sscanf(dev->name, "eth%d", &eth_intf);
                break;
            case 5:
                sscanf(dev->name, "eth0.%d", &eth_intf);
                break;
        }
        return eth_intf - itf_start_idx;
	}else{
	    return eth_intf;
	}
}

struct net_device *find_virtual_itf_by_index(unsigned int interface_index)
{
	struct net_device *dev ;

	if(interface_index <= SEP_ETH_ITF_3)
	{
		dev = lan_virtual_dev[interface_index];
		return dev ; 
	}
	
	return NULL ;
}

struct net_device_stats *virtual_lan_dev_stats(struct net_device *dev)
{
    unsigned int port_id=0;
    unsigned int eth_intf = 0;
    struct net_device_stats *stats;
    stats = &mac_p->stats;
    unsigned int is_extend_switch = 0;
    
    eth_intf = find_port_by_name(dev);
	
	if(eth_intf < itf_num)
	{
	    port_id  =  lan_port_map[eth_intf];
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
		/* if port_id = extend port*/
        if(extend_switch_port == port_id){
            is_extend_switch =1;
            port_id = lan_port_map_ext[eth_intf];
        }
#endif
	}else{
	    port_id = 6;
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
    if(is_extend_switch){
        stats->rx_packets = gswPbusRead_extend(GSW_RX_UNIC(port_id)-GSW_BASE, GSW_PHY_OFFSET_23) + 
		                    gswPbusRead_extend(GSW_RX_MULC(port_id)-GSW_BASE, GSW_PHY_OFFSET_23);
		stats->tx_packets = gswPbusRead_extend(GSW_TX_UNIC(port_id)-GSW_BASE, GSW_PHY_OFFSET_23) + 
		                    gswPbusRead_extend(GSW_RX_MULC(6)-GSW_BASE, GSW_PHY_OFFSET_23);
		stats->rx_bytes = gswPbusRead_extend(GSW_RX_OCL(port_id)-GSW_BASE, GSW_PHY_OFFSET_23);
		stats->tx_bytes = gswPbusRead_extend(GSW_TX_OCL(port_id)-GSW_BASE, GSW_PHY_OFFSET_23);
		stats->rx_dropped = gswPbusRead_extend(GSW_RX_DROC(port_id)-GSW_BASE, GSW_PHY_OFFSET_23);
		stats->tx_dropped = gswPbusRead_extend(GSW_TX_DROC(port_id)-GSW_BASE, GSW_PHY_OFFSET_23);
		stats->multicast = gswPbusRead_extend(GSW_RX_MULC(port_id)-GSW_BASE, GSW_PHY_OFFSET_23);
		stats->rx_errors =  gswPbusRead_extend(GSW_RX_ALIGE(port_id)-GSW_BASE, GSW_PHY_OFFSET_23) + 
		                    gswPbusRead_extend(GSW_RX_CRC(port_id)-GSW_BASE, GSW_PHY_OFFSET_23) + 
		                    gswPbusRead_extend(GSW_RX_RUNT(port_id)-GSW_BASE, GSW_PHY_OFFSET_23) + 
		                    gswPbusRead_extend(GSW_RX_FRGE(port_id)-GSW_BASE, GSW_PHY_OFFSET_23) + 
		                    gswPbusRead_extend(GSW_RX_LONG(port_id)-GSW_BASE, GSW_PHY_OFFSET_23);
		                    
		stats->tx_errors = gswPbusRead_extend(GSW_TX_COLC(port_id)-GSW_BASE, GSW_PHY_OFFSET_23);
		stats->collisions = gswPbusRead_extend(GSW_TX_COLC(port_id)-GSW_BASE, GSW_PHY_OFFSET_23);	

        return stats;

    }
#endif

    if(use_ext_switch)
	{
		stats->rx_packets = gswPbusRead(GSW_RX_UNIC(port_id)-GSW_BASE) + 
		                    gswPbusRead(GSW_RX_MULC(port_id)-GSW_BASE);
		stats->tx_packets = gswPbusRead(GSW_TX_UNIC(port_id)-GSW_BASE) + 
		                    gswPbusRead(GSW_RX_MULC(6)-GSW_BASE);
		stats->rx_bytes = gswPbusRead(GSW_RX_OCL(port_id)-GSW_BASE);
		stats->tx_bytes = gswPbusRead(GSW_TX_OCL(port_id)-GSW_BASE);
		stats->rx_dropped = gswPbusRead(GSW_RX_DROC(port_id)-GSW_BASE);
		stats->tx_dropped = gswPbusRead(GSW_TX_DROC(port_id)-GSW_BASE);
		stats->multicast = gswPbusRead(GSW_RX_MULC(port_id)-GSW_BASE);
		stats->rx_errors =  gswPbusRead(GSW_RX_ALIGE(port_id)-GSW_BASE) + 
		                    gswPbusRead(GSW_RX_CRC(port_id)-GSW_BASE) + 
		                    gswPbusRead(GSW_RX_RUNT(port_id)-GSW_BASE) + 
		                    gswPbusRead(GSW_RX_FRGE(port_id)-GSW_BASE) + 
		                    gswPbusRead(GSW_RX_LONG(port_id)-GSW_BASE);
		                    
		stats->tx_errors = gswPbusRead(GSW_TX_COLC(port_id)-GSW_BASE);
		stats->collisions = gswPbusRead(GSW_TX_COLC(port_id)-GSW_BASE);	

        return stats;
	}
	else	
	{
		stats->rx_packets = read_reg_word(GSW_RX_UNIC(port_id)) + 
		                    read_reg_word(GSW_RX_MULC(port_id));
		stats->tx_packets = read_reg_word(GSW_TX_UNIC(port_id)) + 
		                    read_reg_word(GSW_TX_MULC(port_id));
		stats->rx_bytes = read_reg_word(GSW_RX_OCL(port_id));
		stats->tx_bytes = read_reg_word(GSW_TX_OCL(port_id));
		stats->rx_dropped = read_reg_word(GSW_RX_DROC(port_id));
		stats->tx_dropped = read_reg_word(GSW_TX_DROC(port_id));
		stats->multicast = read_reg_word(GSW_RX_MULC(port_id));
		stats->rx_errors = read_reg_word(GSW_RX_ALIGE(port_id)) + 
		                    read_reg_word(GSW_RX_CRC(port_id)) + 
		                    read_reg_word(GSW_RX_RUNT(port_id)) + 
		                    read_reg_word(GSW_RX_FRGE(port_id)) + 
		                    read_reg_word(GSW_RX_LONG(port_id));
		                    
		stats->tx_errors = read_reg_word(GSW_TX_COLC(port_id));
		stats->collisions = read_reg_word(GSW_TX_COLC(port_id));

        if(link_aggr_en)
        {
            if(!strcmp(dev->name,lan_virtual_dev[SEP_ETH_ITF_2]->name))
            {
                /*eth0.4*/
                port_id  =  lan_port_map[4-itf_start_idx];
                stats->rx_packets += read_reg_word(GSW_RX_UNIC(port_id)) + 
                read_reg_word(GSW_RX_MULC(port_id));
                stats->tx_packets += read_reg_word(GSW_TX_UNIC(port_id)) + 
                read_reg_word(GSW_RX_MULC(6));
                stats->rx_bytes += read_reg_word(GSW_RX_OCL(port_id));
                stats->tx_bytes += read_reg_word(GSW_TX_OCL(port_id));
                stats->rx_dropped += read_reg_word(GSW_RX_DROC(port_id));
                stats->tx_dropped += read_reg_word(GSW_TX_DROC(port_id));
                stats->multicast += read_reg_word(GSW_RX_MULC(port_id));
                stats->rx_errors += read_reg_word(GSW_RX_ALIGE(port_id)) + 
                read_reg_word(GSW_RX_CRC(port_id)) + 
                read_reg_word(GSW_RX_RUNT(port_id)) + 
                read_reg_word(GSW_RX_FRGE(port_id)) + 
                read_reg_word(GSW_RX_LONG(port_id));

                stats->tx_errors += read_reg_word(GSW_TX_COLC(port_id));
                stats->collisions += read_reg_word(GSW_TX_COLC(port_id));
            }
            else if(!strcmp(dev->name,lan_virtual_dev[SEP_ETH_ITF_3]->name))
            {
                memset(stats,0,sizeof(struct net_device_stats));
            }
        }
        
        return stats;
	}
}

struct rtnl_link_stats64* virtual_lan_dev_stats_64(struct net_device* dev, struct rtnl_link_stats64* stats)
{
	unsigned int port_id=0;
    unsigned int eth_intf = 0;
    
    eth_intf = find_port_by_name(dev);
	
	if(eth_intf < itf_num)
	{
	    port_id  =  lan_port_map[eth_intf];
	}else{
	    port_id = 6;
	}
	if(port_id < 0 || port_id >= GSW_MAX_PORT)
		return stats;
	
	{
		stats->rx_packets = (*(*(Expand_64_Rx + 2)+port_id)<<32) +switch_reg_read(GSW_RX_UNIC(port_id)) + (*(*(Expand_64_Rx + 3)+port_id)<<32) +switch_reg_read(GSW_RX_MULC(port_id));
		stats->tx_packets = (*(*(Expand_64_Tx + 2)+port_id)<<32 )+ switch_reg_read(GSW_TX_UNIC(port_id)) + (*(*(Expand_64_Rx + 3)+port_id)<<32) + switch_reg_read(GSW_TX_MULC(port_id));
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

        if(link_aggr_en)
        {
            if(!strcmp(dev->name,lan_virtual_dev[SEP_ETH_ITF_2]->name))
            {
                /*eth0.4*/
                port_id  =  lan_port_map[4-itf_start_idx];
        		stats->rx_packets += (*(*(Expand_64_Rx + 2)+port_id)<<32) +switch_reg_read(GSW_RX_UNIC(port_id)) + (*(*(Expand_64_Rx + 3)+port_id)<<32) +switch_reg_read(GSW_RX_MULC(port_id));
				stats->tx_packets += (*(*(Expand_64_Tx + 2)+port_id)<<32 )+ switch_reg_read(GSW_TX_UNIC(port_id)) + (*(*(Expand_64_Rx + 3)+port_id)<<32) +switch_reg_read(GSW_TX_MULC(port_id));
        		stats->rx_bytes += (*(*(Expand_64_Rx + 12)+port_id)<<32) + switch_reg_read(GSW_RX_OCL(port_id));
        		stats->tx_bytes += (*(*(Expand_64_Tx + 12) +port_id)<<32) + switch_reg_read(GSW_TX_OCL(port_id));
        		stats->rx_dropped += (*(*Expand_64_Rx + port_id)<< 32) + switch_reg_read(GSW_RX_DROC(port_id));
        		stats->tx_dropped += (*(*Expand_64_Tx + port_id)<<32) + switch_reg_read(GSW_TX_DROC(port_id));
        		stats->multicast += (*(*(Expand_64_Rx + 3)+port_id)<<32 )+ switch_reg_read(GSW_RX_MULC(port_id));
        		stats->rx_errors += (*(*(Expand_64_Rx + 5) +port_id)<<32) + switch_reg_read(GSW_RX_ALIGE(port_id)) + (*(*(Expand_64_Rx + 6)+port_id)<<32) + switch_reg_read(GSW_RX_CRC(port_id)) + 
        							(*(*(Expand_64_Rx + 7)+port_id)<<32) + switch_reg_read(GSW_RX_RUNT(port_id)) + (*(*(Expand_64_Rx + 8)+port_id)<<32 )+ switch_reg_read(GSW_RX_FRGE(port_id)) + 
        							(*(*(Expand_64_Rx + 9)+port_id)<<32) + switch_reg_read(GSW_RX_LONG(port_id));
        		stats->tx_errors += (*(*(Expand_64_Tx + 5) +port_id)<<32) + switch_reg_read(GSW_TX_COLC(port_id));
        		stats->collisions += (*(*(Expand_64_Tx+ 5)+ port_id)<<32 )+ switch_reg_read(GSW_TX_COLC(port_id));
            }
            else if(!strcmp(dev->name,lan_virtual_dev[SEP_ETH_ITF_3]->name))
            {
                memset(stats,0,sizeof(struct net_device_stats));
            }
        }

	}
	return stats;	
}

static int virtual_dev_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
#ifdef TCSUPPORT_ECNT_1588v2
	if (cmd == SIOCSHWTSTAMP) {
		return ecnt_config_hwtstamp(dev, ifr);
	} else
#endif
	{
		return generic_mii_ioctl(&mac_p->mii_if, if_mii(ifr), cmd, NULL);
	}
}

static int virtual_dev_set_macaddr(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;

	/* Check if given address is valid ethernet MAC address */
  	if (!is_valid_ether_addr(addr->sa_data))
    		return(-EIO);

	/* Save the customize mac address */
  	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	return 0; /* Do nothing */
}

int init_virtual_interface(void)
{
    struct net_device *dev = NULL;
    int err, index = 0;
    char dev_name[10];
	bool gsw_support_ptp=0;

    for	(;index < itf_num; index++){
	if(check_lanport_type(index)){
		continue;
	}
        /* virtual dev start from eth0.1 to eth0.4*/
        memset(dev_name, 0, sizeof(dev_name));
        sprintf(dev_name, "%s%d", sep_itf, itf_start_idx + index);
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,18,19)      
        dev = alloc_netdev(sizeof(macAdapter_t), dev_name, NET_NAME_USER, ether_setup);
	#else
		dev = alloc_netdev(sizeof(macAdapter_t), dev_name, ether_setup);
	#endif
        if (!dev)
    	    return -ENOMEM;
        lan_virtual_dev[index] = dev;
        mac_virtual_p[index] = netdev_priv(dev);

#if KERNEL_2_6_36
		dev->netdev_ops = &virtual_dev_ops; 
		mac_virtual_p[index]->napi.weight = MAC_NAPI_WEIGHT;	
		netif_napi_add(dev, &mac_virtual_p[index]->napi, 
		    NULL, MAC_NAPI_WEIGHT);
#else
		dev->init = tc3262_gmac_start;
#endif

#if defined(TCSUPPORT_KERNEL_API)  || defined(TCSUPPORT_ECNT_1588v2)
		if (1 == (itf_start_idx + index)) { //GSW eth0.1 support ptp
			gsw_support_ptp=1;
		}
		ecnt_set_ethtool_ops_and_1588v2(dev,gsw_support_ptp);
#endif

        err = register_netdev(dev);
        if (err)
		{
			printk(KERN_INFO "%s(): register netdev fail! itf=%d\n",
			    __FUNCTION__, index);
			return err;
		}
    }
    return 0;
}

void exit_virtual_interface(void)
{
	int itf = 0;

	for(itf = 0; itf < itf_num; itf++)
	{
		unregister_netdev(lan_virtual_dev[itf]);
	}
}

__IMEM int devFromWhichVirtualEthItf(struct net_device *dev)
{
	int itf = 0;
	for(; itf < itf_num; itf++)
	{
		if(dev == lan_virtual_dev[itf])
			return itf;
	}
	//printk("found no proper seperated Ether interface!\r\n");
	return 0;
}

inline void setSkbFromWhichVirtualEthItf(struct sk_buff *skb, int itf)
{
	switch(itf)
	{
		case SEP_ETH_ITF_0:			
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_0;
			break;
		case SEP_ETH_ITF_1:			
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_1;
			break;
		case SEP_ETH_ITF_2:		
                        if(hwnat_skb_to_foe_hook&&link_aggr_en)
                        {
                            if((hwnat_skb_to_foe_hook(skb)&0x4)||(-1 == hwnat_skb_to_foe_hook(skb)))
                                *(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_2;
                            else
                                *(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_3;
                        }
            		else
			    *(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_2;
			break;
		case SEP_ETH_ITF_3:			
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_3;
			break;
		case SEP_ETH_ITF_4:
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_4;
			break;
		case SEP_ETH_ITF_5:
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_5;
			break;
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
		case SEP_ETH_ITF_6: 		
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_6;
			break;
		case SEP_ETH_ITF_7: 		
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_7;
			break;
		case SEP_ETH_ITF_8: 		
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_8;
			break;
		case SEP_ETH_ITF_9:
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_9;
			break;
		case SEP_ETH_ITF_10:
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_10;
			break;
#endif

		default:
			*(unsigned long int *)(skb->cb) = CB_MBGIC_ETH_NONE;
			break;
	}	
	return;	
}

inline int skbFromWhichVirtualEthItf(struct sk_buff *skb)
{

    switch(*(unsigned long int *)(skb->cb))
    {
        case CB_MBGIC_ETH_0:
            return SEP_ETH_ITF_0;
            
        case CB_MBGIC_ETH_1:
            return SEP_ETH_ITF_1;
            
        case CB_MBGIC_ETH_2:
            return SEP_ETH_ITF_2;
            
        case CB_MBGIC_ETH_3:
            return SEP_ETH_ITF_3;
            
		case CB_MBGIC_ETH_4:
			return SEP_ETH_ITF_4;

		case CB_MBGIC_ETH_5:
			return SEP_ETH_ITF_5;
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
		case CB_MBGIC_ETH_6:
			return SEP_ETH_ITF_6;
			
		case CB_MBGIC_ETH_7:
			return SEP_ETH_ITF_7;
			
		case CB_MBGIC_ETH_8:
			return SEP_ETH_ITF_8;
			
		case CB_MBGIC_ETH_9:
			return SEP_ETH_ITF_9;

		case CB_MBGIC_ETH_10:
			return SEP_ETH_ITF_10;
#endif

        default:
            /*printk("found no proper SKB MAGIC Ethernet itf info!\r\n");*/
            return SEP_ETH_ITF_UNKNOWN; 
    }
    return 0;
}

#ifdef TCSUPPORT_QTN_ISOLATION
#define VLAN_VID_MASK               0x0fff 
#define ISOLATION5G_VLAN_NUM        40
int QtnSecondaryPkt(struct sk_buff *skb)
{
    uint8 *cp=NULL;
    uint16 tag;
    uint16 tpid;
    uint16 vlan_id;

    cp = skb->data;
    tpid = *(uint16 *)(cp+12);
    tag  = *(uint16 *)(cp+14);

    if(tpid == ETH_P_8021Q)
    {
        vlan_id = (tag & VLAN_VID_MASK);
        if(vlan_id == ISOLATION5G_VLAN_NUM)
        {
            return 1;
        }
    }
    return 0;
}

struct sk_buff * AddVlanTag(struct sk_buff *skb, unsigned short vid)
{
	skb = __vlan_put_tag(skb, vid);

	return skb;
}

struct sk_buff * RemoveVlanTag(struct sk_buff *skb)
{
	uint8 * buff = skb->data;
	uint16 type = 0;

	type = *(short int*)(buff+16);
	memmove(buff+4,buff,14);
	skb_pull(skb,4);
	skb->mac_header += 4;
	((struct ethhdr *)skb->data)->h_proto = type;
	skb->protocol = type;

	return skb;
}

inline struct sk_buff *virtual_itf_tx_hook(struct sk_buff *skb, struct net_device *dev)
{
    if(isolation_enable){
    	if(strcmp(dev->name, "eth0.6") == 0)
    	{
    		if(AddVlanTag(skb, ISOLATION5G_VLAN_NUM) == NULL)
    		{
    			return NULL;
    		}
    	}
    }
	setSkbFromWhichVirtualEthItf(skb, devFromWhichVirtualEthItf(dev));
	return skb;
}
#else
inline void virtual_itf_tx_hook(struct sk_buff *skb, struct net_device *dev)
{
    setSkbFromWhichVirtualEthItf(skb, devFromWhichVirtualEthItf(dev));
}
#endif

inline void virtual_itf_rx_hook(struct sk_buff *skb)
{
    int interface_index;
    if(vport_enable == DISABLE){
        return ;
    }
    interface_index = skbFromWhichVirtualEthItf(skb);
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
	if((SEP_ETH_ITF_0 <= interface_index) && (interface_index <= SEP_ETH_ITF_7))
#else
	if((SEP_ETH_ITF_0 <= interface_index) && (interface_index <= SEP_ETH_ITF_4))
#endif
	{
#ifdef TCSUPPORT_QTN_ISOLATION
    if(isolation_enable){
        if(interface_index == SEP_ETH_ITF_4)
        {
            if(QtnSecondaryPkt(skb))
            {
            	RemoveVlanTag(skb);
                interface_index = SEP_ETH_ITF_5;
            }
        }
    }
#endif
        if((interface_index == SEP_ETH_ITF_3)&&link_aggr_en)
            skb->dev = lan_virtual_dev[SEP_ETH_ITF_2];
        else
            skb->dev = lan_virtual_dev[interface_index];
    }
}

int virtual_itf_link_aggr_config(int enable)
{    
    if(enable)
    {
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,55)
        /*ifconfig eth0.4 down*/
        dev_change_flags(lan_virtual_dev[SEP_ETH_ITF_3],lan_virtual_dev[SEP_ETH_ITF_3]->flags&(~IFF_UP),NULL);
        #else
        dev_change_flags(lan_virtual_dev[SEP_ETH_ITF_3],lan_virtual_dev[SEP_ETH_ITF_3]->flags&(~IFF_UP));
        #endif
    }
    else
    {
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,55)
        /*ifconfig eth0.4 up*/
        dev_change_flags(lan_virtual_dev[SEP_ETH_ITF_3],lan_virtual_dev[SEP_ETH_ITF_3]->flags|IFF_UP,NULL);
        #else
        dev_change_flags(lan_virtual_dev[SEP_ETH_ITF_3],lan_virtual_dev[SEP_ETH_ITF_3]->flags|IFF_UP);
        #endif
    }
    
    return 0;
}

