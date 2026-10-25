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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <net/if.h>
#include <linux/types.h>
#include "libapi_lib_utility.h"
#include "libapi_lib_switchmgr.h"

/** @defgroup SwitchCmd Switch API Commands
@{
*/

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
struct swconfig
{
	int skfd;
	struct ifreq ifr;
};
/************************************************************************
*        F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/*******************************************************************************/
/*  PORT commands                                                                                                             */
/*******************************************************************************/
static int doPortQosEnable(int argc, char *argv[], void *p)
{
    u8 port = 0, enable = 0;

    port   = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
    
    return switchmgr_lib_set_port_qos(port, enable);
}

static int doPortQosMode(int argc, char *argv[], void *p)
{
    int i;
    u8 port = 0;
    ECNT_SWITCHMGR_QOS_CFG qosparam;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    memset(&qosparam, 0, sizeof(qosparam));
    
    if (strcmp((const char*)(argv[2]), "sp") == 0)
    {
        qosparam.qosmode = 0;
    }
    else if (strcmp((const char*)(argv[2]), "wrr") == 0)
    {
        qosparam.qosmode = 1;
        for (i = 0; i < 4; i++) 
        {
            qosparam.wi[i] = atoi(argv[i + 3]);
        }
    }
    else if (strcmp((const char*)(argv[2]), "sp_wrr") == 0)
    {
        qosparam.qosmode = 2;
        qosparam.wrr_queue_cnt = atoi(argv[3]);
        for (i = 0; i < 3; i++) {
            qosparam.wi[i] = atoi(argv[i + 4]);
        }
    }
    else
    {
        return -1;
    }

    return switchmgr_lib_set_port_qos_param(port, qosparam);
}


static int doPortQosMode_8queue(int argc, char *argv[], void *p)
{
    int i;
    u8 port = 0;
    ECNT_SWITCHMGR_QOS_CFG qosparam;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    memset(&qosparam, 0, sizeof(qosparam));

	qosparam.eight_queue = 1;
    if (strcmp((const char*)(argv[2]), "sp") == 0)
    {
        qosparam.qosmode = 0;
    }
    else if (strcmp((const char*)(argv[2]), "wrr") == 0)
    {
        qosparam.qosmode = 1;
        for (i = 0; i < 8; i++) 
        {
            qosparam.wi[i] = atoi(argv[i + 3]);
        }
    }
    else if (strcmp((const char*)(argv[2]), "sp_wrr") == 0)
    {
        qosparam.qosmode = 2;
        qosparam.wrr_queue_cnt = atoi(argv[3]);
        for (i = 0; i < qosparam.wrr_queue_cnt; i++) {
            qosparam.wi[i] = atoi(argv[i + 4]);
        }
    }
    else
    {
        return -1;
    }

    return switchmgr_lib_set_port_qos_param(port, qosparam);
}


static int doPortRateControl(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, dir = 0;
    u32 rate = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    dir  = strtoul((const char*)(argv[2]), (char **)NULL, 10);

    if (argc > 3)
    {
        rate = strtoul((const char*)(argv[3]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_ratelimit(port, dir, rate);
    }
    else
    {
        ret = switchmgr_lib_get_port_ratelimit(port, dir, &rate);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            if (rate == 0)
            {
                printf("Port%d[%s] ratectl: %s\n", port, dir ? "ingress" : "egress", "disable");
            }
            else
            {
                printf("Port%d[%s] ratectl: %ukbps\n", port, dir ? "ingress" : "egress", rate);
            }
        }
    }

    return ret;
}

static int doPortStpStatus(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    ECNT_SWITCHMGR_STP_STATUS stp_status = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        stp_status = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_stp_status(port, stp_status);
    }
    else
    {
        ret = switchmgr_lib_get_port_stp_status(port, &stp_status);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d stpstatus: %d\n", port, stp_status);
        }
    }

    return ret;
}

static int doPortLinkState(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    ECNT_SWITCHMGR_LINK_STATE state;
    ECNT_SWITCHMGR_LINK_SPEED speed;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);

    ret = switchmgr_lib_get_port_link_state(port, &state, &speed);
    if (ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("Port%d link_sate : %s\n", port, state ? "up" : "down");
        printf("Port%d link_speed: %d\n", port, speed);
    }

    return ret;
}

static int doPortFlowControl(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, enable = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_flow_control(port, enable);
    }
    else
    {
        ret = switchmgr_lib_get_port_flow_control(port, &enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d flowcontrol : %s\n", port, enable ? "enable" : "disable");
        }
    }

    return ret;
}

static int doPortMacLimit(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    u32 num = 0;
    
    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        num = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_mac_limit(port, num);
    }
    else
    {
        ret = switchmgr_lib_get_port_mac_limit(port, &num);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d maclimit : %u\n", port, num);
        }
    }
    
    return ret;
}

static int doPortMacLearn(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    u32 num = 0;
    
    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);

    ret = switchmgr_lib_get_port_mac_learn_num(port, &num);
    if (ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("Port%d maccnt : %u\n", port, num);
    }

    
    return ret;
}



static int doPortMac(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 portmac[6], port = 0;
    
    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    ret = switchmgr_lib_get_port_mac(port, portmac);
    if (ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("Port%d mac : %02x:%02x:%02x:%02x:%02x:%02x\n", port, portmac[0], portmac[1], portmac[2], portmac[3], portmac[4], portmac[5]);
    }
    return ret;
}

static int doPortMacLearning(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, enable = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_learning(port, (enable ? 0 : 1));
    }
    else
    {
        ret = switchmgr_lib_get_port_learning(port, &enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d maclearning : %s\n", port, enable ? "disable" : "enable");
        }
    }
    
    return ret;
}

static int doPortBrgInd(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, ind = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        ind = strtoul((const char*)(argv[2]), (char **)NULL, 16);
        ret = switchmgr_lib_set_port_brgInd(port, ind);
    }
    else
    {
        ret = switchmgr_lib_get_port_brgInd(port, &ind);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d brgind : 0x%x\n", port, ind);
        }
    }
    
    return ret;
}

static int doPortUnknownMac(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, enable = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_discard_unknown_mac(port, enable);
    }
    else
    {
        ret = switchmgr_lib_get_port_discard_unknown_mac(port, &enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d unknown mac : %s\n", port, enable ? "discard" : "forward");
        }
    }
    
    return ret;
}

static int doPortPtp(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, enable = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        ret = switchmgr_lib_set_port_ptp_enable(port);
    }
    else
    {
        ret = switchmgr_lib_get_port_ptp_enable(port, &enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d ptp : %s\n", port, enable ? "enable" : "disable");
        }
    }
    
    return ret;
}

static int doPortMirror(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    ECNT_SWITCHMGR_PORT_MIRROR portbased;

    port                  = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    portbased.port_tx_mir = strtoul((const char*)(argv[2]), (char **)NULL, 10);
    portbased.port_rx_mir = strtoul((const char*)(argv[3]), (char **)NULL, 10);
    portbased.acl_mir     = strtoul((const char*)(argv[4]), (char **)NULL, 10);
    portbased.vlan_mis    = strtoul((const char*)(argv[5]), (char **)NULL, 10);
    portbased.igmp_mir    = strtoul((const char*)(argv[6]), (char **)NULL, 10);

    ret = switchmgr_lib_set_port_mirror(port, &portbased);
    return ret;
}

static int doPortMacEnable(int argc, char *argv[], void *p)
{
    u8 port = 0, enable = 0, direction = 0;

    port      = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    direction = strtoul((const char*)(argv[2]), (char **)NULL, 10);
    enable    = strtoul((const char*)(argv[3]), (char **)NULL, 10);

    return switchmgr_lib_set_port_mac_txrx(port, direction, enable);
}

static int doPortCfgStatus(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, cfg = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);

    ret = switchmgr_lib_get_port_configuration_status(port, &cfg);
    if (ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("Port%d cfgstatus: %x\n", port, cfg);
    }
    return ret;
}

#define HELP_PORT_QOS_ENABLE        "switchmgr port qos <port:0~6> <enable:1|disable:0>\n"

#define HELP_PORT_QOS_MODE          "switchmgr port qosmode <port:0~6> (sp | wrr <w1> <w2> <w3> <w4> | sp_wrr <queuecnt:1~3> <w3> <w2> <w1>)\n"

#define HELP_PORT_RATECTL           "switchmgr port ratectl <port:0~6> <ingress:1|egress:0> [rate(kbps): 0-1000000)]\n"\
                                    "Description: Set/Get port rate limit. Set rate to 0 to disable ratectl.\n"

#define HELP_PORT_STPSTATUS         "switchmgr port stpstatus <port:0~3> [<stpstatus>]\n"\
                                    "Description: Set/Get port stp status.\n"\
                                    "0:disabled\n"\
                                    "1:blocking\n"\
                                    "2:learning\n"\
                                    "3:forwarding\n"\
                                    "5:linkdown (only for get)\n"

#define HELP_PORT_LINK_STATE        "switchmgr port linkstate <port> \n"

#define HELP_PORT_FLOWCONTROL     "switchmgr port flowcontrol <port> [<enable:1|disable:0>]\n"

#define HELP_PORT_MACLIMIT          "switchmgr port maclimit <port> [<num>]\n" //TODO: need description for <num>

#define HELP_PORT_MAC               "switchmgr port mac <port> \n"

#define HELP_PORT_MACLEARNING       "switchmgr port maclearning <port> [<enable:1|disable:0>]\n"

#define HELP_PORT_BRGIND            "switchmgr port brgind <port> [<ind>]\n" //TODO: duplicate api??

#define HELP_PORT_DISCARDUNKNOWN    "switchmgr port discardunknown <port> [<enable:0|disable:1>]\n"

#define HELP_PORT_PTP               "switchmgr port ptp <port> [<enable:1>]\n"\
                                    "Description: Set/Get port ptp enable.\n"\
                                    "             Use port > 6 to disable all ptp port.\n"  //TODO: stupid set api parameter

#define HELP_PORT_MIRROR            "switchmgr port mirror <port> <port_tx_mir:0|1> <port_rx_mir:0|1> <acl_mir:0|1> <vlan_mis:0|1> <igmp_mir:0|1>\n"\

#define HELP_PORT_MACENABLE         "switchmgr port macenable <port> <tx:0|rx:1> <enable:1|disable:0>\n"

#define HELP_PORT_CFGSTATUS         "switchmgr mac getcfgstatus <port>\n"

static const cmds_t portCmds[] = 
{
    {"qos",             doPortQosEnable,        0x12, 2, HELP_PORT_QOS_ENABLE},
    {"qosmode",         doPortQosMode,          0x12, 2, HELP_PORT_QOS_MODE},
    {"qosmode_8queue",  doPortQosMode_8queue,   0x12, 2, HELP_PORT_QOS_MODE},
    {"ratectl",         doPortRateControl,      0x12, 2, HELP_PORT_RATECTL}, 
    {"stpstatus",       doPortStpStatus,        0x12, 1, HELP_PORT_STPSTATUS},
    {"linkstate",       doPortLinkState,        0x12, 1, HELP_PORT_LINK_STATE},
    {"flowcontrol",     doPortFlowControl,      0x12, 1, HELP_PORT_FLOWCONTROL},
    {"mac",             doPortMac,              0x12, 1, HELP_PORT_MAC},
    {"maclimit",        doPortMacLimit,         0x12, 1, HELP_PORT_MACLIMIT},
	{"maclearnnum",		doPortMacLearn, 		0x12, 1, HELP_PORT_MACLIMIT},
    {"maclearning",     doPortMacLearning,      0x12, 1, HELP_PORT_MACLEARNING},
    {"brgind",          doPortBrgInd,           0x12, 1, HELP_PORT_BRGIND},
    {"discardunknown",  doPortUnknownMac,       0x12, 1, HELP_PORT_DISCARDUNKNOWN},
    {"ptp",             doPortPtp,              0x12, 1, HELP_PORT_PTP},
    {"mirror",          doPortMirror,           0x12, 6, HELP_PORT_MIRROR},
    {"macenable",       doPortMacEnable,        0x12, 3, HELP_PORT_MACENABLE},
    {"cfgstatus",       doPortCfgStatus,        0x12, 1, HELP_PORT_CFGSTATUS},
    {NULL,              NULL,                   0x10, 0, NULL}
};

/*******************************************************************************/
/*  VLAN commands                                                                                                             */
/*******************************************************************************/
static int doVlanSetPvid(int argc, char *argv[], void *p)
{
    u8 port=0;
    u16 pvid=0;
    int ret = 0;
    
    port=atoi(argv[1]);
    pvid=atoi(argv[2]);
    ret = switchmgr_lib_set_vlan_pvid(port,pvid);
    return ret;
}

static int doVlanSetVid(int argc, char *argv[], void *p){
    ECNT_SWITCHMGR_VLAN_ENTRY viddata = {0};
    int ret = 0;
    
    viddata.index=atoi(argv[1]);
    viddata.active=atoi(argv[2]);
    viddata.vid=atoi(argv[3]);

    viddata.portMap=atoi(argv[4]);
    viddata.tagPortMap=atoi(argv[5]);

    printf("subcmd parameter argc = %d\r\n", argc);
    if (argc >= 7)
    {
        viddata.ivl_en=atoi(argv[6]);
        if (argc >= 8)
        {
            viddata.fid=atoi(argv[7]);
            if (argc >= 9)
            {
                viddata.stag=atoi(argv[8]);
            }
        }
    }
    ret = switchmgr_lib_set_vlan_vid(&viddata);

    return ret;
}

static int doVlanDisp(int argc, char *argv[], void *p){
    int ret = 0;

    ret = switchmgr_lib_set_vlan_disp();
    return ret;
}

static int doVlanSetPortAttr(int argc, char *argv[], void *p){
    u8 port = 0, attr = 0;
    int ret = 0; 
    
    port=atoi(argv[1]);
    attr=atoi(argv[2]);

    ret = switchmgr_lib_set_vlan_port_attr(port,attr);

    return ret;
}

static int doVlanSetPortMode(int argc, char *argv[], void *p){
    u8 port = 0, mode = 0;
    int ret = 0;
    
    port=atoi(argv[1]);
    mode=atoi(argv[2]);

    ret = switchmgr_lib_set_vlan_port_mode(port,mode);

    return ret;
}

static int doVlanSetEgressTagPVC(int argc, char *argv[], void *p){
    u8 port = 0, eg_tag = 0;
    int ret = 0;
    
    port=atoi(argv[1]);
    eg_tag=atoi(argv[2]);
    
    ret = switchmgr_lib_set_vlan_egress_tag_pvc(port,eg_tag);
    return ret;
}

