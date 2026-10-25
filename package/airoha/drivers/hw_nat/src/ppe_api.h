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
#ifndef _PPE_API_WANTED
#define _PPE_API_WANTED

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <linux/spinlock.h>
#include <linux/list.h>
#include "hwnat_ioctl.h"
#include "foe_fdb.h"
#include <ecnt_hook/ecnt_hook_ppe.h>

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define TLS_CFG_REG_NUM     6
#define TLS_VID_CFG_NUM     16
#define TLS_CFG_TAG_NUM     5

#define VLAN_ID_MAX         4096

#define DONT_CARE   0

#define ENABLE      1
#define DISABLE     0

#define ECNT_PPE_OK     0
#define ECNT_PPE_ERROR  1

#define TLS_REG_OFF_EN      0
#define TLS_REG_OFF_CM      1
#define TLS_REG_OFF_MA      2
#define TLS_REG_OFF_TAG_EN  3
#define TLS_REG_OFF_RNG     4
#define TLS_REG_OFF_MODE    5
#define TLS_REG_OFF_SPORT   8
#define TLS_REG_OFF_TAG0    12
#define TLS_REG_OFF_TAG1    16
#define TLS_REG_OFF_TAG2    20
#define TLS_REG_OFF_TAG3    24
#define TLS_REG_OFF_TAG4    28

#define TLS_VID_REG_OFF_VID_HI 16

#define METER_TOKEN_RATE_PKT_MAX    0x7ff
#define METER_TOKEN_RATE_BYTE_MAX   0x3fff

#define METER_BEGIN_INDEX_FOR_RATELIMIT	30
#define RATELIMIT_BY_INTERFACE_MAX		8
#define RATELIMIT_BY_IP_MAX				6

#define IP_VERSION_V4		0
#define IP_VERSION_V6		1

#define PPE_METER_MAX_NUM       63
#define PPE_NAME_LENGTH         16
#define PPE_METER_GROUP_NUM     2
#define PPE_ACNT0_GROUP_NUM     2
#define PPE_ACNT1_GROUP_NUM     1
#define PPE_ACNT2_GROUP_NUM     2

#define PPE_GRP0_MAX_IDX_NUM         (0x3f)
#define PPE_GRP1_MAX_IDX_NUM         (0x10)
#define PPE_GRP2_MAX_IDX_NUM         (0x40)

#define PPE_ERROR_METER_INDEX -1
#define PPE_METER_STATUS_DONE 0
#define PPEBITSPERWORD 32
#define PPE_WORD_MASK 0x1F

#define ACNT0_DEFAULT           (0x3f)
#define ACNT1_DEFAULT           (0x0)
#define ACNT2_DEFAULT           (0x0)

/************************************************************************
*               M A C R O S
*************************************************************************
*/
#define IS_VALID_TLS_SPORT(sp) \
                ((sp == TLS_SP_QDMA1)  || \
                (sp == TLS_SP_GDMA1)   || \
                (sp == TLS_SP_GDMA2)   || \
                (sp == TLS_SP_QDMA2)   || \
                (sp == TLS_SP_GSW_P0)  || \
                (sp == TLS_SP_GSW_P1)  || \
                (sp == TLS_SP_GSW_P2)  || \
                (sp == TLS_SP_GSW_P3))

#define TLS_REG_EN(reg) \
                ((reg>>TLS_REG_OFF_EN)&0x1)

#define TLS_REG_CM(reg) \
                ((reg>>TLS_REG_OFF_CM)&0x1)

#define TLS_REG_MA(reg) \
                ((reg>>TLS_REG_OFF_MA)&0x1)

#define TLS_REG_TAG_EN(reg) \
                ((reg>>TLS_REG_OFF_TAG_EN)&0x1)

#define TLS_REG_RNG(reg) \
                ((reg>>TLS_REG_OFF_RNG)&0x1)

