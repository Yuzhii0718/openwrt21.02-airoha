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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/dma-mapping.h>
#include <linux/mii.h>
#include <linux/kthread.h>
#include <asm/io.h>
#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>
#ifndef TCSUPPORT_CPU_ARMV8
#include <asm/tc3162/TCIfSetQuery_os.h>
#endif
#include "eth_lan.h"
#include "eth_led.h"
#include "./tcphy/phy_api.h"
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
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
typedef struct 
{
    char flag;
    char state;
}eth_up_down_t;

typedef struct
{
    eth_up_down_t port_state[4]; 
}eth_port_up_down_State;


#endif/*TCSUPPORT_COMPILE*/


/************************************************************************
*                  E X T E R N A L   D A T A   D E C L A R A T I O N S
*************************************************************************
*/
extern macAdapter_t *mac_p;
extern int g_port_reverse;
extern uint8 use_ext_switch;

extern int ext_switch_led_control;
/************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/
#ifdef TCSUPPORT_XPON_LED
static struct timer_list eth_led_timer;
static unsigned int eth_pkt_num[4] = {0};
static uint8 counter[4] = {0};
#endif


#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
eth_port_up_down_State g_eth_up_down = {{0}};
//pid_t up_down_pid  = 0;
static struct task_struct *updownThread = NULL;

typedef int (*eth_up_down_msg_callback)(int port, int eth_state);
eth_up_down_msg_callback mtk_xpon_eth_state_change_hook = NULL;
EXPORT_SYMBOL(mtk_xpon_eth_state_change_hook);

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_CF_ENV) && defined(TCSUPPORT_WAN_ETHER)
extern int wan_port_id;
#endif/*TCSUPPORT_COMPILE*/

#endif/*TCSUPPORT_COMPILE*/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

/************************************************************************
    Funtcion:
    Description:    
    Calls:
    Called by:      eth_led
    Input:
    Output:
    Return:
    Others:     
************************************************************************/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
static int eth_act_up_down_thread(void *arg)
	{
		int i = 0;	
//		daemonize("Up_Down for FH");
	
		while(1)
		{
			for(i=0; i<4; i++)
			{
				if((1 == g_eth_up_down.port_state[i].flag) && (1 == g_eth_up_down.port_state[i].state))
				{
					printk("down to up.\n");
					if(NULL != mtk_xpon_eth_state_change_hook)
					{
						mtk_xpon_eth_state_change_hook(i,1);
					}
				}
				else if((1 == g_eth_up_down.port_state[i].flag) && (0 == g_eth_up_down.port_state[i].state))
				{
					printk("up to down.\n");
					if(NULL != mtk_xpon_eth_state_change_hook)
					{
						mtk_xpon_eth_state_change_hook(i,0);
					}
				}
				g_eth_up_down.port_state[i].flag  = 0;
			}
	
			//1s 1000; msleep(1000) 
			//msleep(200); 
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(1 * HZ);
		}
	return 0;
	}
	
	static void up_down_port_work(void)
	{	
		if(!updownThread)
		{
			updownThread = kthread_run(eth_act_up_down_thread, NULL, "Up_Down for FH");
			if(IS_ERR(updownThread))
		{
			printk("creat up/down thread failed");
		}
		}		
		return ;
	}
	
	static void up_down_port_dework(void)
	{	 
	
		return ;
	}
#endif/*TCSUPPORT_COMPILE*/


#ifdef TCSUPPORT_XPON_LED
extern uint8 led_ether_up_status[];

