/*
    Module Name:
    util.h

    Abstract:

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
    Name        Date            Modification logs
    Steven Liu  2007-01-25      Initial version
*/

#ifndef _UTIL_WANTED
#define _UTIL_WANTED

#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include "foe_fdb.h"
#include <asm/tc3162/tc3162.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
/*
 * DEFINITIONS AND MACROS
 */
#ifdef TCSUPPORT_CPU_ARMV8
#define PHYS_TO_K1(physaddr) (unsigned long)phys_to_virt(physaddr)
#else
#define PHYS_TO_K1(physaddr) KSEG1ADDR(physaddr)
#endif
#define MIN(a, b) ((a)<(b)?(a):(b))

#ifndef IN6ADDRSZ
#define	IN6ADDRSZ	16
#endif

#ifndef INT16SZ
#define	INT16SZ		2
#endif

#ifndef INADDRSZ
#define	INADDRSZ	4
#endif

#define PPE_PROTO_IPV4 4
#define PPE_PROTO_IPV6 6

#define PACKET_IPV4 0x0800
#define PACKET_IPV6 0x86dd

#define HWNAT_IC_VERIFY 0

extern unsigned int get_frame_engine_data(unsigned int reg);
extern void set_frame_engine_data(unsigned int reg, unsigned int val);
extern struct device* get_frame_engine_dev(void);

#ifdef TCSUPPORT_CPU_ARMV8
#define read_reg_word(reg) 			get_frame_engine_data(reg)
#define write_reg_word(reg, wdata) 	set_frame_engine_data(reg, wdata)
#define regRead32(reg)				get_frame_engine_data(reg)
#define regWrite32(reg, wdata)		set_frame_engine_data(reg, wdata)

extern unsigned int ecnt_ppe_get_buffer_base(void);
#endif


#define IO_GREG(reg)							regRead32(reg)
#define IO_SREG(reg, value)						regWrite32(reg, value)
#define IO_GMASK(reg, mask, shift)				((regRead32(reg) & mask) >> shift)
#define IO_SMASK(reg, mask, shift, value)		{ uint t = regRead32(reg); regWrite32(reg, ((t&~(mask))|((value<<shift)&mask))); }
#define IO_SBITS(reg, bit)						{ uint t = regRead32(reg); regWrite32(reg, (t|bit)); }
#define IO_CBITS(reg, bit)						{ uint t = regRead32(reg); regWrite32(reg, (t&~(bit))); }
#define IO_GREG_REP(reg, buf, count)			ioread32_rep((void __iomem *)(reg), buf, count)
#define IO_SREG_REP(reg, buf, count)			iowrite32_rep((void __iomem *)(reg), buf, count)

#ifdef TCSUPPORT_CPU_ARMV8_64
#define RegRead(reg)              (uint32_t)regRead32(reg)
#define RegWrite(reg, wdata)      regWrite32(reg, wdata)
#else
#define RegRead(reg)              (uint32_t)regRead32(PHYS_TO_K1(reg))
#define RegWrite(reg, wdata)      regWrite32(PHYS_TO_K1(reg), wdata)
#endif
#define SWAP32(x) \
    ((uint32_t)( \
    (((uint32_t)(x) & (uint32_t) 0x000000ffUL) << 24) | \
    (((uint32_t)(x) & (uint32_t) 0x0000ff00UL) <<  8) | \
    (((uint32_t)(x) & (uint32_t) 0x00ff0000UL) >>  8) | \
    (((uint32_t)(x) & (uint32_t) 0xff000000UL) >> 24) )

/*
 * TYPEDEFS AND STRUCTURES
 */
#define IS_IC_EN751X2X (isEN751221 || isEN751627)


/*
 * EXPORT FUNCTION
 */
uint8_t *Ip2Str(uint32_t ip);
unsigned int Str2Ip(IN char *str);
int str_to_mac(unsigned char *mac, char *str);
int str_to_ip(unsigned long *ip,char *str);
int inet_pton6(const char *src, u_char *dst);
void MacReverse(uint8_t * Mac);
long strtol(const char *cp, char **endp, unsigned int base);
void RegModifyBits(uint32_t Addr, uint32_t Data, uint32_t Offset, uint32_t Len);
void CalIpRange(uint32_t StartIp, uint32_t EndIp, uint8_t * M, uint8_t * E);
void FoeToOrgTcpHdr(IN struct FoeEntry *foe_entry, IN struct iphdr *iph,
		    OUT struct tcphdr *th);
void FoeToOrgUdpHdr(IN struct FoeEntry *foe_entry, IN struct iphdr *iph,
		    OUT struct udphdr *uh);
void FoeToOrgIpHdr(IN struct FoeEntry *foe_entry, OUT struct iphdr *iph);

int tlsCfgAvaiIdx_get(void);
int tlsVidAvaiIdx_get(void);
void tlsCfgIdx_free(unsigned int idx);
void tlsVidIdx_free(unsigned int idx);
void dashLine_print(int n);
int cmpMacInfo(uint8_t * Dst, uint8_t * Src);
int ppe_set_ether_type(uint index, PPE_Enable_t enable, PPE_PPPOE_t is_pppoe, uint value);
int sramGetEntryValue(uint entryIdx);
int sramSetEntryValue(uint entryIdx);
int setPpeEntry(unsigned int hash_index, struct FoeEntry *local_entry);
struct FoeEntry *getPpeEntryLocal(unsigned int hash_index, struct FoeEntry *local_entry);
struct FoeEntry *getPpeEntry(unsigned int hash_index);
int ppeSetDualEntry(unsigned int hash_index,struct FoeEntry *entry,struct ipv4_hnapt_dual_info1 *dual_info1, struct ipv4_hnapt_dual_info2 *dual_info2);

int FoeDelSramEntryAll(void);
int ppeSetUpdateMemCtrl(uint select, uint addr, uint offset);
int ppeGetUpdateMemCtrl(uint select, uint addr, uint offset);
int ppeInitShrinkTable(void);
int ppeSetShrinkField(int select, int index, struct hwnat_shrink_field *shrinkFieldPtr);
int ppeGetShrinkField(int select, int index, struct hwnat_shrink_field *shrinkFieldPtr);
int find_and_update_shrink_table(int select, struct hwnat_shrink_field *shrinkFieldPtr);
int ppe_dump_shrink_table(void);


int ppe_is_multicast_entry(struct FoeEntry *foe_entry);

int ppe_trans_str_to_ip_format(int proto,unsigned char* ip,unsigned char* out_ip);

int ppe_try_get_number(const char *data, size_t dlen, u_int32_t array[],int array_size, char sep, char term);

#endif
