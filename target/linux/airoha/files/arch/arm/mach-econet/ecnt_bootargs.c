/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/mtd/rt_flash.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define BOOTARGS_TCLINUX_INFO_STR	("tclinux_info")
	
#define BOOTARGS_MAC_SIZE			(6)
#define BOOTARGS_MAC_STR			("ethaddr")

#define BOOTARGS_ONU_TYPE_STR		("onu_type")
#define BOOTARGS_QDMA_INIT_STR		("qdma_init")
#define BOOTARGS_SN_INIT_STR		("sn_test")

#define BOOTARGS_GPON_SN_SIZE		(20)
#define BOOTARGS_GPON_SN_STR		("gpon_sn")

#define BOOTARGS_BOOTFLAG_STR		("bootflag")

#define BOOTARGS_BOARD_ARGS_SIZE	(2)
#define BOOTARGS_BOARD_ARGS_STR		("board_args")

#define LAN_PORT_NUM	(BOOTARGS_BOARD_ARGS_SIZE * 4)
/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/


/************************************************************************
*                  STATIC VARIABLE DECLARATIONS
*************************************************************************
*/
uint64_t tclinux_info[] = {TCLINUX_INFO_UNKNOW_VAL,	/* 0: tclinux.bin size */
						TCLINUX_INFO_UNKNOW_VAL,	/* 1: master kernel offset */
						TCLINUX_INFO_UNKNOW_VAL,	/* 2. master kernel size */
						TCLINUX_INFO_UNKNOW_VAL,	/* 3. master rootfs offset */
						TCLINUX_INFO_UNKNOW_VAL,	/* 4. master rootfs size */
						TCLINUX_INFO_UNKNOW_VAL,	/* 5: slave tclinux.bin size */
						TCLINUX_INFO_UNKNOW_VAL,	/* 6. slave kernel offset */
						TCLINUX_INFO_UNKNOW_VAL,	/* 7. slave kernel size */
						TCLINUX_INFO_UNKNOW_VAL,	/* 8. slave rootfs offset */
						TCLINUX_INFO_UNKNOW_VAL};	/* 9. slave rootfs size */

unsigned char tclinux_mac[BOOTARGS_MAC_SIZE] = {0x00, 0xAA, 0xBB, 0x01, 0x23, 0x40};
char onutype = 0;
unsigned int qdmainit = 0;
char sninit = 0;
unsigned char gpon_sn[BOOTARGS_GPON_SN_SIZE];
char bootflag = 0;

unsigned int board_args[BOOTARGS_BOARD_ARGS_SIZE] = {0xffffffff, 0x00000000};
unsigned char lan_port[LAN_PORT_NUM] = {0xff, 0xff, 0xff, 0xff, 0x09, 0x0a, 0x0b, 0x0c};
int boardflag = 0;
EXPORT_SYMBOL(boardflag);
enum HGSMII_LAN_PORT
{
	HSGMII_USB = 0x09,
	HSGMII_PCIE0,
	HSGMII_PCIE1,
	HSGMII_ETH,
};
int is_hsgmii_lan[LAN_PORT_NUM] = {0,0,0,0,1,1,1,1};
/*
hsgmii_index[0] : usb idx,range 0-7
hsgmii_index[1] : pcie0 idx,range 0-7
hsgmii_index[2] : pcie1 idx,range 0-7
hsgmii_index[3] : eth idx,range 0-7
other for reserve use
*/
int hsgmii_index[4] = {4, 5, 6, 7};

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
static int __init early_tclinux_info(char *p)
{

	unsigned long start, size;
	char *endp;
	int i;

	tclinux_info[0] = memparse(p, &endp);
	for(i = 1; *endp == ','; i++) {
		tclinux_info[i] = memparse(endp + 1, &endp);
	}

	return 0;
}
early_param(BOOTARGS_TCLINUX_INFO_STR, early_tclinux_info);

