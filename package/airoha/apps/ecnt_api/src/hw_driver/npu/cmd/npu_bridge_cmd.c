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
#include <asm/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>

#include "libapi_lib_utility.h"

static int do_set(int argc, char *argv[], void *p);
static int do_get(int argc, char *argv[], void *p);

static int do_set_test_egress_mode(int argc, char *argv[], void *p)
{
	unsigned int mode;
	unsigned char cmd[64];

	if (argc < 2)
		goto help;
	
	mode = strtoul(argv[1],NULL,10);

	if(mode > 9)
		goto help;

	printf("mode = %d\n",mode);
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc %x",mode);
	system(cmd);

	return 0;

help:
	printf("npu_bridge_cmd set test_egress_mode [egress_mode] \n");
	printf("1.loopback\n");		
	printf("2.remove header\n");		
	printf("3.add header(dynamic)\n");		
	printf("4.add header(static)\n");		
	printf("5.remove old and add new header\n");		
	printf("6.Drop packet\n");		
	printf("7.create a new packet\n");		
	printf("8.release header buffer\n");		
	printf("9.packet buffer reuse\n");
	return 0;
}

static int do_set_test_reset(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	if (argc < 1)
		goto help;

	sprintf(cmd,"sys memwl 1ec0c1bc a");
	system(cmd);

	return 0;

help:
	printf("npu_bridge_cmd set test_reset\n");
	return 0;
}

static int do_set_test_bufctrl(int argc, char *argv[], void *p) 
{
#if 1 /* NPU Bridge V2*/
	unsigned char cmd[64];

	if (argc > 1)
		goto help;

	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 64"); /* 100 */
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_bufctrl\n");	
	return 0;

#else
	unsigned int pkt_unit,pkt_num,hdr_unit,hdr_num;
	unsigned char cmd[64];
	unsigned int value;

	if (argc < 5)
		goto help;
	
	pkt_unit = strtoul(argv[1],NULL,16);	
	pkt_num = strtoul(argv[2],NULL,16);
	hdr_unit = strtoul(argv[3],NULL,16);	
	hdr_num = strtoul(argv[4],NULL,16);

	printf("pkt_unit = %x,pkt_num = %d,hdr_unit = %x,hdr_num = %d\n",pkt_unit,pkt_num,hdr_unit,hdr_num);
	
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec12008 1e820000");	
	system(cmd);
	sprintf(cmd,"sys memwl 1ec1200c 1e850000");	
	system(cmd);
	value = (pkt_num<<16)|pkt_unit;
	sprintf(cmd,"sys memwl 1ec12010 %x",value);
	system(cmd);
	value = (hdr_num<<16)|hdr_unit;	
	sprintf(cmd,"sys memwl 1ec12014 %x",value);
	system(cmd);
	sprintf(cmd,"sys memwl 1ec12018 1");	
	system(cmd);

	printf("npu bridge buff init done\n");

	return 0;

help:
	printf("npu_bridge_cmd set test_bufctrl [pkt_unit] [pkt_num] [hdr_unit] [hdr_num]\n");
	printf("[pkt_num]/[hdr_num]:0->8 1->16 2->32 3->64\n");
	printf("[pkt_unit]/[hdr_unit] input value should be hex\n");
	return 0;
#endif
}

static int do_set_test_pkt_bufnum(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	printf("npu bridge pkt buf num test\n");
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc b");
	system(cmd);

	return 0;
}

static int do_set_test_hdr_unit(int argc, char *argv[], void *p) 
{
	unsigned int hdr_unit;
	unsigned char cmd[64];
	unsigned int value;

	if (argc < 2)
		goto help;
	
	hdr_unit = strtoul(argv[1],NULL,10);	

	if(hdr_unit > 8)
		goto help;

	printf("test hdr unit %d bytes\n",hdr_unit*128);
	
	memset(cmd,0,64);
	value = hdr_unit+11;
	sprintf(cmd,"sys memwl 1ec0c1bc %x\n",value);	
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set hdr_unit [hdr_unit]\n");
	printf("hdr_unit:1 unit means 128bytes,max valule supported is 8\n");
	return 0;
}

static int do_set_test_hdr_bufnum(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	printf("npu bridge hdr buf num test\n");
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 14");
	system(cmd);

	return 0;
}

