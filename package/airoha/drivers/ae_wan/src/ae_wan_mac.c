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
************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
//#include <asm/spram.h>
#include <asm/io.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#include <linux/skbuff.h>
#include <linux/mtd/rt_flash.h>
#include <linux/cdev.h>


#include "ae_wan_proc.h"
#include "ae_wan_api.h"
#include "ae_wan_mac.h"
#include "ae_wan_ic_dis.h"

#include <ecnt_hook/ecnt_hook_qdma.h>
#include <ecnt_hook/ecnt_hook_l2tp.h>
#include <ecnt_hook/ecnt_hook_fe.h>
#include <ecnt_hook/ecnt_hook_ppe.h>
#include <ecnt_hook/ecnt_hook_multicast.h>
#include <ecnt_hook/ecnt_hook_multicast_general.h>
#include <ecnt_hook/ecnt_hook_ae_mac_type.h>
#include <ecnt_hook/ecnt_hook_ae_mac.h>
#include <ecnt_hook/ecnt_hook_ether.h>

#include <linux/libcompileoption.h>

#if defined(TCSUPPORT_CPU_EN7523)
#include "ecnt_event_global/ecnt_event_system.h"
#endif

#if defined(TCSUPPORT_ETHER_8811)
#include <ecnt_hook/ecnt_hook_air_en8811.h>
#endif

#if defined(TCSUPPORT_ETHER_AS21XX)
#include <ecnt_hook/ecnt_hook_as21xx.h>
#endif


/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/
extern char *hsgmii_pon_itf;

#if SUPPORT_AS21xx_WA
extern u8 xpon_linkup_wrapper_wa_flag;
extern u16 xpon_linkup_wrapper_delay_time;
#endif

char *ae_wan_itf  = "ae_wan"; //tmp setting for porting
#if defined(CONFIG_TP_IMAGE)
char *hsgmii_pon_itf_origin  = NULL;
#endif
char *aewan_lan_itf  = "eth0.10"; 
struct net_device *ae_wan_dev;
xsiWanAdapter_t *ae_wan_p = NULL;
uint8 xsiMacInitialized = 0; 
uint8 def_ae_wan_mac_addr[] = {0x00, 0xaa, 0xbb, 0x01, 0x23, 0x45};
static struct timer_list xsi_mac_timer;
int xsiPortState = DOWN;
#if defined (TCSUPPORT_SISM_HOST)
int inicState = BOOTING_MODE;
#endif
#if defined(TCSUPPORT_CPU_EN7581)||defined(TCSUPPORT_CPU_AN7583)
unsigned int WAN_LINK_MODE = USXGII_10G;
#else
unsigned int WAN_LINK_MODE = SGMII_SPEED_2500M;
#endif

unsigned int wan_sel = ENUM_UNKNOW_SERDES_ASWAN; //0: pon serdrs as 2.5G wan; 1: usb as 2.5G WAN; 2:pcie0 as 2.5G wan; 3:pcie1 as 2.5G WAN 4:for 7581 eth as wan
unsigned int XSI_IDX_SEL = XSI_AE_IDX;
ECNT_EVENT_SYSTEM_SERDES_SEL_t XSI_IDX_CHECK;
unsigned int XSI_RATE_IDX = ENUM_UNKNOW_SERDES;

#if SUPPORT_AE_WAN_NEW_ARCH
unsigned int HSGMII_BASE_REG[5] = {XSI_PCIE0_BASE,XSI_PCIE1_BASE, XSI_USB_BASE,XSI_AE_BASE,XSI_ETH_BASE};
#else
unsigned int HSGMII_BASE_REG[4] = {XSI_PCIE0_BASE,XSI_PCIE1_BASE, XSI_USB_BASE,XSI_AE_BASE};
#endif

#define AE_WAN_CHANNEL_NUM         0

#if defined(TCSUPPORT_QOS_EIGHT_QUEUE)
#define MAX_PRIORITY_QUEUE_NUM      8
#else
#define MAX_PRIORITY_QUEUE_NUM      4
#endif

#if defined(TCSUPPORT_SISM_HOST)
unsigned int enAeWanInicFilter = 0;
#endif

#if defined(CONFIG_TP_IMAGE)
int is_ae_wan_en8811phy = 1;
#endif

int xsi_speed = 0;
/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A   T Y P E S
************************************************************************/
static int max_pkt_len = 2000;
static DEFINE_SPINLOCK(ae_wan_napilock);

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
************************************************************************/
extern u8 xsi_dbg_level;

extern int ae_wan_fast_mode_flag;

#ifdef WAN2LAN
extern int masko_on_off;
extern int masko;
extern int wlan_masko_on_off;
extern int wlan_masko;

#endif
#define TX_STAG_LEN 6

#ifdef CONFIG_TP_IMAGE
  /* fix bug 856251, wan mark is conflic with 8021.p remark, cause pbit incorrect and packet is dropped by olt */
  #ifdef CONFIG_8021P_REMARK
  #undef CONFIG_8021P_REMARK
  #endif
#else
#define CONFIG_8021P_REMARK 1
#endif /* CONFIG_TP_IMAGE */

#if defined(CONFIG_8021P_REMARK)
#define QOS_8021p_MARK			0x0F00 	/* 8~11 bits used for 802.1p */
#define QOS_8021P_0_MARK		0x08	/* default mark is zero */
#define VLAN_HLEN				4
#define VLAN_ETH_ALEN			6
#endif

#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_MULTICAST_VLAN)
extern int (*ae_wan_multicastVlanRxHook)(struct sk_buff *skb);
extern int (*ae_wan_multicastVlanTxHook)(struct sk_buff *skb);
#endif /* CONFIG_TP_IMAGE && INCLUDE_MULTICAST_VLAN */


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/
static int ae_wan_mac_start(struct net_device *dev);
static int ae_wan_mac_open(struct net_device *dev);
static int ae_wan_mac_close(struct net_device *dev);
static struct net_device_stats *ae_wan_mac_stats(struct net_device *dev);
static int ae_wan_mac_change_mtu(struct net_device *dev, int new_mtu);
static int ae_wan_mac_set_macaddr(struct net_device *dev, void *p);
int ae_wan_mac_rx(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len);
int ae_wan_mac_tx(struct sk_buff *skb, struct net_device *dev);


extern int (*sw_downstream_nat_rx_hook) (struct sk_buff * skb);
extern int (*wan_speed_test_hook)(struct sk_buff*);
extern int xsi_mac_api_logic_reset(int hsgmii_index);
extern int vlan_manage_judgement(struct sk_buff *skb);
extern int vlan_change_judgement(struct sk_buff *skb, int dir);
extern int (*loop_detect_ae_wan_hook)(struct sk_buff *skb);
extern int (*wan_tr471_hook)(struct sk_buff*);
extern int get_ethaddr(unsigned char *ethaddr, int len);
extern int xsi_check_index_valid(uint hsgmii_index);
/************************************************************************
*                  F U N C T I O N   D E F I N A T I O N
************************************************************************/

int ae_wan_mac_api_rx(ECNT_AE_WAN_Data_s *xsi_data)
{ 
	void *msg_p = NULL;
	uint msg_len = 0;
	struct sk_buff *skb = NULL;
	uint rx_len = 0;
	if(xsi_data){
		
		msg_p = xsi_data->xsi_private.rx_info.msg_p;
		msg_len = xsi_data->xsi_private.rx_info.msg_len;
		skb = xsi_data->xsi_private.rx_info.skb;
		rx_len = xsi_data->xsi_private.rx_info.rx_len;

    	ae_wan_mac_rx(msg_p,msg_len,skb,rx_len);  
	}
	
    return 0;
}

int ae_wan_mac_api_tx(ECNT_AE_WAN_Data_s *xsi_data)
{ 
	struct sk_buff *skb = NULL;
	
	if(xsi_data){
		skb = xsi_data->xsi_private.rx_info.skb;
    	ae_wan_mac_tx(skb,ae_wan_dev);  
	}
	
    return 0;
}

static int ae_wan_mac_get_wan_sel(ECNT_AE_WAN_Data_s *xsi_data)
{ 

	xsi_data->xsi_private.wan_sel = wan_sel;
	
	return 0;
}

xsi_aewan_api_op_t
ae_wan_operation[]=
{
	ae_wan_mac_api_rx,
	ae_wan_mac_get_wan_sel,
	ae_wan_mac_api_tx,
};

ecnt_ret_val ecnt_ae_wan_hook(struct ecnt_data *indata)
{
	ECNT_AE_WAN_Data_s *xsi_data = (ECNT_AE_WAN_Data_s *)indata ;
    
	if(xsi_data->function_id >= AE_WAN_FUNCTION_MAX_NUM) {
		return ECNT_HOOK_ERROR;
	}
	xsi_data->retValue = ae_wan_operation[xsi_data->function_id](xsi_data) ;
	
	return ECNT_CONTINUE;
}

struct ecnt_hook_ops ecnt_driver_ae_wan_op = {
    .name = driver_ae_wan_hook_name,
    .hookfn = ecnt_ae_wan_hook,
    .maintype = ECNT_AE_WAN_MAC,
    .is_execute = 1,
    .subtype = ECNT_DRIVER_API,
    .priority = 1
};

void dump_skb(struct sk_buff *skb)
{
	char tmp[80];
	char *p = skb->data;
	char *t = tmp;
	int i = 0, n = 0;

    if(xsi_dbg_level < XSI_DBG_MSG)
        return;
	printk("ERR skb=%08lx skb->head=%08lx \n data=%08lx len=%d mark=%08lX\n cb=%08lx \n", 
	    (uint32) skb, (uint32) skb->head, (uint32) skb->data, skb->len, skb->mark, 
	    *(unsigned long int *)(skb->cb));
    printk("skb dev %s, orig dev %s\n", skb->dev, skb->original_dev);
	for (i = 0; i < skb->len; i++) {
		t += sprintf(t, "%02x ", *p++ & 0xff);
		if ((i & 0x0f) == 0x0f) {
			printk("%04x: %s\n", n, tmp);
			n += 16;
			t = tmp;
		}
	}
	if (i & 0x0f)
		printk("%04x: %s\n", n, tmp);
	
	return;
}

