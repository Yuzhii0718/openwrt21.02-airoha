
/************************************************************************
*               I N C L U D E S
*************************************************************************
*/
#include <linux/types.h>
#include <asm/io.h>
#include "ecnt_sgmii.h"
#include <ecnt_hook/ecnt_hook_sgmii.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                  M A C R O S
*************************************************************************
*/
extern u32 sgmii_cmd_ro(void);

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
*************************************************************************
*/
int sgmii_api_get_test(ecnt_sgmii_data_t *);
ecnt_ret_val ecnt_sgmii_api_hook(struct ecnt_data *);

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
typedef int (*sgmii_api_op_t)(ecnt_sgmii_data_t *in_data);

static sgmii_api_op_t sgmii_operation[] = {
	sgmii_api_get_test
};

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/
int sgmii_api_get_test(ecnt_sgmii_data_t *in_data)
{
	printk("sgmii_api_get_test: in\n");
	u32 test = sgmii_cmd_ro();
	printk("test=%x\n",test);
	printk("sgmii_api_get_test: exit\n");
	return 0;
}



struct ecnt_hook_ops ecnt_sgmii_api_op = {
	.name = "sgmii_api_hook",
	.is_execute = 1,
	.hookfn = ecnt_sgmii_api_hook,
	.maintype = ECNT_SGMII,
	.subtype = ECNT_SGMII_API,
	.priority = 1
};

ecnt_ret_val ecnt_sgmii_api_hook(struct ecnt_data *in_data)
{
	printk("ecnt_sgmii_api_hook: in\n");
	ecnt_sgmii_data_t *data = (ecnt_sgmii_data_t *)in_data ;	
	
	if(data->function_id >= SGMII_FUNCTION_MAX_NUM) {
		printk("sgmii data->function_id is %d, exceed max number: %d", data->function_id, SGMII_FUNCTION_MAX_NUM);
 		return ECNT_HOOK_ERROR;
	}
	
	//spin_lock(&pcie_api_lock);
	sgmii_operation[data->function_id](data) ;
	//spin_unlock(&pcie_api_lock);
	printk("ecnt_sgmii_api_hook: exit\n");
	return ECNT_CONTINUE;
}


