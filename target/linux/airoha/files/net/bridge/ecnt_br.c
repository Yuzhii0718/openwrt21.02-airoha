#include <linux/foe_hook.h>
#include <linux/skbuff.h>
#include "br_private.h"
#include "ecnt_br.h"
#include <asm/tc3162/tc3162.h>
#include <modules/eth_global_def.h>
#if IS_ENABLED(CONFIG_IPV6)
#include <net/ipv6.h>
#endif
#include <ecnt_hook/ecnt_hook.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <linux/proc_fs.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include <lan_port/lan_port_info.h>
#ifdef TCSUPPORT_BRIDGE_MAC_LIMIT
unsigned int macLimit = 0;

static DEFINE_SPINLOCK(mac_limit_set_lock);

typedef struct 
{
	char* devName;
	int devPortNo;
	int macNumByPort;
	int maxNumByPort;
}dev_mac_num;

#if !defined(TCSUPPORT_FWC_FDB_VLAN) 
static dev_mac_num devMacNum[] = 
{
	{"ra0",1,0,0},
	{"eth0.1",3,0,0},
	{"eth0.2",4,0,0},
	{"eth0.3",5,0,0},
	{"eth0.4",6,0,0},
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
	{"eth0.5",7,0,0},
	{"eth0.6",8,0,0},
	{"eth0.7",9,0,0},
	{"eth0.8",10,0,0},
#endif
};
#endif
#define CNT_DEVMACNUM (sizeof(devMacNum)/sizeof(dev_mac_num))

#endif
/*---------------------------------------------------------------------*/
/*TCSUPPORT_VLAN_ACCESS_TRUNK start*/
#define MULTICAST_MIN_VLAN_ID				1
#define MULTICAST_MAX_VLAN_ID				4094
#define MULTICAST_VLAN_OFFSET(vlan_id)		(vlan_id/32)
#define MULTICAST_VLAN_SHIFT(vlan_id)		(vlan_id%32)
#define MULTICAST_VLAN_MAPBIT(vlan_id)		(1 << MULTICAST_VLAN_SHIFT(vlan_id))

static struct proc_dir_entry *br_vlan_snoop_proc = NULL;
struct net_bridge *vlan_net_br = NULL;
/*TCSUPPORT_VLAN_ACCESS_TRUNK end*/

/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED start*/
int g_last_snoop_state = 0;
unsigned int hw_igmp_flood_enable = 1;
#ifdef TCSUPPORT_SNOOPING_SEPERATION
unsigned int g_snooping_enable = 6;
#else
unsigned int g_snooping_enable = 1;
#endif
/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED end*/

/*TCSUPPORT_IGMP_SNOOPING start*/
int snoopingdebug = 0;
/*TCSUPPORT_IGMP_SNOOPING end*/

int olt_l2_bridge_off = 0;

struct HwnatBrList{
	struct list_head list;
	struct net_bridge *br;
};
unsigned long igmp_cur_group_cnt = 0;

/*---------------------------------------------------------------------*/

#define DEBUGP_SNOOP(x, args...) if(snoopingdebug) printk(x, ## args)

#define HIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]

/*---------------------------------------------------------------------*/

int (*check_vtag_match_dev_hook)(unsigned char *devName, unsigned short vid, unsigned char tagFlag);
EXPORT_SYMBOL(check_vtag_match_dev_hook);
int (*dslimit_SetUpTrtcmEnable)(u8 enable);
int (*dslimit_SetUpTrtcm)(u32 idx,u32 ratelimit);
EXPORT_SYMBOL(dslimit_SetUpTrtcm);
EXPORT_SYMBOL(dslimit_SetUpTrtcmEnable);
int (*dslimit_SetDsTrtcmEnable)(u8 enable);
int (*dslimit_SetDsTrtcm)(mt7530_switch_api_trtcm_t *trtcm_p);
EXPORT_SYMBOL(dslimit_SetDsTrtcmEnable);
EXPORT_SYMBOL(dslimit_SetDsTrtcm);

int igmp_force_leave = 0;

#define MAX_VLAN_GROUP 		20
typedef struct vlanBind {
	u16 lVlanId;
	u16 wVlanId;
} vlanBind_t;
#define MAX_PORT_NUM             4
#define		VBIND_INVALID_VLANID	4096
#define 	VTAG_GET_VID(tci)   	( (tci) & 0x0fff )
#if defined(TCSUPPORT_CT_VLAN_BIND)
extern vlanBind_t vBindArray[MAX_LAN_PORT_NUM][MAX_VLAN_GROUP];
extern int (*vlanbind_active_hook)(void);
extern int (*vlanbind_entry_active_hook)(int i);
#endif
extern int (*check_mulif_tci_hook)(u16 tci,u16 multicast_tci);

/*****for joyme4 feature******/
struct MulIPtmpList
{
	struct list_head list;
	unsigned char flow_status;
	unsigned char mul_protocol;
	unsigned char grp_ip[16];	
	
	short mul_port_no;
	int ref_cnt_udpxy;
};
struct list_head mul_tmp_list;
static DEFINE_RWLOCK(mul_stat_lock);

void ecnt_br_Mul_To_LAN_Config(struct sk_buff* skb);
EXPORT_SYMBOL(ecnt_br_Mul_To_LAN_Config);
unsigned int PPE_Mul_Status(void);
EXPORT_SYMBOL(PPE_Mul_Status);
void ecnt_multicast_add_tmp_info(unsigned char* address,u16 proto, short port_no, int ref_cnt_udpxy,unsigned char status_flag);
EXPORT_SYMBOL(ecnt_multicast_add_tmp_info);
/*****for joyme4 feature******/

int get_multicast_snooping_state_by_skb(struct sk_buff* skb);
int get_multicast_snooping_state_by_index(int index);
#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
int igmp_hwnat_multicast_get_local(unsigned char* grp_addr);
int igmp_vbind_get_lanvid( int port,IGMP_HWNATEntry_t* entry);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0) 
extern int br_ip_hash_for_export(struct net_bridge_mdb_htable *mdb, struct br_ip *ip);
extern struct net_bridge_mdb_entry *br_multicast_get_group(
	struct net_bridge *br, struct net_bridge_port *port,
	struct br_ip *group, int hash);
#endif

/*   merge form TC2 main trunck
 * Convert IP6 address to printable (loggable) representation.
 */
static char digits[] = "0123456789abcdef";
static int ip6round = 0;
char* ip6_sprintf(const struct in6_addr *addr)
{
	static char ip6buf[8][48];
	int i = 0;
	char *cp = NULL;
	const u_int16_t *a = (const u_int16_t *)addr;
	const u_int8_t *d = NULL;
	int dcolon = 0;

	ip6round = (ip6round + 1) & 7;
	cp = ip6buf[ip6round];

	for (i = 0; i < 8; i++) {
		if (dcolon == 1) {
			if (*a == 0) {
				if (i == 7)
					*cp++ = ':';
				a++;
				continue;
			} else
				dcolon = 2;
		}
		if (*a == 0) {
			if (dcolon == 0 && *(a + 1) == 0) {
				if (i == 0)
					*cp++ = ':';
				*cp++ = ':';
				dcolon = 1;
			} else {
				*cp++ = '0';
				*cp++ = ':';
			}
			a++;
			continue;
		}
		d = (const u_char *)a;
		{
			char ch[4] = {0};
			char i, j;
			ch[0] = digits[*d >> 4];
			ch[1] = digits[*d++ & 0xf];
			ch[2] = digits[*d >> 4];
			ch[3] = digits[*d & 0xf];
			for(i=0; i<4; i++)
			{
				if(ch[i] != '0')
					break;
			}
			if(i==4)
				*cp++ = digits[0];
			else
				for(j=i; j<4; j++) *cp++ = ch[j];
		}
		*cp++ = ':';
		a++;
	}
	*--cp = 0;
	return (ip6buf[ip6round]);
}
EXPORT_SYMBOL(ip6_sprintf);