void wan_to_lan(struct sk_buff *skb, uint8 local)
{
    struct sk_buff *skb2 = NULL;    // for wan2lan 

#if defined (TCSUPPORT_SISM_HOST)
	/* Do not forward 0x884c for ATM packet. ATM packet forward at SAR driver. */
	if (skb->data[12] == 0x88 && skb->data[13] == 0x4c) {
		return;
	}
#endif

    if(masko_on_off
#ifdef  TCSUPPORT_HOST_TCCONSOLE
        || (skb->data[12] == 0xaa && skb->data[13] == 0xaa)
#endif
#if defined(TCSUPPORT_WAN2LAN_EXT)
        || (masko_ext & 0x10)
#endif
        ){
        if(skb_headroom(skb) < TX_STAG_LEN){
            skb2 = skb_copy_expand(skb, TX_STAG_LEN, skb_tailroom(skb) , GFP_ATOMIC);
        }else{
            skb2 = skb_copy(skb,GFP_ATOMIC);
        }

        if(skb2 == NULL){
            printk("wan2lan failure in rx direction for skb3 allocate failure.\n");
        }else{
#if defined(TCSUPPORT_RA_HWNAT)
            if (ra_sw_nat_hook_xfer){
                ra_sw_nat_hook_xfer(skb, skb2);
            }
#endif
            if(local == 1){
                skb_push(skb2,14);
            }
            skb2->mark |= SKBUF_COPYTOLAN;
#if defined(TCSUPPORT_RA_HWNAT)
			if (ra_sw_nat_hook_free)
				ra_sw_nat_hook_free(skb2);
#endif
            macSend(WAN2LAN_CH_ID,skb2);
        }
    }
}


#if defined(TCSUPPORT_CPU_EN7517) || defined(TCSUPPORT_SISM_HOST)
int ADSL_ETYPE_OFFSET = 38;
#endif /* defined(TCSUPPORT_CPU_EN7517) || defined(TCSUPPORT_SISM_HOST) */

#ifdef CONFIG_8021P_REMARK
/* 
 * Fill vlanTag's PCP (Priority Code Point) by skb->mark's bit8~11.
 * If packet's byte12-13 (vlanTag's TPID) is 0x8100, just change
 * the packet's vlanTag's PCP.
 * If packet's byte12-13 (vlanTag's TPID) isn't 0x8100, 
 * insert a VLAN tag with TPID=0x8100, PCP=mark, and VID=0.
 */
#if defined(TCSUPPORT_CPU_EN7517) || defined(TCSUPPORT_SISM_HOST)
static struct sk_buff* adslVlanPriRemark(struct sk_buff *skb, unsigned char uc802prio){
	char * vlan_p = NULL, *ether_type_ptr = NULL;
	ether_type_ptr = skb->data + ADSL_ETYPE_OFFSET;
	uint16 vid=0;
	int copy_len = 0;

	if(*(unsigned short *)ether_type_ptr == htons(0x8100))
	{
		vid=(ntohs(*(unsigned short *)(ether_type_ptr+2)) & 0xfff);
		skb = skb_unshare(skb, GFP_ATOMIC);
		if (!skb)
		{
			return NULL;
		}
	}
	else{
		/*Insert a vlan tag with vid =0*/
		vid=0;
		if ( skb_headroom(skb) < VLAN_HLEN ) {
			struct sk_buff *skb2 = skb_realloc_headroom(skb, VLAN_HLEN);
			dev_kfree_skb(skb);
			if (skb2 == NULL) {
				printk("Vlan:failed to realloc headroom\n");
				return NULL;
			}
			skb = skb2;
		}
		else {
			skb = skb_unshare(skb, GFP_ATOMIC);
			if ( !skb ) {
				return NULL;
			}
		}
	
		/*offset 4 bytes*/
		skb_push(skb, VLAN_HLEN);
	
		copy_len = ADSL_ETYPE_OFFSET;
		/*move the mac address to the beginning of new header*/
		memmove(skb->data, skb->data+VLAN_HLEN, copy_len);
		skb->network_header -= VLAN_HLEN;
		skb->mac_header -= VLAN_HLEN;
	}

	vlan_p = skb->data + ADSL_ETYPE_OFFSET;
	*(unsigned short *)vlan_p = htons(0x8100);
	
	vlan_p += 2;
	*(unsigned short *)vlan_p = 0;
	*(unsigned short*)vlan_p = htons(((uc802prio & 0x7) << 13)|vid) ;

	ether_type_ptr = skb->data + ADSL_ETYPE_OFFSET + 4;
	
	if(*(unsigned short *)ether_type_ptr == htons(0x8100))
	{
		vid=(ntohs(*(unsigned short *)(ether_type_ptr+2)) & 0xfff);
		skb = skb_unshare(skb, GFP_ATOMIC);
		if (!skb)
		{
			return NULL;
		}
		
		vlan_p = skb->data + ADSL_ETYPE_OFFSET + 4;
		*(unsigned short *)vlan_p = htons(0x8100);
		
		vlan_p += 2;
		*(unsigned short *)vlan_p = 0;
		/*3 bits priority and vid vlaue*/
		*(unsigned short*)vlan_p = htons(((uc802prio & 0x7) << 13)|vid) ;
	}	
	return skb;
} 
#endif
static struct sk_buff* vlanPriRemark(struct sk_buff *skb)
{
	char * vlan_p = NULL, *ether_type_ptr = NULL;
	unsigned char ucprio = 0;
	unsigned char uc802prio = 0;
	uint16 vid=0;
	int copy_len = 0;

	if ( skb->mark & QOS_8021p_MARK ) {
		ether_type_ptr = skb->data + 12;
		ucprio = (skb->mark & QOS_8021p_MARK) >> 8;
		if (ucprio < QOS_8021P_0_MARK) { //0~7 remark
			uc802prio = ucprio;
		}
		else if ( QOS_8021P_0_MARK == ucprio ) {	//zero mark
			uc802prio = 0;
		}
		else{//pass through
			/*do nothing*/
			return skb;
		}

		if(*(unsigned short *)ether_type_ptr == htons(0x8100))
		{
			vid=(ntohs(*(unsigned short *)(ether_type_ptr+2)) & 0xfff);
			skb = skb_unshare(skb, GFP_ATOMIC);
			if (!skb)
			{
				return NULL;
			}
		}
#if defined(TCSUPPORT_CPU_EN7517) || defined(TCSUPPORT_SISM_HOST)
		else if(*(unsigned short *)ether_type_ptr == htons(0x884c)){
			return adslVlanPriRemark(skb, uc802prio);
		}
#endif
		else{
			/*do nothing*/
			return skb;
		}
	
		vlan_p = skb->data + 12;
		*(unsigned short *)vlan_p = htons(0x8100);
		
		vlan_p += 2;
		*(unsigned short *)vlan_p = 0;
		/*3 bits priority and vid vlaue*/
		*(unsigned short*)vlan_p = htons(((uc802prio & 0x7) << 13)|vid) ;
		//skb->network_header -= VLAN_HLEN;
		//skb->mac_header -= VLAN_HLEN;
	}
	return skb;
}
#endif /*CONFIG_8021P_REMARK*/

/* if choose eth or ether serdes as wan,should set gdm2 loopback cfg*/
static void ae_wan_qdma_wan_fe_cfg(void)
{
	int i = 0,gdm3_oq;
	int ae_wan_port = 3;
	int ppe_port = 5;
	int dest_port = 0;
#if defined(TCSUPPORT_CPU_AN7583)

	
	int reg_val = 0;
	if(wan_sel != ENUM_PON_SERDES_ASWAN)
	{
		if(wan_sel == ENUM_USB_SERDES_ASWAN){
		 	gdm3_oq= 0x01010101;
			dest_port = DPORT_GDMA4;
			ae_wan_port = HSGMII_LAN_USB_SRCPORT;
		}else if (wan_sel == ENUM_PCIE0_SERDES_ASWAN){
			gdm3_oq= 0x00000000;
			dest_port = DPORT_GDMA4;
			ae_wan_port = HSGMII_LAN_PCIE0_SRCPORT;
		}else if (wan_sel == ENUM_PCIE1_SERDES_ASWAN){
			gdm3_oq= 0x00000000;
			dest_port = DPORT_GDMA4;
			ae_wan_port = HSGMII_LAN_PCIE1_SRCPORT;
		}else if (wan_sel == ENUM_ETH_SERDES_ASWAN){
			gdm3_oq= 0x00000000;
			dest_port = DPORT_GDMA3;
			ae_wan_port = HSGMII_LAN_ETH_SRCPORT;

		}else 
			return;

		

		/* Gdma2 FWD to PSE Port 1(GDM3), disable CRC strip */
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_OC, dest_port);  //port 3 gdm3
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_MC, dest_port);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_BC, dest_port);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_UC, dest_port);

		FE_API_SET_CRC_STRIP(FE_GDM_SEL_GDMA2, FE_DISABLE);
		FE_API_SET_PACKET_LENGTH(FE_GDM_SEL_GDMA2, 0x7d0, 0x1E);
		
		/* enable Gdma2 Loopback */
		reg_val = 0xffffffff ;
		regWrite32_for_fe(GDM2_TXCHN_EN,reg_val);	
		reg_val = 0x0000ffff ;
		regWrite32_for_fe(GDM2_RXCHN_EN,reg_val);	

		FE_API_SET_LOOPBACK_ENABLE(FE_GDM_SEL_GDMA2, AE_WAN_CHANNEL_NUM, FE_ENABLE);
		FE_API_SET_LOOPBACK_MODE(FE_GDM_SEL_GDMA2, FIX_MODE, FIX_MODE, FIX_MODE);

		FE_API_SET_AEWAN_FWDFQ(FE_GDM_SEL_GDMA2,gdm3_oq);
		
		//FE_API_SET_AEWAN_IFCDISABLE(P2_IFC_EN);
		//disable gdm2 vip & ifc
		reg_val = regRead32_for_fe(FE_VIP_PORT_EN);
		reg_val &= 0xfffffffb ;
		regWrite32_for_fe(FE_VIP_PORT_EN,reg_val);

		reg_val = regRead32_for_fe(FE_IFC_PORT_EN);
		reg_val &= 0xfffffffb ;
		regWrite32_for_fe(FE_IFC_PORT_EN,reg_val);

		//serdes packets go through qdma wan
		if(dest_port == DPORT_GDMA3){
			regWrite32_for_fe(PPE_DFP_CPORT, 0x5500);
		}else{
			regWrite32_for_fe(PPE_DFP_CPORT1, 0x50);
			regWrite32_for_fe(PPE2_DFT_CPORT1, 0x50);
		}			

		FE_API_SET_WAN_PORT_7516(FE_DISABLE, 0, ae_wan_port);

		XSI_SET_SP_DFT_CPORT(ae_wan_port, DPORT_QDMA);
	}
