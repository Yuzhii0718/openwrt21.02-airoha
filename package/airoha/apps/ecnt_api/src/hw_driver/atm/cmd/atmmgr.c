

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libapi_lib_atmmgr.h>
#include <libapi_lib_utility.h>
#include "atmmgr.h"

static cmds_t atmmgr_cnt_show_command[] =
{
	{"wan", do_cnt_show_wan, 0, 0, NULL},
	{"mib", do_cnt_show_mib, 0, 0, NULL},

    {NULL, NULL, 0, 0, NULL}
};

static cmds_t atmmgr_cnt_command[] =
{
	{"show", do_cnt_show, 0, 1, NULL},

    {NULL, NULL, 0, 0, NULL}
};

static cmds_t atmmgr_oam_command[] =
{
	{"start", do_oam_loopback_start, 0, 4, "atm_cmd oamloopback start [vpi] [vci] [f4:0|f5:1] [segment:0|endtoend:1]"},
	{"reset", do_oam_loopback_reset, 0, 0, NULL},
	{"show", do_oam_loopback_show, 0, 0, NULL},

    {NULL, NULL, 0, 0, NULL}
};

#define VCC_CREATE_PARAM  "[ipoe:0|ipoa:1|pppoa:2] [vc_mux:0|llc:1] [qbr:0|cbr:1|vbr:2|nrt-vbr:3] [pcr] [scr] [mbs]"

static cmds_t atmmgr_vcc_command[] =
{
	{"create", do_vcc_create, 0, 8, "atm_cmd vcc create [vpi] [vci] "VCC_CREATE_PARAM},
	{"delete", do_vcc_delete, 0, 2, "atm_cmd vcc delete [vpi] [vci]"},
	{"show", do_vcc_show, 0, 2, "atm_cmd vcc show [vpi] [vci]"},

    {NULL, NULL, 0, 0, NULL}
};

static cmds_t atmmgr_root_command[] =
{
	{"start", do_start, 0, 0, "atm_cmd start"},
	{"stop", do_stop, 0, 0, "atm_cmd stop"},
	{"vcc", do_vcc, 0, 1, "atm_cmd vcc [create|delete|show]"},
	{"oamloopback", do_oam_loopback, 0, 1, "atm_cmd oamloopback [start|reset|show]"},
	{"counter", do_cnt, 0, 2, "atm_cmd counter show [wan|mib]"},

    {NULL, NULL, 0, 0, NULL}
};

int do_cnt_show_mib(int argc, char *argv[], void *p)
{
	ATM_MIB_CNT mib_cnt;
	ECNT_ATMMGR_RET ret = ECNT_ATMMGR_DEF_NO;
	unsigned char i = 0;

	memset(&mib_cnt, 0, sizeof(ATM_MIB_CNT));

	ret = ecnt_atmmgr_lib_show_mib_counter(&mib_cnt);

	if (ret == ECNT_ATMMGR_SUCCESS)
	{
		printf("\n*******OUT PACKET COUNTER*******\n");
		printf("txDataPkts = 0x%08x\n", mib_cnt.txDataPkts);
		printf("txF4Pkts  = 0x%08x\n", mib_cnt.txF4Pkts);
		printf("txF5Pkts  = 0x%08x\n", mib_cnt.txF5Pkts);
		for (i = 0; i < mib_cnt.vc_max; i++)
			printf("txVc%dPkts = 0x%08x\n", i, mib_cnt.txVcNum[i]);

		printf("\n*******IN PACKET COUNTER********\n");
		printf("rxDataPkts = 0x%08x\n", mib_cnt.rxDataPkts);
		printf("rxF4Pkts = 0x%08x\n", mib_cnt.rxF4Pkts);
		printf("rxF5Pkts = 0x%08x\n", mib_cnt.rxF5Pkts);

		printf("\n*******OUT DROP COUNTER*********\n");
		for (i = 0; i < mib_cnt.vc_max; i++)
			printf("txDropVc%dPkts = 0x%08x\n", i, mib_cnt.txHardDropVcNum[i]);

		printf("\n*******IN ERROR COUNTER*********\n");
		printf("rxDiscards  = 0x%08x\n", mib_cnt.rxDiscards);
		printf("rxCrcErr    = 0x%08x\n", mib_cnt.rxCrcErr);
		printf("rxCrc32Err  = 0x%08x\n", mib_cnt.rxCrc32Err);
		printf("rxCrc10Err  = 0x%08x\n", mib_cnt.rxCrc10Err);
		printf("rxL4CsErr   = 0x%08x\n", mib_cnt.rxL4CsErr);
		printf("rxIp4CsErr  = 0x%08x\n", mib_cnt.rxIp4CsErr);
		printf("rxActErr    = 0x%08x\n", mib_cnt.rxActErr);
		printf("rxBufLenErr = 0x%08x\n", mib_cnt.rxBufLenErr);
		printf("rxMpoaErr   = 0x%08x\n", mib_cnt.rxMpoaErr);
	}

	return ret;
}

