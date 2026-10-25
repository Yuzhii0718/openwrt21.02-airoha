#ifndef _LINUX_ECNT_BR_H
#define _LINUX_ECNT_BR_H
#include <linux/kernel.h>
#include <linux/kmemcheck.h>
#include <linux/compiler.h>
#include <linux/time.h>
#include <linux/bug.h>
#include <linux/cache.h>
#include <linux/if_pppox.h>
#include <linux/if_bridge.h>
#include <uapi/linux/ppp_defs.h>
#include <linux/atomic.h>
#include <asm/types.h>
#include <linux/spinlock.h>
#include <linux/net.h>
#include <linux/textsearch.h>
#include <net/checksum.h>
#include <linux/rcupdate.h>
#include <linux/hrtimer.h>
#include <linux/dma-mapping.h>
#include <linux/netdev_features.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <ecnt_hook/ecnt_hook.h>
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
#include <ecnt_hook/ecnt_hook_net.h>
#include <linux/igmp.h>
#if IS_ENABLED(CONFIG_IPV6)
#include <net/ipv6.h>
#include <net/mld.h>
#endif
#include <linux/ecnt_in.h>
#include <linux/spinlock.h>
#include "br_private.h"
#include <lan_port/lan_port_info.h>
#ifdef TCSUPPORT_MULTICAST_BSP_GENERAL
#include <ecnt_hook/ecnt_hook_multicast_general.h>
#endif

#if defined(TCSUPPORT_MULTICAST_BSP_GENERAL)
#define		MTK_MULTICAST_ADD	1
#define		MTK_MULTICAST_DEL	2
#endif

#ifdef TCSUPPORT_BRIDGE_MAC_LIMIT
extern bool br_fdb_total_mac_num_exceed(struct net_bridge *br);
extern bool br_fdb_port_mac_num_exceed(struct net_bridge *br, unsigned char* devName);
extern void bridgeMacLimitProcInit(void);
extern void bridgeMacLimitProcFini(void);
#endif
extern void oltL2BridgeOffProcInit(void);

#ifdef TCSUPPORT_BRIDGE_MAC_LIMIT
int ecnt_br_maclimit_hook
(struct sk_buff* skb, struct net_bridge *br, struct net_bridge_port *source,
const unsigned char *addr, u16 vid, bool added_by_user);
#endif

#if defined(TCSUPPORT_CT_DS_LIMIT)
#include <linux/qos_type.h>
extern int (*dslimit_remarkQueue_hook)( struct sk_buff *skb, int up_dw );
#endif
/*-----------------------------------------------------------------*/

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
extern int (*fe_resource_mark_meter_hook)(struct sk_buff *skb, int dir) ;   
extern int (*fe_resource_mark_acnt_hook)(struct sk_buff *skb, int dir) ; 
#endif

/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED start*/
extern int g_last_snoop_state;
extern unsigned int hw_igmp_flood_enable;
extern unsigned int g_snooping_enable;
/*TCSUPPORT_XPON_IGMP && TCSUPPORT_MULTICAST_SPEED end*/

/*TCSUPPORT_IGMP_SNOOPING start*/
extern int snoopingdebug;
/*TCSUPPORT_IGMP_SNOOPING end*/

extern int igmp_force_leave;
extern unsigned long igmp_cur_group_cnt;

extern int olt_l2_bridge_off;

extern int ecnt_igmp_force_leave_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data);
extern int ecnt_igmp_force_leave_write_proc(struct file *file, const char *buffer, unsigned long count, void *data);

extern int get_multicast_snooping_state_by_skb(struct sk_buff* skb);
extern int (*multicast_get_stb_src_ip4)(unsigned int ip4);
#if IS_ENABLED(CONFIG_IPV6)
extern int (*multicast_get_stb_src_ip6)(struct in6_addr *ip6);
#endif

/*-----------------------------------------------------------------*/

#define DEBUGP_SNOOP(x, args...) if(snoopingdebug) printk(x, ## args)

extern int br_multicast_equal_port_group(struct net_bridge_port_group *pg,  
	struct net_bridge_port *port, struct br_ip *group);
extern char* ip6_sprintf(const struct in6_addr *addr);
extern int igmp_hwnat_get_port(struct net_bridge_port*  p);
extern int igmp_hwnat_update_all(void);
extern int igmp_hwnat_multicast_undrop(void);
extern int igmp_hwnat_multicast_undrop_by_grpip(unsigned char is_ipv6,unsigned char* grp_ip);
extern void igmp_hwnat_init(struct net_bridge *br);
extern void igmp_hwnat_fini(void);
extern void add_multicast_flood_hwentry(struct sk_buff* skb);
extern int clear_multicast_flood_hwentry(void);
extern void igmp_hwnat_del_br(struct net_bridge *br);
extern void update_multicast_flood_hwentry(int index, unsigned long long mask);
extern void update_multicast_flood_mask(int index);

extern int (*xpon_hgu_multicast_data_hook)(struct sk_buff *skb);

#ifdef TCSUPPORT_HWNAT_V3
extern int ecnt_br_multicast_hwnat_update_entry(struct net_bridge_port *port,struct br_ip *group);
/***************joyme4 feature*************/
extern void ecnt_multicast_add_tmp_info(unsigned char* address,u16 proto, short port_no, 
											int ref_cnt_udpxy, unsigned char status_flag);
/***************joyme4 feature*************/
#endif

#ifdef TCSUPPORT_MULTICAST_BSP_GENERAL
extern int ecnt_br_multicast_bsp_general_update_entry(struct net_bridge_port *port,struct br_ip *group,int opt);
#endif

/*-----------------------------------------------------------------*/
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
static void fdb_delete(struct net_bridge *br, struct net_bridge_fdb_entry *f, bool swdev_notify);
#else
static void fdb_delete(struct net_bridge *br, struct net_bridge_fdb_entry *f);
#endif
static int host_list_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data);
#if defined(TCSUPPORT_CT)
/* add/delete a MAC to/from bridge table. 
for pppoe-relay upstream go through hwnat. */
static int fdb_delete_by_addr(struct net_bridge *br, const u8 *addr, u16 vlan);
static int ecnt_br_static_mac_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data);
static int ecnt_br_static_mac_write_proc(struct file *file, const char *buffer, unsigned long count, void *data);
#endif
#if defined(TCSUPPORT_XPON_IGMP)
extern int (*xpon_igmp_ioctl_hook)(unsigned long subcmd,unsigned long argv1,unsigned long argv2);
extern int (*xpon_sfu_up_send_multicast_frame_hook)(struct sk_buff *skb, int clone);
extern int (*xpon_sfu_down_multicast_incoming_hook)(struct sk_buff *skb, int clone);
#endif
#if defined(TCSUPPORT_CT)
#ifdef TCSUPPORT_PORTBIND
extern int (*portbind_check_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff *skb);
#endif
#endif

#ifdef TCSUPPORT_PORT_ISOLATION
extern int checkPacketsDeliver(struct net_bridge_port *prev, struct sk_buff *skb, int portBindMatch);
extern int (*portbind_check_hook)(char *inIf, char *outIf);
extern int (*portbind_sw_hook)(void);
extern int (*portbind_sw_prior_hook)(struct sk_buff *skb);
#endif

#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
extern int ecntMulticastVlanDisableSet(struct net_bridge *br, unsigned int vlan_id, unsigned int mode);
extern int ecntMulticastVlanDisableGet(struct net_bridge *br, unsigned int vlan_id);
extern int bridge_vlan_snoop_init(struct net_bridge *br);
extern void bridge_vlan_snoop_deinit(struct net_bridge *br);
#endif

extern int (*arht_pbind_vbind_bridge_check_hook)(struct net_device *in_dev, struct net_device *out_dev, struct sk_buff *skb);

extern int (*hwnat_clean_entry_by_dst_mac_hook)(const unsigned char* mac);

/*------------------------------------------------------------*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
static void br_ip6_multicast_leave_group(struct net_bridge *br,struct net_bridge_port *port,const struct in6_addr *group,__u16 vid);
static int br_ip6_multicast_add_group(struct net_bridge *br, struct net_bridge_port *port, const struct in6_addr *group, __u16 vid);
#else
extern void br_ip6_multicast_leave_group(struct net_bridge *br,struct net_bridge_port *port,const struct in6_addr *group,__u16 vid, const unsigned char *src);
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
extern int br_ip6_multicast_add_group(struct net_bridge *br, struct net_bridge_port *port, const struct in6_addr *group,   __u16 vid,  const unsigned char *src,  bool mldv1);
#else
extern int br_ip6_multicast_add_group(struct net_bridge *br, struct net_bridge_port *port, const struct in6_addr *group, __u16 vid, const unsigned char *src);
#endif
#endif
extern int (*wan_multicast_drop_hook)(struct sk_buff *skb);
extern __IMEM struct net_bridge_fdb_entry *__br_fdb_get(struct net_bridge *br,const unsigned char *addr,__u16 vid);
extern int __is_ip_udp(struct sk_buff *skb);

#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
extern int get_mc_fdb_entries(struct net_bridge *br, void __user *userbuf, unsigned long maxnum, unsigned long offset);
extern int igmp_hwnat_clear_flows(void);			   
extern int igmp_hwnat_flow_index(struct sk_buff* skb);    
#endif

extern void tc3162wdog_kick(void);
extern ssize_t ecnt_kernel_fs_read(struct file *osfd, char __user *pDataPtr, size_t readLen, loff_t *fpos);

static inline int ecnt_br_forward_inline_hook(struct sk_buff *skb)
{
#if defined(TCSUPPORT_RA_HWNAT)	
	skb->bridge_flag = 1;
#endif
	return ECNT_CONTINUE;
}

#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
static inline int ecnt_br_multicast_ipv4_rcv_inline_hook(struct net_bridge_port *port,struct sk_buff *skb,struct sk_buff *skb2,
	struct iphdr *iph,struct igmphdr *ih,int* err)
{
#if defined(TCSUPPORT_IGMPSNOOPING_ENHANCE)
  /*flooding UPNP packets,239.255.255.250
    add flooding MDNS packets, 224.0.0.251*/
  if(iph->daddr == UPNP_MCAST || iph->daddr == MDNS_MCAST)
    return ECNT_RETURN ;
