/*
    Module Name:
    hwnat_ioctl.c

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2006-10-06      Initial version
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/libcompileoption.h>
#ifdef TCSUPPORT_CPU_ARMV8
#include <asm/div64.h>
#endif

#include "util.h"
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_ether.h>
#include "ppe_api.h"
#include "ra_nat_cust.h"
#include "ra_nat_gen.h"
#include "ppe2_reg.h"
#include "ra_nat_ic_dis.h"

unsigned char bind_dir = BIDIRECTION;
int DebugLevel = 0;
unsigned int hwnat_dbg_bindfail_idx = FOE_ENTRY_NUM_INVALID+1;
unsigned int hwnat_dgb_free_idx = FOE_ENTRY_NUM_INVALID+1;
unsigned int hwnat_dgb_drop_idx = FOE_ENTRY_NUM_INVALID+1;
unsigned int hwnat_dbg_parse_dump = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
struct device *hnat_device = NULL;
int hnat_result = 0;
hwnat_status_t hwnat_enable_status = HWNAT_STATUS_ON;

extern struct device *ecnt_device_create(int result);
extern void ecnt_device_delete(int result);
#endif
extern unsigned char snoopingSpeed_en;

#if !defined (CONFIG_HNAT_V2)
int pre_acl_start_addr;
int pre_ac_start_addr;
int post_ac_start_addr;
int pre_mtr_start_addr;
int post_mtr_start_addr;
#endif
extern unsigned int gPpeFoeNum;
extern void TestsetPpeSetEthtype(int i);
extern ppeAcntGrp_t ppe_acnt_grp[ACNT_GRP_WAN_NUM];

#ifdef CONFIG_TP_IMAGE
extern struct FoeEntryIntf *foe_intf;
extern struct hwnat_entries_args *foe_args;
extern unsigned int foe_args_size;
#endif /* CONFIG_TP_IMAGE */

extern void ppeLocalInEnable(void);
extern void ppeLocalInDisable(void);
extern int ppeLocalInAddDport(unsigned short *dport_ptr,unsigned int port_num);
extern int ppeLocalInAddAll(void);
extern int ppeLocalInDelDport(unsigned short *dport_ptr,unsigned int port_num);
extern int ppeLocalInDelAll(void);
extern int ppeLocalInShowDport(void);
#ifdef CONFIG_TP_IMAGE
extern int ppeLocalInBackupDport(void);
extern int ppeLocalInRestoreDport(void);
#endif /* CONFIG_TP_IMAGE */
extern unsigned char Ppe_Set_Mul_High_Priority(unsigned int data);
extern unsigned int PPE_Mul_Status(void);
static void ppe_null_ioctl(void *arg);
static void FoeAddEntryIoctl(void *arg);
static void FoeDumpCacheEntryIoctl(void *arg);
static void FoeDumpEntryIoctl(void *arg);
static void FoeGetAllEntriesIoctl(void *arg);
static void FoeBindEntryIoctl(void *arg);
static void FoeUnBindEntryIoctl(void *arg);
static void FoeDelEntryByNumIoctl(void *arg);
static void ppe_debug_ioctl(void *arg);
static void PpeGetAGCntIoctl(void *arg);    
static void PpeGetLanIpCntIoctl(void *arg);  
static void PpeTrafficQosFlowCntIoctl(void *arg);
static void PpeTrafficQosFlowRateIoctl(void *arg);
static void PpeSetLanTcpBlackListIoctl(void *arg);
static void PpeSetBindThresholdIoctl(void *arg);
static void PpeSetMaxEntryLimitIoctl(void *arg);
static void PpeSetKaIntervalIoctl(void *arg);
static void PpeSetUnbindLifeTimeIoctl(void *arg);
static void PpeSetBindLifetimeIoctl(void *arg);
static void ppe_dir_ioctl(void *arg);
static void ppe_multicast_en_ioctl(void *arg);
static void ppe_ip_proto_chk_type_ioctl(void *arg);
static void PpeRegDumpIoctl(void *arg);
static void PpeCleanTableHandlerIoctl(void *arg);
static void PpeCleanTableIoctl(void *arg);
static void ppeSnoopingSpeed_en_ioctl(void *arg);
static void SetPpeSetEthtypeIoctl(void *arg);
static void hwnat_clean_lan_entry_ioctl(void *arg);
static void hwnat_clean_wan_entry_ioctl(void *arg);
static void FoeDumpCacheDataIoctl(void *arg);
static void FoeClearCacheIoctl(void *arg);
static void ecnt_ppe_tls_ioctl(void *arg);
static void ecnt_ppe_meter_ioctl(void *arg);
static void ecnt_ppe_trtcm_ioctl(void *arg);
static void ecnt_ppe_account_ioctl(void *arg);
static void ecnt_ppe_cache_ioctl(void *arg);
static void ecnt_ppe_hash_ioctl(void *arg);
static void ecnt_ppe_local_in_ioctl(void *arg);
static void ppe_wan_ratelimit_ioctl(void *arg);
static void ppe_wan_acnt_ioctl(void *arg);
static void ecnt_ppe_meter_group_ioctl(void *arg);
static void ecnt_ppe_meter_flow_ioctl(void *arg);
static void ecnt_ppe_trtcm_group_ioctl(void *arg);
static void ecnt_ppe_trtcm_flow_ioctl(void *arg);
static void ecnt_ppe_account_stats_ioctl(void *arg);
static void ecnt_ppe_black_ip_add_ioctl(void *arg);
static void ecnt_ppe_black_ip_delete_ioctl(void *arg);
static void ecnt_ppe_black_ip_delete_sign_ioctl(void *arg);
static void Ppe_L2b_Ethtype_Clean_Ioctl(void *arg);
static void PpeSetMulHighPriorityIoctl(void *arg);
static void ppe_unknow_mc_cnt_ioctl(void *arg);
static void ecnt_ppe_black_ip_port_protocol_ioctl(void *arg);
static void ecnt_ppe_get_multicast_status_ioctl(void *arg);
static void ecnt_ppe_set_CDS_dlf_ioctl(void* arg);
static void ecnt_ppe_set_CDS_unknown_mul_ioctl(void* arg);
#ifdef CONFIG_COMPAT
static void ppe_debug_compat_ioctl(void *arg);
#endif
static void PpeMirrorCmd(void *arg);
static void PpeSetWanMtu(void *arg);
static void ecnt_ppe_set_mtu_ioctl(void *arg);
static void ecnt_ppe_set_nptv6_prefix(void* arg);
static void ecnt_ppe_do_nptv6_mode(void* arg);
static void ecnt_ppe_set_fport_mtu_ioctl(void *arg);
static void ecnt_ppe_clean_uni_entry_ioctl(void *arg);
static void ecnt_ppe_ppe2_cmd_ioctl(void *arg);
static void ecnt_ppe_vxlan_hash_cmd_ioctl(void *arg);
static void ecnt_ppe_l2tp_hash_cmd_ioctl(void *arg);
static void hwnat_set_enable_status_ioctl(void *arg);
static void FoeGetEntriesIoctl(void *arg);

int PpeSetBindThreshold(uint32_t threshold);
int PpeSetMaxEntryLimit(uint32_t full, uint32_t half, uint32_t qurt);
int PpeSetRuleSize(uint16_t pre_acl, uint16_t pre_meter, uint16_t pre_ac,
		   uint16_t post_meter, uint16_t post_ac);

int PpeSetKaInterval(uint8_t tcp_ka, uint8_t udp_ka);
int PpeSetUnbindLifeTime(uint8_t lifetime);
int PpeSetBindLifetime(uint16_t tcp_fin, uint16_t udp_life, uint16_t fin_life, uint16_t ntu_life);

extern unsigned long long queueRxBytes[63];
extern unsigned int softCntByte[63];
extern int up_down_queue_num;
extern int up_queue_num;
extern int up_queue_cnt_start;

extern unsigned char multicast_en;
void PpeRegDump(void);
extern short int ip_proto_chk;

extern unsigned long long unknow_mc_byte_cnt;
extern unsigned int unknow_mc_pkt_cnt;

extern void (*get_wan_index_info_hook)(char* wan_index);

extern void PpeGetLanIpCnt(uint32_t ipaddr, unsigned long long* cnt_byte_up, unsigned long long* cnt_byte_down);
extern int ppeSetFlowRatelimit(uint index, uint rateLimit);
extern int ppeGetFlowRatelimit(uint index, uint *rateLimit);
extern int32_t PpeSetLanTcpBlackList(uint16_t lanPort, uint16_t enable);
extern void wan_account_read(int i);
extern int wan_account_clear(int wanIndex);
extern void setup_ip_chk(char ip_chk_type);
extern int32_t PpeCleanTableHandler(void);
extern void SetPpeSetEthtype(int i);
extern int hwnat_clean_lan_entry(unsigned int port);
extern int hwnat_clean_wan_entry(unsigned int vid);
extern int hwnat_clean_entry_by_ethertype( unsigned int type, unsigned char port);
extern void PpeSetCacheMode(int mode);
extern void foeHashModeSet(uint32_t HashMode);
extern void foeHashBitShiftSet(uint32_t shift);
extern void foeHashTlsTypeSet(uint32_t dmacDis, uint32_t pbitsDis, uint32_t otherDis);
extern void foeHashL2bSmacEn(uint32_t l2bSmacEn);
extern int PpeBlack_ip_add(unsigned int ip, unsigned int mask, unsigned int type, unsigned int sign);
extern int PpeBlack_ip_delete(unsigned int ip,unsigned int mask,unsigned int type,unsigned int sign);
extern int PpeBlack_ipv6_add(unsigned char *u6_addr8, unsigned int mask, unsigned int type, unsigned int sign);
extern int PpeBlack_ipv6_delete(unsigned char *u6_addr8,unsigned int mask,unsigned int type,unsigned int sign);
extern int PpeBlack_ip_delete_same_sign(unsigned int sign);
extern int PpeBlack_ip_port_protocol_add(unsigned int ip, unsigned int port, unsigned int protocol);
extern int PpeBlack_ip_port_protocol_delete(unsigned int ip, unsigned int port, unsigned int protocol);
extern int PpeBlack_ip_port_protocol_show(void);
extern int PpeBlack_ip_port_protocol_delete_all(void);
extern int Ppe_set_CDS_dlf_mode(unsigned char mode);
extern int Ppe_set_CDS_Unmul_mode(unsigned char mode);

static void ecnt_ppe_fast_rule_ioctl(void *arg);
extern int ppeShowFastRuleList(void);
extern int ppeFastRuleEnable(void);
extern int ppeFastRuleDisable(void);
extern int ppeFastRuleSetMarkBit(unsigned char bitValue);
extern int ppeFastRuleSetMarkEnable(unsigned char value);
extern int ppeFastRuleSetReverseEnable(unsigned char value);
extern int PpeUnMultiDropByIndex(struct multi_drop_args *multi_drop);
extern int PpeMultilistAddDrop(struct multi_drop_args *multi_drop);
extern int PpeQueryMultiDropList(struct multi_drop_args *multi_drop);
extern int PpeSetMtu(unsigned short ppe_mtu);
extern int PpeSetFpMtu(int fPort, unsigned short ppe_mtu);

extern int hwnat_clean_unicast_entry(void);

static void PpeUnMultiDropByIndex_ioctl(void *arg);
static void PpeQueryMultiDropList_ioctl(void * arg);
extern int ppeSetWanRatelimitThreshold(uint threshold);
extern int ppeSetWanRatelimitMtu(uint8_t wan_type, uint wan_mtu);
extern int ppeSetWanRatelimit(uint index, uint rateLimit, uint8_t dir);
extern int ppeNptV6AddPrefixHw(unsigned char* ifname,unsigned short* s_prefix,unsigned int s_prefix_len,unsigned short* d_prefix,unsigned int d_prefix_len);
extern int ppeNptV6DelPrefixHw(unsigned char* ifname);
extern int ppeNptV6DelPrefixHwAll(void);

extern int ppe_ioctl_enable_ppe2(void);
extern int ppe_ioctl_disable_ppe2(void);

extern int ppe_ioctl_enable_vxlan_hash(void);
extern int ppe_ioctl_disable_vxlan_hash(void);
extern int ppe_ioctl_enable_l2tp_hash(void);
extern int ppe_ioctl_disable_l2tp_hash(void);
extern int ppe_ioctl_get_vxlan_hash(void);
extern int ppe_ioctl_get_l2tp_hash(void);

typedef void (*ppe_ioctl_func) (void*);

