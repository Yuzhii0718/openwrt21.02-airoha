#include "xhci-mtk.h"
#include "xhci-mtk-power.h"
#include "xhci-mtk-scheduler.h"
#include "mtk-phy.h"
//#include "ssusb-phy.h"
#include <linux/kernel.h>       /* printk() */
#include <linux/slab.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/usb/ch9.h>
#include <asm/tc3162/tc3162.h>


static const char hcd_name[] = "xhci-hcd";

int get_xhci_u3_port_num(struct device *dev){
	struct mtk_u3h_hw *u3h_hw;	
#ifdef TCSUPPORT_CPU_ARMV8_64
	void __iomem *addr;	
	u64 data;
	int u3_port_num;

	u3h_hw = dev->platform_data;
	addr = (void __iomem *)(u3h_hw->ippc_virtual_base + U3H_SSUSB_IP_XHCI_CAP);
#else
	__u32 __iomem *addr;	
	u32 data;
	int u3_port_num;

	u3h_hw = dev->platform_data;

	addr = (u32 __iomem *)(u3h_hw->ippc_virtual_base + U3H_SSUSB_IP_XHCI_CAP);
#endif
	data = readl(addr);
	u3_port_num = data & SSUSB_IP_XHCI_U3_PORT_NO;

	return u3_port_num;
}

int get_xhci_u2_port_num(struct device *dev){
	struct mtk_u3h_hw *u3h_hw;	
	__u32 __iomem *addr;	
	u32 data;
	int u2_port_num;
	
	u3h_hw = dev->platform_data;

	addr = (u32 __iomem *)(u3h_hw->ippc_virtual_base + U3H_SSUSB_IP_XHCI_CAP);
	data = readl(addr);
	
	u2_port_num = (data & SSUSB_IP_XHCI_U2_PORT_NO) >> 8;
	
	return u2_port_num;
}

#if 1

#define FRAME_LEVEL2_CNT	25
#define LS_EOF_CYCLE_CNT       (1216 * FRAME_CNT_CK_VAL / 10)
#define FS_EOF_CYCLE_CNT       (635  * FRAME_CNT_CK_VAL / 10)
#define HS_SYNC_EOF_CYCLE_CNT  (250  * FRAME_CNT_CK_VAL / 10)
#define HS_ASYNC_EOF_CYCLE_CNT (125  * FRAME_CNT_CK_VAL / 10)
#define SS_EOF_CYCLE_CNT       (25   * FRAME_CNT_CK_VAL / 10)

