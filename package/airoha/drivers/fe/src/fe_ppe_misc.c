#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_fe_type.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <ecnt_hook/ecnt_hook_pon_mac.h>
#include <ecnt_hook/ecnt_hook_qdma.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/libcompileoption.h>
#include <linux/version.h>

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/foe_hook.h>

#include "fe_ppe_misc.h"
#if defined(TCSUPPORT_MULTI_USER_ITF)
#include <lan_port/lan_port_info.h>
#endif

/*TCSUPPORT_TLS*/
static struct list_head tls_entry_list;
static spinlock_t tls_entry_list_lock;

static struct list_head tls_port_vlan_list;
static spinlock_t tls_port_vlan_list_lock;

static int tls_cfg_index_ctl[6] = {0}; 

int tls_enable = 0;

int tls_debug_on = 0;

void fe_tls_entry_list_init(void)
{
	INIT_LIST_HEAD(&tls_entry_list);
}

void fe_tls_tag_vlan_list_init(void)
{
	int i = 0;
	INIT_LIST_HEAD(&tls_port_vlan_list);
	for(i=0;i<6;i++)
		tls_cfg_index_ctl[i] = 0;
}

static  void* fe_tls_entry_list_alloc(int size)
{
	void* ptr = NULL;
	
	if (size>0)
	{
		ptr = kmalloc(size, GFP_ATOMIC);
		memset(ptr,0,size);
	}
	
	return ptr;
} 

static void fe_tls_entry_list_free(void* ptr)
{
	if (ptr)
	{
		kfree(ptr);
		ptr = NULL;
	} 
}

static int checkAddEntryPara(FE_Tls_info_t tls_info)
{
	if((tls_info.ani_vlan < 2) || (tls_info.uni_vlan < 2) ||(tls_info.ani_vlan > 4095) || (tls_info.uni_vlan > 4095))
	{
		printk("invalid ani or uni vlan id,the value should between 2~4095\n");
		return -1;
	}

	if(tls_info.uni_port_id > 3)
	{
		printk("invalid uni port id,the value should between 0~3\n");
		return -1;
	}
	
	return 0;
}

int fe_tls_add_tls_entry(FE_Tls_info_t tls_info)
{
	FE_Tls_List_Entry_t *tls_entry = NULL;
	int entry_exist = 0;

	if(-1 == checkAddEntryPara(tls_info))
		goto out;
	
	spin_lock_bh(&tls_entry_list_lock);
	list_for_each_entry(tls_entry, &tls_entry_list, list)	 
	{
		if((tls_entry->uni_port_id == tls_info.uni_port_id)&&(tls_entry->uni_vlan == tls_info.uni_vlan)&&\
			(tls_entry->gem_port == tls_info.gem_port)&&(tls_entry->ani_vlan == tls_info.ani_vlan))
		{
			printk("same entry exsit\n");
			entry_exist = 1;
			break;
		}
		
		if((tls_entry->uni_port_id == tls_info.uni_port_id)&&(tls_entry->uni_vlan == tls_info.uni_vlan))
		{
			if(tls_entry->gem_port != tls_info.gem_port)
			{
				printk("same uni info exsit from different gem port\n");
				entry_exist = 1;
				break;
			}
		}

		if((tls_entry->gem_port == tls_info.gem_port)&&(tls_entry->ani_vlan == tls_info.ani_vlan))
		{
			if(tls_entry->uni_port_id != tls_info.uni_port_id)
			{
				printk("same ani info exsit from different uni port\n");
				entry_exist = 1;
				break;
			}
		}
	}
	spin_unlock_bh(&tls_entry_list_lock);

	if(!entry_exist)
	{
		tls_entry = (FE_Tls_List_Entry_t*)fe_tls_entry_list_alloc(sizeof(FE_Tls_List_Entry_t));
		if(tls_entry)
		{
			tls_entry->uni_port_id = tls_info.uni_port_id;
			tls_entry->uni_vlan = tls_info.uni_vlan;
			tls_entry->gem_port = tls_info.gem_port;
			tls_entry->ani_vlan = tls_info.ani_vlan;

			spin_lock_bh(&tls_entry_list_lock);
			list_add_tail(&tls_entry->list,&tls_entry_list);
			spin_unlock_bh(&tls_entry_list_lock);

			return 0;
		}
	}

out:
	printk("add tls entry fail\n");
	return -1;
}

