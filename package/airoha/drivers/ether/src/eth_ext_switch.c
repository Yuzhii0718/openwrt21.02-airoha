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
#include "eth_ext_switch.h"

#ifdef TCSUPPORT_SWITCH_8850
#include "../en8850/en8850_init.h"
#include "../en8850/en8850_api.h"
#include "../en8850/ecnt.h"
#endif

#ifdef TCSUPPORT_SWITCH_8851
#include "../en8851/en8851_init.h"
#endif

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

/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int	support_external_switch(void)
{
#if defined(TCSUPPORT_SWITCH_8850) || defined(TCSUPPORT_SWITCH_8851)
	return 1;
#endif
	return 0;
}
int macExtSwitchgswGlobalAPI(mt7530_switch_api_params api_param)
{
#ifdef TCSUPPORT_SWITCH_8850
	return macEN8850gswGlobalAPI(api_param);
#endif
	return 0;
}

int macExtSwitchgswPortAPI(u32 portId, mt7530_switch_api_params api_param)
{
#ifdef TCSUPPORT_SWITCH_8850
	return macEN8850gswPortAPI(portId, api_param);
#endif
	return 0;
}

void external_switch_init(void)
{
#ifdef TCSUPPORT_SWITCH_8850
	en8850_switch_init();
#elif defined(TCSUPPORT_SWITCH_8851)
	en8851_switch_init();
#endif
	return ;
}

uint16_t print_external_switch_link_status(char *buf, uint16_t index, uint32_t port)
{
#ifdef TCSUPPORT_SWITCH_8850
	int reg=0;
	uint8_t tx_fc=0, rx_fc=0;
	ECNT_PORT_STATUS_T ps;
	memset(&ps, 0x0, sizeof(ECNT_PORT_STATUS_T));

	ecnt_port_getLink(0, port, &ps);
    if (!ps.link) {
        index += sprintf(buf+index, "Down\n");
        return index;
    }

    if (ps.speed == ECNT_PORT_SPEED_2500M)
        index += sprintf(buf+index, "2.5G/");
    else if (ps.speed == ECNT_PORT_SPEED_1000M)
        index += sprintf(buf+index, "1000M/");
    else if (ps.speed == ECNT_PORT_SPEED_100M)
        index += sprintf(buf+index, "100M/");
    else
        index += sprintf(buf+index, "10M/");

    if (ps.duplex)
        index += sprintf(buf+index, "Full Duplex");
    else
        index += sprintf(buf+index, "Half Duplex");

	ecnt_port_getFlowCtrl(0, port, 0, &tx_fc);
	ecnt_port_getFlowCtrl(0, port, 1, &rx_fc);

    if (tx_fc | rx_fc) {
        index += sprintf(buf+index, " FC:");
        if (tx_fc)
            index += sprintf(buf+index, " TX");
        if (rx_fc)
            index += sprintf(buf+index, " RX");
    }

	aml_readReg(0, PMSR(port), &reg);
    if (reg & EEE100_STS)
        index += sprintf(buf+index, " EEE100");
    if (reg & EEE1G_STS)
        index += sprintf(buf+index, " EEE1G");

    index += sprintf(buf+index, "\n");
#endif
    return index;
}

int macextLanPortMap2Switch(u32 lanPortId)
{
#ifdef TCSUPPORT_SWITCH_8850
	return macEN8850LanPortMap2Switch(lanPortId-4);
#elif defined(TCSUPPORT_SWITCH_8851)
	return macEN8851LanPortMap2Switch(lanPortId-4);
#else
	return 0;
#endif
}

