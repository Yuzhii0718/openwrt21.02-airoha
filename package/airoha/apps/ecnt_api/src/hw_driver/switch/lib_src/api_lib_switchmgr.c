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
#include <stdio.h>
#include <stdlib.h>
#include <linux/mii.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/types.h>
#include <linux/sockios.h>
#include "libapi_lib_switchmgr.h"
#include <modules/eth_global_def.h>

/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_FWC_ENV) || defined(TCSUPPORT_CF_ENV)
#define LAN_IF  "eth"
#else/*TCSUPPORT_COMPILE*/
#define LAN_IF  "eth0"
#endif/*TCSUPPORT_COMPILE*/
#define IF_UP  	"up"
#define IF_DOWN "down"
#define IF_ADD 	"addif"
#define IF_DEL 	"delif"


/************************************************************************
*                            M A C R O S
*************************************************************************
*/
#define ACTIVED			1
#define DEACTIVED		0
#define START_VLAN_VID	0x1
#define ETH_IF_NUM		3
#define MAX_CMD_LEN		128
#define MAX_PORT_NUM		6
#define RAETH_GSW_REG_READ	0x89F1
#define RAETH_GSW_REG_WRITE	0x89F2

static struct swconfig swcfg;
static u8 swcfg_init=0;
/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
struct swconfig
{
    int skfd;
    struct ifreq ifr;
};

typedef struct {
	u32 off;
	u32 val;
} gsw_reg;

typedef struct {
	u8 active;
	u8 portMap;
	u8 tagPortMap;
	u8 ivl_en;
	u8 fid;
	u16 vid;
	u16 stag;
} mac7530_para;

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                        P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                      E X T E R N A L   D A T A
*************************************************************************
*/

/************************************************************************
*                       P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*        F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int init_swconfig(void)
{
//	char buf[32];
	/*init swcfg*/
	memset(&swcfg, 0, sizeof(struct swconfig));

	/* Open a basic socket.*/
	if ((swcfg.skfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) {
		perror("socket");
		exit(-1);
	}

	strncpy(swcfg.ifr.ifr_name, LAN_IF, IFNAMSIZ);
	if (ioctl(swcfg.skfd, SIOCGMIIPHY, &swcfg.ifr) < 0) {
		if (errno != ENODEV)
			fprintf(stderr, "SIOCGMIIPHY on '%s' failed: %s\n", LAN_IF, strerror(errno));
		close(swcfg.skfd);
		return -1;
	}

	return 0;
}

struct swconfig* getSwcfg(void)
{
	if (swcfg_init == 0)
	{
		if(init_swconfig() == 0){
            swcfg_init = 1;
        }
	}
	return &swcfg;
}

int get_etherwan_port(void)
{
	char buf[4] = {0};
	FILE *fp = NULL;
	
	memset(buf, 0, sizeof(buf));
	fp = fopen("/proc/tc3162/eth_portmap","r");
	if(fp != NULL)
	{
		if(fgets(buf, sizeof(buf), fp) != NULL)
		{
			fclose(fp);	
			return atoi(buf);
		}
		fclose(fp);	
	}
	
	return -1;
}

int switchReg(int cmd, u32 offset, u32 value)
{
	gsw_reg reg;
	struct swconfig *swcfg = getSwcfg();
	
	(swcfg->ifr).ifr_data = (char *)(&reg);
	reg.off = offset;
	reg.val = value;
	
	if (ioctl(swcfg->skfd, cmd, &(swcfg->ifr)) < 0)
	{
		fprintf(stderr, "switchReg IOCTL on %s failed: %s\n", (swcfg->ifr).ifr_name, strerror(errno));
//		close(swcfg->skfd);
		return -1;
	}
	
//	close(swcfg->skfd);
	return reg.val;
}

int switchVlanSetPvid(u8 port, u16 pvid)
{
	u32 value;
	u32 reg;
	
	/*Parameters is error*/
	if (port > 6) 
	{
		return -1;
	}

	reg = 0x2014 + (port * 0x100);
	//value = gsw_reg_read(swcfg->skfd, swcfg->ifr, reg);
	value = switchReg(RAETH_GSW_REG_READ, reg, 0);
	value &= ~0xfff;
	value |= pvid;
	//gsw_reg_write(swcfg->skfd, swcfg->ifr, reg, value);
	switchReg(RAETH_GSW_REG_WRITE, reg, value);
	
	/* switch clear */
	reg = 0x80;
	//gsw_reg_write(swcfg->skfd, swcfg->ifr, reg, 0x8002);
	switchReg(RAETH_GSW_REG_WRITE, reg, 0x8002);
	usleep(5000);
	//value = gsw_reg_read(swcfg->skfd, swcfg->ifr, reg);
	value = switchReg(RAETH_GSW_REG_READ, reg, 0);
	
	return 0;
}

int switchVlanSetVid(mac7530_para para)
{
	int i,reg=0;
	u32 value=0;
	u32 value2=0;

	para.ivl_en=1;
	para.portMap |= (1<<6);

	value = (para.portMap<<16);
	value |= (para.stag<<4);
	value |= (para.ivl_en<<30);
	value |= (para.fid<<1);
	value |= (para.active ? 1 : 0);

	for (i = 0; i < 6; i++)
	{
		if (para.tagPortMap & (1<<i)) 
		{
			value2 |= 0x2<<(i*2);
		}
	}

	if (value2) 
	{
		// eg_tag
		value |= (1<<28);
	}
	
	reg = 0x98; // VAWD2
	//gsw_reg_write(swcfg->skfd, swcfg->ifr, reg, value2);
	switchReg(RAETH_GSW_REG_WRITE, reg, value2);

	reg = 0x94; // VAWD1
	//gsw_reg_write(swcfg->skfd, swcfg->ifr, reg, value);
	switchReg(RAETH_GSW_REG_WRITE, reg, value);

	reg = 0x90; // VTCR
	value = (0x80001000 + para.vid);
	//gsw_reg_write(swcfg->skfd, swcfg->ifr, reg, value);
	switchReg(RAETH_GSW_REG_WRITE, reg, value);

	reg = 0x90; // VTCR
	while (1) 
	{
		//value = gsw_reg_read(swcfg->skfd, swcfg->ifr, reg);
		value = switchReg(RAETH_GSW_REG_READ, reg, 0);
		if ((value & 0x80000000) == 0) 
		{ 
			//table busy
			break;
		}
	}

	/* switch clear */
	reg = 0x80;
	//gsw_reg_write(swcfg->skfd, swcfg->ifr, reg, 0x8002);
	switchReg(RAETH_GSW_REG_WRITE, reg, 0x8002);
	usleep(5000);
	//value = gsw_reg_read(swcfg->skfd, swcfg->ifr, reg);
	value = switchReg(RAETH_GSW_REG_READ, reg, 0);

	return 0;
}