static void switch_led_action(uint8 port, uint8 led_gpio_mode, uint8 led_num)
{
    uint8 switch_port;
    switch_port = macMT7530LanPortMap2Switch(port-1); 
    switch (led_gpio_mode)
    {
        case LED_GPIO_ON:            
            macMT7530LedOn(switch_port, led_num);
            break;
        case LED_GPIO_FAST_BLINK:
            counter[port-1] ++;
            if(counter[port-1] > LED_GPIO_FAST_BLINK_PERIOD)
                counter[port-1] -= LED_GPIO_FAST_BLINK_PERIOD;
            if((counter[port-1] <= LED_GPIO_FAST_BLINK_ON) && (counter[port-1] >= 1))
                macMT7530LedOn(switch_port, led_num);
            else
                macMT7530LedOff(switch_port, led_num);
            break;
        case LED_GPIO_SLOW_BLINK:
            counter[port-1] ++;
            if(counter[port-1] > LED_GPIO_SLOW_BLINK_PERIOD)
                counter[port-1] -= LED_GPIO_SLOW_BLINK_PERIOD;
            if((counter[port-1] <= LED_GPIO_SLOW_BLINK_ON) && (counter[port-1] >= 1))
                macMT7530LedOn(switch_port, led_num);
            else
                macMT7530LedOff(switch_port, led_num);
            break;
        case LED_GPIO_OFF:
            macMT7530LedOff(switch_port, led_num);
            counter[port-1] = 0;
            break;
        default:
            break;
    }
    return;
}

static void switch_led(uint8 port)
{
    uint8 switch_port = 0, lr_state = 0, lr_speed = 0;
    switch_port = macMT7530LanPortMap2Switch(port-1);
    macMT7530GetPortLinkState(switch_port, &lr_state, &lr_speed); 
    if(LED_GPIO_10 && LED_GPIO_100 && LED_GPIO_1000){
    if((lr_speed == 1) || (lr_speed == 6))  /*1000M*/
    {     
        switch_led_action(port, LED_GPIO_1000, GPIO_1000_LED);
    }
    else if((lr_speed == 2) || (lr_speed == 3)) /*100M*/
    {
        switch_led_action(port, LED_GPIO_100, GPIO_100_LED);
    }               
    else if((lr_speed == 4) || (lr_speed == 5)) /*10M*/
    {
        switch_led_action(port, LED_GPIO_10, GPIO_10_LED);
    }
    else /*link down*/
    {
        switch_led_action(port, LED_GPIO_OFF, GPIO_1000_LED);
        switch_led_action(port, LED_GPIO_OFF, GPIO_100_LED);
        switch_led_action(port, LED_GPIO_OFF, GPIO_10_LED);
    }
    }
    return;
}
void eth_led(TIMER_FUN_PAAM data){ 
    int i = 0;
    unsigned long pkts, pkts_diff ;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_TW_BOARD_DASAN)	
    uint8 lr_speed=0;
    uint8 lr_dplx=0;
#endif/*TCSUPPORT_COMPILE*/
    int tempPort = 0;
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)	
    int port_active[4] = {0};
    int switch_port = 0;
    static int port_active_last[4] = {0};
    extern int g_driver_lan_port_led[];
#endif/*TCSUPPORT_COMPILE*/
 
    for(i = 1; i<5;i++){
        if(isMT7520S && i != 4){
            continue;
        }
        if(use_ext_switch && ext_switch_led_control == DRIVER_CTL_LED)
            switch_led(i);
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_WAN_ETHER) && defined(TCSUPPORT_CF_ENV)
        if(wan_port_id != i)
#endif/*TCSUPPORT_COMPILE*/
        {
            switch_port = macMT7530LanPortMap2Switch(i-1);
            if(-1 == switch_port)
            {
                continue;
            }
        }
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_WAN_ETHER) && defined(TCSUPPORT_CF_ENV)
        else if(wan_port_id == i)
        {
            switch_port = 4;
        }
