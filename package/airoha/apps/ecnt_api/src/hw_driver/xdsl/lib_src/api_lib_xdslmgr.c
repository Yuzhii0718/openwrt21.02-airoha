
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <modules/xdsl/xdsl_ioctl.h>

int xdslmgr_ioctl(XDSLMGR_IOCTL *p_arg)
{
   	int fd = -1;
	int ret = -1;

	fd = open(IOCTL_PATH, O_RDWR);
	if(fd == -1)
	{
		printf("No such dev: %s\n", IOCTL_PATH);
        return ret;
    }

	ret = ioctl(fd, XDSLMGR_IOC_OPT, p_arg);
	if(ret < 0)
	{
		printf("xdslmgr_ioctl error\r\n");
        close(fd);
        return ret;
	}

	close(fd);
	return ret;
}


ECNT_XDSLMGR_RET xdslmgr_lib_get_event(XDSL_EVENT *p_event)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_EVENT;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_event, &(arg.xdslmgr_data.event), sizeof(XDSL_EVENT));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_pms(XDSL_PMS *p_pms)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_PMS;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_pms, &(arg.xdslmgr_data.pms), sizeof(XDSL_PMS));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_showtime(XDSL_SHOWTIME *p_showtime)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_SHOWTIME;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_showtime, &(arg.xdslmgr_data.showtime), sizeof(XDSL_SHOWTIME));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_linestate(XDSL_LINE_STATE *p_state)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_LINESTATE;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_state, &(arg.xdslmgr_data.state), sizeof(XDSL_LINE_STATE));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_uptime(unsigned int *p_ms)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_UPTIME;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_ms, &(arg.xdslmgr_data.ms), sizeof(unsigned int));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_attain_rate(XDSL_ATTAIN_RATE *p_rate)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_ATTAIN_RATE;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_rate, &(arg.xdslmgr_data.rate), sizeof(XDSL_ATTAIN_RATE));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_linedata(XDSL_LINEDATA *p_linedata)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_LINEDATA;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_linedata, &(arg.xdslmgr_data.linedata), sizeof(XDSL_LINEDATA));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_perfdata(XDSL_PERFDATA *p_perfdata)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_PERFDATA;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_perfdata, &(arg.xdslmgr_data.perfdata), sizeof(XDSL_PERFDATA));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_chandata(XDSL_CHANDATA *p_chandata)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_CHANDATA;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_chandata, &(arg.xdslmgr_data.chandata), sizeof(XDSL_CHANDATA));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_tpstc(XDSL_TPSTC *p_tpstc)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_TPSTC;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_tpstc, &(arg.xdslmgr_data.tpstc), sizeof(XDSL_TPSTC));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_annex(XDSL_ANNEX *p_annex)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_ANNEX;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_annex, &(arg.xdslmgr_data.annex), sizeof(XDSL_ANNEX));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_mode(XDSL_MODE *p_mode)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_MODE;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_mode, &(arg.xdslmgr_data.mode), sizeof(XDSL_MODE));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_linkstatus(XDSL_LINK_STATUS *p_status)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_LINKSTATUS;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_status, &(arg.xdslmgr_data.status), sizeof(XDSL_LINK_STATUS));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_get_fwver(char *p_fwver)
{
	int ret = -1;
	XDSLMGR_IOCTL arg;

	memset(&arg, 0, sizeof(XDSLMGR_IOCTL));
	arg.func_id = XDSLMGR_FUNC_GET_FWVER;

	ret = xdslmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_XDSLMGR_FAIL;
	}
	else
	{
		memcpy(p_fwver, arg.xdslmgr_data.fwver, sizeof(XDSL_MAX_FWVER_LEN));
		return arg.rtn_val;
	}
}

ECNT_XDSLMGR_RET xdslmgr_lib_set_annex(char *annex)
{
	int ret = -1;
	char cmd_buf[70] = {0};

	sprintf(cmd_buf,"echo wan ghs set annex %s > /proc/tc3162/tcci_cmd", annex);
	ret = system(cmd_buf);

	if (ret == -1)
	{
		printf("system error\n");
		return ECNT_XDSLMGR_FAIL;
	}

	if (!WIFEXITED(ret))
	{
		printf("exit error\n");
		return ECNT_XDSLMGR_FAIL;
	}

	if (WEXITSTATUS(ret))
	{
		printf("failed: %x\n", WEXITSTATUS(ret));
		return ECNT_XDSLMGR_FAIL;
	}

	return ECNT_XDSLMGR_SUCCESS;
}

ECNT_XDSLMGR_RET xdslmgr_lib_set_mode(char *mode)
{
	int ret = -1;
	char cmd_buf[70] = {0};

	sprintf(cmd_buf,"echo wan adsl opencmd %s > /proc/tc3162/tcci_cmd", mode);
	ret = system(cmd_buf);

	if (ret == -1)
	{
		printf("system error\n");
		return ECNT_XDSLMGR_FAIL;
	}

	if (!WIFEXITED(ret))
	{
		printf("exit error\n");
		return ECNT_XDSLMGR_FAIL;
	}

	if (WEXITSTATUS(ret))
	{
		printf("failed: %x\n", WEXITSTATUS(ret));
		return ECNT_XDSLMGR_FAIL;
	}

	return ECNT_XDSLMGR_SUCCESS;
}

