/***************************************************************
Copyright Statement:

This software/firmware and related documentation (��EcoNet Software��) 
are protected under relevant copyright laws. The information contained herein 
is confidential and proprietary to EcoNet (HK) Limited (��EcoNet��) and/or 
its licensors. Without the prior written permission of EcoNet and/or its licensors, 
any reproduction, modification, use or disclosure of EcoNet Software, and 
information contained herein, in whole or in part, shall be strictly prohibited.

EcoNet (HK) Limited  EcoNet. ALL RIGHTS RESERVED.

BY OPENING OR USING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY 
ACKNOWLEDGES AND AGREES THAT THE SOFTWARE/FIRMWARE AND ITS 
DOCUMENTATIONS (��ECONET SOFTWARE��) RECEIVED FROM ECONET 
AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN ��AS IS�� 
BASIS ONLY. ECONET EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, 
OR NON-INFRINGEMENT. NOR DOES ECONET PROVIDE ANY WARRANTY 
WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTIES WHICH 
MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE ECONET SOFTWARE. 
RECEIVER AGREES TO LOOK ONLY TO SUCH THIRD PARTIES FOR ANY AND ALL 
WARRANTY CLAIMS RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES 
THAT IT IS RECEIVER��S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD 
PARTY ALL PROPER LICENSES CONTAINED IN ECONET SOFTWARE.

ECONET SHALL NOT BE RESPONSIBLE FOR ANY ECONET SOFTWARE RELEASES 
MADE TO RECEIVER��S SPECIFICATION OR CONFORMING TO A PARTICULAR 
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
#include <boot/packageInfo.h>

/************************************************************************
*                  D E F I N E S   &   C O N S T A N T S
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
static int isInit = 0;
static PACKAGE_ENTRY	pkgInfo[] = {{0, 3, 0},
							 {3, 1, 0},
							 {4, 4, 0},
							 {8, 4, 0},
							 {12, 8, 0},
							 {20, 5, 0},
							 {25, 5, 0},
							 {30, 6, 0},
							 {36, 5, 0},
							 {41, 5, 0},
							 {46, 6, 0},
							 {52, 5, 0},
							 {57, 5, 0},
							 {62, 6, 0},
							 {68, 5, 0},
							 {73, 5, 0},
							 {78, 6, 0},
							 {84, 6, 0},
							 {90, 2, 0},
							 {92, 2, 0},
							 {94, 2, 0},
							 {96, 2, 0},
							 {98, 2, 0},
							 {100, 2, 0},
							 {102, 4, 0},
							 {106, 6, 0},
							 {112, 6, 0},
							 {118, 6, 0},
							 {124, 6, 0},
							 {130, 6, 0},
							 {136, 6, 0},
							 {142, 6, 0},
							 {148, 6, 0},
							 {154, 7, 0},
							 {161, 7, 0},
							 {168, 7, 0},
							 {175, 7, 0},
							 {182, 6, 0},
							 {188, 6, 0},
							 {194, 6, 0},
							 {200, 6, 0},
							 {206, 6, 0},
							 {212, 6, 0},
							 {218, 6, 0},
							 {224, 6, 0},
							 {230, 7, 0},
							 {237, 7, 0},
							 {244, 7, 0},
							 {251, 7, 0},
							 {258, 3, 0},
							 {261, 6, 0},
							 {267, 6, 0},
							 {273, 6, 0},
							 {279, 6, 0},
							 {285, 6, 0},
							 {291, 6, 0},
							 {297, 6, 0},
							 {303, 6, 0},
							 {309, 6, 0},
							 {315, 7, 0},
							 {322, 7, 0},
							 {329, 7, 0},
							 {336, 7, 0},
							 {343, 6, 0},
							 {349, 6, 0},
							 {355, 6, 0},
							 {361, 6, 0},
							 {367, 6, 0},
							 {373, 6, 0},
							 {379, 6, 0},
							 {385, 6, 0},
							 {391, 7, 0},
							 {398, 7, 0},
							 {405, 7, 0},
							 {412, 7, 0},
							 {419, 12, 0},
							 {431, 5, 0},
							 {436, 5, 0},
							 {441, 10, 0},
							 {451, 5, 0},
							 {456, 16, 0}
							};


/************************************************************************
*                  F U N C T I O N   D E F I N I T I O N S
*************************************************************************
*/

void parse_package_info(void)
{
	int i = 0, j = 0;
	unsigned int addr;
	int byteIdx, bitIdx;
	unsigned char *data = (unsigned char *)PACKAGE_INFO_ADDR;

	if(isInit != 0) {
		return;
	}
	isInit = 1;

	for(i = 0; i < PACKAGE_INFO_IDX_MAX_NO; i++) {
		addr = pkgInfo[i].start;
		for(j = 0; j < pkgInfo[i].len; j++, addr++) {
			byteIdx = addr >> 3; /* addr / 8 */
			bitIdx = addr & 0x7; /* addr % 8 */
			pkgInfo[i].data |= (((*(data + byteIdx) >> bitIdx) & 0x1) << j);
		}
	}
}

unsigned short get_efuse_data(PACKAGE_INFO_IDX_T idx)
{
	return pkgInfo[idx].data;
}
EXPORT_SYMBOL(get_efuse_data);