#define TLS_REG_MODE(reg) \
                ((reg>>TLS_REG_OFF_MODE)&0x7)

#define TLS_REG_SPORT(reg) \
                ((reg>>TLS_REG_OFF_SPORT)&0xf)

#define TLS_REG_OFF_TAGS(idx) \
                (TLS_REG_OFF_TAG0+(idx<<2))

#define TLS_REG_TAGS(reg,idx) \
                ((reg>>TLS_REG_OFF_TAGS(idx))&0xf)


/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/
enum {
    METER_TICK_PKT_1MS = 0,
    METER_TICK_PKT_10MS,
    METER_TICK_PKT_100MS,
    METER_TICK_PKT_1S
};

enum {
    METER_BUCKET_PKT_512 = 0,
    METER_BUCKET_PKT_2K,
    METER_BUCKET_PKT_8K,
    METER_BUCKET_PKT_32K
};

enum {
    METER_TICK_BYTE_125US = 0,
    METER_TICK_BYTE_1MS
};

enum {
    METER_BUCKET_64KB = 0,
    METER_BUCKET_32KB,
    METER_BUCKET_16KB,
    METER_BUCKET_4KB
};

enum {
    RESOURCE_TYPE_METER = 0,
    RESOURCE_TYPE_ACCOUNT    
};

/* use tlsCfg[TLS_CFG_REG_NUM] and tlsVid[TLS_VID_CFG_NUM]
 * to record if registers PPE_TLS_CFG0~5 and PPE_TLS_VID0~7
 * are used */
struct ppeTlsResource
{
    unsigned char tlsCfg[TLS_CFG_REG_NUM];
    unsigned char tlsVid[TLS_VID_CFG_NUM];
    spinlock_t tlsLock;
};

extern struct ppeTlsResource tlsRsr;


struct meterFlow
{
    unsigned int vlan   :16; /* including DEI, Pbits, and vlanID */
    unsigned int dir    :1; /* 0:upstream, 1:downstream */
    unsigned int queue  :4; /* 0~8, for qos queue, 0:not care qos queue */
    unsigned int resv   :11;
};

struct meterGroup
{
    struct list_head list;
    struct meterFlow mFlow;
};

struct meterVlan
{
    struct list_head list;
    unsigned int group  :6; /* 0~62 */
    unsigned int foeIdx :16;
    unsigned int dir    :1; /* 0:upstream, 1:downstream */
    unsigned int pbits  :4; /* including VLAN DEL and Pbits */
    unsigned int isBind :1; /* check if flow is bind or not */
    unsigned int isTrtcm:1; /* check if this Meter is for trrcm */
    unsigned int queue	:4; /* 0~8, for qos queue, 0:not care qos queue */
    unsigned int resv   :31;
};

struct ppeMeterResource
{
    struct list_head mGroup[METER_GROUP_MAX];
    struct list_head mVid[VID_MAX];
    struct ppe_meter_cfg meterCfg[METER_GROUP_MAX];
    spinlock_t meterLock;
};

struct ppeAccountResource
{
    struct list_head aGroup[METER_GROUP_MAX];
    struct list_head aVid[VID_MAX];
    spinlock_t accountLock;
};

struct ppeRatelimitItfInfo
{
	unsigned char enable ;			/*this rule if active?*/
	unsigned int rate ;
};

struct ppeRatelimitIPInfo
{
	unsigned char enable ;			/*this rule if active?*/
	unsigned int rate ;

	unsigned char ip_version; 		/*0: IPv4 , 1: IPv6*/

	unsigned char v4_pair_l[4] ; 	/*IPv4 range*/
	unsigned char v4_pair_h[4] ;

	unsigned char v6_pair_l[16] ;	/*IPv6 range*/
	unsigned char v6_pair_h[16] ;
};