#endif
#if defined(TCSUPPORT_IGMP_SNOOPING)
	if(port){
		memcpy(port->macAddr.addr, eth_hdr(skb)->h_source,ETH_ALEN);
		memset(&port->src_entry, 0, sizeof(port->src_entry));
		port->version = 4;
	}
#endif
	switch(ih->type)
	{
		case IGMP_HOST_MEMBERSHIP_REPORT:
		case IGMPV2_HOST_MEMBERSHIP_REPORT:
			//ignore wan site control packet.
			if((skb2->dev != NULL) && (skb2->dev->name[0] == 'n')){
				//printk("\r\nignore wan site report packet(v1/v2)!");
				*err = 0;
				return ECNT_RETURN_DROP;
			}
			if(multicast_get_stb_src_ip4) {
				if(multicast_get_stb_src_ip4(iph->saddr) == 0) {
					*err = -EINVAL;
					return ECNT_RETURN_DROP;
				}
			}
			break;
		case IGMPV3_HOST_MEMBERSHIP_REPORT:
			//ignore wan site control packet.
			if((skb2->dev != NULL) && (skb2->dev->name[0] == 'n')){
				//printk("\r\nignore wan site report packet(v3)!");
				*err = 0;
				return ECNT_RETURN_DROP;
			}
			if(multicast_get_stb_src_ip4) {
				if(multicast_get_stb_src_ip4(iph->saddr) == 0) {
					*err = -EINVAL;
					return ECNT_RETURN_DROP;
				}
			}
			break;
		case IGMP_HOST_MEMBERSHIP_QUERY:
#if defined(TCSUPPORT_CT_PORTSLIMIT)
			if ( !port )
				skb->vlan_tag_flag |= VLAN_TAG_IGMP_QUERYFLAG;
#endif
#if defined(TCSUPPORT_CT_JOYME4)
			if(skb2->dev->name[0] == 'r' || skb2->dev->name[0] == 'e'){
				*err = -EINVAL;
				return ECNT_RETURN_DROP;
			}
#endif
			break;
		case IGMP_HOST_LEAVE_MESSAGE:
			//ignore wan site control packet.
			if((skb2->dev != NULL) && (skb2->dev->name[0] == 'n')){
				//printk("\r\nignore wan site leave packet!");
				*err = 0;
				return ECNT_RETURN_DROP;
			}
			if(multicast_get_stb_src_ip4) {
				if(multicast_get_stb_src_ip4(iph->saddr) == 0) {
					*err = -EINVAL;
					return ECNT_RETURN_DROP;
				}
			}
			break;
	}

	return ECNT_CONTINUE ;
}

#if IS_ENABLED(CONFIG_IPV6)
static void MultiIP2MAC(struct in6_addr *pIpaddr, unsigned char *mac)
{
	if(pIpaddr == NULL || mac == NULL)
		return;

	*mac = 0x33;
	*(mac + 1) = 0x33;
	*(mac + 2) = pIpaddr->s6_addr[12];
	*(mac + 3) = pIpaddr->s6_addr[13];
	*(mac + 4) = pIpaddr->s6_addr[14];
	*(mac + 5) = pIpaddr->s6_addr[15];

	return;
}

static inline int ecnt_br_multicast_ipv6_rcv_inline_hook(struct net_bridge_port *port,struct sk_buff *skb,struct sk_buff *skb2,
														u8 type, int* err)
{
	const struct ipv6hdr *ip6h = NULL;
	ip6h = ipv6_hdr(skb2);
    
	if(port){
		struct mld_msg *mld = (struct mld_msg *)icmp6_hdr(skb2);
		if ( mld && port )
		{
			MultiIP2MAC(&mld->mld_mca, port->groupMacAddr.addr);
			memcpy(port->macAddr.addr, eth_hdr(skb)->h_source, ETH_ALEN);
			memset(&port->src_entry, 0, sizeof(port->src_entry));
			port->version = 6;
		}
	}
	switch(type)
	{
		case ICMPV6_MGM_REPORT:
		case ICMPV6_MLD2_REPORT:
			if((skb2->dev != NULL) && (skb2->dev->name[0] == 'n')){
				/*printk("\r\n[v6]ignore wan site report packet(v1/v2)!");*/
				*err = 0;
				return ECNT_RETURN_DROP;
			}
			if(multicast_get_stb_src_ip6) {
				if(multicast_get_stb_src_ip6(&ip6h->saddr) == 0) {
					*err = -EINVAL;
					return ECNT_RETURN_DROP;
				}
			}
			break;
		case ICMPV6_MGM_QUERY:
#if defined(TCSUPPORT_CT_PORTSLIMIT)
			if ( !port )
				skb->vlan_tag_flag |= VLAN_TAG_IGMP_QUERYFLAG;
#endif
			break;
		case ICMPV6_MGM_REDUCTION:
			if((skb2->dev != NULL) && (skb2->dev->name[0] == 'n')){
				/*printk("\r\n[v6]ignore wan site leave packet!");*/
				*err = 0;
				return ECNT_RETURN_DROP;
			}
			if(multicast_get_stb_src_ip6) {
				if(multicast_get_stb_src_ip6(&ip6h->saddr) == 0) {
					*err = -EINVAL;
					return ECNT_RETURN_DROP;
				}
			}
			break;
		default:
			break;
	}


	return ECNT_CONTINUE;
}

#if IS_ENABLED(CONFIG_IPV6)
static inline int ecnt_br_multicast_ipv6_rcv_prehandle_inline_hook(struct ipv6hdr *ip6h)
{
#if defined(TCSUPPORT_ANDLINK)
	struct udphdr *uh = NULL;

	if ( NULL == ip6h )
		return ECNT_CONTINUE;

	if ( IPPROTO_UDP == ip6h->nexthdr && ip6h->payload_len > sizeof(*uh) )
	{
		uh = (struct udphdr *)(ip6h + 1);
		if ( 547 == ntohs(uh->dest) )
			return ECNT_RETURN;
	}
#endif
	struct icmp6hdr *icmp6h = NULL;
	
	if ( NULL == ip6h )
		return ECNT_CONTINUE;

	if(IPPROTO_ICMPV6 == ip6h->nexthdr && ip6h->payload_len > sizeof(*icmp6h))
	{
		icmp6h = (struct icmp6hdr *)(ip6h + 1);
		if(NDISC_NEIGHBOUR_SOLICITATION == icmp6h->icmp6_type){
			return ECNT_RETURN;
		}
	}

	return ECNT_CONTINUE;
}
#endif
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
static inline int ecnt_br_ip6_multicast_mld2_report_inline_hook(struct net_bridge *br, 
    struct net_bridge_port *port, struct sk_buff *skb, u16 vid,bool mldv1)

#else
static inline int ecnt_br_ip6_multicast_mld2_report_inline_hook(struct net_bridge *br, 
    struct net_bridge_port *port, struct sk_buff *skb, u16 vid)
#endif
{
	struct icmp6hdr *icmp6h;
	struct mld2_grec *grec;
	const unsigned char *src;
	int i;
	int len;
	int num;
	int err = 0;
    
	if (!pskb_may_pull(skb, sizeof(*icmp6h)))
		return -EINVAL;

	icmp6h = icmp6_hdr(skb);
	num = ntohs(icmp6h->icmp6_dataun.un_data16[1]);
	
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)	
	len = skb_transport_offset(skb) + sizeof(*icmp6h);
#else	
	len = sizeof(*icmp6h);
#endif

	for (i = 0; i < num; i++) {
		__be16 *nsrcs, _nsrcs;

		nsrcs = skb_header_pointer(skb,
					   len + offsetof(struct mld2_grec,
							  grec_nsrcs),
					   sizeof(_nsrcs), &_nsrcs);
		if (!nsrcs)
			return -EINVAL;

		if (!pskb_may_pull(skb,
				   len + sizeof(*grec) +
				   sizeof(struct in6_addr) * ntohs(*nsrcs)))
			return -EINVAL;

		grec = (struct mld2_grec *)(skb->data + len);
		len += sizeof(*grec) +
		       sizeof(struct in6_addr) * ntohs(*nsrcs);

		src = eth_hdr(skb)->h_source;
		/* We support these as MLDv2 reports for now. */
		switch (grec->grec_type) {
        case MLD2_MODE_IS_EXCLUDE:
        case MLD2_CHANGE_TO_EXCLUDE:
        case MLD2_ALLOW_NEW_SOURCES:
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
			err = br_ip6_multicast_add_group(br, port, &grec->grec_mca,vid);
#elif LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
		    err = br_ip6_multicast_add_group(br, port, &grec->grec_mca,
						 vid, src, mldv1);
#else
		    err = br_ip6_multicast_add_group(br, port, &grec->grec_mca,
						 vid, src);
#endif
            break;
            
        case MLD2_MODE_IS_INCLUDE:
		case MLD2_CHANGE_TO_INCLUDE:
		    if(grec->grec_nsrcs){
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
				err = br_ip6_multicast_add_group(br, port, &grec->grec_mca, vid);
#elif LINUX_VERSION_CODE > KERNEL_VERSION(5,10,0)
				err = br_ip6_multicast_add_group(br, port, &grec->grec_mca, vid, src, mldv1);
#else
				err = br_ip6_multicast_add_group(br, port, &grec->grec_mca, vid, src);
#endif
			}
			else{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
				br_ip6_multicast_leave_group(br, port, &grec->grec_mca, vid);
#else
                br_ip6_multicast_leave_group(br, port, &grec->grec_mca, vid, src);
#endif
			}
			break;
		
		case MLD2_BLOCK_OLD_SOURCES:
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
			br_ip6_multicast_leave_group(br, port, &grec->grec_mca, vid);
#else
            br_ip6_multicast_leave_group(br, port, &grec->grec_mca, vid, src);
#endif
			break;

		default:
			continue;
		}
	}
	return err;
}


