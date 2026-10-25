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

#include "tso.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#if defined(TCSUPPORT_TSO_V2_ENABLE)
#define QDMA_LAN 1
#endif

#if defined(QDMA_LAN)
#define TSO_PATH "tso_lan"
#define TSO_DEBUG_PATH "tso_lan/debug"
#define TSO_RULE_PATH "tso_lan/rule"
#define ECNT_TSO ECNT_TSO_LAN
#else
#define TSO_PATH "tso_wan"
#define TSO_DEBUG_PATH "tso_wan/debug"
#define TSO_RULE_PATH "tso_wan/rule"
#define ECNT_TSO ECNT_TSO_WAN
#endif

/************************************************************************
*                  M A C R O S
*************************************************************************
*/	


/************************************************************************
*                  D A T A	 T Y P E S
*************************************************************************
*/

typedef struct{
	struct list_head list;
	tsoRuleEntry_v4_t ruleEntry;
}tsoRuleListEntry_v4_t;

typedef struct{
	struct list_head list;
	tsoRuleEntry_v6_t ruleEntry;
}tsoRuleListEntry_v6_t;

/* ------------------ ECNT_Hook ------------------ */
typedef int (*tso_api_op_t)(struct ECNT_TSO_Data *tso_data);

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
#if defined(TCSUPPORT_TSO_V2_ENABLE)
extern    PDMA_Private_T *tsoPdmaPriv;
#endif
extern tso_api_op_t tso_operation[];
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
static int tso_read_proc(char *buf, char **start, off_t off, int count,
				 int *eof, void *data);

static int tso_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data);
int tso_match_v4(struct ECNT_TSO_Data *tso_data);
int tso_match_v6(struct ECNT_TSO_Data *tso_data);
static int tsoMatchList_v4(tsoRuleEntry_v4_t* dst_entry);
static int tsoMatchList_v6(tsoRuleEntry_v6_t* dst_entry);
ecnt_ret_val ecnt_tso_hook(struct ecnt_data *indata);
static int tsoLocaloutAddRule(int is_v6, void* entry);
static int tsoLocaloutDelRule(int is_v6, void* entry);
static int tso_rule_read_proc(char *buf, char **start, off_t off, int count,
				 int *eof, void *data);
static int tso_rule_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data);
static int tso_rule_proc_init(void);
static int tso_rule_proc_deinit(void);
static void tsoLocaloutShowRule(void);
static void tsoLocaloutListInit(void);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
#if 0
//debug by shi
inline struct QDMA_DscpInfo_S *qdma_bm_pop_tx_dscp(int ringIdx);
#endif
/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
int tsoDbgRecord = 0;	/* Debug flag. */
int skb_count = 0;	/* how many skb sent */
int tsoShortcutPpe = 0;	/* Shortcut to ppe enable flag. */
uint tsoMssCount[TSO_MAX_MSS_COUNT] = {0};	/* how many skb hit each MSS count. */

int fast_path = 0;
int not_drop = 0;

/* used for tso white list. */
static struct list_head tsoRuleList_v4;
static struct list_head tsoRuleList_v6;
struct list_head tsoSessionList_v4;


struct ecnt_hook_ops ecnt_driver_tso_op = {
	.name = "driver_tso_hook",
	.hookfn = ecnt_tso_hook,
	.maintype = ECNT_TSO,
	.is_execute = 1,
	.subtype = ECNT_DRIVER_API,
	.priority = 1
};


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
	
/******************************************************************************
******************************************************************************/
int tso_proc_init(void) {
	struct proc_dir_entry *tso_proc = NULL ;
	
	/* create proc dir */
	proc_mkdir(TSO_PATH, NULL);
	/* create proc node */
	tso_proc = create_proc_entry(TSO_DEBUG_PATH, 0, NULL);
	if(tso_proc) {
		tso_proc->read_proc = tso_read_proc;
		tso_proc->write_proc = tso_write_proc;
	}

	
#if defined(QDMA_LAN)
/* the rule is work for all skb which through L4 level, do not distinguish lan/wan. */
	tso_rule_proc_init();
#endif

	if(ecnt_register_hook(&ecnt_driver_tso_op)) {
		printk("ecnt_dev_tso_op register fail\n");
		return -ENODEV ;
	}
	
	printk("tso_proc_init.\n");
	
	return 0;
}
	