int fe_tls_del_tls_entry(FE_Tls_info_t tls_info)
{
	FE_Tls_List_Entry_t *tls_entry = NULL;
	FE_Tls_List_Entry_t *tmp = NULL;
	int entry_exist = 0;
	
	spin_lock_bh(&tls_entry_list_lock);
	list_for_each_entry_safe(tls_entry, tmp,&tls_entry_list, list)	 
	{
		if((tls_entry->uni_port_id == tls_info.uni_port_id)&&(tls_entry->uni_vlan == tls_info.uni_vlan)&&\
			(tls_entry->gem_port == tls_info.gem_port)&&(tls_entry->ani_vlan == tls_info.ani_vlan))
		{
			list_del(&tls_entry->list);
			fe_tls_entry_list_free(tls_entry);
			entry_exist = 1;
			break;
		}
	}
	spin_unlock_bh(&tls_entry_list_lock);

	if(!entry_exist)
	{
		printk("entry does not exsit,delete tls entry fail\n");
		return -1;
	}

	return 0;
}

int fe_tls_show_tls_entry(void)
{
	FE_Tls_List_Entry_t *tls_entry = NULL;

	printk("%10s%10s%10s%10s\n","uni_port","uni_vlan","gem_port","ani_vlan");
	list_for_each_entry(tls_entry, &tls_entry_list, list)	 
	{
		printk("%10d%10d%10d%10d\n",tls_entry->uni_port_id,tls_entry->uni_vlan,tls_entry->gem_port,tls_entry->ani_vlan);
	}

	return 0;
}

int fe_ioctl_set_trunk_mode_tls_cfg(unsigned char cmd,FE_Tls_info_t tls_info)
{
	switch(cmd)
	{
		case FE_ADD_TLS_ENTRY:
			fe_tls_add_tls_entry(tls_info);
			break;
		case FE_DEL_TLS_ENTRY:
			fe_tls_del_tls_entry(tls_info);
			break;
		case FE_SHOW_TLS_ENTRY:
			fe_tls_show_tls_entry();
			break;
		default:
			printk("cmd error in %s\n",__FUNCTION__);
	}

	return 0;
}

static char get_queue_info(struct sk_buff* skb)
{
	unsigned short vlan_tci = 0;
	unsigned char pbit = 0;
	
	if((0x8100 == skb->protocol)||(0x88a8 == skb->protocol)||(0x9100 == skb->protocol))
	{
		vlan_tci = *(unsigned short *)(skb->data);
		pbit = (vlan_tci&0xE000)>>13;

		return pbit;
	}

	return 0;
}

static int check_tls_vlan(struct sk_buff* skb,unsigned short vlan_tci)
{
	unsigned short* tmp = NULL;
	
	if((htons(0x8100) == skb->protocol)||(htons(0x88a8) == skb->protocol)||(htons(0x9100) == skb->protocol))
	{
		tmp = (unsigned short *)(skb->data);
		if((ntohs(*tmp)&0xFFF) == vlan_tci)
			return 1;
	}

	return 0;
}