#endif
#else
static inline int ecnt_br_multicast_ipv4_rcv_inline_hook(struct net_bridge_port *port,struct sk_buff *skb,struct sk_buff *skb2,
	struct iphdr *iph,struct igmphdr *ih,int* err)
{
	return 0 ;
}
#if IS_ENABLED(CONFIG_IPV6)
static inline int ecnt_br_multicast_ipv6_rcv_inline_hook(struct net_bridge_port *port,struct sk_buff *skb,u8 type)
{
	return 0 ;
}

static inline int ecnt_br_ip6_multicast_mld2_report_inline_hook(struct net_bridge *br,struct net_bridge_port *port,struct sk_buff *skb,u16 vid)
{
    return 0;
}

#endif
#endif

static inline int ecnt_br_input_state_forward_inline_hook(struct sk_buff *skb)
{
#if !defined(TCSUPPORT_FH_UNIFIED_PLATFORM)//defined(CONFIG_PORT_BINDING) || defined(TCSUPPORT_REDIRECT_WITH_PORTMASK)
	int itf_index = 0;
/*_____________________________________________
** remark packet from different lan interfac,  
** use the highest 4 bits.
**
** eth0 	0x10000000
** eth0.1	0x10000000
** eth0.2	0x20000000
** eth0.3	0x30000000
** eth0.4	0x40000000
** ra0		0x50000000
** ra1		0x60000000
** ra2		0x70000000
** ra3		0x80000000
** usb0 	0x90000000
** wds0~3	0xA0000000
** rai0 	0xB0000000
** rai1 	0xC0000000
** rai2 	0xD0000000
** rai3 	0xE0000000
**_________________________________________
*/

	switch (skb->dev->name[0]) {
		case 'e':
#ifdef TCSUPPORT_TC2031
			/* device name format must be eth0 */
			skb->mark |= 0x10000000;
#else
			//single lan port
			if(!strcmp(skb->dev->name, "eth0"))
			{
				skb->mark |= 1 << DEV_OFFSET;
			}
			/* device name format must be eth0.x */
			else if (isLANInterface(skb->dev))
			{
				itf_index = getLogicLANIndex(skb->dev) + 1 ;
				if(itf_index > 0 && itf_index <= MAX_ECNT_ETHER_PORT_NUM)
				{
					skb->mark |= itf_index << DEV_OFFSET;
				}
			}
			//single xfi lan port
			if (isXSIInterface(skb->dev))
			{
				skb->mark2 |= 1 << XFI_LAN_DEV_OFFSET;	
			}
#endif
			break;
		case 'r':
#ifdef TCSUPPORT_WLAN_AC
			if (skb->dev->name[2] == 'i')
				/* device name must be raix */
				skb->mark |= (get5GLogicWifiIndex(skb->dev) + WLAN_AC_DEV_OFFSET) << DEV_OFFSET;
			else
#endif
			/* device name must be rax */
			skb->mark |= (get24GLogicWifiIndex(skb->dev) + WLAN_DEV_OFFSET) << DEV_OFFSET;
			break;
		case 'u':
			/* device name must be usbx */
			skb->mark |= ((skb->dev->name[3] - '0') + USB_DEV_OFFSET) << DEV_OFFSET;
			break;
		case 'w':
			/* device name must be wdsx */
			skb->mark |= (WDS_DEV_OFFSET) << DEV_OFFSET;
			break;
		default:
			break;
	}
#if defined(TCSUPPORT_CT)
#ifdef TCSUPPORT_PORTBIND
	skb->portbind_mark |= MASK_ORIGIN_DEV;
	skb->orig_dev = skb->dev;
#endif
#endif
#endif	
	return ECNT_CONTINUE;
}

static inline int ecnt_br_handle_frame_finish_inline_hook(struct sk_buff *skb, u16 vid)
{	
	struct net_bridge_port *source = NULL;

	if(!skb){
		return ECNT_HOOK_ERROR;
	}
	source = br_port_get_rcu(skb->dev);

#if defined(TCSUPPORT_RA_HWNAT)
	skb->bridge_flag = 0;
#endif

#if defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7527)
	if ( BR_STATE_FORWARDING == source->state
	&& fe_resource_mark_meter_hook )
	{
		fe_resource_mark_meter_hook(skb, UP_STREAM);
	}
	if ( BR_STATE_FORWARDING == source->state
	&& fe_resource_mark_acnt_hook )
	{
		fe_resource_mark_acnt_hook(skb, UP_STREAM);
	}     
#endif

	return ECNT_CONTINUE;
}

static inline int arht_br_handle_frame_finish_forward_inline_hook
(struct sk_buff *skb, struct net_device *in_dev, struct net_device *out_dev)
{
	int ret = ECNT_CONTINUE;
#if defined(TCSUPPORT_PORTBIND)
	if ( arht_pbind_vbind_bridge_check_hook && in_dev && out_dev
		&& ('e' == in_dev->name[0] || 'r' == in_dev->name[0])
		&& 'n' == out_dev->name[0] )
	{
		ret = arht_pbind_vbind_bridge_check_hook(in_dev, out_dev, skb);
	}
#endif

	return ret;
}

static inline int ecnt_old_dev_ioctl_inline_hook(struct net_bridge *br,struct ifreq *rq , int* ret)
{
	unsigned long args[4];
    
#if defined(TCSUPPORT_XPON_IGMP)
    typeof(xpon_igmp_ioctl_hook)  xpon_igmp_ioctl;
#endif

	if (copy_from_user(args, rq->ifr_data, sizeof(args)))
		return ECNT_HOOK_ERROR;

	switch(args[0]) 
	{
		case BRCTL_GET_BRIDGE_INFO:
		{
			struct __bridge_info b;

			memset(&b, 0, sizeof(struct __bridge_info));
			rcu_read_lock();
			memcpy(&b.designated_root, &br->designated_root, 8);
			memcpy(&b.bridge_id, &br->bridge_id, 8);
			b.root_path_cost = br->root_path_cost;
			b.max_age = jiffies_to_clock_t(br->max_age);
			b.hello_time = jiffies_to_clock_t(br->hello_time);
			b.forward_delay = br->forward_delay;
			b.bridge_max_age = br->bridge_max_age;
			b.bridge_hello_time = br->bridge_hello_time;
			b.bridge_forward_delay = jiffies_to_clock_t(br->bridge_forward_delay);
			b.topology_change = br->topology_change;
			b.topology_change_detected = br->topology_change_detected;
			b.root_port = br->root_port;

			b.stp_enabled = (br->stp_enabled != BR_NO_STP);
			b.ageing_time = jiffies_to_clock_t(br->ageing_time);
			b.hello_timer_value = br_timer_value(&br->hello_timer);
			b.tcn_timer_value = br_timer_value(&br->tcn_timer);
			b.topology_change_timer_value = br_timer_value(&br->topology_change_timer);
			
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
			b.gc_timer_value = br_timer_value(&br->gc_work.timer);
#else			
			b.gc_timer_value = br_timer_value(&br->gc_timer);
#endif
#if defined(CONFIG_BRIDGE_IGMP_SNOOPING) && defined(TCSUPPORT_IGMPSNOOPING_ENHANCE)
			b.igmpsnoop_ageing_time = br->multicast_membership_interval;	
#ifdef TCSUPPORT_SNOOPING_SEPERATION
			b.igmpsnoop_enabled = !br->igmp_disabled;
			b.mldsnoop_enabled = !br->mld_disabled;
#else
			b.igmpsnoop_enabled = !br->multicast_disabled;
#endif			
			b.igmpsnoop_quickleave = br->quick_leave;
			b.igmpsnoop_dbg = (__u8)snoopingdebug;
#endif
			b.igmpsnoop_max_group = br->igmpsnoop_max_group;
			rcu_read_unlock();

			if (copy_to_user((void __user *)args[1], &b, sizeof(b)))
				return -EFAULT;

			return ECNT_RETURN_DROP;
		}
		case BRCTL_GET_PORT_INFO:
		{
			struct __port_info p;
			struct net_bridge_port *pt;

			rcu_read_lock();
			if ((pt = br_get_port(br, args[2])) == NULL) {
				rcu_read_unlock();
				return -EINVAL;
			}

			memset(&p, 0, sizeof(struct __port_info));
			memcpy(&p.designated_root, &pt->designated_root, 8);
			memcpy(&p.designated_bridge, &pt->designated_bridge, 8);
			p.port_id = pt->port_id;
			p.designated_port = pt->designated_port;
			p.path_cost = pt->path_cost;
			p.designated_cost = pt->designated_cost;
			p.state = pt->state;
			p.top_change_ack = pt->topology_change_ack;
			p.config_pending = pt->config_pending;
			p.message_age_timer_value = br_timer_value(&pt->message_age_timer);
			p.forward_delay_timer_value = br_timer_value(&pt->forward_delay_timer);
			p.hold_timer_value = br_timer_value(&pt->hold_timer);

#if defined(CONFIG_BRIDGE_IGMP_SNOOPING) && defined(TCSUPPORT_IGMPSNOOPING_ENHANCE)
			p.is_router = pt->multicast_router;
#endif

			rcu_read_unlock();

			if (copy_to_user((void __user *)args[1], &p, sizeof(p)))
				return -EFAULT;

			return ECNT_RETURN_DROP;
		}
#if defined(CONFIG_BRIDGE_IGMP_SNOOPING) && defined(TCSUPPORT_IGMPSNOOPING_ENHANCE)
		case BRCTL_SET_IGMPSNOOPING_STATE:
			if (!capable(CAP_NET_ADMIN))
				return ECNT_HOOK_ERROR;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,15,0)
			br_multicast_toggle(br, args[1]);
#else
			if(!(br_multicast_toggle(br, args[1])))
				netdev_state_change(br->dev);
#endif
			return ECNT_RETURN_DROP;
		
		case BRCTL_SET_IGMPSNOOPING_AGEING_TIME:
			if (!capable(CAP_NET_ADMIN))
				return ECNT_HOOK_ERROR;
			spin_lock_bh(&br->lock);
			br->multicast_membership_interval = clock_t_to_jiffies(args[1]);
			spin_unlock_bh(&br->lock);
			return ECNT_RETURN_DROP;
	
		case BRCTL_GET_MC_FDB_ENTRIES:
			if (!capable(CAP_NET_ADMIN))
				return ECNT_HOOK_ERROR;
			*ret = get_mc_fdb_entries(br, (void __user *)args[1],
				       args[2], args[3]);
			return ECNT_RETURN;
			
		case BRCTL_SET_IGMPSNOOPING_QUICKLEAVE:
			if (!capable(CAP_NET_ADMIN))
				return -EPERM;
			br->quick_leave = args[1];	
			return ECNT_RETURN_DROP;
			
		case BRCTL_SET_IGMPSNOOPING_DBG:
			if (!capable(CAP_NET_ADMIN))
				return -EPERM;
			snoopingdebug = (int)args[1] ;
			return ECNT_RETURN_DROP;
#endif

#if defined(TCSUPPORT_XPON_IGMP)
            case BRCTL_XPON_IGMP_CMD:
			{
                xpon_igmp_ioctl =  rcu_dereference(xpon_igmp_ioctl_hook);
				if(args[1] == 6011)/*6011 is XPON_IGMP_SET_GROUPNUM*/
					br->igmpsnoop_max_group = args[3];
                if (xpon_igmp_ioctl)
                    return xpon_igmp_ioctl(args[1],args[2],args[3]);
                return ECNT_RETURN;
            }
#endif
#if defined(TCSUPPORT_IGMP_SET_GROUP)
		case BRCTL_SET_IGMPSNOOPING_MAX_GROUP:
			if (!capable(CAP_NET_ADMIN))
				return -EPERM;
			br->igmpsnoop_max_group = args[1];
			return ECNT_RETURN_DROP;
#endif

		default:
			return ECNT_CONTINUE;
	}

	return ECNT_CONTINUE;

}

