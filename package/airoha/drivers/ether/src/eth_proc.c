/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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

#include "./tcphy/switch_proc.h"

#ifdef TCSUPPORT_AUTOBENCH
#include "../../auto_bench/autobench.h"
#endif

    
#include "./tcphy/tcconsole.h"
#include "eth_proc.h"

#ifdef TCSUPPORT_MT7530_SWITCH_API
#include "./tcphy/mtkswitch_api.h"
#include <modules/eth_global_def.h>
#endif

    
#ifdef LOOPBACK_SUPPORT
#include "fe_verify.h"
#endif

#include "./tcphy/phy_api.h"
#include <linux/libcompileoption.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#ifdef TCSUPPORT_CPU_MT7520
#define MAX_TX_QUEUE_TRTCM_NUM  32
#define MAX_TRTCM_PARAM_NUM     16
#define MAX_QOS_CHANNEL_NUM     16
#define MAX_LLID_NUM            8
#define MAX_QUEUE_NUM           8
#define MAX_THLD_POS_NUM        3
#define FE_REG_ADDR_STAR	(0xbfb50000)
#define FE_REG_ADDR_END		(0xbfb51944)
#define FE_REG_LEN			(((FE_REG_ADDR_END - FE_REG_ADDR_STAR) >> 2) + 1)
#endif
#define DOWN 0
#define UP 1


unsigned char sync_pkt[100] = {
  0x01, 0x00, 0x5e, 0x00, 0x01, 0x81, 0x00, 0xaa, 0xbb, 0x01, 0x23, 0x45, 0x08, 0x00, 0x45, 0x00,
  0x00, 0x48, 0x00, 0x00, 0x40, 0x00, 0x01, 0x11, 0xd6, 0x7a, 0xc0, 0xa8, 0x01, 0x01, 0xe0, 0x00,
  0x01, 0x81, 0x01, 0x3f, 0x01, 0x3f, 0x00, 0x34, 0x5f, 0x00, 0x80, 0x02, 0x00, 0x2c, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa,
  0xbb, 0xff, 0xfe, 0x01, 0x23, 0x45, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x30,
  0x8a, 0xca, 0x1f, 0xdd, 0xa0, 0xe4
};

unsigned char follow_pkt[100] = {
  0x01, 0x00, 0x5e, 0x00, 0x01, 0x81, 0x00, 0xaa, 0xbb, 0x01, 0x23, 0x45, 0x08, 0x00, 0x45, 0x00,
  0x00, 0x48, 0x00, 0x00, 0x40, 0x00, 0x01, 0x11, 0xd6, 0x6f, 0xc0, 0xa8, 0x01, 0x0c, 0xe0, 0x00,
  0x01, 0x81, 0x01, 0x40, 0x01, 0x40, 0x00, 0x34, 0xbb, 0x61, 0x88, 0x02, 0x00, 0x2c, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f,
  0x3d, 0xff, 0xfe, 0x81, 0x9f, 0x00, 0x00, 0x01, 0x00, 0x26, 0x02, 0x00, 0x00, 0x00, 0x4f, 0x30,
  0x8a, 0xf5, 0x39, 0x12, 0x23, 0x50
};

unsigned char delay_req_pkt[100] = {
  0x01, 0x00, 0x5e, 0x00, 0x00, 0x6b, 0x00, 0xaa, 0xbb, 0x01, 0x23, 0x45, 0x08, 0x00, 0x45, 0x00,
  0x00, 0x52, 0x00, 0x00, 0x40, 0x00, 0x01, 0x11, 0xd7, 0x86, 0xc0, 0xa8, 0x01, 0x01, 0xe0, 0x00,
  0x00, 0x6b, 0x01, 0x3f, 0x01, 0x3f, 0x00, 0x3e, 0xa4, 0x31, 0x82, 0x02, 0x00, 0x36, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa,
  0xbb, 0xff, 0xfe, 0x01, 0x23, 0x45, 0x00, 0x01, 0x00, 0x16, 0x05, 0x7f, 0x00, 0x00, 0x4f, 0x30,
  0x8a, 0xf8, 0x1f, 0x53, 0x55, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#ifdef TCSUPPORT_FREE_BOOTBASE
#define FLAG_ADDR		0x80001fff;
#else
#define FLAG_ADDR		0x8001ffff;
#endif

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define CHK_BUF() pos = begin + index; if (pos < off) { index = 0; begin = pos; }; if (pos > off + count) goto done;

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern ratelimit_global_mac_t ratelimit_global_mac_p;

extern int mac_receive_num;
extern int mac_receive_threshold;
extern macAdapter_t *mac_p;
extern uint8 macInitialized;

extern uint8 use_ext_switch;
extern uint16 lan_port_tpid[];
extern int wan_port_id;
extern int use_soc_lan;
extern int soc_switch_port;
#ifdef TCSUPPORT_QTN_ISOLATION
extern int isolation_enable;
#endif
extern uint16 ether_wan_tpid;
extern uint16 cpu_port_tpid;
extern char switch_port_map[];    
extern char lan_port_map[];

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
extern eth_port_vlan_t eth_port_vlan[];
extern char extend_switch_port;
extern char inter_switch_lanport_num;
extern char extend_switch_lanport_num;
extern char lan_port_map_ext[];
#endif
extern unsigned char inter_switch_mask;
extern unsigned char extend_switch_mask;

extern struct net_device *lan_dev;
extern int g_port_reverse;

extern int qos_flag;
extern int g_port_reverse_kernel; /**/

extern uint8 macSTagFlag;
extern uint8 swicVendor;
extern int g_stag_to_vtag;
extern char esd_has_reset;
extern int vport_enable;

extern unsigned char mac_sync_en;

extern int g_force_dstq;
extern portInfo lanPortState[LIMIT_PORT_NUM];

extern uint burst_monitor_enable;
extern uint g_cdma1_rxhwf_drop_cnt_old;
extern uint32 dsoffload_presetting;

/* for EN7516 SOC test 20170714 */
extern int g_dump_pkt_cnt;
#ifdef CONFIG_SUPPORT_SELF_TEST
extern int g_eth_overwrite_tx_msg;
extern int g_eth_remove_sp_tag;
#endif

extern int g_wan_mode;

extern unsigned char asym_test_soc_switch;

extern int dyn_qdma1_txq_cngst_en;
extern int dyn_qdma1_txq_cngst_total_min;
extern int dyn_qdma1_txq_cngst_timeout;
extern int dyn_qdma1_txq_cngst_stream_rate;
extern int dyn_qdma1_txq_cngst_other_queue_rate;

extern int expand_64bit_monitor_enable;
/************************************************************************
*                  E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
extern int soft_qdma_eth_tx(struct sk_buff *skb);
extern int (*soft_ratelimit_set_queue_hook) (unsigned int queue,unsigned int rate,void* func);
extern int (*soft_ratelimit_allocatequeue_hook)( unsigned int num);


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
#ifdef TCSUPPORT_QOS
    int qos_flag = NULLQOS;
#endif

int pausetime_for_test = 0;

int eth_fast_mode_flag = 0;
/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
#if defined(QOS_REMARKING) || defined(TCSUPPORT_HW_QOS)
        static int qos_wrr_info[5] = {0};
        static int max_prio = 3;
        static uint8 qos_wrr_user = 0x00;
#endif
static int protect_mode = 0;
extern int macqueue_start_idx;

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

void setQdmaChannelRatelimit(uint8 portId)
{
	uint32 reg;
	int speed;
	int rateValue[4] = {10, 100, 1000, 10000};	/* unit: mbps */

	QDMA_TxRateLimitSet_T txRateLimitSet;

	reg = switch_reg_read(GSW_PMSR(portId));
	lanPortState[portId].linkState = (reg & MAC_LINK_STS);
	if (!(reg & MAC_LINK_STS)) {
		return;
	}
	speed = (reg & MAC_SPD_STS) >> MAC_SPD_STS_SHIFT;

	txRateLimitSet.chnlRateLimitEn = 1;
	txRateLimitSet.chnlIdx = portId;
	txRateLimitSet.rateLimitValue = (rateValue[speed] * 1000 ); 	/* unit: kbps */
	QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_LAN, &txRateLimitSet);

	return;
}

//#ifdef TCSUPPORT_WAN_ETHER

uint32 calcnt(uint32 basereg){
	int i;
	uint32 ret_val = 0;

#ifdef TCSUPPORT_CPU_MT7505
	for(i=0; i<5;i++){
#else
	for(i=0; i<6;i++){
#endif
		if(i == wan_port_id)
			continue;
        if(read_reg_word(GSW_PMSR(i)) == 0xdeadbeef)
            continue;
		ret_val += switch_reg_read(basereg + i*0x100);
	}

	return ret_val;
}


uint32 ext_cal_cnt(uint32 basereg){
    int i;
    uint32 ret_val = 0;
    for(i=0; i<6;i++){
        if(i == wan_port_id)
            continue;
        ret_val += gswPbusRead(basereg + i*0x100);
    }
    return ret_val;
}

static int doAclTblDisp(void)
{
	u32 value, vawd1, vawd2, rule_vawd1, rule_vawd2, rate_vawd1, rate_vawd2;
	int i;
	struct swconfig *swcfg=NULL;
	u8 portMac[6];

	/*no chip check ,should be added here*/

	while (1){	// wait until not busy
		value = switch_reg_read(VTCR);
		if ((value & (1<<31)) == 0){
			break;
		}
	}

	// Display ACL pattern table entry 0~31
	printk("Found following ACL table entries :\n");
	for (i = 0; i < 32; i ++){
		switch_reg_write(VTCR, ACL_PATTERN_READ(i));
		while (1){	// wait until not busy
			value = switch_reg_read(VTCR);
			if ((value & (1<<31)) == 0){
				break;
			}
		}

		vawd1 = switch_reg_read(VAWD1);
		vawd2 = switch_reg_read(VAWD2);

		if (vawd2 & (1 << 19)){	// EN==1
			printk("[#%2d] CMP_SEL:%d, SP_map:%02X, OFST_TP:%d, WORD_OFST:%3d, BIT_MASK:%04X, CMP_PAT:%04X\n",
				i, (vawd2 & 1), ((vawd2>>8) & 0xFF), ((vawd2>>16) & 0x07), ((vawd2>>1) & 0x3F), ((vawd1>>16) & 0xFFFF), (vawd1 & 0xFFFF));
		}
	}

	// Display Mask, Rule Ctrl, and Rate Ctrl table entry 0~31
	printk("Found following Mask, Rule ctrl, Rate ctrl table entries : (in vawd1,vawd2)\n");
	for (i = 0; i < 32; i ++){
		switch_reg_write(VTCR, ACL_MASK_READ(i));
		while (1){	// wait until not busy
			value = switch_reg_read(VTCR);
			if ((value & (1<<31)) == 0){
				break;
			}
		}

		vawd1 = switch_reg_read(VAWD1);
		vawd2 = switch_reg_read(VAWD2);

		if (vawd1 || vawd2){	// ACL_MASK[63:0] any bit != 0
			switch_reg_write(VTCR, ACL_RULE_READ(i));
			while (1){	// wait until not busy
				value = switch_reg_read(VTCR);
				if ((value & (1<<31)) == 0){
					break;
				}
			}
			rule_vawd1 = switch_reg_read(VAWD1);
			rule_vawd2 = switch_reg_read(VAWD2);

			switch_reg_write(VTCR, ACL_RATE_READ(i));
			while (1){	// wait until not busy
				value = switch_reg_read(VTCR);
				if ((value & (1<<31)) == 0){
					break;
				}
			}
			rate_vawd1 = switch_reg_read(VAWD1);
			rate_vawd2 = switch_reg_read(VAWD2);

			printk("[#%2d] Mask:%08X,%08X; Rule:%08X,%08X; Rate:%08X,%08X\n",
					i, vawd1, vawd2, rule_vawd1, rule_vawd2, rate_vawd1, rate_vawd2);
		}
	}

	return 0;
}

/********************************proc****************************************/
int acl_rule_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	printk("PATTERN partition: \n\t 0~4 fixed for broadcast&multicast\n") ;
	if( SPECIAL_RULE_NUM )
		printk("\t %d~%d for special rule\n" , 5 , PATTERN_FOR_SPECIAL_NUM-1 );
	if( DMAC_RULE_NUM )
		printk("\t %d~%d for dmac rule\n" , PATTERN_FOR_SPECIAL_NUM , PATTERN_FOR_SPECIAL_NUM+PATTERN_FOR_DMAC_NUM-1 );
	if( SMAC_RULE_NUM )
		printk("\t %d~%d for smac rule\n" , PATTERN_FOR_SPECIAL_NUM+PATTERN_FOR_DMAC_NUM , PATTERN_FOR_SPECIAL_NUM+PATTERN_FOR_DMAC_NUM+PATTERN_FOR_SAMC_NUM-1 );
	if( VLAN_RULE_NUM )
		printk("\t %d~%d for vlan rule\n" , PATTERN_FOR_SPECIAL_NUM+PATTERN_FOR_DMAC_NUM+PATTERN_FOR_SAMC_NUM , PATTERN_ALL_NUM-1 );

	if( PATTERN_ALL_NUM > ACL_MAX_NUM )
	{
		printk("ERROR: the pattern used is over the limit!\n");
	}
	doAclTblDisp();
	return 0;
}

