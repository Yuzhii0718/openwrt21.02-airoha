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

#include <linux/version.h>
#include <linux/ctype.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/if_vlan.h>
#include <linux/if_pppox.h>
#include <linux/ppp_defs.h>
#include <linux/skbuff.h>
#include <linux/foe_hook.h>
#include <linux/libcompileoption.h>
#include <net/ipv6.h>
#include <net/ip.h>
#include <ecnt_hook/ecnt_hook_ifc_type.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,115)
#include <linux/proc_fs.h>
#endif
#include "ifc_dev.h"
#include "ifc_util.h"
#include "ifc_tcam.h"
#include "ifc_api.h"
#ifdef RDKB_BUILD
#include "fe_reg_en7512.h"
#else
#include "../fe/en7512/fe_reg_en7512.h"
#endif
#include "ifc_ic_dis.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


/************************************************************************
*                  M A C R O S
*************************************************************************
*/


/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern int clear_all_ifc_actions(void);
extern int ifc_set_lut1_rules(uint ifcIndex, IFC_Mode_t enMode, uint endFlag, uint fieldIdx, uint command, uint mask, uint key_high, uint key_low);
extern int ifc_set_action(uint ifcIndex, uint actIdx, IFC_Mode_t enMode, uint value0, uint value1, uint value2, uint value3);


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int ifc_rule_init(void);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
int icmp_ring_index=0;
int icmpv6_ring_index=0;

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
typedef struct _ifc_hit_idx_software_action_s
{
    unsigned char using;
    unsigned char action;
    unsigned int ifc_idx;
}ifc_hit_idx_software_action_t;

#define HIT_IFC_MAX 2
static ifc_hit_idx_software_action_t ifc_idx_software_action[HIT_IFC_MAX];

#ifdef INCLUDE_IPSEC_HWNAT_DISABLE
#define UINT32 unsigned int
#define IPSECVPN_RULE_IDX_MAX	8
typedef struct {
	UINT32 src; 
	UINT32 src_mask;
	UINT32 dst;
	UINT32 dst_mask;
	int ifc_idx;
	int enable;
} IPSEC_RULE_ENTRY;

static void init_ipsecvpn_ifc_rule(void);
static int add_ipsecvpn_ifc_rule(IPSEC_RULE_ENTRY *pRule, unsigned int bAction);

static int ipsec_rule_read_proc(char *page, char **start, off_t off,
												int count, int *eof, void *data);
static int ipsec_rule_write_proc(struct file *file, const char *buffer,
												unsigned long count, void *data);
static IPSEC_RULE_ENTRY* find_valid_entry(const IPSEC_RULE_ENTRY *pEntry, int bExist);
static void usage_ipsecskip(void);

IPSEC_RULE_ENTRY g_ipsecRules[IPSECVPN_RULE_IDX_MAX] = {{0,0,0,0,-1}};
unsigned int g_ipsecRuleNum = 0;
#endif /* INCLUDE_IPSEC_HWNAT_DISABLE */
/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
#ifdef INCLUDE_IPSEC_HWNAT_DISABLE
static void usage_ipsecskip(void)
{
	printk("echo <enable> <hex_srcip>/<hex_srcmask> <hex_dstip>/<hex_dstmask> > /proc/ipsec_skip\n");
	return;
}

/* 
 * fn       static void find_valid_entry
 * brief    find the specified entry in the rule data list
 * details  if bExist is true, then try to find one enabled entry with the same src/dst value in the data list;
 *          else then try to find one entry with enable is false.
 *
 * param[in]    const IPSEC_RULE_ENTRY *pEntry    -    then entry data try to find
 *              int bExist    -    try to find one exist entry or not exist entry
 * param[out]   N/A
 *
 * return       IPSEC_RULE_ENTRY *
 * retval       NULL     -    Not find the specified data entry
 *          Others   -    The data entry in the list that we want to find.
 * note
 */
