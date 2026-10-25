 /************************************************************************
*                  I N C L U D E S
*************************************************************************
*/
#include <linux/version.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/libcompileoption.h>
#include <linux/if_ether.h>
#include <linux/jhash.h>
#include <linux/inetdevice.h>
#include <linux/proc_fs.h>

#include <ecnt_hook/ecnt_hook_net.h>
#include <ecnt_hook/ecnt_hook_ether.h>

#include "ra_nat_cust.h"
#include "ra_nat_gen.h"
#include "ra_nat_dev.h"
#include "util.h"
#include "foe_fdb.h"
#include "ppe_api.h"
#include "ra_nat_ic_dis.h"
#include "ppe2_reg.h"

#include "../net/bridge/br_private.h"
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define DEV_METER_START 10
#define WAN_METER_START 10

#define	XSI_PCIE0_IDX	0
#define	XSI_PCIE1_IDX	1
#define	XSI_USB_IDX		2
#define XSI_ETH_IDX		4

int meter_mode = METER_MODE_DEV_MAC;

static ppeMulAcntGrp_t ppeMulAcntGrp;
static unsigned char FlowIdByMask_Mode = NOT_WORK_MODE;

int macMatchFoeEntry[MAX_DEV_BANDWIDTH_NUM*2];/*macMatchFoeEntry[0]:up match, macMatchFoeEntry[1]:down match*/	

int up_queue_cnt_start = 0;
static int down_queue_cnt_start = 0;
int up_down_queue_num = 0;
int up_queue_num = 0;
static int flow_meter_filter = 0xfc000;
static int flow_meter_shift = 14;
static int flow_acnt_filter = 0x03f00;
static int flow_acnt_shift = 8;

static unsigned int wanRatePpsMtuDefault = 1492;	/* Default MTU=1492 */
static unsigned int wanRatePpsThreshold = 10000;	/* Default 10000Mbps */
static unsigned int wanRatePpsCurWanType = 1;		/* Current config wan interface type, 1: route, 0:bridge */
static unsigned int wanRatePpsCurWanMtu = 1492;		/* Current config wan interface MTU */

unsigned int softCntByte[63];
unsigned int softCntPkts[63];

unsigned long long queueRxBytes[63];
unsigned int upDownQueueRate[63];

unsigned int upQueueRate[8];

unsigned int wanRatelimit[16];

unsigned char dslimit_enable = 0;

unsigned char ratelimitByItf_en = 0 ;
unsigned char ratelimitByIP_en = 0 ;
struct ppeRatelimitItfInfo RateLimitItfInfo[8] ;/*support LAN1~4, SSID1~SSID4,ratelimit*/
struct ppeRatelimitIPInfo RateLimitIPInfo[6] ;	/*support 6 pairs of IP range ratelimit*/

/* CDS Gemport ratelimit */
unsigned char upGemPortMeterStart = 10;
unsigned char upGemPortMeterNum = 8;
unsigned char downGemPortMeterNum = 8;
unsigned char downGemPortMeterStart = 0;
unsigned char gemPortMeterNum = 0;

int hwnat_del_en = 1;

lanip_account_info lanip_account;
br_ip br_ip_info;

int updateDevBandWidthMac(unsigned char *mac);
int delDevBandWidthMac(unsigned char *mac);
int updateDevBandWidth(unsigned char *mac, unsigned int upRate, unsigned int downRate);
int ppeSetMeterRate(uint index, uint rateLimit);
int findMacEntryIndex(unsigned char *mac);

void PpeGetBrIp(void);
static DEFINE_SPINLOCK(vip_check_lock);
struct Ppe_VIP_t Ppe_VIP_s;
unsigned int g_mul_high_priority = 0;
/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern struct devBandwidthList_s *gHwBandwidthList;

extern int DebugLevel;
extern unsigned int hwnat_dram_idx_base;

extern int (*dslimit_SetUpTrtcmEnable)(u8 enable);
extern int (*dslimit_SetUpTrtcm)(u32 idx,u32 ratelimit);

extern int wan_port_id;

extern struct FoeEntryExt*	foe_ext;

extern int hwnat_clean_wifi_entry(void);
extern int hwnat_delete_foe_entry(int index);
extern struct FoeEntry	*PpeFoeBase;
extern int32_t PpeCleanTableHandler(void);
extern int ppe_get_vlan_tag_num(struct sk_buff* skb);
extern struct net_bridge_fdb_entry *__br_fdb_get(struct net_bridge *br,const unsigned char *addr,__u16 vid);
extern int PpeCleanCache751221(void);
extern int (*ra_sw_nat_hook_clean_entry_by_src_mac)(struct net_data_s *net_data);
extern int (*hwnat_clean_xsi_lan_hook)(unsigned int hsgmii_index, unsigned int channel);
extern int (*ra_sw_nat_hook_clean_entry_by_mac)(const unsigned char* mac);
extern struct hwnat_shrink_table shnkTbl[UPDMEM_NUM];
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
/*****************joyme bindwidth start******************************************/
/*check mac*/
int isMacMatch(uint8_t * Dst, uint8_t * Src){
	if((Dst[0] == Src[0]) 
		&& (Dst[1] == Src[1]) 
		&& (Dst[2] == Src[2]) 
		&& (Dst[3] == Src[3]) 	
		&& (Dst[4] == Src[4]) 
		&& (Dst[5] == Src[5])){
			return 1;
		}
	
	return 0;
}

/*find bandwitdh item by mac*/
int findMacMatchItem(unsigned char *mac){
	struct devBandwidth_s *devBandWidthPtr = NULL;
	int i = 0;
	
	if(gHwBandwidthList == NULL)
		return -1;

	for(i = 0; i < MAX_DEV_BANDWIDTH_NUM; i++){
		devBandWidthPtr = &(gHwBandwidthList->bandwidthList[i]);
		if(devBandWidthPtr != NULL && isMacMatch(mac,devBandWidthPtr->mac)){
			return i;
		}		
	}

	return -1;	
}

/*set Account Group index*/
inline int setPPeTxACNTGRP(struct sk_buff * skb, struct FoeEntry * foe_entry)
{
	unsigned char dstMacAddr[6];	
	int matchItem = -1;
	struct ethhdr *eth = NULL;

	if(gHwBandwidthList == NULL)
		return 0;

	if(likely(gHwBandwidthList->enable == 0))
		return 0;
	
	eth = (struct ethhdr *)skb->data;
	memcpy(dstMacAddr, eth->h_dest, ETH_ALEN);
	
	matchItem = findMacMatchItem(dstMacAddr);
	if (gHwBandwidthList->enable == 4) {		
		
		printk("setPPeTxACNTGRP : matchItem=%d\n",matchItem);	
		printk("setPPeTxACNTGRP : macMatchFoeEntry[1]=%d\n",FOE_ENTRY_NUM(skb));	
		printk("dstMacAddr : %02x:%02x:%02x:%02x:%02x:%02x\n",
			dstMacAddr[0],dstMacAddr[1],dstMacAddr[2],dstMacAddr[3],dstMacAddr[4],dstMacAddr[5]);
	}

	if(matchItem >= 0){		
		macMatchFoeEntry[matchItem*2+1] = FOE_ENTRY_NUM(skb);	
		if (gHwBandwidthList->enable == 4) {				
			printk("setPPeTxACNTGRP : macMatchFoeEntry[1]=%d\n",macMatchFoeEntry[matchItem*2+1]);		
			printk("setPPeTxACNTGRP : %02x:%02x:%02x:%02x:%02x:%02x\n",
				dstMacAddr[0],dstMacAddr[1],dstMacAddr[2],dstMacAddr[3],dstMacAddr[4],dstMacAddr[5]);	
		}	
		return 1;	
	}
	
	return 0;
}

/*get account group index*/
inline u_int16_t PpeGetMacMatchAcntGrpIndex(struct sk_buff * skb)
{
	int entryNum;
	int i = 0;
	struct devBandwidth_s *devBandWidthPtr = NULL;
	
	if (gHwBandwidthList == NULL || gHwBandwidthList->enable ==0)
		return HWNAT_DEFAULT_GRP_ID;	

	if (likely(gHwBandwidthList->enable ==0))
		return HWNAT_DEFAULT_GRP_ID;	

	entryNum = FOE_ENTRY_NUM(skb);
	for(i = 0; i < MAX_DEV_BANDWIDTH_NUM*2; i++){
		if(macMatchFoeEntry[i] == entryNum){
			devBandWidthPtr = &(gHwBandwidthList->bandwidthList[i/2]);

			if (gHwBandwidthList->enable == 3) {
				printk("PpeGetMacMatchAcntGrpIndex :entryNum=%d,i=%d,valid=%d\n",entryNum,i,devBandWidthPtr->valid);
			}

			if(devBandWidthPtr->valid == 0){
				break; 
			}
			
			return i;
		}	
	}

	return HWNAT_DEFAULT_GRP_ID;	
}

inline u_int16_t PpeGetMacMatchMeterGrpIndex(struct sk_buff * skb)
{
	int entryNum;
	int i = 0;
	struct devBandwidth_s *devBandWidthPtr = NULL;
	if (gHwBandwidthList == NULL || gHwBandwidthList->enable ==0){
		return HWNAT_DEFAULT_GRP_ID;	
		}
	if (likely(gHwBandwidthList->enable ==0))
		return HWNAT_DEFAULT_GRP_ID;	

	entryNum = FOE_ENTRY_NUM(skb);
	for(i = 0; i < MAX_DEV_BANDWIDTH_NUM*2; i++){
		if(macMatchFoeEntry[i] == entryNum){
			devBandWidthPtr = &(gHwBandwidthList->bandwidthList[i/2]);

			if (gHwBandwidthList->enable == 3) {
				printk("PpeGetMacMatchMeterGrpIndex :entryNum=%d,i=%d,valid=%d\n",entryNum,i,devBandWidthPtr->valid);
			}
			if(devBandWidthPtr->valid == 0){
				break; 
			}
			if((i % 2) && (devBandWidthPtr->downRate == 0)){
				//  dev down rate limit
				break;
			}else if(((i&1)==0) && (devBandWidthPtr->upRate == 0)){
				//  dev up rate limit
				break;
			}
			return i+DEV_METER_START;
		}	
	}

	return HWNAT_DEFAULT_GRP_ID;	
}

/*set Account Group index*/
inline int setPPeRxACNTGRP(struct sk_buff * skb){
	unsigned char srcMacAddr[6];
	unsigned char dstMacAddr[6];
	struct ethhdr *eth = NULL;
	int matchItem = -1;

	if(FOE_ENTRY_NUM(skb) == 0){
		return 0;
	}

	if(gHwBandwidthList == NULL)
		return 0;

	if(likely(gHwBandwidthList->enable == 0))
		return 0;
		
	eth = (struct ethhdr *)LAYER2_HEADER(skb);
	memcpy(srcMacAddr, eth->h_source, ETH_ALEN);
	memcpy(dstMacAddr, eth->h_dest, ETH_ALEN);

	matchItem = findMacMatchItem(srcMacAddr);
	if(matchItem >= 0){
		/*match src Mac*/
		macMatchFoeEntry[matchItem*2] = FOE_ENTRY_NUM(skb);
		
		if (gHwBandwidthList->enable == 2) {
			printk("setPPeRxUsACNTGRP:macMatchFoeEntry[0]=%d\n",macMatchFoeEntry[matchItem*2]);
			printk("srcMacAddr : %02x:%02x:%02x:%02x:%02x:%02x\n",srcMacAddr[0],srcMacAddr[1],srcMacAddr[2],srcMacAddr[3],srcMacAddr[4],srcMacAddr[5]);
		}
		return 1;
	}

	
	return 0;
}

/*find mac by bandwitdh item*/
int findItemMatchMac(int index, unsigned char *mac){
    struct devBandwidth_s *devBandWidthPtr = NULL;
    
    if(gHwBandwidthList == NULL)
        return -1;
    
    devBandWidthPtr = &(gHwBandwidthList->bandwidthList[index]);

	if( devBandWidthPtr != NULL ){
		memcpy(mac, devBandWidthPtr->mac, ETH_ALEN);
		return 1;
    }

    return -1;  
}

int findGswIdxByMac(unsigned char *mac)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
	struct net_bridge_fdb_entry *dst;
	struct net_device *ndev = NULL;
	struct net_bridge *br = NULL;
	struct net_device *dev = NULL;
	unsigned char lanPortId = 0;
	unsigned char gswPortId = 0;

	const unsigned char *dest = mac;
	ndev = dev_get_by_name(&init_net, "br0");
	if (ndev)
		br = netdev_priv(ndev);
	else
		return -1;
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,18,19)
	if ((dst = __br_fdb_get(br, dest, 0)) != NULL)
#else
	if ((dst = __br_fdb_get(br, dest)) != NULL)
#endif
	{
		if(IS_FDB_LOCAL(dst)){

			dev_put(ndev);
			return -1;
		}
		
		dev = dst->dst->dev;
		if(dev)
		{
			if(isLANInterface(dev)){
				lanPortId = getSwitchLANIndex(dev);
				ETHER_API_GET_PORTMAP(lanPortId, &gswPortId);

				dev_put(ndev);
				return gswPortId;
			}
		}
	}

	dev_put(ndev);
#endif

	return -1;
}