static int doVlanSetEgressTagPCR(int argc, char *argv[], void *p){
    u8 port = 0, eg_tag = 0;
    int ret = 0;

    port=atoi(argv[1]);
    eg_tag=atoi(argv[2]);

    ret = switchmgr_lib_set_vlan_egress_tag_pcr(port,eg_tag);

    return ret;
}

static int doVlanSetAccFrm(int argc, char *argv[], void *p){
    u8 port = 0, type = 0;
    int ret = 0;

    port=atoi(argv[1]);
    type=atoi(argv[2]);

    ret = switchmgr_lib_set_vlan_acc_frm(port,type);
    return ret;
}

static int doetag(int argc, char *argv[], void *p)
{
    u16 vid = 0;
    u8 port = 0, etag = 0;
    int ret = 0;

    if(argc == 3)
    {
        port = atoi(argv[1]);
        vid = atoi(argv[2]);
        ret = switchmgr_lib_get_vlan_entry_etag(vid,port,&etag );
        printf("[%s]%d:ret = %d,etag = %d\n", __FUNCTION__, __LINE__, ret, etag);
    }
    else if(argc > 3)
    {
        port = atoi(argv[1]);
        vid = atoi(argv[2]);
        etag = atoi(argv[3]);
        ret = switchmgr_lib_set_vlan_entry_etag(vid,port,etag);
        printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    }
    return ret;
}

static int dostag(int argc, char *argv[], void *p)
{
    u16 vid = 0;
    u8 port = 0, stag = 0;
    int ret = 0;

    if(argc == 3)
    {
        port = atoi(argv[1]);
        vid = atoi(argv[2]);
        ret = switchmgr_lib_get_vlan_entry_stag(vid,port,&stag );
        printf("[%s]%d:ret = %d,stag = %d\n", __FUNCTION__, __LINE__, ret, stag);
    }
    else if(argc > 3)
    {
        port = atoi(argv[1]);
        vid = atoi(argv[2]);
        stag = atoi(argv[3]);
        ret = switchmgr_lib_set_vlan_entry_stag(vid,port,stag);
        printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    }
    return ret;
}

static int doaddstag(int argc, char *argv[], void *p)
{
    u16 vid = 0;
    u8 port = 0, stag = 0;
    int ret = 0;

    port = atoi(argv[1]);
    vid = atoi(argv[2]);
    stag = atoi(argv[3]);
    ret = switchmgr_lib_add_vlan_port_stag(port,stag,vid);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

static int dosetetagmode(int argc, char *argv[], void *p)
{
    u16 vid = 0;
    u8 etag_mode = 0;
    int ret = 0;

    vid = atoi(argv[1]);
    etag_mode = atoi(argv[2]);
    ret = switchmgr_lib_set_vlan_entry_etag_mode(vid,etag_mode);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

static int dovlanaction(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port_id; 
    u8 enable;
    int o_vid = 0, n_vid = 0, vlan_mode = 0;


    port_id = atoi(argv[1]);
    o_vid = atoi(argv[2]);
    n_vid = atoi(argv[3]);
    vlan_mode = atoi(argv[4]);
    enable = atoi(argv[5]);
    ret = switchmgr_lib_set_per_vlan_action(port_id,o_vid,n_vid,vlan_mode,enable);
    printf("[%s]%d: ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

static int doportvlanaction(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port_id; 
    u8 enable;
    int n_vid = 0, vlan_mode = 0;


    port_id = atoi(argv[1]);
    n_vid = atoi(argv[2]);
    vlan_mode = atoi(argv[3]);
    enable = atoi(argv[2]);
    ret = switchmgr_lib_set_per_port_vlan_action(port_id,n_vid,vlan_mode,enable);
    printf("[%s]%d: ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

static int dovlanentryenable(int argc, char *argv[], void *p)
{
    u16 vid = 0;
    u8 sw = 0;
    int ret = 0;

    vid = atoi(argv[1]);
    sw = atoi(argv[2]);
    ret = switchmgr_lib_set_vlan_entry_enable(vid,sw);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

#define HELP_VLAN_VID       "vlan vid <index> <active:0|1> <vid> <portMap> <egtagPortMap>\n" \
                            "    <ivl_en> <fid> <stag>\n"
#define HELP_VLAN_PORT_ATTR "vlan port-attr <port> <attr:0~3>\n" \
                            "<attr>: 0: user port\n" \
                            "   1: statck port\n" \
                            "   2: translation port\n" \
                            "   3: transparent port\n"
#define HELP_VLAN_PORT_MODE "vlan port-mode <port> <mode:0~3>\n" \
                            "<mode>: 0: port matrix mode\n" \
                            "   1: fallback mode\n" \
                            "   2: check mode\n" \
                            "   3: security mode\n"
#define HELP_VLAN_EGRESS_TAG_PVC    "vlan eg-tag-pvc <port> <eg_tag:0~7>\n" \
                                 "<eg_tag>: 0: disable\n" \
                                 "  1: consistent\n" \
                                 "  2: reserved\n" \
                                 "  3: reserved\n" \
                                 "  4: untagged\n" \
                                 "  5: swap\n" \
                                 "  6: tagged\n" \
                                 "  7: stack\n"
#define HELP_VLAN_EGRESS_TAG_PCR    "vlan eg-tag-pcr <port> <eg_tag:0~3>\n" \
                                 "<eg_tag>: 0: untagged\n" \
                                 "  1: swap\n" \
                                 "  2: tagged\n" \
                                 "  3: stack\n"
#define HELP_VLAN_ACC_FRM   "vlan acc-frm <port> <acceptable_frame_type:0~3>\n" \
                                "<type>: 0: admit all frames\n" \
                                "   1: admit only vlan-taged frames\n" \
                                "   2: admit only untagged or priority-tagged frames\n" \
                                "   3: reserved\n"
                                
#define HELP_ETAG   "get etag <port:0~6> <vid:1~4095>\n" \
                            "set etag <port:0~6> <vid:1~4095> <etag:0~3>"
#define HELP_STAG   "get stag <port:0~6> <vid:1~4095>\n" \
                            "set stag <port:0~6> <vid:1~4095> <etag:0~3>"

static const cmds_t vlanCmds[] = 
{
    {"pvid",            doVlanSetPvid,          0x12, 2,  "vlan pvid <port> <pvid>"},
    {"vid",             doVlanSetVid,           0x12, 5,  HELP_VLAN_VID},
    {"disp",            doVlanDisp,             0x12, 0,  NULL},
    {"port-attr",       doVlanSetPortAttr,      0x12, 2,  HELP_VLAN_PORT_ATTR},
    {"port-mode",       doVlanSetPortMode,      0x12, 2,  HELP_VLAN_PORT_MODE},
    {"eg-tag-pvc",      doVlanSetEgressTagPVC,  0x12, 2,  HELP_VLAN_EGRESS_TAG_PVC},
    {"eg-tag-pcr",      doVlanSetEgressTagPCR,  0x12, 2,  HELP_VLAN_EGRESS_TAG_PCR},
    {"acc-frm",         doVlanSetAccFrm,        0x12, 2,  HELP_VLAN_ACC_FRM},
    {"etag",            doetag,                 0x12, 2,  HELP_ETAG},
    {"stag",            dostag,                 0x12, 2,  HELP_STAG},
    {"addstag",         doaddstag,              0x12, 3,  "<port:0~6> <vid:1~4095> <etag:0~3>"},
    {"setetagmode",     dosetetagmode,          0x12, 2,  "<vid:1~4095> <etag_mode:0|1>"},
    {"action",          dovlanaction,           0x12, 5,  "<port_id:0~6> <o_vid> <n_vid> <vlan_mode:0~3> <enable:0|1>"},
    {"per-port",        doportvlanaction,       0x12, 4,  "<port_id:0~6> <n_vid> <vlan_mode:0~3> <enable:0|1>"},
    {"vlanentryen",     dovlanentryenable,      0x12, 2,  "<vid:1~4095> <sw:0|1>"},
    {NULL,              NULL,                   0x10, 0, NULL},
};

/*******************************************************************************/
/*  PHY commands                                                                                                              */
/*******************************************************************************/
static int doPhyLoopBack(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port=0, enable=0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_loopback(port, enable);
    }
    else
    {
        ret = switchmgr_lib_get_port_loopback(port, &enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d loopback: %s\n", port, enable ? "enable" : "disable");
        }
    }

    return ret;
}

static int doPhySpeedMode(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port=0;
    ECNT_SWITCHMGR_SPEED_MODE mode;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        mode = atoi(argv[2]);
        ret = switchmgr_lib_set_port_speed_mode(port, mode);
    }
    else
    {
        ret = switchmgr_lib_get_port_speed_mode(port, &mode);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d speedmode: %d\n", port, mode);
        }
    }

    return ret;
}

static int doPhyAdmin(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, up = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        up = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_admin(port, up);
    }
    else
    {
        ret = switchmgr_lib_get_port_admin(port, &up);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d admin : %s\n", port, up ? "up" : "down");
        }
    }
    return ret;
}

static int doPhyPause(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, enable = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_pause(port, enable);
    }
    else
    {
        ret = switchmgr_lib_get_port_pause(port, &enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d pause : %s\n", port, enable ? "enable" : "disable");
        }
    }
    return ret;
}

static int doPhyDuplex(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    char duplex[32] = {0};

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        ret = switchmgr_lib_set_port_duplex(port, argv[2]);
    }
    else
    {
        ret = switchmgr_lib_get_port_duplex(port, duplex);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d duplex : %s\n", port, duplex);
        }
    }
    
    return ret;
}

static int doPhyAutoNeg(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, enable = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_autoneg_enable(port, enable);
    }
    else
    {
        ret = switchmgr_lib_get_port_autoneg_enable(port, &enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d auto-negotiation : %s\n", port, enable ? "enable" : "disable");
        }
    }
    
    return ret;
}

static int doPhyAutoNegRestart(int argc, char *argv[], void *p)
{
    u8 port = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_port_autoneg_restart(port);
}

static int doPhyAutoNegState(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, fail, enable;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    
    ret = switchmgr_lib_get_port_autoneg_enable(port, &enable);
    if (ECNT_SWITCHMGR_SUCCESS != ret)
    {
        return ret;
    }

    if(!enable)
    {
        printf("Port%d auto-negotiation : disable\n", port);
    }
    else
    {
        ret = switchmgr_lib_get_port_autoneg_fail(port, &fail);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d auto-neg state : %s\n", port, fail ? "Fail" : "Success");
        }
    }
    return ret;
}

static int doPhyAutoDetect(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, auto_detection = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        auto_detection = strtoul((const char*)(argv[2]), (char **)NULL, 16);
        ret = switchmgr_lib_set_port_autodetection(port, auto_detection);
    }
    else
    {
        ret = switchmgr_lib_get_port_autodetection(port, &auto_detection);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d autodetect : %d\n", port, auto_detection);
        }
    }
    
    return ret;
}

static int doPhyPauseTime(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    u32 time = 0;
    
    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        time = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_pause_time(port, time);
    }
    else
    {
        ret = switchmgr_lib_get_port_pause_time(port, &time);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d pausetime : %u\n", port, time);
        }
    }
    
    return ret;
}

static int doPhyDteDceInd(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, ind = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        ind = strtoul((const char*)(argv[2]), (char **)NULL, 16);
        ret = switchmgr_lib_set_port_dte_dce_ind(port, ind);
    }
    else
    {
        ret = switchmgr_lib_get_port_dte_dce_ind(port, &ind);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d dtedceind : 0x%x\n", port, ind);
        }
    }
    
    return ret;
}

static int doPhyMaxSpeed(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    char speed[32] = {0};

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if(argc > 2)
    {
        ret = switchmgr_lib_set_port_max_bitrate(port, argv[2]);
    }
    else
    {
        ret = switchmgr_lib_get_port_max_bitrate(port, speed);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d maxspeed : %s\n", port, speed);
        }
    }
    
    return ret;
}

static int doPhyANFail(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, fail;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    
    ret = switchmgr_lib_get_port_fail(port, &fail);
    if (ECNT_SWITCHMGR_SUCCESS != ret)
    {
        printf("Port%d fail : %s\n", port, fail ? "NOK" : "OK");
    }
    return ret;
}

#define HELP_PHY_LOOPBACK          "switchmgr phy loopback <port:0~3> [<enable:1|disable:0>]\n"

#define HELP_PHY_SPEED_MODE        "switchmgr phy speedmode <port:0~3> [<mode>]\n"\
                                    "Description: Set/Get port speed mode.\n"\
                                    "1:auto-negoation\n"\
                                    "2:100FD\n"\
                                    "3:100HD\n"\
                                    "4:10FD\n"\
                                    "5:10HD\n"

#define HELP_PHY_ADMIN             "switchmgr phy admin <port> [<up:1|down:0>]\n"

#define HELP_PHY_PAUSE             "switchmgr phy pause <port> [<enable:1|disable:0>]\n"

#define HELP_PHY_DUPLEX            "switchmgr phy duplex <port> [<Full|Half>]\n" //TODO: use enum as parameter

#define HELP_PHY_AUTONEG           "switchmgr phy autoneg <port> [<enable:1|disable:0>]\n"

#define HELP_PHY_AUTONEG_RESTART   "switchmgr phy autonegrestart <port>\n"

#define HELP_PHY_AUTONEG_STATE     "switchmgr phy autonegstate <port>\n"  //TODO: duplicate api??

#define HELP_PHY_AUTODETECT        "switchmgr phy autodetect <port> [<value>]\n"  //TODO: might need a different name

#define HELP_PHY_PAUSETIME         "switchmgr phy pausetime <port> [<time>]\n"

#define HELP_PHY_DTEDCE_IND        "switchmgr phy dtedceind <port> [<ind>]\n"

#define HELP_PHY_MAXSPEED          "switchmgr phy maxspeed <port> [<Auto|100|10>]\n" //TODO: use enum as parameter

#define HELP_PHY_FAIL              "switchmgr phy fail <port>\n"  //TODO: stupid api, for fh only

