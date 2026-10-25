
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <modules/ptm/ptm_ioctl.h>

int ptmmgr_ioctl(PTMMGR_IOCTL *p_arg)
{
   	int fd = -1;
	int ret = -1;

	fd = open(IOCTL_PATH, O_RDWR);
	if(fd == -1)
	{
		printf("No such dev: %s\n", IOCTL_PATH);
        return ret;
    }

	ret = ioctl(fd, PTMMGR_IOC_OPT, p_arg);
	if(ret < 0)
	{
		printf("ptmmgr_ioctl error\r\n");
        close(fd);
        return ret;
	}

	close(fd);
	return ret;
}


ECNT_PTMMGR_RET ptmmgr_lib_start(char *name)
{
	int ret = -1;
	char cmd_buf[70] = {0};

	if (name != NULL)
		sprintf(cmd_buf,"insmod /lib/modules/mt7510ptm.ko ITFName=%s > /dev/null 2>&1", name);
	else
		sprintf(cmd_buf,"insmod /lib/modules/mt7510ptm.ko > /dev/null 2>&1");
	ret = system(cmd_buf);

	if (ret == -1)
	{
		printf("system error\n");
		return ECNT_PTMMGR_FAIL;
	}

	if (!WIFEXITED(ret))
	{
		printf("exit error\n");
		return ECNT_PTMMGR_FAIL;
	}

	if (!WEXITSTATUS(ret))
	{
		printf("insmod mt7510ptm succcessful\n");
	}
	else
	{
		printf("insmod mt7510ptm failed: %x\n", WEXITSTATUS(ret));
		return ECNT_PTMMGR_FAIL;
	}

	return ECNT_PTMMGR_SUCCESS;
}
ECNT_PTMMGR_RET ptmmgr_lib_stop(void)
{
	int ret = -1;

	ret = system("rmmod mt7510ptm.ko > /dev/null 2>&1");

	if (ret == -1)
	{
		printf("system error\n");
		return ECNT_PTMMGR_FAIL;
	}

	if (!WIFEXITED(ret))
	{
		printf("exit error\n");
		return ECNT_PTMMGR_FAIL;
	}

	if (!WEXITSTATUS(ret))
	{
		printf("rmmod mt7510ptm succcessful\n");
	}
	else
	{
		printf("rmmod mt7510ptm failed: %x\n", WEXITSTATUS(ret));
		return ECNT_PTMMGR_FAIL;
	}

	return ECNT_PTMMGR_SUCCESS;
}


ECNT_PTMMGR_RET ptmmgr_lib_show_wan_counter(PTM_WAN_CNT *p_wan_cnt)
{
	int ret = -1;
	PTMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(PTMMGR_IOCTL));
	arg.func_id = PTMMGR_FUNC_SHOW_WAN_COUNTER;

	ret = ptmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_PTMMGR_FAIL;
	}
	else
	{
		memcpy(p_wan_cnt, &(arg.ptmmgr_data.wan_cnt), sizeof(PTM_WAN_CNT));
		return arg.rtn_val;
	}
}

ECNT_PTMMGR_RET ptmmgr_lib_show_mib_counter(PTM_MIB_CNT *p_mib_cnt)
{
	int ret = -1;
	PTMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(PTMMGR_IOCTL));
	arg.func_id = PTMMGR_FUNC_SHOW_MIB_COUNTER;

	ret = ptmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_PTMMGR_FAIL;
	}
	else
	{
		memcpy(p_mib_cnt, &(arg.ptmmgr_data.mib_cnt), sizeof(PTM_MIB_CNT));
		return arg.rtn_val;
	}
}