int updateMulticastCntToDevCnt(unchar idx, uint *Down_Bytes_lo, uint *Down_Bytes_hi, uint *Down_Pkts)
{
	struct devBandwidth_s *devBandWidthPtr = NULL;
	int i = 0;
	unsigned char mac[6];
	int gswPortId = 0;
	unsigned int gswMcPktCnt = 0;
	unsigned int gswAllByteLoCnt = 0;
	unsigned int gswAllByteHiCnt = 0;
	unsigned long long byteCnt = 0;
	unsigned long long uniAllByteCnt = 0;
	unsigned long long gswAllByteCnt = 0;
	unsigned int byteHigh = 0, byteLow = 0;

	memset(mac, 0, 6 * sizeof(unsigned char));

	if( findItemMatchMac(idx, mac) > 0 ){
		gswPortId = findGswIdxByMac(mac);
		if( gswPortId < 0 ){
			return 0;
		}

		/* 1. add multicast packet cnt to packet cnt */
		ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_MCAST_CNT, gswPortId, &gswMcPktCnt);
		*Down_Pkts += gswMcPktCnt;

		/* 2. add multicast byte cnt to packet cnt */

		/* read this port all tx unicast byte cnt */
		for(i = 0; i < MAX_DEV_BANDWIDTH_NUM; i++){
			devBandWidthPtr = &(gHwBandwidthList->bandwidthList[i]);
			if( (devBandWidthPtr != NULL) &&  (gswPortId == findGswIdxByMac(devBandWidthPtr->mac)) ){
				FE_API_GET_AC_GROUP_BYTE_CNT(2*i + 1, &byteLow, &byteHigh);
				uniAllByteCnt += (((unsigned long long)byteHigh) << 32) + (unsigned long long)byteLow;
			}		
		}

		/* read this port all tx byte cnt */
		ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_OCT_CNT_L, gswPortId, &gswAllByteLoCnt);
		ETHER_API_GET_PER_PORT_MIB_COUNTER(MIB_ID_TX_OCT_CNT_H, gswPortId, &gswAllByteHiCnt);
		gswAllByteCnt = (((unsigned long long)gswAllByteHiCnt) << 32) + (unsigned long long)gswAllByteLoCnt;

		/* this port multicast cnt = all tx byte cnt -  all tx unicast byte cnt */
		byteCnt = (((unsigned long long)*Down_Bytes_hi) << 32) + (unsigned long long)*Down_Bytes_lo;
		if( gswAllByteCnt > uniAllByteCnt){
			byteCnt += gswAllByteCnt - uniAllByteCnt;
		}else{
			byteCnt += gswAllByteCnt + ((~(unsigned long long)0) - uniAllByteCnt);
		}

		*Down_Bytes_hi = (byteCnt >> 32); 
		*Down_Bytes_lo = byteCnt & 0xFFFFFFFF;
	}

	return 0;
}

/*bandwidht read proc*/
static int hwnat_mac_bandwidth_read_proc(char *buf, char **start, off_t off,
		int count, int *eof, void *data)
{
	int index = 0;
	int i;
	struct devBandwidth_s *devBandWidthPtr = NULL;
	unsigned long long uniUpxCnt = 0;
	unsigned long long uniDownCnt = 0;	
	unsigned long long uniUpBytes = 0;
	unsigned long long uniDownBytes = 0;
	uint pktcnts_lo = 0, pktcnts_hi = 0;
	uint Bytes_lo = 0, Bytes_hi = 0;
	static unsigned int pre_ac_byte_cnt_lo[MAX_DEV_BANDWIDTH_NUM*2];
	static unsigned int pre_ac_byte_cnt_hi[MAX_DEV_BANDWIDTH_NUM*2];
	int upRuleIndex = 0;
	int downRuleIndex = 0;

	if(gHwBandwidthList != NULL){
		index += sprintf(buf+index, "enable = %d, portMultiMacEn = %d\n", 
			gHwBandwidthList->enable, gHwBandwidthList->portMultiMacEn);		
		for(i = 0; i < MAX_DEV_BANDWIDTH_NUM; i++){		
			devBandWidthPtr = &(gHwBandwidthList->bandwidthList[i]);
			if(devBandWidthPtr->valid == 0){
				continue;
			}
			
			upRuleIndex = 2*i;
			downRuleIndex = upRuleIndex+1;
            if(isEN7528 || SUPPORT_FLOW_ACNT){
				FE_API_GET_FlOW_CNT(i, UP_STREAM, DEV_MODE, &Bytes_lo, &Bytes_hi);
				FE_API_GET_FlOW_PKT_CNT(i, UP_STREAM, DEV_MODE, &pktcnts_lo, &pktcnts_hi);
				uniUpBytes = ((unsigned long long)Bytes_hi << 32) + Bytes_lo;
				uniUpxCnt = ((unsigned long long)pktcnts_lo << 32) + pktcnts_hi;
				FE_API_GET_FlOW_PKT_CNT(i, DOWN_STREAM, DEV_MODE, &pktcnts_lo, &pktcnts_hi);
				FE_API_GET_FlOW_CNT(i, DOWN_STREAM, DEV_MODE, &Bytes_lo, &Bytes_hi);
				uniDownBytes = ((unsigned long long)Bytes_hi << 32) + Bytes_lo ;
				uniDownCnt = ((unsigned long long)pktcnts_lo << 32) + pktcnts_hi;
			}else if (IS_IC_EN751X2X){
				FE_API_GET_AC_GROUP_PKT_CNT(upRuleIndex, (uint *)&uniUpxCnt);
				FE_API_GET_AC_GROUP_BYTE_CNT(upRuleIndex, &Bytes_lo, &Bytes_hi);
				uniUpBytes = ((((unsigned long long)Bytes_hi << 32) + Bytes_lo) 
					- (((unsigned long long)pre_ac_byte_cnt_hi[upRuleIndex] << 32) 
					+ pre_ac_byte_cnt_lo[upRuleIndex]));
				pre_ac_byte_cnt_lo[upRuleIndex] = Bytes_lo;
				pre_ac_byte_cnt_hi[upRuleIndex] = Bytes_hi;

				FE_API_GET_AC_GROUP_PKT_CNT(downRuleIndex, (uint *)&uniDownCnt);
				FE_API_GET_AC_GROUP_BYTE_CNT(downRuleIndex, &Bytes_lo, &Bytes_hi);
				if(TCSUPPORT_CT_JOYME2_VAL){
					updateMulticastCntToDevCnt(i, &Bytes_lo, &Bytes_hi, (uint *)&uniDownCnt);
				}
				uniDownBytes = ((((unsigned long long)Bytes_hi << 32) + Bytes_lo) 
					- (((unsigned long long)pre_ac_byte_cnt_hi[downRuleIndex] << 32) 
					+ pre_ac_byte_cnt_lo[downRuleIndex]));
				pre_ac_byte_cnt_lo[downRuleIndex] = Bytes_lo;
				pre_ac_byte_cnt_hi[downRuleIndex] = Bytes_hi;
			}else{
				uniUpxCnt = IO_GREG(AC_BASE+8+(upRuleIndex<<4));
				uniUpBytes = (((unsigned long long)IO_GREG(AC_BASE+4+(upRuleIndex<<4)))<<32)+IO_GREG(AC_BASE+(upRuleIndex<<4));
			
				uniDownCnt = IO_GREG(AC_BASE+8+(downRuleIndex<<4));
				uniDownBytes = (((unsigned long long)IO_GREG(AC_BASE+4+(downRuleIndex<<4)))<<32)+IO_GREG(AC_BASE+(downRuleIndex<<4));
			}
			if(isEN7528 || SUPPORT_FLOW_ACNT){
				devBandWidthPtr->upBytes = uniUpBytes;
				devBandWidthPtr->downBytes = uniDownBytes;
				
				index += sprintf(buf+index, "%d,%llu,%llu,%02x%02x%02x%02x%02x%02x\n", 
				i, devBandWidthPtr->upBytes, devBandWidthPtr->downBytes, devBandWidthPtr->mac[0], devBandWidthPtr->mac[1],devBandWidthPtr->mac[2],devBandWidthPtr->mac[3],devBandWidthPtr->mac[4],devBandWidthPtr->mac[5]);
			}else if(TCSUPPORT_CF_JOYMEV2_PON_VAL || TCSUPPORT_CMCCV2_VAL){
				devBandWidthPtr->upBytes += uniUpBytes;
				devBandWidthPtr->downBytes += uniDownBytes;

				index += sprintf(buf+index, "%d,%llu,%llu,%02x%02x%02x%02x%02x%02x\n", 
				i, devBandWidthPtr->upBytes, devBandWidthPtr->downBytes, devBandWidthPtr->mac[0], devBandWidthPtr->mac[1],devBandWidthPtr->mac[2],devBandWidthPtr->mac[3],devBandWidthPtr->mac[4],devBandWidthPtr->mac[5]);
			}else{
				index += sprintf(buf+index, "%d,%llu,%llu,%02x%02x%02x%02x%02x%02x\n", 
					i, uniUpBytes,uniDownBytes,devBandWidthPtr->mac[0], devBandWidthPtr->mac[1],devBandWidthPtr->mac[2],devBandWidthPtr->mac[3],devBandWidthPtr->mac[4],devBandWidthPtr->mac[5]);
				}
		}
	}else{
		index += sprintf(buf+index, "enable = %d\n", 0);
		printk("hwnat_mac_bandwidth_read_proc: gHwBandwidthList == NULL\n");
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

/*bandwidht write proc*/
int hwnat_mac_bandwidth_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	int len;
	char get_buf[64];	
	int mac[6];
	unsigned char strMac[6] = {0};
	int actionType = 0;
	int enable = 0;
	int portMultiMacEn = 0;
    unsigned int upRate = 0;
    unsigned int downRate = 0;
    int index = -1;

	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > 63)
		len = 63;
	else
		len = count;
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	
	get_buf[len]='\0';
	if(len < 12 ){
		sscanf(get_buf, "%d %d",&enable,&portMultiMacEn);
		if (gHwBandwidthList && 
			(enable == gHwBandwidthList->enable) && 
			(portMultiMacEn == gHwBandwidthList->portMultiMacEn))
			return len;
		
		if(enable == 0){		
			if(gHwBandwidthList != NULL)
				gHwBandwidthList->enable = enable;
		}else{
			if(gHwBandwidthList == NULL){
				gHwBandwidthList = (struct devBandwidthList_s *)kmalloc(sizeof(struct devBandwidthList_s), GFP_KERNEL);
				if(gHwBandwidthList == NULL){
					printk("no memory for wifiBandwidthNodes\n");
					return -1;
				}
				memset(gHwBandwidthList,0,sizeof(struct devBandwidthList_s));		
			}
			
			gHwBandwidthList->enable = enable;
			gHwBandwidthList->portMultiMacEn = portMultiMacEn;
			
		}
		
		if(TCSUPPORT_CMCC_VAL){
			/* clean wifi hwnat entry */
			hwnat_clean_wifi_entry();
		}
			
	}else{
        if(SUPPORT_METER_RATELIMIT){
            sscanf(get_buf, "%d %02x:%02x:%02x:%02x:%02x:%02x,%u,%u", 
                &actionType, &mac[0], &mac[1],&mac[2],&mac[3],&mac[4],&mac[5],&upRate,&downRate);

            strMac[0] = (unsigned char)mac[0];
            strMac[1] = (unsigned char)mac[1];
            strMac[2] = (unsigned char)mac[2];
            strMac[3] = (unsigned char)mac[3];
            strMac[4] = (unsigned char)mac[4];
            strMac[5] = (unsigned char)mac[5];

            if(actionType == 1){
                /*add and update mac*/
                updateDevBandWidthMac(strMac);
            }else if(actionType == 2){
                /*delete mac*/
                delDevBandWidthMac(strMac);
            }else if(actionType== 3){
                /*set mac based up and down ratelimit*/
                index = findMacEntryIndex(strMac);
                if(index < 0 || index == MAX_DEV_BANDWIDTH_NUM){          
                    return -EFAULT;
                }
                FE_API_SET_METER_RATELIMIT_MODE(UP_STREAM, DEV_MAC_MODE);
                FE_API_SET_METER_RATELIMIT_MODE(DOWN_STREAM, DEV_MAC_MODE);

                if(upRate == 0)
                    FE_API_SET_METER_RATELIMIT(upRate, index, UP_STREAM, DEV_MAC_MODE, 0);
                else
                    FE_API_SET_METER_RATELIMIT(upRate, index, UP_STREAM, DEV_MAC_MODE, 1);

                if(downRate == 0)
                    FE_API_SET_METER_RATELIMIT(downRate, index, DOWN_STREAM, DEV_MAC_MODE, 0);
                else
                    FE_API_SET_METER_RATELIMIT(downRate, index, DOWN_STREAM, DEV_MAC_MODE, 1);
            } else if (actionType== 4){
                /*clear bandwidth account according mac*/
                index = findMacEntryIndex(strMac);
                if(index < 0 || index == MAX_DEV_BANDWIDTH_NUM){          
                    return -EFAULT;
                }
                FE_API_CLEAR_FlOW_CNT(index, UP_STREAM, DEV_MODE);
                FE_API_CLEAR_FlOW_CNT(index, DOWN_STREAM, DEV_MODE);
            }
        }else {
    		sscanf(get_buf, "%d %02x:%02x:%02x:%02x:%02x:%02x,%u,%u", 
    			&actionType, &mac[0], &mac[1],&mac[2],&mac[3],&mac[4],&mac[5],&upRate,&downRate);

    		strMac[0] = (unsigned char)mac[0];
    		strMac[1] = (unsigned char)mac[1];
    		strMac[2] = (unsigned char)mac[2];
    		strMac[3] = (unsigned char)mac[3];
    		strMac[4] = (unsigned char)mac[4];
    		strMac[5] = (unsigned char)mac[5];

    		if(actionType == 1){
    			/*add and update mac*/
    			updateDevBandWidthMac(strMac);
    		}else if(actionType == 2){
    			delDevBandWidthMac(strMac);
    		}else if(actionType == 3) {
				index = findMacEntryIndex(strMac);
				if(index < 0 || index == MAX_DEV_BANDWIDTH_NUM){			
						return -EFAULT;
				 }
				gHwBandwidthList->bandwidthList[index].upRate = upRate<<10;
				gHwBandwidthList->bandwidthList[index].downRate = downRate<<10;
				updateDevBandWidth(strMac, upRate<<10, downRate<<10);
    		}
        }
	}

	if(gHwBandwidthList)
		printk("enable=%d,%02x%02x%02x%02x%02x%02x,actionType=%d\n", gHwBandwidthList->enable, strMac[0], strMac[1],strMac[2],strMac[3],strMac[4],strMac[5],actionType);
	else
		printk("gHwBandwidthList is NULL\n");
	
	return len;
}

#if !defined(TCSUPPORT_XPON_HAL_API_NG)
static int hwnat_del_by_ip_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	return 0;
}

static int hwnat_del_by_ip_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[64];
	unsigned int ip_addr = 0;

	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;

	memset(get_buf, 0, sizeof(get_buf));

	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%u", &ip_addr);

	hwnat_clean_entry_by_ip(ip_addr);

	ip_addr = htonl(ip_addr);

	printk("the ip address to be deleted is %pI4\n", &ip_addr);

	return count;
}
#endif

int wifi_ratelimit_enable = 0;

static int wifi_ratelimit_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	if(wifi_ratelimit_enable)
		printk("%s wifi tx ratelimit enable \n",__func__);
	else
		printk("%s wifi tx ratelimit disable \n",__func__);
	return 0 ;
}

int wifi_ratelimit_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	int len;
	char get_buf[64];	

	/* do a range checking, don't overflow buffers in kernel modules */
	if(count > 63)
		len = 63;
	else
		len = count;
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	
	get_buf[len]='\0';
	sscanf(get_buf, "%d",&wifi_ratelimit_enable);
	printk("%s wifi ratelimit enable = %d \n",__func__,wifi_ratelimit_enable);
	return len;
}

