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
#include "ae_wan_mac.h"
#include "ae_wan_api.h"
#include "ae_wan_proc.h"
#include <linux/proc_fs.h>


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
#define RX_BUF_LEN 	(2048 - NET_SKB_PAD - 64 - (sizeof(struct skb_shared_info)))
#define CHK_BUF() {pos = begin + index; if (pos < off) { index = 0; begin = pos; }; if (pos > off + count) goto done;}

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern struct net_device *ae_wan_dev;
extern unsigned int WAN_LINK_MODE;
extern unsigned int XSI_IDX_SEL;
#if defined(TCSUPPORT_SISM_HOST)
extern int inicState;
extern int ADSL_ETYPE_OFFSET;
#endif
extern u16 lan_vlan;
extern u16 wan_vlan;
extern u16 mgr_vlan;
extern unsigned int enAeWanInicFilter;
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
u8 xsi_dbg_level = 0;
int ae_wan_fast_mode_flag = 0;

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_MULTICAST_VLAN)
static struct multicastVlanInfo ae_wan_mcastVlanInfo;
#define ae_wan_multicastTci    (ae_wan_mcastVlanInfo.multicastTci)
#define ae_wan_iptvTci         (ae_wan_mcastVlanInfo.iptvTci)
#endif /* CONFIG_TP_IMAGE && INCLUDE_MULTICAST_VLAN */

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_MULTICAST_VLAN)
int (*ae_wan_multicastVlanTxHook)(struct sk_buff *skb) = NULL;
int (*ae_wan_multicastVlanRxHook)(struct sk_buff *skb) = NULL;
#endif /* CONFIG_TP_IMAGE && INCLUDE_MULTICAST_VLAN */


static int ae_wan_mac_dbg_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	off_t begin = 0;
	off_t pos = 0;
	
