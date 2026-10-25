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

#include <ecnt_hook/ecnt_hook_ppe.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <linux/proc_fs.h>

#include "ra_nat_multicast.h"
#include "ra_nat_gen.h"
#include "ra_nat_offload.h"
#include "ra_nat_dev.h"
#include "ra_nat_ic_dis.h"
#include "util.h"
#include "foe_fdb.h"
#include <net/addrconf.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
struct list_head ecnt_multicast_flow;
struct list_head ecnt_multicast_local_flow;
DEFINE_SPINLOCK(ecnt_multicast_lock);
DEFINE_SPINLOCK(ecnt_multicast_local_lock);

static int ecnt_multicast_debug_flag = 0;

unsigned int unknow_mc_pkt_cnt = 0;
unsigned long long unknow_mc_byte_cnt = 0;

#define ECNT_MULTICAST_DEBUG(fmt,args...)  		\
		{									\
			if (ecnt_multicast_debug_flag)		\
			{								\
				printk("\r\n[%s]"fmt,__func__,##args);\
			}								\
		}

int ecnt_multicast_hwnat_state_handler_lan_only(unsigned int foe_index,unsigned int  port_mask);
extern int ecnt_multicast_hwnat_state_handler_wlan_only(unsigned int foe_index,unsigned int  port_mask);
int ecnt_multicast_hwnat_state_handler_lan_wlan(unsigned int foe_index,unsigned int  port_mask);
int ecnt_multicast_hwnat_state_handler_lan_xsi(unsigned int foe_index,unsigned int  port_mask);
int ecnt_multicast_hwnat_state_handler_xsi_only(unsigned int foe_index,unsigned int  port_mask);
int ecnt_multicast_hwnat_state_handler_xsi_wlan(unsigned int foe_index,unsigned int  port_mask);
int ecnt_multicast_hwnat_state_handler_lan_hsgmii_1toN(unsigned int foe_index,unsigned int  port_mask);
int ecnt_multicast_hwnat_state_handler_unknown(unsigned int foe_index,unsigned int  port_mask);

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern uint32 dsoffload_presetting;
extern portInfo lanPortState[5];
extern unsigned short port_vlan_bind_vid;
extern unsigned int hwnat_dram_idx_base;

extern void PpeSetInfoBlk2(struct FoeEntry * foe_entry, uint32_t fpidx, uint32_t port_mg, uint32_t port_ag, uint32_t ds_to_qdma);
extern int hwnat_delete_foe_entry(int index);
extern int ecnt_wlan_force_to_cpu_handler(struct sk_buff *skb, unsigned int dp, unsigned int dsoffload);
extern int ecnt_offload_default_offload(struct sk_buff* skb,unsigned int dp, unsigned int dsoffload, struct FoeEntry *foe_entry);
extern struct net_device* ppeGetDstPort(unsigned int dp);
extern void PpeSetRxForceRingIdx(struct FoeEntry * foe_entry, uint8_t ring_index);
extern int localInShortCut(struct sk_buff *skb, int protocol);
extern IGMP_HWNATEntry_t* (*multicast_speed_find_entry_hook)(int index);
extern void ecnt_multicast_add_tmp_info(unsigned char* address,u16 proto, short port_no, int ref_cnt_udpxy, unsigned char status_flag);
extern struct FoeEntryExt*	foe_ext;
extern u8 ext_swicVendor;
extern int ppe_get_vlan_tag_num(struct sk_buff* skb);
extern void ecnt_multicast_hwnat_set_wlan_dev(unsigned long maskindex,struct sk_buff *skb);
extern void remove_skb_data_vlan_tag(struct sk_buff *skb);
extern bool ipv6_chk_mcast_addr(struct net_device *dev, const struct in6_addr *group,
			 const struct in6_addr *src_addr);
extern int ip_check_mc_rcu(struct in_device *in_dev, __be32 mc_addr,
                           __be32 src_addr, u8 proto);
extern int32_t PpeDropPacketHandler(struct sk_buff * skb);
extern char get_onutype(void);
extern struct net_device *ecnt_vlan_find_dev(struct net_device *real_dev, __be16 vlan_proto, u16 vlan_id);
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

static PPE_MULTICAST_FWD_STATE_HANDLER state_handler[]=
{
	{PPE_MULTICAST_FORWARD_STATE_LAN_ONLY,ecnt_multicast_hwnat_state_handler_lan_only},
	{PPE_MULTICAST_FORWARD_STATE_WLAN_ONLY,ecnt_multicast_hwnat_state_handler_wlan_only},
	{PPE_MULTICAST_FORWARD_STATE_LAN_WLAN,ecnt_multicast_hwnat_state_handler_lan_wlan},
	{PPE_MULTICAST_FORWARD_STATE_LAN_XSI,ecnt_multicast_hwnat_state_handler_lan_xsi},
	{PPE_MULTICAST_FORWARD_STATE_XSI_ONLY,ecnt_multicast_hwnat_state_handler_xsi_only},
	{PPE_MULTICAST_FORWARD_STATE_XSI_WLAN,ecnt_multicast_hwnat_state_handler_xsi_wlan},	
	{PPE_MULTICAST_FORWARD_STATE_LAN_HSGMII_1toN,ecnt_multicast_hwnat_state_handler_lan_hsgmii_1toN},
	{PPE_MULTICAST_FORWARD_STATE_UNKNOWN,ecnt_multicast_hwnat_state_handler_unknown},
};
#define PPE_MULTICAST_STATE_HANDLER_NUM (sizeof(state_handler)/sizeof(PPE_MULTICAST_FWD_STATE_HANDLER))

#define MULTICAST_IDX_OVER_SWITCH_LAN(idx)	(idx >= 4)
#define MULTICAST_HSGMII_EXIST(port_mask)	(port_mask & 0xFF0)
#define MULTICAST_WLAN_EXIST(port_mask)		(port_mask & (HWNAT_WLAN_IF_MASK<<HWNAT_WLAN_IF_BASE))

void ecnt_multicast_set_ds_offload(struct FoeEntry *foe_entry,unsigned int foe_index)
{
	foe_ext[foe_index].ds_offload = 4;

	PpeSetRxForceRingIdx(foe_entry, RING_IDX_1);

	PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);

    	return;
}

inline int ecnt_multicast_get_channel_by_stag(unsigned int stag_dp)
{
	int i, channel = 1 ,tmp = 0;
	
	for(i=0;i<5;i++)
	{
		if( ((1<<i) & stag_dp ) == 0 )
			continue;

		channel = i;
		
		if(2 == lanPortState[i].linkSpeed)
			break;
		if(1 == lanPortState[i].linkSpeed)
			tmp = i;	
	}
	if((i == 5)&&(tmp != 0))
		channel = tmp;
	return channel ;
}

unsigned int ecnt_muliticast_get_forward_state(unsigned int port_mask, unsigned int local, unsigned int ext_port_num)
{
	int i,j = 0;
	int normal_lan_flag = 0;
	int hsgmii_lan_flag = 0;
	int aewan_lan_flag = 0;

	if(0 == port_mask)
    {
        if (local || ext_port_num)
            return PPE_MULTICAST_FORWARD_STATE_WLAN_ONLY;
        else
		    return PPE_MULTICAST_FORWARD_STATE_UNKNOWN;
    }

	for(i = HWNAT_LAN_IF_BASE; i < (HWNAT_LAN_IF_BASE+HWNAT_LAN_IF_MAXNUM);i++)
	{
		if(port_mask&(1<<i))
		{			
			if(check_lanport_type(i))
				hsgmii_lan_flag += 1;
			else
				normal_lan_flag += 1;
		}
	}
	
	if(-1 !=get_aewan_idx() && (port_mask&(1<< get_aewan_idx())))
	{			
		aewan_lan_flag = 1;
	}
	
	if(normal_lan_flag > 0)
	{

			for(j = HWNAT_USB_IF_BASE;j < (HWNAT_USB_IF_BASE+HWNAT_USB_IF_NUM);j++)
			{
				if(port_mask&(1<<j))
					return PPE_MULTICAST_FORWARD_STATE_LAN_WLAN;/* for usb dongle, we treat it as wlan here */
			}
            
			for(j = HWNAT_WLAN_IF_BASE;j < (HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_MAXNUM);j++)
			{
				if(SUPPORT_QDMA_MULTICAST_1toN)
				{
					if(MULTICAST_HSGMII_EXIST(port_mask) || hsgmii_lan_flag > 0)
					{
						return PPE_MULTICAST_FORWARD_STATE_LAN_HSGMII_1toN;		/* wifi handle in this state */
					}
				}

				if(port_mask&(1<<j))
					return PPE_MULTICAST_FORWARD_STATE_LAN_WLAN;/*wlan+lan or wlan+lan+xfi*/
			}
			
			if(SUPPORT_QDMA_MULTICAST_1toN)
			{
				if(MULTICAST_HSGMII_EXIST(port_mask) || hsgmii_lan_flag > 0 || aewan_lan_flag)
				{
					return PPE_MULTICAST_FORWARD_STATE_LAN_HSGMII_1toN;
				}
			}
			
			if(port_mask&(HWNAT_XSI_IF_MASK<<HWNAT_XSI_IF_BASE))
				return PPE_MULTICAST_FORWARD_STATE_LAN_XSI;
            
            if (local || ext_port_num)
                return PPE_MULTICAST_FORWARD_STATE_LAN_WLAN;
            else
			    return PPE_MULTICAST_FORWARD_STATE_LAN_ONLY;
		}

	if(port_mask&(HWNAT_XSI_IF_MASK<<HWNAT_XSI_IF_BASE))
	{	
		for(j = HWNAT_WLAN_IF_BASE;j < (HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_MAXNUM);j++)
		{
			if(port_mask&(1<<j))
				return PPE_MULTICAST_FORWARD_STATE_XSI_WLAN;
		}

		if( hsgmii_lan_flag > 0) /*hsgmii interface is exsit*/
			return PPE_MULTICAST_FORWARD_STATE_LAN_HSGMII_1toN;
							
		return PPE_MULTICAST_FORWARD_STATE_XSI_ONLY;
	}
	
	if( hsgmii_lan_flag > 1) /*normal interface is zero and hsgmii interface > 1*/
		return PPE_MULTICAST_FORWARD_STATE_LAN_HSGMII_1toN;
	else if( hsgmii_lan_flag == 1)
		return PPE_MULTICAST_FORWARD_STATE_XSI_ONLY;
	if(aewan_lan_flag)
	{
		return PPE_MULTICAST_FORWARD_STATE_XSI_ONLY;
	}
	
	return PPE_MULTICAST_FORWARD_STATE_WLAN_ONLY;
}