#elif defined(TCSUPPORT_CPU_EN7581)

	int reg_val = 0;
	if(wan_sel != ENUM_PON_SERDES_ASWAN)
	{
		if(wan_sel == ENUM_USB_SERDES_ASWAN){
		 	gdm3_oq= 0x01010101;
			dest_port = DPORT_GDMA4;
			ae_wan_port = HSGMII_LAN_USB_SRCPORT;
		}else if (wan_sel == ENUM_PCIE0_SERDES_ASWAN){
			gdm3_oq= 0x04040404;
			dest_port = DPORT_GDMA3;
			ae_wan_port = HSGMII_LAN_PCIE0_SRCPORT;
		}else if (wan_sel == ENUM_PCIE1_SERDES_ASWAN){
			gdm3_oq= 0x05050505;
			dest_port = DPORT_GDMA3;
			ae_wan_port = HSGMII_LAN_PCIE1_SRCPORT;
		}else if (wan_sel == ENUM_ETH_SERDES_ASWAN){
			gdm3_oq= 0x00000000;
			dest_port = DPORT_GDMA4;
			ae_wan_port = HSGMII_LAN_ETH_SRCPORT;

		}else 
			return;

		

		/* Gdma2 FWD to PSE Port 1(GDM3), disable CRC strip */
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_OC, dest_port);  //port 3 gdm3
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_MC, dest_port);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_BC, dest_port);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_UC, dest_port);

		FE_API_SET_CRC_STRIP(FE_GDM_SEL_GDMA2, FE_DISABLE);
		FE_API_SET_PACKET_LENGTH(FE_GDM_SEL_GDMA2, 0x7d0, 0x1E);
		
		/* enable Gdma2 Loopback */
		reg_val = 0xffffffff ;
		regWrite32_for_fe(GDM2_TXCHN_EN,reg_val);	
		reg_val = 0x0000ffff ;
		regWrite32_for_fe(GDM2_RXCHN_EN,reg_val);	

		FE_API_SET_LOOPBACK_ENABLE(FE_GDM_SEL_GDMA2, AE_WAN_CHANNEL_NUM, FE_ENABLE);
		FE_API_SET_LOOPBACK_MODE(FE_GDM_SEL_GDMA2, FIX_MODE, FIX_MODE, FIX_MODE);

		FE_API_SET_AEWAN_FWDFQ(FE_GDM_SEL_GDMA2,gdm3_oq);
		
		//FE_API_SET_AEWAN_IFCDISABLE(P2_IFC_EN);
		//disable gdm2 vip & ifc
		reg_val = regRead32_for_fe(FE_VIP_PORT_EN);
		reg_val &= 0xfffffffb ;
		regWrite32_for_fe(FE_VIP_PORT_EN,reg_val);

		reg_val = regRead32_for_fe(FE_IFC_PORT_EN);
		reg_val &= 0xfffffffb ;
		regWrite32_for_fe(FE_IFC_PORT_EN,reg_val);

		//serdes packets go through qdma wan
		if(dest_port == DPORT_GDMA3){
			regWrite32_for_fe(PPE_DFP_CPORT, 0x5500);
		}else{
			if(wan_sel == ENUM_ETH_SERDES_ASWAN){
				reg_val = regRead32_for_fe(FE_SRC_FC_MAP) & fc_eth_mask;
				reg_val = reg_val | 0x2;
				regWrite32_for_fe(FE_SRC_FC_MAP,reg_val);
			}
			else if(wan_sel == ENUM_USB_SERDES_ASWAN){
				reg_val = regRead32_for_fe(FE_SRC_FC_MAP) & fc_usb_mask;
				reg_val = reg_val | 0x200;
				regWrite32_for_fe(FE_SRC_FC_MAP,reg_val);
			}	
			else{
			regWrite32_for_fe(PPE_DFP_CPORT1, 0x50);
			regWrite32_for_fe(PPE2_DFT_CPORT1, 0x50);
				}
		}			

		FE_API_SET_WAN_PORT_7516(FE_DISABLE, 0, ae_wan_port);

		XSI_SET_SP_DFT_CPORT(ae_wan_port, DPORT_QDMA);
	}


	#else

	if(wan_sel != ENUM_PON_SERDES_ASWAN)
	{
		if(wan_sel == ENUM_USB_SERDES_ASWAN)
		 	gdm3_oq= 0x06060606;
		else if (wan_sel == ENUM_PCIE0_SERDES_ASWAN)
			gdm3_oq= 0x04040404;
		else
			gdm3_oq= 0x05050505;

		/* Gdma2 FWD to PSE Port 1(GDM3), disable CRC strip */
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_OC, FE_DP_GDMA3_HWF);  //port 3 gdm3
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_MC, FE_DP_GDMA3_HWF);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_BC, FE_DP_GDMA3_HWF);
		FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_UC, FE_DP_GDMA3_HWF);
		FE_API_SET_CRC_STRIP(FE_GDM_SEL_GDMA2, FE_DISABLE);
		FE_API_SET_PACKET_LENGTH(FE_GDM_SEL_GDMA2, 0x7d0, 0x20);
		
		/* enable Gdma2 Loopback */
		FE_API_SET_LOOPBACK_ENABLE(FE_GDM_SEL_GDMA2, AE_WAN_CHANNEL_NUM, FE_ENABLE);
		FE_API_SET_LOOPBACK_MODE(FE_GDM_SEL_GDMA2, FIX_MODE, FIX_MODE, FIX_MODE);

		FE_API_SET_AEWAN_FWDFQ(FE_GDM_SEL_GDMA2,gdm3_oq);
		FE_API_SET_AEWAN_IFCDISABLE(P2_IFC_EN);
		PPE_API_SET_PPE_PORT(ppe_port);

		FE_API_SET_WAN_PORT_7516(FE_DISABLE, 0, ae_wan_port);
	}
	#endif
	return 0;
}

void update_ae_wan_sw_mib(xsiWanAdapter_t *mib_type,struct sk_buff *skb, int direction)
{
    if (mib_type->statisticOn) {
        switch(direction){
            case XSI_RX:
                mib_type->xsiStat.inOctets += (skb->len > 64 ? skb->len : 64);
				if((skb->data[0] == 0xFF)&&(skb->data[1] == 0xFF)&&(skb->data[2] == 0xFF)
					&&(skb->data[3] == 0xFF)&&(skb->data[4] == 0xFF)&&(skb->data[5] == 0xFF))
					mib_type->xsiStat.inBroadcastPkts++;
				else if(skb->data[0]&0x01)
					mib_type->xsiStat.inMulticastPkts++;
				else
					mib_type->xsiStat.inUnicastPkts++;				
                break;
            case XSI_TX:
                mib_type->xsiStat.outOctets += (skb->len > 64 ? skb->len : 64);
            	if((skb->data[0] == 0xFF)&&(skb->data[1] == 0xFF)&&(skb->data[2] == 0xFF)
					&&(skb->data[3] == 0xFF)&&(skb->data[4] == 0xFF)&&(skb->data[5] == 0xFF))
					mib_type->xsiStat.outBroadcastPkts++;        		
                else if(skb->data[0]&0x01)
            		mib_type->xsiStat.outMulticastPkts++;
            	else
            		mib_type->xsiStat.outUnicastPkts++;
                break;
			default:
				break;
        }
    }
	return;
}


static int ae_wan_mac_set_macaddr(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;

	if(!dev || !addr)
		return(-EIO);

  	if (addr && !is_valid_ether_addr(addr->sa_data))
    	return(-EIO);

  	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
    memcpy(def_ae_wan_mac_addr, addr->sa_data, dev->addr_len);
	
    xsi_api_set_mac(XSI_IDX_SEL,def_ae_wan_mac_addr);
	
    return 0;
}

static int ae_wan_mac_start(struct net_device *dev)
{
    uint8 flashMacAddr[6] = {0};
    struct sockaddr addr = {0};
    uint8 i = 0;
	

	if(get_ethaddr(flashMacAddr, sizeof(flashMacAddr)) != 0){
		printk("func:%s get ethaddr error!!!\n",__func__);
	}

	for (i=0; i<6; i++) {
		addr.sa_data[i] = flashMacAddr[i];
	}


	if (addr.sa_data[0] == 0 && addr.sa_data[1] == 0 && addr.sa_data[2] == 0 && addr.sa_data[3] == 0 && addr.sa_data[4] == 0 && addr.sa_data[5] == 0)
	{
		printk("The MAC address in flash is null!\n");
		printk("Xsi mac interface will be closed!\n");
		xsi_mac_itf_close(XSI_IDX_SEL);
	}

    ae_wan_mac_set_macaddr(dev,&addr);
	
    ae_wan_p->dev = dev;
    
    return 0;
}

void ae_wan_qdma_wan_tx_limit(void)
{
    QDMA_TxRateLimitSet_T txRateLimitSet;
    int overRate = 100;
    int linkRate = 2500;
    static int pre_linkRate = 0;

    if(pre_linkRate != linkRate)
    {
        memset(&txRateLimitSet, 0, sizeof(QDMA_TxRateLimitSet_T));
        txRateLimitSet.chnlRateLimitEn = 1;
    	txRateLimitSet.chnlIdx = AE_WAN_CHANNEL_NUM;
    	txRateLimitSet.rateLimitValue = (linkRate * 1000 + overRate); 	/* unit: kbps */
    	QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_WAN, &txRateLimitSet);
    	pre_linkRate = linkRate;
	}
}

static void xsi_dev_check_linkstate(struct net_device *dev, int linkstate)
{
	static int aewan_not_ready_count=0;
	
	if(dev) {
		if(linkstate&&(dev->state==2 || dev->state==3)&& (aewan_not_ready_count<2)){
		netif_carrier_off(dev);
		netif_carrier_on(dev);
		aewan_not_ready_count++;
		}
		if(linkstate){
			if(!netif_carrier_ok(dev))
				netif_carrier_on(dev);
		} else{
			if(netif_carrier_ok(dev))
				netif_carrier_off(dev);
		}
	}
			
	return;
}
void xsi_handle_link_stats(struct net_device *dev, unsigned int hsgmii_index)
{
    unsigned int link_stats = 0;
    unsigned int rate = 0, rate_tmp = 0, duplex = 0;

	EN8811_MULTI_GET_LINK_RATE(hsgmii_index,&rate_tmp, &duplex);
	if(rate_tmp){
		link_stats = 1; 
	}else{
		link_stats = 0; 
	}
    
    xsi_dev_check_linkstate(dev,link_stats);

}