int acl_rule_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	int i ;
	int len;
	char get_buf[33];
	int func, tmp_func;
	int vlan ;
	int rate ;
	int enable ;
	int index;
	int mac_tmp[6];
	unsigned char mac[6];

	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > 32)
		len = 32;
	else
		len = count;
	/* use the copy_from_user function to copy buffer data to our get_buf */
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	/* zero terminate get_buf */
	get_buf[len]='\0';

	if(sscanf(get_buf, "%d", &func) != 1)
		return count;

	if( (func > 0) && (func < 4) )
	{
		if(sscanf(get_buf, "%d %d %d", &tmp_func , &rate , &enable) != 3)
			return count;
	}
	else if( func < 6)
	{
		if(sscanf(get_buf, "%d %x-%x-%x-%x-%x-%x %d %d %d", &tmp_func , &mac_tmp[0], &mac_tmp[1], &mac_tmp[2], &mac_tmp[3], &mac_tmp[4], &mac_tmp[5], 
																			&rate , &enable , &index) != 10)
			return count;
		for( i = 0 ; i < 6 ; i++ )
		{
			mac[i] = mac_tmp[i]&0xff;
		}
		printk("set mac = %02X-%02X-%02X-%02X-%02X-%02X\n",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}
	else if( func == 6)
	{
		if(sscanf(get_buf, "%d %d %d %d %d", &tmp_func , &vlan ,&rate , &enable, &index) != 5)
			return count;
	}
	else
	{
		printk("error!\n");
		return count;
	}

	if( func < 4 )/*set 1 no-unicast acl rule, 2 broarcast rule, 3 multicast rule*/
		acl_set_upstream_ratelimit_for_special(rate ,enable , (func-1));
	else if( func == 4 )/*4 set dmac*/
		acl_set_upstream_ratelimit_for_mac(mac , rate , enable, index , ACL_SET_MODE_DMAC);
	else if( func == 5 )/*5 set smac*/
		acl_set_upstream_ratelimit_for_mac(mac , rate , enable, index , ACL_SET_MODE_SMAC);
	else if( func == 6 )/*6 set vlan*/
		acl_set_upstream_ratelimit_for_vlan(vlan , rate ,enable, index);

	return count;
}

int special_tpid_read_proc(char *buf, char **start, off_t off, 
    int count, int *eof, void *data)
{
	int i = 0,index = 0;
	off_t pos=0, begin=0 ;
	
	index += sprintf(buf+index, "Switch Port TPID:\nPort   TPID\n");
	for (i = 0; i < MT7530_SWIC_PORTNUM; i++) 
	{
		if(use_ext_switch == 1)
			index += sprintf(buf+index, "%d:    0x%04x\n", i,
			    ((gswPbusRead(0x2010 + (i * 0x100)) & 0xFFFF0000) >> 16));
		else
			index += sprintf(buf+index, "%d:    0x%04x\n", i,
			    ((read_reg_word(GSW_BASE + 0x2010 + 
			        (i * 0x100)) & 0xFFFF0000) >> 16));
		CHK_BUF();
	}
	index += sprintf(buf+index, "Lan Port TPID:\nPort   TPID\n");
	for (i = 0; i < MAX_ETH_ITF_NUM; i++) 
	{
		index += sprintf(buf+index, "%d:    0x%x\n", i, lan_port_tpid[i]);
		CHK_BUF();
	}
	if (wan_port_id >= 0 && wan_port_id<MT7530_SWIC_PORTNUM-1)
		index += sprintf(buf+index, "EtherWanPort: 0x%04x\n", ether_wan_tpid);

	index += sprintf(buf+index, "Cpu Port: 0x%04x\n", cpu_port_tpid);

	*eof = 1;
done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0)
		index = 0;
	if (index>count)
		index = count;
	return index;
}

int special_tpid_write_proc(struct file *file, const char *buffer,
    unsigned long count, void *data)
{
	char val_string[32];
	uint16 addr = 0,tpid = 0;
	uint32 value = 0;
	int i = 0;
	char tmp = 0;
	int port, port_tpid;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	if ((2 != sscanf(val_string, "%d:%x", &port, &port_tpid)) ||
		(port < 0) || (port > MT7530_SWIC_PORTNUM-1)){
		printk("Invalid param, must be like: portId[0~%d]:Tpid\n", 
		    MT7530_SWIC_PORTNUM-1);
		return -1;
	}
	tpid = port_tpid;

	/* update lan port tpid from reg & lan_port_map */
	if ((switch_port_map[port] >=0) && (switch_port_map[port]<MT7530_SWIC_PORTNUM))
		lan_port_tpid[switch_port_map[port]] = tpid;

	if (port == wan_port_id)
		ether_wan_tpid = tpid;
	else if (port == 6)
		cpu_port_tpid = tpid;

	addr = port * 0x100 + 0x2010;

	if(use_ext_switch == 1)
	{
		value = gswPbusRead(addr);
		value &= 0x0000FFFF;
		value |= (tpid << 16);
		gswPbusWrite(addr,value);
	}
	else
	{
		value = read_reg_word(GSW_BASE + addr);
		value &= 0x0000FFFF;
		value |= (tpid << 16);
		write_reg_word(GSW_BASE + addr,value);	
	}
	return count;
}

int gpon_bootflag_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	char *boot_flag = FLAG_ADDR;

	if (!macInitialized) {
		*eof = 1;
		return 0;
	}

	index += sprintf(buf+ index, "%d", *boot_flag) ;
	CHK_BUF() ;

	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0)		index = 0 ;
	if(index>count)		index = count ;
	return index ;
}

static int gpon_commitflag_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	char *commit_flag = COMMIT_FLAG_ADDR;

	if (!macInitialized) {
		*eof = 1;
		return 0;
	}

	index += sprintf(buf+ index, "%d", *commit_flag) ;
	CHK_BUF() ;

	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0)		index = 0 ;
	if(index>count)		index = count ;
	return index ;
}



unsigned int udp_checksum(const struct sk_buff *skb)
{
	unsigned int checksum = 0;
	unsigned int src_ip1, src_ip2, dest_ip1, dest_ip2;
	unsigned int src_port, dest_port;
	unsigned int udp_length;
	unsigned int udp_protocol;
	unsigned int data;
	unsigned int index;

	udp_protocol = (unsigned int)(*((unsigned char*)(&skb->data[23])));
	src_ip1 = (unsigned int)(*((unsigned short*)(&skb->data[26])));
	src_ip2 = (unsigned int)(*((unsigned short*)(&skb->data[28])));
	dest_ip1 = (unsigned int)(*((unsigned short*)(&skb->data[30])));
	dest_ip2 = (unsigned int)(*((unsigned short*)(&skb->data[32])));
	src_port = (unsigned int)(*((unsigned short*)(&skb->data[34])));
	dest_port = (unsigned int)(*((unsigned short*)(&skb->data[36])));
	udp_length = (unsigned int)(*((unsigned short*)(&skb->data[38])));

//	printk("src ip1: %x, src ip2: %x\n", src_ip1, src_ip2);
//	printk("dest ip1: %x, dest ip2: %x\n", dest_ip1, dest_ip2);
//	printk("src port: %x, dest port: %x\n", src_port, dest_port);
//	printk("udp_length: %x\n", udp_length);
//	printk("udp_protocol: %x\n", udp_protocol);
	data = 0;
	for (index=42; index<skb->len; index+=2){
		data += (unsigned int)(*((unsigned short*)(&skb->data[index])));
	}
//	printk("data: %x\n", data);

	checksum = src_ip1 + src_ip2 + dest_ip1 + dest_ip2 + src_port + 
	    dest_port + (2*udp_length) + data + udp_protocol;
//	printk("checksum1: %x\n", checksum);
	checksum += (checksum >> 16);
//	printk("checksum2: %x\n", checksum);
	checksum &= 0xffff;
//	printk("checksum3: %x\n", checksum);
	checksum = (~checksum);
//	printk("checksum4: %x\n", checksum);
	checksum &= 0xffff;
//	printk("checksum5: %x\n", checksum);

	return checksum;
}

int MT7530_gen_1588_packet(char flag)
{
//	int k;
	struct sk_buff *skb;
    ethTxMsg_t ethTxMsg;
    int ringIdx;

	int tx_len;
	int tx_priority;
	unsigned long checksum;
//	unsigned long sec, nsec;
	uint8 *tx_data;
	static short id=0;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,10,0)
	struct timespec cur_time;

	getnstimeofday(&cur_time);