static int ecnt_multicast_ext_switch_lan_only(struct FoeEntry *foe_entry,unsigned int  port_mask)
{
	unsigned int vlan1 = 0, sptag=0;
	unsigned char gsw_port = 0;
	int i = 0;

	if(foe_entry == NULL)
		return -1;

	for(i = 0;i < 8;i++)
	{
		if(port_mask&(1<<i))
		{
			ETHER_API_GET_PORTMAP((i+5),&gsw_port);
			vlan1 |= (1<<gsw_port);
		}
	}
	for(i = 8;i < 10;i++)
	{
		if(port_mask&(1<<i))
		{
			ETHER_API_GET_PORTMAP((i+5),&gsw_port);
			sptag |= (1<<(gsw_port-16));
		}
	}
	
	if(IS_IPV4_GRP(foe_entry))
	{
		foe_entry->ipv4_hnapt.channel = 12;
		foe_entry->ipv4_hnapt.bfib1.vlan_layer = 1;
		foe_entry->ipv4_hnapt.etype = sptag;
		/*foe_entry->ipv4_hnapt.etype = 0;*/
		foe_entry->ipv4_hnapt.vlan1 = vlan1;
		set_info2_nbq(foe_entry, IPV4_HNAPT, 6);

	}
	else if(IS_IPV6_GRP(foe_entry))
	{
		foe_entry->ipv6_5t_route.channel = 12;
		foe_entry->ipv6_5t_route.bfib1.vlan_layer = 1;
		foe_entry->ipv6_5t_route.etype = sptag;
		foe_entry->ipv6_5t_route.vlan1 = vlan1;
		set_info2_nbq(foe_entry, IPV6_5T_ROUTE, 6);
	}
	else
		return -1;

	PpeSetInfoBlk2(foe_entry, FP_GDMA3, 0x3F, 0x3F, 1);

	return 0;
}

int ecnt_multicast_hwnat_state_handler_lan_only(unsigned int foe_index,unsigned int  port_mask)
{
	int force_dstq = 0;
	unsigned int stag_dp = 0;
	unsigned char gsw_port = 0;
	int i = 0, channel = 1;
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	int need_ds_offload = 0;

	foe_entry = getPpeEntryLocal(foe_index, foe_entry);
	if(foe_entry == NULL)
		return -1;
	
	if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&(dsoffload_presetting&DS_MULTICAST))
		need_ds_offload = 1;

	ETHER_API_GET_FORCE_DSTQ(&force_dstq);

	for(i = 0;i < 4;i++)
	{
		if(port_mask&(1<<i))
		{
			ETHER_API_GET_PORTMAP(i,&gsw_port);
			stag_dp |= (1<<gsw_port);
		}
	}

	channel = ecnt_multicast_get_channel_by_stag(stag_dp);

	if(IS_IPV4_GRP(foe_entry))
	{
		foe_entry->ipv4_hnapt.channel = channel;
		//set mcast and act_dp
		foe_entry->ipv4_hnapt.mcast = 0;
		foe_entry->ipv4_hnapt.act_dp = 0;//not useful for pure lan
		//set stag for lan	
		foe_entry->ipv4_hnapt.etype &= (~0x3f);
		foe_entry->ipv4_hnapt.etype |= stag_dp;
		if(FOE_MC_CFG_SUPPORT)
		{	  
			set_info2_fast(foe_entry, IPV4_HNAPT, 0);
			set_info2_nbq(foe_entry, IPV4_HNAPT, 1);
		}
		else
		{
			foe_entry->ipv4_hnapt.bfib1.vlan_layer = 1;  
			if(port_vlan_bind_vid){
				foe_entry->ipv4_hnapt.etype |= 0x0100;
				foe_entry->ipv4_hnapt.vlan1 = (0xfff&port_vlan_bind_vid);
			}	
		}

	}
	else if(IS_IPV6_GRP(foe_entry))
	{
		foe_entry->ipv6_5t_route.channel = channel;
		//set mcast and act_dp
		foe_entry->ipv6_5t_route.mcast = 0;
		foe_entry->ipv6_5t_route.act_dp = 0;//not useful for pure lan
		
		//set stag for lan	
		foe_entry->ipv6_5t_route.etype &= (~0x3f);
		foe_entry->ipv6_5t_route.etype |= stag_dp;   
		if(FOE_MC_CFG_SUPPORT)
		{	   
			set_info2_fast(foe_entry, IPV6_5T_ROUTE, 0);
			set_info2_nbq(foe_entry, IPV6_5T_ROUTE, 1);
		}
		else
		{
			foe_entry->ipv6_5t_route.bfib1.vlan_layer = 1;
			if(port_vlan_bind_vid){
				foe_entry->ipv6_5t_route.etype |= 0x0100;
				foe_entry->ipv6_5t_route.vlan1 = (0xfff&port_vlan_bind_vid);
			}	
		}
	}
	else
		return -1;
	
	if(need_ds_offload)
		ecnt_multicast_set_ds_offload(foe_entry,foe_index);
	else
	{
		if (1 != force_dstq)
			PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);
		else
			PpeSetInfoBlk2(foe_entry, FP_GDMA1, 0x3F, 0x3F, 0);
	}

	if(ext_swicVendor == 12) //for en8851
		ecnt_multicast_ext_switch_lan_only(foe_entry, port_mask);
	if(SUPPORT_SRAM_ENTRY && (foe_index<hwnat_dram_idx_base)) 
	{
		setPpeEntry(foe_index, foe_entry);
	}

	return 0;
}


int ecnt_multicast_hwnat_state_handler_lan_wlan(unsigned int foe_index,unsigned int  port_mask)
{
	int force_dstq = 0;
	unsigned int stag_dp = 0;
	unsigned char gsw_port = 0;
	int i = 0, channel = 1;
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	int need_ds_offload = 0;

	foe_entry = getPpeEntryLocal(foe_index, foe_entry);
	if(foe_entry == NULL)
		return -1;
	
	if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&(dsoffload_presetting&DS_MULTICAST))
		need_ds_offload = 1;

	ETHER_API_GET_FORCE_DSTQ(&force_dstq);

	for(i = 0;i < 4;i++)
	{
		if(port_mask&(1<<i))
		{
			ETHER_API_GET_PORTMAP(i,&gsw_port);
			stag_dp |= (1<<gsw_port);
		}
	}

    PpeSetRxForceRingIdx(foe_entry, RING_IDX_1);
	channel = ecnt_multicast_get_channel_by_stag(stag_dp);

	if(IS_IPV4_GRP(foe_entry))
	{
		foe_entry->ipv4_hnapt.channel = channel;

		//set stag for lan	
		foe_entry->ipv4_hnapt.etype &= (~0x3f);
		foe_entry->ipv4_hnapt.etype |= stag_dp;
		if(FOE_MC_CFG_SUPPORT)
		{	  
			set_info2_fast(foe_entry, IPV4_HNAPT, 0);
		}
		else
		{
			foe_entry->ipv4_hnapt.bfib1.vlan_layer = 1;  
		}
	}
	else if(IS_IPV6_GRP(foe_entry))
	{
		foe_entry->ipv6_5t_route.channel = channel;
		
		//set stag for lan	
		foe_entry->ipv6_5t_route.etype &= (~0x3f);
		foe_entry->ipv6_5t_route.etype |= stag_dp;   
		if(FOE_MC_CFG_SUPPORT)
		{	   
			set_info2_fast(foe_entry, IPV6_5T_ROUTE, 0);
		}
		else
		{
			foe_entry->ipv6_5t_route.bfib1.vlan_layer = 1;
		}
	}
	else
		return -1;
	
	if(need_ds_offload)
		ecnt_multicast_set_ds_offload(foe_entry,foe_index);
	else
	{
		if (1 != force_dstq)
			PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);
		else
			PpeSetInfoBlk2(foe_entry, FP_GDMA1, 0x3F, 0x3F, 0);
	}

	/*
	When the mcast state change from "wlan ony" to "lan+wlan",
	we need to set fport=FP_QDMA1_HW first, amd then set mcast=1.
	Other there will be a short memont when fport=FP_PDMA and mcast=1,
	and that is not permitted.
	*/
	if(IS_IPV4_GRP(foe_entry)){
		//set mcast and act_dp
		foe_entry->ipv4_hnapt.mcast = 1;
		foe_entry->ipv4_hnapt.act_dp = 0;//not useful for pure lan
	}
	else if(IS_IPV6_GRP(foe_entry))
	{
		//set mcast and act_dp
		foe_entry->ipv6_5t_route.mcast = 1;
		foe_entry->ipv6_5t_route.act_dp = 0;//not useful for pure lan
	}

	if(SUPPORT_SRAM_ENTRY && (foe_index<hwnat_dram_idx_base)) 
	{
		setPpeEntry(foe_index, foe_entry);
	}

	return 0;
}
int ecnt_multicast_hwnat_state_handler_lan_xsi(unsigned int foe_index,unsigned int  port_mask)
{
	int force_dstq = 0;
	unsigned int stag_dp = 0;
	unsigned char gsw_port = 0;
	int i = 0, channel = 1;
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	int need_ds_offload = 0;
	foe_entry = getPpeEntryLocal(foe_index, foe_entry);
	if(foe_entry == NULL)
		return -1;
	if(TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&(dsoffload_presetting&DS_MULTICAST))
		need_ds_offload = 1;

	ETHER_API_GET_FORCE_DSTQ(&force_dstq);

	for(i = 0;i < 4;i++)
	{
		if(port_mask&(1<<i))
		{
			ETHER_API_GET_PORTMAP(i,&gsw_port);
			stag_dp |= (1<<gsw_port);
		}
	}
      
	channel = ecnt_multicast_get_channel_by_stag(stag_dp);
	PpeSetRxForceRingIdx(foe_entry, channel);

	if(IS_IPV4_GRP(foe_entry))
	{
		foe_entry->ipv4_hnapt.channel = channel;

        
		//set mcast and act_dp
		foe_entry->ipv4_hnapt.mcast = 1;
		foe_entry->ipv4_hnapt.act_dp = DP_XSI;
		//set stag for lan	
		foe_entry->ipv4_hnapt.etype &= (~0x3f);
		foe_entry->ipv4_hnapt.etype |= stag_dp;
		if(FOE_MC_CFG_SUPPORT)
		{	  
			set_info2_fast(foe_entry, IPV4_HNAPT, 0);
		}
		else
		{
			foe_entry->ipv4_hnapt.bfib1.vlan_layer = 1;  
		}
	}
	else if(IS_IPV6_GRP(foe_entry))
	{
		foe_entry->ipv6_5t_route.channel = channel;
		//set mcast and act_dp
		foe_entry->ipv6_5t_route.mcast = 1;
		foe_entry->ipv6_5t_route.act_dp = DP_XSI;
		
		//set stag for lan	
		foe_entry->ipv6_5t_route.etype &= (~0x3f);
		foe_entry->ipv6_5t_route.etype |= stag_dp;
		if(FOE_MC_CFG_SUPPORT)
		{	   
			set_info2_fast(foe_entry, IPV6_5T_ROUTE, 0);
		}
		else
		{
			foe_entry->ipv6_5t_route.bfib1.vlan_layer = 1;
		}
	}
	else
		return -1;
	
	if(need_ds_offload)
		ecnt_multicast_set_ds_offload(foe_entry,foe_index);
	else
	{
		if (1 != force_dstq)
			PpeSetInfoBlk2(foe_entry, FP_QDMA1_HW, 0x3F, 0x3F, 1);
		else
			PpeSetInfoBlk2(foe_entry, FP_GDMA1, 0x3F, 0x3F, 0);
	}

	if(SUPPORT_SRAM_ENTRY && (foe_index<hwnat_dram_idx_base)) 
	{
		setPpeEntry(foe_index, foe_entry);
	}

	return 0;
}