#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
static inline void ecnt_br_multicast_del_pg_inline_hook(struct net_bridge *br,
				struct net_bridge_port_group *pg)
{
#ifdef TCSUPPORT_IGMPSNOOPING_ENHANCE
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	if(pg->version == 4){
		DEBUGP_SNOOP("mc_fdb_delete delete dev=%s group=" NIPQUAD_FMT " src ip=" NIPQUAD_FMT "\n",	
		pg->key.port->dev->name, NIPQUAD(pg->key.addr.dst.ip4),NIPQUAD(pg->src_entry.src.s_addr));
	}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	else if(pg->version == 6)
	{
		DEBUGP_SNOOP("mc_fdb_delete deleteV6 dev=%s group=[%s] src ip=[%s]\n",	
		pg->key.port->dev->name, ip6_sprintf(&pg->key.addr.dst.ip6),ip6_sprintf(&pg->src_entry.src6));
	}	
#endif
#else
	if(pg->version == 4){
		DEBUGP_SNOOP("mc_fdb_delete delete dev=%s group=" NIPQUAD_FMT " src ip=" NIPQUAD_FMT "\n",	
		pg->port->dev->name, NIPQUAD(pg->addr.u.ip4),NIPQUAD(pg->src_entry.src.s_addr));
	}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	else if(pg->version == 6)
	{
		DEBUGP_SNOOP("mc_fdb_delete deleteV6 dev=%s group=[%s] src ip=[%s]\n",	
		pg->port->dev->name, ip6_sprintf(&pg->addr.u.ip6),ip6_sprintf(&pg->src_entry.src6));
	}	
#endif
#endif
#endif

#if defined (TCSUPPORT_HWNAT_V3) || defined (TCSUPPORT_MULTICAST_BSP_GENERAL)
#if defined(TCSUPPORT_MULTICAST_BSP_GENERAL)
	ecnt_br_multicast_bsp_general_update_entry(pg->port,&(pg->addr),MTK_MULTICAST_DEL);
#else
#if defined(TCSUPPORT_CT_JOYME4)
	ecnt_multicast_add_tmp_info(&(pg->addr.u), (pg->version == 4 ? 1 : 2), pg->port->port_no, -1, 2);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	ecnt_br_multicast_hwnat_update_entry(pg->key.port,&(pg->key.addr));
#else
	ecnt_br_multicast_hwnat_update_entry(pg->port,&(pg->addr));
#endif
#endif
#else
#if defined(TCSUPPORT_XPON_IGMP) || defined(TCSUPPORT_MULTICAST_SPEED)
	igmp_hwnat_update_all();
#endif
#endif

	return ;
}

static inline void ecnt_br_multicast_new_port_group_inline_hook(struct net_bridge_port *port,struct br_ip *group,struct net_bridge_port_group *p)
{
#ifdef TCSUPPORT_IGMPSNOOPING_ENHANCE
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	if(port->version == 4){
		DEBUGP_SNOOP("br_multicast_add_group new portgroup dev=%s group=" NIPQUAD_FMT " src ip=" NIPQUAD_FMT "\n", 
			port->dev->name, NIPQUAD(group->dst.ip4),NIPQUAD(port->src_entry.src.s_addr));
	}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	else{
		DEBUGP_SNOOP("br_multicast_add_group newV6 portgroup dev=%s group=[%s] src ip=[%s]\n", 
			port->dev->name, ip6_sprintf(&group->dst.ip6), ip6_sprintf(&port->src_entry.src6));
	}
#endif
#else
	if(port->version == 4){
		DEBUGP_SNOOP("br_multicast_add_group new portgroup dev=%s group=" NIPQUAD_FMT " src ip=" NIPQUAD_FMT "\n", 
			port->dev->name, NIPQUAD(group->u.ip4),NIPQUAD(port->src_entry.src.s_addr));
	}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	else{
		DEBUGP_SNOOP("br_multicast_add_group newV6 portgroup dev=%s group=[%s] src ip=[%s]\n", 
			port->dev->name, ip6_sprintf(&group->u.ip6), ip6_sprintf(&port->src_entry.src6));
	}
#endif
#endif

	memcpy(&p->src_entry, &port->src_entry, sizeof(port->src_entry)); 
	memcpy(p->group_mac, port->groupMacAddr.addr, sizeof(port->groupMacAddr.addr));
	memcpy(p->host_mac, port->macAddr.addr, sizeof(port->macAddr.addr));
	p->version = port->version;
#endif
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0) 
static inline int ecnt_br_multicast_add_group_inline_hook(struct net_bridge_mdb_entry *mp,struct net_bridge_port *port,
	struct br_ip *group,struct net_bridge *br,unsigned long now, const unsigned char *src, u8 filter_mode)
#else
static inline int ecnt_br_multicast_add_group_inline_hook(struct net_bridge_mdb_entry *mp,struct net_bridge_port *port,
	struct br_ip *group,struct net_bridge *br,unsigned long now, const unsigned char *src)
#endif
{
	struct net_bridge_port_group __rcu **pp;
	struct net_bridge_port_group *p;

	for (pp = &mp->ports ; (p = mlock_dereference(*pp, br)) != NULL ; pp = &p->next) 
	{
#ifdef TCSUPPORT_IGMPSNOOPING_ENHANCE	
		if(br_multicast_equal_port_group(p, port, group)){
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
			if(port->version == 4){
				DEBUGP_SNOOP("br_multicast_add_group update portgroup dev=%s group=" NIPQUAD_FMT " src ip=" NIPQUAD_FMT "\n", 
					port->dev->name, NIPQUAD(group->dst.ip4),NIPQUAD(port->src_entry.src.s_addr));
			}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
			else{
				DEBUGP_SNOOP("br_multicast_add_group updateV6 portgroup dev=%s group=[%s] src ip=[%s]\n", 
					port->dev->name, ip6_sprintf(&group->dst.ip6),ip6_sprintf(&port->src_entry.src6));	
			}
#endif
#else
			if(port->version == 4){
				DEBUGP_SNOOP("br_multicast_add_group update portgroup dev=%s group=" NIPQUAD_FMT " src ip=" NIPQUAD_FMT "\n", 
					port->dev->name, NIPQUAD(group->u.ip4),NIPQUAD(port->src_entry.src.s_addr));
			}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
			else{
				DEBUGP_SNOOP("br_multicast_add_group updateV6 portgroup dev=%s group=[%s] src ip=[%s]\n", 
					port->dev->name, ip6_sprintf(&group->u.ip6),ip6_sprintf(&port->src_entry.src6));	
			}
#endif
#endif
			memcpy(&p->src_entry, &port->src_entry, sizeof(port->src_entry));
			goto found;
		}		
#else
		if (br_port_group_equal(p, port, src))
			goto found;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
		if ((unsigned long)p->key.port < (unsigned long)port)
#else
		if ((unsigned long)p->port < (unsigned long)port)
#endif
			break;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0) 
	p = br_multicast_new_port_group(port, group, *pp, 0, src,filter_mode,RTPROT_KERNEL);

#else
	p = br_multicast_new_port_group(port, group, *pp, 0, src);
#endif
	if (unlikely(!p))
		return ECNT_HOOK_ERROR;
	rcu_assign_pointer(*pp, p);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0) 	
	br_mdb_notify(br->dev, mp, NULL, RTM_NEWMDB);
#else
	br_mdb_notify(br->dev, port, group, RTM_NEWMDB, 0);
#endif

