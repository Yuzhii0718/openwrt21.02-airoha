/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <net/ip.h>
#include <linux/if_pppox.h>
#include <linux/ip.h>
#include <linux/ipv6.h>

#include "util.h"
#include "ppe_api.h"
#include <ecnt_hook/ecnt_hook_qdma.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/


/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
/*TCSUPPORT_DLF_CTRL begin*/
extern int dlf_control_enable ;
extern void PpeDLF_fdb_clear_invalid_index(void);
/*TCSUPPORT_DLF_CTRL end*/

extern unsigned char ratelimitByItf_en ;
extern unsigned char ratelimitByIP_en ;
extern struct ppeRatelimitItfInfo RateLimitItfInfo[8] ; /*support LAN1~4, SSID1~SSID4,ratelimit*/
extern struct ppeRatelimitIPInfo RateLimitIPInfo[6] ;	/*support 6 pairs of IP range ratelimit*/

extern PPE_METER_ACNT_T ppe_meter_acnt_res;

extern int ecnt_multicast_hwnat_list_upate(PPE_MULTICAST_INFO_t* info,unsigned int update_mode,unsigned int op_type,unsigned int port_mask,unsigned int local);
extern int ecnt_multicast_hwnat_clear_all_entry(void);
extern int ecnt_multicast_hwnat_get_orig_dev(PPE_MULTICAST_INFO_t* info);
extern int ecnt_multicast_hwnat_subcribe_group(PPE_MULTICAST_INFO_t* info,unsigned int update_mode);
extern int ecnt_multicast_hwnat_get_local(PPE_MULTICAST_INFO_t* info);

extern int32_t PpeCleanTableHandler(void);
extern uint32_t FoeDumpPkt(struct sk_buff * skb);
extern int ppeGetSoftCnt(uint index, uint *softCnt);
extern int ppeGetSoftPkts(uint index, uint *softPkts);
extern int ppemarkMeterIdxToFoe(struct sk_buff *skb,int meteridx);
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int PpeHookCleanFoeTable(struct ecnt_ppe_data *);
int ppeHookDumpFoePkt(struct ecnt_ppe_data *);
int ppeHookAddFoeEntry(struct ecnt_ppe_data *);
int ppeHookDlfClearInvalid(struct ecnt_ppe_data *);
int ppeHookDlfIfEnable(struct ecnt_ppe_data *);
int ppeHookSetMeterGroup(struct ecnt_ppe_data *);
int ppeHookGetMeterGroup(struct ecnt_ppe_data *);
int ppeHookGetSoftCnt(struct ecnt_ppe_data *);
int ppeHookGetSoftPkts(struct ecnt_ppe_data *ppe_data);
int ppeHookTrafficClassify(struct ecnt_ppe_data *);
int ppeHookSetTlsCfg(struct ecnt_ppe_data *ppe_data);
int ppeHookSetTlsVid(struct ecnt_ppe_data *ppe_data);
int ppeHookSetKaCfg(struct ecnt_ppe_data *ppe_data);
int ppeHookGetAgingCfg(struct ecnt_ppe_data *ppe_data);
int ppeHookSetAgingCfg(struct ecnt_ppe_data *ppe_data);
int ppeHookUpdateMulitcastList(struct ecnt_ppe_data *ppe_data);
int ppeHookClearMulitcastList(struct ecnt_ppe_data *ppe_data);
int ppeHookAllocateMeter(struct ecnt_ppe_data *ppe_data);
int ppeHookFreeMeter(struct ecnt_ppe_data *ppe_data);
int ppeHookAllocateAcnt(struct ecnt_ppe_data *ppe_data);
int ppeHookFreeAcnt(struct ecnt_ppe_data *ppe_data);
int ppeHookGetAcnt0Cnt(struct ecnt_ppe_data *ppe_data);
int ppeHookGetAcnt1Cnt(struct ecnt_ppe_data *ppe_data);
int ppeHookClearAcnt1Cnt(struct ecnt_ppe_data *ppe_data);
int ppeHookGetAcnt2Cnt(struct ecnt_ppe_data *ppe_data);
int ppeHookGetMulticastOrigdev(struct ecnt_ppe_data *ppe_data);
int ppeHookMulitcastSubcribeGroup(struct ecnt_ppe_data *ppe_data);
int ppeHookMulitcastGetLocal(struct ecnt_ppe_data *ppe_data);
int ppeHookSetPpePort(struct ecnt_ppe_data *ppe_data);


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
ppe_hook_func_t ppe_hook_funcs[] =
{
    PpeHookCleanFoeTable,
    ppeHookDumpFoePkt,
    ppeHookAddFoeEntry,
    ppeHookDlfClearInvalid,
    ppeHookDlfIfEnable,
    ppeHookSetMeterGroup,
    ppeHookGetMeterGroup,
    ppeHookGetSoftCnt,
    ppeHookGetSoftPkts,
    ppeHookTrafficClassify,
    ppeHookSetTlsCfg,
    ppeHookSetTlsVid,
    ppeHookSetKaCfg,
    ppeHookGetAgingCfg,
    ppeHookSetAgingCfg,
    ppeHookUpdateMulitcastList,
    ppeHookClearMulitcastList,
    ppeHookAllocateMeter,
    ppeHookFreeMeter,
    ppeHookAllocateAcnt,
    ppeHookFreeAcnt,
    ppeHookGetAcnt0Cnt,
    ppeHookGetAcnt1Cnt,
    ppeHookClearAcnt1Cnt,
    ppeHookGetAcnt2Cnt,
    ppeHookGetMulticastOrigdev,
    ppeHookMulitcastSubcribeGroup,
    ppeHookMulitcastGetLocal,
    ppeHookSetPpePort,
};


/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static char *tlsModeStrArr[TLS_MODE_CMD_MAX] =
{
    TLS_MODE_CMD_UNI,
    TLS_MODE_CMD_UUTAG,
    TLS_MODE_CMD_PRI_TAG,
    TLS_MODE_CMD_CTAG,
    TLS_MODE_CMD_STAG_SIN,
    TLS_MODE_CMD_DBL_TAG,
    TLS_MODE_CMD_CTAG_NON,
    TLS_MODE_CMD_STAG_SPE
};

static char *tlsMaStrArr[TLS_MA_CMD_MAX] =
{
    TLS_MA_CMD_NONTLS,
    TLS_MA_CMD_DROP
};

static struct cmdVal tlsSportArr[TLS_SPORT_CMD_MAX] =
{
    {TLS_SPORT_CMD_LAN0, TLS_SP_GSW_P0},
    {TLS_SPORT_CMD_LAN1, TLS_SP_GSW_P1},
    {TLS_SPORT_CMD_LAN2, TLS_SP_GSW_P2},
    {TLS_SPORT_CMD_LAN3, TLS_SP_GSW_P3},
    {TLS_SPORT_CMD_WAN,  TLS_SP_GDMA2}
};

static char *meterModeStrArr[] =
{
    BYTE_STRING,
    PACKET_STRING
};

static char *meterDirStrArr[] =
{
    UP_STRING,
    DOWN_STRING
};

static char *trtcmStrArr[] =
{
    NO_STRING,
    YES_STRING
};