int ecnt_multicast_hwnat_state_handler_xsi_only(unsigned int foe_index,unsigned int  port_mask)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	int i = 0, channel = 0, nbq = 0,exist_ext_port_num = 0;
	int mcast = 0;
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;
		
	foe_entry = getPpeEntryLocal(foe_index, foe_entry);
	if(foe_entry == NULL)
		return -1;

	if(SUPPORT_QDMA_MULTICAST_1toN)
	{	
		if(ext_swicVendor == 11)  //for en8850
		{
			for(i = 0; i < 4; i++)
			{
				if(port_mask&(1<<(8+i)))
					channel |= (1<<i);		  //eth0.9~eth0.12 channel 6~9: channel bit3~8
			}
		}
		
		if(-1 != get_hsgmii_idx(0) && port_mask&(HWNAT_XSI_IF_MASK <<get_hsgmii_idx(0)))             //usb serdes eth0.5 channel 12: fport bit1
		{
			nbq |= (1<<1);
		}
		if(-1 != get_hsgmii_idx(1) && port_mask&(HWNAT_XSI_IF_MASK<<get_hsgmii_idx(1)))             //pcie0 serdes eth0.6 channel 10: channel bit7
		{
			channel |= (1<<4);
		}
		if(-1 != get_hsgmii_idx(2) && port_mask&(HWNAT_XSI_IF_MASK<<get_hsgmii_idx(2)))             //pcie1 serdes eth0.7 channel 11: fport bit0
		{
			nbq |= (1<<0);
		}
		if(SUPPORT_ETHERNET_SERDES)
		{
			if(-1 != get_hsgmii_idx(3) && port_mask&(HWNAT_XSI_IF_MASK<<get_hsgmii_idx(3)))             //usb serdes eth0.8 channel 12: fport bit2
			{
				nbq |= (1<<2);
			}
		}
		if(-1 != get_aewan_idx() && port_mask & (HWNAT_AEWAN_LAN_IF_MASK << get_aewan_idx()))
		{
			nbq |= (1<<3);
		}
		mcast = MULTICAST_WLAN_EXIST(port_mask)?1:0;

		list_for_each_entry(entry,&ecnt_multicast_flow,list)
		{
			if (entry->foe_index == foe_index)
			{
				exist_ext_port_num = entry->ppe_multicast_info.ext_port_num;
			}
		}
		if (exist_ext_port_num)
		{
			mcast = 1;
		}

		if(IS_IPV4_GRP(foe_entry))
		{
			//set mcast and act_dp
			foe_entry->ipv4_hnapt.mcast = mcast;
			foe_entry->ipv4_hnapt.act_dp = 0;
			//set nbq channel stag, use qdma multicast
			foe_entry->ipv4_hnapt.etype &= (~0x3f);
			set_info2_fast(foe_entry, IPV4_HNAPT, 0);
			set_channel_info(foe_entry, IPV4_HNAPT, channel);
			set_info2_nbq(foe_entry, IPV4_HNAPT, nbq);
			
			foe_entry->ipv4_hnapt.fqos = 1;
		}
		else if(IS_IPV6_GRP(foe_entry))
		{
			//set mcast and act_dp
			foe_entry->ipv6_5t_route.mcast = mcast;
			foe_entry->ipv6_5t_route.act_dp = 0;
			//set stag for lan	
			foe_entry->ipv6_5t_route.etype &= (~0x3f);
			set_info2_fast(foe_entry, IPV6_5T_ROUTE, 0);
			set_info2_nbq(foe_entry, IPV6_5T_ROUTE, nbq);
			set_channel_info(foe_entry, IPV6_5T_ROUTE, channel);
			foe_entry->ipv6_5t_route.fqos = 1;
			
		}
		else
			return -1;

		PpeSetInfoBlk2(foe_entry, FP_QDMA_MCAST, 0x3F, 0x3F, 1);
	}
	else
	{
		if(IS_IPV4_GRP(foe_entry))
		{
			//set mcast and act_dp
			foe_entry->ipv4_hnapt.mcast = 0;
			foe_entry->ipv4_hnapt.act_dp = 0;
			
			foe_entry->ipv4_hnapt.channel = 12; 	   
			foe_entry->ipv4_hnapt.fqos = 1;
			if(isEN7580){
				set_info2_fast(foe_entry, IPV4_HNAPT, 1);
				set_info2_nbq(foe_entry, IPV4_HNAPT, 4);
			}else if(isEN7523){
				set_info2_fast(foe_entry, IPV4_HNAPT, 0);
				set_info2_nbq(foe_entry, IPV4_HNAPT, 6);
			}
		}
		else if(IS_IPV6_GRP(foe_entry))
		{
			//set mcast and act_dp
			foe_entry->ipv6_5t_route.mcast = 0;
			foe_entry->ipv6_5t_route.act_dp = 0;
			
			foe_entry->ipv6_5t_route.channel = 12;		  
			foe_entry->ipv6_5t_route.fqos = 1;
			if(isEN7580){
				set_info2_fast(foe_entry, IPV6_5T_ROUTE, 1);
				set_info2_nbq(foe_entry, IPV6_5T_ROUTE, 4);
			}else if(isEN7523){
				set_info2_fast(foe_entry, IPV4_HNAPT, 0);
				set_info2_nbq(foe_entry, IPV4_HNAPT, 6);
			}
		}
		else
			return -1;

		PpeSetInfoBlk2(foe_entry, FP_GDMA3, 0x3F, 0x3F, 1);
	}
	if(ext_swicVendor == 12) //for en8851
		ecnt_multicast_ext_switch_lan_only(foe_entry, port_mask);

	if(SUPPORT_SRAM_ENTRY && (foe_index<hwnat_dram_idx_base)) 
	{
		setPpeEntry(foe_index, foe_entry);
	}

	return 0;
}

int ecnt_multicast_hwnat_state_handler_xsi_wlan(unsigned int foe_index,unsigned int  port_mask)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;

	foe_entry = getPpeEntryLocal(foe_index, foe_entry);
	if(foe_entry == NULL)
		return -1;
	
	if(IS_IPV4_GRP(foe_entry))
	{
		//set mcast and act_dp
		foe_entry->ipv4_hnapt.mcast = 1;
		foe_entry->ipv4_hnapt.act_dp = 0;

		foe_entry->ipv4_hnapt.channel = 12; 	   
		foe_entry->ipv4_hnapt.fqos = 1;
		if(isEN7580){
			set_info2_fast(foe_entry, IPV4_HNAPT, 1);
			set_info2_nbq(foe_entry, IPV4_HNAPT, 4);
		}else if(isEN7523){
			set_info2_fast(foe_entry, IPV4_HNAPT, 0);
			set_info2_nbq(foe_entry, IPV4_HNAPT, 6);
		}
		
	}
	else if(IS_IPV6_GRP(foe_entry))
	{
		//set mcast and act_dp
		foe_entry->ipv6_5t_route.mcast = 1;
		foe_entry->ipv6_5t_route.act_dp = 0;

		foe_entry->ipv6_5t_route.channel = 12;		  
		foe_entry->ipv6_5t_route.fqos = 1;
		if(isEN7580){
			set_info2_fast(foe_entry, IPV6_5T_ROUTE, 1);
			set_info2_nbq(foe_entry, IPV6_5T_ROUTE, 4);
		}else if(isEN7523){
			set_info2_fast(foe_entry, IPV6_5T_ROUTE, 0);
			set_info2_nbq(foe_entry, IPV6_5T_ROUTE, 6);
		}
	}
	else
		return -1;
	
	PpeSetInfoBlk2(foe_entry, FP_GDMA3, 0x3F, 0x3F, 1);

	if(SUPPORT_SRAM_ENTRY && (foe_index < hwnat_dram_idx_base)) 
	{
		setPpeEntry(foe_index, foe_entry);
	}

	return 0;
}