#if defined (TCSUPPORT_HWNAT_V3) || defined (TCSUPPORT_MULTICAST_BSP_GENERAL)
#if defined(TCSUPPORT_MULTICAST_BSP_GENERAL)
	ecnt_br_multicast_bsp_general_update_entry(port,group,MTK_MULTICAST_ADD);
#else
#if defined(TCSUPPORT_CT_JOYME4)  
	ecnt_multicast_add_tmp_info(&group->u, (port->version == 4 ? 1 : 2) , port->port_no, -1, 1);
#endif
	ecnt_br_multicast_hwnat_update_entry(port,group);
#endif
#else
#if defined(TCSUPPORT_XPON_IGMP) || defined(TCSUPPORT_MULTICAST_SPEED)
	igmp_hwnat_update_all();
#ifdef TCSUPPORT_XPON_IGMP
	if(port->version == 4)
		igmp_hwnat_multicast_undrop_by_grpip(0,&(group->u.ip4));		
	else
		igmp_hwnat_multicast_undrop_by_grpip(1,group->u.ip6.in6_u.u6_addr8);
#endif
#endif
#endif

found:
#if defined(TCSUPPORT_IGMP_SNOOPING)
	p->ageing_time = now;
	p->leave_count = 3;
#endif
	mod_timer(&p->timer, now + br->multicast_membership_interval);

	return ECNT_CONTINUE;
}
#else
static inline int ecnt_br_multicast_add_group_inline_hook(struct net_bridge_mdb_entry *mp,struct net_bridge_port *port,
	struct br_ip *group,struct net_bridge *br,unsigned long now)
{
	struct net_bridge_port_group __rcu **pp;
	struct net_bridge_port_group *p;

	for (pp = &mp->ports ; (p = mlock_dereference(*pp, br)) != NULL ; pp = &p->next) 
	{
#ifdef TCSUPPORT_IGMPSNOOPING_ENHANCE	
		if(br_multicast_equal_port_group(p, port, group)){
			if(port->version == 4){
				DEBUGP_SNOOP("br_multicast_add_group update portgroup dev=%s group=" NIPQUAD_FMT " src ip=" NIPQUAD_FMT "\n", 
					port->dev->name, NIPQUAD(group->u.ip4),NIPQUAD(port->src_entry.src.s_addr));
			}
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
			else{
				DEBUGP_SNOOP("br_multicast_add_group updateV6 portgroup dev=%s group=[%s] src ip=[%s]\n", 
					port->dev->name, ip6_sprintf(&group->u.ip6),ip6_sprintf(&port->src_entry.src6));	
			}
#endif
			memcpy(&p->src_entry, &port->src_entry, sizeof(port->src_entry));
			goto found;
		}		
#else
		if (p->port == port)
			goto found;
#endif
		if ((unsigned long)p->port < (unsigned long)port)
			break;
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0) 
	p = br_multicast_new_port_group(port, group, *pp, MDB_TEMPORARY);
#else
	p = br_multicast_new_port_group(port, group, *pp, MDB_TEMPORARY, NULL);
#endif
	if (unlikely(!p))
		return ECNT_HOOK_ERROR;
	rcu_assign_pointer(*pp, p);
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,4,90)	
	br_mdb_notify(br->dev, port, group, RTM_NEWMDB, MDB_TEMPORARY);
#else	
	br_mdb_notify(br->dev, port, group, RTM_NEWMDB);
#endif

#if defined (TCSUPPORT_HWNAT_V3) || defined (TCSUPPORT_MULTICAST_BSP_GENERAL)
#if defined(TCSUPPORT_MULTICAST_BSP_GENERAL)
	ecnt_br_multicast_bsp_general_update_entry(port,group,MTK_MULTICAST_ADD);
#else
#if defined(TCSUPPORT_CT_JOYME4)  
	ecnt_multicast_add_tmp_info(&group->u, (port->version == 4 ? 1 : 2) , port->port_no, -1, 1);
#endif
	ecnt_br_multicast_hwnat_update_entry(port,group);
#endif
#else
#if defined(TCSUPPORT_XPON_IGMP) || defined(TCSUPPORT_MULTICAST_SPEED)
	igmp_hwnat_update_all();
#ifdef TCSUPPORT_XPON_IGMP
	if(port->version == 4)
		igmp_hwnat_multicast_undrop_by_grpip(0,&(group->u.ip4));		
	else
		igmp_hwnat_multicast_undrop_by_grpip(1,group->u.ip6.in6_u.u6_addr8);
#endif
#endif
#endif

found:
#if defined(TCSUPPORT_IGMP_SNOOPING)
	p->ageing_time = now;
	p->leave_count = 3;
#endif
	mod_timer(&p->timer, now + br->multicast_membership_interval);

	return ECNT_CONTINUE;
}
#endif

static inline void ecnt_br_multicast_skip_vlan(struct sk_buff *skb)
{
	struct vlan_hdr *vhdr = NULL;

	if(skb == NULL)
		return;

	if(skb->protocol != cpu_to_be16(ETH_P_8021Q) && 
		skb->protocol != cpu_to_be16(ETH_P_8021AD))
		return;

	vhdr = (struct vlan_hdr *)skb->data;
	skb_pull_inline(skb, VLAN_HLEN);
	vlan_set_encap_proto(skb, vhdr);

	skb->vlan_tag_flag |= VLAN_TAG_MULTICAST_VLAN;

	skb_reset_network_header(skb);
	skb_reset_transport_header(skb);

	return;
}

static inline void ecnt_br_multicast_recover_vlan_inline_hook(struct sk_buff *skb)
{
	if(skb == NULL)
		return;
	
	if(!(skb->vlan_tag_flag & VLAN_TAG_MULTICAST_VLAN))
		return;

	skb->vlan_tag_flag &= ~(VLAN_TAG_MULTICAST_VLAN);

	skb_push(skb, VLAN_HLEN);
	skb->protocol = *((__be16 *)(skb->data - 2));
    
	skb_reset_network_header(skb);
	skb_reset_transport_header(skb);

	return;
}

static inline int ecnt_br_multicast_rcv_inline_hook(struct net_bridge *br, struct sk_buff *skb, unsigned short vid)
{
#if defined(CONFIG_BRIDGE_VLAN_FILTERING)
	if(br->vlan_enabled)
	{
		if(ra_sw_nat_set_mul_br_vid_hook)
			ra_sw_nat_set_mul_br_vid_hook(skb, vid);
	}
#endif

	ecnt_br_multicast_skip_vlan(skb);

#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
    if(g_last_snoop_state != br->multicast_disabled)
    {
        if(br->multicast_disabled)
        {
            igmp_hwnat_clear_flows();
            igmp_hwnat_multicast_undrop();
        }
        else
        {
            /*snoop disable to enable*/
            clear_multicast_flood_hwentry();
        }
        g_last_snoop_state = br->multicast_disabled;
    }
#endif 

	return ECNT_CONTINUE;
}

static inline void ecnt_br_multicast_toggle_inline_hook(unsigned long val)
{
#if defined(CONFIG_BRIDGE_IGMP_SNOOPING)
#ifdef TCSUPPORT_SNOOPING_SEPERATION
		unsigned long val_tmp = val&1;
		if(val&(1<<BRCTL_IGMPSNOOPING_OFFSET))
		{
			if(val_tmp)
				g_snooping_enable |= (1<<BRCTL_IGMPSNOOPING_OFFSET);
			else
				g_snooping_enable &= ~(1<<BRCTL_IGMPSNOOPING_OFFSET);
		}
		else if(val&(1<<BRCTL_MLDSNOOPING_OFFSET))
		{
			if(val_tmp)
				g_snooping_enable |= (1<<BRCTL_MLDSNOOPING_OFFSET);
			else
				g_snooping_enable &= ~(1<<BRCTL_MLDSNOOPING_OFFSET);
		}
		else
			return;
#else
		if(val)
		{
			g_snooping_enable = 1;
		}
		else
		{
			g_snooping_enable = 0;
		}
#endif	
		#if defined (TCSUPPORT_HWNAT_V3) || defined (TCSUPPORT_MULTICAST_BSP_GENERAL)
		#if defined(TCSUPPORT_MULTICAST_BSP_GENERAL)
		ECNT_HOOK_MC_API_SET_SNOOPING_MODE(&g_snooping_enable);
		#else
		PPE_API_MULTICAST_HWNATENTRY_LIST_CLEAR();
		#endif
		#else
		igmp_hwnat_clear_flows();
		clear_multicast_flood_hwentry();
		igmp_hwnat_multicast_undrop();
		#endif
#endif
}

static inline void ecnt_br_multicast_init_inline_hook(struct net_bridge *br)
{
	br_multicast_set_querier(br,1);
#if defined(TCSUPPORT_XPON_IGMP)|| defined(TCSUPPORT_MULTICAST_SPEED)
	igmp_hwnat_init(br);
#endif

#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
	bridge_vlan_snoop_init(br);
#endif

	return ;
}

static inline void ecnt_br_multicast_leave_group_inline_hook(struct net_bridge *br,struct net_bridge_port *port,
	struct br_ip *group)
{
#if defined (TCSUPPORT_HWNAT_V3) || defined (TCSUPPORT_MULTICAST_BSP_GENERAL)
#if defined(TCSUPPORT_MULTICAST_BSP_GENERAL)
	ecnt_br_multicast_bsp_general_update_entry(port,group,MTK_MULTICAST_DEL);
#else
	ecnt_br_multicast_hwnat_update_entry(port,group);
#endif
#else
#if defined(TCSUPPORT_XPON_IGMP) || defined(TCSUPPORT_MULTICAST_SPEED)
	igmp_hwnat_update_all();
#endif
#endif
}

static inline void ecnt_br_dev_delete_inline_hook(struct net_bridge *br)
{
	if( br )
		igmp_hwnat_del_br(br);
}