int do_cnt_show_wan(int argc, char *argv[], void *p)
{
	ATM_WAN_CNT wan_cnt;
	ECNT_ATMMGR_RET ret = ECNT_ATMMGR_DEF_NO;

	memset(&wan_cnt, 0, sizeof(ATM_WAN_CNT));

	ret = ecnt_atmmgr_lib_show_wan_counter(&wan_cnt);

	if (ret == ECNT_ATMMGR_SUCCESS)
	{
		printf("RX_PKTS      = 0x%08x  TX_PKTS      = 0x%08x\n", wan_cnt.rxPkts, wan_cnt.txPkts);
		printf("RX_DROP_PKTS = 0x%08x  TX_DROP_PKTS = 0x%08x\n", wan_cnt.rxDropPkts, wan_cnt.txDropPkts);
		printf("RX_BYTES     = 0x%08x  TX_BYTES     = 0x%08x\n", wan_cnt.rxBytes, wan_cnt.txBytes);
	}

	return ret;
}

int do_cnt_show(int argc, char *argv[], void *p)
{
	return subcmd(atmmgr_cnt_show_command, argc, argv, NULL);
}

int do_cnt(int argc, char *argv[], void *p)
{
	return subcmd(atmmgr_cnt_command, argc, argv, NULL);
}

int do_oam_loopback_show(int argc, char *argv[], void *p)
{
	ATM_OAM_STATUS oam_status;
	ECNT_ATMMGR_RET ret = ECNT_ATMMGR_DEF_NO;
	unsigned char state = 0;
	char *state_str[] = {"complete", "internal error", "in process", "other error", "none"};

	memset(&oam_status, 0, sizeof(ATM_OAM_STATUS));

	ret = ecnt_atmmgr_lib_show_oam_loopback(&oam_status);

	if (ret == ECNT_ATMMGR_SUCCESS)
	{
		switch (oam_status.diag_state)
		{
			case ATM_OAM_TEST_COMPLETED:
				state = 0;
				break;
			case ATM_OAM_TEST_ERROR_INTERNAL:
				state = 1;
				break;
			case ATM_OAM_TEST_IN_PROCESS:
				state = 2;
				break;
			case ATM_OAM_TEST_ERROR_OTHER:
				state = 3;
				break;
			default:
				state = 4;
		}

		printf("DiagnosticState %s\n", state_str[state]);
		printf("DiagnosticTime %u\n", oam_status.diag_time);
	}

	return ret;
}

int do_oam_loopback_reset(int argc, char *argv[], void *p)
{
	return ecnt_atmmgr_lib_reset_oam_loopback();
}

