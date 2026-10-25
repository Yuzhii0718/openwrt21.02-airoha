#include <net/sock.h>
#include <linux/qos_type.h>
#include <linux/gfp.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/ppp_defs.h>
#include <net/arp.h>
#include <linux/if_vlan.h>
#include <linux/if_pppox.h>
#include <ecnt_hook/ecnt_hook.h>
#include <linux/proc_fs.h>
#if defined(TCSUPPORT_CT_JOYME4)
#include <uapi/common/ecnt_global_macro.h>
#endif
#include <lan_port/lan_port_info.h>

static struct qos_action qa[MAX_RULE_NUM];
#if defined(TCSUPPORT_CT_JOYME4)
static unsigned char wan_remark_queue[MAX_WAN_IF_INDEX];
#endif

extern int ipv6_skip_exthdr(const struct sk_buff *skb, int start, u8 *nexthdrp,__be16 *frag_offp);

#if defined(TCSUPPORT_CT_JOYME4)
/*
* get queue index from wan
*/
int getQueueFromTemplate(char *wan_name)
{
	int pvc_idx = 0, entry_idx = 0, ifidx = 0;

	if ( 'n' != wan_name[0] )
		return -1;

	sscanf(wan_name, "nas%d_%d", &pvc_idx, &entry_idx);
	ifidx = pvc_idx * MAX_SMUX_NUM + entry_idx;
	if ( ifidx >= MAX_WAN_IF_INDEX )
		return -1;

	return wan_remark_queue[ifidx];
}
#endif


/*for joyme4 downstream dscp replace*/
static int getIpProtocolId(struct sk_buff* skb)
{
	unsigned short eth_type = 0;
	unsigned char* buff = NULL;
	unsigned short* tmp = NULL;
	unsigned short pppoe_proto;
	int protocol = 0;
	unsigned char nexthdr = 0;
	unsigned short frag_off = 0;
	int protoff = 0;
	int start = 0;
	
	buff = skb->data;

	buff += 12; //skip 2 byte 
	start += 12;

	tmp = (u16 *)buff;
	if(*tmp == htons(ETH_P_8021Q))
	{
		buff += 4;//only support 2layer vlan
		start += 4;
	}
	eth_type = *(u16*)buff;

	if((eth_type != htons(ETH_P_PPP_SES))&&(eth_type != htons(ETH_P_IP))&&(eth_type != htons(ETH_P_IPV6)))
		return -1;

	buff += 2; // skip ether type
	start += 2;

	if (eth_type == htons(ETH_P_PPP_SES))
	{
		buff+= sizeof(struct pppoe_hdr);
		start += sizeof(struct pppoe_hdr);
		pppoe_proto = *(u_int16_t*)buff;

		buff += 2; // skip ppp header
		start += 2;

		if (pppoe_proto == htons(PPP_IP))
			eth_type = htons(ETH_P_IP);
		else if (pppoe_proto == htons(PPP_IPV6))
			eth_type = htons(ETH_P_IPV6);
	}

	if (eth_type == htons(ETH_P_IP)){
		struct iphdr *iph = (struct iphdr*)buff;

		if (iph->version != 4)
			return -1;
		
		return iph->protocol; 
	}
	else if (eth_type == htons(ETH_P_IPV6)){
		struct ipv6hdr *ip6hdr = (struct ipv6hdr*)buff;
		if (ip6hdr->version != 6)
			return -1;

		start += sizeof(*ip6hdr);
		nexthdr = ip6hdr->nexthdr;
		protoff = ipv6_skip_exthdr(skb,start,&nexthdr,&frag_off);
		if((protoff < 0) || ((frag_off & htons(~0x7)) != 0))
			return -1;

		return nexthdr;
	}

	return -1;
}
static inline int ipv4SetDscp(struct iphdr *iph,u_int8_t dscp)
{
	iph->tos &= 0x03;
	iph->tos |= (dscp<<2);
	iph->check = 0;
	iph->check = ip_fast_csum((u8 *)iph, iph->ihl);

	return 0;
}

static inline int ipv6SetDscp(struct ipv6hdr *ipv6h,u_int8_t dscp)
{
	(*(u16 *) ipv6h) &= htons(0xf03f);
	(*(u16 *) ipv6h) |= htons(dscp<<6);

	return 0;
}

static inline int ipv6SetTC(struct ipv6hdr *ipv6h,u_int8_t tc)
{
	(*(u16 *) ipv6h) &= htons(0xf00f);
	(*(u16 *) ipv6h) |= htons(tc<<4);

	return 0;
}

