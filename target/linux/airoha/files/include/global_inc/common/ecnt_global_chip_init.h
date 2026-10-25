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

#include <linux/kernel.h>
#include <linux/timer.h>
#include <asm/tc3162/tc3162.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#include <ecnt_event_global/ecnt_event_system.h>
#include <ecnt_hook/ecnt_hook_ephy.h>
#include <asm/tc3162/ledcetrl.h>
#endif

enum {
	__ECNT_CHIP_INIT_XPON_ACTION,
	__ECNT_CHIP_INIT_USB0_ACTION,
	__ECNT_CHIP_INIT_USB1_ACTION,
	__ECNT_CHIP_INIT_PCIE0_ACTION,
	__ECNT_CHIP_INIT_PCIE1_ACTION,
	__ECNT_CHIP_INIT_SERDES_ACTION,

	/* all new action constant should be added above this line */
	__ECNT_CHIP_INIT_ACTION_MAX,
};

enum {
	INTERFACE_PKG_ID = 0,
	INTERFACE_XPON_IDX,
	INTERFACE_USB0_IDX,
	INTERFACE_USB1_IDX,
	INTERFACE_PCIE0_IDX,
	INTERFACE_PCIE1_IDX,

	/* all new IDX should be added above this line */
	INTERFACE_IDX_MAX,
};

typedef struct {
	unsigned int package_id;
	union {
		struct {
			unsigned int isXpon		: 1;
			unsigned int isUsbPort0	: 1;
			unsigned int isUsbPort1	: 1;
			unsigned int isPcie0	: 1;
			unsigned int isPcie1	: 1;
			unsigned int rfu		: 27;
		} field;
		unsigned int raw;
	};
} INTERFACE_INFO;

#define END_PACKAGE_ID	(0xFFFFFFFF)

static void (*__ecnt_init_hook[__ECNT_CHIP_INIT_ACTION_MAX])(void) = {0};
static int package_id_idx = END_PACKAGE_ID;
struct task_struct *dis_if_task;

#if defined(TCSUPPORT_CPU_EN7581) && defined(TCSUPPORT_USBHOST)
	extern void ecnt_u3h_phy_PowerDown(unsigned int port_num);
#else
	void ecnt_u3h_phy_PowerDown(unsigned int port_num)
	{	
		/* usb_ext did not compile */
	}
#endif

#ifdef TCSUPPORT_CPU_ARMV8
#define DT_MEMORY_BASE  0x80000000 /* should be got from kernel later */
#define DT_MEMORY_SIZE  0x40000000 /* should be replaced by the result of DDR Calibration later */

static inline unsigned int regRead32_dis_if(unsigned int *physAddr)
{
    void *virtAddr;
	unsigned int val;

    if (((uintptr_t)physAddr)>=DT_MEMORY_BASE) {
        virtAddr = (void *)phys_to_virt((phys_addr_t)physAddr);
        ecnt_dcache_inv((void *)virtAddr, 4);
		val = readl(virtAddr);
    }
    else {
        virtAddr = ioremap((phys_addr_t)physAddr, 4);
		if(virtAddr == NULL) {
			printk("%s: ioremap fail.\n", __func__);
			return 0xdeadbeef;
		}
		//printk("regRead32 physAddr:0x%x virtAddr:0x%x ", (unsigned int)physAddr, (unsigned int)virtAddr);
		val = readl(virtAddr);
		//printk("val:0x%x\n", val);
		iounmap(virtAddr);
    }
	
    return val;
}

static inline void regWrite32_dis_if(unsigned int *physAddr, unsigned int value)
{
    void *virtAddr;

	if (((uintptr_t)physAddr)>=DT_MEMORY_BASE) {
		virtAddr = (void *)phys_to_virt((phys_addr_t)physAddr);
		writel(value, virtAddr);
        ecnt_dcache_wback_inv((void *)virtAddr, 4);
    }
    else {
		virtAddr = ioremap((phys_addr_t)physAddr, 4);
		if(virtAddr == NULL) {
			printk("%s: ioremap fail.\n", __func__);
			return;
		}
		//printk("regWrite32 physAddr:0x%x virtAddr:0x%x, value:0x%x\n", (unsigned int)physAddr, (unsigned int)virtAddr, value);
		writel(value, virtAddr);        
		iounmap(virtAddr);
    }
}
#endif

static inline void __chip_pon_phy_off(void)
{
	#if defined(TCSUPPORT_CPU_EN7581)
	//regWrite32_dis_if((unsigned int *)0x1fa8b460, 0x0);
	ledTurnOn(LED_PHY_TX_POWER_DISABLE);
	#elif defined(TCSUPPORT_CPU_AN7552)
	regWrite32_dis_if((unsigned int *)0x1faf4304, 0x7f007fc0);
	#endif
}

static inline void __chip_ethernet_phy_off(void)
{
	#if defined(TCSUPPORT_CPU_EN7581)
	regWrite32_dis_if((unsigned int *)0x1fa7b460, 0x0);
	#elif defined(TCSUPPORT_CPU_AN7552)
	__u32 reg_val = 0;
	reg_val = regRead32_dis_if((unsigned int *)0x1fb5fc14)&(~0x0F000000);
	regWrite32_dis_if((unsigned int *)0x1fb5fc14, reg_val);
	#endif
}