static IPSEC_RULE_ENTRY* find_valid_entry(const IPSEC_RULE_ENTRY *pEntry, int bExist)
{
	int idx = 0;
	IPSEC_RULE_ENTRY *pos = g_ipsecRules;

	if (NULL == pEntry)
	{
		printk("pEntry is NULL, please check!");
		return NULL;
	}

	while(idx < IPSECVPN_RULE_IDX_MAX)
	{
		pos = &g_ipsecRules[idx];

		if ( bExist && 0 < pos->enable )
		{
			if ((pEntry->dst & pEntry->dst_mask) == (pos->dst & pos->dst_mask) 
				&& (pEntry->src & pEntry->src_mask) == (pos->src & pos->src_mask))
			{
				return pos;
			}
		}
		else if ( !bExist && 0 >= pos->enable )
		{
			return pos;
		}
		idx++;
	}

	return NULL;
}

/* 
 * fn       static void init_ipsecvpn_ifc_rule
 * brief    init ipsec rule data and create proc file
 * details  
 *
 * param[in]	N/A
 * param[out]	N/A
 *
 * return	N/A
 * retval	N/A
 *
 * note		
 */
static void init_ipsecvpn_ifc_rule(void)
{
	int i = 0;
	struct proc_dir_entry *ipsec_proc = NULL;

	if (0 != g_ipsecRuleNum)
	{
		printk("%s(): ipsecvpn_rule_num(%d), seems already inited.\n", __FUNCTION__, g_ipsecRuleNum);
		g_ipsecRuleNum = 0;
	}

	memset(g_ipsecRules, -1, sizeof(g_ipsecRules));

	ipsec_proc = create_proc_entry("ipsec_skip", 0, NULL);
	ipsec_proc->write_proc = ipsec_rule_write_proc;
	ipsec_proc->read_proc = ipsec_rule_read_proc;
}

/* 
 * fn       static void ipsec_rule_read_proc
 * brief    dump the ipsec ifc rule data for check or test
 * details  
 *
 * param[in]	standard proc function format
 * param[out]	standard proc function format
 *
 * return	N/A
 * retval	N/A
 *
 * note		
 */
static int ipsec_rule_read_proc(char *page, char **start, off_t off,
												int count, int *eof, void *data)
{
	int len = 0;
	int i = 0;
	IPSEC_RULE_ENTRY *pos = g_ipsecRules;

	len += snprintf(page + len, count - len, 
						"total ipsec skip rule num:%d\n", g_ipsecRuleNum);
	if (len >= count)
	{
		goto end;
	}
	
	for (i = 0; i < IPSECVPN_RULE_IDX_MAX; i++)
	{
		pos = &g_ipsecRules[i];
		len += snprintf(page + len, count - len,
			"%d: idx:%d enable:%d src:0x%x/0x%x, dst:0x%x/0x%x\n",
			i, pos->ifc_idx, pos->enable, pos->src, pos->src_mask, pos->dst, pos->dst_mask);
		if (len >= count)
		{
			goto end;
		}
	}

end:
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

/* 
* fn			static void ipsec_rule_write_proc
* brief			set the ifc rule for ipsec by proc file /proc/ipsec_skip
* details		create or delete entry for ipsec traffic
*
* param[in]		standard proc function format
* param[out]	standard proc function format
*
* return	N/A
* retval	N/A
*
* note 	
*/
static int ipsec_rule_write_proc(struct file *file, const char *buffer,
												unsigned long count, void *data)
{
	char valString[128];
	int argnum = 0;
	int bAdd = 0;
	IPSEC_RULE_ENTRY entry = {0};
	IPSEC_RULE_ENTRY *pEntry = NULL;

	if (count > sizeof(valString) - 1)
	{
		printk("%s() count(%d) too large!\n", __FUNCTION__, count);
		return -EINVAL;
	}
	
	if (copy_from_user(valString, buffer, count))
		return -EFAULT;

	valString[count] = '\0';

	argnum = sscanf(valString, "%d %x/%x %x/%x", &bAdd,
					&entry.src, &entry.src_mask, &entry.dst, &entry.dst_mask);
	if (5 != argnum)
	{
		usage_ipsecskip();
		return count;
	}

	pEntry = find_valid_entry(&entry, !bAdd);

	if(NULL == pEntry)
	{
		printk("failed to find valid entry for this op!\n");
	}
	else
	{
		entry.ifc_idx = pEntry->ifc_idx;
		memcpy(pEntry, &entry, sizeof(entry));
		if (0 != add_ipsecvpn_ifc_rule(pEntry, bAdd))
		{
			printk("failed to set ipsec skip rule!\n");
		}
		pEntry->enable = bAdd?1:0;
	}

	return count;
}

/* 
* fn			static void add_ipsecvpn_ifc_rule
* brief 		set(add or del) ifc rule according entry
* details		
*
* param[in] 	IPSEC_RULE_ENTRY *pEntry	-	the rule need to set
*				unsigned int bAdd			-	the rule to add or delete
* param[out]	IPSEC_RULE_ENTRY *pEntry	-	the rule after set
*
* return	int
* retval	0	-	success
*
* note	
*/
static int add_ipsecvpn_ifc_rule(IPSEC_RULE_ENTRY *pEntry, unsigned int bAdd)
{
	int ifc_idx = 0;
	
	if (bAdd)
	{
		/* if this entry used before, we can reuse the ifc_idx, just change the rule para
		 * else we should get a new item for the rule.
		 */
		if (0 > pEntry->ifc_idx)
		{
			ifc_idx = ifc_get_valid_idx();
			pEntry->ifc_idx = ifc_idx;
		}
		else
		{
			ifc_idx = pEntry->ifc_idx;
		}
		ifc_set_rule_para(ifc_idx, DIPv4, pEntry->dst, pEntry->dst_mask);
		ifc_set_rule_para(ifc_idx, SIPv4, pEntry->src, pEntry->src_mask);
		ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_1, 0);
		pEntry->enable = 1;
		g_ipsecRuleNum++;
	}
	else
	{
		/* just need to change the item to disabled status
		 * and record the ifc_idx that we may used next time.
		 */
		ifc_idx = pEntry->ifc_idx;
		if (ifc_idx != -1)
		{
			ifc_set_act_para(ifc_idx, IFC_DISABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_1, 0);
		}
		pEntry->enable = 0;
		if (0 < g_ipsecRuleNum)
		{
			g_ipsecRuleNum--;
		}
	}
	return 0;
}
#endif /* INCLUDE_IPSEC_HWNAT_DISABLE */