static int setSkbDscp(struct sk_buff** pskb,u_int8_t dscp)
{
	unsigned short eth_type = 0;
	unsigned char* buff = NULL;
	unsigned short* tmp = NULL;
	unsigned short pppoe_proto;

	*pskb = skb_unshare(*pskb, GFP_ATOMIC);
	if (!*pskb)
	{
		return -ENOMEM;
	}
	
	buff = (*pskb)->data;

		buff += 12; //skip 2 byte 
	
		tmp = (u16 *)buff;
		if(*tmp == htons(ETH_P_8021Q))
		{
			buff += 4;//only support 2layer vlan
		}
		eth_type = *(u16*)buff;

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
			
			return ipv4SetDscp(iph,dscp); 
		}
		else if (eth_type == htons(ETH_P_IPV6)){
			struct ipv6hdr *ip6hdr = (struct ipv6hdr*)buff;
			if (ip6hdr->version != 6)
				return -1;

#if defined(TCSUPPORT_CT_JOYME4)
			return ipv6SetDscp(ip6hdr,dscp);
#else
			return ipv6SetTC(ip6hdr,dscp);
#endif
		}
	return -1;
}
/*for joyme4 downstream dscp replace*/
int qostype_chk(int chk_type, int rule_no, char *wan_if, int rtp_match)
{
	struct qos_action *pqa = NULL;
	char value[MAX_BUF_LEN];
	char *pval = NULL;
	char *p = NULL;
	
	if (rule_no < 0 || rule_no > MAX_RULE_NUM - 1) {
		printk("%s:rule no should be between 0 and %d.\n", __FUNCTION__, MAX_RULE_NUM - 1);
		return -1;
	}

	pqa = qa + rule_no;
	
	if (chk_type == EBT_CHK_TYPE) {
		if (!strcmp(pqa->qm[0].type_name, "wan_if") || !strcmp(pqa->qm[1].type_name, "rtp_proto")) {
			return 0;
		}
		else {
			return -1;
		}
	}
	
	if (wan_if == NULL) {
		return -1;
	}

	if (!strcmp(pqa->qm[0].type_name, "wan_if")) {
		strcpy(value, pqa->qm[0].type_value);
		/* check if wan interface is in group */
		pval = value;
		while ((p = strsep(&pval, ",")) != NULL) {
			if (!strcmp(p, wan_if)) {
				goto rtp_proto_handle;
			}
		}
		return -1;
	}

rtp_proto_handle:
	if ( (strcmp(pqa->qm[1].type_name, "rtp_proto") != 0) || 
		 (!strcmp(pqa->qm[1].type_name, "rtp_proto") && (1 == rtp_match)) ) {
		return 0;
	}

	return -1;
}

#if defined(TCSUPPORT_CT_QOS)
int qostype_get(int rule_no, char *wan_if, int *p_mark, struct sk_buff* skb)
{
	int idx = 0;
	int isQueueWanset = 0;
	int queue_idx = 0;
	
	if ( rule_no < 0 || rule_no > MAX_RULE_NUM - 1)
	{
		printk("%s:rule no should be between 0 and %d.\n", __FUNCTION__, MAX_RULE_NUM - 1);
		return 0;
	}

	if ( !wan_if || !p_mark )
		return 0;

	/*Check waninterface fail, looking for other waninterface qos rules. 
	However, it can't check the rules of other types which checked by ebtables.
	So,  the waninterface rule can't be combined with other rules.*/
	for ( idx=0; idx<MAX_RULE_NUM; idx++ )
	{
	    if ( 0 == qa[idx].qm[0].type_name[0] )
	        continue;

		queue_idx = (qa[idx].qm[0].type_mask & QOSTYPE_QUEUE_INDEX_MARK)
						>> QOSTYPE_QUEUE_INDEX_OFFSET;

	    if ( 0 == strcmp(qa[idx].qm[0].type_name, "wan_if") )
	    {
	    	if ( 0 == isQueueWanset
				&& queue_idx == rule_no )
				isQueueWanset = 1; /* queue match */

	        if ( (0 == strcmp(qa[idx].qm[0].type_value, wan_if))
				&& ((qa[idx].qm[0].type_protocol_type == 258) || /* ALL*/
				((qa[idx].qm[0].type_protocol_type == 257) /* ICMP/ICMPv6 */
				&& (IPPROTO_ICMP == getIpProtocolId(skb) || IPPROTO_ICMPV6 == getIpProtocolId(skb))) ||
				((qa[idx].qm[0].type_protocol_type == 256) /* TCP/UDP */
				&& (IPPROTO_TCP == getIpProtocolId(skb) || IPPROTO_UDP == getIpProtocolId(skb))) 
				||(qa[idx].qm[0].type_protocol_type == getIpProtocolId(skb))))
			{
				*p_mark = qa[idx].qm[0].type_mask;
	            return queue_idx;
	    	}
		}
	}

	/* wan type in queue, but not match */
	if ( 1 == isQueueWanset )
		return -1;
	else
		return 0;
}