static struct stringVal meterScaleStrVals[] =
{
    {K_STRING, METER_SCALE_K},
    {M_STRING, METER_SCALE_M},    
    {G_STRING, METER_SCALE_G},
    {NO_STRING, 0}
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/*
	get meter index by lan interface.
*/
int getMeterIndexfromIface(struct net_device *dev)
{
	int lanidx = 0 ;

	if( NULL == dev )
		return -1 ;

	if( isLANInterface(dev) )/*eth0.1~eth0.4*/
	{
		lanidx = getLogicLANIndex(dev) ;
	}
	else if(is5GWiFiInterface(dev))/*rai0~rai3*/
	{
		lanidx = get5GLogicWifiIndex(dev)+ 4 ;
	}
	else if(is24GWiFiInterface(dev) )/*ra0~ra3*/
	{
		lanidx = get24GLogicWifiIndex(dev) + 4 ;
	}
	else
	{
		return -1 ;
	}

	if( RateLimitItfInfo[lanidx].enable == ENABLE )
	{
		return lanidx ;/*0~7*/
	}

	return -1 ;
}

int getMeterIndexfromiprangev4(__be32 ipaddr)
{
	int idx = 0 ;
	__be32 ip4_l = 0 ;
	__be32 ip4_h = 0 ;

	for(idx = 0 ; idx < RATELIMIT_BY_IP_MAX ; idx++)
	{
		/*active && ipv4*/
		if( (ENABLE == RateLimitIPInfo[idx].enable) && (IP_VERSION_V4 == RateLimitIPInfo[idx].ip_version) )
		{
			ip4_l = *(unsigned int *)RateLimitIPInfo[idx].v4_pair_l ;
			ip4_h = *(unsigned int *)RateLimitIPInfo[idx].v4_pair_h ;

			if( (ipaddr <= ip4_h) && (ipaddr >= ip4_l) )
			{
				return ( idx + RATELIMIT_BY_INTERFACE_MAX );
			}
		}
	}

	return -1 ;
}

int getMeterIndexfromiprangev6(__u8 *ip6addr)
{
	int idx = 0 ;

	for(idx = 0 ; idx < RATELIMIT_BY_IP_MAX ; idx++)
	{
		/*active && ipv6*/
		if( (ENABLE == RateLimitIPInfo[idx].enable) && (IP_VERSION_V6 == RateLimitIPInfo[idx].ip_version) )
		{
			if ( (0 <= memcmp(ip6addr, RateLimitIPInfo[idx].v6_pair_l, 16)) 
				&& (0 >= memcmp(ip6addr, RateLimitIPInfo[idx].v6_pair_h, 16)) )
			{
				return ( idx + RATELIMIT_BY_INTERFACE_MAX );
			}
		}
	}

	return -1 ;
}

int getMeterIndexfromIP(unsigned char *cp)
{
	ushort etherType = 0;
	ushort ppp_proto = 0;
	struct iphdr *iph = NULL;
	struct ipv6hdr *ip6h = NULL;
	struct pppoe_hdr *ppph = NULL;
	__be32 check_addr = 0;
	__u8 check_u6_addr8[16] = {0};
	int iptype = 0; /*0:IPv4  1:IPv6*/
	int ipidx = 0 ; /*0~5*/

#define PPP_IP		0x21	/* Internet Protocol */
#define PPP_IPV6	0x57	/* Internet Protocol Version 6 */
#define IPTYPE_IPV4	0
#define IPTYPE_IPV6	1


	if (cp == NULL)
	{
		return -1 ;
	}
	
	/* skip DA and SA mac address */
	cp += 12 ;
	/* get ether type */
	etherType = *(ushort *) cp ;
	/* skip ether type */
	cp += 2 ;
	
	/*parse if vlan exists*/
	if( (etherType == 0x8100) || (etherType == 0x88a8) || (etherType == 0x9100) ) 
	{
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}

	/*parse if second vlan exists*/
	if( (etherType == 0x8100) || (etherType == 0x88a8) || (etherType == 0x9100) ) 
	{
		/*skip 802.1q tag field*/
		cp += 2;
		/*re-parse ether type*/
		etherType = *(ushort *) cp;
		/* skip ether type */
		cp += 2;
	}

	if( etherType == 0x0800 ) 		/*IPv4*/
	{
		iph = (struct iphdr*)(cp) ;
		check_addr = iph->saddr ;
		iptype = IPTYPE_IPV4 ;
	}
	else if( etherType == 0x86dd ) 	/*IPv6*/
	{
		ip6h = (struct ipv6hdr* )(cp);
		memcpy(check_u6_addr8, ip6h->saddr.s6_addr,sizeof(check_u6_addr8));
		iptype = IPTYPE_IPV6 ;
	}
	else if( etherType == 0x8864 ) 		/*PPPoe session*/
	{
		ppph = (struct pppoe_hdr*)(cp) ;
		cp = (unsigned char *)(ppph + 1) ;		/*skip pppoe head*/
		ppp_proto = *(ushort *)cp;
		cp += 2;						/*skip ppp protocol*/

		if( ppp_proto == PPP_IP )
		{
			iph = (struct iphdr* )(cp);
			check_addr = iph->saddr;
			iptype = IPTYPE_IPV4 ;
		}
		else if( PPP_IPV6 == ppp_proto )
		{
			ip6h = (struct ipv6hdr* )(cp);
			memcpy(check_u6_addr8, ip6h->saddr.s6_addr,sizeof(check_u6_addr8));
			iptype = IPTYPE_IPV6 ;
		}
		else
		{
			return -1 ;
		}
	}
	else
	{
		return -1 ;
	}

	/*get meter index by ip*/
	if ( IPTYPE_IPV4 == iptype )
		ipidx = getMeterIndexfromiprangev4(check_addr);
	else if ( IPTYPE_IPV6 == iptype )
		ipidx = getMeterIndexfromiprangev6(check_u6_addr8);

	return ipidx ;
}

char *tlsSportStr_get(int sport)
{
    int i;

    for (i = 0; i < TLS_SPORT_CMD_MAX; i++)
    {
        if (tlsSportArr[i].val == sport)
            return tlsSportArr[i].cmd;
    }

    return TLS_NOT_FOUND;
}

unsigned int tlsTag2vid(unsigned int tag)
{
    unsigned int addr, val;

    /* a PPE_TLS_VID register contains 2 vids */
    addr = PPE_TLS_VID0 + ((tag>>1)<<2);
    
    val = RegRead(addr);
    
    if (tag & 0x1)
    {
        return ((val>>16) & 0xfff); /* vid: 12 bits */
    }
    else
    {
        return (val & 0xfff);
    }
}

int vidsDuplicatedCheck(struct ppe_tls_cfg *tlsCfg, char tags[])
{
    unsigned short res[5];
    unsigned int i, j, n = 0;

    if (tlsCfg->tags > TLS_TAGS_MAX)
    {
        printk("\nERROR(%s,%d): too many tags:%d\n", __func__, __LINE__, tlsCfg->tags);
        return -1;
    }

    /* no need to do duplicated vids check if there is only one vid */
    if (tlsCfg->tags == 1)
    {
        if ((tags[0] = tlsVidAvaiIdx_get()) == -1)
        {
            printk("\nERROR(%s,%d): No availabe tlsVids\n", __func__, __LINE__);
            return -1;
        }
        
        return 0;
    }


    n = tlsCfg->tags;
    tlsCfg->tags = 0;
    
    /* No need to check 1st vid */
    res[0] = tlsCfg->vids[0];
    tlsCfg->tags++;
    
    /* start to check if vid1~5 are duplicated vids. 
     * res[] is used to store all non dupilcated vids,
     * tlsCfg->tags is used to count non dupilcated vids */
    for (i = 1; i < n; i++) 
    {
        for (j = 0; j < tlsCfg->tags; j++)
            if (tlsCfg->vids[i] == res[j])
                break;

        /* vids[i] differs from all its previous vids, so it's not a duplicated vid */
        if (j == tlsCfg->tags)
            res[tlsCfg->tags++] = tlsCfg->vids[i];
    }

    for (i = 0; i < tlsCfg->tags; i++)
    {
        if ((tags[i] = tlsVidAvaiIdx_get()) == -1)
        {
            printk("\nERROR(%s,%d): No availabe tlsVids\n", __func__, __LINE__);
            for (j = 0; j < i; j++)
                tlsVidIdx_free(j);
            return -1;
        }

        tlsCfg->vids[i] = res[i];
    }

    return 0;
}


/* Concatenate mode is used to extend the coverage of vids. 
 * Concatenated tlsCfgs should have the same uni, concatenate mode,
 * miss action, and tls mode.
 * tlsCfgs with mode SPORT, UNTAG, or PRI_TAG don't need concatenate
 * mode since they have no vids. */
int concatenateModeCheck(struct ppe_tls_cfg *tlsCfg)
{
    int i;
    unsigned int reg;


    for (i = 0; i < TLS_CFG_REG_NUM; i++)
    {
        if (!tlsRsr.tlsCfg[i])
            continue;

        reg = RegRead(PPE_TLS_CFG0+(i<<2));
        
        if (tlsCfg->uni != TLS_REG_SPORT(reg))
            continue;

        /* only need to care those existing tlsCfgs with the same uni */

        if (tlsCfg->mode < TLS_MODE_CTAG)
        {
            printk("\nERROR: tlsCfg with the same sport already exist!\n");
            return -1;
        }
        else
        {
            if ((tlsCfg->cm == TLS_REG_CM(reg)) &&
                (tlsCfg->ma == TLS_REG_MA(reg)) &&
                (tlsCfg->mode == TLS_REG_MODE(reg))
            )
                return 0;
            else
            {
                printk("\nERROR: concatenateModeCheck fail\n");
                return -1;
            }
        }
    }

    return 0;
}


/* If you want to add a flow into a Meter that is already in use,
 * you have to using the same Meter configuration to add this flow */
static int 
isMeterFlowConflict(
    unsigned int gid, unsigned int mode, unsigned int scale, 
    unsigned int rate, unsigned int vlan, unsigned int dir, unsigned int trtcm, unsigned int isFlowOnly)
{
    struct ppe_meter_cfg *mCfg;
    struct list_head *mgHead, *mvHead, *vlist, *rlist;
    struct meterVlan *mVlan;


    mgHead = &meterRsr.mGroup[gid];
    mCfg = &meterRsr.meterCfg[gid];

    if (isFlowOnly==1 && mCfg->enable==0) {
        printk("\nError: Please config Meter Group %d first\n", gid);
        return 1;
    }

    if (list_empty(mgHead))
        return 0;

    if ((isFlowOnly==1) ||
        ((mCfg->mode == mode) && (mCfg->trtcm == trtcm) &&
         ((mCfg->mode == METER_MODE_PACKET && mCfg->rate == rate) ||
          (mCfg->mode == METER_MODE_BYTE && 
          (mCfg->rate*(meterScaleStrVals[mCfg->scale].val)) == (rate*(meterScaleStrVals[scale].val))))))
    {
        mvHead = &meterRsr.mVid[vlan&0xfff];
        
        list_for_each_safe(vlist, rlist, mvHead) {
            mVlan = list_entry(vlist, struct meterVlan, list);
            if (mVlan->pbits == ((vlan>>12)&0xf) && mVlan->dir == dir && mVlan->group == gid) {
                printk("\nError: Flow (vlan:%x,dir:%d) already exists in Meter %d\n", 
                        vlan, dir, gid);
                return 1;
            }
        }
        return 0;
    }
    else {
        printk("\nError: Meter %d is already in use!\n", gid);
        return 1;
    }
}

/* If you want to add a flow into a Meter that is already in use,
 * you have to using the same Meter configuration to add this flow */
static int 
isMeterFlowConflictFunc(struct ppe_meter_cfg *meterCfg, u8 flow_only)
{
    struct ppe_meter_cfg *mCfg;
    struct list_head *mgHead, *mvHead, *vlist, *rlist;
    struct meterVlan *mVlan;

    unsigned int gid = meterCfg->group;
    unsigned int mode = meterCfg->mode;
    unsigned int scale = meterCfg->scale;
    unsigned int rate = meterCfg->rate;
    unsigned int vlan = meterCfg->vlan;
    unsigned int dir = meterCfg->dir;
    unsigned int trtcm = meterCfg->trtcm;
    unsigned int isFlowOnly = meterCfg->trtcm;
    unsigned int queue = meterCfg->queue;
  
    mgHead = &meterRsr.mGroup[gid];
    mCfg = &meterRsr.meterCfg[gid];

    if ((isFlowOnly==1 || flow_only) && mCfg->enable==0) {
        printk("\nError: Please config Meter Group %d first\n", gid);
        return 1;
    }

    if (list_empty(mgHead))
        return 0;

    if (flow_only || (isFlowOnly==1) ||
        ((mCfg->mode == mode) && (mCfg->trtcm == trtcm) &&
         ((mCfg->mode == METER_MODE_PACKET && mCfg->rate == rate) ||
          (mCfg->mode == METER_MODE_BYTE && 
          (mCfg->rate*(meterScaleStrVals[mCfg->scale].val)) == (rate*(meterScaleStrVals[scale].val))))))
    {

        mvHead = &meterRsr.mVid[vlan&0xfff];
        
        list_for_each_safe(vlist, rlist, mvHead) {
            mVlan = list_entry(vlist, struct meterVlan, list);
            if (((mVlan->pbits == ((vlan>>12)&0xf) && (queue == 0)) || (mVlan->queue == (queue&0xf) && (queue != 0)))
				&& mVlan->dir == dir && mVlan->group == gid) {
                printk("\nError: Flow (vlan:%x,dir:%d) already exists in Meter %d\n", 
                        vlan, dir, gid);
                return 1;
            }
        }
		
        return 0;
    }
    else {
        printk("\nError: Meter %d is already in use!\n", gid);
        return 1;
    }
}

static int 
isAccountFlowConflict(unsigned int aGrpId, unsigned int vlan, unsigned int dir)
{
    struct list_head *vHead, *vlist, *rlist;
    struct meterVlan *mVlan;


    vHead = &meterRsr.mVid[vlan&0xfff];

    if (list_empty(vHead))
        return 0;
    
    list_for_each_safe(vlist, rlist, vHead) {
        mVlan = list_entry(vlist, struct meterVlan, list);
        if (mVlan->pbits == ((vlan>>12)&0xf) && mVlan->dir == dir && mVlan->group == aGrpId) {
            printk("\nError: Flow (vlan:%x,dir:%d) already exists in Accounter %d\n", 
                    vlan, dir, aGrpId);
            return 1;
        }
    }
    
    return 0;
}

static int isTrtcmPirCirConflict(struct ppe_meter_cfg *meterCfg)
{
    if (meterCfg->addFlowOnly)
        return 0;

    if (meterCfg->mode == METER_MODE_BYTE)
    {
        if ((meterCfg->rate * meterScaleStrVals[meterCfg->scale].val) <
            (meterCfg->rate2 * meterScaleStrVals[meterCfg->scale2].val))
            return 1;
    }
    else
    {
        if (meterCfg->rate < meterCfg->rate2)
            return 1;
    }

    return 0;
}

static void trtcmIds_get (unsigned int gid, unsigned int *tid0_P, unsigned int *tid1_P)
{
    *tid0_P = gid<<1;
    *tid1_P = (gid<<1)+1;
    return;
}

static void 
meterGroupVlanList_add(
    struct list_head *mgHead, struct list_head *mvHead,
    struct meterGroup *mGrp, struct meterVlan *mVlan,
    unsigned int gid, struct ppe_meter_cfg *meterCfg)
{
    unsigned int vlan = meterCfg->vlan;
    unsigned int dir = meterCfg->dir;
    

    if (list_empty(mgHead) && meterCfg->addFlowOnly==0)
    {
        memcpy(&meterRsr.meterCfg[gid], meterCfg, sizeof(struct ppe_meter_cfg));

        /* ppe app uses a ppe_meter_cfg structure to bring two Meters' info to ppe driver,
         * but ppe driver uses a ppe_meter_cfg structure to save a Meter's info,
         * therefore, save app's "rate2" and "scale2" to driver's "rate" and "scale" */
        if (meterCfg->trtcm && (gid&0x1)) {
            meterRsr.meterCfg[gid].rate = meterCfg->rate2;
            meterRsr.meterCfg[gid].scale = meterCfg->scale2;
        }
    }

    mGrp->mFlow.vlan = vlan;
    mGrp->mFlow.queue = meterCfg->queue;
    mGrp->mFlow.dir = dir;
    list_add_tail(&mGrp->list, mgHead);

    mVlan->group = gid;
    mVlan->dir = dir;
    mVlan->pbits = (vlan>>12)&0xf;
    mVlan->isTrtcm = meterCfg->trtcm;
    mVlan->queue = (meterCfg->queue)&0xf;
	
    list_add_tail(&mVlan->list, mvHead);

    return;
}

static void 
accountGroupVlanList_add(
    struct list_head *gHead, struct list_head *vHead,
    struct meterGroup *mGrp, struct meterVlan *mVlan,
    unsigned int gid, unsigned int vlan, unsigned int dir)
{
    mGrp->mFlow.vlan = vlan;
    mGrp->mFlow.dir = dir;
    list_add_tail(&mGrp->list, gHead);

    mVlan->group = gid;
    mVlan->dir = dir;
    mVlan->pbits = (vlan>>12)&0xf;
    list_add_tail(&mVlan->list, vHead);

    return;
}


static void ppeMeterRegConfig(struct ppe_meter_cfg *meterCfg, unsigned int gid, unsigned int tokenRate, unsigned int rateScale)
{
    unsigned int regVal;
    unsigned int tick, rate, remainder, scale, maxBkSize;


//	printk("\r\nppeMeterRegConfig meterCfg.group = %d, meterCfg.rate = %d kbps", meterCfg->group, meterCfg->rate);
    if (meterCfg->addFlowOnly == 1)
        return;

    if (meterCfg->mode == METER_MODE_PACKET)
    {
        rate = tokenRate;
        
        if (rate <= (METER_PKT_RATE_BASE*METER_SCALE_10)) /* 1~ 2047x10 */
            {
                tick = METER_TICK_PKT_100MS;
                scale = METER_SCALE_10;

            }
            else if ((METER_PKT_RATE_BASE*METER_SCALE_10) < rate && rate <= (METER_PKT_RATE_BASE*METER_SCALE_100)) /* (1~ 2047)x100 */
            {
                tick = METER_TICK_PKT_10MS;
                scale = METER_SCALE_100;
            }
            else /* (1~ 2047)x1000 */
            {
                tick = METER_TICK_PKT_1MS;
                scale = METER_SCALE_1000;
            }

            remainder = rate % scale;
            if (remainder >= (scale>>1))
                rate = ((rate - remainder)/scale) + 1;
            else
                rate = (rate - remainder)/scale;

        if (rate > METER_TOKEN_RATE_PKT_MAX)
            rate = METER_TOKEN_RATE_PKT_MAX;

        regVal = (METER_MODE_PACKET << METER_REG_PKT_OFF_MODE) |
                 (meterCfg->trtcm << METER_REG_PKT_OFF_TRTCM) |
                 (tick << METER_REG_PKT_OFF_TICK) |
                 (rate << METER_REG_PKT_OFF_RATE) |
                 (0 << METER_REG_PKT_OFF_BUCK);
    }
    else /* byte mode */
    {
        rate = tokenRate * (meterScaleStrVals[rateScale].val);

        if (rate <= METER_SCALE_M) /* 0 ~ 1Mbps */
        {
            tick = METER_TICK_BYTE_1MS;
            rate /= METER_SCALE_K<<4; /* unit: 16kbps */
            maxBkSize = METER_BUCKET_4KB;
        }
        else /* 1Mbps ~ 2Gbps */
        {
            tick = METER_TICK_BYTE_125US;
            
#if 1  /*use the config from function 'ppeSetMeterRate'*/
			if(rate <= (METER_SCALE_M<<2))
				maxBkSize = METER_BUCKET_4KB;
			else if(rate <= METER_SCALE_M<<4)
				maxBkSize = METER_BUCKET_16KB;
			else if(rate <= (METER_SCALE_M<<7))
				maxBkSize = METER_BUCKET_32KB;
			else
				maxBkSize = METER_BUCKET_64KB;
			
			if (isEN7528)
			{
				/* for FTP 500Mbps ratelimit */
				if (rate >= (METER_SCALE_M<<9))
				{
					RegModifyBits(PPE_MTR_BBS0,0x8000,0,16);
				}
				else
				{
					/* keep default value */
					RegModifyBits(PPE_MTR_BBS0,0x2600,0,16);
				}
			}
#else           
			/* If rate is 128Mbps, meter will supply 2KByte token in 1 tick,
			 * therefore, bucket size with 4KByte should be OK */
			if (METER_SCALE_M < rate && rate <= (METER_SCALE_M<<7)) /* 1M ~ 128M bps */
				maxBkSize = METER_BUCKET_4KB;
			else if ((METER_SCALE_M<<7) < rate && rate <= (METER_SCALE_M<<9)) /* 128M ~ 512M bps */
				maxBkSize = METER_BUCKET_16KB;
			else if ((METER_SCALE_M<<9) < rate && rate <= METER_SCALE_G) /* 512M ~ 1G bps */
				maxBkSize = METER_BUCKET_32KB;
			else /* 1G ~ 2G bps */
				maxBkSize = METER_BUCKET_64KB;
#endif
			rate /= METER_SCALE_K<<7; /* unit: 128kbps */

        }

        if (rate > METER_TOKEN_RATE_BYTE_MAX)
            rate = METER_TOKEN_RATE_BYTE_MAX;

        regVal = (METER_MODE_BYTE << METER_REG_BYTE_OFF_MODE) |
                 (meterCfg->trtcm << METER_REG_PKT_OFF_TRTCM) |
                 (tick << METER_REG_BYTE_OFF_TICK) |
                 (rate << METER_REG_BYTE_OFF_RATE) |
                 (maxBkSize << METER_REG_BYTE_OFF_BUCK);        
    }

    RegWrite(PPE_GRP_Meter_CNT(gid), regVal); /* replace with FE_API later */

    return;
}

static void ppeMeterConfig7580(struct ppe_meter_cfg *meterCfg, unsigned int gid, unsigned int tokenRate, unsigned int rateScale)
{
	
	int mainType_val=0;
	unsigned int tickSel, rate;
	GENERAL_TrtcmRatelimitCfg_T rxRateLimitCfg;
    GENERAL_TrtcmRatelimitSet_T rxRateLimitSet;

	if (meterCfg->addFlowOnly == 1)
		return;

	if (meterCfg->mode == METER_MODE_PACKET)
	{
		rxRateLimitCfg.PktMode = TRTCM_PACKET_MODE;
		rate = tokenRate;
		if( rate < 80000 )/*80000pps*/
            tickSel = TRTCM_SLOW_TICK;
        else
            tickSel = TRTCM_FAST_TICK;
	}
	else /* byte mode */
	{
		rxRateLimitCfg.PktMode = TRTCM_BYTE_MODE;
		rate = tokenRate * (meterScaleStrVals[rateScale].val)/METER_SCALE_K;	
		if( rate  < 100000 )/*100Mbps*/
            tickSel = TRTCM_SLOW_TICK;
        else
            tickSel = TRTCM_FAST_TICK;
	}
	
    if( meterCfg->dir == 1){  //downstream
        mainType_val = ECNT_QDMA_LAN;            
    }
    else if( meterCfg->dir == 0 ){  //up
        mainType_val = ECNT_QDMA_WAN;
    }
    else
        return;
	
    rxRateLimitCfg.Index = gid;
    rxRateLimitCfg.trtcmModule = INGRESS_TRTCM ;

    rxRateLimitCfg.MeterEn = meterCfg->enable;

    rxRateLimitCfg.PktMode = meterCfg->mode;

    rxRateLimitCfg.TickSel = tickSel;

    QDMA_API_SET_GENERAL_RATELIMIT_CFG(mainType_val, &rxRateLimitCfg);   

    /*call qdma ratelimit api to do meter ratelimit */
    rxRateLimitSet.trtcmModule = INGRESS_TRTCM ;
    rxRateLimitSet.Index = gid;
    QDMA_API_GET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);   