int fe_trunk_mode_tls_forward_upstream(struct sk_buff* skb)
{	
	#ifdef TCSUPPORT_WAN_GPON
	FE_Tls_List_Entry_t *tls_entry = NULL;
	unsigned char port_id = 0;
	unsigned short gemport = 0;	
	int channel = 0;
	unsigned char queue = get_queue_info(skb);
	int entry_exist = 0;
	unsigned int txmsg_word0 = 0;
	unsigned int txmsg_word1 = 0;
	struct port_info xpon_info= {0}; 
	int error = 0;

	if((skb->dev)&&(skb->dev->name[0] == 'e'))
		port_id = skb->dev->name[5] - '1';
	else
		return -1;

	if(port_id > 3)
	{
		if(tls_debug_on)
			printk("port id %d is invalid in %s,skb->dev->name = %s\n",port_id,__FUNCTION__,skb->dev->name);
		return -1;
	}
	
	spin_lock_bh(&tls_entry_list_lock);
	list_for_each_entry(tls_entry, &tls_entry_list, list)	 
	{
		if(port_id ==  tls_entry->uni_port_id)
		{
			if((VLAN_NOT_CARE == tls_entry->uni_vlan)||check_tls_vlan(skb,tls_entry->uni_vlan))
			{
				gemport = tls_entry->gem_port;
				entry_exist = 1;
				break;
			}
		}

	}
	spin_unlock_bh(&tls_entry_list_lock);

	if(entry_exist)
	{
		ECNT_API_XPON_TCONT_GET(gemport,&channel);
		if(-1 != channel)
		{
			txmsg_word0 = queue|(channel<<3)|(gemport<<12);
			txmsg_word1 = PPE_FPORT_GDM2<<19;

			xpon_info.magic = FOE_MAGIC_GPON;
			xpon_info.stag = gemport ;
			xpon_info.channel = channel;
            
			xpon_info.txq_is_valid = 1;
			xpon_info.txq = queue;
			
			skb_push(skb,ETH_HLEN);
			error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_WAN, skb, txmsg_word0, txmsg_word1, &xpon_info) ;
			if(unlikely(error))
				dev_kfree_skb_any(skb);
		}
		else
			dev_kfree_skb_any(skb);

		return 0;
	}
	#endif
	
	return -1;
}

int fe_trunk_mode_tls_forward_downstream(struct sk_buff* skb)
{
	#ifdef TCSUPPORT_WAN_GPON
	FE_Tls_List_Entry_t *tls_entry = NULL;
	unsigned char port_id = 0;
	int entry_exist = 0;
	unsigned char outdev_name[10];
	
	spin_lock_bh(&tls_entry_list_lock);
	list_for_each_entry(tls_entry, &tls_entry_list, list)	 
	{
		if(skb->gem_port ==  tls_entry->gem_port)
		{
			if((VLAN_NOT_CARE == tls_entry->ani_vlan)||check_tls_vlan(skb,tls_entry->ani_vlan))
			{
				port_id = tls_entry->uni_port_id;
				entry_exist = 1;
				break;
			}
		}

	}
	spin_unlock_bh(&tls_entry_list_lock);

	if(entry_exist)
	{
		memset(outdev_name,0,10);
		sprintf(outdev_name,"eth0.%d",port_id+1);
		
		skb_push(skb,ETH_HLEN);
		
		#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)			
		skb->dev = dev_get_by_name(&init_net, outdev_name);
		if(skb->dev)
			skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
		else
			dev_kfree_skb_any(skb);
		#else			
		skb->dev = dev_get_by_name(outdev_name);
		if(skb->dev)
			skb->dev->hard_start_xmit(skb, skb->dev);
		else
			dev_kfree_skb_any(skb);
		#endif

		return 0;
	}
	#endif
	
	return -1;
}


static char g_tls_port = -1;
static short g_tls_vlan = -1;

int fe_tls_add_tls_port (unsigned char tls_port)
{
	struct ecnt_ppe_tls_cfg ppe_tls_cfg;
	unsigned char switch_port = 0;
	unsigned char sp = 0;

	if(tls_port > 3)
		printk("ERROR!tls port should be 0~3\n");
	
	if(-1 == g_tls_port)
		g_tls_port = tls_port;
	else
	{
		printk("tls port already exist\n");
		return 0;
	}

	ETHER_API_GET_PORTMAP(tls_port,&switch_port);
	
	sp = switch_port + 8;
		
	memset(&ppe_tls_cfg,0,sizeof(struct ecnt_ppe_tls_cfg));
	ppe_tls_cfg.en = 1;
	ppe_tls_cfg.sp = sp;
	
	PPE_API_SET_TLS_CFG(0,&ppe_tls_cfg);

	return 0;
}