int qostype_wan_check(int rule_no, struct net_device *dev, int *p_mark, struct sk_buff* skb)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	if ( dev->priv_flags & IFF_OSMUX )
#else
	if ( dev->dev_flags & IFF_OSMUX )
#endif
	{
		return qostype_get(rule_no, dev->name, p_mark, skb);
	}

	return 0;
}

int qos_wan_interface_hook(struct sk_buff **pskb)
{
	int ret_qoswan = 0;
	int idx;
	struct sk_buff* skb = *pskb;
	struct net_device *dev = NULL;

	if ( NULL == skb || NULL== skb->dev )
		return ECNT_CONTINUE;

	if (unlikely(skb->dev->name[0] == 'n') && (skb->mark & LANIF_MASK))
	{
		dev = skb->dev;
		
		for(idx = 0; idx < 8; idx++ )
		{
			if((!strcmp(qa[idx].qm[0].type_name, "wan_if")) && (!strcmp(qa[idx].qm[0].type_value, skb->dev->name))
				&& ((qa[idx].qm[0].type_protocol_type == 258) || /* ALL*/
				((qa[idx].qm[0].type_protocol_type == 257) /* ICMP/ICMPv6 */
				&& (IPPROTO_ICMP == getIpProtocolId(skb) || IPPROTO_ICMPV6 == getIpProtocolId(skb))) ||
				((qa[idx].qm[0].type_protocol_type == 256) /* TCP/UDP */
				&& (IPPROTO_TCP == getIpProtocolId(skb) || IPPROTO_UDP == getIpProtocolId(skb))) 
				||(qa[idx].qm[0].type_protocol_type == getIpProtocolId(skb)) )
				){
				if(qa[idx].qm[0].type_mask & QOSTYPE_DSCP_SW_MARK)/*dscp*/
				{
					setSkbDscp(pskb, qa[idx].qm[0].type_mask &QOSTYPE_DSCP_MARK);
				}

				/* remark 8021p and queue index */
				skb = *pskb;
				skb->mark &= (~QOS_FILTER_MARK);
				skb->mark |= (((qa[idx].qm[0].type_mask & QOSTYPE_QUEUE_INDEX_MARK) >> QOSTYPE_QUEUE_INDEX_OFFSET) << 4);

				skb->mark &= (~QOSTYPE_8021P_MARK);
				skb->mark |= (qa[idx].qm[0].type_mask & QOSTYPE_8021P_MARK);
			}
		}

#if defined(TCSUPPORT_CT_JOYME4)
		if ( 0 == (skb->mark & QOS_FILTER_MARK) )  /* default queue. */
		{
			/* check qos template wan match. */
			ret_qoswan = getQueueFromTemplate(dev->name);
			if ( ret_qoswan > 0 ) /* match wan. */
			{
				skb->mark &= (~QOS_FILTER_MARK);
				skb->mark |= (ret_qoswan << 4);
			}
		}
#endif
	}

	return ECNT_CONTINUE;
}
#endif

int set_tos(int rule_no, unsigned int tos)
{
	struct qos_action *pqa = NULL;
	if (rule_no < 0 || rule_no >= MAX_RULE_NUM) {
		printk("%s:rule no should be between 0 and %d.\n", __FUNCTION__, MAX_RULE_NUM - 1);
		return -1;
	}

	pqa = &qa[rule_no];

	pqa->dscp_flag = 1;
	pqa->dscp = tos;
	return 0;
}

int unset_tos(int rule_no)
{
	struct qos_action *pqa = NULL;
	if (rule_no < 0 || rule_no >= MAX_RULE_NUM) {
		printk("%s:rule no should be between 0 and %d.\n", __FUNCTION__, MAX_RULE_NUM - 1);
		return -1;
	}

	pqa = &qa[rule_no];

	pqa->dscp_flag = 0;

	return 0;
}