void frame_cnt_config(struct device *dev)
{	
	struct mtk_u3h_hw *u3h_hw;
	void __iomem *xhci_reg;	
	u32 integer;
	u32 decimal;
	u32 itp_delta_clk_ratio;
	int i;
	u32 uframe_cycle_cnt;
	u32 bank_sycle_cnt;
	u32 ls_eof_bank;
	u32 fs_eof_bank;
	u32 hs_sync_eof_bank;
	u32 hs_async_eof_bank;
	u32 ss_eof_bank;	
	u32 ls_eof_offset;
	u32 fs_eof_offset;
	u32 hs_sync_eof_offset;
	u32 hs_async_eof_offset;
	u32 ss_eof_offset;

//	dev_dbg(dev, "%s, cur frame_ck = %dMHz\n", __func__, frame_ref_clk[FRAME_CNT_CK_VAL]);
    u3h_hw = dev->platform_data;
	xhci_reg = (void __iomem *) u3h_hw->u3h_virtual_base;
	
	uframe_cycle_cnt = 125 * FRAME_CNT_CK_VAL;
	bank_sycle_cnt = uframe_cycle_cnt / FRAME_LEVEL2_CNT;
	u3h_writelmsk(xhci_reg + HFCNTR_CFG, (bank_sycle_cnt - 1) << 8, 
		INIT_FRMCNT_LEV1_FULL_RANGE);	
#if defined(TCSUPPORT_CPU_EN7528) || defined(TCSUPPORT_CPU_EN7523)
		u3h_writelmsk(xhci_reg + HFCNTR_CFG, ((FRAME_LEVEL2_CNT & 0x10 )>>4) << 7, 
			INIT_FRMCNT_LEV2_FULL_RANGE_HI);
		u3h_writelmsk(xhci_reg + HFCNTR_CFG, ((FRAME_LEVEL2_CNT - 1 )&0xf) << 20, 
			INIT_FRMCNT_LEV2_FULL_RANGE_LO);
#else
	u3h_writelmsk(xhci_reg + HFCNTR_CFG, (FRAME_LEVEL2_CNT - 1) << 17, 
		INIT_FRMCNT_LEV2_FULL_RANGE);
#endif

    ls_eof_bank = LS_EOF_CYCLE_CNT / bank_sycle_cnt;
	ls_eof_offset = LS_EOF_CYCLE_CNT % bank_sycle_cnt;
	u3h_writelmsk(xhci_reg + LS_EOF, ls_eof_offset, LS_EOF_OFFSET);
	u3h_writelmsk(xhci_reg + LS_EOF, ls_eof_bank << 16, LS_EOF_BANK);

    fs_eof_bank = FS_EOF_CYCLE_CNT / bank_sycle_cnt;
    fs_eof_offset = FS_EOF_CYCLE_CNT % bank_sycle_cnt;
	u3h_writelmsk(xhci_reg + FS_EOF, fs_eof_offset, FS_EOF_OFFSET);
	u3h_writelmsk(xhci_reg + FS_EOF, fs_eof_bank << 16, FS_EOF_BANK);
	
    hs_sync_eof_bank = HS_SYNC_EOF_CYCLE_CNT / bank_sycle_cnt;
	hs_sync_eof_offset = HS_SYNC_EOF_CYCLE_CNT % bank_sycle_cnt;
	u3h_writelmsk(xhci_reg + SYNC_HS_EOF, hs_sync_eof_offset, SYNC_HS_EOF_OFFSET);
	u3h_writelmsk(xhci_reg + SYNC_HS_EOF, hs_sync_eof_bank << 16, SYNC_HS_EOF_BANK);
	
    hs_async_eof_bank = HS_ASYNC_EOF_CYCLE_CNT / bank_sycle_cnt;
	hs_async_eof_offset = HS_ASYNC_EOF_CYCLE_CNT % bank_sycle_cnt;
	u3h_writelmsk(xhci_reg + ASYNC_HS_EOF, hs_async_eof_offset, ASYNC_HS_EOF_OFFSET);
	u3h_writelmsk(xhci_reg + ASYNC_HS_EOF, hs_async_eof_bank << 16, ASYNC_HS_EOF_BANK);
	
    ss_eof_bank = SS_EOF_CYCLE_CNT / bank_sycle_cnt;
	ss_eof_offset = SS_EOF_CYCLE_CNT % bank_sycle_cnt;
	u3h_writelmsk(xhci_reg + SS_EOF, ss_eof_offset, SS_EOF_OFFSET);
	u3h_writelmsk(xhci_reg + SS_EOF, ss_eof_bank << 16, SS_EOF_BANK);

	
	integer = 60 / FRAME_CNT_CK_VAL;
	itp_delta_clk_ratio = integer << 3;

#if 0
	/*useless code because decimal always 0*/
	for (i = 2; i >= 0; i--) {
		decimal = (60 * 1000) %  FRAME_CNT_CK_VAL;
		if ((decimal * 2) > 1000)
			itp_delta_clk_ratio |= (1 << i);
	}
#endif
	/* set ITP delta ratio */
	u3h_writelmsk(xhci_reg + HFCNTR_CFG, itp_delta_clk_ratio << 1, ITP_DELTA_CLK_RATIO);
	
	
}

int mu3h_phy_init(struct device *dev)
{
	if(!isFPGA)	
		ecnt_u3h_phy_init();
	return 0;
}

#else
/* 
 * XXX have suggested that IC designers shall set the default value due to ASIC clock
 * i.e., ASIC can use the default value
 */
void set_frame_cnt_ck(struct device *dev) { }
int mu3h_phy_init(struct device *dev){ }

#endif
void setInitialReg_7552(struct device *dev){

	struct mtk_u3h_hw *u3h_hw;
	void __iomem *xhci_reg;	

	printk("AN7552 Initial Setting\n");	
	u3h_hw = dev->platform_data;
	xhci_reg = (void __iomem *) u3h_hw->u3h_virtual_base;

	u3h_writelmsk(xhci_reg + LS_EOF		, 0x07130044, 0xffffffff);
	u3h_writelmsk(xhci_reg + FS_EOF		, 0x070a0017, 0xffffffff);
	u3h_writelmsk(xhci_reg + SS_EOF		, 0x0000003c, 0xffffffff);
	u3h_writelmsk(xhci_reg + HFCNTR_CFG	, 0x003095a8, 0xffffffff); 
	u3h_writelmsk(xhci_reg + 0x990		, 0x0000001e, 0xffffffff); 
}