int fe_tls_del_tls_port (void)
{
	struct ecnt_ppe_tls_cfg ppe_tls_cfg;
	
	if(-1 != g_tls_port)
		g_tls_port = -1;
	else
	{
		printk("tls port do not exist\n");
		return 0;
	}

	memset(&ppe_tls_cfg,0,sizeof(struct ecnt_ppe_tls_cfg));
	ppe_tls_cfg.en = 0;
	PPE_API_SET_TLS_CFG(0,&ppe_tls_cfg);

	return 0;
}

int fe_tls_set_tls_vlan(unsigned short tls_vlan)
{
	if(-1 == g_tls_vlan)
		g_tls_vlan = tls_vlan;
	else
		printk("tls vlan already exist\n");

	return 0;
}

int fe_tls_unset_tls_vlan (void)
{
	if(-1 != g_tls_vlan)
		g_tls_vlan = -1;
	else
		printk("tls vlan do not exist\n");

	return 0;
}

static int checkAddTLSEntryPara(unsigned char tls_port, unsigned short tls_vlan)
{
	if((tls_vlan < 2) || (tls_vlan > 4094))
	{
		printk("invalid vlan,vlan id should between 2~4094\n");
		return -1;
	}

	if(tls_port > 4)
	{
		printk("invalid port id,the value should between 0~4\n");
		return -1;
	}
	
	return 0;
}

int fe_tls_add_group_tls_entry(unsigned char tls_port, unsigned short tls_vlan)
{
	FE_Tls_List_port_Vlan_t *tls_entry = NULL;
	int entry_exist = 0;
	unsigned char switch_port =0;
	unsigned char sp = 0;
	struct ecnt_ppe_tls_cfg ppe_tls_cfg;
	int i = 0;

	if(-1 == checkAddTLSEntryPara(tls_port, tls_vlan))
		goto out;
	
	spin_lock_bh(&tls_port_vlan_list_lock);
	list_for_each_entry(tls_entry, &tls_port_vlan_list, list)	 
	{
		if((tls_entry->port_id == tls_port)&&(tls_entry->tls_vlan == tls_vlan))
		{
			printk("same entry exsit\n");
			entry_exist = 1;
			break;
		}

		if((tls_entry->port_id == tls_port))
		{
			if(tls_entry->tls_vlan != tls_vlan)
			{
				printk("same port id exsit from different vlan\n");
				entry_exist = 1;
				break;
			}
		}

		if(tls_entry->tls_vlan == tls_vlan)
		{
			if(tls_entry->port_id != tls_port)
			{
				printk("same tls vlan exsit from different port id\n");
				entry_exist = 1;
				break;
			}
		}
	}
	spin_unlock_bh(&tls_port_vlan_list_lock);

	if(!entry_exist)
	{
		tls_entry = (FE_Tls_List_port_Vlan_t*)fe_tls_entry_list_alloc(sizeof(FE_Tls_List_port_Vlan_t));
		if(tls_entry)
		{
			tls_entry->port_id = tls_port;
			tls_entry->tls_vlan = tls_vlan;

			for(i=0;i<6;i++) /*hw tls cfg only support 6 rules*/
			{
				if(tls_cfg_index_ctl[i] == 0){
					tls_entry->tls_cfg_index = i;
					tls_cfg_index_ctl[i] = 1;
					break;
				}
			}

			if(i == 6)
				tls_entry->tls_cfg_index = i;

			spin_lock_bh(&tls_port_vlan_list_lock);
			list_add_tail(&tls_entry->list,&tls_port_vlan_list);
			spin_unlock_bh(&tls_port_vlan_list_lock);

			if(tls_port <= 3){ /*lan port*/
				ETHER_API_GET_PORTMAP(tls_port,&switch_port);
			
			#ifdef TCSUPPORT_CPU_EN7580
				sp = switch_port + 16;
			#else
				sp = switch_port + 8;
			#endif

				memset(&ppe_tls_cfg,0,sizeof(struct ecnt_ppe_tls_cfg));
				ppe_tls_cfg.en = 1;
				ppe_tls_cfg.sp = sp;
				
				if(tls_entry->tls_cfg_index < 6)
					PPE_API_SET_TLS_CFG(tls_entry->tls_cfg_index,&ppe_tls_cfg);
				else
					printk("HW tls can only set 6!\n");
				
			}else if(tls_port == 4){ /*xfi port*/
			
				sp = 3;
				
				memset(&ppe_tls_cfg,0,sizeof(struct ecnt_ppe_tls_cfg));
				ppe_tls_cfg.en = 1;
				ppe_tls_cfg.sp = sp;

				if(tls_entry->tls_cfg_index < 6)
					PPE_API_SET_TLS_CFG(tls_entry->tls_cfg_index,&ppe_tls_cfg);
				else
					printk("HW tls can only set 6!\n");
			}

			return 0;
		}
	}

out:
	printk("add tls entry fail\n");
	return -1;
}