static int do_set_test_interrupt(int argc, char *argv[], void *p) 
{
#if 1 /* NPU Bridge V2*/
	unsigned int int_en;
	unsigned int chnl_id;
	unsigned char cmd[64];
	unsigned int reg_addr;
	unsigned int value;

	if (argc < 3)
		goto help;
	
	int_en = strtoul(argv[1],NULL,10);
	chnl_id = strtoul(argv[2],NULL,10);	

	if(int_en > 8)
		goto help;
	if(chnl_id > 7)
		goto help;

	printf("enable channel-%d interrupt bit %d\n", chnl_id, int_en);
	
	memset(cmd,0,64);
	value = 21+int_en;
	sprintf(cmd,"sys memwl 1ec0c1bc %x\n",value);	
	system(cmd);

	reg_addr = (0x1ec12000 + 0x214 + (chnl_id<<4));

	sprintf(cmd,"sys memwl %x 0", reg_addr);
	system(cmd);	

	sprintf(cmd,"sys modifybit %x 4 1", reg_addr);
	system(cmd);	

	sprintf(cmd,"sys modifybit %x %d 1", reg_addr, int_en);	
	system(cmd);	
	
	return 0;

help:
	printf("npu_bridge_cmd set test_interrupt [int_en] [chnl_id]\n");
	printf("0.pktbuf_init_done_int\n");	
	printf("1.headbuf_freeq_dly_int\n");	
	printf("2.pktbuf_full_int\n");	
	printf("3.bridge_send_pkt_int\n"); 
	printf("4.bridge_get_pkt_int\n");	
	printf("5.tx_cmdfifo_full_int\n"); 
	printf("6.rx_cmdfifo_full_int\n");
	return 0;

#else
	unsigned int int_en;
	unsigned char cmd[64];
	unsigned int value;

	if (argc < 2)
		goto help;
	
	int_en = strtoul(argv[1],NULL,10);	

	if(int_en > 8)
		goto help;

	printf("enable interrupt bit %d\n",int_en);
	
	memset(cmd,0,64);
	value = 21+int_en;
	sprintf(cmd,"sys memwl 1ec0c1bc %x\n",value);	
	system(cmd);
	sprintf(cmd,"sys memwl 1ec12004 0");
	system(cmd);	

	sprintf(cmd,"sys modifybit 1ec12004 0 1");
	system(cmd);	

	sprintf(cmd,"sys modifybit 1ec12004 %d 1",int_en);	
	system(cmd);	
	
	return 0;

help:
	printf("npu_bridge_cmd set test_interrupt [int_en]\n");
	printf("0.bridge_igrpkt_rdy_int\n");	
	printf("1.bridge_egress_rdy_int\n");	
	printf("2.bm_headbuf_rdy_int\n");	
	printf("3.pktbuf_init_done_int\n"); 
	printf("4.headbuf_init_done_int\n");	
	printf("5.pktbuf_empty_int\n"); 
	printf("6.headbuf_empty_int\n");
	return 0;
#endif
}

static int do_set_test_rx_msg(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	printf("npu bridge rx msg test\n");
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 1c");
	system(cmd);

	return 0;
}

static int do_set_test_txmsg_co(int argc, char *argv[], void *p) 
{
		unsigned int type;
		unsigned char cmd[64];
		unsigned int value;
	
		if (argc < 2)
			goto help;
		
		type = strtoul(argv[1],NULL,10);	
	
		if(type > 2)
			goto help;
	
		if(0 == type)
			printf("test no chekcsum offload\n");
		else if(1 == type)
			printf("test ico/uco\n");
		else if(2 == type)
			printf("test ico/tco\n");
		
		memset(cmd,0,64);
		value = 29+type;
		sprintf(cmd,"sys memwl 1ec0c1bc %x\n",value);	
		system(cmd);
		
		return 0;
	
	help:
		printf("npu_bridge_cmd set test_txmsg_co [type]\n");
		printf("0.without checksum offload\n");	
		printf("1.ico/uco\n");	
		printf("2.ico/tco\n");	
		return 0;
}

