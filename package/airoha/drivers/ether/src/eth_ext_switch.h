#ifndef _ETH_EXT_SWITCH_H
#define _ETH_EXT_SWITCH_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <modules/eth_global_def.h>

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#if defined(TCSUPPORT_SWITCH_8850)
#define SUPPORT_EXTERNAL_SWITCH	1
#else
#define SUPPORT_EXTERNAL_SWITCH	0
#endif

/************************************************************************
*               M A C R O S
*************************************************************************
*/

/************************************************************************
*               D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*               D A T A   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S
                I N L I N E  F U N C T I O N  D E F I N I T I O N S
*************************************************************************
*/

int	support_external_switch(void);
int macExtSwitchgswGlobalAPI(mt7530_switch_api_params api_param);
int macExtSwitchgswPortAPI(u32 portId, mt7530_switch_api_params api_param);
void external_switch_init(void);
uint16_t print_external_switch_link_status(char *buf, uint16_t index, uint32_t port);
int macextLanPortMap2Switch(u32 lanPortId);

#endif