    rxRateLimitSet.RateLimitValue = rate;
    QDMA_API_SET_GENERAL_RATELIMIT_VALUE(mainType_val, &rxRateLimitSet);
}

/*****************************************************
 * Name:
 *     ecnt_ppe_tls_add
 *
 * Parameters:
 *     mode:
 *         TLS_MODE_SPORT:    frames from specific PPE/GSW source port are TLS
 *         TLS_MODE_UNTAG:    un-tagged frames
 *                            from specific PPE/GSW source port are TLS
 *         TLS_MODE_PRI_TAG:  priority-tagged frames (one VLAN layer with TPID as 0x8100 and VID as 0) 
 *                            from a specific PPE/GSW source port are TLS
 *         TLS_MODE_CTAG:     specific single C-tagged (TPID as 0x8100) frames
 *                            from a specific PPE/GSW source port are TLS
 *         TLS_MODE_STAG_SIN: specific single S-tagged (TPID as 0x88a8 or CSR) frames 
 *                            from a specific PPE/GSW source port are TLS
 *         TLS_MODE_DBL_TAG:  specific double tagged frames
 *                            from a specific PPE/GSW source port are TLS
 *         TLS_MODE_CTAG_NON: specific single C-tagged (TPID as 0x8100) frames 
 *                            from a specific PPE/GSW source port are Non-TLS
 *         TLS_MODE_STAG_SPE: specific single or double S-tagged frames 
 *                            from a specific PPE/GSW source port are TLS
 *     sPort:
 *         TLS_SP_QDMA1:  TLS source port is PSE port 0
 *         TLS_SP_GDMA1:  TLS source port is PSE port 1
 *         TLS_SP_GDMA2:  TLS source port is PSE port 2
 *         TLS_SP_QDMA2:  TLS source port is PSE port 5
 *         TLS_SP_GSW_P0: TLS source port is GSW port 0
 *         TLS_SP_GSW_P1: TLS source port is GSW port 1
 *         TLS_SP_GSW_P2: TLS source port is GSW port 2
 *         TLS_SP_GSW_P3: TLS source port is GSW port 3
 *
 *     cfgFlag:
 *         cfgFlag.missAction:
 *             TLS_MISS_DROP:    drop those packets with TLS result being unmatched.
 *             TLS_MISS_NUN_TLS: treat those with TLS result being unmatched as Non_TLS packets.
 *         cfgFlag.tagRangeEn:
 *             ENABLE:  a range of VLAN IDs will take effects. tagIdx0 and tagIdx1 decide 
 *                      the min and max vlaue of the range respectively. tagIdx2 ~ tagIdx4
 *                      don't take effects.
 *             DISABLE: up to 5 VLAN IDs (indicated by tagIdx0 ~ tagIdx4) will take effect.
 *
 *     tagsIdx:
 *         tag0 stands for tagIdx0
 *         tag1 stands for tagIdx1
 *         tag2 stands for tagIdx2
 *         tag3 stands for tagIdx3
 *         tag4 stands for tagIdx4
 *
 * Description:
 *     add a PPE TLS rule.
 *
 * Returns:
 *     ECNT_PPE_OK: add a TLS rule successfully.
 *     ECNT_PPE_ERROR: fail to add a TLS rule.
 *
 *****************************************************/