/*xsi cnt*/
    index += sprintf(buf+index, "\nXSI MAC debug cnt:\n");
    CHK_BUF();
    index += sprintf(buf+index, "TX_OCTETS_CNT\t\t\t= 0x%08lx, ", xsiGetTxBytes(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "TX_PKT_CNT\t\t\t= 0x%08lx\n", xsiGetTxPktCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "TXMBI_ETH_CNT\t\t\t= 0x%08lx, ", xsiGetTxMbiEthCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "TXMBI_UCETH_CNT\t\t\t= 0x%08lx\n", xsiGetTxMbiUcEthCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "TXMBI_MCETH_CNT\t\t\t= 0x%08lx, ", xsiGetTxMbiMcEthCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "TXMBI_BCETH_CNT\t\t\t= 0x%08lx\n", xsiGetTxMbiBcEthCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "TXMBI_PAUSEON_CNT\t\t= 0x%08lx, ", xsiGetTxMbiPauseOnCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "TXMBI_PAUSEOFF_CNT\t\t= 0x%08lx\n", xsiGetTxMbiPauseOffCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_OCTETS_CNT\t\t\t= 0x%08lx, ", xsiGetRxBytes(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_PKT_CNT\t\t\t= 0x%08lx\n", xsiGetRxPktCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_ETH_CNT\t\t\t= 0x%08lx, ", xsiGetRxEthCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RXMBI_ETH_CNT\t\t\t= 0x%08lx\n", xsiGetRxMbiPktCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_PAUSEON_CNT\t\t\t= 0x%08lx, ", xsiGetRxPauseOnCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_PAUSEOFF_CNT\t\t\t= 0x%08lx\n", xsiGetRxPauseOffCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_LENERR_CNT\t\t\t= 0x%08lx, ", xsiGetRxLenErrCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_FRAGERR_CNT\t\t\t= 0x%08lx\n", xsiGetRxFragErrCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_CRCERR_CNT\t\t\t= 0x%08lx, ", xsiGetRxCrcErrCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_CODINGERR_CNT\t\t\t= 0x%08lx\n", xsiGetRxCodingErrCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RXMBI_ERRDROP_CNT\t\t= 0x%08lx, ", xsiGetRxMbiErrDropCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RXMBI_SOFDROP_CNT\t\t\t= 0x%08lx\n", xsiGetRxMbiSofDropCnt(XSI_IDX_SEL));
    CHK_BUF();
    index += sprintf(buf+index, "RX_FRAME_CNT\t\t\t= 0x%08lx\n", xsiGetRxFrameCnt(XSI_IDX_SEL));
    CHK_BUF();
 
/*itf status*/
    index += sprintf(buf+index, "\nXSI MAC interface status:\n");
    CHK_BUF();
    index += sprintf(buf+index, "TX_MBI_ITF %s\n", (xsiGetTxmbiStopSts(XSI_IDX_SEL) ? "Disable" : "Enable"));
    CHK_BUF();
    index += sprintf(buf+index, "TX_MPI_ITF %s\n", (xsiGetTxmpiStopSts(XSI_IDX_SEL) ? "Disable" : "Enable"));
    CHK_BUF();
    index += sprintf(buf+index, "RX_MBI_ITF %s\n", (xsiGetRxmbiStopSts(XSI_IDX_SEL) ? "Disable" : "Enable"));
    CHK_BUF();
    index += sprintf(buf+index, "RX_MPI_ITF %s\n", (xsiGetRxmpiStopSts(XSI_IDX_SEL) ? "Disable" : "Enable"));
    CHK_BUF();
    index += sprintf(buf+index, "TX_MPI_MASK %s\n",(xsiIsTxmpiIdle(XSI_IDX_SEL) ? "Idle" : "Normal"));
    CHK_BUF();

/*FC enable/disable status*/
    index += sprintf(buf+index, "\nXSI MAC flow control status:\n");
    CHK_BUF();
	index += sprintf(buf+index, "Tx FC %s\n",(xsiIsTxFcOn(XSI_IDX_SEL) ? "Enable" : "Disable"));
    CHK_BUF();
    index += sprintf(buf+index, "Rx FC %s\n",(xsiIsRxFcOn(XSI_IDX_SEL) ? "Enable" : "Disable"));
    CHK_BUF();

/*debug level dump*/
    index += sprintf(buf+index, "\nXsi dbg level = %d\n",xsi_dbg_level);
    CHK_BUF();

/*Xsi mode dump*/
    index += sprintf(buf+index, "\nXsi mode = %d\n",WAN_LINK_MODE);
    CHK_BUF();

	*eof = 1;

done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;

}
#ifdef CONFIG_TP_IMAGE
extern int is_ae_wan_en8811phy;
#endif
static int ae_wan_mac_dbg_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[32] = {0}, subcmd[32] = {0}, param[32] ={0};
	unsigned int value = 0, force_mode = 0, an_mode = 0 ;

	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	sscanf(val_string, "%s %s %d %d %d", subcmd, param, &value, &force_mode, &an_mode);
	
	XSI_MSG(XSI_DBG_ERR,"%s %s %d %d %d\n", subcmd, param, value, force_mode, an_mode);
	
	if(!strcmp(subcmd, "cnt")){
		if(!strcmp(param, "clear"))
			xsiClearAllCnt(XSI_IDX_SEL);
		else
			goto USAGE;
	}else if(!strcmp(subcmd, "itf")){	
		if(!strcmp(param, "tx_mbi")){
			if(value == 0)
				xsiSetTxmbiDisable(XSI_IDX_SEL);
			else if(value == 1)
				xsiSetTxmbiEnable(XSI_IDX_SEL);
			else
				goto USAGE;
		}else if(!strcmp(param, "tx_mpi")){
			if(value == 0)
				xsiSetTxmpiDisable(XSI_IDX_SEL);
			else if(value == 1)
				xsiSetTxmpiEnable(XSI_IDX_SEL);
			else
				goto USAGE;
		}else if(!strcmp(param, "rx_mbi")){
			if(value == 0)
				xsiSetRxmbiDisable(XSI_IDX_SEL);
			else if(value == 1)
				xsiSetRxmbiEnable(XSI_IDX_SEL);
			else
				goto USAGE;
		}else if(!strcmp(param, "rx_mpi")){		
			if(value == 0)
				xsiSetRxmpiDisable(XSI_IDX_SEL);
			else if(value == 1)
				xsiSetRxmpiEnable(XSI_IDX_SEL);
			else
				goto USAGE;
		}else if(!strcmp(param, "tx_mpi_idle")){
			if(value == 0)
				xsiSetTxmpiNormal(XSI_IDX_SEL);
			else if(value == 1)
				xsiSetTxmpiIdle(XSI_IDX_SEL);
			else
				goto USAGE;
#ifdef CONFIG_TP_IMAGE
		}else if (!strcmp(param, "carrier")) {
			if (value == 0)
				netif_carrier_off(ae_wan_dev);
			else if (value == 1)
				netif_carrier_on(ae_wan_dev);
		}else if (!strcmp(param, "8811phy")) {
			is_ae_wan_en8811phy = value;
		}else if (!strcmp(param, "serdes")) {
			if (1 == value) {
				xsi_set_xsgmii_serdes_speed_forcemode_andisable(3, HSGMII_2p5G);
			}
			else if (7 == value) {
				xsi_set_xsgmii_serdes_speed_forcemode_anon(3, SGMII_1000M);
			}
#endif /* CONFIG_TP_IMAGE */
		}
	}
	else if(!strcmp(subcmd, "fc")){
		if(!strcmp(param, "tx")){
			if(value == 0)
				xsiSetTxFcDisable(XSI_IDX_SEL);
			else if(value == 1)
				xsiSetTxFcEnable(XSI_IDX_SEL);
			else
				goto USAGE;
		}else if(!strcmp(param, "rx")){
			if(value == 0)
				xsiSetRxFcDisable(XSI_IDX_SEL);
			else if(value == 1)
				xsiSetRxFcEnable(XSI_IDX_SEL);
			else
				goto USAGE;
		}
	}else if(!strcmp(subcmd, "debug")){
		if(!strcmp(param, "level")){
			if(value>=0 && value<=3)
				xsi_dbg_level = value;
			else
				goto USAGE;
		}else
			goto USAGE;
	}else if(!strcmp(subcmd, "fragment")){
		if(!strcmp(param, "tx")){
			xsi_mac_set_tx_frag_len(XSI_IDX_SEL, value);
		}else if(!strcmp(param, "rx")){
			xsi_mac_set_rx_frag_len(XSI_IDX_SEL, value);
		}
		else
			goto USAGE;
	}else if(!strcmp(subcmd, "lpbk")){
		if(!strcmp(param, "enable")){
			xsi_mac_set_lpbk_enable(XSI_IDX_SEL, value);
		}
		else
			goto USAGE;
	}else if(!strcmp(subcmd, "ipg")){
		if(!strcmp(param, "config")){
			xsi_mac_set_ipg(XSI_IDX_SEL, value);
		}
		else
			goto USAGE;
	}else if(!strcmp(subcmd, "lpi")){
		if(!strcmp(param, "set")){
			xsi_mac_set_lpi(XSI_IDX_SEL, value, force_mode);
		}
		else if(!strcmp(param, "get")){
			xsi_mac_get_lpi(XSI_IDX_SEL, force_mode);
		}
		else
			goto USAGE;
	}else if(!strcmp(subcmd, "phy")){
#if SUPPORT_AE_WAN_NEW_ARCH
		if(!strcmp(param, "up")){
			xsi_mac_api_set_xsgmii_phy_linkstats(XSI_IDX_SEL, 1);
		}
		else if(!strcmp(param, "down")){
			xsi_mac_api_set_xsgmii_phy_linkstats(XSI_IDX_SEL, 0);
		}
		else
			goto USAGE;
#endif
	}else if(!strcmp(subcmd, "change")){
		if(!strcmp(param, "mode")){
#if SUPPORT_AE_WAN_NEW_ARCH
			if(value>=USXGII_10G && value<SPEED_UNKNOW){
				WAN_LINK_MODE = value;
				xsi_mac_api_set_xsgmii_phy_speed(XSI_IDX_SEL,WAN_LINK_MODE, force_mode, an_mode);
			}else
				goto USAGE;
#else
			if(value>=SGMII_SPEED_2500M && value<SGMII_SPEED_UNKNOW){
				WAN_LINK_MODE = value;
				sgmii_api_force_mode(XSI_IDX_SEL,WAN_LINK_MODE);
			}else
				goto USAGE;
#endif
		}else
			goto USAGE;
	}else
		goto USAGE;
	
	return count ;
	