void setInitialReg(struct device *dev)
{
	int ck_factor = 0;
	struct mtk_u3h_hw *u3h_hw;
	void __iomem *usb3_csr_base;
	void __iomem *usb2_csr_base;
	
	__u32 __iomem *addr;	
	u32 data;

    int u3_port_num,u2_port_num;
#ifdef SSM_ENABLE
	ck_factor = 1;
#endif
    u3h_hw = dev->platform_data;
    usb3_csr_base = (u3h_hw->u3h_virtual_base + SSUSB_USB3_CSR_OFFSET);
	usb2_csr_base = (u3h_hw->u3h_virtual_base + SSUSB_USB2_CSR_OFFSET);

	//get u3 & u2 port num
	u3_port_num = get_xhci_u3_port_num(dev);
	u2_port_num = get_xhci_u2_port_num(dev);

	if(u3_port_num){
    	//set MAC reference clock speed
    	addr = usb3_csr_base + U3H_UX_EXIT_LFPS_TIMING_PARAMETER;
    	data = ((300*U3_REF_CK_VAL + (1000-1)) / 1000)<< ck_factor;
    	u3h_writelmsk(addr, (data<<RX_UX_EXIT_LFPS_REF_OFST), RX_UX_EXIT_LFPS_REF);
    	
    	addr = usb3_csr_base + U3H_REF_CK_PARAMETER;
    	data = U3_REF_CK_VAL << ck_factor;
    	u3h_writelmsk(addr,data,REF_1000NS);
    
    	//set SYS_CK
    	addr = usb3_csr_base + U3H_TIMING_PULSE_CTRL;
    	data = U3_SYS_CK_VAL << ck_factor;
    	u3h_writelmsk(addr,data,CNT_1US_VALUE);

	}

	addr = usb2_csr_base + U3H_USB20_TIMING_PARAMETER;
	data = U3_SYS_CK_VAL;
	u3h_writelmsk(addr,data,TIME_VALUE_1US);
	frame_cnt_config(dev);
#ifdef SSM_ENABLE
	addr = u3h_hw->u3h_virtual_base + SS_EOF;
	data = readl(addr) & SS_EOF_OFFSET;
	data = data << ck_factor;
	u3h_writelmsk(addr, data, SS_EOF_OFFSET);
#endif
   /* disable xhci reset MAC & PHYD*/
	addr = u3h_hw->u3h_virtual_base + RST_CTRL1;
	data = 0;
	u3h_writelmsk(addr, data, 0xffffffff);
}

void reinitIP(struct device *dev)
{
	mu3h_phy_init(dev);
	enableAllClockPower(dev);
	if(isFPGA)
		setInitialReg(dev);
	else {
		if(isAN7552)
			setInitialReg_7552(dev);
	}
	mtk_xhci_scheduler_init(dev);

}

void  mtk_xhci_setup(struct xhci_hcd *xhci){
	struct usb_hcd *hcd;
	struct usb_bus *bus;
	struct device *dev;
	struct mtk_u3h_hw *u3h_hw;
        __u32 __iomem *addr;
	int i;

	hcd = xhci_to_hcd(xhci);
	if(hcd == NULL)
		return;
	bus = hcd_to_bus(hcd);
	if(bus == NULL)
		return;
	dev = bus->controller;
	if(dev == NULL)
		return;
	u3h_hw = dev->platform_data;
	if(u3h_hw == NULL)
		return;

	if(isEN7528 || isEN7523 || isAN7552){
		for(i = 0 ; i < 100; i++){ //wait phy clk ready
#ifdef TCSUPPORT_CPU_ARMV8
            if((readl(u3h_hw->ippc_virtual_base + 0x10) & (1<<16)) == 0)
#else	
			if((readl(0xbfb93e10) & (1<<16)) == 0)
#endif				
				mdelay(1);
			else
				break;
		}

		addr = u3h_hw->u3h_virtual_base + SSUSB_USB3_CSR_OFFSET + U3H_LTSSM_TIMING_PARAMETER_5;
		writel(0x203E8, addr); /*TD 6.5 fail Issue*/
	}else{
		addr = u3h_hw->u3h_virtual_base + SSUSB_USB3_CSR_OFFSET + U3H_LTSSM_TIMING_PARAMETER_3;
        	writel(0x3E8012C, addr); /*change 360 ms to 1s */
	}

}
/* return code */
#define RET_SUCCESS 0
#define RET_FAIL -1