static inline void __chip_pcie0_phy_off(void)
{
	#if defined(TCSUPPORT_CPU_EN7581)
	regWrite32_dis_if((unsigned int *)0x1fa5b460, 0x0);
	#elif defined(TCSUPPORT_CPU_AN7552)
	__u32 reg_val = 0;
	reg_val = regRead32_dis_if((unsigned int *)0x1fb00088)&(~0x00008000);
	regWrite32_dis_if((unsigned int *)0x1fb00088, reg_val);
	#endif
}

static inline void __chip_pcie1_phy_off(void)
{
	#if defined(TCSUPPORT_CPU_EN7581)
	regWrite32_dis_if((unsigned int *)0x1fa5c460, 0x0);
	#elif defined(TCSUPPORT_CPU_AN7552)
	__u32 reg_val = 0;
	reg_val = regRead32_dis_if((unsigned int *)0x1fb00088)&(~0x00004000);
	regWrite32_dis_if((unsigned int *)0x1fb00088, reg_val);
	#endif
}

static inline void __chip_usb0_u3phy_off(void)
{
	#if defined(TCSUPPORT_CPU_EN7581)
	__u32 reg_val = 0;
	ecnt_u3h_phy_PowerDown(1);
	reg_val = regRead32_dis_if((unsigned int *)0x1fb00830)|0x00008000;
	regWrite32_dis_if((unsigned int *)0x1fb00830, reg_val);
	#elif defined(TCSUPPORT_CPU_AN7552)
	ecnt_u3h_phy_PowerDown(1);
	#endif
}

static inline void __chip_usb1_u3phy_off(void)
{
	#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
	ecnt_u3h_phy_PowerDown(2);
	#endif
}

static inline void __chip_usb0_u2phy_off(void)
{
	#if defined(TCSUPPORT_CPU_EN7581)
	ecnt_u3h_phy_PowerDown(3);
	#endif
}

static inline void __chip_usb1_u2phy_off(void)
{
	#if defined(TCSUPPORT_CPU_EN7581)
	ecnt_u3h_phy_PowerDown(4);
	#elif defined(TCSUPPORT_CPU_AN7552)
	ecnt_u3h_phy_PowerDown(3);
	#endif
}

static inline void __chip_emmc_phy_off(void)
{
	#if defined(TCSUPPORT_CPU_EN7581)
	__u32 reg_val = 0;
	reg_val = regRead32_dis_if((unsigned int *)0x1fa201e4)&0xdfffffff;
	regWrite32_dis_if((unsigned int *)0x1fa201e4, reg_val);
	#endif
}


static inline int __ecnt_check_interface_all(void *arg)
{
	int i = 0;

	while(!kthread_should_stop()) {
		for (i = 0; i< __ECNT_CHIP_INIT_ACTION_MAX; i++) {
			if(__ecnt_init_hook[i]) {
				__ecnt_init_hook[i]();
			}
		}

		msleep(1000);
	}
	return 0;
}