static ppe_ioctl_func
ppe_ioctl_funcs[] =
{
    ppe_null_ioctl, /* 0x0 */
    FoeAddEntryIoctl,
    FoeDumpCacheEntryIoctl,
    FoeDumpEntryIoctl,
    FoeGetAllEntriesIoctl,
    FoeBindEntryIoctl, /* 0x5 */
    FoeUnBindEntryIoctl,
    FoeDelEntryByNumIoctl,
    ppe_debug_ioctl,
    PpeGetAGCntIoctl,
    ecnt_ppe_set_mtu_ioctl, /* 0xa */
    ecnt_ppe_set_fport_mtu_ioctl,
    ppe_null_ioctl,
    ppe_null_ioctl,
    ppe_unknow_mc_cnt_ioctl,
    ppe_null_ioctl,
    ppe_wan_acnt_ioctl, /* 0x10 */
    PpeTrafficQosFlowCntIoctl,
    PpeTrafficQosFlowRateIoctl,
    PpeGetLanIpCntIoctl,
    PpeSetLanTcpBlackListIoctl,
    ppe_wan_ratelimit_ioctl, /* 0x15 */
    PpeSetBindThresholdIoctl,
    PpeSetMaxEntryLimitIoctl,
    ppe_null_ioctl,
    PpeSetKaIntervalIoctl,
    PpeSetUnbindLifeTimeIoctl, /* 0x1a */
    PpeSetBindLifetimeIoctl,
    ppe_dir_ioctl,
    ppe_multicast_en_ioctl,
    ppe_ip_proto_chk_type_ioctl,
    PpeRegDumpIoctl,
    PpeCleanTableHandlerIoctl, /* 0x20 */
    PpeCleanTableIoctl,
    ppeSnoopingSpeed_en_ioctl,
    SetPpeSetEthtypeIoctl,
    hwnat_clean_lan_entry_ioctl,
    hwnat_clean_wan_entry_ioctl, /* 0x25 */
    FoeDumpCacheDataIoctl,
    FoeClearCacheIoctl,
    ecnt_ppe_tls_ioctl,
    ecnt_ppe_meter_ioctl,
    ecnt_ppe_trtcm_ioctl, /* 0x2a */
    ecnt_ppe_account_ioctl,
    ecnt_ppe_cache_ioctl,
    ecnt_ppe_hash_ioctl,
    ecnt_ppe_local_in_ioctl,
    ecnt_ppe_meter_group_ioctl,
    ecnt_ppe_meter_flow_ioctl,
    ecnt_ppe_trtcm_group_ioctl,
    ecnt_ppe_trtcm_flow_ioctl,
    ecnt_ppe_account_stats_ioctl,
    ecnt_ppe_black_ip_add_ioctl,
    ecnt_ppe_black_ip_delete_ioctl,
    ecnt_ppe_black_ip_delete_sign_ioctl,
    Ppe_L2b_Ethtype_Clean_Ioctl,
    PpeSetMulHighPriorityIoctl,
    ecnt_ppe_fast_rule_ioctl, /*0x39*/
    PpeUnMultiDropByIndex_ioctl,/*0x3a*/
    PpeQueryMultiDropList_ioctl,
    ecnt_ppe_black_ip_port_protocol_ioctl,
    ecnt_ppe_get_multicast_status_ioctl,
    ecnt_ppe_set_CDS_dlf_ioctl,
    ecnt_ppe_set_CDS_unknown_mul_ioctl,
	PpeMirrorCmd,/*0x40*/
    PpeSetWanMtu,
    ecnt_ppe_set_nptv6_prefix,
    ecnt_ppe_do_nptv6_mode,
    ecnt_ppe_clean_uni_entry_ioctl,	
    ecnt_ppe_ppe2_cmd_ioctl,/*0x45*/
    ecnt_ppe_vxlan_hash_cmd_ioctl,
    ecnt_ppe_l2tp_hash_cmd_ioctl,
    hwnat_set_enable_status_ioctl,/*0x48*/
    FoeGetEntriesIoctl, /*0x49*/
    NULL
};

/* For EN7581 64bit kernel + 32bit APP, use compat ioctl */
/* only modify several for EN7581 verify */
#ifdef CONFIG_COMPAT
static ppe_ioctl_func
ppe_compat_ioctl_funcs[] =
{
    ppe_null_ioctl, /* 0x0 */
    FoeAddEntryIoctl,
    FoeDumpCacheEntryIoctl,
    FoeDumpEntryIoctl,
    FoeGetAllEntriesIoctl,
    FoeBindEntryIoctl, /* 0x5 */
    FoeUnBindEntryIoctl,
    FoeDelEntryByNumIoctl,
    ppe_debug_compat_ioctl,
    PpeGetAGCntIoctl,
    ecnt_ppe_set_mtu_ioctl, /* 0xa */
    ppe_null_ioctl,
    ppe_null_ioctl,
    ppe_null_ioctl,
    ppe_unknow_mc_cnt_ioctl,
    ppe_null_ioctl,
    ppe_wan_acnt_ioctl, /* 0x10 */
    PpeTrafficQosFlowCntIoctl,
    PpeTrafficQosFlowRateIoctl,
    PpeGetLanIpCntIoctl,
    PpeSetLanTcpBlackListIoctl,
    ppe_wan_ratelimit_ioctl, /* 0x15 */
    PpeSetBindThresholdIoctl,
    PpeSetMaxEntryLimitIoctl,
    ppe_null_ioctl,
    PpeSetKaIntervalIoctl,
    PpeSetUnbindLifeTimeIoctl, /* 0x1a */
    PpeSetBindLifetimeIoctl,
    ppe_dir_ioctl,
    ppe_multicast_en_ioctl,
    ppe_ip_proto_chk_type_ioctl,
    PpeRegDumpIoctl,
    PpeCleanTableHandlerIoctl, /* 0x20 */
    PpeCleanTableIoctl,
    ppeSnoopingSpeed_en_ioctl,
    SetPpeSetEthtypeIoctl,
    hwnat_clean_lan_entry_ioctl,
    hwnat_clean_wan_entry_ioctl, /* 0x25 */
    FoeDumpCacheDataIoctl,
    FoeClearCacheIoctl,
    ecnt_ppe_tls_ioctl,
    ecnt_ppe_meter_ioctl,
    ecnt_ppe_trtcm_ioctl, /* 0x2a */
    ecnt_ppe_account_ioctl,
    ecnt_ppe_cache_ioctl,
    ecnt_ppe_hash_ioctl,
    ecnt_ppe_local_in_ioctl,
    ecnt_ppe_meter_group_ioctl,
    ecnt_ppe_meter_flow_ioctl,
    ecnt_ppe_trtcm_group_ioctl,
    ecnt_ppe_trtcm_flow_ioctl,
    ecnt_ppe_account_stats_ioctl,
    ecnt_ppe_black_ip_add_ioctl,
    ecnt_ppe_black_ip_delete_ioctl,
    ecnt_ppe_black_ip_delete_sign_ioctl,
    Ppe_L2b_Ethtype_Clean_Ioctl,
    PpeSetMulHighPriorityIoctl,
    ecnt_ppe_fast_rule_ioctl, /*0x39*/
    PpeUnMultiDropByIndex_ioctl,/*0x3a*/
    PpeQueryMultiDropList_ioctl,
    ecnt_ppe_black_ip_port_protocol_ioctl,
    ecnt_ppe_get_multicast_status_ioctl,
    ecnt_ppe_set_CDS_dlf_ioctl,
    ecnt_ppe_set_CDS_unknown_mul_ioctl,
	PpeMirrorCmd,/*0x40*/
    PpeSetWanMtu,
    ecnt_ppe_set_nptv6_prefix,
    ecnt_ppe_do_nptv6_mode,
	 ecnt_ppe_clean_uni_entry_ioctl,
	 ecnt_ppe_ppe2_cmd_ioctl,/*0x45*/
    NULL
};
#endif

static void ppe_null_ioctl(void *arg) {
    printk("\nError: No this ppe ioctl command\n");
    return;
}

static void FoeAddEntryIoctl(void *arg)
{
	struct hwnat_tuple opt;

	memset(&opt,0,sizeof(struct hwnat_tuple));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_tuple)))
		return;

	FoeAddEntry(&opt);

	return;
}

static void FoeDumpEntryIoctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	FoeDumpEntry(opt.entry_num);

	return;
}

static void FoeGetAllEntriesIoctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	FoeGetAllEntries(&opt);

	return;
}

#ifdef CONFIG_TP_IMAGE
static void FoeGetEntriesIoctl(void *arg)
{
	struct hwnat_tuple_ext *tuple = NULL;
	int index = 0;

	if (NULL == foe_intf || NULL == foe_args || NULL == arg)
	{
		return;
	}

	memset(foe_args, 0, foe_args_size);
	if (copy_from_user(foe_args, arg, sizeof(struct hwnat_entries_args)))
	{
		return;
	}

	if (foe_args->num_of_entries >= FOE_ARGS_MAX_ENTRIES_NUM)
	{
		foe_args->num_of_entries = FOE_ARGS_MAX_ENTRIES_NUM;
	}

	FoeGetEntries(foe_args);

	for (index = 0; index < foe_args->num_of_entries; ++index)
	{
		tuple = &foe_args->all_entries[index];

		tuple->act_dp = foe_intf[tuple->entry.hash_index].act_dp;
		tuple->act_sp = foe_intf[tuple->entry.hash_index].act_sp;
	}

	copy_to_user(arg, foe_args, sizeof(struct hwnat_entries_args) +
			foe_args->num_of_entries * sizeof(struct hwnat_tuple_ext));

	return;
}
#else /* CONFIG_TP_IMAGE */
static void FoeGetEntriesIoctl(void *arg)
{
	struct hwnat_entries_args opt;
	struct hwnat_entries_args *entry = NULL;

	memset(&opt,0,sizeof(struct hwnat_entries_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_entries_args)))
	{
		return;
	}

	if (opt.num_of_entries > ((PAGE_SIZE - sizeof(struct hwnat_entries_args)) / sizeof(struct hwnat_tuple_ext)))
	{
		opt.num_of_entries = ((PAGE_SIZE - sizeof(struct hwnat_entries_args)) / sizeof(struct hwnat_tuple_ext));
	}

	entry = (struct hwnat_entries_args *)kmalloc(sizeof(struct hwnat_entries_args) + opt.num_of_entries * sizeof(struct hwnat_tuple_ext), GFP_ATOMIC);
	if (NULL == entry)
	{
		return;
	}

	memset(entry, 0, sizeof(struct hwnat_entries_args) + opt.num_of_entries * sizeof(struct hwnat_tuple_ext));
	if (copy_from_user(entry, arg, sizeof(struct hwnat_entries_args)))
	{
		kfree(entry);
		return;
	}

	entry->num_of_entries = opt.num_of_entries;
	FoeGetEntries(entry);

	if (copy_to_user(arg, entry, sizeof(struct hwnat_entries_args) + opt.num_of_entries * sizeof(struct hwnat_tuple_ext)))
	{
		kfree(entry);
		return;
	}

	kfree(entry);

	return;
}
#endif /* CONFIG_TP_IMAGE */

static void FoeBindEntryIoctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	FoeBindEntry(&opt);

	return;
}

static void FoeUnBindEntryIoctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	FoeUnBindEntry(&opt);

	return;
}

static void FoeDelEntryByNumIoctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	FoeDelEntryByNum(opt.entry_num);

	return;
}

static void FoeDumpCacheEntryIoctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	if (PPE_CACHE_NOT_SUPPORT)
		return;

	if (IS_IC_EN751X2X)
		FoeDumpCacheEntryNew(opt.entry_num);
	else
		FoeDumpCacheEntry();

	return;
}

static void ppe_debug_ioctl(void *arg)
{
	struct hwnat_args opt;
	struct hwnat_debug_args debug;

	memset(&opt, 0, sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	memset(&debug, 0, sizeof(struct hwnat_debug_args));
	if (copy_from_user(&debug, opt.p_dbg_arg, sizeof(struct hwnat_debug_args)))
		return;

	switch(debug.id)
	{
		case HWNAT_DBG_ID_SET_DBGLVL:
			DebugLevel = debug.dbg_lvl;
			printk("set hwnat debug level:%d\n",DebugLevel);
			break;
		case HWNAT_DBG_ID_SET_BINDFAILIDX:
			hwnat_dbg_bindfail_idx = debug.bind_fail_idx;
			printk("set hwnat bind fail index:%d\n",hwnat_dbg_bindfail_idx);
			break;
		case HWNAT_DBG_ID_SET_FREEIDX:
			hwnat_dgb_free_idx = debug.free_idx;
			printk("set hwnat free index:%d\n",hwnat_dgb_free_idx);
			break;
		case HWNAT_DBG_ID_SET_DROPIDX:
			hwnat_dgb_drop_idx = debug.drop_idx;
			printk("set hwnat drop index:%d\n",hwnat_dgb_drop_idx);
			FoeDelEntryByNum(hwnat_dgb_drop_idx);
			break;
		case HWNAT_DBG_ID_SET_DUMP_PARSEINFO:
			hwnat_dbg_parse_dump = debug.parse_dump_enable;
			printk("set hwnat parse dump:%d\n",hwnat_dbg_parse_dump);
			break;
		default:
			printk("hwnat debug id error\n");
	}

	return;
}

static void PpeGetAGCntIoctl(void *arg)
{
	struct hwnat_ac_args opt;

	memset(&opt,0,sizeof(struct hwnat_ac_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_ac_args)))
		return;
	
	PpeGetAGCnt(&opt);

	return;
}

