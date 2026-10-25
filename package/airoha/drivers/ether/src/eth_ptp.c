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
#include <linux/timer.h>
#include <linux/netdevice.h>
#include <linux/net_tstamp.h>
#include <linux/ptp_clock_kernel.h>
#include <linux/ptp_classify.h>
#include <linux/timekeeping.h>
#include <ecnt_hook/ecnt_hook_1588.h>
#include <modules/ptp/ptp.h>
#include "eth_lan.h"

int ecnt_config_hwtstamp(struct net_device *netdev, struct ifreq *ifr)
{
	struct hwtstamp_config hwt_cfg;

	if (copy_from_user(&hwt_cfg, ifr->ifr_data, sizeof(hwt_cfg)))
		return -EFAULT;

	if (hwt_cfg.flags)
		return -EFAULT;

	PTP_1588_API_NETINF_HW_TSTAMP_CONFIG(netdev, &hwt_cfg);

	return copy_to_user(ifr->ifr_data, &hwt_cfg, sizeof(hwt_cfg)) ? -EFAULT : 0;
}
EXPORT_SYMBOL(ecnt_config_hwtstamp);

static int ecnt_get_ts_info(
		struct net_device *netdev,
		struct ethtool_ts_info *info)
{
	ethtool_op_get_ts_info(netdev, info);

	PTP_1588_API_NETINF_HW_TSTAMP_INFO(info);

	return 0;
}

int ecnt_set_ethtool_ops(struct net_device *netdev, struct ethtool_ts_info *info)
{
	/*netdev->ethtool_ops = &ecnt_ethtool_ops;*/
	return ecnt_get_ts_info(netdev,info);
}
EXPORT_SYMBOL(ecnt_set_ethtool_ops);