static inline unsigned long hold_time(const struct net_bridge *br)
{
	return br->topology_change ? br->forward_delay : br->ageing_time;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
static inline int has_expired(const struct net_bridge *br,
				  const struct net_bridge_fdb_entry *fdb)
{
	return !fdb->is_static &&
		time_before_eq(fdb->updated + hold_time(br), jiffies);
}

static struct net_bridge_fdb_entry *fdb_find_rcu(struct hlist_head *head,
						 const unsigned char *addr,
						 __u16 vid)
{
	struct net_bridge_fdb_entry *fdb;

	hlist_for_each_entry_rcu(fdb, head, hlist) {
		if (ether_addr_equal(fdb->addr.addr, addr) &&
		    fdb->vlan_id == vid)
			return fdb;
	}
	return NULL;
}

static struct net_bridge_fdb_entry *fdb_find(struct hlist_head *head,
					     const unsigned char *addr,
					     __u16 vid)
{
	struct net_bridge_fdb_entry *fdb;

	hlist_for_each_entry(fdb, head, hlist) {
		if (ether_addr_equal(fdb->addr.addr, addr) &&
		    fdb->vlan_id == vid)
			return fdb;
	}
	return NULL;
}
#endif

#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
static inline int has_expired_multicast(const struct net_bridge *br,
				  const struct net_bridge_port_group *bpg)
{
	return time_before_eq((bpg->ageing_time + br->multicast_membership_interval+(br->quick_leave?0:2*HZ)), jiffies);
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
inline int br_mdb_fillbuf(struct net_bridge *br, void *buf,
		   unsigned long maxnum, unsigned long skip)
{	
	struct __mc_fdb_entry *fe = buf;
	struct net_bridge_mdb_entry *mp = NULL;
	struct net_bridge_port_group *p;
	struct net_bridge_port_group __rcu **pp;
	int num = 0;
	long result = 0;

	if (!br_opt_get(br, BROPT_MULTICAST_ENABLED))
		return 0;

	memset(buf, 0, maxnum * sizeof(struct __mc_fdb_entry));

	rcu_read_lock();
	spin_lock_bh(&br->multicast_lock);
	hlist_for_each_entry_rcu(mp, &br->mdb_list, mdb_node) {
		for (pp = &mp->ports; (p = rcu_dereference(*pp)) != NULL; pp = &p->next) {
			if (num >= maxnum)
				goto out;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
			if (!p->key.port)
#else
			if (!p->port)
#endif
				continue;
			
			if (has_expired_multicast(br, p)){
				continue;
			}

			if (skip) {
				--skip;
				continue;
			}
			
			if(p->version ==4){			
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
				sprintf(fe->group_addr, NIPQUAD_FMT, HIPQUAD(p->key.addr.dst.ip4));
#else
				sprintf(fe->group_addr, NIPQUAD_FMT, HIPQUAD(p->addr.u.ip4));
#endif
				sprintf(fe->src_addr, NIPQUAD_FMT, HIPQUAD(p->src_entry.src.s_addr));
			}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
			else if(p->version == 6){
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
				strncpy(fe->group_addr, ip6_sprintf(&p->key.addr.dst.ip6), sizeof(fe->group_addr)-1);
#else
				strncpy(fe->group_addr, ip6_sprintf(&p->addr.u.ip6), sizeof(fe->group_addr)-1);
#endif
				strncpy(fe->src_addr, ip6_sprintf(&p->src_entry.src6), sizeof(fe->src_addr)-1);
				fe->group_addr[sizeof(fe->group_addr)-1] = '\0';
				fe->src_addr[sizeof(fe->src_addr)-1] = '\0';
			}
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
			fe->port_no = p->key.port->port_no;
#else
			fe->port_no = p->port->port_no;
#endif
			fe->version = p->version;
			memcpy(fe->group_mac, p->group_mac, ETH_ALEN);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
			memcpy(fe->host_addr, p->key.port->macAddr.addr, ETH_ALEN);
#else
			memcpy(fe->host_addr, p->port->macAddr.addr, ETH_ALEN);
#endif			
			fe->filter_mode = p->src_entry.filt_mode;
			result = jiffies - p->ageing_time;
			fe->ageing_timer_value = jiffies_to_clock_t((result>0) ? result : 0);
			++fe;
			++num;
		}
	}

out:
	spin_unlock_bh(&br->multicast_lock);
	rcu_read_unlock();
	return num;
}
#else
inline int br_mdb_fillbuf(struct net_bridge *br, void *buf,
		   unsigned long maxnum, unsigned long skip)
{	
	struct __mc_fdb_entry *fe = buf;
	struct net_bridge_mdb_htable *mdb = NULL;
	struct net_bridge_port_group *bpg = NULL;
	int i = 0, num = 0;
	long result = 0;
	struct net_bridge_mdb_entry *f = NULL;
	
	mdb = br->mdb;
	if(!mdb)
		return 0;
	memset(buf, 0, maxnum*sizeof(struct __mc_fdb_entry));
	
	rcu_read_lock();
	spin_lock_bh(&br->multicast_lock);
	for (i = 0; i < mdb->max; i++) {
		hlist_for_each_entry_rcu(f, &mdb->mhash[i], hlist[mdb->ver]) {
			if (num >= maxnum)
				goto out;
			if (skip) {
				--skip;
				continue;
			}
			bpg = f->ports;
			while(bpg&&(num<maxnum)){
				if (has_expired_multicast(br, bpg)){
					bpg = bpg->next;
					continue;
				}
				if(bpg->version ==4){
					sprintf(fe->group_addr,NIPQUAD_FMT ,HIPQUAD(bpg->addr.u.ip4));
					sprintf(fe->src_addr, NIPQUAD_FMT, HIPQUAD(bpg->src_entry.src.s_addr));
				}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
				else if(bpg->version == 6){
					strncpy(fe->group_addr,ip6_sprintf(&bpg->addr.u.ip6),sizeof(fe->group_addr)-1);
					strncpy(fe->src_addr,ip6_sprintf(&bpg->src_entry.src6),sizeof(fe->src_addr)-1);
					fe->group_addr[sizeof(fe->group_addr)-1] = '\0';
					fe->src_addr[sizeof(fe->src_addr)-1] = '\0';
				}
#endif
				fe->port_no = bpg->port->port_no;
				fe->version = bpg->version;
				memcpy(fe->group_mac, bpg->group_mac, ETH_ALEN);
				memcpy(fe->host_addr, bpg->port->macAddr.addr, ETH_ALEN);
				
				fe->filter_mode = bpg->src_entry.filt_mode;
				result = jiffies - bpg->ageing_time;
				fe->ageing_timer_value = jiffies_to_clock_t((result>0) ? result : 0);
				bpg = bpg->next;
				++fe;
				++num;
			}
			
		}
	}

 out:
 	spin_unlock_bh(&br->multicast_lock);
	rcu_read_unlock();
	return num;
}
#endif
EXPORT_SYMBOL(br_mdb_fillbuf);

int get_mc_fdb_entries(struct net_bridge *br, void __user *userbuf,
			   unsigned long maxnum, unsigned long offset)
{
	int num = 0;
	void *buf = NULL;
	size_t size = 0;

	/* Clamp size to PAGE_SIZE, test maxnum to avoid overflow */
	if (maxnum > PAGE_SIZE/sizeof(struct __mc_fdb_entry))
		maxnum = PAGE_SIZE/sizeof(struct __mc_fdb_entry);

	size = maxnum * sizeof(struct __mc_fdb_entry);

	buf = kmalloc(size, GFP_USER);
	if (!buf)
		return -ENOMEM;

	num = br_mdb_fillbuf(br, buf, maxnum, offset);
	if (num > 0) {
		if (copy_to_user(userbuf, buf, num*sizeof(struct __mc_fdb_entry)))
			num = -EFAULT;
	}
	kfree(buf);

	return num;
}

int br_multicast_equal_port_group(struct net_bridge_port_group *pg,  
	struct net_bridge_port *port, struct br_ip *group)
{
	if(!pg || !port || !group)
		return 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	if((pg->version != port->version) ||(pg->key.port != port))
#else
	if((pg->version != port->version) ||(pg->port != port))
#endif
		return 0;

	if(port->version == 4)
	{
		if(pg->src_entry.src.s_addr == port->src_entry.src.s_addr)
			return 1;
	}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	else if(port->version == 6)
	{
		if(ipv6_addr_equal(&pg->src_entry.src6, &port->src_entry.src6))//group ip
			return 1;
	}
#endif
	return 0;
}
#else
inline int br_mdb_fillbuf(struct net_bridge *br, void *buf,
		   unsigned long maxnum, unsigned long skip)
{	
	return 0 ;
}
EXPORT_SYMBOL(br_mdb_fillbuf);

int get_mc_fdb_entries(struct net_bridge *br, void __user *userbuf,
			   unsigned long maxnum, unsigned long offset)
{
	return 0 ;
}
int br_multicast_equal_port_group(struct net_bridge_port_group *pg,  
	struct net_bridge_port *port, struct br_ip *group)
{
	return 0 ;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////
/*TCSUPPORT_XPON_IGMP || TCSUPPORT_MULTICAST_SPEED start*/
	#define MUL_PROTO_IGMP 1
	#define MUL_PROTO_MLD 2

	static struct list_head hwnat_igmp_entry;
	static struct list_head hwnat_igmp_local_entry;
	static unsigned int hwnat_igmp_flag = 1;
	static unsigned int hwnat_igmp_1ton = 0;
	static unsigned int hwnat_age_time = 3000;
	static spinlock_t	 hwnat_lock;
	static spinlock_t	 hwnat_br_lock;
	static spinlock_t		hwnat_local_lock;
	static struct list_head hwnat_br_table;
	static struct proc_dir_entry *hwnat_proc = NULL;

/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED start*/
	static spinlock_t	 hwnat_disable_snooping_lock;
	static LIST_HEAD(multicast_flood_hw_list);
/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED end*/

/*TCSUPPORT_XPON_IGMP || TCSUPPORT_MULTICAST_SPEED end*/

/*TCSUPPORT_XPON_IGMP || TCSUPPORT_MULTICAST_SPEED start*/
#ifdef TCSUPPORT_RA_HWNAT
	extern int (*hwnat_is_alive_pkt_hook)(struct sk_buff* skb);
	extern int (*hwnat_skb_to_foe_hook)(struct sk_buff* skb);
	extern int (*hwnat_set_special_tag_hook)(int index, int tag);
	extern int (*hwnat_delete_foe_entry_hook)(int index); 
	extern int (*hwnat_is_multicast_entry_hook)(int index ,unsigned char* grp_addr,unsigned char* src_addr,int type);
	/*TCSUPPORT_MULTICAST_SPEED start*/
	extern IGMP_HWNATEntry_t* (*multicast_speed_find_entry_hook)(int index);
	extern int (*multicast_speed_learn_flow_hook)(struct sk_buff* skb);
	extern int (*hwnat_set_rule_according_to_state_hook)(int index, int state,unsigned long long mask);
	/*TCSUPPORT_MULTICAST_SPEED end*/
	extern int (*xpon_igmp_learn_flow_hook)(struct sk_buff* skb);
	extern int (*hwnat_set_wlan_multicast_hook)(int index,int flag);
	extern int (*wan_multicast_undrop_hook)(void);
	extern int (*wan_multicast_undrop_by_grpip_hook)(unsigned char is_ipv6,unsigned char* grp_ip);
	/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED start*/
	extern multicast_flood_hwentry_t* (*multicast_flood_find_entry_hook)(int index);
	extern int  (*hwnat_set_multicast_speed_enable_hook)(int enable);
	extern int  (*multicast_flood_is_bind_hook)(int index);
	/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED end*/
	extern int (*ra_sw_nat_hook_entry_type)(int index);
	extern int (*multicast_xmit_packet_hook)(struct sk_buff* skb, unsigned short vid, 
                                             struct net_device *skip_eth_dev, unsigned short skip_eth_vid);
	extern int (*hwnat_set_multicast_vlan_hook)(int index, int vid, int vpm);
	extern int (*multicast_subcribe_group_hook)(unsigned char* grp_addr, unsigned int proto,unsigned int update_mode);
	extern int (*multicast_get_local_hook)(unsigned char* grp_addr);
#else
	static int (*hwnat_is_alive_pkt_hook)(struct sk_buff* skb) = NULL;
	static int (*hwnat_skb_to_foe_hook)(struct sk_buff* skb) = NULL;
	static int (*hwnat_set_special_tag_hook)(int index, int tag) = NULL;
	static int (*hwnat_delete_foe_entry_hook)(int index) = NULL; 
	static int (*hwnat_is_multicast_entry_hook)(int index ,unsigned char* grp_addr,unsigned char* src_addr,int type) = NULL;
	/*TCSUPPORT_MULTICAST_SPEED start*/
	extern int (*multicast_speed_find_entry_hook)(int index);
	static int (*multicast_speed_learn_flow_hook)(struct sk_buff* skb) = NULL;
	static int (*hwnat_set_rule_according_to_state_hook)(int index, int state,unsigned long long mask) = NULL;
	/*TCSUPPORT_MULTICAST_SPEED end*/
	static int (*hwnat_set_wlan_multicast_hook)(int index,int flag) = NULL;
	static int (*xpon_igmp_learn_flow_hook)(struct sk_buff* skb) = NULL;
	static int (*wan_multicast_undrop_hook)(void) = NULL;
	static int (*wan_multicast_undrop_by_grpip_hook)(unsigned char is_ipv6,unsigned char* grp_ip) = NULL;
	/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED start*/
	static int (*multicast_flood_find_entry_hook)(int index) = NULL;
	static int (*hwnat_set_multicast_speed_enable_hook)(int enable) = NULL;
	static int (*multicast_flood_is_bind_hook)(int index) = NULL;
	/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED end*/
	static int (*multicast_xmit_packet_hook)(struct sk_buff* skb, unsigned short vid, 
	                                         struct net_device *skip_eth_dev, unsigned short skip_eth_vid) = NULL;
	static int (*hwnat_set_multicast_vlan_hook)(int index, int vid, int vpm);
	static int (*multicast_subcribe_group_hook)(unsigned char* grp_addr, unsigned int proto,unsigned int update_mode) = NULL;
	static int (*multicast_get_local_hook)(unsigned char* grp_addr) = NULL;
#endif

/*------------------------------------------------------------------------------*/
struct list_head* igmp_hwnat_get_list(void)
{
	return &hwnat_igmp_entry;
}

struct list_head* igmp_hwnat_get_local_list(void)
{
	return &hwnat_igmp_local_entry;
}

int igmp_hwnat_debug_on(void)
{
	return hwnat_igmp_flag&0x02;
}

int igmp_hwnat_enable(void)
{
	return hwnat_igmp_flag&0x01;
}

#define IGMP_HWNAT_DEBUG(fmt,args...)  		\
	do{									\
		if(igmp_hwnat_debug_on())	\
    	{								\
    		printk("\r\n%s:"fmt,__FUNCTION__,##args);\
    	}								\
	}while(0)

void* igmp_hwnat_alloc(int size)
{
	void* ptr = NULL;
	if (size>0)
	{
		ptr = kzalloc(size, GFP_ATOMIC);
	}
	return ptr;

}

#ifdef TCSUPPORT_MULTICAST_SPEED
void igmp_hwnat_free(struct rcu_head *head)
{
	struct IGMP_HWNATEntry_s *entry
		= container_of(head, struct IGMP_HWNATEntry_s,rcu);
	if(entry)
		kfree(entry);
	
	return;
}
#else
void igmp_hwnat_free(void* ptr)
{
	if (ptr)
	 	kfree(ptr);

	ptr = NULL;
	return;
}
#endif

#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
static inline int ecnt_multicast_xmit_packet_snoop_on(struct sk_buff* skb, unsigned short vid, 
                                                                struct net_device *skip_eth_dev, unsigned short skip_eth_vid)
{
	struct net_bridge_mdb_entry *mdst = NULL;
	struct net_bridge_port_group *p = NULL;
	struct net_bridge_port *port = NULL;
	struct sk_buff* skb2 = NULL;
	unsigned short pvid;
	int relearn_flag = 1; 

	rcu_read_lock();
	mdst = br_mdb_get(vlan_net_br, skb, vid);
	if (mdst) {
		/* 1. mdb find, loop bridge port in mdb, forward packet */
		p = rcu_dereference(mdst->ports);
		while (p) {
			port = p->port;
			pvid = br_get_pvid(nbp_get_vlan_info(port)); //get bridge port pvid

			/* 1.1. skip full acceleration forward */
			if( port->dev == skip_eth_dev ) {
				if( ((skip_eth_vid == 0) && (vid == pvid)) || (vid == skip_eth_vid) ) {
					relearn_flag = 0;
					p = rcu_dereference(p->next);
					continue;
				}
			}

			/* 1.2. copy skb then send to device */
			skb2 = skb_clone(skb, GFP_ATOMIC);
			if (skb2) {
				if (pvid != vid) {
					/* 1.3. if vid is not pvid, packet should has vlan out */
					skb2 = skb_unshare(skb2, GFP_ATOMIC);
					skb2 = __vlan_put_tag(skb2, vid);
				}
                
				skb2->dev = port->dev;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
				skb2->dev->netdev_ops->ndo_start_xmit(skb2, skb2->dev);
#else
				skb2->dev->hard_start_xmit(skb2, skb2->dev);
#endif
			} else {
				printk("[ecnt_multicast_xmit_packet_snoop_on] skb copy fail!\n");
			}
			
			p = rcu_dereference(p->next);
		}
	}

	rcu_read_unlock();
	return relearn_flag;
}

static inline int ecnt_multicast_xmit_packet_snoop_off(struct sk_buff* skb, unsigned short vid, 
                                                                struct net_device *skip_eth_dev, unsigned short skip_eth_vid)
{
	struct net_bridge_port *port = NULL;
	struct sk_buff* skb2 = NULL;
	unsigned short pvid;
	int relearn_flag = 1;
	struct net_port_vlans *v;

	rcu_read_lock();
	/* loop all bridge ports */
	list_for_each_entry_rcu(port, &vlan_net_br->port_list, list) {
		/* 1. if the packet from this port, do not flood */
		if (!(port->flags & BR_FLOOD) || (skb->dev == port->dev))
			continue;

		/* 2. if the port has this vlan, should forward packet */
		v = nbp_get_vlan_info(port);
		if ( test_bit(vid, v->vlan_bitmap) ) {
			pvid = br_get_pvid(v); //get bridge port pvid

			/* 3. skip full acceleration forward */
			if( port->dev == skip_eth_dev ) {
				if( ((skip_eth_vid == 0) && (vid == pvid)) || (vid == skip_eth_vid) ) {
					relearn_flag = 0;
					continue;
				}
			}

			/* 4. copy skb then send to device */
			skb2 = skb_clone(skb, GFP_ATOMIC);
			if (skb2) {
				if (pvid != vid) {
					/* 5. if vid is not pvid, packet should has vlan out */
					skb2 = skb_unshare(skb2, GFP_ATOMIC);
					skb2 = __vlan_put_tag(skb2, vid);
				}
	            
				skb2->dev = port->dev;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
				skb2->dev->netdev_ops->ndo_start_xmit(skb2, skb2->dev);
#else
				skb2->dev->hard_start_xmit(skb2, skb2->dev);
#endif
			} else {
				printk("[ecnt_multicast_xmit_packet_snoop_off] skb copy fail!\n");
			}
			
		}
	}

	rcu_read_unlock();
	return relearn_flag;
}

int ecnt_multicast_xmit_packet(struct sk_buff* skb, unsigned short vid, 
                                     struct net_device *skip_eth_dev, unsigned short skip_eth_vid)
{
	struct net_bridge *br = NULL;
	struct net_device *dev = NULL;
	int relearn_flag = 1;
	
	if ( unlikely(!vlan_net_br) ) {
		dev = dev_get_by_name(&init_net, "br0");
		if( !dev ){
			printk("[ecnt_multicast_xmit_packet] no net device br0\n");
			kfree_skb(skb);
			return -1;
		}
		br = netdev_priv(dev);
		dev_put(dev);

		if( !br ){
			printk("[ecnt_multicast_xmit_packet] no net device br0\n");
			kfree_skb(skb);
			return -1;
		}

		vlan_net_br = br;
	}

	if(!ecnt_br_get_snooping_state_inline_hook(vlan_net_br, skb->protocol, vid)) {
		/* snooping on */
		relearn_flag = ecnt_multicast_xmit_packet_snoop_on(skb, vid, skip_eth_dev, skip_eth_vid);
	} else {
		/* snooping off */
		relearn_flag = ecnt_multicast_xmit_packet_snoop_off(skb, vid, skip_eth_dev, skip_eth_vid);
	}

	kfree_skb(skb);
	return relearn_flag;
}
#endif

/**************************************************
Function: Get Port Mask Bit
Input: pointer to struct net_bridge_port
Return: 
	0-3: ethernet port mask
	8-11: wifi port mask
	-1: error
**************************************************/
extern int g_port_reverse_kernel;
#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
int igmp_hwnat_get_port(struct net_bridge_port*  p)
{
	if (! p || !( p->dev)) 
	{
		if (! p) 
			IGMP_HWNAT_DEBUG("p == NULL");
		else
			IGMP_HWNAT_DEBUG("p->dev == NULL");
		return -1;
	}

	IGMP_HWNAT_DEBUG("port name = %s ",p->dev->name);
	
	if (isLANInterface(p->dev)) {
#if defined(TCSUPPORT_CUC_C5_2P)
		unsigned int temport = 0;
		unsigned int port = getLogicLANIndex(p->dev);
		if(port>=0 && port<=3)
		{
			if(port == 0) temport = 3;
			else if(port == 1) temport = 0;
			else if(port == 2) temport = 1;
			else if(port == 3) temport = 2;
		}
		port = temport;
			
		return port;
#else
		if (g_port_reverse_kernel == 1 && getLogicLANIndex(p->dev) <= 3)
			return  3 - getLogicLANIndex(p->dev);
		else
			return  getLogicLANIndex(p->dev);
#endif
	}

#ifdef TCSUPPORT_MULTICAST_SPEED
	if (is24GWiFiInterface(p->dev) && 
		((get24GLogicWifiIndex(p->dev) >= 0) && (get24GLogicWifiIndex(p->dev) <= (HWNAT_WLAN_IF_NUM - 1)))) 
	{
		return get24GLogicWifiIndex(p->dev)+HWNAT_WLAN_IF_BASE;
	}

	if (is5GWiFiInterface(p->dev) && 
		((get5GLogicWifiIndex(p->dev) >= 0) && (get5GLogicWifiIndex(p->dev) <= (HWNAT_WLAN_IF_I_NUM - 1)))) 
	{
		return  get5GLogicWifiIndex(p->dev)+HWNAT_WLAN_IF_BASE+HWNAT_WLAN_IF_NUM;
	}

#else
	if ((p->dev->name[0] == 'r') && (p->dev->name[1] == 'a'))
				return  (p->dev->name[5] - '0')+8;
#endif
	if(isUSBInterface(p->dev))
		return (getUSBIndex(p->dev) + HWNAT_USB_IF_BASE);

	if(isXSIInterface(p->dev))
		return HWNAT_XSI_IF_BASE;
    
	return -1;
}

/**************************************************
Function: Check if the port can receive the 
          multicast flow 
Input: 
	br: pointer to struct net_bridge
	port: pointer to struct net_bridge_port
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0: fail;  1: ok
**************************************************/

int igmp_hwnat_should_deliver(struct net_bridge *br,struct net_bridge_port *port,IGMP_HWNATEntry_t* entry)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)	
	struct net_bridge_mdb_htable *mdb = br->mdb;
#endif
	struct net_bridge_mdb_entry *mp = NULL;
	struct net_bridge_port_group *pg = NULL;
	struct br_ip  group;
	int hash = 0, flag = 0;
	char src[16];

#ifndef TCSUPPORT_XPON_HAL_API_MCST
	if(entry->snoop_off)
	{		
		IGMP_HWNAT_DEBUG("igmp/mld snooping off,deliver\n");
		return 1;
	}
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)	
	if (hlist_empty(&br->mdb_list))
#else
	if (!mdb)
#endif
	{
		IGMP_HWNAT_DEBUG("mdb == NULL");
		return 0;
	}

	IGMP_HWNAT_DEBUG("entry->proto=%d",entry->proto);

	memset(&group, 0, sizeof(group));
	if (entry->proto == MUL_PROTO_IGMP)
		group.proto = htons(ETH_P_IP);
	else if (entry->proto == MUL_PROTO_MLD)
		group.proto = htons(ETH_P_IPV6);
	else
		return 0;
	
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	memcpy(group.dst.ip6.s6_addr,entry->grp_addr,16);
#else
	memcpy(group.u.ip6.s6_addr,entry->grp_addr,16);
#endif
#endif
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	if(br->vlan_enabled)
		group.vid = entry->br_vid;
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)	
	hash = br_ip_hash_for_export(mdb, &group);
#endif
	mp = br_multicast_get_group(br, port, &group, hash);
	
	switch (PTR_ERR(mp)) {
		case -E2BIG:
		case -EEXIST:
		case -EAGAIN:
			IGMP_HWNAT_DEBUG("mp == ERR");
			return 0;
	
		default:
			break;
	}

	if (mp==NULL)
	{
		IGMP_HWNAT_DEBUG("mp == NULL");
		return 0;
	}

	memset(src,0,16);
	memcpy(src,entry->src_addr,16);
	pg = mp->ports;

	while(pg)
	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
		if (pg->key.port != port)
#else
		if (pg->port != port)
#endif
		{
			pg = pg->next;
			continue;
		}
		
		if (entry->proto==MUL_PROTO_IGMP)
		{
			if(pg->src_entry.filt_mode == MCAST_INCLUDE)
			{
				if (memcmp(&pg->src_entry.src.s_addr,src,4)==0)
					return 1;
			}
			else if(pg->src_entry.filt_mode == MCAST_EXCLUDE)
			{
				if(0 == pg->src_entry.src.s_addr)
					return 1;
				else if(memcmp(&pg->src_entry.src.s_addr,src,4))
					flag  = 2;
				else if(memcmp(&pg->src_entry.src.s_addr,src,4)==0)
					return 0;		
			}
		}
		if (entry->proto==MUL_PROTO_MLD)
			return 1;

		pg = pg->next;
	}

	if (flag == 2)
		return 1;
	return 0;
	
}