static inline unsigned int get_pkg_info(int pkg_idx, int if_idx)
{
	INTERFACE_INFO if_info[] = {
		/* FPGA Verify */		/* PON, USB0, USB1, PCIE0, PCIE1 */
		{isFPGA,				.field={1, 1, 1, 1, 1}},

		/* Add newest package id of IC at here */
		/* AN7552*/
		{isAN7552CT,			.field={1, 1, 0, 1, 1}},
		{isAN7552ST,			.field={1, 1, 0, 0, 0}},
		{isAN7563CT,			.field={0, 1, 0, 1, 1}},
		{isAN7563PT,			.field={0, 1, 0, 1, 1}},
		{isAN7552FT,			.field={1, 1, 0, 1, 1}},
		/* AN7581*/
		{isAN7581GT,			.field={1, 1, 1, 1, 1}},
		{isAN7566GT,			.field={0, 1, 1, 1, 1}},
		{isAN7581PT,			.field={1, 1, 0, 1, 1}},
		{isAN7581ST,			.field={1, 1, 0, 0, 1}},
		{isAN7551PT,			.field={1, 1, 0, 1, 1}},
		{isAN7581CT,			.field={1, 1, 0, 1, 1}},
		{isAN7581DT,			.field={1, 1, 0, 0, 0}},
		{isAN7581FG,			.field={1, 1, 1, 1, 1}},
		{isAN7581FP,			.field={1, 1, 0, 1, 1}},
		{isAN7581FD,			.field={1, 1, 0, 0, 1}},
		{isAN7551GT,			.field={1, 1, 1, 1, 1}},
		{isAN7566PT,			.field={0, 1, 0, 1, 1}},
		{isAN7581IT,			.field={1, 1, 0, 1, 1}},

		/* EN7523 */
		{isEN7529DU,			.field={1, 1, 1, 1, 1}},
		{isEN7529DT,			.field={1, 1, 1, 1, 1}},
		{isEN7529CU,			.field={1, 1, 1, 1, 1}},
		{isEN7562DU,			.field={0, 1, 1, 1, 1}},
		{isEN7562DT,			.field={0, 1, 1, 1, 1}},
		{isEN7562CU,			.field={0, 1, 1, 1, 1}},
		{isEN7523GU,			.field={1, 0, 1, 0, 0}},
		{isEN7523DU,			.field={1, 0, 1, 1, 0}},
		{isEN7529GTH,			.field={1, 1, 1, 1, 1}},
		{isEN7562GTH,			.field={0, 1, 1, 1, 1}},
		{isEN7523SU,			.field={1, 0, 0, 0, 0}},
		{isEN7529GTS,			.field={1, 1, 0, 1, 1}},
		{isEN7562GTS,			.field={0, 1, 0, 1, 1}},
		{isEN7529IT,			.field={1, 1, 1, 1, 1}},
		{isEN7529CT,			.field={1, 1, 1, 1, 1}},
		{isEN7562CT,			.field={0, 1, 1, 1, 1}},
		{isEN7523DT,			.field={1, 0, 1, 1, 0}},
		{isEN7529DTM,			.field={1, 1, 1, 1, 1}},
		{isEN7562DTM,			.field={0, 1, 1, 1, 1}},
		{isEN7529ITM,			.field={1, 1, 1, 1, 1}},
		{isEN7529CTM,			.field={1, 1, 1, 1, 1}},
		{isEN7562CTM,			.field={0, 1, 1, 1, 1}},
		{isEN7523DTM,			.field={1, 0, 1, 1, 0}},
		
		/* EN7528 */
		{isEN7528HU,			.field={1, 1, 1, 1, 1}},
		{isEN7528DU,			.field={1, 1, 1, 1, 1}},
		{isEN7561DU,			.field={0, 1, 1, 1, 1}},
		{isEN7526FH_EN7528DU,	.field={1, 1, 1, 1, 0}},
		{isEN7521G_EN7528DU,	.field={1, 1, 1, 0, 0}},
		{isEN7561HU,			.field={0, 1, 1, 1, 1}},

		/* EN7580 */
		{isEN7580GT,	.field={1, 1, 1, 1, 1}},
		{isEN7580ST,	.field={1, 0, 0, 0, 0}},
		{isEN7580GAT,	.field={1, 1, 1, 1, 1}},
		{isEN7565,		.field={0, 1, 1, 1, 1}},

		/* EN7516 */
		{isEN7516G,	.field={0, 1, 1, 1, 1}},

		/* EN7527 */
		{isEN7527G,	.field={1, 1, 1, 1, 1}},
		{isEN7561G,	.field={0, 1, 1, 1, 1}},
		
		/* EN7512 */
		{isEN7512,	.field={0, 1, 0, 1, 0}},
		{isEN7513,	.field={0, 1, 1, 1, 1}},
		{isEN7513G,	.field={0, 1, 1, 1, 1}},
		
		/* EN7521, EN7521FC */
		{isEN7521FCUD,	.field={1, 1, 0, 0, 0}},
		{isEN7521F,		.field={1, 0, 0, 0, 0}},
		{isEN7521S,		.field={1, 0, 0, 0, 0}},
		{isEN7526D,		.field={1, 1, 1, 1, 1}},
		{isEN7526F,		.field={1, 1, 0, 1, 0}},
		{isEN7526G,		.field={1, 1, 1, 1, 1}},
		{isEN7526FT,	.field={1, 1, 0, 1, 0}},
		{isEN7526FP,	.field={1, 1, 0, 1, 0}},
		{isEN7526FT_C,	.field={1, 1, 0, 1, 0}},

		/*must last element*/
	    {END_PACKAGE_ID,		.field={0, 0, 0, 0, 0}}
	};

	switch(if_idx) {
		case INTERFACE_PKG_ID:
			return if_info[pkg_idx].package_id;
			break;
		case INTERFACE_XPON_IDX:
			return if_info[pkg_idx].field.isXpon;
			break;
		case INTERFACE_USB0_IDX:
			return if_info[pkg_idx].field.isUsbPort0;
			break;
		case INTERFACE_USB1_IDX:
			return if_info[pkg_idx].field.isUsbPort1;
			break;
		case INTERFACE_PCIE0_IDX:
			return if_info[pkg_idx].field.isPcie0;
			break;
		case INTERFACE_PCIE1_IDX:
			return if_info[pkg_idx].field.isPcie1;
			break;
		default:
			return END_PACKAGE_ID;
	}
}

static inline int __chip_has_modules(int module)
{
	switch(module) {
		case __ECNT_CHIP_INIT_XPON_ACTION:
			return get_pkg_info(package_id_idx, INTERFACE_XPON_IDX);
		case __ECNT_CHIP_INIT_USB0_ACTION:
			return get_pkg_info(package_id_idx, INTERFACE_USB0_IDX);
		case __ECNT_CHIP_INIT_USB1_ACTION:
			return get_pkg_info(package_id_idx, INTERFACE_USB1_IDX);
		case __ECNT_CHIP_INIT_PCIE0_ACTION:
			return get_pkg_info(package_id_idx, INTERFACE_PCIE0_IDX);
		case __ECNT_CHIP_INIT_PCIE1_ACTION:
			return get_pkg_info(package_id_idx, INTERFACE_PCIE1_IDX);
		default:
			return 0;
	}
}