void xsi_handle_sfp_link_stats(struct net_device *dev, unsigned int hsgmii_index)
{
    unsigned int link_stats = 0;
   
    link_stats = xsi_get_xsgmii_sfp_linkstatus(hsgmii_index);
    xsi_dev_check_linkstate(dev,link_stats);

}

int ae_wan_check_rate_index(uint xsi_rate_idx)
{
	if(xsi_rate_idx > ENUM_UNKNOW_SERDES)
		return 0;
	else
		return 1 ;

}

int ae_wan_get_xsgmii_serdes_linkrate(void)
{
	unsigned int rate = 0, rate_tmp = 0, duplex = 0;

	if(0 == xsi_check_index_valid(XSI_IDX_SEL))
		return 0;

	if(0 == ae_wan_check_rate_index(XSI_RATE_IDX)) {
		printk("%s:%d xsgmii_serdes_inphy index error:%d!\n",__FUNCTION__,__LINE__,XSI_RATE_IDX);
		return 0;
	}

	if((WAN_LINK_MODE <= USXGII_100M) && (WAN_LINK_MODE >= USXGII_10G))
	{
		//0:10g, 1:5g, 2:2.5g, 3: 1g, 4:100M
		rate = xsgmii_api(XSI_RATE_IDX,USXGMII,3,0,1);
	}
	else if(WAN_LINK_MODE == HSGMII_2p5G)
	{
		//2:2.5g
		rate = 2;
#if defined(TCSUPPORT_ETHER_8811)
		EN8811_MULTI_GET_LINK_RATE(XSI_IDX_SEL,&rate_tmp, &duplex);
		if(rate_tmp){
			rate = rate_tmp; //if en8811 link up
		}
#endif
	}
	else if((WAN_LINK_MODE > HSGMII_2p5G) && (WAN_LINK_MODE <= SGMII_10M))
	{
		//3:1g, 4:100M, 5:10M
		rate = xsgmii_api(XSI_RATE_IDX,SGMII,4,0,3) + 3;
	}

	if(rate > 5){
		XSI_MSG(XSI_DBG_MSG,"%s:%d error rate:%d!\n",__FUNCTION__,__LINE__,rate);
		return 0;
	}
	return rate ;

}

int ae_wan_ratelimit_init(void)
{
	int rateValue[6] = {10000, 5000, 2500, 1000, 100, 10};	/* unit: mbps */
	QDMA_TxRateLimitSet_T txRateLimitSet;

	memset(&txRateLimitSet, 0, sizeof(QDMA_TxRateLimitSet_T));
	xsi_speed = ae_wan_get_xsgmii_serdes_linkrate();
	txRateLimitSet.chnlRateLimitEn = 1;
	txRateLimitSet.chnlIdx = AE_WAN_CHANNEL_NUM;     //QDMA_API_GET_CHANNEL_CFG(ECNT_QDMA_LAN, HSGMII_LAN_PCIE0_CHNL);
	txRateLimitSet.rateLimitValue = (rateValue[xsi_speed] * 1000); 	// unit: kbps
	if(xsi_speed == 0){
		txRateLimitSet.chnlRateLimitEn = 0;
		txRateLimitSet.rateLimitValue = 0;
	}
	QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_WAN, &txRateLimitSet);

	return 0;
}

static void ae_wan_mac_monitor(unsigned long data)
{
	int preLinkState = DOWN;
	unsigned char reg_val = 0;
    int rateValue[6] = {10000, 5000, 2500, 1000, 100, 10};
	int wan_speed = 0;
	QDMA_TxRateLimitSet_T txRateLimitSet;
	memset(&txRateLimitSet, 0, sizeof(QDMA_TxRateLimitSet_T));

	wan_speed = ae_wan_get_xsgmii_serdes_linkrate();

	if(xsi_speed != wan_speed)
	{
		txRateLimitSet.chnlRateLimitEn = 1;
		txRateLimitSet.chnlIdx = AE_WAN_CHANNEL_NUM;  //QDMA_API_GET_CHANNEL_CFG(ECNT_QDMA_WAN, HSGMII_LAN_PCIE0_CHNL);
		txRateLimitSet.rateLimitValue = (rateValue[wan_speed] * 1000); 	// unit: kbps
		if(wan_speed == 0){
			txRateLimitSet.chnlRateLimitEn = 0;
			txRateLimitSet.rateLimitValue = 0;
		}
		QDMA_API_SET_TX_RATELIMIT(ECNT_QDMA_WAN, &txRateLimitSet);

		xsi_speed = wan_speed;
	}
#if SUPPORT_SERDES_ETHER
#if SUPPORT_AS21xx_WA
	if(get_serdes_phy_sel(ECNT_EVENT_SERDES_SEL_PON) == ECNT_EVENT_SERDES_PHY_F)
	{
		if(xpon_linkup_wrapper_wa_flag ==1)
		{
			printk("xpon linkup_wrapper_wa  mdelay 2000 \n");
			mdelay(xpon_linkup_wrapper_delay_time);
			xsgmii_api(2,0,248,0,0);
		}
	}
#endif

	if((get_serdes_phy_sel(ECNT_EVENT_SERDES_SEL_PON) == ECNT_EVENT_SERDES_PHY_E) && (get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_PON) == ECNT_EVENT_SERDES_PON_USXGMII) && PON_WAN_VALID )
	{
		xsi_handle_sfp_link_stats(ae_wan_dev,XSI_IDX_SEL);
	}
	else if((get_serdes_phy_sel(ECNT_EVENT_SERDES_SEL_ETHER) == ECNT_EVENT_SERDES_PHY_E) && (get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_ETHER) == ECNT_EVENT_SERDES_ETHER_USXGMII) && ETH_WAN_VALID)
	{
		xsi_handle_sfp_link_stats(ae_wan_dev,XSI_IDX_SEL);
	}
	if((get_serdes_phy_sel(ECNT_EVENT_SERDES_SEL_ETHER) == ECNT_EVENT_SERDES_PHY_AN8811)&&ETH_WAN_VALID)
	{
		xsi_handle_link_stats(ae_wan_dev,XSI_IDX_SEL);
	}
#endif
#if SUPPORT_SERDES_USB
	else if((get_serdes_phy_sel(ECNT_EVENT_SERDES_SEL_USB1) == ECNT_EVENT_SERDES_PHY_AN8811)&&USB_WAN_VALID)
	{
		xsi_handle_link_stats(ae_wan_dev,XSI_IDX_SEL);
	}
#endif
#if	SUPPORT_GDMA_CHANNEL_RETIRE
	if (ae_wan_p == NULL)
		return;
	
	preLinkState = xsiPortState;
	xsiPortState = xsi_mac_api_get_xsgmii_phy_linkstats(XSI_IDX_SEL);
	xsiPortState = xsiPortState & (0x1);

	if((preLinkState == UP) && (xsiPortState == DOWN))
	{
		printk("%s:%d do channel retire for GDMA2\n",__FUNCTION__,__LINE__);
		ETHER_API_GET_FLOW_CONTROL(6, &reg_val);
		ETHER_API_SET_FLOW_CONTROL(6, 0);					
		xsiSetTxmbiDisable(XSI_IDX_SEL);
		xsimaclogicrst(XSI_IDX_SEL);
		xsimaclogicrstenable(XSI_IDX_SEL);
		FE_API_SET_CHANNEL_RETIRE_ALL(FE_GDM_SEL_GDMA2, 32);	
		xsiSetTxmbiEnable(XSI_IDX_SEL);
		ETHER_API_SET_FLOW_CONTROL(6, reg_val);
	}

#endif

#if defined(CONFIG_TP_IMAGE)
	if(!ae_wan_dev)
		goto restart_timer;

	int status = 0;
	bool an=0, link_status=0;
	static bool eth_link_status = 0;

	if (1 == is_ae_wan_en8811phy)
	{
		EN8811_MULTI_GET_LINK_STATUS(XSI_AE_IDX, &an, &link_status);
	}
	else	//is ae_wan is not used as en8811 phy, we assume it up now, maybe we should implement detection.
	{
		link_status = 1;
	}
	status = link_status;
	if(eth_link_status != status && status == DOWN)
	{
		netif_carrier_off(ae_wan_dev);
	}
	if(eth_link_status != status && status == UP)
	{
		netif_carrier_on(ae_wan_dev);
	}
	eth_link_status = status;

restart_timer:	
  	mod_timer(&xsi_mac_timer,jiffies + msecs_to_jiffies(250));
#else
	mod_timer(&xsi_mac_timer,jiffies + msecs_to_jiffies(250));
#endif

	return 0;
}

static int ae_wan_mac_open(struct net_device *dev)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    napi_enable(&ae_wan_p->napi);
#endif
    netif_start_queue(dev);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)	
	init_timer(&xsi_mac_timer);
    xsi_mac_timer.expires = jiffies + msecs_to_jiffies(250);
  	xsi_mac_timer.function = ae_wan_mac_monitor;
    xsi_mac_timer.data = 0;
#else
	timer_setup(&xsi_mac_timer, ae_wan_mac_monitor, 0);
	xsi_mac_timer.expires = jiffies + (HZ * 2);
#endif

    add_timer(&xsi_mac_timer);

	FE_API_SET_GDMA_ENABLE(XSI_IDX_SEL);
	ae_wan_qdma_wan_fe_cfg();

    return 0;
}

