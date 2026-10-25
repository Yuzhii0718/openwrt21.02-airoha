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
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "libapi_lib_ifc.h"
#include <ecnt_hook/ecnt_hook_ifc_type.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
************************************************************************/

/************************************************************************
*                  M A C R O S
************************************************************************/

/************************************************************************
*                  D A T A	 T Y P E S
************************************************************************/

/************************************************************************
*                  E X T E R N A L	 D A T A   D E C L A R A T I O N S
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
************************************************************************/

/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

static int ifc_lib_ioctl(int cmd, void *p)
{
	int fd, ret;
	
	fd = open("/dev/ifc", O_RDWR);
    if (fd < 0) {
		printf("Open %s pseudo device failed\n","/dev/ifc");
		return -1;
    }

	ret = ioctl(fd, 0, p);
	close(fd);

	if(ret >= 0) 
		return 0;
	
	return -1;
}

int ifc_lib_set_vip_by_etype(unsigned short ether_ype)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_SET_VIP;
	ifc_data.field = IFC_VIP_FIELD_ETYPE;
	ifc_data.ifc_param.key[0] = ether_ype;

	ret = ifc_lib_ioctl(IFC_SET_VIP,&ifc_data);
	return ret;
}

int ifc_lib_set_vip_by_ipv4_proto(unsigned short proto)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_SET_VIP;
	ifc_data.field = IFC_VIP_FIELD_IPV4_PROTO;
	ifc_data.ifc_param.key[0] = proto;

	ret = ifc_lib_ioctl(IFC_SET_VIP,&ifc_data);
	return ret;
}

int ifc_lib_set_vip_by_ipv6_nxthdr(unsigned short nxthdr)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_SET_VIP;
	ifc_data.field = IFC_VIP_FIELD_IPV6_NXTHDR;
	ifc_data.ifc_param.key[0] = nxthdr;

	ret = ifc_lib_ioctl(IFC_SET_VIP,&ifc_data);
	return ret;
}

int ifc_lib_set_vip_by_port(unsigned short sport,unsigned short dport
    , unsigned short ringIdx, unsigned int swap)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_SET_VIP;
	ifc_data.field = IFC_VIP_FIELD_PORT;
	ifc_data.ifc_param.key[0] = sport;
	ifc_data.ifc_param.key[1] = dport;
	ifc_data.ifc_param.key[2] = ringIdx;
	ifc_data.ifc_param.key[3] = swap;

	ret = ifc_lib_ioctl(IFC_SET_VIP,&ifc_data);
	return ret;
}

int ifc_lib_set_vip_by_sip_and_sport(unsigned int sip,unsigned short sport)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_SET_VIP;
	ifc_data.field = IFC_VIP_FIELD_SIP_SPORT;
	ifc_data.ifc_param.key[0] = sip;	
	ifc_data.ifc_param.key[1] = sport;

	ret = ifc_lib_ioctl(IFC_SET_VIP,&ifc_data);
	return ret;
}

int ifc_lib_set_vip_by_dip_and_dport(unsigned int dip,unsigned short dport)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_SET_VIP;
	ifc_data.field = IFC_VIP_FIELD_DIP_DPORT;
	ifc_data.ifc_param.key[0] = dip;	
	ifc_data.ifc_param.key[1] = dport;

	ret = ifc_lib_ioctl(IFC_SET_VIP,&ifc_data);
	return ret;
}

int ifc_lib_set_flow_by_pbit(unsigned char pbit)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_SET_VIP;
	ifc_data.field = IFC_VIP_FIELD_PBIT;
	ifc_data.ifc_param.key[0] = (pbit << 1);

	ret = ifc_lib_ioctl(IFC_SET_VIP,&ifc_data);

	return ret;
}

int ifc_lib_set_flow_by_dscp(unsigned char dscp)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_SET_VIP;
	ifc_data.field = IFC_VIP_FIELD_DSCP;
	ifc_data.ifc_param.key[0] = dscp;

	ret = ifc_lib_ioctl(IFC_SET_VIP,&ifc_data);

	return ret;
}

