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
#include <asm/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <modules/ra_hwnat/hwnat_ioctl.h>
#include "libapi_lib_utility.h"
#include "libapi_lib_ppe.h"

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
enum ppeCmdEnum 
{
    PPE_CMD_TLS = 0,
    PPE_CMD_METER,
    PPE_CMD_TRTCM,
    PPE_CMD_ACCOUNT,
    PPE_CMD_CACHE_SET,
};

enum 
{
    TLS_ARG_SPORT = 0,
    TLS_ARG_MODE,
    TLS_ARG_MISS,
    TLS_ARG_VIDS,
    TLS_ARG_VID_RANGE,
    TLS_ARG_VID0,
    TLS_ARG_VID1,
    TLS_ARG_VID2,
    TLS_ARG_VID3,
    TLS_ARG_VID4,
    TLS_ARG_MAX
};

enum 
{
    METER_ARG_GROUP = 0,
    METER_ARG_MODE,
    METER_ARG_SCALE,
    METER_ARG_RATE,
    METER_ARG_VLAN,
    METER_ARG_QUEUE,
    METER_ARG_DIR,
    METER_ARG_MAX
};

enum 
{
    TRTCM_ARG_GROUP = 0,
    TRTCM_ARG_MODE,
    TRTCM_ARG_SCALE_PRI,
    TRTCM_ARG_RATE_PRI,
    TRTCM_ARG_SCALE_CRI,
    TRTCM_ARG_RATE_CRI,
    TRTCM_ARG_VLAN,
    TRTCM_ARG_DIR,
    TRTCM_ARG_MAX
};

enum 
{
    ACCOUNT_ARG_GROUP = 0,
    ACCOUNT_ARG_VLAN,
    ACCOUNT_ARG_DIR,
    ACCOUNT_ARG_MAX
};

enum 
{
    CACHE_ARG_MODE = 0,
    CACHE_ARG_MAX
};

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define CMD_METHOD_ADD      "add"
#define CMD_METHOD_DEL      "del"
#define CMD_METHOD_SHOW     "show"
#define CMD_METHOD_ENABLE   "enable"
#define CMD_METHOD_DISABLE  "disable"
#define CMD_METHOD_CLEAR    "clear"
#define CMD_METHOD_SET      "set"
#define CMD_METHOD_GET      "get"

#ifdef CONFIG_TP_IMAGE
#define CMD_METHOD_BACKUP	"backup"
#endif /* CONFIG_TP_IMAGE */


/* TLS command */
#define SPORT_STRING        "sport"
#define MODE_STRING         "mode"
#define MISS_STRING         "miss"
#define VIDS_STRING         "vids"
#define VID_RANGE_STRING    "vid_range"
#define VID0_STRING         "vid0"
#define VID1_STRING         "vid1"
#define VID2_STRING         "vid2"
#define VID3_STRING         "vid3"
#define VID4_STRING         "vid4"

/* meter command */
#define GROUP_STRING        "group"
#define RATE_STRING         "rate"
#define SCALE_STRING        "scale"
#define VLAN_STRING         "vlan"
#define DIR_STRING          "dir"
#define QUEUE_STRING        "queue"
#define ALL_STRING          "all"

/* trtcm command */
#define SCALE_PRI_STRING    "scale_pir"
#define SCALE_CRI_STRING    "scale_cir"
#define RATE_PRI_STRING     "rate_pir"
#define RATE_CRI_STRING     "rate_cir"

#define VALID_STRING        "valid"
#define INVALID_STRING      "invalid"
#define LOCKED_STRING       "locked"
#define LINECNT_STRING      "lineCnt"

#define BITSHIFT_STRING     "bitShift"
#define TLS_STRING          "tls"
#define L2B_STRING          "l2b"

#define ENABLE_STRING       "enable"
#define DISABLE_STRING      "disable"
#define DMAC_STRING         "dmac"
#define PBITS_STRING        "pbits"
#define ETYPE_DSCP_STRING   "etype_dscp"
#define SMAC_STRING         "smac"

#define TRAFFIC_QOS_CNT     "cnt"
#define TRAFFIC_QOS_RATE    "rate"

#define DPORT_STRING        "dport"

#define ENABLE_CONFIG_STRING        "enable_config"
#define MODE_CONFIG_STRING        "mode_config"

#define PPE_ARG_VAL_LENGTH      64

#define PPE_METER_PKT_RATE_MAX  (METER_PKT_RATE_BASE * METER_SCALE_1000)

#ifdef CONFIG_TP_IMAGE
#define BACKUP_STRING	"bk"
#define RESTORE_STRING	"rst"
#define SHOW_BACKUP_STRING	"show"
#endif /* CONFIG_TP_IMAGE */

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/
struct argVal;

typedef int (*argCheckFunc)(char*, struct argVal*, int); 

struct argVal 
{
    char *arg;
    char val[PPE_ARG_VAL_LENGTH];
    argCheckFunc chkFunc;
    unsigned char hasVal; /* 0: val is still NULL_STRING, 1: val is assigned a value string */
    unsigned int data;
};

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
extern int do_ppe_ioctl(void *opt, unsigned long command);
static int isTlsSport(char *sportP, struct argVal argVals[], int idx);
static int isTlsMode(char *modeP, struct argVal argVals[], int idx);
static int isTlsMissAction(char *missP, struct argVal argVals[], int idx);
static int isValidVid(char *vidP, struct argVal argVals[], int idx);
static int isMeterGroupId(char *gidP, struct argVal argVals[], int idx);
static int isMeterMode(char *modeP, struct argVal argVals[], int idx);
static int isMeterScale(char *scaleP, struct argVal argVals[], int idx);
static int isMeterRate(char *rateP, struct argVal argVals[], int idx);
static int is2byteVlan(char *vlanP, struct argVal argVals[], int idx);
static int isMeterQueueId(char *queueId, struct argVal argVals[], int idx);
static int isMeterDir(char *dirP, struct argVal argVals[], int idx);
static int isTrtcmGroupId(char *gidP, struct argVal argVals[], int idx);
static int isPirRate(char *rateP, struct argVal argVals[], int idx);
static int isCirRate(char *rateP, struct argVal argVals[], int idx);

static int do_packet_mirror(int argc, char *argv[], void *p);
static int do_packet_mirror_disable(int argc, char *argv[], void *p);
static int do_packet_mirror_enable(int argc, char *argv[], void *p);
static int do_wan_mtu(int argc, char *argv[], void *p);
static int do_set_wan_mtu(int argc, char *argv[], void *p);
static int do_clear_wan_mtu(int argc, char *argv[], void *p);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
struct ppe_tls_cfg gPpeTlsCfg;
struct ppe_meter_cfg gPpeMeterCfg;
struct ppe_account_cfg gPpeAccountCfg;
struct ppe_cache_cfg gPpeCacheCfg;
struct ppe_hash_cfg gPpeHashCfg;

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
static struct cmdVal tlsModeArr[TLS_MODE_MAX] =
{
    {TLS_MODE_CMD_UNI,      TLS_MODE_SPORT},
    {TLS_MODE_CMD_UUTAG,    TLS_MODE_UNTAG},
    {TLS_MODE_CMD_PRI_TAG,  TLS_MODE_PRI_TAG},
    {TLS_MODE_CMD_CTAG,     TLS_MODE_CTAG},
    {TLS_MODE_CMD_STAG_SIN, TLS_MODE_STAG_SIN},
    {TLS_MODE_CMD_DBL_TAG,  TLS_MODE_DBL_TAG},
    {TLS_MODE_CMD_CTAG_NON, TLS_MODE_CTAG_NON},
    {TLS_MODE_CMD_STAG_SPE, TLS_MODE_STAG_SPE}
};

static struct cmdVal tlsSportArr[TLS_SPORT_CMD_MAX] =
{
    {TLS_SPORT_CMD_LAN0, TLS_SP_GSW_P0},
    {TLS_SPORT_CMD_LAN1, TLS_SP_GSW_P1},
    {TLS_SPORT_CMD_LAN2, TLS_SP_GSW_P2},
    {TLS_SPORT_CMD_LAN3, TLS_SP_GSW_P3},
    {TLS_SPORT_CMD_WAN,  TLS_SP_GDMA2}
};

static struct cmdVal cacheStrVals[] =
{
    {VALID_STRING,      PPE_CACHE_TYPE_VALID},
    {INVALID_STRING,    PPE_CACHE_TYPE_INVALID},
    {LOCKED_STRING,     PPE_CACHE_TYPE_LOCKED},
    {ALL_STRING,        PPE_CACHE_TYPE_ALL},
    {LINECNT_STRING,    PPE_CACHE_TYPE_LINECNT},
    {NULL,              0}
};

static struct argVal tlsArgVals[] =
{
    {SPORT_STRING,      NULL_STRING,    isTlsSport,     0,  0},
    {MODE_STRING,       NULL_STRING,    isTlsMode,      0,  0},
    {MISS_STRING,       NULL_STRING,    isTlsMissAction,0,  0},
    {VIDS_STRING,       NULL_STRING,    NULL,           0,  0},
    {VID_RANGE_STRING,  NULL_STRING,    NULL,           0,  0},
    {VID0_STRING,       NULL_STRING,    isValidVid,     0,  0},
    {VID1_STRING,       NULL_STRING,    isValidVid,     0,  0},
    {VID2_STRING,       NULL_STRING,    isValidVid,     0,  0},
    {VID3_STRING,       NULL_STRING,    isValidVid,     0,  0},
    {VID4_STRING,       NULL_STRING,    isValidVid,     0,  0},
    {NULL,              NULL_STRING,    NULL,           0,  0}
};

static struct argVal meterArgVals[] =
{
    {GROUP_STRING,      NULL_STRING,    isMeterGroupId, 0,  0},
    {MODE_STRING,       NULL_STRING,    isMeterMode,    0,  0},
    {SCALE_STRING,      NULL_STRING,    isMeterScale,   0,  0},
    {RATE_STRING,       NULL_STRING,    isMeterRate,    0,  0},
    {VLAN_STRING,       NULL_STRING,    is2byteVlan,    0,  0},
    {QUEUE_STRING,      NULL_STRING,    isMeterQueueId, 0,  0},
    {DIR_STRING,        NULL_STRING,    isMeterDir,     0,  0},
    {NULL,              NULL_STRING,    NULL,           0,  0}
};

static struct argVal trtcmArgVals[] =
{
    {GROUP_STRING,      NULL_STRING,    isTrtcmGroupId, 0,  0},
    {MODE_STRING,       NULL_STRING,    isMeterMode,    0,  0},
    {SCALE_PRI_STRING,  NULL_STRING,    isMeterScale,   0,  0},
    {RATE_PRI_STRING,   NULL_STRING,    isPirRate,      0,  0},
    {SCALE_CRI_STRING,  NULL_STRING,    isMeterScale,   0,  0},
    {RATE_CRI_STRING,   NULL_STRING,    isCirRate,      0,  0},
    {VLAN_STRING,       NULL_STRING,    is2byteVlan,    0,  0},
    {DIR_STRING,        NULL_STRING,    isMeterDir,     0,  0},
    {NULL,              NULL_STRING,    NULL,           0,  0}
};