#else
	struct timespec64 cur_time;
	ktime_get_real_ts64(&cur_time);
#endif

	skb = dev_alloc_skb(RX_BUF_LEN);
	if (skb == NULL){
		printk("cannot allocate skb\n");
		return 0;
	}

	tx_len = 100;
	tx_data = skb_put(skb, tx_len);

	if (flag == 0)
		memcpy(tx_data, sync_pkt, 100);
	else if (flag == 1)
		memcpy(tx_data, follow_pkt, 100);
	else if (flag == 2)
		memcpy(tx_data, delay_req_pkt, 100);
	else {
		printk("error: 1588 packet type input\n");
        dev_kfree_skb(skb);
		return 0;
	}

	tx_priority = 3;
	skb->priority = tx_priority;

	skb->data[76] = (unsigned char)(0x0);
	skb->data[77] = (unsigned char)(0x0);
	skb->data[78] = (unsigned char)((cur_time.tv_sec>>24) & 0xff);
	skb->data[79] = (unsigned char)((cur_time.tv_sec>>16) & 0xff);
	skb->data[80] = (unsigned char)((cur_time.tv_sec>>8) & 0xff);
	skb->data[81] = (unsigned char)((cur_time.tv_sec>>0) & 0xff);

	skb->data[82] = (unsigned char)((cur_time.tv_nsec>>24) & 0xff);
	skb->data[83] = (unsigned char)((cur_time.tv_nsec>>16) & 0xff);
	skb->data[84] = (unsigned char)((cur_time.tv_nsec>>8) & 0xff);
	skb->data[85] = (unsigned char)((cur_time.tv_nsec>>0) & 0xff);

	skb->data[72] = (unsigned char)((id >> 8) & 0xff);
	skb->data[73] = (unsigned char)(id & 0xff);
	id++;

	checksum = udp_checksum(skb);
	skb->data[40] = (checksum >> 8);
	skb->data[41] = (checksum & 0xff);

	ethTxMsg.msg[0]=0;
    ethTxMsg.msg[1]=0;
    ethTxMsg.raw.channel = 0;
    ethTxMsg.raw.queue = 0;
    ringIdx = 0;
	eth_lan_mac_tx(skb,lan_dev);

	mdelay(100);

	return 0;
}


int gen_1588_pkt_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[32];
	int flag;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	flag = simple_strtoul(val_string, NULL, 16);
#ifndef MT7505_PQA_SWITCH_TEST
	MT7530_gen_1588_packet((char)(flag));
#endif

	return count;
}


int getETHLinkSt(char *buf)
{
	uint16 index = 0;

	if (!macInitialized) {
		index += sprintf(buf+index, "Down\n");
		return index;
	}

	if ((mac_p->macPhyLinkProfile_p->enetMode & LAN_ST_LINK_UP) == 0) {
		index += sprintf(buf+index, "Down\n");
		return index;
	}

	if (mac_p->macPhyLinkProfile_p->enetMode & LAN_ST_1000MB)
		index += sprintf(buf+index, "1000M/");
	else if (mac_p->macPhyLinkProfile_p->enetMode & LAN_ST_100MB)
		index += sprintf(buf+index, "100M/");
	else 
		index += sprintf(buf+index, "10M/");
	if (mac_p->macPhyLinkProfile_p->enetMode & LAN_ST_FULL_DUPLEX)
		index += sprintf(buf+index, "Full Duplex\n");
	else 
		index += sprintf(buf+index, "Half Duplex\n");
	return index;
}


int eth_link_st_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int len = getETHLinkSt(buf);
	if (len <= off+count) 
		*eof = 1;
	*start = buf + off;
	len -= off;
	if (len>count) 
		len = count;
	if (len<0) 
		len = 0;
	return len;
}


#if defined (QOS_REMARKING) || defined (TCSUPPORT_HW_QOS)

static uint8 get_qos_weight(uint8 weight)
{
	if (weight <= 1)
		return MAX_WEIGHT_1023;
	else if ((weight >= 2) && (weight <= 3))
		return MAX_WEIGHT_2047;
	else if ((weight >= 4) && (weight <= 6))
		return MAX_WEIGHT_4095;
	else if (weight >= 7)
		return MAX_WEIGHT_8191;
}

static int eth_qoswrr_read_proc(char *page, char **start, off_t off,
		int count, int *eof, void *data){

	printk("%d %d %d %d %d\n", *qos_wrr_info, *(qos_wrr_info + 1), 
	    *(qos_wrr_info + 2), *(qos_wrr_info + 3), *(qos_wrr_info + 4));
	return 0;
}

static int eth_qoswrr_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	int len;
	char get_buf[33];
	int max_wrr_val = 0, i;
    u8 portid = 0;
	QDMA_TxQosScheduler_T txQos;

    mt7530_switch_qosparam param;
	memset(&param, 0, sizeof(mt7530_switch_qosparam));

	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > 32)
		len = 32;
	else
		len = count;
	/* use the copy_from_user function to copy buffer data to our get_buf */
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	/* zero terminate get_buf */
	get_buf[len]='\0';

	if(sscanf(get_buf, "%d %d %d %d %d", qos_wrr_info, (qos_wrr_info+1), 
	    (qos_wrr_info+2), (qos_wrr_info+3), (qos_wrr_info+4)) != 5)
		return count;
	
	/* find max qos wrr weight */
	for (i = 0; i < 4; i++) {
		if (max_wrr_val < qos_wrr_info[i + 1]) {
			max_wrr_val = qos_wrr_info[i + 1];
			max_prio = 3 - i;
		}
	}
	qos_wrr_user = QOS_DMAWRR_USERDEFINE;

#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
	if(g_wan_mode == USE_QDMA_WAN_ETHER)
	{
		/* Strict Priority */
		if (0 == qos_wrr_info[0])
		{
			//set QDMA as Strict Priority Mode,
			memset(&txQos, 0, sizeof(QDMA_TxQosScheduler_T));
			txQos.channel = wan_port_id;
			txQos.qosType = QDMA_TXQOS_TYPE_SP;
			if (QDMA_API_SET_TX_QOS(ECNT_QDMA_WAN, &txQos)){
				printk
					("\nFAILED(%s): qdma strict priority setting for path%d\n",
					 __FUNCTION__, wan_port_id);
				return -1;
			}

			printk("\nSP setting for txq0~3 is done\n\n");
		}
		else
		{
			// set QDMA's SP & WRR priority registers,
			memset(&txQos, 0, sizeof(QDMA_TxQosScheduler_T));
			txQos.channel = wan_port_id;
			
			if(1 == qos_wrr_info[0]) 
			{ /*WRR*/
				txQos.qosType = QDMA_TXQOS_TYPE_SPWRR4;
				for (i = 0; i < 4; i++)
					txQos.queue[i].weight = qos_wrr_info[4-i];
			}
			else if (2 == qos_wrr_info[0])
			{ /*SPWRR*/
				txQos.qosType = QDMA_TXQOS_TYPE_SPWRR4 + 4 - qos_wrr_info[1];
				for (i = 0; i < qos_wrr_info[1]; i++)
					txQos.queue[i].weight = qos_wrr_info[qos_wrr_info[1]+1-i];
			}
	                if (QDMA_API_SET_TX_QOS(ECNT_QDMA_WAN, &txQos)){
				printk
				    ("\nFAILED(%s): qdma wrr setting for path%d\n",
				     __FUNCTION__, wan_port_id);
				return -1;
	    	}
	    }
	}
	else
#endif
	{	
	    param.qosmode = qos_wrr_info[0];
	    if(1 == qos_wrr_info[0]) { /*WRR*/
	        for (i = 0; i < 4; i++) {
	            param.wi[i] = qos_wrr_info[i+1];
	        }
	    }
	    else if (2 == qos_wrr_info[0]){ /*SPWRR*/
	        param.wrr_queue_cnt = qos_wrr_info[1];
	        for (i = 0; i < param.wrr_queue_cnt; i++) {
	            param.wi[i] = qos_wrr_info[i+2];
	        }
	    }

	    //cmd only support ether_wan port
	    portid = wan_port_id;
	    macMT7530Setqosparam(portid, param);
	}

	return len;
}
#endif



#ifdef TCSUPPORT_QOS
static int eth_tcqos_read_proc(char *page, char **start, off_t off,
		int count, int *eof, void *data){

	if (qos_flag == QOS_SW_PQ) {
		printk("qos discipline is PQ.\n");
	}
	else if (qos_flag == QOS_SW_WRR) {
		printk("qos discipline is WRR.\n");
	}
	else if (qos_flag == QOS_SW_CAR) {
		printk("qos discipline is CAR.\n");
	}
	else if (qos_flag == QOS_HW_WRR) {
		printk("qos discipline is HW WRR.\n");
	}
	else if (qos_flag == QOS_HW_PQ) {
		printk("qos discipline is HW PQ.\n");
	}
	else {
		printk("qos is disabled.\n");
	}
	return 0;
}

static int eth_tcqos_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){

	char qos_disc[11];
	int len;

	if (count > 10) {
		len = 10;
	}
	else {
		len = count;
	}
	memset(qos_disc, 0, sizeof(qos_disc));
	
	if(copy_from_user(qos_disc, buffer, len))
		return -EFAULT;

	qos_disc[len] = '\0';

	if (!strncmp(qos_disc, "PQ", 2)) {
		qos_flag = QOS_SW_PQ;
	}
	else if (!strncmp(qos_disc, "WRR", 3)) {
		qos_flag = QOS_SW_WRR;
	}
	else if (!strncmp(qos_disc, "CAR", 3)) {
		qos_flag = QOS_SW_CAR;
	}
	else if (!strncmp(qos_disc, "HWWRR", 5)) {
		qos_flag = QOS_HW_WRR;
	}
	else if (!strncmp(qos_disc, "HWPQ", 4)) {
		qos_flag = QOS_HW_PQ;
	}
	else {
		qos_flag = NULLQOS;
	}

	return len;
}
#endif



int eth_rx_receive_num_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
        off_t pos = 0;
        off_t begin = 0;
	
	index += sprintf(buf+index, "mac_receive_num %d mac_receive_threshold %d\n", 
	    mac_receive_num, mac_receive_threshold);
        CHK_BUF();

	*eof = 1;

done:
        *start = buf + (off - begin);
        index -= (off - begin);
        if (index<0)
                index = 0;
        if (index>count)
                index = count;
        return index;
}

int eth_rx_receive_num_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data){
	
	char tmp[64];
	int len;

	if (count >= sizeof(tmp)) {
		len = sizeof(tmp) - 1;
	}
	else {
		len = count;
	}

	if(copy_from_user(tmp, buffer, len))
                return -EFAULT;

        /* zero terminate get_buf */
        tmp[len]='\0';

        if ((2 == sscanf(tmp, "%d %d", &mac_receive_num, &mac_receive_threshold)) ){
                printk("mac_receive_num %d mac_receive_threshold %d\n", 
                    mac_receive_num, mac_receive_threshold );
        }else{
                printk("Invalid input %s\n", tmp);

	}
	return count;

}