int ecnt_multicast_hwnat_state_handler_lan_hsgmii_1toN(unsigned int foe_index,unsigned int  port_mask)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	unsigned char gsw_port = 0;
	unsigned int stag_dp = 0;
	int i = 0, channel = 0, nbq = 0,exist_ext_port_num = 0;
	int mcast = 0;
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;
	
	foe_entry = getPpeEntryLocal(foe_index, foe_entry);
	if(foe_entry == NULL)
		return -1;

	for(i = 0;i < 4;i++)
	{
		if(port_mask&(1<<i))
		{
			ETHER_API_GET_PORTMAP(i,&gsw_port);
			stag_dp |= (1<<gsw_port);
		}
	}

	if(ext_swicVendor == 11)  //for en8850
	{
		for(i = 0; i < 4; i++)
		{
			if(port_mask&(1<<(8+i)))
				channel |= (1<<i);        //eth0.9~eth0.12 channel 6~9: channel bit3~6
		}
	}
	
	if(-1  != get_hsgmii_idx(0) && port_mask&(HWNAT_XSI_IF_MASK<<get_hsgmii_idx(0)))  //usb serdes eth0.5 channel 12: fport bit1
	{
		nbq |= (1<<1);
	}
	if(-1 != get_hsgmii_idx(1) && port_mask&(HWNAT_XSI_IF_MASK<<get_hsgmii_idx(1)))  //pcie0 serdes eth0.6 channel 10: channel bit7
	{
		channel |= (1<<4);
	}
	if(-1 != get_hsgmii_idx(2) && port_mask&(HWNAT_XSI_IF_MASK<<get_hsgmii_idx(2)))  //pcie1 serdes eth0.7 channel 11: fport bit0
	{
		nbq |= (1<<0);
	}
	if(SUPPORT_ETHERNET_SERDES)
	{
		if(-1 != get_hsgmii_idx(3) && port_mask&(HWNAT_XSI_IF_MASK<<get_hsgmii_idx(3)))			 //usb serdes eth0.8 channel 12: fport bit1
		{
			nbq |= (1<<2);
		}
	}
	if(-1 != get_aewan_idx() && port_mask & (HWNAT_XSI_IF_MASK << get_aewan_idx()))
	{
		nbq |= (1<<3);
	}
	mcast = MULTICAST_WLAN_EXIST(port_mask)?1:0;

	list_for_each_entry(entry,&ecnt_multicast_flow,list)
	{
		if (entry->foe_index == foe_index)
		{
			exist_ext_port_num = entry->ppe_multicast_info.ext_port_num;
		}
	}
	if (exist_ext_port_num)
	{
		mcast = 1;
	}

	if(IS_IPV4_GRP(foe_entry))
	{
		//set mcast and act_dp
		foe_entry->ipv4_hnapt.mcast = mcast;
		foe_entry->ipv4_hnapt.act_dp = 0;//not useful for pure lan
		//set stag for lan	
		foe_entry->ipv4_hnapt.etype &= (~0x3f);
		foe_entry->ipv4_hnapt.etype |= stag_dp;
		foe_entry->ipv4_hnapt.fqos = 1;
		set_channel_info(foe_entry, IPV4_HNAPT, channel);
		set_info2_nbq(foe_entry, IPV4_HNAPT, nbq);
	}
	else if(IS_IPV6_GRP(foe_entry))
	{
		//set mcast and act_dp
		foe_entry->ipv6_5t_route.mcast = mcast;
		foe_entry->ipv6_5t_route.act_dp = 0;
		set_channel_info(foe_entry, IPV6_5T_ROUTE, channel);		  
		//set stag for lan	
		foe_entry->ipv6_5t_route.etype &= (~0x3f);
		foe_entry->ipv6_5t_route.etype |= stag_dp;
		foe_entry->ipv6_5t_route.fqos = 1;
		set_info2_nbq(foe_entry, IPV6_5T_ROUTE, nbq);
	}
	else
		return -1;

	PpeSetInfoBlk2(foe_entry, FP_QDMA_MCAST, 0x3F, 0x3F, 1);

	if(ext_swicVendor == 12) //for en8851
		ecnt_multicast_ext_switch_lan_only(foe_entry, port_mask);

	if(SUPPORT_SRAM_ENTRY && (foe_index<hwnat_dram_idx_base)) 
	{
		setPpeEntry(foe_index, foe_entry);
	}

	return 0;
}

int ecnt_multicast_hwnat_state_handler_unknown(unsigned int foe_index,unsigned int  port_mask)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;

	foe_entry = getPpeEntryLocal(foe_index, foe_entry);
	if(foe_entry == NULL)
		return -1;
	
	PpeSetInfoBlk2(foe_entry, FP_DROP, 0x3F, 0x3F, 1);

	if(SUPPORT_SRAM_ENTRY && (foe_index<hwnat_dram_idx_base)) 
	{
		setPpeEntry(foe_index, foe_entry);
	}
	
	return 0;
}

int ecnt_multicast_hwnat_state_handler(unsigned int foe_index,unsigned int port_mask,unsigned int local, unsigned int ext_port_num)
{
	unsigned int fwd_state = ecnt_muliticast_get_forward_state(port_mask, local, ext_port_num);
	int i = 0;
    
	for(i = 0;i < PPE_MULTICAST_STATE_HANDLER_NUM;i++)
	{
		if(fwd_state == state_handler[i].fwd_state)
			return state_handler[i].handler_func(foe_index,port_mask);
	}

	return 0;
}

int ecnt_multicast_hwnat_get_local(PPE_MULTICAST_INFO_t* info)
{
	PPE_MULTICAST_LOCALENTRY_t* entry = NULL;

	spin_lock(&ecnt_multicast_local_lock);
	list_for_each_entry(entry,&ecnt_multicast_local_flow,list)
	{
		if(memcmp(entry->ppe_multicast_info.grp_addr,info->grp_addr,4))
			continue;
        
        spin_unlock(&ecnt_multicast_local_lock);
		return 1;
	}
	spin_unlock(&ecnt_multicast_local_lock);	 
	return 0;
}

int ecnt_multicast_hwnat_get_local_bh(PPE_MULTICAST_INFO_t* info)
{
	PPE_MULTICAST_LOCALENTRY_t* entry = NULL;

	spin_lock_bh(&ecnt_multicast_local_lock);
	list_for_each_entry(entry,&ecnt_multicast_local_flow,list)
	{
		if(memcmp(entry->ppe_multicast_info.grp_addr,info->grp_addr,4))
			continue;

		spin_unlock_bh(&ecnt_multicast_local_lock);
		return 1;
	}
	
	spin_unlock_bh(&ecnt_multicast_local_lock);
	return 0;
}

int ecnt_multicast_hwnat_update_local(PPE_MULTICAST_INFO_t* info,unsigned int update_mode)
{
	PPE_MULTICAST_LOCALENTRY_t* entry = NULL;
	PPE_MULTICAST_LOCALENTRY_t* tmp = NULL;
	int find_flag = 0;

	spin_lock_bh(&ecnt_multicast_local_lock);
	list_for_each_entry_safe(entry,tmp,&ecnt_multicast_local_flow,list)
	{		
		if(0 == memcmp(entry->ppe_multicast_info.grp_addr,info->grp_addr,4))
        {
            find_flag = 1;
            if (update_mode)
            {
                entry->ref_cnt++;
				ecnt_multicast_add_tmp_info(info->grp_addr, 1, -1, (int)entry->ref_cnt, 1);
            }
            else
            {
                entry->ref_cnt--;
                if (entry->ref_cnt <= 0)
                {
					ecnt_multicast_add_tmp_info(info->grp_addr, 1, -1, 0, 2);
                    list_del(&entry->list);	
                    kfree(entry);
                }
            }
            spin_unlock_bh(&ecnt_multicast_local_lock);
            return 0;
        }
	}

    if (update_mode && !find_flag)
    {
        entry = (PPE_MULTICAST_LOCALENTRY_t* )kmalloc(sizeof(PPE_MULTICAST_LOCALENTRY_t),GFP_ATOMIC);
        if(!entry)
        {
            printk("Error: Not enought memory!\n");
            spin_unlock_bh(&ecnt_multicast_local_lock);
            return -1;
        }
        memset(entry,0,sizeof(PPE_MULTICAST_LOCALENTRY_t));
        memmove(&(entry->ppe_multicast_info),info,sizeof(PPE_MULTICAST_INFO_t));
        entry->ref_cnt = 1;
        list_add_tail(&entry->list,&ecnt_multicast_local_flow);
		ecnt_multicast_add_tmp_info(info->grp_addr, 1, -1, (int)entry->ref_cnt,1);
    }
	spin_unlock_bh(&ecnt_multicast_local_lock);
    
	return 0;
}

int ecnt_multicast_hwnat_subcribe_group(PPE_MULTICAST_INFO_t* info,unsigned int update_mode)
{
    int local;
    
    if (strcmp(current->comm, "udpxy"))
    {
        return 0;
    }

    if (ecnt_multicast_hwnat_update_local(info, update_mode))
    {
        return 0;
    }

    local = ecnt_multicast_hwnat_get_local_bh(info);
    PPE_API_MULTICAST_HWNATENTRY_LIST_UPDATE(info, PPE_MULTICAST_UPDATE_MODE_GRPIP, 1, 0, local);
    
    return 0;
}

void checkPortMask(PPE_MULTICAST_HWNATENTRY_t* entry,unsigned int *port_mask){
	struct net_device *dev_tmp = entry->ppe_multicast_info.ori_dev;
	int if_id =-1;

	if(isLANInterface(dev_tmp)){
		if_id = getSwitchLANIndex(dev_tmp);
		*port_mask &= ~(1<<(HWNAT_LAN_IF_BASE + if_id));
	}
	else if(is24GWiFiInterface(dev_tmp)){
		if_id = get24GLogicWifiIndex(dev_tmp);
		*port_mask &= ~(1<<(HWNAT_WLAN_IF_BASE + if_id));
	}
	else if(is5GWiFiInterface(dev_tmp)){
		if_id = get5GLogicWifiIndex(dev_tmp);
		*port_mask &= ~(1<<(HWNAT_WLAN_IF_I_BASE + if_id));
	}
	else if(is6GWiFiInterface(dev_tmp)){
		if_id = get6GLogicWifiIndex(dev_tmp);
		*port_mask &= ~(1<<(HWNAT_WLAN_IF_I_BASE + HWNAT_WLAN_IF_I_NUM + if_id));
	}
}