static int ae_wan_mac_close(struct net_device *dev)
{ 
	uint32 value = 0, i = 0, ret = 0;

	netif_stop_queue(dev);
	//Close GDM2 before disable napi otherwise qdma_wan's ring would be filled. 
	FE_API_SET_GDMA_DISABLE(XSI_IDX_SEL);

	while (i++ < 5000) {
			mdelay(1);
			ret = QDMA_API_CHECK_DSCP_IS_FREE(ECNT_QDMA_WAN);
			if(!ret)
				break;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    napi_disable(&ae_wan_p->napi);
#endif
	del_timer_sync(&xsi_mac_timer);
	
    return 0;
}

static struct net_device_stats *ae_wan_mac_stats(struct net_device *dev)
{
	struct net_device_stats *stats = NULL;
	stats = &ae_wan_p->stats;
	
	stats->rx_packets = ae_wan_p->xsiStat.inUnicastPkts + ae_wan_p->xsiStat.inMulticastPkts + ae_wan_p->xsiStat.inBroadcastPkts;
	stats->tx_packets = ae_wan_p->xsiStat.outUnicastPkts + ae_wan_p->xsiStat.outMulticastPkts + ae_wan_p->xsiStat.outBroadcastPkts;
	stats->rx_bytes = ae_wan_p->xsiStat.inOctets;
	stats->tx_bytes = ae_wan_p->xsiStat.outOctets;;
	stats->rx_dropped = ae_wan_p->xsiStat.inDrops;
	stats->tx_dropped = ae_wan_p->xsiStat.outDrops;
	stats->multicast = ae_wan_p->xsiStat.inMulticastPkts;
	stats->rx_errors = ae_wan_p->xsiStat.inErrors;
	stats->tx_errors = ae_wan_p->xsiStat.outErrors;
	return stats;
}
#ifdef CONFIG_TP_IMAGE
static struct net_device_stats *ae_wan_mac_stats_tp(struct net_device *dev)
{
	struct net_device_stats *stats = NULL;
	stats = &ae_wan_p->stats;
	FE_RxCnt_t rxCnt;
	FE_TxCnt_t txCnt;
	
	memset(&rxCnt, 0, sizeof(FE_RxCnt_t));
	memset(&txCnt, 0, sizeof(FE_TxCnt_t));
	FE_API_GET_ETH_RX_CNT(&rxCnt);
	FE_API_GET_ETH_TX_CNT(&txCnt);
	
	stats->rx_packets = rxCnt.rxFrameCnt;
	stats->tx_packets = txCnt.txFrameCnt;
	stats->rx_bytes = rxCnt.rxFrameLen;
	stats->tx_bytes = txCnt.txFrameLen;
	stats->rx_dropped = rxCnt.rxDropCnt;
	stats->tx_dropped = txCnt.txDropCnt;
	stats->multicast = rxCnt.rxMulticastCnt + txCnt.txMulticastCnt;
	stats->rx_errors = ae_wan_p->xsiStat.inErrors + rxCnt.rxCrcCnt + rxCnt.rxFragFameCnt + rxCnt.rxJabberFameCnt;
	stats->tx_errors = ae_wan_p->xsiStat.outErrors;
	return stats;
}



static struct rtnl_link_stats64 * ae_wan_mac_stats_64_tp(struct net_device* dev, struct rtnl_link_stats64* stats)
{
	FE_RxCnt_t rxCnt;
	FE_TxCnt_t txCnt;
	
	memset(&rxCnt, 0, sizeof(FE_RxCnt_t));
	memset(&txCnt, 0, sizeof(FE_TxCnt_t));
	FE_API_GET_ETH_RX_CNT(&rxCnt);
	FE_API_GET_ETH_TX_CNT(&txCnt);
	
	stats->rx_packets = rxCnt.rxFrameCnt;
	stats->tx_packets = txCnt.txFrameCnt;
	stats->rx_bytes = rxCnt.rxFrameLen;
	stats->tx_bytes = txCnt.txFrameLen;
	stats->rx_dropped = rxCnt.rxDropCnt;
	stats->tx_dropped = txCnt.txDropCnt;
	stats->multicast = rxCnt.rxMulticastCnt + txCnt.txMulticastCnt;
	stats->rx_errors = ae_wan_p->xsiStat.inErrors + rxCnt.rxCrcCnt + rxCnt.rxFragFameCnt + rxCnt.rxJabberFameCnt;
	stats->tx_errors = ae_wan_p->xsiStat.outErrors;
	return stats;

}
#endif /* CONFIG_TP_IMAGE */

int ae_wan_mac_tx(struct sk_buff *skb, struct net_device *dev)
{
    QDMA_TxMsg_Ex_T xsiTxMsg;
	int error = 0;
	int tsid = 0x7f;
	int acnt1 = 0x1f;
	int acnt0 = 0x1f;
	struct port_info xsi_info = {0};
	struct port_info *qdma_info = NULL;
#if defined(TCSUPPORT_SISM_HOST)
	unsigned short *type1, *type2;
#endif

	XSI_MSG(XSI_DBG_MSG, "TX enter\n");

	if(unlikely(skb == NULL))
    {
        return NETDEV_TX_OK;
    }

#if defined(TCSUPPORT_SISM_HOST)
	type1 = ntohs(*(unsigned short *)(skb->data+12));
	type2 = ntohs(*(unsigned short *)(skb->data+16));
#if 0
	/* [7562: host_bp1.c & central_ctrl_polling.c] */
	/* if EN7517 is booting, only inic packet could be transmit to EN7517*/
	if ((inicState == BOOTING_MODE)
		&& (type1 != 0xbeef) && (type2 != 0xbeef))
	{
		ae_wan_p->xsiStat.outDrops++;
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}
#endif
#if 1
	/* if EN7517 is booting, only inic packet could be transmit to EN7517*/
	if (enAeWanInicFilter)
	{
		if(!((type1 >= 0xbeea && type1 <= 0xbeef) || (type2 >= 0xbeea && type2 <= 0xbeef)))
		{
			ae_wan_p->xsiStat.outDrops++;
			dev_kfree_skb_any(skb);
			return NETDEV_TX_OK;
		}
	}
#endif
#endif

	update_ae_wan_sw_mib(ae_wan_p, skb, XSI_TX);

	if(SUPPORT_EN8851){	
		vlan_change_judgement(skb, 1);
	}

#ifdef CONFIG_8021P_REMARK
	skb=vlanPriRemark(skb);
	if(skb==NULL){
		printk("802.1p remark failure\r\n");
		return NETDEV_TX_OK;
	}
#endif

	memset(&xsi_info, 0, sizeof(struct port_info));
	FE_API_GET_METER_IDX(skb, UP_STREAM, &tsid, 0);
	xsi_info.tsid = tsid;   
    xsi_info.nbq = AE_WAN_CHANNEL_NUM;
	xsi_info.magic = FOE_MAGIC_AE_WAN;
	xsi_info.channel = AE_WAN_CHANNEL_NUM;/*GDM1: channel 0~6; PPE: channel 7; GDM3: start from channel 8*/
	xsi_info.ds_to_qdma = 0;

	memset(&xsiTxMsg, 0, sizeof(QDMA_TxMsg_Ex_T));
    QDMA_API_SET_TXMSG(ECNT_QDMA_WAN,&xsiTxMsg,TXMSG_FIELD_MTR0,tsid);
    QDMA_API_SET_TXMSG(ECNT_QDMA_WAN,&xsiTxMsg,TXMSG_FIELD_FPORT,DPORT_GDMA2);
    QDMA_API_SET_TXMSG(ECNT_QDMA_WAN,&xsiTxMsg,TXMSG_FIELD_CNH,xsi_info.channel);
	FE_API_GET_ACNT1_IDX(skb, UP_STREAM, &acnt1);
	FE_API_GET_ACNT0_IDX(skb, UP_STREAM, &acnt0);
    QDMA_API_SET_TXMSG(ECNT_QDMA_WAN,&xsiTxMsg,TXMSG_FIELD_ACNT1,acnt1);
    QDMA_API_SET_TXMSG(ECNT_QDMA_WAN,&xsiTxMsg,TXMSG_FIELD_ACNT0,acnt0);

    QDMA_API_SET_TXMSG(ECNT_QDMA_WAN,&xsiTxMsg,TXMSG_FIELD_NBQ,0);
#if defined(TCSUPPORT_SISM_HOST)
	if(*(skb->data+12) == 0xbe &&
		(*(skb->data+13) >= 0xea && *(skb->data+13) <= 0xef))
	{
		xsi_info.txq_is_valid = 1;
		xsi_info.txq = 7; // adsl control packets to highest queue 7
	}
#endif
	qdma_info = &xsi_info;

	if (ae_wan_fast_mode_flag) {
		qdma_info->fast = 1;
	} else {
		qdma_info->fast = 0;
	}

	XSI_MSG(XSI_DBG_MSG,"xsiTxMsg.raw.channel=%d\n",xsi_info.channel);
	XSI_MSG(XSI_DBG_MSG,"xsi_info.txq=%d eth_macSTagEn=%d magic=%d ds_to_qdma=%d channel=%d\n",xsi_info.txq,xsi_info.eth_macSTagEn,xsi_info.magic,xsi_info.ds_to_qdma,xsi_info.channel);

#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_MULTICAST_VLAN)
		if ((NULL != skb) && (NULL != ae_wan_multicastVlanTxHook))
		{
			 ae_wan_multicastVlanTxHook(skb);
		}
#endif /* CONFIG_TP_IMAGE && INCLUDE_MULTICAST_VLAN */

	wan_to_lan(skb,0);
	dump_skb(skb);
	error = QDMA_API_TRANSMIT_PACKETS(ECNT_QDMA_WAN,skb,xsiTxMsg.txmsg0, xsiTxMsg.txmsg1, qdma_info);
	if(unlikely(error)){
		dev_kfree_skb_any(skb);
		ae_wan_p->xsiStat.outErrors++;
		ae_wan_p->xsiStat.outDrops++;
		return NETDEV_TX_OK;
	}
	XSI_MSG(XSI_DBG_MSG, "TX leave\n");
	return NETDEV_TX_OK;

}

static int ae_wan_mac_change_mtu(struct net_device *dev, int new_mtu)
{
	if (new_mtu < 68 || new_mtu > 2000)
		return -EINVAL;
	dev->mtu = new_mtu;
	return 0;
}



static const struct net_device_ops ae_wan_netdev_ops = {
    .ndo_init               = ae_wan_mac_start,
	.ndo_open				= ae_wan_mac_open,
	.ndo_stop				= ae_wan_mac_close,
	.ndo_start_xmit 		= ae_wan_mac_tx,
#ifdef CONFIG_TP_IMAGE
	.ndo_get_stats			= ae_wan_mac_stats_tp,
	.ndo_get_stats64		= ae_wan_mac_stats_64_tp,
#else /* CONFIG_TP_IMAGE */
	.ndo_get_stats          = ae_wan_mac_stats,
#endif /* CONFIG_TP_IMAGE */
#ifdef TCSUPPORT_MAX_PACKET_2000
    .ndo_change_mtu         = ae_wan_mac_change_mtu,
#else
    .ndo_change_mtu         = eth_change_mtu,
#endif
	.ndo_set_mac_address    = ae_wan_mac_set_macaddr,
	.ndo_validate_addr      = eth_validate_addr,
};


/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
    Funtcion:       ae_wan_mac_rx
    Description:
    Calls:
    Called by:      qdma wan
    Input:
    Output:
    Return:
    Others:
************************************************************************/
int ae_wan_mac_rx(void *msg_p, uint msg_len, struct sk_buff *skb, uint rx_len)
{
	QDMA_RxMsg_Ether_T xsiRxMsg;

	XSI_MSG(XSI_DBG_MSG, "RX enter\n");

	if(unlikely(skb == NULL)){
        printk("Error Skb NULL, fail to receive!\n");
    }

    /**************** check packet len *****************/ 
	if (unlikely((rx_len < 52) || (rx_len > 2000)))
	{
        ae_wan_p->xsiStat.inErrors++;
		ae_wan_p->xsiStat.inDrops++;
        dev_kfree_skb_any(skb);
        XSI_MSG(XSI_DBG_ERR, "rx len(%d) invalid\n", rx_len);
        return 1;
    }
    skb_put(skb, rx_len);
    dump_skb(skb);

#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_MULTICAST_VLAN)
	if (NULL != ae_wan_multicastVlanRxHook )
	{
		ae_wan_multicastVlanRxHook(skb);
	}
#endif /* CONFIG_TP_IMAGE && INCLUDE_MULTICAST_VLAN */

    wan_to_lan(skb, 0);

    /**************** check rx msg *****************/
    memset(&xsiRxMsg,0,sizeof(QDMA_RxMsg_Ether_T));
    QDMA_API_GET_RXMSG_ETHER(ECNT_QDMA_LAN,msg_p,&xsiRxMsg);

	if (likely(ra_sw_nat_hook_rxinfo))
		ra_sw_nat_hook_rxinfo(skb, FOE_MAGIC_AE_WAN, (char*)&(xsiRxMsg.rxmsg1), sizeof(unsigned int));

	/**************** check rx msg *****************/ 
	if( unlikely(xsiRxMsg.ip4f)){
		ae_wan_p->xsiStat.inErrors++;
		printk("IP checksum error \n");
		}
	if( unlikely(xsiRxMsg.l4f)){
		ae_wan_p->xsiStat.inErrors++;
		printk("TCP/UDP checksum error \n");
	}
            
	if ((xsiRxMsg.ip4 || xsiRxMsg.ip6) && (!xsiRxMsg.ip4f && !xsiRxMsg.l4f)){
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	}else{
		skb->ip_summed = CHECKSUM_NONE;
	}

	/**************** Count the MIB-II *****************/
	{
		update_ae_wan_sw_mib(ae_wan_p, skb, XSI_RX);
		skb->dev = ae_wan_dev;
	}

#ifdef TCSUPPORT_CT_LOOPDETECT
	if(loop_detect_ae_wan_hook && loop_detect_ae_wan_hook(skb)){
		
		return 1;
	}
#endif
    /**************** Layer 2 packet handle *****************/
	if(SUPPORT_EN8851){
		if(!(vlan_manage_judgement(skb)))
			return 0;		
		vlan_change_judgement(skb, 0);
}
	
    skb->protocol = eth_type_trans(skb, skb->dev);
    skb->dev->last_rx = jiffies;  
	
	if(wan_speed_test_hook)
	{
		if(wan_speed_test_hook(skb)==0)
				return 0;	
	}	
	if(wan_tr471_hook)
	{
		if(wan_tr471_hook(skb)==0)
				return 0;	
	}	

#ifdef TCSUPPORT_RA_HWNAT    
    if (ra_sw_nat_hook_rx) {
        if (ra_sw_nat_hook_rx(skb) == 0) {
            return 0;
        }
    }
#endif

    if(0 == ECNT_HOOK_MULTICAST_DATA_HANLDE(skb))
	{
		dev_kfree_skb_any(skb);
		return 1;
	}

#if defined(TCSUPPORT_SWNAT)
	if(sw_downstream_nat_rx_hook){
		if(sw_downstream_nat_rx_hook(skb)){
			return 0;
		}
	}
#endif
	

    XSI_MSG(XSI_DBG_MSG, "RX leave\n");
    netif_receive_skb(skb);

	return 0;
}

int ae_wan_event_handler(QDMA_EventType_t event)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    unsigned long flags;
	switch(event) {
		case QDMA_EVENT_RECV_PKTS:
		case QDMA_EVENT_NO_RX_BUFFER:
			{				
				if(ae_wan_p == NULL) {
					return IRQ_HANDLED ;
				}				
				if (event == QDMA_EVENT_NO_RX_BUFFER)
                    XSI_MSG(XSI_DBG_ERR,"\nQDMA_EVENT_NO_RX_BUFFER\n");
			    spin_lock_irqsave(&ae_wan_napilock, flags);
				if(napi_schedule_prep(&ae_wan_p->napi)) {
					QDMA_API_DISABLE_RXPKT_INT(ECNT_QDMA_WAN) ;
					__napi_schedule(&ae_wan_p->napi) ;
				}
				spin_unlock_irqrestore(&ae_wan_napilock, flags);
			}
			break ;
		case QDMA_EVENT_TX_CROWDED:
			break ;
		
		default:
			break ;
	}
#endif
	
return 0 ;
}