static const cmds_t phyCmds[] = 
{
    {"loopback",        doPhyLoopBack,          0x12, 1, HELP_PHY_LOOPBACK},
    {"speedmode",       doPhySpeedMode,         0x12, 1, HELP_PHY_SPEED_MODE},
    {"admin",           doPhyAdmin,             0x12, 1, HELP_PHY_ADMIN},
    {"pause",           doPhyPause,             0x12, 1, HELP_PHY_PAUSE},
    {"duplex",          doPhyDuplex,            0x12, 1, HELP_PHY_DUPLEX},
    {"autoneg",         doPhyAutoNeg,           0x12, 1, HELP_PHY_AUTONEG},
    {"autonegrestart",  doPhyAutoNegRestart,    0x12, 1, HELP_PHY_AUTONEG_RESTART},
    {"autonegstate",    doPhyAutoNegState,      0x12, 1, HELP_PHY_AUTONEG_STATE},
    {"autodetect",      doPhyAutoDetect,        0x12, 1, HELP_PHY_AUTODETECT},
    {"pausetime",       doPhyPauseTime,         0x12, 1, HELP_PHY_PAUSETIME},
    {"dtedceind",       doPhyDteDceInd,         0x12, 1, HELP_PHY_DTEDCE_IND},
    {"maxspeed",        doPhyMaxSpeed,          0x12, 1, HELP_PHY_MAXSPEED},
    {"fail",            doPhyANFail,            0x12, 1, HELP_PHY_FAIL},
    {NULL,              NULL,                   0x10, 0, NULL},
};

/*******************************************************************************/
/*  MAC commands                                                                                                              */
/*******************************************************************************/
static int doAgeTime(int argc, char *argv[], void *p)
{
    u32 time = 0;
    int ret = 0;

    if (argc > 1)
    {
        time = strtoul((const char*)(argv[1]), (char **)NULL, 10);
        ret = switchmgr_lib_set_age_time(time);
    }
    else
    {
        ret = switchmgr_lib_get_age_time(&time);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("AgeTime: %u\n", time);
        }
    }
    
    return ret;
}

static int doMacTblClr(int argc, char *argv[], void *p){
    int ret = 0;
    u8 port;

    if(argc > 1)
    {
        port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
        ret = switchmgr_lib_clr_port_mac_table(port);
    }
    else
    {
        ret = switchmgr_lib_clr_mac_table();
    }
    return ret;
}

static int doPortBrtable(int argc, char *argv[], void *p)
{
    int ret = 0, idx = 0;
    u8 port = 0;
    ECNT_SWITCHMGR_BRIDGE_TABLE *brtable = NULL;

    brtable = (ECNT_SWITCHMGR_BRIDGE_TABLE *)malloc(sizeof(ECNT_SWITCHMGR_BRIDGE_TABLE));
    if (!brtable)
    {
        return -1;
    }

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    bzero(brtable, sizeof(ECNT_SWITCHMGR_BRIDGE_TABLE));
    
    ret = switchmgr_lib_get_port_bridge_table(port, brtable);
    if (ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("port-no\tmac-addr\t\tageing-timer\tvid\n");
        for (idx = 0; idx < brtable->count; idx++)
        {
            ECNT_SWITCHMGR_MAC_ENTRY *fdb = &(brtable->macinfo[idx]);
            printf("%3i\t%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\t%d",
                  fdb->port, fdb->mac[0], fdb->mac[1], fdb->mac[2],
                  fdb->mac[3], fdb->mac[4], fdb->mac[5], fdb->timer);
            if(fdb->vid != 0xFFFF)
            {
                printf("\t\t%d\n", fdb->vid);
            }
            else
            {
                printf("\t\tuntag\n");
            }
        }
    }

    free(brtable);
    brtable = NULL;
    return 0;
}

static int doGetPortAllMac(int argc, char *argv[], void *p)
{
    int ret = 0, i = 0;
    u8 port = 0;
    ECNT_SWITCHMGR_PORT_ALL_MAC mac;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);

    ret = switchmgr_lib_get_port_all_mac(port, &mac);
    if (ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("num = %d\n", mac.vaild_entry_num);
        for(i = 0; i < mac.vaild_entry_num; i++)
        {
            printf("num = %d,mac = %02x:%02x:%02x:%02x:%02x:%02x\n", i, 
                    mac.portmac[i].mac[0], mac.portmac[i].mac[1], mac.portmac[i].mac[2],
                    mac.portmac[i].mac[3], mac.portmac[i].mac[4], mac.portmac[i].mac[5]);
        }
    }
    return ret;
}

static int doGetPortByMac(int argc, char *argv[], void *p){ 
	u32 i, j;
    int ret;
	char temp_str[3] = {0,0,0};
    ECNT_SWITCHMGR_MAC_ENTRY inptr_PortMac = {0};
	
    for (i = 0, j = 0; i < 6; i ++){
        temp_str[0] = ((const char*)(argv[1]))[j++];
        temp_str[1] = ((const char*)(argv[1]))[j++];
        inptr_PortMac.mac[i] = strtoul(temp_str, (char **)NULL, 16);
    }
    
    inptr_PortMac.port = 255;
    
    ret = switchmgr_lib_get_port_by_mac(&inptr_PortMac);

    if(inptr_PortMac.port != 255)
        printf("mac %02x:%02x:%02x:%02x:%02x:%02x is found, port is %d.\n", 
        inptr_PortMac.mac[0], inptr_PortMac.mac[1], inptr_PortMac.mac[2],
        inptr_PortMac.mac[3], inptr_PortMac.mac[4], inptr_PortMac.mac[5],
        inptr_PortMac.port);
    else
        printf("mac %02x:%02x:%02x:%02x:%02x:%02x cannot be found!\n", 
        inptr_PortMac.mac[0], inptr_PortMac.mac[1], inptr_PortMac.mac[2],
        inptr_PortMac.mac[3], inptr_PortMac.mac[4], inptr_PortMac.mac[5]);
    
	return ret;    
    
}/*end doArlGetPortByMac*/

static int doArlAging(int argc, char *argv[], void *p)
{
    u8 aging_en=0;
    u32 time=0;
    int ret = 0;    
    aging_en=atoi(argv[1]);
    time=atoi(argv[2]);
    printf("[%s]%d:aging_en: %x, aging time: %x\n", __FUNCTION__, __LINE__, aging_en, time);

    ret = switchmgr_lib_set_arl_aging(aging_en, time);
    printf("[%s]%d:write ret: %d\n", __FUNCTION__, __LINE__, ret);

    return ret;
}

static int doArlMACTblAdd(int argc, char *argv[], void *p)
{
    u32 i = 0, j = 0;
    char temp_str[3] = {0,0,0};
    int ret = 0;
    ECNT_SWITCHMGR_ARL_MAC_ENTRY mactbl = {0};


    for (i = 0, j = 0; i < 6; i ++){
        temp_str[0] = ((const char*)(argv[1]))[j++];
        temp_str[1] = ((const char*)(argv[1]))[j++];
        mactbl.mac[i] = strtoul(temp_str, (char **)NULL, 16);
    }

    printf("[%s]%d:MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", __FUNCTION__, __LINE__, mactbl.mac[0], mactbl.mac[1], mactbl.mac[2], mactbl.mac[3], mactbl.mac[4], mactbl.mac[5]);

    
    mactbl.destportmap=atoi(argv[2]);
    

    printf("[%s]%d:dest port map: %d\n", __FUNCTION__, __LINE__, mactbl.destportmap);

    if (argc >= 4)
    {
        mactbl.leaky_en=atoi(argv[3]) & 0x00000001;
        if (argc >= 5)
        {
            mactbl.eg_tag=atoi(argv[4]) & 0x00000007;
            if (argc >= 6)
            {
                mactbl.usr_pri=atoi(argv[5]) & 0x00000007;
                if (argc >= 7)
                {
                    mactbl.sa_mir_en=atoi(argv[6]) & 0x00000001;
                    if (argc >= 8)
                    {
                        mactbl.sa_port_fw=atoi(argv[7]) & 0x00000007;
                    }
                }
            }
        }
    }

    ret = switchmgr_lib_set_arl_mactbladd(&mactbl);
    return ret;
}/*end doArlMACTblAdd*/

static int doArlAddrTblDisp(int ac_mat)
{
    int ret = 0;
    ret = switchmgr_lib_set_arl_mactbldisp(ac_mat);
    return ret;
}

static int doArlAddrTblDisp2(u32 ext_switch, int ac_mat)
{
    int ret = 0;
    ret = switchmgr_lib_set_arl_mactbldisp2(ext_switch,ac_mat);
    return ret;
}

static int doArlMACTblDisp(int argc, char *argv[], void *p)
{
    int ret = 0;
    if (argc >= 2)/* optional argv[1] : ext_switch*/
    {   
        if (atoi(argv[1]) == 255){
            ret = doArlAddrTblDisp2(0, 0);  /*2'b00 : MAC Address Entry*/
            ret = doArlAddrTblDisp2(1, 0);  /*2'b00 : MAC Address Entry*/
        }
        else{
            ret = doArlAddrTblDisp2(atoi(argv[1]), 0);  /*2'b00 : MAC Address Entry*/
        }
    }
    else{
        ret = doArlAddrTblDisp(0);  /* 2'b00 : MAC Address Entry*/
    }

    return ret;
}/*end doArlMACTblDisp*/

static int doArlAddrTblClr(int ac_mat)
{
    int ret = 0;
    ret = switchmgr_lib_set_arl_mactblclr(ac_mat);
    return ret;
}

static int doArlAddrTblClr2(u32 ext_switch, int ac_mat)
{
    int ret = 0;
    ret = switchmgr_lib_set_arl_mactblclr2(ext_switch,ac_mat);
    return ret;
}

static int doArlMACTblClr(int argc, char *argv[], void *p)
{
    int ret = 0;

    if (argc >= 2)/* optional argv[1] : ext_switch*/
    {   
        if (atoi(argv[1]) == 255){
            ret = doArlAddrTblClr2(0, 0);       /* 2'b00 : MAC Address Entry*/
            ret = doArlAddrTblClr2(1, 0);       /* 2'b00 : MAC Address Entry*/
        }
        else{
            ret = doArlAddrTblClr2(atoi(argv[1]), 0);   /*2'b00 : MAC Address Entry*/
        }
    }
    else{
        ret = doArlAddrTblClr(0);   /* 2'b00 : MAC Address Entry*/
    }

    return ret;
}/*end doArlMACTblClr*/

static int doBackPressure(int argc, char *argv[], void *p)
{
    u8 port = 0, queueid = 0;
    ECNT_SWITCHMGR_QUEUE_BACK_PRESSURE *backPressure;
    int ret = 0;

    backPressure = malloc(sizeof(ECNT_SWITCHMGR_QUEUE_BACK_PRESSURE));
    if (!backPressure)
    {
        return 1;
    }
    
    if(argc == 3)
    {
        port=atoi(argv[1]);
        queueid=atoi(argv[2]);
        ret = switchmgr_lib_get_port_queue_backpressure(port, queueid, backPressure);

        printf("[%s]%d:backPressure.Enable = %d,backPressure.MaxQueueThreshold = %d,backPressure.MinQueueThreshold = %d,backPressure.time = %d\n", __FUNCTION__, __LINE__, 
            backPressure->Enable, backPressure->MaxQueueThreshold, backPressure->MinQueueThreshold, backPressure->time);
    }
    else if(argc > 3)
    {
        port = atoi(argv[1]);
        backPressure->Enable = atoi(argv[2]);
        backPressure->MaxQueueThreshold = atoi(argv[3]);
        backPressure->MinQueueThreshold = atoi(argv[4]);
        queueid = atoi(argv[4]);
        ret = switchmgr_lib_set_port_queue_backpressure(port, queueid, backPressure);
        printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    }
    free(backPressure);
    return ret;
}

#define HELP_MAC_AGETIME        "swtichmgr mac agetime [ <time> ]>\n"

#define HELP_MAC_TABLE_CLEAR    "switchmgr mac clear [ <port> ]\n>"\
                                "Description: Clear all/port mac table:\n"

#define HELP_PORT_BRTABLE       "switchmgr mac brtable <port>\n>"

#define HELP_PORT_ALLMAC        "switchmgr mac all <port>\n>"

#define HELP_PORT_BY_MAC        "switchmgr mac port_by_mac <mac>\n"

#define HELP_MAC_AGING          "swtichmgr mac aging <enable:1|disable:0> <time:1-65535>\n"
                                
#define HELP_BACK_PRESSURE  "get back pressure <port> <queueid>\n" \
                                "set back pressure <port> <queueid> <enable> <MaxQueueThreshold> <MinQueueThreshold>"

static const cmds_t macCmds[] = 
{
    {"agetime",         doAgeTime,              0x12, 0, HELP_MAC_AGETIME}, //TODO: remove
    {"clear",           doMacTblClr,            0x12, 0, HELP_MAC_TABLE_CLEAR},
    {"brtable",         doPortBrtable,          0x12, 1, HELP_PORT_BRTABLE},
    {"all",             doGetPortAllMac,        0x12, 1, HELP_PORT_ALLMAC},
    {"port-by-mac",     doGetPortByMac,         0x12, 1, HELP_PORT_BY_MAC},   
    {"aging",           doArlAging,             0x12, 2, HELP_MAC_AGING}, 
    {"arlmactbl-add",   doArlMACTblAdd,         0x12, 2, "arl mactbl-add <MacAddr> <DestPortMap>\n"},
    {"arlmactbl-disp",  doArlMACTblDisp,        0x12, 0, "arl mactbl-disp\n"},
    {"arlmactbl-clr",   doArlMACTblClr,         0x12, 0, "arl mactbl-clr\n"},
    
    {"back_pressure",   doBackPressure,         0x12, 0, HELP_BACK_PRESSURE},
    {NULL,              NULL,                   0x10, 0, NULL}
};

/*******************************************************************************/
/*  WOL commands                                                                                                              */
/*******************************************************************************/

static int doWolLinkWake(int argc, char *argv[], void *p)
{
    u32 i = 0, j = 0;
    char temp_str[3] = {0,0,0};
	ECNT_SWITCHMGR_WOL_CONFIG *wol_data = NULL;
    int ret = 0;
	
	wol_data = malloc(sizeof(ECNT_SWITCHMGR_WOL_CONFIG));
    if (!wol_data)
    {
        return 1;
    }

    if(argc == 2)
    {
        wol_data->type = 0;
        wol_data->enable= atoi(argv[1]);
        ret = switchmgr_lib_set_wol(wol_data);
    }
	else
		printf("[%s]%d: wrong parameter \n", __FUNCTION__, __LINE__);
	
	free(wol_data);
    return ret;
}