USAGE:
	XSI_MSG(XSI_DBG_ERR,"echo cnt clear > proc/tc3162/ae_wan_mac_dbg \n");
	XSI_MSG(XSI_DBG_ERR,"echo itf [tx_mbi|tx_mpi|rx_mbi|rx_mpi|tx_mpi_idle] [0|1] > proc/tc3162/ae_wan_mac_dbg \n");
	XSI_MSG(XSI_DBG_ERR,"echo fc [tx|rx] [0|1] > proc/tc3162/ae_wan_mac_dbg \n");
	XSI_MSG(XSI_DBG_ERR,"echo debug level [0|1|2|3] > proc/tc3162/ae_wan_mac_dbg \n");
	XSI_MSG(XSI_DBG_ERR,"echo fragment [tx|rx] [value] > proc/tc3162/ae_wan_mac_dbg \n");	
	XSI_MSG(XSI_DBG_ERR,"echo lpbk enable [0|1] > proc/tc3162/ae_wan_mac_dbg \n");	
	XSI_MSG(XSI_DBG_ERR,"echo ipg config [value] > proc/tc3162/ae_wan_mac_dbg \n");
	XSI_MSG(XSI_DBG_ERR,"echo lpi [get|set] [value] dir[0:TX|1:RX] > proc/tc3162/ae_wan_mac_dbg \n");