int ecnt_ppe_tls_add (struct ppe_tls_cfg *tlsCfg)
{
    int regIdx = 0;
    char tags[TLS_CFG_TAG_NUM];
    unsigned int tlsTags = 0, i = 0; 
    unsigned int reg = 0; 
    unsigned int addr = 0, val = 0;


    spin_lock(&tlsRsr.tlsLock);

    if ((regIdx = tlsCfgAvaiIdx_get()) == -1)
    {
        printk("\nERROR: No available TLS CFG resgisters\n");
        tlsCfg->retVal = ECNT_PPE_ERROR;
        spin_unlock(&tlsRsr.tlsLock);
        return ECNT_PPE_ERROR;
    }

    if (!IS_VALID_TLS_SPORT(tlsCfg->uni))
    {
        printk("\nERROR: Source Port:%d is Wrong!\n", tlsCfg->uni);   
        goto ppe_tls_add_fail;
    }

    if (concatenateModeCheck(tlsCfg))
        goto ppe_tls_add_fail;

    if (tlsCfg->mode > TLS_MODE_PRI_TAG)
    {
        if (tlsCfg->rng)
        {
            if (tlsCfg->vids[0] > VLAN_ID_MAX || tlsCfg->vids[0] >= tlsCfg->vids[1] || tlsCfg->tags != 2)
            {
                printk("\nError: vid0:%d should be smaller than vid1:%d (vidNum:%d)\n", 
                                tlsCfg->vids[0], tlsCfg->vids[1], tlsCfg->tags);
                goto ppe_tls_add_fail;
            }

            if ((tags[0] = tlsVidAvaiIdx_get()) == -1)
            {
                printk("\nERROR: No available TLS VID resgisters)\n");
                goto ppe_tls_add_fail;
            }
            if ((tags[1] = tlsVidAvaiIdx_get()) == -1)
            {
                printk("\nERROR: No available TLS VID resgisters\n");
                tlsVidIdx_free(tags[0]);
                goto ppe_tls_add_fail;
            }
        }
        else
        {
            if (vidsDuplicatedCheck(tlsCfg, tags) == -1)
                goto ppe_tls_add_fail;
        }


        for (i = 0; i < tlsCfg->tags; i++)
            tlsTags |= tags[i] << TLS_REG_OFF_TAGS(i);

        /* if you don't want to use all tags for tlsCfg, fill unused tags with a useful tag */
        if (tlsCfg->rng == 0 && tlsCfg->tags < TLS_CFG_TAG_NUM)
            for (i = tlsCfg->tags; i < TLS_CFG_TAG_NUM; i++)
                tlsTags |= tags[tlsCfg->tags-1] << TLS_REG_OFF_TAGS(i);
    }

    switch (tlsCfg->mode)
    {
        case TLS_MODE_SPORT:
        case TLS_MODE_UNTAG:
        case TLS_MODE_PRI_TAG:
            tlsCfg->tags = 0;
            reg = ENABLE << TLS_REG_OFF_EN | TLS_CONCATENATE_OR << TLS_REG_OFF_CM | 
                  tlsCfg->ma << TLS_REG_OFF_MA | DONT_CARE << TLS_REG_OFF_TAG_EN |
                  DONT_CARE << TLS_REG_OFF_RNG | tlsCfg->mode << TLS_REG_OFF_MODE |
                  tlsCfg->uni << TLS_REG_OFF_SPORT;         
            
            break;
        
        case TLS_MODE_CTAG:
        case TLS_MODE_STAG_SIN:
        case TLS_MODE_DBL_TAG:
        case TLS_MODE_STAG_SPE:
            reg = ENABLE << TLS_REG_OFF_EN | TLS_CONCATENATE_OR << TLS_REG_OFF_CM | 
                  tlsCfg->ma << TLS_REG_OFF_MA | ENABLE << TLS_REG_OFF_TAG_EN |
                  tlsCfg->rng << TLS_REG_OFF_RNG | tlsCfg->mode << TLS_REG_OFF_MODE |
                  tlsCfg->uni << TLS_REG_OFF_SPORT | tlsTags; 

            break;
        
        case TLS_MODE_CTAG_NON:
            reg = ENABLE << TLS_REG_OFF_EN | TLS_CONCATENATE_AND << TLS_REG_OFF_CM | 
                  tlsCfg->ma << TLS_REG_OFF_MA | ENABLE << TLS_REG_OFF_TAG_EN |
                  tlsCfg->rng << TLS_REG_OFF_RNG | tlsCfg->mode << TLS_REG_OFF_MODE |
                  tlsCfg->uni << TLS_REG_OFF_SPORT | tlsTags; 

            break;
        
        default:
            printk("\nERROR: TLS Mode:%d is Wrong!\n", tlsCfg->mode);
            goto ppe_tls_add_fail_free_vids;
    }

    /* start to write to PPE_TLS_VID0~7 registers */
    for (i = 0; i < tlsCfg->tags; i++)
    {
        addr = PPE_TLS_VID0 + ((tags[i]>>1)<<2); /* a PPE_TLS_VID register contains 2 vids */
        val = RegRead(addr);
        if (tags[i] & 0x1)
        {
            val &= ~(0xfff0000); /* vid: 12 bits*/
            val |= (tlsCfg->vids[i]&0xfff)<<16;
        }
        else
        {
            val &= ~(0xfff);
            val |= (tlsCfg->vids[i]&0xfff);
        }
        
        RegWrite(addr, val);
    }

    RegWrite(PPE_TLS_CFG0+(regIdx<<2), reg);


    tlsCfg->retVal = ECNT_PPE_OK;
    spin_unlock(&tlsRsr.tlsLock);
    return ECNT_PPE_OK;


ppe_tls_add_fail_free_vids:
    for (i = 0; i < tlsCfg->tags; i++)
        tlsVidIdx_free(tags[i]);

ppe_tls_add_fail:
    tlsCfgIdx_free(regIdx);
    tlsCfg->retVal = ECNT_PPE_ERROR;
    spin_unlock(&tlsRsr.tlsLock);
    return ECNT_PPE_ERROR;
}


/*****************************************************
 * Name:
 *     ecnt_ppe_tls_del
 *
 * Parameters:
 *
 *
 * Description:
 *     delete a PPE TLS rule.
 *
 * Returns:
 *     ECNT_PPE_OK: delete a TLS rule successfully.
 *     ECNT_PPE_ERROR: fail to delete a TLS rule.
 *
 *****************************************************/
int ecnt_ppe_tls_del (struct ppe_tls_cfg *tlsCfg)
{
    int i, j, tags, cnt = 0;
    unsigned int reg;

    spin_lock(&tlsRsr.tlsLock);

    if (!IS_VALID_TLS_SPORT(tlsCfg->uni))
    {
        printk("\nERROR: Source Port:%d is Wrong!\n", tlsCfg->uni);   
        goto ppe_tls_del_fail;
    }

    for (i = 0; i < TLS_CFG_REG_NUM; i++)
    {
        if (!tlsRsr.tlsCfg[i])
            continue;

        reg = RegRead(PPE_TLS_CFG0+(i<<2));
        
        if (tlsCfg->uni != TLS_REG_SPORT(reg))
            continue;

        /* delete a TLS cfg */
        RegWrite(PPE_TLS_CFG0+(i<<2), 0);
        tlsCfgIdx_free(i);
        cnt++;

        if (TLS_REG_TAG_EN(reg) == 0)
            continue;

        if (TLS_REG_RNG(reg))
            tags = 2;
        else
            tags = TLS_TAGS_MAX;

        /* free related TLS vids */
        for (j = 0; j < tags; j++)
            tlsVidIdx_free(TLS_REG_TAGS(reg,j));
    }

    if (cnt)
    {
        tlsCfg->retVal = ECNT_PPE_OK;
        spin_unlock(&tlsRsr.tlsLock);
        return ECNT_PPE_OK;
    }
    else
        printk("\nWARRNING: No such TLS cfg to delete !\n");

ppe_tls_del_fail:
    tlsCfg->retVal = ECNT_PPE_ERROR;
    spin_unlock(&tlsRsr.tlsLock);
    return ECNT_PPE_ERROR;
}

/*****************************************************
 * Name:
 *     ecnt_ppe_tls_show
 *
 * Parameters:
 *
 *
 * Description:
 *     show all PPE TLS rules.
 *
 * Returns:
 *     None.
 *
 *****************************************************/
int ecnt_ppe_tls_show (struct ppe_tls_cfg *tlsCfg)
{
    unsigned int i, j, tag = 0, cnt = 0;
    unsigned int reg;

    spin_lock(&tlsRsr.tlsLock);


    printk("\ncfgReg\tsport\tmode\t\tmissAct\t VIDs\n");
    dashLine_print(7);

    for (i = 0; i < TLS_CFG_REG_NUM; i++)
    {
        if (!tlsRsr.tlsCfg[i])
            continue;
        
        reg = RegRead(PPE_TLS_CFG0+(i<<2));

        if (!TLS_REG_EN(reg))
        {
            printk("\nWARNING: tlsRsr.tlsCfg[%d] not syn with tls reg\n", i);
            goto ppe_tls_show_fail;
        }

        cnt++;
        dashLine_print(7);
        printk("%d\t%s\t%s\t", 
                i, tlsSportStr_get(TLS_REG_SPORT(reg)), tlsModeStrArr[TLS_REG_MODE(reg)]);
        if (strlen(tlsModeStrArr[TLS_REG_MODE(reg)]) < 8)
            printk("\t");
        printk("%s\t ", tlsMaStrArr[TLS_REG_MA(reg)]);

        if (TLS_REG_MODE(reg) <= TLS_MODE_PRI_TAG)
        {
            printk("No Need!\n");
            continue;
        }
            
        if (TLS_REG_RNG(reg))
        {
            printk("%d ~ %d", tlsTag2vid(TLS_REG_TAGS(reg,0)), tlsTag2vid(TLS_REG_TAGS(reg,1)));
        }
        else
        {
            for (j = 0; j < TLS_TAGS_MAX; j++)
            {
                if (j > 0)
                {
                    if (tag == TLS_REG_TAGS(reg,j))
                        break; /* no need to print duplicated vids */
                }
                tag = TLS_REG_TAGS(reg,j);
                printk("%d, ", tlsTag2vid(tag));  
            }
        }
        
        printk("\n");
    }

    if (cnt == 0)
        printk("\nNo TLS configuration to show !\n");

    tlsCfg->retVal = ECNT_PPE_OK;
    spin_unlock(&tlsRsr.tlsLock);
    return ECNT_PPE_OK;

ppe_tls_show_fail:
    tlsCfg->retVal = ECNT_PPE_ERROR;
    spin_unlock(&tlsRsr.tlsLock);
    return ECNT_PPE_ERROR;
}

int ecnt_ppe_meter_add (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;
    unsigned int vlan = meterCfg->vlan;
    struct list_head *mgHead = &meterRsr.mGroup[gid];
    struct list_head *mvHead = &meterRsr.mVid[vlan&0xfff];
    struct meterGroup *mGrp = NULL;
    struct meterVlan *mVlan = NULL;

    spin_lock_bh(&meterRsr.meterLock);

	if(meterCfg->addFlowOnly != 2)
	{
#if 1
	if(isMeterFlowConflictFunc(meterCfg, 0))
	        goto ppe_meter_add_fail;
#else
	if (isMeterFlowConflict(gid, meterCfg->mode, meterCfg->scale, meterCfg->rate, vlan, dir, meterCfg->trtcm, meterCfg->addFlowOnly))
	        goto ppe_meter_add_fail;
#endif

    mGrp = (struct meterGroup*)kzalloc(sizeof(struct meterGroup), GFP_KERNEL);
    mVlan = (struct meterVlan*)kzalloc(sizeof(struct meterVlan), GFP_KERNEL);
    if (mGrp == NULL || mVlan == NULL) {
        printk("\nError: can't allocate meterGroup or meterVlan\n");
        goto ppe_meter_add_fail;
    }

    meterGroupVlanList_add(mgHead, mvHead, mGrp, mVlan, gid, meterCfg);
	}
	if(isEN7580)
		ppeMeterConfig7580(meterCfg, gid, meterCfg->rate, meterCfg->scale);
	else
    	ppeMeterRegConfig(meterCfg, gid, meterCfg->rate, meterCfg->scale);

    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;


ppe_meter_add_fail:
    meterCfg->retVal = ECNT_PPE_ERROR;
    if (mGrp) kfree(mGrp);
    if (mVlan) kfree(mVlan);
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_ERROR;    
}

int ppe_del_by_group_id (unsigned int gid, unsigned int rsrType, void *rsr)
{
    struct list_head *gHead, *vHead, *plist, *vlist, *qlist, *rlist;
    struct meterGroup *mGrp = NULL;
    struct meterVlan *mVlan = NULL;
    unsigned int vlan, dir;
    struct ppeMeterResource *mRsr = NULL;
    struct ppeAccountResource *aRsr = NULL;
    
    
    if (gid >= METER_GROUP_MAX) {
        printk("\nError: Group ID:%d is wrong\n", gid);
        return -1;
    }

    if (rsrType == RESOURCE_TYPE_METER) {
        mRsr = rsr;
        memset(&mRsr->meterCfg[gid], 0, sizeof(struct ppe_meter_cfg));
        gHead = &mRsr->mGroup[gid];
    }
    else {
        aRsr = rsr;
        gHead = &aRsr->aGroup[gid];
    }
  
    list_for_each_safe(plist, qlist, gHead) {
        mGrp = list_entry(plist, struct meterGroup, list);
        vlan = mGrp->mFlow.vlan;
        dir = mGrp->mFlow.dir;
        if (rsrType == RESOURCE_TYPE_METER)
            vHead = &mRsr->mVid[vlan&0xfff];
        else
            vHead = &aRsr->aVid[vlan&0xfff];

        list_for_each_safe(vlist, rlist, vHead) {
            mVlan = list_entry(vlist, struct meterVlan, list);
            if (((vlan>>12)&0xf) == mVlan->pbits && dir == mVlan->dir && gid == mVlan->group) {
                if (mVlan->isBind)
                    FoeDelEntryByNum(mVlan->foeIdx);
                list_del_init(&mVlan->list);
                kfree(mVlan);
                break;
            }
        }

        list_del_init(&mGrp->list);
        kfree(mGrp);
    }

    return 0;
}


int ppe_meter_del_by_group (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;
    

    spin_lock_bh(&meterRsr.meterLock);

    if (gid == METER_GROUP_MAX) /* delete all Meters */
    {
        for (gid = 0; gid < METER_GROUP_MAX; gid++)
            ppe_del_by_group_id (gid, RESOURCE_TYPE_METER, &meterRsr);
    }
    else
        ppe_del_by_group_id (gid, RESOURCE_TYPE_METER, &meterRsr);
    

    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;
}