int switchVlanActive(int type)
{
	int i = 0;
	int portmap,wan_port=-1;
	u32 value,reg;
	mac7530_para para;
	
	wan_port = get_etherwan_port();
	
	if(ACTIVED == type)
	{
		//port 0 to 4, forward broadcast/multicast/unicast only to cpu if lookup miss.
		reg = 0x0010;
		if(wan_port >= 0 && wan_port < 6)
		{
			//set port 6 as mirror port,used for etherwan
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0x404040ee);
			switchReg(RAETH_GSW_REG_WRITE, reg, 0x404040ee);
		}
		else
		{
			//without etherwan, no need to set mirror port
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0x404040e0);
			switchReg(RAETH_GSW_REG_WRITE, reg, 0x404040e0);
		}
		
		//forward every arp packet to cpu port
		reg = 0x0020;
		//value = gsw_reg_read2(swcfg->skfd, swcfg->ifr, ext_switch, reg);
		value = switchReg(RAETH_GSW_REG_READ, reg, 0);
		value &= ~((1<<18) | (1<<17) | (1<<16) | (1<<2) | (1<<1) | (1<<0));
		value |=  ((1<<18) | (1<<17) | (1<<2) | (1<<1));
		//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, value);
		switchReg(RAETH_GSW_REG_WRITE, reg, value);
		
		// vlan mismatch forword to cpu port
		for (i = 0; i <= 6; i++) 
		{
			reg = 0x2004 + i*0x100;
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0xff0003);
			switchReg(RAETH_GSW_REG_WRITE, reg, 0xff0003);
		}
		
		if (wan_port >=0 && wan_port < 6)
		{
			//use mirror port + port matrix to make etherwan packet only sent to CPU port
			reg = 0x2004 + wan_port * 0x100;
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0xf00100);
			switchReg(RAETH_GSW_REG_WRITE, reg, 0xf00100);
		
			//disable etherwan port mac learning
			reg = 0x200c + wan_port * 0x100;
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0xfff10);
			switchReg(RAETH_GSW_REG_WRITE, reg, 0xfff10);
		}
		
		for (i = 0; i <= 5; i++)
		{
			reg = 0x2010 + i*0x100;
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0x81008100); // EG_TAG consistent
			switchReg(RAETH_GSW_REG_WRITE, reg, 0x81008100);
		}
		reg = 0x2610;
		//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0x81008020);// user port & enable stag
		switchReg(RAETH_GSW_REG_WRITE, reg, 0x81008020);
		
		for (i = 0; i <= 6; i++) 
		{	
			// set vlan group
			reg = 0x2014 + i*0x100;
			value = 0x10000 + (i+1);
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, value);
			switchReg(RAETH_GSW_REG_WRITE, reg, value);
		}
		
		#if defined(MT7530_SUPPORT) || defined(TCSUPPORT_MT7510_FE)
		system("echo 1 >/proc/tc3162/vport_enable");
		#endif

		//set the vlan pvid and vid
		para.active = ACTIVED;
		para.tagPortMap = 0;
		para.ivl_en = 0;
		para.fid = 0;
		para.stag = 0;
		for(i=0; i<MAX_PORT_NUM; i++)
		{
			/*Set the default PVID for each port, Default pvid value is start at 2.*/
			//swic.vlan_pvid(i, i+START_VLAN_VID);
			switchVlanSetPvid(i, i+START_VLAN_VID);
				
			/*Set the default vlan table rules*/
			//swic.vlan_vid(i, ACTIVED, i+START_VLAN_VID, (1<<i), 0x0, 0, 0, 0);
			para.vid = i+START_VLAN_VID;
			para.portMap = 0;
			para.portMap = (1<<i);
			switchVlanSetVid(para);
		}
		
		//bind all lan port to cpu port
		//swic.vlan_vid(i, ACTIVED, i+START_VLAN_VID, ((1<<0) | (1<<1) | (1<<2) | (1<<3)), 0x0, 0, 0, 0);
		para.vid = i+START_VLAN_VID;
		para.portMap = 0;
		para.portMap = ((1<<0) | (1<<1) | (1<<2) | (1<<3));
		switchVlanSetVid(para);
	}
	else
	{
		/* Disable VLAN Function */
		// port 0 to 4, forword broadcast/multicast/unicast to every port.
		reg = 0x0010;
		//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0xffffffe0);
		switchReg(RAETH_GSW_REG_WRITE, reg, 0xffffffe0);
		
		// forward every arp packet
		reg = 0x0020;
		//value = gsw_reg_read2(swcfg->skfd, swcfg->ifr, ext_switch, reg);
		value = switchReg(RAETH_GSW_REG_READ, reg, 0);
		value &= ~((1<<18) | (1<<17) | (1<<16) | (1<<2) | (1<<1) | (1<<0));
		//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, value);
		switchReg(RAETH_GSW_REG_WRITE, reg, value);
		
		// port matrix mode 
		for (i = 0; i <= 6; i++) 
		{
			reg = 0x2004 + i*0x100;
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0xff0000);
			switchReg(RAETH_GSW_REG_WRITE, reg, 0xff0000);
		}
		
		// set EG_TAG = CONSISTENT
		for (i = 0; i <= 5; i++)
		{
			reg = 0x2010 + i*0x100;
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0x81008100);
			switchReg(RAETH_GSW_REG_WRITE, reg, 0x81008100);
		}
		
		// set CPU Port EG_TAG=CONSISTENT & enable SP_TAG
		reg = 0x2610;
		//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0x81008120);
		switchReg(RAETH_GSW_REG_WRITE, reg, 0x81008120);

		// set PVID as 1
		for (i = 0; i <= 6; i++) 
		{
			reg = 0x2014 + i*0x100;
			value = 0x10000 + (1);
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, value);
			switchReg(RAETH_GSW_REG_WRITE, reg, value);
		}
		
		//enable mac learning
		for (i = 0; i <= 5; i++)
		{
			reg = 0x200c + i * 0x100;
			//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0xfff00);
			switchReg(RAETH_GSW_REG_WRITE, reg, 0xfff00);
		}	
		
		#if defined(MT7530_SUPPORT) || defined(TCSUPPORT_MT7510_FE)
		system("echo 0 >/proc/tc3162/vport_enable");
		#endif		
	}

	/* switch clear */
	reg = 0x80;
	//gsw_reg_write2(swcfg->skfd, swcfg->ifr, ext_switch, reg, 0x8002);
	switchReg(RAETH_GSW_REG_WRITE, reg, 0x8002);
	usleep(5000);
	//value = gsw_reg_read2(swcfg->skfd, swcfg->ifr, ext_switch, reg);
	value = switchReg(RAETH_GSW_REG_READ, reg, 0);
	
	return 0;
}

int switchVlanptEnable(int enable)
{
	int i = 0;
	char cmd[MAX_CMD_LEN]={0};
	
	for(i=0; i<ETH_IF_NUM; i++)
	{
		sprintf(cmd, "ifconfig %s%d %s",LAN_IF,i,enable ? IF_UP: IF_DOWN);
		system(cmd);
		sprintf(cmd, "brctl %s br0 %s%d",enable ? IF_ADD: IF_DEL,LAN_IF,i);
		system(cmd);
	}
	
	return switchVlanActive(enable);
}

int macMT7530gswAPIEntrance(mt7530_switch_api_cmdid cmdid, u32 port, void *paramext_ptr)
{
    struct swconfig *swcfg = getSwcfg();
    mt7530_switch_api_params api_param;
    
    //printf("[%s]%d:cmdid = %d\n", __FUNCTION__, __LINE__, cmdid);

    (swcfg->ifr).ifr_data = (void *)(&api_param);
    api_param.cmdid = cmdid;
    api_param.PortQueueId = port;
    api_param.paramext_ptr = paramext_ptr;

    if (ioctl(swcfg->skfd, RAETH_GSW_CTLAPI, &(swcfg->ifr)) < 0)
    {
        fprintf(stderr, "RAETH_GSW_CTLAPI on %s failed: %s\n", (swcfg->ifr).ifr_name, strerror(errno));
//       close(swcfg->skfd);
        return -1;
    }
#if 0
    if (api_param.ret_value == -1)
    {
#if (1)
        usleep(10000);
#endif
        //fprintf(stderr, "^%s(%04X, %d, %08X) : kernel return %d !\n", __FUNCTION__, cmdid, port, paramext_ptr, api_param.ret_value);
        //fprintf(stderr, "^MT7530 switch kernel API (%04X, %d, %08X) return -1 !\n", cmdid, port, (u32)(paramext_ptr));
    }
#endif    
 //   close(swcfg->skfd);
    return (api_param.ret_value);
}

