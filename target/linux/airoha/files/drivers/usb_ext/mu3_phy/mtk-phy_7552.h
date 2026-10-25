#ifndef __MTK_PHY_NEW_H
#define __MTK_PHY_NEW_H

/* include system library */
#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/delay.h>

//#define CONFIG_U3D_HAL_SUPPORT
#ifdef CONFIG_U3D_HAL_SUPPORT
#include "mu3d_hal_hw.h"
#endif
#ifdef CONFIG_U3D_HAL_SUPPORT
#define REF_CK U3D_PHY_REF_CK
#else
#define REF_CK 20
#endif

/* CONSTANT DEFINE */
#define U2_SR_COEF                      (28)
#define HSTX_SRCTRL_MEASURE             (0x5)

//AN7552 USB init ADDR
#define RG_SSUSB_CDR_RST_DLY_ADDR       0x0b20
#define RG_SSUSB_CDR_RST_RESERVE_ADDR   0x0b18
#define RG_SSUSB_BG_DIV_ADDR            0x0b00
#define RG_SSUSB_XTAL_TOP_RESERVE_ADDR  0x0b04

#define RG_USB20_BC11_SW_EN_ADDR        0x0318
#define RG_USBPHYACR4_ADDR              0x0310
#define RG_USBPHYACR6_ADDR              0x0318

//AN7552 power up/down
#define RG_c60802_gpio_ctlc_ADDR        0x080c
#define RG_c60802_gpio_ctlc_MASK        0xA0000000
#define RG_c60802_gpio_ctlc_OFST        0x0



//AN7552 USB init MASK
#define RG_SSUSB_CDR_RST_DLY_MASK       (0x03)
#define RG_SSUSB_CDR_RST_RESERVE_MASK   (0xFF)
#define RG_SSUSB_BG_DIV_MASK            (0x03)
#define RG_SSUSB_XTAL_TOP_RESERVE_MASK  (0xFFFF)

#define RG_USB20_BC11_SW_MASK           (0x01) 
#define RG_USB20_FS_CR_MASK             (0x07)
#define RG_USB20_FS_SR_MASK             (0x07)
#define RG_USB20_SQTH_MASK              (0xF)
#define RG_USB20_DISCTH_MASK            (0xF)

//AN7552 USB init OFFSET
#define RG_SSUSB_CDR_RST_DLY_OFST       (06)
#define RG_SSUSB_CDR_RST_RESERVE_OFST   (24)
#define RG_SSUSB_BG_DIV_OFST            (28)
#define RG_SSUSB_XTAL_TOP_RESERVE_OFST  (10)

#define RG_USB20_BC11_SW_EN_OFST        (23)
#define RG_USB20_FS_CR_OFST             (0x08)
#define RG_USB20_FS_SR_OFST             (0x00)
#define RG_USB20_SQTH_OFST              (0)
#define RG_USB20_DISCTH_OFST            (4)


/*EN7523 RG location used for u2 slew rate calibration*/
#define RG_USB20_HSTX_SRCAL_EN_ADDR     0x0314
#define RG_USB20_HSTX_SRCTRL_ADDR       0x0314
#define RG_FRCK_EN_ADDR                 0x0110
#define RG_CYCLECNT_ADDR                0x0100
#define RG_FREQDET_EN_ADDR              0x0100
#define RG_MONCLK_SEL_ADDR              0x0100
#define FM_OUT_ADDR                     0x010c

//EN7523 USB init
#define RG_USB20_INTR_EN_ADDR           0x0300
#define RG_SSUSB_IEXT_INTR_CTRL_ADDR    0x0b00
#define RG_USB20_HS_100U_U3_EN_ADDR     0x0314
#define RG_USB20_INTR_CAL_ADDR          0x0304
#define RG_SSUSB_INTR_EN_ADDR           0x0b00
#define RG_SSUSB_LN0_TX_IMP_SEL_ADDR    0x0910
#define RG_SSUSB_LN0_RX_IMP_SEL_ADDR    0x0914

//EN7523 K slew rate
#define RG_USB20_HSTX_SRCAL_EN          (0x1) //15:15
#define RG_USB20_HSTX_SRCTRL            (0x7) //14:12
#define RG_MONCLK_SEL                   (0x3) //27:26

#define RG_USB20_HSTX_SRCAL_EN_OFST     (15)
#define RG_USB20_HSTX_SRCTRL_OFST       (12)
#define RG_MONCLK_SEL_OFST              (26)

//20MHz PLL setting
#define rg_SCU_Xtal_SEL                 0x254
#define RG_FM_MODE                      (0x1) //25:25
#define RG_FREQDET_EN                   (0x1) //24:24
#define RG_CYCLECNT                     (0xffffff) //23:0
#define RG_FRCK_EN                      (0x1) //8:8
#define RG_USB20_INTR_EN                (0x1) //5:5
#define RG_SSUSB_IEXT_INTR_CTRL         (0x3f) //15:10
#define RG_USB20_HS_100U_U3_EN          (0x1) //11:11
#define RG_USB20_INTR_CAL               (0x1f) //23:19
#define RG_SSUSB_INTR_EN                (0x1) //26:26
#define RG_SSUSB_LN0_TX_IMP_SEL         (0x1f) //28:24
#define RG_SSUSB_LN0_RX_IMP_SEL         (0x1f) //28:24
#define RG_SSUSB_IDEM_3p5db             (0x3f) //21:16
#define RG_SSUSB_FORCE_IDEM_3p5db       (0x1) //22:22

/*offset*/
#define RG_FM_MODE_OFST                 (25)
#define RG_FREQDET_EN_OFST              (24)
#define RG_CYCLECNT_OFST                (0)
#define RG_FRCK_EN_OFST                 (8)
#define RG_USB20_INTR_EN_OFST           (5)
#define RG_SSUSB_IEXT_INTR_CTRL_OFST    (10)
#define RG_USB20_HS_100U_U3_EN_OFST     (11)
#define RG_USB20_INTR_CAL_OFST          (19)
#define RG_SSUSB_INTR_EN_OFST           (26)
#define RG_SSUSB_LN0_TX_IMP_SEL_OFST    (24)
#define RG_SSUSB_LN0_RX_IMP_SEL_OFST    (24)
#define RG_SSUSB_IDEM_3p5db_OFST        (16) 
#define RG_SSUSB_FORCE_IDEM_3p5db_OFST  (22) 

#endif