static inline int ecnt_br_add_bridge_1_inline_hook(const char *name)
{
	struct net_device* dev = dev_get_by_name(&init_net, name);

	if (dev){
		printk("bridge name %s has exist\n", name);
		dev_put(dev);
		return ECNT_RETURN;
	}
	
	return ECNT_CONTINUE;
}

static inline void ecnt_br_add_bridge_2_inline_hook(struct net_device *dev)

{
	struct net_bridge *br = NULL;

	br = netdev_priv(dev);
	if(br) {
		br_multicast_stop(br);
		igmp_hwnat_del_br(br);
	}
}

static inline void ecnt_br_fdb_set_vlan_inline_hook(struct net_bridge *br, const unsigned char *addr, u16 vid, struct sk_buff *skb)
{
	struct vlan_hdr *vhdr;
	struct net_bridge_fdb_entry *fdb;
	u16 vlan_tci = 0;

	fdb = __br_fdb_get(br,addr,vid);
	if (fdb != NULL)
	{
		if(ntohs(skb->protocol) != 0x8100)
		{
			fdb->vlan = 0;
		}
		else
		{
			vhdr = (struct vlan_hdr *)skb->data;
			vlan_tci = ntohs(vhdr->h_vlan_TCI);
			fdb->vlan = vlan_tci;
		}
	}
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,8,0)
static inline int ecnt_br_multicast_flood_inline_hook(struct sk_buff *skb, struct sk_buff *skb0)
{
#if defined(TCSUPPORT_XPON_IGMP)
    int local = 0;
    struct iphdr *iph = NULL;
#endif
	struct sk_buff* new_skb = NULL;

#if defined(TCSUPPORT_XPON_IGMP)
    if (skb->protocol == htons(ETH_P_IP))
    {
        iph = ip_hdr(skb);
        if ((ntohl(iph->daddr) >= 0xe0000100) && (ntohl(iph->daddr) < 0xef000000))
        {
            PPE_MULTICAST_INFO_t info;
            
            memset(&info, 0, sizeof(PPE_MULTICAST_INFO_t));
            info.proto = PPE_MULTICAST_PROTO_IPV4;
            memmove(info.grp_addr,&(iph->daddr),4);      

            PPE_API_MULTICAST_GET_LOCAL(&info, &local);
        }
    }
    
	if(!local && wan_multicast_drop_hook)
	{
		if(skb0)
		{
			new_skb = skb_copy(skb,GFP_ATOMIC);
			if(new_skb)
			{
				wan_multicast_drop_hook(new_skb);
				return ECNT_RETURN;
			}
			else
				return ECNT_CONTINUE;
		}
		else
		{
			wan_multicast_drop_hook(skb);
			return ECNT_RETURN;
		}
	}
	else
#endif
		return ECNT_CONTINUE;
}
#else
static inline int ecnt_br_multicast_flood_inline_hook(struct sk_buff *skb, bool local_rcv)
{
#if defined(TCSUPPORT_XPON_IGMP)
    int local = 0;
    struct iphdr *iph = NULL;
#endif
	struct sk_buff* new_skb = NULL;

#if defined(TCSUPPORT_XPON_IGMP)
    if (skb->protocol == htons(ETH_P_IP))
    {
        iph = ip_hdr(skb);
        if ((ntohl(iph->daddr) >= 0xe0000100) && (ntohl(iph->daddr) < 0xef000000))
        {
            PPE_MULTICAST_INFO_t info;
            
            memset(&info, 0, sizeof(PPE_MULTICAST_INFO_t));
            info.proto = PPE_MULTICAST_PROTO_IPV4;
            memmove(info.grp_addr,&(iph->daddr),4);      

            PPE_API_MULTICAST_GET_LOCAL(&info, &local);
        }
    }
    
	if(!local && wan_multicast_drop_hook)
	{
		if(local_rcv)
		{
			new_skb = skb_copy(skb,GFP_ATOMIC);
			if(new_skb)
			{
				wan_multicast_drop_hook(new_skb);
				return ECNT_RETURN;
			}
			else
				return ECNT_CONTINUE;
		}
		else
		{
			wan_multicast_drop_hook(skb);
			return ECNT_RETURN;
		}
	}
	else
#endif
		return ECNT_CONTINUE;
}
#endif

static inline int ecnt_br_multicast_flood_check_bind_inline_hook(struct sk_buff *skb, struct net_bridge_port *port)
{
#if defined(TCSUPPORT_CT)
	const unsigned char *dest = eth_hdr(skb)->h_dest;
	if(skb->dev == NULL || port == NULL || port->dev == NULL)
		return ECNT_CONTINUE;

	if(!is_multicast_ether_addr(dest) || is_broadcast_ether_addr(dest))
		return ECNT_CONTINUE;

	if(isWANInterface(skb->dev) && isWANInterface(port->dev))
		return ECNT_RETURN_DROP;
	
#endif
	return ECNT_CONTINUE;
}

#else
static inline void ecnt_br_multicast_del_pg_inline_hook(struct net_bridge *br,
				struct net_bridge_port_group *pg)
{
	return ;
}
static inline void ecnt_br_multicast_new_port_group_inline_hook(struct net_bridge_port *port,struct br_ip *group,struct net_bridge_port_group *p)
{
	return ;
}
static inline int ecnt_br_multicast_add_group_inline_hook(struct net_bridge_mdb_entry *mp,struct net_bridge_port *port,
	struct br_ip *group,struct net_bridge *br,unsigned long now)
{
	return 0;
}

static inline void ecnt_br_multicast_skip_vlan(struct sk_buff *skb)
{
	return;
}

static inline void ecnt_br_multicast_recover_vlan_inline_hook(struct sk_buff *skb)
{
	return;
}

static inline int ecnt_br_multicast_rcv_inline_hook(struct net_bridge *br, struct sk_buff *skb, unsigned short vid)
{
	return ECNT_CONTINUE;
}
static inline void ecnt_br_multicast_toggle_inline_hook(unsigned long val)
{
	return ;
}
static inline void ecnt_br_multicast_init_inline_hook(struct net_bridge *br)
{
	return ;
}
static inline void ecnt_br_multicast_leave_group_inline_hook(struct net_bridge *br,struct net_bridge_port *port,
	struct br_ip *group)
{
    return ;
}
static inline void ecnt_br_dev_delete_inline_hook(struct net_bridge *br)
{
	return ;
}

static inline int ecnt_br_add_bridge_1_inline_hook(const char *name)
{
	return ECNT_CONTINUE;
}

static inline void ecnt_br_add_bridge_2_inline_hook(struct net_device *dev)
{
	return ;
}

static inline int ecnt_br_multicast_flood_inline_hook(struct sk_buff *skb, struct sk_buff *skb0)
{
	return ECNT_CONTINUE;
}

static inline int ecnt_br_multicast_flood_check_bind_inline_hook(struct sk_buff *skb, struct net_bridge_port *port)
{
	return ECNT_CONTINUE;
}

#endif

#if defined(TCSUPPORT_CT) && defined(TCSUPPORT_CT_PON)
#ifdef TCSUPPORT_PORTBIND
/*
check port bind when br flood
return code:  ZERO-> bind check ok or ignore
		     NON ZERO -> check fail or not bind
*/
static inline int ecnt_br_flood_portbind_inline_hook
( struct sk_buff *skb, struct net_device *lan_dev )
{
	struct ethhdr *ethhdr = NULL;

	if(!skb)
		return 0;

	ethhdr = eth_hdr(skb);
	if ( !skb->orig_dev || !lan_dev || !ethhdr )
		return 0;

	/* only check multicast data when if from WAN */
	if ( is_broadcast_ether_addr(ethhdr->h_dest)
		|| !(skb->orig_dev->bind_type & 
		(IF_TYPE_WAN_ROUTE|IF_TYPE_WAN_BRIDE))
		|| !is_multicast_ether_addr(ethhdr->h_dest) )
		return 0;

	if ( portbind_check_hook 
		&& (portbind_check_hook(skb->orig_dev, lan_dev, skb) == 0) )
	{
		return 1;
	}

	return 0;
}
#endif
#endif

/*-------------------------------------------------------------------------*/
/* Don't forward packets to originating port or forwarding diasabled */
static inline int ecnt_should_deliver(const struct net_bridge_port *p,
				 const struct sk_buff *skb, int bind_check)
{
	/* 
	* bind_check==1 --> only check port bind
	* bind_check==2 --> check port bind & check port state
	**/

#if !defined(TCSUPPORT_VLAN_ACCESS_TRUNK) /* VLAN_ACCESS_TRUNK no need bind check */
	if ( bind_check )
	{
#if defined(TCSUPPORT_CT) && defined(TCSUPPORT_CT_PON)
#if defined(TCSUPPORT_PORTBIND) && defined(TCSUPPORT_RA_HWNAT) && defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
		if ( !get_multicast_snooping_state_by_skb(skb) && hw_igmp_flood_enable
			&& skb 
			&& ( htons(ETH_P_IP) == skb->protocol
			|| htons(ETH_P_IPV6) == skb->protocol )
			&& 0 != ecnt_br_flood_portbind_inline_hook(skb, p->dev) )
			return 0;
#endif
#endif
		if ( 1 == bind_check )
			return 1;
	}
#endif
	
	return (((p->flags & BR_HAIRPIN_MODE) || skb->dev != p->dev) &&
		p->state == BR_STATE_FORWARDING);
}

static inline int ecnt_hgu_multicast_data(struct sk_buff * skb)
{
	char mac[3]  =  { 0x01,0x00,0x5e};
	char mac2[2] = {0x33,0x33};
	if(NULL == skb || NULL == skb->dev)
    {
        return false;
    }
	if(NULL == skb_mac_header(skb))
	{
		return false;
	}
	if(memcmp((unsigned char*)skb_mac_header(skb), mac, 3) && memcmp((unsigned char*)skb_mac_header(skb), mac2, 2))
	{
		return false;
	}
	if(!__is_ip_udp(skb))
	{
		return false;
	}
	return true;
}