/*find Entry index by mac*/
int findMacEntryIndex(unsigned char *mac){
	struct devBandwidth_s *devBandWidthPtr = NULL;
	int firstIndex = -1;
	int i = 0;
	
	if(gHwBandwidthList == NULL)
		return -1;

	for(i = 0; i < MAX_DEV_BANDWIDTH_NUM; i++){
		devBandWidthPtr = &(gHwBandwidthList->bandwidthList[i]);
		if(devBandWidthPtr != NULL && isMacMatch(mac,devBandWidthPtr->mac)){
			return i;
		}

		if(devBandWidthPtr != NULL && devBandWidthPtr->valid){
			continue;
		}
		
		if(firstIndex == -1){
			firstIndex = i;
		}
	}

	return firstIndex;	
}

/*del bandwidth itm by mac*/
int delDevBandWidthMac(unsigned char *mac){
	int index = -1;
	struct devBandwidth_s *devBandWidthPtr = NULL;

	if(gHwBandwidthList == NULL)
		return 0;

	if(mac == NULL){
		return -1;
	}

	index = findMacMatchItem(mac);

	if(index < 0){
		return -1;
	}

	devBandWidthPtr = &(gHwBandwidthList->bandwidthList[index]);
	if(devBandWidthPtr == NULL){
		printk("delDevBandWidthMac:devBandWidthPtr == NULL,index =%d\n",index);
		return -2;
	}
	
	devBandWidthPtr->valid = 0;
	return 0;
}

/*addd or update bandwidth itm by mac*/
int updateDevBandWidthMac(unsigned char *mac){
	int index = -1;
	struct devBandwidth_s *devBandWidthPtr = NULL;

	if(gHwBandwidthList == NULL){
		gHwBandwidthList = (struct devBandwidthList_s *)kmalloc(sizeof(struct devBandwidthList_s), GFP_ATOMIC);
		if(gHwBandwidthList == NULL){
			printk("no memory for wifiBandwidthNodes\n");
			return -1;
		}
		memset(gHwBandwidthList,0,sizeof(struct devBandwidthList_s));		
	}	


	index = findMacEntryIndex(mac);
	if(index < 0){
		return -2;
	}

	devBandWidthPtr = &(gHwBandwidthList->bandwidthList[index]);
	if(devBandWidthPtr == NULL){
		printk("updateDevBandWidthMac:devBandWidthPtr == NULL,index =%d\n",index);
		return -3;
	}
	if(devBandWidthPtr->valid == 1)
		return 0;
	
	memcpy(&(devBandWidthPtr->mac),mac, ETH_ALEN);
	devBandWidthPtr->valid = 1;

	if (DebugLevel >= 1)
		printk( "updateDevBandWidthMac:index=%d,%02x:%02x:%02x:%02x:%02x:%02x\n", 
			index,devBandWidthPtr->mac[0], devBandWidthPtr->mac[1],devBandWidthPtr->mac[2],devBandWidthPtr->mac[3],devBandWidthPtr->mac[4],devBandWidthPtr->mac[5]);
	return 0;
}
int updateDevBandWidth(unsigned char *mac, unsigned int upRate, unsigned int downRate)
{

	int index = -1;
	struct devBandwidth_s *devBandWidthPtr = NULL;

	if(gHwBandwidthList == NULL){
		gHwBandwidthList = (struct devBandwidthList_s *)kmalloc(sizeof(struct devBandwidthList_s), GFP_KERNEL);
		if(gHwBandwidthList == NULL){
			printk("no memory for wifiBandwidthNodes\n");
			return -1;
		}
		memset(gHwBandwidthList,0,sizeof(struct devBandwidthList_s));		
	}	

	index = findMacEntryIndex(mac);
	if(index < 0){
		return -2;
	}
	if(upRate > 0){
		ppeSetMeterRate(index*2+DEV_METER_START, upRate);
	}
	if(downRate > 0){
		ppeSetMeterRate(index*2+1+DEV_METER_START, downRate);
	}

	devBandWidthPtr = &(gHwBandwidthList->bandwidthList[index]);
	if(devBandWidthPtr == NULL){
		printk("updateDevBandWidthMac:devBandWidthPtr == NULL,index =%d\n",index);
		return -3;
	}
	
	memcpy(&(devBandWidthPtr->mac),mac, ETH_ALEN);
	devBandWidthPtr->valid = 1;

	if (DebugLevel >= 1)
		printk( "updateDevBandWidthMac:index=%d,%02x:%02x:%02x:%02x:%02x:%02x\n", 
			index,devBandWidthPtr->mac[0], devBandWidthPtr->mac[1],devBandWidthPtr->mac[2],devBandWidthPtr->mac[3],devBandWidthPtr->mac[4],devBandWidthPtr->mac[5]);
	return 0;
}

ecnt_ret_val ecnt_hwnat_updateDevBandWidthMac(struct ecnt_data *in_data){
	struct net_data_s *net_data = (struct net_data_s *)in_data;
	unsigned char *addr = (unsigned char *)net_data->br_fdb_data.addr;
	struct net_bridge_fdb_entry *fdb = net_data->br_fdb_data.fdb;
	
	if(fdb == NULL || fdb->dst == NULL || fdb->dst->dev == NULL){
		return ECNT_CONTINUE;
	}

    if (!IS_FDB_LOCAL(fdb))
    {
		if ( isLANInterface(fdb->dst->dev) || isWiFiInterface(fdb->dst->dev) ){
			updateDevBandWidthMac(addr);
		}
    }

	return ECNT_CONTINUE;
}

ecnt_ret_val ecnt_hwnat_delDevBandWidthMac(struct ecnt_data *in_data){
	struct net_data_s *net_data = (struct net_data_s *)in_data;
	struct net_bridge_fdb_entry *fdb = net_data->br_fdb_data.fdb;

	if(fdb == NULL || fdb->dst == NULL || fdb->dst->dev == NULL){
		return ECNT_CONTINUE;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)	
	unsigned char *addr = fdb->addr.addr;
#else
	unsigned char *addr = fdb->key.addr.addr;
#endif
	


   if (!IS_FDB_LOCAL(fdb))
    {
    	if ( isLANInterface(fdb->dst->dev) || isWiFiInterface(fdb->dst->dev)){
			delDevBandWidthMac(addr);
		}
    }

	return ECNT_CONTINUE;
}

struct ecnt_hook_ops ecnt_hwnat_updateDevBandWidthMac_op = {
	.name = "ecnt_hwnat_updateDevBandWidthMac",
	.is_execute = 1,
	.hookfn = ecnt_hwnat_updateDevBandWidthMac,
	.maintype = ECNT_NET_BR_FDB,
	.subtype = ECNT_BR_FDB_UPDATE,
	.priority = 1
};


struct ecnt_hook_ops ecnt_hwnat_delDevBandWidthMac_op = {
	.name = "ecnt_delDevBandWidthMac",
	.is_execute = 1,
	.hookfn = ecnt_hwnat_delDevBandWidthMac,
	.maintype = ECNT_NET_BR_FDB,
	.subtype = ECNT_BR_FDB_DELETE,
	.priority = 1
};

static int hwnat_multicast_account_read_proc(char *buf, char **start, off_t off,
		int count, int *eof, void *data)
{
	int index = 0;
	
	index += sprintf(buf+index, "multiPktCnt multiBytes\n");

	ppeMulAcntGrp.mulCnt += IO_GREG(AC_BASE+8+(HWNAT_MULTI_ACNTGRP<<4));
	ppeMulAcntGrp.mulBytes += ((((unsigned long long)(IO_GREG(AC_BASE+4+(HWNAT_MULTI_ACNTGRP<<4))))<<32)+IO_GREG(AC_BASE+(HWNAT_MULTI_ACNTGRP<<4)));

    index += sprintf(buf+index, "   %u     %llu\n", ppeMulAcntGrp.mulCnt, ppeMulAcntGrp.mulBytes);

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

static int hwnat_multicast_account_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
    char val_string[64];
    char* endpo; 
    int ret = 0;
    int enable = 0;
        
    if (count > sizeof(val_string) - 1)
    {
        ret = -EFAULT;
        goto end;
    }
    
    memset(val_string,0,64);
    if (copy_from_user(val_string, buffer, count))
    {
        ret = -EFAULT;
        goto end;
    }

    enable = simple_strtol(val_string,&endpo,10);

    if(enable == 1)
    {
        //read clear
        IO_GREG(AC_BASE+8+(HWNAT_MULTI_ACNTGRP<<4));
        IO_GREG(AC_BASE+4+(HWNAT_MULTI_ACNTGRP<<4));
        IO_GREG(AC_BASE+(HWNAT_MULTI_ACNTGRP<<4));
        
        memset(&ppeMulAcntGrp,0,sizeof(ppeMulAcntGrp));

        printk("Mulcast account clear!\n");
    }
    else
    {
        ret = -EFAULT;
        goto end;
    }

    ret = count;
    end:
    return ret;

}

/*init function of bandwidth*/
int joymeBandWidthFuncInit(void){
	struct proc_dir_entry *hwnat_proc;

	ecnt_register_hook(&ecnt_hwnat_updateDevBandWidthMac_op);
	ecnt_register_hook(&ecnt_hwnat_delDevBandWidthMac_op);

	hwnat_proc = create_proc_entry("tc3162/hwnat_multicast_account", 0, NULL);
	if(hwnat_proc){
		hwnat_proc->read_proc = hwnat_multicast_account_read_proc;
		hwnat_proc->write_proc = hwnat_multicast_account_write_proc;
	}else{
		printk("proc_entry tc3162/hwnat_multicast_account alloc fail\n");
	}

	memset(&ppeMulAcntGrp,0,sizeof(ppeMulAcntGrp_t));

	return 0;
}

/*deinit function of bandwidth*/
void joymeBandWidthFuncDeInit(void){

	remove_proc_entry("tc3162/hwnat_multicast_account", NULL); 

	ecnt_unregister_hook(&ecnt_hwnat_updateDevBandWidthMac_op);
	ecnt_unregister_hook(&ecnt_hwnat_delDevBandWidthMac_op);
	
	return;
}
/*****************joyme bindwidth end******************************************/

/*****************traffic qos start*****************************/
int ppeSetMeterRate(uint index, uint rateLimit)
{
	uint tick, maxBkSize, regVal;

    if(rateLimit <= (METER_SCALE_M<<2))
        maxBkSize = METER_BUCKET_4KB;
    else if(rateLimit <= METER_SCALE_M<<4)
        maxBkSize = METER_BUCKET_16KB;
    else if(rateLimit <= (METER_SCALE_M<<7))
        maxBkSize = METER_BUCKET_32KB;
    else
        maxBkSize = METER_BUCKET_64KB;

	if (isEN7528)
	{
		/* for FTP 500Mbps ratelimit */
		if (rateLimit >= (METER_SCALE_M<<9))
		{
			RegModifyBits(PPE_MTR_BBS0,0x8000,0,16);
		}
		else
		{
			/* keep default value */
			RegModifyBits(PPE_MTR_BBS0,0x2600,0,16);
		}
	}
        
	if (rateLimit <= METER_SCALE_M) /* 0 ~ 1Mbps */
	{
		tick = METER_TICK_BYTE_1MS;
		rateLimit /= METER_SCALE_K<<4; /* unit: 16kbps */
	}
	else 
	{
		tick = METER_TICK_BYTE_125US;
		rateLimit /= METER_SCALE_K<<7; /* unit: 128kbps */
	}

	if (rateLimit > METER_TOKEN_RATE_BYTE_MAX)
		rateLimit = METER_TOKEN_RATE_BYTE_MAX;

	regVal = (tick << METER_REG_BYTE_OFF_TICK) |
			 (rateLimit << METER_REG_BYTE_OFF_RATE) |
			 (maxBkSize << METER_REG_BYTE_OFF_BUCK);	
	RegWrite(PPE_GRP_Meter_CNT(index), regVal);

	return 0;
}

int ppeSetMeterRatePps(uint index, uint rateLimit)
{
	uint tick, maxBkSize, regVal;

	if(rateLimit <= (METER_SCALE_1000 * 20))
		maxBkSize = METER_BUCKET_PKT_8K;
	else
		maxBkSize = METER_BUCKET_PKT_32K;

	if (rateLimit <= METER_PKT_RATE_BASE)				/* 0 ~ 2047pps */
	{
		tick = METER_TICK_PKT_1S;
		rateLimit /= (METER_SCALE_10 / 10);				/* unit: 1pps */
	}
	else if (rateLimit <= (METER_PKT_RATE_BASE*10))		/* 2048 ~ 2047*10pps */
	{
		tick = METER_TICK_PKT_100MS;
		rateLimit /= (METER_SCALE_100 / 10);			/* unit: 10pps */
	}
	else if (rateLimit <= (METER_PKT_RATE_BASE*100))	/* 2047*10 ~ 2047*100pps */
	{
		tick = METER_TICK_PKT_10MS;
		rateLimit /= (METER_SCALE_1000 / 10);			/* unit: 100pps */
	}
	else												/* 2047*100 ~ 2047*1000pps */
	{
		tick = METER_TICK_PKT_1MS;
		rateLimit /= (METER_SCALE_1000);				/* unit: 1000pps */
	}

	if (rateLimit > METER_TOKEN_RATE_PKT_MAX)
		rateLimit = METER_TOKEN_RATE_PKT_MAX;

	regVal = (tick << METER_REG_PKT_OFF_TICK) |
			 (rateLimit << METER_REG_PKT_OFF_RATE) |
			 (maxBkSize << METER_REG_PKT_OFF_BUCK) |
			 (METER_MODE_PACKET << METER_REG_PKT_OFF_MODE);	

	RegWrite(PPE_GRP_Meter_CNT(index), regVal);

	return 0;
}

/*reserve bit 0~3: index num , bit 4: enable or not*/
int ppemarkMeterIdxToFoe(struct sk_buff *skb,int meteridx)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	unsigned int foe_entry_idx=0 ;
	foe_entry_idx = FOE_ENTRY_NUM(skb);
	foe_entry = getPpeEntryLocal(foe_entry_idx, foe_entry);
	if(foe_entry == NULL)
		return -1;

	foe_ext[foe_entry_idx].classify_index = 0;

	if( (meteridx < 0) || ( meteridx > (RATELIMIT_BY_INTERFACE_MAX+RATELIMIT_BY_IP_MAX-1) ) )
	{
		return 0 ;
	}

	foe_ext[foe_entry_idx].classify_index = (meteridx & 0xf) | (1 << 4);

	if(SUPPORT_SRAM_ENTRY && (foe_entry_idx<hwnat_dram_idx_base)) {
		setPpeEntry(foe_entry_idx, foe_entry);
	}

	return 0 ;
}

