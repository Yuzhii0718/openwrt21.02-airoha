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
/* _fixdfdi.o */
extern void __fixdfdi(void);
EXPORT_SYMBOL(__fixdfdi);
extern void __aeabi_d2lz(void);
EXPORT_SYMBOL(__aeabi_d2lz);

/* _fixsfdi.o */
extern void __fixsfdi(void);
EXPORT_SYMBOL(__fixsfdi);
extern void __aeabi_f2lz(void);
EXPORT_SYMBOL(__aeabi_f2lz);

/* _arm_cmpdf2.o */
extern void __gtdf2(void);
EXPORT_SYMBOL(__gtdf2);
extern void __gedf2(void);
EXPORT_SYMBOL(__gedf2);
extern void __ltdf2(void);
EXPORT_SYMBOL(__ltdf2);
extern void __ledf2(void);
EXPORT_SYMBOL(__ledf2);
extern void __cmpdf2(void);
EXPORT_SYMBOL(__cmpdf2);
extern void __nedf2(void);
EXPORT_SYMBOL(__nedf2);
extern void __eqdf2(void);
EXPORT_SYMBOL(__eqdf2);
extern void __aeabi_cdrcmple(void);
EXPORT_SYMBOL(__aeabi_cdrcmple);
extern void __aeabi_cdcmpeq(void);
EXPORT_SYMBOL(__aeabi_cdcmpeq);
extern void __aeabi_cdcmple(void);
EXPORT_SYMBOL(__aeabi_cdcmple);
extern void __aeabi_dcmpeq(void);
EXPORT_SYMBOL(__aeabi_dcmpeq);
extern void __aeabi_dcmplt(void);
EXPORT_SYMBOL(__aeabi_dcmplt);
extern void __aeabi_dcmple(void);
EXPORT_SYMBOL(__aeabi_dcmple);
extern void __aeabi_dcmpge(void);
EXPORT_SYMBOL(__aeabi_dcmpge);
extern void __aeabi_dcmpgt(void);
EXPORT_SYMBOL(__aeabi_dcmpgt);

/* _arm_addsubdf3.o */
extern void __aeabi_drsub(void);
EXPORT_SYMBOL(__aeabi_drsub);
extern void __subdf3(void);
EXPORT_SYMBOL(__subdf3);
extern void __aeabi_dsub(void);
EXPORT_SYMBOL(__aeabi_dsub);
extern void __adddf3(void);
EXPORT_SYMBOL(__adddf3);
extern void __aeabi_dadd(void);
EXPORT_SYMBOL(__aeabi_dadd);
extern void __floatunsidf(void);
EXPORT_SYMBOL(__floatunsidf);
extern void __aeabi_ui2d(void);
EXPORT_SYMBOL(__aeabi_ui2d);
extern void __floatsidf(void);
EXPORT_SYMBOL(__floatsidf);
extern void __aeabi_i2d(void);
EXPORT_SYMBOL(__aeabi_i2d);
extern void __extendsfdf2(void);
EXPORT_SYMBOL(__extendsfdf2);
extern void __aeabi_f2d(void);
EXPORT_SYMBOL(__aeabi_f2d);
extern void __floatundidf(void);
EXPORT_SYMBOL(__floatundidf);
extern void __aeabi_ul2d(void);
EXPORT_SYMBOL(__aeabi_ul2d);
extern void __floatdidf(void);
EXPORT_SYMBOL(__floatdidf);
extern void __aeabi_l2d(void);
EXPORT_SYMBOL(__aeabi_l2d);

/* _arm_cmpsf2.o */
extern void __gtsf2(void);
EXPORT_SYMBOL(__gtsf2);
extern void __gesf2(void);
EXPORT_SYMBOL(__gesf2);
extern void __ltsf2(void);
EXPORT_SYMBOL(__ltsf2);
extern void __lesf2(void);
EXPORT_SYMBOL(__lesf2);
extern void __cmpsf2(void);
EXPORT_SYMBOL(__cmpsf2);
extern void __nesf2(void);
EXPORT_SYMBOL(__nesf2);
extern void __eqsf2(void);
EXPORT_SYMBOL(__eqsf2);
extern void __aeabi_cfrcmple(void);
EXPORT_SYMBOL(__aeabi_cfrcmple);
extern void __aeabi_cfcmpeq(void);
EXPORT_SYMBOL(__aeabi_cfcmpeq);
extern void __aeabi_cfcmple(void);
EXPORT_SYMBOL(__aeabi_cfcmple);
extern void __aeabi_fcmpeq(void);
EXPORT_SYMBOL(__aeabi_fcmpeq);
extern void __aeabi_fcmple(void);
EXPORT_SYMBOL(__aeabi_fcmple);
extern void __aeabi_fcmpge(void);
EXPORT_SYMBOL(__aeabi_fcmpge);
extern void __aeabi_fcmpgt(void);
EXPORT_SYMBOL(__aeabi_fcmpgt);
extern void __aeabi_fcmplt(void);
EXPORT_SYMBOL(__aeabi_fcmplt);