static int ecnt_multicast_hwnat_del_drop_by_grp(unsigned int grp_addr)
{
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;
	__be32 addr = 0;

	ECNT_MULTICAST_DEBUG("ecnt_multicast_hwnat_del_drop_by_grp %x\n", grp_addr);

	spin_lock_bh(&ecnt_multicast_lock);
	list_for_each_entry(entry,&ecnt_multicast_flow,list)
	{
		if(PPE_PROTO_IPV4 != entry->ppe_multicast_info.proto)
			continue;

		addr = *(unsigned int *)entry->ppe_multicast_info.grp_addr;
		if(addr != grp_addr)
			continue;

        if (PPE_MULTICAST_HWNATENTRY_STATE_DROP != entry->state)
			continue;
		
		hwnat_delete_foe_entry(entry->foe_index);
		entry->state = PPE_MULTICAST_HWNATENTRY_STATE_UNBIND;
	}
	spin_unlock_bh(&ecnt_multicast_lock);

	return 0;
}	

int ecnt_multicast_hwnat_list_upate(PPE_MULTICAST_INFO_t* info,unsigned int update_mode,unsigned int op_type,unsigned int port_mask,unsigned int local)
{
	int i = 0;
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;
	int find_flag = 0;
	unsigned int foe_index = 0;
	unsigned int  ext_port_num_tmp = 0;
	ECNT_MULTICAST_DEBUG("update entry start, portmask to 0x%x\n",port_mask);

	spin_lock_bh(&ecnt_multicast_lock);
	list_for_each_entry(entry,&ecnt_multicast_flow,list)
	{
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	    if(entry->ppe_multicast_info.br_vid != info->br_vid)
            continue;
#endif  

		if(update_mode&PPE_MULTICAST_UPDATE_MODE_VLAN)
		{
			if((entry->ppe_multicast_info.vlan_tag_num != info->vlan_tag_num)||(entry->ppe_multicast_info.outer_tci != info->outer_tci)||(entry->ppe_multicast_info.inner_tci != info->inner_tci))
				continue;
		}
		
		if(update_mode&PPE_MULTICAST_UPDATE_MODE_GRPIP)
		{
			if(PPE_PROTO_IPV4 == info->proto)
			{
				if(memcmp(entry->ppe_multicast_info.grp_addr,info->grp_addr,4))
					continue;
			}
			else if(PPE_PROTO_IPV6 == info->proto)
			{
				if(memcmp(entry->ppe_multicast_info.grp_addr,info->grp_addr,16))
					continue;
			}
			else
			{
				spin_unlock_bh(&ecnt_multicast_lock);
				return -1;
			}
		}

		if(update_mode&PPE_MULTICAST_UPDATE_MODE_SRCIP)
		{
			if(PPE_PROTO_IPV4 == info->proto)
			{
				if(memcmp(entry->ppe_multicast_info.src_addr,info->src_addr,4))
					continue;
			}
			else if(PPE_PROTO_IPV6 == info->proto)
			{
				if(memcmp(entry->ppe_multicast_info.src_addr,info->src_addr,16))
					continue;
			}
			else
			{
				spin_unlock_bh(&ecnt_multicast_lock);
				return -1;
			}
		}
		ext_port_num_tmp = entry->ppe_multicast_info.ext_port_num;
		PPE_API_MULTICAST_HWNAT_GET_PORTMASK(&(entry->ppe_multicast_info),entry->foe_index,&port_mask);
		if(TCSUPPORT_CT_VAL)
		{
			checkPortMask(entry,&port_mask);
		}

        if ((PPE_MULTICAST_HWNATENTRY_STATE_UNBIND == entry->state)
            || ((op_type == 0) && (entry->port_mask == port_mask)&& (ext_port_num_tmp == entry->ppe_multicast_info.ext_port_num))
            || ((op_type == 1) && (entry->local == local)))
		{
			continue;
		}
		else
		{
			if((((op_type == 0) && (port_mask || entry->ppe_multicast_info.ext_port_num)) || ((op_type == 1) && local))
                &&(entry->state == PPE_MULTICAST_HWNATENTRY_STATE_DROP))
			{
				hwnat_delete_foe_entry(entry->foe_index);
				entry->state = PPE_MULTICAST_HWNATENTRY_STATE_UNBIND;
				continue;
			}

			if (((op_type == 0) && ((0 == port_mask)&& (0 == entry->ppe_multicast_info.ext_port_num)) && !entry->local)
                || ((op_type == 1) && (0 == entry->port_mask) && !local))
				entry->state = PPE_MULTICAST_HWNATENTRY_STATE_DROP;

            if (op_type == 0)
            {
			    entry->port_mask  = port_mask;
                local = entry->local;
            }
            else
            {
                entry->local = local;
                port_mask = entry->port_mask;
            }
			ECNT_MULTICAST_DEBUG("[%s][%d]update entry %d portmask to 0x%x,local=%d\n",
				__FUNCTION__, __LINE__,entry->foe_index,entry->port_mask,local);
			foe_index = entry->foe_index;
			find_flag = 1;
			ecnt_multicast_hwnat_state_handler(foe_index,port_mask,local,entry->ppe_multicast_info.ext_port_num);			
			continue;
		}
	}
	spin_unlock_bh(&ecnt_multicast_lock);

	if(find_flag)
		return 0;

	return -1;
}

int ecnt_multicast_hwnat_get_orig_dev(PPE_MULTICAST_INFO_t* info)
{
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;

	spin_lock_bh(&ecnt_multicast_lock);
	list_for_each_entry(entry,&ecnt_multicast_flow,list)
	{

		if(PPE_PROTO_IPV4 == info->proto)
		{
			if(memcmp(entry->ppe_multicast_info.grp_addr,info->grp_addr,4))
				continue;
			info->ori_dev = entry->ppe_multicast_info.ori_dev;
			memcpy(info->src_addr, entry->ppe_multicast_info.src_addr, 4);
			break;
		}
		else if(PPE_PROTO_IPV6 == info->proto)
		{
			if(memcmp(entry->ppe_multicast_info.grp_addr,info->grp_addr,16))
				continue;
			info->ori_dev = entry->ppe_multicast_info.ori_dev;
			memcpy(info->src_addr, entry->ppe_multicast_info.src_addr, 16);
			break;
		}
		else
		{
			spin_unlock_bh(&ecnt_multicast_lock);
			return -1;
		}
	}
				
	spin_unlock_bh(&ecnt_multicast_lock);	 

	return 0;
}

static int ecnt_multicast_hwnat_check_grp_same
	(struct FoeEntry *foe_entry, PPE_MULTICAST_HWNATENTRY_t* entry)
{
	PPE_MULTICAST_INFO_t *ppe_multicast_info = &entry->ppe_multicast_info;
	unsigned int grp_addr = 0, foe_dip = 0;
	unsigned int *ptr_dip = NULL;
	int i = 0;

	if(IPV4_HNAPT == foe_entry->bfib1.pkt_type)
	{
		if(PPE_PROTO_IPV4 != ppe_multicast_info->proto)
		{
			goto delete_old_entry;
		}
		grp_addr = (*(unsigned int *)entry->ppe_multicast_info.grp_addr);
		foe_dip = htonl(foe_entry->ipv4_hnapt.dip);
		if(grp_addr != foe_dip)
		{
			goto delete_old_entry;
		}
	}
	else if(IPV6_5T_ROUTE == foe_entry->bfib1.pkt_type)
	{
		if(PPE_PROTO_IPV6 != ppe_multicast_info->proto)
		{
			goto delete_old_entry;
		}
		ptr_dip = (unsigned int *)(&foe_entry->ipv6_5t_route.ipv6_dip0);
		for(i=0; i<4; i++)
		{
			grp_addr = (*(unsigned int *)&entry->ppe_multicast_info.grp_addr[i*4]);
			foe_dip = htonl(*ptr_dip);
			if(grp_addr != foe_dip)
			{
				goto delete_old_entry;
			}
			ptr_dip++;
		}
	}

	return 1;
	
delete_old_entry:	
	del_timer(&entry->age_timer);
	list_del(&entry->list); 
	kfree(entry);	
	return 0;
}

int ecnt_multicast_hwnat_learn_flow(struct sk_buff* skb)
{
	unsigned int foe_index = FOE_ENTRY_NUM(skb);	
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;
	unsigned int port_mask = 0, local = 0;
		
	spin_lock(&ecnt_multicast_lock);
	list_for_each_entry(entry,&ecnt_multicast_flow,list)
	{
		if(entry->foe_index == foe_index)
		{
			struct FoeEntry *foe_entry = getPpeEntry(foe_index);
			if(foe_entry && 0 == ecnt_multicast_hwnat_check_grp_same(foe_entry, entry))
				break;
			if(TCSUPPORT_CT_VAL)
				entry->ppe_multicast_info.ori_dev = skb->orig_dev;
			else
				entry->ppe_multicast_info.ori_dev = skb->original_dev;
			entry->ppe_multicast_info.dst_dev = skb->dev;
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
			if(ra_sw_nat_get_mul_br_vid_hook)
				entry->ppe_multicast_info.br_vid = ra_sw_nat_get_mul_br_vid_hook(skb);
#endif
			PPE_API_MULTICAST_HWNAT_GET_PORTMASK(&(entry->ppe_multicast_info),foe_index,&port_mask);
			if(TCSUPPORT_CT_VAL)
			{
				checkPortMask(entry,&port_mask);
			}
			entry->port_mask = port_mask;

            local = ecnt_multicast_hwnat_get_local(&(entry->ppe_multicast_info));
			entry->local = local;
			ecnt_multicast_hwnat_state_handler(foe_index,port_mask,local,entry->ppe_multicast_info.ext_port_num);
			entry->state = PPE_MULTICAST_HWNATENTRY_STATE_BINDED;
		}
	}
	spin_unlock(&ecnt_multicast_lock);
	
	return ECNT_CONTINUE;
}