extern int (*portbind_check_mc_hook)(struct net_device *in_dev,  struct net_device *out_dev);

static int br_strtou16(const char* pbuf, u_int16_t* pval)
{
    int n = 0;

    *pval = 0;
    while (isdigit(pbuf[n]))
    {
        *pval = (*pval * 10) + (pbuf[n] - '0');
        n++;
    }

    return n;
}

/**************************************************
Function: Get forwarded ports given a multicast
          group 
Input: 
	br: pointer to struct net_bridge
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0x0-0x0f0f: port mask
***************************************************/
unsigned long long igmp_hwnat_port_mask(struct net_bridge *br,IGMP_HWNATEntry_t* entry)
{
	struct net_bridge_port *p = NULL;
	int port = 0;
	unsigned long long mask = 0;
    int switch_port = 0;
#ifndef TCSUPPORT_SEPERATED_ETH_ITF		
	unsigned short vid_val = 0;
#endif
	
	IGMP_HWNAT_DEBUG("enter");
	list_for_each_entry(p, &br->port_list, list)
	{
		if (igmp_hwnat_should_deliver(br,p,entry)==0)
			continue;	
		
#ifdef TCSUPPORT_PORTBIND
		if(portbind_check_mc_hook)
		{
			if (portbind_check_mc_hook(entry->orig_dev,p->dev) == 0)
				continue;
		}
#endif
		port = igmp_hwnat_get_port(p);
		
		if (port < 0 )
			 continue;
        /*lan port*/
#ifdef TCSUPPORT_HSGMII_LAN
        if(port >= 0 && port <= 11)
#else
        if(port >= 0 && port <= 7 || get_aewan_idx()>0)
#endif
        {
            mask |= 1 << port;
        }
        /*wifi port*/
        else if(port >= HWNAT_WLAN_IF_BASE)
        {
            mask |= (unsigned long long)1 << port;
        }
        else if(port >= HWNAT_USB_IF_BASE)
        {
            mask |= 1 << port;
        }
        else if(port == HWNAT_XSI_IF_BASE)
        {
            mask |= 1 << port;
        }
#ifndef TCSUPPORT_SEPERATED_ETH_ITF	
        if (isLANInterface(p->dev)) 
        {
                if(br_strtou16(&p->dev->name[5], &vid_val) > 0)
	                entry->m_vlan = (vid_val & 0xfff);
        }
#ifdef TCSUPPORT_MULTICAST_SPEED		
        if (is24GWiFiInterface(p->dev) && (strlen(p->dev->name) >= 5)) 
        {
                if(br_strtou16(&p->dev->name[4], &vid_val) > 0)
	                entry->m_vlan = (vid_val & 0xfff);
        }
        if (is5GWiFiInterface(p->dev) && (strlen(p->dev->name) >= 6) )
        {
                if(br_strtou16(&p->dev->name[5], &vid_val) > 0)
	                entry->m_vlan = (vid_val & 0xfff);
        }
#endif		
#endif
	}	
	IGMP_HWNAT_DEBUG("mask = %llu",mask);	
	
	return mask;
}

/**************************************************
Function: Delete a entry maintained by SW
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	  0: ok
**************************************************/
int igmp_hwnat_delete_entry(IGMP_HWNATEntry_t* entry)
{
	if (entry == NULL)
	{
		IGMP_HWNAT_DEBUG("entry == NULL");
		return 0;
	}
	IGMP_HWNAT_DEBUG("enter");

	if(hwnat_igmp_1ton)
	{
	
		int port=0;
	    int  i = 0;
		unsigned char switch_port_id = 0;
		u16 src_vid = 0;
	
		for(i = 0; i < entry->mask; i++)
		{
		
			if(0 == (entry->mask & (1 << i)))
			{
				continue ;
			}
			
			port = i;
			IGMP_HWNAT_DEBUG("port =%d",port);
			ETHER_API_GET_PORTMAP(port,&switch_port_id);
			
			src_vid = entry->m_vlan;
			
			IGMP_HWNAT_DEBUG("switch_port_id %d src_vid %d \n", switch_port_id,src_vid);
			ETHER_API_PER_VLAN_ACTION(switch_port_id, src_vid, 0, 0, 0);
		}
	}

	del_timer(&entry->age_timer);
#ifdef TCSUPPORT_MULTICAST_SPEED
	list_del_rcu(&(entry->list));
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	call_rcu_bh(&(entry->rcu), igmp_hwnat_free);
#else
  	call_rcu(&(entry->rcu), igmp_hwnat_free);
#endif
#else
	list_del(&entry->list);
	igmp_hwnat_free(entry);
#endif
	return 0;
}

/**************************************************
Function: Check if the multicast group is still 
          accelerated  by HW
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0: fail;  1: ok
**************************************************/
int igmp_hwnat_check_entry_valid(IGMP_HWNATEntry_t* entry)
{
	int valid = 0,type = 0;
	
	if (entry->proto == MUL_PROTO_MLD)
		type = 1;
	
	if (hwnat_is_multicast_entry_hook)
		valid = hwnat_is_multicast_entry_hook(entry->index,entry->grp_addr,entry->src_addr,type);
	
	return valid;
}

/**************************************************
Function: Get the foe index from skb
Input: 
	entry: pointer to struct sk_buff
Return: 
	foe entry index in hwnat
**************************************************/
int igmp_hwnat_flow_index(struct sk_buff* skb)
{
	int index = -1; 
	
	if (hwnat_skb_to_foe_hook && skb)
		index = hwnat_skb_to_foe_hook(skb);

	return index;
}

/**************************************************
Function: Clean the HW accelebration given
          the multicast group
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0: ok
*************************************************/
int igmp_hwnat_delete_foe(IGMP_HWNATEntry_t* entry)
{
	if (igmp_hwnat_check_entry_valid(entry)==0)
	{
		IGMP_HWNAT_DEBUG("entry is not valid");
		return 0;
	}
	
	IGMP_HWNAT_DEBUG("index = %d ,mask = %x",entry->index,entry->mask);

	if (hwnat_delete_foe_entry_hook)
		hwnat_delete_foe_entry_hook(entry->index);

	return 0;
}

/**************************************************
Function: Delete the SW and HW maintained
          multicast flow
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0: ok
**************************************************/
int igmp_hwnat_delete_flow(IGMP_HWNATEntry_t* entry)
{
	if (entry == NULL)
	{
		IGMP_HWNAT_DEBUG("entry == NUL");
		return 0;
	}
	
	IGMP_HWNAT_DEBUG("entry index = %d",entry->index);	
	
	igmp_hwnat_delete_foe(entry);
	igmp_hwnat_delete_entry(entry);
	return 0;
}

/**************************************************
Function: Clear all the SW and HW maintained
          multicast flow
Input: 
	N/A
Return: 
	0: ok
**************************************************/
int igmp_hwnat_clear_flows(void)
{
	IGMP_HWNATEntry_t* entry = NULL,*tmp = NULL;
	struct list_head* hwnat_flow = igmp_hwnat_get_list();
	IGMP_LOCALEntry_t* entry_local = NULL, *tmp_local = NULL;
	struct list_head* hwnat_flow_local = igmp_hwnat_get_list();
	
	IGMP_HWNAT_DEBUG("enter");
	
	spin_lock_bh(&hwnat_lock);
#ifdef TCSUPPORT_MULTICAST_SPEED
	list_for_each_entry_rcu(entry,hwnat_flow,list)
#else
	list_for_each_entry_safe(entry,tmp,hwnat_flow,list)
#endif
	{
		igmp_hwnat_delete_flow(entry);
	}
	spin_unlock_bh(&hwnat_lock);
	
	spin_lock_bh(&hwnat_local_lock);
#ifdef TCSUPPORT_MULTICAST_SPEED
	list_for_each_entry_rcu(entry_local,hwnat_flow_local,list)
#else
	list_for_each_entry_safe(entry_local,tmp_local,hwnat_flow_local,list)
#endif
	{
		list_del(&entry_local->list);
		kfree(entry_local);
	}
	spin_unlock_bh(&hwnat_local_lock);
	
	return 0;
}
EXPORT_SYMBOL(igmp_hwnat_clear_flows);

/**************************************************
Function: Find the SW maintained entry given 
          foe entry index
Input: 
	index: foe entry index
Return: 
	0: ok
**************************************************/
IGMP_HWNATEntry_t* igmp_hwnat_find_entry_rcu(int index)
{
	IGMP_HWNATEntry_t* entry = NULL;
	struct list_head* hwnat_flow = igmp_hwnat_get_list();
	
	rcu_read_lock();

	list_for_each_entry_rcu(entry,hwnat_flow,list)
	{
		if (entry->index == index)
		{
			rcu_read_unlock();
			return entry;
		}
	}

	rcu_read_unlock();
	return NULL;
}

/**************************************************
Function: Find the SW maintained entry given 
          foe entry index
Input: 
	index: foe entry index
Return: 
	0: ok
**************************************************/
IGMP_HWNATEntry_t* igmp_hwnat_find_entry(int index)
{
	IGMP_HWNATEntry_t* entry = NULL;
    
#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
    if(0 == get_multicast_snooping_state_by_index(index))
    {
        return NULL;
    }
#endif

	struct list_head* hwnat_flow = igmp_hwnat_get_list();

	list_for_each_entry_rcu(entry,hwnat_flow,list)
	{
		if (entry->index == index)
		{
			return entry;
	}
	}
	
	return NULL;
}

/**************************************************
Function: Callback function to check if the HW 
          mulitcast flow is still valid
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0: ok
**************************************************/
void igmp_hwnat_timer_timeout(unsigned long arg)
{
	IGMP_HWNATEntry_t* entry = (IGMP_HWNATEntry_t* )arg;
	IGMP_HWNAT_DEBUG("enter");

	if (entry)
	{
		if (igmp_hwnat_check_entry_valid(entry)==0)
		{
			spin_lock(&hwnat_lock);
			igmp_hwnat_delete_entry(entry);	
			spin_unlock(&hwnat_lock);
		}
		else
		{
			mod_timer(&entry->age_timer,round_jiffies(jiffies) + hwnat_age_time);
		}
	}
	return;
}

/**************************************************
Function: Add a HW accelebrated multicast flow into
          SW maintained entry
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0: ok
***************************************************/
IGMP_HWNATEntry_t* igmp_hwnat_add_flow(struct sk_buff* skb ,int proto,unsigned char* grp_addr, unsigned char* src_addr)
{
	IGMP_HWNATEntry_t* entry = NULL;
	struct list_head* hwnat_flow = igmp_hwnat_get_list();
	int index = igmp_hwnat_flow_index(skb);
	unsigned int local = 0;

	IGMP_HWNAT_DEBUG("index = %d",index);

	if (index < 0)
		return NULL;

	entry = (IGMP_HWNATEntry_t* )igmp_hwnat_alloc(sizeof(IGMP_HWNATEntry_t));
	
	if (entry==NULL)
	{
		IGMP_HWNAT_DEBUG("alloc entry fail");
		return NULL;
	}
	entry->index = index;
	entry->mask = 0;
	entry->proto = proto;
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	if(ra_sw_nat_get_mul_br_vid_hook)
		entry->br_vid = ra_sw_nat_get_mul_br_vid_hook(skb);
#endif
#if defined(TCSUPPORT_PORTBIND)
#if defined(TCSUPPORT_CT)
	entry->orig_dev = skb->orig_dev;
#else
	entry->orig_dev = skb->dev;
#endif
#endif
	
	memcpy(entry->grp_addr,grp_addr,16);
	memcpy(entry->src_addr,src_addr,16);
	entry->m_vlan = VTAG_GET_VID(skb->pon_vlan_tci[0]);

	IGMP_HWNAT_DEBUG(" src_vid %d.\n", entry->m_vlan);
	
	local = igmp_hwnat_multicast_get_local(entry->grp_addr);
	entry->local = local;
	
#ifdef TCSUPPORT_MULTICAST_SPEED
	list_add_tail_rcu(&entry->list, hwnat_flow);	
#else
	list_add_tail(&entry->list,hwnat_flow);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	setup_timer(&entry->age_timer, igmp_hwnat_timer_timeout, (unsigned long)entry);	
#else
	timer_setup(&entry->age_timer, igmp_hwnat_timer_timeout, (unsigned long)entry);
#endif
	mod_timer(&entry->age_timer,round_jiffies(jiffies) + hwnat_age_time);
	
	return entry;
}

/**************************************************
Function: Clear all the SW and HW maintained
          multicast flow
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0: ok
***************************************************/
int igmp_hwnat_update_flow(IGMP_HWNATEntry_t* entry ,int mask)
{
	IGMP_HWNAT_DEBUG("index = %d ,mask = %d ",entry->index,mask);
	
	if (hwnat_set_special_tag_hook)
		hwnat_set_special_tag_hook(entry->index,mask);

	entry->mask = mask;
	
	return 0;
}

/**************************************************
Function: Enable hwnat wlan multicast 
          accelebration give a multicast group
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0: ok
**************************************************/
int igmp_hwnat_open_wlan(IGMP_HWNATEntry_t* entry)
{
	
	IGMP_HWNAT_DEBUG("enter");

	if (hwnat_set_wlan_multicast_hook)/*always NULL*/
		hwnat_set_wlan_multicast_hook(entry->index,1);
	
	return 0;
}

/**************************************************
Function: Disble hwnat wlan multicast 
          accelebration give a multicast group
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
Return: 
	0: ok
**************************************************/
int igmp_hwnat_close_wlan(IGMP_HWNATEntry_t* entry)
{
	IGMP_HWNAT_DEBUG("enter");

	if (hwnat_set_wlan_multicast_hook)/*always NULL*/
		hwnat_set_wlan_multicast_hook(entry->index,0);
	
	return 0;
}

/**************************************************
Function: update hw_nat mask to hw&sw entry, update sw wifinum
Input: 
	entry: pointer to struct IGMP_HWNATEntry_t
	mask: port mask
	state: port state, lan? wlan?
Return: 
	0: ok
**************************************************/
int igmp_hwnat_update_hw_nat_info(IGMP_HWNATEntry_t* entry,unsigned long long mask,unsigned int local,int state)
{
	int i;
	unsigned long long wlanmask;
	unsigned long long masktemp = mask;
	entry->wifinum = 0;
	IGMP_HWNAT_DEBUG("state=%d,mask=%llu,local:%d",state,mask,local);
	switch(state)
	{
		//only state i and state iii need to know how much wifi interfaces
		case MULTICAST_SPEED_STATE_I:
		case MULTICAST_SPEED_STATE_III:
			//for multi ssid speed
			wlanmask = (masktemp>>HWNAT_WLAN_IF_BASE) & HWNAT_WLAN_IF_MASK;
			for(i = 0;i < HWNAT_WLAN_IF_MAXNUM; i++)
			{
				if(wlanmask&(1 << i))
					entry->wifinum++;
			}
			break;

		//fall through and do nothing
		case MULTICAST_SPEED_STATE_II:
		default:
			break;	
	}

	if(hwnat_set_rule_according_to_state_hook)
		hwnat_set_rule_according_to_state_hook(entry->index,state,mask);
	
	entry->mask = mask;
	entry->local = local;

	return 0;
}