static int init_ifc_idx_software_action(void)
{
    memset(ifc_idx_software_action, 0 ,(sizeof(ifc_hit_idx_software_action_t) * HIT_IFC_MAX));
    
    return 0;
}

static int set_ifc_idx_software_action(unsigned int ifc_idx, unsigned int action)
{
    int i=0;

    for(i=0; i<HIT_IFC_MAX; i++)
    {
        if(ifc_idx_software_action[i].using)
        {
            continue;
        }
        ifc_idx_software_action[i].using   = 1;
        ifc_idx_software_action[i].ifc_idx = ifc_idx;
        ifc_idx_software_action[i].action  = action;
        
        return 0;
    }

    return -1;
}

int get_ifc_idx_software_action(unsigned int ifc_idx, unsigned int *action)
{
    int i=0;
    
    for(i=0; i<HIT_IFC_MAX; i++)
    {
        if(0 == ifc_idx_software_action[i].using)
        {
            continue;
        }
        
        if(ifc_idx_software_action[i].ifc_idx == ifc_idx)
        {
            *action  = ifc_idx_software_action[i].action;
            return 0;
        }
    }
    return -1;
}

/*only lut0 for en7516/27/28*/
static int ifc_rule_init_en751627(void)
{
#if defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516) 
	/* IPv4 ->ICMP(0x01) */
    ifc_set_lut0_rules(0, IFC_ENABLE, IPv4_PROTOCOL, 0, 0, 0xFF, 0, 0x01);
    ifc_set_fcpu_ring(0, 1, 2);

    /* IPv6 ->ICMPv6(0x3a) */
    ifc_set_lut0_rules(1, IFC_ENABLE, IPv6_NEXT_HEADER, 0, 0, 0xFF, 0, 0x3a);
    ifc_set_fcpu_ring(1, 1, 2);
    
    /* Ether ->ARP(0x0806) */
    ifc_set_lut0_rules(2, IFC_ENABLE, ETYPE, 0, 0, 0xFFFF, 0, 0x0806);
    ifc_set_fcpu_ring(2, 1, 2);
    
    /* Ether -> DNS(0x35) */
    ifc_set_lut0_rules(3, IFC_ENABLE, DPORT, 0, 0, 0xFFFF, 0, 0x35);
    ifc_set_fcpu_ring(3, 1, 2);
    
    /* Ether -> DHCP/BOOTP(0x43~0x44) */
    ifc_set_lut0_rules(4, IFC_ENABLE, SPORT, 2, 0, 0x44, 0, 0x43);
    ifc_set_fcpu_ring(4, 1, 2);
    
    /* Ether -> 8021-AUTH(0x888E) */
    ifc_set_lut0_rules(5, IFC_ENABLE, ETYPE, 0, 0, 0xFFFF, 0, 0x888E);
    ifc_set_fcpu_ring(5, 1, 2);

	/* IPv4 ->IGMP(0x02) */
    ifc_set_lut0_rules(6, IFC_ENABLE, IPv4_PROTOCOL, 0, 0, 0xFF, 0, 0x02);
    ifc_set_fcpu_ring(6, 1, 2);	

	/*send boradcast to qdma rx ring 8*/
	ifc_set_lut0_rules(15, IFC_ENABLE, DMAC, 0, 0xFFFF, 0xFFFFFFFF, 0xffff, 0xffffffff);
	ifc_set_fcpu_ring(15, 1, 8);	
#endif
	
	return 0;
}