static inline void ecnt_br_flood_inline_hook(struct net_bridge *br, struct sk_buff *skb )
{
	struct net_bridge_port *p;

#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
	int index = -1;
	int hw_flood_flag   = false;
#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
	int igmp_snoop_flag = false;
	int switch_port = 0;
	int port = 0;
	unsigned long long mask = 0;
	int proc_hw_flag = 0;
#endif

/*step1 : add to multicast_flood list*/
#ifdef TCSUPPORT_RA_HWNAT	
#if defined(TCSUPPORT_HWNAT_V3) || defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
	if(xpon_hgu_multicast_data_hook && (!xpon_hgu_multicast_data_hook(skb)))
	{
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
	}
	else if(!xpon_hgu_multicast_data_hook)
	{
		if(ecnt_hgu_multicast_data(skb))
		{
			index = igmp_hwnat_flow_index(skb);
			if(index >= 0)
			{
				hw_flood_flag = true;
			}
			
			if(hw_flood_flag)
			{
				add_multicast_flood_hwentry(skb);
			}
			else{
				if (ra_sw_nat_hook_free)
					ra_sw_nat_hook_free(skb);
			}
		}
		else{
			if (ra_sw_nat_hook_free)
				ra_sw_nat_hook_free(skb);
		}
	}
#elif defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
	proc_hw_flag	= hw_igmp_flood_enable ;
	igmp_snoop_flag = get_multicast_snooping_state_by_skb(skb) ;
	/*disable snooping*/
	if(!igmp_snoop_flag && proc_hw_flag)
	{
		/*hgu multicast data flow*/
		if(xpon_hgu_multicast_data_hook && xpon_hgu_multicast_data_hook(skb))
		{
			/*hw accelerating*/
			index = igmp_hwnat_flow_index(skb);
			if(index >= 0)
			{
				hw_flood_flag = true;
			}
			//printk("line = %d, function= %s,index = %d.\n",__LINE__,__FUNCTION__, index);
		}
	}

	if(hw_flood_flag)
	{
		add_multicast_flood_hwentry(skb);
	}		
	else
	{
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
	}
#else
	{
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
	}
#endif
#endif

	tc3162wdog_kick();

/*step2: check if some port can not be forward, cal the mask*/
#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
	if(hw_flood_flag)
	{
		list_for_each_entry_rcu(p, &br->port_list, list) 
		{
        	if( ecnt_should_deliver(p, skb, 2) )
       		{
                /*calc mask*/
                port = igmp_hwnat_get_port(p);
                /*lan port*/
                if(port >= 0 && port <= 7)
                {
                    mask |= 1 << port;
                }
                /*wifi port*/
                else if(port >= HWNAT_WLAN_IF_BASE)
                {
                    mask |= 1 << port;
                }
            }
        }
	}
#endif

#if defined(TCSUPPORT_XPON_IGMP) && defined(TCSUPPORT_MULTICAST_SPEED)
    /*step3: update mask*/
    if(hw_flood_flag)
    {
        update_multicast_flood_hwentry(index, mask);
        update_multicast_flood_mask(index);
    }
#endif
#else
	{
		if (ra_sw_nat_hook_free)
			ra_sw_nat_hook_free(skb);
	}
#endif

}

static int br_fdb_operator_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int i = 0;
	char val_string[64];
	int  operator = 0;
	struct net_device *netdev = NULL;
	struct net_bridge *br = NULL;
	char name[IFNAMSIZ];
	unsigned char addr[ETH_ALEN] = {0};
	int flag = 0;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	memset(val_string, 0, 64);
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%d", &operator);
	switch(operator)
	{
		case 1:
		{
			memset(name, 0, sizeof(name));
			sscanf(val_string, "%d %s", &operator, name);
			return count;
		}
		case 2:
		{
			memset(addr, 0, sizeof(addr));
			sscanf(val_string, "%d %hhx:%hhx:%hhx:%hhx:%hhx:%hhx",\
				&operator, &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);
			break;
		}
		default:
		{
			return -EINVAL;
		}
	}
	
	netdev = __dev_get_by_name(&init_net, "br0");
	if (NULL != netdev )
	{
		br = netdev_priv(netdev);
	}

	if ( br )
	{
		spin_lock_bh(&br->hash_lock);	
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		for (i = 0; i < BR_HASH_SIZE; i++) 
#endif
		{
			struct hlist_node *h, *g;
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
			hlist_for_each_safe(h, g, &br->fdb_list)
#else
			hlist_for_each_safe(h, g, &br->hash[i])
#endif
			{
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
				struct net_bridge_fdb_entry *f
					= hlist_entry(h, struct net_bridge_fdb_entry, fdb_node);
#else
				struct net_bridge_fdb_entry *f
					= hlist_entry(h, struct net_bridge_fdb_entry, hlist);
#endif
				if ( IS_FDB_LOCAL(f) ||( IS_FDB_STATIC(f)) )
				{
					continue;
				}

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
				if( (2 == operator) && (ether_addr_equal(f->key.addr.addr, addr)) )
#else
				if( (2 == operator) && (ether_addr_equal(f->addr.addr, addr)) )
#endif
				{
					flag = 1;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
					fdb_delete(br, f, true);
#else
					fdb_delete(br, f);
#endif
					goto out;
				}
			}
		}
out:
		spin_unlock_bh(&br->hash_lock);
		
#if 1
		if( 1 == flag )
		{
			if(NULL != hwnat_clean_entry_by_dst_mac_hook)
			{
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
				hwnat_clean_entry_by_dst_mac_hook(addr);
#else
				hwnat_clean_entry_by_dst_mac_hook(addr);
#endif
			}	
		}
#endif
	}
	
	return count;
}

static inline int ecnt_br_fdb_init_inline_hook(void)
{
#if defined(TCSUPPORT_CT)
	struct proc_dir_entry *ptr_proc = NULL;
#endif
	struct proc_dir_entry *br_igmp_proc = NULL;
    
	struct proc_dir_entry *br_host_stb_proc = NULL;

	struct proc_dir_entry *br_fdb_procdir = NULL;
	struct proc_dir_entry *proc = NULL;

	br_fdb_procdir = proc_mkdir("br_fdb", NULL);

	proc_mkdir("br_fdb_host", NULL);
	br_host_stb_proc = create_proc_entry("br_fdb_host/stb_list", 0, NULL);
	if( NULL == br_host_stb_proc )
	{
		printk("ERROR!Create proc entry stb_list fail!\n");
		return -ENOMEM;
	}
	br_host_stb_proc->read_proc = host_list_read_proc;
#if defined(TCSUPPORT_CT)
	ptr_proc = create_proc_entry("tc3162/br_static_mac", 0, NULL);
	if( !ptr_proc )
	{
		printk("ERROR!Create proc entry br_static_mac fail!\n");
		return -ENOMEM;
	}
	ptr_proc->read_proc = ecnt_br_static_mac_read_proc;
	ptr_proc->write_proc = ecnt_br_static_mac_write_proc;
#endif
#ifdef TCSUPPORT_BRIDGE_MAC_LIMIT
	bridgeMacLimitProcInit();
#endif
#if 0
	ether_port_clients_proc_init();
#endif

	br_igmp_proc = create_proc_entry("tc3162/igmp_force_leave", 0, NULL);
	if(NULL == br_igmp_proc)
	{
		printk("ERROR!Create proc entry igmp_force_leave fail!");
		return -ENOMEM;
	}
	br_igmp_proc->read_proc = ecnt_igmp_force_leave_read_proc;
	br_igmp_proc->write_proc = ecnt_igmp_force_leave_write_proc;

	proc = create_proc_entry("operator", 0, br_fdb_procdir);
	if(NULL != proc)
	{
		proc->write_proc = br_fdb_operator_proc;
	}
	else
	{
		printk("ERROR!Create proc entry operator fail!\n");
	}
	
	oltL2BridgeOffProcInit();
		
	return ECNT_CONTINUE;
}

static inline int ecnt_dp_br_fdb_delete_by_port_inline_hook(const struct net_bridge_port *p)
{
	if(p && ra_sw_nat_hook_clean_entry_by_dev) {
		ra_sw_nat_hook_clean_entry_by_dev(p->dev);
	}
	
	return ECNT_CONTINUE;
}

static inline unsigned long hold_time_ecnt(const struct net_bridge *br)
{
	return br->topology_change ? br->forward_delay : br->ageing_time;
}

static inline int has_expired_unicast(const struct net_bridge *br,
				  const struct net_bridge_fdb_entry *fdb)
{
	return !IS_FDB_STATIC(fdb) &&
		time_before_eq(fdb->updated + hold_time_ecnt(br), jiffies);
}

static inline int host_list_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0, i = 0;
	char *stb_mac_list, stb_mac[32] = {0};
	int hasnew = 0;
	int max_stb_cnt = 128;
	struct hlist_node  *n = NULL;
	struct net_bridge_fdb_entry *f = NULL;
	unsigned char *addr = NULL;
	int stb_count = 0;
	static int stb_store_count = 0;
	struct net_device *ndev = NULL;
	struct net_bridge *br = NULL;
