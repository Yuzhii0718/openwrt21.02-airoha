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


/************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#if defined(TCSUPPORT_CMCCV2)
#include <linux/inetdevice.h>
#endif

#ifdef TCSUPPORT_CT_DUALWLAN_LED
int isWLanUp = 0;
EXPORT_SYMBOL(isWLanUp);
int isWLan11acUp = 0;
EXPORT_SYMBOL(isWLan11acUp);
int hasWLanClient = 0;
EXPORT_SYMBOL(hasWLanClient);
int hasWLan11acClient = 0;
EXPORT_SYMBOL(hasWLan11acClient);

void (*update_wifi_led_status)(void);
EXPORT_SYMBOL(update_wifi_led_status);
#endif

/****************************************************************************
**function name
	 __vlan_proto
**description:
	get protocol via skb
**return 
	eth_type
**parameter:
	skb: the packet information
****************************************************************************/
static inline __be16 __vlan_proto(const struct sk_buff *skb)
{
	return vlan_eth_hdr(skb)->h_vlan_encapsulated_proto;
}

/****************************************************************************
**function name
	 check_ppp_udp_multicast
**description:
	check multicast packet in downstream
**return 
	0:	check ok or ignore
	-1:	fail
**parameter:
	skb: the packet information
	vdev: virtual net device
****************************************************************************/
int check_ppp_udp_multicast
(struct sk_buff *skb, struct net_device *vdev)
{
#if defined(TCSUPPORT_CT_PON_SC)
	struct pppoe_hdr *ppph = NULL;
	struct iphdr *iph = NULL;
	__be16 ppp_proto = 0;
	unsigned char *pppoe_h = NULL, dest_addr[16] = {0};;
	struct ipv6hdr *ip6h = NULL;
	u8 nexthdr = 0;
	int offset = 0, skip_start = 0;
#define VTAG_SUCCESS 0
	__be16 frag_off;

	if ( htons(ETH_P_PPP_SES) == skb->protocol )
		ppph = pppoe_hdr(skb);
	else if( ( htons(ETH_P_8021Q) == skb->protocol )
			&& ( htons(ETH_P_PPP_SES) == __vlan_proto(skb) ) )
		ppph = (struct pppoe_hdr *)(skb_mac_header(skb) + VLAN_ETH_HLEN);
	else
		return 0;

	pppoe_h = (ppph + 1);
	ppp_proto = *(__be16*)(pppoe_h);
	pppoe_h += 2; /* skip ppp protocol */
	memset(dest_addr, 0, sizeof(dest_addr));
	/* IPv4 */
	if ( PPP_IP == ppp_proto )
	{
		iph = (struct iphdr* )(pppoe_h);
		if ( IPPROTO_UDP != iph->protocol )
			return 0;
		memcpy(dest_addr, (unsigned char*)&iph->daddr, 4);
		if ( 0xe0 != (dest_addr[0] & 0xf0) )
			return 0;

		/* drop it when multicast vlanid isn't cofigured correctly. */
		if ( match_multicast_vtag_check
			&& VTAG_SUCCESS != match_multicast_vtag_check(skb, vdev) )
			return -1;
	}
#ifdef TCSUPPORT_IPV6
	else if ( PPP_IPV6 == ppp_proto ) /* IPv6 */
	{
		ip6h = (struct ipv6hdr* )(pppoe_h);
		nexthdr = ip6h->nexthdr;
		offset = ipv6_skip_exthdr(skb, skip_start, &nexthdr, &frag_off);
		if ( offset >= 0 && IPPROTO_UDP == nexthdr )
		{
			memcpy(dest_addr, ip6h->daddr.s6_addr, 16);
	 		if ( 0xff != dest_addr[0] )
				return 0;

			/* drop it when multicast vlanid isn't cofigured correctly. */
			if ( match_multicast_vtag_check
				&& VTAG_SUCCESS != match_multicast_vtag_check(skb, vdev) )
				return -1;
		}
	}
#endif
#endif

	return 0; /* check ok or ignore. */
}
EXPORT_SYMBOL(check_ppp_udp_multicast);