static void ecnt_ppe_set_mtu_ioctl(void *arg)
{
	unsigned short ppe_mtu = 0;

	if (copy_from_user(&ppe_mtu, arg, sizeof(unsigned short)))
		return;

	PpeSetMtu(ppe_mtu);

	return;
}

static void ecnt_ppe_set_fport_mtu_ioctl(void *arg)
{
	struct hwnat_fport_mtu_args *op = (struct hwnat_fport_mtu_args *)arg;
	
	PpeSetFpMtu(op->fport, op->mtu);

	return;
}

static void PpeGetLanIpCntIoctl(void *arg)
{
	struct hwnat_lanip_cnt opt;

	memset(&opt,0,sizeof(struct hwnat_lanip_cnt));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_lanip_cnt)))
		return;

	PpeGetLanIpCnt(opt.ipaddr, &opt.cnt_byte_up, &opt.cnt_byte_down);

	return;
}

static void PpeTrafficQosFlowCntIoctl(void *arg)
{
	struct hwnat_tr_qos_flow_cnt opt;

	memset(&opt,0,sizeof(struct hwnat_tr_qos_flow_cnt));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_tr_qos_flow_cnt)))
		return;

	PpeTrafficQosFlowCnt(&opt);

	return;
}

static void PpeTrafficQosFlowRateIoctl(void *arg)
{
	struct hwnat_tr_qos_flow_rate opt;
	uint idx = 0;
	int dir = 0;

	memset(&opt,0,sizeof(struct hwnat_tr_qos_flow_rate));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_tr_qos_flow_rate)))
		return;
	
	if(SUPPORT_METER_RATELIMIT){
		idx = opt.flowid;
		dir = opt.direction? (DOWN_STREAM) : (UP_STREAM);	
		if(PPE_SET == opt.action){
			FE_API_SET_METER_RATELIMIT_MODE(dir, TRAFFIC_QOS_MODE);
			if(opt.ratelimit == 0)
				FE_API_SET_METER_RATELIMIT(0 , idx, dir, TRAFFIC_QOS_MODE, 0);
			else
				FE_API_SET_METER_RATELIMIT(opt.ratelimit/1024, idx, dir, TRAFFIC_QOS_MODE, 1);
		}else{
			FE_API_GET_METER_RATELIMIT(&opt.ratelimit, idx, dir, TRAFFIC_QOS_MODE);
		}
	}else{
		idx = opt.direction?(opt.flowid+UP_QUEUE_NUM):(opt.flowid);
		if(PPE_SET == opt.action)
		{
			ppeSetFlowRatelimit(idx, opt.ratelimit);
		}
		else
		{
			ppeGetFlowRatelimit(idx, &opt.ratelimit);
		}
	}
	return;
}

static void PpeSetLanTcpBlackListIoctl(void *arg)
{
	struct hwnat_tcp_lanPort opt;

	memset(&opt,0,sizeof(struct hwnat_tcp_lanPort));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_tcp_lanPort)))
		return;

	PpeSetLanTcpBlackList(opt.lanPort, opt.enable);

	return;
}

void PpeFlowCounterRead(struct hwnat_wan_acnt_args *arg, unsigned char wanIndex)
{
    IO_SREG(PPE_FCNT_CTRL, ((wanIndex*2)<<4));
    arg->wan_acnt.uniTxCnt = IO_GREG(PPE_FCNT_PCNT);
    arg->wan_acnt.uniTxBytes = ((((unsigned long long)(IO_GREG(PPE_FCNT_BCNT_H)))<<32)+IO_GREG(PPE_FCNT_BCNT_L));
    IO_SREG(PPE_FCNT_CTRL, ((wanIndex*2+1)<<4));
    arg->wan_acnt.uniRxCnt = IO_GREG(PPE_FCNT_PCNT);
    arg->wan_acnt.uniRxBytes = ((((unsigned long long)(IO_GREG(PPE_FCNT_BCNT_H)))<<32)+IO_GREG(PPE_FCNT_BCNT_L));

    return;
}

void PpeFlowCounterClear(unsigned char wanIndex)
{    
    if(wanIndex < 8)
    {
        IO_SREG(PPE_FCNT_CTRL, ((wanIndex*2)<<4) | 1);
        IO_SREG(PPE_FCNT_CTRL, ((wanIndex*2+1)<<4) | 1);
    }
    else if(99 == wanIndex)     /* clear all */
    {
        IO_SREG(PPE_FCNT_CTRL, 1<<1);
    }
    else
    {
        printk("wanIndex %d error\n", wanIndex);
    }

    return;
}

void PpeFlowCounterHandle(struct hwnat_wan_acnt_args *arg, unsigned char wanid, unsigned char action)
{
    int index = 0;
    char wan_index_info[8]={-1,-1,-1,-1,-1,-1,-1,-1};

    if(99 == wanid)
    {
        index = 99;
        goto clear_counter;
    }
    
    if(get_wan_index_info_hook){
        get_wan_index_info_hook(wan_index_info);
    }

    for(index=0;index<8;index++)
    {
        if(wan_index_info[index] == wanid)
            break;
    }

    if(index >= 8)
    {
        printk("wanid is %d, error\n", wanid);
        return; 
    }

    if(PPE_METHOD_READ == action)
    {
        PpeFlowCounterRead(arg, index);
        return;
    }

clear_counter:
    if(PPE_METHOD_CLEAR == action)
    {
        PpeFlowCounterClear(index);
    }

    return;
}

static void PpeWanRateLimit(struct hwnat_wan_ratelimit_args *arg)
{
	unsigned char wanid = arg->wanid;
	int index;
	char wan_index_info[8]={-1,-1,-1,-1,-1,-1,-1,-1};

	if(get_wan_index_info_hook)
		get_wan_index_info_hook(wan_index_info);
	
	for (index = 0; index < 8; index++) 
	{
		if(wan_index_info[index] == wanid)
			break;
	}
	
	if (index > 7)
	{
		printk("wanid is %d, error\n", wanid);
		return; 
	}

    switch(arg->method){
		case WAN_RATELIMIT_METHOD_SET_RATE:
			ppeSetWanRatelimit(index, arg->ratelimit, arg->direction);
			break;

		case WAN_RATELIMIT_METHOD_SET_MTU:
			ppeSetWanRatelimitMtu(arg->wan_type, arg->mtu);
			break;

		case WAN_RATELIMIT_METHOD_SET_THRESHOLD:
			ppeSetWanRatelimitThreshold(arg->threshold);
			break;
		
		default:
			printk("method is %d, error\n", arg->method);
			break;
	}

    return;
}

static void ppe_wan_ratelimit_ioctl(void *arg)
{
	struct hwnat_wan_ratelimit_args opt;
	
	memset(&opt,0,sizeof(struct hwnat_wan_ratelimit_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_wan_ratelimit_args)))
		return;
	
	PpeWanRateLimit(&opt);

	return;
}

void PpeWanACnt(struct hwnat_wan_acnt_args *arg)
{
	unsigned char wanid = arg->wanid;
	unsigned char action = arg->action;
	int index;
	char wan_index_info[8]={-1,-1,-1,-1,-1,-1,-1,-1};
	ppeAcntGrp_t orig_wan_acnt;
	#ifdef TCSUPPORT_CPU_ARMV8
	unsigned long long tmp;
	#endif

	if((PPE_METHOD_CLEAR == action) && 99 == wanid)
	{
		wan_account_clear(wanid);
		return;
	}
		
	if(!TCSUPPORT_ACCOUNT_METER_V2_VAL){		
		if(get_wan_index_info_hook)
			get_wan_index_info_hook(wan_index_info);
		
		for (index = 0; index < 8; index++) 
		{
			if(wan_index_info[index] == wanid)
				break;
		}
	}
	else
		index = wanid;
	
	if (index >= 8)
	{
		printk("wanid is %d, error\n", wanid);
		return; 
	}

	if(PPE_METHOD_READ == action)
	{
		memset(&orig_wan_acnt, 0, sizeof(ppeAcntGrp_t));
		memcpy(&orig_wan_acnt, &ppe_acnt_grp[index], sizeof(ppeAcntGrp_t));
		wan_account_read(index);
		ppe_acnt_grp[index].jiffies = jiffies;

 		if(orig_wan_acnt.jiffies != 0)
		{
			#ifdef TCSUPPORT_CPU_ARMV8
			tmp = (ppe_acnt_grp[index].uniTxBytes - orig_wan_acnt.uniTxBytes 
                                        + ppe_acnt_grp[index].mulTxBytes - orig_wan_acnt.mulTxBytes) * 100 * 8;
			do_div(tmp,(ppe_acnt_grp[index].jiffies - orig_wan_acnt.jiffies) * 1024);
			ppe_acnt_grp[index].txRate = tmp;
			tmp = (ppe_acnt_grp[index].uniRxBytes - orig_wan_acnt.uniRxBytes 
                                        + ppe_acnt_grp[index].mulRxBytes - orig_wan_acnt.mulRxBytes) * 100 * 8;
			do_div(tmp,(ppe_acnt_grp[index].jiffies - orig_wan_acnt.jiffies) * 1024);
			ppe_acnt_grp[index].rxRate = tmp;
			#else
			ppe_acnt_grp[index].txRate = ((ppe_acnt_grp[index].uniTxBytes - orig_wan_acnt.uniTxBytes 
                                        + ppe_acnt_grp[index].mulTxBytes - orig_wan_acnt.mulTxBytes) * 100 * 8)
                                        / ((ppe_acnt_grp[index].jiffies - orig_wan_acnt.jiffies) * 1024);
			ppe_acnt_grp[index].rxRate = ((ppe_acnt_grp[index].uniRxBytes - orig_wan_acnt.uniRxBytes 
                                        + ppe_acnt_grp[index].mulRxBytes - orig_wan_acnt.mulRxBytes) * 100 * 8)
                                        / ((ppe_acnt_grp[index].jiffies - orig_wan_acnt.jiffies) * 1024);
			#endif
		}
		memcpy(&arg->wan_acnt, &ppe_acnt_grp[index], sizeof(ppeAcntGrp_t));
	}
	else if(PPE_METHOD_CLEAR == action)
	{
		wan_account_clear(index);
	}

    return;
}

static void ppe_wan_acnt_ioctl(void *arg)
{
	struct hwnat_wan_acnt_args opt;
	struct hwnat_wan_acnt_args *argPtr = (struct hwnat_wan_acnt_args *)arg;

	memset(&opt,0,sizeof(struct hwnat_wan_acnt_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_wan_acnt_args)))
		return;

	PpeWanACnt(&opt);

	 if (copy_to_user(argPtr, &opt, sizeof(struct hwnat_wan_acnt_args)))
		return;
	return;
}

static void PpeSetBindThresholdIoctl(void *arg)
{
	struct hwnat_config_args opt4;

	memset(&opt4,0,sizeof(struct hwnat_config_args));
	if (copy_from_user(&opt4, arg, sizeof(struct hwnat_config_args)))
		return;

	PpeSetBindThreshold(opt4.bind_threshold);

	return;
}

static void PpeSetMaxEntryLimitIoctl(void *arg)
{
	struct hwnat_config_args opt4;

	memset(&opt4,0,sizeof(struct hwnat_config_args));
	if (copy_from_user(&opt4, arg, sizeof(struct hwnat_config_args)))
		return;

	PpeSetMaxEntryLimit(opt4.foe_full_lmt,opt4.foe_half_lmt, opt4.foe_qut_lmt);
	return;
}

static void PpeSetKaIntervalIoctl(void *arg)
{
	struct hwnat_config_args opt4;

	memset(&opt4,0,sizeof(struct hwnat_config_args));
	if (copy_from_user(&opt4, arg, sizeof(struct hwnat_config_args)))
		return;

	PpeSetKaInterval(opt4.foe_tcp_ka, opt4.foe_udp_ka);

	return;
}

static void PpeSetUnbindLifeTimeIoctl(void *arg)
{
	struct hwnat_config_args opt4;

	memset(&opt4,0,sizeof(struct hwnat_config_args));
	if (copy_from_user(&opt4, arg, sizeof(struct hwnat_config_args)))
		return;

	PpeSetUnbindLifeTime(opt4.foe_unb_dlta);

	return;
}

void PpeSetBindLifetimeIoctl(void *arg)
{
	struct hwnat_config_args opt4;

	memset(&opt4,0,sizeof(struct hwnat_config_args));
	if (copy_from_user(&opt4, arg, sizeof(struct hwnat_config_args)))
		return;

	PpeSetBindLifetime(opt4.foe_tcp_dlta, opt4.foe_udp_dlta,
	                opt4.foe_fin_dlta, opt4.foe_ntu_dlta);
	return;
}


static void ppe_dir_ioctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	bind_dir = opt.bind_dir;
	return;
}

static void ppe_multicast_en_ioctl(void *arg)
{
	struct hwnat_config_args opt4;

	memset(&opt4,0,sizeof(struct hwnat_config_args));
	if(copy_from_user(&opt4, arg, sizeof(struct hwnat_config_args)))
		return;

	multicast_en = opt4.enable;

	return;
}