#endif/*TCSUPPORT_COMPILE*/

        //port_active 0/down, 1/up
        port_active[i-1] = macMT7530GetPortUpDownState(switch_port);
        if(g_driver_lan_port_led[i-1])
        {
            if(1 == port_active[i-1])
            {
                led_ether_up_status[i-1] = 2;
            }
            else
            {
                led_ether_up_status[i-1] = 0;
            }
        }
        if((port_active_last[i-1] != port_active[i-1]) && (1 == port_active[i-1]))
        {
            /*down to up*/
            g_eth_up_down.port_state[i-1].flag  = 1;
            g_eth_up_down.port_state[i-1].state = 1;
        }
        else if((port_active_last[i-1] != port_active[i-1]) && (0 == port_active[i-1]))
        {
            /*up to down*/
            g_eth_up_down.port_state[i-1].flag  = 1;
            g_eth_up_down.port_state[i-1].state = 0;
        }
        
        port_active_last[i-1] = port_active[i-1];
#endif/*TCSUPPORT_COMPILE*/
        pkts = eth_pkt_num[i-1];

        if(g_port_reverse == 1){
            tempPort = 4- i +1;         
            if (isMT7525G){
                eth_pkt_num[i-1] = gswPbusRead(EXT_GSW_RX_UNIC(tempPort-1)) + 
                    gswPbusRead(EXT_GSW_RX_MULC(tempPort-1)) + 
                    gswPbusRead(EXT_GSW_RX_BROC(tempPort-1)) +
                    gswPbusRead(EXT_GSW_TX_UNIC(tempPort-1)) + 
                    gswPbusRead(EXT_GSW_TX_MULC(tempPort-1)) +
                    gswPbusRead(EXT_GSW_TX_BROC(tempPort-1));
            }
            else
            {
                eth_pkt_num[i-1] = read_reg_word(GSW_RX_UNIC(tempPort)) + 
                    read_reg_word(GSW_RX_MULC(tempPort)) + 
                    read_reg_word(GSW_RX_BROC(tempPort)) + 
                    read_reg_word(GSW_TX_UNIC(tempPort)) +
                    read_reg_word(GSW_TX_MULC(tempPort)) +
                    read_reg_word(GSW_TX_BROC(tempPort));
            }
        
        }else{
            if (isMT7525G){
                eth_pkt_num[i-1] = gswPbusRead(EXT_GSW_RX_UNIC(i-1)) + 
                    gswPbusRead(EXT_GSW_RX_MULC(i-1)) + 
                    gswPbusRead(EXT_GSW_RX_BROC(i-1)) + 
                    gswPbusRead(EXT_GSW_TX_UNIC(i-1)) + 
                    gswPbusRead(EXT_GSW_TX_MULC(i-1)) + 
                    gswPbusRead(EXT_GSW_TX_BROC(i-1));
            }
            else
            {
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
                eth_pkt_num[i-1] = read_reg_word(GSW_RX_UNIC(switch_port))+read_reg_word(GSW_RX_MULC(switch_port))+read_reg_word(GSW_RX_BROC(switch_port))
                          + read_reg_word(GSW_TX_UNIC(switch_port))+read_reg_word(GSW_TX_MULC(switch_port))+read_reg_word(GSW_TX_BROC(switch_port));  
#else/*TCSUPPORT_COMPILE*/	
                eth_pkt_num[i-1] = read_reg_word(GSW_RX_UNIC(i))+read_reg_word(GSW_RX_MULC(i))+read_reg_word(GSW_RX_BROC(i))
                          + read_reg_word(GSW_TX_UNIC(i))+read_reg_word(GSW_TX_MULC(i))+read_reg_word(GSW_TX_BROC(i));
#endif/*TCSUPPORT_COMPILE*/
            }
        }
        pkts_diff = eth_pkt_num[i-1] - pkts;

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_TW_BOARD_CDS)
        if (led_ether_up_status[i-1] == 2){ //  LINK up
            getEthLinkSpeed(i-1, &lr_speed, &lr_dplx);  
            if (pkts_diff > 0){ /* BLINK*/
                if(lr_speed == 3)
                {                   
                    ledTurnOff(LED_ETHER_PORT1_ACT_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_1000M_ACT_STATUS + 2*(i-1)); 
                    ledTurnOff(LED_ETHER_PORT1_1000M_STATUS + 2*(i-1)); 
                    ledTurnOn(LED_ETHER_PORT1_10M_ACT_STATUS+2*(i-1));
                    ledTurnOff(LED_ETHER_PORT1_10M_STATUS + 2*(i-1));                   
                }
                else if(lr_speed == 2)
                {
                    ledTurnOff(LED_ETHER_PORT1_10M_ACT_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_10M_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_1000M_ACT_STATUS + 2*(i-1)); 
                    ledTurnOff(LED_ETHER_PORT1_1000M_STATUS + 2*(i-1)); 
                    ledTurnOn(LED_ETHER_PORT1_ACT_STATUS+2*(i-1));
                    ledTurnOff(LED_ETHER_PORT1_STATUS + 2*(i-1));   
                    
                }               
                else if(lr_speed == 1)
                {
                    ledTurnOff(LED_ETHER_PORT1_10M_ACT_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_10M_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_ACT_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_STATUS + 2*(i-1));                   
                    ledTurnOn(LED_ETHER_PORT1_1000M_ACT_STATUS+2*(i-1));
                    ledTurnOff(LED_ETHER_PORT1_1000M_STATUS + 2*(i-1));                 
                }
            }else
            { /* ON     */
                if(lr_speed == 3)
                {
                    ledTurnOff(LED_ETHER_PORT1_ACT_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_1000M_ACT_STATUS + 2*(i-1)); 
                    ledTurnOff(LED_ETHER_PORT1_1000M_STATUS + 2*(i-1)); 
                    ledTurnOff(LED_ETHER_PORT1_10M_ACT_STATUS+2*(i-1));
                    ledTurnOn(LED_ETHER_PORT1_10M_STATUS + 2*(i-1));                    
                }
                else if(lr_speed == 2)
                {
                    ledTurnOff(LED_ETHER_PORT1_10M_ACT_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_10M_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_1000M_ACT_STATUS + 2*(i-1)); 
                    ledTurnOff(LED_ETHER_PORT1_1000M_STATUS + 2*(i-1));                 
                    ledTurnOff(LED_ETHER_PORT1_ACT_STATUS+2*(i-1));
                    ledTurnOn(LED_ETHER_PORT1_STATUS + 2*(i-1));                                
                }                   
                else if(lr_speed == 1)
                {   
                    ledTurnOff(LED_ETHER_PORT1_10M_ACT_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_10M_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_ACT_STATUS + 2*(i-1));   
                    ledTurnOff(LED_ETHER_PORT1_STATUS + 2*(i-1));                                   
                    ledTurnOff(LED_ETHER_PORT1_1000M_ACT_STATUS+2*(i-1));
                    ledTurnOn(LED_ETHER_PORT1_1000M_STATUS + 2*(i-1));                  
                }           
            }

            if(lr_dplx == 0)
            {
                ledTurnOff(LED_DUX1_HD+2*(i-1));    
                ledTurnOn(LED_DUX1_FD+2*(i-1));
            }
            else
            {
                ledTurnOff(LED_DUX1_FD+2*(i-1));
                ledTurnOn(LED_DUX1_HD+2*(i-1));                 
            }   
        }else if (
            led_ether_up_status[i-1] == 0){ /* LINK DOWN*/
            ledTurnOff(LED_ETHER_PORT1_10M_ACT_STATUS+2*(i-1));
            ledTurnOff(LED_ETHER_PORT1_10M_STATUS + 2*(i-1));       
            ledTurnOff(LED_ETHER_PORT1_ACT_STATUS+2*(i-1));
            ledTurnOff(LED_ETHER_PORT1_STATUS + 2*(i-1));               
            ledTurnOff(LED_ETHER_PORT1_1000M_ACT_STATUS+2*(i-1));
            ledTurnOff(LED_ETHER_PORT1_1000M_STATUS + 2*(i-1));             
            ledTurnOff(LED_DUX1_FD+2*(i-1));
            ledTurnOff(LED_DUX1_HD+2*(i-1));                            
        }       