/****************************************************************************
**function name
	 __is_ip_udp
**description:
	check whether packet is IP udp packets.
**return 
	0:	check ok or ignore
	-1:	fail
**parameter:
	skb: the packet information
	vdev: virtual net device
****************************************************************************/
int __is_ip_udp(struct sk_buff *skb)
{
#if defined(TCSUPPORT_CT_PON_SC) || defined(TCSUPPORT_CT)
	struct iphdr *iph = NULL;
#ifdef TCSUPPORT_IPV6
	struct ipv6hdr *ip6h = NULL;
#endif
	u8 nexthdr = 0;
	int offset = 0, skip_start = 0;
	__be16 frag_off;

	if ( htons(ETH_P_IP) == skb->protocol )
		iph = ip_hdr(skb);
	else if( ( htons(ETH_P_8021Q) == skb->protocol )
			&& ( htons(ETH_P_IP) == __vlan_proto(skb) ) )
		iph = (struct iphdr *)(skb_mac_header(skb) + VLAN_ETH_HLEN);
#ifdef TCSUPPORT_IPV6	
	else if ( skb->protocol == htons(ETH_P_IPV6) )
		ip6h = ipv6_hdr(skb);	
	else if( (skb->protocol == htons(ETH_P_8021Q))
			&& (__vlan_proto(skb) == htons(ETH_P_IPV6)) )
		ip6h = (struct iphdr *)(skb_mac_header(skb) + VLAN_ETH_HLEN);
#endif	
	else
		return 0;

	if ( iph && IPPROTO_UDP == iph->protocol )
		return 1;
#ifdef TCSUPPORT_IPV6
	else if (ip6h )
	{
		nexthdr = ip6h->nexthdr;
		offset = ipv6_skip_exthdr(skb, skip_start, &nexthdr, &frag_off);
		if ( offset >= 0 && IPPROTO_UDP == nexthdr )
			return 1;
		else
			return 0;
	}
#endif
	else
		return 0;
#else
	return 0;
#endif
}
EXPORT_SYMBOL(__is_ip_udp);

/****************************************************************************
**function name
	 tr143RxShortCut
**description:
	tr143 test shortcut data path
**return 
	0:	receive succeed.
	-1:	fail
**parameter:
	skb: the packet information
	vlanLayer: vlan layer counts
	ifaceidx: interface index
	iptype: ip type, 1: IP, 2:PPP, 3:dslite+IP, 4:dslite+PPP 
****************************************************************************/
int tr143RxShortCut(int enable
, struct sk_buff *skb
, int vlanLayer
, int ifaceidx
, int iptype
)
{
#if defined(TCSUPPORT_CT_PON_GDV20) || defined(TCSUPPORT_IS_FH_PON)
	struct iphdr *iph = NULL;
	struct net_device *dev = NULL;
	int isPPP = 0, isDslite = 0, isTag = 0;
	char devName[IFNAMSIZ] = {0};
#define IP6_NETWORK_HLEN	40
#define MAX_SMUX_NUM 8


	switch ( iptype )
	{
		case 1: /* IP */
		case 3: /* dslite+IP */
			isDslite = (3 == iptype ? 1 : 0 );
			snprintf(devName, sizeof(devName) - 1, "%snas%d_%d"
				, ( 1 == isDslite ? "ds." : "")
				, ifaceidx / MAX_SMUX_NUM
				, ifaceidx % MAX_SMUX_NUM );
			break;
		case 2: /* PPP */
		case 4: /* dslite+PPP */
			isDslite = (4 == iptype ? 1 : 0 );
			snprintf(devName, sizeof(devName) - 1, "%sppp%d"
				, ( 1 == isDslite ? "ds." : "")
				, ifaceidx );
			isPPP = 1;
			break;
		default:
			return -2;
	}
	dev = dev_get_by_name(&init_net, devName);
	if ( !dev )
		return -3;

	skb->pkt_type = PACKET_HOST;
	skb->protocol = htons(ETH_P_IP);
	skb->ip_summed = CHECKSUM_UNNECESSARY ;

	/* remove vlan tag */
	if ( vlanLayer != 0 )
		skb_pull(skb, 4*vlanLayer);
	skb_reset_network_header(skb);
	skb_reset_transport_header(skb);
	skb->mac_len = skb->network_header - skb->mac_header;

	skb->dev = dev;
	if ( !skb->skb_iif )
		skb->skb_iif = skb->dev->ifindex;

	if ( 2 == enable ) /* debug on */
	{
		printk("\nTR143 shortcut dev=[%s], iptype=[%d] \n", devName, iptype);
	}

	/* remove ppp header */
	if ( isPPP )
	{
		skb_pull(skb, PPPOE_SES_HLEN);
		skb_reset_network_header(skb);
	}

	/* remove ipv6 header */
	if ( isDslite )
	{
		skb_pull(skb, IP6_NETWORK_HLEN);
		skb_reset_network_header(skb);

	}

	/* remove ip header */
	skb_pull(skb, ip_hdrlen(skb));
	skb_reset_transport_header(skb);
	iph = ip_hdr(skb);
	if ( NULL == skb_dst(skb) )
	{
		ip_route_input_noref(skb, iph->daddr, iph->saddr,
								   iph->tos, skb->dev);
	}

	/* Point into the IP datagram, just past the header. */
	tcp_v4_rcv(skb);
	dev_put(dev);
#endif

	return 0;
}
EXPORT_SYMBOL(tr143RxShortCut);

