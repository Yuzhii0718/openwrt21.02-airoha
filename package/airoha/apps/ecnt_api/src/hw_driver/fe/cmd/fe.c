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
#include "libapi_lib_utility.h"
#include "libapi_lib_fe.h"
#include <ecnt_hook/ecnt_hook_fe_type.h>

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
/*******************************************************************************/
/*  RATELIMIT commands                                                                                                     */
/*******************************************************************************/
static int do_set_pkt_ratelimit_clear(int argc, char *argv[], void *p);
static int do_set_pkt_ratelimit_total(int argc, char *argv[], void *p);
static int do_set_pkt_ratelimit_singal(int argc, char *argv[], void *p);

static int do_set_tx(int argc, char *argv[], void *p)
{
    unsigned int rate, mode;

    if (argc < 3)
    {
        printf("fe set tx <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n");
        return 0;
    }
    
    rate = strtoul(argv[1], NULL, 10);
    rate = rate / 8;
    mode = strtoul(argv[2], NULL, 10);
    
    return fe_lib_set_gdma2_tx_rate(mode, rate);
}

static int do_set_rxuc(int argc, char *argv[], void *p)
{
    unsigned int rate, mode;

    if (argc < 3)
    {
        printf("fe set rxuc <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n");
        return 0;
    }
    
    rate = strtoul(argv[1], NULL, 10);
    rate = rate / 8;
    mode = strtoul(argv[2], NULL, 10);
    
    return fe_lib_set_gdma2_rx_rate(ECNT_FEMGR_PKT_TYPE_MYMAC, mode, rate);
}

static int do_set_rxbc(int argc, char *argv[], void *p)
{
    unsigned int rate, mode;

    if (argc < 3)
    {
        printf("fe set rxbc <rate(kBps or kpps)> <mode>(0:bps mode 1:pps mode) \n");
        return 0;
    }
    
    rate = strtoul(argv[1], NULL, 10);
    rate = rate / 8;
    mode = strtoul(argv[2], NULL, 10);
    
    return fe_lib_set_gdma2_rx_rate(ECNT_FEMGR_PKT_TYPE_BROADCAST, mode, rate);
}

static int do_set_rxmc(int argc, char *argv[], void *p)
{
    unsigned int rate, mode;

    if (argc < 3)
    {
        printf("fe set rxmc <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n");
        return 0;
    }
    
    rate = strtoul(argv[1], NULL, 10);
    rate = rate / 8;
    mode = strtoul(argv[2], NULL, 10);
    
    return fe_lib_set_gdma2_rx_rate(ECNT_FEMGR_PKT_TYPE_MULTICAST, mode, rate);
}

static int do_set_rxoc(int argc, char *argv[], void *p)
{
    unsigned short rate, mode;

    if (argc < 3)
    {
        printf("fe set rxoc <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n");
        return 0;
    }
    
    rate = strtoul(argv[1], NULL, 10);
    rate = rate / 8;
    mode = strtoul(argv[2], NULL, 10);

    return fe_lib_set_gdma2_rx_rate(ECNT_FEMGR_PKT_TYPE_OTHERCAST, mode, rate);
}

static int do_set_rxmac_filter(int argc, char *argv[], void *p)
{
    unsigned short rate, mode;

    if (argc < 3)
    {
        printf("femgr set rx_mac_filter <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n");
        return 0;
    }
    
    rate = strtoul(argv[1], NULL, 10);
    mode = strtoul(argv[2], NULL, 10);

    return fe_lib_set_gdma2_rx_rate(ECNT_FEMGR_PKT_TYPE_MAC_FILTER, mode, rate);
}

static int do_set_rx_if(int argc, char *argv[], void *p)
{

    unsigned int rate = 0;
    unsigned int enable = 0;
    unsigned char idx = 0;
    unsigned int dir = 0;
    ECNT_FEMGR_RATELIMIT_IF_INFO if_info = {0};
    ECNT_FEMGR_METER_INFO p_meter_info = {0};

    if(argc < 4)
    {
        printf("fe set rx_if <dir (0:down_stream 1:up_stream)> <idx (0-7) > <enable (0:disable 1:enable one rule)> <rate (unit:kbps)> \n");
        return 0;
    }

    dir = strtoul(argv[1],NULL,10);
    idx = strtoul(argv[2],NULL,10);
    enable = strtoul(argv[3],NULL,10);
    rate = strtoul(argv[4],NULL,10);
    
    if_info.index = idx;
    if_info.enable = enable;
    if_info.rate = rate;

    p_meter_info.dir = dir;
    p_meter_info.enable = enable;
    p_meter_info.rate = rate;
    p_meter_info.index = idx;
    p_meter_info.mode = 2;
    
    fe_lib_set_meter_ratelimit_by(&p_meter_info);
    fe_lib_set_if_ratelimit_rule(&if_info, dir);
    
    return 0;
}

static int do_set_rx_ip(int argc, char *argv[], void *p)
{
    unsigned int rate = 0;
    unsigned int enable = 0;
    unsigned char idx = 0;
    unsigned int dir = 0;
    unsigned int ip_version = 0;
    int i = 0;
    unsigned int ipv4_add[4];
    unsigned int ipv6_add[16];
    
    ECNT_FEMGR_RATELIMIT_IP_INFO ip_info = {0};
    ECNT_FEMGR_METER_INFO p_meter_info = {0};

    if(argc < 7)
    {
        printf("fe set rx_ip <dir (0:down_stream 1:up_stream)> <idx (0-7) > <enable (0:disable 1:enable one rule)> <rate (unit:kbps)> \
            <ip_version (0:ipv4 1:ipv6)> <ip_address_low(X.X.X.X or X:X:X:X:X:X:X)> <ip_address_high (X.X.X.X or X:X:X:X:X:X:X)>\n");
        return 0;
    }
    dir = strtoul(argv[1],NULL,10);
    idx = strtoul(argv[2],NULL,10);
    enable = strtoul(argv[3],NULL,10);
    rate = strtoul(argv[4],NULL,10);
    ip_version = strtoul(argv[5],NULL,10);

    ip_info.enable = enable;
    ip_info.index = idx;
    ip_info.ip_version = ip_version;
    ip_info.rate = rate;

    p_meter_info.dir = dir;
    p_meter_info.enable = enable;
    p_meter_info.rate = rate;
    p_meter_info.index = idx;
    p_meter_info.mode = 1;
    
    if(ip_version == 0){
        sscanf(argv[6], "%d.%d.%d.%d", &(ipv4_add[0]), &(ipv4_add[1]), &(ipv4_add[2]), &(ipv4_add[3]));
        for(i = 0; i < 4; i++){
            ip_info.v4_pair_l[i] = (unsigned char)ipv4_add[i];
        }
        sscanf(argv[7], "%d.%d.%d.%d", &(ipv4_add[0]), &(ipv4_add[1]), &(ipv4_add[2]), &(ipv4_add[3]));
        for(i = 0; i < 4; i++){
            ip_info.v4_pair_h[i] = (unsigned char)ipv4_add[i];
        }
    }else{ 
        sscanf(argv[6], "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X", 
            &(ipv6_add[0]), &(ipv6_add[1]), &(ipv6_add[2]), &(ipv6_add[3]), 
            &(ipv6_add[4]), &(ipv6_add[5]), &(ipv6_add[6]), &(ipv6_add[7]),
            &(ipv6_add[8]), &(ipv6_add[9]), &(ipv6_add[10]), &(ipv6_add[11]), 
            &(ipv6_add[12]), &(ipv6_add[13]), &(ipv6_add[14]), &(ipv6_add[15]));
        for(i = 0; i < 16; i++){
            ip_info.v6_pair_l[i] = (unsigned char)ipv6_add[i];
        }

        sscanf(argv[7], "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X", 
            &(ipv6_add[0]), &(ipv6_add[1]), &(ipv6_add[2]), &(ipv6_add[3]), 
            &(ipv6_add[4]), &(ipv6_add[5]), &(ipv6_add[6]), &(ipv6_add[7]),
            &(ipv6_add[8]), &(ipv6_add[9]), &(ipv6_add[10]), &(ipv6_add[11]), 
            &(ipv6_add[12]), &(ipv6_add[13]), &(ipv6_add[14]), &(ipv6_add[15]));
        for(i = 0; i < 16; i++){
            ip_info.v6_pair_h[i] = (unsigned char)ipv6_add[i];
        }

    }
    
    fe_lib_set_meter_ratelimit_by(&p_meter_info);
    fe_lib_set_ip_ratelimit_rule(&ip_info, dir);

    return 0;
}

static int do_set_rx_ratelimit_mode(int argc, char *argv[], void *p)
{
    unsigned int dir = 0;
    unsigned int mode = 0;
    unsigned int meter_group = 0;
    if(argc < 2){
        printf("fe set rx_mode <dir>(0:down 1:up) <mode>(1:IP based 2:IF based 3:NULL ratelimit)\n");

    }
    dir = strtoul(argv[1],NULL,10);
    mode = strtoul(argv[2],NULL,10);
#if defined(TCSUPPORT_ACCOUNT_METER_V2) 
    meter_group = strtoul(argv[3],NULL,10);
#endif

    return fe_lib_set_meter_ratelimit_mode(dir, meter_group, mode);
}

static int do_set_meter_rate(int argc, char *argv[], void *p)
{
    unsigned int rate = 0;
    unsigned int enable = 0;
    unsigned char idx = 0;
    unsigned int dir = 0;
    unsigned int mode =0 ;
    ECNT_FEMGR_METER_INFO p_meter_info = {0};

    if(argc < 5){
        printf("fe set rate <dir(:down_stream 1:up_stream)> <mode>(1:IP based 2:IF based other:error) <idx (0-7) > <enable (0:disable 1:enable one rule)> <rate (unit:kbps)\n");
        return 0;
    }
    dir = strtoul(argv[1],NULL,10);
    mode = strtoul(argv[2],NULL,10);
    idx = strtoul(argv[3],NULL,10);
    enable = strtoul(argv[4],NULL,10);
    rate = strtoul(argv[5],NULL,10);

    p_meter_info.dir = dir;
    p_meter_info.enable = enable;
    p_meter_info.rate = rate;
    p_meter_info.index = idx;
    p_meter_info.mode = mode;

    fe_lib_set_meter_ratelimit_by(&p_meter_info);
    
    return 0;
}

static int do_set_pkt_ratelimit_clear(int argc, char *argv[], void *p)
{	
 	unsigned char pkt_mode = 255;

	if(argc < 2){
		printf("fe set pkt_ratelimit_clear <mode(0:Boardcast 1:Multicast)>\n");
		return 0;
	}
	pkt_mode = strtoul(argv[1], NULL, 10);
	return fe_lib_set_pkt_ratelimit_clear(pkt_mode);
	
}
static int do_set_pkt_ratelimit_total(int argc, char *argv[], void *p)
{
	unsigned char pkt_mode = 255;

	if(argc < 2){
		printf("fe set pkt_ratelimit_total <mode(0:Boardcast 1:Multicast)>\n");
		return 0;
	}
	pkt_mode = strtoul(argv[1], NULL, 10);
	return fe_lib_set_pkt_ratelimit_total(pkt_mode);
}
static int do_set_pkt_ratelimit_singal(int argc, char *argv[], void *p)
{
	unsigned char pkt_mode = 255;

	if(argc < 2){
		printf("fe set pkt_ratelimit_singal <mode(0:Boardcast 1:Multicast)>\n");
		return 0;
	}
	pkt_mode = strtoul(argv[1], NULL, 10);
	return fe_lib_set_pkt_ratelimit_singal(pkt_mode);
}