#ifdef ECNT_DISABLED
/* automatilcally check frame cnt value in case that SA forgets to confirm */
int chk_frmcnt_clk(struct usb_hcd *hcd)
{
	static bool frm_ck_checked = false;
	struct xhci_hcd *xhci;
	int frame_id, frame_id_new, delta, ret;

	ret = RET_SUCCESS;
	if (frm_ck_checked)
		return ret;
	frm_ck_checked = true;
	xhci = hcd_to_xhci(hcd);
	frame_id = xhci_readl(xhci, &xhci->run_regs->microframe_index);
	msleep(1000);
	frame_id_new = xhci_readl(xhci, &xhci->run_regs->microframe_index);
	if (frame_id_new < frame_id)
		frame_id_new += 1<<14;
	delta = (frame_id_new - frame_id) >> 3;
	xhci_err(xhci, "frame timing delta = %d\n",
		delta);
	if (abs(delta-1000) > 10){
		ret = RET_FAIL;
		xhci_err(xhci, "\n\n+++++++++\nERROR! maybe frame cnt clock is wrong!\n");
		xhci_err(xhci, "start=%d, end=%d\n+++++++++\n\n", frame_id, frame_id_new);
	}
	return ret;
}
#endif

#if CFG_DEV_U3H0
static struct resource mtk_resource_u3h0[] = {
	[0] = {
		    .start = U3H_IRQ0,
		    .end   = U3H_IRQ0,
		    .flags = IORESOURCE_IRQ,
	},
	[1] = {
            .name = "u3h",
			 /*physical address*/
		    .start = U3H_BASE0,
		    .end   = U3H_BASE0 + MTK_U3H_SIZE - 1,
		    .flags = IORESOURCE_MEM,
	},
	[2] = {
            .name = "ippc",
			 /*physical address*/
		    .start = IPPC_BASE0,
		    .end   = IPPC_BASE0 + MTK_IPPC_SIZE - 1,
		    .flags = IORESOURCE_MEM,
	},
	
};
#endif

#if CFG_DEV_U3H1
static struct resource mtk_resource_u3h1[] = {
	[0] = {
		    .start = U3H_IRQ1,
		    .end   = U3H_IRQ1,
		    .flags = IORESOURCE_IRQ,
	},
	[1] = {
            .name = "u3h",
			 /*physical address*/
		    .start = U3H_BASE1,
		    .end   = U3H_BASE1 + MTK_U3H_SIZE - 1,
		    .flags = IORESOURCE_MEM,
	},
	[2] = {
            .name = "ippc",
			 /*physical address*/
		    .start = IPPC_BASE1,
		    .end   = IPPC_BASE1 + MTK_IPPC_SIZE - 1,
		    .flags = IORESOURCE_MEM,
	},
	
};
#endif

#if CFG_DEV_U3H0
struct mtk_u3h_hw u3h_hw0;
#endif

#if CFG_DEV_U3H1 || defined(TCSUPPORT_CPU_EN7581) 
/* 7528/7523/7581 doesn't define CFG_DEV_U3H1, but 7581 has both U3H0 and U3H1 */
struct mtk_u3h_hw u3h_hw1;
#endif

static u64 mtk_u3h_dma_mask = 0xffffffffUL;

static struct platform_device mtk_device_u3h[] = {
#if CFG_DEV_U3H0
	{
	    .name          = hcd_name,
	    .id            = 0,
	    .resource      = mtk_resource_u3h0,
	    .num_resources = ARRAY_SIZE(mtk_resource_u3h0),
	    .dev           = { 
                            .platform_data = &u3h_hw0,
						    .dma_mask = &mtk_u3h_dma_mask,					
				            .coherent_dma_mask = 0xffffffffUL,
                         },
     },
#endif
#if CFG_DEV_U3H1
	{
	    .name          = hcd_name,
	    .id            = 1,
	    .resource      = mtk_resource_u3h1,
	    .num_resources = ARRAY_SIZE(mtk_resource_u3h1),
	    .dev           = { 
                            .platform_data = &u3h_hw1,
						    .dma_mask = &mtk_u3h_dma_mask,					
				            .coherent_dma_mask = 0xffffffffUL,
                         },
     },
#endif

};
	

static 	int __init mtk_u3h_init(void)
{
    int ret;
	int i;
	int u3h_dev_num;

#ifdef TCSUPPORT_CPU_ARMV8
	return 0;
#endif
	u3h_dev_num = sizeof(mtk_device_u3h) / sizeof(mtk_device_u3h[0]);
    for (i = 0; i < u3h_dev_num; i++){
        ret = platform_device_register(&mtk_device_u3h[i]); 
        if (ret != 0){
            return ret;
        }
    }

	return ret;
}

static void __exit mtk_u3h_cleanup(void)
{
	int u3h_dev_num;
	int i;

	u3h_dev_num = sizeof(mtk_device_u3h) / sizeof(mtk_device_u3h[0]);
    for (i = 0; i < u3h_dev_num; i++){
       platform_device_unregister(&mtk_device_u3h[i]); 
    }
}

module_init(mtk_u3h_init);
module_exit(mtk_u3h_cleanup);
MODULE_LICENSE("GPL");
