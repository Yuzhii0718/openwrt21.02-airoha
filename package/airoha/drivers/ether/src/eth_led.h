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
#ifndef _ETH_LED_H
#define _ETH_LED_H

/************************************************************************
*               I N C L U D E S
*************************************************************************
*/

/************************************************************************
*               D E F I N E S   &   C O N S T A N T S
*************************************************************************
*/
#define USER_CTL_LED 0
#define DRIVER_CTL_LED 1

#define LED_HW_MODE 1
#define LED_GPIO_MODE 0

#define P0_LED_MODE_SHIFT   0
#define P1_LED_MODE_SHIFT   4
#define P2_LED_MODE_SHIFT   8
#define P3_LED_MODE_SHIFT   12
#define P4_LED_MODE_SHIFT   16

#define LED0_ON_CTRL_REG     (0x24)
#define LED0_BLINK_CTRL_REG  (0x25)
#define LED1_ON_CTRL_REG     (0x26)
#define LED1_BLINK_CTRL_REG  (0x27)
#define LED2_ON_CTRL_REG     (0x28)
#define LED2_BLINK_CTRL_REG  (0x29)

#define LED_IO_ENABLE_REG       (0x7D00)
#define LED_IO_MODE_CTRL_REG    (0x7D04)
#define LED_GPIO_IO_MODE_REG    (0x7D10)
#define LED_GPIO_OUT_ENABLE_REG (0x7D14)
#define LED_GPIO_DARA_REG       (0x7D18)

#define LED_GPIO_ON             1
#define LED_GPIO_FAST_BLINK     2
#define LED_GPIO_SLOW_BLINK     3
#define LED_GPIO_OFF            4
#define LED_GPIO_NONE           0

#define LED0    0
#define LED1    1
#define LED2    2

/************************************************************************
*               M A C R O S
*************************************************************************
*/
#define P0_LED0_MODE   LED_HW_MODE
#define P1_LED0_MODE   LED_HW_MODE
#define P2_LED0_MODE   LED_HW_MODE
#define P3_LED0_MODE   LED_HW_MODE
#define P4_LED0_MODE   LED_HW_MODE
#define P0_LED1_MODE   (LED_HW_MODE<<1)
#define P1_LED1_MODE   (LED_HW_MODE<<1)
#define P2_LED1_MODE   (LED_HW_MODE<<1)
#define P3_LED1_MODE   (LED_HW_MODE<<1)
#define P4_LED1_MODE   (LED_HW_MODE<<1)
#define P0_LED2_MODE   (LED_HW_MODE<<2)
#define P1_LED2_MODE   (LED_HW_MODE<<2)
#define P2_LED2_MODE   (LED_HW_MODE<<2)
#define P3_LED2_MODE   (LED_HW_MODE<<2)
#define P4_LED2_MODE   (LED_HW_MODE<<2)

#define LED0_EN             (1<<15)
#define LED0_POLARITY       (1<<14)
#define LED0_FORCE_ON       (0<<6)
#define LED0_HALF_DUPLEX_ON (0<<5)
#define LED0_FULL_DUPLEX_ON (0<<4)
#define LED0_LINK_DOWN_ON   (0<<3)
#define LED0_LINK_10_ON     (1<<2)
#define LED0_LINK_100_ON    (1<<1)
#define LED0_LINK_1000_ON   (1<<0)

#define LED0_BLK_FORCE       (0<<9)
#define LED0_BLK_RX_IDLE     (0<<8)
#define LED0_BLK_RX_CRC      (0<<7)
#define LED0_BLK_COLLISION   (0<<6)
#define LED0_BLK_10_RX       (1<<5)
#define LED0_BLK_10_TX       (1<<4)
#define LED0_BLK_100_RX      (1<<3)
#define LED0_BLK_100_TX      (1<<2)
#define LED0_BLK_1000_RX     (1<<1)
#define LED0_BLK_1000_TX     (1<<0)





#define LED1_EN             (1<<15)
#define LED1_POLARITY       (1<<14)
#define LED1_FORCE_ON       (0<<6)
#define LED1_HALF_DUPLEX_ON (0<<5)
#define LED1_FULL_DUPLEX_ON (0<<4)
#define LED1_LINK_DOWN_ON   (0<<3)
#define LED1_LINK_10_ON     (1<<2)
#define LED1_LINK_100_ON    (1<<1)
#define LED1_LINK_1000_ON   (0<<0)