int eth_port_stat_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	uint32 regValue=0;
	int i =0;
	int port_num;
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;
	uint8 portstatus[MAX_ETH_ITF_NUM]={0};


    if(macSTagFlag){
	#ifdef TCSUPPORT_2PORTS
		port_num = 2;
	#else
	    port_num = 4;
	#endif
	}else{
	    port_num = 1;
	}
	if (isMT7520S){
		port_num = 1; /*only one giga port*/
	}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
		port_num = MAX_ETH_ITF_NUM; /* read all eth status */
		/* need to be extend for 8 port */
#endif

	if(swicVendor!=0){
		for (i=0; i<port_num; i++){
			if ((lan_port_map[i] != -1)
#ifdef CONFIG_TP_IMAGE
			 && (lan_port_map[i] != 255)
#endif
			)
			{
				regValue = switch_reg_read(GSW_PMSR(lan_port_map[i]));
				#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
				if(lan_port_map[i] == extend_switch_port)
					regValue = gswRegRead_extend(GSW_PMSR(lan_port_map_ext[i]), 1);
				#endif
            } else {
                regValue =0;
            }

        #if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CUC_C5_2P)
			if(i == 3) portstatus[0] = (regValue & MAC_LINK_STS);
			else if(i == 0) portstatus[1] = (regValue & MAC_LINK_STS);
			else if(i == 1) portstatus[2] = (regValue & MAC_LINK_STS);
			else if(i == 2) portstatus[3] = (regValue & MAC_LINK_STS);
        #else/*TCSUPPORT_COMPILE*/
    		if (g_port_reverse)
    			portstatus[port_num - i - 1] = (regValue & MAC_LINK_STS);
    		else
    			portstatus[i] = (regValue & MAC_LINK_STS);
        #endif/*TCSUPPORT_COMPILE*/		
		}	
	}
	
	for (i=0; i<port_num; i++){
		index += sprintf(buf+index, "%d ", portstatus[i]);
	}
	index += sprintf(buf+index, "\n");
	CHK_BUF();

	*eof = 1;

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}


/* support port reverse */
int port_reverse_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;

	index += sprintf(buf+index, "%d\n", g_port_reverse);
	CHK_BUF();

	*eof = 1;

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

int port_reverse_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	int len;
	char get_buf[33];

	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > 32)
		len = 32;
	else
		len = count;
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	
	get_buf[len]='\0';

	/*printk("get_buf is aa%s\n", get_buf);*/

	g_port_reverse = atoi(get_buf);


#if defined(TCSUPPORT_HWNAT)
 	 port_reverse = g_port_reverse;
#endif

	g_port_reverse_kernel = g_port_reverse;

	/* debug */
	/*printk("g_port_reverse is %d\n", g_port_reverse);*/
	
	return len;
}

/* support stag to vtag */
int stag_to_vtag_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;

	index += sprintf(buf+index, "%d\n", g_stag_to_vtag);
	CHK_BUF();

	*eof = 1;

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

int stag_to_vtag_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	int len;
	char get_buf[32];

	/* do a range checking, don't overflow buffers in kernel modules */
	if(count >= 32)
		len = 31;
	else
		len = count;
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	
	get_buf[len]='\0';

	/*printk("get_buf is aa%s\n", get_buf);*/

	g_stag_to_vtag = atoi(get_buf);

	/* debug */
	/*printk("g_port_reverse is %d\n", g_port_reverse);*/
	
	return len;
}



int ether_7516bt_init(int enable)
{
	int i = 0;
	uint32 reg = 0, value = 0;
	
	if(enable ==1 )
	{
		/*disable unused phy*/
		for(i = 0; i < 5; i++)
		{
			if(switch_port_map[i] == -1){
				tcMiiStationWrite(8+i, 0, 0x800);
			}
		}
		/*set gdm1/gdm2 config*/
		value = read_reg_word(GDMA1_BASE);
		value &= 0xFFFF0000;
		value |= 0x2222;
		write_reg_word(GDMA1_BASE, value);

		value = read_reg_word(GDMA2_BASE);
		value &= 0xFFFF0000;
		value |= 0x1111;
		write_reg_word(GDMA2_BASE, value);

		/*enable port 5/6 fc */
		reg = 0x3600 ;
		value = read_reg_word(GSW_BASE+reg);
		value |= (3<<4);
		write_reg_word(GSW_BASE+reg, value);	

		reg = 0x3500 ;
		value = read_reg_word(GSW_BASE+reg);
		value |= (3<<4);
		write_reg_word(GSW_BASE+reg, value);

		/*disable port5 6 sptag*/
		reg = 0x2610 ;
		value = read_reg_word(GSW_BASE+reg);
		write_reg_word(GSW_BASE+reg, 0x81008100);

		reg = 0x2510 ;
		value = read_reg_word(GSW_BASE+reg);
		write_reg_word(GSW_BASE+reg, 0x81008100);

		/*set mc/bc/uc frames flooding ports*/
		reg = 0x10 ;
		value = read_reg_word(GSW_BASE+reg);
		write_reg_word(GSW_BASE+reg, 0xFFFFFF00);

		/* disable switch port BC ratelimit*/
		for(i=0;i<7; i++)
		{
			reg = 0x201c + (i<<8);
			write_reg_word(GSW_BASE+reg, 0);
		}

		printk("init switch for 7516BT \n");
	}else if(enable ==2){
		
		/*enable port 5/6 fc */
		reg = 0x3600 ;
		value = read_reg_word(GSW_BASE+reg);
		value |= (3<<4);
		write_reg_word(GSW_BASE+reg, value);	

		reg = 0x3500 ;
		value = read_reg_word(GSW_BASE+reg);
		value |= (3<<4);
		write_reg_word(GSW_BASE+reg, value);

		/*disable port5  sptag*/
		reg = 0x2510 ;
		value = read_reg_word(GSW_BASE+reg);
		write_reg_word(GSW_BASE+reg, 0x81008100);

		/* disable switch port BC ratelimit*/
		for(i=0;i<7; i++)
		{
			reg = 0x201c + (i<<8);
			write_reg_word(GSW_BASE+reg, 0);
		}
		

		printk("init switch for 7527 to test\n");
	}
	
	return 0;

}
int ether_7516bt_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	int len;
	char get_buf[32];
	int enable;

	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > 31)
		len = 31;
	else
		len = count;
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	
	get_buf[len]='\0';

	/*printk("get_buf is aa%s\n", get_buf);*/

	enable = atoi(get_buf);
	ether_7516bt_init(enable);
	
	return len;
}


#if defined(CONFIG_MIPS_RT63365)
#if defined(TCPHY_SUPPORT)

int gsw_check_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;

  	index += sprintf(buf+index, "%d\n", esd_has_reset);
	CHK_BUF();

	*eof = 1;
	/*printk("esd_read_value %d\n",esd_has_reset);*/

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}


int gsw_check_write_proc(struct file *file, const char *buffer,
					 	unsigned long count, void *data)
{
	char val_string[32];
	int val;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	val = simple_strtoul(val_string, NULL, 10);	
	/*printk("esd_write_value %d\n",val);*/
	if(val == 1)
		esd_has_reset = 0;
	else{
		esd_has_reset = 1;
	}	

	return count;
}


#endif
#endif


#if 1/*def VPORT*/
int vport_enable_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;

  	index += sprintf(buf+index, "%d\n", vport_enable);
	CHK_BUF();

	*eof = 1;
	/*printk("vport_enable_value %d\n",esd_has_reset);*/

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

int vport_enable_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[4];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	vport_enable = simple_strtoul(val_string, NULL, 10);	
	/*printk("vport_enable_value %d\n", vport_enable);*/

	return count;
}
#endif

int ethernet_portmap_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;
	uint8 port;

	index += sprintf(buf+index, "%d\n", wan_port_id);
	CHK_BUF();

	index += sprintf(buf+index, "use_soc_lanport:\n%d\n", soc_switch_port);
	CHK_BUF();

	index += sprintf(buf+index, "lan_port_map\n");
	CHK_BUF();
	for (port=0; port<MAX_ETH_ITF_NUM; port++){
        #if defined(TCSUPPORT_MULTI_SWITCH_EXT)
        index += sprintf(buf+index, "%d %d %d\n", port, lan_port_map[port], lan_port_map_ext[port]);
        #else
		index += sprintf(buf+index, "%d %d \n", port, lan_port_map[port]);
        #endif
		CHK_BUF();
	}

    index += sprintf(buf+index, "switch_port_map\n");
	CHK_BUF();
	for (port=0; port<(RT63365_SWIC_PORTNUM-1); port++){
		index += sprintf(buf+index, "%d %d\n", port, switch_port_map[port]);
		CHK_BUF();
	}
	
	*eof = 1;

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

int ethernet_portmap_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
    int len;
    char get_buf[32];
    int lan[MAX_ETH_ITF_NUM];
    int wan_port;
    u8 port = 0;
    u8 value = 0;
    char port_in_extsw = 0;

    /* do a range checking, don't overflow buffers in kernel modules */
    if(count > 31)
        len = 31;
    else
        len = count;
    /* use the copy_from_user function to copy buffer data to our get_buf */
    if(copy_from_user(get_buf, buffer, len))
        return -EFAULT;
    /* zero terminate get_buf */
    get_buf[len]='\0';

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
	if(sscanf(get_buf, "%d %d %d %d %d %d %d %d %d %d %d %d", &wan_port, 
        &lan[0], &lan[1], &lan[2], &lan[3], &lan[4], &lan[5],
        &lan[6], &lan[7], &lan[8], &lan[9], &lan[10]) != 12){
        printk("echo wan_port lan1_port lan2_port lan3_port lan4_port lan5_port lan6_port lan7_port lan8_port lan9_port lan10_port lan11_port > /proc/tc3162/eth_portmap\n");
        return count;
    }
#else
    if(sscanf(get_buf, "%d %d %d %d %d %d %d", &wan_port, 
        &lan[0], &lan[1], &lan[2], &lan[3], &lan[4], &lan[5]) != 7){
        printk("echo wan_port lan1_port lan2_port lan3_port lan4_port lan5_port lan6_port > /proc/tc3162/eth_portmap\n");
        return count;
    }