int do_oam_loopback_start(int argc, char *argv[], void *p)
{
	ATM_OAM_PARAM oam_param;
	unsigned char vpi = 0; 
	unsigned int vci = 0;
	unsigned char type = 0;
	unsigned char method = 0;
	int i = 0, j = 0;

	for (j = 1; j < argc; j++)
	{
		for(i = 0; i < strlen(argv[j]); i++)
		{
		    if(!isdigit(argv[j][i]))
			{
		       return ECNT_ATMMGR_FAIL;
		    }
		}
	}

	memset(&oam_param, 0, sizeof(ATM_OAM_PARAM));

	vpi = (unsigned char) atoi(argv[1]);
	vci = (unsigned int) atoi(argv[2]);
	type = (unsigned char) atoi(argv[3]);
	method = (unsigned char) atoi(argv[4]);

	switch (type)
	{
		case 0:
			oam_param.oam_type = ATM_OAM_F4;
			break;
		case 1:
			oam_param.oam_type= ATM_OAM_F5;
			break;
		default:
			oam_param.oam_type= ATM_OAM_F5;
	}

	switch (method)
	{
		case 0:
			oam_param.test_type= ATM_OAM_SEGMENT;
			break;
		case 1:
			oam_param.test_type = ATM_OAM_ENDTOEND;
			break;
		default:
			oam_param.test_type= ATM_OAM_ENDTOEND;
	}

	return ecnt_atmmgr_lib_start_oam_loopback(vpi, vci, &oam_param);
}

int do_oam_loopback(int argc, char *argv[], void *p)
{
	return subcmd(atmmgr_oam_command, argc, argv, NULL);
}

int do_vcc_show(int argc, char *argv[], void *p)
{
	ATM_VCC_PARAM vcc_param;
	unsigned char vpi = 0; 
	unsigned int vci = 0;
	unsigned char mode = 0;
	unsigned char encap = 0;
	unsigned char qos = 0;
	ECNT_ATMMGR_RET ret = ECNT_ATMMGR_DEF_NO;
	char *mode_str[] = {"br2684_bridged", "br2684_routed", "pppoa", "none"};
	char *encap_str[] = {"vc_mux", "llc", "none"};
	char *qos_str[] = {"ubr", "cbr", "rt-vbt", "nrt-vbt", "none"};
	int i = 0, j = 0;

	for (j = 1; j < argc; j++)
	{
		for(i = 0; i < strlen(argv[j]); i++)
		{
		    if(!isdigit(argv[j][i]))
			{
		       return ECNT_ATMMGR_FAIL;
		    }
		}
	}

	memset(&vcc_param, 0, sizeof(ATM_VCC_PARAM));

	vpi = (unsigned char) atoi(argv[1]);
	vci = (unsigned int) atoi(argv[2]);

	ret = ecnt_atmmgr_lib_show_vcc(vpi, vci, &vcc_param);

	if (ret == ECNT_ATMMGR_SUCCESS)
	{
		switch (vcc_param.mode)
		{
			case ATM_MODE_BR2684_BRIDGED:
				mode = 0;
				break;
			case ATM_MODE_BR2684_ROUTED:
				mode = 1;
				break;
			case ATM_MODE_PPPOA_ROUTED:
				mode = 2;
				break;
			default:
				mode = 3;
		}

		switch (vcc_param.encap)
		{
			case ATM_ECNAP_VC_MUX:
				encap = 0;
				break;
			case ATM_ECNAP_LLC:
				encap = 1;
				break;
			default:
				encap = 2;
		}

		switch (vcc_param.qos_type)
		{
			case ATM_QOS_UBR:
				qos = 0;
				break;
			case ATM_QOS_CBR:
				qos = 1;
				break;
			case ATM_QOS_RTVBR:
				qos = 2;
				break;
			case ATM_QOS_NRTVBR:
				qos = 3;
				break;
			default:
				qos = 4;
		}
		printf("VPI %d VCI %d:\n", vpi, vci);
		printf("MODE %s\n", mode_str[mode]);
		printf("ENCAP %s\n", encap_str[encap]);
		printf("QOS %s\n", qos_str[qos]);
		printf("PCR %d\n", vcc_param.pcr);
		printf("SCR %d\n", vcc_param.scr);
		printf("MBS %d\n", vcc_param.mbs);
	}

	return ret;
}

