/***************************************************************
Copyright Statement:

This software/firmware and related documentation (¡°EcoNet Software¡±) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (¡°EcoNet¡±) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (¡°ECONET SOFTWARE¡±) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ¡°AS IS¡± 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER¡¯S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER¡¯S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/string.h>


#include "tcetherphy_hook.h"
/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/

/************************************************************************
*                  D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern void EphyMonitor(void);
extern void doEphySetSLTMode(uint8 phyaddr, uint16 speed_mode);
extern void doEphySetSETMode(uint8 set_mode, uint16 parameter);

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
int ephy_monitor(struct ECNT_EPHY_Data *eth_data);
int ephy_power_down(struct ECNT_EPHY_Data *eth_data);
int EphyPhyMiiWriteTrDbg(struct ECNT_EPHY_Data *data);
int EphyMtMiiRegWrite(struct ECNT_EPHY_Data *data);
int EphyMtMiiRegRead(struct ECNT_EPHY_Data *data);
int EphyMtEMiiRegWrite(struct ECNT_EPHY_Data *data);
int EphyMtEMiiRegRead(struct ECNT_EPHY_Data *data);
#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521) 
#if !defined(TCSUPPORT_CPU_EN7516) && !defined(TCSUPPORT_CPU_EN7527)
int EphyMt7512FeReadProbe(struct ECNT_EPHY_Data *data);
int EphyMt7512FeSnrSum(struct ECNT_EPHY_Data *data);
int EphyMt7512FeReadAdcSum(struct ECNT_EPHY_Data *data);
#endif
#endif
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
int EphySLTMode(struct ECNT_EPHY_Data *data);
int EphySETMode(struct ECNT_EPHY_Data *data);
#endif

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
typedef int (*ephy_api_op_t)(struct ECNT_EPHY_Data *ephy_data);

/* Warning: same sequence with enum 'EPHY_HookFunction_t' */
ephy_api_op_t ephy_operation[]=
{
    ephy_monitor,
    ephy_power_down,
	EphyPhyMiiWriteTrDbg,		/* EPHY_FUNCTION_PHY_MII_WRITE_TRDBG */
	EphyMtMiiRegWrite,			/* EPHY_FUNCTION_MT_MII_REG_WRITE */
	EphyMtMiiRegRead,			/* EPHY_FUNCTION_MT_MII_REG_READ */
	EphyMtEMiiRegWrite,			/* EPHY_FUNCTION_MT_EMII_REG_WRITE */
	EphyMtEMiiRegRead,			/* EPHY_FUNCTION_MT_EMII_REG_READ */
	#if  defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
	#if !defined(TCSUPPORT_CPU_EN7516) && !defined(TCSUPPORT_CPU_EN7527) 
	EphyMt7512FeReadProbe,		/* EPHY_FUNCTION_MT7512FE_READ_PROBE */
	EphyMt7512FeSnrSum,			/* EPHY_FUNCTION_MT7512FE_SNR_SUM */
	EphyMt7512FeReadAdcSum,		/* EPHY_FUNCTION_MT7512_FE_READ_ADC_SUM */
	#endif
	#endif
#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
	EphySLTMode,				/* EPHY_FUNCTION_SLT_MODE */
	EphySETMode,				/* EPHY_FUNCTION_SET_MODE */
#endif
};

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
spinlock_t hookFuncLock[EPHY_FUNCTION_MAX_NUM];

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

ecnt_ret_val ecnt_ephy_hook(struct ecnt_data *indata)
{
	struct ECNT_EPHY_Data *ephy_data = (struct ECNT_EPHY_Data *)indata ;
	ulong flags=0 ;	
	
	if(ephy_data->function_id >= EPHY_FUNCTION_MAX_NUM) {
		printk("ephy_data->function_id is %d, exceed max number: %d", ephy_data->function_id, EPHY_FUNCTION_MAX_NUM);
		return ECNT_HOOK_ERROR;
	}
	//if(ephy_data->function_id <= EPHY_FUNCTION_MAX_NUM)
	ephy_data->retValue = ephy_operation[ephy_data->function_id](ephy_data) ;
	
	return ECNT_CONTINUE;

}

int ephy_monitor(struct ECNT_EPHY_Data *eth_data)
{
    EphyMonitor();
    return ECNT_CONTINUE;
}

int ephy_power_down(struct ECNT_EPHY_Data *eth_data)
{

	#if defined(TCSUPPORT_CPU_EN7581)
	tcMiiStationWrite(eth_data->phy_add, 31, 0);
	tcMiiStationWrite(eth_data->phy_add, 0, 0x0800);
	#else
	tcMiiStationWrite(eth_data->phy_add, 31, 0);
	tcMiiStationWrite(eth_data->phy_add, 0, 0x1800);
	#endif

    return ECNT_CONTINUE;
}

