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
THAT IT IS RECEIVERS SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVERS SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/igmp.h>
#include "qdma_dev.h"
#include "qdma_bmgr.h"
#include "qdma_dvt.h"
#include "qdma_ic_dis.h"
#include "qdma_glb.h"
#include <linux/random.h>
#include <linux/proc_fs.h>
#include "qdma_pri.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#ifdef CONFIG_TP_IMAGE
int tcp_ack_flag = 1;
#else
int tcp_ack_flag = 0;
#endif /* CONFIG_TP_IMAGE */

extern int priPktChkLen;
extern int priPktChk;
extern unsigned char NATv6_USE_HIGH_PRIORITY;
extern unchar qdma_vip_num;
extern QDMA_VIP_INFO_T qdma_vip_info;

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/
typedef enum{
	LOW_PRIORITY_Q0,
	HIGH_PRIORITY_Q1,
	HIGH_PRIORITY_Q2,
	HIGH_PRIORITY_Q3,
	HIGH_PRIORITY_Q4,
	HIGH_PRIORITY_Q5,
	HIGH_PRIORITY_Q6,
	HIGH_PRIORITY_Q7,
	PRIORITY_UNKNOW,
}pkt_priority;

#define ICMPV6_ROUTE_SOL 133
#define ICMPV6_ROUTE_ADV 134
#define ICMPV6_NEIGH_SOL 135
#define ICMPV6_NEIGH_ADV 136

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/



/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
	
static int set_pppoa_packet_priority(unchar *cp) {
	ushort pppProtocol = *(ushort *) cp;
	if (pppProtocol == htons(0xc021)) {
		return HIGH_PRIORITY_Q7;
	} else if(pppProtocol == htons(0x0021)) {
		cp += 2;
	}
	return LOW_PRIORITY_Q0;
}
static int set_igmp_packet_priority(unchar *cp) {
	struct igmphdr *ih = (struct igmphdr *)(cp + 20);
	if(ih->type == IGMP_HOST_MEMBERSHIP_QUERY) {
		return -1;
	} else {
		return HIGH_PRIORITY_Q7;
	}
}

static int set_icmp_packet_priority(void) {
	return HIGH_PRIORITY_Q7;
}

static int set_arp_packet_priority(void) {
	return HIGH_PRIORITY_Q7;
}

static int set_ipv6_sptype_priority(unchar ipv6_type) {
	if (ipv6_type == ICMPV6_ROUTE_SOL || 
		ipv6_type == ICMPV6_ROUTE_ADV ||
		ipv6_type == ICMPV6_NEIGH_SOL ||
		ipv6_type == ICMPV6_NEIGH_ADV) {
		return HIGH_PRIORITY_Q7;
	}
	return LOW_PRIORITY_Q0;
}

static int set_vip_packet_priority(unchar *cp, unchar ipVerLen, unchar ipProtocol) {
#ifdef TCSUPPORT_FE_VIP_ENHANCEMENT
	ushort sport = *(ushort *)(cp + ((ipVerLen & 0x0f) << 2));
	ushort dport = *(ushort *)(cp + ((ipVerLen & 0x0f) << 2) + 2);
	for (int i = 0; i < qdma_vip_num; i++) {
		if (ipProtocol != qdma_vip_info[i].ip_type) {
			continue;
		}
		if (qdma_vip_info[i].sport_en && qdma_vip_info[i].dport_en) {
			if (qdma_vip_info[i].swap_en) {
				if (((sport == qdma_vip_info[i].sport) && (dport == qdma_vip_info[i].dport)) ||
					((sport == qdma_vip_info[i].dport) && (dport == qdma_vip_info[i].sport))) {
					return HIGH_PRIORITY_Q7;
				}
			} else {
				if ((sport == qdma_vip_info[i].sport) && (dport == qdma_vip_info[i].dport)) {
					return HIGH_PRIORITY_Q7;
				}
			}
		} else if (qdma_vip_info[i].sport_en && (sport == qdma_vip_info[i].sport)) {
			return HIGH_PRIORITY_Q7;
		} else if (qdma_vip_info[i].dport_en && (dport == qdma_vip_info[i].dport)) {
			return HIGH_PRIORITY_Q7;
		}
	}
#endif
	return LOW_PRIORITY_Q0;
}

static int set_dns_packet_priority(unchar *cp, unchar ipVerLen) {
	if(*(ushort *)(cp + ((ipVerLen & 0x0f) << 2) + 2) == htons(0x0035)) {
		return HIGH_PRIORITY_Q7;
	}
	return LOW_PRIORITY_Q0;
}

static int set_lcp_and_ipcpv6_packet_priority(void) {
	return HIGH_PRIORITY_Q7;
}

static int set_ppp_discover_packet_priority(void) {
	return HIGH_PRIORITY_Q7;
}

