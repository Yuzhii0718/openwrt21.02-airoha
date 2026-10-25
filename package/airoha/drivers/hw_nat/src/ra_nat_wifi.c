#include <linux/spinlock.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/libcompileoption.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <net/ipv6.h>
#include <net/ip.h>
#include <linux/if_pppox.h>
#include <linux/ppp_defs.h>
#include <linux/proc_fs.h>
#include <net/gre.h>
#include <linux/if_tunnel.h>

#include <ecnt_hook/ecnt_hook_net.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_asym.h>
#include <ecnt_hook/ecnt_hook_crypto.h>

#include "ra_nat_ety.h"
#include "ra_nat_gen.h"
#include "ra_nat_cust.h"
#include "ra_nat_offload.h"
#include "ra_nat_dev.h"
#include "util.h"
#include "foe_fdb.h"
#include "ppe_api.h"
#include <linux/foe_hook.h>
#include "ra_nat_ic_dis.h"

extern void PpeSetRxForceRingIdx(struct FoeEntry * foe_entry, uint8_t ring_index);
extern void PpeSetInfoBlk2(struct FoeEntry * foe_entry, uint32_t fpidx, uint32_t port_mg, uint32_t port_ag, uint32_t ds_to_qdma);
extern struct net_device* ppeGetDstPort(unsigned int dp);
int wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM*WLAN_BAND_NUM];
extern int DebugLevel;
extern struct FoeEntryExt*  foe_ext;

extern unsigned int hwnat_dram_idx_base;

extern int PpeCleanCache751221(void);

int IsWifiInfDown(struct sk_buff * skb,int band)
{
   if(band == 0){//2g
		if(is24GWiFiInterface(skb->dev))
		{
			if(wifiInfDown[get24GWifiIndex(skb->dev)]){
			   if (DebugLevel >= 1) {
				   printk("(%s)wifiInfDown[%d] is 1\n", __func__, get24GWifiIndex(skb->dev));
			   }			   
						   return 1;
			}
		}
		else if(is24GWDSInterface(skb->dev))
		{
			if(wifiInfDown[SINGLE_WLAN_SSID_NUM*2+get24GWDSIndex(skb->dev)]){
			   if (DebugLevel >= 1) {
				   printk("(%s)wifiInfDown[%d] is 1\n", __func__, SINGLE_WLAN_SSID_NUM*2+get24GWDSIndex(skb->dev));
			   }			   
						   return 1;
			}
		}
		else if(is24GAPCLIInterface(skb->dev))
		{
			if(wifiInfDown[SINGLE_WLAN_SSID_NUM*2+SINGLE_WDS_NUM*2]){
			   if (DebugLevel >= 1) {
				   printk("(%s)wifiInfDown[%d] is 1\n", __func__, SINGLE_WLAN_SSID_NUM*2+SINGLE_WDS_NUM*2);
			   }			   
						   return 1;
			}
		}
		
   }
   else  if(band == 1){//5g
	   	if(is5GWiFiInterface(skb->dev))
	   	{
			if(wifiInfDown[SINGLE_WLAN_SSID_NUM+get5GWifiIndex(skb->dev)]){
				   if (DebugLevel >= 1) {
					   printk("(%s)wifiInfDown[%d] is 1\n", __func__, SINGLE_WLAN_SSID_NUM+get5GWifiIndex(skb->dev));
				   }			   
				   return 1;   
			}
	   	}
		else if(is5GWDSInterface(skb->dev))
	   	{
			if(wifiInfDown[SINGLE_WLAN_SSID_NUM*2+SINGLE_WDS_NUM+get5GWDSIndex(skb->dev)]){
				   if (DebugLevel >= 1) {
					   printk("(%s)wifiInfDown[%d] is 1\n", __func__, SINGLE_WLAN_SSID_NUM*2+SINGLE_WDS_NUM+get5GWDSIndex(skb->dev));
				   }			   
				   return 1;   
			}
	   	}
		else if(is5GAPCLIInterface(skb->dev))
		{
			if(wifiInfDown[SINGLE_WLAN_SSID_NUM*2+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM]){
			   if (DebugLevel >= 1) {
				   printk("(%s)wifiInfDown[%d] is 1\n", __func__, SINGLE_WLAN_SSID_NUM*2+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM);
			   }			   
						   return 1;
			}
		}
   }
   return 0;
}