static cmds_t ratelimit_setCmds[] = 
{
    {"tx",    do_set_tx,        0x0,  2, "fe set tx <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n"},
    {"rxuc",  do_set_rxuc,      0x0,  2, "fe set rxuc <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n"},
    {"rxbc",  do_set_rxbc,      0x0,  2, "fe set rxbc <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n"},
    {"rxmc",  do_set_rxmc,      0x0,  2, "fe set rxmc <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n"},
    {"rxoc",  do_set_rxoc,      0x0,  2, "fe set rxoc <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n"},   
	{"rx_mac_filter",  do_set_rxmac_filter,      0x0,  2, "femgr set rx_mac_filter <rate(kbps or kpps)> <mode>(0:bps mode 1:pps mode) \n"},
    {"rx_if", do_set_rx_if,     0x0, 4, "fe ratelimit set rx_if <dir (0:down_stream 1:up_stream)> <idx (0-7) > <enable (0:disable 1:enable one rule)> <rate (unit:kbps)> \n"},
    {"rx_ip", do_set_rx_ip,     0x0, 7, "fe ratelimit set rx_ip <dir (0:down_stream 1:up_stream)> <idx (0-7) > <enable (0:disable 1:enable one rule)> <rate (unit:kbps)> \
            <ip_version (0:ipv4 1:ipv6)> <ip_address_low(X.X.X.X or X:X:X:X:X:X:X)> <ip_address_high (X.X.X.X or X:X:X:X:X:X:X)> \n"},
    {"rx_mode", do_set_rx_ratelimit_mode, 0x0, 2, "fe set rx_mode <dir>(0:down 1:up) <mode>(1:IP based 2:IF based 3:NULL)\n"},
    {"rate",  do_set_meter_rate,      0x0,    5,  "fe ratelimit set rate <dir(:down_stream 1:up_stream)> <mode>(1:IP based 2:IF based other:error) <idx (0-7) >\
    <enable (0:disable 1:enable one rule)> <rate (unit:kbps)"},
    {"pkt_ratelimit_clear", do_set_pkt_ratelimit_clear, 0x0, NULL},
    {"pkt_ratelimit_total",do_set_pkt_ratelimit_total, 0x0, NULL},
    {"pkt_ratelimit_singal",do_set_pkt_ratelimit_singal, 0x0, NULL},  
    {NULL,    NULL,             0x0,  0, NULL}
};

static int do_ratelimit_set(int argc, char *argv[], void *p)
{
    return subcmd(ratelimit_setCmds, argc, argv, NULL);
}

static cmds_t ratelimitCmds[] = 
{
    {"set",     do_ratelimit_set,   0x0,  0, NULL},
    {NULL,      NULL,               0x0,  0, NULL}
};

/*******************************************************************************/
/*  MAC commands                                                                                                              */
/*******************************************************************************/
static int do_set_mymac(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, mask = 0, i = 0, mactemp[6];
    unsigned char mac[6];

    sscanf(argv[1], "%02x:%02x:%02x:%02x:%02x:%02x", &mactemp[0], &mactemp[1], 
            &mactemp[2], &mactemp[3], &mactemp[4], &mactemp[5]);
    mac[0] =(unsigned char)mactemp[0];
    mac[1] =(unsigned char)mactemp[1];
    mac[2] =(unsigned char)mactemp[2];
    mac[3] =(unsigned char)mactemp[3];
    mac[4] =(unsigned char)mactemp[4];
    mac[5] =(unsigned char)mactemp[5];
    gdm_sel = strtoul(argv[2], NULL, 10);
    mask    = strtoul(argv[3], NULL, 10);
    
    return fe_lib_set_mymac(gdm_sel, mac, mask);
}

static cmds_t macCmds[] = 
{
    {"set",     do_set_mymac,       0x0,  3, "fe mac set <mac> <gdm_sel> <7526:mask/7527:range>"},
    {NULL,      NULL,               0x0,  0, NULL}
};

/*******************************************************************************/
/*  DROP commands                                                                                                            */
/*******************************************************************************/
static int do_set_drop_udp_chksum_err_enable(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, enable = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    enable  = strtoul(argv[2], NULL, 10);
    return fe_lib_set_drop_frame_enable(gdm_sel, ECNT_FEMGR_DROP_FRAME_TYPE_UDP_CHECKSUM_ERR, enable);
}

static int do_set_drop_tcp_chksum_err_enable(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, enable = 0;
        
    gdm_sel = strtoul(argv[1], NULL,10);
    enable  = strtoul(argv[2], NULL,10);
    return fe_lib_set_drop_frame_enable(gdm_sel, ECNT_FEMGR_DROP_FRAME_TYPE_TCP_CHECKSUM_ERR, enable);
}

static int do_set_drop_ip_chksum_err_enable(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, enable = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    enable  = strtoul(argv[2], NULL, 10);
    return fe_lib_set_drop_frame_enable(gdm_sel, ECNT_FEMGR_DROP_FRAME_TYPE_IP_CHECKSUM_ERR, enable);
}

static int do_set_drop_crc_err_enable(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, enable = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    enable  = strtoul(argv[2], NULL, 10);
    return fe_lib_set_drop_frame_enable(gdm_sel, ECNT_FEMGR_DROP_FRAME_TYPE_CRC_ERR, enable);
}

static int do_set_drop_runt_enable(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, enable = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    enable  = strtoul(argv[2], NULL, 10);
    return fe_lib_set_drop_frame_enable(gdm_sel, ECNT_FEMGR_DROP_FRAME_TYPE_RUNT, enable);
}

static int do_set_drop_long_enable(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, enable = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    enable  = strtoul(argv[2], NULL, 10);
    return fe_lib_set_drop_frame_enable(gdm_sel, ECNT_FEMGR_DROP_FRAME_TYPE_LONG, enable);
}

#if 0
static int do_set_cdm_rx_red_drop_mode(int argc, char *argv[], void *p)
{
    unsigned int cdm_sel = 0, dropQ_sel = 0, dropMode_sel = 0;

    cdm_sel      = strtoul(argv[1], NULL, 10);
    dropQ_sel    = strtoul(argv[2], NULL, 10);
    dropMode_sel = strtoul(argv[3], NULL, 10);
    return fe_lib_set_cdm_rx_red_drop_mode(cdm_sel, dropQ_sel, dropMode_sel);
}
#endif

static cmds_t drop_setCmds[] = 
{
    {"udp",                 do_set_drop_udp_chksum_err_enable,      0x0,  2, "fe drop set udp <gdm_set> <enbale>"},
    {"tcp",                 do_set_drop_tcp_chksum_err_enable,      0x0,  2, "fe drop set tcp <gdm_set> <enbale>"},
    {"ip",                  do_set_drop_ip_chksum_err_enable,       0x0,  2, "fe drop set ip <gdm_set> <enbale>"},
    {"crc",                 do_set_drop_crc_err_enable,             0x0,  2, "fe drop set crc <gdm_set> <enbale>"},
    {"runt",                do_set_drop_runt_enable,                0x0,  2, "fe drop set runt <gdm_set> <enbale>"},
    {"long",                do_set_drop_long_enable,                0x0,  2, "fe drop set long <gdm_set> <enbale>"},
    //{"cdm_rx_drop_mode",    do_set_cdm_rx_red_drop_mode,            0x0,  3, "<cdm_set> <dropQ_sel> <dropMode_sel>"},
    {NULL,                  NULL,                                   0x0,  0, NULL}
};

static int do_drop_set(int argc, char *argv[], void *p)
{
    return subcmd(drop_setCmds, argc, argv, NULL);
}

#if 0
static int do_get_cdm_rx_red_drop_mode(int argc, char *argv[], void *p)
{
    unsigned int cdm_sel = 0, dropQ_sel = 0;

    cdm_sel   = strtoul(argv[1],NULL,10);
    dropQ_sel = strtoul(argv[2],NULL,10);
    return fe_lib_get_cdm_rx_red_drop_mode(cdm_sel, dropQ_sel);
}

static cmds_t drop_getCmds[] = 
{
    {"cdm_rx_drop_mode",    do_get_cdm_rx_red_drop_mode,        0x0,  2, "<cdm_set> <channel>"},
    {NULL,                  NULL,                               0x10, 0, NULL}
};

static int do_drop_get(int argc, char *argv[], void *p)
{
    return subcmd(drop_getCmds, argc, argv, NULL);
}
#endif

static cmds_t dropCmds[] = 
{
    {"set",     do_drop_set,        0x0,  0, NULL},
    //{"get",     do_drop_get,        0x0,  0, NULL},
    {NULL,      NULL,               0x0,  0, NULL}
};

/*******************************************************************************/
/*  PCP commands                                                                                                               */
/*******************************************************************************/
static int do_set_gdm_pcp_coding(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, txrx_sel = 0, mode = 0;

    gdm_sel  = strtoul(argv[1], NULL, 10);
    txrx_sel = strtoul(argv[2], NULL, 10);
    mode     = strtoul(argv[3], NULL, 10);
    return fe_lib_set_gdma_pcp_coding(gdm_sel, txrx_sel, mode);
}

static int do_set_cdm_pcp_coding(int argc, char *argv[], void *p)
{
    unsigned int cdm_sel = 0, txrx_sel = 0, mode = 0;

    cdm_sel  = strtoul(argv[1], NULL, 10);
    txrx_sel = strtoul(argv[2], NULL, 10);
    mode     = strtoul(argv[3], NULL, 10);
    return fe_lib_set_cdma_pcp_coding(cdm_sel, txrx_sel, mode);
}

static cmds_t pcp_setCmds[] = 
{
    {"gdm",     do_set_gdm_pcp_coding,      0x0,  3, "fe pcp set gdm <gmd_sel> <txrx_sel> <mode>"},
    {"cdm",     do_set_cdm_pcp_coding,      0x0,  3, "fe pcp set cdm <gmd_sel> <txrx_sel> <mode>"},
    {NULL,      NULL,                       0x0,  0, NULL}
};

static int do_pcp_set(int argc, char *argv[], void *p)
{
    return subcmd(pcp_setCmds, argc, argv, NULL);
}

static cmds_t pcpCmds[] =
{
    {"set",     do_pcp_set,     0x0,  0, NULL},
    {NULL,      NULL,           0x0,  0, NULL}
};

/*******************************************************************************/
/*  VIP commands                                                                                                               */
/*******************************************************************************/
static int do_set_vip_enable(int argc, char *argv[], void *p)
{
    unsigned int index = 0, enable = 0, patten_type = 0, patten = 0;

    index       = strtoul(argv[1], NULL, 10);
    enable      = strtoul(argv[2], NULL, 10);
    patten_type = strtoul(argv[3], NULL, 10);
    patten      = strtoul(argv[4], NULL, 10);
    return fe_lib_set_vip(index, enable, patten_type, patten);
}

static cmds_t vip_setCmds[] = 
{
    {"enable",      do_set_vip_enable,  0x0,  4, "<index> <enable> <patten_type> <patten>"},
    {NULL,          NULL,               0x0,  0, NULL}
};

static int do_vip_set(int argc, char *argv[], void *p)
{
    return subcmd(vip_setCmds, argc, argv, NULL);
}