#endif
    if((wan_port < 0 && wan_port != -1) || wan_port >= RT63365_SWIC_PORTNUM-1){
        printk("Error: Config ether wan port: %d\n", wan_port);
        printk("       Ethernet wan port cannot find mapped switch port\n");
        printk("       The mapped switch port should be between 0 and 5\n");
        printk("       If you do not want to use etherwan, the number should be -1\n");
        return count;
    }
    for (port=0; port<MAX_ETH_ITF_NUM; port++){
        if((lan[port] < 0 && lan[port] != -1) || lan[port] >= RT63365_SWIC_PORTNUM-1){
            printk("Error: Config ether lan port: %d\n", lan[port]);
            printk("       Ethernet lan port cannot find mapped switch port\n");
            printk("       The mapped switch port should be between 0 and 5\n");
            printk("       If you do not want to use this lan port, the number should be -1\n");
            return count;
        }
    }
#ifdef CONFIG_TP_IMAGE
    /* 在驱动内部判断如果从ewan切换为lan，则disable原本ewan口对应的switch ratectl   ，避免应用层不配置qos
    如果从lan切换到ewan，认为目前应用层会自行处理 */
    if(wan_port == -1 && wan_port_id >= 0)
    {
        macMT7530SetPortRatelimitingEnable(wan_port_id, 0);
    }
#endif /* CONFIG_TP_IMAGE */
#if defined(TCSUPPORT_WAN_ETHER)
    wan_port_id = wan_port;
    /* add wan_port_id to GDM2 wan_port */
    if(SUPPORT_SET_DOUBLE_WAN_PORT){
        FE_API_SET_WAN_PORT_7516(FE_ENABLE, FE_WANPORT_GSW_P0+wan_port_id, FE_WANPORT_PSE_P2);
    }
#if defined(TCSUPPORT_QDMA_WAN_FOR_ETHER)
    if(wan_port_id != -1)
        ether_wan_use_qdma_wan_set(wan_port_id, USE_QDMA_WAN_ETHER);
    else
        ether_wan_use_qdma_wan_set(wan_port_id, USE_QDMA_WAN_PON_XDSL);
#endif
#endif
    for (port=0; port<MAX_ETH_ITF_NUM; port++){
        lan_port_map[port] = (char)lan[port];
#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
        if(lan_port_map[port] == extend_switch_port){
            lan_port_map_ext[port] = port_in_extsw;
            port_in_extsw ++; 
        }else{
            lan_port_map_ext[port] = -1;
        }
#endif  
    }

    memset(switch_port_map, -1, RT63365_SWIC_PORTNUM*sizeof(char));
    for (port=0; port<MAX_ETH_ITF_NUM; port++){
        value = lan_port_map[ port ];
        if (value <RT63365_SWIC_PORTNUM){
            switch_port_map[ value ] = port;
        }
    }

    return len;
}

#ifdef TCSUPPORT_QTN_ISOLATION
int isolation_enable_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;

  	index += sprintf(buf+index, "%d\n", isolation_enable);
	CHK_BUF();

	*eof = 1;

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

int isolation_enable_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[4];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	isolation_enable = simple_strtoul(val_string, NULL, 10);

	return count;
}
#endif

static int protect_mode_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+ index, "protect_mode:%d\n", protect_mode) ;
	CHK_BUF() ;
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0)		index = 0 ;
	if(index>count)		index = count ;
	return index ;
}

int eth_sync_mactable_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;

  	index += sprintf(buf+index, "%d\n", mac_sync_en);
	CHK_BUF();

	*eof = 1;
	/*printk("vport_enable_value %d\n",esd_has_reset);*/

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

int eth_sync_mactable_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[4];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	mac_sync_en = simple_strtoul(val_string, NULL, 10);	

	return count;
}

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
int eth_port_vlan_status_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    int idx = 0;
    printk("inter_switch_lanport_num : %d extend_switch_lanport_num : %d inter_switch_mask %02x extend_switch_mask %02x \n", 
        inter_switch_lanport_num, extend_switch_lanport_num, inter_switch_mask, extend_switch_mask);
    printk("eth_idx\t\tstatus\tvlan\n");
    for(idx = 0 ; idx < MAX_ETH_ITF_NUM; idx ++)
    {   
        if(idx < 9)
            printk("eth_0.%d\t\t%d\t%d\t \n",idx + 1, eth_port_vlan[idx].eth_port_state, eth_port_vlan[idx].vlan_id);
        else
            printk("eth_0.%d\t%d\t%d\t \n",idx + 1, eth_port_vlan[idx].eth_port_state, eth_port_vlan[idx].vlan_id);
    }
    printk("status state : (0:lan port in inter switch) (1:lan port in exter switch)  (-1: lan port reserved)\n");
    printk("vlan : (-1:untag)    (1~4095:vlan id)\n");
	return 0;
}

int eth_port_vlan_status_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char get_buf[128];
	int  len = 0; 
    int  vlan_id[MAX_ETH_ITF_NUM];
    int idx = 0;
	
	if (count > sizeof(get_buf))
		len = sizeof(get_buf) - 1;
	else 
		len = count;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;

	get_buf[len] = '\0';

    if(sscanf(get_buf, "%d %d %d %d %d %d %d %d %d %d %d ",&vlan_id[0], &vlan_id[1], &vlan_id[2], &vlan_id[3],
        &vlan_id[4],&vlan_id[5],&vlan_id[6],&vlan_id[7],&vlan_id[8],&vlan_id[9],&vlan_id[10]) != MAX_ETH_ITF_NUM){
        printk("echo lan1_vlan lan2_vlan lan3_vlan lan4_vlan lan5_vlan lan6_vlan lan7_vlan lan8_vlan lan9_vlan lan10_vlan lan11_vlan > /proc/tc3162/eth_port_vlan\n");
        return count;
    }
    inter_switch_lanport_num = 0;
    extend_switch_lanport_num = 0;
    inter_switch_mask = 0;
    extend_switch_mask = 0;
    for(idx = 0; idx < MAX_ETH_ITF_NUM; idx ++){
        eth_port_vlan[idx].vlan_id = vlan_id[idx];
        if(lan_port_map[idx] == -1)
            eth_port_vlan[idx].eth_port_state = -1;
        else if(lan_port_map[idx] == extend_switch_port){
            eth_port_vlan[idx].eth_port_state = 1;
            extend_switch_lanport_num ++;
        }else{
            eth_port_vlan[idx].eth_port_state = 0;
            inter_switch_lanport_num ++;
        }
    }
    for(idx = 0; idx < inter_switch_lanport_num; idx ++ ){
        inter_switch_mask |= (1<<idx);
    }
    for(idx = inter_switch_lanport_num; idx < (inter_switch_lanport_num + extend_switch_lanport_num); idx ++ ){
        extend_switch_mask |= (1<<idx);
    }
    
    return len;
}

#endif

int eth_force_destq_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	printk("%d\n", g_force_dstq);
	return 0;
}

int eth_force_destq_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	int tmp_val;
	char val_string[3];	
	if (count > sizeof(val_string) - 1){
		printk("Input Value [0/1/2]\n");
		return -EINVAL;
	}
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;	
	val_string[count] = '\0';	
	sscanf(val_string, "%d\n", &tmp_val);	
	if( (tmp_val<0) || (tmp_val>2)){
		printk("Input Value [0/1/2]\n");
		return -EFAULT;
	}
	else{
		eth_dstq_write(tmp_val);
		printk("g_force_dstq:%d \n", g_force_dstq);
	}
	return count;
}

static int total_Bandwidth_read_proc(char *buf, char **start, off_t off,
		int count, int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;
		
	if (isMT7520G || isMT7525G){
		index += sprintf(buf+index, "%llu,%llu\n", 
			(gswPbusRead(EXT_GSW_TX_OCL(6))+((unsigned long long)gswPbusRead(EXT_GSW_TX_OCH(6))<<32)),
			(gswPbusRead(EXT_GSW_RX_OCL(6))+((unsigned long long)gswPbusRead(EXT_GSW_RX_OCH(6))<<32)));
	}else{
		index += sprintf(buf+index, "%llu,%llu\n", 
			(switch_reg_read(GSW_TX_OCL(6))+((unsigned long long)switch_reg_read(GSW_TX_OCH(6))<<32)),
			(switch_reg_read(GSW_RX_OCL(6))+((unsigned long long)switch_reg_read(GSW_RX_OCH(6))<<32)));
	}
	
	if (index <= off + count)
		*eof = 1;
	
	*start = buf + off;
	index -= off;
	if (index > count)
		index = count;
	if (index < 0)
		index = 0;
	return index;
}

int ether_wan_state_clear_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[4];
	uint8 portId;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	portId = simple_strtoul(val_string, NULL, 10);	
	if(portId>=LIMIT_PORT_NUM)
		return -EFAULT;

	setQdmaChannelRatelimit(portId);

	return count;
}

static int hwnat_ds_offload_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    printk("Soft Qdma Flag: 0x%x , burstMonitor :%u\n", dsoffload_presetting, burst_monitor_enable);
    printk("Soft Qdma Flag: bit 0:  multicast\n"); 
    printk("                bit 1:  ftp packet\n");
    printk("                bit 2:  http download src port 80\n");
    printk("                bit 3:  http download src port 8080\n");
    printk("                bit 4:  all tcp\n");
    printk("                bit 5:  all udp\n");
    printk("                bit 6:  down stream mac ratelimit\n");
    printk("                bit 15: user defined tcp port, which is set in proc softqdma_udf_tcpport\n");
    printk("                bit 16: RTSP\n");
    return 0;
}

static int hwnat_ds_offload_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char get_buf[64];
    int len = 0;
    uint32 hwnat_ds_offload = 0, reg, regval;
    uint32 dsswitch = 0;

    if (count >= 64)
    {
        len = 63;
    }
    else 
    {
        len = count;
    }
    
    memset(get_buf, 0, sizeof(get_buf));
    if (copy_from_user(get_buf, buffer, len))
    {
        return -EFAULT;
    }
    get_buf[len] = '\0';

    sscanf(get_buf, "0x%x %u", &hwnat_ds_offload, &dsswitch);
    if (dsoffload_presetting != hwnat_ds_offload)
    {
        dsoffload_presetting = hwnat_ds_offload;

        /* clear all FOE entries, the entries will be re-learned afterwards */
#if defined(TCSUPPORT_RA_HWNAT_ENHANCE_HOOK)
        if(ra_sw_nat_hook_clean_multicast_entry)
        {
            ra_sw_nat_hook_clean_multicast_entry();
        }
#endif
    }

    if ((burst_monitor_enable != dsswitch) && !burst_monitor_enable)
    {
        g_cdma1_rxhwf_drop_cnt_old = read_reg_word(CDMA1_RXHWF_DROP_CNT);
    }
    burst_monitor_enable = dsswitch;  

    return len;
}

/* for EN7516 SOC test 20170714 */
static int eth_dump_rx_pkt_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    printk("g_dump_pkt_cnt: %d\n", g_dump_pkt_cnt);
    return 0;
}