static int ifc_rule_init_en7523(void)
{
	int index = 0,next_index=0;
	int igmp_index = -1;		
	int ftp_sync_index = -1;	
	int arp_index = -1;

	/*IPv4 ->ICMP(0x01),src ip != 192.168.1.1,dst ip != 192.168.1.255,prevent smurf attack*/ 
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SIPv4, 1, 0, 0, 0xc0a80101);	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, DIPv4, 1, 0, 0, 0xc0a801ff);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, IPv4_PROTOCOL, 0, 0xFF, 0, 0x01);
	icmp_ring_index = next_index-1;  

	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SIPv4, 0, 0x18, 0, 0xc0a80101); 	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SIPv4, 1, 0, 0, 0xc0a80101); 	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, DIPv4, 0, 0, 0, 0xc0a80101);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, DPORT, 0, 0xFFFF, 0, 0x50);

	/*icmpv6*/
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, IPv6_NEXT_HEADER, 0, 0xFF, 0, 0x3a);
	icmpv6_ring_index = next_index-1;
	/*igmp*/
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, IPv4_PROTOCOL, 0, 0xFF, 0, 0x02);
	igmp_index = next_index -1;

	/*FTP SNY/FIN From Lan,DIP != 192.168.1.x*/
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SINF, 0, 0, 0, 1);		
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, IPv4_PROTOCOL, 0, 0xFF, 0, 0x06);		
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, DIPv4, 1, 0x18, 0, 0xc0a80101);		
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, DPORT, 0, 0xFFFF, 0, 0x15);	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, UDF0+3, 1, 0x0003, 0, 0);
	ftp_sync_index = next_index-1;
	IfcSetUdfBase(3, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(3, 12);

	/*DHCPv6*/
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SPORT, 0, 0xFFFF, 0, 0x222); 	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, DPORT, 0, 0xFFFF, 0, 0x223);

    /************* Notice: only 16 rule before this line *************/
	/* RTSP Ctrl  (SrcPort:0x22A,  Data[0:4]='RTSP/') */	
	next_index = 16;
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SPORT, 0, 0xFFFF, 0, 0x022A);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, UDF0, 0, 0xFFFF, 0, 0x5254);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, UDF0+1, 0, 0xFFFF, 0, 0x5350);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, UDF0+2, 0, 0xFF00, 0, 0x2F00);
	/* set UDF0 as L4+20, set UDF1 as L4+22, set UDF2 as L4+24 */
	IfcSetUdfBase(0, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(0, 20);
	IfcSetUdfBase(1, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(1, 22);
	IfcSetUdfBase(2, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(2, 24);

	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, ETYPE, 0, 0xFFFF, 0, 0x0806);	
	arp_index = next_index-1;

	for(index=0; index < next_index; index++) 
	{
		if((index == icmpv6_ring_index)||(index == igmp_index)||(index == icmp_ring_index))
			/*send to qdma rx ring2*/
			ifc_set_action(IFC_LUT0_TOTAL_ENTRY+index, 14, IFC_ENABLE, 1, 2, 0, 0);
		else if(index == ftp_sync_index)
			/*send to qdma rx ring3*/
			ifc_set_action(IFC_LUT0_TOTAL_ENTRY+index, 14, IFC_ENABLE, 1, FTP_PASV_PORT_RING, 0, 0);
		else if(index == arp_index)
			/*send to qdma rx ring 7*/
			ifc_set_action(IFC_LUT0_TOTAL_ENTRY+index, 14, IFC_ENABLE, 1, 7, 0, 0);
		else
			/*default send to qdma rx ring1*/
			ifc_set_action(IFC_LUT0_TOTAL_ENTRY+index, 14, IFC_ENABLE, 1, 1, 0, 0);
	}

#if 1    
    /*PPPOE type tcp Pkt length is smaller 46 from pse p2*/
    next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0,  SINF,          0,  0xF,  0, 2);
    /*tcp*/
    next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0,  IPv4_PROTOCOL, 0,  0xFF, 0, 0x06);
    /* 0x8864*/
    next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0,  FRAME_TYPE,    0,  0xF,  0, 0x4);
    /*packet length 0 ~ 46*/
    next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1,  UDF0+4,        2,  46,   0, 0);
    /*set UDF4 as L3+3*/
    IfcSetUdfBase(4,    FE_IFC_UDF_L3_BASE);
    IfcSetUdfOffset(4, 3);