static void ppe_ip_proto_chk_type_ioctl(void *arg)
{
	struct hwnat_config_args opt4;

	memset(&opt4,0,sizeof(struct hwnat_config_args));
	if (copy_from_user(&opt4, arg, sizeof(struct hwnat_config_args)))
		return;

	ip_proto_chk = opt4.ip_chk_type;
	setup_ip_chk(ip_proto_chk);

	return;
}

static void PpeRegDumpIoctl(void *arg)
{
    PpeRegDump();
    return;
}

static void PpeCleanTableHandlerIoctl(void *arg)
{
	PpeCleanTableHandler();

	return;
}

static void PpeCleanTableIoctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	if (opt.entry_num == 0){
		PpeCleanTableHandler();
	}else{
		FoeDelEntryByNum(opt.entry_num);
	}

	return;
}

static void ppeSnoopingSpeed_en_ioctl(void *arg)
{
	struct hwnat_config_args opt4;

	memset(&opt4,0,sizeof(struct hwnat_config_args));
	if (copy_from_user(&opt4, arg, sizeof(struct hwnat_config_args)))
		return;

	snoopingSpeed_en = opt4.snoopingEn;

	return;
}

static void SetPpeSetEthtypeIoctl(void *arg)
{
	struct hwnat_config_args opt4;

	memset(&opt4,0,sizeof(struct hwnat_config_args));
	if (copy_from_user(&opt4, arg, sizeof(struct hwnat_config_args)))
		return;

	SetPpeSetEthtype((int)opt4.L2ethtypeEN);

	return;
}

static void hwnat_clean_lan_entry_ioctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	if (TCSUPPORT_XPON_HAL_API_EXT_VAL == 1)
		hwnat_clean_lan_entry(opt.entry_num);

	return;
}

static void hwnat_clean_wan_entry_ioctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	if (TCSUPPORT_XPON_HAL_API_EXT_VAL == 1)
		hwnat_clean_wan_entry(opt.entry_num);

	return;
}

static void FoeDumpCacheDataIoctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;
	
	if (IS_IC_EN751X2X)
		FoeDumpCacheData(opt.cache_type);

	return;
}

static void FoeClearCacheIoctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;
	
	if (IS_IC_EN751X2X)
		FoeClearCache(opt.entry_num);

	return;
}

static void Ppe_L2b_Ethtype_Clean_Ioctl(void *arg)
{
	struct ppe_del_l2betype_cfg opt;

	memset(&opt,0,sizeof(struct ppe_del_l2betype_cfg));
	if (copy_from_user(&opt, arg, sizeof(struct ppe_del_l2betype_cfg)))
		return;
	
	hwnat_clean_entry_by_ethertype(opt.etype, (unsigned char)(opt.port));	
	return;
}

static void PpeSetMulHighPriorityIoctl(void *arg)
{
	struct ppe_mulhigh_cfg opt;

	memset(&opt,0,sizeof(struct ppe_mulhigh_cfg));
	if (copy_from_user(&opt, arg, sizeof(struct ppe_mulhigh_cfg)))
		return;

	Ppe_Set_Mul_High_Priority(opt.data);
	return ;
}

int ConvertStagToPID(unsigned short sp_tag)
{
	int i = 0;
	
	for(i=0; i < 6; i++)
	{
		if(sp_tag & (1 << i))
		{
			break;
		}
	}

	return i;
}

static void ppeSetMirrorEnable(unsigned char port)
{
    unsigned char switch_port = 0;
    unsigned short sptag = 0;

    if((port>0)&&(port < 5))/*port 1~4 means lan1~lan4*/
    {
        ETHER_API_GET_PORTMAP(port-1,&switch_port);
        sptag = (1<<switch_port);

        /* set MIRROR_EN */
        ppeMirrorEnable();
	    /* set MIRROR_FP */
        ppeSetMirrorFP(SP_GDMA1<<5);
	    /* set MIRROR_SPTAG */
	    ppeSetMirrorSPTAG(sptag);

    	if(PPE2_SUPPORTED){
            ppe2MirrorEnable();
            ppe2SetMirrorFP(SP_GDMA1<<5);/*9bit 4bit fp and 5bit nbq*/
    	    ppe2SetMirrorSPTAG(sptag);
    	}

    	/* ppe mirror extention */
    	if(SUPPORT_PPE_MIRROR_EXTENTION)
		    ETHER_API_SET_PORT_MIRROR(1, switch_port);
    }
    else
    {
        printk("port %d not support\n",port);
        return; 
    }
    
    return;
}

static void ppeSetMirrorDisable(void)
{
    unsigned char port_id = 0;
    
    ppeMirrorDisable();

	if(PPE2_SUPPORTED){
        ppe2MirrorDisable();
	}

	/* ppe mirror extention */
	if(SUPPORT_PPE_MIRROR_EXTENTION)
	{
	    for(port_id = 0;port_id < 5;port_id++)
	        ETHER_API_SET_PORT_MIRROR(0, port_id);
    }
    
    return;
}

static void ppeSetMirrorMode(unsigned char mode)
{
    if(!SUPPORT_PPE_MIRROR_DEC)
    {
        printk("set ppe mirror mode is not supported on this chip\n");
        return;
    }  
    
    switch(mode)
    {
        case 1:/*mirror upstream atfer NAT,downstream before NAT*/
            ppeMirrorDecClear();
            ppeMirrorDecModeOr();
            ppeMirrorDstPostEnable();
            ppeSetMirrorDst(SP_GDMA2<<5);/*9bit:4bit fp and 5bit nbq*/
            ppeMirrorSrcPreEnable();
            ppeSetMirrorSrc(SP_GDMA2);/*src gdm2*/
            break;
        case 2:/*mirror upstream atfer NAT only*/
            ppeMirrorDecClear();
            ppeMirrorDecModeOr();
            ppeMirrorDstPostEnable();
            ppeSetMirrorDst(SP_GDMA2<<5);/*9bit:4bit fp and 5bit nbq*/
            break;
        case 3:/*mirror downstream before NAT only*/
            ppeMirrorDecClear();
            ppeMirrorDecModeOr();
            ppeMirrorSrcPreEnable();
            ppeSetMirrorSrc(SP_GDMA2);/*src gdm2*/
            break;
        default:
            printk("ppe mirror mode %d wrong\n",mode);
            return;
    }
    
    return;
}

static void ppeSetMirrorEnableConfig(int enable,int fport,int nbq,int stag)
{
    printk("enable = %d,fport = %d,nbq = %d,stag = 0x%x\n",enable,fport,nbq,stag);
    ppeSetMirrorFP(((fport<<5)|nbq));
    ppeSetMirrorSPTAG(stag);
    if(enable){
        ppeMirrorEnable();
    }
    else{
        ppeMirrorDisable();
    }
    
    return;
}

/*mirror_mask:5bit bitmap dst_pre_en|dst_post_en|dst_q_en|src_pre_en|src_port_en*/
static void ppeSetMirrorModeConfig(int mode,int mirror_dst,int mirror_src,int mirror_mask)
{    
    if(!SUPPORT_PPE_MIRROR_DEC)
    {
        printk("set ppe mirror mode is not supported on this chip\n");
        return;
    }  
        
    if(mode){
        ppeMirrorDecModeOr();
    }
    else{
        ppeMirrorDecModeAnd();
    }

    ppeSetMirrorDst(mirror_dst);
    ppeSetMirrorSrc(mirror_src);

    if(mirror_mask&0x1){/*src_post_en*/
        ppeMirrorSrcPostEnable();
    }
    else{
        ppeMirrorSrcPostDisable();
    }

    if(mirror_mask&0x2){/*src_pre_en*/
        ppeMirrorSrcPreEnable();
    }
    else{
        ppeMirrorSrcPreDisable();
    }

    if(mirror_mask&0x4){/*src_dst_q_en*/
        ppeMirrorDstQEnable();
    }
    else{
        ppeMirrorDstQDisable();
    }

    if(mirror_mask&0x8){/*src_dst_post_en*/
        ppeMirrorDstPostEnable();
    }
    else{
        ppeMirrorDstPostDisable();
    }

    if(mirror_mask&0x10){/*src_dst_pre_en*/
        ppeMirrorDstPreEnable();
    }
    else{
        ppeMirrorDstPreDisable();
    }
        
    return; 
}

static void PpeMirrorCmd(void *arg)
{
	struct ppe_mirror_args opt;

    if (!PPE_MIRROR_SUPPORT)
	{
	    printk("ppe mirror not supported\n");
		return;
	}
	
	memset(&opt,0,sizeof(struct ppe_mirror_args));
	if (copy_from_user(&opt, arg, sizeof(struct ppe_mirror_args)))
		return;

    switch(opt.func_id)
    {
        case PPE_MIRROR_FUNC_ID_ENABLE:
            ppeSetMirrorEnable(opt.port);
            break;
        case PPE_MIRROR_FUNC_ID_DISABLE:
            ppeSetMirrorDisable();
            break;
        case PPE_MIRROR_FUNC_ID_MODE:
            ppeSetMirrorMode(opt.mode);
            break;
        case PPE_MIRROR_FUNC_ID_ENABLE_CONFIG:
            ppeSetMirrorEnableConfig(opt.enable_config.enable,opt.enable_config.fport,opt.enable_config.nbq,opt.enable_config.sptag);
            break;
        case PPE_MIRROR_FUNC_ID_MODE_CONFIG:
            ppeSetMirrorModeConfig(opt.mode_config.mode,opt.mode_config.mirror_dst,opt.mode_config.mirror_src,opt.mode_config.mirror_mask);
            break;
        default:
            printk("set ppe mirror function id error\n");
            return;
    }

    return;
}

static void PpeSetWanMtu(void *arg)
{
	int wan_idx = 0;
	unsigned char bit_offset = 0;
	struct ppe_wan_mtu_args opt;

	memset(&opt,0,sizeof(struct ppe_wan_mtu_args));
	if (copy_from_user(&opt, arg, sizeof(struct ppe_wan_mtu_args)))
		return;

	if (!PPE_WAN_MTU_SUPPORT)
	{
		return;
	}

	wan_idx = opt.wan_idx;
	printk("wan_idx = %d\n", wan_idx);
	
	/* set WAN MTU according to index */
	if (wan_idx % 2)
	{
		bit_offset = 16;
	}
	else
	{
		bit_offset = 0;
	}
	
	RegModifyBits(WAN_MTU_N(wan_idx), opt.wan_mtu, bit_offset, 14);
	
	return;
}

static void ecnt_ppe_tls_ioctl(void *arg)
{
    struct ppe_tls_cfg *tlsCfg = (struct ppe_tls_cfg *)arg;
    struct ppe_tls_cfg ppeTlsCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeTlsCfg, tlsCfg, sizeof(struct ppe_tls_cfg)))
		return;

    switch (ppeTlsCfg.method)
    {
        case PPE_METHOD_ADD:
            ecnt_ppe_tls_add(&ppeTlsCfg);
        break;
        case PPE_METHOD_DEL:
            ecnt_ppe_tls_del(&ppeTlsCfg);
        break;
        case PPE_METHOD_SHOW:
            ecnt_ppe_tls_show(&ppeTlsCfg);
        break;
        default:
            printk("\nWrong ppe tls method:%d\n", ppeTlsCfg.method);
            ppeTlsCfg.retVal = 1;
        return;
    }

    if (copy_to_user(tlsCfg, &ppeTlsCfg, sizeof(struct ppe_tls_cfg)))
		return;
    
    return;
}

static void ecnt_ppe_meter_ioctl(void *arg)
{
    struct ppe_meter_cfg *meterCfg = (struct ppe_meter_cfg *)arg;
    struct ppe_meter_cfg ppeMeterCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeMeterCfg, meterCfg, sizeof(struct ppe_meter_cfg)))
		return;

    switch (ppeMeterCfg.method)
    {
        case PPE_METHOD_ADD:
            ecnt_ppe_meter_add(&ppeMeterCfg);
        break;
        case PPE_METHOD_DEL:
            ecnt_ppe_meter_del(&ppeMeterCfg);
        break;
        case PPE_METHOD_SHOW:
            ecnt_ppe_meter_show(&ppeMeterCfg);
        break;
        default:
            printk("\nWrong ppe meter method:%d\n", ppeMeterCfg.method);
            ppeMeterCfg.retVal = 1;
        return;
    }

    if (copy_to_user(meterCfg, &ppeMeterCfg, sizeof(struct ppe_meter_cfg)))
		return;
    
    return;
}

static void ecnt_ppe_trtcm_ioctl(void *arg)
{
    struct ppe_meter_cfg *meterCfg = (struct ppe_meter_cfg *)arg;
    struct ppe_meter_cfg ppeMeterCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeMeterCfg, meterCfg, sizeof(struct ppe_meter_cfg)))
		return;

    switch (ppeMeterCfg.method)
    {
        case PPE_METHOD_ADD:
            ecnt_ppe_trtcm_add(&ppeMeterCfg);
        break;
        case PPE_METHOD_DEL:
            ecnt_ppe_trtcm_del(&ppeMeterCfg);
        break;
        case PPE_METHOD_SHOW:
            ecnt_ppe_trtcm_show(&ppeMeterCfg);
        break;
        default:
            printk("\nWrong ppe trtcm method:%d\n", ppeMeterCfg.method);
            ppeMeterCfg.retVal = 1;
        return;
    }

    if (copy_to_user(meterCfg, &ppeMeterCfg, sizeof(struct ppe_meter_cfg)))
		return;
    
    return;
}