static inline void __chip_init_check_xpon(void)
{
#ifndef TCSUPPORT_CPU_ARMV8
	#define PHY_CSR_PHYSET3			(0xBFAF0108)
	#define PHY_CSR_PHYSET4			(0xBFAF010C)
	#define PHY_CSR_ANAPWD			(0xBFAF0150)
	#define PHY_CSR_ANATXREG2		(0xBFAF01A4)

	#define PHY_CSR_PHYSET3_PLLRST						(1 << 31)
	#define PHY_CSR_PHYSET3_XP_SRST						(1 << 27)
	#define PHY_CSR_PHYSET4_AUTOK_SQ					(0xfffffffb)
	#define PHY_CSR_ANAPWD_BIAS_PWD						(1 << 9)
	#define PHY_CSR_ANAPWD_CDR_PI_PWD					(1 << 7)
	#define PHY_CSR_ANATXREG2_PON_TX_TXOP_PD_PRE		(1 << 17)
	#define PHY_CSR_ANATXREG2_PON_TX_BEN_TXOP_PD_PRE	(1 << 18)

	__u32 reg_val = 0;

	/*PON PHY PLL & Software Reset*/
	reg_val = ioread32((void __iomem *)(PHY_CSR_PHYSET3));
	reg_val |= PHY_CSR_PHYSET3_PLLRST;
	reg_val |= PHY_CSR_PHYSET3_XP_SRST;
	iowrite32(reg_val, (void __iomem *)(PHY_CSR_PHYSET3));

	/*PON PHY DA_PON_RX_SIGDET_CAL_EN*/
	reg_val = ioread32((void __iomem *)(PHY_CSR_PHYSET4));
	reg_val &= PHY_CSR_PHYSET4_AUTOK_SQ;
	iowrite32(reg_val, (void __iomem *)(PHY_CSR_PHYSET4));

	/*PON PHY DA_PON_RX_CDR_PI_PWD & DA_PON_BIAS_PWD*/
	reg_val = ioread32((void __iomem *)(PHY_CSR_ANAPWD));
	reg_val |= PHY_CSR_ANAPWD_BIAS_PWD;
	reg_val |= PHY_CSR_ANAPWD_CDR_PI_PWD;
	iowrite32(reg_val, (void __iomem *)(PHY_CSR_ANAPWD));

	/*PON PHY RG_PON_TX_TXOP_PD & RG_PON_TX_BEN_TXOP_PD*/
	reg_val = ioread32((void __iomem *)(PHY_CSR_ANATXREG2));
	reg_val |= PHY_CSR_ANATXREG2_PON_TX_TXOP_PD_PRE;
	reg_val |= PHY_CSR_ANATXREG2_PON_TX_BEN_TXOP_PD_PRE;
	iowrite32(reg_val, (void __iomem *)(PHY_CSR_ANATXREG2));

	#undef PHY_CSR_PHYSET3			
	#undef PHY_CSR_PHYSET4			
	#undef PHY_CSR_ANAPWD			
	#undef PHY_CSR_ANATXREG2		

	#undef PHY_CSR_PHYSET3_PLLRST						
	#undef PHY_CSR_PHYSET3_XP_SRST					
	#undef PHY_CSR_PHYSET4_AUTOK_SQ					
	#undef PHY_CSR_ANAPWD_BIAS_PWD					
	#undef PHY_CSR_ANAPWD_CDR_PI_PWD					
	#undef PHY_CSR_ANATXREG2_PON_TX_TXOP_PD_PRE		
	#undef PHY_CSR_ANATXREG2_PON_TX_BEN_TXOP_PD_PRE	

	return;
	#else
	#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
		__chip_pon_phy_off();
	#else
		__u32 reg_val = 0;
	
		//disable 7523 PHY
		reg_val = regRead32_dis_if((unsigned int *)0x1faf4300)&0xffffffcf;
		regWrite32_dis_if((unsigned int *)0x1faf4300, reg_val);
    #endif
#endif
}

static inline void __chip_init_check_usb0(void)
{
#ifndef TCSUPPORT_CPU_ARMV8
	//TODO: disable USB0
	if(isEN7580ST)
	{
	        RegWrite(0xbfa84700, 0x4410c01);//disable 7580 USB port0 phy
	        RegWrite(0xbfa84704, 0x1);//disable 7580 USB port0 phy
	        RegWrite(0xbfa84708, 0x1);//disable 7580 USB port0 phy
	        RegWrite(0xbfa8470c, 0x1);//disable 7580 USB port0 phy
	        RegWrite(0xbfa84730, 0xf);//disable 7580 USB port0 phy
	        RegWrite(0xbfa84750, 0x4f);//disable 7580 USB port0 phy
	        RegWrite(0xbfb90420, 0x80);//disable 7580 USB3 port0 mac
	        RegWrite(0xbfb90430, 0x80);//disable 7580 USB3 port0 mac
	}
#else
	#if defined(TCSUPPORT_CPU_EN7581)
		__chip_usb0_u3phy_off();
	#elif defined(TCSUPPORT_CPU_AN7552)
		__chip_usb0_u3phy_off();
		__chip_usb0_u2phy_off();
    #else
		__u32 reg_val = 0;
		//disable HSGMII
		reg_val = regRead32_dis_if((unsigned int *)0x1fb00094)|0x20000000;
		regWrite32_dis_if((unsigned int *)0x1fb00094, reg_val);

        regWrite32_dis_if((unsigned int *)0x1fab0420, 0x80);//disable 7523 USB3 port0 mac
        regWrite32_dis_if((unsigned int *)0x1fab0430, 0x80);//disable 7523 USB2 port0 mac
    #endif
#endif
	return;
}