int ppeSetFlowRatelimit(uint index, uint rateLimit)
{
	if(index >=  up_down_queue_num)
		return -1;
		
	upDownQueueRate[index] = rateLimit;

	ppeSetMeterRate(up_queue_cnt_start + index ,rateLimit) ;

	return 0;
}

int ppeGetFlowRatelimit(uint index, uint *rateLimit)
{
	if(index < up_down_queue_num)
		*rateLimit = upDownQueueRate[index];
	else{
		*rateLimit = 0;
		return -1;
	}

	return 0;
}

int ppeGetSoftCnt(uint index, uint *softCnt)
{
	if(index < up_down_queue_num)
		*softCnt = softCntByte[index];
	else{
		*softCnt = 0;
		return -1;
	}

	return 0;
}

int ppeGetSoftPkts(uint index, uint *softPkts)
{
	if(index < up_down_queue_num) {
		*softPkts = softCntPkts[index];
	} else {
		*softPkts = 0;
		return -1;
	}

	return 0;
}

int isUpStreamFlow(int magic, struct port_info* pinfo)
{
	if((FOE_MAGIC_EPON == magic) 
		|| (FOE_MAGIC_GPON == magic) 
		|| (magic == FOE_MAGIC_ATM) 
		|| (magic == FOE_MAGIC_PTM)
		|| (magic == FOE_MAGIC_L2TP_VPN_UPSTREAM)
		|| (magic == FOE_MAGIC_AE_WAN))
		return 1;
	else if(FOE_MAGIC_GE == magic){
		if(pinfo->eth_is_wan)
			return 1;
	}

	return 0;
}

int PpeInitFlowIdByMarkMode(void)
{
	if ( TCSUPPORT_CMCCV2_VAL )
	{
		FlowIdByMask_Mode = CMCCV2_MODE;
	}else if(TCSUPPORT_XPON_HAL_API_EXT_VAL || TCSUPPORT_XPON_HAL_API_VAL)
	{
		FlowIdByMask_Mode = XPON_HAL_MODE;
	}
	else if(TCSUPPORT_CMCC_VAL || (TCSUPPORT_CT_DS_LIMIT_VAL && TCSUPPORT_WAN_ETHER_VAL))
	{
		FlowIdByMask_Mode = CMCC_MODE;
	}
	if(TCSUPPORT_CSC_EEUROPE_VAL)
	{
		FlowIdByMask_Mode = CSC_EEUROPE_MODE;
	}
	
	return 0;
}

int PpeGetAcntFlowIdByMark(struct sk_buff * skb)
{
	int flowid = NO_TRAFFIC_QOS;
	
	if(FlowIdByMask_Mode == CMCCV2_MODE)
	{
		flowid = (skb->mark2 & 0x0f);
		if ( flowid > 0 && flowid <= 8 )
			return flowid - 1;

		return NO_TRAFFIC_QOS;
	}
	
	if(FlowIdByMask_Mode == XPON_HAL_MODE)
	{
		if(skb->pon_vlan_flag & 0x80)
			flowid = (skb->pon_vlan_flag& 0x70)>>4;
		else 
			return NO_TRAFFIC_QOS;
	}
	else if(FlowIdByMask_Mode == CMCC_MODE)
	{
		flowid = (skb->mark&0xf0)>>4;
		if(0 == flowid)
			return NO_TRAFFIC_QOS;	
		
		if(flowid>UP_QUEUE_NUM)
			flowid = up_queue_num - 1;
		else
			flowid = flowid - 1;
	}
	else if(FlowIdByMask_Mode == CSC_EEUROPE_MODE)
	{
		flowid = (skb->mark&flow_acnt_filter)>>flow_acnt_shift;
		if(0 == flowid)
			return NO_TRAFFIC_QOS;	
		
		if(flowid>up_down_queue_num)
			flowid = up_down_queue_num - 1;
		else
			flowid = flowid - 1;
	}
	return flowid;
}

int PpeGetMeterFlowIdByMark(struct sk_buff * skb)
{
	int flowid = NO_TRAFFIC_QOS;
	
	if(FlowIdByMask_Mode == CMCCV2_MODE )
	{
		flowid = (skb->mark2 & 0x0f);
		if ( flowid > 0 && flowid <= 8 )
			return flowid - 1;

		return NO_TRAFFIC_QOS;
	}
	
	if(FlowIdByMask_Mode == XPON_HAL_MODE)
	{
		if(skb->pon_vlan_flag & 0x80)
			flowid = (skb->pon_vlan_flag& 0x70)>>4;
		else 
			return NO_TRAFFIC_QOS;
	}
	else if(FlowIdByMask_Mode == CMCC_MODE)
	{
		flowid = (skb->mark&0xf0)>>4;
		if(0 == flowid)
			return NO_TRAFFIC_QOS;	
		
		if(flowid>UP_QUEUE_NUM)
			flowid = up_queue_num - 1;
		else
			flowid = flowid - 1;
	}
	else if(FlowIdByMask_Mode == CSC_EEUROPE_MODE)
	{
		flowid = (skb->mark&flow_meter_filter)>>flow_meter_shift;
		if(0 == flowid)
			return NO_TRAFFIC_QOS;	
		
		if(flowid>up_down_queue_num)
			flowid = up_down_queue_num - 1;
		else
			flowid = flowid - 1;
	}
	return flowid;
}

u_int16_t PpeGetAcntIdByMark(struct sk_buff * skb, int magic, struct port_info * pinfo)
{
	u_int16_t acntGrpIndex = HWNAT_DEFAULT_GRP_ID;
	int flowid;

	flowid = PpeGetAcntFlowIdByMark(skb);
	if(NO_TRAFFIC_QOS == flowid)
		return acntGrpIndex;

	if(isUpStreamFlow(magic, pinfo))
		acntGrpIndex = up_queue_cnt_start + flowid;
	else
		acntGrpIndex = down_queue_cnt_start + flowid;
	return acntGrpIndex;
}

u_int16_t PpeGetMeterIdByMark(struct sk_buff * skb, int magic,  struct port_info * pinfo)
{
	int flowid;
	u_int16_t idx = HWNAT_DEFAULT_GRP_ID;

	flowid = PpeGetMeterFlowIdByMark(skb);
	if(NO_TRAFFIC_QOS == flowid)
		return idx;

	if(isUpStreamFlow(magic, pinfo))
	{
		if(!upDownQueueRate[flowid])
			return HWNAT_DEFAULT_GRP_ID;
		idx = up_queue_cnt_start + flowid;
	}
	else
	{
		if(!upDownQueueRate[up_queue_num + flowid])
			return HWNAT_DEFAULT_GRP_ID;
		idx = down_queue_cnt_start + flowid;
	}

	return idx;
}

void PpeGetAcntMeterIdByFlowId(struct sk_buff * skb, int magic, struct port_info * pinfo, 
							u_int16_t *meterGrpId, u_int16_t *acntGrpIndex)
{
	if(HWNAT_DEFAULT_GRP_ID== *acntGrpIndex)
		*acntGrpIndex = PpeGetAcntIdByMark(skb, magic, pinfo);
	if(HWNAT_DEFAULT_GRP_ID== *meterGrpId)
		*meterGrpId = PpeGetMeterIdByMark(skb, magic, pinfo);

	return;
}

int getPpeMeterBindInfo(struct sk_buff * skb, u_int16_t *meterGrpId)
{
	u_int16_t meterId=0;
	
	meterId = (skb->mark >> LIMIT_GROUP_REMARK_OFFSET) & LIMIT_GROUP_REMARK_MASK;
	if (DebugLevel >= 3) {
		printk("getPpeMeterBindInfo:---skb->mark = 0x%x, get bind meterGroupId = %d\r\n", skb->mark, meterId);
	}

	if(meterId >= FWC_START_ACNTGRP_ID)
		*meterGrpId = meterId;
				
	return 0;
}

void ppeCalcFlowCnt(struct sk_buff * skb, int magic, struct port_info * pinfo)
{
	uint flowid = 0;

	if ((FOE_AI(skb) == HIT_BIND_KEEPALIVE_MC_NEW_HDR)
		|| (FOE_AI(skb) == HIT_BIND_KEEPALIVE_DUP_OLD_HDR)
		|| (FOE_AI(skb) == HIT_BIND_KEEPALIVE_UC_OLD_HDR))
	{
		return;
	}

	flowid = PpeGetAcntFlowIdByMark(skb);
	if(NO_TRAFFIC_QOS == flowid)
		return;

    if(TCSUPPORT_CSC_EEUROPE_VAL)
    {
        softCntByte[flowid] += skb->len;
    }
    else
    {
    	if(isUpStreamFlow(magic, pinfo))
    		softCntByte[flowid] += skb->len;
    	else
    		softCntByte[up_queue_num + flowid] += skb->len;
    }
	return;
}

static int queue_byte_counter_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[8];
	unsigned int flag = 1;
	
	if (count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d",&flag);

	if(0==flag)
		memset(queueRxBytes, 0x0, sizeof(queueRxBytes));
	
	return count;
}

static int queue_byte_counter_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int i = 0,index = 0;
	off_t pos=0, begin=0 ;
	unsigned int cnt32_lo=0, cnt32_hi=0;
		
	for (i = 0; i < UP_QUEUE_NUM; i++) 
	{
		if (isEN751221) {
			FE_API_GET_AC_GROUP_BYTE_CNT(UP_QUEUE_CNT_START + i, &cnt32_lo, &cnt32_hi);
			queueRxBytes[i] = ((((unsigned long long)cnt32_hi)<<32) + cnt32_lo);
		} else {
			queueRxBytes[i] += (((unsigned long long)IO_GREG(AC_BASE+4+((UP_QUEUE_CNT_START + i)<<4)))<<32)+IO_GREG(AC_BASE+((UP_QUEUE_CNT_START + i)<<4));
		}
		index += sprintf(buf+index, "up-stream queue%d: 0x%llx (Byte)\n", i+1, queueRxBytes[i]);

		CHK_BUF();
	}
	
	for (i = 0; i < UP_QUEUE_NUM; i++) 
	{
		if (isEN751221) {
			FE_API_GET_AC_GROUP_BYTE_CNT(DOWN_QUEUE_CNT_START + i, &cnt32_lo, &cnt32_hi);
			queueRxBytes[8+i] = ((((unsigned long long)cnt32_hi)<<32) + cnt32_lo);
		} else {
			queueRxBytes[8+i] += (((unsigned long long)IO_GREG(AC_BASE+4+((DOWN_QUEUE_CNT_START + i)<<4)))<<32)+IO_GREG(AC_BASE+((DOWN_QUEUE_CNT_START + i)<<4));
		}
		index += sprintf(buf+index, "down-stream queue%d: 0x%llx (Byte)\n", i+1, queueRxBytes[8+i]);

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

static int up_queue_ratelimit_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int i = 0,index = 0;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+index, "up-stream queue ratelimit:\n\n");

	for (i = 0; i < UP_QUEUE_NUM; i++) 
	{
		index += sprintf(buf+index, "queue%d: %d\n", i+1, upQueueRate[i]);

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

static int up_queue_ratelimit_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[128];
	int i = 0;
	unsigned int tick, tempRate, maxBkSize, regVal;

	if(count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d %d %d %d %d %d\n",
		&upQueueRate[0], &upQueueRate[1], &upQueueRate[2], &upQueueRate[3], &upQueueRate[4], &upQueueRate[5]);

	for(i=0;i<UP_QUEUE_NUM;i++)
	{
		tempRate = upQueueRate[i];	

        if (tempRate <= METER_SCALE_M) /* 0 ~ 1Mbps */
        {
            tick = METER_TICK_BYTE_1MS;
            tempRate /= METER_SCALE_K<<4; /* unit: 16kbps */
            maxBkSize = METER_BUCKET_4KB;
        }
        else /* 1Mbps ~ 2Gbps */
        {
            tick = METER_TICK_BYTE_125US;
            tempRate /= METER_SCALE_K<<7; /* unit: 128kbps */
            
            /* If rate is 128Mbps, meter will supply 2KByte token in 1 tick,
             * therefore, bucket size with 4KByte should be OK */
            if (METER_SCALE_M < tempRate && tempRate <= (METER_SCALE_M<<7)) /* 1M ~ 128M bps */
                maxBkSize = METER_BUCKET_4KB;
            else if ((METER_SCALE_M<<7) < tempRate && tempRate <= (METER_SCALE_M<<9)) /* 128M ~ 512M bps */
                maxBkSize = METER_BUCKET_16KB;
            else if ((METER_SCALE_M<<9) < tempRate && tempRate <= METER_SCALE_G) /* 512M ~ 1G bps */
                maxBkSize = METER_BUCKET_32KB;
            else /* 1G ~ 2G bps */
                maxBkSize = METER_BUCKET_64KB;
        }

        if (tempRate > METER_TOKEN_RATE_BYTE_MAX)
            tempRate = METER_TOKEN_RATE_BYTE_MAX;

        regVal = (METER_MODE_BYTE << METER_REG_BYTE_OFF_MODE) |
                 (tick << METER_REG_BYTE_OFF_TICK) |
                 (tempRate << METER_REG_BYTE_OFF_RATE) |
                 (maxBkSize << METER_REG_BYTE_OFF_BUCK);    
		IO_SREG(PPE_GRP_Meter_CNT(UP_QUEUE_CNT_START + i), regVal);
	}    

	PpeCleanTableHandler();

	return count;
}

int PpeGetMeterIdforDslimit(struct sk_buff * skb)
{
	int idx = HWNAT_DEFAULT_GRP_ID;
	int flowid = 0;

	if(wan_port_id!=-1 && dslimit_enable)
		flowid = PpeGetMeterFlowIdByMark(skb);

	if((flowid >= 0) &&(upDownQueueRate[flowid]))
		idx = (UP_QUEUE_CNT_START + flowid);

	return idx;
}
int PpeGetAcntIdforDslimit(struct sk_buff * skb)
{
	int idx = HWNAT_DEFAULT_GRP_ID;
	int flowid = 0;

	if(wan_port_id!=-1 && dslimit_enable)
		flowid = PpeGetAcntFlowIdByMark(skb);

    if(flowid >= 0)
	    idx = (UP_QUEUE_CNT_START + flowid); 

	return idx;
}

int dslimit_UpTrtcmEnable(u8 enable)
{ 
	if( (wan_port_id != -1) && TCSUPPORT_WAN_ETHER_VAL )
		dslimit_enable = enable;
	return 0;
}
/****************traffic qos end**********************************/