static int do_set_test_txmsg_hwf(int argc, char *argv[], void *p) 
{
		unsigned int hwf;
		unsigned char cmd[64];
		unsigned int value;
	
		if (argc < 2)
			goto help;
		
		hwf = strtoul(argv[1],NULL,10);	
	
		if(hwf > 4)
			goto help;
	
		if(0 == hwf)
			printf("test hwf = 0, direct to GDM1\n");
		else if(1 == hwf)
			printf("test hwf = 1, fqos is set\n");
		else if(2 == hwf)
			printf("test hwf = 2, fqos & fast are set\n");
		else if(3 == hwf)
			printf("test hwf = 3, qdma lan, acnt0-3, acnt1-5, meter0-0x20\n");
		else if(4 == hwf)
			printf("test hwf = 4, qdma wan, acnt0-3, acnt1-5, meter0-0x20\n");
		
		memset(cmd,0,64);
		if(hwf >= 2)
			value = 90+(hwf-2);
		else
			value = 32+hwf;
		sprintf(cmd,"sys memwl 1ec0c1bc %x\n",value);	
		system(cmd);
		
		return 0;
	
	help:
		printf("npu_bridge_cmd set test_txmsg_hwf [hwf]\n");
		printf("0.hwf = 0\n");	
		printf("1.hwf = 1\n");	
		return 0;
}

static int do_set_test_txmsg_nbq(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	printf("npu bridge tx msg  nbq test\n");
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 22");
	system(cmd);

	return 0;
}

static int do_set_test_sram(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	printf("npu bridge sram test\n");
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 23");
	system(cmd);

	return 0;
}

static int do_set_test_mib(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	printf("npu bridge mib test\n");
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 24");
	system(cmd);

	return 0;
}

static int do_set_test_oq_pri(int argc, char *argv[], void *p) 
{
	unsigned int mode;
	unsigned char cmd[64];

	if (argc < 2)
		goto help;
	
	mode = strtoul(argv[1],NULL,10);	

	if(mode  > 2)
		goto help;

	if(0 == mode)
		printf("test queue priority round robin\n");
	else if(1 == mode)
		printf("test queue priority oq0 high\n");
	else if(2 == mode)
		printf("test queue priority oq1 high\n");
	
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec1201c %x\n",mode);	
	system(cmd);

	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 25");
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_oq_pri [mode]\n");
	printf("0.round robin\n");	
	printf("1.oq0 high priority\n");			
	printf("2.oq1 high priority\n");	
	return 0;
}

static int do_set_test_rbus_burst_size(int argc, char *argv[], void *p) 
{
	unsigned int mode;
	unsigned char cmd[64];

	if (argc < 2)
		goto help;
	
	mode = strtoul(argv[1],NULL,10);	

	if(mode > 15)
		goto help;

	if(0 == mode) {
		printf("rbus burst size 16bytes, txmbi_max_pkt_len 0xFFFF\n");
		mode= 0xffff0004;
	} else if(1 == mode) {
		printf("rbus burst size 32bytes, txmbi_max_pkt_len 0xFFFF\n");
		mode= 0xffff0005;
	} else if(2 == mode) {
		printf("rbus burst size 64bytes, txmbi_max_pkt_len 0xFFFF\n");
		mode= 0xffff0006;
	} else if(3 == mode) {		
		printf("rbus burst size 128bytes, txmbi_max_pkt_len 0xFFFF\n");
		mode= 0xffff0007;
	} else if(4 == mode) {
		printf("rbus burst size 16bytes, txmbi_max_pkt_len 0x800\n");
		mode= 0x08000004;
	} else if(5 == mode) {
		printf("rbus burst size 32bytes, txmbi_max_pkt_len 0x800\n");
		mode= 0x08000005;
	} else if(6 == mode) {
		printf("rbus burst size 64bytes, txmbi_max_pkt_len 0x800\n");
		mode= 0x08000006;
	} else if(7 == mode) {		
		printf("rbus burst size 128bytes, txmbi_max_pkt_len 0x800\n");
		mode= 0x08000007;
	} else if(8 == mode) {
		printf("rbus burst size 16bytes, txmbi_max_pkt_len 0x400\n");
		mode= 0x04000004;
	} else if(9 == mode) {
		printf("rbus burst size 32bytes, txmbi_max_pkt_len 0x400\n");
		mode= 0x04000005;
	} else if(10 == mode) {
		printf("rbus burst size 64bytes, txmbi_max_pkt_len 0x400\n");
		mode= 0x04000006;
	} else if(11 == mode) {		
		printf("rbus burst size 128bytes, txmbi_max_pkt_len 0x400\n");
		mode= 0x04000007;
	} else if(12 == mode) {
		printf("rbus burst size 16bytes, txmbi_max_pkt_len 0x200\n");
		mode= 0x02000004;
	} else if(13 == mode) {
		printf("rbus burst size 32bytes, txmbi_max_pkt_len 0x200\n");
		mode= 0x02000005;
	} else if(14 == mode) {
		printf("rbus burst size 64bytes, txmbi_max_pkt_len 0x200\n");
		mode= 0x02000006;
	} else if(15 == mode) {		
		printf("rbus burst size 128bytes, txmbi_max_pkt_len 0x200\n");
		mode= 0x02000007;
	}
	
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec12020 %x",mode);	
	system(cmd);

	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 1");
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_rbus_burst_size [size]\n");
	printf("0.16bytes\n");	
	printf("1.32bytes\n");			
	printf("2.64bytes\n");		
	printf("3.128bytes\n");	
	
	return 0;
}

