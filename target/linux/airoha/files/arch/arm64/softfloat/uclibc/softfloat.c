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
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <net/protocol.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/wait.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/if.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <net/net_namespace.h>
#include <net/sock.h>
#include <ecnt_hook/ecnt_hook.h>

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
*************************************************************************
*/


/************************************************************************
*                  P U B L I C   D A T A
*************************************************************************
*/
/* _arm_negdf2.o */
extern void __aeabi_dneg(void);
EXPORT_SYMBOL(__aeabi_dneg);

/* _arm_addsubdf3.o */
extern void __aeabi_dadd(void);
extern void __aeabi_drsub(void);
extern void __aeabi_dsub(void);
extern void __aeabi_f2d(void);
extern void __aeabi_i2d(void);
extern void __aeabi_l2d(void);
extern void __aeabi_ui2d(void);
extern void __aeabi_ul2d(void);
EXPORT_SYMBOL(__aeabi_dadd);
EXPORT_SYMBOL(__aeabi_drsub);
EXPORT_SYMBOL(__aeabi_dsub);
EXPORT_SYMBOL(__aeabi_f2d);
EXPORT_SYMBOL(__aeabi_i2d);
EXPORT_SYMBOL(__aeabi_l2d);
EXPORT_SYMBOL(__aeabi_ui2d);
EXPORT_SYMBOL(__aeabi_ul2d);

/* _arm_muldivdf3.o */
extern void __aeabi_ddiv(void);
extern void __aeabi_dmul(void);
EXPORT_SYMBOL(__aeabi_ddiv);
EXPORT_SYMBOL(__aeabi_dmul);

/* _arm_cmpdf2.o  */
extern void __aeabi_cdcmpeq(void);
extern void __aeabi_cdcmple(void);
extern void __aeabi_cdrcmple(void);
extern void __aeabi_dcmpeq(void);
extern void __aeabi_dcmpge(void);
extern void __aeabi_dcmpgt(void);
extern void __aeabi_dcmple(void);
extern void __aeabi_dcmplt(void);
EXPORT_SYMBOL(__aeabi_cdcmpeq);
EXPORT_SYMBOL(__aeabi_cdcmple);
EXPORT_SYMBOL(__aeabi_cdrcmple);
EXPORT_SYMBOL(__aeabi_dcmpeq);
EXPORT_SYMBOL(__aeabi_dcmpge);
EXPORT_SYMBOL(__aeabi_dcmpgt);
EXPORT_SYMBOL(__aeabi_dcmple);
EXPORT_SYMBOL(__aeabi_dcmplt);

/* _arm_unorddf2.o */
extern void __aeabi_dcmpun(void);
EXPORT_SYMBOL(__aeabi_dcmpun);

/* _arm_fixdfsi.o */
extern void __aeabi_d2iz(void);
EXPORT_SYMBOL(__aeabi_d2iz);

/* _arm_fixunsdfsi.o */
extern void __aeabi_d2uiz(void);
EXPORT_SYMBOL(__aeabi_d2uiz);

/* _arm_truncdfsf2.o */
extern void __aeabi_d2f(void);
EXPORT_SYMBOL(__aeabi_d2f);

/* _arm_addsubsf3.o */
extern void __aeabi_fadd(void);
extern void __aeabi_frsub(void);
extern void __aeabi_fsub(void);
extern void __aeabi_i2f(void);
extern void __aeabi_l2f(void);
extern void __aeabi_ui2f(void);
extern void __aeabi_ul2f(void);
EXPORT_SYMBOL(__aeabi_fadd);
EXPORT_SYMBOL(__aeabi_frsub);
EXPORT_SYMBOL(__aeabi_fsub);
EXPORT_SYMBOL(__aeabi_i2f);
EXPORT_SYMBOL(__aeabi_l2f);
EXPORT_SYMBOL(__aeabi_ui2f);
EXPORT_SYMBOL(__aeabi_ul2f);

/* _arm_negsf2.o */
extern void __aeabi_fneg(void);
EXPORT_SYMBOL(__aeabi_fneg);

/* _arm_muldivsf3.o */
extern void __aeabi_fdiv(void);
extern void __aeabi_fmul(void);
EXPORT_SYMBOL(__aeabi_fdiv);
EXPORT_SYMBOL(__aeabi_fmul);

/* _arm_cmpsf2.o */
extern void __aeabi_cfcmpeq(void);
extern void __aeabi_cfcmple(void);
extern void __aeabi_cfrcmple(void);
extern void __aeabi_fcmpeq(void);
extern void __aeabi_fcmpge(void);
extern void __aeabi_fcmpgt(void);
extern void __aeabi_fcmple(void);
extern void __aeabi_fcmplt(void);
EXPORT_SYMBOL(__aeabi_cfcmpeq);
EXPORT_SYMBOL(__aeabi_cfcmple);
EXPORT_SYMBOL(__aeabi_cfrcmple);
EXPORT_SYMBOL(__aeabi_fcmpeq);
EXPORT_SYMBOL(__aeabi_fcmpge);
EXPORT_SYMBOL(__aeabi_fcmpgt);
EXPORT_SYMBOL(__aeabi_fcmple);
EXPORT_SYMBOL(__aeabi_fcmplt);

/* _arm_unordsf2.o */
extern void __aeabi_fcmpun(void);
EXPORT_SYMBOL(__aeabi_fcmpun);

/* _arm_fixunssfsi.o */
extern void __aeabi_f2uiz(void);
EXPORT_SYMBOL(__aeabi_f2uiz);

/* _arm_fixsfsi.o */
extern void __aeabi_f2iz(void);
EXPORT_SYMBOL(__aeabi_f2iz);

/* _aeabi_ldivmod.o */
extern void __aeabi_ldivmod(void);
EXPORT_SYMBOL(__aeabi_ldivmod);

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