int get_tos(int rule_no, unsigned int *tos)
{
	struct qos_action *pqa = NULL;
	if (rule_no < 0 || rule_no >= MAX_RULE_NUM) {
		printk("%s:rule no should be between 0 and %d.\n", __FUNCTION__, MAX_RULE_NUM - 1);
		return -1;
	}

	pqa = &qa[rule_no];

	if (0 == pqa->dscp_flag) {
		return -1;
	}

	*tos = pqa->dscp;

	return 0;
}

static int qostype_settype(struct qos_type *pqt)
{
	int rule_no = pqt->rule_no;
	if (rule_no < 0 || rule_no > MAX_RULE_NUM - 1) {
		printk("%s:rule no should be between 0 and %d.\n", __FUNCTION__, MAX_RULE_NUM - 1);
		return -1;
	}

	memcpy(qa[pqt->rule_no].qm, pqt->qm, sizeof(struct qos_match)*2);

	return 0;
}

int
qostype_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct qos_type qt;
#if defined(TCSUPPORT_CT_JOYME4)
	unsigned char temp_queue[MAX_WAN_IF_INDEX];
#endif
	
	switch (cmd) {
	case QOSTYPE_IOC_SET_TYPE:
		if (copy_from_user(&qt, (struct qos_type*)arg, sizeof(qt))) {
			return -EFAULT;
		}
		qostype_settype(&qt);
		break;
#if defined(TCSUPPORT_CT_JOYME4)
	case QOSTYPE_IOC_SET_TEMPLATE_WAN:
		if ( copy_from_user(temp_queue, (unsigned char*)arg, MAX_WAN_IF_INDEX) )
			return -EFAULT;
		memcpy(wan_remark_queue, temp_queue, MAX_WAN_IF_INDEX);
		break;
#endif
	default:
		break;
	}

	return 0;
}

static int qos_type_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0, i=0;
	off_t begin=0 ;

	index += sprintf(buf+index, "rule_num\tqm[0].name\tqm[0].value\tqm[0].mask\tqm[1].name\tqm[1].value\tqm[1].mask\n");

	for(i=0;i<MAX_RULE_NUM;i++)
	{
		index += sprintf(buf+index, "%d:\t\t%s\t\t%s\t\t%d\t\t%s\t\t%s\t\t%d\n",i,
			qa[i].qm[0].type_name[0]==0?"NULL":qa[i].qm[0].type_name,
			qa[i].qm[0].type_value[0]==0?"NULL":qa[i].qm[0].type_value,qa[i].qm[0].type_mask,
			qa[i].qm[1].type_name[0]==0?"NULL":qa[i].qm[1].type_name,
			qa[i].qm[1].type_value[0]==0?"NULL":qa[i].qm[1].type_value,qa[i].qm[1].type_mask);
	}
	
	*eof = 1;

	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0)
		index = 0;
	if (index>count)
		index = count;
	return index;
}

int qostype_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static struct file_operations qostype_fops = {
	.owner =		THIS_MODULE,
	.write =		NULL,
	.read =			NULL,
	.unlocked_ioctl =		qostype_ioctl,
	.open =			qostype_open,
	.release =		NULL,
};

/***************************************************************************
 * Function Name: qostype_drv_init
 * Description  : Initialization of qostype driver
 * Returns      : 0
 ***************************************************************************/
static int __init qostype_drv_init(void)
{
	int status = 0;
	struct proc_dir_entry *qos_action_proc = NULL;
	
	/* qostype ioctl */
	status = register_chrdev(QOSTYPE_MAJOR, "qostype", &qostype_fops);
	if (status < 0)
		return status;
	
	memset(qa, 0, sizeof(qa));
#if defined(TCSUPPORT_CT_JOYME4)
	memset(wan_remark_queue, 0, sizeof(wan_remark_queue));
#endif

	qos_action_proc = create_proc_entry("tc3162/qos_type", 0, NULL);
	qos_action_proc->read_proc = qos_type_read_proc;

  	return 0;
}


/*_______________________________________________________________________
** Function Name: qostype_drv_exit
** Description  : qostype module clean routine
** Returns      : None
**_______________________________________________________________________
*/
static void __exit qostype_drv_exit(void)
{
	unregister_chrdev(QOSTYPE_MAJOR, "qostype");
	remove_proc_entry("tc3162/qos_type", NULL);
}

EXPORT_SYMBOL(qostype_chk);
EXPORT_SYMBOL(set_tos);
EXPORT_SYMBOL(unset_tos);

module_init(qostype_drv_init);
module_exit(qostype_drv_exit);