/**************hal_api_ext start********************************/
int hwnat_is_tpid(unsigned short tpid)
{
	if (tpid == htons(0x8100) || tpid == htons(0x88a8))
		return 1;

	return 0;
}

int hwnat_lan_wan_rx(unsigned int idx,struct sk_buff* sk)
{
	unsigned int info;
	struct vlan_ethhdr * hdr;
	if (idx >= FOE_4TB_SIZ_TOTAL|| foe_ext == NULL)
		return 0;

	if ((strncmp(sk->dev->name,"eth",3) == 0) && (FOE_SP(sk) >= SP_GSW_PORT0)){
		info = FOE_SP(sk) & 07;
		foe_ext[idx].rx_info = (0xbeaf << 16) | info;
	}else if ((strncmp(sk->dev->name,"pon",3) == 0) && (FOE_SP(sk) == SP_GDMA2)){
		hdr = vlan_eth_hdr(sk);
		info = hwnat_is_tpid(hdr->h_vlan_proto) ? hdr->h_vlan_TCI : 0;
		foe_ext[idx].rx_info = (0xbeaf << 16) | ( info & 0xfff) ;
	}
	
	return 0;
}

void hwnat_offload_mark_lan_rx(unsigned int idx, unsigned short stag)
{
    int i = 0;
    int dport = 0;
    
    if (idx >= FOE_4TB_SIZ_TOTAL || foe_ext == NULL)
    {
        return ;
    }

    dport = stag & 0x1f;
    for(i = 0; i < 6; i++)
    {
        if(dport & 1<<i)
        {
            foe_ext[idx].rx_info = (0xbeaf << 16) | ( i );
            break;
        }
    }
    
	return ;
}

static int hwnat_del_all_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int i = 0;

	printk("HWNAT Delete Enable: %d \n",hwnat_del_en);
	for(i = 0; i< FOE_4TB_SIZ_TOTAL; i++)
	{
		if (foe_ext[i].rx_info != 0)
			printk("Entry(%d):%08x \n",i,foe_ext[i].rx_info);
	}

	return 0;	
}

static int hwnat_del_all_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[9];

	if (count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d", &hwnat_del_en);

	if (hwnat_del_en == 0)
		memset(foe_ext,0,FOE_4TB_SIZ_TOTAL * sizeof(struct FoeEntryExt));
	
	return count;
}

/*ratelimit info display*/
void ppeTrafficRatelimitInfoDisplay(void)
{
	int i = 0 ;
	int j = 0 ;

	printk("TrafficRatelimitInfoDisplay\n");

	printk("\nInterface Info: %s\n", ratelimitByItf_en? "ENABLE":"DISABLE");
	printk("Interface-Name\tActive\tRate\n");
	for( i  = 0 ; i < 8 ; i++ )
	{
		if( i < 4 )
		{
			printk("LAN%d\t\t%s\t%d Kbps\n\r",
				(i+1) ,
				RateLimitItfInfo[i].enable?"Yes":"No",
				RateLimitItfInfo[i].rate);
		}
		else
		{
			printk("SSID%d\t\t%s\t%d Kbps\n\r",
				(i-3) ,
				RateLimitItfInfo[i].enable?"Yes":"No",
				RateLimitItfInfo[i].rate);
		}
	}

	printk("\nIP range Info: %s\n", ratelimitByIP_en? "ENABLE":"DISABLE");
	printk("No\tActive\tRate\tMode\tIP range\n");
	for( i  = 0 ; i < 6 ; i++ )
	{
		printk("%d\t%s\t%d Kbps\t%s\t",
				(i+1) ,
				RateLimitIPInfo[i].enable?"Yes":"No",
				RateLimitIPInfo[i].rate,
				RateLimitIPInfo[i].ip_version?"IPv6":"IPv4");
		if( IP_VERSION_V6 == RateLimitIPInfo[i].ip_version )/*IPv6*/
		{
			for( j = 0 ; j < 16 ; )
			{
				printk("%02X%02X",RateLimitIPInfo[i].v6_pair_l[j],RateLimitIPInfo[i].v6_pair_l[j+1]);
				j = j + 2 ;
				if( j < 16 )
					printk(":");
			}
			printk("~");
			for( j = 0 ; j < 16 ; )
			{
				printk("%02X%02X",RateLimitIPInfo[i].v6_pair_h[j],RateLimitIPInfo[i].v6_pair_h[j+1]);
				j = j + 2 ;
				if( j < 16 )
					printk(":");
			}
		}
		else if( IP_VERSION_V4 == RateLimitIPInfo[i].ip_version )/*IPv4*/
		{
			for( j = 0 ; j < 4 ; j ++ )
			{
				printk("%d",RateLimitIPInfo[i].v4_pair_l[j]);
				if( j < 3 )
					printk(".");
			}
			printk("~");
			for( j = 0 ; j < 4 ; j ++ )
			{
				printk("%d",RateLimitIPInfo[i].v4_pair_h[j]);
				if( j < 3 )
					printk(".");
			}
		}
		printk("\n\r");
	}

}

static int hwnat_ratelimit_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	ppeTrafficRatelimitInfoDisplay();

	return 0;	
}

/*echo mode(0:interface,1:ip range) index(1~8 or 1~6) enable(0/1) rate(Kbps) ip_mode(0:ipv4 1:ipv6) ip_range*/
static int hwnat_ratelimit_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	int i=0 ;
	
	char get_buf[120];
	char ip_buf[100];
	unsigned int mode = 0;
	unsigned int index = 0;
	unsigned int enable = 0;
	unsigned int rate = 0;
	unsigned int ip_version = 0 ;

	int ip_tmp[16] = {0} ;
	
	if (count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d",&mode);
	if( 0 == mode )/*set interface ratelimit*/
	{
		sscanf(get_buf, "%d %d %d %d",&mode,&index,&enable,&rate);
		if( index <= 8 && index > 0 )
		{
			RateLimitItfInfo[index-1].enable = enable ;
			RateLimitItfInfo[index-1].rate = rate ;
			ppeSetMeterRate( (METER_BEGIN_INDEX_FOR_RATELIMIT + index - 1) , (rate<<10) ) ;
		}
	}
	else if( 1 == mode )/*set ip ratelimit*/
	{
		sscanf(get_buf, "%d %d %d %d %d",&mode,&index,&enable,&rate,&ip_version);
		if( index <= 6 && index > 0 )
		{
			sscanf(get_buf, "%d %d %d %d %d %s",&mode,&index,&enable,&rate,&ip_version,ip_buf);
			ip_buf[99] = '\0';
				
			RateLimitIPInfo[index-1].enable = enable ;
			RateLimitIPInfo[index-1].rate = rate ;
			RateLimitIPInfo[index-1].ip_version = ip_version ;
			if( IP_VERSION_V4 == ip_version )/*ipv4*/
			{
				sscanf(ip_buf, "%d.%d.%d.%d~%d.%d.%d.%d" ,&ip_tmp[0],&ip_tmp[1],&ip_tmp[2],&ip_tmp[3],
					&ip_tmp[4],&ip_tmp[5],&ip_tmp[6],&ip_tmp[7]);

				for( i = 0 ; i < 4 ; i++ )
				{
					RateLimitIPInfo[index-1].v4_pair_l[i] = (ip_tmp[i]) & 0xff ;
					RateLimitIPInfo[index-1].v4_pair_h[i] = (ip_tmp[i+4]) & 0xff ;
				}
			}
			else if( IP_VERSION_V6 == ip_version )/*ipv6*/
			{
				sscanf(ip_buf, "%x:%x:%x:%x:%x:%x:%x:%x~%x:%x:%x:%x:%x:%x:%x:%x" ,&ip_tmp[0],&ip_tmp[1],&ip_tmp[2],&ip_tmp[3],
					&ip_tmp[4],&ip_tmp[5],&ip_tmp[6],&ip_tmp[7],&ip_tmp[8],&ip_tmp[9],&ip_tmp[10],&ip_tmp[11],
					&ip_tmp[12],&ip_tmp[13],&ip_tmp[14],&ip_tmp[15]);
				for( i = 0 ; i < 8 ; i++ )
				{
					RateLimitIPInfo[index-1].v6_pair_l[i*2] = (ip_tmp[i] >> 8) & 0xff ;
					RateLimitIPInfo[index-1].v6_pair_l[i*2+1] = (ip_tmp[i]) & 0xff ;
					RateLimitIPInfo[index-1].v6_pair_h[i*2] = (ip_tmp[i+8] >> 8) & 0xff ;
					RateLimitIPInfo[index-1].v6_pair_h[i*2+1] = (ip_tmp[i+8]) & 0xff ;
				}
			}

			ppeSetMeterRate( (METER_BEGIN_INDEX_FOR_RATELIMIT + RATELIMIT_BY_INTERFACE_MAX + index - 1 ) , (rate<<10) ) ;
		}
	}
	else if( 2 == mode )/*set function enable or not*/
	{
		sscanf(get_buf, "%d %d %d",&mode,&index,&enable);
		if( index == 0 )/*set ratelimit mode : by user interface*/
			ratelimitByItf_en = enable ;
		else if( index == 1 )/*set ratelimit mode : by src ip*/
			ratelimitByIP_en = enable ;
	}
	else
	{
		printk("help:\n");
		printk("0(mode:set interface info) index(LAN1~LAN4:1~4,SSID1~SSID4:5~8) enable(0/1) rate(unit:kbps)\n\r");
		printk("1(mode:set IP info) index(1~6) enable(0/1) rate(unit:kbps) ip_version(0:ipv4,1:ipv6)\n");
		printk("\tip_range(X.X.X.X~X.X.X.X/xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx~xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx)\n\r");
		printk("2(mode:set enable) func(0:interface ratelimit 1:Ip ratelimit) enable(0/1)\n\r");
	}

	PpeCleanTableHandler();

	return count;
}

static int hwnat_flowid_mode_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{

	printk("%d:  ", FlowIdByMask_Mode);

	switch(FlowIdByMask_Mode){
		case NOT_WORK_MODE:
			printk("Not Work mode\n");
			break;
		case CMCCV2_MODE:
			printk("CMCCV2 mode\n");
			break;
		case XPON_HAL_MODE:
			printk("XPON_HAL mode\n");
			break;
		case CMCC_MODE:
			printk("CMCC mode\n");
			break;
		case CSC_EEUROPE_MODE:
			printk("CSC_EEUROPE mode\n");
			break;
		default:
			printk("unknown mode\n");
			break;
	}

	return 0;	
}

static int hwnat_flowid_mode_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[8];
	unsigned int flag = 0;
	
	if (count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d", &flag);

	FlowIdByMask_Mode = flag;
	
	printk("FlowIdByMask_Mode: %d \n", FlowIdByMask_Mode);
	
	return count;
}

u_int16_t PpeGetLanIpAcntId(struct FoeEntry* foe_entry, struct sk_buff* skb, struct port_info* pinfo, int magic)
{
	uint i, offset, idx, mask;
	uint acntId = HWNAT_DEFAULT_GRP_ID;
	uint32_t ipaddr;

	if(!br_ip_info.brlan_exist)
		return HWNAT_DEFAULT_GRP_ID;
	
	if(!br_ip_info.ip_exist)
	{
		PpeGetBrIp();
	}

	if(!br_ip_info.ip_exist)
		return HWNAT_DEFAULT_GRP_ID;

	if(isUpStreamFlow(magic, pinfo))
	{
		ipaddr = foe_entry->ipv4_hnapt.sip;
		offset = 0;
	}
	else
	{
		ipaddr = foe_entry->ipv4_hnapt.new_dip;
		offset = 1;
	}
	mask = br_ip_info.mask;

	if((ipaddr&mask) != (br_ip_info.ipddr&mask))
		return HWNAT_DEFAULT_GRP_ID;

	for(i=0;i<lanIpMaxAcNum;i++)
	{
		if(ipaddr == lanip_account.acnt[i].ipaddr)
		{
			return (LANIP_CNT_START + (i*2) + offset); 
		}
	}

	idx = lanip_account.currentIdx%lanIpMaxAcNum;

	lanip_account.acnt[idx].ipaddr = ipaddr;
	lanip_account.acnt[idx].up_cnt = 0;
	lanip_account.acnt[idx].down_cnt = 0;
	lanip_account.currentIdx = (idx + 1)%lanIpMaxAcNum;

	acntId = LANIP_CNT_START + (idx*2) + offset;

	return acntId;
}

void PpeSetLanIpAcnt(struct FoeEntry * foe_entry, struct sk_buff * skb, struct port_info* pinfo, int magic)
{
	u_int16_t acntGrpIndex = HWNAT_DEFAULT_GRP_ID;

	if (!IS_IPV4_GRP(foe_entry))		/* Only IPV4&Route mode */
		return;
	
	acntGrpIndex = PpeGetLanIpAcntId(foe_entry, skb, pinfo, magic);
	if(acntGrpIndex != HWNAT_DEFAULT_GRP_ID)
	{
	    foe_entry->ipv4_hnapt.port_ag = acntGrpIndex;
	}

	return;
}

void PpeAWifiClearAWifiInfo(void)
{
	memset(&lanip_account, 0x0, sizeof(lanip_account));
	memset(&br_ip_info, 0x0, sizeof(br_ip_info));
}

void PpeGetBrIp(void)
{
	struct net_device *dev;
	struct in_device *in_dev;
	struct in_ifaddr *ifa;

	dev = dev_get_by_name(&init_net,AWIFI_ITF);
	if(dev)
	{
		in_dev = __in_dev_get_rcu(dev);
		if (!in_dev)
		{
			return;
		}

        ifa = in_dev->ifa_list;
		if (ifa)
		{
			br_ip_info.ipddr = ifa->ifa_local;
			br_ip_info.mask = ifa->ifa_mask;
			br_ip_info.ip_exist = 1;
		}
		
		dev_put(dev);
	}
	
	return;
}