void ppe_del_by_flow (unsigned int vlan, unsigned int dir, unsigned int rsrType, void *rsr)
{
    struct list_head *gHead, *vHead, *plist, *vlist, *qlist, *rlist;
    struct meterGroup *mGrp = NULL;
    struct meterVlan *mVlan;
    struct ppeMeterResource *mRsr = NULL;
    struct ppeAccountResource *aRsr = NULL;
    

    if (rsrType == RESOURCE_TYPE_METER) {
        mRsr = rsr;
        spin_lock_bh(&mRsr->meterLock);
        vHead = &mRsr->mVid[vlan&0xfff];
    }
    else {
        aRsr = rsr;
        spin_lock_bh(&aRsr->accountLock);
        vHead = &aRsr->aVid[vlan&0xfff];
    }


    list_for_each_safe(vlist, qlist, vHead) {
        mVlan = list_entry(vlist, struct meterVlan, list);
        
        if (((vlan>>12)&0xf) == mVlan->pbits && dir == mVlan->dir) {
            if (rsrType == RESOURCE_TYPE_METER)
                gHead = &mRsr->mGroup[mVlan->group];
            else
                gHead = &aRsr->aGroup[mVlan->group];

            list_for_each_safe(plist, rlist, gHead) {
                mGrp = list_entry(plist, struct meterGroup, list);
                
                if (vlan == mGrp->mFlow.vlan && dir == mGrp->mFlow.dir) {
                    list_del_init(&mGrp->list);
                    kfree(mGrp);
                    break;
                }
            }

            if (rsrType == RESOURCE_TYPE_METER && list_empty(gHead))
                memset(&mRsr->meterCfg[mVlan->group], 0, sizeof(struct ppe_meter_cfg));

            if (mVlan->isBind)
                FoeDelEntryByNum(mVlan->foeIdx);
            list_del_init(&mVlan->list);
            kfree(mVlan);
        }
    }

    if (rsrType == RESOURCE_TYPE_METER)
        spin_unlock_bh(&mRsr->meterLock);
    else
        spin_unlock_bh(&aRsr->accountLock);
    
    return;
}


int ppe_meter_del_by_flow (struct ppe_meter_cfg *meterCfg)
{
    ppe_del_by_flow (meterCfg->vlan, meterCfg->dir, RESOURCE_TYPE_METER, &meterRsr);

    meterCfg->retVal = ECNT_PPE_OK;
    return ECNT_PPE_OK;
}

int ecnt_ppe_meter_del (struct ppe_meter_cfg *meterCfg)
{
    if (meterCfg->grpOrFlow == METER_HANDLE_BY_GROUP)
        return ppe_meter_del_by_group(meterCfg);
    else
        return ppe_meter_del_by_flow(meterCfg);
}

int ppe_meter_show_by_group_id (unsigned int gid)
{
    struct list_head *mgHead, *plist, *qlist;
    struct meterGroup *mGrp = NULL;
    unsigned int i = 1;
    struct ppe_meter_cfg *mCfg;
    
    if (gid >= METER_GROUP_MAX) {
        printk("\nError: Group ID:%d is wrong\n", gid);
        return -1;
    }

    mgHead = &meterRsr.mGroup[gid];
    mCfg = &meterRsr.meterCfg[gid];

    if (list_empty(mgHead))
        return 0;

    if (mCfg->mode == METER_MODE_PACKET)
        mCfg->scale = METER_SCALE_MAX; /* scale isn't used in packet mode. Here, use it for printing */
        
    printk("\nMeter\tgroup:%d\tmode:%s\tscale:%s\trate:%d\ttrtcm:%s\n\n",
                gid, meterModeStrArr[mCfg->mode], meterScaleStrVals[mCfg->scale].str,
                mCfg->rate, trtcmStrArr[mCfg->trtcm]);

    
    printk("flow\tvlan\tqueue\tdir\n");
    
    list_for_each_safe(plist, qlist, mgHead) {
        mGrp = list_entry(plist, struct meterGroup, list);
        dashLine_print(3);
        printk("%d\t%x\t%x\t%s\n",
                i, mGrp->mFlow.vlan,mGrp->mFlow.queue,meterDirStrArr[mGrp->mFlow.dir]);
        i++;
    }
    
    printk("\n");

    return 1;
}

void ppe_meter_show_by_group_all(void)
{
    unsigned int gid, cnt = 0;

    for (gid = 0; gid < METER_GROUP_MAX; gid++)
        if (ppe_meter_show_by_group_id(gid) == 1)
            cnt++;

    if (cnt == 0)
        printk("\nNo Meter in use!\n\n");
}

int ppe_meter_show_by_group (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;
    

    spin_lock_bh(&meterRsr.meterLock);

    if (gid == METER_GROUP_MAX) { /* delete all Meters */
        ppe_meter_show_by_group_all();
    }
    else {
        if (ppe_meter_show_by_group_id(gid) == 0)
            printk("\nMeter#%d: No flows!\n\n", gid);
    }


    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;
}

int ppe_meter_show_by_flow (struct ppe_meter_cfg *meterCfg)
{
    unsigned int vlan = meterCfg->vlan;
    unsigned int dir = meterCfg->dir;
    struct list_head *mvHead, *vlist, *qlist;
    struct meterVlan *mVlan;
    struct ppe_meter_cfg *mCfg;

    spin_lock_bh(&meterRsr.meterLock);

    mvHead = &meterRsr.mVid[vlan&0xfff];

    if (list_empty(mvHead)) {
        printk("\nNo Info for this Flow !\n");
        goto meter_show_by_flow_end;
    }

    printk("\nFlow\tvlan:%x\tdir:%s\n\n", vlan, meterDirStrArr[dir]);
    printk("meter\tmode\tscale\trate\ttrtcm\n");

    list_for_each_safe(vlist, qlist, mvHead) {
        mVlan = list_entry(vlist, struct meterVlan, list);
        if (((vlan>>12)&0xf) == mVlan->pbits && dir == mVlan->dir) {
            dashLine_print(5);
            mCfg = &meterRsr.meterCfg[mVlan->group];
            if (mCfg->mode == METER_MODE_PACKET)
                mCfg->scale = METER_SCALE_MAX; /* scale isn't used in packet mode. Here, use it for printing */
            printk("%d\t%s\t%s\t%d\t%s\n",
                    mVlan->group, meterModeStrArr[mCfg->mode], meterScaleStrVals[mCfg->scale].str, 
                    mCfg->rate, trtcmStrArr[mCfg->trtcm]);
        }
    }
    printk("\n");

meter_show_by_flow_end:
    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;
}

int ecnt_ppe_meter_show (struct ppe_meter_cfg *meterCfg)
{
    if (meterCfg->grpOrFlow == METER_HANDLE_BY_GROUP)
        return ppe_meter_show_by_group(meterCfg);
    else
        return ppe_meter_show_by_flow(meterCfg);
}

int ecnt_ppe_meter_add_group (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;

    spin_lock_bh(&meterRsr.meterLock);

    memcpy(&meterRsr.meterCfg[gid], meterCfg, sizeof(struct ppe_meter_cfg));
    
    /* ppe app uses a ppe_meter_cfg structure to bring two Meters' info to ppe driver,
     * but ppe driver uses a ppe_meter_cfg structure to save a Meter's info,
     * therefore, save app's "rate2" and "scale2" to driver's "rate" and "scale" */
    if (meterCfg->trtcm && (gid&0x1)) {
        meterRsr.meterCfg[gid].rate = meterCfg->rate2;
        meterRsr.meterCfg[gid].scale = meterCfg->scale2;
    }

    ppeMeterRegConfig(meterCfg, gid, meterCfg->rate, meterCfg->scale);

    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;
}

int ecnt_ppe_meter_del_group (struct ppe_meter_cfg *meterCfg)
{
    return ppe_meter_del_by_group(meterCfg);
}

int ecnt_ppe_meter_add_flow (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;
    unsigned int vlan = meterCfg->vlan;
    struct list_head *mgHead = &meterRsr.mGroup[gid];
    struct list_head *mvHead = &meterRsr.mVid[vlan&0xfff];
    struct meterGroup *mGrp = NULL;
    struct meterVlan *mVlan = NULL;

    spin_lock_bh(&meterRsr.meterLock);

    if(isMeterFlowConflictFunc(meterCfg, 1))
    {
        goto ppe_meter_add_flow_fail;
    }

    mGrp = (struct meterGroup*)kzalloc(sizeof(struct meterGroup), GFP_KERNEL);
    mVlan = (struct meterVlan*)kzalloc(sizeof(struct meterVlan), GFP_KERNEL);
    if (mGrp == NULL || mVlan == NULL) 
    {
        printk("\nError: can't allocate meterGroup or meterVlan\n");
        goto ppe_meter_add_flow_fail;
    }

    meterGroupVlanList_add(mgHead, mvHead, mGrp, mVlan, gid, meterCfg);

    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;

ppe_meter_add_flow_fail:
    meterCfg->retVal = ECNT_PPE_ERROR;
    if (mGrp)
    {
        kfree(mGrp);
    }
    
    if (mVlan)
    {
        kfree(mVlan);
    }
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_ERROR;    
}

int ecnt_ppe_meter_del_flow (struct ppe_meter_cfg *meterCfg)
{
    return ppe_meter_del_by_flow(meterCfg);
}

int ecnt_ppe_trtcm_add (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;
    /* a TRTCM group uses two Meter groups */
    unsigned int tid0, tid1;
    unsigned int vlan = meterCfg->vlan;
    unsigned int dir = meterCfg->dir;
    struct list_head *mgHead0, *mgHead1;
    struct list_head *mvHead = &meterRsr.mVid[vlan&0xfff];
    struct meterGroup *mGrp0 = NULL, *mGrp1 = NULL;
    struct meterVlan *mVlan0 = NULL, *mVlan1 = NULL;


    spin_lock_bh(&meterRsr.meterLock);

    trtcmIds_get(gid, &tid0, &tid1);
    mgHead0 = &meterRsr.mGroup[tid0];
    mgHead1 = &meterRsr.mGroup[tid1];

    if (isMeterFlowConflict(tid0, meterCfg->mode, meterCfg->scale, meterCfg->rate, vlan, dir, meterCfg->trtcm, meterCfg->addFlowOnly) ||
        isMeterFlowConflict(tid1, meterCfg->mode, meterCfg->scale2, meterCfg->rate2, vlan, dir, meterCfg->trtcm, meterCfg->addFlowOnly))
        goto ppe_trtcm_add_fail;

    if (isTrtcmPirCirConflict(meterCfg)) {
        printk("\n\Error: PIR:(%d,%d) should be larger than CIR:(%d,%d)\n",
                meterCfg->rate, meterCfg->scale, meterCfg->rate2, meterCfg->scale2);
        goto ppe_trtcm_add_fail;
    }

    mGrp0 = (struct meterGroup*)kzalloc(sizeof(struct meterGroup), GFP_KERNEL);
    mVlan0 = (struct meterVlan*)kzalloc(sizeof(struct meterVlan), GFP_KERNEL);
    mGrp1 = (struct meterGroup*)kzalloc(sizeof(struct meterGroup), GFP_KERNEL);
    mVlan1 = (struct meterVlan*)kzalloc(sizeof(struct meterVlan), GFP_KERNEL);
    if (mGrp0 == NULL || mVlan0 == NULL || mGrp1 == NULL || mVlan1 == NULL) {
        printk("\nError: can't allocate meterGroup or meterVlan\n");
        goto ppe_trtcm_add_fail;
    }

    meterGroupVlanList_add(mgHead0, mvHead, mGrp0, mVlan0, tid0, meterCfg);
    meterGroupVlanList_add(mgHead1, mvHead, mGrp1, mVlan1, tid1, meterCfg);

    ppeMeterRegConfig(meterCfg, tid0, meterCfg->rate, meterCfg->scale);
    ppeMeterRegConfig(meterCfg, tid1, meterCfg->rate2, meterCfg->scale2);

    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;


ppe_trtcm_add_fail:
    meterCfg->retVal = ECNT_PPE_ERROR;
    if (mGrp0) kfree(mGrp0);
    if (mVlan0) kfree(mVlan0);
    if (mGrp1) kfree(mGrp1);
    if (mVlan1) kfree(mVlan1);
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_ERROR;    
}

int ppe_trtcm_del_by_group (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;
    unsigned int tid0, tid1;
    

    spin_lock_bh(&meterRsr.meterLock);

    if (gid == TRTCM_GROUP_MAX) /* delete all (trtcm) Meters */
    {
        for (gid = 0; gid < TRTCM_GROUP_MAX; gid++) {
            trtcmIds_get(gid, &tid0, &tid1);
            ppe_del_by_group_id (tid0, RESOURCE_TYPE_METER, &meterRsr);
            ppe_del_by_group_id (tid1, RESOURCE_TYPE_METER, &meterRsr);
        }
    }
    else {
        trtcmIds_get(gid, &tid0, &tid1);
        ppe_del_by_group_id (tid0, RESOURCE_TYPE_METER, &meterRsr);
        ppe_del_by_group_id (tid1, RESOURCE_TYPE_METER, &meterRsr);
    }


    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;
}