int WifiInfUpHandle(struct net_device *dev)
{
	if (is24GWiFiInterface(dev) &&
#if defined(TCSUPPORT_WLAN_UNION_ITF)
		((get24GWifiIndex(dev) >= 0) && (get24GWifiIndex(dev) <= 15))) 
#else
		((get24GWifiIndex(dev) >= 0) && (get24GWifiIndex(dev) <= SINGLE_WLAN_SSID_NUM - 1))) 
#endif
	{
		if (wifiInfDown[get24GWifiIndex(dev)] == 1){
			wifiInfDown[get24GWifiIndex(dev)] = 0;			
		}
	}
	if (is5GWiFiInterface(dev) &&
		((get5GWifiIndex(dev) >= 0) && (get5GWifiIndex(dev) <= SINGLE_WLAN_SSID_NUM - 1))) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM+get5GWifiIndex(dev)] == 1){
			wifiInfDown[SINGLE_WLAN_SSID_NUM+get5GWifiIndex(dev)] = 0;
		}
	} 
	if (is6GWiFiInterface(dev) &&
		((get6GWifiIndex(dev) >= 0) && (get6GWifiIndex(dev) <= SINGLE_WLAN_SSID_NUM - 1))) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM * 2 + get6GWifiIndex(dev)] == 1){
			wifiInfDown[SINGLE_WLAN_SSID_NUM * 2 + get6GWifiIndex(dev)] = 0;
		}
	} 
	
	if (is24GWDSInterface(dev) &&
		((get24GWDSIndex(dev) >= 0) && (get24GWDSIndex(dev) <= 3))) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+get24GWDSIndex(dev)] == 1){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+get24GWDSIndex(dev)] = 0;
		}
	} 

	if (is5GWDSInterface(dev) &&
		((get5GWDSIndex(dev) >= 0) && (get5GWDSIndex(dev) <= 3))) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM+get5GWDSIndex(dev)] == 1){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM+get5GWDSIndex(dev)] = 0;
		}
	} 

	if (is24GAPCLIInterface(dev)) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2] == 1){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2] = 0;
		}
	} 

	if (is5GAPCLIInterface(dev)) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM] == 1){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM] = 0;
		}
	} 
	if (is6GAPCLIInterface(dev)) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM*(WLAN_BAND_NUM - 1)] == 1){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM*(WLAN_BAND_NUM - 1)] = 0;
		}
	} 
	return 0;
}



int hwnat_clean_wifi_inf_entry(int index)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	unsigned int idx;
	int flag = 0;
	int clean_cnt = 0;

	
	for (idx = 0; idx < FOE_4TB_SIZ_TOTAL; idx++)
	{
		if((SUPPORT_SRAM_MAX_512) && (idx >= FOE_4TB_SIZ_SRAM) && (idx < hwnat_dram_idx_base))
			continue;
		
		foe_entry=&foe_entry_local;
		foe_entry = getPpeEntryLocal(idx, foe_entry);
		if(foe_entry == NULL)
			continue;

		if (foe_entry->bfib1.state != BIND)
			continue;
		
		if (foe_ext[idx].dp == index) {
			flag = 1;
			memset(foe_entry,0,sizeof(struct FoeEntry));
			clean_cnt++;
			if(SUPPORT_SRAM_ENTRY && (idx<hwnat_dram_idx_base)) {
				setPpeEntry(idx, foe_entry);
			}
		}
	}

	if(clean_cnt != 0)
		printk("\r\n clean wifi entry,clean_cnt=%d\n",clean_cnt);
	
	if (flag == 0)
		return 0;

	if(!PPE_CACHE_NOT_SUPPORT) {
		if (isEN751627){
			PpeCleanCache751221();
		}else{
			// cache disable
			RegModifyBits(CAH_CTRL, 0, 0, 1);
					
			//clear cache table before enabling cache
			RegModifyBits(CAH_CTRL, 1, 9, 1);
			RegModifyBits(CAH_CTRL, 0, 9, 1);
					
			// cache enable
			RegModifyBits(CAH_CTRL, 1, 0, 1);
		}
	}
	
	return 0;
}
 