int ecnt_multicast_hwnat_drop_flow(struct sk_buff* skb)
{
	unsigned int foe_index = FOE_ENTRY_NUM(skb);	
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;

	spin_lock(&ecnt_multicast_lock);
	list_for_each_entry(entry,&ecnt_multicast_flow,list)
	{
		if(entry->foe_index == foe_index)
		{
			entry->port_mask = 0;
            entry->local = 0;
			entry->state = PPE_MULTICAST_HWNATENTRY_STATE_DROP;
		}
		
	}
	spin_unlock(&ecnt_multicast_lock);
	
	return 0;
}

void ecnt_multicast_hwnat_list_timer_timeout(TIMER_FUN_PAAM arg)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	PPE_MULTICAST_HWNATENTRY_t* entry = (PPE_MULTICAST_HWNATENTRY_t* )arg;
#else
	PPE_MULTICAST_HWNATENTRY_t *entry = from_timer(entry, arg, age_timer);
#endif

	struct FoeEntry *foe_entry = NULL;

	if(!entry)
	{
		printk("%s:%d\n",__FUNCTION__,__LINE__);
		return;
	}

	foe_entry = getPpeEntry(entry->foe_index);
	
	if(!foe_entry)
	{
		printk("%s:%d\n",__FUNCTION__,__LINE__);
		return;
	}

	foe_entry = getPpeEntry(entry->foe_index);

	if(!foe_entry)
		return;

	if (foe_entry->bfib1.state == BIND) 
	{
		mod_timer(&entry->age_timer,jiffies + PPE_MULTICAST_HWNATENTRY_AGE_TIME);
		return;
	}

	
	spin_lock_bh(&ecnt_multicast_lock);
	del_timer(&entry->age_timer);
	list_del(&entry->list);	
	spin_unlock_bh(&ecnt_multicast_lock);
	kfree(entry);

	return;
}

int ppe_get_skb_vlan(struct sk_buff* skb,unsigned int* vlan_tag_num,unsigned short* outer_vlan,unsigned short* inner_vlan)
{
	int tag_num = ppe_get_vlan_tag_num(skb);

	*vlan_tag_num = tag_num;

	if(tag_num > 0)
	{
		*outer_vlan = htons(*(unsigned short*)(skb->data+14));
		if(tag_num > 1)
			*inner_vlan = htons(*(unsigned short*)(skb->data+18));
	}
	
	return 0;
}

#ifdef TCSUPPORT_OPENWRT
/* start of drop multicast*/
int skb_is_multicastudp(struct sk_buff* skb)
{
	char* buff = NULL;
	unsigned short ethtype = 0;
	struct iphdr*  ip = NULL;
	struct ipv6hdr* ipv6 = NULL;
	if (vlan_eth_hdr(skb)->h_vlan_proto == htons(ETH_P_8021Q))
	{
		ethtype = vlan_eth_hdr(skb)->h_vlan_encapsulated_proto;
		buff = skb->data + 4;
	}else
	{
		ethtype=eth_hdr(skb)->h_proto;
		buff = skb->data;
	}
	if(ethtype == htons(ETH_P_IP))
	{
		ip = (struct iphdr*)(buff);
		if(!ipv4_is_multicast(ip->daddr))
			return 0;
		if(ip->protocol != IPPROTO_UDP)
			return 0;
		return 1;
	}else if(ethtype == htons(ETH_P_IPV6))
	{
		ipv6 = (struct ipv6hdr*)(buff);
		if(!ipv6_addr_is_multicast(&ipv6->daddr))
			return 0;
		if(ipv6->nexthdr != IPPROTO_UDP)
			return 0;
		return 1;
	}
	return 0;
}

inline int ecnt_has_multiwan_dev(struct net_device *dev)
{
	return !!(dev->priv_flags & IFF_RSMUX);
}

inline int ecnt_vlan_exist(struct sk_buff* skb)
{
	return (vlan_eth_hdr(skb)->h_vlan_proto == htons(ETH_P_8021Q));
}

struct net_device* ecnt_get_vlandev(struct sk_buff* skb)
{
	struct net_device* dev = NULL;
	char dev_name[IFNAMSIZ] = {0};
	struct vlan_ethhdr *vlan_eth = NULL;
	
	vlan_eth = vlan_eth_hdr(skb);
	if(!skb->dev || !vlan_eth){
		return NULL;
	}

	dev = ecnt_vlan_find_dev(skb->dev, vlan_eth->h_vlan_proto, ntohs(vlan_eth->h_vlan_TCI) & VLAN_VID_MASK);
	
	return dev;
}

int ecnt_multicast_should_deliver(struct sk_buff* skb, struct net_device* dev)
{	char* buff = NULL;
	int ret = 0;
	unsigned short ethtype = 0;
	struct iphdr*  ip = NULL;
	struct ipv6hdr* ipv6 = NULL;
	struct in_device* in_dev = NULL;

	if (vlan_eth_hdr(skb)->h_vlan_proto == htons(ETH_P_8021Q))
	{
		ethtype = vlan_eth_hdr(skb)->h_vlan_encapsulated_proto;
		buff = skb->data + 4;
	}else
	{
		ethtype=eth_hdr(skb)->h_proto;
		buff = skb->data;
	}
	if(ethtype == htons(ETH_P_IP))
	{
		ip = (struct iphdr*)(buff);
		in_dev = (struct in_device*)dev->ip_ptr;
	
		if (in_dev==NULL)
			return 0;

		rcu_read_lock();
		ret = ip_check_mc_rcu(in_dev,ip->daddr,0,ip->protocol);
		rcu_read_unlock();
		return ret;
	}
#if CONFIG_IPV6
	else if (ethtype == htons(ETH_P_IPV6))
	{
		ipv6 = (struct ipv6hdr*)(buff);
		return ipv6_chk_mcast_addr(dev,&ipv6->daddr,NULL);
	}
#endif	

	return 1;
}
inline int ecnt_dev_is_under_bridge(struct sk_buff* skb, struct net_device* dev)
{
	return (dev->rx_handler != NULL);
}

int ecnt_multicast_hwnat_drop(struct sk_buff* skb)
{
	struct net_device *dev = skb->dev;
    struct net_device *dev_vlan = NULL;
	
	//multicast,udp
	if(!skb_is_multicastudp(skb))
		return 1;
	if(ecnt_has_multiwan_dev(dev))
		return 1;

	if(ecnt_vlan_exist(skb)){
		dev_vlan = ecnt_get_vlandev(skb);
        if(!dev_vlan)
        {
            if(ecnt_dev_is_under_bridge(skb, dev))
                return 1;
            if(!dev->vlan_info){
                PpeDropPacketHandler(skb);
                return 0;
            }
        }
        else{
            dev = dev_vlan;
        }
    }

	if(ecnt_dev_is_under_bridge(skb, dev))
		return 1;

	if(ecnt_multicast_should_deliver(skb,dev))
		return 1;

	PpeDropPacketHandler(skb);
	return 0;
}
/* start of drop multicast*/
#endif /* TCSUPPORT_OPENWRT */

int ecnt_multicast_hwnat_list_add(struct sk_buff* skb,struct FoeEntry *foe_entry)
{
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;
	PPE_MULTICAST_INFO_t mcast_info;
	unsigned int ip_tmp = 0;

	memset(&mcast_info,0,sizeof(PPE_MULTICAST_INFO_t));
	
	ppe_get_skb_vlan(skb,&mcast_info.vlan_tag_num,&mcast_info.outer_tci,&mcast_info.inner_tci);

	if(IS_IPV4_GRP(foe_entry))
	{
		mcast_info.proto = PPE_PROTO_IPV4;
		ip_tmp = htonl(foe_entry->ipv4_hnapt.sip);
		memmove(mcast_info.src_addr,&ip_tmp,4);
		ip_tmp = htonl(foe_entry->ipv4_hnapt.dip);
		memmove(mcast_info.grp_addr,&ip_tmp,4);
	}
	else
	{
		mcast_info.proto = PPE_PROTO_IPV6;
		ip_tmp = htonl(foe_entry->ipv6_5t_route.ipv6_sip0);
		memmove(mcast_info.src_addr,&ip_tmp,4);
		ip_tmp = htonl(foe_entry->ipv6_5t_route.ipv6_sip1);
		memmove(mcast_info.src_addr+4,&ip_tmp,4);		
		ip_tmp = htonl(foe_entry->ipv6_5t_route.ipv6_sip2);
		memmove(mcast_info.src_addr+8,&ip_tmp,4);		
		ip_tmp = htonl(foe_entry->ipv6_5t_route.ipv6_sip3);
		memmove(mcast_info.src_addr+12,&ip_tmp,4);
		ip_tmp = htonl(foe_entry->ipv6_5t_route.ipv6_dip0);
		memmove(mcast_info.grp_addr,&ip_tmp,4);
		ip_tmp = htonl(foe_entry->ipv6_5t_route.ipv6_dip1);
		memmove(mcast_info.grp_addr+4,&ip_tmp,4);	
		ip_tmp = htonl(foe_entry->ipv6_5t_route.ipv6_dip2);
		memmove(mcast_info.grp_addr+8,&ip_tmp,4);	
		ip_tmp = htonl(foe_entry->ipv6_5t_route.ipv6_dip3);
		memmove(mcast_info.grp_addr+12,&ip_tmp,4);
	}

	entry = (PPE_MULTICAST_HWNATENTRY_t* )kmalloc(sizeof(PPE_MULTICAST_HWNATENTRY_t),GFP_ATOMIC);
	
	if(!entry)
		return -1;

	memset(entry,0,sizeof(PPE_MULTICAST_HWNATENTRY_t));
	entry->foe_index = FOE_ENTRY_NUM(skb);
	memmove(&(entry->ppe_multicast_info),&mcast_info,sizeof(PPE_MULTICAST_INFO_t));

	list_add_tail(&entry->list,&ecnt_multicast_flow);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	setup_timer(&entry->age_timer, ecnt_multicast_hwnat_list_timer_timeout, (unsigned long)entry);
#else
	timer_setup(&entry->age_timer, ecnt_multicast_hwnat_list_timer_timeout, 0);
#endif
	mod_timer(&entry->age_timer,jiffies + PPE_MULTICAST_HWNATENTRY_AGE_TIME);
	
	return 0;
}
#define ONUTYPE_MASK 0x03 //bit[1:0]
int ecnt_multicast_drop(struct sk_buff *skb)
{
#ifdef TCSUPPORT_OPENWRT
    if ((get_onutype() & ONUTYPE_MASK) == 0x2) //0x1:SFU, 0x2:HGU.
	{
        if(ecnt_multicast_hwnat_drop(skb) == 0)
		{
			dev_kfree_skb_any(skb);
			return 0;
		}
    }
#endif
	return 1;
}    