#else/*TCSUPPORT_COMPILE*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
    if(g_driver_lan_port_led[i-1])
#endif/*TCSUPPORT_COMPILE*/
   {
        if (led_ether_up_status[i-1] == 2){ /*  LINK up*/
            if (pkts_diff > 0){ /* BLINK*/
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)			
                if (isMT7525) {
                    ledTurnOn(LED_ETHER_PORT3_ACT_STATUS+2*(i-1));
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C5_XPON_AUTH_LED)				
                ledTurnOn(LED_ETHER_PORT3_STATUS + 2*(i-1));
#else/*TCSUPPORT_COMPILE*/
                ledTurnOff(LED_ETHER_PORT3_STATUS + 2*(i-1));
#endif/*TCSUPPORT_COMPILE*/			
                } else 
#endif/*TCSUPPORT_COMPILE*/				
                {
                ledTurnOn(LED_ETHER_PORT1_ACT_STATUS+2*(i-1));
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C5_XPON_AUTH_LED)				
                ledTurnOn(LED_ETHER_PORT1_STATUS + 2*(i-1));
#else/*TCSUPPORT_COMPILE*/
                ledTurnOff(LED_ETHER_PORT1_STATUS + 2*(i-1));
#endif/*TCSUPPORT_COMPILE*/	
}       
            }else{ // ON
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)			
                if (isMT7525) {
                    ledTurnOff(LED_ETHER_PORT3_ACT_STATUS+2*(i-1));
                    ledTurnOn(LED_ETHER_PORT3_STATUS + 2*(i-1));
                } else 