static int doWolDAWake(int argc, char *argv[], void *p)
{
    u32 i = 0, j = 0, k = 0;
    char temp_str[3] = {0,0,0};
	ECNT_SWITCHMGR_WOL_CONFIG *wol_data = NULL;
    int ret = 0, mc_num = 0;;
	
	wol_data = malloc(sizeof(ECNT_SWITCHMGR_WOL_CONFIG));
    if (!wol_data)
    {
        return 1;
    }
	
    if(argc >= 3)
    {
   		wol_data->type = 1;
        wol_data->enable= atoi(argv[1]);
	    mc_num = argc-3;

	    for (i = 0, j = 0; i < 6; i++){
	        temp_str[0] = ((const char*)(argv[2]))[j++];
	        temp_str[1] = ((const char*)(argv[2]))[j++];
	        wol_data->sys_mac[i] = strtoul(temp_str, (char **)NULL, 16);
	    }

		for (k = 0; k < mc_num; k++){
		    for (i = 0, j = 0; i < 6; i++){
		        temp_str[0] = ((const char*)(argv[k+3]))[j++];
		        temp_str[1] = ((const char*)(argv[k+3]))[j++];
		        wol_data->mul_mac[k][i] = strtoul(temp_str, (char **)NULL, 16);
		    }
		}
        ret = switchmgr_lib_set_wol(wol_data);
    }
	else
		printf("[%s]%d: wrong parameter \n", __FUNCTION__, __LINE__);
	
	free(wol_data);
    return 0;
}

static int doWolMPWake(int argc, char *argv[], void *p)
{
    u32 i = 0, j = 0, k = 0;
    char temp_str[3] = {0,0,0};
	ECNT_SWITCHMGR_WOL_CONFIG *wol_data = NULL;
    int ret = 0, mc_num = 0;
	
	wol_data = malloc(sizeof(ECNT_SWITCHMGR_WOL_CONFIG));
    if (!wol_data)
    {
        return 1;
    }

    if(argc >= 3)
    {
   		wol_data->type = 2;
        wol_data->enable= atoi(argv[1]);
	    mc_num = argc-3;

	    for (i = 0, j = 0; i < 6; i++){
	        temp_str[0] = ((const char*)(argv[2]))[j++];
	        temp_str[1] = ((const char*)(argv[2]))[j++];
	        wol_data->sys_mac[i] = strtoul(temp_str, (char **)NULL, 16);
	    }
		
		for (k = 0; k < mc_num; k++){
		    for (i = 0, j = 0; i < 6; i++){
		        temp_str[0] = ((const char*)(argv[k+3]))[j++];
		        temp_str[1] = ((const char*)(argv[k+3]))[j++];
		        wol_data->mul_mac[k][i] = strtoul(temp_str, (char **)NULL, 16);
		    }
		}

	    ret = switchmgr_lib_set_wol(wol_data);
    }
	else
		printf("[%s]%d: wrong parameter \n", __FUNCTION__, __LINE__);
	
	free(wol_data);
    return 0;
}

static int doWolWulWake(int argc, char *argv[], void *p)
{
    u32 i = 0, j = 0, k = 0;
	u8 index = 0;
    char temp_str[3] = {0,0,0};
	ECNT_SWITCHMGR_WOL_CONFIG *wol_data = NULL;
    int ret = 0;
	
	wol_data = malloc(sizeof(ECNT_SWITCHMGR_WOL_CONFIG));
    if (!wol_data)
    {
        return 1;
    }

    if(argc >= 15 && argc <= 22)
    {
    	wol_data->type = 3;
        wol_data->enable= atoi(argv[1]);
		wol_data->index = atoi(argv[2]);
		wol_data->match_len = strtoul(argv[3], (char **)NULL, 16);
		wol_data->final_byte = strtoul(argv[4], (char **)NULL, 16);
		wol_data->checksum = strtoul(argv[5], (char **)NULL, 16);
		wol_data->byte_mask_num = argc - 6;
	    for (i = 0, j = 0; i < wol_data->byte_mask_num; i ++){
	        wol_data->byte_mask[i] = strtoul(argv[i+6], (char **)NULL, 16);
	    }
			
        ret = switchmgr_lib_set_wol(wol_data);
    }
	else
		printf("[%s]%d: wrong parameter \n", __FUNCTION__, __LINE__);
	
	free(wol_data);	
    return 0;
}

static int doWolStatus(int argc, char *argv[], void *p)
{
    int ret = 0;
	ECNT_SWITCHMGR_WOL_STATUS wol_data;
	
	ret = switchmgr_lib_get_wol(&wol_data);
    if (ret != ECNT_SWITCHMGR_SUCCESS)
    {
        return ret;
    }

    printf("[%s]%d:interrupt = 0x%x, da cnt = %d, mp cnt = %d, wuf cnt = %d\n", __FUNCTION__, __LINE__, 
		wol_data.interrupt, wol_data.da_cnt, wol_data.mp_cnt, wol_data.wuf_cnt);
	
	return 0;
}

#define HELP_WOL_LINK_WAKE      "switchmgr wol linkwake <enable:1|disable:0> \n>"
#define HELP_WOL_DA_WAKE        "switchmgr wol dawake <enable:1|disable:0> <sysmac> <mcmac1> <mcmac2> <mcmac3> <mcmac4> <mcmac5> <mcmac6> <mcmac7> <mcmac8>\n"
#define HELP_WOL_MP_WAKE        "swtichmgr wol mpwake <enable:1|disable:0> <sysmac> <mcmac1> <mcmac2> <mcmac3> <mcmac4> <mcmac5> <mcmac6> <mcmac7> <mcmac8>\n"            
#define HELP_WOL_WUL_WAKE       "swtichmgr wol wulwake <enable:1|disable:0> <index> <match_len> <final_byte> <check_sum> \n" \
								"<mask1> <mask2> <mask3> ...... <mask15> <mask16>\n"                      

static const cmds_t WolCmds[] = 
{
    {"linkwake",     doWolLinkWake,         0x12, 1, HELP_WOL_LINK_WAKE},
    {"dawake",       doWolDAWake,           0x12, 1, HELP_WOL_DA_WAKE},
    {"mpwake",       doWolMPWake,           0x12, 1, HELP_WOL_MP_WAKE},
    {"wulwake",      doWolWulWake,          0x12, 1, HELP_WOL_WUL_WAKE},
    {"status",       doWolStatus,           0x12, 0, NULL},   
    {NULL,              NULL,               0x10, 0, NULL},
};

/*******************************************************************************/
/*  QOS commands                                                                                                              */
/*******************************************************************************/
#define HELP_QUEUE_MARK     "switchmgr skb_mark <enable:1|disable:0> <shift:0~31> <width>\n"

#define HELP_WEIGHT "get weight <port:0~6> <queueid>\n" \
                        "set weight <port:0~6> <weight> <queueid>"

#define HELP_DROP_POLICY    "get drop policy <port> <queueid>\n" \
                                "set drop policy <DropPrecdenceColourMarking> <greenPacketDropMax_p> <greenPacketDropQueueMaxThr> <greenPacketDropQueueMinThr> <QueueDropW-q> <yellowPacketDropMaxn_p> <yellowPacketDropQueueMaxThr> <yellowPacketDropQueueMinThr>"

#define HELP_QOS_BASE       "qos base <port-based:0|tag-based:1|dscp-based:2|acl-based:3|arl-based:4|stag-based:5>\n"
#define HELP_QOS_PRIO_QMAP  "qos prio-qmap <prio:0~7> <queue:0~7>\n"
#define HELP_QOS_PRIO_TAGMAP    "qos prio-tagmap <prio:0~7> <tag:0~7>\n"
#define HELP_QOS_PRIO_DSCPMAP   "qos prio-dscpmap <prio:0~7> <dscp:0~63>\n"
#define HELP_QOS_PORT_PRIO  "qos port-prio <port> <prio:0~7>\n"
#define HELP_QOS_PORT_WEIGHT    "qos port-weight <port:0~7> <q0> <q1> <q2> <q3> <q4> <q5> <q6> <q7>\n" \
                             " <qn>: the weight of queue n, range: 1~16\n"
#define HELP_QOS_DSCP_PRIO  "qos dscp-prio <dscp:0~63> <prio:0~7> : for ingress\n"

static int doQueueMark(int argc, char *argv[], void*p)
{
    int enable, shift, width;
    
    enable = atoi(argv[1]);
    shift  = atoi(argv[2]);
    width  = atoi(argv[3]);
    return switchmgr_lib_set_queue_mark((u8)enable, (u8)shift, (u8)width);
}

static int doWeight(int argc, char *argv[], void *p)
{
    u8 port = 0, queueid = 0, weight = 0;
    int ret = 0;

    if(argc == 3)
    {
        port=atoi(argv[1]);
        queueid=atoi(argv[2]);
        ret = switchmgr_lib_get_port_queue_weight(port, queueid, &weight);
        printf("[%s]:%d:Weight is %08x\n", __FUNCTION__, __LINE__, weight);
    }
    else if(argc > 3)
    {
        port = atoi(argv[1]);
        weight = atoi(argv[2]);
        queueid = atoi(argv[3]);
        ret = switchmgr_lib_set_port_queue_weight(port, queueid, weight);
        printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    }
    return ret;
}

static int doqueuepriority(int argc, char *argv[], void *p)
{
    u16 priority = 0; 
    u8 queueId = 0;
    int ret = 0;

    queueId=atoi(argv[1]);
    priority=atoi(argv[2]);

    ret = switchmgr_lib_set_queue_priority(priority,queueId);
    return ret;
}

static int domaxqueuesize(int argc, char *argv[], void *p)
{
    int ret = 0;
    u32 size;

    ret = switchmgr_lib_get_maximun_queue_size(&size);
    printf("[%s]%d:size = %d\n", __FUNCTION__, __LINE__, size);
    return ret;
}