int WifiInfDownHandle(struct net_device *dev)
{
	if (is24GWiFiInterface(dev) &&
#if defined(TCSUPPORT_WLAN_UNION_ITF)
		((get24GWifiIndex(dev) >= 0) && (get24GWifiIndex(dev) <= 15))) 
#else
		((get24GWifiIndex(dev) >= 0) && (get24GWifiIndex(dev) <= SINGLE_WLAN_SSID_NUM - 1))) 
#endif
	{
		if (wifiInfDown[get24GWifiIndex(dev)] == 0){
			wifiInfDown[get24GWifiIndex(dev)] = 1;
			hwnat_clean_wifi_inf_entry(DP_RA0 + get24GWifiIndex(dev));				
		}
	}
	if (is5GWiFiInterface(dev) &&
		((get5GWifiIndex(dev) >= 0) && (get5GWifiIndex(dev) <= SINGLE_WLAN_SSID_NUM - 1))) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM+get5GWifiIndex(dev)] == 0){
			wifiInfDown[SINGLE_WLAN_SSID_NUM+get5GWifiIndex(dev)] = 1;
			hwnat_clean_wifi_inf_entry(DP_RAI0 + get5GWifiIndex(dev));
		}
	} 
	if (is6GWiFiInterface(dev) &&
		((get6GWifiIndex(dev) >= 0) && (get6GWifiIndex(dev) <= SINGLE_WLAN_SSID_NUM - 1))) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*2+get6GWifiIndex(dev)] == 0){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*2+get6GWifiIndex(dev)] = 1;
			hwnat_clean_wifi_inf_entry(DP_RAX0 + get6GWifiIndex(dev));
		}
	} 
	
	if (is24GWDSInterface(dev) &&
		((get24GWDSIndex(dev) >= 0) && (get24GWDSIndex(dev) <= 3))) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+get24GWDSIndex(dev)] == 0){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+get24GWDSIndex(dev)] = 1;
			hwnat_clean_wifi_inf_entry(DP_WDS0 + get24GWDSIndex(dev));
		}
	} 
	
	if (is5GWDSInterface(dev) &&
			((get5GWDSIndex(dev) >= 0) && (get5GWDSIndex(dev) <= 3))) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM+get5GWDSIndex(dev)] == 0){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM+get5GWDSIndex(dev)] = 1;
			hwnat_clean_wifi_inf_entry(DP_WDSI0 + get5GWDSIndex(dev));
		}
	} 

	
	if (is24GAPCLIInterface(dev)) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2] == 0){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2] = 1;
			hwnat_clean_wifi_inf_entry(DP_APCLI0);
		}
	} 
	
	if (is5GAPCLIInterface(dev)) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM] == 0){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM] = 1;
			hwnat_clean_wifi_inf_entry(DP_APCLII0);
		}
	}

	if (is6GAPCLIInterface(dev)) 
	{
		if (wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM*(WLAN_BAND_NUM - 1)] == 0){
			wifiInfDown[SINGLE_WLAN_SSID_NUM*WLAN_BAND_NUM+SINGLE_WDS_NUM*2+SINGLE_APCLI_NUM*(WLAN_BAND_NUM - 1)] = 1;
			hwnat_clean_wifi_inf_entry(DP_APCLIX0);
		}
	}

	
	return 0;
}

int ecnt_multicast_hwnat_state_handler_wlan_only(unsigned int foe_index,unsigned int  port_mask)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	int i = 0;
	int dp = 0;

	foe_entry = getPpeEntryLocal(foe_index, foe_entry);
	if(foe_entry == NULL)
		return -1;

	for(i = HWNAT_WLAN_IF_BASE;i < (HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_NUM);i++)
	{
		if(port_mask&(1<<i))
		{
			dp = DP_RA0+(i -HWNAT_WLAN_IF_BASE);
			break;
		}
	}

	if(0 == dp)
	{
		for(i = HWNAT_WLAN_IF_I_BASE;i < (HWNAT_WLAN_IF_I_BASE+HWNAT_WLAN_IF_I_NUM);i++)
		{
			if(port_mask&(1<<i))
			{
				dp = DP_RAI0+(i -HWNAT_WLAN_IF_I_BASE);
				break;
			}
		}
	}