#if SUPPORT_AE_WAN_NEW_ARCH
	XSI_MSG(XSI_DBG_ERR,"echo change mode [0|1|2|3|4|5] [force 0|1] [an 0|1] > proc/tc3162/ae_wan_mac_dbg \n \
							Mode = 0, USXGII_10G.\n\
							Mode = 1, USXGII_5G.\n\
							Mode = 2, USXGII_2p5G.\n\
							Mode = 3, USXGII_1G.\n\
							Mode = 4, USXGII_100M.\n\
							Mode = 5, HSGMII_2p5G.\n\
							Mode = 6, SGMII_1000M.\n\
							Mode = 8, SGMII_100M.\n\
							Mode = 7, SGMII_10M.\n");

#else
	XSI_MSG(XSI_DBG_ERR,"echo change mode [0|1|2|3|4|5] > proc/tc3162/ae_wan_mac_dbg \n \
						Mode = 0, PHY_XFI_10G_CONFIG.\n\
						Mode = 1, PHY_XFI_5G_CONFIG.\n\
						Mode = 2, PHY_XFI_2P5G_CONFIG.\n\
						Mode = 3, PHY_HSGMII_5G_CONFIG.\n\
						Mode = 4, PHY_HSGMII_2P5G_CONFIG.\n\
						Mode = 5, PHY_HSGMII_1G_CONFIG.\n");
#endif
	return count ;
}

/******************************************************************************
******************************************************************************/
static int ae_wan_fast_slow_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	if (ae_wan_fast_mode_flag) {
		printk("ae_wan_fast_mode_flag value is %d, hsgmii down stream fast mode enable\n", ae_wan_fast_mode_flag);
	} else {
		printk("ae_wan_fast_mode_flag value is %d, hsgmii down stream fast mode disable\n", ae_wan_fast_mode_flag);
	}
	return 0;
}

static int ae_wan_fast_slow_write_proc(struct file *file, const char *buffer,unsigned long count, void *data)	
{
	char val_string[96] = {0}, cmd[32]={0}, subcmd[32]={0};
	// int value = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	// sscanf(val_string, "%d", &value);
	sscanf(val_string, "%31s %31s", cmd, subcmd) ;
	val_string[count] = '\0';
	if(!strcmp(cmd, "fast")){
		if(!strcmp(subcmd, "enable") || !strcmp(subcmd, "1")) {
			ae_wan_fast_mode_flag = 1;
		} else {
			ae_wan_fast_mode_flag = 0;
		}
	} else{
	
	}
	
	// printk("fast_mode value is written %d\n", fast_mode);
	return count;
}


/******************************************************************************
******************************************************************************/

static int ae_wan_link_status_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int index = 0;
    off_t begin = 0;
    off_t pos = 0;
	int sync = 0, an = 0, dump = 0, argc=3;
	int *argv[] = {&sync, &an, &dump};
	unsigned int link_stats = 0;

	#if SUPPORT_AE_WAN_NEW_ARCH
	link_stats = xsi_mac_api_get_xsgmii_phy_linkstats(XSI_IDX_SEL);
	sync = link_stats & (0x1);
	#else
	sgmii_api_get_info(XSI_IDX_SEL, &argc, argv);
	#endif


    index += sprintf(buf+index, "ae_wan_phy_link_status = %s\n", sync==1?"up":"down");
    CHK_BUF();
    index += sprintf(buf+index, "ae_wan_phy_mode = %d\n", WAN_LINK_MODE);
    CHK_BUF();


    *eof = 1;