static int do_set_test_register_read(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	printf("npu bridge register read test\n");
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 26");
	system(cmd);

	return 0;
}

static int do_set_test_register_write(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	printf("npu bridge register write test\n");
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 27");
	system(cmd);

	return 0;
}

static int do_set_test_modify_len(int argc, char *argv[], void *p) 
{
	unsigned int mode;
	unsigned char cmd[64];
	int value = 0;

	if (argc < 2)
		goto help;
	
	mode = strtoul(argv[1],NULL,10);	

	if(mode > 2)
		goto help;

	if(0 == mode)
		printf("test modify len0\n");
	else if(1 == mode)
		printf("test modify len1\n");
	else if(2 == mode)
		printf("test modify len2\n");

	value = 40+mode;
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc %x",value);
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_modify_len [mode]\n");
	printf("0.modify len0\n");	
	printf("1.modify len0&len1\n");
	printf("2.modify len0&len1&IP\n");	
	
	return 0;
}

static int do_set_test_wan_hwf(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];

	printf("npu bridge wan hwf test\n");
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc 2a");
	system(cmd);

	return 0;
}

static int do_set_test_ppe_bufid_isr(int argc, char *argv[], void *p) 
{
	unsigned int mode;
	unsigned char cmd[64];
	int value = 0;

	if (argc < 2)
		goto help;
	
	mode = strtoul(argv[1],NULL,10);	

	if((mode  > 2)||(0 == mode))
		goto help;

	if(1 == mode)
		printf("disable ppe buf-id isr\n");
	else if(2 == mode)
		printf("enable ppe buf-id isr\n");

	value = mode;
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1b4 %x",value);
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_ppe_bufid_isr [enable]\n");
	printf("1.disable ppe buf-id isr\n");	
	printf("2.enable ppe buf-id isr\n");			
	
	return 0;
}

static int do_set_test_send_wifi_pkt(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];
	int value = 0;

	if (argc < 2)
		goto help;
	
	value = strtoul(argv[1],NULL,10);

	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1b0 %x",value);
	printf("cmd = %s\n",cmd);
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_send_wifi_pkt [value]\n");
	printf("value:pkt num,if value==10000,send pkt contiuously");	
	
	return 0;
}

static int do_set_test_set_wifi_pkt_oq(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];
	int value = 0;

	if (argc < 2)
		goto help;
	
	value = strtoul(argv[1],NULL,10);

	memset(cmd,0,64);
	if(0 == value)
		sprintf(cmd,"sys memwl 1ec0c1b4 %x",3);
	else if(1 == value)
		sprintf(cmd,"sys memwl 1ec0c1b4 %x",4);
	else
		goto help;
	printf("cmd = %s\n",cmd);
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_set_wifi_pkt_oq [0/1]\n");
	
	return 0;
}

static int do_set_test_send_wifi_pkt_one_stream(int argc, char *argv[], void *p) 
{
	unsigned char cmd[64];
	int value = 0;

	if (argc < 2)
		goto help;
	
	value = strtoul(argv[1],NULL,10);

	memset(cmd,0,64);
	if(0 == value)
		sprintf(cmd,"sys memwl 1ec0c1ac %x",1);
	else if(1 == value)
		sprintf(cmd,"sys memwl 1ec0c1ac %x",2);
	else
		goto help;
	printf("cmd = %s\n",cmd);
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_send_wifi_pkt_one_stream [0/1]\n");
	
	return 0;
}

