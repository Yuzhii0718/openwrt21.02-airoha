
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <atm/atm_ioctl.h>

int atmmgr_ioctl(ATMMGR_IOCTL *p_arg)
{
   	int fd = -1;
	int ret = -1;

	fd = open(IOCTL_PATH, O_RDWR);
	if(fd == -1)
	{
		printf("No such dev: %s\n", IOCTL_PATH);
        return ret;
    }

	ret = ioctl(fd, ATMMGR_IOC_OPT, p_arg);
	if(ret < 0)
	{
		printf("atmmgr_ioctl error\r\n");
        close(fd);
        return ret;
	}

	close(fd);
	return ret;
}


ECNT_ATMMGR_RET atmmgr_lib_start(void)
{
	int ret = -1;

	ret = system("insmod /lib/modules/mt7510sar.ko BSPMode=1 > /dev/null 2>&1");

	if (ret == -1)
	{
		printf("system error\n");
		return ECNT_ATMMGR_FAIL;
	}

	if (!WIFEXITED(ret))
	{
		printf("exit error\n");
		return ECNT_ATMMGR_FAIL;
	}

	if (!WEXITSTATUS(ret))
	{
		printf("insmod mt7510sar succcessful\n");
	}
	else
	{
		printf("insmod mt7510sar failed: %x\n", WEXITSTATUS(ret));
		return ECNT_ATMMGR_FAIL;
	}

	return ECNT_ATMMGR_SUCCESS;
}
ECNT_ATMMGR_RET atmmgr_lib_stop(void)
{
	int ret = -1;

	ret = system("rmmod mt7510sar.ko > /dev/null 2>&1");

	if (ret == -1)
	{
		printf("system error\n");
		return ECNT_ATMMGR_FAIL;
	}

	if (!WIFEXITED(ret))
	{
		printf("exit error\n");
		return ECNT_ATMMGR_FAIL;
	}

	if (!WEXITSTATUS(ret))
	{
		printf("rmmod mt7510sar succcessful\n");
	}
	else
	{
		printf("rmmod mt7510sar failed: %x\n", WEXITSTATUS(ret));
		return ECNT_ATMMGR_FAIL;
	}

	return ECNT_ATMMGR_SUCCESS;
}

ECNT_ATMMGR_RET atmmgr_lib_create_vcc(unsigned char vpi, unsigned int vci, ATM_VCC_PARAM *p_vcc_param)
{
	int ret = -1;
	ATMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(ATMMGR_IOCTL));
	arg.func_id = ATMMGR_FUNC_CREATE_VCC;
	arg.vpi = vpi;
	arg.vci = vci;
	memcpy(&(arg.atmmgr_data.vcc_param), p_vcc_param, sizeof(ATM_VCC_PARAM));

	ret = atmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_ATMMGR_FAIL;
	}
	else
	{
		return arg.rtn_val;
	}
}

ECNT_ATMMGR_RET atmmgr_lib_delete_vcc(unsigned char vpi, unsigned int vci)
{
	int ret = -1;
	ATMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(ATMMGR_IOCTL));
	arg.func_id = ATMMGR_FUNC_DELETE_VCC;
	arg.vpi = vpi;
	arg.vci = vci;

	ret = atmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_ATMMGR_FAIL;
	}
	else
	{
		return arg.rtn_val;
	}
}

ECNT_ATMMGR_RET atmmgr_lib_show_vcc(unsigned char vpi, unsigned int vci, ATM_VCC_PARAM *p_vcc_param)
{
	int ret = -1;
	ATMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(ATMMGR_IOCTL));
	arg.func_id = ATMMGR_FUNC_SHOW_VCC;
	arg.vpi = vpi;
	arg.vci = vci;

	ret = atmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_ATMMGR_FAIL;
	}
	else
	{
		memcpy(p_vcc_param, &(arg.atmmgr_data.vcc_param), sizeof(ATM_VCC_PARAM));
		return arg.rtn_val;
	}
}

ECNT_ATMMGR_RET atmmgr_lib_start_oam_loopback(unsigned char vpi, unsigned int vci, ATM_OAM_PARAM *p_oam_param)
{
	int ret = -1;
	ATMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(ATMMGR_IOCTL));
	arg.func_id = ATMMGR_FUNC_START_OAM_LOOPBACK;
	arg.vpi = vpi;
	arg.vci = vci;
	memcpy(&(arg.atmmgr_data.oam_param), p_oam_param, sizeof(ATM_OAM_PARAM));

	ret = atmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_ATMMGR_FAIL;
	}
	else
	{
		return arg.rtn_val;
	}
}

ECNT_ATMMGR_RET atmmgr_lib_reset_oam_loopback(void)
{
	int ret = -1;
	ATMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(ATMMGR_IOCTL));
	arg.func_id = ATMMGR_FUNC_RESET_OAM_LOOPBACK;

	ret = atmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_ATMMGR_FAIL;
	}
	else
	{
		return arg.rtn_val;
	}
}

ECNT_ATMMGR_RET atmmgr_lib_show_oam_loopback(ATM_OAM_STATUS *p_oam_status)
{
	int ret = -1;
	ATMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(ATMMGR_IOCTL));
	arg.func_id = ATMMGR_FUNC_SHOW_OAM_LOOPBACK;

	ret = atmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_ATMMGR_FAIL;
	}
	else
	{
		memcpy(p_oam_status, &(arg.atmmgr_data.oam_status), sizeof(ATM_OAM_STATUS));
		return arg.rtn_val;
	}
}

ECNT_ATMMGR_RET atmmgr_lib_show_wan_counter(ATM_WAN_CNT *p_wan_cnt)
{
	int ret = -1;
	ATMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(ATMMGR_IOCTL));
	arg.func_id = ATMMGR_FUNC_SHOW_WAN_COUNTER;

	ret = atmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_ATMMGR_FAIL;
	}
	else
	{
		memcpy(p_wan_cnt, &(arg.atmmgr_data.wan_cnt), sizeof(ATM_WAN_CNT));
		return arg.rtn_val;
	}
}

ECNT_ATMMGR_RET atmmgr_lib_show_mib_counter(ATM_MIB_CNT *p_mib_cnt)
{
	int ret = -1;
	ATMMGR_IOCTL arg;

	memset(&arg, 0, sizeof(ATMMGR_IOCTL));
	arg.func_id = ATMMGR_FUNC_SHOW_MIB_COUNTER;

	ret = atmmgr_ioctl(&arg);

	if (ret == -1)
	{
		return ECNT_ATMMGR_FAIL;
	}
	else
	{
		memcpy(p_mib_cnt, &(arg.atmmgr_data.mib_cnt), sizeof(ATM_MIB_CNT));
		return arg.rtn_val;
	}
}

