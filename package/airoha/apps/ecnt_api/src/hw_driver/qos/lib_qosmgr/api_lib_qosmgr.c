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

#include "libapi_lib_qosmgr.h"
#include "qdma_type.h"


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


/******************************************************************************
 Descriptor:    It's used to ioctl to qosmgr qdma device.
 Input Args:    qdma_data
 Ret Value: success: ECNT_QOSMGR_SUCCESS
                       fail: ECNT_QOSMGR_FAILED
******************************************************************************/
static int qosmgr_qdma_lib_ioctl(struct ECNT_QDMA_Data *qdma_data)
{
    int fd, ret;
    
    fd = open(qdma_path, O_RDWR);
    if (fd < 0) {
        printf("Open %s pseudo device failed\n", qdma_path);
        return ECNT_QOSMGR_FAILED;
    }

    ret = ioctl(fd, IO_QDMA_CMD_OPT, qdma_data);
    close(fd);

    if(ret >= 0) {
        return ECNT_QOSMGR_SUCCESS;
    } else {
        return ECNT_QOSMGR_FAILED;
    }
}

/******************************************************************************
 Descriptor:    It's used to ioctl to qosmgr fe device.
 Input Args:    fe_data
 Ret Value: success: ECNT_QOSMGR_SUCCESS
                       fail: ECNT_QOSMGR_FAILED
******************************************************************************/

static int qosmgr_fe_lib_ioctl(struct ecnt_fe_data *fe_data)
{
	int fd, ret;
	
	fd = open(fe_path, O_RDWR);
    if (fd < 0) {
		printf("Open %s pseudo device failed\n", fe_path);
        return ECNT_QOSMGR_FAILED;
    }

	ret = ioctl(fd, 0, fe_data);
	close(fd);

	if(ret >= 0) 
        return ECNT_QOSMGR_SUCCESS;
	
    return ECNT_QOSMGR_FAILED;
}