#define LED1_BLK_FORCE       (0<<9)
#define LED1_BLK_RX_IDLE     (0<<8)
#define LED1_BLK_RX_CRC      (0<<7)
#define LED1_BLK_COLLISION   (0<<6)
#define LED1_BLK_10_RX       (1<<5)
#define LED1_BLK_10_TX       (1<<4)
#define LED1_BLK_100_RX      (1<<3)
#define LED1_BLK_100_TX      (1<<2)
#define LED1_BLK_1000_RX     (0<<1)
#define LED1_BLK_1000_TX     (0<<0)


#define LED2_EN             (1<<15)
#define LED2_POLARITY       (1<<14)
#define LED2_FORCE_ON       (0<<6)
#define LED2_HALF_DUPLEX_ON (0<<5)
#define LED2_FULL_DUPLEX_ON (0<<4)
#define LED2_LINK_DOWN_ON   (0<<3)
#define LED2_LINK_10_ON     (0<<2)
#define LED2_LINK_100_ON    (0<<1)
#define LED2_LINK_1000_ON   (1<<0)

#define LED2_BLK_FORCE       (0<<9)
#define LED2_BLK_RX_IDLE     (0<<8)
#define LED2_BLK_RX_CRC      (0<<7)
#define LED2_BLK_COLLISION   (0<<6)
#define LED2_BLK_10_RX       (0<<5)
#define LED2_BLK_10_TX       (0<<4)
#define LED2_BLK_100_RX      (0<<3)
#define LED2_BLK_100_TX      (0<<2)
#define LED2_BLK_1000_RX     (1<<1)
#define LED2_BLK_1000_TX     (1<<0)

#define GPIO_1000_LED           LED0         
#define GPIO_100_LED            LED0
#define GPIO_10_LED             LED0

#define LED_GPIO_1000           LED_GPIO_NONE
#define LED_GPIO_100            LED_GPIO_NONE
#define LED_GPIO_10             LED_GPIO_NONE

#define LED_GPIO_FAST_BLINK_ON     2    
#define LED_GPIO_FAST_BLINK_PERIOD 4
#define LED_GPIO_SLOW_BLINK_ON     4
#define LED_GPIO_SLOW_BLINK_PERIOD 8

#define LED0_ON_CTRL (LED0_EN | LED0_POLARITY | LED0_LINK_1000_ON |     \
    LED0_LINK_100_ON | LED0_LINK_10_ON | LED0_FULL_DUPLEX_ON |          \
    LED0_HALF_DUPLEX_ON | LED0_FORCE_ON)
    
#define LED0_BLK_CTRL (LED0_BLK_1000_TX | LED0_BLK_1000_RX | LED0_BLK_100_TX |      \
    LED0_BLK_100_RX | LED0_BLK_10_TX | LED0_BLK_10_RX | LED0_BLK_COLLISION |        \
    LED0_BLK_RX_CRC | LED0_BLK_RX_IDLE | LED0_BLK_FORCE)
    
#define LED1_ON_CTRL (LED1_EN | LED1_POLARITY | LED1_LINK_1000_ON |     \
    LED1_LINK_100_ON | LED1_LINK_10_ON | LED1_FULL_DUPLEX_ON |          \
    LED1_HALF_DUPLEX_ON | LED1_FORCE_ON)
    
#define LED1_BLK_CTRL (LED1_BLK_1000_TX | LED1_BLK_1000_RX | LED1_BLK_100_TX |      \ 
    LED1_BLK_100_RX | LED1_BLK_10_TX | LED1_BLK_10_RX | LED1_BLK_COLLISION |        \
    LED1_BLK_RX_CRC | LED1_BLK_RX_IDLE | LED1_BLK_FORCE)
    
#define LED2_ON_CTRL (LED2_EN | LED2_POLARITY | LED2_LINK_1000_ON |     \
    LED2_LINK_100_ON | LED2_LINK_10_ON | LED2_FULL_DUPLEX_ON |          \
    LED2_HALF_DUPLEX_ON | LED2_FORCE_ON)
    
#define LED2_BLK_CTRL (LED2_BLK_1000_TX | LED2_BLK_1000_RX | LED2_BLK_100_TX |      \
    LED2_BLK_100_RX | LED2_BLK_10_TX | LED2_BLK_10_RX | LED2_BLK_COLLISION |        \
    LED2_BLK_RX_CRC | LED2_BLK_RX_IDLE | LED2_BLK_FORCE)

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
void eth_led(TIMER_FUN_PAAM data);
void eth_led_init(void);
void eth_led_deinit(void);



#endif /*_ETH_LED_H*/