static void ecnt_ppe_account_ioctl(void *arg)
{
    struct ppe_account_cfg *accountCfg = (struct ppe_account_cfg *)arg;
    struct ppe_account_cfg ppeAccountCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeAccountCfg, accountCfg, sizeof(struct ppe_account_cfg)))
		return;

    switch (ppeAccountCfg.method)
    {
        case PPE_METHOD_ADD:
            ecnt_ppe_account_add(&ppeAccountCfg);
        break;
        case PPE_METHOD_DEL:
            ecnt_ppe_account_del(&ppeAccountCfg);
        break;
        case PPE_METHOD_SHOW:
            ecnt_ppe_account_show(&ppeAccountCfg);
        break;
        default:
            printk("\nWrong ppe trtcm method:%d\n", ppeAccountCfg.method);
            ppeAccountCfg.retVal = 1;
        return;
    }

    if (copy_to_user(accountCfg, &ppeAccountCfg, sizeof(struct ppe_account_cfg)))
		return;
    
    return;
}

static void ecnt_ppe_cache_ioctl(void *arg)
{
    struct ppe_cache_cfg *cacheCfg = (struct ppe_cache_cfg *)arg;
    struct ppe_cache_cfg ppeCacheCfg;
	
    if (!IS_IC_EN751X2X)
        return;

    if (copy_from_user(&ppeCacheCfg, cacheCfg, sizeof(struct ppe_cache_cfg)))
		return;

    switch (ppeCacheCfg.method)
    {
        case CACHE_METHOD_ENABLE:
            PpeSetCacheEbl(ENABLE);
            ppeCacheCfg.retVal = 0;
        break;
        case CACHE_METHOD_DISABLE:
            PpeSetCacheEbl(DISABLE);
            ppeCacheCfg.retVal = 0;
        break;
        case CACHE_METHOD_SET_MODE:
            PpeSetCacheMode(ppeCacheCfg.mode);
            ppeCacheCfg.retVal = 0;
        break;
        case CACHE_METHOD_CLEAR:
            if (ppeCacheCfg.foeIdx < gPpeFoeNum) {
                FoeClearCache(ppeCacheCfg.foeIdx);
                ppeCacheCfg.retVal = 0;
            }
            else {
                printk("\nWrong ppe foe index:%d\n", ppeCacheCfg.foeIdx);
                ppeCacheCfg.retVal = 1;
            }
        break;
        case CACHE_METHOD_SHOW:
            if (FoeDumpCacheData(ppeCacheCfg.showType) == 1)
                ppeCacheCfg.retVal = 0;
            else
                ppeCacheCfg.retVal = 1;
        break;
        default:
            printk("\nWrong ppe cache method:%d\n", ppeCacheCfg.method);
            ppeCacheCfg.retVal = 1;
        return;
    }

    if (copy_to_user(cacheCfg, &ppeCacheCfg, sizeof(struct ppe_cache_cfg)))
		return;
    
    return;
}

static void ecnt_ppe_hash_ioctl(void *arg)
{
    struct ppe_hash_cfg *hashCfg = (struct ppe_hash_cfg *)arg;
    struct ppe_hash_cfg ppeHashCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeHashCfg, hashCfg, sizeof(struct ppe_hash_cfg)))
		return;

    switch (ppeHashCfg.method)
    {
        case HASH_METHOD_SET_MODE:
            foeHashModeSet(ppeHashCfg.mode);
            ppeHashCfg.retVal = 0;
        break;
        case HASH_METHOD_SET_BITSHIFT:
            foeHashBitShiftSet(ppeHashCfg.bitShift);
            ppeHashCfg.retVal = 0;
        break;
        case HASH_METHOD_SET_TLS:
            foeHashTlsTypeSet(ppeHashCfg.tlsDmacDis, ppeHashCfg.tlsPbitsDis, ppeHashCfg.tlsEtypeDis);
            ppeHashCfg.retVal = 0;
        break;
        case HASH_METHOD_SET_L2B:
            foeHashL2bSmacEn(ppeHashCfg.l2bSmacEn);
            ppeHashCfg.retVal = 0;
        break;
        default:
            printk("\nWrong ppe hash method:%d\n", ppeHashCfg.method);
            ppeHashCfg.retVal = 1;
        return;
    }

    if (copy_to_user(hashCfg, &ppeHashCfg, sizeof(struct ppe_hash_cfg)))
		return;
    
    return;
}

static void ecnt_ppe_local_in_ioctl(void *arg)
{
    struct ppe_local_in_cfg *localInCfg = (struct ppe_local_in_cfg *)arg;
    struct ppe_local_in_cfg ppeLocalInCfg;
	unsigned short *dport_tmp_ptr;

    if (copy_from_user(&ppeLocalInCfg, localInCfg, sizeof(struct ppe_local_in_cfg)))
		return;
	
	dport_tmp_ptr = (unsigned short *)kmalloc(ppeLocalInCfg.dport_num * sizeof(unsigned short), GFP_USER);
	if( NULL == dport_tmp_ptr )
	{	
		printk("\nNot enough space to ppe_local_in_ioctl\n");
		ppeLocalInCfg.retVal = 1;
		
		if (copy_to_user(localInCfg, &ppeLocalInCfg, sizeof(struct ppe_local_in_cfg)))
			printk("\ncopy_to_user failed\n");
		
		return;
	}

	if (copy_from_user(dport_tmp_ptr, ppeLocalInCfg.dport_ptr, ppeLocalInCfg.dport_num * sizeof(unsigned short))){
		kfree(dport_tmp_ptr);
		return;
	}
	
	ppeLocalInCfg.dport_ptr = dport_tmp_ptr;

    switch (ppeLocalInCfg.method)
    {
        case LOCAL_IN_METHOD_ENABLE:
            ppeLocalInEnable();
            ppeLocalInCfg.retVal = 0;
        break;
        case LOCAL_IN_METHOD_DISABLE:
            ppeLocalInDisable();
            ppeLocalInCfg.retVal = 0;
        break;
         case LOCAL_IN_METHOD_ADD_DPORT:
            ppeLocalInAddDport(ppeLocalInCfg.dport_ptr,ppeLocalInCfg.dport_num);
            ppeLocalInCfg.retVal = 0;
        break;
        case LOCAL_IN_METHOD_ADD_ALL:
            ppeLocalInAddAll();
            ppeLocalInCfg.retVal = 0;
        break;
        case LOCAL_IN_METHOD_DEL_DPORT:
            ppeLocalInDelDport(ppeLocalInCfg.dport_ptr,ppeLocalInCfg.dport_num);            
            ppeLocalInCfg.retVal = 0;
        break;
        case LOCAL_IN_METHOD_DEL_ALL:
            ppeLocalInDelAll();
            ppeLocalInCfg.retVal = 0;
        break;
        case LOCAL_IN_METHOD_SHOW:
            ppeLocalInShowDport();
            ppeLocalInCfg.retVal = 0;
        break;
#ifdef CONFIG_TP_IMAGE
        case LOCAL_IN_METHOD_BACKUP:
            ppeLocalInBackupDport();
            ppeLocalInCfg.retVal = 0;
        break;
        case LOCAL_IN_METHOD_RESTORE:
            ppeLocalInRestoreDport();
            ppeLocalInCfg.retVal = 0;
        break;
        case LOCAL_IN_METHOD_BACKUP_SHOW:
            ppeLocalInShowBackupList();
            ppeLocalInCfg.retVal = 0;			
        break;
#endif /* CONFIG_TP_IMAGE */
        default:
            printk("\nWrong ppe local in method:%d\n", ppeLocalInCfg.method);
            ppeLocalInCfg.retVal = 1;
        
    }

    if (copy_to_user(localInCfg, &ppeLocalInCfg, sizeof(struct ppe_local_in_cfg)))
		printk("\ncopy_to_user failed\n");
	
	kfree(ppeLocalInCfg.dport_ptr);
    return;
}

static void ecnt_ppe_meter_group_ioctl(void *arg)
{
    struct ppe_meter_cfg *meterCfg = (struct ppe_meter_cfg *)arg;
    struct ppe_meter_cfg ppeMeterCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeMeterCfg, meterCfg, sizeof(struct ppe_meter_cfg)))
		return;

    switch (ppeMeterCfg.method)
    {
        case PPE_METHOD_ADD:
            ecnt_ppe_meter_add_group(&ppeMeterCfg);
        break;
        case PPE_METHOD_DEL:
            ecnt_ppe_meter_del_group(&ppeMeterCfg);
        break;
        default:
            printk("\nWrong ppe meter method:%d\n", ppeMeterCfg.method);
            ppeMeterCfg.retVal = 1;
        return;
    }

    if (copy_to_user(meterCfg, &ppeMeterCfg, sizeof(struct ppe_meter_cfg)))
		printk("\ncopy_to_user failed\n");
    
    return;
}

static void ecnt_ppe_meter_flow_ioctl(void *arg)
{
    struct ppe_meter_cfg *meterCfg = (struct ppe_meter_cfg *)arg;
    struct ppe_meter_cfg ppeMeterCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeMeterCfg, meterCfg, sizeof(struct ppe_meter_cfg)))
		return;

    switch (ppeMeterCfg.method)
    {
        case PPE_METHOD_ADD:
            ecnt_ppe_meter_add_flow(&ppeMeterCfg);
        break;
        case PPE_METHOD_DEL:
            ecnt_ppe_meter_del_flow(&ppeMeterCfg);
        break;
        default:
            printk("\nWrong ppe meter method:%d\n", ppeMeterCfg.method);
            ppeMeterCfg.retVal = 1;
        return;
    }

    if (copy_to_user(meterCfg, &ppeMeterCfg, sizeof(struct ppe_meter_cfg)))
		printk("\ncopy_to_user failed\n");
    
    return;
}

static void ecnt_ppe_trtcm_group_ioctl(void *arg)
{
    struct ppe_meter_cfg *meterCfg = (struct ppe_meter_cfg *)arg;
    struct ppe_meter_cfg ppeMeterCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeMeterCfg, meterCfg, sizeof(struct ppe_meter_cfg)))
		return;

    switch (ppeMeterCfg.method)
    {
        case PPE_METHOD_ADD:
            ecnt_ppe_trtcm_add_group(&ppeMeterCfg);
        break;
        case PPE_METHOD_DEL:
            ecnt_ppe_trtcm_del_group(&ppeMeterCfg);
        break;
        default:
            printk("\nWrong ppe trtcm method:%d\n", ppeMeterCfg.method);
            ppeMeterCfg.retVal = 1;
        return;
    }

    if (copy_to_user(meterCfg, &ppeMeterCfg, sizeof(struct ppe_meter_cfg)))
		printk("\ncopy_to_user failed\n");
    
    return;
}

static void ecnt_ppe_trtcm_flow_ioctl(void *arg)
{
    struct ppe_meter_cfg *meterCfg = (struct ppe_meter_cfg *)arg;
    struct ppe_meter_cfg ppeMeterCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeMeterCfg, meterCfg, sizeof(struct ppe_meter_cfg)))
		return;

    switch (ppeMeterCfg.method)
    {
        case PPE_METHOD_ADD:
            ecnt_ppe_trtcm_add_flow(&ppeMeterCfg);
        break;
        case PPE_METHOD_DEL:
            ecnt_ppe_trtcm_del_flow(&ppeMeterCfg);
        break;
        default:
            printk("\nWrong ppe trtcm method:%d\n", ppeMeterCfg.method);
            ppeMeterCfg.retVal = 1;
        return;
    }

    if (copy_to_user(meterCfg, &ppeMeterCfg, sizeof(struct ppe_meter_cfg)))
		printk("\ncopy_to_user failed\n");
    
    return;
}

static void ecnt_ppe_account_stats_ioctl(void *arg)
{
    struct ppe_account_cfg *accountCfg = (struct ppe_account_cfg *)arg;
    struct ppe_account_cfg ppeAccountCfg;

	/* hw_nat_v3 support 7528/7580/7523/7581, so delete this logic */
    /* if ((!IS_IC_EN751X2X) && (!isEN7580) && (!isEN7523))
        return; */

    if (copy_from_user(&ppeAccountCfg, accountCfg, sizeof(struct ppe_account_cfg)))
		return;

    switch (ppeAccountCfg.method)
    {
        case PPE_METHOD_GET:
            ecnt_ppe_account_stats_get(&ppeAccountCfg);
        break;
        default:
            printk("\nWrong ppe trtcm method:%d\n", ppeAccountCfg.method);
            ppeAccountCfg.retVal = 1;
        return;
    }

    if (copy_to_user(accountCfg, &ppeAccountCfg, sizeof(struct ppe_account_cfg)))
		printk("\ncopy_to_user failed\n");
    
    return;
}