static int set_udp_packet_priority(unchar *cp, unchar ipVerLen) {
	ushort sport = *(ushort *)(cp + ((ipVerLen & 0x0f) << 2));
	ushort dport = *(ushort *)(cp + ((ipVerLen & 0x0f) << 2) + 2);
	if(sport == htons(0x0223) && dport == htons(0x0222)) {
		return HIGH_PRIORITY_Q7;
	}
	if(sport == htons(0x0222) && dport == htons(0x0223)) {
		return HIGH_PRIORITY_Q7;
	}
	if(sport == htons(0x44) && dport == htons(0x43)) {
		return HIGH_PRIORITY_Q7;
	}
	return LOW_PRIORITY_Q0;
}

static int set_tcp_packet_priority(unchar *cp, unchar ipVerLen, struct sk_buff *skb, unchar ipProtocol) {
	unchar tcpFlags=0;

	/* check if TCP protocol */
	if (ipProtocol != 6) {
		return LOW_PRIORITY_Q0;
	}

	/* align to TCP header */
	cp += (ipVerLen & 0x0f) << 2;
	/* get TCP flags */
	tcpFlags = *(cp + 13);
	
	/* check if TCP fin/syn/reset */
	if (((tcpFlags & 0x01) == 0x01) || ((tcpFlags & 0x02) == 0x02) || ((tcpFlags & 0x04) == 0x04)) {
		return HIGH_PRIORITY_Q7;
	}

	//upstream cloudvr for queue-5, upstream ack for queue-1
	if( NATv6_USE_HIGH_PRIORITY && ((tcpFlags & 0x10) == 0x10) ) {
		if(((skb->mark & QOS_FILTER_MARK) >> 4) > 2){
			skb->mark = (skb->mark & (~QOS_FILTER_MARK)) | 0x20;
		}
	}

	/* check if TCP ack */
	if((tcp_ack_flag == 1) && ((tcpFlags & 0x10) == 0x10)) {
#ifdef CONFIG_TP_IMAGE /*  */
		return HIGH_PRIORITY_Q7;
#else /* CONFIG_TP_IMAGE */
		return HIGH_PRIORITY_Q1;
#endif /* CONFIG_TP_IMAGE */
	}
	
	return LOW_PRIORITY_Q0;
}

