/*****************************************************************************
 * Airoha (HK) Limited  Airoha. ALL RIGHTS RESERVED.
 * 
 * BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
 * ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
 * DOCUMENTATIONS (??Airoha SOFTWARE??) RECEIVED FROM Airoha 
 * AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ??AS IS?? 
 * BASIS ONLY. Airoha EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
 * WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
 * OR NON-INFRINGEMENT. NOR DOES Airoha PROVIDE ANY WARRANTY 
 * WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
 * MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE Airoha SOFTWARE. 
 * RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
 * WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
 * THAT IT IS RECEIVER??S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
 * PARTY ALL PROPER LICENSES CONTAINED IN Airoha SOFTWARE.
 * 
 * Airoha SHALL NOT BE RESPONSIBLE FOR ANY Airoha SOFTWARE RELEASES 
 * MADE TO RECEIVER??S SPECIFICATION OR CONFORMING TO A PARTICULAR 
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
 * Airoha'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE Airoha 
 * SOFTWARE RELEASED HEREUNDER SHALL BE, AT Airoha'S SOLE OPTION, TO 
 * REVISE OR REPLACE THE Airoha SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
 * LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO Airoha FOR SUCH 
 * Airoha SOFTWARE.
 *
 *****************************************************************************/

/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/

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

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
#if defined(TCSUPPORT_CT_PORTSLIMIT)
unsigned int ecnt_br_deliver_hook_fn(const struct nf_hook_ops *ops,
				   struct sk_buff *skb,
				   const struct net_device *in,
				   const struct net_device *out,
				   int (*okfn)(struct sk_buff *));
#endif

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
#if defined(TCSUPPORT_CT_PORTSLIMIT)
struct nf_hook_ops ecnt_bridge_local_out_hook_op = {
	.hook = ecnt_br_deliver_hook_fn,
	.pf = NFPROTO_BRIDGE,
	.hooknum = NF_BR_LOCAL_OUT,
	.priority = NF_BR_PRI_FIRST,
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,4,90)
	.owner = THIS_MODULE,
#endif	
};
#endif

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/*****************************************************************************
//function :
//		ecnt_net_filter_hook_init
//description : 
//		register nf_hook_ops, called by sock_init()
//input :	
//		void
//return :
//		-1:fail
//		0: success
******************************************************************************/
int ecnt_net_filter_hook_init(void)
{
/*
	if(nf_register_ipv4_hook()){
		printk("nf_register_ipv4_hook() failed\n");
		return -1;
	}
	
	if(nf_register_ipv6_hook()){
		printk("nf_register_ipv6_hook() failed\n");
		return -1;
	}

	if(nf_register_bridge_hook()){
		printk("nf_register_bridge_hook() failed\n");
		return -1;
	}

	if(nf_register_arp_hook()){
		printk("nf_register_arp_hook() failed\n");
		return -1;
	}
*/
#if defined(TCSUPPORT_CT_PORTSLIMIT)
	if(nf_register_hook(&ecnt_bridge_local_out_hook_op)){
		printk("nf_register_hook() ecnt_bridge_local_out_hook_op failed\n");
		return -1;
	}
#endif
	return 0;
}

#if defined(TCSUPPORT_CT_PORTSLIMIT)
unsigned int ecnt_br_deliver_hook_fn(const struct nf_hook_ops *ops,
			       struct sk_buff *skb,
			       const struct net_device *in,
			       const struct net_device *out,
			       int (*okfn)(struct sk_buff *))
{
	if ( skb->vlan_tag_flag & VLAN_TAG_IGMP_QUERYFLAG )
	{
		skb->vlan_tag_flag &= ~(VLAN_TAG_IGMP_QUERYFLAG);
		if ( NULL != skb->dev
			&& ('n' == skb->dev->name[0] && 'a' == skb->dev->name[1] && 's' == skb->dev->name[2]) )
		{
			return NF_DROP;
		}
	}
	
	return NF_ACCEPT;

}
#endif