int ecnt_ppe_trtcm_del (struct ppe_meter_cfg *meterCfg)
{
    if (meterCfg->grpOrFlow == METER_HANDLE_BY_GROUP)
        return ppe_trtcm_del_by_group(meterCfg);
    else
        return ppe_meter_del_by_flow(meterCfg);
}

void ppe_trtcm_show_by_group_all(void)
{
    unsigned int gid, cnt = 0;
    unsigned int tid0, tid1;

    for (gid = 0; gid < TRTCM_GROUP_MAX; gid++) {
        trtcmIds_get(gid, &tid0, &tid1);
        if (ppe_meter_show_by_group_id(tid0) == 1)
            cnt++;
        if (ppe_meter_show_by_group_id(tid1) == 1)
            cnt++;
    }

    if (cnt == 0)
        printk("\nNo (trtcm) Meter in use!\n\n");
}

int ppe_trtcm_show_by_group (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;
    unsigned int tid0, tid1;
    

    spin_lock_bh(&meterRsr.meterLock);

    if (gid == TRTCM_GROUP_MAX) { /* show all (trtcm) Meters */
        ppe_trtcm_show_by_group_all();
    }
    else {
        trtcmIds_get(gid, &tid0, &tid1);
        if (ppe_meter_show_by_group_id(tid0) == 0)
            printk("\nMeter#%d: No flows!\n\n", tid0);
        if (ppe_meter_show_by_group_id(tid1) == 0)
            printk("\nMeter#%d: No flows!\n\n", tid1);        
    }


    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;
}

int ecnt_ppe_trtcm_show (struct ppe_meter_cfg *meterCfg)
{
    if (meterCfg->grpOrFlow == METER_HANDLE_BY_GROUP)
        return ppe_trtcm_show_by_group(meterCfg);
    else
        return ppe_meter_show_by_flow(meterCfg);
}

int ecnt_ppe_trtcm_add_group (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;
    unsigned int tid0, tid1; /* a TRTCM group uses two Meter groups */

    spin_lock_bh(&meterRsr.meterLock);

    trtcmIds_get(gid, &tid0, &tid1);
    memcpy(&meterRsr.meterCfg[tid0], meterCfg, sizeof(struct ppe_meter_cfg));
    memcpy(&meterRsr.meterCfg[tid1], meterCfg, sizeof(struct ppe_meter_cfg));
    
    /* ppe app uses a ppe_meter_cfg structure to bring two Meters' info to ppe driver,
      * but ppe driver uses a ppe_meter_cfg structure to save a Meter's info,
      * therefore, save app's "rate2" and "scale2" to driver's "rate" and "scale" */
    meterRsr.meterCfg[tid1].rate = meterCfg->rate2;
    meterRsr.meterCfg[tid1].scale = meterCfg->scale2;

    ppeMeterRegConfig(meterCfg, tid0, meterCfg->rate, meterCfg->scale);
    ppeMeterRegConfig(meterCfg, tid1, meterCfg->rate2, meterCfg->scale2);

    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    
    return ECNT_PPE_OK;
}

int ecnt_ppe_trtcm_del_group (struct ppe_meter_cfg *meterCfg)
{
    return ppe_trtcm_del_by_group(meterCfg);
}

int ecnt_ppe_trtcm_add_flow (struct ppe_meter_cfg *meterCfg)
{
    unsigned int gid = meterCfg->group;
    unsigned int tid0, tid1; /* a TRTCM group uses two Meter groups */
    unsigned int vlan = meterCfg->vlan;
    unsigned int dir = meterCfg->dir;
    struct list_head *mgHead0, *mgHead1;
    struct list_head *mvHead = &meterRsr.mVid[vlan&0xfff];
    struct meterGroup *mGrp0 = NULL, *mGrp1 = NULL;
    struct meterVlan *mVlan0 = NULL, *mVlan1 = NULL;

    spin_lock_bh(&meterRsr.meterLock);

    trtcmIds_get(gid, &tid0, &tid1);
    mgHead0 = &meterRsr.mGroup[tid0];
    mgHead1 = &meterRsr.mGroup[tid1];

    if (isMeterFlowConflict(tid0, meterCfg->mode, meterCfg->scale, meterCfg->rate, vlan, dir, meterCfg->trtcm, meterCfg->addFlowOnly) ||
        isMeterFlowConflict(tid1, meterCfg->mode, meterCfg->scale2, meterCfg->rate2, vlan, dir, meterCfg->trtcm, meterCfg->addFlowOnly))
    {
        goto ppe_trtcm_add_flow_fail;
    }

    mGrp0 = (struct meterGroup*)kzalloc(sizeof(struct meterGroup), GFP_KERNEL);
    mVlan0 = (struct meterVlan*)kzalloc(sizeof(struct meterVlan), GFP_KERNEL);
    mGrp1 = (struct meterGroup*)kzalloc(sizeof(struct meterGroup), GFP_KERNEL);
    mVlan1 = (struct meterVlan*)kzalloc(sizeof(struct meterVlan), GFP_KERNEL);
    if (mGrp0 == NULL || mVlan0 == NULL || mGrp1 == NULL || mVlan1 == NULL) 
    {
        printk("\nError: can't allocate meterGroup or meterVlan\n");
        goto ppe_trtcm_add_flow_fail;
    }

    meterGroupVlanList_add(mgHead0, mvHead, mGrp0, mVlan0, tid0, meterCfg);
    meterGroupVlanList_add(mgHead1, mvHead, mGrp1, mVlan1, tid1, meterCfg);

    meterCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_OK;


ppe_trtcm_add_flow_fail:
    meterCfg->retVal = ECNT_PPE_ERROR;
    if (mGrp0) kfree(mGrp0);
    if (mVlan0) kfree(mVlan0);
    if (mGrp1) kfree(mGrp1);
    if (mVlan1) kfree(mVlan1);
    spin_unlock_bh(&meterRsr.meterLock);
    return ECNT_PPE_ERROR;    
}

int ecnt_ppe_trtcm_del_flow (struct ppe_meter_cfg *meterCfg)
{
    return ppe_meter_del_by_flow(meterCfg);
}

int ecnt_ppe_account_add (struct ppe_account_cfg *accountCfg)
{
    unsigned int gid = accountCfg->group;
    unsigned int vlan = accountCfg->vlan;
    unsigned int dir = accountCfg->dir;
    struct list_head *gHead = &accountRsr.aGroup[gid];
    struct list_head *vHead = &accountRsr.aVid[vlan&0xfff];
    struct meterGroup *mGrp = NULL;
    struct meterVlan *mVlan = NULL;


    spin_lock_bh(&accountRsr.accountLock);

    if (isAccountFlowConflict(gid, vlan, dir))
        goto ppe_account_add_fail;


    mGrp = (struct meterGroup*)kzalloc(sizeof(struct meterGroup), GFP_KERNEL);
    mVlan = (struct meterVlan*)kzalloc(sizeof(struct meterVlan), GFP_KERNEL);
    if (mGrp == NULL || mVlan == NULL) {
        printk("\nError: can't allocate meterGroup or meterVlan\n");
        goto ppe_account_add_fail;
    }

    accountGroupVlanList_add(gHead, vHead, mGrp, mVlan, gid, vlan, dir);


    accountCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&accountRsr.accountLock);
    return ECNT_PPE_OK;


ppe_account_add_fail:
    accountCfg->retVal = ECNT_PPE_ERROR;
    if (mGrp) kfree(mGrp);
    if (mVlan) kfree(mVlan);
    spin_unlock_bh(&accountRsr.accountLock);
    return ECNT_PPE_ERROR;    
}

int ppe_account_del_by_group (struct ppe_account_cfg *accountCfg)
{
    unsigned int gid = accountCfg->group;
    

    spin_lock_bh(&accountRsr.accountLock);

    if (gid == METER_GROUP_MAX) /* delete all Accounters */
    {
        for (gid = 0; gid < METER_GROUP_MAX; gid++)
            ppe_del_by_group_id (gid, RESOURCE_TYPE_ACCOUNT, &accountRsr);
    }
    else
        ppe_del_by_group_id (gid, RESOURCE_TYPE_ACCOUNT, &accountRsr);
    

    accountCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&accountRsr.accountLock);
    return ECNT_PPE_OK;
}

int ppe_account_del_by_flow (struct ppe_account_cfg *accountCfg)
{
    ppe_del_by_flow (accountCfg->vlan, accountCfg->dir, RESOURCE_TYPE_ACCOUNT, &accountRsr);

    accountCfg->retVal = ECNT_PPE_OK;
    return ECNT_PPE_OK;
}

int ecnt_ppe_account_del (struct ppe_account_cfg *accountCfg)
{
    if (accountCfg->grpOrFlow == METER_HANDLE_BY_GROUP)
        return ppe_account_del_by_group(accountCfg);
    else
        return ppe_account_del_by_flow(accountCfg);
}

int ppe_account_show_by_group_id (unsigned int gid)
{
    struct list_head *gHead, *plist, *qlist;
    struct meterGroup *mGrp = NULL;
    unsigned int i = 1;

    
    if (gid >= METER_GROUP_MAX) {
        printk("\nError: Group ID:%d is wrong\n", gid);
        return -1;
    }

    gHead = &accountRsr.aGroup[gid];

    if (list_empty(gHead))
        return 0;

        
    printk("\nAccounter\tgroup:%d\tbyteCnt_h32:%d\tbyteCnt_l32:%d\tpacketCnt:%d\n\n", 
            gid, RegRead(0xbfb52004+(gid<<4)), 
            RegRead(0xbfb52000+(gid<<4)), RegRead(0xbfb52008+(gid<<4))); /* replace with FE API later */

    printk("flow\tvlan\tdir\n");
    
    list_for_each_safe(plist, qlist, gHead) {
        mGrp = list_entry(plist, struct meterGroup, list);
        dashLine_print(3);
        printk("%d\t%x\t%s\n",
                i, mGrp->mFlow.vlan, meterDirStrArr[mGrp->mFlow.dir]);
        i++;
    }
    
    printk("\n");

    return 1;
}

void ppe_account_show_by_group_all(void)
{
    unsigned int gid, cnt = 0;

    for (gid = 0; gid < METER_GROUP_MAX; gid++)
        if (ppe_account_show_by_group_id(gid) == 1)
            cnt++;

    if (cnt == 0)
        printk("\nNo Accounter in use!\n\n");
}


int ppe_account_show_by_group (struct ppe_account_cfg *accountCfg)
{
    unsigned int gid = accountCfg->group;
    

    spin_lock_bh(&accountRsr.accountLock);

    if (gid == METER_GROUP_MAX) { /* delete all Accounters */
        ppe_account_show_by_group_all();
    }
    else {
        if (ppe_account_show_by_group_id(gid) == 0)
            printk("\nAccounter#%d: No flows!\n\n", gid);
    }


    accountCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&accountRsr.accountLock);
    return ECNT_PPE_OK;
}

int ppe_account_show_by_flow (struct ppe_account_cfg *accountCfg)
{
    unsigned int vlan = accountCfg->vlan;
    unsigned int dir = accountCfg->dir;
    struct list_head *vHead, *vlist, *qlist;
    struct meterVlan *mVlan;
    unsigned int gid;


    spin_lock_bh(&accountRsr.accountLock);

    vHead = &accountRsr.aVid[vlan&0xfff];

    if (list_empty(vHead)) {
        printk("\nNo Info for this Flow !\n");
        goto account_show_by_flow_end;
    }

    printk("\nFlow\tvlan:%x\tdir:%s\n\n", vlan, meterDirStrArr[dir]);
    printk("Acnter\tbyteCnt_h32\tbyteCnt_l32\tpacketCnt\n");

    list_for_each_safe(vlist, qlist, vHead) {
        mVlan = list_entry(vlist, struct meterVlan, list);
        if (((vlan>>12)&0xf) == mVlan->pbits && dir == mVlan->dir) {
            gid = mVlan->group;
            dashLine_print(7);
            printk("%d\t%d\t\t%d\t\t%d\n",
                    gid, RegRead(0xbfb52004+(gid<<4)), 
                    RegRead(0xbfb52000+(gid<<4)), RegRead(0xbfb52008+(gid<<4))); /* replace with FE API later */
        }
    }
    printk("\n");

account_show_by_flow_end:
    accountCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&accountRsr.accountLock);
    return ECNT_PPE_OK;
}