static int eth_dump_rx_pkt_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char get_buf[64];
    int len = 0;

    if (count >= 64)
    {
        len = 63;
    }
    else 
    {
        len = count;
    }
    
    memset(get_buf, 0, sizeof(get_buf));
    if (copy_from_user(get_buf, buffer, len))
    {
        return -EFAULT;
    }
    get_buf[len] = '\0';

    sscanf(get_buf, "%d", &g_dump_pkt_cnt);
    return len;
}

#ifdef CONFIG_SUPPORT_SELF_TEST
static int eth_self_test_setting_read_proc(char *page, char **start, off_t off,
    int count, int *eof, void *data)
{
    printk("g_eth_overwrite_tx_msg: %d\n", g_eth_overwrite_tx_msg);
    printk("g_eth_remove_sp_tag:    %d\n", g_eth_remove_sp_tag);
    return 0;
}

static int eth_self_test_setting_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char get_buf[64];
    int len = 0;

    if (count >= 64)
    {
        len = 63;
    }
    else 
    {
        len = count;
    }
    
    memset(get_buf, 0, sizeof(get_buf));
    if (copy_from_user(get_buf, buffer, len))
    {
        return -EFAULT;
    }
    get_buf[len] = '\0';

    sscanf(get_buf, "%d %d", &g_eth_overwrite_tx_msg, &g_eth_remove_sp_tag);
    return len;
}

static int eth_padding_test_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char get_buf[8];
    int len = 0;
    struct sk_buff *skb = NULL;
    unsigned char tmp[22];
    struct net_device* dev = NULL;
    int flag = 0;
    int to_gsw = 0;
    static int skb_length = 0;

    if (count > 8)
        len = 8;
    else
        len = count;

    memset(get_buf, 0, sizeof(get_buf));

    if (copy_from_user(get_buf, buffer, len - 1))
        return -EFAULT;

    get_buf[len] = '\0';

    sscanf(get_buf, "%d %d %d", &skb_length, &flag, &to_gsw);

    //printk("skb_length = %d\n", skb_length);

    skb = skbmgr_dev_alloc_skb2k();
    if (!skb)
    {
        printk("Failed to alloc skb!\n");
        return -EFAULT;
    }

    memset(tmp, 0 , sizeof(tmp));
    if (0 == flag)
    /* dst mac 00:00:00:00:00:07, src mac 00:00:00:00:00:08, ethertype 0800 */
    {
        printk("untag packet\n");
        tmp[5] = 7;
        tmp[11] = 8;
        tmp[12] = 0x88;
        tmp[13] = 0xb5;
    }
    else if (1 == flag)
    {
        printk("1 vlan 8100 packet\n");
        tmp[5] = 7;
        tmp[11] = 8;
        tmp[12] = 0x81;
        tmp[15] = 0x01;
        tmp[16] = 0x88;
        tmp[17] = 0xb5;       
    }
    else if (2 == flag)
    {
        printk("1 vlan 88a8 packet\n");
        tmp[5] = 7;
        tmp[11] = 8;
        tmp[12] = 0x88;
        tmp[13] = 0xa8;
        tmp[15] = 0x02;
        tmp[16] = 0x88;
        tmp[17] = 0xb5;  
    }
    else if (3 == flag)
    {
        printk("1 vlan 9100 packet\n");
        tmp[5] = 7;
        tmp[11] = 8;
        tmp[12] = 0x91;
        tmp[15] = 0x03;
        tmp[16] = 0x88;
        tmp[17] = 0xb5;  
    }
    else if (4 == flag)
    {
        printk("2 vlan 8100-8100 packet\n");
        tmp[5] = 7;
        tmp[11] = 8;
        tmp[12] = 0x81;
        tmp[15] = 0x04;
        tmp[16] = 0x81;
        tmp[19] = 0x05;      
        tmp[20] = 0x88;
        tmp[21] = 0xb5;     
    }
    else if (5 == flag)
    {
        printk("2 vlan 88a8-8100 packet\n");
        tmp[5] = 7;
        tmp[11] = 8;
        tmp[12] = 0x88;
        tmp[13] = 0xa8;
        tmp[15] = 0x06;
        tmp[16] = 0x81;
        tmp[19] = 0x07;      
        tmp[20] = 0x88;
        tmp[21] = 0xb5;    
    }
    else if (6 == flag)
    {
        printk("2 vlan 9100-8100 packet\n");
        tmp[5] = 7;
        tmp[11] = 8;
        tmp[12] = 0x91;
        tmp[15] = 0x08;
        tmp[16] = 0x81;
        tmp[19] = 0x09;      
        tmp[20] = 0x88;
        tmp[21] = 0xb5;     
    }
    else if (7 == flag)
    {
        printk("other1\n");
        tmp[5] = 7;
        tmp[11] = 8;
        tmp[12] = 0x80;
        tmp[13] = 0x01;
        tmp[16] = 0x88;
        tmp[17] = 0xb5;  
    }
    else if (8 == flag)
    {
        printk("other2\n");
        tmp[5] = 7;
        tmp[11] = 8;
        tmp[12] = 0x02;
        tmp[13] = 0x01;
        tmp[15] = 0x64;
        tmp[16] = 0x81;
        tmp[19] = 0x65;      
        tmp[20] = 0x88;
        tmp[21] = 0xb5;   
    }

    skb_put(skb, skb_length);
    memset(skb->data, 0, skb_length);

    memmove(skb->data, tmp, 22);
    skb->data[skb_length-1] = 0xff;

    if (!to_gsw)
    {
        ra_sw_nat_hook_set_magic(skb, FOE_MAGIC_PPE);
    }
    else
    {
        ra_sw_nat_hook_set_magic(skb, FOE_MAGIC_GE);
    }

    //printk("tx packet:\n");
    //dump_skb(skb);

    dev = dev_get_by_name(&init_net, "eth0.1");
    if (dev)
    {
        skb->dev = dev;

        #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
        skb->dev->netdev_ops->ndo_start_xmit(skb, skb->dev);
        #else
        skb->dev->hard_start_xmit(skb, skb->dev);
        #endif
    }
    else
    {
        printk("dev is NULL\n");
    }

    return len;
}
#endif

static unsigned char ratelimit_atoh(char a)
{
	if (a >= '0' && a <= '9')
		return (a-'0');
	if (a >= 'a' && a <= 'f')
		return (a - 'a') + 10;
	if (a >= 'A' && a <= 'F')
		return (a - 'A') + 10;
	return 0;
}

static int skb_ratelimit_tx(struct sk_buff*skb)
{
	if(skb==NULL || skb->dev==NULL)
		return 0;
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
	skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
#else
	skb->dev->hard_start_xmit(skb, skb->dev);
#endif
	return 0;
}

static int mac_read_proc(
		char *page, char **start, off_t off,
		int count, int *eof, void *data)
{
	int i = 0;
	ratelimit_global_mac_t *ratelimit_global_mac = &ratelimit_global_mac_p;
	
	printk("1:UP      0:DOWN\n");
    for(i=0; i<MAC_NUM; i++)
	{
		if(ratelimit_global_mac->mac_q[i].active)
			printk("i:%d mac  %02x:%02x:%02x:%02x:%02x:%02x   ratelimit:%d up/down:%d\n",i,
					ratelimit_global_mac->mac_q[i].mac[0],ratelimit_global_mac->mac_q[i].mac[1],
					ratelimit_global_mac->mac_q[i].mac[2],ratelimit_global_mac->mac_q[i].mac[3],
					ratelimit_global_mac->mac_q[i].mac[4],ratelimit_global_mac->mac_q[i].mac[5],
					ratelimit_global_mac->mac_q[i].recordBandWidth,
					ratelimit_global_mac->mac_q[i].updown);
    }

	return 0;
}

static int mac_write_proc(
		struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[32];
	unsigned char mac_temp[12];
	unsigned char dir_tmp[8];
	unsigned char action[8];
	unsigned int ratelimit = 0;


	ratelimit_global_mac_t *ratelimit_global_mac = &ratelimit_global_mac_p;
	
	int i,j;
	if (count > sizeof(get_buf) - 1)
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;	
	sscanf(get_buf, "%s %s %s %d",action,dir_tmp,mac_temp, &ratelimit);

	if(-1 == macqueue_start_idx && soft_ratelimit_allocatequeue_hook)
		macqueue_start_idx = soft_ratelimit_allocatequeue_hook(16);
	if(-1 == macqueue_start_idx )
		goto help;

	if(!strcmp(action, "clear"))
	{
			memset(ratelimit_global_mac,0,sizeof(ratelimit_global_mac_t));	
			return count;
	}
	if(!strcmp(action, "add"))
	{
		dsoffload_presetting |= 0x40;
		for(i=0;i<MAC_NUM;i++)
		{
			if(ratelimit_global_mac->mac_q[i].active)
			{
				if(!strcmp(dir_tmp, "up"))
					if(	ratelimit_global_mac->mac_q[i].updown!=UP)
						continue;

				if(!strcmp(dir_tmp, "down"))
					if(ratelimit_global_mac->mac_q[i].updown!=DOWN)
						continue;
				for(j=0;j<6;j++)
				{
					if(ratelimit_global_mac->mac_q[i].mac[j] != ((ratelimit_atoh(mac_temp[2*j]) << 4 )  |  ratelimit_atoh(mac_temp[2*j+1])))
						break;
				}
				if(j==6)
				{
					ratelimit_global_mac->mac_q[i].recordBandWidth= ratelimit;
					if(soft_ratelimit_set_queue_hook)
					{
						if(!strcmp(dir_tmp, "down"))
							soft_ratelimit_set_queue_hook(macqueue_start_idx+i,ratelimit,soft_qdma_eth_tx);
						if(!strcmp(dir_tmp, "up"))
							soft_ratelimit_set_queue_hook(macqueue_start_idx+i,ratelimit,skb_ratelimit_tx);
					}
					return count;
				}
			}
		}
		for(i=0;i<MAC_NUM;i++)
		{
			if(!ratelimit_global_mac->mac_q[i].active)
			{
				for(j=0;j<6;j++)
				{
					ratelimit_global_mac->mac_q[i].mac[j] = (ratelimit_atoh(mac_temp[2*j]) << 4 )  |  ratelimit_atoh(mac_temp[2*j+1]);
				}
				if(!strcmp(dir_tmp, "up"))
					ratelimit_global_mac->mac_q[i].updown=UP;
				if(!strcmp(dir_tmp, "down"))
					ratelimit_global_mac->mac_q[i].updown=DOWN;
				ratelimit_global_mac->mac_q[i].active = 1;
				ratelimit_global_mac->mac_q[i].recordBandWidth= ratelimit;
				if(soft_ratelimit_set_queue_hook)
				{
					if(!strcmp(dir_tmp, "down"))
						soft_ratelimit_set_queue_hook(macqueue_start_idx+i,ratelimit,soft_qdma_eth_tx);
					if(!strcmp(dir_tmp, "up"))
						soft_ratelimit_set_queue_hook(macqueue_start_idx+i,ratelimit,skb_ratelimit_tx);
				}
				break;
			}
		}
		if(MAC_NUM == i)
			printk("mac ratelimit queue has full\n");
	}
	
	return count;
	help:
	printk("echo add/clear up/down mac ratelimit(kbps) > /proc/tc3162/mac_ratelimit\n");
	printk("Example:echo add down 00aabbc13194 > /proc/tc3162/wlan_ssid_ratelimit\n");
	return count;
	
}