/******************************************************************************
 Descriptor:    It's used to ioctl to qosmgr ppe device. to set meter
 Input Args:    opt & command
 Ret Value: success: ECNT_QOSMGR_SUCCESS
                       fail: ECNT_QOSMGR_FAILED
******************************************************************************/
static int qosmgr_ppe_lib_ioctl(void *opt, unsigned long command)
{
    int fd, retVal = 1;
    struct ppe_meter_cfg *meterCfg = opt;
    
    fd = open(hw_nat_path, O_RDWR);
    if (fd < 0)
    {
    	printf("Open %s pseudo device failed\n", hw_nat_path);
    	return ECNT_QOSMGR_FAILED;
    }
    
    if (ioctl(fd, command, opt)<0)
    {
        printf("PPE_API: ioctl error\n");
        close(fd);
        return ECNT_QOSMGR_FAILED;
    }

    close(fd);
	
    retVal = meterCfg->retVal;

    if (retVal) 
    {
        printf("PPE_API: return value error\n");
        return ECNT_QOSMGR_FAILED;
    }
    
    return ECNT_QOSMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to set the qos config NULLQOS.
 Input Args:	queue_mask.
 Ret Value:	success: ECNT_QOSMGR_SUCCESS
                     fail: ECNT_QOSMGR_FAILED
******************************************************************************/
int qosmgr_lib_set_qos_config_NULLQOS(char queue_mask)
{
	struct ECNT_QDMA_Data qdma_data;
    QDMA_MacQoSCfg_T macQosCfg;
    macQosCfg.queueMask = queue_mask;
    macQosCfg.qosClsfy = NULLQOS;
	qdma_data.function_id = QDMA_FUNCTION_SET_MAC_QOS_CONFIG;
    qdma_data.qdma_private.macQosCfg = &macQosCfg;    
    
	return qosmgr_qdma_lib_ioctl(&qdma_data);
}

/******************************************************************************
 Descriptor:	It's used to set the qos config PQ.
 Input Args:	queue_mask.
 Ret Value:	success: ECNT_QOSMGR_SUCCESS
                     fail: ECNT_QOSMGR_FAILED
******************************************************************************/
int qosmgr_lib_set_qos_config_PQ(char queue_mask)
{
	struct ECNT_QDMA_Data qdma_data;
    QDMA_MacQoSCfg_T macQosCfg;
    macQosCfg.queueMask = queue_mask;
    macQosCfg.qosClsfy = QOS_HW_PQ;
	//memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
	qdma_data.function_id = QDMA_FUNCTION_SET_MAC_QOS_CONFIG;
    qdma_data.qdma_private.macQosCfg = &macQosCfg;    
    
	return qosmgr_qdma_lib_ioctl(&qdma_data);
}

/******************************************************************************
 Descriptor:	It's used to set the qos config WRR.
 Input Args:	queue_mask & weight array.
 Ret Value:	success: ECNT_QOSMGR_SUCCESS
                     fail: ECNT_QOSMGR_FAILED
******************************************************************************/
int qosmgr_lib_set_qos_config_WRR(char queue_mask,  unsigned int *weight)
{
	struct ECNT_QDMA_Data qdma_data;
    QDMA_MacQoSCfg_T macQosCfg;
    int i=0;
    
    macQosCfg.queueMask = queue_mask;
    macQosCfg.qosClsfy = QOS_HW_WRR;
	memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
	qdma_data.function_id = QDMA_FUNCTION_SET_MAC_QOS_CONFIG;
    
    for(i=0; i<4; i++)
      macQosCfg.qosWrrInfo[i] = weight[i];
 
    qdma_data.qdma_private.macQosCfg = &macQosCfg;
    
	return qosmgr_qdma_lib_ioctl(&qdma_data);

}

/******************************************************************************
 Descriptor:	It's used to set the qos config CAR.
 Input Args:	queue_mask & ratelimit array.
 Ret Value:	success: ECNT_QOSMGR_SUCCESS
                     fail: ECNT_QOSMGR_FAILED
******************************************************************************/
int qosmgr_lib_set_qos_config_CAR(char queue_mask,  unsigned int *ratelimit)
{
    int i = 0, ret = ECNT_QOSMGR_SUCCESS;
    QDMA_MacQoSCfg_T macQosCfg;
    struct ECNT_QDMA_Data qdma_data;
    
    memset(&qdma_data, 0, sizeof(struct ECNT_QDMA_Data));
    macQosCfg.queueMask = queue_mask;
    macQosCfg.qosClsfy = QOS_SW_CAR;
    qdma_data.function_id = QDMA_FUNCTION_SET_MAC_QOS_CONFIG;
    qdma_data.qdma_private.macQosCfg = &macQosCfg;
    if (qosmgr_qdma_lib_ioctl(&qdma_data))
    {       
		ret = ECNT_QOSMGR_FAILED;
        printf("qosmgr_lib_set_qos_config_CAR: qdma_lib_ioctl failed\n");
        return ret;
    }

    /* config meter ratelimit */
    for (i=0; i< CAR_QUEUE_NUM; i++)
    {
        if (!(queue_mask & (1<<i))) 
        {
            continue;
        }
        
        if (qosmgr_lib_set_qos_config_meter(ratelimit[i], METER_ENABLE, i+1))
        {
            printf("qosmgr_lib_set_qos_config_CAR: set_qos_config_meter failed\n");
            ret = ECNT_QOSMGR_FAILED;
            break;
        }
    }
        
    return ret;
}


/******************************************************************************
 Descriptor:	It's used to set the qos config CAR meter.
 Input Args:	mode & ratelimit array & index.
 Ret Value:	success: ECNT_QOSMGR_SUCCESS
                     fail: ECNT_QOSMGR_FAILED
******************************************************************************/
int qosmgr_lib_set_qos_config_meter(unsigned int ratelimit, int mode, char index ){
    
    struct ecnt_fe_data fe_data;
#if defined(TCSUPPORT_CPU_EN7528) || defined(TCSUPPORT_CPU_EN7527)|| defined(TCSUPPORT_CPU_EN7516)
    struct ppe_meter_cfg meter = {0};
#endif

#if defined(TCSUPPORT_CPU_EN7580)
    memset(&fe_data, 0, sizeof(struct ecnt_fe_data));
    fe_data.function_id = FE_SET_METER_RATELIMIT;
    fe_data.api_data.meter_cfg.rate = ratelimit;
    fe_data.api_data.meter_cfg.enable = mode;
    fe_data.api_data.meter_cfg.idx = index;
    
    fe_data.api_data.meter_cfg.pkt_mode = METER_BYTE_MODE;
    fe_data.api_data.meter_cfg.tick_sel = FAST_TICK;
    fe_data.api_data.meter_cfg.dir = UP_STREAM;  //0
    fe_data.api_data.meter_cfg.mode = CAR_QOS_MODE; //0    
    fe_data.api_data.meter_cfg.bucket_size = 0; 
    return qosmgr_fe_lib_ioctl(&fe_data);
#elif defined(TCSUPPORT_CPU_EN7528) || defined(TCSUPPORT_CPU_EN7527) || defined(TCSUPPORT_CPU_EN7516)
    meter.group = index;
    meter.rate = ratelimit;
    if (mode)
    {
        meter.method = PPE_METHOD_ADD;
        meter.mode = METER_BYTE_MODE;
        meter.scale = BYTE_MODE_SCALE_K;
        meter.enable = METER_ENABLE;
        meter.addFlowOnly = METER_TYPE;
    }
    else
    {
    	meter.enable = METER_DISABLE;
        meter.method = PPE_METHOD_DEL;
    }
	
    return qosmgr_ppe_lib_ioctl(&meter, PPE_COMMAND_METER_GROUP);
#else
	printf("qosmgr_lib_set_qos_config_meter:Set ERROR!,Not Support this IC\n");
    return ECNT_QOSMGR_FAILED;
#endif
	return ECNT_QOSMGR_FAILED;
}

/******************************************************************************
 Descriptor:	It's used to set the qos config CAR meter.
 Input Args:	dir & vlan & queue & group_id.
 Ret Value:	success: ECNT_QOSMGR_SUCCESS
                     fail: ECNT_QOSMGR_FAILED
******************************************************************************/
int qosmgr_lib_add_qos_config_meter_flow(unsigned int dir, unsigned int vlan, unsigned int queue, unsigned int group_id)
{
    struct ppe_meter_cfg meter = {0};

    meter.method = PPE_METHOD_ADD;
    meter.dir = dir;
    meter.vlan = vlan;
    meter.queue = queue;
    meter.group = group_id;
    meter.addFlowOnly = 1;
    
    if (qosmgr_ppe_lib_ioctl(&meter, PPE_COMMAND_METER_FLOW))
    {
        return ECNT_QOSMGR_FAILED;
    }

    return ECNT_QOSMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to set the qos config CAR meter.
 Input Args:	dir & vlan & queue & group_id.
 Ret Value:	success: ECNT_QOSMGR_SUCCESS
                     fail: ECNT_QOSMGR_FAILED
******************************************************************************/
int qosmgr_lib_del_qos_config_meter_flow(unsigned int dir, unsigned int vlan)
{
    struct ppe_meter_cfg meter = {0};

    meter.method = PPE_METHOD_DEL;
    meter.dir = dir;
    meter.vlan = vlan;

    if (qosmgr_ppe_lib_ioctl(&meter, PPE_COMMAND_METER_FLOW))
    {
        return ECNT_QOSMGR_FAILED;
    }

    return ECNT_QOSMGR_SUCCESS;
}

/******************************************************************************
 Descriptor:	It's used to set the qos config ratelimit.
 Input Args:	uplinkBandwidth.
 Ret Value:	success: ECNT_QOSMGR_SUCCESS
                     fail: ECNT_QOSMGR_FAILED
******************************************************************************/
int qosmgr_lib_set_qos_config_rate(unsigned int uplinkBandwidth)
{
    struct ecnt_fe_data fe_data;

	memset(&fe_data, 0, sizeof(struct ecnt_fe_data));
    fe_data.function_id = FE_SET_TX_RATE;    
    fe_data.api_data.rate_cfg.rate = uplinkBandwidth;
    return qosmgr_fe_lib_ioctl(&fe_data);
}