int getPktPriority(struct sk_buff *skb, unchar *cp, struct port_info *pMacInfo) {
	ushort etherType = 0;
	unchar ipVerLen = 0;
	unchar ipProtocol = 0;
	ushort pppProtocol = 0;
	unchar ipv6_protocol = 0, ipv6_type = 0;
	//ushort dport = 0, sport = 0;
	//struct igmphdr *ih = NULL;
	//int i = 0;
	int vipPktResult = 0, udpPktResult = 0, tcpPktResult = 0, dnsPktResult = 0,
		pppoaResult = 0, igmpPktResult= 0, ipv6SpTypeResult = 0;

	if (cp == NULL) {
		QDMA_ERR("ERROR: cp is NULL at isPriorityPkt\n");
		return 0;
	}
	
	if (priPktChk && (skb->len > priPktChkLen)) {	/*add "len" to check tcp packet*/
		return LOW_PRIORITY_Q0;
	}

	/*atm router mode, no Mac header and vlan*/
	if((gpQdmaPriv->devCfg.macType == MAC_TYPE_ATM) && ((pMacInfo->atm_pppoa == 1) || (pMacInfo->atm_ipoa == 1))) {
		goto ipv4_header;
	}

	/* skip DA and SA mac address */
	cp += 12;
	/* get ether type */
	etherType = *(ushort *) cp;
	/* skip ether type */
	cp += 2;

	/* EtherWan remove special tag */
	if(pMacInfo->eth_macSTagEn == 1)
	{
		cp += 2;
		etherType = *(ushort *) cp;
		cp += 2;
	}
	
	/*parse if vlan exists*/
	if (etherType == htons(0x8100)) {
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}
		/*parse if vlan exists*/
	if (etherType == htons(0x8100)) {
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}

	/*check whether PPP packets*/
	if (etherType == htons(0x8864)) {
		/* skip pppoe head */
		cp += 6;					/* 6: PPPoE header 2: PPP protocol */
		/* get ppp protocol */
		pppProtocol = *(ushort *) cp;
		/* check if LCP protocol and ipcpv6 protocol */
		if ((pppProtocol == htons(0xc021)) || (pppProtocol == htons(0x8021)) || (pppProtocol == htons(0x8057)) 
			|| (pppProtocol == htons(0xc223)) || (pppProtocol == htons(0xc057))) {
			return set_lcp_and_ipcpv6_packet_priority();
		/* check if IPv6 protocol */
		} 
		else if (pppProtocol == htons(0x0057)) {
			cp += 2;
			cp += 6;
			/* get ip protocol */
			ipProtocol = *(unchar*)cp;
			ipVerLen = 0;
			cp += 34;
		 
			if (ipProtocol == 0x3a) {
				ipv6_type = *(unchar*)cp;
				if(ipv6_type == 130){ 
						return -1;// lan/wifi to wan mld query will drop
				} 
			}

			goto ipv6_header;
		/* check if IP protocol */
		} else if (pppProtocol != htons(0x0021)) {
			return LOW_PRIORITY_Q0;
		}
		/* skip ppp protocol */
		cp += 2;					/* 6: PPPoE header 2: PPP protocol */
	} else if (etherType == htons(0x8863)) {
		return set_ppp_discover_packet_priority();
	} else if (etherType == htons(0x0806)) {
		return set_arp_packet_priority();
	} else if (etherType == htons(0x86dd)) {
		cp += 6;
		 
		ipv6_protocol = *(unchar*)cp;
		cp += 34;
		if (ipv6_protocol == 0x3a) {
			ipv6_type = *(unchar*)cp;
			if(ipv6_type == 130){ 
				return -1;// lan/wifi to wan mld query will drop
			}
		}
		/* get ip protocol */
		ipProtocol = ipv6_protocol;
		if(ipProtocol == 0x11) /* udp + dns */
		{
			dnsPktResult = set_dns_packet_priority(cp,ipVerLen);
			if (dnsPktResult != 0) {
				return dnsPktResult;
			}
		}
		ipVerLen = 0;
		goto ipv6_header;
	} else {
		/* check if ip packet */
		if (etherType != htons(0x0800)) {
			return LOW_PRIORITY_Q0;
		}
	}

ipv4_header:
	pppoaResult = set_pppoa_packet_priority(cp);
	if (pppoaResult != 0) {
		return pppoaResult;
	}

	ipVerLen = *cp;
	if ((ipVerLen & 0xf0) != 0x40) {
		return LOW_PRIORITY_Q0;
	}

	/* get ip protocol */
	ipProtocol = *(cp + 9);

	/* udp + dns */
	if(ipProtocol == 0x11) {
		dnsPktResult = set_dns_packet_priority(cp,ipVerLen);
		if (dnsPktResult != 0) {
			return dnsPktResult;
		}
	}

	/*check if IGMP protocol */
	if(ipProtocol == 2) {
		igmpPktResult = set_igmp_packet_priority(cp);
		if (igmpPktResult != 0) {
			return igmpPktResult;
		}
	}

	/* check if ICMP*/
	if(ipProtocol == 1) {
		return set_icmp_packet_priority();
	}

ipv6_header:

	/* check if ipv6 special type*/
	ipv6SpTypeResult = set_ipv6_sptype_priority(ipv6_type);
	if (ipv6SpTypeResult != 0) {
		return ipv6SpTypeResult;
	}

	/*vip packet*/
	vipPktResult = set_vip_packet_priority(cp, ipVerLen, ipProtocol);
	if (vipPktResult != 0) {
		return vipPktResult;
	}

	/* udp */
	udpPktResult = set_udp_packet_priority(cp, ipVerLen);
	if (udpPktResult != 0) {
		return udpPktResult;
	}

	/* tcp */
	tcpPktResult = set_tcp_packet_priority(cp, ipVerLen, skb, ipProtocol);
	if (tcpPktResult != 0) {
		return tcpPktResult;
	}

	return LOW_PRIORITY_Q0;
}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,6,23)
static ssize_t tcp_ack_flag_read_proc(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	printk("tcp_ack_flag is %d\n",tcp_ack_flag);		
	return 0;
}

static ssize_t tcp_ack_flag_write_proc(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	char valString[8];
	int val = 0;
	
	if (count > sizeof(valString) - 1)
		return -EINVAL;

	if (copy_from_user(valString, buf, count))
		return -EINVAL;

	valString[count] = '\0';
	sscanf(valString, "%d", &val);

	tcp_ack_flag = val;	

	return count;
}

static const struct proc_ops tcp_ack_flag_proc_fops = {
	.proc_read = tcp_ack_flag_read_proc,
	.proc_write = tcp_ack_flag_write_proc,
};

#else
static int tcp_ack_flag_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	printk("tcp_ack_flag is %d\n",tcp_ack_flag);		
	return 0;
}

static int tcp_ack_flag_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char valString[8];
	int val = 0;
	
	if (count > sizeof(valString) - 1)
		return -EINVAL;

	if (copy_from_user(valString, buffer, count))
		return -EINVAL;

	valString[count] = '\0';
	sscanf(valString, "%d", &val);

	tcp_ack_flag = val;	

	return count;
}
#endif
/******************************************************************************
******************************************************************************/

int qdma_pri_proc_init(void){
	struct proc_dir_entry *qdma_proc=NULL;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,6,23)
	qdma_proc = proc_create(tcp_ack_flag_path, 0, NULL, &tcp_ack_flag_proc_fops);
#else
	qdma_proc = create_proc_entry(tcp_ack_flag_path, 0, NULL);
	if (qdma_proc)
	{
		qdma_proc->write_proc = tcp_ack_flag_write_proc;
		qdma_proc->read_proc = tcp_ack_flag_read_proc;
	}
#endif
	return 0;
}

int qdma_pri_proc_exit(void){
	remove_proc_entry(tcp_ack_flag_path, NULL) ;
	return 0;
}

MODULE_LICENSE("Proprietary");