static int __init early_tclinux_mac(char *p)
{

	int ret;

	ret = sscanf(p, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
								&tclinux_mac[0],
								&tclinux_mac[1],
								&tclinux_mac[2],
								&tclinux_mac[3],
								&tclinux_mac[4],
								&tclinux_mac[5]);

	if(ret != BOOTARGS_MAC_SIZE) {
		printk("parse ethaddr error:%d\n", ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_MAC_STR, early_tclinux_mac);

int get_partition_info(unsigned int index, uint64_t *val)
{
	*val = tclinux_info[index];
	
	if(TCLINUX_INFO_UNKNOW_VAL == *val) {
		printk("Get partition error, index:%d!\n", index);
		return -1;
	}

	return 0;
}
EXPORT_SYMBOL(get_partition_info);

int get_ethaddr(unsigned char *ethaddr, int len)
{
	if(len != 6) {
		printk("Error get ethaddr length!\n");
		return -1;
	}

	if(ethaddr == NULL) {
		printk("Error! ethaddr is NULL.\n");
		return -1;
	}

	memcpy(ethaddr, tclinux_mac, len);

	return 0;
}
EXPORT_SYMBOL(get_ethaddr);

static int __init early_onutype(char *p)
{

	int ret;

	ret = sscanf(p, "%hhx", &onutype);

	if(ret != 1) {
		printk("parse onutype error:%d\n", ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_ONU_TYPE_STR, early_onutype);

char get_onutype(void)
{
	return onutype;
}
EXPORT_SYMBOL(get_onutype);

static int __init early_qdmainit(char *p)
{

	int ret;

	ret = sscanf(p, "%x", &qdmainit);
	if(ret != 1) {
		printk("parse qdma_init error:%d\n", ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_QDMA_INIT_STR, early_qdmainit);

unsigned int get_qdmainit(void)
{
	return qdmainit;
}
EXPORT_SYMBOL(get_qdmainit);

static int __init early_sninit(char *p)
{

	int ret;

	ret = sscanf(p, "%hhx", &sninit);
	if(ret != 1) {
		printk("parse sn_init error:%d\n", ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_SN_INIT_STR, early_sninit);

char get_sninit(void)
{
	return sninit;
}

EXPORT_SYMBOL(get_sninit);

static int __init early_bootflag(char *p)
{

	int ret;

	ret = sscanf(p, "%hhx", &bootflag);
	if(ret != 1) {
		printk("parse bootflag error:%d\n", ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_BOOTFLAG_STR, early_bootflag);

char get_bootflag(void)
{
	return bootflag;
}
EXPORT_SYMBOL(get_bootflag);

static int __init early_board_args(char *p)
{
	int ret;
	int i, j ,offset=0;

	memset(is_hsgmii_lan,0,sizeof(is_hsgmii_lan));
	memset(hsgmii_index,-1,sizeof(hsgmii_index));
	
	ret = sscanf(p, "%08x:%08x", 
				&board_args[0],
				&board_args[1]);	
	if(ret != BOOTARGS_BOARD_ARGS_SIZE) {
		printk("parse %s error:%d\n", BOOTARGS_BOARD_ARGS_STR, ret);
		return -1;
	}
	for(;i < LAN_PORT_NUM; i++)
	{
		j = i/4;
		offset = (3 - i + 4 * j)*8;
		lan_port[i] = (board_args[j] >> offset) & 0xff;

		switch(lan_port[i])
		{
			case HSGMII_USB:
				hsgmii_index[0] = i;
				break;
			case HSGMII_PCIE0:
				hsgmii_index[1] = i;
				break;
			case HSGMII_PCIE1:
				hsgmii_index[2] = i;
				break;
			case HSGMII_ETH:
				hsgmii_index[3] = i;
					break;
			default:
				continue;
		}

		is_hsgmii_lan[i] = 1;
	}
	boardflag = 1;
	
	return 0;
}
early_param(BOOTARGS_BOARD_ARGS_STR, early_board_args);


unsigned char get_lan_info(int index)
{
	if(index < LAN_PORT_NUM && index >= 0)
		return lan_port[index];
	else
		return 0;
}
EXPORT_SYMBOL(get_lan_info);

int check_lanport_type(int index)
{
	if(index < LAN_PORT_NUM && index >= 0)
		return is_hsgmii_lan[index];
	return 0;
}
EXPORT_SYMBOL(check_lanport_type);

/*
serdes:
0:usb
1:pcie0
2:pcie1
3:eth
other:reserve
*/
int get_hsgmii_idx(int serdes)
{
	return hsgmii_index[serdes];
}
EXPORT_SYMBOL(get_hsgmii_idx);

int get_board_flag(void)
{
	return boardflag;
}
EXPORT_SYMBOL(get_board_flag);

/*
 * 8811 PHY address list.
 * The bootloader passes "8811phy_addr=xx-xx-xx-xx-xx"; the 8811_bbu_api
 * module asks for it through get_8811phy_addr(). Taken from the reference
 * SDK kernel, add-only so existing bootargs handling is untouched.
 */
#define BOOTARGS_8811PHY_ADDR_SIZE	(5)
#define BOOTARGS_8811PHY_ADDR_STR	("8811phy_addr")

unsigned char phy8811Addrs[BOOTARGS_8811PHY_ADDR_SIZE] = {0, 0, 0, 0, 0};

static int __init early_8811phy_addr(char *p)
{
	int ret;

	ret = sscanf(p, "%hhx-%hhx-%hhx-%hhx-%hhx",
		     &phy8811Addrs[0],
		     &phy8811Addrs[1],
		     &phy8811Addrs[2],
		     &phy8811Addrs[3],
		     &phy8811Addrs[4]);

	if (ret != BOOTARGS_8811PHY_ADDR_SIZE) {
		printk("parse 8811 phy addrs error:%d\n", ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_8811PHY_ADDR_STR, early_8811phy_addr);

int get_8811phy_addr(unsigned char *phyaddr, int len)
{
	if (len != BOOTARGS_8811PHY_ADDR_SIZE) {
		printk("Error get phy addr length!\n");
		return -1;
	}

	if (phyaddr == NULL) {
		printk("Error! phyaddr is NULL.\n");
		return -1;
	}

	memcpy(phyaddr, phy8811Addrs, len);

	return 0;
}
EXPORT_SYMBOL(get_8811phy_addr);

/*
 * Hybrid mode flag, parsed from "hybrid=0x.." and consumed by fe_core.
 * Taken from the reference SDK kernel; add-only.
 */
#define BOOTARGS_HYBRID_MODE		("hybrid")

unsigned char hybrid_mode = 0;

static int __init early_hybrid_mode(char *p)
{
	int ret;

	ret = sscanf(p, "%x", &hybrid_mode);
	if (ret != 1) {
		printk("parse hybrid error:%d\n", ret);
		return -1;
	}

	return 0;
}
early_param(BOOTARGS_HYBRID_MODE, early_hybrid_mode);

unsigned char get_hybrid_mode(void)
{
	return hybrid_mode;
}
EXPORT_SYMBOL(get_hybrid_mode);