done:
   *start = buf + (off - begin);
   index -= (off - begin);
   if (index<0) 
       index = 0;
   if (index>count) 
       index = count;
   return index;

}


static int hsgmii_ratelimit_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	return 0;			
}

static int hsgmii_ratelimit_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int gdm_rc_cfg_id = -1;
	uint8_t byte_mode = 0;
	uint8_t val_string[64] = {0};
	unsigned int ratelimit_value = 0;
	ECNT_AE_WAN_Data_s xsi_data ={0};

	if (count > (sizeof(val_string) - 1))
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;
	
	sscanf(val_string, "%d %d %d %d", &xsi_data.hsgmii_index,&gdm_rc_cfg_id, &xsi_data.xsi_private.rate_cfg.rate,&xsi_data.xsi_private.rate_cfg.mode);
	xsi_data.hsgmii_index = XSI_IDX_SEL;
	printk("set gdm_sel= %d, gdm_rc_cfg_id=%d, rateLimit_value=%d, byte_mode=%d\n", xsi_data.hsgmii_index,gdm_rc_cfg_id, 
		xsi_data.xsi_private.rate_cfg.rate, xsi_data.xsi_private.rate_cfg.mode);
	
	if (HSGMII_RX_UC_RATE == gdm_rc_cfg_id)
	{
		xsi_mac_api_set_rxuc_rate(&xsi_data);
	}
	else if (HSGMII_RX_BC_RATE == gdm_rc_cfg_id)
	{
		xsi_mac_api_set_rxbc_rate(&xsi_data);
	}
	else if (HSGMII_RX_MC_RATE == gdm_rc_cfg_id)
	{
		xsi_mac_api_set_rxmc_rate(&xsi_data);
	}
	else if (HSGMII_RX_TOTAL_RATE == gdm_rc_cfg_id)
	{
		xsi_mac_api_set_rxtotal_rate(&xsi_data);
	}
	else if (HSGMII_TX_TOTAL_RATE == gdm_rc_cfg_id)
	{
		xsi_mac_api_set_txtotal_rate(&xsi_data);
	}
	else
	{
		printk("error gdm_rc_cfg_id, return. \n");
		return -EINVAL ;
	}
	
	return count;
}

static int transform_vlan_params_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	off_t begin = 0;
	off_t pos = 0;

	index += sprintf(buf+index, "lan_vlan      : %d\n", lan_vlan);
	CHK_BUF();
	index += sprintf(buf+index, "wan_vlan      : %d\n", wan_vlan);
	CHK_BUF();	
	index += sprintf(buf+index, "usage: echo <lan_vlan> <wan_vlan> >/proc/tc3162/transform_vlan\n");
	CHK_BUF();
	*eof = 1;
done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}
static int transform_vlan_params_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[128] = {0}, subcmd[32] = {0}, param[32] ={0};
	u16 vlan1;
	u16 vlan2;

	if (count > sizeof(val_string) - 1)
		return -EINVAL ;
	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;
    sscanf(val_string, "%d %d", &vlan1, &vlan2);
	XSI_MSG(XSI_DBG_ERR,"%d %d\n", vlan1, vlan2);

	lan_vlan = vlan1;
	wan_vlan = vlan2;

    return count;
}
static int manage_vlan_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index = 0;
	off_t begin = 0;
	off_t pos = 0;

	index += sprintf(buf+index, "mgr_vlan      : %d\n", mgr_vlan);
	CHK_BUF();
	index += sprintf(buf+index, "usage: echo <mgr_vlan> >/proc/tc3162/mgr_vlan\n");
	CHK_BUF();
	*eof = 1;
done:
	*start = buf + (off - begin);
	index -= (off - begin);
	if (index<0) 
		index = 0;
	if (index>count) 
		index = count;
	return index;
}
static int manage_vlan_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[128], subcmd[32] = {0}, param[32] ={0};
	u16 manage_vlan;

	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

    sscanf(val_string, "%d", &manage_vlan);
	XSI_MSG(XSI_DBG_ERR," %d\n", manage_vlan);
	
	mgr_vlan = manage_vlan;
	
    return count;
}