void PpeGetLanIpCnt(uint32_t ipaddr, unsigned long long* cnt_byte_up, unsigned long long* cnt_byte_down)
{
	uint32_t i, acntUpId = 0, acntDownId = 0, cnt_hi, cnt_lo;
	unsigned long long unit = 0x100000000;

	for(i=0;i<lanIpMaxAcNum;i++)
	{
		if(ipaddr == lanip_account.acnt[i].ipaddr)
		{
			acntUpId = LANIP_CNT_START + (i*2);
			acntDownId = LANIP_CNT_START + (i*2) + 1;
			break;
		}
	}

	if(lanIpMaxAcNum <= i)
	{
		*cnt_byte_up = 0;
		*cnt_byte_down = 0;
		return;
	}

	FE_API_GET_AC_GROUP_BYTE_CNT(acntUpId, &cnt_lo, &cnt_hi);
	lanip_account.acnt[i].up_cnt = (unit*cnt_hi) + cnt_lo;
	*cnt_byte_up = lanip_account.acnt[i].up_cnt;

	FE_API_GET_AC_GROUP_BYTE_CNT(acntDownId, &cnt_lo, &cnt_hi);
	lanip_account.acnt[i].down_cnt = (unit*cnt_hi) + cnt_lo;
	*cnt_byte_down = lanip_account.acnt[i].down_cnt;

	return;
}

/**************hal_api_ext end*********************************/

/**************cds gemport ratelimit start************************/
u_int16_t PpeGetMeterIdByMapping(struct sk_buff * skb, int magic,  struct port_info * pinfo)
{
	__u32 pon_mark, ts_enable_mark;
	__u8 tse, tsid;
	__u8 meterStart = 0;
	__u8 meterMaxNum = 0;
	

	pon_mark = SKB_GET_PON_MARK(skb);

	if(isUpStreamFlow(magic, pinfo))
	{
	    ts_enable_mark = QOS_TSE_MARK;
	    meterStart = upGemPortMeterStart;
	    meterMaxNum = upGemPortMeterNum;
	}
	else
	{
	    ts_enable_mark = DS_TRTCM_ENABLE_MARK;
	    meterStart = downGemPortMeterStart;
	    meterMaxNum = downGemPortMeterNum;
	}

    tse = (pon_mark & ts_enable_mark) ? 1 : 0 ;
    if(!tse)
        return HWNAT_DEFAULT_GRP_ID;
    
    tsid = pon_mark & QOS_TSID_MARK;  
    if(tsid >= meterMaxNum)
        return HWNAT_DEFAULT_GRP_ID;

    return meterStart + tsid;
}
/**************cds gemport ratelimit end*************************/

/**************change priority for thunder& mul start***************/
unsigned char Ppe_Set_Mul_High_Priority(unsigned int data)
{
	g_mul_high_priority = data;
	return g_mul_high_priority;
}

void VIP_expire_delete(TIMER_FUN_PAAM data)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	struct Ppe_VIP_t *Ppe_VIP_p = (struct Ppe_VIP_t *)data;
#else
	struct Ppe_VIP_t *Ppe_VIP_p = from_timer(Ppe_VIP_p, data, VIP_expire_timer);
#endif
	spin_lock(&vip_check_lock);

	del_timer(&Ppe_VIP_p->mul_vip_timer);
	del_timer(&Ppe_VIP_p->VIP_expire_timer);
	list_del(&Ppe_VIP_p->list);
	kfree(Ppe_VIP_p);
	
	spin_unlock(&vip_check_lock);
}

void Ppe_Vip_Expire_Check(TIMER_FUN_PAAM data)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	struct Ppe_VIP_t *Ppe_VIP_p = (struct Ppe_VIP_t *)data;
#else
	struct Ppe_VIP_t *Ppe_VIP_p = from_timer(Ppe_VIP_p, data, mul_vip_timer);
#endif
	struct FoeEntry *new_foe_entry;
	struct FoeEntry *new_foe_entry_plus;

	new_foe_entry= (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + Ppe_VIP_p->hash_index * SIZE_OF_FOE_ENTRY);
	new_foe_entry_plus=(struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + (Ppe_VIP_p->hash_index - 1) * SIZE_OF_FOE_ENTRY);
	/*repeat 5 times*/
	if(Ppe_VIP_p->count < 5)
	{		
		if(Ppe_VIP_p->unbind_sip == new_foe_entry->ipv4_hnapt.sip|| Ppe_VIP_p->unbind_sip == new_foe_entry_plus->ipv4_hnapt.sip)
		{		
			if(0 == Ppe_VIP_p->tcp_flag)
			{
				FE_API_DEL_VIP_UDP(Ppe_VIP_p->sport, Ppe_VIP_p->dport, 7); 
			}
			else if(1 == Ppe_VIP_p->tcp_flag)
			{
				FE_API_DEL_VIP_TCP(Ppe_VIP_p->sport, Ppe_VIP_p->dport, 7);
			}
			
			spin_lock(&vip_check_lock);
			del_timer(&Ppe_VIP_p->mul_vip_timer);			
			list_del(&Ppe_VIP_p->list);
			kfree(Ppe_VIP_p);			
			spin_unlock(&vip_check_lock);
			
		}
		else
		{
			spin_lock(&vip_check_lock);
			Ppe_VIP_p->mul_vip_timer.expires  = jiffies + msecs_to_jiffies(10);
			Ppe_VIP_p->count++;
			mod_timer(&Ppe_VIP_p->mul_vip_timer, Ppe_VIP_p->mul_vip_timer.expires);
			spin_unlock(&vip_check_lock);
		}
	}
	else
	{	
		if(0 == Ppe_VIP_p->tcp_flag)
		{
			FE_API_DEL_VIP_UDP(Ppe_VIP_p->sport, Ppe_VIP_p->dport, 7); 
		}
		else if(1 == Ppe_VIP_p->tcp_flag)
		{
			FE_API_DEL_VIP_TCP(Ppe_VIP_p->sport, Ppe_VIP_p->dport, 7);
		}
		spin_lock(&vip_check_lock);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
		setup_timer(&Ppe_VIP_p->VIP_expire_timer, &VIP_expire_delete, (unsigned long)Ppe_VIP_p);
#else
		timer_setup(&Ppe_VIP_p->VIP_expire_timer, VIP_expire_delete, 0);
#endif
		Ppe_VIP_p->VIP_expire_timer.expires  = jiffies + 30*HZ;
		add_timer(&Ppe_VIP_p->VIP_expire_timer);
		
		spin_unlock(&vip_check_lock);		
	}
	
}



void Ppe_Vip_Tmp_Handler(struct sk_buff * skb)
{ 
    int hash_index = 0;
	unsigned char tcp_flag = 0;
	unsigned short sport = 0;
	unsigned short dport = 0;
	unsigned int unbindsip = 0;
	int vlan_tag_num =0;

	struct FoeEntry *old_foe_entry;
	struct FoeEntry *new_foe_entry;
	struct FoeEntry *new_foe_entry_plus;

	struct sk_buff * skb_tmp_data = skb;	
	struct Ppe_VIP_t *Vip_local_p = NULL;
	struct iphdr *iph = NULL;
	
	skb_tmp_data->data -= ETH_HLEN;
	vlan_tag_num = ppe_get_vlan_tag_num(skb_tmp_data);
	skb_tmp_data->data += ETH_HLEN;
	if(vlan_tag_num > 0)
	{
		skb_tmp_data ->data += VLAN_HLEN;
		if(vlan_tag_num > 1)
		{
			skb_tmp_data ->data += VLAN_HLEN;
		}
		if(vlan_tag_num > 2)
		{
			printk("Only support 2layer vlan!\n");
			return;
		}	
	}
	
	iph = (struct iphdr *)(skb_tmp_data ->data);
	if (NULL == iph)
		return;
	unbindsip = iph->saddr;
	if(IPPROTO_UDP != iph->protocol && IPPROTO_TCP != iph->protocol)
		return;
	
    /*no need to use 5 tuple to caculate conflict hash index*/
	hash_index = FOE_ENTRY_NUM(skb);
	if(0 == hash_index%2)
		return;
	
	old_foe_entry = (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + hash_index * SIZE_OF_FOE_ENTRY);
	if(old_foe_entry->ipv4_hnapt.sip == unbindsip)
	{
		return;
	}

	spin_lock(&vip_check_lock);	
	list_for_each_entry(Vip_local_p,&Ppe_VIP_s.list, list)
	{
		if(Vip_local_p -> hash_index == hash_index)
		{
			spin_unlock(&vip_check_lock);
			return;
		}
			
	}
	spin_unlock(&vip_check_lock);
	
	/*only be used in TCP/UDP  format*/
	if((!old_foe_entry->ipv4_hnapt.dmac_hi[0]&0x01) && 0 == old_foe_entry->bfib1.pkt_type )
	{
		if(1 == old_foe_entry->ipv4_hnapt.bfib1.udp)
		{
			tcp_flag = 0;
			sport = old_foe_entry->ipv4_hnapt.sport;
			dport = old_foe_entry->ipv4_hnapt.dport;
			FE_API_ADD_VIP_UDP(old_foe_entry->ipv4_hnapt.sport, old_foe_entry->ipv4_hnapt.dport, 7); 
		}
		else if(0 == old_foe_entry->ipv4_hnapt.bfib1.udp)
		{
			tcp_flag = 1;			
			sport = old_foe_entry->ipv4_hnapt.sport;
			dport = old_foe_entry->ipv4_hnapt.dport;
			FE_API_ADD_VIP_TCP(old_foe_entry->ipv4_hnapt.sport, old_foe_entry->ipv4_hnapt.dport, 7);
		}
		hwnat_delete_foe_entry(hash_index);
		
		new_foe_entry= (struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + hash_index * SIZE_OF_FOE_ENTRY);
		new_foe_entry_plus=(struct FoeEntry*)(((unsigned char*)&PpeFoeBase[0]) + (hash_index -1) * SIZE_OF_FOE_ENTRY);
		
		if(unbindsip == new_foe_entry->ipv4_hnapt.sip || unbindsip == new_foe_entry_plus->ipv4_hnapt.sip)
		{		
			if(0 == tcp_flag)
			{
				 FE_API_DEL_VIP_UDP(sport, dport, 7); 				
			}
			else if(1 ==  tcp_flag)
			{
				 FE_API_DEL_VIP_TCP(sport, dport, 7);				
			}
		}
		else
		{
			Vip_local_p = (struct Ppe_VIP_t*)kmalloc(sizeof(struct Ppe_VIP_t), GFP_ATOMIC);
			if(NULL == Vip_local_p)
			{
				printk("Not enough space!\n");
				return;
			}			
			spin_lock(&vip_check_lock);
			Vip_local_p->hash_index = hash_index;
			Vip_local_p->count    = 0;
			Vip_local_p->unbind_sip		= unbindsip;
			Vip_local_p->sport    = sport;
			Vip_local_p->dport    = dport;
			Vip_local_p->tcp_flag = tcp_flag;

			list_add_tail(&Vip_local_p->list, &Ppe_VIP_s.list);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
			setup_timer(&Vip_local_p->mul_vip_timer, &Ppe_Vip_Expire_Check, (unsigned long)Vip_local_p);
#else
			timer_setup(&Vip_local_p->mul_vip_timer, Ppe_Vip_Expire_Check, 0);
#endif
			Vip_local_p->mul_vip_timer.expires  = jiffies + msecs_to_jiffies(10);
			add_timer(&Vip_local_p->mul_vip_timer);
			spin_unlock(&vip_check_lock);
		}

	}
	return;
	
}

static int hwnat_meter_mode_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	off_t begin=0 ;

	index += sprintf(buf+index, "%d:  ", meter_mode);

	switch(meter_mode){
		case METER_MODE_DEV_MAC:
			index += sprintf(buf+index, "dev mac mode\n");
			break;
		case METER_MODE_WAN:
			index += sprintf(buf+index, "wan mode\n");
			break;
		case METER_MODE_NULL:
			index += sprintf(buf+index, "Null mode\n");
			break;
		default:
			index += sprintf(buf+index, "unknown mode\n");
			break;
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

static int hwnat_meter_mode_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[8];
	unsigned int flag = 0;
	
	if (count > (sizeof(get_buf) - 1))
		return -EINVAL ;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, count))
		return -EFAULT;

	get_buf[count] = '\0';

	sscanf(get_buf, "%d", &flag);

	meter_mode = flag;
	
	return count;
}

static int hwnat_wan_ratelimit_use_pps_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	printk("wanRatePpsMtuDefault: %d\n\r", wanRatePpsMtuDefault);
	printk("wanRatePpsThreshold: %dMbps\n\r", wanRatePpsThreshold);

	printk("wanRatePpsCurWanType: %d, %s\n\r", wanRatePpsCurWanType, (wanRatePpsCurWanType == 1)?"route":"bridge");
	printk("wanRatePpsCurWanMtu: %d\n\r", wanRatePpsCurWanMtu);
	
	return 0;	
}

static int hwnat_wan_ratelimit_use_pps_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char get_buf[32];
	char cmd_buf[16];
	unsigned int value = 0;
	int len = 0;
	
	if (count > sizeof(get_buf) - 1)
		len = sizeof(get_buf) - 1;
	else 
		len = count;
	
	memset(get_buf, 0, sizeof(get_buf));
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;

	get_buf[len] = '\0';

	sscanf(get_buf, "%s %d", cmd_buf, &value);

	if(!strcmp(cmd_buf, "mtu")) {
		wanRatePpsMtuDefault = value;
	} else if (!strcmp(cmd_buf, "threshold")) {
		wanRatePpsThreshold = value;
	} else {
		printk("Help: \n");
		printk("echo mtu <value> > /proc/tc3162/wan_ratelimit_use_pps\n");
		printk("echo threshold <value>(unit:Mbps) > /proc/tc3162/wan_ratelimit_use_pps\n\r");
	}
	
	return len;
}

static void PpeSetWanMeterId(struct sk_buff *skb, uint8_t wan_index)
{
	if(meter_mode != METER_MODE_WAN)
		return;
	
	if(wan_index > 7)
		return; 

	FOE_WAN_METER_EN(skb) = PPE_ENABLE;
	FOE_WAN_IDX(skb) = wan_index;
		
	return;
}

void PpeGetWanMeterId(struct sk_buff * skb, u_int16_t *meterGrpId, int magic, struct port_info * pinfo)
{
	u_int16_t wan_meter_index = 0;

	if(meter_mode != METER_MODE_WAN)
		return;
 
	if(FOE_WAN_METER_EN(skb) != PPE_ENABLE)
		return;

	if(isUpStreamFlow(magic, pinfo))
		wan_meter_index = FOE_WAN_IDX(skb)*2;
	else
		wan_meter_index = FOE_WAN_IDX(skb)*2 + 1;

	if(wan_meter_index >= 16)
		return;

	if(0 == wanRatelimit[wan_meter_index])
		return;
	
	*meterGrpId = WAN_METER_START + wan_meter_index;

	return;
}

int ppeSetWanRatelimitThreshold(uint threshold)
{
	wanRatePpsThreshold = threshold;

	return 0;
}