static int dolocatedqueuesize(int argc, char *argv[], void *p)
{
    u16 AllocatedQueueSize = 0; 
    u32 size;
    u8 queueId = 0;
    int ret = 0;

    if(argc == 2)
    {
        queueId = atoi(argv[1]);
        ret = switchmgr_lib_get_all_located_queue_size(queueId, &size);

        printf("[%s]%d:size = %d\n", __FUNCTION__, __LINE__, size);
        return ret;
    }
    else if(argc > 2)
    {
        queueId = atoi(argv[1]);
        AllocatedQueueSize = atoi(argv[2]);
        ret = switchmgr_lib_set_all_located_queue_size(AllocatedQueueSize,queueId);
        printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    return ret;
}

static int dodroppolicy(int argc, char *argv[], void *p)
{
    u16 greenPacketDropQueueMaxThr = 0;
    u16 greenPacketDropQueueMinThr = 0;
    u16 yellowPacketDropQueueMaxThr = 0;
    u16 yellowPacketDropQueueMinThr = 0;
    u8 greenPacketDropMax_p = 0;
    u8 yellowPacketDropMaxn_p = 0;
    u8 QueueDropW_q = 0;
    u8 DropPrecdenceColourMarking = 0;
    u8 queueId = 0, port = 0;
    int ret = 0;
    ECNT_SWITCHMGR_DROP_POLICY DropPolicy;

    if(argc == 3)
    {
        port = atoi(argv[1]);
        queueId = atoi(argv[2]);
        ret = switchmgr_lib_get_drop_policy(port,queueId,&DropPolicy);

        printf("[%s]%d:DropPolicy->DropPrecdenceColourMarking = %d, DropPolicy->greenPacketDropMax_p = %d, DropPolicy->greenPacketDropQueueMaxThr = %d, DropPolicy->greenPacketDropQueueMinThr = %d, DropPolicy->QueueDropW_q = %d, DropPolicy->yellowPacketDropMaxn_p = %d, DropPolicy->yellowPacketDropQueueMaxThr = %d, DropPolicy->yellowPacketDropQueueMinThr = %d\n", __FUNCTION__, __LINE__, 
            DropPolicy.DropPrecdenceColourMarking, 
            DropPolicy.greenPacketDropMax_p, 
            DropPolicy.greenPacketDropQueueMaxThr, 
            DropPolicy.greenPacketDropQueueMinThr, 
            DropPolicy.QueueDropW_q, 
            DropPolicy.yellowPacketDropMaxn_p,
            DropPolicy.yellowPacketDropQueueMaxThr, 
            DropPolicy.yellowPacketDropQueueMinThr);
    }
    else if(argc > 3)
    {
        port = atoi(argv[1]);
        queueId = atoi(argv[2]);
        DropPolicy.DropPrecdenceColourMarking = atoi(argv[3]);
        DropPolicy.greenPacketDropMax_p = atoi(argv[4]);
        DropPolicy.greenPacketDropQueueMaxThr = atoi(argv[5]);
        DropPolicy.greenPacketDropQueueMinThr = atoi(argv[6]);
        DropPolicy.QueueDropW_q = atoi(argv[7]);
        DropPolicy.yellowPacketDropMaxn_p = atoi(argv[8]);
        DropPolicy.yellowPacketDropQueueMaxThr = atoi(argv[9]);
        DropPolicy.yellowPacketDropQueueMinThr = atoi(argv[10]);
        ret = switchmgr_lib_set_drop_policy(port,queueId,&DropPolicy);
        printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    }
    return ret;
}

static int doQosBase(int argc, char *argv[], void *p)
{
    u8 base=0;
    int ret = 0;

    base=atoi(argv[1]);

    if(base>6)
    {
        printf(HELP_QOS_BASE);
        return -1;
    }

    printf("\r\nswitch qos base : %d. (port-based:0, tag-based:1, dscp-based:2, acl-based:3, arl-based:4, stag-based:5)\n", base);
    ret = switchmgr_lib_set_qos_base(base);
    return ret;
}

static int doQosSetPrioQmap(int argc, char *argv[], void *p)
{
    u8 prio = 0, queue = 0;
    int ret = 0;
    
    prio=atoi(argv[1]);
    queue=atoi(argv[2]);
    
    if ((prio>7) || (queue>7)) {   /* MT7530 total 7 port */
        printf(HELP_QOS_PRIO_QMAP);
        return -1;
    }

    ret = switchmgr_lib_set_qos_prio_qmap(prio, queue);
    return ret;
}

static int doQosSetPrioTagmap(int argc, char *argv[], void *p)
{
    u8 prio = 0, tag = 0;
    int ret = 0;
    
    prio=atoi(argv[1]);
    tag=atoi(argv[2]);

    if ((prio>7) || (tag>7)) {   /* MT7530 total 7 port */
        printf(HELP_QOS_PRIO_QMAP);
        return -1;
    }

    ret = switchmgr_lib_set_qos_prio_tagmap(prio, tag);
    return ret;
}

static int doQosSetPrioDSCPmap(int argc, char *argv[], void *p)
{
    u8 prio = 0, dscp = 0;
    int ret = 0;

    prio=atoi(argv[1]);
    dscp=atoi(argv[2]);

    if ((prio>7) || (dscp>63)){
        printf(HELP_QOS_PRIO_QMAP);
        return -1;
    }

    ret = switchmgr_lib_set_qos_prio_dscpmap(prio, dscp);
    return ret;
}

static int doQosSetPortPrio(int argc, char *argv[], void *p)
{
    u8 port = 0, prio = 0;
    int ret = 0;

    port=atoi(argv[1]);
    prio=atoi(argv[2]);

    if ((port>=7) || (prio>7)) {    /* MT7530 total 7 port */
        printf(HELP_QOS_PORT_PRIO);
        return -1;
    }

    ret = switchmgr_lib_set_qos_port_prio(port, prio);
    return ret;
}

static int doQosSetPortWeight(int argc, char *argv[], void *p)
{
    u8 i = 0;
    int ret = 0;
    ECNT_SWITCHMGR_PORT_WEIGHT qosweight;

    qosweight.port=atoi(argv[1]);

    for (i=0; i<8; i++){
        qosweight.weight[i]=atoi(argv[i+2]);
    }

    if (qosweight.port>6) { /* MT7530 total 7 port */
        printf(HELP_QOS_PORT_WEIGHT);
        return -1;
    }

    for (i=0; i<8; i++){
        if (qosweight.weight[i]<1 || qosweight.weight[i]>16){
            printf(HELP_QOS_PORT_WEIGHT);
            return -1;
        }
    }

    printf("port: %x, q0: %x, q1: %x, q2: %x, q3: %x, q4: %x, q5: %x, q6: %x, q7: %x\n",
        qosweight.port, qosweight.weight[0], qosweight.weight[1], qosweight.weight[2], qosweight.weight[3], qosweight.weight[4], qosweight.weight[5], qosweight.weight[6], qosweight.weight[7]);
                
    ret = switchmgr_lib_set_qos_port_weight(&qosweight);
    return ret;
}

static int doQosSetDSCPPrio(int argc, char *argv[], void *p)
{
    u8 prio = 0, dscp= 0;
    int ret = 0;

    dscp=atoi(argv[1]);
    prio=atoi(argv[2]);

    if ((dscp>63) || (prio>7)) {    /* MT7530 total 7 port */
        printf(HELP_QOS_DSCP_PRIO);
        return -1;
    }

    ret = switchmgr_lib_set_qos_dscp_prio(dscp,prio);
    return ret;
}

static const cmds_t qosCmds[] = 
{
    {"skb_mark",        doQueueMark,            0x12, 3, HELP_QUEUE_MARK},


    {"weight",          doWeight,               0x12, 2, HELP_WEIGHT},
    {"queuepriority",   doqueuepriority,        0x12, 2, "<queueId:0~7> <priority:0~7>"},
    {"maxqueuesize",    domaxqueuesize,         0x12, 1, NULL},
    {"locatedqueuesize",    dolocatedqueuesize, 0x12, 1, "<queueID:0~7> <alllocatedqueuesize:0~15>"},
    {"droppolicy",      dodroppolicy,           0x12, 2, HELP_DROP_POLICY},
    
    {"base",            doQosBase,              0x12, 1, HELP_QOS_BASE},
    {"prio-qmap",       doQosSetPrioQmap,       0x12, 2, HELP_QOS_PRIO_QMAP},
    {"prio-tagmap",     doQosSetPrioTagmap,     0x12, 2, HELP_QOS_PRIO_TAGMAP},
    {"prio-dscpmap",    doQosSetPrioDSCPmap,    0x12, 2, HELP_QOS_PRIO_DSCPMAP},
    {"port-prio",       doQosSetPortPrio,       0x12, 2, HELP_QOS_PORT_PRIO},
    {"port-weight",     doQosSetPortWeight,     0x12, 9, HELP_QOS_PORT_WEIGHT},
    {"dscp-prio",       doQosSetDSCPPrio,       0x12, 2, HELP_QOS_DSCP_PRIO},
    {NULL,              NULL,                   0x10, 0, NULL}
};

/*******************************************************************************/
/*  STATISTICS commands                                                                                                  */
/*******************************************************************************/
static int doStatistic(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    ECNT_SWITCHMGR_PORT_STATISTICS portcnt;

    if(argc < 2)
    {
        ret = switchmgr_lib_clr_all_port_statistics();
        return ret;
    }

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    memset(&portcnt, 0, sizeof(portcnt));
    ret = switchmgr_lib_get_port_statistics(port, &portcnt);
    if(ECNT_SWITCHMGR_SUCCESS == ret)
    {
        /*tx show*/
        printf("TxPktsCnt:              %-10u    TxUniPktsCnt:          %-10u\n", portcnt.TxPktsCnt, portcnt.TxUniPktsCnt);
        printf("TxBroadPktsCnt:         %-10u    TxMultiPktsCnt:        %-10u\n", portcnt.TxBroadPktsCnt, portcnt.TxMultiPktsCnt);
        printf("TxNonUniPktsCnt:        %-10u    TxBytesCnt:            %-20llu\n", portcnt.TxNonUniPktsCnt, (((unsigned long long)portcnt.TxBytesCnt_Hi)<<32) + (portcnt.TxBytesCnt_Lo));
        
        printf("TxUnderSizePktsCnt:     %-10u    Tx64BytePktsCnt:       %-10u\n", portcnt.TxUnderSizePktsCnt, portcnt.Tx64BytePktsCnt);
        printf("Tx65_127BytePktsCnt:    %-10u    Tx128_255BytePktsCnt:  %-10u\n", portcnt.Tx65_127BytePktsCnt, portcnt.Tx128_255BytePktsCnt);
        printf("Tx256_511BytePktsCnt:   %-10u    Tx512_1023BytePktsCnt: %-10u\n", portcnt.Tx256_511BytePktsCnt, portcnt.Tx512_1023BytePktsCnt);
        printf("Tx1024_MAXBytePktsCnt:  %-10u    TxOverSizePktsCnt:     %-10u\n", portcnt.Tx1024_MAXBytePktsCnt, portcnt.TxOverSizePktsCnt);
		printf("Tx1024_1518BytePktsCnt: %-10u    Tx1519_MAXBytePktsCnt: %-10u\n", portcnt.Tx1024_1518BytePktsCnt, portcnt.Tx1519_MAXBytePktsCnt);

        printf("TxDropFramesCnt:        %-10u    TxPauseFramesCnt:      %-10u\n", portcnt.TxDropFramesCnt, portcnt.TxPauseFramesCnt);
        printf("TxCRCFramesCnt:         %-10u    TxSingleCollsionEvent: %-10u\n", portcnt.TxCRCFramesCnt, portcnt.TxSingleCollsionEvent);
        printf("TxMultiCollsionEvent:   %-10u    TxLateCollsionEvent:   %-10u\n", portcnt.TxMultiCollsionEvent, portcnt.TxLateCollisionEvent);
        printf("TxExcessCollisionEvent: %-10u    TxDeferredEvent:       %-10u\n", portcnt.TxExcessCollisionEvent, portcnt.TxDeferredEvent);
        printf("TxOverFlowFrameCnt:     %-10u    TxInternalMacErrorCnt: %-10u\n", portcnt.TxOverFlowFrameCnt, portcnt.TxInternalMacErrorCnt);
        printf("TXCarrierSenseErrorCnt: %-10u\n", portcnt.TXCarrierSenseErrorCnt);

        /*rx show*/
        printf("\n");
        printf("RxPktsCnt:              %-10u    RxUniPktsCnt:          %-10u\n", portcnt.RxPktsCnt, portcnt.RxUniPktsCnt);
        printf("RxBroadPktsCnt:         %-10u    RxMultiPktsCnt:        %-10u\n", portcnt.RxBroadPktsCnt, portcnt.RxMultiPktsCnt);
        printf("RxNonUniPktsCnt:        %-10u    RxBytesCnt:            %-20llu\n", portcnt.RxNonUniPktsCnt, (((unsigned long long)portcnt.RxBytesCnt_Hi)<<32) + (portcnt.RxBytesCnt_Lo));

        printf("RxUnderSizePktsCnt:     %-10u    Rx64BytePktsCnt:       %-10u\n", portcnt.RxUnderSizePktsCnt, portcnt.Rx64BytePktsCnt);
        printf("Rx65_127BytePktsCnt:    %-10u    Rx128_255BytePktsCnt:  %-10u\n", portcnt.Rx65_127BytePktsCnt, portcnt.Rx128_255BytePktsCnt);
        printf("Rx256_511BytePktsCnt:   %-10u    Rx512_1023BytePktsCnt: %-10u\n", portcnt.Rx256_511BytePktsCnt, portcnt.Rx512_1023BytePktsCnt);
        printf("Rx1024_1518BytePktsCnt: %-10u    RxOverSizePktsCnt:     %-10u\n", portcnt.Rx1024_1518BytePktsCnt, portcnt.RxOverSizePktsCnt);

        printf("RxDropFramesCnt:        %-10u    RxPauseFramesCnt:      %-10u\n", portcnt.RxDropFramesCnt, portcnt.RxPauseFramesCnt);
        printf("RxCRC(FCS)FramesCnt:    %-10u    RxOverFlowFrameCnt:    %-10u\n", portcnt.RxCRCFramesCnt, portcnt.RxOverFlowFrameCnt);
        printf("RxFragmentErrorCnt:     %-10u    RxJabberErrorCnt:      %-10u\n", portcnt.RxFragmentErrorCnt, portcnt.RxJabberErrorCnt);
        printf("RxAlignmentErrorCnt:    %-10u    Rx1519_MAXBytePktsCnt: %-10u\n", portcnt.RxAlignmentErrorCnt, portcnt.Rx1519_MAXBytePktsCnt);
    }
    
    return ret;
}

static int doGetMIBCounter(int argc, char *argv[], void *p)
{
    u32 MibCntType = 0;
    u32 cnt = 0;
    u8 port = 0;
    int ret = 0;

    port=atoi(argv[1]);
    MibCntType=atoi(argv[2]);
    ret = switchmgr_lib_get_port_mib_count(port, MibCntType, &cnt);
    
    printf("[%s]%d:MIB Counter is %d\n", __FUNCTION__, __LINE__, cnt);
    return ret;

}

static int doEnableMIBCounter(int argc, char *argv[], void *p)
{
    int ret = 0;
    u32 enable = 0;
    enable = atoi(argv[1]);

    ret = switchmgr_lib_set_enable_mibcounter(enable);
    return ret;
}

static int doClearMIBCounter(int argc, char *argv[], void *p)
{
    int ret = 0;
	u8 port = 0;

	if(argc < 2)
	{
	   ret = switchmgr_lib_set_clear_mibcounter();
	   return ret;
	}
	
	port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
	ret = switchmgr_lib_clr_single_port_statistics(port);
	
    return ret;
}

static int dogetportrxbyte(int argc, char *argv[], void *p)
{
    u8 port = 0;
    int ret = 0;
    int RxByte_H = 0, RxByte_L = 0;

    port = atoi(argv[1]);
    ret = switchmgr_lib_get_port_rx_byte(port,&RxByte_H,&RxByte_L);
    printf("[%s]%d: ret = %d,RxByte_L = %d,RxByte_H = %d\n", __FUNCTION__, __LINE__, ret, RxByte_L, RxByte_H);
    return ret;
}

static int dogetporttxbyte(int argc, char *argv[], void *p)
{
    u8 port = 0;
    int ret = 0;
    int TxByte_H = 0, TxByte_L = 0;

    port = atoi(argv[1]);
    ret = switchmgr_lib_get_port_tx_byte(port,&TxByte_H,&TxByte_L);
    printf("[%s]%d: ret = %d,TxByte_L = %d,TxByte_H = %d\n", __FUNCTION__, __LINE__, ret, TxByte_L, TxByte_H);
    return ret;
}

static int doethcongestion(int argc, char *argv[], void *p)
{
    u8 port = 0, congestion;
    int ret = 0;

    port = atoi(argv[1]);

    ret = switchmgr_lib_get_eth_port_congestion(port, &congestion);
    printf("[%s]%d:congestion = %d\n", __FUNCTION__, __LINE__, congestion);
    return ret;
}

static int doSetRxOtcMode(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 mode = 0;
    mode = atoi(argv[1]);

    ret = switchmgr_lib_set_rx_otcmode(mode);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

static int doSetTxOtcMode(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 mode = 0;
    mode = atoi(argv[1]);

    ret = switchmgr_lib_set_tx_otcmode(mode);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

static int doSetBxPktMode(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 mode = 0;
    mode = atoi(argv[1]);
    
    ret = switchmgr_lib_set_bx_pktmode(mode);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

#define HELP_STATS_PORT                 "switchmgr stats port <port:0~6>\n"


static const cmds_t statsCmds[] = 
{
    {"port",            doStatistic,            0x12, 1, HELP_STATS_PORT},
    {"clear",           doStatistic,            0x12, 0, NULL},

    {"mib", doGetMIBCounter,       0x12,   2,  "<port> <MibCntType>"},
    {"enable", doEnableMIBCounter,      0x12,   1,  "<enable>"},
    {"clr",  doClearMIBCounter,     0x12,   0,  NULL},
    {"portclr",  doClearMIBCounter,     0x12,   0,  NULL},
    {"port rx", dogetportrxbyte,    0x12,   1,  "<port:0~6>"},
    {"port tx", dogetporttxbyte,    0x12,   1,  "<port:0~6>"},
    {"congestion",  doethcongestion,    0x12,   1,  "<port:0~6>"},
    {"rxmode",   doSetRxOtcMode,        0x12,   1,  "<mode>"},
    {"txmode",   doSetTxOtcMode,        0x12,   1,  "<mode>"},
    {"bxpktmode",    doSetBxPktMode,        0x12,   1,  "<mode>"}, //TODO: remove
    {"mib_counter", doEnableMIBCounter,     0x12,   1,  "<enable>"},
    {"clr_mib_counter",  doClearMIBCounter,     0x12,   0,  NULL},
    {NULL,          NULL,           0x10,   0,  NULL}
};

/*******************************************************************************/
/*  STORMCTL commands                                                                                                     */
/*******************************************************************************/
static int doStormCtrlPort(int argc, char *argv[], void *p)
{
    u16 port = 0;
    
    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_storm_ctrl_port(port);
}

static int doStormCtrlUnit(int argc, char *argv[], void *p)
{
    u8 unit = 0;
    
    unit = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_storm_ctrl_unit(unit);
}

static int doStormCtrlType(int argc, char *argv[], void *p)
{
    u8 type = 0;
    
    type = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_storm_ctrl_type(type);  
}

static int doStormCtrlMode(int argc, char *argv[], void *p)
{
    u8 mode = 0;

    mode = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_storm_ctrl_mode(mode);
}

static int doStormCtrlPeriod(int argc, char *argv[], void *p)
{
    u8 period = 0;

    period = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_storm_ctrl_period(period);
}

#define HELP_STORMCTRL_PORT   "switchmgr stormctrl port <0xff>\n"\
                              "(default: 0x7f, bit0~6 means port0~6)\n"
#define HELP_STORMCTRL_UNIT   "switchmgr stormctrl unit <0|1|2|3>\n"\
                              "(0: 64, 1: 256, 2: 1K, 3: 4K)\n"
#define HELP_STORMCTRL_TYPE   "switchmgr stormctrl type <0xf>\n"\
                              "(bit2: broadcast, bit1: multicast, bit0: unknow MAC unicast)\n"
#define HELP_STORMCTRL_MODE   "switchmgr stormctrl mode <0|1>\n"\
                              "(0: packet-base, 1: rate-base)\n"
#define HELP_STORMCTRL_PERIOD "switchmgr stormctrl period <0|1>\n"\
                              "(0: 1 second, 1: 125us)\n"

static const cmds_t stormctrlCmds[] = 
{
    {"port",            doStormCtrlPort,        0x12, 1, HELP_STORMCTRL_PORT},
    {"unit",            doStormCtrlUnit,        0x12, 1, HELP_STORMCTRL_UNIT},
    {"type",            doStormCtrlType,        0x12, 1, HELP_STORMCTRL_TYPE},
    {"mode",            doStormCtrlMode,        0x12, 1, HELP_STORMCTRL_MODE},
    {"period",          doStormCtrlPeriod,      0x12, 1, HELP_STORMCTRL_PERIOD},
    {NULL,              NULL,                   0x10, 0, NULL}
};

/*******************************************************************************/
/*  TRTCM commands                                                                                                          */
/*******************************************************************************/
static int doDsTrtcmActive(int argc, char *argv[], void *p)
{
    u8 enable = 0;

    enable = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_dstrtcm_enable(enable);
}

static int doDsTrtcmRule(int argc, char *argv[], void *p)
{
    int ret = 0, i;
    ECNT_SWITCHMGR_TRTCM trtcm;
    ECNT_SWITCHMGR_TRTCM_ACL trtcm_acl = {0};

    if (argc < 6)
    {
        ret = switchmgr_lib_get_dstrtcm_rule(&trtcm_acl);
        if(ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Downstream Trtcm:%s\n", trtcm_acl.enable ?"Enable": "Disable");
            printf("      Id       CBS       PBS       CIR       PIR\n");
            for (i = 0; i < 32; i++)
            {
                if ((trtcm_acl.trtcm_acl_table[i].trtcmId != 0xff) && (trtcm_acl.trtcm_acl_table[i].trtcmId != 0x0))
                {
                    printf("%8d%10d%10d%10d%10d\n", trtcm_acl.trtcm_acl_table[i].trtcmId, trtcm_acl.trtcm_acl_table[i].CBS, 
                            trtcm_acl.trtcm_acl_table[i].PBS, trtcm_acl.trtcm_acl_table[i].CIR, trtcm_acl.trtcm_acl_table[i].PIR);
                }
            }
        }
    }
    else
    {
        trtcm.trtcmId = strtoul((const char*)(argv[1]), (char **)NULL, 10);
        trtcm.CBS     = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        trtcm.PBS     = strtoul((const char*)(argv[3]), (char **)NULL, 10);
        trtcm.CIR     = strtoul((const char*)(argv[4]), (char **)NULL, 10);
        trtcm.PIR     = strtoul((const char*)(argv[5]), (char **)NULL, 10);       
        ret = switchmgr_lib_set_dstrtcm_rule(&trtcm);
    }
    
    return ret;
}


#define HELP_TRTCM_ACTIVE "switchmgr dstrtcm active <0|1>\n"

#define HELP_TRTCM_RULE   "switchmgr dstrtcm rule [<id:1-31> <CBS> <PBS> <CIR> <PIR>]\n"

static const cmds_t dsTrtcmCmds[] = {
    {"active",      doDsTrtcmActive,        0x12,   1,  HELP_TRTCM_ACTIVE},
    {"rule",        doDsTrtcmRule,          0x12,   5,  HELP_TRTCM_RULE},
    {NULL,          NULL,           0x10,   0,  NULL},
};

/*******************************************************************************/
/*  TRAFFICSEL commands                                                                                                   */
/*******************************************************************************/
static int doTrafficSet(int argc, char *argv[], void *p)
{
    u8 wan_type, interface;
    
    if (strcmp(argv[1], "wan") == 0)
    {
        wan_type = 0;
    }
    else if (strcmp(argv[1], "wifi0") == 0)
    {
        wan_type = 1;
    }
    else if (strcmp(argv[1], "wifi1") == 0)
    {
        wan_type = 2;
    }
    else
    {
        printf("traffic type error\n");
        return 0;
    }
    
    if (strcmp(argv[2], "trgmii") == 0)
    {
        interface = 0;
    }
    else if(strcmp(argv[2], "rgmii") == 0)
    {
        interface = 1;
    }
    else
    {
        printf("interface error\n");
        return 0;
    }
    
    return switchmgr_lib_set_rgmii_trafficsetting(wan_type, interface);
}

static int doTrafficShow(int argc, char *argv[], void *p)
{
    return switchmgr_lib_set_rgmii_display();
}

static int doRgmiiSetting(int argc, char *argv[], void *p)
{
    int ret = 0;
    
    if(strcmp(argv[1],"enable")==0)
    {
        ret = switchmgr_lib_set_rgmii_setting(1);
    }
    else if(strcmp(argv[1],"disable")==0)
    {
        ret = switchmgr_lib_set_rgmii_setting(0);
    }
    return ret;
}

#define HELP_TRAFFIC_SET    "switchmgr traffic set wan|wifi0|wifi1 trgmii|rgmii\n"\
                            "wifi0:2.4G, wifi1:5G\n"
#define HELP_RGMII_SETTING  "switchmgr traffic rgmii <enable|disable>\n"

static const cmds_t TrafficCmds[] = 
{
    {"set",             doTrafficSet,           0x12, 2, HELP_TRAFFIC_SET},
    {"show",            doTrafficShow,          0x12, 0, NULL},
    {"rgmii",           doRgmiiSetting,         0x12, 1, HELP_RGMII_SETTING},
    {NULL,              NULL,                   0x10, 0, NULL},
};

/*******************************************************************************/
/*  RATECTL commands                                                                                                       */
/*******************************************************************************/
static int doRateLimitEnable(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, enable;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if (argc > 2)
    {
        enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_rate_limit_enable(port, enable);
    }
    else
    {
        ret = switchmgr_lib_get_port_rate_limit_enable(port, &enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d ratelimit : %s\n", port, enable ? "enable" : "disable");
        }
    }
    
    return ret;
}

static int doratelmting(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    u32 cir = 0, pir = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if (argc > 2)
    {
        cir = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        pir = strtoul((const char*)(argv[3]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_rate_limiting(port, cir, pir);
    }
    else
    {
        ret = switchmgr_lib_get_port_rate_limiting(port, &cir, &pir);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d : cir %u, pir %u\n", port, cir, pir);
        }
    }

    return ret;
}

static int doPolicingEnable(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, enable;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if (argc > 2)
    {
        enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_policing_enable(port,enable);
    }
    else
    {
        ret = switchmgr_lib_get_port_policing_enable(port, &enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d policing : %s\n", port, enable ? "enable" : "disable");
        }
    }
    
    return ret;
}

static int doPolicing(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0, enable;
    u32 cir = 0, cbs = 0, ebs = 0;

    port = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    if (argc > 2)
    {
        cir = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        cbs = strtoul((const char*)(argv[3]), (char **)NULL, 10);
        ebs = strtoul((const char*)(argv[4]), (char **)NULL, 10);
        ret = switchmgr_lib_set_port_policing(port, cir, cbs, ebs);
    }
    else
    {
        ret = switchmgr_lib_get_port_policing(port, &cir, &cbs, &ebs);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("Port%d : cir %u, cbs %u, ebs %u\n", port, cir, cbs, ebs);
        }
    }

    return ret;
}

static int dotrafficdescriptor(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    ECNT_SWITCHMGR_TRAFFIC_DESCRIPTOR trafficDescriptor;

    if(argc == 3)
    {
        port = atoi(argv[1]);
        trafficDescriptor.direction = atoi(argv[2]);
        ret = switchmgr_lib_get_traffic_descriptor(port,&trafficDescriptor);

        printf("[%s]%d:ret = %d,trafficDescriptor_p->CIR = %d,trafficDescriptor_p->PIR = %d,trafficDescriptor_p->CBS = %d,trafficDescriptor_p->PBS = %d,trafficDescriptor_p->colourMode = %d,trafficDescriptor_p->ingressColourMarking = %d,trafficDescriptor_p->engressColourMarking = %d,trafficDescriptor_p->meterType = %d,trafficDescriptor_p->direction = %d\n", __FUNCTION__, __LINE__, ret,  
            trafficDescriptor.CIR,
            trafficDescriptor.PIR,
            trafficDescriptor.CBS,
            trafficDescriptor.PBS,
            trafficDescriptor.colourMode,
            trafficDescriptor.ingressColourMarking,
            trafficDescriptor.engressColourMarking,
            trafficDescriptor.meterType,
            trafficDescriptor.direction);
    }
    else if(argc > 3)
    {
        port = atoi(argv[1]);
        trafficDescriptor.CIR = atoi(argv[2]);
        trafficDescriptor.PIR = atoi(argv[3]);
        trafficDescriptor.CBS = atoi(argv[4]);
        trafficDescriptor.PBS = atoi(argv[5]);
        trafficDescriptor.colourMode = atoi(argv[6]);
        trafficDescriptor.ingressColourMarking = atoi(argv[7]);
        trafficDescriptor.engressColourMarking = atoi(argv[8]);
        trafficDescriptor.meterType = atoi(argv[9]);
        trafficDescriptor.direction = atoi(argv[10]);
        ret = switchmgr_lib_set_traffic_descriptor(port,&trafficDescriptor);
        printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    }
    return ret;
}

static int docleandescriptor(int argc, char *argv[], void *p)
{
    u8 port = 0, direction = 0;
    int ret = 0;

    port=atoi(argv[1]);
    direction=atoi(argv[2]);

    ret = switchmgr_lib_set_clean_traffic_descriptor(port,direction);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

#define HELP_DESCRIPTOR "get descriptor <port> <direction>\n"\
                        "set descriptor <port> <CIR> <PIR> <CBS> <PBS> <colourMode> <ingressColourMarking> <engressColourMarking> <meterType> <deriction>"

static const cmds_t trafficdscpCmds[] = 
{
    {"set",             dotrafficdescriptor,    0x12, 2, HELP_DESCRIPTOR},   
    {"clr",             docleandescriptor,      0x12, 2, "<port:0~6> <direction:0|1>"},
    {NULL,              NULL,                   0x10, 0, NULL},
};

static int dotrafficdscp(int argc, char *argv[], void *p)
{
    return subcmd(trafficdscpCmds, argc, argv, p);
}

#define HELP_RATE_LIMIT_ENABLE  "switchmgr ratectl shaping <port> [<enable:1|disable:0>]\n"

#define HELP_PORT_RATE_LIMITING "switchmgr ratectl shapingrate <port> [<cir> <pir>]\n"

#define HELP_POLICING_ENABLE    "switchmgr ratectl policing <port> [<enable:1|disable:0>]\n"

#define HELP_POLICING           "switchmgr ratectl policingrate <port> [<cir> <chs> <ebs>]\n"
                            
static const cmds_t ratectlCmds[] = 
{
    {"portenable",      doRateLimitEnable,      0x12, 1, HELP_RATE_LIMIT_ENABLE},
    {"ratelmting",      doratelmting,           0x12, 1, HELP_PORT_RATE_LIMITING},
    {"policing",        doPolicing,             0x12, 1, HELP_POLICING},
    {"policingen",      doPolicingEnable,       0x12, 1, HELP_POLICING_ENABLE},
    {"traffic_dscp",    dotrafficdscp,          0x12, 0, NULL},
    {NULL,              NULL,                   0x10, 0, NULL},
};

/*******************************************************************************/
/*  DSP5 commands                                                                                                            */
/*******************************************************************************/
//TODO: DSP5 = ds port5, need to remove all following API/CMD
static int dodsp5ratelimitenable(int argc, char *argv[], void *p)
{
    u8 ratectl_enable = 0;
    int ret = 0;

    ratectl_enable = atoi(argv[1]);
    ret = switchmgr_lib_set_dsp5_rate_limit_enable(ratectl_enable);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

static int dodsp5ratelimit(int argc, char *argv[], void *p)
{
    u32 rate = 0;
    int ret = 0;

    rate = atoi(argv[1]);
    ret = switchmgr_lib_set_dsp5_rate_limit(rate);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

static int dodsp5qos(int argc, char *argv[], void *p)
{
    u8 qos_enable = 0;
    int ret = 0;

    qos_enable = atoi(argv[1]);
    ret = switchmgr_lib_set_dsp5_qos(qos_enable);
    printf("[%s]%d:ret = %d\n", __FUNCTION__, __LINE__, ret);
    return ret;
}

static const cmds_t dsp5Cmds[] = 
{
    {"dsp5rl",          dodsp5ratelimit,        0x12, 1, "<rate>"},
    {"dsp5rlen",        dodsp5ratelimitenable,  0x12, 1, "<enable:0|1>"},
    {"dsp5qos",         dodsp5qos,              0x12, 1, "<enable:0|1>"},
    {NULL,              NULL,                   0x10, 0, NULL},
};

/*******************************************************************************/
/*  ACL commands                                                                                                              */
/*******************************************************************************/
#define HELP_ACL_SETPORTEN      "acl enable <port> <port_enable:0|1>\n"
#define HELP_ACL_SETMULTIHIT    "acl multi-hit <acl_multi:0|1>\n"
#define HELP_ACL_ACL_TBL_ADD    "arl acltbl-add <tbl_idx:0~63> <vawd1> <vawd2>\n"
#define HELP_ACL_MASK_TBL_ADD   "arl masktbl-add <tbl_idx:0~31> <vawd1> <vawd2>\n"
#define HELP_ACL_RULE_TBL_ADD   "arl ruletbl-add <tbl_idx:0~31> <vawd1> <vawd2>\n"
#define HELP_ACL_RATE_TBL_ADD   "arl ratetbl-add <tbl_idx:0~31> <vawd1> <vawd2>\n"

#define HELP_ACL_MAC_RATE       "switchmgr acl macrate <enable:1|disable:0> (<mac> <downrate> <uprate>)\n"

static int doAclSetPortEn(int argc, char *argv[], void *p)
{
    u8 acl_port = 0, acl_en = 0;
    int ret = 0;

    acl_port = atoi(argv[1]);
    acl_en = atoi(argv[2]);

    printf("acl_port:%d, acl_en:%d\n", acl_port, acl_en);

    ret = switchmgr_lib_set_acl_port_enable(acl_port,acl_en);

    return ret;
}

static int doAclSetMultiHit(int argc, char *argv[], void *p)
{
    u8 acl_multi = 0;
    int ret = 0;

    acl_multi = atoi(argv[1]);

    printf("acl_multi:%d\n", acl_multi);

    /*Check the input parameters is right or not.*/
    if (acl_multi > 1){
        printf(HELP_ACL_SETMULTIHIT);
        return -1;
    }
        
    ret = switchmgr_lib_set_acl_multi_hit(acl_multi);

    return ret;
}

static int doAclTblClr(int argc, char *argv[], void *p)
{
    return switchmgr_lib_set_acl_tbl_clr();
}

static int doAclTblDisp(int argc, char *argv[], void *p)
{
    u8 ext_switch = 0;
    int ret = 0;

    ext_switch = atoi(argv[1]);

    ret = switchmgr_lib_set_acl_tbl_disp(ext_switch);
    return ret;
}

static int doAclAclTblAdd(int argc, char *argv[], void *p)
{
    u32 vawd1, vawd2;
    u8 tbl_idx;
    int ret = 0;


    tbl_idx = atoi(argv[1]);
    vawd1 = strtoul(argv[2], (char **)NULL, 16);
    vawd2 = strtoul(argv[3], (char **)NULL, 16);

    printf("tbl_idx:%d\n", tbl_idx);

    if (tbl_idx > 63) { /*Check the input parameters is right or not.*/
        printf(HELP_ACL_ACL_TBL_ADD);
        return -1;
    }

    ret = switchmgr_lib_set_acl_tbl_add(tbl_idx,vawd1,vawd2);
    return ret;
}

static int doAclMaskTblAdd(int argc, char *argv[], void *p)
{
    u32 vawd1, vawd2;
    u8 tbl_idx;
    int ret = 0;

    tbl_idx = atoi(argv[1]);
    vawd1 = strtoul(argv[2], (char **)NULL, 16);
    vawd2 = strtoul(argv[3], (char **)NULL, 16);

    printf("tbl_idx:%d\n", tbl_idx);

    if (tbl_idx > 31) { /*Check the input parameters is right or not.*/
        printf(HELP_ACL_MASK_TBL_ADD);
        return -1;
    }

    ret = switchmgr_lib_set_acl_mask_tbl_add(tbl_idx,vawd1,vawd2);
    return ret;
}

static int doAclRuleTblAdd(int argc, char *argv[], void *p)
{
    u32 vawd1, vawd2;
    u8 tbl_idx;
    int ret = 0;

    tbl_idx = atoi(argv[1]);
    vawd1 = strtoul(argv[2], (char **)NULL, 16);
    vawd2 = strtoul(argv[3], (char **)NULL, 16);

    printf("tbl_idx:%d\n", tbl_idx);

    if (tbl_idx > 31) { /*Check the input parameters is right or not.*/
        printf(HELP_ACL_RULE_TBL_ADD);
        return -1;
    }

    ret = switchmgr_lib_set_acl_rule_tbl_add(tbl_idx,vawd1,vawd2);
    return ret;
}

static int doAclRateTblAdd(int argc, char *argv[], void *p)
{
    u32 vawd1, vawd2;
    u8 tbl_idx;
    int ret = 0;

    tbl_idx = atoi(argv[1]);
    vawd1 = strtoul(argv[2], (char **)NULL, 16);
    vawd2 = strtoul(argv[3], (char **)NULL, 16);

    printf("tbl_idx:%d\n", tbl_idx);

    if (tbl_idx > 31) { /*Check the input parameters is right or not.*/
        printf(HELP_ACL_RATE_TBL_ADD);
        return -1;
    }

    ret = switchmgr_lib_set_acl_rate_tbl_add(tbl_idx,vawd1,vawd2);
    return ret;
}

#define HELP_ACL_UP_NOTUNICAT      "switchmgr acl upstream notunicastrate <enable:1|disable:0> [<rate>]\n"

#define HELP_ACL_UP_VLAN           "switchmgr acl upstream vlanrate <index> <enable:1|disable:0> [<vlan> <rate>]\n"

static int doAclUpStreamNotUnicastRate(int argc, char *argv[], void *p)
{
    u8 enable = 0;
    u32 rate = 0;

    enable = strtoul(argv[1], (char **)NULL, 10);
    if (enable)
    {
        rate = strtoul(argv[2], (char **)NULL, 10);
    }

    return switchmgr_lib_set_acl_upstream_notunicast_ratelimit(enable, rate);
}

static int doAclUpStreamVlanRate(int argc, char *argv[], void *p)
{
    u8 enable = 0, index = 0;
    u32 vlan = 0, rate = 0;

    index = strtoul(argv[1], (char **)NULL, 10);
    enable = strtoul(argv[2], (char **)NULL, 10);
    if (enable)
    {
        vlan = strtoul(argv[3], (char **)NULL, 10);
        rate = strtoul(argv[4], (char **)NULL, 10);
    }

    return switchmgr_lib_set_acl_upstream_vlan_ratelimit(index, enable, vlan, rate);
}

static const cmds_t aclUpStreamCmds[] = 
{
    {"notunicastrate",  doAclUpStreamNotUnicastRate,    0x12, 1, HELP_ACL_UP_NOTUNICAT},
    {"vlanrate",        doAclUpStreamVlanRate,          0x12, 2, HELP_ACL_UP_VLAN},
    {NULL,              NULL,                           0x10, 0, NULL},
};

static int doAclUpStream(int argc, char *argv[], void *p)
{
    return subcmd(aclUpStreamCmds, argc, argv, p);
}

static int doAclMacRate(int argc, char *argv[], void *p)
{
    int i, j, k, index = 0;
    char temp_str[3] = {0};
    ECNT_SWITCHMGR_DEV_SPEED dev_mac_acl;

    memset(&dev_mac_acl, 0, sizeof(ECNT_SWITCHMGR_DEV_SPEED));

    dev_mac_acl.enable = strtoul(argv[1], (char **)NULL, 10);
    if (dev_mac_acl.enable)
    {
        for (i = 2; i < argc; i++)
        {
            for (j = 0, k = 0; j < 6; j++)
            {
                temp_str[0] = ((const char*)(argv[i]))[k++];
                temp_str[1] = ((const char*)(argv[i]))[k++];
                k++;
                dev_mac_acl.trtcm_acl_mac_table[index].dev_mac[j] = strtoul(temp_str, (char **)NULL, 16);
            }
            i++;
            dev_mac_acl.trtcm_acl_mac_table[index].ds_queue_speed = strtoul(argv[i++], (char **)NULL, 10);
            dev_mac_acl.trtcm_acl_mac_table[index].us_queue_speed = strtoul(argv[i], (char **)NULL, 10);
            index++;
        }
    }

    return switchmgr_lib_set_mac_match_acl(&dev_mac_acl);
}

static const cmds_t aclCmds[] = 
{
    {"enable",          doAclSetPortEn,         0x12, 2, HELP_ACL_SETPORTEN},
    {"multi-hit",       doAclSetMultiHit,       0x12, 1, HELP_ACL_SETMULTIHIT},
    {"tbl-clr",         doAclTblClr,            0x12, 0, "acl tbl-clr : to clear all acl; mask, rule ctrl, rate ctrl; and trTCM table entries"},
    {"tbl-disp",        doAclTblDisp,           0x12, 1, "acl tbl-disp ext_switch: to display all acl; mask, rule ctrl, rate ctrl; and trTCM table entries"},
    {"acltbl-add",      doAclAclTblAdd,         0x12, 3, HELP_ACL_ACL_TBL_ADD},
    {"masktbl-add",     doAclMaskTblAdd,        0x12, 3, HELP_ACL_MASK_TBL_ADD},
    {"ruletbl-add",     doAclRuleTblAdd,        0x12, 3, HELP_ACL_RULE_TBL_ADD},
    {"ratetbl-add",     doAclRateTblAdd,        0x12, 3, HELP_ACL_RATE_TBL_ADD},
    {"upstream",        doAclUpStream,          0x12, 0, NULL},
    {"macrate",         doAclMacRate,           0x12, 1, HELP_ACL_MAC_RATE},
    {NULL,              NULL,                   0x10, 0, NULL},
};

/*******************************************************************************/
/*  REG commands                                                                                                              */
/*******************************************************************************/
static int doGSWRead(int argc, char *argv[], void *p)
{
    int ret = 0;
    u32 offset = 0, value = 0;

    offset = strtoul((const char*)(argv[1]), (char **)NULL, 16);

    ret = switchmgr_lib_get_gswr(offset, &value);
    if(ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("GSW[%x]: %x\n", offset, value);
    }
    return ret;
}

static int doGSWWrite(int argc, char *argv[], void *p)
{
    int ret = 0;
    u32 offset = 0, value = 0;
    
    offset = strtoul((const char*)(argv[1]), (char **)NULL, 16);
    value  = strtoul((const char*)(argv[2]), (char **)NULL, 16);
    
    ret = switchmgr_lib_set_gsww(offset,value);
    return ret;
}

static int doGSWRead2(int argc, char *argv[], void *p)
{
    int ret = 0;
    u32 ext_switch = 0, offset = 0, value = 0;

    ext_switch = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    offset     = strtoul((const char*)(argv[2]), (char **)NULL, 16);

    ret = switchmgr_lib_get_gswr2(ext_switch, offset, &value);
    if(ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("%sGSW[%x]: %x\n", ext_switch ? "EXT " : "", offset, value);
    }
    return ret;
}

static int doGSWWrite2(int argc, char *argv[], void *p)
{
    int ret = 0;
    u32 ext_switch = 0, offset = 0, value = 0;

    ext_switch = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    offset     = strtoul((const char*)(argv[2]), (char **)NULL, 16);
    value      = strtoul((const char*)(argv[3]), (char **)NULL, 16);

    ret = switchmgr_lib_set_gsww2(ext_switch, offset, value);
    return ret;
}

static int doTCMiiRead(int argc, char *argv[], void *p)
{
    int phyaddr = 0, reg = 0, ret = 0;
    u32 value = 0;

    phyaddr = atoi(argv[1]);
    reg     = atoi(argv[2]);
    
    ret = switchmgr_lib_get_mii(phyaddr, reg, &value);
    if(ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("*PhyAddr=%d Reg=%d value:%x\r\n", phyaddr , reg , value);
    }
    return ret;
}

static int doTCMiiWrite(int argc, char *argv[], void *p)
{
    int ret = 0, phyaddr = 0, reg = 0;
    u32 value=0;

    
    phyaddr = atoi(argv[1]);
    reg     = atoi(argv[2]);
    value   = strtoul((const char*)(argv[3]), (char **)NULL, 16);
        
    ret = switchmgr_lib_set_mii(phyaddr, reg, value);
    if(ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("*PhyAddr=%d Reg=%d value:%x\r\n", phyaddr , reg , value);
    }
    return ret;
}

static int doTCMiiRead2(int argc, char *argv[], void *p)
{
    int ext_switch = 0, phyaddr = 0, reg = 0, ret = 0;
    u32 value = 0;

    ext_switch = atoi(argv[1]);
    phyaddr    = atoi(argv[2]);
    reg        = atoi(argv[3]);

    ret = switchmgr_lib_get_mii2(ext_switch, phyaddr, reg, &value);
    if(ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("*PhyAddr=%x Reg=%x value:%x (ext_switch:%d)\r\n", phyaddr, reg, value, ext_switch);
    }
    return 0;
}

static int doTCMiiWrite2(int argc, char *argv[], void *p)
{
    int ret = 0;
    int ext_switch = 0, phyaddr = 0, reg = 0;
    u32 value = 0;

    ext_switch = atoi(argv[1]);
    phyaddr    = atoi(argv[2]);
    reg        = atoi(argv[3]);
    value      = strtoul((const char*)(argv[4]), (char **)NULL, 16);

    ret = switchmgr_lib_set_mii2(ext_switch, phyaddr, reg, value);
    
    if(ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("*PhyAddr=%x Reg=%x value:%x (ext_switch:%d)\r\n", phyaddr, reg, value, ext_switch);
    }
    return ret;
}

static const cmds_t regCmds[] = 
{
    {"gswr",            doGSWRead,              0x12, 1, "gswr <reg>"},
    {"gsww",            doGSWWrite,             0x12, 2, "gsww <reg> <value>"},
    {"gswr2",           doGSWRead2,             0x12, 2, "gswr2 <ext_switch:0|1> <reg>"},
    {"gsww2",           doGSWWrite2,            0x12, 3, "gsww2 <ext_switch:0|1> <reg> <value>"},
    {"miir",            doTCMiiRead,            0x12, 2, "miir <phyAddr> <reg>"},
    {"miiw",            doTCMiiWrite,           0x12, 3, "miiw <phyAddr> <reg> <value>"},
    {"miir2",           doTCMiiRead2,           0x12, 3, "miir2 <ext_switch:0|1> <phyAddr> <reg>"},
    {"miiw2",           doTCMiiWrite2,          0x12, 4, "miiw2 <ext_switch:0|1> <phyAddr> <reg> <value>"},
    {NULL,              NULL,                   0x10, 0, NULL}
};

/*******************************************************************************/
/*  SWITCH commands                                                                                                        */
/*******************************************************************************/
static int doSwitchPortMap(int argc, char *argv[], void *p)
{
    int i, wan_port = -1, lan_port_map[6] = {-1, -1, -1, -1, -1, -1};
    
    wan_port = atoi(argv[1]);
    for(i = 0; (i < 6) && (i < (argc - 2)); i++)
    {
        lan_port_map[i] = atoi(argv[i + 2]);
    }
    
    return switchmgr_lib_set_portmap(wan_port, lan_port_map);
}

static int doVlanActive(int argc, char *argv[], void *p)
{
    u8 enable=0;

    enable = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_vlan_active(enable);
}

static int doGlobalFlowControl(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 enable = 0;

    if(argc > 1)
    {
        enable = strtoul((const char*)(argv[1]), (char **)NULL, 10);
        ret = switchmgr_lib_set_flow_control(enable);
    }
    else
    {
        ret = switchmgr_lib_get_flow_control(&enable);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("flowcontrol : %s\n", enable ? "enable" : "disable");
        }
    }

    return ret;
}

static int doMaxFrameSize(int argc, char *argv[], void *p)
{
    int ret = 0;
    u16 size = 0;

    if(argc > 1)
    {
        size = strtoul((const char*)(argv[1]), (char **)NULL, 10);
        ret = switchmgr_lib_set_max_framesize(size);
    }
    else
    {
        ret = switchmgr_lib_get_max_framesize(&size);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("maxframesize : %u\n", size);
        }
    }
    return ret;
}

static int doChipType(int argc, char *argv[], void *p)
{
    int ret = 0;
    ECNT_SWITCHMGR_CHIP_TYPE chip_type = 0;
    
    ret = switchmgr_lib_get_chip_type(&chip_type);
    if (ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("chiptype: %d\n", chip_type);
    }
    
    return ret;
}

static int doPortIsolate(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 enable = 0;

    enable = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    ret = switchmgr_lib_set_port_isolate(enable);
    return ret;
}

static int doPortMatrix(int argc, char *argv[], void *p)
{
    int i;
    int port_group[6], type = 0;
    memset(port_group,-1,sizeof(port_group));
    
    type = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    for(i = 0; i < 6; i++)
    {
        port_group[i] = strtoul((const char*)(argv[i + 2]), (char **)NULL, 10);
    }
    return switchmgr_lib_set_port_matrix(type, port_group);
}

static int dosetethgeneralapi(int argc, char *argv[], void *p)
{
    u8 enable = 0;

    enable = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_eth_general_api(enable);
}

static int doAPIDebug(int argc, char *argv[], void *p)
{
    u8 enable=0;

    enable = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    return switchmgr_lib_set_api_debug_dump_enable(enable);
}

static int doTimeStamp(int argc, char *argv[], void *p)
{
    int ret = 0;
    ECNT_SWITCHMGR_TIMESTAMP_FIFO ts;

    if (strcmp((const char*)argv[1], "tx") == 0)
    {
        ret = switchmgr_lib_get_tx_time_stamp(&ts);
    }
    else
    {
        ret = switchmgr_lib_get_rx_time_stamp(&ts);
    }

    if (ECNT_SWITCHMGR_SUCCESS == ret)
    {
        printf("msg_type    : %u\n", ts.msg_type);
        printf("seq_id      : %u\n", ts.seq_id);
        printf("seconds     : %u\n", ts.tstamp.seconds);
        printf("nanoseconds : %u\n", ts.tstamp.nanoseconds);
    }
    
    return ret;
}

static int doSwitchCurTime(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 type = 0;
    ECNT_SWITCHMGR_TIMESTAMP ts;

    if(argc > 3)
    {
        type            = strtoul((const char*)(argv[1]), (char **)NULL, 10);
        ts.seconds      = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        ts.nanoseconds  = strtoul((const char*)(argv[3]), (char **)NULL, 10);
        ret = switchmgr_lib_set_curr_time(type, &ts);
    }
    else
    {
        ret = switchmgr_lib_get_curr_time(&ts);
        if (ECNT_SWITCHMGR_SUCCESS == ret)
        {
            printf("seconds     : %u\n", ts.seconds);
            printf("nanoseconds : %u\n", ts.nanoseconds);
        }
    }

    return ret;
}

static int doSwitchTimeOffset(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 opt = 0;
    ECNT_SWITCHMGR_TIMESTAMP ts;

    opt             = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    ts.seconds      = strtoul((const char*)(argv[2]), (char **)NULL, 10);
    ts.nanoseconds  = strtoul((const char*)(argv[3]), (char **)NULL, 10);
    return switchmgr_lib_set_time_offset(opt, &ts);
}

static int doLan2lan(int argc, char *argv[], void *p)
{
    int ret = 0, i;
    ECNT_SWITCHMGR_LAN2LAN lan2lan = {0};
    ECNT_SWITCHMGR_LAN2LAN_INFO lan2laninfo = {0};

    if(argc < 4)
    {
        ret = switchmgr_lib_get_lan2lan(&lan2laninfo);
        if (ret != ECNT_SWITCHMGR_SUCCESS)
        {
            return ret;
        }
        
        if(!lan2laninfo.enable)
        {
            printf("lan2lan disable\n");
            return 0;
        }
        
        printf("dest port  :%d\n", lan2laninfo.dstport);
        printf("srcportnum :%d\n", lan2laninfo.srcportnum);
        for (i = 0; i < lan2laninfo.srcportnum; i++)
        {
            printf("src port %d: %s%s\n", lan2laninfo.srcport[i].port, 
                lan2laninfo.srcport[i].rx_enable ? "rx" : "",
                lan2laninfo.srcport[i].tx_enable ? "tx" : "");
        }
    }
    else
    {
        lan2lan.srcport = strtoul((const char*)(argv[1]), (char **)NULL, 10);
        lan2lan.dstport = strtoul((const char*)(argv[2]), (char **)NULL, 10);
        if(strcmp(argv[3], "rx") == 0)
        {
            lan2lan.rx_enable = 1;
            lan2lan.tx_enable = 0;
        }
        else if(strcmp(argv[3], "tx") == 0)
        {
            lan2lan.tx_enable = 1;
            lan2lan.rx_enable = 0;
        }
        else
        {
            lan2lan.rx_enable = 1;
            lan2lan.tx_enable = 1;
        }
        ret = switchmgr_lib_set_lan2lan(lan2lan);
    }   

    return ret;
}

static int doLan2lanClear(int argc, char *argv[], void *p)
{
    return switchmgr_lib_clear_lan2lan();
}

static int doDstq(int argc, char *argv[], void *p)
{
    u8 fc_enable= 0, mode = 0;

    mode      = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    fc_enable = strtoul((const char*)(argv[2]), (char **)NULL, 10);
    return switchmgr_lib_set_dstq(mode, fc_enable);
}

static int doMirrorEn(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 enable = 0, port = 0;
    
    enable = strtoul((const char*)(argv[1]), (char **)NULL, 10);
    port   = strtoul((const char*)(argv[2]), (char **)NULL, 10);
    ret = switchmgr_lib_set_mirror_enable(enable, port);
    return ret;
}

static int doVlanBind(int argc, char *argv[], void *p)
{
    int ret = 0;
    u8 port = 0;
    u16 bindvid = 0;

    port    = strtoul((const char*)(argv[2]), (char **)NULL, 10);
    bindvid = strtoul((const char*)(argv[3]), (char **)NULL, 10);
    
    if (strcmp((const char*)argv[1], "add") == 0)
    {
        ret = switchmgr_lib_add_port_vlan_bind(port, bindvid);
    }
    else
    {
        ret = switchmgr_lib_del_port_vlan_bind(port, bindvid);
    }
    
    return ret;
}

static int dop6acl(int argc, char *argv[], void *p)
{
    return switchmgr_lib_set_do_p6_cal();
}

static int doPort(int argc, char *argv[], void *p)
{
    return subcmd(portCmds, argc, argv, p);
}

static int doVlan(int argc, char *argv[], void *p)
{
    return subcmd(vlanCmds, argc, argv, p);
}

static int doPhy(int argc, char *argv[], void *p)
{
    return subcmd(phyCmds, argc, argv, p);
}

static int doMac(int argc, char *argv[], void *p)
{
    return subcmd(macCmds, argc, argv, p);
}

static int doQos(int argc, char *argv[], void *p)
{
    return subcmd(qosCmds, argc, argv, p);
}

static int doStats(int argc, char *argv[], void *p)
{
    return subcmd(statsCmds, argc, argv, p);
}

static int doStormCtrl(int argc, char *argv[], void *p)
{
    return subcmd(stormctrlCmds, argc, argv, p);
}

static int doDsTrtcm(int argc, char *argv[], void *p)
{
    return subcmd(dsTrtcmCmds, argc, argv, p);
}

static int doTrafficSel(int argc, char *argv[], void *p)
{
    return subcmd(TrafficCmds, argc, argv, p);
}

static int doratectl(int argc, char *argv[], void *p)
{
    return subcmd(ratectlCmds, argc, argv, p);
}

static int dodsp5(int argc, char *argv[], void *p)
{
    return subcmd(dsp5Cmds, argc, argv, p);
}

static int doAcl(int argc, char *argv[], void *p)
{
    return subcmd(aclCmds, argc, argv, p);
}

static int doReg(int argc, char *argv[], void *p)
{
    return subcmd(regCmds, argc, argv, p);
}

static int doWol(int argc, char *argv[], void *p)
{
    return subcmd(WolCmds, argc, argv, p);
}


#define HELP_SWITCH_PORT_MAP    "switchmgr portmap <wan_port> <lan_port0> <lan_port1> <lan_port2> <lan_port3> [<lan_port4> [<lan_port5>]]\n"\
                                "Description: Set switch port map\n"\
                                "             Use -1 as default or in case a port is invalid\n"\
                                "eg: swtichmgr portmap -1 1 2 3 4 -1 -1\n"\
                                "    swtichmgr portmap  0 1 2 3 4 -1 -1\n"
                                
#define HELP_SWITCH_VLAN_ACTIVE "switchmgr vlanactive <enable:1|disable:0>>\n"

#define HELP_SWITCH_FLOWCONTROL "switchmgr flowcontrol [<enable:1|disable:0>]>\n"
                                
#define HELP_switch_MAXFRAMESIZE "switchmgr port maxframesize [<size:1518|1536|1552>]\n"
                                
#define HELP_SWITCH_PORTISOLATE "switchmgr portisolate <enable:1|disable:0>>\n"

#define HELP_SWITCH_PORT_MATRIX "switchmgr portmatrix <type> <port_id0> <port_id1> <port_id2> <port_id3> <port_id4> <port_id5>\n"\
                                "type:\n"\
                                "0:general_group\n"\
                                "1:vlanbind_group\n"\
                                "2:portbind_group\n"\
                                "port_id0~5 is group id for eth0.1~eth0.6\n"\
                                "if eth0.x is not exist, set -1\n"\
                                "if no need to group, set 0\n"

#define HELP_SWITCH_ETHGENERAL  "switchmgr ethgeneral [<enable:1|disable:0>]>\n"

#define HELP_SWITCH_API_DEBUG   "switchmgr apidebug <enable:1|disable:0>>\n"

#define HELP_SWITCH_TIMESTAMP   "switchmgr timestamp <tx|rx>>\n"

#define HELP_SWITCH_CURRTIME    "switchmgr curtime [<type> <seconds> <nanoseconds>]\n"

#define HELP_SWITCH_TIMEOFFSET  "switchmgr timeoffset <add:1|sub:0> <seconds> <nanoseconds>\n"

#define HELP_LAN2LAN            "switchmgr lan2lan [<srcport:0~6> <dstport:0~6> <rx|tx|rxtx>]\n"

#define HELP_DSTQ               "switchmgr dstq <mode> <flowcontrol>\n"\
                                "mode:\n"\
                                "   0: disable\n"\
                                "   1: down stream to gdm1\n"\
                                "   2: down stream to qdma_lan\n"\
                                "flowcontrol:\n"\
                                "   0: disable flow control\n"\
                                "   1: enable  flow control\n"

#define HELP_GLOBAL_MIRROR      "switchmgr mirror <enable:1|disable:0> <port>\n"

#define HELP_VLANBIND           "switchmgr vlanbind <add|del> <port> <vid>\n"


static cmds_t switchCmds[] = 
{
    
    {"port",            doPort,                 0x12, 0, NULL},
    {"vlan",            doVlan,                 0x12, 0, NULL},
    {"portmap",         doSwitchPortMap,        0x12, 5, HELP_SWITCH_PORT_MAP},
    {"vlanactive",      doVlanActive,           0x12, 1, HELP_SWITCH_VLAN_ACTIVE},
    {"flowcontrol",     doGlobalFlowControl,    0x12, 0, HELP_SWITCH_FLOWCONTROL},
    {"maxframesize",    doMaxFrameSize,         0x12, 0, HELP_switch_MAXFRAMESIZE},
    {"chiptype",        doChipType,             0x12, 0, NULL},                     //TODO: should move to other module
    {"portisolate",     doPortIsolate,          0x12, 1, HELP_SWITCH_PORTISOLATE},  //TODO: need get api
    {"portmatrix",      doPortMatrix,           0x12, 7, HELP_SWITCH_PORT_MATRIX},  //TODO: need get api
    {"ethgeneral",      dosetethgeneralapi,     0x12, 1, HELP_SWITCH_ETHGENERAL},   //TODO: need get api
    {"apidebug",        doAPIDebug,             0x12, 1, HELP_SWITCH_API_DEBUG},
    {"timestamp",       doTimeStamp,            0x12, 1, HELP_SWITCH_TIMESTAMP},
    {"curtime",         doSwitchCurTime,        0x12, 0, HELP_SWITCH_CURRTIME},
    {"timeoffset",      doSwitchTimeOffset,     0x12, 3, HELP_SWITCH_TIMEOFFSET},
    {"lan2lan",         doLan2lan,              0x12, 0, HELP_LAN2LAN},
    {"lan2lanclear",    doLan2lanClear,         0x12, 0, NULL},
    {"dstq",            doDstq,                 0x12, 2, HELP_DSTQ},
    {"mirror",          doMirrorEn,             0x12, 2, HELP_GLOBAL_MIRROR},   //TODO: need get api
    {"vlanbind",        doVlanBind,             0x12, 3, HELP_VLANBIND},        //TODO: need get api
    {"p6cal",           dop6acl,                0x12, 0, NULL}, //7525G/7526G only

    {"phy",             doPhy,                  0x12, 0, NULL},
    {"mac",             doMac,                  0x12, 0, NULL},
    {"qos",             doQos,                  0x12, 0, NULL},
    {"stats",           doStats,                0x12, 0, NULL},
    {"stormctrl",       doStormCtrl,            0x12, 0, NULL},
    {"dstrtcm",         doDsTrtcm,              0x12, 0, NULL}, //only for MT7525G/MT7520G
    {"traffic",         doTrafficSel,           0x12, 0, NULL}, 
    {"ratectl",         doratectl,              0x12, 0, NULL},
    {"dsp5",            dodsp5,                 0x12, 0, NULL},
    {"acl",             doAcl,                  0x12, 0, NULL},
    {"reg",             doReg,                  0x12, 0, NULL},
    {"wol",             doWol,                  0x12, 0, NULL},    
    {NULL,              NULL,                   0x10, 0, NULL}
};

int main(int argc, char **argv) 
{
	struct swconfig *swcfg=NULL;
	swcfg=getSwcfg();
	//If init fail, socket will be closed. We can not close socket twice. shnwind 20110317.
	if(init_swconfig() == 0){
		subcmd(switchCmds, argc, argv, NULL);
		close(swcfg->skfd);
	}	
    return 0;
}

/** @{ */ //end of SwitchCmd

