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


#include "ecnt_hook/ecnt_hook.h"
#include "ecnt_hook/ecnt_hook_lro.h"
#include "lro_api.h"
#include "lro.h"
#include "lro_glb.h"


/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/


/************************************************************************
*                  M A C R O S
*************************************************************************
*/    

/************************************************************************
*                  D A T A     T Y P E S
*************************************************************************
*/


/************************************************************************
*                  E X T E R N A L     D A T A   D E C L A R A T I O N S
*************************************************************************
*/


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
ecnt_ret_val ecnt_lro_hook(struct ecnt_data *indata);
static int lro_get_free_ring(struct ECNT_LRO_Data *lro_data);

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/



/* ------------------ ECNT_Hook ------------------ */
typedef int (*lro_api_op_t)(struct ECNT_LRO_Data *lro_data);

/* Warning: same sequence with enum 'ETHER_HookFunction_t' */
lro_api_op_t
lro_operation[]=
{
    lro_get_free_ring,
};


struct ecnt_hook_ops ecnt_driver_lro_op = {
    .name = "driver_lro_hook",
    .hookfn = ecnt_lro_hook,
    .maintype = ECNT_LRO,
    .is_execute = 1,
    .subtype = ECNT_DRIVER_API,
    .priority = 1
};

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
	


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

ecnt_ret_val ecnt_lro_hook(struct ecnt_data *indata)
{
	struct ECNT_LRO_Data *lro_data = (struct ECNT_LRO_Data *)indata ;
	ulong flags=0 ;	
	
	if(lro_data->function_id >= LRO_FUNCTION_MAX_NUM) {
		printk("lro_data->function_id is %d, exceed max number: %d", 
		    lro_data->function_id, LRO_FUNCTION_MAX_NUM);
		return ECNT_HOOK_ERROR;
	}
	lro_data->retValue = lro_operation[lro_data->function_id](lro_data) ;
	
	return ECNT_CONTINUE;
}

static int lro_get_free_ring(struct ECNT_LRO_Data *lro_data)
{
	int ppe_idx;

	ppe_idx = lro_data->lro_private.get_ring.ppe_entry_idx;
	lro_data->lro_private.get_ring.free_ring_idx = get_free_ring(ppe_idx);
	return 0;
}