int ppeSetWanRatelimitMtu(uint8_t wan_type, uint wan_mtu)
{
	wanRatePpsCurWanType = wan_type;
	wanRatePpsCurWanMtu = wan_mtu;

	return 0;
}

int ppeSetWanRatelimit(uint index, uint rateLimit, uint8_t dir)
{
	uint ratePpsVal = 0;
	
	if(meter_mode != METER_MODE_WAN)
	{
		printk("meter_mode %d not match\n", meter_mode);
		return -1;
	}

	if((UPSTREAM_ONLY != dir)&&(DOWNSTREAM_ONLY != dir))
	{	
		printk("dir %d error\n", dir);
		return -1;
	}
	
	wanRatelimit[index*2+dir] = rateLimit;

	if(isEN7526c && (rateLimit > (wanRatePpsThreshold<<10))) /* if bigger rate value, use packet ratelimit mode for more bucket size */
	{
		if(wanRatePpsCurWanType == 1)	/* route wan, use config MTU */
		{
			ratePpsVal = (rateLimit<<10) / 8 / (wanRatePpsCurWanMtu+14+24) ;
		}
		else							/* bridge wan, use default MTU */
		{
			ratePpsVal = (rateLimit<<10) / 8 / (wanRatePpsMtuDefault+14+24) ;
		}

		ppeSetMeterRatePps(WAN_METER_START + index*2+dir, ratePpsVal);
	}
	else
	{
		ppeSetMeterRate(WAN_METER_START + index*2+dir ,rateLimit<<10);
	}

	return 0;
}

/**************change priority for thunder& mul end ***************/
void ecnt_hwnat_pppoe_len_checksum(void)
{
/* enable pppoe len cal use ipheader len */
	if(SUPPORT_PPPOE_LEN_CAL_USE_IP_LEN){
		RegModifyBits(PPE_GLO_CFG, 1, 21, 1);
		RegModifyBits(PPE_GLO_CFG, 1, 22, 1);			
		if(PPE2_SUPPORTED){
			RegModifyBits(PPE2_GLO_CFG, 1, 21, 1);
			RegModifyBits(PPE2_GLO_CFG, 1, 22, 1);
		}
	}
	return;
}


int hwnat_clean_entry_by_src_mac(const unsigned char* mac)
{
	struct FoeEntry *foe_entry ;
	unsigned int idx,type;
    unsigned char smac[ETH_ALEN];
	int index = 0;
	int i = 0;

	memset(smac, 0, sizeof(smac));

	if(mac==NULL)
		return 0;
	for (idx = 0; idx < FOE_4TB_SIZ_TOTAL; idx++)
	{
		if((SUPPORT_SRAM_MAX_512) && (idx >= FOE_4TB_SIZ_SRAM) && (idx < hwnat_dram_idx_base))
			continue;
		
		//foe_entry = &PpeFoeBase[idx];
		foe_entry = getPpeEntry(idx);
		if(foe_entry == NULL)
			return -1;

		if (foe_entry->bfib1.state != BIND)
			continue;
		
		type = foe_entry->bfib1.pkt_type;
		switch (type)
		{
			case IPV4_HNAPT:
			case IPV4_HNAT:
				FoeGetEntryMac(smac, (foe_entry->ipv4_hnapt.smac_hi), (foe_entry->ipv4_hnapt.smac_lo));	
				break;
			case L2_BRIDGE:
				FoeGetEntryMac(smac, (foe_entry->l2_bridge32.in_smac_hi), (foe_entry->l2_bridge32.in_smac_lo));
				break;
			case IPV4_DSLITE:
				FoeGetEntryMac(smac, (foe_entry->ipv4_dslite.smac_hi), (foe_entry->ipv4_dslite.smac_lo));
				break;
			case IPV6_3T_ROUTE:
			case IPV6_5T_ROUTE:
			case IPV6_6RD:
			{
				if( PPE_SHNK_TABLE_SUPPORT && (PPE_FOE_SIZE_BYTE_MODE == IS_64_BYTE_MODE) && !IS_IPV4_DSLITE(foe_entry) ) {
					index =	get_ppe_entry_smac_index(foe_entry, IPV6_5T_ROUTE);
					if(index== 0x10) {
						continue;
					} else {
						for(i = 0; i < UPDMEM_SMAC_CNT; i++){
							if( index < UPDMEM_NUM )
						    	smac[i] = shnkTbl[index].smac[i];
						}
					}
				}
			}
			break;

			case IPV6_MC_ROUTE:
				FoeGetEntryMac(smac, (foe_entry->ipv6_1t_route.smac_hi), (foe_entry->ipv6_1t_route.smac_lo));					
				break;
			default:
				continue;
		}
		
		if (ether_addr_equal(smac, mac))
		{
			if (DebugLevel > 0)
				printk("hwnat_clean_wifi_entry: idx = %d \n",idx);
			memset(foe_entry,0,sizeof(struct FoeEntry));

			if(SUPPORT_SRAM_ENTRY && (idx<hwnat_dram_idx_base)) {
				setPpeEntry(idx, foe_entry);
			}
		}

	}
	
	if(!PPE_CACHE_NOT_SUPPORT) {
		if (isEN751627){
			PpeCleanCache751221();
		}else{
			// cache disable
			RegModifyBits(CAH_CTRL, 0, 0, 1);
					
			//clear cache table before enabling cache
			RegModifyBits(CAH_CTRL, 1, 9, 1);
			RegModifyBits(CAH_CTRL, 0, 9, 1);
					
			// cache enable
			RegModifyBits(CAH_CTRL, 1, 0, 1);
		}
	}

	return 0;
}

#if !defined(TCSUPPORT_XPON_HAL_API_NG)
int hwnat_clean_entry_by_ip(unsigned int ip_addr)
{
	struct FoeEntry *foe_entry=NULL;
	struct hwnat_tuple ppe_info, *opt=&ppe_info;
	int index=0;
	unsigned int idx=0, ptype=0;
	unsigned int ip1=0, ip2=0, ip3=0, ip4=0;

	for (idx = 0; idx < FOE_4TB_SIZ_TOTAL;idx++)
	{
		foe_entry = getPpeEntry(idx);
		if(foe_entry == NULL)
			continue;

		if (foe_entry->bfib1.state != BIND)
			continue;

		ptype = foe_entry->bfib1.pkt_type;

		switch (ptype)
		{
			case IPV4_HNAPT:
			case IPV4_HNAT:
				ip1 = foe_entry->ipv4_hnapt.sip;
				ip2 = foe_entry->ipv4_hnapt.dip;
				ip3 = foe_entry->ipv4_hnapt.new_sip;
				ip4 = foe_entry->ipv4_hnapt.new_dip;
				break;
			case IPV4_DSLITE:
				ip1 = foe_entry->ipv4_dslite.sip;
				ip2 = foe_entry->ipv4_dslite.dip;
				ip3 = ip4 = 0;
				break;
			case IPV6_6RD:
				ip1 = ip2 = 0;
				if((isEN7580) && (gPpeFoeSize == PPE_FOE_SIZE_64))
				{
					index =	get_ppe_entry_tunnel_ip_index(foe_entry, IPV6_6RD);
					ip3 = shnkTbl[index].eg_ipv4[0];
					ip4 = shnkTbl[index].eg_ipv4[1];
				}
				else
				{
					get_ppe_entry_tunnel_ip(foe_entry, IPV6_6RD, opt);
					ip3 = opt->eg_sipv4;
					ip4 = opt->eg_dipv4;
				}
				break;
			case L2_BRIDGE:
			case IPV6_3T_ROUTE:
			case IPV6_5T_ROUTE:
			default:
				continue;
		}

		if ((ip1 != ip_addr) && (ip2 != ip_addr) && (ip3 != ip_addr) && (ip4 != ip_addr))
			continue;

		if (DebugLevel > 0)
		{
			ip1 = htonl(ip1);
			ip2 = htonl(ip2);
			ip3 = htonl(ip3);
			ip4 = htonl(ip4);
			printk("current entry(%d): %pI4->%pI4 => %pI4->%pI4 \n", idx, &ip1, &ip2, &ip3, &ip4);
		}

		hwnat_delete_foe_entry(idx);
	}

	return 0;
}
#endif


int ecnt_hwnat_clean_entry_by_src_mac(struct net_data_s *net_data){
	struct net_bridge_fdb_entry *fdb = net_data->br_fdb_data.fdb;

	if(fdb == NULL || fdb->dst == NULL || fdb->dst->dev == NULL){
		return 0;
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)	
	unsigned char *addr = fdb->addr.addr;
#else
	unsigned char *addr = fdb->key.addr.addr;
#endif




	hwnat_clean_entry_by_src_mac(addr);

	return 0;
}

int hwnat_update_xsi_multicast_entry(unsigned int hsgmii_index, unsigned int nbq, unsigned int channel, unsigned int etype, struct FoeEntry *foe_entry)
{
	unsigned int org_channel = channel;
	unsigned int org_nbq = nbq;
	if (DebugLevel > 2)
		printk("hwnat_update_xsi_multicast_entry: hsgmii_index=%d nbq=%d channel = %d etype=%d\n", hsgmii_index, nbq, channel, etype);
	switch (hsgmii_index)
		{
			case XSI_PCIE0_IDX:
				channel &= ~(1<<4); 
				break;
			case XSI_PCIE1_IDX:
				nbq &= ~(1<<0);
				break;
			case XSI_USB_IDX:
				nbq &= ~(1<<1);
				break;
			case XSI_ETH_IDX:
				nbq &= ~(1<<2);	
				break;
			default:
				return 0;
		}
	if(((nbq & 0x7) == 0) && ((channel & 0x10) == 0) && ((etype & 0x3F) == 0))
		return -1;
	if(IS_IPV4_GRP(foe_entry)){
		if(channel != org_channel)
			set_channel_info(foe_entry, IPV4_HNAPT, channel);
		if(nbq != org_nbq)
			set_info2_nbq(foe_entry, IPV4_HNAPT, nbq);
	}else if(IS_IPV6_GRP(foe_entry)){
		if(channel != org_channel)
			set_channel_info(foe_entry, IPV6_5T_ROUTE, channel);
		if(nbq != org_nbq)
			set_info2_nbq(foe_entry, IPV6_5T_ROUTE, nbq);	
	}
	return 0;
}

int hwnat_clean_xsi_entry(unsigned int hsgmii_index, unsigned int channel)
{
	struct FoeEntry foe_entry_local;
	struct FoeEntry *foe_entry=&foe_entry_local;
	unsigned int idx, type, cur_channel, cur_fport, cur_nbq, cur_etype;
	if (DebugLevel > 2)
		printk("hwnat_clean_xsi_entry: channel = %d \n",channel);
	for (idx = 0; idx < FOE_4TB_SIZ_TOTAL; idx++)
	{	
		if((SUPPORT_SRAM_MAX_512) && (idx >= FOE_4TB_SIZ_SRAM) && (idx < hwnat_dram_idx_base))
			continue;
		foe_entry=&foe_entry_local;
		foe_entry = getPpeEntryLocal(idx, foe_entry);
		if(foe_entry == NULL)
			continue;
		if (foe_entry->bfib1.state != BIND)
			continue;
		type = foe_entry->bfib1.pkt_type;
		switch (type)
		{
			case IPV4_HNAPT:
			case IPV4_HNAT:
				cur_channel = foe_entry->ipv4_hnapt.channel ; 
				cur_fport = foe_entry->ipv4_hnapt.fpidx ;
				cur_nbq = foe_entry->ipv4_hnapt.nbq; 
				cur_etype = foe_entry->ipv4_hnapt.etype;
				break;
			case L2_BRIDGE:
				cur_channel = foe_entry->l2_bridge32.channel ;
				cur_fport = foe_entry->l2_bridge32.fpidx ;
				cur_nbq = foe_entry->l2_bridge32.nbq;
				cur_etype = foe_entry->l2_bridge32.etype;
				break;
			case IPV4_DSLITE:
				cur_channel = foe_entry->ipv4_dslite.channel ;
				cur_fport = foe_entry->ipv4_dslite.fpidx ;
				cur_nbq = foe_entry->ipv4_dslite.nbq;
				cur_etype = foe_entry->ipv4_dslite.etype;
				break;
			case IPV6_3T_ROUTE:
				cur_channel = foe_entry->ipv6_3t_route.channel;	
				cur_fport = foe_entry->ipv6_3t_route.fpidx ;
				break;
			case IPV6_5T_ROUTE:
				cur_channel = foe_entry->ipv6_5t_route.channel ;
				cur_fport = foe_entry->ipv6_5t_route.fpidx ;
				cur_nbq = foe_entry->ipv6_5t_route.nbq;
				cur_etype = foe_entry->ipv6_5t_route.etype;
				break;
			case IPV6_6RD:
				cur_channel = foe_entry->ipv6_6rd.channel ;
				cur_fport = foe_entry->ipv6_6rd.fpidx;		
				break;
			default:
				continue;
		}
		if(cur_fport != FP_QDMA_MCAST){
//			if((cur_fport != FP_GDMA3) && (cur_fport != FP_GDMA4)){
//				continue;
//			}
			if(cur_channel !=channel){
				continue;
			}
			if (DebugLevel > 2)
				printk("hwnat_clean_xsi_entry: idx = %d \n",idx);
			foe_ext[idx].rx_info = 0;
			memset(foe_entry,0,sizeof(struct FoeEntry));
		}
		else{
			if(hwnat_update_xsi_multicast_entry(hsgmii_index, cur_nbq, cur_channel, cur_etype, foe_entry)){
				foe_ext[idx].rx_info = 0;
				memset(foe_entry,0,sizeof(struct FoeEntry));
			}
		}
		if(SUPPORT_SRAM_ENTRY && (idx<hwnat_dram_idx_base)) {
			setPpeEntry(idx, foe_entry);
		}
	}
	return 0;
}