static int do_add_eth(int argc, char *argv[], void *p)
{
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 2)
    {
        printf("fe add eth <type> \n");
        return 0;
    }
    
    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_ETHER_TYPE;
    vip.u.ether_type = strtoul(argv[1],NULL,16);
    return fe_lib_add_vip(&vip);
}

static int do_add_ppp(int argc, char *argv[], void *p)
{
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 2)
    {
        printf("fe add ppp <type> \n");
        return 0;
    }

    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_PPPOE_PROTOCOL;
    vip.u.pppoe_protocol = strtoul(argv[1],NULL,16);
    
    return fe_lib_add_vip(&vip);
}

static int do_add_ip(int argc, char *argv[], void *p)
{
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 2)
    {
        printf("fe add ip <type> \n");
        return 0;
    }
    
    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_IP_PROTOCOL;
    vip.u.ip_protocol = strtoul(argv[1],NULL,16);
    
    return fe_lib_add_vip(&vip);
}

static int do_add_tcp(int argc, char *argv[], void *p)
{
    int mode;
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 3)
    {
        printf("fe add tcp <src> <dst> <mode> \n");
        return 0;
    }

    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS;
    vip.u.tcp.src_port = strtoul(argv[1],NULL,10);
    vip.u.tcp.dst_port = strtoul(argv[2],NULL,10);
    mode = strtoul(argv[3],NULL,10);
    if (mode & 0x1)
    {
        vip.u.tcp.src_port_en = 1;
    }
    if (mode & 0x2)
    {
        vip.u.tcp.dst_port_en = 1;
    }
    if (mode & 0x4)
    {
        vip.u.tcp.swap_port_en = 1;
    }
    return fe_lib_add_vip(&vip);
}

static int do_add_udp(int argc, char *argv[], void *p)
{
    int mode;
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 3)
    {
        printf("fe add udp <src> <dst> <mode> \n"); 
        return 0;
    }
    
    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS;
    vip.u.udp.src_port = strtoul(argv[1],NULL,10);
    vip.u.udp.dst_port = strtoul(argv[2],NULL,10);
    mode = strtoul(argv[3],NULL,10);
    if (mode & 0x1)
    {
        vip.u.udp.src_port_en = 1;
    }
    if (mode & 0x2)
    {
        vip.u.udp.dst_port_en = 1;
    }
    if (mode & 0x4)
    {
        vip.u.udp.swap_port_en = 1;
    }
    return fe_lib_add_vip(&vip);
}

static int do_add_dscp(int argc, char *argv[], void *p)
{
    int mask;
    ECNT_FEMGR_L2LU_KEY_ENTRY l2lu_key = {0};

    if (argc < 5)
    {
        printf("fe add dscp <svlan> <cvlan> <dscp> <mask> \n");
        return 0;
    }

    l2lu_key.is_dscp = 1;
    l2lu_key.svid = strtoul(argv[1],NULL,16);
    l2lu_key.cvid = strtoul(argv[2],NULL,16);   
    l2lu_key.dscp_or_traffic_class = strtoul(argv[3],NULL,16);
    mask = strtoul(argv[4],NULL,16);

    if (mask & 0x1)
    {
        l2lu_key.svid_en = 1;
    }

    if (mask & 0x2)
    {
        l2lu_key.spcp_en= 1;
    }

    if (mask & 0x4)
    {
        l2lu_key.cvid_en= 1;
    }

    if (mask & 0x8)
    {
        l2lu_key.cpcp_en= 1;
    }

    if (mask & 0x10)
    {
        l2lu_key.dscp_mask_en= 1;
    }
    return fe_lib_add_l2lu_key(&l2lu_key);
}

static int do_add_trfc(int argc, char *argv[], void *p)
{
    int mask;
    ECNT_FEMGR_L2LU_KEY_ENTRY l2lu_key = {0};

    if (argc < 5)
    {
        printf("fe add trfc <svlan> <cvlan> <trfc> <mask> \n");
        return 0;
    }
    
    l2lu_key.is_dscp = 0;
    l2lu_key.svid = strtoul(argv[1],NULL,16);
    l2lu_key.cvid = strtoul(argv[2],NULL,16);   
    l2lu_key.dscp_or_traffic_class = strtoul(argv[3],NULL,16);
    mask = strtoul(argv[4],NULL,16);

    if (mask & 0x1)
    {
        l2lu_key.svid_en = 1;
    }

    if (mask & 0x2)
    {
        l2lu_key.spcp_en= 1;
    }

    if (mask & 0x4)
    {
        l2lu_key.cvid_en= 1;
    }

    if (mask & 0x8)
    {
        l2lu_key.cpcp_en= 1;
    }

    if (mask & 0x10)
    {
        l2lu_key.dscp_mask_en= 1;
    }
    return fe_lib_add_l2lu_key(&l2lu_key);
}

static cmds_t vip_addCmds[] = {
    {"eth",     do_add_eth,         0x0,  1, "fe add eth <type> \n"},
    {"ppp",     do_add_ppp,         0x0,  1, "fe add ppp <protocol> \n"},
    {"ip",      do_add_ip,          0x0,  1, "fe add ip <protocol> \n"},
    {"tcp",     do_add_tcp,         0x0,  3, "fe add tcp <src> <dst> <mode> \n"},    
    {"udp",     do_add_udp,         0x0,  3, "fe add udp <src> <dst> <mode> \n"},
    {"dscp",    do_add_dscp,        0x0,  4, "fe add dscp <svlan> <cvlan> <prio> <mask> \n"},    
    {"trfc",    do_add_trfc,        0x0,  4, "fe add trfc <svlan> <cvlan> <prio> <mask> \n"},    
    {NULL,      NULL,               0x0,  0, NULL}
};

static int do_vip_add(int argc, char *argv[], void *p)
{
    return subcmd(vip_addCmds, argc, argv, NULL);
}

static int do_del_eth(int argc, char *argv[], void *p)
{
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 2)
    {
        printf("fe del eth <type> \n");
        return 0;
    }
    
    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_ETHER_TYPE;
    vip.u.ether_type = strtoul(argv[1],NULL,16);
    return fe_lib_del_vip(&vip);
}

static int do_del_ppp(int argc, char *argv[], void *p)
{
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 2)
    {
        printf("fe del ppp <type> \n");
        return 0;
    }

    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_PPPOE_PROTOCOL;
    vip.u.pppoe_protocol = strtoul(argv[1],NULL,16);
    
    return fe_lib_del_vip(&vip);
}

static int do_del_ip(int argc, char *argv[], void *p)
{
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 2)
    {
        printf("fe del ip <type> \n");
        return 0;
    }
    
    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_IP_PROTOCOL;
    vip.u.ip_protocol = strtoul(argv[1],NULL,16);
    
    return fe_lib_del_vip(&vip);
}

static int do_del_tcp(int argc, char *argv[], void *p)
{
    int mode;
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 3)
    {
        printf("fe del tcp <src> <dst> <mode> \n"); 
        return 0;
    }
    
    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_TCP_PORTS;
    vip.u.tcp.src_port = strtoul(argv[1],NULL,10);
    vip.u.tcp.dst_port = strtoul(argv[2],NULL,10);
    mode = strtoul(argv[3],NULL,10);
    if (mode & 0x1)
    {
        vip.u.tcp.src_port_en = 1;
    }
    if (mode & 0x2)
    {
        vip.u.tcp.dst_port_en = 1;
    }
    if (mode & 0x4)
    {
        vip.u.tcp.swap_port_en = 1;
    }
    return fe_lib_del_vip(&vip);
}

static int do_del_udp(int argc, char *argv[], void *p)
{
    int mode;
    ECNT_FEMGR_VIP_ENTRY vip = {0};

    if (argc < 3)
    {
        printf("fe del udp <src> <dst> <mode> \n");
        return 0;
    }
    
    vip.type = ECNT_FEMGR_VIP_PATTERN_TYPE_UDP_PORTS;
    vip.u.udp.src_port = strtoul(argv[1],NULL,10);
    vip.u.udp.dst_port = strtoul(argv[2],NULL,10);
    mode = strtoul(argv[3],NULL,10);
    if (mode & 0x1)
    {
        vip.u.udp.src_port_en = 1;
    }
    if (mode & 0x2)
    {
        vip.u.udp.dst_port_en = 1;
    }
    if (mode & 0x4)
    {
        vip.u.udp.swap_port_en = 1;
    }
    return fe_lib_del_vip(&vip);
}

static int do_del_dscp(int argc, char *argv[], void *p)
{
    int mask;
    ECNT_FEMGR_L2LU_KEY_ENTRY l2lu_key = {0};

    if (argc < 5)
    {
        printf("fe del dscp <svlan> <cvlan> <dscp> <mask> \n");
        return 0;
    }

    l2lu_key.is_dscp = 1;
    l2lu_key.svid = strtoul(argv[1],NULL,16);
    l2lu_key.cvid = strtoul(argv[2],NULL,16);   
    l2lu_key.dscp_or_traffic_class = strtoul(argv[3],NULL,16);
    mask = strtoul(argv[4],NULL,16);

    if (mask & 0x1)
    {
        l2lu_key.svid_en = 1;
    }

    if (mask & 0x2)
    {
        l2lu_key.spcp_en= 1;
    }

    if (mask & 0x4)
    {
        l2lu_key.cvid_en= 1;
    }

    if (mask & 0x8)
    {
        l2lu_key.cpcp_en= 1;
    }

    if (mask & 0x10)
    {
        l2lu_key.dscp_mask_en= 1;
    }
    return fe_lib_del_l2lu_key(&l2lu_key);
}

static int do_del_trfc(int argc, char *argv[], void *p)
{
    int mask;
    ECNT_FEMGR_L2LU_KEY_ENTRY l2lu_key = {0};

    if (argc < 5)
    {
        printf("fe del trfc <svlan> <cvlan> <trfc> <mask> \n"); 
        return 0;
    }
    
    l2lu_key.is_dscp = 0;
    l2lu_key.svid = strtoul(argv[1],NULL,16);
    l2lu_key.cvid = strtoul(argv[2],NULL,16);   
    l2lu_key.dscp_or_traffic_class = strtoul(argv[3],NULL,16);
    mask = strtoul(argv[4],NULL,16);

    if (mask & 0x1)
    {
        l2lu_key.svid_en = 1;
    }

    if (mask & 0x2)
    {
        l2lu_key.spcp_en= 1;
    }

    if (mask & 0x4)
    {
        l2lu_key.cvid_en= 1;
    }

    if (mask & 0x8)
    {
        l2lu_key.cpcp_en= 1;
    }

    if (mask & 0x10)
    {
        l2lu_key.dscp_mask_en= 1;
    }
    return fe_lib_del_l2lu_key(&l2lu_key);
}

static cmds_t vip_delCmds[] = 
{
    {"eth",     do_del_eth,         0x0, 1, "fe del eth <type> \n"},
    {"ppp",     do_del_ppp,         0x0, 1, "fe del ppp <ppp protocol> \n"},
    {"ip",      do_del_ip,          0x0, 1, "fe del ip <protocol> \n"},
    {"tcp",     do_del_tcp,         0x0, 3, "fe del tcp <src> <dst> <mode> \n"},    
    {"udp",     do_del_udp,         0x0, 3, "fe del udp <src> <dst> <mode> \n"},    
    {"dscp",    do_del_dscp,        0x0, 4, "fe add dscp <svlan> <cvlan> <prio> <mask> \n"},    
    {"trfc",    do_del_trfc,        0x0, 4, "fe add trfc <svlan> <cvlan> <prio> <mask> \n"},    
    {NULL,      NULL,               0x0, 0, NULL}
};