/**************************************************
Function: update switch vlan active 
         
Input: 
	br:pointer to struct net_bridge
	entry: pointer to struct net_bridge
Return: 
	0: ok
***************************************************/
void igmp_hwnat_1toN_vlan(struct net_bridge *br,IGMP_HWNATEntry_t* entry)
{
	int i,port=0,switch_port=0,multi_vlan_flag=0;
	int lan_vid=VBIND_INVALID_VLANID;
	u16 src_vid = entry->m_vlan;
	ECNT_SWITCH_VLAN_MODE vlan_mode;
	struct net_bridge_port *p = NULL;
	unsigned char switch_port_id = 0;

	if (!hwnat_igmp_1ton)
	{
		IGMP_HWNAT_DEBUG("hwnat_igmp_1ton is not set");
		return;

	}
	vlan_mode = 1;//PROT_VLSN_SWAP

	list_for_each_entry(p, &br->port_list, list)
	{
		if (igmp_hwnat_should_deliver(br,p,entry)==0)
			continue;	
		
#if defined(TCSUPPORT_CT)
	#ifdef TCSUPPORT_PORTBIND
		if(portbind_check_mc_hook)
		{
			if (portbind_check_mc_hook(entry->orig_dev,p->dev) == 0)
				continue;
		}
	#endif
#endif
		
		port = igmp_hwnat_get_port(p);
		if(-1 == port)
			continue;
		IGMP_HWNAT_DEBUG("port %d \n", port);
		ETHER_API_GET_PORTMAP(port,&switch_port_id);

		lan_vid = igmp_vbind_get_lanvid(port,entry);
		if(VBIND_INVALID_VLANID == lan_vid)
			continue;

		IGMP_HWNAT_DEBUG("port %d src_vid %d tmp_vid %d.\n", switch_port_id,src_vid, lan_vid);
		ETHER_API_PER_VLAN_ACTION(switch_port_id, src_vid, lan_vid, vlan_mode, 1);
	}

	if((hwnat_set_multicast_vlan_hook)&&(VBIND_INVALID_VLANID != lan_vid)){
		IGMP_HWNAT_DEBUG("hwnat_set_multicast_vlan_hook vid:%d\n", src_vid);
		hwnat_set_multicast_vlan_hook(entry->index, src_vid, 1);
	}

	return;
}

/**************************************************
Function: Sync the SW maintained multicast 
          flow with HW maintained flow
Input: 
	entry: pointer to struct net_bridge
Return: 
	0: ok
***************************************************/
int igmp_hwnat_update_all(void)
{
	IGMP_HWNATEntry_t* entry = NULL,*temp = NULL;
	struct list_head* hwnat_flow = igmp_hwnat_get_list();
	struct HwnatBrList *hwnat_br = NULL;
	struct HwnatBrList *tmp = NULL;
	unsigned long long mask=0,old_mask;
	unsigned int local = 0;
	unsigned int old_local = 0;
#ifdef TCSUPPORT_MULTICAST_SPEED
	int interfaceflag;
	unsigned long long masktemp;
#endif
	if (igmp_hwnat_enable()==0)
		return 0;

	IGMP_HWNAT_DEBUG("enter");

	spin_lock(&hwnat_lock);

#ifdef TCSUPPORT_MULTICAST_SPEED
	list_for_each_entry_rcu(entry,hwnat_flow,list)
#else
	list_for_each_entry_safe(entry,temp,hwnat_flow,list)
#endif
	{
		if (igmp_hwnat_check_entry_valid(entry)==0)/*check if is a valid hw_nat entry*/
		{
			igmp_hwnat_delete_entry(entry);
			continue;
		}
		mask= 0;
		spin_lock(&hwnat_br_lock);
		list_for_each_entry_safe(hwnat_br,tmp,&hwnat_br_table,list)
		{
			mask |= igmp_hwnat_port_mask(hwnat_br->br,entry);
		}
		spin_unlock(&hwnat_br_lock);
		
		local = igmp_hwnat_multicast_get_local(entry->grp_addr);
		
		old_local = entry->local;
		old_mask = entry->mask;
		if ((mask != old_mask) || (old_local != local))
		{
#ifdef TCSUPPORT_MULTICAST_SPEED
			masktemp = mask;
		
			interfaceflag = (masktemp & HWNAT_LAN_IF_MASK) >0?1:0;//compute lan interface				
			interfaceflag |= (((masktemp>>HWNAT_WLAN_IF_BASE) & HWNAT_WLAN_IF_MASK) >0?1:0)<<1;//compute wlan interface
			interfaceflag |= ((local == 1)?1:0)<<1; /*if local, treat as wlan*/
			switch (interfaceflag)
			{
				case MULTICAST_SPEED_STATE_I:
					igmp_hwnat_update_hw_nat_info(entry,mask,local,MULTICAST_SPEED_STATE_I);	/*reset special tag*/
				break;

				case MULTICAST_SPEED_STATE_II:
					igmp_hwnat_update_hw_nat_info(entry,mask,local,MULTICAST_SPEED_STATE_II);
				break;
				
				case MULTICAST_SPEED_STATE_III:
					igmp_hwnat_update_hw_nat_info(entry,mask,local,MULTICAST_SPEED_STATE_III);
				break;

				case MULTICAST_SPEED_STATE_IV:
					igmp_hwnat_delete_flow(entry);
				break;

				default://do nothing
					break;					
			}	
#else
			if ((mask&0x0f)==0)
				igmp_hwnat_delete_flow(entry);

			if ((mask&0x0f) != (old_mask&0x0f) && (mask&0x0f)>0)
				igmp_hwnat_update_flow(entry,mask);
			
			if (((mask>>8)&0x0f) > 0 && ((old_mask>>8)&0x0f) == 0)
				igmp_hwnat_open_wlan(entry);

			if (((mask>>8)&0x0f) == 0 && ((old_mask>>8)&0x0f) > 0)
				igmp_hwnat_close_wlan(entry);
#endif
		}
			
		spin_lock(&hwnat_br_lock);
		list_for_each_entry_safe(hwnat_br,tmp,&hwnat_br_table,list)
		{
			igmp_hwnat_1toN_vlan(hwnat_br->br,entry);
		}
		spin_unlock(&hwnat_br_lock);
	}

	spin_unlock(&hwnat_lock);

	return 0;
}

/**************************************************
Function: Check if the addr is multicast addr 
Input: 
	entry: pointer to mac address 
Return: 
	0: fail; 1: ok
***************************************************/
int igmp_hwnat_is_flow_pkt(char* dst)
{
	char mac[3] = {0x01,0x00,0x5e};
		
	if (memcmp(dst,mac,3)==0)
		return 1;
	
	if (dst[0]==0x33 && dst[1] == 0x33)
		return 1;
	
	return 0;
}


/**************************************************
Function: SW learn the multicast flow from
          passed skb by hw_nat module
Input: 
	entry: pointer to struct sk_buff
Return: 
	0: ok
***************************************************/
int igmp_hwnat_learn_flow(struct sk_buff* skb)
{
	unsigned char dest_addr[16],src_addr[16];
	short int proto = 0;
	struct iphdr*  ih;
	struct ipv6hdr* i6h;
	unsigned short eth_type;
	unsigned char* buff = skb_mac_header(skb)+18;
	IGMP_HWNATEntry_t* entry = NULL;
	int index;
#ifdef TCSUPPORT_MULTICAST_SPEED	
	struct ethhdr *eth;
	struct vlan_ethhdr *vlan_eth;
	char *dst;
#endif

	if (igmp_hwnat_enable()==0)
		return 0;

#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
    if(0 == get_multicast_snooping_state_by_skb(skb))
    {
        return 0;
    }
#endif

	IGMP_HWNAT_DEBUG("name=%s",skb->dev->name);

#ifdef TCSUPPORT_MULTICAST_SPEED
	if(!isEN7580 && (strncmp(skb->dev->name,"eth", 3) == 0))
	{
		vlan_eth = vlan_eth_hdr(skb);
		buff = skb_mac_header(skb)+ ETH_HLEN + VLAN_HLEN;
		if(vlan_eth)
		{
			dst = vlan_eth->h_dest;
			eth_type = vlan_eth->h_vlan_encapsulated_proto;
		}
		else
		{
			printk("\r\n%s:vlan_eth == NULL,return",__FUNCTION__);
			return 0;
		}
	}
	else
	{
		eth =(struct ethhdr *)skb->data;
		
		if(eth)
		{
			dst = eth->h_dest;
			if(eth->h_proto == htons(ETH_P_8021Q) || 
				eth->h_proto == htons(ETH_P_8021AD))
			{
				vlan_eth = vlan_eth_hdr(skb);
				buff = skb_mac_header(skb)+ ETH_HLEN + VLAN_HLEN;
				eth_type = vlan_eth->h_vlan_encapsulated_proto;
			}
			else
			{
				buff = skb_mac_header(skb)+ ETH_HLEN;
				eth_type = eth->h_proto;
			}
		}
		else
		{
			printk("\r\n%s:eth == NULL,return",__FUNCTION__);
			return 0;
		}	
	}
	
	if (igmp_hwnat_is_flow_pkt(dst)==0)
	{
		IGMP_HWNAT_DEBUG("hw nat rule not match pkt");
		return 0;
	}
#else
	if (igmp_hwnat_is_flow_pkt(vlan_eth_hdr(skb)->h_dest)==0)
		return 0;
	
	eth_type = vlan_eth_hdr(skb)->h_vlan_encapsulated_proto;
#endif	

#if defined(TCSUPPORT_CT_SIMCARD_SEPARATION) && defined(TCSUPPORT_CT_2PORTS) 
	IGMP_HWNAT_DEBUG("eth_type=%x",ntohs(eth_type));
#else
	IGMP_HWNAT_DEBUG("eth_type=%x",ntohs(eth_type));
#endif
	memset(dest_addr,0,16);
	memset(src_addr,0,16);
	
	if (eth_type==htons(ETH_P_IP))
	{
		proto = MUL_PROTO_IGMP;
		ih = (struct iphdr*)buff;
		memcpy(dest_addr,&ih->daddr,4);
		memcpy(src_addr,&ih->saddr,4);
	}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	else if(eth_type==htons(ETH_P_IPV6))
	{
		proto = MUL_PROTO_MLD;
		i6h = (struct ipv6hdr*)buff;
		memcpy(dest_addr, i6h->daddr.s6_addr,16);
		memcpy(src_addr, i6h->saddr.s6_addr,16);	
	}
#endif
	else
	{
		return  0;
	}

	index = igmp_hwnat_flow_index(skb);
	
	spin_lock(&hwnat_lock);
#ifdef TCSUPPORT_MULTICAST_SPEED
	entry = igmp_hwnat_find_entry_rcu(index);
#else
	entry = igmp_hwnat_find_entry(index);	
	#endif

	if (entry != NULL)
		igmp_hwnat_delete_entry(entry);

	igmp_hwnat_add_flow(skb,proto,dest_addr,src_addr);
	
	spin_unlock(&hwnat_lock);

	igmp_hwnat_update_all();

	return 0;
}


/**************************************************
Function: Clean all dropped multicast flow by HW
Input: 
	N/A
Return: 
	0: ok
***************************************************/
int igmp_hwnat_multicast_undrop(void)
{
	if (wan_multicast_undrop_hook)
		wan_multicast_undrop_hook();
		
	return 0;
}

int igmp_hwnat_multicast_undrop_by_grpip(unsigned char is_ipv6,unsigned char* grp_ip)
{
	if (wan_multicast_undrop_by_grpip_hook)
		wan_multicast_undrop_by_grpip_hook(is_ipv6,grp_ip);
		
	return 0;
}
int igmp_vbind_get_lanvid( int port,IGMP_HWNATEntry_t* entry)
{
	int  idx = 0, j = 0;
	u16 lVlanId = VBIND_INVALID_VLANID;
#if defined(TCSUPPORT_CT_VLAN_BIND)

	/* vlan bind is ON. */
	if ( vlanbind_active_hook 
		&& ( 0 != vlanbind_active_hook() ) )
	{
		/* packets TO LAN port and it is vlan binded. */
		if ( vlanbind_entry_active_hook
			&& 0 != vlanbind_entry_active_hook(port+1) )
		{
		
			for ( idx = 0; idx < MAX_VLAN_GROUP; idx ++ )
			{
				if ( VBIND_INVALID_VLANID
					== vBindArray[port][idx].lVlanId )
						break;
				if ( check_mulif_tci_hook)
				{
					if (check_mulif_tci_hook(vBindArray[port][idx].wVlanId,entry->m_vlan) )
					/* the first vbind vlan */
						lVlanId = vBindArray[port][idx].lVlanId;
					break;
					}
			}
				if ( lVlanId < VBIND_INVALID_VLANID )
				
					return lVlanId;
			}								
		}
#endif

	return VBIND_INVALID_VLANID;
}

int igmp_hwnat_multicast_get_local(unsigned char* grp_addr)
{
	IGMP_LOCALEntry_t* entry = NULL;
	struct list_head* hwnat_local_flow = igmp_hwnat_get_local_list();

	spin_lock(&hwnat_local_lock);
	list_for_each_entry(entry,hwnat_local_flow,list)
	{
		if(memcmp(entry->grp_addr,grp_addr,4))
			continue;

        spin_unlock(&hwnat_local_lock);
		return 1;
	}
	spin_unlock(&hwnat_local_lock);
	return 0;
}

int igmp_hwnat_multicast_update_local(unsigned char* grp_addr, unsigned int proto,unsigned int update_mode)
{
	IGMP_LOCALEntry_t* entry = NULL;
	IGMP_LOCALEntry_t* tmp = NULL;
	struct list_head* hwnat_local_flow = igmp_hwnat_get_local_list();
	int find_flag = 0;

	spin_lock(&hwnat_local_lock);
	list_for_each_entry_safe(entry,tmp,hwnat_local_flow,list)
	{
		if(0 == memcmp(entry->grp_addr,grp_addr,4))
        {
            find_flag = 1;
            if (update_mode)
            {
                entry->ref_cnt++;
            }
            else
            {
                entry->ref_cnt--;
                if (entry->ref_cnt <= 0)
                {
                    list_del(&entry->list);
                    kfree(entry);
                }
            }
            spin_unlock(&hwnat_local_lock);
            return 0;
        }
	}

    if (update_mode && !find_flag)
    {
        entry = (IGMP_LOCALEntry_t* )kmalloc(sizeof(IGMP_LOCALEntry_t),GFP_ATOMIC);
        if(!entry)
        {
            printk("Error: Not enought memory!\n");
            spin_unlock(&hwnat_local_lock);
            return -1;
        }

        memset(entry,0,sizeof(IGMP_LOCALEntry_t));
		memmove(entry->grp_addr,grp_addr,4);
		entry->proto = proto;
		
        entry->ref_cnt = 1;
        list_add_tail(&entry->list,hwnat_local_flow);
    }
	spin_unlock(&hwnat_local_lock);

	return 0;
}

int igmp_hwnat_multicast_subcribe_group(unsigned char* grp_addr, unsigned int proto, unsigned int update_mode)
{	
    if (strcmp(current->comm, "udpxy"))
    {
        return 0;
    }

    if (igmp_hwnat_multicast_update_local(grp_addr, proto, update_mode))
    {
        return 0;
    }

  	igmp_hwnat_update_all();

    return 0;
}