#endif/*TCSUPPORT_COMPILE*/				
                {
                ledTurnOff(LED_ETHER_PORT1_ACT_STATUS+2*(i-1));
                ledTurnOn(LED_ETHER_PORT1_STATUS + 2*(i-1));
            }
            }
        }else if (led_ether_up_status[i-1] == 0){ // LINK DOWN
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)		
            if (isMT7525) {
                ledTurnOff(LED_ETHER_PORT3_ACT_STATUS+2*(i-1));
                ledTurnOff(LED_ETHER_PORT3_STATUS + 2*(i-1));
            } else 
#endif/*TCSUPPORT_COMPILE*/			
            {       
            ledTurnOff(LED_ETHER_PORT1_ACT_STATUS+2*(i-1));
            ledTurnOff(LED_ETHER_PORT1_STATUS + 2*(i-1));
}
        }
#endif/*TCSUPPORT_COMPILE*/
    }

    }
    /* Schedule for the next time */
    eth_led_timer.expires = jiffies + msecs_to_jiffies(250);
    add_timer(&eth_led_timer);
}
#endif

inline void eth_led_tx_hook(uint8 port_mask)
{
    /***************** light the led *****************/	
  	ledTurnOn(LED_ETHER_ACT_STATUS);
	if (mac_p->macPhyLinkProfile_p->enetMode & LAN_ST_100MB)
		ledTurnOn(LED_ETHER_100M_ACT_STATUS);
	else
		ledTurnOn(LED_ETHER_10M_ACT_STATUS);
#ifdef TCSUPPORT_XPON_LED
	if (isMT7520S){
		if (port_mask & (1<<4) && led_ether_up_status[3] == 2){
			ledTurnOn(LED_ETHER_PORT4_ACT_STATUS);
		}
	}else if (isMT7525 || isMT7520){
		int i = 0;
		for (i = 0; i < 4; i++){
			if ((port_mask & (1<<(i+1))) && led_ether_up_status[i] ==2){
				ledTurnOn(LED_ETHER_PORT1_ACT_STATUS + 2*i);
			}
		}
	}
#endif
}
EXPORT_SYMBOL(eth_led_tx_hook);