int ecnt_multicast_hwnat_data_handler(struct sk_buff* skb)
{		
	unsigned int foe_index = FOE_ENTRY_NUM(skb);
	struct FoeEntry *foe_entry = getPpeEntry(foe_index);
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;

	if(!ppe_is_multicast_entry(foe_entry))
		return 1;
	
	skb_push(skb,ETH_HLEN);
	
	spin_lock_bh(&ecnt_multicast_lock);
	list_for_each_entry(entry,&ecnt_multicast_flow,list)
	{
		if(entry->foe_index == foe_index)
		{
			if(0 == ecnt_multicast_hwnat_check_grp_same(foe_entry, entry))
				break;
			spin_unlock_bh(&ecnt_multicast_lock);			
			skb_pull(skb,ETH_HLEN);
			return 1;
		}
	}

	ecnt_multicast_hwnat_list_add(skb,foe_entry);	
	spin_unlock_bh(&ecnt_multicast_lock);

	skb_pull(skb,ETH_HLEN);

	return 1;
}

int ecnt_multicast_hwnat_clear_all_entry(void)
{				
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;	
	PPE_MULTICAST_HWNATENTRY_t* tmp = NULL;
	
	spin_lock_bh(&ecnt_multicast_lock);
	list_for_each_entry_safe(entry,tmp,&ecnt_multicast_flow,list)
	{
		hwnat_delete_foe_entry(entry->foe_index);
		if(try_to_del_timer_sync(&entry->age_timer) >= 0)
		{
			list_del(&entry->list);	
			kfree(entry);
		}	
	}

	list_for_each_entry_safe(entry,tmp,&ecnt_multicast_local_flow,list)
	{
		list_del(&entry->list);	
		kfree(entry);	
	}
	spin_unlock_bh(&ecnt_multicast_lock);

	return 0;
}

void ecnt_multicast_hwnat_get_mask_and_num(PPE_MULTICAST_HWNATENTRY_t* entry, 
	unsigned int *mask, unsigned int *num)
{
	int i = 0;
	unsigned int mask_tmp = 0, num_tmp = 0;

	mask_tmp = entry->port_mask&((HWNAT_WLAN_IF_MASK<<HWNAT_WLAN_IF_BASE)|(HWNAT_USB_IF_MASK<<HWNAT_USB_IF_BASE));
	for(i = HWNAT_WLAN_IF_BASE;i < HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_MAXNUM; i++)
	{
		if(mask_tmp & (1 << i))
			num_tmp++;
	}
    
	for(i = HWNAT_USB_IF_BASE;i < HWNAT_USB_IF_BASE+HWNAT_USB_IF_NUM; i++)
	{
		if(mask_tmp & (1 << i))
			num_tmp++;
	}

	/* send to xsi interface, only when both with LAN */
	if((entry->port_mask & (HWNAT_XSI_IF_MASK << HWNAT_XSI_IF_BASE))
		&& (entry->port_mask & (HWNAT_LAN_IF_MASK<<HWNAT_LAN_IF_BASE)))
	{	
		mask_tmp |= (HWNAT_XSI_IF_MASK << HWNAT_XSI_IF_BASE);
		num_tmp++;
	}
		
	*mask = mask_tmp;
	*num = num_tmp;

	return;
}

int ecnt_multicast_hwnat_sendto_usb_interface(struct sk_buff* skb, unsigned int mask)
{
	int i = 0;
	struct sk_buff* skb2 = NULL;

	for(i = HWNAT_USB_IF_BASE;i < HWNAT_USB_IF_BASE+HWNAT_USB_IF_NUM; i++)
	{
		if(mask & (1 << i))
		{
			skb2 = skb_clone(skb, GFP_ATOMIC);
			if(!skb2)
				continue;

			skb2->dev = ppeGetDstPort(DP_USBNET0+(i-HWNAT_USB_IF_BASE));

			if (skb2->dev != NULL && (skb2->dev->flags & IFF_UP))
			{
				skb2->dev->netdev_ops->ndo_start_xmit(skb2,skb2->dev);
			}
			else
			{
				kfree_skb(skb2);
				continue;
			}
		}
	}

	return 0;
}

int ecnt_multicast_hwnat_force_to_cpu_handler(struct sk_buff* skb,unsigned int dp, unsigned int dsoffload, struct FoeEntry *foe_entry)
{
	unsigned int wifi_mask = 0,wifi_num = 0, local = 0;
	int i = 0;
	int ext_port_num = 0;
	struct net_device* ext_port[MC_EXT_PORT_MAX_NUM];
	struct sk_buff* skb2 = NULL;
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;
	IGMP_HWNATEntry_t* ms_stats_p = NULL;
	unsigned int foe_index = FOE_ENTRY_NUM(skb);
    struct net_device *orig_dev = NULL;
	
	if(((FOE_AI(skb) == HIT_BIND_MUL_CPUR)&&TCSUPPORT_DS_HWNAT_OFFLOAD_VAL&&dsoffload_presetting) || TCSUPPORT_VLAN_ACCESS_TRUNK_VAL)
		return 1;

	if (multicast_speed_find_entry_hook) {
		ms_stats_p = (IGMP_HWNATEntry_t *)multicast_speed_find_entry_hook(FOE_ENTRY_NUM(skb));
		if(ms_stats_p) {
			wifi_mask = ms_stats_p->mask;
			wifi_num = ms_stats_p->wifinum;
		}
	}
	else
	{
		spin_lock_bh(&ecnt_multicast_lock);
		list_for_each_entry(entry,&ecnt_multicast_flow,list)
		{
			if(entry->foe_index == foe_index)
			{
				ecnt_multicast_hwnat_get_mask_and_num(entry, &wifi_mask, &wifi_num);
 				local = entry->local;
				orig_dev = entry->ppe_multicast_info.ori_dev;
				ext_port_num = entry->ppe_multicast_info.ext_port_num;
				for(i=0; i<ext_port_num;i++)
				{
					ext_port[i] = entry->ppe_multicast_info.ext_port[i];
				}
				break;
			}
		}
		spin_unlock_bh(&ecnt_multicast_lock);

        if (local && (IPV4_HNAPT == foe_entry->bfib1.pkt_type) 
            && (foe_entry->ipv4_hnapt.dip > 0xe0000000) && ((foe_entry->ipv4_hnapt.info_blk1 >> 30) & 0x1))
        {
            skb2 = skb_clone(skb, GFP_ATOMIC);
            if(!skb2)
                goto free_skb;
			
			if(orig_dev == NULL){
				orig_dev = foe_ext[foe_index].dev_info;
			}
            skb2->dev = orig_dev;
            skb2->protocol = htons(ETH_P_IP);
            localInShortCut(skb2, 1);
        }
		
		if(ext_port_num)
		{
			if(wifi_num > 0)
			{
				for(i=0; i<ext_port_num;i++)
				{
					skb2 = skb_copy(skb, GFP_ATOMIC);
            		if(!skb2)
                		continue;
					skb2->dev = ext_port[i];
					if ((skb2->dev != NULL) && (skb2->dev->flags & IFF_UP))
					{
						skb_push(skb2, ETH_HLEN);
						skb2->dev->netdev_ops->ndo_start_xmit(skb2,skb2->dev);
					}
					else
					{
						kfree_skb(skb2);
						continue;
					}
				}
			}
			else
			{
				skb_push(skb, ETH_HLEN);
				for(i=1; i<ext_port_num;i++)
				{
					skb2 = skb_copy(skb, GFP_ATOMIC);
            		if(!skb2)
                		continue;
					skb2->dev = ext_port[i];
					if ((skb2->dev != NULL) && (skb2->dev->flags & IFF_UP))
					{
						skb2->dev->netdev_ops->ndo_start_xmit(skb2,skb2->dev);
					}
					else
					{
						kfree_skb(skb2);
						continue;
					}
				}
				skb->dev = ext_port[0];
				if ((skb->dev != NULL) && (skb->dev->flags & IFF_UP))
				{
					skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
				}
				else
				{
					kfree_skb(skb);
				}				
				return 0;
			}
		}
	}

	if(0 == wifi_num)
		goto free_skb;
		
	/*if send to only one wifi interface,call wlan handler*/
	if(1 == wifi_num)
	{	
		if(wifi_mask & (HWNAT_USB_IF_MASK<<HWNAT_USB_IF_BASE))
			return ecnt_offload_default_offload(skb,dp,dsoffload,foe_entry);
		else if(wifi_mask & (HWNAT_XSI_IF_MASK << HWNAT_XSI_IF_BASE))
			return ecnt_offload_default_offload(skb,dp,dsoffload,foe_entry);
		else
			return ecnt_wlan_force_to_cpu_handler(skb,dp,dsoffload);
	}

	skb_set_network_header(skb, 0);

	skb_push(skb, ETH_HLEN);	//pointer to layer2 header

	/*send to multiple wifi interface*/
	for(i = HWNAT_WLAN_IF_BASE;i < HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_MAXNUM; i++)
	{
		if(wifi_mask & (1 << i))
		{
			skb2 = skb_clone(skb, GFP_ATOMIC);
			if(!skb2)
				goto free_skb;

			ecnt_multicast_hwnat_set_wlan_dev(i,skb2);

			if ((skb2->dev != NULL) && (skb2->dev->flags & IFF_UP))
			{
				remove_skb_data_vlan_tag(skb2);
				skb2->dev->netdev_ops->ndo_start_xmit(skb2,skb2->dev);
			}
			else
			{
				kfree_skb(skb2);
				continue;
			}
		}
	}

	/* this must be last sent, because of skb, not copied skb2 */
	if(wifi_mask & (HWNAT_XSI_IF_MASK<<HWNAT_XSI_IF_BASE))
	{
		skb->dev = ppeGetDstPort(DP_XSI);
		
		if (skb->dev != NULL && (skb->dev->flags & IFF_UP))
		{
			skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
			return 0;
		}
	}

free_skb:
	kfree_skb(skb);
	return 0;
}

