

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libapi_lib_xdslmgr.h>
#include <libapi_lib_utility.h>
#include "xdslmgr.h"

static cmds_t xdslmgr_get_command[] =
{
	{"fwver", do_get_fwver, 0, 0, NULL},
	{"linkstatus", do_get_linkstatus, 0, 0, NULL},
	{"mode", do_get_mode, 0, 0, NULL},
	{"annex", do_get_annex, 0, 0, NULL},
	{"tpstc", do_get_tpstc, 0, 0, NULL},
	{"chandata", do_get_chandata, 0, 0, NULL},
	{"perfdata", do_get_perfdata, 0, 0, NULL},
	{"linedata", do_get_linedata, 0, 0, NULL},
	{"attain_rate", do_get_attain_rate, 0, 0, NULL},
	{"uptime", do_get_uptime, 0, 0, NULL},
	{"linestate", do_get_linestate, 0, 0, NULL},
	{"showtime", do_get_showtime, 0, 0, NULL},
	{"pms", do_get_pms, 0, 0, NULL},
	{"event", do_get_event, 0, 0, NULL},


    {NULL, NULL, 0, 0, NULL}
};

#define SET_MODE_PARAM  "[glite:0|t1.413:1|gdmt:2|multimode:3|adsl2:4|adsl2plus:5|vdsl2:6|adsl2plus_multi:7]"
#define SET_ANNEX_PARAM  "[a:0|b:1|i:2|j:3|al:4][m:5][bj:6][aijlm:7]"

static cmds_t xdslmgr_set_command[] =
{
	{"mode", do_set_mode, 0, 1, "xdsl_cmd set mode "SET_MODE_PARAM},
	{"annex", do_set_annex, 0, 1, "xdsl_cmd set annex "SET_ANNEX_PARAM},

    {NULL, NULL, 0, 0, NULL}
};

#define GET_PARAM1 "[fwver|linkstatus|mode|annex|tpstc|chandata|perfdata|linedata"
#define GET_PARAM2 "                     |attain_rate|uptime|linestate|showtime|pms|event]"
static cmds_t xdslmgr_root_command[] =
{
	{"set", do_set, 0, 1, "xdsl_cmd set [mode|annex]"},
	{"get", do_get, 0, 1, "xdsl_cmd get "GET_PARAM1"\n"GET_PARAM2},

    {NULL, NULL, 0, 0, NULL}
};


int do_get_event(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_pms(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_showtime(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_linestate(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_uptime(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_attain_rate(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_linedata(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_perfdata(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_chandata(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_tpstc(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_annex(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_mode(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_linkstatus(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get_fwver(int argc, char *argv[], void *p)
{
	printf("TODO\n");

	return 0;
}

int do_get(int argc, char *argv[], void *p)
{
	return subcmd(xdslmgr_get_command, argc, argv, NULL);
}

int do_set_annex(int argc, char *argv[], void *p)
{
	char *annex_str[] = {"a", "b", "i", "j", "al", "m", "bj", "aijlm"};
	unsigned char annex = XDSL_ANNEX_DEF_NO;
	int i = 0;

	for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return ECNT_XDSLMGR_FAIL;
	    }
	}

	annex = (unsigned char) atoi(argv[1]);
	switch(annex)
	{
		case XDSL_ANNEX_A:
		case XDSL_ANNEX_B:
		case XDSL_ANNEX_I:
		case XDSL_ANNEX_J:
		case XDSL_ANNEX_AL:
		case XDSL_ANNEX_M:
		case XDSL_ANNEX_BJ:
		case XDSL_ANNEX_AIJLM:
			return ecnt_xdslmgr_lib_set_annex(annex_str[annex]);
		default:
			printf("invalid annex %s\n", argv[1]);
			return ECNT_XDSLMGR_FAIL;
	}
}
int do_set_mode(int argc, char *argv[], void *p)
{
	char *mode_str[] = {"glite", "t1.413", "gdmt", "multimode", "adsl2", "adsl2plus", "vdsl2", "adsl2plus_multi"};
	unsigned char mode = XDSL_MODE_DEF_NO;
	int i = 0;

	for(i = 0; i < strlen(argv[1]); i++)
	{
	    if(!isdigit(argv[1][i]))
		{
	       return ECNT_XDSLMGR_FAIL;
	    }
	}

	mode = (unsigned char) atoi(argv[1]);
	switch(mode)
	{
		case XDSL_MODE_GLITE:
		case XDSL_MODE_T1_413:
		case XDSL_MODE_GDMT:
		case XDSL_MODE_MULTIMODE:
		case XDSL_MODE_ADSL2:
		case XDSL_MODE_ADSL2PLUS:
		case XDSL_MODE_VDSL2:
		case XDSL_MODE_ADSL2PLUS_MULTI:
			return ecnt_xdslmgr_lib_set_mode(mode_str[mode]);
		default:
			printf("invalid mode %s\n", argv[1]);
			return ECNT_XDSLMGR_FAIL;
	}
}

int do_set(int argc, char *argv[], void *p)
{
	return subcmd(xdslmgr_set_command, argc, argv, NULL);
}


int main(int argc, char **argv) 
{
	int ret = -1;

	ret = subcmd(xdslmgr_root_command, argc, argv, NULL);

	return ret;
}

