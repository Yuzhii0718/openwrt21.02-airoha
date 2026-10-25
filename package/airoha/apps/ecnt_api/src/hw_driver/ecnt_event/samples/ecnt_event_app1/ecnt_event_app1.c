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
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <ecnt_event_global/ecnt_event_global.h>

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
static void down_action(struct ecnt_event_data *event_data);
static void up_action(struct ecnt_event_data *event_data);
static void change_action(struct ecnt_event_data *event_data);

/************************************************************************
*                  P U B L I C   D A T A
************************************************************************/

/************************************************************************
*                  P R I V A T E   D A T A
************************************************************************/
int event1_time = 0;
int event2_time = 0;
int event3_time = 0;

static struct ecnt_event_handle event_handle[] = {
	{
		.maintype	= ECNT_EVENT_DSL,
		.subtype	= ECNT_EVENT_DSL_DOWN,
		.handle		= down_action,
	},
	{
		.maintype	= ECNT_EVENT_XPON,
		.subtype	= ECNT_EVENT_DSL_UP,
		.handle		= up_action,
	},
	{
		.maintype	= ECNT_EVENT_USB,
		.subtype	= ECNT_EVENT_DSL_CHANGE,
		.handle		= change_action,
	},
	{
		.maintype	= ECNT_EVENT_MAX,
		.subtype	= ECNT_EVENT_DSL_MAX,
		.handle		= NULL,
	},
};


/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
************************************************************************/

static void down_action(struct ecnt_event_data *event_data)
{
	struct event1_data *data;
	data = (struct event1_data*)event_data;

	event1_time++;
	if(event1_time%100 == 0){
		printf("tony test=====> %s %s(%d) %x %x %s event1_time = %d\n", __FILE__, __FUNCTION__, __LINE__, 
			data->type, data->sub_type, data->message, event1_time);
	}
}

static void up_action(struct ecnt_event_data *event_data)
{
	struct event2_data *data;
	data = (struct event2_data*)event_data;

	event2_time++;
	if(event2_time%100 == 0){
	printf("tony test=====> %s %s(%d) %x %s event2_time = %d\n", __FILE__, __FUNCTION__, __LINE__, 
		data->status, data->message, event2_time);
	}
}

static void change_action(struct ecnt_event_data *event_data)
{
	struct event3_data *data;
	data = (struct event3_data*)event_data;

	event3_time++;
	if(event3_time%100 == 0){
		printf("tony test=====> %s %s(%d) %s event3_time = %d\n", __FILE__, __FUNCTION__, __LINE__,
			data->message, event3_time);
	}
}

int ecnt_event_hook(struct ecnt_data *in_data)
{
	return ecnt_event_execute(in_data, event_handle);
}

int main(void)
{   
	struct ecnt_event_source* source;
	
	source = ecnt_event_register("ecnt_event_app1", ecnt_event_hook);

	/*Do something  and need close event register*/
	sleep(60);
	ecnt_event_unregister(source);
	
    return 0;
}