/************************************************************************
*        SWITCH/GLOBAL ATTRIBUTES API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_portmap(int wan_port, int lan_portmap[6])
{
    int ret = 0, i;
    mt7530_switch_eth_portmap_t eth_portmap;

    eth_portmap.wan_port = wan_port;
    for(i = 0; i < 6; i++)
    {
        eth_portmap.lan_portmap[i] = lan_portmap[i];
    }
    ret = macMT7530gswAPIEntrance(CMD_SET_GSW_SETETHPORTMAP, 0, (void *)&eth_portmap);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

	/*vlanpt action */
    switchVlanptEnable(DEACTIVED);
    switchVlanptEnable(ACTIVED);
	
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_portmap(char *lan_portmap)
{
	int ret = 0; 
	
    ret = macMT7530gswAPIEntrance(CMD_GET_GSW_GETETHPORTMAP, 0, lan_portmap);	
		
	if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_active(u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_ACTIVE, 0, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_flow_control(u8 enable)
{
    int ret=0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_FLOWCONTROL_GLOBAL, 0, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_flow_control(u8 *p_enable)
{
    int ret=0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_FLOWCONTROL_GLOBAL, 0, (void *)p_enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_max_framesize(u16 size)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_MAX_FRAME_SIZE, 0, (void *)(u32)size);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_max_framesize(u16 *p_size)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_MAX_FRAME_SIZE, 0, (void *)p_size);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_chip_type(ECNT_SWITCHMGR_CHIP_TYPE *p_chip_type)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_CHIP_TYPE, 0, (void *)p_chip_type);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_isolate(u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_ISOLATE, 0, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_matrix(int type, int port_group[6])
{
    int i = 0, ret = 0;
    mt7530_switch_api_matrix_t matrix_p;

    matrix_p.type = type;
    for(i = 0; i < 6; i++)
    {
        matrix_p.group[i] = port_group[i];
    }
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_MATRIX, 0, (void *)&matrix_p);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_eth_general_api(u8 enable)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;
    
    paramext.p1 = (u32)HAL_API_EXT;
    paramext.p2 = (u32)enable;
    ret = macMT7530gswAPIEntrance(CMD_ETH_GENERAL_API_CTL, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_api_debug_dump_enable(u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_APIDBGDUMP_EN, 0, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_tx_time_stamp(ECNT_SWITCHMGR_TIMESTAMP_FIFO *p_tx_ts)
{
    int ret = 0;
    mt7530_switch_api_timestamp_fifo tx_ts;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_TX_TIMESTAMP, 0, (void *)&tx_ts);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    p_tx_ts->msg_type = tx_ts.msg_type;
    p_tx_ts->seq_id = tx_ts.seq_id;
    p_tx_ts->tstamp.seconds = tx_ts.tstamp.seconds;
    p_tx_ts->tstamp.nanoseconds = tx_ts.tstamp.nanoseconds;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_rx_time_stamp(ECNT_SWITCHMGR_TIMESTAMP_FIFO *p_rx_ts)
{
    int ret = 0;
    mt7530_switch_api_timestamp_fifo rx_ts;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_RX_TIMESTAMP, 0, (void *)&rx_ts);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    p_rx_ts->msg_type = rx_ts.msg_type;
    p_rx_ts->seq_id = rx_ts.seq_id;
    p_rx_ts->tstamp.seconds = rx_ts.tstamp.seconds;
    p_rx_ts->tstamp.nanoseconds = rx_ts.tstamp.nanoseconds;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_curr_time(u8 type, ECNT_SWITCHMGR_TIMESTAMP *ts)
{
    int ret = 0;
    mt7530_switch_api_timestamp tsdata;
    
    tsdata.seconds = ts->seconds;
    tsdata.nanoseconds = ts->nanoseconds;
    ret = macMT7530gswAPIEntrance(CMD_SET_CURRTIME, (u32)type, (void *)&tsdata);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_curr_time(ECNT_SWITCHMGR_TIMESTAMP *p_ts)
{
    int ret = 0;
    mt7530_switch_api_timestamp ts;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_CURRTIME, 0, (void *)&ts);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    p_ts->nanoseconds = ts.nanoseconds;
    p_ts->seconds = ts.seconds;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_time_offset(int opt, ECNT_SWITCHMGR_TIMESTAMP *ts)
{
    int ret = 0;
    mt7530_switch_api_timestamp tsdata;
    
    tsdata.seconds = ts->seconds;
    tsdata.nanoseconds = ts->nanoseconds;
    ret = macMT7530gswAPIEntrance(CMD_SET_OFFSET, (u32)opt, (void *)&tsdata);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_lan2lan(ECNT_SWITCHMGR_LAN2LAN lan2lan)
{
    int ret=0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_LAN2LAN, 0, (void *)&lan2lan);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_lan2lan(ECNT_SWITCHMGR_LAN2LAN_INFO *p_lan2lan)
{
    int ret = 0;

    ret = macMT7530gswAPIEntrance(CMD_GET_LAN2LAN, 0, (void *)p_lan2lan);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_clear_lan2lan(void)
{
    int ret=0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_LAN2LAN_CLEAR, 0, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_dstq(u8 mode, u8 fc_enable)
{   
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = mode;
    paramext.p2 = fc_enable;
    ret = macMT7530gswAPIEntrance(CMD_SET_DSTQ, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_mirror_enable(u8 enable, u8 mirror_port)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = (u32)enable;
    paramext.p2 = (u32)mirror_port;
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_MIRROR_ENABLE, 0 ,(void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_add_port_vlan_bind(u8 port, u16 bindvid)  
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_ADD_VLAN_BIND_MODE, port, (void *)(u32)bindvid);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_del_port_vlan_bind(u8 port, u16 bindvid)  
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_DEL_VLAN_BIND_MODE, port, (void *)(u32)bindvid);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_do_p6_cal(void)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_DO_P6Cal, 0, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        PORT ATTRIBUTES API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_qos(u8 port, u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_QOS_ENABLE, port, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_qos_param(u8 port, ECNT_SWITCHMGR_QOS_CFG qos_param)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_QOS_PARAM, port, (void *)&qos_param);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_admin(u8 port, u8 up)
{
    int ret = 0;
    u8 down_flag = up ? 0 : 1;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_ADMIN, port, (void *)(u32)down_flag);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_admin(u8 port, u8 *p_up)
{
    int ret = 0;
    u8 down_flag = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_ADMIN, port, (void *)&down_flag);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_up = down_flag ? 0 : 1;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_ratelimit(u8 port, u8 dir, u32 rate)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;
    
    paramext.p1 = rate;
    paramext.p2 = (u32)dir;
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_RATE_LIMIT, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_ratelimit(u8 port, u8 dir, u32 *p_rate)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = (u32)dir;
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_RATE_LIMIT, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_rate = paramext.p2;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_etherwan_port_ratelimit(u32 dir, u32 *rate)
{
	int ret = 0;
	mt7530_switch_api_paramext2 paramext;

	paramext.p1 = (u32)rate;
	paramext.p2 = (u32)dir;
	ret = macMT7530gswAPIEntrance(CMD_GET_ETHERWAN_PORT_RATE_LIMIT, 0, (void *)(&paramext));
	if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
	
	*rate = ret;
	return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_etherwan_port_ratelimit(u32 rate, u8 dir)
{
	int ret = 0;
	
    mt7530_switch_api_paramext2 paramext;
    paramext.p1 = (u32)rate;
    paramext.p2 = (u32)dir;

    ret = macMT7530gswAPIEntrance(CMD_SET_ETHERWAN_PORT_RATE_LIMIT, 0, (void *)(&paramext));
	if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

#ifdef CONFIG_TP_IMAGE
ECNT_SWITCHMGR_RET switchmgr_lib_set_etherwan_port_rate_limit_enable(u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_ETHERWAN_PORT_RATELMTEN, 0, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_etherwan_port_rate_limit_enable(u8 *p_enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_ETHERWAN_PORT_RATELMTEN, 0, NULL);
    if (ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    *p_enable = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

#endif /* CONFIG_TP_IMAGE */

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_loopback(u8 port, u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_LOOPBACK_REMOTE, port, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_loopback(u8 port, u8 *p_enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_LOOPBACK_REMOTE, port, (void *)p_enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_stp_status(u8 port, ECNT_SWITCHMGR_STP_STATUS stp_status)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORTSTATUS, port, (void *)(u32)stp_status);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_stp_status(u8 port, ECNT_SWITCHMGR_STP_STATUS *p_stp_status)
{
    int ret = 0;

    ret = macMT7530gswAPIEntrance(CMD_GET_PORTSTATUS, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_stp_status = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_speed_mode(u8 port, ECNT_SWITCHMGR_SPEED_MODE mode)
{
    int ret = 0;

    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_SPEEDMODE, port, (void *)mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_speed_mode(u8 port, ECNT_SWITCHMGR_SPEED_MODE *p_mode)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_SPEEDMODE, port, (void *)p_mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_link_state(u8 port, ECNT_SWITCHMGR_LINK_STATE *p_link_state, ECNT_SWITCHMGR_LINK_SPEED *p_speed)
{
    int ret=0;
    mt7530_switch_api_paramext2 paramext;

    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_LINKSTATE, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_link_state = paramext.p1;
    *p_speed = paramext.p2;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_link_state(u8 port, u8 link_state)
{
	int ret=0;
	
	ret = macMT7530gswAPIEntrance(CMD_SET_PORT_UP_DOWN, port, (void *)(u32)link_state);
	if(ret < 0)
	{
	    return ECNT_SWITCHMGR_FAIL;
	}

	return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_flow_control(u8 port, u8 enable)
{
    int ret=0;
    
    ret =  macMT7530gswAPIEntrance(CMD_SET_PORT_FLOWCONTROL, port, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_flow_control(u8 port, u8 *p_enable)
{
    int ret=0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_FLOWCONTROL, port, (void *)p_enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_flow_control_cfg(u8 port, u8 *p_enable)
{
    int ret=0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_FLOWCONTROL_CFG, port, (void *)p_enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_mac_limit(u8 port, u32 cnt)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_MACLMT, port, (void *)cnt);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_mac_limit(u8 port, u32 *p_cnt)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_MACLMT, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_cnt = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_mac_learn_num(u8 port, u32 *p_cnt)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_MACNUM, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_cnt = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}


ECNT_SWITCHMGR_RET switchmgr_lib_get_port_mac(u8 port, u8 mac[6])
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORTMAC, port, (void *)mac);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_pause(u8 port, u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_PAUSE, port, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_pause(u8 port, u8 *p_enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_PAUSE, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_enable = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_learning(u8 port, u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_BRGLEARNINGIND, port, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_learning(u8 port, u8 *p_enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_BRGLEARNINGIND, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_enable = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}


ECNT_SWITCHMGR_RET switchmgr_lib_set_port_brgInd(u8 port, u8 Ind)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORTBRGIND, port, (void *)(u32)Ind);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_brgInd(u8 port, u8 *Ind)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORTBRGIND, port, (void *)Ind);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_discard_unknown_mac(u8 port, u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_DISCARDUNKNOWNMACIND, port, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_discard_unknown_mac(u8 port, u8 *p_enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_DISCARDUNKNOWNMACIND, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_enable = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_max_bitrate(int port, char* speed)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_MAX_BIT_RATE, port, (void *)speed);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_max_bitrate(int port, char* speed)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_MAX_BIT_RATE, port, (void *)speed);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_duplex(int port, char* Mode)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_DUPLEX_MODE, port, (void *)Mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_duplex(int port, char* Mode)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_DUPLEX_MODE, port, (void *)Mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_autoneg_enable(u8 port, u8 enable)
{
    int ret = 0;

    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_AN, port, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_autoneg_enable(u8 port, u8 *p_enable)
{
    int ret = 0;
   
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_AN, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    *p_enable = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_autoneg_restart(u8 port)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_AN_RESTART, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_autoneg_fail(u8 port, u8* p_fail)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_AN_FAIL, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_fail = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_autodetection(u8 port, u8 auto_detction)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_AUTO_DETECT, port, (void *)(u32)auto_detction);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_autodetection(u8 port, u8 *p_auto_detction)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_AUTO_DETECT, port, (void *)p_auto_detction);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_pause_time(u8 port, u32 time)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PAUSE_TIME, port, (void *)(u32)time);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_pause_time(u8 port, u32 *p_time)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PAUSE_TIME, port, (void *)p_time);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_fail(u8 port, u8 *p_fail)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_FAIL, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    *p_fail = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_ptp_enable(u8 port)
{
    int ret = 0;
    int tmp_port;

    if (port >= ECNT_SWITCHMGR_PORT_NUM)
    {
        tmp_port = -1;
    }
    else
    {
        tmp_port = port;
    }
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PTPSTATE, 0, (void *)(u32)tmp_port);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_ptp_enable(u8 port, u8 *p_enable)
{
    int ret = 0;
    int bitmap = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PTPSTATE, 0, (void *)&bitmap);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    if (bitmap & (1 << port))
    {
        *p_enable = 1;
    }
    else
    {
        *p_enable = 0;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_mirror(u8 port, ECNT_SWITCHMGR_PORT_MIRROR *portbased)
{
    int ret = 0;
    mt7530_switch_api_port_mirror_port_based_t portbaseddata;
        
    portbaseddata.port_tx_mir = portbased->port_tx_mir;
    portbaseddata.port_rx_mir = portbased->port_rx_mir;
    portbaseddata.acl_mir = portbased->acl_mir;
    portbaseddata.vlan_mis = portbased->vlan_mis;
    portbaseddata.igmp_mir = portbased->igmp_mir;
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_MIRROR_PORT_BASED, port ,(void *)&portbaseddata);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_mac_txrx(u8 port, u8 direction, u8 enable)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = enable;
    paramext.p2 = direction;
    ret = macMT7530gswAPIEntrance(CMD_SET_MAC_TX_RX, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_configuration_status(u8 port, u8 *p_cfg)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_CONFIG_STAT, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    *p_cfg = (u8)ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_dte_dce_ind(u8 port, u8 ind)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_DTEDCE_Ind, port, (void *)(u32)ind);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_dte_dce_ind(u8 port, u8 *p_ind)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_DTEDCE_Ind, port, (void *)p_ind);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        VLAN ATTRIBUTES API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_pvid(u8 port, u16 pvid)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_PVID, port ,(void *)(u32)pvid);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_vid(ECNT_SWITCHMGR_VLAN_ENTRY *viddata)
{
    mt7530_switch_api_vlan_vid_t vid;
    int ret = 0;
    
    vid.index = viddata->index;
    vid.active = viddata->active;
    vid.vid = viddata->vid;
    vid.portMap = viddata->portMap;
    vid.tagPortMap = viddata->tagPortMap;
    vid.ivl_en = viddata->ivl_en;
    vid.fid = viddata->fid;
    vid.stag = viddata->stag;
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_VID, 0 ,(void *)&vid);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_disp()
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_DISP, 0 , NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_port_attr(u8 port, u8 attr)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_PORT_ATTR, port , (void *)(u32)attr);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_port_mode(u8 port, u8 mode)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_PORT_MODE, port , (void *)(u32)mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_egress_tag_pvc(u8 port, u8 eg_tag)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_EGRESS_TAG_PVC, port , (void *)(u32)eg_tag);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_egress_tag_pcr(u8 port, u8 eg_tag)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_EGRESS_TAG_PCR, port , (void *)(u32)eg_tag);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_acc_frm(u8 port, u8 type)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_ACC_FRM, port , (void *)(u32)type);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_vlan_entry_etag(u16 vid, u8 port, u8* p_etag)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = (u32)vid;
    paramext.p2 = (u32)p_etag;
    ret = macMT7530gswAPIEntrance(CMD_GET_VLAN_ENTRY_EGTAG, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_entry_stag(u16 vid, u8 port, u8 stag)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = (u32)vid;
    paramext.p2 = (u32)stag;
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_ENTRY_STAG, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_vlan_entry_stag(u16 vid, u8 port, u8* p_stag)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = (u32)vid;
    paramext.p2 = (u32)p_stag;
    ret = macMT7530gswAPIEntrance(CMD_GET_VLAN_ENTRY_STAG, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_entry_etag(u16 vid, u8 port, u8 etag)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = (u32)vid;
    paramext.p2 = (u32)etag;
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_ENTRY_EGTAG, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_add_vlan_port_stag(u8 port, u8 stag, u16 vid)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = (u32)stag;
    paramext.p2 = (u32)vid;
    ret = macMT7530gswAPIEntrance(CMD_ADD_VLAN_PORT_STAG, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_entry_etag_mode(u16 vid, u8 etag_mode)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_ENTRY_ETAGMODE, (u32)vid, (void *)(u32)etag_mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_vlan_entry_enable(u16 vid, u8 sw)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = (u32)vid;
    paramext.p2 = (u32)sw;
    ret = macMT7530gswAPIEntrance(CMD_SET_VLAN_ENTRY_ENABLE, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_per_vlan_action(char port_id, int o_vid, int n_vid, int vlan_mode, char enable)
{   
    int ret = 0;
    mt7530_switch_api_vlan_table_t vlantable_entry;

    vlantable_entry.port_id = port_id;
    vlantable_entry.o_vid = o_vid;
    vlantable_entry.n_vid = n_vid;
    vlantable_entry.vlan_mode = vlan_mode;
    vlantable_entry.enable = enable;
    ret = macMT7530gswAPIEntrance(CMD_SET_PER_VLAN_ACTION, 0, (void *)&vlantable_entry);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_per_port_vlan_action(u8 port_id, int n_vid, int vlan_mode, u8 enable)
{   
    int ret = 0;
    mt7530_switch_api_vlan_table_t vlantable_entry;

    vlantable_entry.port_id = port_id;
    vlantable_entry.n_vid = n_vid;
    vlantable_entry.vlan_mode = vlan_mode;
    vlantable_entry.enable = enable;
    ret = macMT7530gswAPIEntrance(CMD_SET_PER_PORT_VLAN_ACTION, 0, (void *)&vlantable_entry);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        MAC ATTRIBUTES API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_age_time(u32 time)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_AGETIME, 0, (void *)time);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_age_time( u32 *p_time)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_AGETIME, 0, (void *)p_time);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_clr_mac_table(void)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_CLR_MAC_TABLE, 0, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_clr_port_mac_table(u8 port)
{
    int ret = 0;
    printf("[%s]%d\n", __FUNCTION__, __LINE__);
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_CLEAN_MAC, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_bridge_table(u8 port, ECNT_SWITCHMGR_BRIDGE_TABLE *p_br_table)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_BR_TABLE, port, (void *)p_br_table);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_by_mac(ECNT_SWITCHMGR_MAC_ENTRY *inptr_PortMac)
{
    int ret = 0;
    
    ret =  macMT7530gswAPIEntrance(CMD_GET_PORT_BY_MAC, 0, (void *)(inptr_PortMac));

    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_all_mac(u8 port, ECNT_SWITCHMGR_PORT_ALL_MAC *mac)
{
    int ret = 0, i = 0;
    mt7530_switch_port_mac p_PortMac;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_ALL_MAC, (u32)port, &p_PortMac);
    if (ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    mac->vaild_entry_num = p_PortMac.vaild_entry_num;  
    for(i = 0; i < mac->vaild_entry_num; i++)
    {
        mac->portmac[i].mac[0] = p_PortMac.portmac[i].mac[0];
        mac->portmac[i].mac[1] = p_PortMac.portmac[i].mac[1];
        mac->portmac[i].mac[2] = p_PortMac.portmac[i].mac[2];
        mac->portmac[i].mac[3] = p_PortMac.portmac[i].mac[3];
        mac->portmac[i].mac[4] = p_PortMac.portmac[i].mac[4];
        mac->portmac[i].mac[5] = p_PortMac.portmac[i].mac[5];
    }
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_arl_aging(u8 aging_en, u32 time)
{
    mt7530_switch_api_paramext2 paramext;
    int ret = 0;

    paramext.p1 = (u8)aging_en;
    paramext.p2 = (u32)time;
    ret = macMT7530gswAPIEntrance(CMD_SET_ARL_AGING, 0 ,(void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_arl_mactbladd(ECNT_SWITCHMGR_ARL_MAC_ENTRY *mactbl)
{
    mt7530_switch_mactbladd_t mactbladd;
    int ret = 0;

    mactbladd.mac[0] = mactbl->mac[0];
    mactbladd.mac[1] = mactbl->mac[1];
    mactbladd.mac[2] = mactbl->mac[2];
    mactbladd.mac[3] = mactbl->mac[3];
    mactbladd.mac[4] = mactbl->mac[4];
    mactbladd.mac[5] = mactbl->mac[5];
    mactbladd.destportmap = mactbl->destportmap;
    mactbladd.eg_tag = mactbl->eg_tag;
    mactbladd.leaky_en = mactbl->leaky_en;
    mactbladd.sa_mir_en = mactbl->sa_mir_en;
    mactbladd.sa_port_fw = mactbl->sa_port_fw;
    mactbladd.usr_pri = mactbl->usr_pri;
    ret = macMT7530gswAPIEntrance(CMD_SET_ARL_MACTBLADD, 0 ,(void *)&mactbladd);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_arl_mactbldisp(int ac_mat)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_ARL_ADDRTBLDISP, 0 ,(void *)ac_mat);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_arl_mactbldisp2(u32 ext_switch, int ac_mat)
{
    mt7530_switch_api_paramext2 paramext;
    int ret = 0;

    paramext.p1 = (u32)ext_switch;
    paramext.p2 = (u32)ac_mat;
    ret = macMT7530gswAPIEntrance(CMD_SET_ARL_ADDRTBLDISP2, 0 ,(void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_arl_mactblclr(int ac_mat)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_ARL_ADDRTBLCLR, 0 ,(void *)ac_mat);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_arl_mactblclr2(u32 ext_switch, int ac_mat)
{
    mt7530_switch_api_paramext2 paramext;
    int ret = 0;

    paramext.p1 = (u32)ext_switch;
    paramext.p2 = (u32)ac_mat;
    ret = macMT7530gswAPIEntrance(CMD_SET_ARL_ADDRTBLCLR2, 0 ,(void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        QOS ATTRIBUTES API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_queue_mark(u8 enable, u8 shift, u8 width)
{
    int ret = 0;
    mt7530_switch_queue_info paramext;
    
    paramext.enable = enable;
    paramext.shift = shift;
    paramext.width = width;
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_QOS_MODE, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_queue_weight(u8 port, u8 queueId, u8 weight)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_WEIGHT, ((port << 16) + queueId), (void *)(u32)weight);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_queue_weight(u8 port, u8 queueId, u8 *p_weight)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_WEIGHT, ((port << 16) + queueId), NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_weight = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_queue_backpressure(u8 port, u8 queueId, ECNT_SWITCHMGR_QUEUE_BACK_PRESSURE *backPressure)
{
    int ret = 0;
    mt7530_switch_api_backPressure_t backPressure_p;
    
    backPressure_p.Enable = backPressure->Enable;
    backPressure_p.MaxQueueThreshold = backPressure->MaxQueueThreshold;
    backPressure_p.MinQueueThreshold = backPressure->MinQueueThreshold;
    ret = macMT7530gswAPIEntrance(CMD_SET_BACK_PRESSURE, ((port << 16) + queueId), &backPressure_p);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_queue_backpressure(u8 port, u8 queueId, ECNT_SWITCHMGR_QUEUE_BACK_PRESSURE *backPressure)
{
    int ret = 0;
    mt7530_switch_api_backPressure_t backPressure_p;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_BACK_PRESSURE, ((port << 16) + queueId), &backPressure_p);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    backPressure->Enable = backPressure_p.Enable;
    backPressure->MaxQueueThreshold = backPressure_p.MaxQueueThreshold;
    backPressure->MinQueueThreshold = backPressure_p.MinQueueThreshold;
    backPressure->time = backPressure_p.time;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_queue_priority(u16 priority, u8 queueId)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_QUEUE_PRIORITY, queueId, (void *)(u32)priority);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_maximun_queue_size(u32 *p_size)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_QUEUE_MAX_SIZE, 0, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_size = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_all_located_queue_size(u16 AllocatedQueueSize, u8 queueId)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_ALLOC_QUEUE_SIZE, queueId, (void *)(u32)AllocatedQueueSize);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_all_located_queue_size(u8 queueId, u32 *p_size)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_ALLOC_QUEUE_SIZE, queueId, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_size = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_drop_policy(u8 port, u8 queueId, ECNT_SWITCHMGR_DROP_POLICY *DropPolicy)
{
    int ret = 0;
    mt7530_switch_DropPolicy_t DropPolicydata;
    
    DropPolicydata.DropPrecdenceColourMarking = DropPolicy->DropPrecdenceColourMarking;
    DropPolicydata.greenPacketDropMax_p = DropPolicy->greenPacketDropMax_p;
    DropPolicydata.greenPacketDropQueueMaxThr = DropPolicy->greenPacketDropQueueMaxThr;
    DropPolicydata.greenPacketDropQueueMinThr = DropPolicy->greenPacketDropQueueMinThr;
    DropPolicydata.QueueDropW_q = DropPolicy->QueueDropW_q;
    DropPolicydata.yellowPacketDropMaxn_p = DropPolicy->yellowPacketDropMaxn_p;
    DropPolicydata.yellowPacketDropQueueMaxThr = DropPolicy->yellowPacketDropQueueMaxThr;
    DropPolicydata.yellowPacketDropQueueMinThr = DropPolicy->yellowPacketDropQueueMinThr;
    ret = macMT7530gswAPIEntrance(CMD_SET_DROP_POLICY, ((port << 16) + queueId), (void *)&DropPolicydata);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_drop_policy(u8 port, u8 queueId, ECNT_SWITCHMGR_DROP_POLICY *p_DropPolicy)
{
    int ret = 0;
    mt7530_switch_DropPolicy_t p_DropPolicydata;
        
    ret = macMT7530gswAPIEntrance(CMD_GET_DROP_POLICY, ((port << 16) + queueId), (void *)&p_DropPolicydata);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    p_DropPolicy->DropPrecdenceColourMarking = p_DropPolicydata.DropPrecdenceColourMarking;
    p_DropPolicy->greenPacketDropMax_p = p_DropPolicydata.greenPacketDropMax_p;
    p_DropPolicy->greenPacketDropQueueMaxThr = p_DropPolicydata.greenPacketDropQueueMaxThr;
    p_DropPolicy->greenPacketDropQueueMinThr = p_DropPolicydata.greenPacketDropQueueMinThr;
    p_DropPolicy->QueueDropW_q = p_DropPolicydata.QueueDropW_q;
    p_DropPolicy->yellowPacketDropMaxn_p = p_DropPolicydata.yellowPacketDropMaxn_p;
    p_DropPolicy->yellowPacketDropQueueMaxThr = p_DropPolicydata.yellowPacketDropQueueMaxThr;
    p_DropPolicy->yellowPacketDropQueueMinThr = p_DropPolicydata.yellowPacketDropQueueMinThr;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_qos_base(u8 base)
{   
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_QOS_BASE, 0, (void *)(u32)base);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_qos_prio_qmap(u8 prio, u8 queue)
{   
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = prio;
    paramext.p2 = queue;
    ret = macMT7530gswAPIEntrance(CMD_SET_QOS_PRIO_QMAP, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_qos_prio_tagmap(u8 prio, u8 tag)
{   
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = prio;
    paramext.p2 = tag;
    ret = macMT7530gswAPIEntrance(CMD_SET_QOS_PRIO_TAGMAP, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_qos_prio_dscpmap(u8 prio, u8 dscp)
{   
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = prio;
    paramext.p2 = dscp;
    ret = macMT7530gswAPIEntrance(CMD_SET_QOS_PRIO_DSCPMAP, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_qos_port_prio(u8 port, u8 prio)
{   
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = port;
    paramext.p2 = prio;
    ret = macMT7530gswAPIEntrance(CMD_SET_QOS_PORT_PRIO, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_qos_port_weight(ECNT_SWITCHMGR_PORT_WEIGHT *qosweight)
{   
    int ret = 0;
    mt7530_switch_api_qos_port_weight_t qosweight2;

    qosweight2.port = qosweight->port;
    qosweight2.weight[0] = qosweight->weight[0];
    qosweight2.weight[1] = qosweight->weight[1];
    qosweight2.weight[2] = qosweight->weight[2];
    qosweight2.weight[3] = qosweight->weight[3];
    qosweight2.weight[4] = qosweight->weight[4];
    qosweight2.weight[5] = qosweight->weight[5];
    qosweight2.weight[6] = qosweight->weight[6];
    qosweight2.weight[7] = qosweight->weight[7];
    ret = macMT7530gswAPIEntrance(CMD_SET_QOS_PORT_WEIGHT, 0, (void *)&qosweight2);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_qos_dscp_prio(u8 dscp, u8 prio)
{   
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = dscp;
    paramext.p2 = prio;
    ret = macMT7530gswAPIEntrance(CMD_SET_QOS_DSCP_PRIO, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        STATISTICS API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_clr_all_port_statistics(void)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_STATISTICS_CLEAR, 0, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_clr_single_port_statistics(u8 port)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_CLEAR_PORT_STATISTICS, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}


ECNT_SWITCHMGR_RET switchmgr_lib_get_port_statistics(u8 port, ECNT_SWITCHMGR_PORT_STATISTICS *p_stats)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_STATISTICS, port, (void *)p_stats);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_mib_count(u8 port, u32 type, u32 *p_cnt)
{
    int ret = 0;
    mt7530_switch_GetMibCnt_param para;
    
    para.MibCntType = type;
    ret = macMT7530gswAPIEntrance(CMD_GET_MIBCNT, port, (void *)&para);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_cnt = para.Out__Cnt;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_txpkts_count(u8 port, u32 *p_cnt)
{
	u32 ucast_cnt = 0, mcast_cnt = 0, bcast_cn = 0;

    if(switchmgr_lib_get_port_mib_count(port, MIB_ID_TX_UCAST_CNT, &ucast_cnt) == 0 &&
	   switchmgr_lib_get_port_mib_count(port, MIB_ID_TX_MCAST_CNT, &mcast_cnt) == 0 &&
	   switchmgr_lib_get_port_mib_count(port, MIB_ID_TX_BCAST_CNT, &bcast_cn) == 0)
    {
		*p_cnt = ucast_cnt + mcast_cnt + bcast_cn;
    }
    else
    {
		return ECNT_SWITCHMGR_FAIL;
    }

	return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_rxpkts_count(u8 port, u32 *p_cnt)
{
	u32 ucast_cnt = 0, mcast_cnt = 0, bcast_cn = 0;

	if(switchmgr_lib_get_port_mib_count(port, MIB_ID_RX_UCAST_CNT, &ucast_cnt) == 0 &&
	   switchmgr_lib_get_port_mib_count(port, MIB_ID_RX_MCAST_CNT, &mcast_cnt) == 0 &&
	   switchmgr_lib_get_port_mib_count(port, MIB_ID_RX_BCAST_CNT, &bcast_cn) == 0)
    {
		*p_cnt = ucast_cnt + mcast_cnt + bcast_cn;
    }
    else
    {
		return ECNT_SWITCHMGR_FAIL;
	}

	return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_enable_mibcounter(u32 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_MIBCNT_EN, 0, (void *)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_clear_mibcounter(void)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_MIBCNT_CLR, 0, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_rx_otcmode(u8 mode)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_RXOCT_MODE, 0, (void *)(u32)mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_tx_otcmode(u8 mode)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_TXOCT_MODE, 0, (void *)(u32)mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_bx_pktmode(u8 mode)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_BXPKT_MODE, 0, (void *)(u32)mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_rx_byte(int port, unsigned int* RxByte_H, unsigned int* RxByte_L)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_RX_BYTE, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *RxByte_H = paramext.p1;
	*RxByte_L = paramext.p2;
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_tx_byte(int port, unsigned int* TxByte_H, unsigned int* TxByte_L)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_TX_BYTE, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

	*TxByte_H = paramext.p1;
	*TxByte_L = paramext.p2;
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_eth_port_congestion(u8 port, u8 *p_congestion)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_CONGESTION, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    *p_congestion = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        STORMCTRL API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_port(u16 port)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_STORM_CTRL_PORT, 0, (void *)(u32)port);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_unit(u8 unit)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_STORM_CTRL_UNIT, 0, (void *)(u32)unit);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_type(u8 type)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_STORM_CTRL_TYPE, 0, (void *)(u32)type);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_mode(u8 mode)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_STORM_CTRL_MODE, 0, (void *)(u32)mode);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_storm_ctrl_period(u8 period)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_STORM_CTRL_PERIOD, 0, (void *)(u32)period);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        TRTCM API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_dstrtcm_enable(u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_DS_TRTCM_ENABLE, 0, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_dstrtcm_rule(ECNT_SWITCHMGR_TRTCM *trtcm)
{
    int ret = 0;
    mt7530_switch_api_trtcm_t trtcm_p;
    
    trtcm_p.trtcmId = trtcm->trtcmId;
    trtcm_p.CBS = trtcm->CBS;
    trtcm_p.PBS = trtcm->PBS;
    trtcm_p.CIR = trtcm->CIR;
    trtcm_p.PIR = trtcm->PIR;
    ret = macMT7530gswAPIEntrance(CMD_SET_DS_TRTCM, 0, (void *)&trtcm_p);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_dstrtcm_rule(ECNT_SWITCHMGR_TRTCM_ACL *trtcm_acl)
{
    int ret = 0, i = 0;
    mt7530_switch_api_trtcm_acl_t trtcm_acl_p;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_DS_TRTCM, 0, (void *)&trtcm_acl_p);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    trtcm_acl->enable = trtcm_acl_p.enable;
    for (i = 0; i < 32; i++)
    {
        trtcm_acl->trtcm_acl_table[i].trtcmId = trtcm_acl_p.trtcm_acl_table[i].trtcmId;
        trtcm_acl->trtcm_acl_table[i].CBS = trtcm_acl_p.trtcm_acl_table[i].CBS;
        trtcm_acl->trtcm_acl_table[i].PBS = trtcm_acl_p.trtcm_acl_table[i].PBS;
        trtcm_acl->trtcm_acl_table[i].CIR = trtcm_acl_p.trtcm_acl_table[i].CIR;
        trtcm_acl->trtcm_acl_table[i].PIR = trtcm_acl_p.trtcm_acl_table[i].PIR;
    }
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        TRAFFIC API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_rgmii_trafficsetting(u8 type, u8 interface)
{
    int ret = 0;
    rgmii_setting_s para;
    
    para.wan_type = type;
    para.interface = interface;
    ret = macMT7530gswAPIEntrance(CMD_TRAFFIC_SET, 0 ,(void *)&para);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_rgmii_setting(u8 mode)
{
    int ret = 0;
    rgmii_setting_s para;
    
    para.interface = 1;
    para.mode = mode;
    ret = macMT7530gswAPIEntrance(CMD_TRAFFIC_RGMII_MODE, 0 ,(void *)&para);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_rgmii_display()
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_TRAFFIC_DISPLAY, 0 ,NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        RATECTL API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_port_rate_limit_enable(u8 port, u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_RATELMTEN, port, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_rate_limit_enable(u8 port, u8 *p_enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_RATELMTEN, port, NULL);
    if (ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    *p_enable = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_rate_limiting(u8 port, u32 *p_cir, u32 *p_pir)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = (u32)p_cir;
    paramext.p2 = (u32)p_pir;
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_RATELMT, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_rate_limiting(u8 port, u32 cir, u32 pir)
{
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = cir;
    paramext.p2 = pir;
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_RATELMT, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_policing_enable(u8 port, u8 enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_POLEN, port, (void *)(u32)enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_policing_enable(u8 port, u8 *p_enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_POLEN, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    *p_enable = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_policing(u8 port, u32 *p_cir, u32 *p_cbs, u32 *p_ebs)
{
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;

    paramext.p1 = (u32)p_cir;
    paramext.p2 = (u32)p_cbs;
    paramext.p3 = (u32)p_ebs;
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_POL, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_port_policing(u8 port, u32 cir, u32 cbs, u32 ebs)
{
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;

    paramext.p1 = cir;
    paramext.p2 = cbs;
    paramext.p3 = ebs;
    ret = macMT7530gswAPIEntrance(CMD_SET_PORT_POL, port, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_traffic_descriptor(u8 port, ECNT_SWITCHMGR_TRAFFIC_DESCRIPTOR *trafficDescriptor_p)
{
    int ret = 0;
    mt7530_switch_api_trafficDescriptor_t trafficDescriptor;
    
    trafficDescriptor.CBS = trafficDescriptor_p->CBS;
    trafficDescriptor.CIR = trafficDescriptor_p->CIR;
    trafficDescriptor.colourMode = trafficDescriptor_p->colourMode;
    trafficDescriptor.direction = trafficDescriptor_p->direction;
    trafficDescriptor.engressColourMarking = trafficDescriptor_p->engressColourMarking;
    trafficDescriptor.ingressColourMarking = trafficDescriptor_p->ingressColourMarking;
    trafficDescriptor.meterType = trafficDescriptor_p->meterType;
    trafficDescriptor.PBS = trafficDescriptor_p->PBS;
    trafficDescriptor.PIR = trafficDescriptor_p->PIR;
    ret = macMT7530gswAPIEntrance(CMD_SET_TRAFFIC_DESCRIPTOR, port, (void *)&trafficDescriptor);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_traffic_descriptor(u8 port, ECNT_SWITCHMGR_TRAFFIC_DESCRIPTOR *p_trafficDescriptor_p)
{
    int ret = 0;
    mt7530_switch_api_trafficDescriptor_t trafficDescriptor;
    
    memset(&trafficDescriptor, 0, sizeof(mt7530_switch_api_trafficDescriptor_t));
    trafficDescriptor.direction = p_trafficDescriptor_p->direction;
    ret = macMT7530gswAPIEntrance(CMD_GET_TRAFFIC_DESCRIPTOR, port, (void *)&trafficDescriptor);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    p_trafficDescriptor_p->CBS = trafficDescriptor.CBS;
    p_trafficDescriptor_p->CIR = trafficDescriptor.CIR;
    p_trafficDescriptor_p->colourMode = trafficDescriptor.colourMode;
    p_trafficDescriptor_p->engressColourMarking = trafficDescriptor.engressColourMarking;
    p_trafficDescriptor_p->ingressColourMarking = trafficDescriptor.ingressColourMarking;
    p_trafficDescriptor_p->meterType = trafficDescriptor.meterType;
    p_trafficDescriptor_p->PBS = trafficDescriptor.PBS;
    p_trafficDescriptor_p->PIR = trafficDescriptor.PIR;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_clean_traffic_descriptor(u8 port, u8 direction)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_CLR_TRAFFIC_DESCRIPTOR, port, (void *)(u32)direction);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        DSP5 API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_dsp5_rate_limit_enable(u8 ratectl_enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_DSP5_RATELIMIT_ENABLE, 0, (void *)(u32)ratectl_enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_dsp5_rate_limit(u32 rate)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_DSP5_RATELIMIT, 0, (void *)(u32)rate);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_dsp5_qos(u8 qos_enable)
{
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_DSP5_QOS, 0, (void *)(u32)qos_enable);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        ACL API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_port_enable(u8 acl_port, u8 acl_en)
{   
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = acl_port;
    paramext.p2 = acl_en;
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_PORT_ENABLE, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_multi_hit(u8 acl_multi)
{   
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_MULTI_HIT, 0, (void *)(u32)acl_multi);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_tbl_clr(void)
{   
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_TBL_CLR, 0, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_tbl_disp(u8 ext_switch)
{   
    int ret = 0;
    
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_TBL_DISP, 0, (void *)(u32)ext_switch);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_tbl_add(u8 tbl_idx, u32 vawd1, u32 vawd2)
{   
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;

    paramext.p1 = tbl_idx;
    paramext.p2 = vawd1;
    paramext.p3 = vawd2;
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_TBL_ADD, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_mask_tbl_add(u8 tbl_idx, u32 vawd1, u32 vawd2)
{   
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;
    
    paramext.p1 = tbl_idx;
    paramext.p2 = vawd1;
    paramext.p3 = vawd2;
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_MASK_TBL_ADD, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_rule_tbl_add(u8 tbl_idx, u32 vawd1, u32 vawd2)
{   
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;

    paramext.p1 = tbl_idx;
    paramext.p2 = vawd1;
    paramext.p3 = vawd2;
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_RULE_TBL_ADD, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_rate_tbl_add(u8 tbl_idx, u32 vawd1, u32 vawd2)
{   
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;

    paramext.p1 = tbl_idx;
    paramext.p2 = vawd1;
    paramext.p3 = vawd2;
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_RATE_TBL_ADD, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_upstream_notunicast_ratelimit(u8 enable, u32 rate)
{
    int ret = 0;
    mt7530_switch_acl_info acl_rule_info ;

    acl_rule_info.rate = rate ;
    acl_rule_info.mode = ACL_SET_MODE_NO_UNICAST ;      /* 0:for no unicast*/
    acl_rule_info.action = 0 ;                          /*0 : ratelimit  1 : drop , no use now*/
    acl_rule_info.enable = enable ;
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_RULE, 0, (void *)(u32)&acl_rule_info);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_acl_upstream_vlan_ratelimit(u8 index, u8 enable, u32 vlan, u32 rate)
{
    int ret = 0;
    mt7530_switch_acl_info acl_rule_info ;
    
    acl_rule_info.pattern_info.vlan = vlan ;
    acl_rule_info.rate = rate ;
    acl_rule_info.mode = ACL_SET_MODE_VLAN ;        /* 5: for vlan*/
    acl_rule_info.index = index ;
    acl_rule_info.action = 0 ;                      /*0 : ratelimit  1 : drop , no use now*/
    acl_rule_info.enable = enable ;
    ret = macMT7530gswAPIEntrance(CMD_SET_ACL_RULE, 0, (void *)(u32)&acl_rule_info);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_mac_match_acl(ECNT_SWITCHMGR_DEV_SPEED *p_dev_mac_acl)
{
    int ret = 0;
    int i;
    mt7530_switch_dev_speed_acl_t dev_mac_acl;
    printf("[%s]%d\n", __FUNCTION__, __LINE__);

    dev_mac_acl.enable = p_dev_mac_acl->enable;
    for (i = 0; i < ECNT_SWITCHMGR_TRTCM_MATCH_MAC_RULE_NUM; i++)
    {
        dev_mac_acl.trtcm_acl_mac_table[i].dev_mac[0] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[0];
        dev_mac_acl.trtcm_acl_mac_table[i].dev_mac[1] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[1];
        dev_mac_acl.trtcm_acl_mac_table[i].dev_mac[2] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[2];
        dev_mac_acl.trtcm_acl_mac_table[i].dev_mac[3] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[3];
        dev_mac_acl.trtcm_acl_mac_table[i].dev_mac[4] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[4];
        dev_mac_acl.trtcm_acl_mac_table[i].dev_mac[5] = p_dev_mac_acl->trtcm_acl_mac_table[i].dev_mac[5];
        dev_mac_acl.trtcm_acl_mac_table[i].ds_used = p_dev_mac_acl->trtcm_acl_mac_table[i].ds_used;
        dev_mac_acl.trtcm_acl_mac_table[i].mac_entry_index = p_dev_mac_acl->trtcm_acl_mac_table[i].mac_entry_index;
        dev_mac_acl.trtcm_acl_mac_table[i].ds_queue_speed = p_dev_mac_acl->trtcm_acl_mac_table[i].ds_queue_speed;
        dev_mac_acl.trtcm_acl_mac_table[i].us_queue_speed = p_dev_mac_acl->trtcm_acl_mac_table[i].us_queue_speed;
    }
    ret = macMT7530gswAPIEntrance(CMD_SET_DEV_TRTCM_RULE, 0, (void *)&dev_mac_acl);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*        REGISTER API
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_set_gsww(u32 offset, u32 value)
{   
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = offset;
    paramext.p2 = value;
    ret = macMT7530gswAPIEntrance(CMD_SET_GSWW, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_gswr(u32 offset, u32 *p_value)
{   
    int ret = 0;
    mt7530_switch_api_paramext2 paramext;

    paramext.p1 = offset;
    ret = macMT7530gswAPIEntrance(CMD_GET_GSWR, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_value = paramext.p2;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_gsww2(u32 ext_switch, u32 reg, u32 value)
{   
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;

    paramext.p1 = ext_switch;
    paramext.p2 = reg;
    paramext.p3 = value;
    ret = macMT7530gswAPIEntrance(CMD_SET_GSWW2, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }
    
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_gswr2(u32 ext_switch, u32 reg, u32 *p_value)
{   
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;

    paramext.p1 = ext_switch;
    paramext.p2 = reg;
    ret = macMT7530gswAPIEntrance(CMD_GET_GSWR2, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_value = paramext.p3;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_mii(int phyaddr, int reg, u32 value)
{
    int ret = 0;
    mt7530_switch_config_t paramext;

    paramext.phyaddr = phyaddr;
    paramext.reg = reg;
    paramext.value = value;
    ret = macMT7530gswAPIEntrance(CMD_MIIW, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_mii(int phyaddr, int reg, u32 *p_value)
{
    int ret = 0;
    mt7530_switch_config_t paramext;

    paramext.phyaddr = phyaddr;
    paramext.reg = reg;
    ret = macMT7530gswAPIEntrance(CMD_MIIR, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_value = paramext.value;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_mii2(int ext_switch, int phyAddr, int reg, u32 value)
{
    int ret = 0;
    mt7530_switch_config_t paramext;

    paramext.ext_switch = ext_switch;
    paramext.phyaddr = phyAddr;
    paramext.reg = reg;
    paramext.value = value;
    ret = macMT7530gswAPIEntrance(CMD_MIIW2, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}


ECNT_SWITCHMGR_RET switchmgr_lib_get_mii2(int ext_switch, int phyAddr, int reg, u32 *p_value)
{
    int ret = 0;
    mt7530_switch_config_t paramext;

    paramext.ext_switch = ext_switch;
    paramext.phyaddr = phyAddr;
    paramext.reg = reg;
    ret = macMT7530gswAPIEntrance(CMD_MIIR2, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_value = paramext.value;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_jabber(u8 port, u8 *p_status)
{
    int ret = 0;
   
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_JABBER, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_status = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_remote_fault(u8 port, u8 *p_status)
{
    int ret = 0;
   
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_REMOTE_FAULT, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_status = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_port_mdi_crossover(u8 port, u8 *p_status)
{
    int ret = 0;
   
    ret = macMT7530gswAPIEntrance(CMD_GET_PORT_MDI_CROSSOVER, port, NULL);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_status = ret;
    return ECNT_SWITCHMGR_SUCCESS;
}

/************************************************************************
*       OTHER APIs
*************************************************************************/
ECNT_SWITCHMGR_RET switchmgr_lib_get_emii(u32 phyaddr, u32 devaddr, u32 reg, u32 *p_value)
{
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;

    paramext.p1 = phyaddr;
    paramext.p2 = devaddr;
    paramext.p3 = reg;
    ret = macMT7530gswAPIEntrance(CMD_EMIIR, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    *p_value = paramext.p4;
    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_emii(u32 phyaddr, u32 devaddr, u32 reg, u32 value)
{
    int ret = 0;
    mt7530_switch_api_paramext4 paramext;

    paramext.p1 = phyaddr;
    paramext.p2 = devaddr;
    paramext.p3 = reg;
    paramext.p4 = value;
    ret = macMT7530gswAPIEntrance(CMD_EMIIW, 0, (void *)&paramext);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_get_wol(ECNT_SWITCHMGR_WOL_STATUS *wol_p)
{
    int ret = 0;

    ret = macMT7530gswAPIEntrance(CMD_GET_WOL_STATUS, 0, (void *)wol_p);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

ECNT_SWITCHMGR_RET switchmgr_lib_set_wol(ECNT_SWITCHMGR_WOL_CONFIG *wol_p)
{
    int ret = 0;

    ret = macMT7530gswAPIEntrance(CMD_SET_WOL_CONFIG, 0, (void *)wol_p);
    if(ret < 0)
    {
        return ECNT_SWITCHMGR_FAIL;
    }

    return ECNT_SWITCHMGR_SUCCESS;
}