static int do_set_test_channel_id(int argc, char *argv[], void *p) 
{
	unsigned int mode;
	unsigned char cmd[64];
	int value = 0;

	if (argc < 2)
		goto help;
	
	mode = strtoul(argv[1],NULL,10);	

	if(mode > 7)
		goto help;

	value = 80+mode;
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc %x",value);
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_channel_id [0-7]\n");		
	
	return 0;
}

static int do_set_test_freeq_dump(int argc, char *argv[], void *p) 
{
	unsigned int mode;
	unsigned char cmd[64];
	int value = 0;

	if (argc < 2)
		goto help;
	
	mode = strtoul(argv[1],NULL,10);	

	if(mode > 7)
		goto help;

	value = 110+mode;
	memset(cmd,0,64);
	sprintf(cmd,"sys memwl 1ec0c1bc %x",value);
	system(cmd);
	
	return 0;

help:
	printf("npu_bridge_cmd set test_freeq_dump [0-7]\n");		
	
	return 0;
}

static cmds_t setCmds[] = {
	{"test_egress_mode",    do_set_test_egress_mode,     	0x0, 0, NULL},
	{"test_reset",	do_set_test_reset,		0x0, 0, NULL},	
	{"test_bufctrl",	do_set_test_bufctrl,		0x0, 0, NULL},	
	{"test_pkt_bufnum",	do_set_test_pkt_bufnum,		0x0, 0, NULL},	
	{"test_hdr_unit",	do_set_test_hdr_unit,		0x0, 0, NULL},	
	{"test_hdr_bufnum",	do_set_test_hdr_bufnum,		0x0, 0, NULL},	
	{"test_interrupt",	do_set_test_interrupt,		0x0, 0, NULL},	
	{"test_rxmsg",	do_set_test_rx_msg,		0x0, 0, NULL},	
	{"test_txmsg_co",	do_set_test_txmsg_co,		0x0, 0, NULL},	
	{"test_txmsg_hwf",	do_set_test_txmsg_hwf,		0x0, 0, NULL},
	{"test_txmsg_nbq",	do_set_test_txmsg_nbq,		0x0, 0, NULL},	
	{"test_txmsg_wan_hwf",	do_set_test_wan_hwf,		0x0, 0, NULL},
	{"test_sram",	do_set_test_sram,		0x0, 0, NULL},	
	{"test_mib",	do_set_test_mib,		0x0, 0, NULL},	
	{"test_oq_pri",	do_set_test_oq_pri,		0x0, 0, NULL},	
	{"test_rbus_burst_size",	do_set_test_rbus_burst_size,		0x0, 0, NULL},	
	{"test_register_read",	do_set_test_register_read,		0x0, 0, NULL},	
	{"test_register_write",	do_set_test_register_write,		0x0, 0, NULL},	
	{"test_modify_len",	do_set_test_modify_len,		0x0, 0, NULL},
	{"test_ppe_bufid_isr",	do_set_test_ppe_bufid_isr,		0x0, 0, NULL},	
	{"test_send_wifi_pkt",	do_set_test_send_wifi_pkt,		0x0, 0, NULL},
	{"test_set_wifi_pkt_oq",	do_set_test_set_wifi_pkt_oq,		0x0, 0, NULL},	
	{"test_send_wifi_pkt_one_stream",	do_set_test_send_wifi_pkt_one_stream,		0x0, 0, NULL},	
	{"test_channel_id",	do_set_test_channel_id,		0x0, 0, NULL},
	{"test_freeq_dump",	do_set_test_freeq_dump,		0x0, 0, NULL},
	{NULL,    NULL,      	 		0x0, 0, NULL}
};

static cmds_t getCmds[] = {	
    {"test_egress_mode", NULL,   	0x0, 0, NULL},
    {NULL,    NULL,      	 		0x0, 0, NULL}
};

static cmds_t npuCmds[] = {
    {"set",     do_set,     0x10, 0, NULL},
    {"get",     do_get,     0x10, 0, NULL},
    {NULL,      NULL,       0x10, 0, NULL}
};

static int do_set(int argc, char *argv[], void *p)
{
	return subcmd(setCmds, argc, argv, NULL);
}

static int do_get(int argc, char *argv[], void *p)
{
    return subcmd(getCmds, argc, argv, NULL);
}

int main(int argc, char **argv) 
{
 //   init_compileoption_val();

	return subcmd(npuCmds, argc, argv, NULL);
}


