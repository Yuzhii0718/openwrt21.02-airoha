#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libapi_lib_cputempmgr.h>
#include <libapi_lib_utility.h>


int main(int argc, char **argv) 
{
	int ret = -1;
	cpu_temp_msg_t msg;

	memset(&msg, 0, sizeof(cpu_temp_msg_t));
	ret = ecnt_cputempmgr_lib_get(&msg);

	printf("temp=[%d]\r\n", msg.cpu_temp);

	return ret;
}