inline void eth_led_rx_hook(uint8 pvid)
{
#ifdef TCSUPPORT_XPON_LED

	if (isMT7520S && pvid == 0){
		if (led_ether_up_status[3] == 2){
			ledTurnOn(LED_ETHER_PORT4_ACT_STATUS);
		}
	}else if ((isMT7525 || isMT7520) && (pvid < 4)){
		if (led_ether_up_status[pvid] == 2){
			ledTurnOn(LED_ETHER_PORT1_ACT_STATUS + 2*(pvid));
		}
	}	
#endif
}
EXPORT_SYMBOL(eth_led_rx_hook);

void eth_led_mode_init(void)
{
    u32 led_mode_val;

    //config led mode    
    led_mode_val = ((P4_LED2_MODE | P4_LED1_MODE | P4_LED0_MODE)<<P4_LED_MODE_SHIFT)|
        ((P3_LED2_MODE | P3_LED1_MODE | P3_LED0_MODE)<<P3_LED_MODE_SHIFT)|
        ((P2_LED2_MODE | P2_LED1_MODE | P2_LED0_MODE)<<P2_LED_MODE_SHIFT)|
        ((P1_LED2_MODE | P1_LED1_MODE | P1_LED0_MODE)<<P1_LED_MODE_SHIFT)|
        ((P0_LED2_MODE | P0_LED1_MODE | P0_LED0_MODE)<<P0_LED_MODE_SHIFT);
    gswPbusWrite(LED_IO_MODE_CTRL_REG, led_mode_val);

    //config led hw mode
    if(P0_LED0_MODE | P1_LED0_MODE | P2_LED0_MODE | P3_LED0_MODE | P4_LED0_MODE)
    {
        mdio_cl22_write(0, 0x1f, LED0_ON_CTRL_REG, LED0_ON_CTRL);
        mdio_cl22_write(0, 0x1f, LED0_BLINK_CTRL_REG, LED0_BLK_CTRL);
    }
    
    if(P0_LED1_MODE | P1_LED1_MODE | P2_LED1_MODE | P3_LED1_MODE | P4_LED1_MODE)
    {
        mdio_cl22_write(0, 0x1f, LED1_ON_CTRL_REG, LED1_ON_CTRL);
        mdio_cl22_write(0, 0x1f, LED1_BLINK_CTRL_REG, LED1_BLK_CTRL);
    }
    
    if(P0_LED2_MODE | P1_LED2_MODE | P2_LED2_MODE | P3_LED2_MODE | P4_LED2_MODE)
    {
        mdio_cl22_write(0, 0x1f, LED2_ON_CTRL_REG, LED2_ON_CTRL);
        mdio_cl22_write(0, 0x1f, LED2_BLINK_CTRL_REG, LED2_BLK_CTRL);
    }

    //config led gpio mode
    
    //gpio mode set output mode
    gswPbusWrite(LED_GPIO_IO_MODE_REG, ~led_mode_val);
    
    //gpio output enable
    gswPbusWrite(LED_GPIO_OUT_ENABLE_REG, ~led_mode_val);
}

void eth_led_init(void)
{
    if(use_ext_switch)
        eth_led_mode_init();
#ifdef TCSUPPORT_XPON_LED
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    init_timer(&eth_led_timer);
    eth_led_timer.expires = jiffies + msecs_to_jiffies(250);
    eth_led_timer.function = eth_led;
    eth_led_timer.data = 0;
#else
	timer_setup(&eth_led_timer,eth_led,0);
	eth_led_timer.expires = jiffies + msecs_to_jiffies(250);
#endif
    add_timer(&eth_led_timer);
    memset(&eth_pkt_num, 0, 4*sizeof(unsigned int));
#endif	
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
    up_down_port_work();
#endif/*TCSUPPORT_COMPILE*/	
}


void eth_led_deinit(void)
{
#ifdef TCSUPPORT_XPON_LED
    del_timer_sync(&eth_led_timer);
#endif	
#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_XPON_HAL_API_EXT)
    up_down_port_dework();
#endif/*TCSUPPORT_COMPILE*/

}
