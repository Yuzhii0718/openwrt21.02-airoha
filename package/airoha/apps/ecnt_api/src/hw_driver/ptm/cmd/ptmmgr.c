

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libapi_lib_ptmmgr.h>
#include <libapi_lib_utility.h>
#include "ptmmgr.h"

static cmds_t ptmmgr_cnt_show_command[] =
{
	{"wan", do_cnt_show_wan, 0, 0, NULL},
	{"mib", do_cnt_show_mib, 0, 0, NULL},

    {NULL, NULL, 0, 0, NULL}
};

static cmds_t ptmmgr_cnt_command[] =
{
	{"show", do_cnt_show, 0, 1, NULL},

    {NULL, NULL, 0, 0, NULL}
};

static cmds_t ptmmgr_root_command[] =
{
	{"start", do_start, 0, 0, "ptm_cmd start"},
	{"stop", do_stop, 0, 0, "ptm_cmd stop"},
	{"counter", do_cnt, 0, 2, "ptm_cmd counter show [wan|mib]"},

    {NULL, NULL, 0, 0, NULL}
};

int do_cnt_show_mib(int argc, char *argv[], void *p)
{
	PTM_MIB_CNT mib_cnt;
	ECNT_PTMMGR_RET ret = ECNT_PTMMGR_DEF_NO;
	int i = 0;

	memset(&mib_cnt, 0, sizeof(PTM_MIB_CNT));

	ret = ecnt_ptmmgr_lib_show_mib_counter(&mib_cnt);

	if (ret == ECNT_PTMMGR_SUCCESS)
	{
		for (i = 0; i < mib_cnt.bearer_max; i++)
		{
			printf("\n*******BEARER%d*******\n", i);
			printf("\n*******OUT PACKET COUNTER*******\n");
			printf("txPkts      = 0x%08x\n", mib_cnt.txPkts[i]);
			printf("txBytes     = 0x%08x\n", mib_cnt.txBytes[i]);

			printf("\n*******IN PACKET COUNTER********\n");
			printf("rxPkts      = 0x%08x\n", mib_cnt.rxPkts[i]);
			printf("rxBytes     = 0x%08x\n", mib_cnt.rxBytes[i]);
		
			printf("\n*******OUT DROP COUNTER*********\n");
			printf("txDiscards  = 0x%08x\n", mib_cnt.txDiscards[i]);

			printf("\n*******IN ERROR COUNTER*********\n");
			printf("rxDiscards  = 0x%08x\n", mib_cnt.rxDiscards[i]);
			printf("rxErrors    = 0x%08x\n", mib_cnt.rxErrors[i]);
			printf("rxErrCrc    = 0x%08x\n", mib_cnt.rxErrCrc[i]);
			printf("rxErrLong   = 0x%08x\n", mib_cnt.rxErrLong[i]);
			printf("rxErrRunt   = 0x%08x\n", mib_cnt.rxErrRunt[i]);
			printf("rxErrIp4Cs  = 0x%08x\n", mib_cnt.rxErrIp4Cs[i]);
			printf("rxErrL4Cs   = 0x%08x\n", mib_cnt.rxErrL4Cs[i]);
		}
	}

	return ret;
}

int do_cnt_show_wan(int argc, char *argv[], void *p)
{
	PTM_WAN_CNT wan_cnt;
	ECNT_PTMMGR_RET ret = ECNT_PTMMGR_DEF_NO;

	memset(&wan_cnt, 0, sizeof(PTM_WAN_CNT));

	ret = ecnt_ptmmgr_lib_show_wan_counter(&wan_cnt);

	if (ret == ECNT_PTMMGR_SUCCESS)
	{
		printf("RX_PKTS      = 0x%08x  TX_PKTS      = 0x%08x\n", wan_cnt.rxPkts, wan_cnt.txPkts);
		printf("RX_DROP_PKTS = 0x%08x  TX_DROP_PKTS = 0x%08x\n", wan_cnt.rxDropPkts, wan_cnt.txDropPkts);
		printf("RX_BYTES     = 0x%08x  TX_BYTES     = 0x%08x\n", wan_cnt.rxBytes, wan_cnt.txBytes);
	}

	return ret;
}

int do_cnt_show(int argc, char *argv[], void *p)
{
	return subcmd(ptmmgr_cnt_show_command, argc, argv, NULL);
}

int do_cnt(int argc, char *argv[], void *p)
{
	return subcmd(ptmmgr_cnt_command, argc, argv, NULL);
}

int do_stop(int argc, char *argv[], void *p)
{
	return ecnt_ptmmgr_lib_stop();
}

int do_start(int argc, char *argv[], void *p)
{
	if (argc == 1)
		return ecnt_ptmmgr_lib_start(NULL);
	else
		return ecnt_ptmmgr_lib_start(argv[1]);
}


int main(int argc, char **argv) 
{
	int ret = -1;

	ret = subcmd(ptmmgr_root_command, argc, argv, NULL);

	return ret;
}