static void ecnt_ppe_black_ip_add_ioctl(void *arg)
{
	struct black_ip_cnt opt ;
	
	memset(&opt,0,sizeof(struct black_ip_cnt));
	if (copy_from_user(&opt, arg, sizeof(struct black_ip_cnt)))
		return;

	if(opt.ipv6_mode == 0)
		PpeBlack_ip_add(opt.ip, opt.mask, opt.type, opt.sign);
	else 
		PpeBlack_ipv6_add(opt.u6_addr8, opt.mask, opt.type, opt.sign);

	return;
}

static void ecnt_ppe_black_ip_delete_ioctl(void *arg)
{
	struct black_ip_cnt opt ;

	memset(&opt,0,sizeof(struct black_ip_cnt));
	if (copy_from_user(&opt, arg, sizeof(struct black_ip_cnt)))
		return;

	if(opt.ipv6_mode == 0)
		PpeBlack_ip_delete(opt.ip, opt.mask, opt.type, opt.sign);
	else
		PpeBlack_ipv6_delete(opt.u6_addr8, opt.mask, opt.type, opt.sign);

	return;
}

static void ecnt_ppe_black_ip_delete_sign_ioctl(void *arg)
{
	unsigned int *tmp_sign = (unsigned int*)arg;
	unsigned int sign = 0;

	if (copy_from_user(&sign, tmp_sign, sizeof(unsigned int)))
		return;

	PpeBlack_ip_delete_same_sign(sign);
    
    return;
}

static void ecnt_ppe_black_ip_port_protocol_ioctl(void *arg)
{
	struct black_ip_port_portocol *blackIpPort = (struct black_ip_port_portocol *)arg;
	struct black_ip_port_portocol ppeBlackIpPort;

	if (copy_from_user(&ppeBlackIpPort, blackIpPort, sizeof(struct black_ip_port_portocol)))
		return;

	switch(ppeBlackIpPort.action)
	{
		case PPE_BLACK_IP_PORT_PROTOCOL_ADD:
			PpeBlack_ip_port_protocol_add(ppeBlackIpPort.ip,ppeBlackIpPort.port,ppeBlackIpPort.protocol);
			ppeBlackIpPort.retVal = 0;
        break;
		case PPE_BLACK_IP_PORT_PROTOCOL_DELETE:
			PpeBlack_ip_port_protocol_delete(ppeBlackIpPort.ip,ppeBlackIpPort.port,ppeBlackIpPort.protocol);
			ppeBlackIpPort.retVal = 0;
		break;
		case PPE_BLACK_IP_PORT_PROTOCOL_SHOW:
			PpeBlack_ip_port_protocol_show();
			ppeBlackIpPort.retVal = 0;
		break;
		case PPE_BLACK_IP_PORT_PROTOCOL_DELETE_ALL:
			PpeBlack_ip_port_protocol_delete_all();
			ppeBlackIpPort.retVal = 0;
		break;
		default:
			printk("\nWrong ppe black ip port action:%d\n", ppeBlackIpPort.action);
            ppeBlackIpPort.retVal = 1;
		return;	
	}

	if (copy_to_user(blackIpPort, &ppeBlackIpPort, sizeof(struct black_ip_port_portocol)))
		printk("\ncopy_to_user failed\n");
    
    return;
}
static void PpeUnMultiDropByIndex_ioctl(void *arg)
{
	struct hwnat_args opt ;
	int ret = 0;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	ret = PpeUnMultiDropByIndex(&opt.multi_drop);

	if (ret == 1)
	{
		PpeMultilistAddDrop(&opt.multi_drop);
	}	

	return;
}

static void PpeQueryMultiDropList_ioctl(void *arg)
{
	struct hwnat_args opt;

	memset(&opt,0,sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	PpeQueryMultiDropList(&opt.multi_drop);
	
	return;
}

static void ppe_unknow_mc_cnt_ioctl(void *arg)
{
    struct hwnat_byte_cnt_args unknow_mc_cnt;
    unsigned int cnt_lo = 0, cnt_hi = 0, cnt_pkt = 0;

	memset(&unknow_mc_cnt,0,sizeof(struct hwnat_byte_cnt_args));
	if (copy_from_user(&unknow_mc_cnt, arg, sizeof(struct hwnat_byte_cnt_args)))
		return;

    if(PPE_COUNT_READ == unknow_mc_cnt.method){
        cnt_lo = RegRead(AC_BASE + (UNKNOW_MC_ACNT_ID << 4));
    	cnt_hi = RegRead(AC_BASE + (UNKNOW_MC_ACNT_ID << 4) + 4);
        unknow_mc_byte_cnt += ((((unsigned long long)cnt_hi)<<32)+ cnt_lo);

        cnt_pkt = RegRead(AC_BASE + (UNKNOW_MC_ACNT_ID << 4) + 8);
        unknow_mc_pkt_cnt += cnt_pkt;
            
        unknow_mc_cnt.count_byte = unknow_mc_byte_cnt;
        unknow_mc_cnt.count_pkt = unknow_mc_pkt_cnt;
    }
    else
    {
        RegRead(AC_BASE + (UNKNOW_MC_ACNT_ID << 4));
    	RegRead(AC_BASE + (UNKNOW_MC_ACNT_ID << 4) + 4);
        RegRead(AC_BASE + (UNKNOW_MC_ACNT_ID << 4) + 8);
        unknow_mc_byte_cnt = 0;
        unknow_mc_pkt_cnt = 0;
    }
    
    return;
}

static void ecnt_ppe_fast_rule_ioctl(void *arg)
{
	struct hwnat_fast_rule_args opt;
	memset(&opt,0,sizeof(struct hwnat_fast_rule_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_fast_rule_args)))
		return;

	switch(opt.method)
	{
		case FAST_RULE_METHOD_ENABLE:
			ppeFastRuleEnable();
			break;
		case FAST_RULE_METHOD_DISABLE:
			ppeFastRuleDisable();
			break;
		case FAST_RULE_METHOD_SET_MARK_BIT:
			ppeFastRuleSetMarkBit(opt.value);
			break;
		case FAST_RULE_METHOD_SHOW:
			ppeShowFastRuleList();
			break;
		case FAST_RULE_METHOD_SET_MARK_ENABLE:
			ppeFastRuleSetMarkEnable(opt.value);
			break;
		case FAST_RULE_METHOD_SET_REVERSE_ENABLE:
			ppeFastRuleSetReverseEnable(opt.value);
			break;
		default:
			break;
	}
	
	return;
}
static void ecnt_ppe_get_multicast_status_ioctl(void *arg)
{
	if(isEN7528)
	{
		unsigned int* mul_status_p = (unsigned int*)arg; 
		unsigned int mul_status;
		if (copy_from_user(&mul_status, mul_status_p, sizeof(mul_status)))
			return;
		
		mul_status = PPE_Mul_Status();

		if (copy_to_user(mul_status_p, &mul_status, sizeof(mul_status)))
			printk("\ncopy_to_user failed\n");
		
		return ;
	}else{
		printk("Not Support this IC yet\n");
	}
}
static void ecnt_ppe_set_CDS_dlf_ioctl (void *arg)
{
	struct hwnat_CDS_all_ratelimit dlf_cfg;
	struct hwnat_CDS_all_ratelimit* cfg =  (struct hwnat_CDS_all_ratelimit*)arg;
	
	if (copy_from_user(&dlf_cfg, cfg, sizeof(struct hwnat_CDS_all_ratelimit)))
		return;
	
if(isEN7580 ||isEN7528){/*Only support hwnat v3*/
	if(Ppe_set_CDS_dlf_mode(dlf_cfg.dlf_flag)){
		dlf_cfg.retVal = 1;
		printk("Set CDS Dlf All ratelimit mode %d Succ.\n",dlf_cfg.dlf_flag);
		if (copy_to_user(cfg, &dlf_cfg, sizeof(struct hwnat_CDS_all_ratelimit)))
			printk("\ncopy_to_user failed\n");
		
		return ;
	}
}
	dlf_cfg.retVal =0;
	if (copy_to_user(cfg, &dlf_cfg, sizeof(struct hwnat_CDS_all_ratelimit)))
		printk("\ncopy_to_user failed\n");

	return ;
}

static void ecnt_ppe_set_CDS_unknown_mul_ioctl(void* arg)
{
	struct hwnat_CDS_all_ratelimit Unmul_cfg;
	struct hwnat_CDS_all_ratelimit* cfg =  (struct hwnat_CDS_all_ratelimit*)arg;
	
	if (copy_from_user(&Unmul_cfg, cfg, sizeof(struct hwnat_CDS_all_ratelimit)))
		return;
	
if(isEN7580 ||isEN7528){/*Only support hwnat v3*/
	if(Ppe_set_CDS_Unmul_mode(Unmul_cfg.UN_Mul_flag)){
		Unmul_cfg.retVal = 1;
		printk("Set CDS Unknown Mul All ratelimit mode %d Succ. \n",Unmul_cfg.UN_Mul_flag);
		if (copy_to_user(cfg, &Unmul_cfg, sizeof(struct hwnat_CDS_all_ratelimit)))
			printk("\ncopy_to_user failed\n");
		
		return ;
	}
}
	Unmul_cfg.retVal =0;
	if (copy_to_user(cfg, &Unmul_cfg, sizeof(struct hwnat_CDS_all_ratelimit)))
		printk("\ncopy_to_user failed\n");

	return ;

}

static void ecnt_ppe_clean_uni_entry_ioctl(void *arg)
{
	hwnat_clean_unicast_entry();
	return;
}

static void ecnt_ppe_ppe2_cmd_ioctl(void *arg)
{
    struct hwnat_ppe2_cmd_args tmp_arg;

    if(!PPE2_SUPPORTED)
    {
        printk("do not support PPE2 in this IC\n");
        return;
    }

    memset(&tmp_arg,0,sizeof(struct hwnat_ppe2_cmd_args));

    if(copy_from_user(&tmp_arg, arg, sizeof(struct hwnat_ppe2_cmd_args)))
        return;

    switch(tmp_arg.function_id)
    {
        case HWNAT_PPE2_CMD_ID_ENABLE:
            printk("enable ppe2..\n");
            ppe_ioctl_enable_ppe2();
            printk("done\n");
            break;
        case HWNAT_PPE2_CMD_ID_DISABLE:
            printk("disable ppe2..\n");
            ppe_ioctl_disable_ppe2();
            printk("done\n");
            break;
    }

    return;
}

static void ecnt_ppe_vxlan_hash_cmd_ioctl(void *arg)
{
    struct hwnat_vxlan_hash_cmd_args tmp_arg;

    memset(&tmp_arg,0,sizeof(struct hwnat_vxlan_hash_cmd_args));

    if(copy_from_user(&tmp_arg, arg, sizeof(struct hwnat_vxlan_hash_cmd_args)))
        return;

    switch(tmp_arg.function_id)
    {
        case HWNAT_VXLAN_HASH_CMD_ID_ENABLE:
            printk("enable vxlan hash..\n");
            ppe_ioctl_enable_vxlan_hash();
            printk("done\n");
            break;
        case HWNAT_VXLAN_HASH_CMD_ID_DISABLE:
            printk("disable vxlan hash..\n");
            ppe_ioctl_disable_vxlan_hash();
            printk("done\n");
		case HWNAT_VXLAN_HASH_CMD_ID_SHOW:
            printk("Show vxlan hash..\n");
            ppe_ioctl_get_vxlan_hash();
            printk("done\n");
            break;
    }

    return;
}

static void ecnt_ppe_l2tp_hash_cmd_ioctl(void *arg)
{
    struct hwnat_vxlan_hash_cmd_args tmp_arg;

    memset(&tmp_arg,0,sizeof(struct hwnat_l2tp_hash_cmd_args));

    if(copy_from_user(&tmp_arg, arg, sizeof(struct hwnat_l2tp_hash_cmd_args)))
        return;

    switch(tmp_arg.function_id)
    {
        case HWNAT_L2TP_HASH_CMD_ID_ENABLE:
            printk("enable l2tp hash..\n");
            ppe_ioctl_enable_l2tp_hash();
            printk("done\n");
            break;
        case HWNAT_L2TP_HASH_CMD_ID_DISABLE:
            printk("disable l2tp hash..\n");
            ppe_ioctl_disable_l2tp_hash();
            printk("done\n");
		case HWNAT_L2TP_HASH_CMD_ID_SHOW:
            printk("Show l2tp hash..\n");
            ppe_ioctl_get_l2tp_hash();
            printk("done\n");
            break;
    }

    return;
}