#if defined(TCSUPPORT_SISM_HOST)
/*Avoid packet transmit to EN7517 when EN7517 is booting*/
static int ae_wan_inic_state_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char valString[4];
	int val = 0;

	if (count > sizeof(valString) - 1)
		return -EINVAL;

	if (copy_from_user(valString, buffer, count))
		return -EINVAL;

	valString[count] = '\0';

	sscanf(valString, "%d", &val);

	if ((val != BOOTING_MODE ) && (val!= NORMAL_MODE))
	{
		printk("%s: wrong input. ",__func__);
		return -EINVAL;
	}
	inicState = val;
	printk("%s: mode changes to %d\n", __func__, inicState);

	return count;
}

static int ae_wan_inic_filter_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char valString[12];

	if (count > sizeof(valString) - 1)
		return -EINVAL;

	if (copy_from_user(valString, buffer, count))
		return -EINVAL;

	valString[count] = '\0';

	if(!strncmp("enable", valString, strlen("enable")))
		enAeWanInicFilter = 1;
	else
		enAeWanInicFilter = 0;

	printk("%s: inic filter enable = %d\n", __func__, enAeWanInicFilter);

	return count;
}

static int ae_wan_adsl_mpoa_head_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	printk("ADSL_ETYPE_OFFSET %d\n", ADSL_ETYPE_OFFSET);
	return 0;
}

static int ae_wan_adsl_mpoa_head_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int mode = 0;
	uint8_t val_string[8] = {0};

	if (count > (sizeof(val_string) - 1))
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	sscanf(val_string, "%d", &mode);

	if(mode==1)
		ADSL_ETYPE_OFFSET = 38;
	else if(mode==0)
		ADSL_ETYPE_OFFSET = 16;

	return count;
}

#endif

#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_MULTICAST_VLAN)
int ae_wan_multicastVlanReplaceTxTag(struct sk_buff *skb)
{
	struct vlanHdr *vlanHdr = NULL;
	unsigned int offset = 2 * ETH_ALEN;

	if (NULL == skb)
	{
		return 0;
	}

	vlanHdr = (struct vlanHdr *)(skb->data + offset);

	if ((0x01 == skb->data[0]) && (0x00 == skb->data[1]) && (0x5E == skb->data[2]) /* multicast dst MAC */
		&& (0x8100 == ntohs(vlanHdr->tpid)) && (ae_wan_iptvTci == ntohs(vlanHdr->tci)))
	{
		vlanHdr->tci = htons(ae_wan_multicastTci); /* replace iptv tag with multicast tag */
	}

	return 0;
}

int ae_wan_multicastVlanReplaceRxTag(struct sk_buff *skb)
{
	struct vlanHdr *vlanHdr = NULL;
	unsigned int offset = 2 * ETH_ALEN;

	if (NULL == skb)
	{
		return 0;
	}

	vlanHdr = (struct vlanHdr *)(skb->data + offset);

	if ((0x8100 == ntohs(vlanHdr->tpid)) && (0 == ((ntohs(vlanHdr->tci) ^ ae_wan_multicastTci) & 0xFFF)))
	{
		vlanHdr->tci = htons(ae_wan_iptvTci); /* replace multicast tag with iptv tag */
	}

	return 0;
}


static int ae_wan_iptv_multicast_read_proc(char *buf, char **start, off_t off, int count,
				 int *eof, void *data)
{
	printk("iptvVid:%d,multicastVid:%d\n", ae_wan_iptvTci, ae_wan_multicastTci);
	return 0;
}

static int ae_wan_iptv_multicast_write_proc(struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	char val_string[32] = {0};
	int iptvVid = 0;
	int multicastVid = 0;

	if (NULL == buffer || count > sizeof(val_string) - 1)
	{
		printk("ae_wan_iptv_multicast:Invalid param\n");
		return -EINVAL;
	}

	if (copy_from_user(val_string, buffer, count))
	{
		printk("ae_wan_iptv_multicast:copy_from_user error\n");
		return -EFAULT;
	}

	val_string[count] = '\0';

	if (2 != sscanf(val_string, "%d %d", &iptvVid, &multicastVid)){
		printk("ae_wan_iptv_multicast:Invalid param\n");
		return -1;
	}

	ae_wan_multicastTci = multicastVid;
	ae_wan_iptvTci = iptvVid;

	if (0 != (ae_wan_multicastTci & 0xFFF))
	{
		ae_wan_multicastVlanTxHook = ae_wan_multicastVlanReplaceTxTag;
		ae_wan_multicastVlanRxHook = ae_wan_multicastVlanReplaceRxTag;
	}
	else
	{
		ae_wan_multicastVlanTxHook = NULL;
		ae_wan_multicastVlanRxHook = NULL;
	}

	return count;
}
#endif /* CONFIG_TP_IMAGE && INCLUDE_MULTICAST_VLAN */



