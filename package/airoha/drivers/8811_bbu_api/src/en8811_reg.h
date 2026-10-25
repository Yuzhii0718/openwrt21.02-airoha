/* FILE NAME:  en8811_reg.h
 * PURPOSE:
 *    It provides en8811 phy register definations.
 *
 * NOTES:
 *
 */

//define const value
#ifndef _EN8811_REG_H
#define _EN8811_REG_H

typedef enum {
    EN8811_ERROR = -1,
    EN8811_CONTINUE = 1
}EN8811_return_t;

typedef enum {
    EN8811_LINK_DOWN = 0,
    EN8811_2P5G = 2,
    EN8811_1G   = 3,
    EN8811_100M = 4
}EN8811_SPEED_INDEX;


#define    EN8811_READ  0
#define    EN8811_WRITE 1
#define    EN8811_WRITE_0 0
#define    EN8811_WRITE_1 1

//auto-negotation or force 
#define EN8811_AUTONEGO_REG 0       //reg0
#define EN8811_AUTONEGO_BIT (1<<12) //reg0
#define EN8811_AN_COMPLETE_REG  1     //reg1
#define EN8811_AN_COMPLETE_BIT (1<<5) //reg1

//restart an mode
#define EN8811_RestartAN_REG 0      //reg0
#define EN8811_RestartAN_BIT (1<<9) //reg0

//full duplex or hslf duplex
#define EN8811_DUPLEX_REG 0      //reg0
#define EN8811_DUPLEX_BIT (1<<8) //reg0
#define EN8811_FULL_DUPLEX 1
#define EN8811_HALF_DUPLEX 0

//status
#define EN8811_STATUS_REG 1      //reg0x1
#define EN8811_STATUS_BIT (1<<2) //reg0x1

//flow control
#define EN8811_FC_REG 4          //reg4
#define EN8811_FC_BIT (1<<10)    //reg4

//speed of autonego mode
#define EN8811_SPEED_100M_REG1 4      //reg4
#define EN8811_SPEED_100M_REG2 5      //reg5
#define EN8811_SPEED_100M_BIT (1<<8)  //reg4,reg5
    
#define EN8811_SPEED_1000M_REG1 9        //reg9
#define EN8811_SPEED_1000M_BIT1 (1<<9)  //reg9
#define EN8811_SPEED_1000M_REG2 10       //reg10
#define EN8811_SPEED_1000M_BIT2 (1<<11) //reg10
    
#define EN8811_SPEED_2500M_REG_BUCKPBUS 0x109D4
#define EN8811_SPEED_2500M_BIT (1<<4) //reg24

//speed of force mode    
#define EN8811_SPEED_FORCE_REG 0        //reg0
#define EN8811_SPEED_FORCE_BIT1 (1<<6)  //reg0
#define EN8811_SPEED_FORCE_BIT2 (1<<13) //reg0

#define EN8811_FIRMWARE_REG_BUCKPBUS 0x3B3C

//an mode to advertise
#define EN8811_ADV_2500M_REG_CL45_DEV 0x07 
#define EN8811_ADV_2500M_REG_CL45_REG 0x20
#define EN8811_ADV_2500M_BIT          (1<<7)

#define EN8811_ADV_1000M_REG 9
#define EN8811_ADV_1000M_BIT (1<<9)

#define EN8811_ADV_100M_REG 4
#define EN8811_ADV_100M_BIT (1<<8)

#endif