int ecnt_ppe_account_show (struct ppe_account_cfg *accountCfg)
{
    if (accountCfg->grpOrFlow == METER_HANDLE_BY_GROUP)
        return ppe_account_show_by_group(accountCfg);
    else
        return ppe_account_show_by_flow(accountCfg);
}

int ecnt_ppe_account_stats_get (struct ppe_account_cfg *accountCfg)
{
    unsigned int gid = accountCfg->group;
    
    spin_lock_bh(&accountRsr.accountLock);

    if (gid >= METER_GROUP_MAX) 
    {
        printk("\nError: Group ID:%d is wrong\n", gid);
        return -1;
    }

    accountCfg->byteCntH = RegRead(0xbfb52004+(gid<<4));
    accountCfg->byteCntL = RegRead(0xbfb52000+(gid<<4));
    accountCfg->pktCntH  = RegRead(0xbfb52008+(gid<<4));

    accountCfg->retVal = ECNT_PPE_OK;
    spin_unlock_bh(&accountRsr.accountLock);
    return ECNT_PPE_OK;
}

int PpeHookCleanFoeTable(struct ecnt_ppe_data *ppe_data)
{
	PpeCleanTableHandler();
	
	return 0 ;
}

int ppeHookDumpFoePkt(struct ecnt_ppe_data *ppe_data)
{
    FoeDumpPkt((struct sk_buff *)ppe_data->skb);

	return 0 ;
}

int ppeHookAddFoeEntry(struct ecnt_ppe_data *ppe_data)
{
    FoeAddEntry((struct hwnat_tuple *)ppe_data->hwnat_tuple);

	return 0 ;
}    
/*TCSUPPORT_DLF_CTRL begin*/
int ppeHookDlfClearInvalid(struct ecnt_ppe_data *ppe_data)
{
	PpeDLF_fdb_clear_invalid_index();
	return 1 ;
}

int ppeHookDlfIfEnable(struct ecnt_ppe_data *ppe_data)
{
	ppe_data->retValue = dlf_control_enable ;
	return 1 ;
}
/*TCSUPPORT_DLF_CTRL end*/
int ppeHookSetMeterGroup(struct ecnt_ppe_data *ppe_data)
{
	unsigned long int meter_addr = 0;
	unsigned int val = 0;
	
	if(ppe_data->index > 0x3e)
		return -1;

	meter_addr = PPE_GRP_Meter_CNT(ppe_data->index);
	if(0 == ppe_data->meter_cfg.mode)
		val = ppe_data->meter_cfg.mode |(ppe_data->meter_cfg.trtcmEn << 1) |(ppe_data->meter_cfg.tickSel << 2) |(ppe_data->meter_cfg.tokenRate << 3) |(ppe_data->meter_cfg.maxBkSize << 17);
	else if(1 == ppe_data->meter_cfg.mode)
		val = ppe_data->meter_cfg.mode |(ppe_data->meter_cfg.trtcmEn << 1) |(ppe_data->meter_cfg.tickSel << 2) |(ppe_data->meter_cfg.tokenRate << 4) |(ppe_data->meter_cfg.maxBkSize << 15);
	else
		return -1;

	RegWrite(meter_addr, val);
	
	return 0;
}

int ppeHookGetMeterGroup(struct ecnt_ppe_data *ppe_data)
{
	unsigned long int meter_addr = 0;
	unsigned int val = 0;
	
	if(ppe_data->index > 0x3e)
		return -1;

	meter_addr = PPE_GRP_Meter_CNT(ppe_data->index);

	val = RegRead(meter_addr);

	ppe_data->meter_cfg.mode = val&1;
	ppe_data->meter_cfg.trtcmEn = (val>>1)&1;
	if(0 == ppe_data->meter_cfg.mode)
	{
		ppe_data->meter_cfg.tickSel = (val>>2)&1;
		ppe_data->meter_cfg.tokenRate = (val>>3)&0x3fff;
		ppe_data->meter_cfg.maxBkSize = (val>>17)&0x3;
	}
	else if(1 == ppe_data->meter_cfg.mode)
	{
		ppe_data->meter_cfg.tickSel = (val>>2)&0x3;
		ppe_data->meter_cfg.tokenRate = (val>>4)&0x7ff;
		ppe_data->meter_cfg.maxBkSize = (val>>15)&0x3;
	}
	else
		return -1;
	
	return 0;
}

int ppeHookGetSoftCnt(struct ecnt_ppe_data *ppe_data)
{
	ppeGetSoftCnt(ppe_data->index, &ppe_data->cntByte);
	
	return 0;
}

int ppeHookGetSoftPkts(struct ecnt_ppe_data *ppe_data)
{
	ppeGetSoftPkts(ppe_data->index, &ppe_data->cntByte);
	
	return 0;
}

int ppeHookTrafficClassify(struct ecnt_ppe_data *ppe_data)
{
	struct sk_buff *skb = NULL ;
	int meteridx = -1 ;

	skb = (struct sk_buff *)ppe_data->skb ;
	if( NULL == skb )
		return -1 ;

	if( ratelimitByItf_en == ENABLE )
	{
		meteridx = getMeterIndexfromIface(skb->dev) ;
	}
	else if( ratelimitByIP_en == ENABLE )
	{
		meteridx = getMeterIndexfromIP(skb_mac_header(skb)) ;
	}

	ppemarkMeterIdxToFoe(skb , meteridx) ;
	
	return 0 ;
}

int ppeHookSetTlsCfg(struct ecnt_ppe_data *ppe_data)
{
	unsigned char vid_idx[5];	
	unsigned char sp = ppe_data->ppe_tls_cfg->sp;
	unsigned char mode = ppe_data->ppe_tls_cfg->mode;
	unsigned char rng = ppe_data->ppe_tls_cfg->range;
	unsigned char vid_en = ppe_data->ppe_tls_cfg->vid_en;
	unsigned char ma = ppe_data->ppe_tls_cfg->ma;
	unsigned char cm = ppe_data->ppe_tls_cfg->cm;
	unsigned char en = ppe_data->ppe_tls_cfg->en;
	unsigned long val = 0;
	unsigned int reg_addr = PPE_TLS_CFG0+4*ppe_data->index;

	memmove(vid_idx,ppe_data->ppe_tls_cfg->vid_idx,5);

	val = (vid_idx[4]<<TLS_REG_OFF_TAG4)|(vid_idx[3]<<TLS_REG_OFF_TAG3)|(vid_idx[2]<<TLS_REG_OFF_TAG2)|(vid_idx[1]<<TLS_REG_OFF_TAG1)|(vid_idx[0]<<TLS_REG_OFF_TAG0)|\
			(sp<<TLS_REG_OFF_SPORT)|(mode<<TLS_REG_OFF_MODE)|(rng<<TLS_REG_OFF_RNG)|(vid_en<<TLS_REG_OFF_TAG_EN)|(ma<<TLS_REG_OFF_MA)|(cm<<TLS_REG_OFF_CM)|en;

	RegWrite(reg_addr,val);

	return 0;
}

int ppeHookSetTlsVid(struct ecnt_ppe_data *ppe_data)
{
	unsigned short vid_lo = ppe_data->tls_vid.vid_lo;
	unsigned short vid_hi = ppe_data->tls_vid.vid_hi;
	unsigned long val = 0;
	unsigned int reg_addr = PPE_TLS_VID0+4*ppe_data->index;

	val = (vid_hi<<TLS_VID_REG_OFF_VID_HI)|vid_lo;

	RegWrite(reg_addr,val);

	return 0;
}

int ppeHookSetKaCfg(struct ecnt_ppe_data *ppe_data)
{
	unsigned int val = 0;
	unsigned int ka_cfg = ppe_data->ka_cfg;
	
	val = RegRead(PPE_TB_CFG);

	val &= ~0x3000;
	val |= ka_cfg<<12;

	RegWrite(PPE_TB_CFG,val);
	
	return 0 ;
}

int ppeHookGetAgingCfg(struct ecnt_ppe_data *ppe_data)
{
	unsigned int val = 0;
	
	val = RegRead(PPE_TB_CFG);

	ppe_data->aging_cfg = (val&0xfc0)>>6;
	
	return 0 ;
}

int ppeHookSetAgingCfg(struct ecnt_ppe_data *ppe_data)
{
	unsigned int val = 0;
	
	val = RegRead(PPE_TB_CFG);

	val &= ~0xfc0;
	val |= ppe_data->aging_cfg<<6;

	RegWrite(PPE_TB_CFG,val);
	
	return 0 ;
}

int ppeHookUpdateMulitcastList(struct ecnt_ppe_data *ppe_data)
{
	PPE_MULTICAST_INFO_t* multicast_info = ppe_data->multicast_update.info;
	unsigned int update_mode = ppe_data->multicast_update.update_mode;
	unsigned int op_type = ppe_data->multicast_update.op_type;
	unsigned int port_mask = ppe_data->multicast_update.port_mask;
	unsigned int local = ppe_data->multicast_update.local;

	return ecnt_multicast_hwnat_list_upate(multicast_info,update_mode,op_type,port_mask,local);
}

int ppeHookClearMulitcastList(struct ecnt_ppe_data *ppe_data)
{
	return ecnt_multicast_hwnat_clear_all_entry();
}

int ppeHookGetMulticastOrigdev(struct ecnt_ppe_data *ppe_data)
{
	PPE_MULTICAST_INFO_t* multicast_info = ppe_data->multicast_update.info;

	return ecnt_multicast_hwnat_get_orig_dev(multicast_info);
}

int ppeHookMulitcastSubcribeGroup(struct ecnt_ppe_data *ppe_data)
{
	PPE_MULTICAST_INFO_t* multicast_info = ppe_data->multicast_update.info;
	unsigned int update_mode = ppe_data->multicast_update.update_mode;

	return ecnt_multicast_hwnat_subcribe_group(multicast_info,update_mode);
}

int ppeHookMulitcastGetLocal(struct ecnt_ppe_data *ppe_data)
{
	PPE_MULTICAST_INFO_t* multicast_info = ppe_data->multicast_update.info;

	return ecnt_multicast_hwnat_get_local(multicast_info);
}

/* PPE Meter Acnt manage */
int ppe_find_first_unused_meter_index(void)
{
    int meter_index = 0x3f;
    int word_num = 0;
    int word_offset = 0;

    for(word_num = 0; word_num < PPE_METER_GROUP_NUM; word_num++){
        if(ppe_meter_acnt_res.meterMgr.MeterUseState[word_num] == 0xFFFFFFFF){
            continue;
        }else{
            for(word_offset = 0; word_offset < PPEBITSPERWORD; word_offset++){
                if(0 == ((ppe_meter_acnt_res.meterMgr.MeterUseState[word_num]>>word_offset)&1)){
                    meter_index = word_num*PPEBITSPERWORD + word_offset;
                    return meter_index;
                }
            }
        }
    }
    return meter_index;
}

int ppe_set_meter_status_enable(int index, PPE_MeterManage_T *meterManage)
{   
    if(index < 0 || index >= PPE_METER_MAX_NUM )
    {
        printk("correct meter idx range 0 : %d; real meter idx :%d\n", PPE_METER_MAX_NUM-1, index);
        return PPE_ERROR_METER_INDEX;
    }else{
        ppe_meter_acnt_res.meterMgr.MeterUseState[index>>5] |= (1<<(index&PPE_WORD_MASK));
        strlcpy(ppe_meter_acnt_res.meterMgr.MeterUseFunc[index], meterManage->bind_func_name, PPE_NAME_LENGTH);
        return PPE_METER_STATUS_DONE;
    }
}

int ppe_set_meter_status_disable(int index)
{
    if(index < 0 || index >= PPE_METER_MAX_NUM )
    {
        printk("correct meter idx range 0 : %d; real meter idx :%d\n", PPE_METER_MAX_NUM-1, index);
        return PPE_ERROR_METER_INDEX;
    }else{
        ppe_meter_acnt_res.meterMgr.MeterUseState[index>>5] &= ~(1<<(index&PPE_WORD_MASK));
        strcpy(ppe_meter_acnt_res.meterMgr.MeterUseFunc[index], "");
        return PPE_METER_STATUS_DONE;
    }
}

