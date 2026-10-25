/*****************************************************************************
 * Airoha (HK) Limited  Airoha. ALL RIGHTS RESERVED.
 * 
 * BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
 * ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
 * DOCUMENTATIONS (��Airoha SOFTWARE��) RECEIVED FROM Airoha 
 * AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
 * BASIS ONLY. Airoha EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
 * WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
 * OR NON-INFRINGEMENT. NOR DOES Airoha PROVIDE ANY WARRANTY 
 * WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
 * MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE Airoha SOFTWARE. 
 * RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
 * WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
 * THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
 * PARTY ALL PROPER LICENSES CONTAINED IN Airoha SOFTWARE.
 * 
 * Airoha SHALL NOT BE RESPONSIBLE FOR ANY Airoha SOFTWARE RELEASES 
 * MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND 
 * Airoha'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE Airoha 
 * SOFTWARE RELEASED HEREUNDER SHALL BE, AT Airoha'S SOLE OPTION, TO 
 * REVISE OR REPLACE THE Airoha SOFTWARE AT ISSUE OR REFUND ANY SOFTWARE 
 * LICENSE FEES OR SERVICE CHARGES PAID BY RECEIVER TO Airoha FOR SUCH 
 * Airoha SOFTWARE.
 *
 *****************************************************************************/

/****************************************************************************
**function name
	 get_fdb_by_skb
**description:
	get net bridge fdb entry by skb information
**return 
	dst: if find match entry
	NULL: if not find match entry
**parameter:
	skb: the packet information
****************************************************************************/
struct net_bridge_fdb_entry *get_fdb_by_skb(struct sk_buff *skb)
{
	unsigned char dest[6] = {0};
	struct net_bridge_fdb_entry *dst = NULL;
	struct net_bridge_port *p = br_port_get_rcu(skb->dev);
	struct net_bridge *br;

	if (!p || p->state == BR_STATE_DISABLED)
	{
		return NULL;
	}

	memcpy(dest, skb->data, sizeof(dest));
	if(is_broadcast_ether_addr(dest))
	{
		if(get_client_mac_from_dhcp_packet(skb, dest) == -1)
		{
			return dst;
		}

	}

	br = p->br;
	dst = __br_fdb_get(br, dest,0);
	return dst;
}
EXPORT_SYMBOL(get_fdb_by_skb);