int igmp_hwnat_read_proc(char *buf, char **start, off_t off, int count,int *eof, void *data)
{
	int len = 0;
	IGMP_HWNATEntry_t* entry = NULL;
	IGMP_LOCALEntry_t* entry_local = NULL;
	struct list_head* hwnat_flow = igmp_hwnat_get_list();
	struct list_head* hwnat_flow_local = igmp_hwnat_get_local_list();
#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
    multicast_flood_hwentry_t* flood_entry = NULL;
    multicast_flood_hwentry_t* ptr = NULL;
#endif

	len = sprintf(buf,"flag = %d  time = %d 1tonflag = %d\n",hwnat_igmp_flag,hwnat_age_time,hwnat_igmp_1ton);
#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
	len += sprintf(buf+len,"hw_igmp_flood_enable = %d.\n",hw_igmp_flood_enable);
    spin_lock_bh(&hwnat_disable_snooping_lock);
    list_for_each_entry_safe(flood_entry, ptr, &multicast_flood_hw_list, list)
    {
        len += sprintf(buf+len,"flood index = %d   port_mask = 0x%lX.\n",flood_entry->index, flood_entry->port_mask);
    }
    spin_unlock_bh(&hwnat_disable_snooping_lock);
#endif

	len += sprintf(buf+len,"index  type  mask  wlannum	grp_addr   src_addr m_vlan	local\n");
#ifdef TCSUPPORT_MULTICAST_SPEED
	rcu_read_lock();
#else
	spin_lock(&hwnat_lock);
#endif

#ifdef TCSUPPORT_MULTICAST_SPEED
	list_for_each_entry_rcu(entry,hwnat_flow,list)
#else
	list_for_each_entry(entry,hwnat_flow,list)
#endif
	{
#ifdef TCSUPPORT_MULTICAST_SPEED
		if(MUL_PROTO_IGMP == entry->proto)
		{
			len += sprintf(buf+len,"%d	%d	0x%lX	%d	%u.%u.%u.%u   %u.%u.%u.%u  %d	%d\n",entry->index,entry->proto,entry->mask,entry->wifinum
							,entry->grp_addr[0],entry->grp_addr[1],entry->grp_addr[2],entry->grp_addr[3],
							entry->src_addr[0],entry->src_addr[1],entry->src_addr[2],entry->src_addr[3],entry->m_vlan,entry->local);
		}
		else
		{
			len += sprintf(buf+len,"%d	%d	0x%lX	%d	%s	 %s\n",entry->index,entry->proto,entry->mask,entry->wifinum
							,ip6_sprintf((struct in6_addr*)(entry->grp_addr)),ip6_sprintf((struct in6_addr*)(entry->src_addr)));
		}
#else
		len += sprintf(buf+len,"%d  %d  0x%lX  %d  %u.%u.%u.%u   %u.%u.%u.%u  \n",entry->index,entry->proto,entry->mask,entry->wifinum
							,entry->grp_addr[0],entry->grp_addr[1],entry->grp_addr[2],entry->grp_addr[3],
							entry->src_addr[0],entry->src_addr[1],entry->src_addr[2],entry->src_addr[3]);	
#endif	
	}
#ifndef TCSUPPORT_MULTICAST_SPEED
	spin_unlock(&hwnat_lock);
#else
	rcu_read_unlock();
#endif

    if (!list_empty(hwnat_flow_local))
    {
		len += sprintf(buf+len,"\n------------------------------------------\n");
    	list_for_each_entry(entry_local,hwnat_flow_local,list)
        {
			len += sprintf(buf+len,"%u.%u.%u.%u, ref_cnt: %d \n",entry_local->grp_addr[0], entry_local->grp_addr[1],
                entry_local->grp_addr[2], entry_local->grp_addr[3],entry_local->ref_cnt);	
        }
    }

	*start = buf + off;
	if (len < off + count)
		*eof = 1;
	len -= off;
	if (len > count)
		len = count ;
	if (len <0)
		len = 0;
		
	return len;

}
static int igmp_hwnat_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char buff[32],cmd[16];
	int len,flag;
    int flood_flag = 0;
	
	if (count >= (sizeof(buff) - 1))
		len = sizeof(buff) - 1;
	else
		len = count;

	memset(buff,0,32);
	memset(cmd,0,16);
	if (copy_from_user(buff, buffer, len))
		return -EFAULT;

	sscanf(buff,"%s %d",cmd,&flag);
#ifdef TCSUPPORT_MULTICAST_SPEED
	if (memcmp(cmd,"switch",4)==0)
	{
		hwnat_igmp_flag &= 0xfffffffe;
		hwnat_igmp_flag |= ((flag > 0? 1:0));
	}
	
	if (memcmp(cmd,"debug",4)==0)
	{
		hwnat_igmp_flag &= 0xfffffffd;
		hwnat_igmp_flag |= (((flag > 0? 1:0)) << 1);
	}
	
	if (memcmp(cmd,"default",4)==0)
	{
		hwnat_igmp_flag = 1;
		hwnat_age_time =3000;
	}
#endif
	if (memcmp(cmd,"flag",4)==0)
		hwnat_igmp_flag = flag;

	if (memcmp(cmd,"1tonflag",8)==0)
		hwnat_igmp_1ton = flag;

	if (memcmp(cmd,"time",4)==0)
		hwnat_age_time = flag;

#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
    if (memcmp(cmd,"m_flood_hw",10)==0)
    {
        if(flag)
        {
            flood_flag = 1;
        }
        else
        {
            flood_flag = 0;
        }

        hw_igmp_flood_enable = flood_flag;
        if(hwnat_set_multicast_speed_enable_hook)
        {
            printk("flood_flag = %d, func = %s, line = %d.\n", flood_flag, __FUNCTION__,__LINE__);
            hwnat_set_multicast_speed_enable_hook(flood_flag);
        }
    }
#endif

	return len;
}

void add_multicast_flood_hwentry(struct sk_buff* skb)
{
    int index = -1;
    multicast_flood_hwentry_t* entry = NULL;
    multicast_flood_hwentry_t* del_entry = NULL;
    multicast_flood_hwentry_t* ptr = NULL;
    int flood_flag = false;
	unsigned long flags;
    
    if(0 == hw_igmp_flood_enable)
    {
        return ;
    }

    flood_flag = get_multicast_snooping_state_by_skb(skb) ;
    if(flood_flag)
    {
        return ;
    }

    index = igmp_hwnat_flow_index(skb);
    if(0 > index )
    {
        return;
    }

    entry = (multicast_flood_hwentry_t* )igmp_hwnat_alloc(sizeof(multicast_flood_hwentry_t));
    if (NULL == entry)
    {
        return ;
    }
    spin_lock_bh(&hwnat_disable_snooping_lock);
    list_for_each_entry_safe(del_entry, ptr, &multicast_flood_hw_list, list)
    {
        if(del_entry->index == index)
        {
            list_del(&del_entry->list);
            kfree(del_entry);
            del_entry = NULL;
        }
    }
    
    entry->index = index;
    entry->port_mask = 0;

    list_add_tail(&entry->list, &multicast_flood_hw_list);
    
    spin_unlock_bh(&hwnat_disable_snooping_lock);
    
    return ;
}
void update_multicast_flood_hwentry(int index, unsigned long long mask)
{
    multicast_flood_hwentry_t* entry = NULL;
    multicast_flood_hwentry_t* ptr = NULL;
    int flood_flag = false;
	unsigned long flags;
    
    if(0 == hw_igmp_flood_enable)
    {
        return ;
    }

    flood_flag = get_multicast_snooping_state_by_index(index);
    if(flood_flag)
    {
        return ;
    }
    
    if(0 >= index)
    {
        return ;
    }

    spin_lock_bh(&hwnat_disable_snooping_lock);

    list_for_each_entry_safe(entry, ptr, &multicast_flood_hw_list, list)
    {
        if(entry->index == index)
        {
            entry->port_mask |= mask;
        }
    }

    spin_unlock_bh(&hwnat_disable_snooping_lock);

    return ;
}

multicast_flood_hwentry_t* find_multicast_flood_hwentry(int index)
{
    multicast_flood_hwentry_t* entry = NULL;
    multicast_flood_hwentry_t* ptr = NULL;
    int find_flag = 0;
    int flood_flag = false;
	unsigned long flags;
    
    if(0 == hw_igmp_flood_enable)
    {
        return NULL;
    }

    flood_flag = get_multicast_snooping_state_by_index(index) ;
    if(flood_flag)
    {
        return NULL;
    }
    
    if(0 >= index)
    {
        return NULL ;
    }
    
    spin_lock_bh(&hwnat_disable_snooping_lock);
    
    list_for_each_entry_safe(entry, ptr, &multicast_flood_hw_list, list)
    {
        if(entry->index == index)
        {
            find_flag = 1;
            goto out;
        }
    }
out:
    spin_unlock_bh(&hwnat_disable_snooping_lock);
    if(find_flag)
    {
        return entry;
    }
    
    return NULL;
}

/*update mask */
void update_multicast_flood_mask(int index)
{
    int wifi_flag = false;
    multicast_flood_hwentry_t* entry = NULL;
    multicast_flood_hwentry_t* ptr = NULL;
    int lan_flag = false;
    int flood_flag = 0;
	unsigned long flags;

#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
    return ;
#endif	    

    if(0 >= index)
    {
        return ;
    }
    
    if(0 == hw_igmp_flood_enable)
    {
        return ;
    }

    flood_flag = get_multicast_snooping_state_by_index(index) ;
    if(flood_flag)
    {
        return ;
    }

    spin_lock_bh(&hwnat_disable_snooping_lock);
    list_for_each_entry_safe(entry, ptr, &multicast_flood_hw_list, list)
    {
        if(entry->index == index)
        {
            lan_flag  |= ((entry->port_mask & HWNAT_LAN_IF_MASK) > 0 ? 1 : 0);
            wifi_flag |= (((entry->port_mask >> HWNAT_WLAN_IF_BASE) & HWNAT_WLAN_IF_MASK) > 0 ? 1 : 0);
            if(wifi_flag)
            {
                if(lan_flag)
                {
                    if(hwnat_set_rule_according_to_state_hook)
                        hwnat_set_rule_according_to_state_hook(index, MULTICAST_SPEED_STATE_I, entry->port_mask);
                }
                else
                {
                    if(hwnat_set_rule_according_to_state_hook)
                        hwnat_set_rule_according_to_state_hook(index, MULTICAST_SPEED_STATE_III, entry->port_mask);
                }
            }
            else
            {
                if(hwnat_set_rule_according_to_state_hook)
                    hwnat_set_rule_according_to_state_hook(index, MULTICAST_SPEED_STATE_II, entry->port_mask);
            }
            
        }
        else
        {
            if(multicast_flood_is_bind_hook && (0 == multicast_flood_is_bind_hook(entry->index)))
            {
                list_del(&entry->list);
                kfree(entry);
                entry = NULL;
            }
        }
    }

    spin_unlock_bh(&hwnat_disable_snooping_lock);

    return ;
}

int clear_multicast_flood_hwentry(void)
{
    multicast_flood_hwentry_t* entry = NULL;
    multicast_flood_hwentry_t* ptr = NULL;
    unsigned long flags;
    
    spin_lock_bh(&hwnat_disable_snooping_lock);
    list_for_each_entry_safe(entry, ptr, &multicast_flood_hw_list, list)
    {
        if (hwnat_delete_foe_entry_hook)
        {
            hwnat_delete_foe_entry_hook(entry->index);
        }
        list_del(&entry->list);
        kfree(entry);
        entry = NULL;
    }
    
    spin_unlock_bh(&hwnat_disable_snooping_lock);
	return 0;
}

#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
struct ecnt_hook_ops ecnt_hwnat_mc_undrop_ops = {
    .name = "igmp_hwnat_multicast_undrop",
    .hookfn = igmp_hwnat_multicast_undrop,
    .is_execute = 1,
    .maintype = ECNT_NET_IGMP,
    .subtype = ECNT_NET_IGMP_GROUP_ADDED,
};

struct ecnt_hook_ops ecnt_clear_flood_mc_ops = {
    .name = "clear_multicast_flood_hwentry",
    .hookfn = clear_multicast_flood_hwentry,
    .is_execute = 1,
    .maintype = ECNT_NET_IGMP,
    .subtype = ECNT_NET_IGMP_GROUP_DROPPED,
};
#endif

void igmp_hwnat_del_br(struct net_bridge *br)
{
	struct HwnatBrList *hwnat_br = NULL, *tmp = NULL;

	spin_lock_bh(&hwnat_br_lock);
	list_for_each_entry_safe(hwnat_br,tmp,&hwnat_br_table,list)
	{	
		if(br == hwnat_br->br)
		{
			list_del(&(hwnat_br->list));
			kfree(hwnat_br);
			hwnat_br = NULL;
			spin_unlock_bh(&hwnat_br_lock);
			return;
		}
	}
	spin_unlock_bh(&hwnat_br_lock);

	return;
}

void igmp_hwnat_add_br(struct net_bridge *br)
{
	struct HwnatBrList *hwnat_br = NULL, *tmp = NULL;

	hwnat_br = (struct HwnatBrList *)kmalloc(sizeof(struct HwnatBrList), GFP_KERNEL);

	if (hwnat_br == NULL){
		printk("igmp_hwnat_add_br: alloc fail\n");
		return ;
	}

	memset(hwnat_br,0,sizeof(struct HwnatBrList));
	hwnat_br->br = br;
	
	spin_lock_bh(&hwnat_br_lock);
	list_add(&(hwnat_br->list),&hwnat_br_table);
	spin_unlock_bh(&hwnat_br_lock);

	return;
}

#ifdef TCSUPPORT_HWNAT_V3
/*****for joyme4 feature******/
int checkipmode(struct sk_buff* skb, unsigned char* tmp_addr,unsigned short* proto)
{
	unsigned short eth_type = 0;
	unsigned char* buff = NULL;
	unsigned short* tmp = NULL;
	unsigned short pppoe_proto;
	unsigned short protocol = 0;

	buff = skb->data;

	protocol = eth_hdr(skb)->h_proto;
	if(protocol == htons(ETH_P_8021Q))
	{
		buff += 2;
	}
	eth_type = *(u16*)buff;
	if(eth_type == htons(ETH_P_8021Q))
	{
		buff += 4;
	}

	if((eth_type != htons(ETH_P_PPP_SES))&&(eth_type != htons(ETH_P_IP))&&(eth_type != htons(ETH_P_IPV6)))
		return -1;

	buff += 2; // skip ether type

	if (eth_type == htons(ETH_P_PPP_SES))
	{
		buff+= sizeof(struct pppoe_hdr);
		pppoe_proto = *(u_int16_t*)buff;

		buff += 2; // skip ppp header

		if (pppoe_proto == htons(PPP_IP))
			eth_type = htons(ETH_P_IP);
		else if (pppoe_proto == htons(PPP_IPV6))
			eth_type = htons(ETH_P_IPV6);
	}

	if (eth_type == htons(ETH_P_IP)){
		struct iphdr *iph = (struct iphdr*)buff;

		if (iph->version != 4)
			return -1;
		if(iph->protocol == 0x11)
		{
			memmove(tmp_addr, &(iph->daddr), 4);			
			*proto = htons(ETH_P_IP);
			return 0;
		}
	}
	else if (eth_type == htons(ETH_P_IPV6)){
		struct ipv6hdr *ip6hdr = (struct ipv6hdr*)buff;
		if (ip6hdr->version != 6)
			return -1;
		if(ip6hdr->nexthdr == 0x11){
			memmove(tmp_addr, &(ip6hdr->daddr), 16);			
			*proto = htons(ETH_P_IPV6);
			return 0;
		}
	}
	return -1;
}