static inline void __chip_init_check_usb1(void)
{
#ifndef TCSUPPORT_CPU_ARMV8
	//TODO: disable USB1
	if(isEN7580ST)
	{
	        RegWrite(0xbfa94700, 0x4410c01);//disable 7580 USB port1 phy
	        RegWrite(0xbfa94704, 0x1);//disable 7580 USB port1 phy
	        RegWrite(0xbfa94708, 0x1);//disable 7580 USB port1 phy
	        RegWrite(0xbfa9470c, 0x1);//disable 7580 USB port1 phy
	        RegWrite(0xbfa94730, 0xf);//disable 7580 USB port1 phy
	        RegWrite(0xbfa94750, 0x4f);//disable 7580 USB port1 phy
	        RegWrite(0xbfba0420, 0x80);//disable 7580 USB3 port1 mac
	        RegWrite(0xbfba0430, 0x80);//disable 7580 USB3 port1 mac
	}
#else
	#if defined(TCSUPPORT_CPU_EN7581)
		__chip_usb1_u3phy_off();
	#elif defined(TCSUPPORT_CPU_AN7552)
		__chip_usb1_u2phy_off();
    #else
        regWrite32_dis_if((unsigned int *)0x1fab0440, 0x80);//disable 7523 USB2 port1 mac
	#endif
#endif
	return;
}

static inline void __chip_init_check_pcie0(void)
{
#ifndef TCSUPPORT_CPU_ARMV8
	//TODO: disable PCIE0
     if(isEN7580ST)
        RegWrite(0xbfaf280c, 0xc0000000);//disable 7580 PCIe port0 phy
	return;
#else
	#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
		__chip_pcie0_phy_off();
	#else
		__u32 reg_val = 0;
		//disable HSGMII
		reg_val = regRead32_dis_if((unsigned int *)0x1fb00094)|0x80000000;
		regWrite32_dis_if((unsigned int *)0x1fb00094, reg_val);
		regWrite32_dis_if((unsigned int *)0x1fa9380c, 0xc0000000);
	#endif
#endif
}

static inline void __chip_init_check_pcie1(void)
{
#ifndef TCSUPPORT_CPU_ARMV8
	//TODO: disable PCIE1
     if(isEN7580ST)
        RegWrite(0xbfac080c, 0xc0000000);//disable 7580 PCIe port1 phy
	return;
#else
	#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
		__chip_pcie1_phy_off();
	#else
		__u32 reg_val = 0;
		//disable HSGMII
		reg_val = regRead32_dis_if((unsigned int *)0x1fb00094)|0x40000000;
		regWrite32_dis_if((unsigned int *)0x1fb00094, reg_val);
		regWrite32_dis_if((unsigned int *)0x1fa9580c, 0xc0000000);
	#endif
#endif
}


#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
void check_serdes_pon_status(uint8_t port_idx, uint8_t serdes_if_num, const char *serdes_port_name)
{
	uint8_t serdes_if = 0;
	serdes_if = get_serdes_interface_sel(port_idx);
	if (serdes_if == serdes_if_num)
	{
		switch(serdes_if)
		{
			case 0:        //ECNT_EVENT_SERDES_PON_PON
				panic("Don't support pon for %s.\n", serdes_port_name);
				break;
			case 1:        //ECNT_EVENT_SERDES_PON_HSGMII
				panic("Don't support HSGMII for %s.\n", serdes_port_name);
				break;
			case 2:        //ECNT_EVENT_SERDES_PON_XFI
				panic("Don't support XFI  for %s.\n", serdes_port_name);
				break;
			case 3:        //ECNT_EVENT_SERDES_PON_USXGMII
				panic("Don't support USXGMII  for %s.\n", serdes_port_name);
				break;
			case 4:        //ECNT_EVENT_SERDES_PON_NONE
				printk("power off for %s.\n", serdes_port_name);
				break;

			default:
				panic("Unkwon interface for %s.\n", serdes_port_name);
				break;
		}
	}
}

void check_serdes_ethernet_status(uint8_t port_idx, uint8_t serdes_if_num, const char *serdes_port_name)
{
	uint8_t serdes_if = 0;
	serdes_if = get_serdes_interface_sel(port_idx);

	if (serdes_if == serdes_if_num)
	{
		switch(serdes_if)
		{
			case 0:        //ECNT_EVENT_SERDES_ETHER_XFI
				panic("Don't support XFI for %s.\n", serdes_port_name);
				break;
			case 1:        //ECNT_EVENT_SERDES_ETHER_USXGMII
				panic("Don't support USXGMII for %s.\n", serdes_port_name);
				break;
			case 2:        //ECNT_EVENT_SERDES_ETHER_HSGMII
				panic("Don't support HSGMII  for %s.\n", serdes_port_name);
				break;
			case 3:        //ECNT_EVENT_SERDES_ETHER_5G_BASER
				panic("Don't support 5G_BASER  for %s.\n", serdes_port_name);
				break;
			case 4:        //ECNT_EVENT_SERDES_ETHER_NONE
				printk("power off for %s.\n", serdes_port_name);
				break;

			default:
				panic("Unkwon interface for %s.\n", serdes_port_name);
				break;
		}
	}
}

