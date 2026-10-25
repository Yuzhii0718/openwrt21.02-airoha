#ifndef __HW_NAT_API
#define __HW_NAT_API

#ifdef TCSUPPORT_LITTLE_ENDIAN
#define NIPQUAD(addr) \
	((unsigned char *)&addr)[3], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[0]
#define NIPTWOBYTE(addr) \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[0]
#define NIPHALF(addr) \
        ((unsigned short *)&addr)[1], \
        ((unsigned short *)&addr)[0]
        
#define swab16(x) ((unsigned short)(				\
	(((unsigned short)(x) & (unsigned short)0x00ffU) << 8) |			\
	(((unsigned short)(x) & (unsigned short)0xff00U) >> 8)))

#define swab32(x) ((unsigned int)(				\
	(((unsigned int)(x) & (unsigned int)0x000000ffUL) << 24) |		\
	(((unsigned int)(x) & (unsigned int)0x0000ff00UL) <<  8) |		\
	(((unsigned int)(x) & (unsigned int)0x00ff0000UL) >>  8) |		\
	(((unsigned int)(x) & (unsigned int)0xff000000UL) >> 24)))
#else
#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]
#define NIPTWOBYTE(addr) \
        ((unsigned char *)&addr)[0], \
        ((unsigned char *)&addr)[1]
#define NIPHALF(addr) \
        ((unsigned short *)&addr)[0], \
        ((unsigned short *)&addr)[1]
#define swab16(x) (x)
#define swab32(x) (x)
#endif


int HwNatDumpEntry(unsigned int entry_num);

int HwNatBindEntry(unsigned int entry_num);

int HwNatUnBindEntry(unsigned int entry_num);

int HwNatInvalidEntry(unsigned int entry_num);

int HwNatCacheClear(unsigned int entry_num);

int HwNatCacheDumpData(unsigned char cache_type);

int HwNatCacheDumpEntry(unsigned int entry_num);

int HwNatGetAGCnt(struct hwnat_ac_args *opt);

int HwNatREGDUMP(struct hwnat_config_args *opt);

int HwNatDebug(struct hwnat_debug_args *arg);

int HwNatSnoopEN(struct hwnat_config_args *opt);

int HwNatL2br_ethtypeEN(struct hwnat_config_args *opt);

int HwNatAddEntry(struct hwnat_tuple *opt);

int HwNatSetMulticastEn(struct hwnat_config_args *opt);

int HwNatSetIPChk(struct hwnat_config_args *opt);

int HwNatGetAllEntries(struct hwnat_args *opt);

int HwNatGetEntries(struct hwnat_entries_args *opt);

int HwNatSetBindThreshold(struct hwnat_config_args *opt);

int HwNatSetMaxEntryRateLimit(struct hwnat_config_args *opt);

int HwNatSetKaInterval(struct hwnat_config_args *opt);

int HwNatSetUnbindLifeTime(struct hwnat_config_args *opt);

int HwNatSetBindLifeTime(struct hwnat_config_args *opt);

int HwNatSetBindDir(unsigned int dir);

int HwNatTblClean(struct hwnat_config_args *opt);

int HwNatRemoveFoe(int entry_idx);

int HwNatCleanLanWan(int cmd,int entry_idx);

int HwNatGetEnableStatus(struct hwnat_args *opt);

#endif