void ecnt_multicast_add_tmp_info(unsigned char* address,u16 proto, short port_no, int ref_cnt_udpxy, unsigned char status_flag)
{
	unsigned char flow_status = 0;
	if(status_flag == 1)
	{		
		if(proto == MUL_PROTO_IGMP){
			struct MulIPtmpList* tmp;	
			struct MulIPtmpList* multmp;
			write_lock(&mul_stat_lock);
			list_for_each_entry(tmp, &mul_tmp_list, list)
			{
					if(!memcmp(tmp->grp_ip, address, 4))
					{
						if(tmp->mul_port_no == port_no)/*if repeat report pkt,drop it*/
						{
							write_unlock(&mul_stat_lock);
							return;
						}
						if(tmp->ref_cnt_udpxy != -1)
						{
							tmp->ref_cnt_udpxy = ref_cnt_udpxy;
							write_unlock(&mul_stat_lock);
							return;
						}
						if((tmp->mul_port_no != -1 && (ref_cnt_udpxy != -1 || port_no != -1 ))
							||(tmp->ref_cnt_udpxy != -1 && (port_no != -1 ||ref_cnt_udpxy != -1 )))
							{
								if(tmp->flow_status == 1)
									flow_status = 1;/*grp id is binded pkt , just confirm it*/
							}
					}						
			}
			multmp = (struct MulIPtmpList*)kmalloc(sizeof(struct MulIPtmpList), GFP_ATOMIC);
			if(multmp != NULL)
			{
				memmove(multmp->grp_ip, address, 4);
				multmp->mul_protocol = 1;
				multmp->mul_port_no = port_no;
				multmp->ref_cnt_udpxy = ref_cnt_udpxy;
				multmp->flow_status= flow_status;
				list_add_tail(&multmp->list, &mul_tmp_list);
			}
			write_unlock(&mul_stat_lock);
		}
		else if (proto == MUL_PROTO_MLD){
			struct MulIPtmpList* tmp;
			write_lock(&mul_stat_lock);
			list_for_each_entry(tmp, &mul_tmp_list, list)
			{				
				if(!memcmp(tmp->grp_ip, address, 16))
				{
					if(tmp->mul_port_no == port_no)
					{
						write_unlock(&mul_stat_lock);
						return;
					}
					if(tmp->mul_port_no != -1 &&  port_no != -1){
						if(tmp->flow_status == 1)
						flow_status = 1;
					}
				}
					
			}
			struct MulIPtmpList* multmp;
			multmp = (struct MulIPtmpList*)kmalloc(sizeof(struct MulIPtmpList), GFP_ATOMIC);
			if(multmp != NULL)
			{
				memmove(multmp->grp_ip, address, 16);
				multmp->mul_protocol = 2;
				multmp->mul_port_no = port_no;
				multmp->ref_cnt_udpxy = -1;
				multmp->flow_status = flow_status;
				list_add_tail(&multmp->list, &mul_tmp_list);
			}
			write_unlock(&mul_stat_lock);
		}
		status_flag = 0;
	}
	else if(status_flag == 2)
	{
		if(proto == MUL_PROTO_IGMP)
		{
			struct MulIPtmpList* multmp;
			struct MulIPtmpList* p;
			write_lock(&mul_stat_lock);
			list_for_each_entry_safe(multmp, p, &mul_tmp_list, list)
			{
					if(!memcmp(multmp->grp_ip, address, 4))
					{						
						if(multmp->mul_port_no == port_no || multmp->ref_cnt_udpxy == 0)
						{
							list_del(&multmp->list);
							kfree(multmp);
							multmp = NULL;
							write_unlock(&mul_stat_lock);
							return;
						}
					}
			}
			write_unlock(&mul_stat_lock);
		}
		else if(proto == MUL_PROTO_MLD)/*no udpxy for ipv6*/
		{
			struct MulIPtmpList* multmp;
			struct MulIPtmpList* p;
			write_lock(&mul_stat_lock);
			list_for_each_entry_safe(multmp, p, &mul_tmp_list, list)
			{
					if(!memcmp(multmp->grp_ip, address, 16))
					{						
						if(multmp->mul_port_no == port_no)
						{
							list_del(&multmp->list);
							kfree(multmp);
							multmp = NULL;
							write_unlock(&mul_stat_lock);
							return;
						}
					}
			}
			write_unlock(&mul_stat_lock);
		}		
		status_flag = 0;
	}
}

void ecnt_br_Mul_To_LAN_Config(struct sk_buff* skb)
{

	unsigned char tmp_addr[16];
	unsigned short proto;	
	struct MulIPtmpList* MulLIPtmp_p = NULL;
	
	const unsigned char *dest = eth_hdr(skb)->h_dest;	
	
	/*Return when the list is empty or the list is not initialized*/	
	if(list_empty(&mul_tmp_list) || ((&mul_tmp_list)->next == NULL)){
		return;
	}
	
	if(!is_broadcast_ether_addr(dest)&& is_multicast_ether_addr(dest))
	{
		if(checkipmode(skb, tmp_addr, &proto))
		{
			return;
		}
		write_lock(&mul_stat_lock);
		list_for_each_entry(MulLIPtmp_p, &mul_tmp_list, list)
		{		
			if(MulLIPtmp_p->mul_protocol == 1 && proto == htons(ETH_P_IP))
			{
				if(MulLIPtmp_p->flow_status == 0 && !memcmp(MulLIPtmp_p->grp_ip, tmp_addr, 4)){ /*unconfirm flow*/
					MulLIPtmp_p->flow_status = 1;					

				}
					
			}
			else if (MulLIPtmp_p->mul_protocol == 2 && proto == htons(ETH_P_IPV6))
			{
				if(MulLIPtmp_p->flow_status == 0 && !memcmp(MulLIPtmp_p->grp_ip, tmp_addr, 16)){ /*unconfirm flow*/
					MulLIPtmp_p->flow_status = 1;
				}
			}			
		}
		write_unlock(&mul_stat_lock);
	}
}
/*****for joyme4 feature******/
static inline int get_multicast_snooping_state_by_ip_version(int ip_ver)
{
#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
	#ifdef TCSUPPORT_SNOOPING_SEPERATION
	if(2 == ip_ver)
		return (g_snooping_enable>>BRCTL_IGMPSNOOPING_OFFSET)&1;
	else if(3 == ip_ver)
		return (g_snooping_enable>>BRCTL_MLDSNOOPING_OFFSET)&1;
	else
		return 0;
	#else
		return g_snooping_enable;
	#endif
#else
	return 0;
#endif
}

int ecnt_br_multicast_hwnat_update_entry(struct net_bridge_port *port,struct br_ip *group)
{
	IGMP_HWNATEntry_t entry;
	PPE_MULTICAST_INFO_t ppe_mcst_info;
	unsigned long long port_mask = 0;
	struct HwnatBrList *hwnat_br = NULL;
	struct HwnatBrList *tmp = NULL;
	int ip_ver = 0;

	if(!port||!group)
		return 0;
	
	memset(&entry,0,sizeof(IGMP_HWNATEntry_t));
	memset(&ppe_mcst_info,0,sizeof(PPE_MULTICAST_INFO_t));
	ip_ver = (4 == port->version)?2:3;
#ifndef TCSUPPORT_XPON_HAL_API_MCST
	entry.snoop_off = !get_multicast_snooping_state_by_ip_version(ip_ver);
#endif

	if(4 == port->version)
	{
		entry.proto = MUL_PROTO_IGMP;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
		memmove(entry.grp_addr,&(group->dst.ip4),4);
#else
		memmove(entry.grp_addr,&(group->u.ip4),4);
#endif    
		memmove(entry.src_addr,&(port->src_entry.src.s_addr),4);

		ppe_mcst_info.proto = PPE_MULTICAST_PROTO_IPV4;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
		memmove(ppe_mcst_info.grp_addr,&(group->dst.ip4),4);		
#else
		memmove(ppe_mcst_info.grp_addr,&(group->u.ip4),4);		
#endif    
		memmove(ppe_mcst_info.src_addr,&(port->src_entry.src.s_addr),4);
	}
	else
	{
		entry.proto = MUL_PROTO_MLD;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
		memmove(entry.grp_addr,group->dst.ip6.in6_u.u6_addr8,16);
#else
		memmove(entry.grp_addr,group->u.ip6.in6_u.u6_addr8,16);
#endif    
		memmove(entry.src_addr,port->src_entry.src6.in6_u.u6_addr8,16);

		ppe_mcst_info.proto = PPE_MULTICAST_PROTO_IPV6;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
		memmove(ppe_mcst_info.grp_addr,group->dst.ip6.in6_u.u6_addr8,16);		
#else
		memmove(ppe_mcst_info.grp_addr,group->u.ip6.in6_u.u6_addr8,16);	
#endif    
		memmove(ppe_mcst_info.src_addr,port->src_entry.src6.in6_u.u6_addr8,16);
	}
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	entry.br_vid = group->vid;
	ppe_mcst_info.br_vid = group->vid;
#endif    
    PPE_API_GET_MULTICAST_ORIGDEV(&ppe_mcst_info);
    entry.orig_dev = ppe_mcst_info.ori_dev;

	spin_lock(&hwnat_br_lock);
	list_for_each_entry_safe(hwnat_br,tmp,&hwnat_br_table,list)
	{
		port_mask |= igmp_hwnat_port_mask(hwnat_br->br, &entry);
	}
	spin_unlock(&hwnat_br_lock);
#ifndef TCSUPPORT_SEPERATED_ETH_ITF
	ppe_mcst_info.outer_tci = entry.m_vlan;
#endif
	PPE_API_MULTICAST_HWNATENTRY_LIST_UPDATE(&ppe_mcst_info,PPE_MULTICAST_UPDATE_MODE_GRPIP,0,port_mask,0);
	return 0;
}
unsigned long long ecnt_br_multicast_hwnat_get_portmask(struct ecnt_ppe_data *ppe_data)
{
	IGMP_HWNATEntry_t entry;
	unsigned int foe_index = ppe_data->index;
	int snoop_enable = get_multicast_snooping_state_by_index(foe_index);
	unsigned long long port_mask = 0;
	struct HwnatBrList *hwnat_br = NULL;
	struct HwnatBrList *tmp = NULL;
	
	memset(&entry,0,sizeof(IGMP_HWNATEntry_t));

#ifndef TCSUPPORT_XPON_HAL_API_MCST
	if(!snoop_enable)
		entry.snoop_off = 1;
#endif

	if(PPE_MULTICAST_PROTO_IPV4 == ppe_data->multicast_info->proto)
		entry.proto = MUL_PROTO_IGMP;
	else if(PPE_MULTICAST_PROTO_IPV6 == ppe_data->multicast_info->proto)
		entry.proto = MUL_PROTO_MLD;
	else
		return ECNT_HOOK_ERROR;

	memmove(entry.grp_addr,ppe_data->multicast_info->grp_addr,16);
	memmove(entry.src_addr,ppe_data->multicast_info->src_addr,16);

	entry.orig_dev = ppe_data->multicast_info->ori_dev;
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	entry.br_vid = ppe_data->multicast_info->br_vid;
#endif

	spin_lock(&hwnat_br_lock);
	list_for_each_entry_safe(hwnat_br,tmp,&hwnat_br_table,list)
	{
		port_mask |= igmp_hwnat_port_mask(hwnat_br->br, &entry);
	}
	spin_unlock(&hwnat_br_lock);

	return port_mask;
}

ecnt_ret_val ecnt_br_multicast_ppe_ex_hook_func(struct ecnt_data *in_data)
{
	struct ecnt_ppe_data *ppe_data = (struct ecnt_ppe_data *)in_data;

	if(PPE_MCST_EX_API_ID_GET_PORTMASK == ppe_data->function_id)
		ppe_data->retValue = ecnt_br_multicast_hwnat_get_portmask(ppe_data);

	return ECNT_CONTINUE;
}

struct ecnt_hook_ops ecnt_br_multicast_ppe_ex_op = {
	.name = "ecnt_br_multicast_ppe_ex_hook",
	.is_execute = 1,
	.hookfn = ecnt_br_multicast_ppe_ex_hook_func,
	.maintype = ECNT_PPE,
	.subtype = ECNT_DRIVER_PPE_API_MCST_EX,
	.priority = 1
};
/*****for joyme4 feature******/
unsigned int PPE_Mul_Status(void)
{

	if(list_empty(&mul_tmp_list))/* no flow download*/
		return 0;	
	struct MulIPtmpList* tmp = NULL;
	read_lock_bh(&mul_stat_lock);
	list_for_each_entry(tmp, &mul_tmp_list, list)
	{
		if(tmp->flow_status == 1){
			read_unlock_bh(&mul_stat_lock);
			return 1;
		}
	}
	read_unlock_bh(&mul_stat_lock);
	return 0;
}
#else
void ecnt_multicast_add_tmp_info(unsigned char* address,u16 proto, short port_no, int ref_cnt_udpxy, unsigned char status_flag)
{
	return;
}

void ecnt_br_Mul_To_LAN_Config(struct sk_buff* skb)
{
	return;
}

unsigned int PPE_Mul_Status(void)
{
	printk("Not Support this version\n");
	return 0;
}
#endif
/*****for joyme4 feature******/

#ifdef TCSUPPORT_MULTICAST_BSP_GENERAL
int ecnt_br_multicast_bsp_general_update_entry(struct net_bridge_port *port,struct br_ip *group,int opt)
{
	ECNT_MC_PORT_INFO pInfo;
	int port_id = 0;
	char grpMac[6]={0x01,0x00,0x5e,0x00,0x00,0x00};
	
	memset(&pInfo, 0, sizeof(ECNT_MC_PORT_INFO));

	port_id = igmp_hwnat_get_port(port);

	if(port_id < 0)
	{
		printk("line:%d,func:%s, error port id!\n",__LINE__,__FUNCTION__);
		return -1;
	}
	pInfo.port_id = port_id;
	pInfo.vlan_id = group -> vid;
	pInfo.vlan_action.vlan_mode = ECNT_VLAN_STRIP;

	if(4 == port->version)
	{
		pInfo.group_addr.is_ipv6 = 0;
		memmove(&(pInfo.group_addr.IP.ipv4_address),&(group->u.ip4),4);

		pInfo.src_addr.is_ipv6 = 0;
		memmove(&(pInfo.src_addr.IP.ipv4_address),&(port->src_entry.src.s_addr),4);

		grpMac[3]= ((char *)&(group->u.ip4))[1] & 0x7f;  //the 25th bit is equal to zero
		grpMac[4]= ((char *)&(group->u.ip4))[2] ;
		grpMac[5]= ((char *)&(group->u.ip4))[3] ;
		memcpy(&pInfo.group_mac,grpMac,6);
	}
	else
	{
		pInfo.group_addr.is_ipv6 = 1;
		memmove(&(pInfo.group_addr.IP.ipv6_address),group->u.ip6.in6_u.u6_addr8,16);

		pInfo.src_addr.is_ipv6 = 1;
		memmove(&(pInfo.src_addr.IP.ipv6_address),port->src_entry.src6.in6_u.u6_addr8,16);

		grpMac[0]=0x33;
		grpMac[1]=0x33;
		grpMac[2]=group->u.ip6.in6_u.u6_addr8[12];
		grpMac[3]=group->u.ip6.in6_u.u6_addr8[13];
		grpMac[4]=group->u.ip6.in6_u.u6_addr8[14];
		grpMac[5]=group->u.ip6.in6_u.u6_addr8[15];
		memcpy(&pInfo.group_mac,grpMac,6);
	}

	if(MTK_MULTICAST_ADD == opt)
		ECNT_HOOK_MC_API_ADD_ENTRY(&pInfo);
	else
		ECNT_HOOK_MC_API_DEL_ENTRY(&pInfo);

	return 0;

}
#endif

void igmp_hwnat_init(struct net_bridge *br)
{
	if(hwnat_proc == NULL)
	{
#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
		hwnat_igmp_flag = 0;
		hw_igmp_flood_enable = 0;
		multicast_xmit_packet_hook = ecnt_multicast_xmit_packet;
#endif
		INIT_LIST_HEAD(&hwnat_igmp_entry);
		spin_lock_init(&hwnat_lock);
		INIT_LIST_HEAD(&hwnat_br_table);
		spin_lock_init(&hwnat_br_lock);
		INIT_LIST_HEAD(&hwnat_igmp_local_entry);
		spin_lock_init(&hwnat_local_lock);
#if defined(TCSUPPORT_MULTICAST_SPEED)&&!defined(TCSUPPORT_HWNAT_V3)
		multicast_speed_learn_flow_hook = igmp_hwnat_learn_flow;
		multicast_speed_find_entry_hook	= igmp_hwnat_find_entry;
		multicast_subcribe_group_hook = igmp_hwnat_multicast_subcribe_group;
		multicast_get_local_hook = igmp_hwnat_multicast_get_local;

#endif

#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
		spin_lock_init(&hwnat_disable_snooping_lock);
#ifndef TCSUPPORT_HWNAT_V3
	    multicast_flood_find_entry_hook = find_multicast_flood_hwentry;
#endif
		ecnt_register_hook(&ecnt_hwnat_mc_undrop_ops);
		ecnt_register_hook(&ecnt_clear_flood_mc_ops);
#endif

#ifndef TCSUPPORT_HWNAT_V3
		xpon_igmp_learn_flow_hook = igmp_hwnat_learn_flow;/*for what? likely no use*/
#endif

		hwnat_proc = create_proc_entry("tc3162/igmp_hwnat", 0, NULL);
		hwnat_proc->read_proc = igmp_hwnat_read_proc;
		hwnat_proc->write_proc = igmp_hwnat_write_proc;
		INIT_LIST_HEAD(&mul_tmp_list);

#ifdef TCSUPPORT_HWNAT_V3
		ecnt_register_hook(&ecnt_br_multicast_ppe_ex_op);
#endif
	}
	igmp_hwnat_add_br(br);
	
	return;
}