void check_serdes_pcie_status(uint8_t port_idx, uint8_t serdes_if_num, const char *serdes_port_name)
{
	uint8_t serdes_if = 0;
	serdes_if = get_serdes_interface_sel(port_idx);

	if (serdes_if == serdes_if_num)
	{
		switch(serdes_if)
		{
			case 0:        //ECNT_EVENT_SERDES_WIFI1_PCIE0_2LANE or  ECNT_EVENT_SERDES_WIFI2_PCIE0_2LANE
				panic("Don't support 2-LANE PCIE for %s.\n", serdes_port_name);
				break;
			case 1:        //ECNT_EVENT_SERDES_WIFI1_PCIE0_1LANE or  ECNT_EVENT_SERDES_WIFI2_PCIE1_1LANE
				panic("Don't support 1-LANE PCIE for %s.\n", serdes_port_name);
				break;
			case 2:        //ECNT_EVENT_SERDES_WIFI1_HSGMII      or  ECNT_EVENT_SERDES_WIFI2_HSGMII
				panic("Don't support HSGMII  for %s.\n", serdes_port_name);
				break;
			case 3:        //ECNT_EVENT_SERDES_WIFI1_USXGMII     or  ECNT_EVENT_SERDES_WIFI2_USXGMII
				panic("Don't support USXGMII  for %s.\n", serdes_port_name);
				break;
			case 4:        //ECNT_EVENT_SERDES_WIFI1_XFI         or  ECNT_EVENT_SERDES_WIFI2_XFI
				panic("Don't support XFI  for %s.\n", serdes_port_name);
				break;
            case 5:        //ECNT_EVENT_SERDES_WIFI1_NONE        or  ECNT_EVENT_SERDES_WIFI2_NONE
				printk("power off for %s.\n", serdes_port_name);
				break;

			default:
				panic("Unkwon interface for %s.\n", serdes_port_name);
				break;
		}
	}
}

void check_serdes_usb_status(uint8_t port_idx, uint8_t serdes_if_num, const char *serdes_port_name)
{
	uint8_t serdes_if = 0;
	serdes_if = get_serdes_interface_sel(port_idx);

	if (serdes_if == serdes_if_num)
	{
		switch(serdes_if)
		{
			case 0:        //ECNT_EVENT_SERDES_USB1_USB30 or  ECNT_EVENT_SERDES_USB2_USB30
				panic("Don't support USB3.0 for %s.\n", serdes_port_name);
				break;
			case 1:
			    if (port_idx == 4)      // SerDes-Port USB1,  ECNT_EVENT_SERDES_USB1_HSGMII
					panic("Don't support HSGMII for %s.\n", serdes_port_name);
				else if (port_idx == 5) // SerDes-Port USB2,  ECNT_EVENT_SERDES_USB2_PCIE2_1LANE
					panic("Don't support 1-LANE PCIE for %s.\n", serdes_port_name);
				break;
			case 2:        //ECNT_EVENT_SERDES_USB1_NONE  or  ECNT_EVENT_SERDES_USB2_NONE
			    printk("power off for %s.\n", serdes_port_name);
				break;

			default:
				panic("Unkwon for %s.\n", serdes_port_name);
				break;
		}
	}
}

void check_serdes_power_status(uint8_t port_idx, const char *serdes_port_name)
{
	int status = 0 ;

	switch(port_idx)
	{
		case 0:   //ECNT_EVENT_SERDES_SEL_PON
			status = (get_serdes_interface_sel(port_idx) != ECNT_EVENT_SERDES_PON_NONE) ? 1 : 0;
			if (status)
				panic("No PON for %s.\n", serdes_port_name);
			break;
		case 1:   //ECNT_EVENT_SERDES_SEL_ETHER
			status = (get_serdes_interface_sel(port_idx) != ECNT_EVENT_SERDES_ETHER_NONE) ? 1 : 0;
			if (status)
				panic("No Ethernet for %s.\n", serdes_port_name);
			break;
		case 2:   //ECNT_EVENT_SERDES_SEL_WIFI1
			status = (get_serdes_interface_sel(port_idx) != ECNT_EVENT_SERDES_WIFI1_NONE) ? 1 : 0;
			if (status)
				panic("No PCIE for %s.\n", serdes_port_name);
			break;
		case 3:   //ECNT_EVENT_SERDES_SEL_WIFI2
			status = (get_serdes_interface_sel(port_idx) != ECNT_EVENT_SERDES_WIFI2_NONE) ? 1 : 0;
			if (status)
				panic("No PCIE for %s.\n", serdes_port_name);
			break;
		case 4:   //ECNT_EVENT_SERDES_SEL_USB1
			status = (get_serdes_interface_sel(port_idx) != ECNT_EVENT_SERDES_USB1_NONE) ? 1 : 0;
			if (status)
				panic("No USB for %s.\n", serdes_port_name);
			break;
		case 5:   //ECNT_EVENT_SERDES_SEL_USB2
			status = (get_serdes_interface_sel(port_idx) != ECNT_EVENT_SERDES_USB2_NONE) ? 1 : 0;
			if (status)
				panic("No USB for %s.\n", serdes_port_name);
			break;

		default:
			panic("Unkwon for %s.\n", serdes_port_name);
			break;
	}
}