#else
    next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0,  SINF,          0,  0xF,  0, 2);
    next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1,  FRAME_TYPE,    0,  0xF,  0, 0x4);
#endif
    set_ifc_idx_software_action((next_index + IFC_LUT0_TOTAL_ENTRY - 1), PPPOE_HEADER_LENGTH_TRANSLATE);

#if 0
    printk("###########func = %s, line=%d, next_index = %d\n", __func__, __LINE__, (next_index + IFC_LUT0_TOTAL_ENTRY - 1));
#endif    
    	/*lut1 last 2 entries,30&31,keep this ifc rule matched last,send boradcast to qdma rx ring 8*/
	ifc_set_lut1_rules(30, IFC_ENABLE, 1, DMAC, 0, 0, 0xffff, 0xffffffff);	
    	ifc_set_action(IFC_LUT0_TOTAL_ENTRY+31, 14, IFC_ENABLE, 1, 8, 0, 0);
    

	return 0;
}

static int ifc_rule_init_en7580(void)
{
	int index=0, next_index=0;
	int igmp_ring_index = 0;
#if defined(TCSUPPORT_CT_JOYME4)
	int dns_ring = 0;
#endif
	int ftp_sync_index=-1;

	/*IPv4 ->ICMP(0x01),src ip != 192.168.1.1,dst ip != 192.168.1.255,prevent smurf attack*/ 
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SIPv4, 1, 0, 0, 0xc0a80101);	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, DIPv4, 1, 0, 0, 0xc0a801ff);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, IPv4_PROTOCOL, 0, 0xFF, 0, 0x01);
	icmp_ring_index = next_index-1;    

	/*LAN->ONU WEB,src ip = 192.168.1.x && src ip != 192.168.1.1 && dst ip = 192.168.1.1,tcp dport = 80*/
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SIPv4, 0, 0x18, 0, 0xc0a80101); 	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SIPv4, 1, 0, 0, 0xc0a80101); 	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, DIPv4, 0, 0, 0, 0xc0a80101);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, DPORT, 0, 0xFFFF, 0, 0x50);
		
	/* ISAKMP (udpSrcPort:500, udpDstPort:500) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SPORT, 0, 0xFFFF, 0, 0x01F4);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, DPORT, 0, 0xFFFF, 0, 0x01F4);

	#if !defined(TCSUPPORT_CT_JOYME2) 
	/* Ether ->ARP(0x0806) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, ETYPE, 0, 0xFFFF, 0, 0x0806);
	#endif
	/* PPP ->IPCP (0x8021) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, ETYPE, 0, 0xFFFF, 0, 0x8021);

	/* Ether ->PPP(0x8863) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, ETYPE, 0, 0xFFFF, 0, 0x8863);

	/* PPP ->LCP(0xc021) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, ETYPE, 0, 0xFFFF, 0, 0xc021);

	/* UDP Ports: bootp  (0x43) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, SPORT, 0, 0xFFFF, 0, 0x43);

	/* UDP Ports: bootp (0x44) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, SPORT, 0, 0xFFFF, 0, 0x44);


    /************* Notice: only 16 rule before this line *************/
	next_index = 16;
	/* RTSP Ctrl  (SrcPort:0x22A,  Data[0:4]='RTSP/') */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SPORT, 0, 0xFFFF, 0, 0x022A);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, UDF0, 0, 0xFFFF, 0, 0x5254);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, UDF0+1, 0, 0xFFFF, 0, 0x5350);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, UDF0+2, 0, 0xFF00, 0, 0x2F00);
	/* set UDF0 as L4+20, set UDF1 as L4+22, set UDF2 as L4+24 */
	IfcSetUdfBase(0, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(0, 20);
	IfcSetUdfBase(1, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(1, 22);
	IfcSetUdfBase(2, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(2, 24);
        

	/*FTP SNY/FIN From Lan,DIP != 192.168.1.x*/
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SINF, 0, 0, 0, 1);		
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, IPv4_PROTOCOL, 0, 0xFF, 0, 0x06);		
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, DIPv4, 1, 0x18, 0, 0xc0a80101);		
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, DPORT, 0, 0xFFFF, 0, 0x15);	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, UDF0+3, 1, 0x0003, 0, 0);
	ftp_sync_index = next_index-1;
	IfcSetUdfBase(3, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(3, 12);
	

	/* IPv6 ->ICMPv6(0x3a) && NS/NA */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, IPv6_NEXT_HEADER, 0, 0xFF, 0, 0x3a);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, UDF0+3, 2, 0x88FF, 0, 0x8700);
	/* set UDF3 as L4+0 */
	IfcSetUdfBase(4, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(4, 0);

	/* IPv6 ->ICMPv6(0x3a) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, IPv6_NEXT_HEADER, 0, 0xFF, 0, 0x3a);
	icmpv6_ring_index = next_index-1;

	/* TR069,TCP DPorts: 0x1d7b(default) */
	if(!TCSUPPORT_CDS_VAL)
		next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, DPORT, 0, 0xFFFF, 0, 0x1d7b);

	/* IPv4 ->IGMP(0x02) */
	if(!TCSUPPORT_CDS_CT_VAL)/*CDS CHT 7580 set igmp ratelimit in ifc, so no need to set it*/
	{
		next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, IPv4_PROTOCOL, 0, 0xFF, 0, 0x02);
		igmp_ring_index = next_index-1;  
	}

	/* PPP ->CHAP (0xc223) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, ETYPE, 0, 0xFFFF, 0, 0xc223);

	/* VOIP SIP,UDP DPorts: 5060(default) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, DPORT, 0, 0xFFFF, 0, 0x13c4);

	/* VOIP SIP,UDP DPorts: 5061(default) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, DPORT, 0, 0xFFFF, 0, 0x13c5);

	/*FTP TCP SPorts 21*/	
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, SPORT, 0, 0xFFFF, 0, 0x15);

	/* DHCPv6 (udpSrcPort:547, udpDstPort:546) */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SPORT, 0, 0xFFFF, 0, 0x223);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, DPORT, 0, 0xFFFF, 0, 0x222);