static int do_vip_del(int argc, char *argv[], void *p)
{
    return subcmd(vip_delCmds, argc, argv, NULL);
}

static cmds_t vipCmds[] = 
{
    {"set",     do_vip_set,     0x0,  0, NULL},
    {"add",     do_vip_add,     0x0,  0, NULL},
    {"del",     do_vip_del,     0x0,  0, NULL},
    {NULL,      NULL,           0x0,  0, NULL}
};

/*******************************************************************************/
/*  CNT commands                                                                                                              */
/*******************************************************************************/
static int do_get_ok_cnt(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int gdm_sel = 0, txrx_sel = 0, cnt = 0;
        
    gdm_sel  = strtoul(argv[1], NULL, 10);
    txrx_sel = strtoul(argv[2], NULL, 10);
    if (txrx_sel)
    {
        ret = fe_lib_get_cnt(gdm_sel, ECNT_FEMGR_STATISTICS_TYPE_RX_OK, &cnt);
    }
    else
    {
        ret = fe_lib_get_cnt(gdm_sel, ECNT_FEMGR_STATISTICS_TYPE_TX_OK, &cnt);
    }

    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }

    printf("%u\n", cnt);
    return 0;
}

static int do_get_ok_byte_cnt(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int gdm_sel = 0, txrx_sel = 0, cnt = 0;
        
    gdm_sel  = strtoul(argv[1], NULL, 10);
    txrx_sel = strtoul(argv[2], NULL, 10);
    if (txrx_sel)
    {
        ret = fe_lib_get_cnt(gdm_sel, ECNT_FEMGR_STATISTICS_TYPE_RX_BYTE, &cnt);
    }
    else
    {
        ret = fe_lib_get_cnt(gdm_sel, ECNT_FEMGR_STATISTICS_TYPE_TX_BYTE, &cnt);
    }
    
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }

    printf("%u\n", cnt);
    return 0;
}

static int do_get_rx_err_crc_cnt(int argc, char *argv[], void *p)
{
	return fe_lib_get_rx_err_crc_cnt();
}

static int do_get_rx_drop_fifo_cnt(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int gdm_sel = 0, cnt = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    ret = fe_lib_get_cnt(gdm_sel, ECNT_FEMGR_STATISTICS_TYPE_RX_DROP_FIFO, &cnt);
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }

    printf("%u\n", cnt);
    return 0;
}

static int do_get_rx_drop_err_cnt(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int gdm_sel = 0, cnt = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    ret = fe_lib_get_cnt(gdm_sel, ECNT_FEMGR_STATISTICS_TYPE_RX_DROP_ERR, &cnt);
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }

    printf("%u\n", cnt);
    return 0;
}

static int do_get_tx_get_cnt(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int gdm_sel = 0, cnt = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    ret = fe_lib_get_cnt(gdm_sel, ECNT_FEMGR_STATISTICS_TYPE_TX_GET, &cnt);
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }

    printf("%u\n", cnt);
    return 0;
}

static int do_get_tx_drop_cnt(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int gdm_sel = 0, cnt = 0;

    gdm_sel = strtoul(argv[1], NULL, 10);
    ret = fe_lib_get_cnt(gdm_sel, ECNT_FEMGR_STATISTICS_TYPE_TX_DROP, &cnt);
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }

    printf("%u\n", cnt);
    return 0;
}

static int do_get_eth_all_cnt(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int gdm_sel = 0, txrx_sel = 0;
    ECNT_FEMGR_GDMA2_RX_STATISTICS rx_stats = {0};
    ECNT_FEMGR_GDMA2_TX_STATISTICS tx_stats = {0};

    txrx_sel = strtoul(argv[1], NULL, 10);
    
    if (txrx_sel)
    {
        ret = fe_lib_get_gdma2_rx_statistics(&rx_stats);
        if (ret != ECNT_FEMGR_SUCCESS)
        {
            printf("Failed to get gdma2 rx statistics!\n");
            return -1;
        }

        printf("\nGDMA2 RX statistics:\n"
               "oversize            :%llu\n"
               "undersize           :%llu\n"
               "frame_cnt           :%llu\n"
               "frame_len           :%llu\n"
               "drop_cnt            :%llu\n"
               "broadcast           :%llu\n"
               "multicast           :%llu\n"
               "crc                 :%llu\n"
               "fragment            :%llu\n"
               "jabber              :%llu\n"
               "less_64             :%llu\n"
               "more_1518           :%llu\n"
               "eq_64               :%llu\n"
               "from_65_to_127      :%llu\n"
               "from_128_to_255     :%llu\n"
               "from_256_to_511     :%llu\n"
               "from_512_to_1023    :%llu\n"
               "from_1024_to_1518   :%llu\n",
               rx_stats.oversize,
               rx_stats.undersize,
               rx_stats.frame_cnt,
               rx_stats.frame_len,
               rx_stats.drop_cnt,
               rx_stats.broadcast,
               rx_stats.multicast,
               rx_stats.crc,
               rx_stats.fragment,
               rx_stats.jabber,
               rx_stats.less_64,
               rx_stats.more_1518,
               rx_stats.eq_64,
               rx_stats.from_65_to_127,
               rx_stats.from_128_to_255,
               rx_stats.from_256_to_511,
               rx_stats.from_512_to_1023,
               rx_stats.from_1024_to_1518);
    }
    else
    {
        ret = fe_lib_get_gdma2_tx_statistics(&tx_stats);
        if (ret != ECNT_FEMGR_SUCCESS)
        {
            printf("Failed to get gdma2 rx statistics!\n");
            return -1;
        }
        printf("\nGDMA2 TX statistics:\n"
               "frame_cnt           :%llu\n"
               "frame_len           :%llu\n"
               "drop_cnt            :%llu\n"
               "broadcast           :%llu\n"
               "multicast           :%llu\n"
               "less_64             :%llu\n"
               "more_1518           :%llu\n"
               "eq_64               :%llu\n"
               "from_65_to_127      :%llu\n"
               "from_128_to_255     :%llu\n"
               "from_256_to_511     :%llu\n"
               "from_512_to_1023    :%llu\n"
               "from_1024_to_1518   :%llu\n",
               tx_stats.frame_cnt,
               tx_stats.frame_len,
               tx_stats.drop_cnt,
               tx_stats.broadcast,
               tx_stats.multicast,
               tx_stats.less_64,
               tx_stats.more_1518,
               tx_stats.eq_64,
               tx_stats.from_65_to_127,
               tx_stats.from_128_to_255,
               tx_stats.from_256_to_511,
               tx_stats.from_512_to_1023,
               tx_stats.from_1024_to_1518);
    }
    return 0;
}
static int do_get_eth_rx_cnt(int argc, char *argv[], void *p)
{
    int ret;
        
    ret = fe_lib_get_eth_rx_cnt();
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }
    return 0;
}

static int do_get_eth_tx_cnt(int argc, char *argv[], void *p)
{
    int ret;
        
    ret = fe_lib_get_eth_tx_cnt();
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }
    return 0;
}

static int do_get_eth_frame_cnt(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int txrx_sel = 0;
        
    txrx_sel = strtoul(argv[1], NULL, 10);
    ret = fe_lib_get_eth_frame_cnt(txrx_sel);
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }
    return 0;
}

static int do_get_eth_err_cnt(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int err_type = 0;
        
    err_type = strtoul(argv[1], NULL, 10);
    ret = fe_lib_get_eth_err_cnt(err_type);
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to retrieved statistics!\n");
        return -1;
    }
    return 0;
}

static int do_get_mac_dev(int argc, char *argv[], void *p)
{
	int ret = 0;
	int idx = 0;
	unsigned int dir = 0;
	unsigned long long pktcnt = 0;
	unsigned char dev_mac[6] = {0};
	unsigned long long bytecnt = 0;
	unsigned int mac_addr[6] = {0};
	
	dir = strtoul(argv[1],NULL,10);

	sscanf(argv[2], "%02X:%02X:%02X:%02X:%02X:%02X", &(mac_addr[0]), &(mac_addr[1]), 
		&(mac_addr[2]), &(mac_addr[3]), &(mac_addr[4]), &(mac_addr[5]));
	for(idx = 0 ;idx < 6; idx++)
	{
		dev_mac[idx] = (unsigned char)mac_addr[idx];
	}
	
	ret = ecnt_fe_lib_get_account(DEV_MODE,dir,dev_mac,&bytecnt,&pktcnt);
	
	printf("%s byte cnt is %llu, pkt cnt is %llu\n", dir?"upstream":"downstream",bytecnt,pktcnt);
	return ret;
}

static int do_get_wan_itf(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned char dir = 0;
	unsigned long long pktcnt = 0;
	char itf_name[16];
	unsigned long long bytecnt = 0;

	dir = atoi(argv[1]);
	memcpy(itf_name, argv[2], 16);

#if defined(TCSUPPORT_ACCOUNT_METER_V2)
	ret = fe_lib_get_wan_itf_pkt_account(dir, itf_name, &bytecnt, &pktcnt);
#else
	ret = ecnt_fe_lib_get_account(WAN_ITF_MODE,dir,itf_name,&bytecnt,&pktcnt);
#endif

	printf("wan(%s) %s byte cnt is %llu, pkt cnt is %llu\n", itf_name, dir?"up":"down", bytecnt, pktcnt);

	return ret;
}

static int do_get_traffic_flow(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned char dir = 0;
	unsigned long long pktcnt = 0;
	unsigned char flowid = 0;
	unsigned long long bytecnt = 0;

	dir = atoi(argv[1]);
	flowid = atoi(argv[2]);

	ret = ecnt_fe_lib_get_account(TRAFFIC_FLOW_MODE,dir,&flowid,&bytecnt,&pktcnt);

	printf("flow(%d) %s byte cnt is %llu, pkt cnt is %llu\n", flowid, dir?"up":"down", bytecnt,pktcnt);

	return ret;
}

static int do_get_llid(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned char dir = 0;
	unsigned long long pktcnt = 0;
	unsigned int llid = 0;
	unsigned long long bytecnt = 0;

	dir = atoi(argv[1]);
	llid = atoi(argv[2]);

	if ((llid < 0) || (llid > 7))
    {
        printf("llid should be 0~7!\n");
        return -1;
    }
	
#if defined(TCSUPPORT_ACCOUNT_METER_V2)	
	ret = fe_lib_get_llid_pkt_account(dir, llid, &bytecnt, &pktcnt);
	printf("llid(%d) %s byte cnt is %llu, pkt cnt is %llu\n", llid, dir?"up":"down", bytecnt, pktcnt);
#else
    printf("llid only support TCSUPPORT_ACCOUNT_METER_V2\n");
#endif

	return ret;
}

static int do_get_multicast(int argc, char *argv[], void *p)
{
	int ret = 0;
	int in_data = 0;
	unsigned long long pktcnt = 0;
	unsigned int bytecnt_L = 0;
	unsigned int bytecnt_H = 0;
	unsigned long long bytecnt = 0;

	ret = ecnt_fe_lib_get_account(MULTICAST_MODE,0,&in_data,&bytecnt,&pktcnt);

	printf("multicast byte cnt is %llu, pkt cnt is %llu\n",bytecnt,pktcnt);

	return ret;
}