int fe_tls_del_group_tls_entry(unsigned char tls_port, unsigned short tls_vlan)
{
	FE_Tls_List_port_Vlan_t *tls_entry = NULL;
	FE_Tls_List_port_Vlan_t *tmp = NULL;
	int entry_exist = 0;
	int tls_cfg_en = 1;
	int tls_index = 0;
	struct ecnt_ppe_tls_cfg ppe_tls_cfg;
	
	spin_lock_bh(&tls_port_vlan_list_lock);
	list_for_each_entry_safe(tls_entry, tmp,&tls_port_vlan_list, list)	 
	{
		if((tls_entry->port_id == tls_port)&&(tls_entry->tls_vlan == tls_vlan))
		{
			if(tls_entry->tls_cfg_index >= 6)
				tls_cfg_en = 0;
			else
				tls_index = tls_entry->tls_cfg_index;
			list_del(&tls_entry->list);
			fe_tls_entry_list_free(tls_entry);
			entry_exist = 1;
			break;
		}
	}
	spin_unlock_bh(&tls_port_vlan_list_lock);

	if(!entry_exist)
	{
		printk("entry does not exsit,delete tls entry fail\n");
		return -1;
	}

	memset(&ppe_tls_cfg, 0, sizeof(struct ecnt_ppe_tls_cfg));
	ppe_tls_cfg.en = 0;

	if(tls_cfg_en){
		tls_cfg_index_ctl[tls_index] = 0;
		PPE_API_SET_TLS_CFG(tls_index,&ppe_tls_cfg);
	}else{
		printk("do not have to disable HW tls!\n");
	}
	
	return 0;
}

int fe_tls_show_group_tls_entry(void)
{
	FE_Tls_List_port_Vlan_t *tls_entry = NULL;

	printk("%10s%10s\n","port_id","tls_vlan");
	spin_lock_bh(&tls_port_vlan_list_lock);
	list_for_each_entry(tls_entry, &tls_port_vlan_list, list)	 
	{
		printk("%10d%10d\n",tls_entry->port_id,tls_entry->tls_vlan);
	}
	spin_unlock_bh(&tls_port_vlan_list_lock);

	return 0;
}