int serdes_phy_init(uint hsgmii_index)
{
#if SUPPORT_AE_WAN_NEW_ARCH
	ECNT_EVENT_SYSTEM_SERDES_PHY_SEL_t phy_sel = ECNT_EVENT_SERDES_PHY_AN8811;
#ifdef TCSUPPORT_BOARD_SELECT
	ECNT_EVENT_SYSTEM_SERDES_SEL_t serdes_port = get_serdes_port_count();
#else
	ECNT_EVENT_SYSTEM_SERDES_SEL_t serdes_port = ECNT_EVENT_SERDES_SEL_MAX;
#endif

    switch(hsgmii_index)
    {
#if SUPPORT_SERDES_PCIE0
        case XSI_PCIE0_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_WIFI1;
            break;
#endif
#if SUPPORT_SERDES_PCIE1
        case XSI_PCIE1_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_WIFI2;
            break;
#endif
#if SUPPORT_SERDES_USB
        case XSI_USB_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_USB1;
            break;
#endif
#if SUPPORT_SERDES_PON
        case XSI_AE_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_PON;
            break;
#endif
#if SUPPORT_SERDES_ETHER
        case XSI_ETH_IDX:
            serdes_port = ECNT_EVENT_SERDES_SEL_ETHER;
            break;
#endif
        default:
            printk("ae_wan_index input error, hsgmii_index = %d\n",hsgmii_index);
    }

	phy_sel = get_serdes_phy_sel(serdes_port);
	
	switch (phy_sel){
		case ECNT_EVENT_SERDES_PHY_AN8811:
			xsi_set_xsgmii_serdes_speed_forcemode_andisable(hsgmii_index, WAN_LINK_MODE);
			break;
		case ECNT_EVENT_SERDES_PHY_A:
			//set phy A through mdio
			xsi_set_mdio();
			//xsgmii_api(1,0,7,1,1)
			xsi_set_xsgmii_serdes_enable_interrupt(hsgmii_index);
			//set phy an auto
			xsi_set_xsgmii_serdes_speed_forcemode_anauto(hsgmii_index, WAN_LINK_MODE);
			break;
		case ECNT_EVENT_SERDES_PHY_B:
			//xsgmii_api(1,0,7,1,1)
			xsi_set_xsgmii_serdes_enable_interrupt(hsgmii_index);
			//set phy an disable
			xsi_set_xsgmii_serdes_speed_forcemode_andisable(hsgmii_index, WAN_LINK_MODE);
			break;
		case ECNT_EVENT_SERDES_PHY_C:
			//set phy an auto
			xsi_set_xsgmii_serdes_speed_forcemode_anauto(hsgmii_index, WAN_LINK_MODE);
			break;
		case ECNT_EVENT_SERDES_PHY_D:
			//xsgmii_api(1,0,7,1,1)
			xsi_set_xsgmii_serdes_enable_interrupt(hsgmii_index);
			//set phy an auto
			xsi_set_xsgmii_serdes_speed_forcemode_anauto(hsgmii_index, WAN_LINK_MODE);
			break;
		case ECNT_EVENT_SERDES_PHY_E:
			xsi_set_xsgmii_serdes_speed_forcemode_anauto(hsgmii_index, WAN_LINK_MODE);
			break;
		case ECNT_EVENT_SERDES_PHY_DSL:
			xsi_set_xsgmii_serdes_speed_forcemode_anon(hsgmii_index, SGMII_1000M);
#if defined(TCSUPPORT_CPU_AN7583)
			xsgmii_api(2,0,253,0,1);
			xsgmii_api(2,2,0,0,1);
			/*set IPG=10*/
			xsi_mac_set_ipg(XSI_IDX_SEL, 10);
			/*enable tx/rx FC*/
			xsiSetTxFcEnable(XSI_IDX_SEL);
			xsiSetRxFcEnable(XSI_IDX_SEL);
			xsgmii_api(2,0,253,0,3);
#endif
			break;
		case ECNT_EVENT_SERDES_PHY_F:/*usxgmii*/
			//xsgmii_api(2,0,252,11,1)
			printk("pon serdes interrupt\n");
			xsgmii_api(2,0,7,1,1);
			xsgmii_api(2,0,7,1,5);
			xsi_set_xsgmii_serdes_aeonsemi_phy_preset(hsgmii_index);
			xsi_set_xsgmii_serdes_speed_forcemode_anauto(hsgmii_index, WAN_LINK_MODE);
#if SUPPORT_AS21xx
			AS21XX_AN_RESTART();
#endif
			break;
		default:
			printk("use default setting, force mode an disable, phy_sel=%d\n",phy_sel);
			xsi_set_xsgmii_serdes_speed_forcemode_andisable(hsgmii_index, WAN_LINK_MODE);
			break;

	}
#endif
	return 0;
}

static int ae_wan_qdma_init(void)
{
	QDMA_InitCfg_t qdmaInitCfg ;
			
	memset(&qdmaInitCfg, 0, sizeof(QDMA_InitCfg_t)) ;
    qdmaInitCfg.cbRecvPkts = ae_wan_mac_rx ;
	qdmaInitCfg.cbEventHandler = ae_wan_event_handler ;
	QDMA_API_INIT(ECNT_QDMA_WAN, &qdmaInitCfg);
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN, QDMA_ENABLE);
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_WAN, QDMA_ENABLE);
	QDMA_API_LOOPBACK_MODE(ECNT_QDMA_WAN, QDMA_LOOPBACK_DISABLE) ;

	ae_wan_qdma_wan_fe_cfg();

	return 0 ;
}

