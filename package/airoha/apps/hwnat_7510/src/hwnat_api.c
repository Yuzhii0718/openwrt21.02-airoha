#include <stdlib.h>             
#include <stdio.h>             
#include <string.h>           
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <modules/ra_hwnat/hwnat_ioctl.h>
#include "libapi_lib_ppe.h"
#include "libcompileoption.h"

extern int do_ppe_ioctl(void *opt, unsigned long command);

int HwNatDumpEntry(unsigned int entry_num)
{
    struct hwnat_args opt;

    opt.entry_num=entry_num;

    return do_ppe_ioctl(&opt, HW_NAT_DUMP_ENTRY);
}

int HwNatBindEntry(unsigned int entry_num)
{
    struct hwnat_args opt;

    opt.entry_num=entry_num;

    return do_ppe_ioctl(&opt, HW_NAT_BIND_ENTRY);
}

int HwNatUnBindEntry(unsigned int entry_num)
{
    struct hwnat_args opt;

    opt.entry_num=entry_num;

    return do_ppe_ioctl(&opt, HW_NAT_UNBIND_ENTRY);
}

int HwNatInvalidEntry(unsigned int entry_num)
{
    struct hwnat_args opt;

    opt.entry_num=entry_num;

    return do_ppe_ioctl(&opt, HW_NAT_INVALID_ENTRY);
}

int HwNatCacheClear(unsigned int entry_num)
{
   struct hwnat_args opt;

    opt.entry_num=entry_num;

    return do_ppe_ioctl(&opt, HW_NAT_CLEAR_CACHE);
}
int HwNatCacheDumpData(unsigned char cache_type)
{
    struct hwnat_args opt;

    opt.cache_type = cache_type;

    return do_ppe_ioctl(&opt, HW_NAT_DUMP_CACHE_DATA);
}
int HwNatCacheDumpEntry(unsigned int entry_num)
{
    struct hwnat_args opt;

    opt.entry_num=entry_num & 0xffff;

    return do_ppe_ioctl(&opt, HW_NAT_DUMP_CACHE_ENTRY);
}

int HwNatGetAGCnt(struct hwnat_ac_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_GET_AC_CNT);

}

int HwNatREGDUMP(struct hwnat_config_args *opt)
{
     return do_ppe_ioctl(opt, HW_NAT_REG_DUMP);
}

int HwNatDebug(struct hwnat_debug_args *arg)
{
    struct hwnat_args opt;

#ifdef TCSUPPORT_HWNAT_V3
    opt.p_dbg_arg = arg;
#else
    opt.debug = arg->dbg_lvl;
#endif

    return do_ppe_ioctl(&opt, HW_NAT_DEBUG);
}

int HwNatSnoopEN(struct hwnat_config_args *opt){
    return do_ppe_ioctl(opt, HW_NAT_SNOOPING_EN);
}

int HwNatL2br_ethtypeEN(struct hwnat_config_args *opt){
    return do_ppe_ioctl(opt, HW_NAT_L2BR_ETHTYPE_EN);
}

int HwNatAddEntry(struct hwnat_tuple *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_ADD_ENTRY);
}

int HwNatSetMulticastEn(struct hwnat_config_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_MULTICAST_EN);
}

int HwNatSetIPChk(struct hwnat_config_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_SET_IP_CHK);
}

int HwNatGetAllEntries(struct hwnat_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_GET_ALL_ENTRIES);
}

int HwNatGetEntries(struct hwnat_entries_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_GET_ENTRIES);
}

int HwNatSetBindThreshold(struct hwnat_config_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_BIND_THRESHOLD);
}

int HwNatSetMaxEntryRateLimit(struct hwnat_config_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_MAX_ENTRY_LMT);
}

int HwNatSetKaInterval(struct hwnat_config_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_KA_INTERVAL);
}

int HwNatSetUnbindLifeTime(struct hwnat_config_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_UB_LIFETIME);
}

int HwNatSetBindLifeTime(struct hwnat_config_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_BIND_LIFETIME);
}

int HwNatSetBindDir(unsigned int dir)
{
    struct hwnat_args opt;

    opt.bind_dir=dir;

    return do_ppe_ioctl(&opt, HW_NAT_BIND_DIRECTION);
}

int HwNatTblClean(struct hwnat_config_args *opt)
{
    return do_ppe_ioctl(opt, HW_NAT_TAB_CLEAN);
}

int HwNatRemoveFoe(int entry_idx){
    struct hwnat_args opt;
    int fd;

    opt.entry_num=entry_idx;

    return do_ppe_ioctl(&opt, HW_NAT_FOE_REMOVE);
}

int HwNatCleanLanWan(int cmd,int entry_idx){
    struct hwnat_args opt;

    opt.entry_num=entry_idx;

    return do_ppe_ioctl(&opt, cmd);
}

int HwNatGetEnableStatus(unsigned int enable_status)
{

    struct hwnat_args opt;

    opt.hwnat_enable_status = enable_status;

    return do_ppe_ioctl(&opt, HW_NAT_ENABLE_STATUS);
}