void check_all_serdes_status(void)
{
	uint8_t port_idx;
	//uint8_t serdes_if;
	//const char *serdes_if_name;
	const char *serdes_port_name;

	const int port_cnt = get_serdes_port_count();

	for(port_idx = 0; port_idx < port_cnt; port_idx++)
	{
		//serdes_if = get_serdes_interface_sel(port_idx);
		//serdes_if_name = get_serdes_interface_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)port_idx, serdes_if);
		serdes_port_name = get_serdes_port_name((ECNT_EVENT_SYSTEM_SERDES_SEL_t)port_idx);
		//printk("[serdes_check] port_idx=%d, serdes_port_name=%s, serdes_if:%d, serdes_if_name:%s \n", port_idx, serdes_port_name, serdes_if, serdes_if_name);
#if defined(TCSUPPORT_CPU_EN7581)
		if (isAN7566GT)
		{
			if(port_idx==ECNT_EVENT_SERDES_SEL_PON)//Serdes-PON
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_PON, serdes_port_name);           //Don't support PON

		}
		else if (isAN7581PT ||  isAN7581IT || isAN7581FP || isAN7566PT)
		{
			if (isAN7566PT)
			{
				if(port_idx==ECNT_EVENT_SERDES_SEL_PON)//Serdes-PON
					check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_PON, serdes_port_name);        //Don't support PON
			}

			if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI1)//Serdes-PCIE0
			{
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_PCIE0_2LANE, serdes_port_name);//Don't support 2LANE PCIE
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_USXGMII, serdes_port_name);    //Don't support USXGMII
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_XFI, serdes_port_name);        //Don't support XFI
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI2)//Serdes-PCIE1
			{
				check_serdes_pcie_status(port_idx, ECNT_EVENT_SERDES_WIFI2_PCIE0_2LANE, serdes_port_name);//Don't support 2LANE PCIE
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_USB2)//Serdes-USB1
			{
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_USB30, serdes_port_name);//Don't support USB3.0
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_PCIE2_1LANE, serdes_port_name);//Don't support 1LANE PCIE
				
			}
		}
		else if (isAN7581ST || isAN7581FD) // 7581SIT same as 7581ST
		{
			if(isAN7581ST)
			{
				EPHY_API_POWER_DOWN(10);
				EPHY_API_POWER_DOWN(11);
				EPHY_API_POWER_DOWN(12);
			}

            
			if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI1)//Serdes-PCIE0
			{
				check_serdes_pcie_status(port_idx, ECNT_EVENT_SERDES_WIFI1_PCIE0_2LANE, serdes_port_name);//Don't support 2LANE PCIE
				check_serdes_pcie_status(port_idx, ECNT_EVENT_SERDES_WIFI1_PCIE0_1LANE, serdes_port_name);//Don't support 1LANE PCIE
				check_serdes_pcie_status(port_idx, ECNT_EVENT_SERDES_WIFI1_HSGMII, serdes_port_name);//Don't support HSGMII
				check_serdes_pcie_status(port_idx, ECNT_EVENT_SERDES_WIFI1_USXGMII, serdes_port_name);//Don't support USXGMII
				check_serdes_pcie_status(port_idx, ECNT_EVENT_SERDES_WIFI1_XFI, serdes_port_name);//Don't support XFI
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI2)//Serdes-PCIE1
			{
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_PCIE0_2LANE, serdes_port_name);//Don't support 2LANE PCIE
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_PCIE1_1LANE, serdes_port_name);//Don't support 1LANE PCIE
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_USB1)//Serdes-USB0
			{
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB1_HSGMII, serdes_port_name);//Don't support HSGMII
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_USB2)//Serdes-USB1
			{
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_USB30, serdes_port_name);//Don't support USB3.0
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_PCIE2_1LANE, serdes_port_name);//Don't support 1LANE PCIE
			}
		}
		else if (isAN7551PT)
		{
			if(port_idx==ECNT_EVENT_SERDES_SEL_PON)//Serdes-PON
			{
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_XFI, serdes_port_name);           //Don't support XFI
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_USXGMII, serdes_port_name);       //Don't support USXGMII
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_ETHER)//Serdes-Ethernet
			{
				check_serdes_ethernet_status(port_idx,  ECNT_EVENT_SERDES_ETHER_XFI, serdes_port_name);    //Don't support XFI
				check_serdes_ethernet_status(port_idx,  ECNT_EVENT_SERDES_ETHER_USXGMII, serdes_port_name);//Don't support USXGMII
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI1)//Serdes-PCIE0
			{
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_PCIE0_2LANE, serdes_port_name);//Don't support 2LANE PCIE
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_USXGMII, serdes_port_name);    //Don't support USXGMII
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_XFI, serdes_port_name);        //Don't support XFI
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI2)//Serdes-PCIE1
			{
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_PCIE0_2LANE, serdes_port_name);//Don't support 2LANE PCIE
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_USXGMII, serdes_port_name);    //Don't support USXGMII
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_XFI, serdes_port_name);        //Don't support XFI
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_USB2)//Serdes-USB1
			{
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_USB30, serdes_port_name);//Don't support USB3.0
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_PCIE2_1LANE, serdes_port_name);//Don't support 1LANE PCIE
			}
		}
		else if (isAN7581CT)
		{
			__chip_emmc_phy_off();

			if(port_idx==ECNT_EVENT_SERDES_SEL_PON)//Serdes-PON
			{
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_XFI, serdes_port_name);           //Don't support XFI
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_USXGMII, serdes_port_name);       //Don't support USXGMII
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_HSGMII, serdes_port_name);        //Don't support HSGMII
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI1)//Serdes-PCIE0
			{
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_PCIE0_2LANE, serdes_port_name);//Don't support 2LANE PCIE
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_HSGMII, serdes_port_name);     //Don't support HSGMII
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_USXGMII, serdes_port_name);    //Don't support USXGMII
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_XFI, serdes_port_name);        //Don't support XFI
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI2)//Serdes-PCIE1
			{
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_PCIE0_2LANE, serdes_port_name);//Don't support 2LANE PCIE
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_HSGMII, serdes_port_name);     //Don't support HSGMII
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_USXGMII, serdes_port_name);    //Don't support USXGMII
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_XFI, serdes_port_name);        //Don't support XFI
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_USB2)//Serdes-USB1
			{
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_USB30, serdes_port_name);//Don't support USB3.0
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_PCIE2_1LANE, serdes_port_name);//Don't support 1LANE PCIE
			}

		}
		else if (isAN7581DT)
		{
			__chip_emmc_phy_off();

			if(port_idx==ECNT_EVENT_SERDES_SEL_PON)//Serdes-PON
			{
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_XFI, serdes_port_name);           //Don't support XFI
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_USXGMII, serdes_port_name);       //Don't support USXGMII
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_HSGMII, serdes_port_name);        //Don't support HSGMII
			}
            
			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI1)//Serdes-PCIE0
				check_serdes_power_status(port_idx, serdes_port_name); //No PCIE

			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI2)//Serdes-PCIE1
				check_serdes_power_status(port_idx, serdes_port_name); //No PCIE
            

			else if(port_idx==ECNT_EVENT_SERDES_SEL_USB2)//Serdes-USB1
			{
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_USB30, serdes_port_name);//Don't support USB3.0
				check_serdes_usb_status(port_idx, ECNT_EVENT_SERDES_USB2_PCIE2_1LANE, serdes_port_name);//Don't support 1LANE PCIE
			}
		}
		else if(isAN7551GT)
		{
			if(port_idx==ECNT_EVENT_SERDES_SEL_PON)//Serdes-PON
			{
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_XFI, serdes_port_name);           //Don't support XFI
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_USXGMII, serdes_port_name);       //Don't support USXGMII
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_ETHER)//Serdes-Ethernet
			{
				check_serdes_ethernet_status(port_idx,  ECNT_EVENT_SERDES_ETHER_XFI, serdes_port_name);    //Don't support XFI
				check_serdes_ethernet_status(port_idx,  ECNT_EVENT_SERDES_ETHER_USXGMII, serdes_port_name);//Don't support USXGMII
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI1)//Serdes-PCIE0
			{
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_USXGMII, serdes_port_name);    //Don't support USXGMII
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI1_XFI, serdes_port_name);        //Don't support XFI
			}
			else if(port_idx==ECNT_EVENT_SERDES_SEL_WIFI2)//Serdes-PCIE1
			{
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_USXGMII, serdes_port_name);    //Don't support USXGMII
				check_serdes_pcie_status(port_idx,  ECNT_EVENT_SERDES_WIFI2_XFI, serdes_port_name);        //Don't support XFI
			}
		}