#define MAC_LIST_LEN (4096)
	int portNum = 0;
	// read lan link status
	int link_status_read = 0;
	char link_status[32] = {0};
	int lanStatus[MAX_ECNT_ETHER_PORT_ITF_NUM] = {0};
	mm_segment_t orgfs;
	struct file *srcf = NULL; 
	char *src = "/proc/tc3162/eth_port_status";

	stb_mac_list = kmalloc(MAC_LIST_LEN, GFP_ATOMIC);
	if (stb_mac_list == NULL) {
		printk("Allocate memory for stb_mac fail.\n");
		return -1;
	}
	
	memset(stb_mac_list, 0, MAC_LIST_LEN);
	ndev = dev_get_by_name(&init_net, "br0");
	if ( ndev )
		br = netdev_priv(ndev);

	// read lan link status
	orgfs = get_fs();
	set_fs(KERNEL_DS);
	srcf = filp_open(src, O_RDONLY, 0);
	if ( !IS_ERR(srcf) )
	{
		link_status_read = 1;
		ecnt_kernel_fs_read(srcf, link_status, sizeof(link_status) - 1, &srcf->f_pos);
		filp_close(srcf,NULL);
	}
	set_fs(orgfs);
	sscanf(link_status, "%d %d %d %d %d %d %d %d", &lanStatus[0], &lanStatus[1], &lanStatus[2], &lanStatus[3], &lanStatus[4], &lanStatus[5], &lanStatus[6], &lanStatus[7]);
	// end
	if ( br )
	{
		spin_lock_bh(&br->hash_lock);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		for ( i = 0; i < BR_HASH_SIZE; i ++ )
#endif
		{
			//hlist_for_each_entry_safe(f, h, n, &br->hash[i], hlist)
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
			hlist_for_each_entry_safe(f, n, &br->fdb_list, fdb_node)
#else
			hlist_for_each_entry_safe(f, n, &br->hash[i], hlist)
#endif
			{
				if ( IS_FDB_LOCAL(f)
					|| has_expired_unicast(br, f) )
				{
					continue;
				}

#if defined(TCSUPPORT_CT_PMINFORM) || defined(TCSUPPORT_CT_JOYME) || defined(TCSUPPORT_CT_JOYME2) || defined(TCSUPPORT_NP_CMCC)
				portNum = 0;
#endif
				if (isLANInterface(f->dst->dev))
				{
					portNum = getLogicLANIndex(f->dst->dev) + 1;
#if !defined(TCSUPPORT_CT_JOYME)
					if ( portNum >=1 && portNum <= MAX_ECNT_ETHER_PORT_ITF_NUM
						&& 0 == lanStatus[portNum-1] )
					{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
						fdb_delete(br, f, true);
#else
						fdb_delete(br,f);
#endif
						continue;
					}
#endif
				}
				else if ( is5GWiFiInterface(f->dst->dev) )
				{
					portNum = get5GLogicWifiIndex(f->dst->dev) + MAX_ECNT_ETHER_PORT_ITF_NUM + MAX_ECNT_WLAN_PORT_ITF_NUM + 1;
				}
				else if ( is24GWiFiInterface(f->dst->dev) )
				{
					portNum = get24GLogicWifiIndex(f->dst->dev) + MAX_ECNT_ETHER_PORT_ITF_NUM + 1;
				}
				else
				{
					continue;
				}


				stb_count ++;
				if ( stb_count <= max_stb_cnt )
				{
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,15,0)
					addr = f->key.addr.addr;
#else
					addr = f->addr.addr;
#endif
					snprintf(stb_mac, sizeof(stb_mac), "%d=%02x:%02x:%02x:%02x:%02x:%02x\n", portNum,addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
					strcat(stb_mac_list, stb_mac);
				}

			} // hlist_for_each_entry_rcu(f, h, &br->hash[i], hlist)
		} // for ( i = 0; i < BR_HASH_SIZE; i ++ )
		spin_unlock_bh(&br->hash_lock);
              
	} // if ( br )
        if ( ndev )
            dev_put(ndev);
	len = sprintf(buf, "%s", stb_mac_list);

	*start = buf + off;
	if (len < off + count)
		*eof = 1;
	len -= off;
	if (len > count)
		len = count ;
	if (len <0)
		len = 0;

	kfree(stb_mac_list);

	return len;
}

static inline int ecnt_br_mdb_get_inline_hook
(struct sk_buff *skb)
{
#ifdef TCSUPPORT_IGMPSNOOPING_ENHANCE
	struct iphdr *iph = NULL;
	struct udphdr *uh = NULL;

	if ( !skb || htons(ETH_P_IP) != skb->protocol )
		return ECNT_CONTINUE;

	iph = ip_hdr(skb);
	/* ignore for SSDP udp packets. */
	if ( iph 
		&& IPPROTO_UDP == iph->protocol
		&& UPNP_MCAST == iph->daddr )
	{
		uh = (struct udphdr *)(skb_network_header(skb) + (iph->ihl * 4));

		if ( uh
			&& htons(1900) == uh->dest )
		{
			return ECNT_RETURN;
		}
	}
#endif
	return ECNT_CONTINUE;
}
#endif


static inline int ecnt_br_get_snooping_state_inline_hook(struct net_bridge *br, unsigned short protocol, u16 vid)
{
#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
	if(br->vlan_enabled){
		return ecntMulticastVlanDisableGet(br, vid);
	}
#endif

#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
	#ifdef TCSUPPORT_SNOOPING_SEPERATION	
		switch(protocol)
		{
			case htons(ETH_P_IP):
				return br->igmp_disabled;
			case htons(ETH_P_IPV6):
				return br->mld_disabled;
			default:
				return 0;
		}
	#else
		return br->multicast_disabled;
	#endif
#else
	return 0;
#endif
 }

static inline int ecnt_br_set_snooping_state_inline_hook(struct net_bridge *br,unsigned long  val)
{
#if defined(TCSUPPORT_SNOOPING_SEPERATION)&&defined(CONFIG_BRIDGE_IGMP_SNOOPING)
		if(val&(1<<BRCTL_IGMPSNOOPING_OFFSET))
			br->igmp_disabled = !(val&1);
		else if(val&(1<<BRCTL_MLDSNOOPING_OFFSET))
			br->mld_disabled = !(val&1);
		else
			return 0;
	
		br->multicast_disabled = br->igmp_disabled&br->mld_disabled;
	
		return 1;
#else
		return 0;
#endif
}

static inline int ecnt_br_disable_snooping_inline_hook(struct net_bridge *br, u16 vid)
{
#if defined(TCSUPPORT_VLAN_ACCESS_TRUNK)
	if(br->vlan_enabled){
		ecntMulticastVlanDisableSet(br, vid, 1);
		return 1;
	}
#endif

#if defined(TCSUPPORT_SNOOPING_SEPERATION)&&defined(CONFIG_BRIDGE_IGMP_SNOOPING)
	br->igmp_disabled = 1;
	br->mld_disabled = 1;
	br->multicast_disabled = 1;
	return 1;
#else
	return 0;
#endif
}

static inline int 
ecnt_br_drop_ip4_leave_pg_hook(struct net_bridge *br,
			      __be32 group,
				  __u16 vid
			      )
{
    if (igmp_force_leave)
        return 0;
    
#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
#if !defined(TCSUPPORT_CMCC)

	struct net_bridge_mdb_htable *mdb;
	struct net_bridge_mdb_entry *mp;
	struct br_ip br_group;
	
    memset(&br_group, 0, sizeof(struct br_ip));
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	br_group.dst.ip4 = group;
#else
	br_group.u.ip4 = group;
#endif
	br_group.proto = htons(ETH_P_IP);
	br_group.vid = vid;

	spin_lock(&br->multicast_lock);

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
	mp = br_mdb_ip_get(br, &br_group);
#else
	mdb = mlock_dereference(br->mdb, br);
	mp = br_mdb_ip_get(mdb, &br_group);
#endif
	if(!mp)
	{
		goto out;
	}
	if (mp->ports)
	{
		spin_unlock(&br->multicast_lock);
		return 1; 
	}
out:
	spin_unlock(&br->multicast_lock);
#endif
#endif
	return 0;
}

static inline int 
ecnt_br_drop_ip6_leave_pg_hook(struct net_bridge *br,
			      const struct in6_addr *group,
				  __u16 vid
			      )
{
    if (igmp_force_leave)
        return 0;
    
#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
#if !defined(TCSUPPORT_CMCC)
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
#else
	struct net_bridge_mdb_htable *mdb;
#endif
	struct net_bridge_mdb_entry *mp;
	struct br_ip br_group;

	memset(&br_group, 0, sizeof(struct br_ip));
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,10,0)
	br_group.dst.ip6 = *group;
#else
	br_group.u.ip6 = *group;
#endif
	br_group.proto = htons(ETH_P_IPV6);
	br_group.vid = vid;

	spin_lock(&br->multicast_lock);

#if LINUX_VERSION_CODE > KERNEL_VERSION(5,4,0)
	mp = br_mdb_ip_get(br, &br_group);
#else
	mdb = mlock_dereference(br->mdb, br);
	mp = br_mdb_ip_get(mdb, &br_group);
#endif
	if(!mp)
	{
		goto out;
	}
	if (mp->ports)
	{
		spin_unlock(&br->multicast_lock);
		return 1; 
	}
out:
	spin_unlock(&br->multicast_lock);
#endif
#endif
	return 0;
}
static inline int ecnt_br_mdb_get_upnp_inline_hook(struct sk_buff *skb)
{
#ifdef TCSUPPORT_IGMPSNOOPING_ENHANCE
	struct iphdr *iph = NULL;
	struct udphdr *uh = NULL;

	if ( !skb || htons(ETH_P_IP) != skb->protocol )
		return ECNT_CONTINUE;

	iph = ip_hdr(skb);
	/* get UPNP packets. */
	if ( iph 
		&& IPPROTO_UDP == iph->protocol
		&& UPNP_MCAST == iph->daddr )
	{
		return ECNT_RETURN;
	}
#endif
	return ECNT_CONTINUE;
}

static inline int 
ecnt_br_fdb_update_hook(struct net_bridge_port *source, 
	struct net_bridge_port *fdb_dst)
{
	if(0 == olt_l2_bridge_off)
		return ECNT_CONTINUE;
	
	if(isWANInterface(source->dev) && 
		(isLANInterface(fdb_dst->dev) || isWiFiInterface(fdb_dst->dev)))
		return ECNT_RETURN;
	
	return ECNT_CONTINUE;
}

#ifndef AIROHA_BSP
#include "ecnt_br_sdk.h"
#endif/*AIROHA_BSP*/
