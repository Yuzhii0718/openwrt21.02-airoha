#ifndef _XHCI_MTK_H
#define _XHCI_MTK_H

#include <linux/version.h>
#include <linux/usb.h>
#include "../mu3h_drv/xhci.h"
#include "xhci-mtk-scheduler.h"
#include "ssusb_sifslv_ippc.h"
#include "ssusb_usb3_mac_csr.h"
#include "ssusb_usb3_sys_csr.h"
#include "ssusb_usb2_csr.h"
#include "ssusb_xHCI_exclude_port_csr.h"
#ifndef TCSUPPORT_CPU_ARMV8
#include <asm/tc3162/tc3182_int_source.h>
#endif
/* U3H IP CONFIG: 
 * enable this according to the U3H IP num of the project
 */
#ifdef SSM_ENABLE
#define DR_FPGA		1
#define MT7662T		2
#define MT6632		3
#define MT8581		4
#define MT8173		5
#define Wukong		6
/* let ssm_project default value = 0 so that it can generate build error explicitly */
#define SSM_PROJECT DR_FPGA
#if (SSM_PROJECT == 0)
#error "SA must specify project name when ssm enabled!"
#endif
#endif
#define  CFG_DEV_U3H0   1  //if the project has one or more U3H IP, enable this
#if defined(TCSUPPORT_CPU_EN7528) || defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_AN7552)
#define  CFG_DEV_U3H1   0  //if the project has two or more U3H IP, enable this
#else
#define  CFG_DEV_U3H1   1  //if the project has two or more U3H IP, enable this
#endif

#define FPGA_MODE       1   //if run in FPGA,enable this
#define OTG_SUPPORT     0   //if OTG support,enable this


/* U3H irq number*/
#if CFG_DEV_U3H0
    #ifdef TCSUPPORT_MIPS_1004K
    #define U3H_IRQ0 IRQ_RT3XXX_USB
    #else
    #define U3H_IRQ0 18
    #endif
#endif
#if CFG_DEV_U3H1
    #ifdef TCSUPPORT_MIPS_1004K
    #define U3H_IRQ1 USB_HOST_2
    #else
    #define U3H_IRQ1 49
    #endif
#endif


/*U3H register bank*/
#if CFG_DEV_U3H0
    //physical base address for U3H IP0
    //#define U3H_BASE0	    0xf0040000
    #define U3H_BASE0	    0x1FB90000
#ifdef TCSUPPORT_CPU_EN7528
	#define IPPC_BASE0      0x1FB93E00
#else
	#define IPPC_BASE0      0x1FA84700
#endif
#endif
#if CFG_DEV_U3H1
    //physical base address for U3H IP1
    //#define U3H_BASE1	    0xf0040000
    #define U3H_BASE1	    0x1FBA0000
    //#define IPPC_BASE1      0xf0044700
	#define IPPC_BASE1      0x1FA94700
#endif


/* Clock source */
//Clock source may differ from project to project. Please check integrator
#ifdef TCSUPPORT_CPU_EN7581
#define	U3_REF_CK_VAL	25			//MHz = value
#else
#define	U3_REF_CK_VAL	20			//MHz = value
#endif
#define	U3_SYS_CK_VAL	100			//MHz = value
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define FRAME_CNT_CK_VAL	60		//60 used on VU19p, 24 used on VU440
#else
#ifdef TCSUPPORT_CPU_EN7523
#define FRAME_CNT_CK_VAL	20
#else
#define FRAME_CNT_CK_VAL	60
#endif
#endif

#if FPGA_MODE
    /*Defined for PHY init in FPGA MODE*/
    //change this value according to U3 PHY calibration
    /* for ssm, mdt0055-012, 0x10 is recommeded (0x8 with CATC)*/
    #define U3_PHY_PIPE_PHASE_TIME_DELAY	0x8 
#endif


//offset may differ from project to project. Please check integrator
#define SSUSB_USB3_CSR_OFFSET 0x00002400
#define SSUSB_USB2_CSR_OFFSET 0x00003400

#ifdef TCSUPPORT_CPU_EN7528
#define MTK_U3H_SIZE	0x3e00
#else
#define MTK_U3H_SIZE	0x4000
#endif
#define MTK_IPPC_SIZE	0x100
#define MAX_PORT_NUM        7
#define MAX_EP_NUM			64



/*=========================================================================================*/


#define u3h_writelmsk(addr, data, msk) \
	{ writel(((readl(addr) & ~(msk)) | ((data) & (msk))), addr); \
	}

#define u3h_clrmsk(addr, msk) writel(readl(addr) & ~(msk))
#define u3h_setmsk(addr, msk) writel(readl(addr) | msk)

struct mtk_u3h_hw {
//  char u3_port_num;
//	char u2_port_num;
#ifdef TCSUPPORT_CPU_ARMV8
	void __iomem *u3h_virtual_base;
	void __iomem *ippc_virtual_base;
#else
    void  *u3h_virtual_base;
	void  *ippc_virtual_base;
#endif
	struct sch_port u3h_sch_port[MAX_PORT_NUM];
};
	
extern struct mtk_u3h_hw u3h_hw;	

void reinitIP(struct device *dev);
void setInitialReg(struct device *dev);
void dbg_prb_out(void);
int u3h_phy_init(void);
int get_xhci_u3_port_num(struct device *dev);
int get_xhci_u2_port_num(struct device *dev);
int chk_frmcnt_clk(struct usb_hcd *hcd);
void  mtk_xhci_setup(struct xhci_hcd *xhci);


#if 0
/*
  mediatek probe out
*/
/************************************************************************************/

#define SW_PRB_OUT_ADDR	(SIFSLV_IPPC+0xc0)		//0xf00447c0
#define PRB_MODULE_SEL_ADDR	(SIFSLV_IPPC+0xbc)	//0xf00447bc

static inline void mtk_probe_init(const u32 byte){
	__u32 __iomem *ptr = (__u32 __iomem *) PRB_MODULE_SEL_ADDR;
	writel(byte, ptr);
}

static inline void mtk_probe_out(const u32 value){
	__u32 __iomem *ptr = (__u32 __iomem *) SW_PRB_OUT_ADDR;
	writel(value, ptr);
}

static inline u32 mtk_probe_value(void){
	__u32 __iomem *ptr = (__u32 __iomem *) SW_PRB_OUT_ADDR;

	return readl(ptr);
}
#endif

#endif