int eth_pausetime_for_test_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;

  	index += sprintf(buf+index, "%d\n", pausetime_for_test);
	CHK_BUF();

	*eof = 1;

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

int eth_pausetime_for_test_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[4];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	pausetime_for_test = simple_strtoul(val_string, NULL, 10);

	return count;
}

int eth_asym_test_soc_switch_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	int index = 0;
	off_t pos = 0;
	off_t begin = 0;

  	index += sprintf(buf+index, "%d\n", asym_test_soc_switch);
	CHK_BUF();

	*eof = 1;
	/*printk("vport_enable_value %d\n",esd_has_reset);*/

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}

int eth_asym_test_soc_switch_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[6];

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	asym_test_soc_switch = simple_strtoul(val_string, NULL, 10);	

	return count;
}

extern int eth_gQueueMask;

static int eth_mac_default_queuemask_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data
)
{
	printk("\n ether qos gQueueMask is 0x%x\n",eth_gQueueMask);		
	return 0;
}

static int eth_mac_default_queuemask_write_proc(
		struct file *file, const char *buffer,
		unsigned long count, void *data
)
{
	char valString[8];
	int val = 0;
	
	if (count > sizeof(valString) - 1)
		return -EINVAL;

	if (copy_from_user(valString, buffer, count))
		return -EINVAL;

	valString[count] = '\0';
	sscanf(valString, "%d", &val);
	if (val)
	{
		eth_gQueueMask = val;
		printk("\n ether qos gQueueMask is 0x%x\n",eth_gQueueMask);
	}

	return count;
}
static int dyn_qdma_txq_cngst_threshold_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
    printk("dyn_qdma1_txq_cngst_en is %d. /* 0:fixed parameter, 1:configured paramete */\n", dyn_qdma1_txq_cngst_en);
    printk("dyn_qdma1_txq_cngst_stream_rate is %dMbps.\n", dyn_qdma1_txq_cngst_stream_rate);
    printk("dyn_qdma1_txq_cngst_total_min is 0x%x.\n", dyn_qdma1_txq_cngst_total_min);
    printk("dyn_qdma1_txq_cngst_timeout is %ds.\n", dyn_qdma1_txq_cngst_timeout);
    printk("dyn_qdma1_txq_cngst_other_queue_rate is %d pps.\n", dyn_qdma1_txq_cngst_other_queue_rate);
    return 0;
}

static int dyn_qdma_txq_cngst_threshold_write_proc(struct file *file, const char *buffer,
        unsigned long count, void *data)
{
    char val_string[64], cmd[32];
    uint value=0;
    QDMA_TxQDynCngstTotalThrh_T total_threshold;
    
    if (count > sizeof(val_string) - 1)
        return -EINVAL ;

    if (copy_from_user(val_string, buffer, count))
        return -EFAULT ;
    
    sscanf(val_string, "%s %d", cmd, &value) ;
    
    if(!strncmp(cmd, "enable", 6)) {
        dyn_qdma1_txq_cngst_en = value;
        printk("dyn_qdma_txq_cngst_en is %d.\n", dyn_qdma1_txq_cngst_en);
    } else if(!strncmp(cmd, "stream_rate", 11)) {
        dyn_qdma1_txq_cngst_stream_rate = value;
        printk("dyn_qdma_txq_cngst_stream_rate is %dmbps.\n", dyn_qdma1_txq_cngst_stream_rate);
    } else if(!strncmp(cmd, "total_min", 9)) {
        dyn_qdma1_txq_cngst_total_min = value;
        printk("dyn_qdma_txq_cngst_total_min is 0x%x.\n", dyn_qdma1_txq_cngst_total_min);
    } else if(!strncmp(cmd, "timeout", 7)) {
        dyn_qdma1_txq_cngst_timeout = value;
        printk("dyn_qdma_txq_cngst_timeout is %ds.\n", dyn_qdma1_txq_cngst_timeout);
    } else if(!strncmp(cmd, "other_queue_rate", 16)) {
        dyn_qdma1_txq_cngst_other_queue_rate = value;
        printk("dyn_qdma_txq_cngst_other_queue_rate is %dpps.\n", dyn_qdma1_txq_cngst_other_queue_rate);
    } else {
        printk("echo enable [0~1] > /proc/tc3162/dyn_qdma_txq_cngst_threshold_write_proc \n");
        printk("echo stream_rate [eg:200 (unit is mbps)] > /proc/tc3162/dyn_qdma_txq_cngst_threshold_write_proc \n");
        printk("echo total_min [eg:14336] > /proc/tc3162/dyn_qdma_txq_cngst_threshold_write_proc \n");
        printk("echo timeout [eg:300 (unit is second)] > /proc/tc3162/dyn_qdma_txq_cngst_threshold_write_proc \n");
        printk("echo other_queue_rate [eg:500 (unit is kbps)] > /proc/tc3162/dyn_qdma_txq_cngst_threshold_write_proc \n");
    }
    
    return count;
}

int expand_64bit_monitor_read_proc(char *buf, char **start, off_t off, int count,
                 int *eof, void *data)
{
	printk("expand_64bit_monitor_enable = %d\n", expand_64bit_monitor_enable);
	return 0;
}

int expand_64bit_monitor_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[64];
    uint value=0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';
	sscanf(val_string, "%d", &value) ;
	expand_64bit_monitor_enable = value;
	printk("expand_64bit_monitor_enable = %d\n", expand_64bit_monitor_enable);

	return count;
}

/*****************************************************************************
******************************************************************************/

static int eth_fast_slow_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data){
	if (eth_fast_mode_flag) {
		printk("eth_fast_mode_flag value is %d, eth down stream fast mode enable\n", eth_fast_mode_flag);
	} else {
		printk("eth_fast_mode_flag value is %d, eth down stream fast mode disable\n", eth_fast_mode_flag);
	}
	return 0;
}


/*****************************************************************************
******************************************************************************/

static int eth_fast_slow_write_proc(struct file *file, const char *buffer,unsigned long count, void *data)	
{
	char val_string[96] = {0}, cmd[32]={0}, subcmd[32]={0};
	// int value = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	// sscanf(val_string, "%d", &value);
	sscanf(val_string, "%31s %31s", cmd, subcmd) ;
	val_string[count] = '\0';
	if(!strcmp(cmd, "fast")){
		if(!strcmp(subcmd, "enable") || !strcmp(subcmd, "1")) {
			eth_fast_mode_flag= 1;
		} else {
			eth_fast_mode_flag= 0;
		}
	} else{
	
	}
	
	// printk("fast_mode value is written %d\n", fast_mode);
	return count;
}