#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
#if !defined(TCSUPPORT_CPU_EN7516) && !defined(TCSUPPORT_CPU_EN7527)
/* int32 mt7512FEReadProbe(uint8 port_num, uint8 mode) */
int EphyMt7512FeReadProbe(struct ECNT_EPHY_Data *data)
{
	#if !defined(TCSUPPORT_CPU_EN7580)
	return mt7512FEReadProbe(data->ephy_private.probe.port_num, data->ephy_private.probe.mode);
	#else
	return 0;
	#endif
}

/* uint16 mt7512FEReadSnrSum(uint8 port_num, uint16 cnt) */
int EphyMt7512FeSnrSum(struct ECNT_EPHY_Data *data)
{
	#if !defined(TCSUPPORT_CPU_EN7580)
	return mt7512FEReadSnrSum(data->ephy_private.snr.port_num, data->ephy_private.snr.cnt);
	#else
	return 0;
	#endif
}
#endif
#endif

/* int mtPhyMiiWrite_TrDbg(uint8 phyaddr, char *type, uint32 data_addr ,uint32 value, uint8 ch_num) */
int EphyPhyMiiWriteTrDbg(struct ECNT_EPHY_Data *data)
{
	return mtPhyMiiWrite_TrDbg(data->ephy_private.trDbg.phyaddr, data->ephy_private.trDbg.type, \
	data->ephy_private.trDbg.data_addr, data->ephy_private.trDbg.value, data->ephy_private.trDbg.ch_num);
}

/* void mtMiiRegWrite(uint32 port_num, uint32 reg_num, uint32 reg_data) */
int EphyMtMiiRegWrite(struct ECNT_EPHY_Data *data)
{
	mtMiiRegWrite(data->ephy_private.mii.port_num, data->ephy_private.mii.reg_num, data->ephy_private.mii.reg_data);
	return 0;
}

/* uint32 mtMiiRegRead(uint8 port_num,uint8 reg_num) */
int EphyMtMiiRegRead(struct ECNT_EPHY_Data *data)
{
	return mtMiiRegRead(data->ephy_private.miiRead.port_num, data->ephy_private.miiRead.reg_num);
}

/* void mtEMiiRegWrite(uint32 port_num, uint32 dev_num, uint32 reg_num, uint32 reg_data) */
int EphyMtEMiiRegWrite(struct ECNT_EPHY_Data *data)
{
	mtEMiiRegWrite(data->ephy_private.mii.port_num, data->ephy_private.mii.dev_num, \
					data->ephy_private.mii.reg_num, data->ephy_private.mii.reg_data);
	return 0;
}

/* uint32 mtEMiiRegRead(uint32 port_num, uint32 dev_num, uint32 reg_num) */
int EphyMtEMiiRegRead(struct ECNT_EPHY_Data *data)
{
	return mtEMiiRegRead(data->ephy_private.mii.port_num, data->ephy_private.mii.dev_num, data->ephy_private.mii.reg_num);
}

#if 0 //defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
/* int32 mt7512FEReadAdcSum(uint8 port_num); */
int EphyMt7512FeReadAdcSum(struct ECNT_EPHY_Data *data)
{
	return mt7512FEReadAdcSum(data->ephy_private.adc.port_num);
}
#endif

#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
#if !defined(TCSUPPORT_CPU_EN7516) && !defined(TCSUPPORT_CPU_EN7527)
/* int32 mt7512FEReadAdcSum(uint8 port_num); */
int EphyMt7512FeReadAdcSum(struct ECNT_EPHY_Data *data)
{
	#if !defined(TCSUPPORT_CPU_EN7580)
	return mt7512FEReadAdcSum(data->ephy_private.adc.port_num);
	#else
	return 0;
	#endif
}
#endif
#endif

#if defined(TCSUPPORT_CPU_EN7516) || defined(TCSUPPORT_CPU_EN7580) || defined(TCSUPPORT_CPU_EN7528)
int EphySLTMode(struct ECNT_EPHY_Data *data)
{
	doEphySetSLTMode(data->ephy_private.slt.port_num, data->ephy_private.slt.speed_mode);
    return ECNT_CONTINUE;
}

int EphySETMode(struct ECNT_EPHY_Data *data)
{
doEphySetSETMode(data->ephy_private.set.set_mode, data->ephy_private.set.parameter);
    return ECNT_CONTINUE;
}
#endif