static int do_clean_mac_dev(int argc, char *argv[], void *p)
{
	int ret = 0;
	int idx = 0; 
	unsigned char dev_mac[6] = {0};
	unsigned int dir = 0;
	unsigned int mac_addr[6] = {0};

	dir = strtoul(argv[1],NULL,10);

	sscanf(argv[2], "%02X:%02X:%02X:%02X:%02X:%02X", &(mac_addr[0]), &(mac_addr[1]), 
		&(mac_addr[2]), &(mac_addr[3]), &(mac_addr[4]), &(mac_addr[5]));

	for(idx = 0 ;idx < 6; idx++)
	{
		dev_mac[idx] = (unsigned char)mac_addr[idx];
	}

	ret = ecnt_fe_lib_clean_account(DEV_MODE,dir,dev_mac);

	printf("finish do_clean_mac_dev.\n");
	return ret;
}

static int do_clean_wan_itf(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned char dir = 0;
	char itf_name[16] = {0};

	dir = atoi(argv[1]);
	memcpy(itf_name, argv[2], 16);

	ret = ecnt_fe_lib_clean_account(WAN_ITF_MODE,dir,itf_name);

	printf("finish do_clean_wan_itf.\n");
	return ret;
}

static int do_clean_traffic_flow(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned char dir = 0;
	unsigned char flowid = 0;
	
	dir = atoi(argv[1]);
	flowid = atoi(argv[2]);
	
	ret = ecnt_fe_lib_clean_account(TRAFFIC_FLOW_MODE,dir,&flowid);

	printf("finish do_clean_traffic_flow.\n");
	return ret;
}

static int do_clean_llid(int argc, char *argv[], void *p)
{
	int ret = 0;
	unsigned char dir = 0;
	unsigned int llid = 0;
	
	dir = atoi(argv[1]);
	llid = atoi(argv[2]);
	
	ret = ecnt_fe_lib_clean_account(LLID_MODE,dir,&llid);

	printf("finish do_clean_llid.\n");
	return ret;
}

static int do_clean_multicast(int argc, char *argv[], void *p)
{
	int ret = 0;
	int in_data = 0;
	
	ret = ecnt_fe_lib_clean_account(MULTICAST_MODE,0,&in_data);
    
	printf("finish do_clean_multicast.\n");
    return ret;
}

static cmds_t cnt_getCmds[] = {
    {"ok_cnt",              do_get_ok_cnt,                  0x0,  2, "fe cnt get ok_cnt <gdm_sel> <txrx_sel>"},
    {"ok_byte_cnt",         do_get_ok_byte_cnt,             0x0,  2, "fe cnt get ok_byte_cnt <gdm_sel> <txrx_sel>"},
    {"rx_err_crc_cnt",      do_get_rx_err_crc_cnt,     	    0x0,  0, NULL},
    {"rx_drop_fifo_cnt",    do_get_rx_drop_fifo_cnt,        0x0,  1, "fe cnt get rx_drop_fifo_cnt <gdm_sel>"},
    {"rx_drop_err_cnt",     do_get_rx_drop_err_cnt,         0x0,  1, "fe cnt get rx_drop_err_cnt <gdm_sel>"},
    {"tx_get_cnt",          do_get_tx_get_cnt,              0x0,  1, "fe cnt get tx_get_cnt <gdm_sel>"},
    {"tx_drop_cnt",         do_get_tx_drop_cnt,             0x0,  1, "fe cnt get tx_drop_cnt <gdm_sel>"},
    {"gdma2",               do_get_eth_all_cnt,             0x0,  1, "fe cnt get gdma2 <txrx_sel>"},
    {"eth_rx_cnt",          do_get_eth_rx_cnt,              0x0,  0, "fe cnt get eth_rx_cnt"},
    {"eth_tx_cnt",          do_get_eth_tx_cnt,              0x0,  0, "fe cnt get eth_tx_cnt"},
    {"eth_frame_cnt",       do_get_eth_frame_cnt,           0x0,  1, "fe cnt get eth_frame_cnt <txrx_sel>"},
    {"eth_err_cnt",         do_get_eth_err_cnt,             0x0,  1, "fe cnt get eth_err_cnt <err_type:0(FE_ERR_CRC);1(ERR_LONG);2(ERR_RUNT)>"},
	{"mac_dev",             do_get_mac_dev,                 0x0,  1, "femgr cnt get mac_dev <dir:0(down),1(up)> <mac_address(X:X:X:X:X:X)>"},
    {"wan_itf",             do_get_wan_itf,                 0x0,  1, "femgr cnt get wan_itf <dir:0(down),1(up)> <itf name>"},
    {"traffic_flow",        do_get_traffic_flow,            0x0,  1, "femgr cnt get traffic_flow <dir:0(down),1(up)> <flowid(0~7)>"},
    {"multicast",           do_get_multicast,               0x0,  1, "femgr cnt get multicast"},
	{"llid",				do_get_llid,					0x0,  1, "femgr cnt get llid <dir:0(down),1(up)> <llid>"},
	{NULL,                  NULL,                           0x0,  0, NULL}
};

static cmds_t cnt_cleanCmds[] = {
    {"mac_dev",             do_clean_mac_dev,               0x0,  1, "femgr cnt clean mac_dev <dir:0(down),1(up)> <mac_address(X:X:X:X:X:X)>"},
    {"wan_itf",             do_clean_wan_itf,               0x0,  1, "femgr cnt clean wan_itf <dir:0(down),1(up)> <itf name>"},
    {"traffic_flow",        do_clean_traffic_flow,          0x0,  1, "femgr cnt clean traffic_flow <dir:0(down),1(up)> <flowid(0~7)>"},
    {"multicast",           do_clean_multicast,             0x0,  1, "femgr cnt clean multicast"},
    {"llid",				do_clean_llid,					0x0,  1, "femgr cnt clean llid <dir:0(down),1(up)> <llid>"},
	{NULL,                  NULL,                           0x0,  0, NULL}
};

static int do_cnt_get(int argc, char *argv[], void *p)
{
    return subcmd(cnt_getCmds, argc, argv, NULL);
}

static int do_cnt_clean(int argc, char *argv[], void *p)
{
    return subcmd(cnt_cleanCmds, argc, argv, NULL);
}

static int do_set_clear_mib(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, txrx_sel = 0;

    gdm_sel  = strtoul(argv[1], NULL, 10);
    txrx_sel = strtoul(argv[2], NULL, 10);
    return fe_lib_clear_gdma_statistics(gdm_sel, txrx_sel);
}

static cmds_t cntCmds[] = 
{
    {"get",     do_cnt_get,         0x0,  0, NULL},
	{"clean",   do_cnt_clean,       0x0,  0, NULL},
    {"clr",     do_set_clear_mib,   0x0,  2, "fe cnt clr <gdm_sel> <txrx_sel>"},
    {NULL,      NULL,               0x0,  0, NULL}
};

/*******************************************************************************/
/*  CHANNEL RETIRE commands                                                                                           */
/*******************************************************************************/
static int do_set_channel_retire(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, channel = 0, reg_val = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    channel = strtoul(argv[2], NULL, 10);
    reg_val = strtoul(argv[3], NULL, 10);
    return fe_lib_set_channel_retire(gdm_sel, channel, reg_val);
}

static int do_set_channel_retire_all(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, channel = 0;

    gdm_sel = strtoul(argv[1], NULL, 10);
    channel = strtoul(argv[2], NULL, 10);
    return fe_lib_set_channel_retire_all(gdm_sel, channel);
}

static int do_set_channel_retire_one(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, channel = 0;

    gdm_sel = strtoul(argv[1], NULL, 10);
    channel = strtoul(argv[2], NULL, 10);
    return fe_lib_set_channel_retire_one(gdm_sel, channel);
}

static cmds_t channel_retireCmds[] = 
{
    {"set",     do_set_channel_retire,          0x0,  3, "fe channel_retire set <gdm_sel> <channel> <mode>"},
    {"all",     do_set_channel_retire_all,      0x0,  2, "fe channel_retire all <gdm_sel> <channel>"},
    {"one",     do_set_channel_retire_one,      0x0,  2, "fe channel_retire one <gdm_sel> <channel>"},
    {NULL,      NULL,                           0x0,  0, NULL}
};

/*******************************************************************************/
/*  SET commands                                                                                                               */
/*******************************************************************************/
#ifdef TCSUPPORT_CSC_EEUROPE
static int do_show_tls(int argc, char *argv[], void *p)
{
    if (argc < 2)
    {
        printf("fe set tls show\n");
        return 0;
    }

    return fe_lib_show_tls_entry();
}
#endif

static int do_set_crc_strip(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, enable = 0;
        
    gdm_sel = strtoul(argv[1], NULL,10);
    enable  = strtoul(argv[2], NULL,10);
    return fe_lib_set_crc_strip(gdm_sel, enable);
}

static int do_set_padding(int argc, char *argv[], void *p)
{
    unsigned int enable = 0;
        
    enable = strtoul(argv[1], NULL, 10);
    return fe_lib_set_padding(enable);
}

static int do_set_vlan_check(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, enable = 0;
        
    gdm_sel = strtoul(argv[1], NULL, 10);
    enable  = strtoul(argv[2], NULL, 10);
    return fe_lib_set_vlan_check(gdm_sel, enable);
}

static int do_set_ins_vlan_tpid(int argc, char *argv[], void *p)
{
    unsigned int cdm_sel = 0, reg_val = 0;

    cdm_sel = strtoul(argv[1], NULL, 10);
    reg_val = strtoul(argv[2], NULL, 10);
    return fe_lib_set_insert_vlan_tpid(cdm_sel, reg_val);
}

static int do_set_vlan_enable(int argc, char *argv[], void *p)
{
    unsigned int cdm_sel = 0, enable = 0;

    cdm_sel = strtoul(argv[1], NULL, 10);
    enable  = strtoul(argv[2], NULL, 10);
    return fe_lib_set_vlan_enable(cdm_sel, enable);
}

static int do_set_tx_favor_oam_enable(int argc, char *argv[], void *p)
{
    unsigned int enable = 0;

    enable = strtoul(argv[1], NULL, 10);
    return fe_lib_set_tx_favor_oam_enable(enable);
}

static int do_set_tls_port(int argc, char *argv[], void *p)
{
    unsigned int enable = 0, tls_port = 0;

    enable   = strtoul(argv[1], NULL, 10);
    tls_port = strtoul(argv[2], NULL, 10);
    return fe_lib_set_tls_port(enable, tls_port);
}

static int do_set_tls_vlan(int argc, char *argv[], void *p)
{
    unsigned int enable = 0, tls_vlan = 0;

    enable   = strtoul(argv[1], NULL, 10);
    tls_vlan = strtoul(argv[3], NULL, 10);
    return fe_lib_set_tls_vlan(enable, tls_vlan);
}

static int do_set_fe_reset(int argc, char *argv[], void *p)
{
    unsigned int reset_mode = 0;

    reset_mode = strtoul(argv[1], NULL, 10);
    return fe_lib_reset_frame_engine(reset_mode);
}

static int do_set_pkt_length(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, length_long = 0, length_short = 0;
    
    gdm_sel      = strtoul(argv[1], NULL, 10);
    length_long  = strtoul(argv[2], NULL, 10);
    length_short = strtoul(argv[3], NULL, 10);
    
    return fe_lib_set_pkt_length(gdm_sel, length_long, length_short);
}

static int do_set_channel_enable(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, txrx_sel = 0, enable = 0, channel = 0;
    
    gdm_sel  = strtoul(argv[1], NULL,10);
    txrx_sel = strtoul(argv[2], NULL,10);
    enable   = strtoul(argv[3], NULL,10);
    channel  = strtoul(argv[4], NULL,10);
    return fe_lib_set_gdma_channel_enable(gdm_sel, txrx_sel, channel, enable);
}