int ae_wan_proc_init(void)
{
    struct proc_dir_entry *xsi_proc;

	xsi_proc = create_proc_entry("tc3162/ae_wan_mac_dbg", 0, NULL);
	xsi_proc->read_proc = ae_wan_mac_dbg_read_proc;
	xsi_proc->write_proc = ae_wan_mac_dbg_write_proc;  

	xsi_proc = create_proc_entry("tc3162/ae_wan_link_status", 0, NULL);
	xsi_proc->read_proc = ae_wan_link_status_read_proc;
	xsi_proc->write_proc = NULL;  

#if defined(TCSUPPORT_SISM_HOST)
	xsi_proc = create_proc_entry("tc3162/ae_wan_inic_status", 0, NULL);
	xsi_proc->read_proc = NULL;
	xsi_proc->write_proc = ae_wan_inic_state_write_proc;

	xsi_proc = create_proc_entry("tc3162/ae_wan_inic_filter", 0, NULL);
	xsi_proc->read_proc = NULL;
	xsi_proc->write_proc = ae_wan_inic_filter_write_proc;

	xsi_proc = create_proc_entry("tc3162/adsl_mpoa_header", 0, NULL);
	xsi_proc->read_proc = ae_wan_adsl_mpoa_head_read_proc;
	xsi_proc->write_proc = ae_wan_adsl_mpoa_head_write_proc;
#endif

	xsi_proc = create_proc_entry("tc3162/ae_wan_ratelimit", 0, NULL);
	if(xsi_proc) 
	{
		xsi_proc->read_proc = hsgmii_ratelimit_read_proc;
		xsi_proc->write_proc = hsgmii_ratelimit_write_proc;
	}
	xsi_proc = create_proc_entry("tc3162/transform_vlan", 0, NULL);
	if(xsi_proc) 
	{
		xsi_proc->read_proc = transform_vlan_params_read_proc;
		xsi_proc->write_proc = transform_vlan_params_write_proc;  
	}
	
	xsi_proc = create_proc_entry("tc3162/mgr_vlan", 0, NULL);
	if(xsi_proc) 
	{
		xsi_proc->read_proc = manage_vlan_read_proc;
		xsi_proc->write_proc = manage_vlan_write_proc;  
	}

	xsi_proc = create_proc_entry("tc3162/ae_wan_fastmode", 0, NULL);
	if(xsi_proc){
		xsi_proc->read_proc = ae_wan_fast_slow_read_proc;
		xsi_proc->write_proc = ae_wan_fast_slow_write_proc;  
	}

#if defined(CONFIG_TP_IMAGE) && defined(INCLUDE_MULTICAST_VLAN)
	xsi_proc = create_proc_entry("tc3162/ae_wan_iptv_multicast", 0, NULL);
	xsi_proc->read_proc = ae_wan_iptv_multicast_read_proc;
	xsi_proc->write_proc = ae_wan_iptv_multicast_write_proc;
#endif /* CONFIG_TP_IMAGE && INCLUDE_MULTICAST_VLAN */

	return 0;
}

int ae_wan_proc_exit(void)
{
	remove_proc_entry("tc3162/ae_wan_mac_dbg", 0);
	remove_proc_entry("tc3162/ae_wan_ratelimit", 0);
	remove_proc_entry("tc3162/ae_wan_link_status", 0);
	remove_proc_entry("tc3162/transform_vlan", 0);
	remove_proc_entry("tc3162/mgr_vlan", 0);
#if defined(TCSUPPORT_SISM_HOST)
	remove_proc_entry("tc3162/ae_wan_inic_status", 0);
	remove_proc_entry("tc3162/ae_wan_inic_filter", 0);
	remove_proc_entry("tc3162/adsl_mpoa_header", 0);
#endif
	remove_proc_entry("tc3162/ae_wan_fastmode", 0);

    return 0;
}