__IMEM static int ae_wan_napiPoll(struct napi_struct *napi, int budget)
{
	int n;
	unsigned long flags;

	/* call eth_rx to receive a packet 
	 * until all packets are received */
	n = QDMA_API_RECEIVE_PACKETS(ECNT_QDMA_WAN, budget);

	if (n < budget)
	{
		spin_lock_irqsave(&ae_wan_napilock, flags);
	
		__napi_complete(napi);
		QDMA_API_ENABLE_RXPKT_INT(ECNT_QDMA_WAN);

		spin_unlock_irqrestore(&ae_wan_napilock, flags);
	}
	
	return n;

}


static int ae_wan_dev_register(void)
{
	struct net_device *dev;
	dev = alloc_netdev(sizeof(xsiWanAdapter_t), ae_wan_itf, NET_NAME_USER, ether_setup);
#if defined(CONFIG_TP_IMAGE)
	hsgmii_pon_itf_origin = hsgmii_pon_itf;
#endif
	hsgmii_pon_itf = ae_wan_itf;
  
	if (!dev){
        XSI_MSG(XSI_DBG_ERR,"alloc_netdev fail!\n");
		return -ENOMEM;
    }
		
	ae_wan_dev = dev;
	ae_wan_p = netdev_priv(dev);

    dev->netdev_ops = &ae_wan_netdev_ops;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    ae_wan_p->napi.weight = 256;

    netif_napi_add(dev, &ae_wan_p->napi, ae_wan_napiPoll, 256);
#endif

	XSI_MSG(XSI_DBG_MSG,"xsi_dev = %p\n",ae_wan_dev);
	return register_netdev(dev);

}


static irqreturn_t ae_wan_mac_isr(int irq, void *dev_id)
{
	printk("Detect ae_wan mac eee interrupt.\n");
	return IRQ_HANDLED;
}

static int ae_wan_mac_irq_init(void)
{
	struct device *dev = NULL;
	
	if((dev=get_xsi_dev()) == NULL){
		printk("Get device failed.\n") ; 
		return 0 ;
	}
	
	if(request_irq(get_xsi_mac_eee_irq(1), ae_wan_mac_isr, 0, "ae_wan_mac_isr", dev) != 0) {
		printk("Request the XSI interrupt service routine fail, irq:%d.\n", get_xsi_mac_eee_irq(1)) ;
		return -ENODEV ;
	}

	return 0;
}

int ae_wan_mac_init(void)
{
	uint32 i = 0;
    printk("ae wan mac driver init \n");
	
	if (xsiMacInitialized)
		return 0;
	
	//gdm2 rx drop
	FE_API_SET_GDMA_DISABLE(XSI_IDX_SEL);
	/*setting xsi phy*/
#if SUPPORT_AE_WAN_NEW_ARCH
	//xsi_mac_api_set_xsgmii_phy_speed_forcemode_andisable(XSI_IDX_SEL,WAN_LINK_MODE);
	//serdes_phy_init(XSI_IDX_SEL);
	xsi_mac_itf_reset(XSI_IDX_SEL);

	ae_wan_p->statisticOn = XSI_MAC_STATISTIC_ON;
	
	for ( i = 0; i < 6; i++ )
		ae_wan_p->macAddr[i] = def_ae_wan_mac_addr[i];

	if(wan_sel == ENUM_PON_SERDES_ASWAN)
	{
		FE_API_SET_CRC_STRIP(PON_SEL_GDMA, FE_ENABLE);
		FE_API_SET_PACKET_LENGTH(PON_SEL_GDMA, 0x7d0, 0x3c);
	}else if(wan_sel == ENUM_ETH_SERDES_ASWAN){
		FE_API_SET_CRC_STRIP(ETH_SEL_GDMA, FE_ENABLE);
		FE_API_SET_PACKET_LENGTH(ETH_SEL_GDMA, 0x7d0, 0x3c);
	}else if(wan_sel == ENUM_USB_SERDES_ASWAN){
		FE_API_SET_CRC_STRIP(USB_SEL_GDMA, FE_ENABLE);
		FE_API_SET_PACKET_LENGTH(USB_SEL_GDMA, 0x7d0, 0x3c);
	}else if(wan_sel == ENUM_PCIE0_SERDES_ASWAN){
		FE_API_SET_CRC_STRIP(PCIE0_SEL_GDMA, FE_ENABLE);
		FE_API_SET_PACKET_LENGTH(PCIE0_SEL_GDMA, 0x7d0, 0x3c);
	}else if(wan_sel == ENUM_PCIE1_SERDES_ASWAN){
		FE_API_SET_CRC_STRIP(PCIE1_SEL_GDMA, FE_ENABLE);
		FE_API_SET_PACKET_LENGTH(PCIE1_SEL_GDMA, 0x7d0, 0x3c);
	}else{
		printk("ae wan mac driver init fail, wan_sel %u error \n", wan_sel);
		return 0;
	}
	
	//fe protect
	xsi_mac_set_mpi_mbi_disable(XSI_IDX_SEL);
	mdelay(1);
	//mac reset
	xsi_mac_api_logic_reset(XSI_IDX_SEL);
	//mac protect
	xsi_mac_set_mpi_mbi_disable(XSI_IDX_SEL);
	mdelay(1);
	xsi_mac_set_ipg(XSI_IDX_SEL, 10);

	//E1 disable tx/rx frag & use fe frag
	if(FC_FRAG_ERR || (XSI_AE_IDX == XSI_IDX_SEL)){
		xsiSetTxFragLen(XSI_IDX_SEL,31);
		xsi_mac_api_set_tx_frag(XSI_IDX_SEL);
	}
	else{
		xsi_mac_set_tx_frag_len(XSI_IDX_SEL,4);
	}
	if(SERDES_EOF || (XSI_AE_IDX == XSI_IDX_SEL)){
		xsiSetRxFragLen(XSI_IDX_SEL,31);
		xsi_mac_api_set_rx_frag(XSI_IDX_SEL);
	}
	else{
		xsi_mac_set_rx_frag_len(XSI_IDX_SEL,4);
	}
	
	xsi_mac_set_mpi_mbi_enable(XSI_IDX_SEL);
	
	ae_wan_qdma_init();

#else
	sgmii_api_force_mode(XSI_IDX_SEL,WAN_LINK_MODE);

	xsi_mac_itf_reset(XSI_IDX_SEL);

	ae_wan_p->statisticOn = XSI_MAC_STATISTIC_ON;
	
	for ( i = 0; i < 6; i++ )
		ae_wan_p->macAddr[i] = def_ae_wan_mac_addr[i];

	if(wan_sel == ENUM_USB_SERDES_ASWAN ||wan_sel == ENUM_PCIE0_SERDES_ASWAN || wan_sel == ENUM_PCIE1_SERDES_ASWAN)
	{
		FE_API_SET_CRC_STRIP(FE_GDM_SEL_GDMA3, FE_ENABLE);
		FE_API_SET_PACKET_LENGTH(FE_GDM_SEL_GDMA3, 0x7d0, 0x3c);
	}
	else{
	    FE_API_SET_CRC_STRIP(FE_GDM_SEL_GDMA2, FE_ENABLE);
	    FE_API_SET_PACKET_LENGTH(FE_GDM_SEL_GDMA2, 0x7d0, 0x3c);
	}
	xsi_mac_set_ipg(XSI_IDX_SEL, 10);
    ae_wan_qdma_init();
#endif
	
    xsiSetTxFcEnable(XSI_IDX_SEL);
	xsiSetRxFcEnable(XSI_IDX_SEL);
	
	xsiMacInitialized = 1;
	printk("ae wan mac driver init OK  \n");
	return 0;
}