/******************************************************************************
******************************************************************************/
int tso_proc_deinit(void) { 

	ecnt_unregister_hook(&ecnt_driver_tso_op);	
#if defined(QDMA_LAN)
	tso_rule_proc_deinit(); 
#endif
	
	remove_proc_entry(TSO_DEBUG_PATH, NULL);
	remove_proc_entry(TSO_PATH, NULL);
	
	return 0;
}


/******************************************************************************
******************************************************************************/
static int tso_read_proc(char *buf, char **start, off_t off, int count,
				 int *eof, void *data)
{
	int index = 0;
	off_t begin = 0;
	int free_txdscp = 0;

	printk("tsoDbgRecord:%d\n", tsoDbgRecord);
	printk("tsoShortcut Enable:%d\n", tsoShortcutPpe);
#if defined(TCSUPPORT_TSO_V2_ENABLE)
	free_txdscp = atomic_read(&tsoPdmaPriv->free_txd_num[0]);
	printk("free txdscp = %d,\nrecycleIdx = %d\ndscpInfoAddr = %x\nhead_index = %d\ntail_index = %d\nusing_index = %d\ndgblevel=%d",\
		free_txdscp,tsoPdmaPriv->recycleIdx[0],tsoPdmaPriv->dscpInfoAddr,tsoPdmaPriv->txHeadPtr[0]->dscpIdx,\
		tsoPdmaPriv->txTailPtr[0]->dscpIdx,tsoPdmaPriv->txUsingPtr[0]->dscpIdx,tsoPdmaPriv->dbgLevel);
#endif

	*eof = 1;

	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

/*****************************************************************************
******************************************************************************/
static int tso_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[64];
	
	char cmd[64];
	uint8_t param1[16], param2[16], param3[16], param4[16], param5[16] ;
	uint dbgRecord = 0;
	uint shortcut_enable = 0;
	int i, sum_skb, sum_segs,dbglevel=0, ringIdx, recycleThr;
	
	if (count > sizeof(val_string) - 1){
		printk("command too long\n");
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	sscanf(val_string, "%s %s %s %s %s %s", cmd, param1, param2, param3, param4, param5) ;
	
	if(!strcmp(cmd, "record")) {
		sscanf(val_string, "record %d", &dbgRecord);
		if(dbgRecord <= 3){
			tsoDbgRecord = dbgRecord;
			printk("Debug Level: %d\n", tsoDbgRecord) ;
		}
		else{
			printk("CMD Format: echo record [0,3] > /proc/%s\n", TSO_DEBUG_PATH);
		}
	}
	if(!strcmp(cmd, "shortcut")) {
		sscanf(val_string, "shortcut %d", &shortcut_enable);
		if(shortcut_enable <= 1){
			tsoShortcutPpe = shortcut_enable;
			printk("Shortcut Enable: %d\n", tsoShortcutPpe) ;
		}
		else{
			printk("CMD Format: echo shortcut [0,1] > /proc/%s\n", TSO_DEBUG_PATH);
		}
	}
	else if(!strcmp(cmd, "dump") && !strcmp(param1, "record")) {
		if(TSO_RECORD_DEBUG_ON == tsoDbgRecord){
			printk("(MSS_COUNT, HIT_TIMES):\n");
			sum_skb = 0;
			sum_segs = 0;
			for(i = 0; i < TSO_MAX_MSS_COUNT; i++){
				printk("(%2d, %4d)", i+1, tsoMssCount[i]);
				sum_skb += tsoMssCount[i];
				sum_segs += (tsoMssCount[i]*(i+1));
				if((i + 1) % 8){
					printk(", ");
				}
				else{
					printk("\n");
				}
			}
			printk("\nsum_skb:%d, sum_segs:%d, skb_count:%d\n", sum_skb, sum_segs, skb_count);
		}
		else{
			printk("Please start record first!\n");
		}
	}
#if defined(TCSUPPORT_TSO_V2_ENABLE)
	else if(!strcmp(cmd, "test")) {
		/*
			Debug TSO with these three parameters:
			1. set or clear fast bit in tx msg0;
			2. set or clear no drop bit in tx msg1;
			3. set recyle threshold of txd count.
		*/
		sscanf(val_string, "test %d %d %d", &fast_path, &not_drop, &recycleThr);
		tsoPdmaPriv->recycleThr[0] = recycleThr;
		tsoPdmaPriv->recycleThr[1] = recycleThr;
		printk("Debug fast_path: %d, not_drop: %d, recycleThr:%d\n", fast_path, not_drop, tsoPdmaPriv->recycleThr[0]) ;
	}
	else if(!strcmp(cmd, "level")){
		//dbglevel = atoi(param1);
		sscanf(param1, "%d", &dbglevel);
		if(dbglevel < 0)
			printk("debuglevel : 0~4\n");
		else{
			printk("debuglevel from %d to %d\n", tsoPdmaPriv->dbgLevel, dbglevel);
			tsoPdmaPriv->dbgLevel = dbglevel;
		}
	 }
#endif
	else if(!strcmp(cmd, "clear") && !strcmp(param1, "record")) {
		memset(tsoMssCount, 0, sizeof(tsoMssCount));
		skb_count = 0;
		printk("record cleared.\n");
	}
#if defined(TCSUPPORT_TSO_V2_ENABLE)
	else if(!strcmp(cmd, "recy")){
		//dbglevel = atoi(param1);
		sscanf(param1, "%d", &dbglevel);

		/*set txdscp recycle thredhold*/
		for(ringIdx=0; ringIdx < PDMA_TX_RING_NUM; ringIdx++){
			tsoPdmaPriv->recycleThr[ringIdx] = dbglevel;
		}
		printk("recyle Threshold:%d.\n", dbglevel);

	}
#endif
	else{
		printk("CMD Format: echo record [0,1] > /proc/%s\n", TSO_DEBUG_PATH);
		printk("CMD Format: echo shortcut [0,1] > /proc/%s\n", TSO_DEBUG_PATH);
		printk("CMD Format: echo dump record > /proc/%s\n", TSO_DEBUG_PATH);
		printk("CMD Format: echo clear record > /proc/%s\n", TSO_DEBUG_PATH);
		printk("CMD Format: echo debuglevel [0,4] > /proc/%s\n", TSO_DEBUG_PATH);
	}
	
	return count;
}


/******************************************************************************
************************               RULE  CONTROL BEGIN           *********************
******************************************************************************/
	
/***********************                for proc API.                *******************/
static int tso_rule_proc_init(void) {
/* the rule is work for all skb which through L4 level, do not distinguish lan/wan. */
	struct proc_dir_entry *tso_rule_proc = NULL ;
	
	/* create proc node */
	tso_rule_proc = create_proc_entry(TSO_RULE_PATH, 0, NULL);
	if(tso_rule_proc) {
		tso_rule_proc->read_proc = tso_rule_read_proc;
		tso_rule_proc->write_proc = tso_rule_write_proc;
	}

	tsoLocaloutListInit();
	return 0;
}

/******************************************************************************
******************************************************************************/
static int tso_rule_proc_deinit(void) {	
	remove_proc_entry(TSO_RULE_PATH, NULL);
	return 0;
}

/******************************************************************************
******************************************************************************/
static int tso_rule_read_proc(char *buf, char **start, off_t off, int count,
				 int *eof, void *data)
{
	int index = 0;
	off_t begin = 0;

	printk("tsoDbgRecord:%d\n", tsoDbgRecord);

	*eof = 1;

	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

/*****************************************************************************
******************************************************************************/
static int tso_rule_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[256];
	
	char cmd[64];
	uint8_t param1[16], param2[16], param3[16], param4[64], param5[64] ;
	int param_count;
	tsoRuleEntry_v4_t temp_rule_entry_v4 = {0} ;
	tsoRuleEntry_v6_t temp_rule_entry_v6 = {0} ;
	int port;
	unsigned int ip[4];
	unsigned int *ipv6;
	unsigned int ip6[8];
	
	if (count > sizeof(val_string) - 1){
		printk("command too long\n");
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	
	val_string[count]= '\0';

	param_count = sscanf(val_string, "%s %s %s %s %s %s", cmd, param1, param2, param3, param4, param5) ;
	
	printk("val_string:%s\n", val_string);
	if(!strcmp(cmd, "add")) {
		if(param_count != 6){
			printk("add param count error! count:%d, val_string:%s\n", param_count, val_string);
			return -EINVAL;
		}
		if(!strcmp(param1, "v4")){
			if(!strcmp(param2, "-")){
				temp_rule_entry_v4.dport = 0;
			}
			else{
				if(!sscanf(param2, "%d", &port)){
					printk("dport error! dport:%s\n", param2);
					return -EINVAL;
				}
				else{
					temp_rule_entry_v4.dport = port & 0xffff;
				}
			}
			
			if(!strcmp(param3, "-")){
				temp_rule_entry_v4.sport = 0;
			}
			else{
				if(!sscanf(param3, "%d", &port)){
					printk("sport error! sport:%s\n", param3);
					return -EINVAL;
				}
				else{
					temp_rule_entry_v4.sport = port & 0xffff;
				}
			}
			
			if(!strcmp(param4, "-")){
				temp_rule_entry_v4.daddr_v4 = 0;
			}
			else{
				param_count = sscanf(param4, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
				if(param_count != 4){
					printk("daddr error! daddr:%s\n", param4);
					return -EINVAL;
				}
				temp_rule_entry_v4.daddr_v4 = htonl(((ip[0]&0xff)<<24) | ((ip[1]&0xff)<<16) | ((ip[2]&0xff)<<8) | ((ip[3]&0xff)));
			}
			
			if(!strcmp(param5, "-")){
				temp_rule_entry_v4.saddr_v4 = 0;
			}
			else{
				param_count = sscanf(param5, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
				if(param_count != 4){
					printk("saddr error! saddr:%s\n", param5);
					return -EINVAL;
				}
				temp_rule_entry_v4.saddr_v4 = htonl(((ip[0]&0xff)<<24) | ((ip[1]&0xff)<<16) | ((ip[2]&0xff)<<8) | ((ip[3]&0xff)));
			}
			
			printk("dport:%d, sport:%d, daddr:%x, saddr:%x\n", 
				temp_rule_entry_v4.dport, temp_rule_entry_v4.sport, 
				temp_rule_entry_v4.daddr_v4, temp_rule_entry_v4.saddr_v4);
			
			tsoLocaloutAddRule( 0, &temp_rule_entry_v4);
		}
		else if(!strcmp(param1, "v6")){
			if(!strcmp(param2, "-")){
				temp_rule_entry_v6.dport = 0;
			}
			else{
				if(!sscanf(param2, "%d", &port)){
					printk("dport error! dport:%s\n", param2);
					return -EINVAL;
				}
				else{
					temp_rule_entry_v6.dport = port & 0xffff;
				}
			}
			
			if(!strcmp(param3, "-")){
				temp_rule_entry_v6.sport = 0;
			}
			else{
				if(!sscanf(param3, "%d", &port)){
					printk("sport error! sport:%s\n", param3);
					return -EINVAL;
				}
				else{
					temp_rule_entry_v6.sport = port & 0xffff;
				}
			}
			
			if(!strcmp(param4, "-")){
				memset(&temp_rule_entry_v6.daddr_v6, 0, sizeof(struct in6_addr));
			}
			else{
				ipv6 = (unsigned int*)(&temp_rule_entry_v6.daddr_v6);
				param_count = sscanf(param4, "%x:%x:%x:%x:%x:%x:%x:%x\n", 
					&ip6[0], &ip6[1], &ip6[2], &ip6[3], &ip6[4], &ip6[5], &ip6[6], &ip6[7]);
				ipv6[0] = htonl(((ip6[0]&0xffff)<<16) | (ip6[1]&0xffff));
				ipv6[1] = htonl(((ip6[2]&0xffff)<<16) | (ip6[3]&0xffff));
				ipv6[2] = htonl(((ip6[4]&0xffff)<<16) | (ip6[5]&0xffff));
				ipv6[3] = htonl(((ip6[6]&0xffff)<<16) | (ip6[7]&0xffff));

				if(param_count != 8){
					printk("daddr error! daddr:%s, count:%d\n", param4, param_count);
					return -EINVAL;
				}
			}
			
			if(!strcmp(param5, "-")){
				memset(&temp_rule_entry_v6.saddr_v6, 0, sizeof(struct in6_addr));
			}
			else{
				ipv6 = (unsigned int *)(&temp_rule_entry_v6.saddr_v6);
				param_count = sscanf(param5, "%x:%x:%x:%x:%x:%x:%x:%x\n", 
					&ip6[0], &ip6[1], &ip6[2], &ip6[3], &ip6[4], &ip6[5], &ip6[6], &ip6[7]);
				ipv6[0] = htonl(((ip6[0]&0xffff)<<16) | (ip6[1]&0xffff));
				ipv6[1] = htonl(((ip6[2]&0xffff)<<16) | (ip6[3]&0xffff));
				ipv6[2] = htonl(((ip6[4]&0xffff)<<16) | (ip6[5]&0xffff));
				ipv6[3] = htonl(((ip6[6]&0xffff)<<16) | (ip6[7]&0xffff));
				if(param_count != 8){
					printk("saddr error! saddr:%s\n", param5);
					return -EINVAL;
				}
			}
			tsoLocaloutAddRule( 1, &temp_rule_entry_v6);

		}
	}
	else if(!strcmp(cmd, "del")) {
		if(!strcmp(param1, "v4")){
			if(!strcmp(param2, "-")){
				temp_rule_entry_v4.dport = 0;
			}
			else{
				if(!sscanf(param2, "%d", &port)){
					printk("dport error! dport:%s\n", param2);
					return -EINVAL;
				}
				else{
					temp_rule_entry_v4.dport = port & 0xffff;
				}
			}
			
			if(!strcmp(param3, "-")){
				temp_rule_entry_v4.sport = 0;
			}
			else{
				if(!sscanf(param3, "%d", &port)){
					printk("sport error! sport:%s\n", param3);
					return -EINVAL;
				}
				else{
					temp_rule_entry_v4.sport = port & 0xffff;
				}
			}
			
			if(!strcmp(param4, "-")){
				temp_rule_entry_v4.daddr_v4 = 0;
			}
			else{
				sscanf(param4, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
				temp_rule_entry_v4.daddr_v4 = htonl(((ip[0]&0xff)<<24) | ((ip[1]&0xff)<<16) | ((ip[2]&0xff)<<8) | ((ip[3]&0xff)));
			}
			
			if(!strcmp(param5, "-")){
				temp_rule_entry_v4.saddr_v4 = 0;
			}
			else{
				sscanf(param5, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
				temp_rule_entry_v4.saddr_v4 = htonl(((ip[0]&0xff)<<24) | ((ip[1]&0xff)<<16) | ((ip[2]&0xff)<<8) | ((ip[3]&0xff)));
			}
			
			tsoLocaloutDelRule( 0, &temp_rule_entry_v4);
		}
		else if(!strcmp(param1, "v6")){
			if(!strcmp(param2, "-")){
				temp_rule_entry_v6.dport = 0;
			}
			else{
				if(!sscanf(param2, "%d", &port)){
					printk("dport error! dport:%s\n", param2);
					return -EINVAL;
				}
				else{
					temp_rule_entry_v6.dport = port & 0xffff;
				}
			}
			
			if(!strcmp(param3, "-")){
				temp_rule_entry_v6.sport = 0;
			}
			else{
				if(!sscanf(param3, "%d", &port)){
					printk("sport error! sport:%s\n", param3);
					return -EINVAL;
				}
				else{
					temp_rule_entry_v6.sport = port & 0xffff;
				}
			}
			
			if(!strcmp(param4, "-")){
				memset(&temp_rule_entry_v6.daddr_v6, 0, sizeof(struct in6_addr));
			}
			else{
				ipv6 = (unsigned int*)(&temp_rule_entry_v6.daddr_v6);
				param_count = sscanf(param4, "%x:%x:%x:%x:%x:%x:%x:%x\n", 
					&ip6[0], &ip6[1], &ip6[2], &ip6[3], &ip6[4], &ip6[5], &ip6[6], &ip6[7]);
				ipv6[0] = htonl(((ip6[0]&0xffff)<<16) | (ip6[1]&0xffff));
				ipv6[1] = htonl(((ip6[2]&0xffff)<<16) | (ip6[3]&0xffff));
				ipv6[2] = htonl(((ip6[4]&0xffff)<<16) | (ip6[5]&0xffff));
				ipv6[3] = htonl(((ip6[6]&0xffff)<<16) | (ip6[7]&0xffff));

				if(param_count != 8){
					printk("daddr error! daddr:%s, count:%d\n", param4, param_count);
					return -EINVAL;
				}
			}
			

			if(!strcmp(param5, "-")){
				memset(&temp_rule_entry_v6.saddr_v6, 0, sizeof(struct in6_addr));
			}
			else{
				ipv6 = (unsigned int *)(&temp_rule_entry_v6.saddr_v6);
				param_count = sscanf(param5, "%x:%x:%x:%x:%x:%x:%x:%x\n", 
					&ip6[0], &ip6[1], &ip6[2], &ip6[3], &ip6[4], &ip6[5], &ip6[6], &ip6[7]);
				ipv6[0] = htonl(((ip6[0]&0xffff)<<16) | (ip6[1]&0xffff));
				ipv6[1] = htonl(((ip6[2]&0xffff)<<16) | (ip6[3]&0xffff));
				ipv6[2] = htonl(((ip6[4]&0xffff)<<16) | (ip6[5]&0xffff));
				ipv6[3] = htonl(((ip6[6]&0xffff)<<16) | (ip6[7]&0xffff));
				if(param_count != 8){
					printk("saddr error! saddr:%s\n", param5);
					return -EINVAL;
				}
			}
			tsoLocaloutDelRule( 1, &temp_rule_entry_v6);

		}
	}
	else if(!strcmp(cmd, "clear") && !strcmp(param1, "record")) {
		memset(tsoMssCount, 0, sizeof(tsoMssCount));
		skb_count = 0;
		printk("record cleared.\n");
	}
	else if(!strcmp(cmd, "show") && !strcmp(param1, "rule")) {
		tsoLocaloutShowRule();
	}
	else{
		printk("CMD Format: echo add <v4|v6> <dport|-> <sport|-> <daddr|-> <saddr|-> > /proc/%s\n", TSO_RULE_PATH);
		printk("   Example: echo add v4 - 445 - - > /proc/%s\n", TSO_RULE_PATH);
		printk("CMD Format: echo del <v4|v6> <dport|-> <sport|-> <daddr|-> <saddr|-> > /proc/%s\n", TSO_RULE_PATH);
		printk("CMD Format: echo clear rule > /proc/%s\n", TSO_RULE_PATH);
		printk("CMD Format: echo show rule > /proc/%s\n", TSO_RULE_PATH);
	}
	
	return count;
}



/***********************     for eternal ECNT_HOOK API.    *******************/
ecnt_ret_val ecnt_tso_hook(struct ecnt_data *indata)
{
	struct ECNT_TSO_Data *tso_data = (struct ECNT_TSO_Data *)indata ;
	
	if(tso_data->function_id >= TSO_FUNCTION_MAX_NUM) {
		printk("tso_data->function_id is %d, exceed max number: %d", 
			tso_data->function_id, TSO_FUNCTION_MAX_NUM);
		return ECNT_HOOK_ERROR;
	}
	
	return tso_operation[tso_data->function_id](tso_data) ;
}

int tso_match_v4(struct ECNT_TSO_Data *tso_data)
{
	tsoRuleEntry_v4_t* entry;

	entry = tso_data->tso_private.entry_v4;
	if(entry){
		tso_data->retValue = tsoMatchList_v4(entry);
	}
	return ECNT_CONTINUE;
}

int tso_match_v6(struct ECNT_TSO_Data *tso_data)
{
	tsoRuleEntry_v6_t* entry;

	entry = tso_data->tso_private.entry_v6;
	if(entry){
		tso_data->retValue = tsoMatchList_v6(entry);
	}
	
	return ECNT_CONTINUE;
}


/***********************              internal functions.           *******************/
static void tsoLocaloutListInit(void)
{
	INIT_LIST_HEAD(&tsoRuleList_v4);
	INIT_LIST_HEAD(&tsoRuleList_v6);
	INIT_LIST_HEAD(&tsoSessionList_v4);
}

void* tsoLocaloutListAlloc(int size)
{
	void* ptr = NULL;
	
	if (size>0)
	{
		ptr = kmalloc(size, GFP_ATOMIC);
		if(ptr == NULL) {
			printk("Alloc memory for ppeLocalInlist failed\n") ;
			return NULL ;
		}
		memset(ptr,0,size);
	}
	
	return ptr;
} 

void tsoLocaloutListFree(void* ptr)
{
	if (ptr)
	{
		kfree(ptr);
		ptr = NULL;
	}
}

/* Return 0 means this entry hit the rule in the list.  */
static int tsoMatchRule_v4(tsoRuleEntry_v4_t* dst_entry, tsoRuleEntry_v4_t* list_entry){
	if((list_entry->sport != 0) && (dst_entry->sport != list_entry->sport)){
		return -1;
	}
	else if((list_entry->dport != 0) && (dst_entry->dport != list_entry->dport)){
		return -1;
	}
	else if((list_entry->saddr_v4 != 0) && (dst_entry->saddr_v4 != list_entry->saddr_v4)){
		return -1;
	}
	else if((list_entry->daddr_v4 != 0) && (dst_entry->daddr_v4 != list_entry->daddr_v4)){
		return -1;
	}

	return 0;
}

/* Return 0 means this entry hit the rule in the list.  */
static int tsoMatchRule_v6(tsoRuleEntry_v6_t* dst_entry, tsoRuleEntry_v6_t* list_entry){
	
	__u8	u6_addr8[16] = {0};

	if((list_entry->sport != 0) && (dst_entry->sport != list_entry->sport)){
		return -1;
	}
	else if((list_entry->dport != 0) && (dst_entry->dport != list_entry->dport)){
		return -1;
	}
 	else if(memcmp( &list_entry->saddr_v6, u6_addr8, sizeof(uint32)*4) && memcmp( &list_entry->saddr_v6, &dst_entry->saddr_v6, sizeof(uint32)*4)){
		return -1;
	}
	else if(memcmp( &list_entry->daddr_v6, u6_addr8, sizeof(uint32)*4) && memcmp( &list_entry->daddr_v6, &dst_entry->daddr_v6, sizeof(uint32)*4)){
		return -1;
	}


	return 0;
}

/* Return 0 means this entry hit a rule in the list.  */
int tsoMatchList_v4(tsoRuleEntry_v4_t* dst_entry){
	
	tsoRuleListEntry_v4_t *in_entry_v4 = NULL;
	list_for_each_entry(in_entry_v4, &tsoRuleList_v4, list)	 
	{
		if(!tsoMatchRule_v4(dst_entry, &in_entry_v4->ruleEntry)){
			return 0;
		}
	}
	
	return -1;
}

/* Return 0 means this entry hit a rule in the list.  */
int tsoMatchList_v6(tsoRuleEntry_v6_t* dst_entry){
	
	tsoRuleListEntry_v6_t *in_entry_v6 = NULL;
	list_for_each_entry(in_entry_v6, &tsoRuleList_v6, list)	 
	{
		if(!tsoMatchRule_v6(dst_entry, &in_entry_v6->ruleEntry)){
			return 0;
		}
	}
	
	return -1;
}

/* Return 0 means equal, used for insert or delete a rule.  */
static int tsoEqualRule_v4(tsoRuleEntry_v4_t* dst_entry, tsoRuleEntry_v4_t* src_entry){
	
	if(dst_entry->sport != src_entry->sport){
		return -1;
	}
	else if(dst_entry->dport != src_entry->dport){
		return -1;
	}
	else{
		if(dst_entry->saddr_v4 != src_entry->saddr_v4){
			return -1;
		}
		else if(dst_entry->daddr_v4 != src_entry->daddr_v4){
			return -1;
		}
	}

	return 0;
}

static int tsoEqualRule_v6(tsoRuleEntry_v6_t* dst_entry, tsoRuleEntry_v6_t* src_entry){
	
	if(dst_entry->sport != src_entry->sport){
		return -1;
	}
	else if(dst_entry->dport != src_entry->dport){
		return -1;
	}
	else{
		if(memcmp( &src_entry->saddr_v6, &dst_entry->saddr_v6, sizeof(uint32)*4)){
			return -1;
		}
		else if(memcmp( &src_entry->daddr_v6, &dst_entry->daddr_v6, sizeof(uint32)*4)){
			return -1;
		}
	}

	return 0;
}

static int tsoLocaloutAddRule(int is_v6, void* entry){

	tsoRuleListEntry_v4_t *temp_rule_entry_v4 = NULL;
	tsoRuleListEntry_v6_t *temp_rule_entry_v6 = NULL;
	tsoRuleEntry_v4_t *in_entry_v4 = NULL;
	tsoRuleEntry_v6_t *in_entry_v6 = NULL;

	/*because only config manually,no lock for this list*/
	if(is_v6){
		in_entry_v6 = (tsoRuleEntry_v6_t *)entry;
		list_for_each_entry(temp_rule_entry_v6, &tsoRuleList_v6, list)	 
		{
			if(!tsoEqualRule_v6(&temp_rule_entry_v6->ruleEntry, in_entry_v6)){
				printk("Rule is existed, add fail!\n");
				return -1;
			}
		}

		temp_rule_entry_v6 = (tsoRuleListEntry_v6_t*)tsoLocaloutListAlloc(sizeof(tsoRuleListEntry_v6_t));
		if(temp_rule_entry_v6)
		{
			memcpy(&temp_rule_entry_v6->ruleEntry, in_entry_v6, sizeof(tsoRuleEntry_v6_t));
			list_add_tail(&temp_rule_entry_v6->list, &tsoRuleList_v6);
		}
	}
	else{
		in_entry_v4 = (tsoRuleEntry_v4_t *)entry;
		list_for_each_entry(temp_rule_entry_v4, &tsoRuleList_v4, list)	 
		{
			if(!tsoEqualRule_v4(&temp_rule_entry_v4->ruleEntry, in_entry_v4)){
				printk("Rule is existed, add fail!\n");
				return -1;
			}
		}

		temp_rule_entry_v4 = (tsoRuleListEntry_v4_t*)tsoLocaloutListAlloc(sizeof(tsoRuleListEntry_v4_t));
		if(temp_rule_entry_v4)
		{
			memcpy(&temp_rule_entry_v4->ruleEntry, in_entry_v4, sizeof(tsoRuleEntry_v4_t));
			list_add_tail(&temp_rule_entry_v4->list, &tsoRuleList_v4);
		}
	}

	return 0;

}
	
static int tsoLocaloutDelRule(int is_v6, void* entry){

	tsoRuleListEntry_v4_t *temp_rule_entry_v4 = NULL;
	tsoRuleListEntry_v6_t *temp_rule_entry_v6 = NULL;
	tsoRuleEntry_v4_t *in_entry_v4 = NULL;
	tsoRuleEntry_v6_t *in_entry_v6 = NULL;

	/*because only config manually,no lock for this list*/
	if(is_v6){
		in_entry_v6 = (tsoRuleEntry_v6_t *)entry;
		list_for_each_entry(temp_rule_entry_v6, &tsoRuleList_v6, list)	 
		{
			if(!tsoEqualRule_v6(&temp_rule_entry_v6->ruleEntry, in_entry_v6)){
				list_del(&temp_rule_entry_v6->list);
				tsoLocaloutListFree(temp_rule_entry_v6);
				return 0;
			}
		}
	}
	else{
		in_entry_v4 = (tsoRuleEntry_v4_t *)entry;
		list_for_each_entry(temp_rule_entry_v4, &tsoRuleList_v4, list)	 
		{
			if(!tsoEqualRule_v4(&temp_rule_entry_v4->ruleEntry, in_entry_v4)){
				list_del(&temp_rule_entry_v4->list);
				tsoLocaloutListFree(temp_rule_entry_v4);
				return 0;
			}
		}
	}
	
	printk("Rule is not existed, del fail!\n");
	return -1;
}

static void tsoLocaloutShowRule(void){

	tsoRuleListEntry_v4_t *temp_rule_entry_v4 = NULL;
	tsoRuleListEntry_v6_t *temp_rule_entry_v6 = NULL;
	tsoRuleEntry_v4_t *entry_v4 = NULL;
	tsoRuleEntry_v6_t *entry_v6 = NULL;

	/*because only config manually,no lock for this list*/
	printk("IPv4 rules:\n");
	
	list_for_each_entry(temp_rule_entry_v4, &tsoRuleList_v4, list)	 
	{
		entry_v4 = &temp_rule_entry_v4->ruleEntry;
		printk("sport:%d, dport:%d\nsaddr:%pI4, daddr:%pI4\n", 
			entry_v4->sport, entry_v4->dport, &(entry_v4->saddr_v4), &(entry_v4->daddr_v4));
	}

	printk("IPv6 rules:\n");
	
	list_for_each_entry(temp_rule_entry_v6, &tsoRuleList_v6, list)	 
	{
		entry_v6 = &temp_rule_entry_v6->ruleEntry;
		printk("sport:%d, dport:%d\nsaddr:%pI6c, daddr:%pI6c\n", 
			entry_v6->sport, entry_v6->dport, &(entry_v6->saddr_v6), &(entry_v6->daddr_v6));
	}

}
/******************************************************************************
************************               RULE  CONTROL END              *********************
******************************************************************************/