#endif
#if defined(TCSUPPORT_CPU_AN7552)
		if(isAN7563CT || isAN7563PT)
		{
			if(port_idx==ECNT_EVENT_SERDES_SEL_PON)//Serdes-PON
			{
				check_serdes_pon_status(port_idx,  ECNT_EVENT_SERDES_PON_PON, serdes_port_name);           //Don't support PON
			}
		}
#endif
	}
}

static inline void __chip_init_check_serdes(void)
{
	check_all_serdes_status();
}
#endif


// if chipid is known, return 1, else return 0
static inline int __chipid_in_white_list(void)
{
	int i = 0;

	for(i = 0; get_pkg_info(i, INTERFACE_PKG_ID) != END_PACKAGE_ID; i++) {
		if(get_pkg_info(i, INTERFACE_PKG_ID)) {
			package_id_idx = i;
			return 1;
		}
	}
	
	return 0;
}

/*
 * This is the public API intended to be called by closed-srouce module_init function
 * */
static inline void ecnt_chip_interface_init(void)
{
#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
	if(!__chipid_in_white_list() )
		panic("unkonw chipid, module load fail!\n");

	if(!__chip_has_modules(__ECNT_CHIP_INIT_XPON_ACTION) )
		__ecnt_init_hook[__ECNT_CHIP_INIT_XPON_ACTION] = __chip_init_check_xpon;
		
	if(!__chip_has_modules(__ECNT_CHIP_INIT_USB0_ACTION) )
		__ecnt_init_hook[__ECNT_CHIP_INIT_USB0_ACTION] = __chip_init_check_usb0;

	if(!__chip_has_modules(__ECNT_CHIP_INIT_USB1_ACTION) )
		__ecnt_init_hook[__ECNT_CHIP_INIT_USB1_ACTION] = __chip_init_check_usb1;

	if(!__chip_has_modules(__ECNT_CHIP_INIT_PCIE0_ACTION) )
		__ecnt_init_hook[__ECNT_CHIP_INIT_PCIE0_ACTION] = __chip_init_check_pcie0;

	if(!__chip_has_modules(__ECNT_CHIP_INIT_PCIE1_ACTION) )
		__ecnt_init_hook[__ECNT_CHIP_INIT_PCIE1_ACTION] = __chip_init_check_pcie1;

	#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
	__ecnt_init_hook[__ECNT_CHIP_INIT_SERDES_ACTION] = __chip_init_check_serdes;
	#endif

	dis_if_task = kthread_run(__ecnt_check_interface_all, NULL, "dis_if");
#endif
	return;
}
/*
 * This is the public API intended to be called by closed-srouce module_exit function
 * */
static inline void ecnt_chip_interface_deinit(void)
{
	kthread_stop(dis_if_task);
	return;
}