int fe_ioctl_set_tls_cfg(unsigned char cmd,unsigned char tls_port,unsigned short tls_vlan)
{
	switch(cmd)
	{
		case FE_ADD_TLS_PORT:
			fe_tls_add_tls_port(tls_port);
			break;
		case FE_DEL_TLS_PORT:
			fe_tls_del_tls_port();
			break;
		case FE_SET_TLS_VLAN:
			fe_tls_set_tls_vlan(tls_vlan);
			break;
		case FE_UNSET_TLS_VLAN:
			fe_tls_unset_tls_vlan();
			break;
		case FE_ADD_GROUP_TLS_ENTRY:
			fe_tls_add_group_tls_entry(tls_port,tls_vlan);
			break;
		case FE_DEL_GROUP_TLS_ENTRY:
			fe_tls_del_group_tls_entry(tls_port,tls_vlan);
			break;
		case FE_SHOW_GROUP_TLS_ENTRY:
			fe_tls_show_group_tls_entry();
			break;
		default:
			printk("cmd error in %s\n",__FUNCTION__);
	}

	return 0;
}

static int get_vlan_tag_num(struct sk_buff* skb)
{
	unsigned short* tmp = NULL;
	int vlan_tag_num = 0; 

	if((htons(0x8100) == skb->protocol)||(htons(0x88a8) == skb->protocol)||(htons(0x9100) == skb->protocol))
	{
		vlan_tag_num++;
		tmp = (unsigned short *)(skb->data);
        if(tmp)
        {      
            tmp += 1;

            while(*tmp == htons(0x8100) || *tmp == htons(0x88a8) || *tmp == htons(0x9100))
            {
                vlan_tag_num++;
                tmp+=2;
            }
        }
	}

	return vlan_tag_num;
}

static int tls_vlan_handle(struct sk_buff* skb,unsigned char direction,unsigned char* outdev_name)
{
	unsigned int  onu_type;

	ECNT_API_XPON_ONU_TYPE_GET(&onu_type);

	if(ONU_TYPE_HGU == onu_type)
	{
		if(FE_TLS_FORWARD_UPSTREAM == direction)
		{
			if(TCSUPPORT_PON_VLAN_VAL)
				skb->pon_vlan_flag |= PON_VLAN_TX_CALL_HOOK;
		}
		else
		{
			if(TCSUPPORT_PON_VLAN_VAL)
			{
				skb->pon_vlan_flag |= PON_VLAN_RX_CALL_HOOK;
				skb_pull(skb,ETH_HLEN);
			}
		}
	}
	else if(ONU_TYPE_SFU == onu_type)
	{
		if(TCSUPPORT_PON_VLAN_VAL)
		{
			if(FE_TLS_FORWARD_UPSTREAM == direction)
			{
				skb->pon_vlan_flag |= PON_PKT_FROM_LAN;
				skb->mark |= (skb->dev->name[5] - '0') << DEV_OFFSET;
				#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
				skb->dev = dev_get_by_name(&init_net, outdev_name);
				#else
				skb->dev = dev_get_by_name(outdev_name);
				#endif
			}
			else
			{
				skb->pon_vlan_flag |= PON_PKT_FROM_WAN;
				#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
				skb->dev = dev_get_by_name(&init_net, outdev_name);
				#else
				skb->dev = dev_get_by_name(outdev_name);
				#endif
			}
		}
	}
	else
		return 0;

	if(ra_sw_nat_hook_tls_vtag_handle_hook)
		ra_sw_nat_hook_tls_vtag_handle_hook(&skb);

	if((TCSUPPORT_PON_VLAN_VAL)&&(FE_TLS_FORWARD_DOWNSTREAM == direction)&&(ONU_TYPE_HGU == onu_type))
		skb_push(skb,ETH_HLEN);
	
	return 0;
}