/* both directions */
int hwnat_clean_entry_by_mac(const unsigned char* mac)
{
	struct FoeEntry *foe_entry ;
	unsigned int idx,type;
    unsigned char smac[ETH_ALEN];
    unsigned char dmac[ETH_ALEN];
	int index = 0,i=0;
	
	memset(smac, 0, sizeof(smac));
	memset(dmac, 0, sizeof(dmac));

	if(mac==NULL)
		return 0;
	
	for (idx = 0; idx < FOE_4TB_SIZ_TOTAL; idx++)
	{
		if((SUPPORT_SRAM_MAX_512) && (idx >= FOE_4TB_SIZ_SRAM) && (idx < hwnat_dram_idx_base))
			continue;
		
		//foe_entry = &PpeFoeBase[idx];
		foe_entry = getPpeEntry(idx);
		if(foe_entry == NULL)
			return -1;

		if (foe_entry->bfib1.state != BIND)
			continue;
		
		type = foe_entry->bfib1.pkt_type;

		
		switch (type)
		{
			case IPV4_HNAPT:
			case IPV4_HNAT:
				FoeGetEntryMac(dmac, (foe_entry->ipv4_hnapt.dmac_hi), (foe_entry->ipv4_hnapt.dmac_lo));	
				FoeGetEntryMac(smac, (foe_entry->ipv4_hnapt.smac_hi), (foe_entry->ipv4_hnapt.smac_lo));	
				break;
			case L2_BRIDGE:
				FoeGetEntryMac(dmac, (foe_entry->l2_bridge32.in_dmac_hi), (foe_entry->l2_bridge32.in_dmac_lo));
				FoeGetEntryMac(smac, (foe_entry->l2_bridge32.in_smac_hi), (foe_entry->l2_bridge32.in_smac_lo));
				break;
			case IPV4_DSLITE:
				FoeGetEntryMac(dmac, (foe_entry->ipv4_dslite.dmac_hi), (foe_entry->ipv4_dslite.dmac_lo));
				FoeGetEntryMac(smac, (foe_entry->ipv4_dslite.smac_hi), (foe_entry->ipv4_dslite.smac_lo));
				break;
			case IPV6_3T_ROUTE:
				FoeGetEntryMac(dmac, (foe_entry->ipv6_3t_route.dmac_hi), (foe_entry->ipv6_3t_route.dmac_lo));	
				break;
			case IPV6_5T_ROUTE:
				FoeGetEntryMac(dmac, (foe_entry->ipv6_5t_route.dmac_hi), (foe_entry->ipv6_5t_route.dmac_lo));						
				break;
			case IPV6_6RD:
				FoeGetEntryMac(dmac, (foe_entry->ipv6_6rd.dmac_hi), (foe_entry->ipv6_6rd.dmac_lo));
				break;
			case IPV6_MC_ROUTE:
				FoeGetEntryMac(dmac, (foe_entry->ipv6_1t_route.dmac_hi), (foe_entry->ipv6_1t_route.dmac_lo));					
				FoeGetEntryMac(smac, (foe_entry->ipv6_1t_route.smac_hi), (foe_entry->ipv6_1t_route.smac_lo));					
				break;
			default:
				continue;
		}

		if((IPV6_3T_ROUTE == type) || (IPV6_5T_ROUTE == type) || (IPV6_6RD == type)){
			if((PPE_SHNK_TABLE_SUPPORT) && (PPE_FOE_SIZE_BYTE_MODE == IS_64_BYTE_MODE) ) {
				index = get_ppe_entry_smac_index(foe_entry, IPV6_5T_ROUTE);
				if(index != 0x10) {
					for(i=0; i<UPDMEM_SMAC_CNT; i++){
						if(index < UPDMEM_NUM)
							smac[i] = shnkTbl[index].smac[i];
					}
				}
			}
		}

		
		if (ether_addr_equal(smac, mac) || ether_addr_equal(dmac, mac))
		{
			if (DebugLevel > 0)
				printk("hwnat_clean_entry_by_mac: idx = %d \n",idx);
			
			memset(foe_entry,0,sizeof(struct FoeEntry));
			if(SUPPORT_SRAM_ENTRY && (idx<hwnat_dram_idx_base)) {
				setPpeEntry(idx, foe_entry);
			}
		}	

	}

	if(!PPE_CACHE_NOT_SUPPORT) {
		if (isEN751627){
			PpeCleanCache751221();
		}else{
			// cache disable
			RegModifyBits(CAH_CTRL, 0, 0, 1);
					
			//clear cache table before enabling cache
			RegModifyBits(CAH_CTRL, 1, 9, 1);
			RegModifyBits(CAH_CTRL, 0, 9, 1);
					
			// cache enable
			RegModifyBits(CAH_CTRL, 1, 0, 1);
		}
	}

	return 0;
}

/* delete entry by src_mac and dst_mac */

int hwnat_delete_entry_by_mac_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	int len;
	char get_buf[64];	
	int mac[6];
	unsigned char strMac[6] = {0};

	if(count > 63)
		len = 63;
	else
		len = count;
	
	if(copy_from_user(get_buf, buffer, len))
		return -EFAULT;
	
	get_buf[len]='\0';
	
    sscanf(get_buf, "%02x:%02x:%02x:%02x:%02x:%02x", 
         &mac[0], &mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);

    strMac[0] = (unsigned char)mac[0];
    strMac[1] = (unsigned char)mac[1];
    strMac[2] = (unsigned char)mac[2];
    strMac[3] = (unsigned char)mac[3];
    strMac[4] = (unsigned char)mac[4];
    strMac[5] = (unsigned char)mac[5];

	hwnat_clean_entry_by_mac(strMac);
	
	return len;
}
void ecnt_hwnat_cust_init(void)
{	
	struct proc_dir_entry *hwnat_proc;
	struct proc_dir_entry *ratelimit_proc;

	hwnat_proc = create_proc_entry("tc3162/hwnat_bandwidth", 0, NULL);
	if (hwnat_proc){
		hwnat_proc->read_proc = hwnat_mac_bandwidth_read_proc;
		hwnat_proc->write_proc = hwnat_mac_bandwidth_write_proc;
	} else {
		printk("proc_entry tc3162/hwnat_bandwidth alloc fail\n");
	}
	/*fix wifi7 tx ratelimit issue*/
	ratelimit_proc = create_proc_entry("tc3162/wifi_ratelimit_enable", 0, NULL);
	if (ratelimit_proc){
                ratelimit_proc->read_proc = wifi_ratelimit_read_proc;
		ratelimit_proc->write_proc = wifi_ratelimit_write_proc;
	} else {
		printk("proc_entry tc3162/wifi_ratelimit_enable alloc fail\n");
	}	
	
	if(TCSUPPORT_CT_JOYME_BANDWIDTH_VAL || TCSUPPORT_CF_JOYMEV2_PON_VAL || TCSUPPORT_XPON_HAL_API_EXT_VAL
		|| TCSUPPORT_CRJO_VAL)
		memset(macMatchFoeEntry, 0, sizeof(macMatchFoeEntry));

	/* To create hwnat_mac_bandwidth proc  */	
	if(TCSUPPORT_CT_JOYME_BANDWIDTH_VAL || TCSUPPORT_CF_JOYMEV2_PON_VAL || TCSUPPORT_XPON_HAL_API_EXT_VAL 
		|| TCSUPPORT_CRJO_VAL || SUPPORT_MAC_BANDWIDTH){
		joymeBandWidthFuncInit();
	}

	
	if(TCSUPPORT_CSC_EEUROPE_VAL)
	{
		up_queue_cnt_start = 0;
		down_queue_cnt_start = 0;
		up_down_queue_num = 63;
		up_queue_num = 0;
		flow_meter_filter = 0xfc000;
		flow_meter_shift = 14;
		flow_acnt_filter = 0x03f00;
		flow_acnt_shift = 8;
	}
	else
	{
		up_queue_cnt_start = 10;
		down_queue_cnt_start = 18;
		up_down_queue_num = 16;
		up_queue_num = 8;        
	}

	memset(upQueueRate, 0x0, sizeof(upQueueRate));
	memset(upDownQueueRate, 0x0, sizeof(upDownQueueRate));
	memset(softCntByte, 0x0, sizeof(softCntByte));
	INIT_LIST_HEAD(&Ppe_VIP_s.list);

	hwnat_proc = create_proc_entry("tc3162/queue_account", 0, NULL);
	if(hwnat_proc)
	{
		hwnat_proc->read_proc = queue_byte_counter_read_proc;
		hwnat_proc->write_proc =  queue_byte_counter_write_proc;	
	}
	memset(queueRxBytes, 0x0, sizeof(queueRxBytes));

	hwnat_proc = create_proc_entry("tc3162/en7521_up_queue_ratelimit", 0, NULL);
	if(hwnat_proc)
	{
		hwnat_proc->read_proc = up_queue_ratelimit_read_proc;
		hwnat_proc->write_proc =  up_queue_ratelimit_write_proc;	
	}
	memset(upQueueRate, 0x0, sizeof(upQueueRate));

	dslimit_SetUpTrtcmEnable = dslimit_UpTrtcmEnable;	
	dslimit_SetUpTrtcm = ppeSetFlowRatelimit;

	hwnat_proc = create_proc_entry("tc3162/hwnat_del_all", 0, NULL);
	hwnat_proc->read_proc = hwnat_del_all_read_proc;
	hwnat_proc->write_proc =  hwnat_del_all_write_proc;

#if !defined(TCSUPPORT_XPON_HAL_API_NG)
	/* Delete hwnat entry by related ip */
	hwnat_proc = create_proc_entry("tc3162/hwnat_del_by_ip", 0, NULL);
	hwnat_proc->read_proc = hwnat_del_by_ip_read_proc;
	hwnat_proc->write_proc =  hwnat_del_by_ip_write_proc;
#endif

	if (TCSUPPORT_XPON_HAL_API_EXT_VAL || TCSUPPORT_XPON_HAL_API_VAL){
		hwnat_proc = create_proc_entry("tc3162/hwnat_traffic_ratelimit", 0, NULL);
		hwnat_proc->read_proc = hwnat_ratelimit_read_proc;
		hwnat_proc->write_proc =  hwnat_ratelimit_write_proc;	
 	}

	hwnat_proc = create_proc_entry("tc3162/hwnat_flowid_mode", 0, NULL);
	hwnat_proc->read_proc = hwnat_flowid_mode_read_proc;
	hwnat_proc->write_proc =  hwnat_flowid_mode_write_proc; 

	memset(RateLimitItfInfo, 0x0, sizeof(RateLimitItfInfo));
	memset(RateLimitIPInfo, 0x0, sizeof(RateLimitIPInfo));

	downGemPortMeterStart = upGemPortMeterStart + upGemPortMeterNum;
	gemPortMeterNum = upGemPortMeterNum + downGemPortMeterNum;

	if(TCSUPPORT_ALPHION_PON_VAL)
	{
		upGemPortMeterStart = 0;
		upGemPortMeterNum = 32;
		downGemPortMeterNum = 32;
	}

	if(TCSUPPORT_XPON_HAL_API_EXT_VAL)
		PpeAWifiClearAWifiInfo();

	memset(wanRatelimit, 0x0, sizeof(wanRatelimit));
	ra_sw_nat_set_wan_meterid_hook = PpeSetWanMeterId;
	hwnat_proc = create_proc_entry("tc3162/hwnat_meter_mode", 0, NULL);
	hwnat_proc->read_proc = hwnat_meter_mode_read_proc;
	hwnat_proc->write_proc =  hwnat_meter_mode_write_proc; 

	ra_sw_nat_hook_clean_entry_by_mac = hwnat_clean_entry_by_mac;
	hwnat_proc = create_proc_entry("tc3162/hwnat_clean_entry_by_mac", 0, NULL);
	hwnat_proc->write_proc =  hwnat_delete_entry_by_mac_write_proc; 
	if(isEN7526c){
		hwnat_proc = create_proc_entry("tc3162/wan_ratelimit_use_pps", 0, NULL);
		if(hwnat_proc)
		{
			hwnat_proc->read_proc = hwnat_wan_ratelimit_use_pps_read_proc;
			hwnat_proc->write_proc =  hwnat_wan_ratelimit_use_pps_write_proc;	
		}
	}

	PpeInitFlowIdByMarkMode();
	ecnt_hwnat_pppoe_len_checksum();
	ra_sw_nat_hook_clean_entry_by_src_mac = ecnt_hwnat_clean_entry_by_src_mac;
	hwnat_clean_xsi_lan_hook = hwnat_clean_xsi_entry;
}

void ecnt_hwnat_cust_deinit(void)
{	
	remove_proc_entry("tc3162/hwnat_bandwidth", NULL);
	remove_proc_entry("tc3162/wifi_ratelimit_enable", NULL);
  
	if(TCSUPPORT_CT_JOYME_BANDWIDTH_VAL || TCSUPPORT_CF_JOYMEV2_PON_VAL || TCSUPPORT_XPON_HAL_API_EXT_VAL
		|| TCSUPPORT_CRJO_VAL)
		memset(macMatchFoeEntry, 0, sizeof(macMatchFoeEntry));

	if(TCSUPPORT_CT_JOYME_BANDWIDTH_VAL || TCSUPPORT_CF_JOYMEV2_PON_VAL || TCSUPPORT_XPON_HAL_API_EXT_VAL 
		|| TCSUPPORT_CRJO_VAL || SUPPORT_MAC_BANDWIDTH){
		joymeBandWidthFuncDeInit();
	}

	if(!list_empty(&Ppe_VIP_s.list))
	{
		struct Ppe_VIP_t *Ppe_VIP_deltmp_s;
		struct Ppe_VIP_t *tmp;
		list_for_each_entry_safe(Ppe_VIP_deltmp_s,tmp, &Ppe_VIP_s.list, list)
		{
			del_timer_sync(&Ppe_VIP_deltmp_s->mul_vip_timer);
			del_timer_sync(&Ppe_VIP_deltmp_s->VIP_expire_timer);
			list_del(&Ppe_VIP_deltmp_s->list);
		}
	}
	remove_proc_entry("tc3162/queue_account", NULL);	
	remove_proc_entry("tc3162/en7521_up_queue_ratelimit", NULL);
	
	dslimit_SetUpTrtcmEnable = NULL;	
	dslimit_SetUpTrtcm = NULL;

	remove_proc_entry("tc3162/hwnat_del_all", NULL);

	if (TCSUPPORT_XPON_HAL_API_EXT_VAL  || TCSUPPORT_XPON_HAL_API_VAL)
	{
		remove_proc_entry("tc3162/hwnat_traffic_ratelimit", NULL);
	}

#if !defined(TCSUPPORT_XPON_HAL_API_NG)
	remove_proc_entry("tc3162/hwnat_del_by_ip", NULL);
#endif

	remove_proc_entry("tc3162/hwnat_flowid_mode", NULL);

	ra_sw_nat_set_wan_meterid_hook = NULL;
    remove_proc_entry("tc3162/hwnat_meter_mode", NULL);

	ra_sw_nat_hook_clean_entry_by_mac = NULL;
    remove_proc_entry("tc3162/hwnat_clean_entry_by_mac", NULL);
	if(isEN7526c){
		remove_proc_entry("tc3162/wan_ratelimit_use_pps", NULL);
	}
	ra_sw_nat_hook_clean_entry_by_src_mac = NULL;
	hwnat_clean_xsi_lan_hook = NULL;
}