static int __init ae_wan_mac_module_init(void)
{

    int err = 0;
#ifdef TCSUPPORT_BOARD_SELECT
	ECNT_EVENT_SYSTEM_SERDES_SEL_t serdes_port_count = get_serdes_port_count();
#else
	ECNT_EVENT_SYSTEM_SERDES_SEL_t serdes_port_count = ECNT_EVENT_SERDES_SEL_MAX;
#endif
	#if SUPPORT_AE_WAN_NEW_ARCH
	//in 7581 ,we can get  serdes work as wan or lan,so wan_sel no loonger need to set

    #if SUPPORT_SERDES_PON
	if(PON_LAN_VALID ){
		ae_wan_itf=aewan_lan_itf;
	}
	if(PON_VALID)
	{
		if(WAN_LINK_MODE == USXGII_10G)
		{
			if(get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_PON) ==ECNT_EVENT_SERDES_PON_HSGMII)
			{
				WAN_LINK_MODE = HSGMII_2p5G;
			}
			else if((get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_PON) ==ECNT_EVENT_SERDES_PON_XFI) || (get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_PON) ==ECNT_EVENT_SERDES_PON_USXGMII))
			{
				WAN_LINK_MODE = USXGII_10G;
			}
			else
			{
				printk("not hsgmii/usxgmii/xfi mode!\n");
				return 0;
			}
		}
		wan_sel = ENUM_PON_SERDES_ASWAN;
        XSI_IDX_SEL = XSI_AE_IDX;
        XSI_IDX_CHECK = ECNT_EVENT_SERDES_SEL_PON;
		XSI_RATE_IDX = ENUM_XPON_SERDES;
	}
    #endif
    #if SUPPORT_SERDES_USB
	if (USB_VALID)
	{
		if(WAN_LINK_MODE == USXGII_10G)
		{
			if(get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_USB1) ==ECNT_EVENT_SERDES_USB1_HSGMII)
			{
				WAN_LINK_MODE = HSGMII_2p5G;
			}
			else
			{
				printk("not hsgmii mode!\n");
				return 0;
			}
		}
		wan_sel = ENUM_USB_SERDES_ASWAN;
        XSI_IDX_SEL = XSI_USB_IDX;
        XSI_IDX_CHECK = ECNT_EVENT_SERDES_SEL_USB1;
		XSI_RATE_IDX = ENUM_USB_SERDES;
	}
    #endif
    #if SUPPORT_SERDES_PCIE0
	if (PCIE0_VALID)
	{
		if(WAN_LINK_MODE == USXGII_10G)
		{
			if(get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_WIFI1) ==ECNT_EVENT_SERDES_WIFI1_HSGMII)
			{
				WAN_LINK_MODE = HSGMII_2p5G;
			}
			else if((get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_WIFI1) ==ECNT_EVENT_SERDES_WIFI1_XFI) || (get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_WIFI1) ==ECNT_EVENT_SERDES_WIFI1_USXGMII))
			{
				WAN_LINK_MODE = USXGII_10G;
			}
			else
			{
				printk("not hsgmii/usxgmii/xfi mode!\n");
				return 0;
			}
		}
		wan_sel = ENUM_PCIE0_SERDES_ASWAN;
        XSI_IDX_SEL = XSI_PCIE0_IDX;
        XSI_IDX_CHECK = ECNT_EVENT_SERDES_SEL_WIFI1;
		XSI_RATE_IDX = ENUM_PCIE0_SERDES;
	}
    #endif
    #if SUPPORT_SERDES_PCIE1
	if (PCIE1_VALID)
	{
		if(WAN_LINK_MODE == USXGII_10G)
		{
			if(get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_WIFI2) ==ECNT_EVENT_SERDES_WIFI2_HSGMII)
			{
				WAN_LINK_MODE = HSGMII_2p5G;
			}
			else if((get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_WIFI2) ==ECNT_EVENT_SERDES_WIFI2_XFI) || (get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_WIFI2) ==ECNT_EVENT_SERDES_WIFI2_USXGMII))
			{
				WAN_LINK_MODE = USXGII_10G;
			}
			else
			{
				printk("not hsgmii/usxgmii/xfi mode!\n");
				return 0;
			}
		}
		wan_sel = ENUM_PCIE1_SERDES_ASWAN;
        XSI_IDX_SEL = XSI_PCIE1_IDX;
        XSI_IDX_CHECK = ECNT_EVENT_SERDES_SEL_WIFI2;
		XSI_RATE_IDX = ENUM_PCIE1_SERDES;
	}
    #endif
    #if SUPPORT_SERDES_ETHER
	if (ETH_VALID)
	{
		if(WAN_LINK_MODE == USXGII_10G)
		{
			if(get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_ETHER) ==ECNT_EVENT_SERDES_ETHER_HSGMII)
			{
				WAN_LINK_MODE = HSGMII_2p5G;
			}
			else if((get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_ETHER) ==ECNT_EVENT_SERDES_ETHER_XFI) || (get_serdes_interface_sel(ECNT_EVENT_SERDES_SEL_ETHER) ==ECNT_EVENT_SERDES_ETHER_USXGMII))
			{
				WAN_LINK_MODE = USXGII_10G;
			}
			else
			{
				printk("not hsgmii/usxgmii/xfi mode!\n");
				return 0;
			}
		}
		wan_sel = ENUM_ETH_SERDES_ASWAN;
        XSI_IDX_SEL = XSI_ETH_IDX;
        XSI_IDX_CHECK = ECNT_EVENT_PCIE1_HSGMII;
		XSI_RATE_IDX = ENUM_ETH_SERDES;
	}
    #endif

    if(ENUM_UNKNOW_SERDES_ASWAN == wan_sel)
    {
		printk("no serdes set as wan in bootcfg,set bootcfg first\n");
        XSI_IDX_CHECK = serdes_port_count;
		return 0;
    }

	if (WAN_LINK_MODE >= SPEED_UNKNOW)
	{
		printk("\nWrong LINK_MODE:%d, insmod xsi.ko Fail(0:USXGII_10G, 1:USXGII_5G,2:USXGII_2p5G,3:USXGII_1G,4:USXGII_100M,5:HSGMII_2p5G,6:SGMII_1000M,7:SGMII_100M,8:SGMII_10M)\n", WAN_LINK_MODE);
		return 0;
	}
	serdes_phy_init(XSI_IDX_SEL);
	ae_wan_ratelimit_init();
	#else

	switch(wan_sel){
		case ENUM_PON_SERDES_ASWAN:
			XSI_IDX_SEL = XSI_AE_IDX;
			XSI_IDX_CHECK = ECNT_EVENT_PON_HSGMII;
			break;
		case ENUM_USB_SERDES_ASWAN:
			XSI_IDX_SEL = XSI_USB_IDX;
			XSI_IDX_CHECK = ECNT_EVENT_USB3_HSGMII;
			break;
		case ENUM_PCIE0_SERDES_ASWAN:
			XSI_IDX_SEL = XSI_PCIE0_IDX;
#ifdef TCSUPPORT_BOARD_SELECT
			XSI_IDX_CHECK = ECNT_EVENT_SERDES_SEL_WIFI1;
#else
			XSI_IDX_CHECK = ECNT_EVENT_PCIE0_HSGMII;
#endif
			break;
		case ENUM_PCIE1_SERDES_ASWAN:
			XSI_IDX_SEL = XSI_PCIE1_IDX;
#ifdef TCSUPPORT_BOARD_SELECT
			XSI_IDX_CHECK = ECNT_EVENT_SERDES_SEL_WIFI2;
#else
			XSI_IDX_CHECK = ECNT_EVENT_PCIE1_HSGMII;
#endif
			break;
		default:
			XSI_IDX_CHECK = serdes_port_count;
			break;
	}

	if (WAN_LINK_MODE >= SGMII_SPEED_UNKNOW)
	{
		printk("\nWrong LINK_MODE:%d, insmod xsi.ko Fail(0:2.5g,1:1000M,2:100M,3:10M)\n", WAN_LINK_MODE);
		return 0;
	}
	
	if((((get_serdes_sel(XSI_IDX_CHECK) & (0x1 << XSI_IDX_CHECK)) >> XSI_IDX_CHECK)) != 1)
	{
		printk("\nWrong AE_WAN_MODE:%d, insmod ae_wan.ko Fail(0:PON,1:USB,2:PCIE0,3:PCIE1)\n", XSI_IDX_CHECK);
		return 0;
	}
	#endif
	
    err = ae_wan_dev_register();
    if(err)
        return err;

#ifdef CONFIG_TP_IMAGE
	/* initial state is off */
	//netif_carrier_off(ae_wan_dev);
#endif

    ae_wan_proc_init();

    if(ecnt_register_hook(&ecnt_driver_ae_wan_op)) {
		return -ENODEV ;
    }

	ae_wan_mac_init();
	
	if(SUPPORT_MAC_EEE)
	{
		ae_wan_mac_irq_init();
	}
    return 0;
}
#if defined(CONFIG_TP_IMAGE)
static void _recover_origin_pointer(void)
{
	if(hsgmii_pon_itf_origin != NULL)
		hsgmii_pon_itf = hsgmii_pon_itf_origin;
}
#endif

static void __exit ae_wan_mac_module_exit(void)
{
#if defined(CONFIG_TP_IMAGE)
	_recover_origin_pointer();
#endif	
	#if SUPPORT_AE_WAN_NEW_ARCH
	#if defined(TCSUPPORT_CPU_EN7581)
		// gdm2 drop packets
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_UC, FE_DP_DROP);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_BC, FE_DP_DROP);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_MC, FE_DP_DROP);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_OC, FE_DP_DROP);
	// gdm fwd
	QDMA_API_LOOPBACK_MODE(ECNT_QDMA_WAN, QDMA_LOOPBACK_DISABLE) ;
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_OC, 8);  
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_MC, 8);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_BC, 8);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA2, FE_TYPE_UC, 8);
		
	//gdm 4 fwd	
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA4, FE_TYPE_OC, 0);  
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA4, FE_TYPE_MC, 0);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA4, FE_TYPE_BC, 0);
	FE_API_SET_FW_CFG(FE_GDM_SEL_GDMA4, FE_TYPE_UC, 0);
	int reg_val = 0;
	// restoring ifc and vip
	reg_val = regRead32_for_fe(FE_VIP_PORT_EN);
	regWrite32_for_fe(FE_VIP_PORT_EN,reg_val);
	reg_val = regRead32_for_fe(FE_IFC_PORT_EN);
	regWrite32_for_fe(FE_IFC_PORT_EN,reg_val);

	switch(wan_sel){
			case ENUM_PON_SERDES_ASWAN:	
				break;
			case ENUM_USB_SERDES_ASWAN:
				FE_API_SET_WAN_PORT_7516(FE_ENABLE, 0, HSGMII_LAN_USB_SRCPORT);
				XSI_SET_SP_DFT_CPORT(HSGMII_LAN_USB_SRCPORT, DPORT_PPE);
				break;
			case ENUM_PCIE0_SERDES_ASWAN:
				FE_API_SET_WAN_PORT_7516(FE_ENABLE, 0, HSGMII_LAN_PCIE0_SRCPORT);
				XSI_SET_SP_DFT_CPORT(HSGMII_LAN_PCIE0_SRCPORT, DPORT_PPE);
				break;
			case ENUM_PCIE1_SERDES_ASWAN:
				FE_API_SET_WAN_PORT_7516(FE_ENABLE, 0, HSGMII_LAN_PCIE1_SRCPORT);
				XSI_SET_SP_DFT_CPORT(HSGMII_LAN_PCIE1_SRCPORT, DPORT_PPE);
				break;
			case ENUM_ETH_SERDES_ASWAN:
				FE_API_SET_WAN_PORT_7516(FE_ENABLE, 0, HSGMII_LAN_ETH_SRCPORT);
				XSI_SET_SP_DFT_CPORT(HSGMII_LAN_ETH_SRCPORT, DPORT_PPE);
				break;
			default:
				break;
						
	}
	
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN, QDMA_DISABLE); 
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_WAN, QDMA_DISABLE);
	QDMA_API_DEINIT(ECNT_QDMA_WAN);
	#endif
	ecnt_unregister_hook(&ecnt_driver_ae_wan_op);
	
	unregister_netdev(ae_wan_dev);
	free_netdev(ae_wan_dev);
	ae_wan_proc_exit();

	#else
	if ((WAN_LINK_MODE < SGMII_SPEED_UNKNOW) && (get_serdes_sel(XSI_IDX_CHECK) == 1))
	{
		ecnt_unregister_hook(&ecnt_driver_ae_wan_op);
		
		unregister_netdev(ae_wan_dev);
		free_netdev(ae_wan_dev);
		ae_wan_proc_exit();
	}
	return;
	#endif
}

module_init(ae_wan_mac_module_init);
module_exit(ae_wan_mac_module_exit);
module_param(ae_wan_itf, charp, S_IRUGO);
module_param(aewan_lan_itf, charp, S_IRUGO);
module_param(WAN_LINK_MODE, uint, 0);
module_param(wan_sel, uint, 0);

MODULE_LICENSE("Proprietary");