int ecnt_multicast_hwnat_stag_handle(struct sk_buff *skb)
{
	uint8 *cp=NULL;
	uint16 sp_tag = 0;
	uint8 vpm = 0;

	cp = skb->data;
	sp_tag = htons(*(uint16 *)(cp+12));

	vpm = (sp_tag & 0x300) >> 8;

	switch(vpm)
	{
		case 0:/*untag*/
			memmove(skb->data+4, skb->data, 12);
			skb_pull(skb, 4);
			break;
			
		case 1:/*8100*/
			*(uint16 *)(cp+12) = ntohs(0x8100);
			break;

		case 2:/*88a8*/
			*(uint16 *)(cp+12) = ntohs(0x88a8);
			break;
			
		default:
			return -1;
	}

	return 0;
}

int ecnt_multicast_hwnat_mul_to_cpu_handler(struct sk_buff *skb, struct FoeEntry *foe_entry)
{
	int type = ppe_is_multicast_entry(foe_entry);
	unsigned int foe_index = FOE_ENTRY_NUM(skb);
	int dp = 0;
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;
	unsigned wifi_mask = 0;
	int i = 0;

	if(!type)
		return 0;

	spin_lock_bh(&ecnt_multicast_lock);
	list_for_each_entry(entry,&ecnt_multicast_flow,list)
	{
		if(entry->foe_index == foe_index)
		{
			wifi_mask = entry->port_mask&(HWNAT_WLAN_IF_MASK<<HWNAT_WLAN_IF_BASE);
			for(i = HWNAT_WLAN_IF_BASE;i < HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_NUM; i++)
			{
				if(wifi_mask & (1 << i))
				{
					dp = DP_RA0+i-HWNAT_WLAN_IF_BASE;/*dp is used when wifi_num=1*/
					break;
				}
			}

			if(0 == dp)
			{
				for(i = HWNAT_WLAN_IF_I_BASE;i < HWNAT_WLAN_IF_I_BASE+HWNAT_WLAN_IF_I_NUM; i++)
				{
					if(wifi_mask & (1 << i))
					{
						dp = DP_RAI0+i-HWNAT_WLAN_IF_I_BASE;/*dp is used when wifi_num=1*/
						break;
					}
				}
			}
#if defined(TCSUPPORT_WLAN_MT7990)
			if(0 == dp)
			{
				for(i = HWNAT_WLAN_IF_I_BASE;i < HWNAT_WLAN_IF_I_BASE+HWNAT_WLAN_IF_I_NUM+HWNAT_WLAN_IF_X_NUM; i++)
				{
					if(wifi_mask & (1 << i))
					{
						dp = DP_RAX0+i-HWNAT_WLAN_IF_I_BASE-HWNAT_WLAN_IF_I_NUM;/*dp is used when wifi_num=1*/
						break;
					}
				}
			}
#endif
			if(0 == dp)
			{
				for(i = HWNAT_USB_IF_BASE;i < HWNAT_USB_IF_BASE+HWNAT_USB_IF_NUM; i++)
				{
					if((entry->port_mask&(HWNAT_USB_IF_MASK<<HWNAT_USB_IF_BASE)) & (1 << i))
					{
						dp = DP_USBNET0+i-HWNAT_USB_IF_BASE;/*dp is used when usb_num=1*/
						break;
					}
				}
			}
			
			if(0 == dp)
			{		  
				if(entry->port_mask & (HWNAT_XSI_IF_MASK << HWNAT_XSI_IF_BASE))
					dp = DP_XSI;
			}
		}
	}
	spin_unlock_bh(&ecnt_multicast_lock);


	if(!SUPPORT_STAG_FROM_INFO)
	{
		skb_push(skb, ETH_HLEN);	
		ecnt_multicast_hwnat_stag_handle(skb);
		skb_pull(skb, ETH_HLEN);	
	}

	return ecnt_multicast_hwnat_force_to_cpu_handler(skb,dp,0, foe_entry);
}

static int enct_multicast_flow_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int i = 1,j = 0;
	PPE_MULTICAST_HWNATENTRY_t* entry = NULL;
	PPE_MULTICAST_LOCALENTRY_t* local_entry = NULL;
	unsigned char out_ip[128];
	unsigned char state[32];

    if (!list_empty(&ecnt_multicast_local_flow))
    {
    	printk("------------------------------------------\n");
    	list_for_each_entry(local_entry,&ecnt_multicast_local_flow,list)
        {
            printk("grp_addr:%d.%d.%d.%d, ref_cnt: %d\n", 
                local_entry->ppe_multicast_info.grp_addr[0], local_entry->ppe_multicast_info.grp_addr[1], 
                local_entry->ppe_multicast_info.grp_addr[2], local_entry->ppe_multicast_info.grp_addr[3], 
                local_entry->ref_cnt);
        }
    }
    
	if(list_empty(&ecnt_multicast_flow))
	{
		printk("no multicast entry\n");
		return 0;
	}
	
	spin_lock_bh(&ecnt_multicast_lock);

	printk("------------------------------------------\n");
	
	list_for_each_entry(entry,&ecnt_multicast_flow,list)
	{
		printk("index:%d\n",i);
		printk("vlan num:%d,outer_tci:0x%x,inner_tci:0x%x\n",entry->ppe_multicast_info.vlan_tag_num,entry->ppe_multicast_info.outer_tci,entry->ppe_multicast_info.inner_tci);		
		printk("proto:%s\n",(entry->ppe_multicast_info.proto==PPE_PROTO_IPV4)?"ipv4":"ipv6");
		memset(out_ip,0,128);
		ppe_trans_str_to_ip_format(entry->ppe_multicast_info.proto,entry->ppe_multicast_info.src_addr,out_ip);
		printk("src_ip:%s\n",out_ip);
		memset(out_ip,0,128);
		ppe_trans_str_to_ip_format(entry->ppe_multicast_info.proto,entry->ppe_multicast_info.grp_addr,out_ip);
		printk("grp_ip:%s\n",out_ip);
		printk("foe index:%d\n",entry->foe_index);
		memset(state,0,32);
		if(PPE_MULTICAST_HWNATENTRY_STATE_DROP == entry->state)
			strcpy(state,"drop");
		else if(PPE_MULTICAST_HWNATENTRY_STATE_BINDED == entry->state)
			strcpy(state,"bind");
		else
			strcpy(state,"unbind");
		printk("state:%s\n",state);
		printk("port_mask:0x%x\n",entry->port_mask);
        printk("ori_dev:%s\n",entry->ppe_multicast_info.ori_dev->name);
		printk("dst_dev:%s\n",entry->ppe_multicast_info.dst_dev->name);
        printk("local:%d\n", entry->local);
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
		printk("br vid:0x%x\n",entry->ppe_multicast_info.br_vid);   
#endif
		if(entry->ppe_multicast_info.ext_port_num)
			printk("extern port list:\n");
		for(j=0; j<entry->ppe_multicast_info.ext_port_num;j++)
			printk("\t%s\n", ((struct net_device*)(entry->ppe_multicast_info.ext_port[j]))->name);
		printk("------------------------------------------\n");
		i++;
	}
	
	spin_unlock_bh(&ecnt_multicast_lock);
	
	return 0;	
}

static int ecnt_multicast_debug_flag_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len = 0;
	len = sprintf(page, "%d\n", ecnt_multicast_debug_flag);
	
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

static int ecnt_multicast_debug_flag_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[32];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	if (sscanf(val_string, "%d", &ecnt_multicast_debug_flag) != 1) {
		printk("usage: 0/1\n");
	}
	
	return count;
}

static int unknow_mc_v6_cnt_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[32];
	unsigned long long mc_byte = 0 ;
	unsigned int mc_pkt = 0 ;
    
	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d %llu",&mc_pkt, &mc_byte);

	unknow_mc_byte_cnt += mc_byte;
	unknow_mc_pkt_cnt += mc_pkt;

	return count;
}

int hwnat_multicast_set_info_for_sfu(int index, int mask)
{
	ecnt_multicast_hwnat_state_handler(index,mask,0,0);
	return 0;
}


void ecnt_hwnat_multicast_init(void)
{	
	struct proc_dir_entry *multicast_proc;

	create_proc_read_entry("tc3162/ecnt_multicast_hwnat_list", 0, NULL, enct_multicast_flow_read_proc, NULL);
	
	multicast_proc = create_proc_entry("tc3162/ecnt_multicast_debug", 0, NULL);
	multicast_proc->read_proc = ecnt_multicast_debug_flag_read_proc;
	multicast_proc->write_proc = ecnt_multicast_debug_flag_write_proc;

	multicast_proc = create_proc_entry("tc3162/unknow_multicast_v6_cnt", 0, NULL);
	multicast_proc->write_proc =  unknow_mc_v6_cnt_write_proc; 

	ra_nat_delete_mc_drop_by_grp = ecnt_multicast_hwnat_del_drop_by_grp; 
}

void ecnt_hwnat_multicast_deinit(void)
{
	ra_nat_delete_mc_drop_by_grp = NULL;
	
	ecnt_multicast_hwnat_clear_all_entry();
	remove_proc_entry("tc3162/ecnt_multicast_hwnat_list",NULL);
	remove_proc_entry("tc3162/ecnt_multicast_debug",NULL);
	remove_proc_entry("tc3162/unknow_multicast_v6_cnt", NULL);
}