int ifc_lib_set_flow_by_traffic_class(unsigned char tc)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_SET_VIP;
	ifc_data.field = IFC_VIP_FIELD_TRAFFIC_CLASS;
	ifc_data.ifc_param.key[0] = tc;
	
	ret = ifc_lib_ioctl(IFC_SET_VIP,&ifc_data);

	return ret;
}

int ifc_lib_del_vip_by_etype(unsigned short ether_ype)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_DEL_VIP;
	ifc_data.field = IFC_VIP_FIELD_ETYPE;
	ifc_data.ifc_param.key[0] = ether_ype;

	ret = ifc_lib_ioctl(IFC_DEL_VIP,&ifc_data);
	return ret;
}

int ifc_lib_del_vip_by_ipv4_proto(unsigned short proto)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_DEL_VIP;
	ifc_data.field = IFC_VIP_FIELD_IPV4_PROTO;
	ifc_data.ifc_param.key[0] = proto;

	ret = ifc_lib_ioctl(IFC_DEL_VIP,&ifc_data);
	return ret;
}

int ifc_lib_del_vip_by_ipv6_nxthdr(unsigned short nxthdr)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_DEL_VIP;
	ifc_data.field = IFC_VIP_FIELD_IPV6_NXTHDR;
	ifc_data.ifc_param.key[0] = nxthdr;

	ret = ifc_lib_ioctl(IFC_DEL_VIP,&ifc_data);
	return ret;
}

int ifc_lib_del_vip_by_port(unsigned short sport,unsigned short dport, unsigned int swap)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_DEL_VIP;
	ifc_data.field = IFC_VIP_FIELD_PORT;
	ifc_data.ifc_param.key[0] = sport;
	ifc_data.ifc_param.key[1] = dport;
	ifc_data.ifc_param.key[2] = swap;

	ret = ifc_lib_ioctl(IFC_DEL_VIP,&ifc_data);
	return ret;
}

int ifc_lib_del_vip_by_sip_and_sport(unsigned int sip,unsigned short sport)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_DEL_VIP;
	ifc_data.field = IFC_VIP_FIELD_SIP_SPORT;
	ifc_data.ifc_param.key[0] = sip;	
	ifc_data.ifc_param.key[1] = sport;

	ret = ifc_lib_ioctl(IFC_DEL_VIP,&ifc_data);
	return ret;
}

int ifc_lib_del_vip_by_dip_and_dport(unsigned int dip,unsigned short dport)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_DEL_VIP;
	ifc_data.field = IFC_VIP_FIELD_DIP_DPORT;
	ifc_data.ifc_param.key[0] = dip;	
	ifc_data.ifc_param.key[1] = dport;

	ret = ifc_lib_ioctl(IFC_DEL_VIP,&ifc_data);
	return ret;
}

int ifc_lib_del_flow_by_pbit(unsigned char pbit)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_DEL_VIP;
	ifc_data.field = IFC_VIP_FIELD_PBIT;
	ifc_data.ifc_param.key[0] = (pbit << 1);

	ret = ifc_lib_ioctl(IFC_DEL_VIP,&ifc_data);

	return ret;
}

int ifc_lib_del_flow_by_dscp(unsigned char dscp)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_DEL_VIP;
	ifc_data.field = IFC_VIP_FIELD_DSCP;
	ifc_data.ifc_param.key[0] = dscp;

	ret = ifc_lib_ioctl(IFC_DEL_VIP,&ifc_data);

	return ret;
}

int ifc_lib_del_flow_by_traffic_class(unsigned char tc)
{
	int ret = 0;
	struct ecnt_ifc_data ifc_data;

	ifc_data.function_id = IFC_DEL_VIP;
	ifc_data.field = IFC_VIP_FIELD_TRAFFIC_CLASS;
	ifc_data.ifc_param.key[0] = tc;

	ret = ifc_lib_ioctl(IFC_DEL_VIP,&ifc_data);

	return ret;
}