void igmp_hwnat_fini(void)
{
	igmp_hwnat_clear_flows();
	remove_proc_entry("tc3162/igmp_hwnat",0);	

	#ifdef TCSUPPORT_HWNAT_V3
	ecnt_unregister_hook(&ecnt_br_multicast_ppe_ex_op);
	#endif
	
	return;
}
#else
int igmp_hwnat_get_port(struct net_bridge_port*  p)
{
	return 0 ;
}
int igmp_hwnat_should_deliver(struct net_bridge *br,struct net_bridge_port *port,IGMP_HWNATEntry_t* entry)
{
	return 0 ;
}
int igmp_hwnat_port_mask(struct net_bridge *br,IGMP_HWNATEntry_t* entry)
{
	return 0 ;
}
int igmp_hwnat_delete_entry(IGMP_HWNATEntry_t* entry)
{
	return 0 ;
}
int igmp_hwnat_check_entry_valid(IGMP_HWNATEntry_t* entry)
{
	return 0 ;
}
int igmp_hwnat_flow_index(struct sk_buff* skb)
{
	return 0 ;
}
int igmp_hwnat_delete_foe(IGMP_HWNATEntry_t* entry)
{
	return 0 ;
}
int igmp_hwnat_delete_flow(IGMP_HWNATEntry_t* entry)
{
	return 0 ;
}
int igmp_hwnat_clear_flows(void)
{
	return 0 ;
}
IGMP_HWNATEntry_t* igmp_hwnat_find_entry_rcu(int index)
{
	return NULL ;
}
IGMP_HWNATEntry_t* igmp_hwnat_find_entry(int index)
{
	return NULL ;
}
void igmp_hwnat_timer_timeout(unsigned long arg)
{
	return ;
}
IGMP_HWNATEntry_t* igmp_hwnat_add_flow(struct sk_buff* skb ,int proto,unsigned char* grp_addr, unsigned char* src_addr)
{
	return NULL ;
}
int igmp_hwnat_update_flow(IGMP_HWNATEntry_t* entry ,int mask)
{
	return 0 ;
}
int igmp_hwnat_open_wlan(IGMP_HWNATEntry_t* entry)
{
	return 0 ;
}
int igmp_hwnat_close_wlan(IGMP_HWNATEntry_t* entry)
{
	return 0 ;
}
int igmp_hwnat_update_hw_nat_info(IGMP_HWNATEntry_t* entry,unsigned long mask,unsigned int local,int state)
{
	return 0 ;
}
int igmp_hwnat_update_all(void)
{
	return 0 ;
}
int igmp_hwnat_is_flow_pkt(char* dst)
{
	return 0 ;
}
int igmp_hwnat_learn_flow(struct sk_buff* skb)
{
	return 0 ;
}
int igmp_hwnat_multicast_undrop(void)
{
	return 0 ;
}
int igmp_hwnat_multicast_undrop_by_grpip(unsigned char is_ipv6,unsigned char* grp_ip)
{
	return 0;
}
int igmp_hwnat_read_proc(char *buf, char **start, off_t off, int count,int *eof, void *data)
{
	return 0 ;
}
static int igmp_hwnat_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	return 0 ;
}
void add_multicast_flood_hwentry(struct sk_buff* skb)
{
	return ;
}
void update_multicast_flood_hwentry(int index, unsigned long long mask)
{
	return ;
}
multicast_flood_hwentry_t* find_multicast_flood_hwentry(int index)
{
	return NULL ;
}
void update_multicast_flood_mask(int index)
{
	return ;
}
int clear_multicast_flood_hwentry(void)
{
	return 0 ;
}
void igmp_hwnat_del_br(struct net_bridge *br)
{
	return ;
}
void igmp_hwnat_add_br(struct net_bridge *br)
{
	return ;
}
void igmp_hwnat_init(struct net_bridge *br)
{
	return ;
}
void igmp_hwnat_fini(void)
{
	return ;
}
int igmp_hwnat_multicast_subcribe_group(unsigned char* grp_addr, unsigned int proto, unsigned int update_mode)
{
	return;
}
int igmp_hwnat_multicast_get_local(unsigned char* grp_addr)
{
	return;
}
#endif

#ifdef TCSUPPORT_PORT_ISOLATION
int checkPacketsDeliver(struct net_bridge_port *prev, struct sk_buff *skb, int portBindMatch)
{
	int deliverFlag = 0;//0 means not deliver, 1 menas deliver
	int routePolicyMatch = 0;
	unsigned char vlanNum = 0;
	unsigned short vlanId= {0};
	unsigned char *pdata=NULL;

	if(skb && isWANInterface(skb->dev))
	{
		if(isWANInterface(prev->dev))
		{
			goto out;
		}	
		else if(isLANInterface(prev->dev) || isUSBInterface(prev->dev) || isWiFiInterface(prev->dev))
		{
			if(portBindMatch==0)
				goto out;
		}		
	}	
	else if(skb && (isLANInterface(skb->dev) || isUSBInterface(skb->dev) || isWiFiInterface(skb->dev)))
	{
		if(isLANInterface(prev->dev) || isUSBInterface(prev->dev) || isWiFiInterface(prev->dev))
		{
			if(portBindMatch==0)
				goto out;
		}
		else if(isWANInterface(prev->dev))
		{		
			if((skb->mark & 0x1E00000) != 0)
			{
				routePolicyMatch = 1;
			}
		
			pdata = (unsigned char *)eth_hdr(skb);
			
			if(pdata){ //check vlan_tag
				pdata += 12;
				if(*((unsigned short int *)pdata) == 0x8100){
					vlanNum++;
					vlanId = ((*(pdata+2) << 8) + *(pdata + 3)) & VLAN_VID_MASK;
					pdata += 4;
					if(*((unsigned short int *)pdata) == 0x8100){
						vlanNum++;
						vlanId = ((*(pdata+2) << 8) + *(pdata + 3)) & VLAN_VID_MASK;
						pdata += 4;
					}
				}
			}

			if(check_vtag_match_dev_hook)
			{
				if(vlanNum != 0)
				{
					if(check_vtag_match_dev_hook(prev->dev->name, vlanId, 1) == -1)
						goto out;
				}
				else
				{
					// no vlan packets,  if in group, flood to group member, or just sent to untag device.
					if((portBindMatch == 0) && (routePolicyMatch == 0))
					{				
						if(check_vtag_match_dev_hook(prev->dev->name, vlanId, 0) == -1)
							goto out;
					}	
				}
			}
		}		
	}
	deliverFlag = 1;

out:
	return deliverFlag;
}

#endif

/****************************************************************************
**function name
	 get_client_mac_from_dhcp_packet
**description:
	get clinet mac from broadcast dhcp packet
**return 
	0: success
	-1: fail
**parameter:
	skb: the packet information
	cliMacAddr: client MAC Address
****************************************************************************/

int get_client_mac_from_dhcp_packet(struct sk_buff *skb, unsigned char *cliMacAddr)
{
	int eth_type = 0;
	int dhcp_mode = 0;
	struct iphdr *ip_hdr = NULL;
	struct udphdr *udp_hdr = NULL;
	unsigned char *p;
	
	if(skb == NULL)
		return -1;
	if(skb->dev == NULL)
		return -1;

	p = skb->data;
	p = p + 12;//point to ethernet type
	eth_type = *(unsigned short *)p;
	if(eth_type == htons(0x8100)){
		p = p + 4;
		eth_type = *(unsigned short *)p;
		if(eth_type == htons(0x8100)){
			p = p + 4;
			eth_type = *(unsigned short *)p;
		}
	}

	if (eth_type != htons(ETH_P_IP)){
		return -1;
	}
	
	p = p + 2;//point to ip header
	ip_hdr = (struct iphdr *)p;
	if(ip_hdr->protocol != 0x11) {//not udp
		return -1;
	}
	
	p = p + 20; //point to UDP header
	udp_hdr = (struct udphdr *)p;
	if(udp_hdr->source == htons(68) || udp_hdr->dest == htons(67))
	{
		dhcp_mode = 1; //dhcp boot request
	}
	else if(udp_hdr->source == htons(67) || udp_hdr->dest == htons(68))
	{
		dhcp_mode = 2; //dhcp boot reply
	}
	
	if(dhcp_mode == 0){//not dhcp
		return -1;
	}
	
	p = p + 8; //point to DHCP 
	p += 28;//poinrt to Client Mac address
	memcpy(cliMacAddr, p, 6);
	
	return 0;
	
}

#ifdef TCSUPPORT_BRIDGE_MAC_LIMIT
bool br_fdb_total_mac_num_exceed(struct net_bridge *br)
{
	int i = 0;
	struct net_bridge_fdb_entry *f;
	struct hlist_node *n;
	int macNumTotal = 0;
	if (br == NULL)
	{
		return false;
	}
	
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)	
	for (i = 0; i < BR_HASH_SIZE; i++) {
		hlist_for_each_entry_safe(f, n, &br->hash[i], hlist) {
			if (f && !f->is_local
#if defined(TCSUPPORT_CDS_CT)
			&& (likely(f->dst->dev->name[0] != 'n'))
#endif
			) 
			{
				macNumTotal++;
			}	
		}
	}
#else
	hlist_for_each_entry_rcu(f, &br->fdb_list, fdb_node) {
		if (f && !f->is_local
#if defined(TCSUPPORT_CDS_CT)
		&& (likely(f->dst->dev->name[0] != 'n'))
#endif
		) 
		{
			macNumTotal++;
		}	
	}
#endif

	spin_lock_bh(&mac_limit_set_lock);
	if((0 != macLimit)&&(macNumTotal >= macLimit))
	{
		spin_unlock_bh(&mac_limit_set_lock);
		return true;
	}
#if defined (TCSUPPORT_CDS_CT)
	else if((0 == macLimit) && (macNumTotal >=4000))
	{
		
		spin_unlock_bh(&mac_limit_set_lock);
		return true;
	}
#endif
	spin_unlock_bh(&mac_limit_set_lock);
	return false;
}

bool br_fdb_port_mac_num_exceed(struct net_bridge *br, unsigned char* devName)
{	
    struct net_bridge_fdb_entry *f;
    struct hlist_node *n;
    int i = 0;
    int portMacNum = 0;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)	
    for (i = 0; i < BR_HASH_SIZE; i++) 
    {
        hlist_for_each_entry_safe(f, n, &br->hash[i], hlist){
        	if (f && !f->is_local) 
        	{   
        		if(0 == strcmp(f->dst->dev->name,devName))
        		{
        			portMacNum++;
        		}
        	}
        }
    }
#else
	hlist_for_each_entry_rcu(f, &br->fdb_list, fdb_node) {
		if (f && !f->is_local) 
		{   
			if(0 == strcmp(f->dst->dev->name,devName))
			{
				portMacNum++;
			}
		}
	}
#endif
    
	spin_lock_bh(&mac_limit_set_lock);
	for(i = 0;i < CNT_DEVMACNUM;i++)
	{
		if(0 == strcmp(devName,devMacNum[i].devName))
		{
			if((0 != devMacNum[i].maxNumByPort)&&(portMacNum >= devMacNum[i].maxNumByPort))
			{
				spin_unlock_bh(&mac_limit_set_lock);
				return true;
			}
		}
	}
	spin_unlock_bh(&mac_limit_set_lock);

	return false;
}


static int ecnt_br_fdb_update_inline_hook
(struct net_bridge *br, struct net_bridge_port *p)
{
	if(br_fdb_total_mac_num_exceed(br))
		return -1;

	if(br_fdb_port_mac_num_exceed(br,p->dev->name))
		return -1;

	return 0;
}

int checkMacLimitEnable(void){
	int i = 0;
	
	if(0 != macLimit)
		return 1;

	for(i = 0; i < CNT_DEVMACNUM; i++)
	{
		if(0 != devMacNum[i].maxNumByPort)
		{
			return 1;
		}
	}
	
	return 0;
}

int ecnt_br_maclimit_hook
(struct sk_buff* skb, struct net_bridge *br, struct net_bridge_port *source,
		   const unsigned char *addr, u16 vid, bool added_by_user)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
	struct hlist_head *head;
#endif
	struct net_bridge_fdb_entry *fdb;
	bool fdb_modified = false;

	if( !checkMacLimitEnable()){
		return ECNT_CONTINUE;
	}

	if ( !br || !source || !addr )
		return ECNT_CONTINUE;

	if ( !(source->flags & BR_LEARNING) )
		return ECNT_CONTINUE;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)	
	head = &br->hash[br_mac_hash(addr, vid)];
#endif
	/* some users want to always flood. */
	if (hold_time(br) == 0)
		return ECNT_CONTINUE;

	/* ignore packets unless we are using this port */
	if (!(source->state == BR_STATE_LEARNING ||
	      source->state == BR_STATE_FORWARDING))
		return ECNT_CONTINUE;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)	
	fdb = fdb_find_rcu(head, addr, vid);
#else
	fdb = br_fdb_find_rcu(br, addr, vid);
#endif
	if (likely(fdb))
		return ECNT_CONTINUE;

	spin_lock_bh(&br->hash_lock);
	if ( -1 == ecnt_br_fdb_update_inline_hook(br, source) )
	{
		spin_unlock_bh(&br->hash_lock);
#if defined(TCSUPPORT_CDS_CT)
		if(!macLimit)/*Drop up 4000 MAC limit pkt*/
		{
			return ECNT_RETURN_DROP;
		}
		else
		{
			if(isWANInterface(skb))
			{
				return ECNT_CONTINUE;
			}
		}
#endif
		return ECNT_RETURN_DROP;
	}
	spin_unlock_bh(&br->hash_lock);

	return ECNT_CONTINUE;
}

static void flush_br0_mac(void)
{
	struct net_device* dev = dev_get_by_name(&init_net, "br0");
	if(dev != NULL){
		struct net_bridge *br = netdev_priv(dev);
		if( br != NULL )
		br_fdb_flush(br);	
		dev_put(dev);
	}
}

static int mac_limit_total_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64];
	char* endpo;

	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	memset(val_string,0,64);
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	spin_lock_bh(&mac_limit_set_lock);
	macLimit = simple_strtol(val_string,&endpo,10);
	spin_unlock_bh(&mac_limit_set_lock);

	flush_br0_mac();

	#ifdef TCSUPPORT_RA_HWNAT_ENHANCE_HOOK
	if(ra_sw_nat_hook_clean_table)
		ra_sw_nat_hook_clean_table();
	#endif
	
	return count;
}

static int mac_limit_total_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("%d\n",macLimit);
	
	return 0;
}


static int mac_limit_by_port_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64];
	char* endpo;
	char port[64];
	int portNo;
	char portMacLimit[64];
	int portMacLimitNum;
	char* p = NULL;
	int i;

	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	memset(val_string,0,64);
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	p = strchr(val_string,'-');

	if(NULL == p)
	{
		printk("p = NULL\n");
		return -EINVAL;
	}

	memset(port,0,64);
	memmove(port,val_string,p-val_string);
	portNo = simple_strtol(port,&endpo,10);

	memset(portMacLimit,0,64);
	strcpy(portMacLimit,p+1);
	portMacLimitNum = simple_strtol(portMacLimit,&endpo,10);

	spin_lock_bh(&mac_limit_set_lock);
	for(i = 0;i < CNT_DEVMACNUM;i++)
	{
		if(devMacNum[i].devPortNo == portNo)
		{
			devMacNum[i].maxNumByPort = portMacLimitNum;
			break;
		}
	}
	spin_unlock_bh(&mac_limit_set_lock);

	flush_br0_mac();

	#ifdef TCSUPPORT_RA_HWNAT_ENHANCE_HOOK
	if(ra_sw_nat_hook_clean_table)
		ra_sw_nat_hook_clean_table();
	#endif
	
	return count;
}