int do_vcc_delete(int argc, char *argv[], void *p)
{
	unsigned char vpi = 0; 
	unsigned int vci = 0;
	int i = 0, j = 0;

	for (j = 1; j < argc; j++)
	{
		for(i = 0; i < strlen(argv[j]); i++)
		{
		    if(!isdigit(argv[j][i]))
			{
		       return ECNT_ATMMGR_FAIL;
		    }
		}
	}

	vpi = (unsigned char) atoi(argv[1]);
	vci = (unsigned int) atoi(argv[2]);

	return ecnt_atmmgr_lib_delete_vcc(vpi, vci);
}

int do_vcc_create(int argc, char *argv[], void *p)
{
	ATM_VCC_PARAM vcc_param;
	unsigned char vpi = 0; 
	unsigned int vci = 0;
	unsigned char mode = 0;
	unsigned char encap = 0;
	unsigned char qos = 0;
	int i = 0, j = 0;

	for (j = 1; j < argc; j++)
	{
		for(i = 0; i < strlen(argv[j]); i++)
		{
		    if(!isdigit(argv[j][i]))
			{
		       return ECNT_ATMMGR_FAIL;
		    }
		}
	}

	memset(&vcc_param, 0, sizeof(ATM_VCC_PARAM));

	vpi = (unsigned char) atoi(argv[1]);
	vci = (unsigned int) atoi(argv[2]);
	mode = (unsigned char) atoi(argv[3]);
	encap = (unsigned char) atoi(argv[4]);
	qos = (unsigned char) atoi(argv[5]);

	switch (mode)
	{
		case 0:
			vcc_param.mode = ATM_MODE_BR2684_BRIDGED;
			break;
		case 1:
			vcc_param.mode = ATM_MODE_BR2684_ROUTED;
			break;
		case 2:
			vcc_param.mode = ATM_MODE_PPPOA_ROUTED;
			break;
		default:
			vcc_param.mode = ATM_MODE_BR2684_BRIDGED;
	}

	switch (encap)
	{
		case 0:
			vcc_param.encap= ATM_ECNAP_VC_MUX;
			break;
		case 1:
			vcc_param.encap= ATM_ECNAP_LLC;
			break;
		default:
			vcc_param.encap= ATM_ECNAP_VC_MUX;
	}

	switch (qos)
	{
		case 0:
			vcc_param.qos_type= ATM_QOS_UBR;
			break;
		case 1:
			vcc_param.qos_type= ATM_QOS_CBR;
			break;
		case 2:
			vcc_param.qos_type= ATM_QOS_RTVBR;
			break;
		case 3:
			vcc_param.qos_type= ATM_QOS_NRTVBR;
			break;
		default:
			vcc_param.qos_type= ATM_QOS_UBR;
	}

	vcc_param.pcr = atoi(argv[6]);
	vcc_param.scr = atoi(argv[7]);
	vcc_param.mbs = atoi(argv[8]);

	return ecnt_atmmgr_lib_create_vcc(vpi, vci, &vcc_param);
}


int do_vcc(int argc, char *argv[], void *p)
{
	return subcmd(atmmgr_vcc_command, argc, argv, NULL);
}

int do_stop(int argc, char *argv[], void *p)
{
	return ecnt_atmmgr_lib_stop();
}

int do_start(int argc, char *argv[], void *p)
{
	return ecnt_atmmgr_lib_start();
}


int main(int argc, char **argv) 
{
	int ret = -1;

	ret = subcmd(atmmgr_root_command, argc, argv, NULL);

	return ret;
}