static int do_set_time_stamp(int argc, char *argv[], void *p)
{
    unsigned int timeStamp = 0;

    timeStamp = strtoul(argv[1], NULL,10);
    return fe_lib_set_time_stamp(timeStamp);
}

static int do_set_hwfwd_channel(int argc, char *argv[], void *p)
{
    unsigned int cdm_sel = 0, channel = 0, enable;
        
    cdm_sel = strtoul(argv[1], NULL, 10);
    channel = strtoul(argv[2], NULL, 10);
    enable  = strtoul(argv[3], NULL, 10);
    return fe_lib_set_cdma_channel_enable(cdm_sel, channel, enable);
}

static int do_set_ext_tpid(int argc, char *argv[], void *p)
{
    unsigned int reg_val = 0;
        
    reg_val = strtoul(argv[1],NULL,16);
    return fe_lib_set_ext_tpid(reg_val);
}

static int do_set_fw_cfg(int argc, char *argv[], void *p)
{
    unsigned int gdm_sel = 0, pkt_type = 0, dst_port = 0;
        
    gdm_sel   = strtoul(argv[1], NULL, 10);
    pkt_type  = strtoul(argv[2], NULL, 10);
    dst_port  = strtoul(argv[3], NULL, 10);
    return fe_lib_set_fw_cfg(gdm_sel, pkt_type, dst_port);
}

static int do_set_pppoe_reset(int argc, char *argv[], void *p)	
{
	return fe_lib_set_pppoe_info_clean();
}

static int do_set_mc_vlan_global(int argc, char *argv[], void *p)
{
	ECNT_FEMGR_RET ret = ECNT_FEMGR_SUCCESS;
	ECNT_FEMGR_ENABLE_MODE enable = ECNT_FEMGR_DISABLE;

	if(argc<2)
		goto err;

	/* 1. get enable info */
	if(strncasecmp(argv[1], "enable", strlen("enable")) == 0) {
		enable = ECNT_FEMGR_ENABLE;
	} else if(strncasecmp(argv[1], "disable", strlen("disable")) == 0) {
		enable = ECNT_FEMGR_DISABLE;
	} else {
		goto err;
	}

	/* 2. call api to set */
	ret = fe_lib_set_mc_vlan_global(enable);
	if( ret == ECNT_FEMGR_NOT_SUPPORT ){
		printf("fe set mc_vlan: not support \n");
	}else if ( ret != ECNT_FEMGR_SUCCESS ){
		printf("fe set mc_vlan: cmd fail \n");
	}

	return 0;

err:
	printf("Format: fe set mc_vlan global enable/disable\n") ;
	return -1;
}

static int do_set_mc_vlan_table_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_FEMGR_RET ret = ECNT_FEMGR_SUCCESS;
	ECNT_FEMGR_MC_VLAN_TABLE_t mcVlanTableCfg;

	if(argc<4)
		goto err;

	memset(&mcVlanTableCfg, 0, sizeof(ECNT_FEMGR_MC_VLAN_TABLE_t));

	/* 1. get table index info */
	mcVlanTableCfg.mc_table_id = strtoul(argv[1], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input mc_table_id is '%s', which should be digital\n", argv[1]);
		goto err;
	}

	/* 2. get table enable info */
	if(strncasecmp(argv[2], "enable", strlen("enable")) == 0) {
		mcVlanTableCfg.enable = ECNT_FEMGR_ENABLE;
	} else if(strncasecmp(argv[2], "disable", strlen("disable")) == 0) {
		mcVlanTableCfg.enable = ECNT_FEMGR_DISABLE;
	} else {
		goto err;
	}

	/* 3. get ppe entry index info */
	mcVlanTableCfg.ppe_entry_id = strtoul(argv[3], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input ppe_entry_id is '%s', which should be digital\n", argv[3]);
		goto err;
	}

	/* 4. call api to set */
	ret = fe_lib_set_mc_vlan_table_cfg(&mcVlanTableCfg);
	if( ret == ECNT_FEMGR_NOT_SUPPORT ){
		printf("fe set mc_vlan: not support \n");
	}else if ( ret != ECNT_FEMGR_SUCCESS ){
		printf("fe set mc_vlan: cmd fail \n");
	}

	return 0;

err:
	printf("Format: fe set mc_vlan table_cfg <table_id (0-63)> <enable/disable> <ppe_entry_id>\n") ;
	return -1;
}

static int do_set_mc_vlan_action_cfg(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_FEMGR_RET ret = ECNT_FEMGR_SUCCESS;
	ECNT_FEMGR_MC_VLAN_ACTION_t mcVlanActionCfg;

	if(argc<6)
		goto err;

	memset(&mcVlanActionCfg, 0, sizeof(ECNT_FEMGR_MC_VLAN_ACTION_t));

	/* 1. get table index info */
	mcVlanActionCfg.mc_table_id = strtoul(argv[1], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input mc_table_id is '%s', which should be digital\n", argv[1]);
		goto err;
	}

	/* 2. get port index info */
	mcVlanActionCfg.mc_chnl_id = strtoul(argv[2], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input mc_chnl_id is '%s', which should be digital\n", argv[2]);
		goto err;
	}

	/* 3. get vlan operation info */
	if(strncasecmp(argv[3], "keep", strlen("keep")) == 0) {
		mcVlanActionCfg.mc_vlan_op = ECNT_FEMGR_MC_VLAN_KEEP;
	} else if(strncasecmp(argv[3], "replace", strlen("replace")) == 0) {
		mcVlanActionCfg.mc_vlan_op = ECNT_FEMGR_MC_VLAN_REPLACE;
	} else if(strncasecmp(argv[3], "add", strlen("add")) == 0) {
		mcVlanActionCfg.mc_vlan_op = ECNT_FEMGR_MC_VLAN_ADD;
	} else if(strncasecmp(argv[3], "remove", strlen("remove")) == 0) {
		mcVlanActionCfg.mc_vlan_op = ECNT_FEMGR_MC_VLAN_REMOVE;
	} else {
		goto err;
	}

	/* 4. get vlan operation info */
	if(strncasecmp(argv[4], "keep", strlen("keep")) == 0) {
		mcVlanActionCfg.mc_vlan_vpm = ECNT_FEMGR_MC_VPM_KEEP;
	} else if(strncasecmp(argv[4], "8100", strlen("8100")) == 0) {
		mcVlanActionCfg.mc_vlan_vpm = ECNT_FEMGR_MC_VPM_8100;
	} else if(strncasecmp(argv[4], "88a8", strlen("88a8")) == 0) {
		mcVlanActionCfg.mc_vlan_vpm = ECNT_FEMGR_MC_VPM_88a8;
	} else if(strncasecmp(argv[4], "csr", strlen("csr")) == 0) {
		mcVlanActionCfg.mc_vlan_vpm = ECNT_FEMGR_MC_VPM_CSR;
	} else {
		goto err;
	}

	/* 5. get ppe entry index info */
	mcVlanActionCfg.mc_vlan = strtoul(argv[5], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input mc_vlan is '%s', which should be digital\n", argv[5]);
		goto err;
	}

	/* 4. call api to set */
	ret = fe_lib_set_mc_vlan_action_cfg(&mcVlanActionCfg);
	if( ret == ECNT_FEMGR_NOT_SUPPORT ){
		printf("fe set mc_vlan: not support \n");
	}else if ( ret != ECNT_FEMGR_SUCCESS ){
		printf("fe set mc_vlan: cmd fail \n");
	}

	return 0;

err:
	printf("Format: fe set mc_vlan action_cfg <table_id (0-63)> <port_id (0-15)> <op (keep/replace/add/remove)>\n");
	printf("<vpm (keep/8100/88a8/csr)> <vlan (0xXXXX)>\n") ;
	return -1;
}

static int do_set_mc_vlan_clear(int argc, char *argv[], void *p)
{
	char *endPtr = NULL;
	ECNT_FEMGR_RET ret = ECNT_FEMGR_SUCCESS;
	ECNT_FEMGR_ENABLE_MODE enable = ECNT_FEMGR_DISABLE;
	unsigned char mc_table_id = 0;

	if(argc<2)
		goto err;

	/* 1. get table index info */
	mc_table_id = strtoul(argv[1], &endPtr, 0);
	if(*endPtr!='\0') {
		printf("Error: input mc_table_id is '%s', which should be digital\n", argv[1]);
		goto err;
	}

	/* 2. set table info clear */
	ret = fe_lib_set_mc_vlan_clear_all(mc_table_id);
	if( ret == ECNT_FEMGR_NOT_SUPPORT ){
		printf("fe set mc_vlan: not support \n");
	}else if ( ret != ECNT_FEMGR_SUCCESS ){
		printf("fe set mc_vlan: cmd fail \n");
	}

	return 0;

err:
	printf("Format: fe set mc_vlan clear <table_id (0-63)>\n") ;
	return -1;
}

static cmds_t setMcVlanCmd[] = {
    {"global", 		do_set_mc_vlan_global,   		0x0, 0, NULL},
    {"table_cfg",	do_set_mc_vlan_table_cfg,   	0x0, 0, NULL},
    {"action_cfg",	do_set_mc_vlan_action_cfg,   	0x0, 0, NULL},
    {"clear",		do_set_mc_vlan_clear,   		0x0, 0, NULL},
    {NULL,  NULL,       		0x0, 0, NULL}
};

static int do_set_mc_vlan(int argc, char *argv[], void *p)
{
	return subcmd(setMcVlanCmd, argc, argv, NULL);
}

static int do_set_dev_stat_ratelimit_mode(int argc, char *argv[], void *p)
{
    unsigned int wan_conn_type = 0;
    char wan_itf_name[16];
    wan_conn_type = atoi(argv[1]);
    memcpy(wan_itf_name, argv[2], 16);
    return fe_lib_set_dev_stat_ratelimit_mode(wan_conn_type, wan_itf_name);
}

static cmds_t setCmds[] =
{
    #ifdef TCSUPPORT_CSC_EEUROPE
    {"tls",                 do_show_tls,                    0x0, 0, NULL},   
    #endif
    {"crc_strip",           do_set_crc_strip,               0x0, 2, "<gdm_sel> <enable>"},
    {"padding",             do_set_padding,                 0x0, 1, "<enable>"},
    {"vlan_check",          do_set_vlan_check,              0x0, 2, "<gdm_sel> <enable>"},
    {"ins_vlan_tpid",       do_set_ins_vlan_tpid,           0x0, 2, "<cdm_sel> <enable>"},
    {"vlan_enable",         do_set_vlan_enable,             0x0, 2, "<cdm_sel> <enable>"},
    {"tx_oam_enable",       do_set_tx_favor_oam_enable,     0x0, 1, "<enable>"},
    {"tls_port",            do_set_tls_port,                0x0, 2, "<enable> <tls_port>"},
    {"tls_vlan",            do_set_tls_vlan,                0x0, 2, "<enable> <tls_vlan>"},
    {"fe_reset",            do_set_fe_reset,                0x0, 1, "<reset_mode>"},
    {"pkt_len",             do_set_pkt_length,              0x0, 3, "<gdm_sel> <length_long> <length_short>"},
    {"channel_enable",      do_set_channel_enable,          0x0, 4, "<gdm_sel> <txrx_sel> <enable> <channel>"},
    {"time_stamp",          do_set_time_stamp,              0x0, 1, "<timestamp>"}, 
    {"hwfwd_channel",       do_set_hwfwd_channel,           0x0, 3, "<cdm_sel> <channel> <enable>"},
    {"ext_tpid",            do_set_ext_tpid,                0x0, 1, "reg_val"},
    {"fw_cfg",              do_set_fw_cfg,                  0x0, 3, "<gdm_sel> <pkt_type> <dst_port>"},
    {"pppoe_info",          do_set_pppoe_reset,             0x0, 0, NULL},
    {"mc_vlan",				do_set_mc_vlan,					0x0, 1, "fe set mc_vlan global/table_cfg/action_cfg/clear ...\n"},
    {"dev_stat_ratelimit_mode",		do_set_dev_stat_ratelimit_mode, 	0x0, 2, "<wan_conn_type> <wan_itf_name>"},
    {NULL,                  NULL,                           0x0, 0, NULL}
};