#if defined(TCSUPPORT_CT_WAN_CHILD_PREFIX)
#define IN6ADDRSZ sizeof(struct in6_addr)
#define INADDRSZ 4
#define INT16SZ 2

/* int
 * inet_pton6(src, dst)
 *  convert presentation level address to network order binary form.
 * return:
 *  1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *  (1) does not touch `dst' unless it's returning 1.
 *  (2) :: in a full address is silently ignored.
 * credit:
 *  inspired by Mark Andrews.
 * author:
 *  Paul Vixie, 1996.
 */
int
inet_pton6(
    const char *src,
    u_char *dst
)
{
    static const char xdigits_l[] = "0123456789abcdef",
              xdigits_u[] = "0123456789ABCDEF";
    u_char tmp[IN6ADDRSZ], *tp, *endp, *colonp;
    const char *xdigits, *curtok;
    int ch, saw_xdigit;
    unsigned val;
	
    memset((tp = tmp), 0, IN6ADDRSZ);
    endp = tp + IN6ADDRSZ;
    colonp = NULL;
    /* Leading :: requires some special handling. */
    if (*src == ':')
        if (*++src != ':')
            return (0);
    curtok = src;
    saw_xdigit = 0;
    val = 0;
    while ((ch = *src++) != '\0') {
        const char *pch;
 
        if ((pch = (char *) strchr((xdigits = xdigits_l), ch)) == NULL)
            pch = (char *) strchr((xdigits = xdigits_u), ch);
        if (pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if (val > 0xffff)
                return (0);
            saw_xdigit = 1;
            continue;
        }
        if (ch == ':') {
            curtok = src;
            if (!saw_xdigit) {
                if (colonp)
                    return (0);
                colonp = tp;
                continue;
            }
            if (tp + INT16SZ > endp)
                return (0);
            *tp++ = (u_char) (val >> 8) & 0xff;
            *tp++ = (u_char) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
	#if 0
        if (ch == '.' && ((tp + INADDRSZ) <= endp) &&
            inet_pton4(curtok, tp) > 0) {
            tp += INADDRSZ;
            saw_xdigit = 0;
            break;  /* '\0' was seen by inet_pton4(). */
        }
	#endif
        return (0);
    }
    if (saw_xdigit) {
        if (tp + INT16SZ > endp)
            return (0);
        *tp++ = (u_char) (val >> 8) & 0xff;
        *tp++ = (u_char) val & 0xff;
    }
    if (colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;
        int i;
 
        for (i = 1; i <= n; i++) {
            endp[- i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp)
        return (0);
    /* bcopy(tmp, dst, IN6ADDRSZ); */
    memcpy(dst, tmp, IN6ADDRSZ);

	return (1);
}

/*******************************************************************************************
**function name
	generate_prefix
**description:
	according the parent PD and childprefixbits in tr069, generate new prefix.
 **retrun 
 	0:success
 	-1:failure
**parameter:
	skb: packet buffer
	pinfo_ptr: prefix infor pointer
********************************************************************************************/
int generate_prefix(struct sk_buff *skb ,struct prefix_info * pinfo_ptr){
	int ret = -1;
	struct inet6_dev *in6_dev = NULL;
	struct in6_addr		parent_prefix;
	struct in6_addr		child_prefix;
	u8	temp_u8 = 0;
	int start_mask = 0, end_mask = 0, mask_range = 0;
	int in6_addr_len = sizeof(struct in6_addr);
	int i = 0, j = 0, k = 0;
	char * tmp_p=NULL;
	char * tmp2_p=NULL;
	char * tmp3_p=NULL;
	char tmpLen_str[40] = {0};
	char parent_prefix_str[64] = {0};
	char child_prefix_str[64] = {0};
	int child_prefix_len = 0, parent_prefix_len = 0;
	
	/*init*/
	if(skb == NULL || pinfo_ptr == NULL){
		goto end;
	}
	in6_dev = in6_dev_get(skb->dev);
	//in6_dev = __in6_dev_get(skb->dev);
	
	if (in6_dev == NULL || in6_dev->dev == NULL) {		
		goto end;
	}
	if(!is_wan_dev(in6_dev->dev)){	
		goto end;
	}
	memset(&(pinfo_ptr->prefix), 0, in6_addr_len);
	pinfo_ptr->prefix_len = 0 ;
	memset(&(parent_prefix), 0, in6_addr_len);
	memset(&(child_prefix), 0, in6_addr_len);
	memcpy(parent_prefix_str, in6_dev->cnf.parent_pd_prefix, sizeof(parent_prefix_str));
	memcpy(child_prefix_str, in6_dev->cnf.child_prefix, sizeof(child_prefix_str));
	if(in6_dev->cnf.child_prefix_orign != PREFIX_ORIGN_SLLA){
		goto end;
	}

	if(child_prefix_str[0] == '\0'){
		goto end;
	}
	/*get child prefix len*/
	tmp_p = strstr(child_prefix_str, "/");
	if(tmp_p == NULL){
		goto end;
	}
	
	tmp2_p = tmp_p+1;
	child_prefix_len = simple_strtoul(tmp2_p, NULL, 10);
	*tmp_p = '\0';

	if(parent_prefix_str[0] == '\0'){
		goto end;
	}
	tmp_p = strstr(parent_prefix_str, "/");
	if(tmp_p == NULL){
		goto end;
	}
	
	tmp2_p = tmp_p+1;
	parent_prefix_len = simple_strtoul(tmp2_p, NULL, 10);
	*tmp_p = '\0';
	if(parent_prefix_len >= MAX_PD_PREFIX){
		goto end;
	}
	/*convert ipv6 string to in6_addr type*/
	if(inet_pton6(parent_prefix_str, parent_prefix.s6_addr) != 1){
		goto end;
	}
	if(inet_pton6(child_prefix_str, child_prefix.s6_addr) != 1){
		goto end;
	}

	start_mask = parent_prefix_len;
	end_mask = child_prefix_len;
	mask_range = end_mask - start_mask;
	if(mask_range <= 0 ){
		if(mask_range == 0){
			memcpy(&(pinfo_ptr->prefix), &(parent_prefix),in6_addr_len);
			pinfo_ptr->prefix_len = parent_prefix_len;
			ret = 0;
		}
		goto end;
	}
	/*calculate new prefix based on childprefix and parentprefix*/
	for(i=start_mask; i<end_mask; i++){
		j = i/8;
		k = i%8;
		temp_u8 = (1<<k);
		temp_u8 &= child_prefix.s6_addr[j];
		parent_prefix.s6_addr[j] |= temp_u8;
	}

	memcpy(&(pinfo_ptr->prefix), &(parent_prefix),in6_addr_len);	
	pinfo_ptr->prefix_len = child_prefix_len;
	
	ret = 0;
	
end:
	if(ret == 0){
		if(pinfo_ptr->prefix_len>64){
			pinfo_ptr->prefix_len = 64;
		}
	}
	return ret;
}
#endif

/****************************************************************************
**function name
	 __is_igmp
**description:
	check whether packet is igmp packets.
**return 
	0:	match fail
	1:	match ok
**parameter:
	skb: the packet information
	vdev: virtual net device
****************************************************************************/
int __is_igmp(struct sk_buff *skb)
{
#if defined(TCSUPPORT_CMCC)
	struct iphdr *iph = NULL;
#ifdef TCSUPPORT_IPV6
	struct ipv6hdr *ip6h = NULL;
#endif
	struct pppoe_hdr *ppph = NULL;
	u8 nexthdr = 0;
	int offset = 0, skip_start = 0;
	unsigned char *pppoe_h = NULL;
	__be16 ppp_proto = 0;
	__be16 frag_off;

	if ( !skb )
		return 0;

	if ( htons(ETH_P_IP) == skb->protocol )
		iph = ip_hdr(skb);
	else if( ( htons(ETH_P_8021Q) == skb->protocol )
			&& ( htons(ETH_P_IP) == __vlan_proto(skb) ) )
		iph = (struct iphdr *)(skb_mac_header(skb) + VLAN_ETH_HLEN);
#ifdef TCSUPPORT_IPV6	
	else if ( skb->protocol == htons(ETH_P_IPV6) )
	{   
		ip6h = ipv6_hdr(skb);	
		skip_start = sizeof(*ip6h) + ETH_HLEN;
	}
	else if( (skb->protocol == htons(ETH_P_8021Q))
			&& (__vlan_proto(skb) == htons(ETH_P_IPV6)) )
	{   
		ip6h = (struct iphdr *)(skb_mac_header(skb) + VLAN_ETH_HLEN);
		skip_start = sizeof(*ip6h) + VLAN_HLEN + ETH_HLEN;
	}
#endif
	else if ( htons(ETH_P_PPP_SES) == skb->protocol )
		ppph = pppoe_hdr(skb);
	else if( ( htons(ETH_P_8021Q) == skb->protocol )
			&& ( htons(ETH_P_PPP_SES) == __vlan_proto(skb) ) )
		ppph = (struct pppoe_hdr *)(skb_mac_header(skb) + VLAN_ETH_HLEN);
	else
		return 0;

	if ( iph && IPPROTO_IGMP == iph->protocol )
		return 1;
#ifdef TCSUPPORT_IPV6
	else if (ip6h )
	{
		nexthdr = ip6h->nexthdr;
		offset = ipv6_skip_exthdr(skb, skip_start, &nexthdr, &frag_off);
		if ( (offset >= 0) && (IPPROTO_ICMPV6 == nexthdr) )
			return 1;
	}
#endif
	else if ( ppph )
	{
		pppoe_h = (ppph + 1);
		ppp_proto = *(__be16*)(pppoe_h);
		pppoe_h += 2; /* skip ppp protocol */
		/* IPv4 */
		if ( IPPROTO_IGMP == ppp_proto )
		{
			return 1;
		}
#ifdef TCSUPPORT_IPV6
		else if ( PPP_IPV6 == ppp_proto ) /* IPv6 */
		{
			ip6h = (struct ipv6hdr* )(pppoe_h);
			nexthdr = ip6h->nexthdr;
			offset = ipv6_skip_exthdr(skb, skip_start, &nexthdr, &frag_off);
			if ( offset >= 0 && IPPROTO_ICMPV6 == nexthdr )
			{
				return 1;
			}
		}
#endif
	}
	else
		return 0;
#endif
	return 0;

}
EXPORT_SYMBOL(__is_igmp);

/****************************************************************************
**function name
	 __is_udp_multicast
**description:
	check multicast packet in downstream
**return 
	0:	match fail
	1:	match ok
**parameter:
	skb: the packet information
	vdev: virtual net device
****************************************************************************/
int __is_udp_multicast(struct sk_buff *skb)
{
#if defined(TCSUPPORT_CMCC)
	struct pppoe_hdr *ppph = NULL;
	struct iphdr *iph = NULL;
	__be16 ppp_proto = 0;
	unsigned char *pppoe_h = NULL, dest_addr[16] = {0};;
#ifdef TCSUPPORT_IPV6
	struct ipv6hdr *ip6h = NULL;
#endif
	u8 nexthdr = 0;
	int offset = 0, skip_start = 0;
	unsigned char *dstAddr = NULL;
	__be16 frag_off;

	if ( !skb )
		return 0;

	dstAddr = eth_hdr(skb)->h_dest;

	if ( dstAddr && 
		(htons(ETH_P_IP) == skb->protocol
		|| ((htons(ETH_P_8021Q) == skb->protocol)
			&& ( htons(ETH_P_IP) == __vlan_proto(skb)))
#ifdef TCSUPPORT_IPV6	
			|| (htons(ETH_P_IPV6) == skb->protocol)
			|| ((htons(ETH_P_8021Q) == skb->protocol)
				&& (htons(ETH_P_IPV6) == __vlan_proto(skb)))
#endif
			)
		&& ((dstAddr[0] & 1)
		&& ((dstAddr[0] & dstAddr[1] & dstAddr[2] 
		& dstAddr[3] & dstAddr[4] & dstAddr[5]) != 0xff)) )
	{
		return 1;
	}
	else if ( htons(ETH_P_PPP_SES) == skb->protocol )
		ppph = pppoe_hdr(skb);
	else if( ( htons(ETH_P_8021Q) == skb->protocol )
			&& ( htons(ETH_P_PPP_SES) == __vlan_proto(skb) ) )
		ppph = (struct pppoe_hdr *)(skb_mac_header(skb) + VLAN_ETH_HLEN);
	else
		return 0;

	if ( ppph )
	{
		pppoe_h = (ppph + 1);
		ppp_proto = *(__be16*)(pppoe_h);
		pppoe_h += 2; /* skip ppp protocol */
		memset(dest_addr, 0, sizeof(dest_addr));
		/* IPv4 */
		if ( PPP_IP == ppp_proto )
		{
			iph = (struct iphdr* )(pppoe_h);
			if ( IPPROTO_UDP != iph->protocol )
				return 0;
			memcpy(dest_addr, (unsigned char*)&iph->daddr, 4);
			if ( 0xe0 != (dest_addr[0] & 0xf0) )
				return 0;
	
			return 1;
		}
#ifdef TCSUPPORT_IPV6
		else if ( PPP_IPV6 == ppp_proto ) /* IPv6 */
		{
			ip6h = (struct ipv6hdr* )(pppoe_h);
			nexthdr = ip6h->nexthdr;
			offset = ipv6_skip_exthdr(skb, skip_start, &nexthdr, &frag_off);
			if ( offset >= 0 && IPPROTO_UDP == nexthdr )
			{
				memcpy(dest_addr, ip6h->daddr.s6_addr, 16);
				if ( 0xff != dest_addr[0] )
					return 0;

				return 1;
			}
		}
#endif
	}
#endif

	return 0;
}
EXPORT_SYMBOL(__is_udp_multicast);

#if defined(TCSUPPORT_FH_JOYMEV2_PON) || defined(TCSUPPORT_CT_JOYME_BANDWIDTH)
int (*wifi_bandwidth_hook_tx) (struct sk_buff * skb) = NULL;
int  (*wifi_bandwidth_hook_rx)(unsigned char* srcMac, int length) = NULL;

EXPORT_SYMBOL(wifi_bandwidth_hook_tx);
EXPORT_SYMBOL(wifi_bandwidth_hook_rx);
#endif


#if defined(TCSUPPORT_CMCCV2)
__be32 ecnt_ipv4_dev_get_saddr_by_index(
	struct net *net, int if_index)
{
	struct net_device *dev = NULL;
	struct in_device *in_dev = NULL;
	__be32 addr = 0;

	dev = dev_get_by_index(net, if_index);
	if(dev){
		in_dev = in_dev_get(dev);		
		if (in_dev) {
			if(in_dev->ifa_list)
				addr = in_dev->ifa_list->ifa_local;	
			in_dev_put(in_dev);
		}
		dev_put(dev);
	}

	return addr;
}
EXPORT_SYMBOL(ecnt_ipv4_dev_get_saddr_by_index);


int ecnt_ipv6_dev_get_saddr_by_index(
	struct net *net, int if_index, struct in6_addr *ifa_saddr, int max_addr)
{
	struct net_device *dev = NULL;
	struct inet6_dev *idev = NULL;
	struct inet6_ifaddr *ifa = NULL;
	int index = 0;
	
	dev = dev_get_by_index(net, if_index);
	if(dev){
		idev = in6_dev_get(dev);		
		if (idev) {
			list_for_each_entry_rcu(ifa, &idev->addr_list, if_list){
				ifa_saddr[index] = ifa->addr;
				index ++;
				if(index >= max_addr)
					break;
			}
			in6_dev_put(idev);
		}
		dev_put(dev);
	}
	return index;
}
EXPORT_SYMBOL(ecnt_ipv6_dev_get_saddr_by_index);
#endif