typedef struct {
    uint32                      MeterUseState[PPE_METER_GROUP_NUM] ;   /*global meter ctrl variable, bit map data*/
    char                        MeterUseFunc[PPE_METER_MAX_NUM][PPE_NAME_LENGTH] ;        /* name of meter bind function*/
}PPE_Meter_Manage_T ;

typedef struct {
    uint32                      Acnt0UseState[PPE_ACNT0_GROUP_NUM];   /*global acnt0 ctrl variable, bit map data*/
    char                        Acnt0UseFunc[PPE_GRP0_MAX_IDX_NUM][PPE_NAME_LENGTH] ;        /* name of acnt0 bind function*/
}PPE_ACNT0_Manage_T ;

typedef struct {
    uint32                      Acnt1UseState[PPE_ACNT1_GROUP_NUM];   /*global acnt1 ctrl variable, bit map data*/
    char                        Acnt1UseFunc[PPE_GRP1_MAX_IDX_NUM][PPE_NAME_LENGTH] ;        /* name of acnt1 bind function*/
}PPE_ACNT1_Manage_T ;

typedef struct {
    uint32                      Acnt2UseState[PPE_ACNT2_GROUP_NUM];   /*global acnt2 ctrl variable, bit map data*/
    char                        Acnt2UseFunc[PPE_GRP2_MAX_IDX_NUM][PPE_NAME_LENGTH] ;        /* name of acnt2 bind function*/
}PPE_ACNT2_Manage_T ;


typedef struct {
    PPE_Meter_Manage_T         meterMgr ;
    PPE_ACNT0_Manage_T         acnt0Mgr ;
    PPE_ACNT1_Manage_T         acnt1Mgr ;
    PPE_ACNT2_Manage_T         acnt2Mgr ;
} PPE_METER_ACNT_T ;

typedef int (*ppe_hook_func_t)(struct ecnt_ppe_data *);


/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern struct ppeMeterResource meterRsr;
extern struct ppeAccountResource accountRsr;
extern ppe_hook_func_t ppe_hook_funcs[];

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/
int ecnt_ppe_tls_add (struct ppe_tls_cfg *tlsCfg);
int ecnt_ppe_tls_del (struct ppe_tls_cfg *tlsCfg);
int ecnt_ppe_tls_show (struct ppe_tls_cfg *tlsCfg);

int ecnt_ppe_meter_add (struct ppe_meter_cfg *meterCfg);
int ecnt_ppe_meter_del (struct ppe_meter_cfg *meterCfg);
int ecnt_ppe_meter_show (struct ppe_meter_cfg *meterCfg);
int ecnt_ppe_meter_add_group (struct ppe_meter_cfg *meterCfg);
int ecnt_ppe_meter_del_group (struct ppe_meter_cfg *meterCfg);
int ecnt_ppe_meter_add_flow (struct ppe_meter_cfg *meterCfg);
int ecnt_ppe_meter_del_flow (struct ppe_meter_cfg *meterCfg);

int ecnt_ppe_trtcm_add (struct ppe_meter_cfg *trtcmCfg);
int ecnt_ppe_trtcm_del (struct ppe_meter_cfg *trtcmCfg);
int ecnt_ppe_trtcm_show (struct ppe_meter_cfg *trtcmCfg);
int ecnt_ppe_trtcm_add_group (struct ppe_meter_cfg *meterCfg);
int ecnt_ppe_trtcm_del_group (struct ppe_meter_cfg *meterCfg);
int ecnt_ppe_trtcm_add_flow (struct ppe_meter_cfg *meterCfg);
int ecnt_ppe_trtcm_del_flow (struct ppe_meter_cfg *meterCfg);


int ecnt_ppe_account_add (struct ppe_account_cfg *accountCfg);
int ecnt_ppe_account_del (struct ppe_account_cfg *accountCfg);
int ecnt_ppe_account_show (struct ppe_account_cfg *accountCfg);
int ecnt_ppe_account_stats_get (struct ppe_account_cfg *accountCfg);

#endif