#ifdef CONFIG_COMPAT
static void ppe_debug_compat_ioctl(void *arg)
{
	struct hwnat_args opt;
	struct hwnat_debug_args debug;

	memset(&opt, 0, sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	memset(&debug, 0, sizeof(struct hwnat_debug_args));
	if (copy_from_user(&debug, compat_ptr(opt.compat_ptr), sizeof(struct hwnat_debug_args)))
		return;

	switch(debug.id)
	{
		case HWNAT_DBG_ID_SET_DBGLVL:
			DebugLevel = debug.dbg_lvl;
			printk("set hwnat debug level:%d\n",DebugLevel);
			break;
		case HWNAT_DBG_ID_SET_BINDFAILIDX:
			hwnat_dbg_bindfail_idx = debug.bind_fail_idx;
			printk("set hwnat bind fail index:%d\n",hwnat_dbg_bindfail_idx);
			break;
		case HWNAT_DBG_ID_SET_FREEIDX:
			hwnat_dgb_free_idx = debug.free_idx;
			printk("set hwnat free index:%d\n",hwnat_dgb_free_idx);
			break;
		case HWNAT_DBG_ID_SET_DROPIDX:
			hwnat_dgb_drop_idx = debug.drop_idx;
			printk("set hwnat drop index:%d\n",hwnat_dgb_drop_idx);
			FoeDelEntryByNum(hwnat_dgb_drop_idx);
			break;
		case HWNAT_DBG_ID_SET_DUMP_PARSEINFO:
			hwnat_dbg_parse_dump = debug.parse_dump_enable;
			printk("set hwnat parse dump:%d\n",hwnat_dbg_parse_dump);
			break;
		default:
			printk("hwnat debug id error\n");
	}

	return;
}
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
long HwNatIoctl(struct file *file, unsigned int cmd, unsigned long arg)
#else
int
HwNatIoctl(struct inode *inode, struct file *filp,
	   unsigned int cmd, unsigned long arg)
#endif
{
	ppe_ioctl_funcs[cmd]((void*)arg);
	return 0;
}

#ifdef CONFIG_COMPAT
long HwNatCompatIoctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	ppe_compat_ioctl_funcs[cmd]((void*)compat_ptr(arg));
	return 0;
}
#endif

struct file_operations hw_nat_fops = {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35)
      unlocked_ioctl:HwNatIoctl,
#else
      ioctl:HwNatIoctl,
#endif

#ifdef CONFIG_COMPAT
      compat_ioctl:HwNatCompatIoctl,
#endif
};

void PpeTrafficQosFlowCnt(struct hwnat_tr_qos_flow_cnt *opt)
{
	uint acntId, idx ;
	uint8_t acnt_mode = 0;
	uint32_t cnt32_lo = 0, cnt32_hi = 0;
	unchar dir = 0;

	if(SUPPORT_FLOW_ACNT){
		idx = opt->flowid;
		dir = opt->direction? (DOWN_STREAM) : (UP_STREAM);
		FE_API_GET_ACNT1_MODE(dir, &acnt_mode);
		if(WAN_ITF_AND_TRAFFIC_FLOW_MODE == acnt_mode)
			idx += WAN_ITF_HAS_FLOW_ACNT_NUM;
		FE_API_GET_FlOW_CNT(idx, dir, acnt_mode, &cnt32_lo, &cnt32_hi);
		opt->cntbyte = (((unsigned long long)cnt32_hi)<<32) + cnt32_lo;
	}else if(isEN7528){
		idx = opt->flowid;
		dir = opt->direction? (DOWN_STREAM) : (UP_STREAM);
		FE_API_GET_FlOW_CNT(idx, dir, TRAFFIC_FLOW_MODE, &cnt32_lo, &cnt32_hi);
		opt->cntbyte = (((unsigned long long)cnt32_hi)<<32) + cnt32_lo;
	}else{
		idx = opt->direction?(opt->flowid + up_queue_num):(opt->flowid);
		if(idx >= up_down_queue_num)
			return;
		acntId = idx + up_queue_cnt_start;
		if ((IS_IC_EN751X2X)){
			FE_API_GET_AC_GROUP_BYTE_CNT(acntId, &cnt32_lo, &cnt32_hi);
			queueRxBytes[idx] = ((((unsigned long long)cnt32_hi)<<32) + cnt32_lo);
		}else{
			cnt32_lo = RegRead(AC_BASE + (acntId << 4));
			cnt32_hi = RegRead(AC_BASE + (acntId << 4) + 4);
			queueRxBytes[idx] += ((((unsigned long long)cnt32_hi)<<32) + cnt32_lo);
		}
		opt->cntbyte = queueRxBytes[idx];
	}
	
	return;
}
int PpeRegIoctlHandler(void)
{

	int result = 0;
	result = register_chrdev(HW_NAT_MAJOR, HW_NAT_DEVNAME, &hw_nat_fops);
	if (result < 0) {
		NAT_PRINT(KERN_WARNING "hw_nat: can't get major %d\n",
			  HW_NAT_MAJOR);
		return result;
	}

	if (HW_NAT_MAJOR == 0) {
		printk("HNAT Major num=%d\n", result);
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
	hnat_result = result;
	hnat_device = ecnt_device_create(result);
#endif	
	return 0;
}


void PpeUnRegIoctlHandler(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)		
	ecnt_device_delete(hnat_result);
#endif	
	unregister_chrdev(HW_NAT_MAJOR, HW_NAT_DEVNAME);
}

#if defined (CONFIG_HNAT_V2)
int32_t PpeGetAGCnt(struct hwnat_ac_args * opt3)
{
	int32_t cnt_hi;
#ifdef TCSUPPORT_RA_HWNAT
        if ((IS_IC_EN751X2X) || (SUPPORT_FLOW_ACNT)){
        	FE_API_GET_AC_GROUP_PKT_CNT(opt3->ag_index, &(opt3->ag_pkt_cnt));
        	FE_API_GET_AC_GROUP_BYTE_CNT(opt3->ag_index, &(opt3->ag_byte_cnt), &cnt_hi);
        }else{
        	opt3->ag_pkt_cnt = RegRead(AC_BASE + (opt3->ag_index << 4) + 8);
        	opt3->ag_byte_cnt = RegRead(AC_BASE + (opt3->ag_index << 4));  //low 32 bit
        }
#else
        opt3->ag_pkt_cnt = RegRead(AC_BASE + opt3->ag_index * 8);       /* Low bytes */
        opt3->ag_byte_cnt = RegRead(AC_BASE + opt3->ag_index * 8 + 4);  /* High bytes */
#endif
        return HWNAT_SUCCESS;
}
#else
int PpeSetDscpRemarkEbl(uint32_t enable)
{
	RegModifyBits(PPE_GLO_CFG, enable, 11, 1);
	
	if(PPE2_SUPPORTED)
		RegModifyBits(PPE2_GLO_CFG, enable, 11, 1);
	return HWNAT_SUCCESS;
}

int PpeSetVpriRemarkEbl(uint32_t enable)
{
	/* Re-generate VLAN Priority */
	RegModifyBits(PPE_GLO_CFG, enable, 10, 1);
	if(PPE2_SUPPORTED)
		RegModifyBits(PPE2_GLO_CFG, enable, 10, 1);
	return HWNAT_SUCCESS;
}

int PpeSetWeightFOE(uint32_t weight)
{
	/* Set weight of decision in resolution */
	RegModifyBits(UP_RES, weight, FUP_WT_OFFSET, 3);
	return HWNAT_SUCCESS;
}

int PpeSetWeightACL(uint32_t weight)
{
	/* Set weight of decision in resolution */
	RegModifyBits(UP_RES, weight, AUP_WT_OFFSET, 3);
	return HWNAT_SUCCESS;
}

int PpeSetWeightDSCP(uint32_t weight)
{
	RegModifyBits(UP_RES, weight, DUP_WT_OFFSET, 3);
	return HWNAT_SUCCESS;
}

int PpeSetWeightVPRI(uint32_t weight)
{
	/* Set weight of decision in resolution */
	RegModifyBits(UP_RES, weight, VUP_WT_OFFSET, 3);
	return HWNAT_SUCCESS;
}

int PpeSetDSCP_UP(uint32_t DSCP_SET, unsigned char UP)
{
	int DSCP_UP;

	DSCP_UP = ((UP << 0) | (UP << 4) | (UP << 8) | (UP << 12)
		   | (UP << 16) | (UP << 20) | (UP << 24) | (UP << 28));
	/* Set DSCP to User priority mapping table */
	switch (DSCP_SET) {
	case 0:
		RegWrite(DSCP0_7_MAP_UP, DSCP_UP);
		break;
	case 1:
		RegWrite(DSCP8_15_MAP_UP, DSCP_UP);
		break;
	case 2:
		RegWrite(DSCP16_23_MAP_UP, DSCP_UP);
		break;
	case 3:
		RegWrite(DSCP24_31_MAP_UP, DSCP_UP);
		break;
	case 4:
		RegWrite(DSCP32_39_MAP_UP, DSCP_UP);
		break;
	case 5:
		RegWrite(DSCP40_47_MAP_UP, DSCP_UP);
		break;
	case 6:
		RegWrite(DSCP48_55_MAP_UP, DSCP_UP);
		break;
	case 7:
		RegWrite(DSCP56_63_MAP_UP, DSCP_UP);
		break;
	default:

		break;
	}
	return HWNAT_SUCCESS;
}

int PpeSetUP_IDSCP(uint32_t UP, uint32_t IDSCP)
{
	/* Set mapping table of user priority to in-profile DSCP */
	switch (UP) {
	case 0:
		RegModifyBits(UP0_3_MAP_IDSCP, IDSCP, 0, 6);
		break;
	case 1:
		RegModifyBits(UP0_3_MAP_IDSCP, IDSCP, 8, 6);
		break;
	case 2:
		RegModifyBits(UP0_3_MAP_IDSCP, IDSCP, 16, 6);
		break;
	case 3:
		RegModifyBits(UP0_3_MAP_IDSCP, IDSCP, 24, 6);
		break;
	case 4:
		RegModifyBits(UP4_7_MAP_IDSCP, IDSCP, 0, 6);
		break;
	case 5:
		RegModifyBits(UP4_7_MAP_IDSCP, IDSCP, 8, 6);
		break;
	case 6:
		RegModifyBits(UP4_7_MAP_IDSCP, IDSCP, 16, 6);
		break;
	case 7:
		RegModifyBits(UP4_7_MAP_IDSCP, IDSCP, 24, 6);
		break;
	default:
		break;
	}
	return HWNAT_SUCCESS;
}

int PpeSetUP_ODSCP(uint32_t UP, uint32_t ODSCP)
{
	/* Set mapping table of user priority to out-profile DSCP */
	switch (UP) {
	case 0:
		RegModifyBits(UP0_3_MAP_ODSCP, ODSCP, 0, 6);
		break;
	case 1:
		RegModifyBits(UP0_3_MAP_ODSCP, ODSCP, 8, 6);
		break;
	case 2:
		RegModifyBits(UP0_3_MAP_ODSCP, ODSCP, 16, 6);
		break;
	case 3:
		RegModifyBits(UP0_3_MAP_ODSCP, ODSCP, 24, 6);
		break;
	case 4:
		RegModifyBits(UP4_7_MAP_ODSCP, ODSCP, 0, 6);
		break;
	case 5:
		RegModifyBits(UP4_7_MAP_ODSCP, ODSCP, 8, 6);
		break;
	case 6:
		RegModifyBits(UP4_7_MAP_ODSCP, ODSCP, 16, 6);
		break;
	case 7:
		RegModifyBits(UP4_7_MAP_ODSCP, ODSCP, 24, 6);
		break;
	default:
		break;
	}
	return HWNAT_SUCCESS;
}

int PpeSetUP_VPRI(uint32_t UP, uint32_t VPRI)
{
	/* Set mapping table of user priority to vlan priority */
	switch (UP) {
	case 0:
		RegModifyBits(UP_MAP_VPRI, VPRI, 0, 3);
		break;
	case 1:
		RegModifyBits(UP_MAP_VPRI, VPRI, 4, 3);
		break;
	case 2:
		RegModifyBits(UP_MAP_VPRI, VPRI, 8, 3);
		break;
	case 3:
		RegModifyBits(UP_MAP_VPRI, VPRI, 12, 3);
		break;
	case 4:
		RegModifyBits(UP_MAP_VPRI, VPRI, 16, 3);
		break;
	case 5:
		RegModifyBits(UP_MAP_VPRI, VPRI, 20, 3);
		break;
	case 6:
		RegModifyBits(UP_MAP_VPRI, VPRI, 24, 3);
		break;
	case 7:
		RegModifyBits(UP_MAP_VPRI, VPRI, 28, 3);
		break;
	default:
		break;
	}
	return HWNAT_SUCCESS;
}

int PpeSetUP_AC(uint32_t UP, uint32_t AC)
{
	/* Set mapping table of user priority to access category */
	switch (UP) {
	case 0:
		RegModifyBits(UP_MAP_AC, AC, 0, 2);
		break;
	case 1:
		RegModifyBits(UP_MAP_AC, AC, 2, 2);
		break;
	case 2:
		RegModifyBits(UP_MAP_AC, AC, 4, 2);
		break;
	case 3:
		RegModifyBits(UP_MAP_AC, AC, 6, 2);
		break;
	case 4:
		RegModifyBits(UP_MAP_AC, AC, 8, 2);
		break;
	case 5:
		RegModifyBits(UP_MAP_AC, AC, 10, 2);
		break;
	case 6:
		RegModifyBits(UP_MAP_AC, AC, 12, 2);
		break;
	case 7:
		RegModifyBits(UP_MAP_AC, AC, 14, 2);
		break;
	default:
		break;
	}
	return HWNAT_SUCCESS;
}