/******************************************************************************
******************************************************************************/
int eth_proc_init()
{
    struct proc_dir_entry *eth_proc;

    eth_proc = create_proc_entry("tc3162/fastmode", 0, NULL);
    eth_proc->read_proc = eth_fast_slow_read_proc;
    eth_proc->write_proc = eth_fast_slow_write_proc;
    
    eth_proc = create_proc_entry("tc3162/gsw_sp_tpid", 0, NULL);
    eth_proc->read_proc = special_tpid_read_proc;
    eth_proc->write_proc = special_tpid_write_proc;
  
    eth_proc = create_proc_entry("tc3162/gpon_bootflag", 0, NULL);
    eth_proc->read_proc = gpon_bootflag_read_proc;
    eth_proc = create_proc_entry("tc3162/gpon_commitflag", 0, NULL);
    eth_proc->read_proc = gpon_commitflag_read_proc;

    /* ethernet related stats */
    eth_proc = create_proc_entry("tc3162/eth_stats", 0, NULL);
    eth_proc->read_proc = eth_stats_read_proc;
    eth_proc->write_proc = eth_stats_write_proc;


    eth_proc = create_proc_entry("tc3162/gsw_stats", 0, NULL);
    eth_proc->read_proc = gsw_stats_read_proc;
    eth_proc->write_proc = gsw_stats_write_proc;

    /* wplin added 20120703 */
    eth_proc = create_proc_entry("tc3162/gsw_mib0", 0, NULL);
    eth_proc->read_proc = gsw_mib0_read_proc;
    eth_proc->write_proc = gsw_stats_write_proc;
    eth_proc = create_proc_entry("tc3162/gsw_mib1", 0, NULL);
    eth_proc->read_proc = gsw_mib1_read_proc;
    eth_proc->write_proc = gsw_stats_write_proc;
    eth_proc = create_proc_entry("tc3162/gsw_mib2", 0, NULL);
    eth_proc->read_proc = gsw_mib2_read_proc;
    eth_proc->write_proc = gsw_stats_write_proc;
    eth_proc = create_proc_entry("tc3162/gsw_mib3", 0, NULL);
    eth_proc->read_proc = gsw_mib3_read_proc;
    eth_proc->write_proc = gsw_stats_write_proc;
    eth_proc = create_proc_entry("tc3162/gsw_mib4", 0, NULL);
    eth_proc->read_proc = gsw_mib4_read_proc;
    eth_proc->write_proc = gsw_stats_write_proc;
    eth_proc = create_proc_entry("tc3162/gsw_mib5", 0, NULL);
    eth_proc->read_proc = gsw_mib5_read_proc;
    eth_proc->write_proc = gsw_stats_write_proc;
    eth_proc = create_proc_entry("tc3162/gsw_mib6", 0, NULL);
    eth_proc->read_proc = gsw_mib6_read_proc;
    eth_proc->write_proc = gsw_stats_write_proc;

    eth_proc = create_proc_entry("tc3162/gen_1588_pkt", 0, NULL);
    eth_proc->write_proc = gen_1588_pkt_write_proc;

	eth_proc = create_proc_entry("tc3162/7516bt_init", 0, NULL);
    eth_proc->write_proc = ether_7516bt_write_proc;

    create_proc_read_entry("tc3162/gsw_link_st", 0, NULL, gsw_link_st_proc, NULL);

#ifdef LOOPBACK_SUPPORT
    eth_proc = create_proc_entry("tc3162/eth_loopback", 0, NULL);
    eth_proc->read_proc = eth_loopback_read_proc;
    eth_proc->write_proc = eth_loopback_write_proc;

	eth_proc = create_proc_entry("tc3162/eth_vlanivl_set", 0, NULL);
    eth_proc->write_proc = eth_vlan_set_write_proc;

    eth_proc = create_proc_entry("tc3162/eth_loopback_test", 0, NULL);
    eth_proc->read_proc = eth_loopback_test_read_proc;
    eth_proc->write_proc = eth_loopback_test_write_proc;

    eth_proc = create_proc_entry("tc3162/vlantable_test", 0, NULL);
    eth_proc->write_proc = vlantable_test;
#endif

    create_proc_read_entry("tc3162/eth_link_st", 0, NULL, eth_link_st_proc, NULL);

    eth_proc = create_proc_entry("tc3162/port_reverse", 0, NULL);
    eth_proc->read_proc = port_reverse_read_proc;
    eth_proc->write_proc = port_reverse_write_proc;
    
    eth_proc = create_proc_entry("tc3162/stag_to_vtag", 0, NULL);
    eth_proc->read_proc = stag_to_vtag_read_proc;
    eth_proc->write_proc = stag_to_vtag_write_proc;


    /*vport enable/disable control*/
    eth_proc = create_proc_entry("tc3162/vport_enable", 0, NULL);
    if(eth_proc){
        eth_proc->read_proc = vport_enable_read_proc;
        eth_proc->write_proc = vport_enable_write_proc;
    }
    eth_proc = create_proc_entry("tc3162/eth_rx_packet_number", 0, NULL);
    if(eth_proc){
            eth_proc->read_proc = eth_rx_receive_num_read_proc;
            eth_proc->write_proc = eth_rx_receive_num_write_proc;
    }   
    eth_proc = create_proc_entry("tc3162/eth_portmap", 0, NULL);
    eth_proc->read_proc = ethernet_portmap_read_proc;
    eth_proc->write_proc = ethernet_portmap_write_proc;
    
#ifdef TCSUPPORT_QTN_ISOLATION
    eth_proc = create_proc_entry("tc3162/isolation_enable", 0, NULL);
    eth_proc->read_proc = isolation_enable_read_proc;
    eth_proc->write_proc = isolation_enable_write_proc;
#endif
#ifdef TCPHY_SUPPORT
  	create_proc_read_entry("tc3162/eth_port_status", 0, NULL, 
  	    eth_port_stat_read_proc, NULL);
#endif

#if defined(QOS_REMARKING) || defined(TCSUPPORT_HW_QOS)
        eth_proc = create_proc_entry("tc3162/eth1_qoswrr", 0, NULL);
        eth_proc->read_proc = eth_qoswrr_read_proc;
        eth_proc->write_proc = eth_qoswrr_write_proc;
#endif
    
#ifdef TCSUPPORT_QOS
        eth_proc = create_proc_entry("tc3162/eth1_tcqos_disc", 0, NULL);
        eth_proc->read_proc = eth_tcqos_read_proc;
        eth_proc->write_proc = eth_tcqos_write_proc;
#endif
	eth_proc = create_proc_entry("tc3162/totalBandwidth", 0, NULL);
	eth_proc->read_proc = total_Bandwidth_read_proc;

	eth_proc = create_proc_entry("tc3162/acl_rule", 0, NULL);
	eth_proc->read_proc = acl_rule_read_proc;
	eth_proc->write_proc = acl_rule_write_proc;

	eth_proc = create_proc_entry("tc3162/eth_sync_mactable", 0, NULL);
	eth_proc->read_proc = eth_sync_mactable_read_proc;
	eth_proc->write_proc = eth_sync_mactable_write_proc;

	eth_proc = create_proc_entry("tc3162/eth_force_destq",0, NULL);
	eth_proc->read_proc = eth_force_destq_read_proc;
	eth_proc->write_proc = eth_force_destq_write_proc;

	eth_proc = create_proc_entry("tc3162/ether_wan_state_clear", 0, NULL);
	eth_proc->write_proc = ether_wan_state_clear_write_proc;

	eth_proc = create_proc_entry("tc3162/soft_ratelimit_mac", 0, NULL);
	eth_proc->read_proc = mac_read_proc;
	eth_proc->write_proc = mac_write_proc;

	eth_proc = create_proc_entry("tc3162/eth_pausetime_for_test", 0, NULL);
        eth_proc->read_proc = eth_pausetime_for_test_read_proc;
	eth_proc->write_proc = eth_pausetime_for_test_write_proc;

    /* for EN7516 SOC test 20170714 */
	eth_proc = create_proc_entry("tc3162/eth_dump_rx_pkt", 0, NULL);
	eth_proc->read_proc = eth_dump_rx_pkt_read_proc;
	eth_proc->write_proc = eth_dump_rx_pkt_write_proc;

#ifdef CONFIG_SUPPORT_SELF_TEST
    eth_proc = create_proc_entry("tc3162/eth_self_test_setting", 0, NULL);
    eth_proc->read_proc = eth_self_test_setting_read_proc;
    eth_proc->write_proc = eth_self_test_setting_write_proc;

    eth_proc = create_proc_entry("tc3162/padding_test", 0, NULL);
    eth_proc->write_proc =  eth_padding_test_write_proc;
#endif


#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
	eth_proc = create_proc_entry("tc3162/gsw_ext_port", 0, NULL);
	eth_proc->read_proc = gsw_ext_port_read_proc;
	eth_proc->write_proc = gsw_ext_port_write_proc;
	eth_proc = create_proc_entry("tc3162/eth_port_vlan", 0, NULL);
	eth_proc->read_proc = eth_port_vlan_status_read_proc;
	eth_proc->write_proc = eth_port_vlan_status_write_proc;
#endif

    /*
    tc3162/gsw_mtu
    tc3162/mac_esd_check
    tc3162/protect_mode
    tc3162/cache_line_test
    tc3162/fe_reset
    tc3162/service_change_reset
    tc3162/eth_forward
    tc3162/avalanche_test
    */
    if (TCSUPPORT_DS_HWNAT_OFFLOAD_VAL)
    {
        eth_proc = create_proc_entry("tc3162/soft_qdma", 0, NULL);
        eth_proc->read_proc = hwnat_ds_offload_read_proc;
        eth_proc->write_proc =  hwnat_ds_offload_write_proc;
    }

	eth_proc = create_proc_entry("tc3162/eth_mac_default_queuemask", 0, NULL);
	eth_proc->read_proc = eth_mac_default_queuemask_read_proc;
	eth_proc->write_proc = eth_mac_default_queuemask_write_proc;
	
#ifdef TCSUPPORT_ASYM_VOD
	eth_proc = create_proc_entry("tc3162/eth_asym_test_soc_switch",0, NULL);
	eth_proc->read_proc = eth_asym_test_soc_switch_read_proc;
	eth_proc->write_proc = eth_asym_test_soc_switch_write_proc;
#endif

    eth_proc = create_proc_entry("tc3162/dyn_qdma1_txq_cngst_threshold", 0, NULL);
    eth_proc->read_proc = dyn_qdma_txq_cngst_threshold_read_proc;
    eth_proc->write_proc = dyn_qdma_txq_cngst_threshold_write_proc;

	eth_proc = create_proc_entry("tc3162/expand_64bit_monitor_status", 0, NULL);
	eth_proc->read_proc = expand_64bit_monitor_read_proc;
	eth_proc->write_proc = expand_64bit_monitor_write_proc;
	
    return 0;
}

int eth_proc_exit()
{    
    remove_proc_entry("tc3162/fastmode", 0);
    remove_proc_entry("tc3162/gsw_sp_tpid", 0);
    remove_proc_entry("tc3162/gpon_bootflag", 0);
    remove_proc_entry("tc3162/gpon_commitflag", 0);
    remove_proc_entry("tc3162/eth_stats", 0);
    remove_proc_entry("tc3162/gsw_stats", 0);
    remove_proc_entry("tc3162/gsw_link_st", 0);
#ifdef LOOPBACK_SUPPORT
    remove_proc_entry("tc3162/eth_loopback", 0);
	remove_proc_entry("tc3162/eth_vlanivl_set", 0);

    remove_proc_entry("tc3162/eth_loopback_test", 0);
    remove_proc_entry("tc3162/vlantable_test", 0);
#endif
    
    remove_proc_entry("tc3162/gen_1588_pkt", 0);

    remove_proc_entry("tc3162/gsw_mib0", 0);
    remove_proc_entry("tc3162/gsw_mib1", 0);
    remove_proc_entry("tc3162/gsw_mib2", 0);
    remove_proc_entry("tc3162/gsw_mib3", 0);
    remove_proc_entry("tc3162/gsw_mib4", 0);
    remove_proc_entry("tc3162/gsw_mib5", 0);
    remove_proc_entry("tc3162/gsw_mib6", 0);

	remove_proc_entry("tc3162/7516bt_init", 0);


    remove_proc_entry("tc3162/eth_link_st", 0);
    remove_proc_entry("tc3162/port_reverse", 0);
    remove_proc_entry("tc3162/stag_to_vtag", 0);
    remove_proc_entry("tc3162/vport_enable", 0);
    remove_proc_entry("tc3162/eth_rx_packet_number", 0);
    remove_proc_entry("tc3162/eth_portmap", 0);
    
#ifdef TCSUPPORT_QTN_ISOLATION
    remove_proc_entry("tc3162/isolation_enable", 0);
#endif
#ifdef TCPHY_SUPPORT
    remove_proc_entry("tc3162/eth_port_status", 0);
#endif

#if defined(QOS_REMARKING) || defined(TCSUPPORT_HW_QOS)
    remove_proc_entry("tc3162/eth1_qoswrr", 0);
#endif

#ifdef TCSUPPORT_QOS
    remove_proc_entry("tc3162/eth1_tcqos_disc", 0);
#endif
    
    remove_proc_entry("tc3162/protect_mode", 0);
    remove_proc_entry("tc3162/eth_sync_mactable", 0);
    remove_proc_entry("tc3162/eth_force_destq", 0);
    remove_proc_entry("tc3162/totalBandwidth", 0);
    remove_proc_entry("tc3162/ether_wan_state_clear", 0);
    remove_proc_entry("tc3162/soft_ratelimit_mac", 0);

#if defined(TCSUPPORT_MULTI_SWITCH_EXT)
	remove_proc_entry("tc3162/gsw_ext_port", 0);
	remove_proc_entry("tc3162/eth_port_vlan", 0);
#endif

    if (TCSUPPORT_DS_HWNAT_OFFLOAD_VAL)
    {
        remove_proc_entry("tc3162/soft_qdma", 0);
    }

#ifdef CONFIG_SUPPORT_SELF_TEST
    /* for EN7516 SOC test 20170714 */
    remove_proc_entry("tc3162/eth_dump_rx_pkt", 0);
    remove_proc_entry("tc3162/eth_self_test_setting", 0);
    remove_proc_entry("tc3162/tc3162/padding_test", NULL);
#endif

#ifdef TCSUPPORT_ASYM_VOD
	remove_proc_entry("tc3162/eth_asym_test_soc_switch", 0);
#endif
	remove_proc_entry("tc3162/dyn_qdma1_txq_cngst_threshold", 0);

	remove_proc_entry("tc3162/expand_64bit_monitor_status", 0);
    return 0;
}