/* _fixunsdfdi.o */
extern void __fixunsdfdi(void);
EXPORT_SYMBOL(__fixunsdfdi);
extern void __aeabi_d2ulz(void);
EXPORT_SYMBOL(__aeabi_d2ulz);

/* _fixunssfdi.o */
extern void __fixunssfdi(void);
EXPORT_SYMBOL(__fixunssfdi);
extern void __aeabi_f2ulz(void);
EXPORT_SYMBOL(__aeabi_f2ulz);

/* _arm_muldivdf3.o */
extern void __muldf3(void);
EXPORT_SYMBOL(__muldf3);
extern void __aeabi_dmul(void);
EXPORT_SYMBOL(__aeabi_dmul);
extern void __divdf3(void);
EXPORT_SYMBOL(__divdf3);
extern void __aeabi_ddiv(void);
EXPORT_SYMBOL(__aeabi_ddiv);

/* _arm_fixunsdfsi.o */
extern void __fixunsdfsi(void);
EXPORT_SYMBOL(__fixunsdfsi);
extern void __aeabi_d2uiz(void);
EXPORT_SYMBOL(__aeabi_d2uiz);

/* _arm_truncdfsf2.o */
extern void __truncdfsf2(void);
EXPORT_SYMBOL(__truncdfsf2);
extern void __aeabi_d2f(void);
EXPORT_SYMBOL(__aeabi_d2f);

/* _arm_addsubsf3.o */
extern void __aeabi_frsub(void);
EXPORT_SYMBOL(__aeabi_frsub);
extern void __subsf3(void);
EXPORT_SYMBOL(__subsf3);
extern void __aeabi_fsub(void);
EXPORT_SYMBOL(__aeabi_fsub);
extern void __addsf3(void);
EXPORT_SYMBOL(__addsf3);
extern void __aeabi_fadd(void);
EXPORT_SYMBOL(__aeabi_fadd);
extern void __floatunsisf(void);
EXPORT_SYMBOL(__floatunsisf);
extern void __aeabi_ui2f(void);
EXPORT_SYMBOL(__aeabi_ui2f);
extern void __floatsisf(void);
EXPORT_SYMBOL(__floatsisf);
extern void __aeabi_i2f(void);
EXPORT_SYMBOL(__aeabi_i2f);
extern void __floatundisf(void);
EXPORT_SYMBOL(__floatundisf);
extern void __aeabi_ul2f(void);
EXPORT_SYMBOL(__aeabi_ul2f);
extern void __floatdisf(void);
EXPORT_SYMBOL(__floatdisf);
extern void __aeabi_l2f(void);
EXPORT_SYMBOL(__aeabi_l2f);

/* _arm_muldivsf3.o */
extern void __mulsf3(void);
EXPORT_SYMBOL(__mulsf3);
extern void __aeabi_fmul(void);
EXPORT_SYMBOL(__aeabi_fmul);
extern void __divsf3(void);
EXPORT_SYMBOL(__divsf3);
extern void __aeabi_fdiv(void);
EXPORT_SYMBOL(__aeabi_fdiv);

/* _arm_muldivsf3.o */
extern void __fixunssfsi(void);
EXPORT_SYMBOL(__fixunssfsi);
extern void __aeabi_f2uiz(void);
EXPORT_SYMBOL(__aeabi_f2uiz);

/* _arm_muldivsf3.o */
extern void __fixdfsi(void);
EXPORT_SYMBOL(__fixdfsi);
extern void __aeabi_d2iz(void);
EXPORT_SYMBOL(__aeabi_d2iz);

/* _arm_muldivsf3.o */
extern void __fixsfsi(void);
EXPORT_SYMBOL(__fixsfsi);
extern void __aeabi_f2iz(void);
EXPORT_SYMBOL(__aeabi_f2iz);

/* _aeabi_ldivmod.o */
extern void __aeabi_ldivmod(void);
EXPORT_SYMBOL(__aeabi_ldivmod);

/* _dvmd_lnx.o */
extern void __aeabi_ldiv0(void);
EXPORT_SYMBOL(__aeabi_ldiv0);
extern void __aeabi_idiv0(void);
EXPORT_SYMBOL(__aeabi_idiv0);

/************************************************************************
*                  P R I V A T E   D A T A
*************************************************************************
*/

/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