#if defined(TCSUPPORT_WLAN_MT7990)
	if(0 == dp)
	{
		for(i = HWNAT_WLAN_IF_I_BASE;i < (HWNAT_WLAN_IF_I_BASE+HWNAT_WLAN_IF_I_NUM+HWNAT_WLAN_IF_X_NUM);i++)
		{
			if(port_mask&(1<<i))
			{
				dp = DP_RAX0+(i -HWNAT_WLAN_IF_I_BASE -HWNAT_WLAN_IF_I_NUM);
				break;
			}
		}
	}
#endif
	if(0 == dp)
	{
		for(i = HWNAT_USB_IF_BASE;i < HWNAT_USB_IF_BASE+HWNAT_USB_IF_NUM; i++)
		{
			if(port_mask & (1<<i))
			{
				dp = DP_USBNET0+i-HWNAT_USB_IF_BASE;
				break;
			}
		} 
	}

	PpeSetRxForceRingIdx(foe_entry, RING_IDX_1);
	
	if(IS_IPV4_GRP(foe_entry))
	{
		foe_entry->ipv4_hnapt.channel = 1;
		foe_entry->ipv4_hnapt.qid = RING_IDX_1;
	    foe_entry->ipv4_hnapt.etype = 0;
	    foe_entry->ipv4_hnapt.fqos = 0;
		//set mcast and act_dp
		foe_entry->ipv4_hnapt.act_dp = dp;
		foe_entry->ipv4_hnapt.mcast = 0;
		foe_entry->ipv4_hnapt.bfib1.vlan_layer = 0x0;
	}
	else if(IS_IPV6_GRP(foe_entry))
	{
		foe_entry->ipv6_5t_route.channel = 1;
		foe_entry->ipv6_5t_route.qid = RING_IDX_1;
	    foe_entry->ipv6_5t_route.etype = 0;
	    foe_entry->ipv6_5t_route.fqos = 0;
		//set mcast and act_dp
		foe_entry->ipv6_5t_route.act_dp = dp;
		foe_entry->ipv6_5t_route.mcast = 0;
		foe_entry->ipv6_5t_route.bfib1.vlan_layer = 0x0;
	}

	PpeSetInfoBlk2(foe_entry, FP_PDMA, 0x3F, 0x3F, 1);

	if(SUPPORT_SRAM_ENTRY && (foe_index<DRAM_HASH_IDX_BASE)) 
	{
		setPpeEntry(foe_index, foe_entry);
	}
	
	return 0;
}

void ecnt_multicast_hwnat_set_wlan_dev(unsigned long maskindex,struct sk_buff *skb)
{
	if(maskindex >= HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_NUM+HWNAT_WLAN_IF_I_NUM)
	{
		skb->dev = ppeGetDstPort(DP_RAX0+(maskindex-HWNAT_WLAN_IF_BASE-HWNAT_WLAN_IF_NUM-HWNAT_WLAN_IF_I_NUM));
	}
	else if(maskindex >= HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_NUM)
	{
		skb->dev = ppeGetDstPort(DP_RAI0+(maskindex-HWNAT_WLAN_IF_BASE-HWNAT_WLAN_IF_NUM));
	}
	else
	{
		skb->dev = ppeGetDstPort(DP_RA0+(maskindex-HWNAT_WLAN_IF_BASE));
	}
	
	return;
}

void ecnt_hwnat_wifi_init(void)
{
	ra_sw_wifi_hook_is_wifi_down = IsWifiInfDown;

	ra_sw_wifi_hook_wifi_down_handle = WifiInfDownHandle;

	ra_sw_wifi_hook_wifi_up_handle = WifiInfUpHandle;

}
void ecnt_hwnat_wifi_deinit(void)
{
	ra_sw_wifi_hook_is_wifi_down = NULL;

	ra_sw_wifi_hook_wifi_down_handle = NULL;

	ra_sw_wifi_hook_wifi_up_handle = NULL;

}