#if defined(TCSUPPORT_CT_JOYME4)
	/* DNS,UDP SPorts: 53 */
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, SPORT, 0, 0xFFFF, 0, 0x35);
	dns_ring = next_index-1;	 
#endif

	/* set ICMP & ICMPv6 & IGMP go to rx ring2, other go to rx ring1 */
	/* set special rtsp pasv port pkt go to rx ring3, other go to rx ring1 */
	for(index=0; index < next_index; index++) {
		if((index==icmp_ring_index) || (index==icmpv6_ring_index) || (index==igmp_ring_index))
			ifc_set_action(IFC_LUT0_TOTAL_ENTRY+index, 14, IFC_ENABLE, 1, 2, 0, 0);
		else if(index==ftp_sync_index)
			ifc_set_action(IFC_LUT0_TOTAL_ENTRY+index, 14, IFC_ENABLE, 1, FTP_PASV_PORT_RING, 0, 0);
#if defined(TCSUPPORT_CT_JOYME4) 
		else if ( index == dns_ring )
			ifc_set_action(IFC_LUT0_TOTAL_ENTRY+index, 14, IFC_ENABLE, 1, 3, 0, 0);
#endif
		else
			ifc_set_action(IFC_LUT0_TOTAL_ENTRY+index, 14, IFC_ENABLE, 1, 1, 0, 0);
	}

	/* for 7580 ppe tunnel error packet issue */
	ifc_set_lut1_rules(106, IFC_ENABLE, 0, IPv4_PROTOCOL, 0, 0xFFFF, 0, 0x29);
	ifc_set_lut1_rules(107, IFC_ENABLE, 1, UDF15, 0, 0, 0, 0);
	ifc_set_lut1_rules(108, IFC_ENABLE, 1, IPv4_PROTOCOL, 0, 0xFFFF, 0, 0x29);
	ifc_set_lut1_rules(109, IFC_ENABLE, 0, IPv6_NEXT_HEADER, 0, 0xFFFF, 0, 0x4);
	ifc_set_lut1_rules(110, IFC_ENABLE, 1, UDF15, 0, 0, 0, 0);
	ifc_set_lut1_rules(111, IFC_ENABLE, 1, IPv6_NEXT_HEADER, 0, 0xFFFF, 0, 0x4);
	IfcSetUdfBase(15, FE_IFC_UDF_L4_BASE);
	IfcSetUdfOffset(15, 4);
	ifc_set_action(IFC_LUT0_TOTAL_ENTRY+108, 14, IFC_ENABLE, 1, 1, 0, 0);
	ifc_set_action(IFC_LUT0_TOTAL_ENTRY+111, 14, IFC_ENABLE, 1, 1, 0, 0);

	/*FTP TCP SPort 20,if from PSE P2,do not force to cpu*/
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 0, SINF, 0, 0xFFFF, 0, 2);
	next_index = ifc_set_lut1_rules(next_index, IFC_ENABLE, 1, SPORT, 0, 0xFFFF, 0, 0x14);
	ifc_set_action(IFC_LUT0_TOTAL_ENTRY+next_index-1, 1, IFC_ENABLE, 5, 1,0, 0);
	
	return 0;
}