int ppeHookAllocateMeter(struct ecnt_ppe_data *ppe_data)
{
    PPE_MeterManage_T *meterManage = ppe_data->meterManage;

    meterManage->meter_id= ppe_find_first_unused_meter_index();
    ppe_set_meter_status_enable(meterManage->meter_id, meterManage);
    return 0 ;
}

int ppeHookFreeMeter(struct ecnt_ppe_data *ppe_data)
{
    PPE_MeterManage_T *meterManage = ppe_data->meterManage;
    
    ppe_set_meter_status_disable(meterManage->meter_id);
    
    return 0;
}

int ppe_get_default_acnt_idx(int acnt_type){
    
    if(acnt_type == PPE_ACNT0_TYPE)
        return ACNT0_DEFAULT;
    else if(acnt_type == PPE_ACNT1_TYPE)
        return ACNT1_DEFAULT;
    else if(acnt_type == PPE_ACNT2_TYPE)
        return ACNT2_DEFAULT;

    return ACNT0_DEFAULT;
}

int ppe_find_first_unused_acnt_index(int acnt_type){
    int acnt_idx = 0;
    int word_offset = 0;
    int word_num = 0;
    
    if(acnt_type == PPE_ACNT0_TYPE){
        for(word_num = 0; word_num < PPE_ACNT0_GROUP_NUM; word_num++){
            if(ppe_meter_acnt_res.acnt0Mgr.Acnt0UseState[word_num] == 0xFFFFFFFF){
                continue;
            }else{
                for(word_offset = 0; word_offset < PPEBITSPERWORD; word_offset++){
                    if(0 == ((ppe_meter_acnt_res.acnt0Mgr.Acnt0UseState[word_num]>>word_offset)&1)){
                        acnt_idx = word_num*PPEBITSPERWORD + word_offset;
                        return acnt_idx;
                    }
                }
            }
        }
    }else if(acnt_type == PPE_ACNT1_TYPE){
        for(word_num = 0; word_num < PPE_ACNT1_GROUP_NUM; word_num++){
            if(ppe_meter_acnt_res.acnt1Mgr.Acnt1UseState[word_num] == 0xFFFFFFFF){
                continue;
            }else{
                for(word_offset = 0; word_offset < PPEBITSPERWORD; word_offset++){
                    if(0 == ((ppe_meter_acnt_res.acnt1Mgr.Acnt1UseState[word_num]>>word_offset)&1)){
                        acnt_idx = word_num*PPEBITSPERWORD + word_offset;
                        return acnt_idx;
                    }
                }
            }
        }
    }else if(acnt_type == PPE_ACNT2_TYPE){
            for(word_num = 0; word_num < PPE_ACNT2_GROUP_NUM; word_num++){
            if(ppe_meter_acnt_res.acnt2Mgr.Acnt2UseState[word_num] == 0xFFFFFFFF){
                continue;
            }else{
                for(word_offset = 0; word_offset < PPEBITSPERWORD; word_offset++){
                    if(0 == ((ppe_meter_acnt_res.acnt2Mgr.Acnt2UseState[word_num]>>word_offset)&1)){
                        acnt_idx = word_num*PPEBITSPERWORD + word_offset;
                        return acnt_idx;
                    }
                }
            }
        }
    }else
        return ppe_get_default_acnt_idx(acnt_type); 

    return ppe_get_default_acnt_idx(acnt_type); 
}

int ppe_set_acnt_status_enable(int index, int acnt_type, PPE_AcntManage_T  *acntManage){
    if(acnt_type == PPE_ACNT0_TYPE){
        if((index >= PPE_GRP0_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt0 idx range 0 : %d; real acnt0 idx :%d\n", PPE_GRP0_MAX_IDX_NUM, index);
            return 0;
        }else{
            ppe_meter_acnt_res.acnt0Mgr.Acnt0UseState[index>>5] |= (1<<(index&PPE_WORD_MASK)) ;   
            strlcpy(ppe_meter_acnt_res.acnt0Mgr.Acnt0UseFunc[index], acntManage->bind_func_name, PPE_NAME_LENGTH);
        }
    }else if(acnt_type == PPE_ACNT1_TYPE){
        if((index >= PPE_GRP1_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt1 idx range 0 : %d; real acnt1 idx :%d\n", PPE_GRP1_MAX_IDX_NUM, index);
            return 0;
        }else{
            ppe_meter_acnt_res.acnt1Mgr.Acnt1UseState[index>>5] |= (1<<(index&PPE_WORD_MASK)) ;
            strlcpy(ppe_meter_acnt_res.acnt1Mgr.Acnt1UseFunc[index], acntManage->bind_func_name, PPE_NAME_LENGTH);
        }
    }else if(acnt_type == PPE_ACNT2_TYPE){
        if((index >= PPE_GRP2_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt2 idx range 0 : %d; real acnt2 idx :%d\n", PPE_GRP2_MAX_IDX_NUM, index);
            return 0;
        }else{
            ppe_meter_acnt_res.acnt2Mgr.Acnt2UseState[index>>5] |= (1<<(index&PPE_WORD_MASK)) ;
            strlcpy(ppe_meter_acnt_res.acnt2Mgr.Acnt2UseFunc[index], acntManage->bind_func_name, PPE_NAME_LENGTH);
        }
    }else
        return 0; 

    return 0;
}

int ppe_set_acnt_status_disable(int index, int acnt_type){
    if(acnt_type == PPE_ACNT0_TYPE){
        if((index >= PPE_GRP0_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt0 idx range 0 : %d; real acnt0 idx :%d\n", PPE_GRP0_MAX_IDX_NUM, index);
            return 0;
        }else{
            ppe_meter_acnt_res.acnt0Mgr.Acnt0UseState[index>>5] &= ~(1<<(index&PPE_WORD_MASK)) ;    
            strcpy(ppe_meter_acnt_res.acnt0Mgr.Acnt0UseFunc[index], "");
        }                  
    }else if(acnt_type == PPE_ACNT1_TYPE){
        if((index >= PPE_GRP1_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt1 idx range 0 : %d; real acnt1 idx :%d\n", PPE_GRP1_MAX_IDX_NUM, index);
            return 0;
        }else{
            ppe_meter_acnt_res.acnt1Mgr.Acnt1UseState[index>>5] &= ~(1<<(index&PPE_WORD_MASK)) ;
            strcpy(ppe_meter_acnt_res.acnt1Mgr.Acnt1UseFunc[index], "");
        }   
    }else if(acnt_type == PPE_ACNT2_TYPE){
        if((index >= PPE_GRP2_MAX_IDX_NUM) || (index < 0)){
            printk("correct acnt2 idx range 0 : %d; real acnt2 idx :%d\n", PPE_GRP2_MAX_IDX_NUM, index);
            return 0;
        }else{
            ppe_meter_acnt_res.acnt2Mgr.Acnt2UseState[index>>5] &= ~(1<<(index&PPE_WORD_MASK)) ;
            strcpy(ppe_meter_acnt_res.acnt2Mgr.Acnt2UseFunc[index], "");
        }   
    }else
        return 0; 

    return 0;
}

int ppeHookAllocateAcnt(struct ecnt_ppe_data *ppe_data){
    int acnt_type = 0;
    PPE_AcntManage_T  *acntManage = ppe_data->acntManage;
    acnt_type = acntManage->acnt_type;
    
    acntManage->acnt_id = ppe_find_first_unused_acnt_index(acnt_type);

    if(acnt_type == PPE_ACNT0_TYPE){
        if(acntManage->acnt_id == PPE_GRP0_MAX_IDX_NUM){
            printk("%d acnt0 id has been used, there is no left\n", PPE_GRP0_MAX_IDX_NUM);
            return 0;
        }
    }
    else if(acnt_type == PPE_ACNT1_TYPE){
        if(acntManage->acnt_id == PPE_GRP1_MAX_IDX_NUM){
            printk("%d acnt1 id has been used, there is no left\n", PPE_GRP1_MAX_IDX_NUM);
            return 0;
        }
    }
    else if(acnt_type == PPE_ACNT2_TYPE){
        if(acntManage->acnt_id == PPE_GRP2_MAX_IDX_NUM){
            printk("%d acnt2 id has been used, there is no left\n", PPE_GRP2_MAX_IDX_NUM);
            return 0;
        }
    }

    ppe_set_acnt_status_enable(acntManage->acnt_id, acnt_type, acntManage);
    
    return 0;
}

int ppeHookFreeAcnt(struct ecnt_ppe_data *ppe_data){
    PPE_AcntManage_T  *acntManage = ppe_data->acntManage;
    int dex = 0;
    int acnt_type = 0;

    dex = acntManage->acnt_id ; 
    acnt_type = acntManage->acnt_type ;
    
    ppe_set_acnt_status_disable(dex, acnt_type);
    
    return 0;
}

int ppeHookGetAcnt0Cnt(struct ecnt_ppe_data *ppe_data)
{
    unchar acnt_id = ppe_data->acnt_statistic.acnt_id;

    if(!isEN7528)
        return -1;

    if(acnt_id>=PPE_GRP0_MAX_IDX_NUM)
        return -1;
    
    ppe_data->acnt_statistic.pkt_cnt_low = RegRead(PPE_ACNT_PCNT_L(0,acnt_id));
    ppe_data->acnt_statistic.pkt_cnt_High = RegRead(PPE_ACNT_PCNT_H(0,acnt_id));
    ppe_data->acnt_statistic.byte_low = RegRead(PPE_ACNT_BCNT_L(0,acnt_id));
    ppe_data->acnt_statistic.byte_high = RegRead(PPE_ACNT_BCNT_H(0,acnt_id));

	return 0;
}

int ppeHookGetAcnt1Cnt(struct ecnt_ppe_data *ppe_data)
{
    unchar acnt_id = ppe_data->acnt_statistic.acnt_id;

    if(!isEN751627 && !isEN7528)
        return -1;

    if(acnt_id>=PPE_GRP1_MAX_IDX_NUM)
        return -1;

    if(isEN7528)
    {
        ppe_data->acnt_statistic.pkt_cnt_low = RegRead(PPE_ACNT_PCNT_L(1,acnt_id));
        ppe_data->acnt_statistic.pkt_cnt_High = RegRead(PPE_ACNT_PCNT_H(1,acnt_id));
        ppe_data->acnt_statistic.byte_low = RegRead(PPE_ACNT_BCNT_L(1,acnt_id));
        ppe_data->acnt_statistic.byte_high = RegRead(PPE_ACNT_BCNT_H(1,acnt_id));

    }
    else
    {
        RegWrite(PPE_FCNT_CTRL, ((acnt_id)<<4));
        ppe_data->acnt_statistic.pkt_cnt_low = RegRead(PPE_FCNT_PCNT);
        ppe_data->acnt_statistic.byte_low = RegRead(PPE_FCNT_BCNT_L);
        ppe_data->acnt_statistic.byte_high = RegRead(PPE_FCNT_BCNT_H);
    }

	return 0;
}

int ppeHookClearAcnt1Cnt(struct ecnt_ppe_data *ppe_data)
{
    unchar acnt_id = ppe_data->acnt_statistic.acnt_id;

    if (isEN7528)
        return 0;

    if(!isEN751627)
        return -1;

    if(acnt_id < PPE_GRP1_MAX_IDX_NUM)
        RegWrite(PPE_FCNT_CTRL, (acnt_id<<4) | 1);
    else if(99 == acnt_id) 
        RegWrite(PPE_FCNT_CTRL, 1<<1);
    else
        return -1;

	return 0;
}

int ppeHookGetAcnt2Cnt(struct ecnt_ppe_data *ppe_data)
{
    unchar acnt_id = ppe_data->acnt_statistic.acnt_id;

    if(!isEN7528)
        return -1;

    if(acnt_id>=PPE_GRP2_MAX_IDX_NUM)
        return -1;
    
    ppe_data->acnt_statistic.pkt_cnt_low = RegRead(PPE_ACNT_PCNT_L(2,acnt_id));
    ppe_data->acnt_statistic.pkt_cnt_High = RegRead(PPE_ACNT_PCNT_H(2,acnt_id));
    ppe_data->acnt_statistic.byte_low = RegRead(PPE_ACNT_BCNT_L(2,acnt_id));
    ppe_data->acnt_statistic.byte_high = RegRead(PPE_ACNT_BCNT_H(2,acnt_id));

	return 0;
}
/* PPE Meter Acnt manage */

int ppeHookSetPpePort(struct ecnt_ppe_data *ppe_data)
{
	if(ppe_data->ppe_port == 5)
		RegWrite(PPE_DFP_CPORT, 0x5500);	//usb as 2.5G wan

	return 0;
}