int fe_group_tls_forward_upstream(struct sk_buff* skb)
{
	unsigned char port_id = 0;
	FE_Tls_List_port_Vlan_t *tls_port_vlan = NULL;
	unsigned char outdev_name[10];
	int port_exist = 0;
	unsigned char dir = FE_TLS_FORWARD_UPSTREAM;

	if((skb->dev)&&(skb->dev->name[0] == 'e')&&(skb->dev->name[4] == '.'))
		port_id = skb->dev->name[5] - '1';	
	else if((skb->dev)&&(skb->dev->name[0] == 'e')&&(skb->dev->name[3] == '1'))
		port_id = 4; 
	else
		return -1;

	if(port_id > 4)
	{
		return -1;
	}

	spin_lock_bh(&tls_port_vlan_list_lock);
	list_for_each_entry(tls_port_vlan, &tls_port_vlan_list, list)	 
	{
		if((port_id ==  tls_port_vlan->port_id))
		{
			port_exist = 1;
			break;
		}
	}
	spin_unlock_bh(&tls_port_vlan_list_lock);

	if(port_exist)
	{
		if(get_vlan_tag_num(skb) >= 2)
		{
			if(ra_sw_nat_hook_free)
				ra_sw_nat_hook_free(skb);
		}

		memset(outdev_name,0,10);
		sprintf(outdev_name,"pon");

		skb_push(skb,ETH_HLEN);

		tls_vlan_handle(skb,dir,outdev_name);

	#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
		skb->dev = dev_get_by_name(&init_net, outdev_name);
		if(skb->dev){
			skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
			dev_put(skb->dev);
		}
		else
			dev_kfree_skb_any(skb);
	#else
		skb->dev = dev_get_by_name(outdev_name);			
		if(skb->dev){
			skb->dev->hard_start_xmit(skb, skb->dev);
			dev_put(skb->dev);
		}
		else
			dev_kfree_skb_any(skb);
	#endif

		return 0;
	}

	return -1;
}

int fe_group_tls_forward_downstream(struct sk_buff* skb)
{
	FE_Tls_List_port_Vlan_t *tls_port_vlan = NULL;
	int vlan_exist = 0;
	unsigned char port_id = 0;
	unsigned char outdev_name[10];
	unsigned char dir = FE_TLS_FORWARD_DOWNSTREAM;

	spin_lock_bh(&tls_port_vlan_list_lock);
	list_for_each_entry(tls_port_vlan, &tls_port_vlan_list, list)	 
	{
		if((check_tls_vlan(skb,tls_port_vlan->tls_vlan)))
		{
			port_id = tls_port_vlan->port_id;
			vlan_exist = 1;
			break;
		}
	}
	spin_unlock_bh(&tls_port_vlan_list_lock);

	if(vlan_exist)
	{
		if(ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
		
		memset(outdev_name,0,10);
		
		if((port_id > 0)&&(port_id <=3))
			sprintf(outdev_name,"eth0.%d",port_id+1); 
		else if (port_id == 4)
			strcpy(outdev_name, "eth1");
		
		skb_push(skb,ETH_HLEN);

		tls_vlan_handle(skb,dir,outdev_name);
		
		#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)			
		skb->dev = dev_get_by_name(&init_net, outdev_name);
		if(skb->dev){
			skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
			dev_put(skb->dev);
		}
		else
			dev_kfree_skb_any(skb);
		#else			
		skb->dev = dev_get_by_name(outdev_name);
		if(skb->dev){
			skb->dev->hard_start_xmit(skb, skb->dev);
			dev_put(skb->dev);
		}
		else
			dev_kfree_skb_any(skb);
		#endif

		return 0;
	}

	return -1;
}


int fe_api_tls_forwad(struct ecnt_fe_data *fe_data)
{
	unsigned char lan_port = 0;
	struct sk_buff* skb = (struct sk_buff*)fe_data->api_data.tls_forward.skb;
	unsigned char dir = fe_data->api_data.tls_forward.dir;
	unsigned char outdev_name[10];

	#ifdef TCSUPPORT_CSC_EEUROPE
	if(!tls_enable)
		return -1;
	
	if(FE_TLS_FORWARD_UPSTREAM == dir)
		return fe_trunk_mode_tls_forward_upstream(skb);
	else
		return fe_trunk_mode_tls_forward_downstream(skb);
	#endif

	if(FE_TLS_FORWARD_UPSTREAM == dir)
		return fe_group_tls_forward_upstream(skb);
	else
		return fe_group_tls_forward_downstream(skb);

	return 0;
}

/*TCSUPPORT_TLS end*/