/*******************************************************************************/
/*  GET commands                                                                                                               */
/*******************************************************************************/
static int do_get_time_stamp(int argc, char *argv[], void *p)
{
    int ret;
    unsigned int ts = 0;
    
    ret =  fe_lib_get_time_stamp(&ts);
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to get time stamp!\n");
        return -1;
    }

    printf("TimeStamp: %u\n", ts);
    return 0;
}

static int do_get_ext_tpid(int argc, char *argv[], void *p)
{
    int ret;
    unsigned short tpid = 0;
    
    ret =  fe_lib_get_ext_tpid(&tpid);
    if (ret != ECNT_FEMGR_SUCCESS)
    {
        printf("Failed to get ext tpid!\n");
        return -1;
    }

    printf("EXT TPID: %x\n", tpid);
    return 0;
}

static int do_get_pppoe_info(int argc, char *argv[], void *p)
{
	ECNT_PPPOE_INFO_DATA pppoe_info = {0}; 
	int ret = 0;

	ret = fe_lib_get_pppoe_info(&pppoe_info);
	if (ret != ECNT_FEMGR_SUCCESS)
	{
	    printf("Failed to get pppoe info!\n");
	    return -1;
	}

	printf("valid: %d \n"
		   "pppoe SessionId: %d \n"
		   "wan_name: %s \n"
		   "smac: %02X:%02X:%02X:%02X:%02X:%02X \n"
		   "dmac: %02X:%02X:%02X:%02X:%02X:%02X \n"
		   "scr_ip: %d.%d.%d.%d \n"
		   "dst_ip: %d.%d.%d.%d \n"
		   "dns_valid: %d \n"
		   "pri_dns: %d.%d.%d.%d \n"
		   "snd_dns: %d.%d.%d.%d \n",
		   pppoe_info.valid,
		   pppoe_info.sessionID,
		   pppoe_info.ifname,
		   pppoe_info.smac_add[0],pppoe_info.smac_add[1],pppoe_info.smac_add[2],
		   pppoe_info.smac_add[3],pppoe_info.smac_add[4],pppoe_info.smac_add[5],
		   pppoe_info.dmac_add[0],pppoe_info.dmac_add[1],pppoe_info.dmac_add[2],
		   pppoe_info.dmac_add[3],pppoe_info.dmac_add[4],pppoe_info.dmac_add[5],
		   ((pppoe_info.src_ip) &(0xFF000000))>>24,((pppoe_info.src_ip)&(0x00FF0000))>>16, 
			((pppoe_info.src_ip)&(0x0000FF00))>>8, ((pppoe_info.src_ip)&(0x000000FF)), 
			((pppoe_info.dst_ip) &(0xFF000000))>>24,((pppoe_info.dst_ip)&(0x00FF0000))>>16, 
			((pppoe_info.dst_ip)&(0x0000FF00))>>8, ((pppoe_info.dst_ip)&(0x000000FF)),
			pppoe_info.dns_valid,
			(pppoe_info.pri_dns & 0xFF000000)>>24, (pppoe_info.pri_dns & 0xFF0000)>>16,
			(pppoe_info.pri_dns & 0xFF00)>>8, (pppoe_info.pri_dns & 0xFF),
			(pppoe_info.snd_dns & 0xFF000000)>>24, (pppoe_info.snd_dns & 0xFF0000)>>16,
			(pppoe_info.snd_dns & 0xFF00)>>8, (pppoe_info.snd_dns & 0xFF)
			);

	return 0;
}

static int do_get_mc_vlan(int argc, char *argv[], void *p)
{
	ECNT_FEMGR_RET ret = ECNT_FEMGR_SUCCESS;
	ECNT_FEMGR_ENABLE_MODE enable = ECNT_FEMGR_DISABLE;
	ECNT_FEMGR_MC_VLAN_TABLE_t mcVlanTableCfg;
	ECNT_FEMGR_MC_VLAN_ACTION_t mcVlanActionCfg;
	unsigned char mc_table_id = 0;
	unsigned char table_index = 0;
	unsigned char chnl_index = 0;

	unsigned char mc_vlan_op[4][10] = {"keep", "replace", "add", "remove"};
	unsigned char mc_vlan_vpm[4][20] = {"keep", "0x8100", "0x88a8", "csr extend tpid"};
		
	if(argc < 2){
		printf("fe get mc_vlan <table_id (0-63)> \n");
		return 0;
	}

	memset(&mcVlanTableCfg, 0, sizeof(ECNT_FEMGR_MC_VLAN_TABLE_t));
	memset(&mcVlanActionCfg, 0, sizeof(ECNT_FEMGR_MC_VLAN_ACTION_t));

	mc_table_id = (unsigned char)strtoul(argv[1],NULL,10);
	
	/* 1. get global enable/disable info */
	ret = fe_lib_get_mc_vlan_global(&enable);
	if( ret == ECNT_FEMGR_NOT_SUPPORT ){
		printf("fe get mc_vlan: not support \n");
	}else if ( ret != ECNT_FEMGR_SUCCESS ){
		printf("fe get mc_vlan: cmd fail \n");
	}
	printf("HWF multicast 1:N action: %s\n", (enable == ECNT_FEMGR_ENABLE)?"ENABLE":"DISABLE");

	for(table_index = 0 ; table_index < ECNT_FEMGR_MC_VLAN_TABLE_MAX ; table_index++)
	{
		if( mc_table_id <  ECNT_FEMGR_MC_VLAN_TABLE_MAX )
		{
			if( table_index != mc_table_id )
				continue ;
		}
		/* 2. get per-table global info */
		mcVlanTableCfg.mc_table_id = table_index;
		ret = fe_lib_get_mc_vlan_table_cfg(&mcVlanTableCfg);
		if( ret == ECNT_FEMGR_NOT_SUPPORT ){
			printf("fe get mc_vlan: not support \n");
		}else if ( ret != ECNT_FEMGR_SUCCESS ){
			printf("fe get mc_vlan: cmd fail \n");
		}

		printf("Table-%d, %s, PPE_entry-%d:\n", mcVlanTableCfg.mc_table_id, 
									(mcVlanTableCfg.enable == ECNT_FEMGR_ENABLE)?"enable":"disable",
									mcVlanTableCfg.ppe_entry_id);


		for(chnl_index = 0 ; chnl_index < ECNT_FEMGR_MC_VLAN_PORT_MAX ; chnl_index++)
		{
			/* 3. get per-port vlan action info */
			mcVlanActionCfg.mc_table_id = table_index;
			mcVlanActionCfg.mc_chnl_id = chnl_index;
			ret = fe_lib_get_mc_vlan_action_cfg(&mcVlanActionCfg);
			if( ret == ECNT_FEMGR_NOT_SUPPORT ){
				printf("fe get mc_vlan: not support \n");
			}else if ( ret != ECNT_FEMGR_SUCCESS ){
				printf("fe get mc_vlan: cmd fail \n");
			}
			printf("Chnl-%02d, op-%s, vpm-%s, vlan-0x%04X;\t\t", mcVlanActionCfg.mc_chnl_id, 
									mc_vlan_op[mcVlanActionCfg.mc_vlan_op],
									mc_vlan_vpm[mcVlanActionCfg.mc_vlan_vpm],
									mcVlanActionCfg.mc_vlan);
			if((chnl_index % 2) == 1)
				printf("\n");
		}

	}

	return 0;
}

static cmds_t getCmds[] = 
{
    {"time_stamp",      do_get_time_stamp,          0x0, 0, NULL},
    {"ext_tpid",        do_get_ext_tpid,            0x0, 0, NULL},
    {"pppoe_info",  	do_get_pppoe_info,          0x0, 0, NULL},
    {"mc_vlan",			do_get_mc_vlan,				0x0, 1, "fe get mc_vlan <table_id (0-63)> "},
    {NULL,              NULL,                       0x0, 0, NULL}
};

/*******************************************************************************/
/*  ADD commands                                                                                                              */
/*******************************************************************************/
#ifdef TCSUPPORT_CSC_EEUROPE
static int do_add_tls(int argc, char *argv[], void *p)
{
    ECNT_FEMGR_TLS_ENTRY tls_info;

    if (argc < 5)
    {
        printf("fe add tls <uni_port_id> <uni_vlan> <gemport> <ani_vlan> \n");
        return 0;
    }
    tls_info.uni_port_id = strtoul(argv[1],NULL,10);
    tls_info.uni_vlan    = strtoul(argv[2],NULL,10);
    tls_info.gem_port    = strtoul(argv[3],NULL,10);
    tls_info.ani_vlan    = strtoul(argv[4],NULL,10);

    return fe_lib_add_tls_entry(tls_info);
}
#endif