int PpeSetSchMode(uint32_t policy)
{
	/* Set GDMA1&2 Schduling Mode */
	RegModifyBits(FE_GDMA1_SCH_CFG, policy, 24, 2);
	RegModifyBits(FE_GDMA2_SCH_CFG, policy, 24, 2);

	return HWNAT_SUCCESS;
}

/* In general case, we only need 1/2/4/8 weight */
int PpeWeightRemap(uint8_t W)
{
	switch (W) {
	case 8:
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3883)
		return 3;
#else
		return 7;
#endif
	case 4:
#if defined (CONFIG_RALINK_RT3052) || defined (CONFIG_RALINK_RT3883)
		return 2;
#else
		return 3;
#endif
	case 2:
		return 1;
	case 1:
		return 0;
	default:
		/* invalid value */
		return -1;
	}
}

int PpeSetSchWeight(uint8_t W0, uint8_t W1, uint8_t W2, uint8_t W3)
{
	int32_t _W0, _W1, _W2, _W3;

	_W0 = PpeWeightRemap(W0);
	_W1 = PpeWeightRemap(W1);
	_W2 = PpeWeightRemap(W2);
	_W3 = PpeWeightRemap(W3);

	if ((_W0 == -1) || (_W1 == -1) || (_W2 == -1) || (_W3 == -1)) {
		return HWNAT_FAIL;
	}

	/* Set GDMA1 Schduling Weight */
	RegModifyBits(FE_GDMA1_SCH_CFG, _W0, 0, 3);
	RegModifyBits(FE_GDMA1_SCH_CFG, _W1, 4, 3);
	RegModifyBits(FE_GDMA1_SCH_CFG, _W2, 8, 3);
	RegModifyBits(FE_GDMA1_SCH_CFG, _W3, 12, 3);

	/* Set GDMA2 Schduling Weight */
	RegModifyBits(FE_GDMA2_SCH_CFG, _W0, 0, 3);
	RegModifyBits(FE_GDMA2_SCH_CFG, _W1, 4, 3);
	RegModifyBits(FE_GDMA2_SCH_CFG, _W2, 8, 3);
	RegModifyBits(FE_GDMA2_SCH_CFG, _W3, 12, 3);

	return HWNAT_SUCCESS;
}

void PpeRstPreAclPtr(void)
{
	RegModifyBits(PPE_PRE_ACL, 0, 0, 9);
	RegModifyBits(PPE_PRE_ACL, 0, 16, 9);
}

void PpeRstPreAcPtr(void)
{
	RegModifyBits(PPE_PRE_AC, pre_ac_start_addr, 0, 9);
	RegModifyBits(PPE_PRE_AC, pre_ac_start_addr, 16, 9);
}

void PpeRstPostAcPtr(void)
{
	RegModifyBits(PPE_POST_AC, post_ac_start_addr, 0, 9);
	RegModifyBits(PPE_POST_AC, post_ac_start_addr, 16, 9);
}

void PpeRstPreMtrPtr(void)
{
	RegModifyBits(PPE_PRE_MTR, pre_mtr_start_addr, 0, 9);
	RegModifyBits(PPE_PRE_MTR, pre_mtr_start_addr, 16, 9);
}

void PpeRstPostMtrPtr(void)
{
	RegModifyBits(PPE_POST_MTR, post_mtr_start_addr, 0, 9);
	RegModifyBits(PPE_POST_MTR, post_mtr_start_addr, 16, 9);
}

int
PpeSetRuleSize(uint16_t pre_acl, uint16_t pre_meter, uint16_t pre_ac,
	       uint16_t post_meter, uint16_t post_ac)
{


	pre_acl_start_addr  = 0;
	pre_ac_start_addr   = 0 + pre_acl;
	post_ac_start_addr  = 0 + pre_acl + pre_meter;
	pre_mtr_start_addr  = 0 + pre_acl + pre_meter + pre_ac;
	post_mtr_start_addr = 0 + pre_acl + pre_meter + pre_ac + post_meter;
	  
	/* Set Pre ACL Table */
	RegModifyBits(PPE_PRE_ACL, 0, 0, 9);
	RegModifyBits(PPE_PRE_ACL, 0, 16, 9);

	/* Set Pre AC Table */
	RegModifyBits(PPE_PRE_AC, 0 + pre_acl, 0, 9);
	RegModifyBits(PPE_PRE_AC, 0 + pre_acl, 16, 9);

	/* Set Post AC Table */
	RegModifyBits(PPE_POST_AC, 0 + pre_acl + pre_meter, 0, 9);
	RegModifyBits(PPE_POST_AC, 0 + pre_acl + pre_meter, 16, 9);

	/* Set Pre MTR Table */
	RegModifyBits(PPE_PRE_MTR, 0 + pre_acl + pre_meter + pre_ac, 0, 9);
	RegModifyBits(PPE_PRE_MTR, 0 + pre_acl + pre_meter + pre_ac, 16, 9);

	/* Set Post MTR Table */
	RegModifyBits(PPE_POST_MTR,
		      0 + pre_acl + pre_meter + pre_ac + post_meter, 0, 9);
	RegModifyBits(PPE_POST_MTR,
		      0 + pre_acl + pre_meter + pre_ac + post_meter, 16, 9);

	return HWNAT_SUCCESS;
}
#endif

int PpeSetBindThreshold(uint32_t threshold)
{
	/* Set reach bind rate for unbind state */
	RegWrite(PPE_FOE_BNDR, threshold);
	if(PPE2_SUPPORTED){
    	RegWrite(PPE2_FOE_BNDR, threshold);
    }
	
	/* Set L2B reach bind rate for unbind state */
	RegModifyBits(PPE_FOE_BNDR,threshold,16,16);
	if(PPE2_SUPPORTED){
    	RegModifyBits(PPE2_FOE_BNDR,threshold,16,16);
    }

	return HWNAT_SUCCESS;
}

int PpeSetMaxEntryLimit(uint32_t full, uint32_t half, uint32_t qurt)
{
	/* Allowed max entries to be build during a time stamp unit */

	/* smaller than 1/4 of total entries */
	RegModifyBits(PPE_FOE_LMT1, qurt, 0, 14);

	/* between 1/2 and 1/4 of total entries */
	RegModifyBits(PPE_FOE_LMT1, half, 16, 14);

	/* between full and 1/2 of total entries */
	RegModifyBits(PPE_FOE_LMT2, full, 0, 14);

	if(PPE2_SUPPORTED){
    	/* smaller than 1/4 of total entries */
		RegModifyBits(PPE2_FOE_LMT1, qurt, 0, 14);

		/* between 1/2 and 1/4 of total entries */
		RegModifyBits(PPE2_FOE_LMT1, half, 16, 14);

		/* between full and 1/2 of total entries */
		RegModifyBits(PPE2_FOE_LMT2, full, 0, 14);
    }

	return HWNAT_SUCCESS;
}

int PpeSetKaInterval(uint8_t tcp_ka, uint8_t udp_ka)
{
	/* Keep alive time for bind FOE TCP entry */
	RegModifyBits(PPE_FOE_KA, tcp_ka, 16, 8);

	/* Keep alive timer for bind FOE UDP entry */
	RegModifyBits(PPE_FOE_KA, udp_ka, 24, 8);

	if(PPE2_SUPPORTED){
    	/* Keep alive time for bind FOE TCP entry */
		RegModifyBits(PPE2_FOE_KA, tcp_ka, 16, 8);

		/* Keep alive timer for bind FOE UDP entry */
		RegModifyBits(PPE2_FOE_KA, udp_ka, 24, 8);
    }

	return HWNAT_SUCCESS;
}

int PpeSetUnbindLifeTime(uint8_t lifetime)
{
	/* set Delta time for aging out an unbind FOE entry */
	RegModifyBits(PPE_FOE_UNB_AGE, lifetime, 0, 8);

    if(PPE2_SUPPORTED)
    {
        RegModifyBits(PPE2_FOE_UNB_AGE, lifetime, 0, 8);
    }

	return HWNAT_SUCCESS;
}
#ifdef TCSUPPORT_RA_HWNAT
int PpeSetBindLifetime(uint16_t tcp_life, uint16_t udp_life, uint16_t fin_life, uint16_t ntu_life)
{

	/* set Delta time for aging out an bind UDP FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE0, ntu_life, 16, 15);

	/* set Delta time for aging out an bind UDP FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE0, udp_life, 0, 15);

	/* set Delta time for aging out an bind TCP FIN FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE1, fin_life, 16, 15);

	/* set Delta time for aging out an bind TCP FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE1, tcp_life, 0, 15);

    if(PPE2_SUPPORTED)
    {
        /* set Delta time for aging out an bind UDP FOE entry */
        RegModifyBits(PPE2_FOE_BND_AGE0, ntu_life, 16, 15);

        /* set Delta time for aging out an bind UDP FOE entry */
        RegModifyBits(PPE2_FOE_BND_AGE0, udp_life, 0, 15);

        /* set Delta time for aging out an bind TCP FIN FOE entry */
        RegModifyBits(PPE2_FOE_BND_AGE1, fin_life, 16, 15);

        /* set Delta time for aging out an bind TCP FOE entry */
        RegModifyBits(PPE2_FOE_BND_AGE1, tcp_life, 0, 15);
    }

	return HWNAT_SUCCESS;

}

#else
int PpeSetBindLifetime(uint16_t tcp_life, uint16_t udp_life, uint16_t fin_life)
{
	/* set Delta time for aging out an bind UDP FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE0, udp_life, 0, 16);

	/* set Delta time for aging out an bind TCP FIN FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE1, fin_life, 16, 16);

	/* set Delta time for aging out an bind TCP FOE entry */
	RegModifyBits(PPE_FOE_BND_AGE1, tcp_life, 0, 16);

	return HWNAT_SUCCESS;
}
#endif

static void ecnt_ppe_set_nptv6_prefix(void* arg)
{
	struct hwnat_nptv6_prefix_args prefix_arg;

	memset(&prefix_arg,0,sizeof(struct hwnat_nptv6_prefix_args));
	
	if(copy_from_user(&prefix_arg, arg, sizeof(struct hwnat_nptv6_prefix_args)))
		return;

	if(sizeof(prefix_arg.ifname) <= 16){
		prefix_arg.ifname[15] = '\0';
		if(HWNAT_NPTV6_PREFIX_ID_ADD == prefix_arg.function_id)
			ppeNptV6AddPrefixHw(prefix_arg.ifname,prefix_arg.s_prefix,prefix_arg.s_prefix_len,prefix_arg.d_prefix,prefix_arg.d_prefix_len);
		else if(HWNAT_NPTV6_PREFIX_ID_DEL == prefix_arg.function_id)	
			ppeNptV6DelPrefixHw(prefix_arg.ifname);
		else if(HWNAT_NPTV6_PREFIX_ID_DEL_ALL == prefix_arg.function_id)
			ppeNptV6DelPrefixHwAll();
	}
	
	return;
}

static void ecnt_ppe_do_nptv6_mode(void* arg)
{
	struct hwnat_nptv6_mode_args mode_arg;

	memset(&mode_arg,0,sizeof(struct hwnat_nptv6_mode_args));
	
	if(copy_from_user(&mode_arg, arg, sizeof(struct hwnat_nptv6_mode_args)))
		return;

	if(HWNAT_NPTV6_MODE_ID_SET == mode_arg.function_id)
	{
		if((0 != mode_arg.mode)&&(1 != mode_arg.mode))
		{
			printk("nptv6 mode %d error,should be 0 or 1\n",mode_arg.mode);
			return;
		}

		ppeSetNptV6Mode(mode_arg.mode);
	}
	else if(HWNAT_NPTV6_MODE_ID_GET == mode_arg.function_id)
	{
		mode_arg.mode = ppeGetNptV6Mode();
		printk("mode_arg.mode = %d\n",mode_arg.mode);
		if(copy_to_user(arg, &mode_arg, sizeof(struct hwnat_nptv6_mode_args)))
			return;
	}
	
	return;
}

static void hwnat_set_enable_status_ioctl(void *arg)
{
	struct hwnat_args opt;
    unsigned int enable_status;

    memset(&opt, 0, sizeof(struct hwnat_args));
	if (copy_from_user(&opt, arg, sizeof(struct hwnat_args)))
		return;

	enable_status = opt.hwnat_enable_status;
	switch(enable_status)
	{
		case HWNAT_STATUS_OFF:
            HENAT_STATUS_DISABLE();
            HENAT_PRINT_ENABLE_STATUS();
        	PpeCleanTableHandler();
			break;
        case HWNAT_STATUS_ON:
            HENAT_STATUS_ENABLE();
            HENAT_PRINT_ENABLE_STATUS();
            break;
        default:
            printk("Usage: hw_nat -en 0|1.\n");
            HENAT_PRINT_ENABLE_STATUS();
            break;
    }
}