static struct argVal accountArgVals[] =
{
    {GROUP_STRING,      NULL_STRING,    isMeterGroupId, 0,  0},
    {VLAN_STRING,       NULL_STRING,    is2byteVlan,    0,  0},
    {DIR_STRING,        NULL_STRING,    isMeterDir,     0,  0},
    {NULL,              NULL_STRING,    NULL,           0,  0}
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
static int isNumber(char *vidP)
{
    int i;

    for (i = 0; i < strlen(vidP); i++)
        if (!isdigit(vidP[i]))
            return 0;

    return 1;
}

static int isTlsSport(char *sportP, struct argVal argVals[], int idx)
{
    int i;

    for (i = 0; i < TLS_SPORT_CMD_MAX; i++) 
    {
        if (strncmp(sportP, tlsSportArr[i].cmd, strlen(tlsSportArr[i].cmd)) == 0) 
        {
            argVals[idx].data = tlsSportArr[i].val;
            return 1;
        }
    }

    return 0;
}

static int isTlsMode(char *modeP, struct argVal argVals[], int idx)
{
    int i;

    for (i = 0; i < TLS_MODE_CMD_MAX; i++) 
    {
        if (strncmp(modeP, tlsModeArr[i].cmd, strlen(tlsModeArr[i].cmd)) == 0) 
        {
            argVals[idx].data = tlsModeArr[i].val;
            return 1;
        }
    }

    return 0;
}

static int isTlsMissAction(char *missP, struct argVal argVals[], int idx)
{
    if (strncmp(missP, TLS_MA_CMD_DROP, strlen(TLS_MA_CMD_DROP)) == 0) 
    {
        argVals[idx].data = TLS_MISS_DROP;
        return 1;
    }
    else if (strncmp(missP, TLS_MA_CMD_NONTLS, strlen(TLS_MA_CMD_NONTLS)) == 0) 
    {
        argVals[idx].data = TLS_MISS_NON_TLS;
        return 1;
    }
    else
        return 0;
}

static int isValidVid(char *vidP, struct argVal argVals[], int idx)
{
    unsigned int vid;

    if (!isNumber(vidP))
        return 0;

    vid = atoi(vidP);

    if (vid < VID_MAX)
        return 1;
    else
        return 0;
}

static int isTlsModeNeedVid(char *modeP)
{
    int i;

    for (i = 0; i < TLS_MODE_MAX; i++)
        if (strncmp(modeP, tlsModeArr[i].cmd, strlen(tlsModeArr[i].cmd)) == 0)
            break;

    if (i >= TLS_MODE_CTAG)
        return 1;
    else
        return 0;
}

static int isTlsArgsHaveVids(void)
{
    int i;

    for (i = TLS_ARG_VIDS; i <= TLS_ARG_VID4; i++)
    {
        if (tlsArgVals[i].hasVal)
            return 1;
    }

    return 0;
}

static int isTlsArgsVidsCheckOK(void)
{
    if (tlsArgVals[TLS_ARG_VID_RANGE].hasVal && tlsArgVals[TLS_ARG_VIDS].hasVal) 
    {
        printf("\nError: \"%s\" and \"%s\" should not both exist\n",
                        tlsArgVals[TLS_ARG_VIDS].arg, tlsArgVals[TLS_ARG_VID_RANGE].arg);
    }
    else if (tlsArgVals[TLS_ARG_VID_RANGE].hasVal && !tlsArgVals[TLS_ARG_VIDS].hasVal)
    {
        if (tlsArgVals[TLS_ARG_VID0].hasVal && tlsArgVals[TLS_ARG_VID1].hasVal && 
            !tlsArgVals[TLS_ARG_VID2].hasVal && !tlsArgVals[TLS_ARG_VID3].hasVal &&
            !tlsArgVals[TLS_ARG_VID4].hasVal)
        {
            return 1;
        }
        else
            printf("\nError: \"%s\" should have %s and %s only\n",tlsArgVals[TLS_ARG_VID_RANGE].arg,
                        tlsArgVals[TLS_ARG_VID0].arg, tlsArgVals[TLS_ARG_VID1].arg);
    }
    else if (!tlsArgVals[TLS_ARG_VID_RANGE].hasVal && tlsArgVals[TLS_ARG_VIDS].hasVal)
    {
        if (tlsArgVals[TLS_ARG_VID0].hasVal)
            return 1;
        else
            printf("\nError: \"%s\" should have value\n", tlsArgVals[TLS_ARG_VID0].arg);
    }
    else
        printf("\nError: \"%s\" and \"%s\" should not both non-exist\n",
                        tlsArgVals[TLS_ARG_VIDS].arg, tlsArgVals[TLS_ARG_VID_RANGE].arg);

    return 0;
}

static int isGroupId(char *gidP, struct argVal argVals[], int idx, int max)
{
    int i, gid;

    if (strncmp(gidP, ALL_STRING, strlen(ALL_STRING)) == 0)
    {
        argVals[idx].data = max;
        return 1;
    }

    for (i = 0; i < strlen(gidP); i++)
        if (!isdigit(gidP[i]))
            return 0;

    gid = atoi(gidP);

    if (0 <= gid && gid < max) 
    {
        argVals[idx].data = gid;
        return 1;
    }
    else
        return 0;
}

static int isMeterGroupId(char *gidP, struct argVal argVals[], int idx)
{
    return isGroupId(gidP, argVals, idx, METER_GROUP_MAX);
}

static int isTrtcmGroupId(char *gidP, struct argVal argVals[], int idx)
{
    return isGroupId(gidP, argVals, idx, TRTCM_GROUP_MAX);
}

static int isMeterMode(char *modeP, struct argVal argVals[], int idx)
{
    if (strncmp(modeP, BYTE_STRING, strlen(BYTE_STRING)) == 0) 
    {
        argVals[idx].data = METER_MODE_BYTE;
        return 1;
    }
    else if (strncmp(modeP, PACKET_STRING, strlen(PACKET_STRING)) == 0) 
    {
        argVals[idx].data = METER_MODE_PACKET;
        return 1;
    }
    else
        return 0;
}

static int isMeterScale(char *scaleP, struct argVal argVals[], int idx)
{
    if (!argVals[METER_ARG_MODE].hasVal || 
        strncmp(argVals[METER_ARG_MODE].val, PACKET_STRING, strlen(PACKET_STRING)) == 0)
    {
        printf("\nERROR: packet mode doesn't need arg \"%s\"\n", SCALE_STRING);
        return 0;
    }

    if (strncmp(scaleP, K_STRING, strlen(K_STRING)) == 0) 
    {
        argVals[idx].data = METER_SCALE_KBPS;
        return 1;
    }
    else if (strncmp(scaleP, M_STRING, strlen(M_STRING)) == 0) 
    {
        argVals[idx].data = METER_SCALE_MBPS;
        return 1;
    }
    else if (strncmp(scaleP, G_STRING, strlen(G_STRING)) == 0) 
    {
        argVals[idx].data = METER_SCALE_GBPS;
        return 1;
    }
    else
        return 0;
}

static int isCorrectRate(char *rateP, struct argVal argVals[], int idx, int scaleEnum)
{
    unsigned int rate, val;

    if (!isNumber(rateP)) 
    {
        printf("\nERROR: arg \"%s\"'s value should a number\n", RATE_STRING);
        return 0;
    }
    else
        rate = atoi(rateP);

    if (!argVals[METER_ARG_MODE].hasVal)
    {
        printf("\nERROR: arg \"%s\" needs arg \"%s\"\n", RATE_STRING, MODE_STRING);
        return 0;
    }

    if (strncmp(argVals[METER_ARG_MODE].val, BYTE_STRING, strlen(BYTE_STRING)) == 0)
    {
        if (!argVals[scaleEnum].hasVal)
        {
            printf("\nERROR: arg \"%s\" needs arg \"%s\" in %s mode\n", RATE_STRING, SCALE_STRING, BYTE_STRING);
            return 0;
        }

        if (strncmp(argVals[scaleEnum].val, K_STRING, strlen(K_STRING)) == 0)
            val = rate * METER_SCALE_K;
        else if (strncmp(argVals[scaleEnum].val, M_STRING, strlen(M_STRING)) == 0)
            val = rate * METER_SCALE_M;
        else /* Gbps */
            val = rate * METER_SCALE_G;

        if (!(val == 0 || (16*METER_SCALE_K <= val && val <= 2*METER_SCALE_G)))
        {
            printf("\nERROR: arg \"%s\"'s value:%d is wrong for %s mode\n", RATE_STRING, rate, BYTE_STRING);
            return 0;
        }
    }
    else /* packet mode */
    {
        if (!(rate <= PPE_METER_PKT_RATE_MAX))
        {
            printf("\nERROR: arg \"%s\"'s value:%d is wrong for %s mode\n", RATE_STRING, rate, PACKET_STRING);
            return 0;
        }
    }

    argVals[idx].data = rate;
    return 1;

}

static int isMeterRate(char *rateP, struct argVal argVals[], int idx)
{
    return isCorrectRate(rateP, argVals, idx, METER_ARG_SCALE);
}

static int isPirRate(char *rateP, struct argVal argVals[], int idx)
{
    return isCorrectRate(rateP, argVals, idx, TRTCM_ARG_SCALE_PRI);
}

static int isCirRate(char *rateP, struct argVal argVals[], int idx)
{
    return isCorrectRate(rateP, argVals, idx, TRTCM_ARG_SCALE_CRI);
}

static int is2byteVlan(char *vlanP, struct argVal argVals[], int idx)
{
    unsigned int vlan;

    vlan = strtoll(vlanP, NULL, 16);

    if (vlan <= 0xffff) 
    {
        argVals[idx].data = vlan;
        return 1;
    }
    else
        return 0;
}

static int isMeterQueueId(char *queueId, struct argVal argVals[], int idx)
{
    int qId;

    qId = atoi(queueId);

    printf("\nisMeterQueueId qId = %d \n", qId);

    if (0 <= qId && qId <= METER_QUEUE_MAX) 
    {
        argVals[idx].data = qId;
        return 1;
    }
    else
        return 0;

}

static int isMeterDir(char *dirP, struct argVal argVals[], int idx)
{
    if (strncmp(dirP, UP_STRING, strlen(UP_STRING)) == 0) 
    {
        argVals[idx].data = METER_DIR_UP;
        return 1;
    }
    else if (strncmp(dirP, DOWN_STRING, strlen(DOWN_STRING)) == 0) 
    {
        argVals[idx].data = METER_DIR_DOWN;
        return 1;
    }
    else
        return 0;
}

static int isMustMaveNotFound(int idx, char *method, enum ppeCmdEnum cmd)
{
    if (
        (cmd == PPE_CMD_TLS && 
        (((strncmp(method, CMD_METHOD_ADD, strlen(CMD_METHOD_ADD)) == 0) && idx <= TLS_ARG_MISS) ||
         ((strncmp(method, CMD_METHOD_DEL, strlen(CMD_METHOD_DEL)) == 0) && idx == TLS_ARG_SPORT))) ||
        (cmd == PPE_CMD_ACCOUNT && 
         (strncmp(method, CMD_METHOD_ADD, strlen(CMD_METHOD_ADD)) == 0)) ||
        (cmd == PPE_CMD_CACHE_SET && 
         (strncmp(method, CMD_METHOD_SET, strlen(CMD_METHOD_SET)) == 0))
    ){
        return 1;
    }
    else
        return 0;
}
        
static int argsParsing(int argc, char *argv[], struct argVal argVals[], enum ppeCmdEnum cmd)
{
    int i = 0, j;


    while (argVals[i].arg)
    {
        /* initialize */
        strcpy(argVals[i].val, NULL_STRING);
        argVals[i].hasVal = 0;
        
        for (j = 0; j < argc; j++)
        {
            if (strncmp(argVals[i].arg, argv[j], strlen(argVals[i].arg)) == 0)
            {
                if (argVals[i].chkFunc) /* for those args that come with value */
                {
                    if (j < (argc-1)) {
                        if (argVals[i].chkFunc(argv[j+1], argVals, i)) {
                            strncpy(argVals[i].val, argv[j+1], PPE_ARG_VAL_LENGTH);
                            argVals[i].val[PPE_ARG_VAL_LENGTH - 1] = '\0';
                            argVals[i].hasVal = 1;
                        }
                        else {
                            printf("\nError: Wrong value \"%s\" for arg \"%s\"\n", argv[j+1], argVals[i].arg);
                            return -1;
                        }
                    }
                    else {
                        printf("\nError: No value for arg \"%s\"\n", argVals[i].arg);
                        return -1;
                    }
                } 
                else {
                    strcpy(argVals[i].val, YES_STRING);
                    argVals[i].hasVal = 1;
                }
                
                break;
            }
        }

        if (j == argc && isMustMaveNotFound(i, argv[0], cmd))
        {
            printf("\nError: can't find arg \"%s\"\n", argVals[i].arg);
            return -1;
        }
        i++;
    }

    return 0;
}

static int isCheckTlsAddArgsFailed(void)
{
    struct argVal *argVals = tlsArgVals;

    if (!isTlsModeNeedVid(argVals[TLS_ARG_MODE].val) && isTlsArgsHaveVids()) 
    {
        printf("\nError: TLS mode \"%s\" doesn't need vids\n", argVals[TLS_ARG_MODE].val);
        return -1;
    }
    else if (isTlsModeNeedVid(argVals[TLS_ARG_MODE].val) && !isTlsArgsHaveVids()) 
    {
        printf("\nError: TLS mode \"%s\" should have vids\n", argVals[TLS_ARG_MODE].val);
        return -1;
    }
    else if (isTlsModeNeedVid(argVals[TLS_ARG_MODE].val) && !isTlsArgsVidsCheckOK()) 
    {
        return -1;
    }  
    else
        return 0; /* check OK */
}

static int isCheckMeterAddArgsFailed(void)
{
    struct argVal *argVals = meterArgVals;

    if (argVals[METER_ARG_GROUP].hasVal
        && argVals[METER_ARG_MODE].hasVal
        && argVals[METER_ARG_RATE].hasVal
        && !argVals[METER_ARG_VLAN].hasVal
        && !argVals[METER_ARG_QUEUE].hasVal
        && !argVals[METER_ARG_DIR].hasVal) 
    {
        return 0;
    }

    if (argVals[METER_ARG_GROUP].hasVal
        && !argVals[METER_ARG_MODE].hasVal
        && !argVals[METER_ARG_SCALE].hasVal
        && !argVals[METER_ARG_RATE].hasVal
        && argVals[METER_ARG_VLAN].hasVal
        && argVals[METER_ARG_DIR].hasVal) 
    {
        return 0;
    }
    
    printf("\nError: isCheckMeterAddArgsFailed\n");
    return -1; /* check fail */
}

static int isCheckMeterDelShowArgsFailed(void)
{   
    struct argVal *argVals = meterArgVals;

    if (argVals[METER_ARG_GROUP].hasVal==1 && argVals[METER_ARG_MODE].hasVal==0 &&
        argVals[METER_ARG_SCALE].hasVal==0 && argVals[METER_ARG_RATE].hasVal==0 &&
        argVals[METER_ARG_VLAN].hasVal==0 && argVals[METER_ARG_DIR].hasVal==0)
        return 0;
    else if (argVals[METER_ARG_GROUP].hasVal==0 && argVals[METER_ARG_MODE].hasVal==0 &&
        argVals[METER_ARG_SCALE].hasVal==0 && argVals[METER_ARG_RATE].hasVal==0 &&
        argVals[METER_ARG_VLAN].hasVal==1 && argVals[METER_ARG_DIR].hasVal==1)
        return 0;
    else 
    {
        printf("\nError: only need arg \"%s\" or args \"%s and %s\"\n",
                    GROUP_STRING, VLAN_STRING, DIR_STRING);
        return -1; /* check fail */
    }      
}

static int isCheckTrtcmAddArgsFailed(void)
{
    struct argVal *argVals = trtcmArgVals;

    if (argVals[TRTCM_ARG_GROUP].hasVal
        && argVals[TRTCM_ARG_MODE].hasVal
        && argVals[TRTCM_ARG_RATE_PRI].hasVal
        && argVals[TRTCM_ARG_RATE_CRI].hasVal
        && !argVals[TRTCM_ARG_VLAN].hasVal
        && !argVals[TRTCM_ARG_DIR].hasVal) 
    {
        return 0;
    }

    if (argVals[TRTCM_ARG_GROUP].hasVal
        && !argVals[TRTCM_ARG_MODE].hasVal
        && !argVals[TRTCM_ARG_SCALE_PRI].hasVal
        && !argVals[TRTCM_ARG_SCALE_CRI].hasVal
        && !argVals[TRTCM_ARG_RATE_PRI].hasVal
        && !argVals[TRTCM_ARG_RATE_CRI].hasVal
        && argVals[TRTCM_ARG_VLAN].hasVal
        && argVals[TRTCM_ARG_DIR].hasVal) 
    {
        return 0;
    }

    printf("\nError: isCheckTrtcmAddArgsFailed\n");
    return -1; /* check fail */
}

static int isCheckTrtcmDelShowArgsFailed(void)
{
    struct argVal *argVals = trtcmArgVals;

    if (argVals[TRTCM_ARG_GROUP].hasVal==1 && argVals[TRTCM_ARG_MODE].hasVal==0 &&
        argVals[TRTCM_ARG_SCALE_PRI].hasVal==0 && argVals[TRTCM_ARG_RATE_PRI].hasVal==0 &&
        argVals[TRTCM_ARG_SCALE_CRI].hasVal==0 && argVals[TRTCM_ARG_RATE_CRI].hasVal==0 &&
        argVals[TRTCM_ARG_VLAN].hasVal==0 && argVals[TRTCM_ARG_DIR].hasVal==0)
        return 0;
    else if (argVals[TRTCM_ARG_GROUP].hasVal==0 && argVals[TRTCM_ARG_MODE].hasVal==0 &&
        argVals[TRTCM_ARG_SCALE_PRI].hasVal==0 && argVals[TRTCM_ARG_SCALE_PRI].hasVal==0 &&
        argVals[TRTCM_ARG_SCALE_CRI].hasVal==0 && argVals[TRTCM_ARG_RATE_CRI].hasVal==0 &&
        argVals[TRTCM_ARG_VLAN].hasVal==1 && argVals[TRTCM_ARG_DIR].hasVal==1)
        return 0;
    else 
    {
        printf("\nError: only need arg \"%s\" or args \"%s and %s\"\n",
                    GROUP_STRING, VLAN_STRING, DIR_STRING);
        return -1; /* check fail */
    }      
}

static int isCheckAccountDelShowArgsFailed(void)
{
    struct argVal *argVals = accountArgVals;    

    if (argVals[ACCOUNT_ARG_GROUP].hasVal==1 &&
        argVals[ACCOUNT_ARG_VLAN].hasVal==0 && argVals[ACCOUNT_ARG_DIR].hasVal==0)
        return 0;
    else if (argVals[ACCOUNT_ARG_GROUP].hasVal==0 &&
        argVals[ACCOUNT_ARG_VLAN].hasVal==1 && argVals[ACCOUNT_ARG_DIR].hasVal==1)
        return 0;
    else 
    {
        printf("\nError: only need arg \"%s\" or args \"%s and %s\"\n",
                    GROUP_STRING, VLAN_STRING, DIR_STRING);
        return -1; /* check fail */
    }      
}

/*******************************************************************************/
/*  TLS commands                                                                                                               */
/*******************************************************************************/
static int do_tls_add(int argc, char *argv[], void *p)
{
    struct argVal *argVals = tlsArgVals;
    unsigned int i, idx = 0;
    ECNT_PPEMGR_TLS_ENTRY tls = {0};

    if (argsParsing(argc, argv, argVals, PPE_CMD_TLS))
        goto tls_add_args_check_fail;

    if (isCheckTlsAddArgsFailed())
        goto tls_add_args_check_fail;

    tls.sport = argVals[TLS_ARG_SPORT].data;
    tls.mode  = argVals[TLS_ARG_MODE].data;
    tls.miss_action = argVals[TLS_ARG_MISS].data;

    if (argVals[TLS_ARG_VID_RANGE].hasVal)
    {
        tls.vlan_type = ECNT_PPEMGR_TLS_VID_PATTERN_RANGE;
        tls.vid_min = atoi(argVals[TLS_ARG_VID0].val);
        tls.vid_max = atoi(argVals[TLS_ARG_VID1].val);
    }
    else if (argVals[TLS_ARG_VIDS].hasVal)
    {
        tls.vlan_type = ECNT_PPEMGR_TLS_VID_PATTERN_VID;
        for (i = TLS_ARG_VID0; i <= TLS_ARG_VID4; i++) 
        {
            if (argVals[i].hasVal)
            {
                tls.vids[idx++] = atoi(argVals[i].val);
                tls.vid_num++;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        tls.vlan_type = ECNT_PPEMGR_TLS_VID_PATTERN_NONE;
    }

    return ppemgr_lib_add_transparet_line_service(&tls);

tls_add_args_check_fail:
    printf("\nUsage:\n" 
            "ppe tls add\n\t"
            "sport (%s/%s/%s/%s/%s)\n\t"
            "mode (%s/%s/%s/%s/%s/%s/%s/%s)\n\t"
            "miss (%s/%s)\n\t"
            "vids/vid_range\n\t"
            "vid0 (0~4095) vid1 (0~4095) vid2 (0~4095) vid3 (0~4095) vid4 (0~4095)\n\n",
            TLS_SPORT_CMD_LAN0, TLS_SPORT_CMD_LAN1, TLS_SPORT_CMD_LAN2, TLS_SPORT_CMD_LAN3, TLS_SPORT_CMD_WAN,
            TLS_MODE_CMD_UNI, TLS_MODE_CMD_UUTAG, TLS_MODE_CMD_PRI_TAG, TLS_MODE_CMD_CTAG, TLS_MODE_CMD_STAG_SIN, TLS_MODE_CMD_DBL_TAG, TLS_MODE_CMD_CTAG_NON, TLS_MODE_CMD_STAG_SPE,
            TLS_MA_CMD_DROP, TLS_MA_CMD_NONTLS);
        
    return -1;
}

static int do_tls_del(int argc, char *argv[], void *p)
{
    struct argVal *argVals = tlsArgVals;
    ECNT_PPEMGR_TLS_ENTRY tls = {0};

    if (argsParsing(argc, argv, argVals, PPE_CMD_TLS))
        goto tls_del_args_check_fail;

    tls.sport = argVals[TLS_ARG_SPORT].data;

    return ppemgr_lib_del_transparet_line_service(&tls);

tls_del_args_check_fail:
    printf("\nUsage:\n" 
            "ppe tls del\n\t"
            "sport (%s/%s/%s/%s/%s)\n\n", 
            TLS_SPORT_CMD_LAN0, TLS_SPORT_CMD_LAN1, TLS_SPORT_CMD_LAN2, TLS_SPORT_CMD_LAN3, TLS_SPORT_CMD_WAN);
    return -1; 
}

static int do_tls_show(int argc, char *argv[], void *p)
{
    struct ppe_tls_cfg *tlsCfg = &gPpeTlsCfg;

    tlsCfg->method = PPE_METHOD_SHOW;

    if (do_ppe_ioctl(tlsCfg, PPE_COMMAND_TLS))
        return -1;

    return 0;
}

static cmds_t tlsCmds[] = 
{
    {CMD_METHOD_ADD,        do_tls_add,         0x10, 0, NULL},
    {CMD_METHOD_DEL,        do_tls_del,         0x10, 0, NULL},
    {CMD_METHOD_SHOW,       do_tls_show,        0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};

/*******************************************************************************/
/*  METER commands                                                                                                           */
/*******************************************************************************/
static int do_meter_add(int argc, char *argv[], void *p)
{
    struct argVal *argVals = meterArgVals;
    ECNT_PPEMGR_METER_GROUP group = {0};
    ECNT_PPEMGR_METER_FLOW flow = {0};

    if (argsParsing(argc, argv, argVals, PPE_CMD_METER))
        goto meter_add_args_check_fail;
    
    if (isCheckMeterAddArgsFailed())
        goto meter_add_args_check_fail;

    if (argVals[METER_ARG_MODE].hasVal)
    {
        group.group_id  = argVals[METER_ARG_GROUP].data;
        group.enable    = 1;
        group.mode      = argVals[METER_ARG_MODE].data;
        group.scale     = argVals[METER_ARG_SCALE].data;
        group.rate      = argVals[METER_ARG_RATE].data;
        
        return ppemgr_lib_set_meter_group(&group);
    }

    if (argVals[METER_ARG_VLAN].hasVal)
    {
        flow.group_id   = argVals[METER_ARG_GROUP].data;
        flow.direction  = argVals[METER_ARG_DIR].data;
        flow.vlan       = argVals[METER_ARG_VLAN].data;
        flow.queue      = argVals[METER_ARG_QUEUE].data;

        return ppemgr_lib_add_meter_flow(&flow);
    }

meter_add_args_check_fail:
    printf("\nUsage:\n" 
            "ppe meter add\n\t"
            "group (0~62)\n\t"
            "mode (byte/packet)\n\t"
            "[scale (K/M/G)] for byte mode only. K/M/G means 1000/1024K/1024M\n\t"
            "rate (16K~2G) bps for byte mode / (1~2047000) fps for packet mode\n\n"
            "or\n"
            "ppe meter add\n\t"
            "group (0~62)\n\t"
            "vlan (0~ffff)\n\t"
            "queue (0~8)\n\t"
            "dir (up/down)\n\n");

    return -1;
}

static int do_meter_del(int argc, char *argv[], void *p)
{
    struct argVal *argVals = meterArgVals;
    ECNT_PPEMGR_METER_GROUP group = {0};
    ECNT_PPEMGR_METER_FLOW flow = {0};

    if (argsParsing(argc, argv, argVals, PPE_CMD_METER))
        goto meter_del_args_check_fail;

    if (isCheckMeterDelShowArgsFailed())
         goto meter_del_args_check_fail;

    if (argVals[METER_ARG_GROUP].hasVal) 
    {
        group.group_id  = argVals[METER_ARG_GROUP].data;
        group.enable    = 0;
        
        return ppemgr_lib_set_meter_group(&group);
    }
    else 
    {
        flow.vlan       = argVals[METER_ARG_VLAN].data;
        flow.direction  = argVals[METER_ARG_DIR].data;
        
        return ppemgr_lib_del_meter_flow(&flow);
    }

meter_del_args_check_fail:
    printf("\nUsage:\n" 
            "ppe meter del\n\t"
            "group (0~62/all)\n"
            "or\n"
            "ppe meter del\n\t"
            "vlan (0~ffff) dir (up/down)\n\n");

    return -1;
}

static int do_meter_show(int argc, char *argv[], void *p)
{
    struct ppe_meter_cfg *meterCfg = &gPpeMeterCfg;
    struct argVal *argVals = meterArgVals;

    if (argsParsing(argc, argv, argVals, PPE_CMD_METER))
        goto meter_show_args_check_fail;

    if (isCheckMeterDelShowArgsFailed())
         goto meter_show_args_check_fail;

    memset(meterCfg, 0, sizeof(struct ppe_meter_cfg));

    meterCfg->method = PPE_METHOD_SHOW;
    if (argVals[METER_ARG_GROUP].hasVal) 
    {
        meterCfg->group     = argVals[METER_ARG_GROUP].data;
        meterCfg->grpOrFlow = METER_HANDLE_BY_GROUP;
    }
    else 
    {
        meterCfg->vlan      = argVals[METER_ARG_VLAN].data;
        meterCfg->dir       = argVals[METER_ARG_DIR].data;
        meterCfg->grpOrFlow = METER_HANDLE_BY_FLOW;
    }

    if (do_ppe_ioctl(meterCfg, PPE_COMMAND_METER))
        return -1;

    return 0;

meter_show_args_check_fail:
    printf("\nUsage:\n" 
            "ppe meter show\n\t"
            "group (0~62/all)\n"
            "or\n"
            "ppe meter show\n\t"
            "vlan (0~ffff) dir (up/down)\n\n");
    
    return -1;
}

static cmds_t meterCmds[] = 
{
    {CMD_METHOD_ADD,        do_meter_add,       0x10, 0, NULL},
    {CMD_METHOD_DEL,        do_meter_del,       0x10, 0, NULL},
    {CMD_METHOD_SHOW,       do_meter_show,      0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};


/*******************************************************************************/
/*  TRTCM commands                                                                                                          */
/*******************************************************************************/
static int do_trtcm_add(int argc, char *argv[], void *p)
{
    struct argVal *argVals = trtcmArgVals;
    ECNT_PPEMGR_TRTCM_GROUP group = {0};
    ECNT_PPEMGR_TRTCM_FLOW flow = {0};

    if (argsParsing(argc, argv, argVals, PPE_CMD_TRTCM))
        goto trtcm_add_args_check_fail;

    if (isCheckTrtcmAddArgsFailed())
        goto trtcm_add_args_check_fail;

    if (argVals[TRTCM_ARG_MODE].hasVal)
    {
        group.group_id  = argVals[TRTCM_ARG_GROUP].data;
        group.enable    = 1;
        group.mode      = argVals[TRTCM_ARG_MODE].data;
        group.pir_scale = argVals[TRTCM_ARG_SCALE_PRI].data;
        group.cir_scale = argVals[TRTCM_ARG_SCALE_CRI].data;
        group.pir       = argVals[TRTCM_ARG_RATE_PRI].data;
        group.cir       = argVals[TRTCM_ARG_RATE_CRI].data;

        return ppemgr_lib_set_trtcm_group(&group);
    }

    if (argVals[TRTCM_ARG_VLAN].hasVal)
    {
        flow.group_id   = argVals[TRTCM_ARG_GROUP].data;
        flow.direction  = argVals[TRTCM_ARG_DIR].data;
        flow.vlan       = argVals[TRTCM_ARG_VLAN].data;
        
        return ppemgr_lib_add_trtcm_flow(&flow);
    }

trtcm_add_args_check_fail:
    printf("\nUsage:\n" 
            "ppe trtcm add\n\t"
            "group (0~30)\n\t"
            "mode (byte/packet)\n\t"
            "[scale_pir (K/M/G)] for byte mode only. K/M/G means 1000/1024K/1024M\n\t"
            "rate_pir (16K~2G) bps for byte mode / (1~2047000) fps for packet mode\n\t"
            "[scale_cir (K/M/G)] for byte mode only. K/M/G means 1000/1024K/1024M\n\t"
            "rate_cir (16K~2G) bps for byte mode / (1~2047000) fps for packet mode\n\n"
            "or\n"
            "ppe trtcm add\n\t"
            "group (0~30)\n\t"
            "vlan (0~ffff)\n\t"
            "dir (up/down)\n\n");
    
    return -1;
}

static int do_trtcm_del(int argc, char *argv[], void *p)
{
    struct argVal *argVals = trtcmArgVals;
    ECNT_PPEMGR_TRTCM_GROUP group = {0};
    ECNT_PPEMGR_TRTCM_FLOW flow = {0};

    if (argsParsing(argc, argv, argVals, PPE_CMD_TRTCM))
        goto trtcm_del_args_check_fail;

    if (isCheckTrtcmDelShowArgsFailed())
         goto trtcm_del_args_check_fail;

    if (argVals[TRTCM_ARG_GROUP].hasVal) 
    {
        group.group_id  = argVals[TRTCM_ARG_GROUP].data;
        group.enable    = 0;

        return ppemgr_lib_set_trtcm_group(&group);
    }
    else 
    {
        flow.group_id   = argVals[TRTCM_ARG_GROUP].data;
        flow.direction  = argVals[TRTCM_ARG_DIR].data;
        flow.vlan       = argVals[TRTCM_ARG_VLAN].data;
        
        return ppemgr_lib_del_trtcm_flow(&flow);
    }

trtcm_del_args_check_fail:
    printf("\nUsage:\n" 
            "ppe trtcm del\n\t"
            "group (0~30/all)\n"
            "or\n"
            "ppe meter del\n\t"
            "vlan (0~ffff) dir (up/down)\n\n");

    return -1;
}


static int do_trtcm_show(int argc, char *argv[], void *p)
{
    struct ppe_meter_cfg *meterCfg = &gPpeMeterCfg;
    struct argVal *argVals = trtcmArgVals;

    if (argsParsing(argc, argv, argVals, PPE_CMD_TRTCM))
        goto trtcm_show_args_check_fail;

    if (isCheckTrtcmDelShowArgsFailed())
         goto trtcm_show_args_check_fail;

    memset(meterCfg, 0, sizeof(struct ppe_meter_cfg));

    meterCfg->method = PPE_METHOD_SHOW;
    if (argVals[TRTCM_ARG_GROUP].hasVal) 
    {
        meterCfg->group     = argVals[TRTCM_ARG_GROUP].data;
        meterCfg->grpOrFlow = METER_HANDLE_BY_GROUP;
    }
    else 
    {
        meterCfg->vlan      = argVals[TRTCM_ARG_VLAN].data;
        meterCfg->dir       = argVals[TRTCM_ARG_DIR].data;
        meterCfg->grpOrFlow = METER_HANDLE_BY_FLOW;
    }

    if (do_ppe_ioctl(meterCfg, PPE_COMMAND_TRTCM))
        return -1;

    return 0;

trtcm_show_args_check_fail:
    printf("\nUsage:\n" 
            "ppe trtcm show\n\t"
            "group (0~30/all)\n"
            "or\n"
            "ppe trtcm show\n\t"
            "vlan (0~ffff) dir (up/down)\n\n");
    
    return -1;
}

static cmds_t trtcmCmds[] = 
{
    {CMD_METHOD_ADD,        do_trtcm_add,       0x10, 0, NULL},
    {CMD_METHOD_DEL,        do_trtcm_del,       0x10, 0, NULL},
    {CMD_METHOD_SHOW,       do_trtcm_show,      0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};

/*******************************************************************************/
/*  ACCOUNT commands                                                                                                      */
/*******************************************************************************/
static int do_account_add(int argc, char *argv[], void *p)
{
    struct argVal *argVals = accountArgVals;
    ECNT_PPEMGR_ACCOUNT_FLOW flow = {0};

    if (argsParsing(argc, argv, argVals, PPE_CMD_ACCOUNT))
        goto account_add_args_check_fail;

    flow.group_id   = argVals[ACCOUNT_ARG_GROUP].data;
    flow.direction  = argVals[ACCOUNT_ARG_DIR].data;
    flow.vlan       = argVals[ACCOUNT_ARG_VLAN].data;

    return ppemgr_lib_add_accounting_flow(&flow);

account_add_args_check_fail:
    printf("\nUsage:\n" 
            "ppe account add\n\t"
            "group (0~62)\n\t"
            "vlan (0~ffff)\n\t"
            "dir (up/down)\n\n");
    
    return -1;
}

static int do_account_del(int argc, char *argv[], void *p)
{
    struct argVal *argVals = accountArgVals;
    u8 group_id = 0;
    ECNT_PPEMGR_ACCOUNT_FLOW flow = {0};

    if (argsParsing(argc, argv, argVals, PPE_CMD_ACCOUNT))
        goto account_del_args_check_fail;

    if (isCheckAccountDelShowArgsFailed())
         goto account_del_args_check_fail;

    if (argVals[ACCOUNT_ARG_GROUP].hasVal) 
    {
        group_id = argVals[ACCOUNT_ARG_GROUP].data;

        return ppemgr_lib_del_accounting_all_flows(group_id);
    }
    else 
    {
        flow.direction  = argVals[ACCOUNT_ARG_DIR].data;
        flow.vlan       = argVals[ACCOUNT_ARG_VLAN].data;
        
        return ppemgr_lib_del_accounting_flow(&flow);
    }

account_del_args_check_fail:
    printf("\nUsage:\n" 
            "ppe account del\n\t"
            "group (0~62/all)\n"
            "or\n"
            "ppe account del\n\t"
            "vlan (0~ffff) dir (up/down)\n\n");

    return -1;
}

static int do_account_show(int argc, char *argv[], void *p)
{
    struct ppe_account_cfg *accountCfg = &gPpeAccountCfg;
    struct argVal *argVals = accountArgVals;

    if (argsParsing(argc, argv, argVals, PPE_CMD_ACCOUNT))
        goto account_show_args_check_fail;

    if (isCheckAccountDelShowArgsFailed())
         goto account_show_args_check_fail;

    memset(accountCfg, 0, sizeof(struct ppe_account_cfg));

    accountCfg->method = PPE_METHOD_SHOW;
    if (argVals[ACCOUNT_ARG_GROUP].hasVal) 
    {
        accountCfg->group       = argVals[ACCOUNT_ARG_GROUP].data;
        accountCfg->grpOrFlow   = METER_HANDLE_BY_GROUP;
    }
    else 
    {
        accountCfg->vlan        = argVals[ACCOUNT_ARG_VLAN].data;
        accountCfg->dir         = argVals[ACCOUNT_ARG_DIR].data;
        accountCfg->grpOrFlow   = METER_HANDLE_BY_FLOW;
    }

    if (do_ppe_ioctl(accountCfg, PPE_COMMAND_ACCOUNT))
        return -1;

    return 0;

account_show_args_check_fail:
    printf("\nUsage:\n" 
            "ppe account show\n\t"
            "group (0~62/all)\n"
            "or\n"
            "ppe account show\n\t"
            "vlan (0~ffff) dir (up/down)\n\n");
    
    return -1;
}

static cmds_t accountCmds[] = 
{
    {CMD_METHOD_ADD,        do_account_add,     0x10, 0, NULL},
    {CMD_METHOD_DEL,        do_account_del,     0x10, 0, NULL},
    {CMD_METHOD_SHOW,       do_account_show,    0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};

/*******************************************************************************/
/*  CACHE commands                                                                                                          */
/*******************************************************************************/
static int do_cache_enable(int argc, char *argv[], void *p)
{
    return ppemgr_lib_set_cache_enable(1);
}

static int do_cache_disable(int argc, char *argv[], void *p)
{
    return ppemgr_lib_set_cache_enable(0);
}

static int do_cache_set_mode(int argc, char *argv[], void *p)
{
    int mode;

    if (argc != 2)
        goto cache_set_args_check_fail;

    if (isNumber(argv[1])) 
    {
        mode = atoi(argv[1]);
        if (mode < 0 || mode >= CACHE_MODE_MAX) 
        {
            printf("\nERROR: wrong cache mode:%d\n", mode);
            goto cache_set_args_check_fail;
        }
    }
    else 
    {
        printf("\nERROR: hash cache:\"%s\" is not a number\n", argv[1]);
        goto cache_set_args_check_fail;
    }

    return ppemgr_lib_set_cache_mode(mode);

cache_set_args_check_fail:
    printf("\nUsage:\n" 
            "ppe cache set mode (0~3)\n\n");
    
    return -1;
}

static cmds_t cacheSetCmds[] = {
    {MODE_STRING,   do_cache_set_mode,  0x10, 0, NULL},
    {NULL,          NULL,               0x10, 0, NULL}
};

static int do_cache_set(int argc, char *argv[], void *p)
{
    return subcmd(cacheSetCmds, argc, argv, NULL);
}

static int do_cache_clear(int argc, char *argv[], void *p)
{
    u32 index = 0;

    if (argc != 2)
        goto cache_clear_args_check_fail;

    if(strncmp(argv[1], ALL_STRING, strlen(ALL_STRING)) == 0) 
    {
        index = 0;
    }
    else if (isNumber(argv[1])) 
    {
        index = atoi(argv[1]);
    }
    else 
    {
        goto cache_clear_args_check_fail;
    }

    return ppemgr_lib_clear_cache(index);
    
cache_clear_args_check_fail:
    printf("\nUsage:\n" 
            "ppe cache clear (foeIdx/all)\n\n");
    
    return -1;
}

static int do_cache_show(int argc, char *argv[], void *p)
{
    struct ppe_cache_cfg *cacheCfg = &gPpeCacheCfg;
    int i = 0;

    if (argc != 2)
        goto cache_show_args_check_fail;

    memset(cacheCfg, 0, sizeof(struct ppe_cache_cfg));

    cacheCfg->method = CACHE_METHOD_SHOW;

    while (cacheStrVals[i].cmd) 
    {
        if(strncmp(argv[1], cacheStrVals[i].cmd, strlen(cacheStrVals[i].cmd)) == 0) 
        {
            cacheCfg->showType = cacheStrVals[i].val;
            break;
        }
        i++;
    }

    if (i == PPE_CACHE_TYPE_NUM) 
    {
        printf("\nERROR: wrong cache type: \"%s\"\n", argv[1]);
        goto cache_show_args_check_fail;
    }
    
    if (do_ppe_ioctl(cacheCfg, PPE_COMMAND_CACHE))
        return -1;

    return 0;

cache_show_args_check_fail:
    printf("\nUsage:\n" 
            "ppe cache show (valid/invalid/locked/all/lineCnt)\n\n");
    
    return -1;
}

static cmds_t cacheCmds[] = 
{
    {CMD_METHOD_ENABLE,     do_cache_enable,    0x10, 0, NULL},
    {CMD_METHOD_DISABLE,    do_cache_disable,   0x10, 0, NULL},
    {CMD_METHOD_SET,        do_cache_set,       0x10, 0, NULL},
    {CMD_METHOD_CLEAR,      do_cache_clear,     0x10, 0, NULL},
    {CMD_METHOD_SHOW,       do_cache_show,      0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};

/*******************************************************************************/
/*  HASH commands                                                                                                            */
/*******************************************************************************/
static int do_hash_set_mode(int argc, char *argv[], void *p)
{
    int mode;

    if (argc != 2)
        goto hash_set_mode_args_check_fail;

    if (isNumber(argv[1])) 
    {
        mode = atoi(argv[1]);
    }
    else 
    {
        printf("\nERROR: hash mode:\"%s\" is not a number\n", argv[1]);
        goto hash_set_mode_args_check_fail;
    }

    return ppemgr_lib_set_hash_mode(mode);

hash_set_mode_args_check_fail:
    printf("\nUsage:\n" 
            "ppe hash set mode (0~3)\n\n");
    
    return -1;
}

static int do_hash_set_bitShift(int argc, char *argv[], void *p)
{
    int shift;

    if (argc != 2)
        goto hash_set_bitShift_args_check_fail;

    if (isNumber(argv[1])) 
    {
        shift = atoi(argv[1]);
    }
    else 
    {
        printf("\nERROR: hash bitShift:\"%s\" is not a number\n", argv[1]);
        goto hash_set_bitShift_args_check_fail;
    }

    return ppemgr_lib_set_hash_bitshift(shift);

hash_set_bitShift_args_check_fail:
    printf("\nUsage:\n" 
            "ppe hash set bitShift (0~15)\n\n");
    
    return -1;
}

static int do_hash_set_tls(int argc, char *argv[], void *p)
{
    u8 enable;
    ECNT_PPEMGR_HASH_TLS_TYPE type;

    if (argc != 3)
        goto hash_set_tls_args_check_fail;

    if (strncmp(argv[1], ENABLE_STRING, strlen(ENABLE_STRING)) == 0) 
    {
        enable = 1;
        if (strncmp(argv[2], DMAC_STRING, strlen(DMAC_STRING)) == 0)
        {
            type = ECNT_PPEMGR_HASH_TLS_DMAC;
        }
        else if (strncmp(argv[2], PBITS_STRING, strlen(PBITS_STRING)) == 0)
        {
            type = ECNT_PPEMGR_HASH_TLS_PBIT;
        }
        else if (strncmp(argv[2], ETYPE_DSCP_STRING, strlen(ETYPE_DSCP_STRING)) == 0)
        {
            type = ECNT_PPEMGR_HASH_TLS_ETYPE_DSCP;
        }
        else 
        {
            printf("\nERROR: wrong hash tls type:%s\n", argv[2]);
            goto hash_set_tls_args_check_fail;
        }
    }
    else if (strncmp(argv[1], DISABLE_STRING, strlen(DISABLE_STRING)) == 0) 
    {
        enable = 0;
        if (strncmp(argv[2], DMAC_STRING, strlen(DMAC_STRING)) == 0)
        {
            type = ECNT_PPEMGR_HASH_TLS_DMAC;
        }
        else if (strncmp(argv[2], PBITS_STRING, strlen(PBITS_STRING)) == 0)
        {
            type = ECNT_PPEMGR_HASH_TLS_PBIT;
        }
        else if (strncmp(argv[2], ETYPE_DSCP_STRING, strlen(ETYPE_DSCP_STRING)) == 0)
        {
            type = ECNT_PPEMGR_HASH_TLS_ETYPE_DSCP;
        }
        else 
        {
            printf("\nERROR: wrong hash tls type:%s\n", argv[2]);
            goto hash_set_tls_args_check_fail;
        }
    }
    else 
    {
        goto hash_set_tls_args_check_fail;
    }

    return ppemgr_lib_set_hash_tls(type, enable);

hash_set_tls_args_check_fail:
    printf("\nUsage:\n" 
            "ppe hash set tls enable/disable (dmac/pbits/etype_dscp)\n\n");
    
    return -1;
}

static int do_hash_set_l2b(int argc, char *argv[], void *p)
{
    u8 enable;
    
    if (argc != 3)
        goto hash_set_l2b_args_check_fail;

    if (strncmp(argv[1], SMAC_STRING, strlen(SMAC_STRING)) == 0) 
    {
        if (strncmp(argv[2], ENABLE_STRING, strlen(ENABLE_STRING)) == 0)
        {
            enable = 1;
        }
        else if (strncmp(argv[2], DISABLE_STRING, strlen(DISABLE_STRING)) == 0)
        {
            enable = 0;
        }
        else 
        {
            printf("\nERROR: wrong hash l2b smac \"%s\"\n", argv[2]);
            goto hash_set_l2b_args_check_fail;
        }
    }
    else 
    {
        printf("\nERROR: wrong hash l2b \"%s\"\n", argv[1]);
        goto hash_set_l2b_args_check_fail;
    }

    return ppemgr_lib_set_hash_l2b_smac(enable);

hash_set_l2b_args_check_fail:
    printf("\nUsage:\n" 
            "ppe hash set l2b smac (enable/disable)\n\n");
    
    return -1;
}

static cmds_t hashSetCmds[] = {
    {MODE_STRING,       do_hash_set_mode,       0x10, 0, NULL},
    {BITSHIFT_STRING,   do_hash_set_bitShift,   0x10, 0, NULL},
    {TLS_STRING,        do_hash_set_tls,        0x10, 0, NULL},
    {L2B_STRING,        do_hash_set_l2b,        0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};

static int do_hash_set(int argc, char *argv[], void *p)
{
    return subcmd(hashSetCmds, argc, argv, NULL);
}

static cmds_t hashCmds[] = 
{
    {CMD_METHOD_SET,        do_hash_set,        0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};

/*******************************************************************************/
/*  TRQOS commands                                                                                                          */
/*******************************************************************************/
static int do_trqos_set_flow_ratelimit(int argc, char *argv[], void *p)
{
    unsigned char direction, flowid;
    unsigned int ratelimit;

    if (argc != 4)
        goto trqos_set_flow_rate_args_check_fail;

    if(strncmp(argv[1], UP_STRING, strlen(UP_STRING)) == 0)
        direction = METER_DIR_UP;
    else
        direction = METER_DIR_DOWN;

    if (isNumber(argv[2])) 
    {
        flowid = atoi(argv[2]);
    }
    else
    {
        printf("\nERROR: wrong flow id \"%s\"\n", argv[2]);
        goto trqos_set_flow_rate_args_check_fail;
    }
    
    if (isNumber(argv[3])) 
    {
        ratelimit = atoi(argv[3]);
    }
    else
    {
        printf("\nERROR: wrong ratelimit \"%s\"\n", argv[3]);
        goto trqos_set_flow_rate_args_check_fail;
    }
    
    if (ppemgr_lib_set_traffic_qos_flow_ratelimit(flowid, direction, ratelimit))
        return -1;

    return 0;


trqos_set_flow_rate_args_check_fail:
    printf("\nUsage:\n" 
            "ppe trqos set <'up':'down'> <flowid> <ratelimit>\n\n");
    
    return -1;
}

static int do_trqos_get_flow_cnt(int argc, char *argv[], void *p)
{
    unsigned char direction, flowid;
    unsigned long long cntbyte;
    unsigned int cntbyte_l, cntbyte_h;
    int ret;
    
    if (argc != 3)
        goto trqos_get_flow_cnt_args_check_fail;

    if(strncmp(argv[1], UP_STRING, strlen(UP_STRING)) == 0)
        direction = METER_DIR_UP;
    else
        direction = METER_DIR_DOWN;

    if (isNumber(argv[2])) 
    {
        flowid = atoi(argv[2]);
    }
    else
    {
        printf("\nERROR: wrong flow id \"%s\"\n", argv[2]);
        goto trqos_get_flow_cnt_args_check_fail;
    }

    ret = ppemgr_lib_get_traffic_qos_flow_cnt(flowid, direction, &cntbyte);
    
    cntbyte_h = (unsigned int)(cntbyte>>32);
    cntbyte_l = (unsigned int)cntbyte;
    printf("cntbyte_h: 0x%x, cntbyte_l: 0x%x\n", cntbyte_h, cntbyte_l);

    return ret;


trqos_get_flow_cnt_args_check_fail:
    printf("\nUsage:\n" 
            "ppe trqos get cnt <'up':'down'> <flowid>\n\n");
    
    return -1;
}

static int do_trqos_get_flow_ratelimit(int argc, char *argv[], void *p)
{
    unsigned char direction, flowid;
    unsigned int ratelimit;
    int ret;
    
    if (argc != 3)
        goto trqos_get_flow_rate_args_check_fail;

    if(strncmp(argv[1], UP_STRING, strlen(UP_STRING)) == 0)
        direction = METER_DIR_UP;
    else
        direction = METER_DIR_DOWN;

    if (isNumber(argv[2])) 
    {
        flowid = atoi(argv[2]);
    }
    else
    {
        printf("\nERROR: wrong flow id \"%s\"\n", argv[2]);
        goto trqos_get_flow_rate_args_check_fail;
    }

    ret = ppemgr_lib_get_traffic_qos_flow_ratelimit(flowid, direction, &ratelimit);
    printf("ratelimit: %d\n", ratelimit);

    return ret;


trqos_get_flow_rate_args_check_fail:
    printf("\nUsage:\n" 
            "ppe trqos get rate <'up':'down'> <flowid>\n\n");
    
    return -1;
}


static cmds_t trQosGetCmds[] = {
    {TRAFFIC_QOS_CNT,       do_trqos_get_flow_cnt,   0x10, 0, NULL},
    {TRAFFIC_QOS_RATE,      do_trqos_get_flow_ratelimit,     0x10, 0, NULL},
    {NULL,                      NULL,                0x10, 0, NULL}
};

static int do_trqos_get(int argc, char *argv[], void *p)
{
    return subcmd(trQosGetCmds, argc, argv, NULL);
}


static cmds_t trQosCmds[] = 
{
    {CMD_METHOD_SET,        do_trqos_set_flow_ratelimit,    0x10, 0, NULL},
    {CMD_METHOD_GET,        do_trqos_get,                   0x10, 0, NULL},
    {NULL,                  NULL,                           0x10, 0, NULL}
};

/*******************************************************************************/
/*  LOCALIN commands                                                                                                        */
/*******************************************************************************/
static int do_local_in_enable(int argc, char *argv[], void *p)
{
    return ppemgr_lib_set_localin_enable(1);
}

static int do_local_in_disable(int argc, char *argv[], void *p)
{
    return ppemgr_lib_set_localin_enable(0);
}

static int local_in_parse_para(char* argv[],unsigned short* port,unsigned int* port_num)
{
    char* ptr = NULL;
    char* start = argv[1];
    unsigned char tmp[8] = {0};
    unsigned int len = 0;
    unsigned int port_num_tmp = 0;
    
    do
    {
        ptr = strchr(start,',');
        if(ptr)
        {
            len = ptr-start;
            if(len > 8)
                len = 8;
            
            memset(tmp,0,8);
            memmove(tmp,start,len);
            port_num_tmp++;

            start = ptr+1;
        }
        else
        {
            len = strlen(start);
            if(len > 8)
                len = 8;

            memset(tmp,0,8);
            memmove(tmp,start,len);
            port_num_tmp++;
        }

        if((0 == atoi(tmp)) ||(atoi(tmp) > 0xffff))
        {
            printf("port value should less than 65535\n");
            return -1;
        }
        else if(port_num_tmp > 8)
        {
            printf("max port number is 8\n");
            return -1;
        }
        else
            *(port+port_num_tmp-1)= atoi(tmp);             
    }while(ptr);
    
    *port_num = port_num_tmp;
    
    return 0;
}

static int do_local_in_add_del_dport_common(unsigned short* port_ptr,unsigned int port_num,unsigned int flag)
{
    int i;

    for (i = 0; i < port_num; i++)
    {
        if (0 == flag)
        {
            ppemgr_lib_add_localin_dst_port(port_ptr[i]);
        }
        else
        {
            ppemgr_lib_del_localin_dst_port(port_ptr[i]);
        }
    }
    return 0;
}

static int do_local_in_add_dport(int argc, char *argv[], void *p)
{
    unsigned short port[8] = {0};
    unsigned int port_num = 0;

    if(2 != argc)
        goto help;

    if(-1 == local_in_parse_para(argv, port, &port_num))
        goto help;

    do_local_in_add_del_dport_common(port,  port_num,0);

    return 0;

help:
    printf("\nUsage:\n" 
            "ppe localin add dport <port1,port2,...port8>\n");
    
    return -1;
}

static int do_local_in_add_dport_all(int argc, char *argv[], void *p)
{
    if(1 != argc)
        goto help;
   
    return ppemgr_lib_add_localin_dst_port(0);

help:
    printf("\nUsage:\n" 
            "ppe localin add all\n");
    
    return -1;
}


static int do_local_in_del_dport(int argc, char *argv[], void *p)
{
    unsigned short port[8] = {0};
    unsigned int port_num = 0;

    if(2 != argc)
        goto help;

    if(-1 == local_in_parse_para(argv,port,&port_num))
        goto help;

    do_local_in_add_del_dport_common(port,port_num,1);

    return 0;

help:
    printf("\nUsage:\n" 
        "ppe localin del dport <port1,port2,...port8>\n");

    return -1;

}

static int do_local_in_del_dport_all(int argc, char *argv[], void *p)
{
    if(1 != argc)
        goto help;
   
    return ppemgr_lib_del_localin_dst_port(0);

help:
    printf("\nUsage:\n" 
            "ppe localin del all\n");
    
    return -1;
}

static int do_local_in_show_dport(int argc, char *argv[], void *p)
{
    struct ppe_local_in_cfg localInCfg; 

    memset(&localInCfg, 0, sizeof(localInCfg));

    if(1 != argc)
        goto help;

    localInCfg.method = LOCAL_IN_METHOD_SHOW;

    if (do_ppe_ioctl(&localInCfg, PPE_COMMAND_LOCAL_IN))
        return -1;
   
    return 0;

help:
    printf("\nUsage:\n" 
            "ppe localin show\n");
        
    return -1;
}

#ifdef CONFIG_TP_IMAGE		
static int do_local_in_backup_dport(int argc, char *argv[], void *p)
{
    struct ppe_local_in_cfg localInCfg; 

    memset(&localInCfg, 0, sizeof(localInCfg));

    if(1 != argc)
        goto help;

    localInCfg.method = LOCAL_IN_METHOD_BACKUP;

    if (do_ppe_ioctl(&localInCfg, PPE_COMMAND_LOCAL_IN))
        return -1;
   
    return 0;

help:
    printf("\nUsage:\n" 
            "ppe localin backup bk\n");
        
    return -1;
}

static int do_local_in_restore_dport(int argc, char *argv[], void *p)
{
    struct ppe_local_in_cfg localInCfg; 

    memset(&localInCfg, 0, sizeof(localInCfg));

    if(1 != argc)
        goto help;

    localInCfg.method = LOCAL_IN_METHOD_RESTORE;

    if (do_ppe_ioctl(&localInCfg, PPE_COMMAND_LOCAL_IN))
        return -1;
   
    return 0;

help:
    printf("\nUsage:\n" 
            "ppe localin backup rst\n");
        
    return -1;
}
	

static int do_local_in_show_backup_dport(int argc, char *argv[], void *p)
{
    struct ppe_local_in_cfg localInCfg; 

    memset(&localInCfg, 0, sizeof(localInCfg));

    if(1 != argc)
        goto help;

    localInCfg.method = LOCAL_IN_METHOD_BACKUP_SHOW;

    if (do_ppe_ioctl(&localInCfg, PPE_COMMAND_LOCAL_IN))
        return -1;
   
    return 0;

help:
    printf("\nUsage:\n" 
            "ppe localin backup show\n");
        
    return -1;
}
#endif /* CONFIG_TP_IMAGE */

static cmds_t localInAddCmds[] = 
{
    {DPORT_STRING,       do_local_in_add_dport,       0x10, 0, NULL},
    {ALL_STRING,       do_local_in_add_dport_all,       0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};

static cmds_t localInDelCmds[] = 
{
    {DPORT_STRING,       do_local_in_del_dport,       0x10, 0, NULL},
    {ALL_STRING,       do_local_in_del_dport_all,       0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};

#ifdef CONFIG_TP_IMAGE
static cmds_t localInBackupCmds[] = 
{
    {BACKUP_STRING,       do_local_in_backup_dport,       0x10, 0, NULL},
    {RESTORE_STRING,      do_local_in_restore_dport,      0x10, 0, NULL},
    {SHOW_BACKUP_STRING,  do_local_in_show_backup_dport,  0x10, 0, NULL},
    {NULL,                  NULL,               0x10, 0, NULL}
};
#endif /* CONFIG_TP_IMAGE */

static int do_local_in_add(int argc, char *argv[], void *p)
{
    return subcmd(localInAddCmds, argc, argv, NULL);
}

static int do_local_in_del(int argc, char *argv[], void *p)
{
    return subcmd(localInDelCmds, argc, argv, NULL);
}

#ifdef CONFIG_TP_IMAGE
static int do_local_in_backup(int argc, char *argv[], void *p)
{
	return subcmd(localInBackupCmds, argc, argv, NULL);
}
#endif /* CONFIG_TP_IMAGE */

static cmds_t localInCmds[] = 
{
    {CMD_METHOD_ENABLE,     do_local_in_enable,         0x10, 0, NULL},
    {CMD_METHOD_DISABLE,    do_local_in_disable,        0x10, 0, NULL},
    {CMD_METHOD_ADD,        do_local_in_add,            0x10, 0, NULL},
    {CMD_METHOD_DEL,        do_local_in_del,            0x10, 0, NULL},
    {CMD_METHOD_SHOW,       do_local_in_show_dport,     0x10, 0, NULL},
#ifdef CONFIG_TP_IMAGE
    {CMD_METHOD_BACKUP,     do_local_in_backup,   		0x10, 0, NULL},
#endif /* CONFIG_TP_IMAGE */
    {NULL,                  NULL,                       0x10, 0, NULL}
};

static int do_black_ip_port_proto_add(int argc, char *argv[], void *p)
{
	unsigned int ip, port, protocol;
	unsigned int ipv4_add[4] = {0};
	int ret=0;

	if(argc != 4)
	{
		printf("ppe black_ip_port_proto add <ip(X.X.X.X,255.255.255.255 means all ip)> <port(0~65536,65536 means all port)> <proto(1:TCP, 2:UDP, 3:ICMP, 4:ALL)>\n");
		return -1;
	}

	sscanf(argv[1], "%d.%d.%d.%d", &ipv4_add[0],  &ipv4_add[1],  &ipv4_add[2],  &ipv4_add[3]);
	ip = ipv4_add[0]<<24 | ipv4_add[1]<<16 | ipv4_add[2]<<8 | ipv4_add[3];

	port = atoi(argv[2]);
	protocol = atoi(argv[3]);

	ret = ppemgr_lib_black_ip_port_protocol_add(ip,port,protocol);
	if(ret)
		return -1;

	return 0;
}

static int do_black_ip_port_proto_delete(int argc, char *argv[], void *p)
{
	unsigned int ip, port, protocol;
	unsigned int ipv4_add[4] = {0};
	int ret=0;

	if(argc != 4)
	{
		printf("ppe black_ip_port_proto del <ip(X.X.X.X)> <port(0~65536,65536 means all port)> <proto(1:TCP, 2:UDP, 3:ICMP, 4:ALL)>\n");
		return -1;
	}

	sscanf(argv[1], "%d.%d.%d.%d", &ipv4_add[0],  &ipv4_add[1],  &ipv4_add[2],  &ipv4_add[3]);
	ip = ipv4_add[0]<<24 | ipv4_add[1]<<16 | ipv4_add[2]<<8 | ipv4_add[3];

	port = atoi(argv[2]);
	protocol = atoi(argv[3]);

	ret = ppemgr_lib_black_ip_port_protocol_delete(ip,port,protocol);
	
	if(ret)
		return -1;

	return 0;
}

static int do_black_ip_port_proto_show(int argc, char *argv[], void *p)
{
	int ret=0;

	if(argc != 1)
	{
		printf("ppe black_ip_port_proto show\n");
		return -1;
	}

	ret = ppemgr_lib_black_ip_port_protocol_show();
	
	if(ret)
		return -1;

	return 0;
}

static cmds_t blackIpPortProtoCmds[] = {
	{CMD_METHOD_ADD,		do_black_ip_port_proto_add,		0x10, 0, NULL},
	{CMD_METHOD_DEL,		do_black_ip_port_proto_delete,		0x10, 0, NULL},
	{CMD_METHOD_SHOW,		do_black_ip_port_proto_show,		0x10, 0, NULL},
	{NULL,						NULL,			0x10, 0, NULL}
};

static int do_wan_set_ratelimit(int argc, char *argv[], void *p)
{
    unsigned char direction, wanid;
    unsigned int ratelimit;

    if (argc != 4)
        goto wan_set_ratelimit_fail;

    if(strncmp(argv[1], UP_STRING, strlen(UP_STRING)) == 0)
        direction = METER_DIR_UP;
    else
        direction = METER_DIR_DOWN;

    if (isNumber(argv[2])) 
    {
        wanid = atoi(argv[2]);
    }
    else
    {
        printf("\nERROR: wrong wanid id \"%s\"\n", argv[2]);
        goto wan_set_ratelimit_fail;
    }
    
    if (isNumber(argv[3])) 
    {
        ratelimit = atoi(argv[3]);
    }
    else
    {
        printf("\nERROR: wrong ratelimit \"%s\"\n", argv[3]);
        goto wan_set_ratelimit_fail;
    }
    
    if (ppemgr_lib_set_wan_ratelimit(wanid, direction, ratelimit))
        return -1;

    return 0;


wan_set_ratelimit_fail:
    printf("\nUsage:\n" 
            "ppe wan_rate set <'up':'down'> <wanid> <ratelimit>\n\n");
    
    return -1;
}

static cmds_t wanRateCmds[] = 
{
    {CMD_METHOD_SET,        do_wan_set_ratelimit,            0x10, 0, NULL},
    {NULL,                  NULL,                           0x10, 0, NULL}
};

/*******************************************************************************/
/*  PPE commands                                                                                                               */
/*******************************************************************************/
static int do_tls(int argc, char *argv[], void *p)
{
    return subcmd(tlsCmds, argc, argv, NULL);
}

static int do_meter(int argc, char *argv[], void *p)
{
    return subcmd(meterCmds, argc, argv, NULL);
}

static int do_trtcm(int argc, char *argv[], void *p)
{
    return subcmd(trtcmCmds, argc, argv, NULL);
}

static int do_account(int argc, char *argv[], void *p)
{
    return subcmd(accountCmds, argc, argv, NULL);
}

static int do_cache(int argc, char *argv[], void *p)
{
    return subcmd(cacheCmds, argc, argv, NULL);
}

static int do_hash(int argc, char *argv[], void *p)
{
    return subcmd(hashCmds, argc, argv, NULL);
}

static int do_trqos(int argc, char *argv[], void *p)
{
    return subcmd(trQosCmds, argc, argv, NULL);
}

static int do_local_in(int argc, char *argv[], void *p)
{
    return subcmd(localInCmds, argc, argv, NULL);
}

static int do_black_ip_port_proto(int argc, char *argv[], void *p)
{
	return subcmd(blackIpPortProtoCmds, argc, argv, NULL);
}

static int do_wan_rate(int argc, char *argv[], void *p)
{
	return subcmd(wanRateCmds, argc, argv, NULL);
}

static int do_packet_mirror_disable(int argc, char *argv[], void *p)
{
	int ret;

	if (argc < 1)
	{
		printf("Usage: ppe mirror disable\n");
		return -1;
	}
		
	ret = ppemgr_lib_packet_mirror_disable();

	if (ret > 0)
	{
		printf("Exec. Failed\n");
		return -1;
	}
	
	return 0;
}

static int do_packet_mirror_enable(int argc, char *argv[], void *p)
{
	int ret,port;

	if (argc < 2)
	{
		printf("Usage: ppe mirror enable [port]\n");
        printf("port:1-4 lan1-lan4\n");
		return -1;
	}
	
	port = atoi(argv[1]);

	printf("ppe mirror to lan%d\n", port);
	
	ret = ppemgr_lib_packet_mirror_enable(port);

	if (ret > 0)
	{
		printf("Exec. Failed\n");
		return -1;
	}
	
	return 0;
}

static int do_packet_mirror_mode(int argc, char *argv[], void *p)
{
    int ret,mode;

	if (argc < 2)
	{
		printf("Usage: ppe mirror mode [mode]\n");
        printf("mode:\n");
        printf("1 -- mirror upstream after NAT and downstream before NAT\n");
        printf("2 -- mirror upstream after NAT only\n");
        printf("3 -- mirror downstream before NAT only\n");

		return -1;
	}
	
	mode = atoi(argv[1]);

	printf("ppe mirror mode %d\n", mode);
	
	ret = ppemgr_lib_packet_mirror_mode(mode);

	if (ret > 0)
	{
		printf("Exec. Failed\n");
		return -1;
	}
	
	return 0;
}

static int do_packet_mirror_enable_config(int argc, char *argv[], void *p)
{
    int ret,enable,fport,nbq,sptag;

	if (argc < 5)
	{
		printf("Usage: ppe mirror enable_config [enable] [fport] [nbq] [sptag(hex)]\n");
        printf("enable: 0 -- disable 1 -- enable\n");
        printf("fport: pse fport\n");
        printf("nbq: pse nbq\n");
        printf("sptag: special tag for switch\n");

		return -1;
	}
	
	enable = atoi(argv[1]);
    fport = atoi(argv[2]);
    nbq = atoi(argv[3]);
    sscanf(argv[4],"0x%x",&sptag);

	printf("ppe mirror enable_config %d %d %d 0x%x\n", enable,fport,nbq,sptag);
	
	ret = ppemgr_lib_packet_mirror_enable_config(enable,fport,nbq,sptag);

	if (ret > 0)
	{
		printf("Exec. Failed\n");
		return -1;
	}
	
	return 0; 
}

static int do_packet_mirror_mode_config(int argc, char *argv[], void *p)
{
    int ret,mode,mirror_dst,mirror_src,mirror_mask;

	if (argc < 5)
	{
		printf("Usage: ppe mirror mode_config [mode] [mirror_dst(hex)] [mirror_src(hex)] [mirror_mask(hex)]\n");
        printf("mode: 0 -- and 1 -- or\n");
        printf("mirror_dst: 9bit 4bit fport+5bit nbq\n");
        printf("mirror_src: pse src port\n");
        printf("mirror_mask:5bit bitmap dst_pre_en|dst_post_en|dst_q_en|src_pre_en|src_port_en\n");

		return -1;
	}
	
	mode = atoi(argv[1]);
    sscanf(argv[2],"0x%x",&mirror_dst);
    sscanf(argv[3],"0x%x",&mirror_src);
    sscanf(argv[4],"0x%x",&mirror_mask);

	printf("ppe mirror mode_config %d 0x%x 0x%x 0x%x\n", mode,mirror_dst,mirror_src,mirror_mask);
	
	ret = ppemgr_lib_packet_mirror_mode_config(mode,mirror_dst,mirror_src,mirror_mask);

	if (ret > 0)
	{
		printf("Exec. Failed\n");
		return -1;
	}
	
	return 0;
}

static cmds_t pktMirrorCmds[] = {
    {CMD_METHOD_DISABLE,    do_packet_mirror_disable,	0x10, 0, NULL},
    {CMD_METHOD_ENABLE,     do_packet_mirror_enable,	0x10, 0, NULL},
    {MODE_STRING,     do_packet_mirror_mode,	0x10, 0, NULL},
    {ENABLE_CONFIG_STRING,     do_packet_mirror_enable_config,	0x10, 0, NULL},  
    {MODE_CONFIG_STRING,     do_packet_mirror_mode_config,	0x10, 0, NULL},  
    {NULL,                  NULL,			0x10, 0, NULL}
};

static int do_packet_mirror(int argc, char *argv[], void *p)
{
	return subcmd(pktMirrorCmds, argc, argv, NULL);
}

static int do_set_wan_mtu(int argc, char *argv[], void *p)
{
	int ret,mtu;
	int wan_idx = -1;
	
	if (argc != 3)
	{
		printf("Usage: ppe wanmtu set <wan_idx> <mtu>\n");
		return -1;
	}
	
	/* get wan_index */
	wan_idx = atoi(argv[1]);

	/* get wan mtu */
	mtu = atoi(argv[2]);
	ret = ppemgr_lib_wan_mtu(wan_idx, mtu);
	
	if (ret > 0)
	{
		printf("Exec. Failed\n");
		return -1;
	}
	
	return 0;
}

static int do_clear_wan_mtu(int argc, char *argv[], void *p)
{
	int ret,mtu;
	int wan_idx = -1;
	
	if (argc != 2)
	{
		printf("Usage: ppe wanmtu clear <wan_idx>\n");
		return -1;
	}
	
	/* get wan_index */
	wan_idx = atoi(argv[1]);
	
	/* max value means MTU disable */
	mtu = 0x3fff;
	ret = ppemgr_lib_wan_mtu(wan_idx, mtu);
	
	if (ret > 0)
	{
		printf("Exec. Failed\n");
		return -1;
	}
	
	return 0;
}

static cmds_t wanMtuCmds[] = {
    {CMD_METHOD_SET,        do_set_wan_mtu,	0x10, 0, NULL},
    {CMD_METHOD_CLEAR,      do_clear_wan_mtu,	0x10, 0, NULL},
    {NULL,                  NULL,		0x10, 0, NULL}
};

static int do_wan_mtu(int argc, char *argv[], void *p)
{
	return subcmd(wanMtuCmds, argc, argv, NULL);
}

static int do_set_ppe_mtu(int argc, char *argv[], void *p)
{
	int mtu = 0;

	if(2 != argc)
		goto help;

	mtu = atoi(argv[1]);

	if (ppemgr_lib_set_mtu(mtu))
		return -1;
   
	return 0;

help:
	printf("\nUsage:\n" 
			"ppe mtu <mtu>\n");
		
	return -1;
}

static int do_nptv6_prefix_add(int argc, char *argv[], void *p)
{
    	if(4 != argc)
        		goto help;

	ppemgr_lib_add_nptv6_prefix(argv[1],argv[2],argv[3]);

    return 0;

help:
printf("\nUsage:\n" 
        "ppemgr nptv6_prefix add [ifname] [snpt_str] [dnpt_str]\n");
printf("\nExample:\n"
	"ppemgr nptv6_prefix add nas0_0 2001::/64 2002::/64\n");
    
    return -1;
}

static int do_nptv6_prefix_del(int argc, char *argv[], void *p)
{
	if(2 != argc)
		goto help;

	if(!strcmp(argv[1],"all"))
		ppemgr_lib_del_nptv6_prefix_all();
	else
		ppemgr_lib_del_nptv6_prefix(argv[1]);

	return 0;

	help:
	printf("\nUsage:\n" 
		"ppemgr nptv6_prefix del [ifname]/all\n");
	printf("\nExample:\n"
		"ppemgr nptv6_prefix add nas0_0/all\n");

	return -1;
}

static cmds_t nptv6PrefixCmds[] = {
    {CMD_METHOD_ADD,        do_nptv6_prefix_add,    0x10, 0, NULL},		
    {CMD_METHOD_DEL,        do_nptv6_prefix_del,    0x10, 0, NULL},
    {NULL,                  NULL,           0x10, 0, NULL}
};

static int do_nptv6_prefix(int argc, char *argv[], void *p)
{
    return subcmd(nptv6PrefixCmds, argc, argv, NULL);
}

static int do_nptv6_mode_set(int argc, char *argv[], void *p)
{
	unsigned int mode = 0;
	
	if(2 != argc)
		goto help;

	mode = atoi(argv[1]);
	
	ppemgr_lib_set_nptv6_mode((unsigned char)mode);

	return 0;

	help:
	printf("\nUsage:\n" 
	"ppemgr nptv6_mode set [mode]\n");
	printf("mode 0 -- reverse mode 1--prefix mode\n");

	return -1;
}

static int do_nptv6_mode_get(int argc, char *argv[], void *p)
{
	unsigned char mode = 0;;

	ppemgr_lib_get_nptv6_mode(&mode);

	printf("mode is %d,%s\n",mode,mode?"prefix mode":"reverse mode");

	return 0;

}

static cmds_t nptv6PrefixModeCmds[] = {
    {CMD_METHOD_SET,        do_nptv6_mode_set,    0x10, 0, NULL},		
    {CMD_METHOD_GET,        do_nptv6_mode_get,    0x10, 0, NULL},
    {NULL,                  NULL,           0x10, 0, NULL}
};

static int do_nptv6_mode(int argc, char *argv[], void *p)
{	
    return subcmd(nptv6PrefixModeCmds, argc, argv, NULL);
}


static int do_clean_uni_entry(int argc, char *argv[], void *p)
{
	if (ppemgr_lib_clean_unicast_entry())
		return -1;
   
	return 0;
}

static int do_ppe2_enable(int argc, char *argv[], void *p)
{
	ppemgr_lib_ppe2_enable();

	return 0;
}

static int do_ppe2_disable(int argc, char *argv[], void *p)
{
	ppemgr_lib_ppe2_disable();

	return 0;
}

static cmds_t ppe2Cmds[] = {
    {CMD_METHOD_ENABLE,        do_ppe2_enable,    0x10, 0, NULL},		
    {CMD_METHOD_DISABLE,        do_ppe2_disable,    0x10, 0, NULL},
    {NULL,                  NULL,           0x10, 0, NULL}
};

static int do_ppe2_cmd(int argc, char *argv[], void *p)
{
    return subcmd(ppe2Cmds, argc, argv, NULL);
}

static int do_vxlan_hash_enable(int argc, char *argv[], void *p)
{
	ppemgr_lib_vxlan_hash_enable();

	return 0;
}

static int do_vxlan_hash_disable(int argc, char *argv[], void *p)
{
	ppemgr_lib_vxlan_hash_disable();

	return 0;
}

static int do_vxlan_hash_show(int argc, char *argv[], void *p)
{
	ppemgr_lib_vxlan_hash_show();

	return 0;
}

static cmds_t ppeVxlanHashCmds[] = {
    {CMD_METHOD_ENABLE,        do_vxlan_hash_enable,    0x10, 0, NULL},		
    {CMD_METHOD_DISABLE,        do_vxlan_hash_disable,    0x10, 0, NULL},
    {CMD_METHOD_SHOW,       do_vxlan_hash_show,      0x10, 0, NULL},
    {NULL,                  NULL,           0x10, 0, NULL}
};

static int do_ppe_vxlanhash(int argc, char *argv[], void *p)
{
    return subcmd(ppeVxlanHashCmds, argc, argv, NULL);
}

static int do_l2tp_hash_enable(int argc, char *argv[], void *p)
{
	ppemgr_lib_l2tp_hash_enable();

	return 0;
}

static int do_l2tp_hash_disable(int argc, char *argv[], void *p)
{
	ppemgr_lib_l2tp_hash_disable();

	return 0;
}

static int do_l2tp_hash_show(int argc, char *argv[], void *p)
{
	ppemgr_lib_l2tp_hash_show();

	return 0;
}

static cmds_t ppeL2tpHashCmds[] = {
    {CMD_METHOD_ENABLE,        do_l2tp_hash_enable,    0x10, 0, NULL},		
    {CMD_METHOD_DISABLE,        do_l2tp_hash_disable,    0x10, 0, NULL},
    {CMD_METHOD_SHOW,       do_l2tp_hash_show,      0x10, 0, NULL},
    {NULL,                  NULL,           0x10, 0, NULL}
};

static int do_ppe_l2tphash(int argc, char *argv[], void *p)
{
    return subcmd(ppeL2tpHashCmds, argc, argv, NULL);
}

static cmds_t ppeCmds[] = 
{
    {"tls",                 do_tls,                 0x10, 0, NULL},
    {"meter",               do_meter,               0x10, 0, NULL},
    {"trtcm",               do_trtcm,               0x10, 0, NULL},
    {"account",             do_account,             0x10, 0, NULL},
    {"cache",               do_cache,               0x10, 0, NULL},
    {"hash",                do_hash,                0x10, 0, NULL},
    {"trqos",               do_trqos,               0x10, 0, NULL},
    {"localin",             do_local_in,            0x10, 0, NULL},
    {"black_ip_port_proto", do_black_ip_port_proto, 0x10, 0, NULL},
    {"wan_rate",            do_wan_rate,            0x10, 0, NULL},
    {"mirror",              do_packet_mirror,       0x10, 0, NULL},
    {"wanmtu",              do_wan_mtu,             0x10, 0, NULL},
    {"mtu",                 do_set_ppe_mtu,             0x10, 0, NULL},
    {"nptv6_prefix",    do_nptv6_prefix,    0x10, 0, NULL},    
    {"nptv6_mode",    do_nptv6_mode,    0x10, 0, NULL},
	{"cleanuni",            do_clean_uni_entry,     0x10, 0, NULL},
	{"ppe2",            do_ppe2_cmd,     0x10, 0, NULL},
	{"vxlan_hash",            do_ppe_vxlanhash,     0x10, 0, NULL},
	{"l2tp_hash",            do_ppe_l2tphash,     0x10, 0, NULL},
    {NULL,                  NULL,                   0x10, 0, NULL},
};

int main(int argc, char **argv) 
{
    return subcmd(ppeCmds, argc, argv, NULL);
}