static int mac_limit_by_port_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int i = 0;

	for(i = 1;i < CNT_DEVMACNUM;i++)
	{
		printk("%s%d:%d\n",lanNamePre(),i,devMacNum[i].maxNumByPort);
	}

	return 0;
}

void bridgeMacLimitProcInit(void)
{
	struct proc_dir_entry *br_fdb_proc = NULL;
	br_fdb_proc = create_proc_entry("br_fdb/mac_limit_total", 0, NULL);
	if(NULL == br_fdb_proc)
	{
		printk("ERROR!Create proc entry mac_limit_total fail!");
		return;
	}
	br_fdb_proc->read_proc = mac_limit_total_read_proc;
	br_fdb_proc->write_proc = mac_limit_total_write_proc;

	br_fdb_proc = create_proc_entry("br_fdb/mac_limit_by_port",0,NULL);
	if(NULL == br_fdb_proc)
	{
		printk("ERROR!Create proc entry mac_limit_by_port fail!");
		return;
	}
	br_fdb_proc->read_proc = mac_limit_by_port_read_proc;
	br_fdb_proc->write_proc = mac_limit_by_port_write_proc;
}

void bridgeMacLimitProcFini(void)
{
	remove_proc_entry("br_fdb/mac_limit_total",NULL);
	remove_proc_entry("br_fdb/mac_limit_by_port",NULL);
}

#endif

static int olt_l2_bridge_off_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char buff[8];
	int flag;
	
	if (count > sizeof(buff) - 1)
		return -EINVAL ;

	memset(buff,0,8);
	if (copy_from_user(buff, buffer, count))
		return -EFAULT ;

	sscanf(buff,"%d",&flag);

	olt_l2_bridge_off = flag;

	return count;

}

static int olt_l2_bridge_off_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("%d\n",olt_l2_bridge_off);
	
	return 0;
}

void oltL2BridgeOffProcInit(void)
{
	struct proc_dir_entry *proc = NULL;
	proc = create_proc_entry("tc3162/oltL2BridgeOff", 0, NULL);
	if(NULL == proc)
	{
		printk("ERROR!Create proc oltL2BridgeOff fail!");
		return;
	}
	proc->read_proc = olt_l2_bridge_off_read_proc;
	proc->write_proc = olt_l2_bridge_off_write_proc;
}

int get_multicast_snooping_state_by_skb(struct sk_buff* skb)
{
#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
	#ifdef TCSUPPORT_SNOOPING_SEPERATION
	int index = 0;
	int ip_ver = 0;

	if(hwnat_skb_to_foe_hook)
		index = hwnat_skb_to_foe_hook(skb);

	if(ra_sw_nat_hook_entry_type)
		ip_ver = ra_sw_nat_hook_entry_type(index);

	if(2 == ip_ver)
		return (g_snooping_enable>>BRCTL_IGMPSNOOPING_OFFSET)&1;
	else if(3 == ip_ver)
		return (g_snooping_enable>>BRCTL_MLDSNOOPING_OFFSET)&1;
	else
		return 0;
	#else
	return g_snooping_enable;
	#endif
#else
	return 0;
#endif
}
EXPORT_SYMBOL(get_multicast_snooping_state_by_skb);

int get_multicast_snooping_state_by_index(int index)
{
#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
	#ifdef TCSUPPORT_SNOOPING_SEPERATION
	int ip_ver = 0;

	if(ra_sw_nat_hook_entry_type)
		ip_ver = ra_sw_nat_hook_entry_type(index);
	
	if(2 == ip_ver)
		return (g_snooping_enable>>BRCTL_IGMPSNOOPING_OFFSET)&1;
	else if(3 == ip_ver)
		return (g_snooping_enable>>BRCTL_MLDSNOOPING_OFFSET)&1;
	else
		return 0;
	#else
	return g_snooping_enable;
	#endif
#else
	return 0;
#endif
}

int ecnt_br_multicast_querier_exists(struct net_bridge *br, struct ethhdr *eth)
{
#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
	struct vlan_hdr *vhdr = NULL;

	if(eth->h_proto != cpu_to_be16(ETH_P_8021Q) && 
		eth->h_proto != cpu_to_be16(ETH_P_8021AD))
		return 0;

	vhdr = (struct vlan_hdr *)(((unsigned char *)eth) + ETH_HLEN);
	switch (vhdr->h_vlan_encapsulated_proto) {
	case (htons(ETH_P_IP)):
		
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)	
		if(__br_multicast_querier_exists(br, &br->ip4_other_query, false))
#else	
		if(__br_multicast_querier_exists(br, &br->ip4_other_query))
#endif
			return 1;
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case (htons(ETH_P_IPV6)):
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)	
		if(__br_multicast_querier_exists(br, &br->ip6_other_query, true))
#else	
		if(__br_multicast_querier_exists(br, &br->ip6_other_query))
#endif
			return 1;
		break;
#endif
	default:
		return 0;
	}
#endif
	return 0;
}

#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
static int ecntMulticastVlanDisableSetAll(struct net_bridge *br){
	int array_index = 0;

	for(array_index=0; array_index<128; array_index++){
		br->multicast_vlan_disabled[array_index] = 0xFFFFFFFF;
	}

	return 0;
}

static int ecntMulticastVlanDisableDelAll(struct net_bridge *br){
	int array_index = 0;

	for(array_index=0; array_index<128; array_index++){
		br->multicast_vlan_disabled[array_index] = 0;
	}

	return 0;
}

int ecntMulticastVlanDisableSet(struct net_bridge *br, unsigned int vlan_id, unsigned int mode){

	if( (vlan_id < MULTICAST_MIN_VLAN_ID) || (vlan_id > MULTICAST_MAX_VLAN_ID) ){
		printk("\n%s[%d]: vlan id should be %d ~ %d \n", __func__, __LINE__, MULTICAST_MIN_VLAN_ID, MULTICAST_MAX_VLAN_ID);
		return -1;
	}

	if(mode > 0){
		br->multicast_vlan_disabled[MULTICAST_VLAN_OFFSET(vlan_id)] |= MULTICAST_VLAN_MAPBIT(vlan_id);
	}else{
		br->multicast_vlan_disabled[MULTICAST_VLAN_OFFSET(vlan_id)] &= (~ MULTICAST_VLAN_MAPBIT(vlan_id));
	}

	return 0;
}

int ecntMulticastVlanDisableGet(struct net_bridge *br, unsigned int vlan_id){
	unsigned int multicastVlanDisabled = 0; 

	if( (vlan_id < MULTICAST_MIN_VLAN_ID) || (vlan_id > MULTICAST_MAX_VLAN_ID) ){
		return br->multicast_disabled;
	}

	multicastVlanDisabled = (br->multicast_vlan_disabled[MULTICAST_VLAN_OFFSET(vlan_id)] & MULTICAST_VLAN_MAPBIT(vlan_id));
	multicastVlanDisabled = multicastVlanDisabled >> MULTICAST_VLAN_SHIFT(vlan_id);

	return multicastVlanDisabled;
}

static int bridge_vlan_snoop_dump(struct net_bridge *br)
{
	int vlan_id = 0;
	int if_has_vlan = 0;
	int vlan_show_flag_1 = 0;
	int vlan_show_flag_2 = 0;
	int last_active_vlan_id = 0;
	int first_show_dot_flag = 0;

	printk("%s multicast vlan disabled list:\n", br->dev->name);
	
	for(vlan_id=MULTICAST_MIN_VLAN_ID; vlan_id<=MULTICAST_MAX_VLAN_ID; vlan_id++){
		if(ecntMulticastVlanDisableGet(br, vlan_id)){
			if_has_vlan = 1;
			if(first_show_dot_flag == 0){
				first_show_dot_flag = 1;
			}else if(vlan_show_flag_1 == 0){
				printk(",");
			}
			
			if(vlan_show_flag_1 == 0){
				vlan_show_flag_1 = 1;
				printk("%d", vlan_id);
			}else{
				vlan_show_flag_2 = 1;
			}
			last_active_vlan_id = vlan_id;
		}else{
			if(vlan_show_flag_2 == 1){
				printk("-%d", last_active_vlan_id);
			}
			vlan_show_flag_2 = vlan_show_flag_1 = 0;
		}
	}
	if(vlan_show_flag_2 == 1){
		printk("-%d", last_active_vlan_id);
	}
	if(if_has_vlan)
		printk("\n");
	else
		printk("-\n");

	return 0;
}

static bridge_vlan_snoop_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	struct net_bridge *br = NULL;
	struct net_device* dev = dev_get_by_name(&init_net, "br0");
	if(dev != NULL){
		br = netdev_priv(dev);
		if( br == NULL ){
			printk("no bridge br0\n");
			dev_put(dev);
			return 0;
		}
	}else{
		printk("no net device br0\n");
		return 0;
	}

	bridge_vlan_snoop_dump(br);

	dev_put(dev);
	return 0;
}

static int bridge_vlan_snoop_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64];
	char bridge_name[10];
	char cmd[10];
	char vlan[15];
	int vlan_id;
	struct net_bridge *br = NULL;
	struct net_device* dev = NULL;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

 	if(sscanf(val_string, "%9s %14s %9s", bridge_name, vlan, cmd) < 2)
		return -EFAULT ;

	dev = dev_get_by_name(&init_net, bridge_name);
	if(dev != NULL){
		br = netdev_priv(dev);
		if( br == NULL ){
			printk("no bridge %s\n", bridge_name);
			dev_put(dev);
			return count;
		}
	}else{
		printk("no net device %s\n", bridge_name);
		return count;
	}

	if(!br->vlan_enabled){
		printk("bridge vlan is not enabled\n");
		dev_put(dev);
		return count;
	}

	if(strcmp(vlan, "dump") == 0){
		bridge_vlan_snoop_dump(br);
	}else if(strcmp(vlan, "all") == 0){
		if(strcmp(cmd, "on") == 0){
			ecntMulticastVlanDisableDelAll(br);
		}else if(strcmp(cmd, "off") == 0){
			ecntMulticastVlanDisableSetAll(br);
		}else{
			goto help_of_bridge_vlan_snoop;
		}
	}else{
		if(1 != sscanf(vlan, "%d", &vlan_id)){
			goto help_of_bridge_vlan_snoop;
	 	}
		if( (vlan_id < MULTICAST_MIN_VLAN_ID) || (vlan_id > MULTICAST_MAX_VLAN_ID) ){
			printk("vlan id should be %d ~ %d \n", MULTICAST_MIN_VLAN_ID, MULTICAST_MAX_VLAN_ID);
			goto help_of_bridge_vlan_snoop;
		}

		if(strcmp(cmd, "on") == 0){
			ecntMulticastVlanDisableSet(br, vlan_id, 0);
		}else if(strcmp(cmd, "off") == 0){
			ecntMulticastVlanDisableSet(br, vlan_id, 1);
		}else{
			goto help_of_bridge_vlan_snoop;
		}
	}

	//printk("\nsuccess!\n");
	dev_put(dev);
	return count;

help_of_bridge_vlan_snoop:
	printk("\necho bridge_name dump > /proc/tc3162/bridge_vlan_snoop\n");
	printk("echo bridge_name vlan_id/all on/off > /proc/tc3162/bridge_vlan_snoop\n");
	printk("Bridge vlan snooping work, only when bridge vlan is enabled\n");
	dev_put(dev);
	return count;
	
}

/******************************************************************************
******************************************************************************/
int bridge_vlan_snoop_init(struct net_bridge *br)
{
	if(br_vlan_snoop_proc == NULL)
	{
	    //******** register proc node ********//
	    br_vlan_snoop_proc = create_proc_entry("tc3162/bridge_vlan_snoop", 0, NULL) ;
		if(br_vlan_snoop_proc) {
			br_vlan_snoop_proc->read_proc = bridge_vlan_snoop_read_proc ;
			br_vlan_snoop_proc->write_proc = bridge_vlan_snoop_write_proc ;
		}
	}
    
    return 0;
}

void bridge_vlan_snoop_deinit(struct net_bridge *br)
{

    //******** unregister proc node ********//
    remove_proc_entry("tc3162/bridge_vlan_snoop", NULL) ;
}

#endif
/****************************************************/
#if 0
#define BR_NAME  "br0"

#define ADDR_FMT_STR "%02x:%02x:%02x:%02x:%02x:%02x"

#define ADDR_FMT_ARS(ea) \
	(ea)[0], (ea)[1], (ea)[2], (ea)[3], (ea)[4], (ea)[5]

int read_ether_port_status(int* port_status)
{
	struct file* filp = NULL;
	mm_segment_t oldfs;
	loff_t pos;
	char buf[64];
	int ret = -1;
	ssize_t result;
	char *src = "/proc/tc3162/eth_port_status";

	filp = filp_open(src, O_RDONLY, 0);
	if (IS_ERR(filp) )
	{
		printk("Unable to load '%s'.\n", src);
		return ret;
	}
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	pos = 0;
	result = vfs_read(filp, buf, sizeof(buf), &pos);
	if (result > 0)
	{
		sscanf(buf, "%d %d %d %d", &port_status[0], &port_status[1], &port_status[2], &port_status[3]);
		ret = 0;
	}
	set_fs(oldfs);
	filp_close(filp, NULL);
	
	return ret;
}

static int ether_port_clients_read_proc(char *buf, char **start, \
	off_t off, int count, int *eof, void *data)
{
	int ret = 0;
	int i = 0;
	size_t len = 0;
	int length = 0;
	int port_status[16];
	int port = 0;
	struct net_device *dev = NULL;
	struct net_bridge *br = NULL;
	struct net_bridge_fdb_entry *f = NULL;
	int client = 0;
	dev = __dev_get_by_name(&init_net, BR_NAME);
	if (NULL == dev)
	{
		ret =  -ENXIO;
		return -1;
	}
	br = netdev_priv(dev);
	if(NULL == br)
	{
		ret =  -ENXIO;
		return -1;
	}
	ret = read_ether_port_status(port_status);
	if(0 > ret)
	{
		return -1;
	}
	
	rcu_read_lock();
	for (i = 0; i < BR_HASH_SIZE; i++) 
	{
		hlist_for_each_entry_rcu(f, &br->hash[i], hlist)
		{
			if ((has_expired(br, f)) || (f->is_local) || (!f->dst))
			{
				continue;
			}
			if(NULL == strstr(f->dst->dev->name, "eth"))
			{
				continue;
			}
			length = strlen(f->dst->dev->name);
			port = f->dst->dev->name[length-1] - '0';
			port -= 1;
			if(1 != port_status[port])
			{
				continue;
			}
			client++;
			if(client > 163)
			{
				printk("func = %s, warning clients exceed max=163\n", __func__);
				goto exit;
			}
			len += sprintf(buf+len, "%s "ADDR_FMT_STR"\n", \
				f->dst->dev->name, ADDR_FMT_ARS(f->addr.addr));
		}
	}
	
exit:
	rcu_read_unlock();
	*start = buf + off;
	if (len < off + count)
		*eof = 1;
	len -= off;
	if (len > count)
		len = count ;
	if (len < 0)
		len = 0;
		
	return len;
}

void ether_port_clients_proc_init(void)
{
	struct proc_dir_entry *ether_port_proc = NULL;

	proc_mkdir("ether_port", NULL);
	ether_port_proc = create_proc_entry("ether_port/clients", 0, NULL);
	if(NULL == ether_port_proc)
	{
		printk("error, ether_port mac proc fail!");
		return;
	}
	ether_port_proc->read_proc = ether_port_clients_read_proc;

	return ;
}
#endif

int ecnt_igmp_force_leave_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    printk("%d\n", igmp_force_leave);
    return 0;
}

int ecnt_igmp_force_leave_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
    char val_string[64];

    if (count > sizeof(val_string) - 1)
        return -EINVAL ;

    memset(val_string, 0,64);
    if (copy_from_user(val_string, buffer, count))
        return -EFAULT ;

    sscanf(val_string, "%d", &igmp_force_leave);    
    return count;
}

#ifndef AIROHA_BSP
#include "ecnt_br_sdk.c"
#endif/*AIROHA_BSP*/