static int do_add_stb_ip4(int argc, char *argv[], void *p)
{
	unsigned int tmp1=0, tmp2=0, tmp3=0, tmp4=0;
    unsigned int ip4=0;

	if (argc < 2)
	{
		printf("fe add stb_ip4 <src_ip(example: 192.168.1.10)> \n");
		return 0;
	}
	if(sscanf(argv[1], "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4) == 0)
	{
		printf("fe add stb_ip4 <src_ip(example: 192.168.1.10)> \n");
		return 0;
	}
    
    ip4 = (tmp1<<24) | (tmp2<<16) | (tmp3<<8) | (tmp4);

	return fe_lib_add_stb_src_ipv4(ip4);
}

static int do_add_stb_ip6(int argc, char *argv[], void *p)
{
	if (argc < 2)
	{
		printf("fe add stb_ip6 <src_ip(example: 2001::1)> \n");
		return 0;
	}

	return fe_lib_add_stb_src_ipv6(argv[1]);
}

static cmds_t addCmds[] = {
    #ifdef TCSUPPORT_CSC_EEUROPE
    {"tls",     do_add_tls,        0x0, 0, NULL},  
    #endif
    {"stb_ip4", do_add_stb_ip4,  0x0,    0,  NULL},
    {"stb_ip6", do_add_stb_ip6,  0x0,    0,  NULL},
    {NULL,      NULL,              0x0, 0, NULL}
};

/*******************************************************************************/
/*  DEL commands                                                                                                               */
/*******************************************************************************/
#ifdef TCSUPPORT_CSC_EEUROPE
static int do_del_tls(int argc, char *argv[], void *p)
{
    ECNT_FEMGR_TLS_ENTRY tls_info;

    if (argc < 5)
    {
        printf("fe del tls <uni_port_id> <uni_vlan> <gemport> <ani_vlan> \n");
        return 0;
    }
    tls_info.uni_port_id = strtoul(argv[1],NULL,10);
    tls_info.uni_vlan= strtoul(argv[2],NULL,10);
    tls_info.gem_port= strtoul(argv[3],NULL,10);
    tls_info.ani_vlan= strtoul(argv[4],NULL,10);

    return fe_lib_del_tls_entry(tls_info);
}
#endif

static int do_del_stb_ip4(int argc, char *argv[], void *p)
{
	unsigned int tmp1=0, tmp2=0, tmp3=0, tmp4=0;
    unsigned int ip4=0;

	if (argc < 2)
	{
		printf("fe del stb_ip4 <src_ip(example: 192.168.1.10)> \n");
		return 0;
	}
	if(sscanf(argv[1], "%d.%d.%d.%d", &tmp1, &tmp2, &tmp3, &tmp4) == 0)
	{
		printf("fe del stb_ip4 <src_ip(example: 192.168.1.10)> \n");
		return 0;
	}
    
    ip4 = (tmp1<<24) | (tmp2<<16) | (tmp3<<8) | (tmp4);

	return fe_lib_del_stb_src_ipv4(ip4);
}

static int do_del_stb_ip6(int argc, char *argv[], void *p)
{
	if (argc < 2)
	{
		printf("fe del stb_ip6 <src_ip(example: 2001::1)> \n");
		return 0;
	}

	return fe_lib_del_stb_src_ipv6(argv[1]);
}

static cmds_t delCmds[] = {
    #ifdef TCSUPPORT_CSC_EEUROPE
    {"tls",     do_del_tls,         0x0, 0, NULL},  
    #endif
    {"stb_ip4", do_del_stb_ip4,  0x0,    0,  NULL},
    {"stb_ip6", do_del_stb_ip6,  0x0,    0,  NULL},
    {NULL,      NULL,               0x0, 0, NULL}
};

/*******************************************************************************/
/*  L2B commands                                                                                                               */
/*******************************************************************************/
static int do_set_black_list(int argc, char *argv[], void *p)
{
    u8 enable = 0;
    
    enable = strtoul(argv[1], NULL, 10);
    return fe_lib_set_l2b_black_list(enable);
}

static int do_set_ether_type(int argc, char *argv[], void *p)
{
    unsigned int index = 0, enable =0, is_pppoe = 0, value = 0;

    index       = strtoul(argv[1], NULL, 10);
    enable      = strtoul(argv[2], NULL, 10);
    is_pppoe    = strtoul(argv[3], NULL, 10);
    value       = strtoul(argv[4], NULL, 10);
    return fe_lib_set_l2b_ether_type(index, enable, is_pppoe, value);
}

static cmds_t l2bCmds[] = 
{
    {"black_list",      do_set_black_list,      0x0,  1, "<enable>"},
    {"set_type",        do_set_ether_type,      0x0,  4, "<index> <enable> <is_pppoe> <value>"},
    {NULL,              NULL,                   0x0,  0, NULL}
};

static int do_add_rx_mac_filter(int argc, char *argv[], void *p)
{
	unsigned int mactemp[6] = {0};
	ECNT_FEMGR_MAC_FILTER_INFO macfilter_cfg;

	memset(&macfilter_cfg, 0, sizeof(ECNT_FEMGR_MAC_FILTER_INFO));
	macfilter_cfg.cmd_id = ECNT_FEMGR_MAC_FILTER_ADD;
	macfilter_cfg.gdm_sel = strtoul(argv[1], NULL, 10);

	sscanf(argv[2], "%02x:%02x:%02x:%02x:%02x:%02x", &mactemp[0], &mactemp[1], 
		&mactemp[2], &mactemp[3], &mactemp[4], &mactemp[5]);

	macfilter_cfg.mac[0] = (unsigned char)mactemp[0];
	macfilter_cfg.mac[1] = (unsigned char)mactemp[1];
	macfilter_cfg.mac[2] = (unsigned char)mactemp[2];
	macfilter_cfg.mac[3] = (unsigned char)mactemp[3];
	macfilter_cfg.mac[4] = (unsigned char)mactemp[4];
	macfilter_cfg.mac[5] = (unsigned char)mactemp[5];

    return fe_lib_set_rx_mac_filter(macfilter_cfg);
}

static int do_del_rx_mac_filter(int argc, char *argv[], void *p)
{
	unsigned int mactemp[6] = {0};
	ECNT_FEMGR_MAC_FILTER_INFO macfilter_cfg;

	memset(&macfilter_cfg, 0, sizeof(ECNT_FEMGR_MAC_FILTER_INFO));
	macfilter_cfg.cmd_id = ECNT_FEMGR_MAC_FILTER_DEL;
	macfilter_cfg.gdm_sel = strtoul(argv[1], NULL, 10);

	sscanf(argv[2], "%02x:%02x:%02x:%02x:%02x:%02x", &mactemp[0], &mactemp[1], 
		&mactemp[2], &mactemp[3], &mactemp[4], &mactemp[5]);

	macfilter_cfg.mac[0] = (unsigned char)mactemp[0];
	macfilter_cfg.mac[1] = (unsigned char)mactemp[1];
	macfilter_cfg.mac[2] = (unsigned char)mactemp[2];
	macfilter_cfg.mac[3] = (unsigned char)mactemp[3];
	macfilter_cfg.mac[4] = (unsigned char)mactemp[4];
	macfilter_cfg.mac[5] = (unsigned char)mactemp[5];

    return fe_lib_set_rx_mac_filter(macfilter_cfg);
}

static int do_disable_rx_mac_filter(int argc, char *argv[], void *p)
{
	ECNT_FEMGR_MAC_FILTER_INFO macfilter_cfg;

	memset(&macfilter_cfg, 0, sizeof(ECNT_FEMGR_MAC_FILTER_INFO));
	macfilter_cfg.cmd_id = ECNT_FEMGR_MAC_FILTER_DISABLE;
	macfilter_cfg.gdm_sel = strtoul(argv[1], NULL, 10);

    return fe_lib_set_rx_mac_filter(macfilter_cfg);
}

static int do_enable_rx_mac_filter(int argc, char *argv[], void *p)
{
	ECNT_FEMGR_MAC_FILTER_INFO macfilter_cfg;

	memset(&macfilter_cfg, 0, sizeof(ECNT_FEMGR_MAC_FILTER_INFO));
	macfilter_cfg.cmd_id = ECNT_FEMGR_MAC_FILTER_ENABLE;
	macfilter_cfg.gdm_sel = strtoul(argv[1], NULL, 10);
	macfilter_cfg.pkt_type = strtoul(argv[2], NULL, 10);
    macfilter_cfg.mac_sel = strtoul(argv[3], NULL, 10);
    macfilter_cfg.action = strtoul(argv[4], NULL, 10);
    macfilter_cfg.list_type = strtoul(argv[5], NULL, 10);

    return fe_lib_set_rx_mac_filter(macfilter_cfg);
}

static cmds_t RxMacFilterCmds[] = 
{
    {"enable",	do_enable_rx_mac_filter,	0x0,  5, "<gdm_sel> <pkt_type> <mac_sel> <action> <list_type>"},
	{"add",		do_add_rx_mac_filter,       0x0,  2, "<gdm_sel> <mac>"},
	{"del",		do_del_rx_mac_filter,       0x0,  2, "<gdm_sel> <mac>"},
    {"disable",	do_disable_rx_mac_filter,   0x0,  1, "<gdm_sel>"},
    {NULL,		NULL,                   	0x0,  0, NULL}
};

// force slow cmds
static int do_force_slow_enable(int argc, char *argv[], void *p)
{
	u8 enable = 0;
    
    enable = strtoul(argv[1], NULL, 10);
    return fe_lib_set_force_slow_enable(enable);
}

static int do_force_slow_set_duty(int argc, char *argv[], void *p)
{
	unsigned int fix_duty = 0, fast_duty =0, slow_duty = 0;

    fix_duty       = strtoul(argv[1], NULL, 10);
    fast_duty      = strtoul(argv[2], NULL, 10);
    slow_duty      = strtoul(argv[3], NULL, 10);
    return fe_lib_set_force_slow_duty(fix_duty, fast_duty, slow_duty);
}

static cmds_t ForceSlowCmds[] = 
{
    {"enable",	do_force_slow_enable,	 0x0,  1, "<enable 1/enable 0>"},
    {"set",		do_force_slow_set_duty,  0x0,  3, "<fix_duty> <fast_duty> <slow_duty>"},
    {NULL,		NULL,                    0x0,  0, NULL}
};

/*******************************************************************************/
/*  FE commands                                                                                                                 */
/*******************************************************************************/
static int do_ratelimit(int argc, char *argv[], void *p)
{
    return subcmd(ratelimitCmds, argc, argv, NULL);
}

static int do_mac(int argc, char *argv[], void *p)
{
    return subcmd(macCmds, argc, argv, NULL);
}

static int do_drop(int argc, char *argv[], void *p)
{
    return subcmd(dropCmds, argc, argv, NULL);
}

static int do_pcp(int argc, char *argv[], void *p)
{
    return subcmd(pcpCmds, argc, argv, NULL);
}

static int do_vip(int argc, char *argv[], void *p)
{
    return subcmd(vipCmds, argc, argv, NULL);
}

static int do_cnt(int argc, char *argv[], void *p)
{
    return subcmd(cntCmds, argc, argv, NULL);
}

static int do_channel_retire(int argc, char *argv[], void *p)
{
    return subcmd(channel_retireCmds, argc, argv, NULL);
}

static int do_set(int argc, char *argv[], void *p)
{
    return subcmd(setCmds, argc, argv, NULL);
}

static int do_get(int argc, char *argv[], void *p)
{
    return subcmd(getCmds, argc, argv, NULL);
}

static int do_add(int argc, char *argv[], void *p)
{
    return subcmd(addCmds, argc, argv, NULL);
}

static int do_del(int argc, char *argv[], void *p)
{
    return subcmd(delCmds, argc, argv, NULL);
}

static int do_l2b(int argc, char *argv[], void *p)
{
    return subcmd(l2bCmds, argc, argv, NULL);
}

static int do_rx_mac_filter(int argc, char *argv[], void *p)
{
    return subcmd(RxMacFilterCmds, argc, argv, NULL);
}

static int do_force_slow(int argc, char *argv[], void *p)
{
    return subcmd(ForceSlowCmds, argc, argv, NULL);
}

static cmds_t feCmds[] = 
{
    {"ratelimit",       do_ratelimit,       0x0,  0, NULL},
    {"mac",             do_mac,             0x0,  0, NULL},
    {"drop",            do_drop,            0x0,  0, NULL},
    {"pcp",             do_pcp,             0x0,  0, NULL},
    {"vip",             do_vip,             0x0,  0, NULL},
    {"cnt",             do_cnt,             0x0,  0, NULL},
    {"channel_retire",  do_channel_retire,  0x0,  0, NULL},
    {"set",             do_set,             0x0,  0, NULL},
    {"get",             do_get,             0x0,  0, NULL},
    {"add",             do_add,             0x0,  0, NULL},
    {"del",             do_del,             0x0,  0, NULL},
    {"l2b",             do_l2b,             0x0,  0, NULL},
    {"rx_mac_filter",   do_rx_mac_filter,   0x0,  0, NULL},
    {"force_slow",   	do_force_slow,      0x0,  0, NULL},
    {NULL,              NULL,               0x0,  0, NULL}
};

int main(int argc, char **argv) 
{
    return subcmd(feCmds, argc, argv, NULL);
}