int ifc_rule_init_en7581(void)
{
	int ifc_idx = 0;

	ifc_clear_all();

	/* IPv4 ->ICMP(0x01),src ip != 192.168.1.1,dst ip != 192.168.1.255, prevent smurf attack */ 
	ifc_idx = ifc_get_valid_idx();
	ifc_set_rule_para(ifc_idx, DIP_EQ_ATK, 0x0, 0x1);
	ifc_set_atk_ip(ATK_IP_KEY0, 0xc0a801ff, 0xffffffff);
	ifc_set_rule_para(ifc_idx, SIP_EQ_ATK, 0x0, 0x2);
	ifc_set_atk_ip(ATK_IP_KEY1, 0xc0a80101, 0xffffffff);
	ifc_set_rule_para(ifc_idx, IPv4_PROTOCOL, 0x1, 0xff);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_7, 0);// ICMP use different ring as SYN/ICMPV6 for attack test

	/* LAN->ONU WEB,src ip = 192.168.1.x && src ip != 192.168.1.1 && dst ip = 192.168.1.1,tcp dport = 80 */
	ifc_idx = ifc_get_valid_idx();
	ifc_set_rule_para(ifc_idx, DIPv4, 0xc0a80101, 0xffffffff);
	ifc_set_rule_para(ifc_idx, SIPv4, 0xc0a80100, 0xffffff00);
	ifc_set_rule_para(ifc_idx, SIP_EQ_ATK, 0x0, 0x2);
	ifc_set_rule_para(ifc_idx, DPORT, 0x50, 0xffff);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_1, 0);

	/* IPv6 ->ICMPv6(0x3a), send to qdma rx ring2 */
	ifc_idx = ifc_get_valid_idx();
	ifc_set_rule_para(ifc_idx, IPv6_NEXT_HEADER, 0x3a, 0xFF);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_2, 0);

	/* IPv4 -> IGMP(0x02), send to qdma rx ring2 */
	ifc_idx = ifc_get_valid_idx();
	ifc_set_rule_para(ifc_idx, IPv4_PROTOCOL, 0x02, 0xFF);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_2, 0);

	/* FTP SNY/FIN From Lan, DIP != 192.168.1.x */
	ifc_idx = ifc_get_valid_idx();
	ifc_set_rule_para(ifc_idx, SINF, 0xf0000, 0xFFFFFFFF);
	ifc_set_rule_para(ifc_idx, IPv4_PROTOCOL, 0x06, 0xFF);
	ifc_set_rule_para(ifc_idx, DPORT, 0x15, 0xFFFF);
	ifc_set_rule_para(ifc_idx, TCP_FLAGS, 0x2, 0x2);
	ifc_set_rule_para(ifc_idx, DIP_EQ_ATK, 0x0, 0x1);
	ifc_set_atk_ip(ATK_IP_KEY0, 0xc0a80100, 0xffffff00);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_3, 0);

	ifc_idx = ifc_get_valid_idx();
	ifc_set_rule_para(ifc_idx, SINF, 0xf0000, 0xFFFFFFFF);
	ifc_set_rule_para(ifc_idx, IPv4_PROTOCOL, 0x06, 0xFF);
	ifc_set_rule_para(ifc_idx, DPORT, 0x15, 0xFFFF);
	ifc_set_rule_para(ifc_idx, TCP_FLAGS, 0x1, 0x1);
	ifc_set_rule_para(ifc_idx, DIP_EQ_ATK, 0x0, 0x1);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_3, 0);

	/* RTSP Ctrl  (SrcPort:0x22A,  Data[0:4]='RTSP/') */
	ifc_idx = ifc_get_valid_idx();
	ifc_set_rule_para(ifc_idx, SPORT, 0x22A, 0xffff);
	ifc_set_rule_para(ifc_idx, RTSP_CTRL, 0x1, 0x1);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_1, 0);

	/* etype -> ARP(0x0806), send to qdma rx ring9 */
	ifc_idx = ifc_get_valid_idx();
	ifc_set_rule_para(ifc_idx, ETYPE, 0x0806, 0xFFFF);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_9, 0);

    /* refer to 7523, boradcast send to qdma rx ring8 */
	ifc_idx = ifc_get_valid_idx();
	ifc_set_mac_para(ifc_idx, DMAC, 0xffffffff, 0xffffffff, 0xffff, 0xffff);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_8, 0);

	/*DHCPv6, send to qdma rx ring7 */
	ifc_idx = ifc_get_valid_idx();
	ifc_set_rule_para(ifc_idx, SPORT, 0x222, 0xffff);
	ifc_set_rule_para(ifc_idx, DPORT, 0x223, 0xffff);
	ifc_set_act_para(ifc_idx, IFC_ENABLE, 1, 1, ACT_ForceCPU, 1, RING_IDX_7, 0);

#ifdef INCLUDE_IPSEC_HWNAT_DISABLE
	/* IPSEC VPN, create proc op for rule settings */
	init_ipsecvpn_ifc_rule();
#endif

	return 0;
}

int ifc_rule_init(void)
{
	clear_all_ifc_actions();
    
    init_ifc_idx_software_action();
    
	if(isEN751627)
		ifc_rule_init_en751627();
	else if(isEN7580)
		ifc_rule_init_en7580();
	else if(SUPPORT_IFC_TCAM)
		ifc_rule_init_en7581();
	else if(isEN7523)
		ifc_rule_init_en7523();
	else
		return -1;

	return 0;
}



